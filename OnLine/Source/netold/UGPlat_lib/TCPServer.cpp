/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : TCPServer.cpp
*owner        : Ben
*description  : 服务器端程序实现
*modified     : 2004/12/20
******************************************************************************/ 

#include "IncAll.h"
#include "zlib.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define MAX_LISTEN			SOMAXCONN //连接的最大数据,listen的参数
#define SEND_BUFFER			32768 //50 * 1024
#define RECV_BUFFER			10240 //10 * 1024

UG_PCHAR convertip(UG_DWORD dwIP)
{
	static UG_CHAR szIP[256];
	sprintf(szIP,"%d.%d.%d.%d",dwIP & 0x000000ff,(dwIP & 0x0000ff00) >> 8,(dwIP & 0x00ff0000) >> 16,(dwIP & 0xff000000) >> 24);
	szIP[255] = '\0';
	return szIP;
}

CTCPServer::CTCPServer()
{
	m_hSocket = NULL;
	m_pInit = NULL;
	m_pConnect = NULL;
	m_listConnectIdle.clear();
}

CTCPServer::~CTCPServer()
{
	cleanup();
}

UG_ULONG CTCPServer::startupWinsock2()
{
	UG_WORD wVersionRequested = MAKEWORD(2,2);
	WSADATA wsaData;
	memset(&wsaData,0,sizeof(WSADATA));
	UG_INT32 n32Ret = WSAStartup(wVersionRequested,&wsaData);
	if(n32Ret)
	{
		return -1;
	}
	if(2 != LOBYTE(wsaData.wVersion) || 2 != HIBYTE(wsaData.wVersion))
	{
		WSACleanup();
		return -1;
	}
	return 0;
}

HANDLE CTCPServer::createThread(LPTHREAD_START_ROUTINE pfnProc,UG_PVOID pvKey,UG_INT32 n32Priority)
{
	UG_DWORD dwID = 0;
	HANDLE hThread = CreateThread(NULL,0,pfnProc,pvKey,CREATE_SUSPENDED,&dwID);
	if(hThread)
	{
		SetThreadPriority(hThread,n32Priority);
		ResumeThread(hThread);
	}
	return hThread;
}

UG_ULONG CTCPServer::terminateThread(HANDLE& hThread,UG_DWORD dwMilliseconds)
{
	if(hThread)
	{
		if(WAIT_TIMEOUT == WaitForSingleObject(hThread,dwMilliseconds))
		{
			TerminateThread(hThread,0);
		}
		ReleaseH(hThread);
	}
	return 0;
}

UG_VOID CTCPServer::ReleaseH(HANDLE& h)
{
	if(h)
	{
		if(INVALID_HANDLE_VALUE != h)
		{
			CloseHandle(h);
		}
		h = NULL;
	}
	return;
}

UG_VOID CTCPServer::ReleaseHS(SOCKET& h)
{
	if(h)
	{
		if(INVALID_SOCKET != h && SOCKET_ERROR != h)
		{
			if(closesocket(h))
			{
				printf("close socket error.\n");
				Sleep(10);
				closesocket(h);
			}
		}
		h = NULL;
	}
	return;
}

UG_ULONG CTCPServer::setSocketOpt(SOCKET hSocket,UG_BOOL bReuseAddr)
{
	UG_INT32 n32Send = 60 * 1024;
	UG_INT32 n32Recv = 16 * 1024;
	setsockopt(hSocket,SOL_SOCKET,SO_RCVBUF,(UG_PCHAR)(&n32Recv),sizeof(UG_INT32));
	setsockopt(hSocket,SOL_SOCKET,SO_SNDBUF,(UG_PCHAR)(&n32Send),sizeof(UG_INT32));
	setsockopt(hSocket,SOL_SOCKET,SO_REUSEADDR,(UG_PCHAR)(&bReuseAddr),sizeof(UG_BOOL));
	LINGER linger;
	linger.l_linger = 0;
	linger.l_onoff = 1;
	setsockopt(hSocket,SOL_SOCKET,SO_LINGER,(UG_PCHAR)(&linger),sizeof(SO_LINGER));
	return 0;
}

UG_ULONG CTCPServer::checkInitData(PINIT_SERCER pInit)
{
	if(!(pInit->dwIP))
	{
		return -1;
	}
	if(pInit->dwPort < 1024 || pInit->dwPort > 65535)
	{
		return -1;
	}
	if(!(pInit->pvCallback))
	{
		return -1;
	}
	if(pInit->ulMaxConnect < 1)
	{
		return -1;
	}
	if(pInit->ulSendSocketBufferSize < 1)
	{
		return -1;
	}
	if(pInit->ulRecvSocketBufferSize < 1)
	{
		return -1;
	}
	return 0;
}

UG_ULONG CTCPServer::initSocket()
{
	m_hSocket = socket(AF_INET,SOCK_STREAM,0);
	if(INVALID_SOCKET == m_hSocket)
	{
		cleanup();
		return -1;
	}
	SOCKADDR_IN sockaddr;
	memset(&sockaddr,0,sizeof(SOCKADDR_IN));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.S_un.S_addr = m_pInit->dwIP;
	sockaddr.sin_port = htons(m_pInit->dwPort);
	setSocketOpt(m_hSocket,FALSE);
	UG_INT32 n32Ret = bind(m_hSocket,(LPSOCKADDR)(&sockaddr),sizeof(SOCKADDR_IN));
	if(SOCKET_ERROR == n32Ret)
	{
		cleanup();
		return -1;
	}
	n32Ret = listen(m_hSocket,MAX_LISTEN);
	if(SOCKET_ERROR == n32Ret)
	{
		cleanup();
		return -1;
	}
	UG_ULONG ulNonblocking = 1;
	ioctlsocket(m_hSocket,FIONBIO,&ulNonblocking);
	return 0;
}

UG_ULONG CTCPServer::cleanup()
{
	ReleaseHS(m_hSocket);
	freeMemory();
	WSACleanup();
	m_log.cleanup();
	return 0;
}

UG_ULONG CTCPServer::init(PINIT_SERCER pInit)
{
	UG_ULONG ulRet = startupWinsock2();
	if(ulRet)
	{
		return ulRet;
	}
	ulRet = checkInitData(pInit);
	if(ulRet)
	{
		cleanup();
		return ulRet;
	}
	ulRet = mallocMemory(pInit);
	if(ulRet)
	{
		cleanup();
		return ulRet;
	}
	ulRet = initSocket();
	if(ulRet)
	{
		cleanup();
		return ulRet;
	}
	m_log.init("./server_net.log");
	return 0;
}

UG_ULONG CTCPServer::disconnect(UG_PVOID pvConnect)
{
	if(!pvConnect)
	{
		return UG_ULONG(-1);
	}
	CTCPConnect* pConnect = (CTCPConnect*)pvConnect;
	double dbTimes = Plat_FloatTime() - pConnect->m_dbBeginer;
	m_log.UGLog("disconnect ip = %s, port = %d, write size = %d,send size = %d,send times = %f,%f.\n",convertip(pConnect->m_dwIP),pConnect->m_wPort,pConnect->m_ulWriteSize,pConnect->m_ulSendSize,dbTimes,(pConnect->m_ulSendSize) / dbTimes);
	ReleaseHS(pConnect->m_hSocket);
	m_listConnectIdle.push_back(pConnect);
	return 0;
}

UG_ULONG CTCPServer::writeBuffer(PUGGCP_T pUGGcp,UG_PVOID pvConnect,UG_PCHAR pchBuffer)
{
	if(!pvConnect)
	{
		return UG_ULONG(-1);
	}
	WORD wLen = pUGGcp->wDataLen + sizeof(UGGCP_T);
	
	CTCPConnect* pConnect = (CTCPConnect*)pvConnect;
	if(pConnect->m_ulSendPos + wLen + 2 + sizeof(UG_WORD) > SEND_BUFFER)
	{
		m_log.UGLog("write buffer error, ip = %d, port = %d.",convertip(pConnect->m_dwIP),pConnect->m_wPort);
		return -1;
	}
	if(pConnect->m_dbBeginer <= 0.0)
	{
		pConnect->m_dbBeginer = Plat_FloatTime();
	}
	pConnect->m_ulWriteSize += wLen + 2 + sizeof(UG_WORD);
	memcpy(pConnect->m_pchSendBuffer + pConnect->m_ulSendPos,g_pchTCPCommFlag,2);
	memcpy(pConnect->m_pchSendBuffer + pConnect->m_ulSendPos + 2,&wLen,sizeof(UG_WORD));
	memcpy(pConnect->m_pchSendBuffer + pConnect->m_ulSendPos + 2 + sizeof(UG_WORD),pUGGcp,sizeof(UGGCP_T));
	memcpy(pConnect->m_pchSendBuffer + pConnect->m_ulSendPos + 2 + sizeof(UG_WORD) + sizeof(UGGCP_T),pchBuffer,pUGGcp->wDataLen);
	wLen += 2 + sizeof(UG_WORD);
	pConnect->m_ulSendPos += wLen;
	return UG_ULONG(wLen);
}

UG_ULONG CTCPServer::sendData(PUGGCP_T pUGGcp,UG_PVOID pvConnect,UG_PCHAR pchBuffer)
{
/*	if(!pvConnect)
	{
		return UG_ULONG(-1);
	}
	CTCPConnect* pConnect = (CTCPConnect*)pvConnect;
	UG_ULONG ulRet = sendBuffer(pConnect);
	if(-1 == ulRet)
	{
		return -1;
	}
	ulRet = writeBuffer(pUGGcp,pvConnect,pchBuffer);
	if(-1 == ulRet)
	{
		return -1;
	}
	sendBuffer(pConnect);
	return 0;*/
	writeBuffer(pUGGcp,pvConnect,pchBuffer);
	return sendBuffer(pvConnect,50000);
}

UG_ULONG CTCPServer::sendBuffer(UG_PVOID pvConnect,int nSize)
{
	CTCPConnect* pConnect = (CTCPConnect*)pvConnect;
	if(!(pConnect->m_ulSendPos))
	{
		return 0;
	}
	UG_INT32 n32Ret = 0;
	int send_len = min(pConnect->m_ulSendPos,nSize);
	n32Ret = send(pConnect->m_hSocket,pConnect->m_pchSendBuffer,send_len,0);
	if(SOCKET_ERROR == n32Ret)
	{
		double dbTimes = Plat_FloatTime() - pConnect->m_dbBeginer;
		DWORD dwError = WSAGetLastError();
		if(WSAEWOULDBLOCK == dwError)
		{
			printf("WSAEWOULDBLOCK.\n");
			return 0;
		}
		else if(WSAENOBUFS == dwError)
		{
			printf("WSAENOBUFS.\n");
			return 0;
		}
		else
		{
			m_log.UGLog("disconnect ip = %d, port = %d.",convertip(pConnect->m_dwIP),pConnect->m_wPort);
			m_log.UGLog("error is %d, send is -1. pos = %d, write size = %d, send size = %d,send times = %f, %f.",WSAGetLastError(),pConnect->m_ulSendPos,pConnect->m_ulWriteSize,pConnect->m_ulSendSize,dbTimes,(pConnect->m_ulSendSize) / dbTimes);
			return -1;
		}
	}
	else if(!n32Ret)
	{
		double dbTimes = Plat_FloatTime() - pConnect->m_dbBeginer;
		m_log.UGLog("disconnect ip = %d, port = %d.",convertip(pConnect->m_dwIP),pConnect->m_wPort);
		m_log.UGLog("send is 0. pos = %d, port = %d, write size = %d, send size = %d,send times = %f.\n",pConnect->m_ulSendPos,pConnect->m_wPort,pConnect->m_ulWriteSize,pConnect->m_ulSendSize,dbTimes);
		return -1;
	}
	else
	{
		pConnect->m_ulSendSize += n32Ret;
		pConnect->m_ulSendPos -= n32Ret;
		memmove(pConnect->m_pchSendBuffer,pConnect->m_pchSendBuffer + n32Ret,pConnect->m_ulSendPos);
		if((n32Ret != nSize) && pConnect->m_ulSendPos)
		{
			m_log.UGLog("send ok not send all, port = %d, pos = %d, sended = %d\n",pConnect->m_wPort,pConnect->m_ulSendPos,n32Ret);
		}
	}
	return n32Ret;
}

/***************************************************************************************
	Function:		recvData	
	Describe:		接收数据，数据在回调函数中导出。
	----------------------------------------------------------------------------------
	parameter:		UG_PVOID pvConnect, //连接的客户端
					UG_LONG lRecvCount //回调个数，<1表示全部，否则表示个数
	Return:			UG_ULONG
	Author:			Ben
	Create Timer:	2004/10/20 17:56:00
	Modify Timer:	
/**************************************************************************************/
UG_ULONG CTCPServer::recvData(UG_PVOID pvConnect,double& dbRecv,double& dbRecvs)
{
	if(!pvConnect)
	{
		return UG_ULONG(-1);
	}
	CTCPConnect* pConnect = (CTCPConnect*)pvConnect;
	int nPos = pConnect->m_ulRecvPos;
	char* pchRecv = pConnect->m_pchRecvBuffer;
	
	double dbBeginer = Plat_FloatTime();
	UG_INT32 n32Ret = recv(pConnect->m_hSocket,pchRecv + nPos,RECV_BUFFER - nPos,0);
	dbRecv = Plat_FloatTime() - dbBeginer;
	dbRecvs = 0;
	if(SOCKET_ERROR == n32Ret)
	{
		if(WSAEWOULDBLOCK == GetLastError())
		{
			return 0;
		}
		else
		{
			printf("CTCPServer, recv error, disconnect ip = %s, error code is %d.\n",convertip(pConnect->m_dwIP),GetLastError());
			return -1;
		}
	}
	else if(!n32Ret)
	{
		printf("CTCPServer, recv error, disconnect ip = %s.\n",convertip(pConnect->m_dwIP));
		return -1;
	}
	else
	{
		pConnect->m_ulRecvPos += n32Ret;
		for(UG_INT32 i = 0; ; i ++)
		{
			if(pConnect->m_ulRecvPos < 2 + sizeof(UG_WORD)) //数据太小，不能处理
			{
				return 0;
			}
			if((*g_pchTCPCommFlag != *pchRecv) || (*(g_pchTCPCommFlag + 1) != *(pchRecv + 1))) //数据标志不对，丢弃
			{
				return 0;
			}
			UG_WORD wLen = 0;
			memcpy(&wLen,pchRecv + 2,sizeof(UG_WORD));
			UG_ULONG ulBufLen = wLen + 2 + sizeof(UG_WORD);
			if((UG_ULONG)wLen > m_pInit->ulRecvSocketBufferSize) //数据过大，丢弃
			{
				return 0;
			}
			if(pConnect->m_ulRecvPos > ulBufLen) //数据较大
			{
				TCP_PARAM tp;
				tp.pvFlag = pConnect->m_pvPlayer;
				tp.pvKey = m_pInit->pvKey;
				tp.tcpEvent = TCPCOMM_RECVDATA;
				memcpy(&(tp.ugGcp),pchRecv + 2 + sizeof(UG_WORD),sizeof(UGGCP_T));
				if(tp.ugGcp.wDataLen)
				{
					tp.pchData = pchRecv + 2 + sizeof(UG_WORD) + sizeof(UGGCP_T);
				}
				else
				{
					tp.pchData = NULL;
				}
				
				dbRecvs += Plat_FloatTime() - dbBeginer;
				dbBeginer = Plat_FloatTime();

				(m_pInit->pvCallback)(&tp);
				pConnect->m_ulRecvPos -= ulBufLen;
				memmove(pchRecv,pchRecv + ulBufLen,pConnect->m_ulRecvPos);
				continue;
			}
			else if(pConnect->m_ulRecvPos == ulBufLen) //完整数据
			{
				TCP_PARAM tp;
				tp.pvFlag = pConnect->m_pvPlayer;
				tp.pvKey = m_pInit->pvKey;
				tp.tcpEvent = TCPCOMM_RECVDATA;
				memcpy(&(tp.ugGcp),pchRecv + 2 + sizeof(UG_WORD),sizeof(UGGCP_T));
				if(tp.ugGcp.wDataLen)
				{
					tp.pchData = pchRecv + 2 + sizeof(UG_WORD) + sizeof(UGGCP_T);
				}
				else
				{
					tp.pchData = NULL;
				}
				
				dbRecvs += Plat_FloatTime() - dbBeginer;

				(m_pInit->pvCallback)(&tp);
				pConnect->m_ulRecvPos = 0;
				break;
			}
		}
	}
	return 0;
}

UG_ULONG CTCPServer::listenProc()
{
	SOCKADDR_IN addrConnect;
	memset(&addrConnect,0,sizeof(SOCKADDR_IN));
	UG_INT32 n32Addr = sizeof(SOCKADDR_IN);
	SOCKET hSocket = accept(m_hSocket,(LPSOCKADDR)(&addrConnect),&n32Addr);
	if(INVALID_SOCKET == hSocket)
	{
		if(WSAEWOULDBLOCK == GetLastError())
		{
			return 0;
		}
		DWORD dwError = GetLastError();
		printf("listenProc error because socket is closed %d.\n",dwError);
		return -1;
	}
	else
	{
		SOCKADDR saPeer; 
		memset(&saPeer,0,sizeof(SOCKADDR));
		UG_INT32 n32Sa = sizeof(SOCKADDR);
		getpeername(hSocket,&saPeer,&n32Sa);
		CTCPConnect* pConnect = NULL;
		LPSOCKADDR_IN pIN = (LPSOCKADDR_IN)(&saPeer);
		list<CTCPConnect*>::iterator it = m_listConnectIdle.begin();
		if(it != m_listConnectIdle.end())
		{
			pConnect = *it;
			m_listConnectIdle.erase(it);
		}
		else
		{
			ReleaseHS(hSocket);
			return 0;
		}
		pConnect->init();
		pConnect->m_dwIP = pIN->sin_addr.S_un.S_addr;
		pConnect->m_wPort = pIN->sin_port;
		pConnect->m_hSocket = hSocket;
		pConnect->m_ulEncrypted = GetTickCount();
		TCP_PARAM tp;
		tp.pvKey = m_pInit->pvKey;
		tp.tcpEvent = TCPCOMM_CONNECT;
		tp.dwIP = pConnect->m_dwIP;
		tp.wPort = pConnect->m_wPort;
		tp.pvConnect = pConnect;
		UGGCP_T ut;
		ut.ulSvrType = NET_MSG_CHECK_DATA_RETURN;
		ut.ulSvrParam = pConnect->m_ulEncrypted;
		ut.wDataLen = sizeof(GUID);
		static GUID guid;
		CoCreateGuid(&guid);
		sendData(&ut,tp.pvConnect,(UG_PCHAR)(&guid));
		(m_pInit->pvCallback)(&tp);
		setSocketOpt(hSocket);
		UG_ULONG ulNonblocking = 1;
		ioctlsocket(hSocket,FIONBIO,&ulNonblocking);
	}
	return 0;
}

UG_ULONG CTCPServer::mallocMemory(PINIT_SERCER pInit)
{
	m_pInit = UGNew<INIT_SERCER>(0,__FILE__,__LINE__);
	memcpy(m_pInit,pInit,sizeof(INIT_SERCER));
	m_pConnect = UGNew<CTCPConnect>(m_pInit->ulMaxConnect);
	if(!m_pConnect)
	{
		return -1;
	}
	m_listConnectIdle.clear();
	for(UG_INT32 i = 0; i < m_pInit->ulMaxConnect; i ++)
	{
		CTCPConnect* p = m_pConnect + i;
		p->m_pchSendBuffer = UGMalloc<UG_CHAR>(SEND_BUFFER,__FILE__,__LINE__);
		p->m_pchRecvBuffer = UGMalloc<UG_CHAR>(RECV_BUFFER,__FILE__,__LINE__);
		m_listConnectIdle.push_back(p);
	}
	return 0;
}

UG_ULONG CTCPServer::freeMemory()
{
	m_listConnectIdle.clear();
	if(m_pInit)
	{
		for(UG_INT32 i = 0; i < m_pInit->ulMaxConnect; i ++)
		{
			CTCPConnect* p = m_pConnect + i;
			UGDel<UG_PCHAR>(p->m_pchSendBuffer);
			UGDel<UG_PCHAR>(p->m_pchRecvBuffer);
		}
	}
	UGDel<CTCPConnect*>(m_pConnect);
	UGDel<PINIT_SERCER>(m_pInit);
	return 0;
}

UG_ULONG CTCPServer::getConnect(UG_PVOID pvConnect,UG_DWORD& dwIP,UG_WORD& wPort,SOCKET& hSocket)
{
	if(!pvConnect)
	{
		return UG_ULONG(-1);
	}
	CTCPConnect* pConnect = (CTCPConnect*)pvConnect;
	dwIP = pConnect->m_dwIP;
	wPort = pConnect->m_wPort;
	hSocket = pConnect->m_hSocket;
	return 0;
}

UG_ULONG CTCPServer::setPlayer(UG_PVOID pvConnect,UG_PVOID pvPlayer)
{
	if(!pvConnect)
	{
		return UG_ULONG(-1);
	}
	CTCPConnect* pConnect = (CTCPConnect*)pvConnect;
	pConnect->m_pvPlayer = pvPlayer;
	return 0;
}

UG_ULONG CTCPServer::acceptPlayer()
{
	return listenProc();
}

UG_DWORD CTCPServer::sendProc()
{
	return 0;
}
