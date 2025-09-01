/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : TCPServer.cpp
*owner        : Ben
*description  : 服务器端程序实现
*modified     : 2004/12/20
******************************************************************************/ 

#include "IncAll.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

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
	m_mapConnect.clear();
	m_pRecvBuffer = NULL;
	
	m_nSocketSend = 20480;
	m_nSocketRecv = 10240;

	m_pRecvBuffer = NULL;
	m_dwRecvBufferSize = 10240;

	m_nWriteBuffer = 32768;
	m_nMessageBuffer = 8192;
	
	m_bWSACleanup = FALSE;
	m_dwTickCount = 0;
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
	m_bWSACleanup = TRUE;
	if(2 != LOBYTE(wsaData.wVersion) || 2 != HIBYTE(wsaData.wVersion))
	{
		cleanup();
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
			closesocket(h);
		}
		h = NULL;
	}
	return;
}

UG_ULONG CTCPServer::setSocketOpt(SOCKET hSocket,UG_BOOL bReuseAddr)
{
	setsockopt(hSocket,SOL_SOCKET,SO_RCVBUF,(UG_PCHAR)(&m_nSocketSend),sizeof(UG_INT32));
	setsockopt(hSocket,SOL_SOCKET,SO_SNDBUF,(UG_PCHAR)(&m_nSocketRecv),sizeof(UG_INT32));
	setsockopt(hSocket,SOL_SOCKET,SO_REUSEADDR,(UG_PCHAR)(&bReuseAddr),sizeof(UG_BOOL));
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
	n32Ret = listen(m_hSocket,SOMAXCONN);
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
	if(m_bWSACleanup)
	{
		WSACleanup();
		m_bWSACleanup = FALSE;
	}
	m_log.cleanup();
	return 0;
}

UG_ULONG CTCPServer::init(PINIT_SERCER pInit)
{
	UG_ULONG ulRet = startupWinsock2();
	if(ulRet)
	{
		cleanup();
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
	ReleaseHS(pConnect->m_hSocket);
	map<UG_PVOID,CTCPConnect*>::iterator it = m_mapConnect.find(pvConnect);
	if(it != m_mapConnect.end())
	{
		m_mapConnect.erase(it);
	}
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
	if(pConnect->m_ulSendPos + wLen + 2 + sizeof(UG_WORD) > m_nWriteBuffer)
	{
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
	if(-1 == writeBuffer(pUGGcp,pvConnect,pchBuffer))
	{
		return -1;
	}
	return sendBuffer(pvConnect,m_nWriteBuffer);
}

UG_ULONG CTCPServer::sendBuffer(UG_PVOID pvConnect,int nSize)
{
	if(!pvConnect)
	{
		m_log.UGLog("sendBuffer error because pvConnect is null.");
		return UG_ULONG(-1);
	}
	
	UGDWORD dwSendBufferTimer = GetTickCount();
	CTCPConnect* pConnect = (CTCPConnect*)pvConnect;
	if((pConnect->m_ulSendPos < 1) || (nSize < 1))
	{
		if(dwSendBufferTimer - pConnect->m_dwSendBufferTimer > 1000)
		{
			pConnect->m_dwSendBufferTimer = dwSendBufferTimer;
			static UGGCP_T ut;
			ut.ulSvrType = NET_MSG_CHECK_ONLINE;
			ut.ulSvrParam = 0;
			ut.wDataLen = 0;
			writeBuffer(&ut,pvConnect,NULL);
		}
		return 0;
	}

	if(pConnect->m_ulSendError)
	{
		pConnect->m_ulSendError = 0;
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
			m_log.UGLog("WSAEWOULDBLOCK ip = %s, port = %d, pos = %d, size = %d, a = %f",convertip(pConnect->m_dwIP),pConnect->m_wPort,pConnect->m_ulSendPos,pConnect->m_ulSendSize,pConnect->m_ulSendSize / dbTimes);
			pConnect->m_ulSendError ++;
			return 0;
		}
		else if(WSAENOBUFS == dwError)
		{
			m_log.UGLog("WSAENOBUFS ip = %s, pos = %d.",convertip(pConnect->m_dwIP),pConnect->m_ulSendPos);
			pConnect->m_ulSendError ++;
			return 0;
		}
		else
		{
			ReleaseHS(pConnect->m_hSocket);
			return -1;
		}
	}
	else if(!n32Ret)
	{
		m_log.UGLog("sendbuffer because sended is 0.");
		ReleaseHS(pConnect->m_hSocket);
		return -1;
	}
	else
	{
		pConnect->m_ulSendSize += n32Ret;
		pConnect->m_ulSendPos -= n32Ret;
		memmove(pConnect->m_pchSendBuffer,pConnect->m_pchSendBuffer + n32Ret,pConnect->m_ulSendPos);
	}
	pConnect->m_ulSendError = 0;
	pConnect->m_dwSendBufferTimer = dwSendBufferTimer;
	return n32Ret;
}

UG_ULONG CTCPServer::recvData(UG_PVOID pvConnect,double& dbRecv,double& dbRecvs)
{
	if(!pvConnect)
	{
		return UG_ULONG(-1);
	}
	CTCPConnect* pConnect = (CTCPConnect*)pvConnect;
	int nPos = pConnect->m_ulRecvPos;
	memcpy(m_pRecvBuffer,pConnect->m_pchRecvBuffer,pConnect->m_ulRecvPos);
	char* pchRecv = m_pRecvBuffer;
	double dbBeginer = Plat_FloatTime();
	UG_INT32 n32Ret = recv(pConnect->m_hSocket,pchRecv + nPos,m_dwRecvBufferSize - nPos,0);
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
			double dbTimes = Plat_FloatTime() - pConnect->m_dbBeginer;
			m_log.UGLog("recv error = -1 %d, ip = %s, port = %d, pos = %d, size = %d, a = %f",WSAGetLastError(),convertip(pConnect->m_dwIP),pConnect->m_wPort,pConnect->m_ulSendPos,pConnect->m_ulSendSize,pConnect->m_ulSendSize / dbTimes);
			return -1;
		}
	}
	else if(!n32Ret)
	{
		double dbTimes = Plat_FloatTime() - pConnect->m_dbBeginer;
		m_log.UGLog("recv error = 0, ip = %s, port = %d, pos = %d, size = %d, a = %f",convertip(pConnect->m_dwIP),pConnect->m_wPort,pConnect->m_ulSendPos,pConnect->m_ulSendSize,pConnect->m_ulSendSize / dbTimes);
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
				m_log.UGLog("message error because flag is not UG.");
				pConnect->m_ulRecvPos = 0;//清空
				return 0;
			}
			UG_WORD wLen = 0;
			memcpy(&wLen,pchRecv + 2,sizeof(UG_WORD));
			UG_ULONG ulBufLen = wLen + 2 + sizeof(UG_WORD);
			if((int)ulBufLen > m_nMessageBuffer) //数据过大，丢弃
			{
				m_log.UGLog("message error because %d > %d, %d.",ulBufLen,m_nMessageBuffer,pConnect->m_ulRecvPos);
				if(pConnect->m_ulRecvPos >= ulBufLen)
				{
					pConnect->m_ulRecvPos -= ulBufLen;//清空
					continue;
				}
				else
				{
					pConnect->m_ulRecvPos = 0;
					return 0;
				}
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
				
				dbBeginer = Plat_FloatTime();
				
				if(NET_MSG_CHECK_ONLINE == tp.ugGcp.ulSvrType)
				{
					m_log.UGLog("CTCPServer NET_MSG_CHECK_ONLINE. ip = %s, port = %d.",convertip(pConnect->m_dwIP),pConnect->m_wPort);
				}
				else
				{
					(m_pInit->pvCallback)(&tp);
				}
				
				pConnect->m_ulRecvPos -= ulBufLen;
				memmove(pchRecv,pchRecv + ulBufLen,pConnect->m_ulRecvPos);
				dbRecvs += Plat_FloatTime() - dbBeginer;
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
				
				dbBeginer = Plat_FloatTime();

				if(NET_MSG_CHECK_ONLINE == tp.ugGcp.ulSvrType)
				{
					m_log.UGLog("CTCPServer NET_MSG_CHECK_ONLINE. ip = %s, port = %d.",convertip(pConnect->m_dwIP),pConnect->m_wPort);
				}
				else
				{
					(m_pInit->pvCallback)(&tp);
				}

				pConnect->m_ulRecvPos = 0;
				dbRecvs += Plat_FloatTime() - dbBeginer;
				break;
			}
			else //数据太小
			{
				memcpy(pConnect->m_pchRecvBuffer,pchRecv,pConnect->m_ulRecvPos);
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
			m_mapConnect[pConnect] = pConnect;
		}
		else
		{
			m_log.UGLog("not idle list socket.");
			ReleaseHS(hSocket);
			return 0;
		}
		pConnect->init();
		pConnect->m_dwIP = pIN->sin_addr.S_un.S_addr;
		pConnect->m_wPort = pIN->sin_port;
		pConnect->m_hSocket = hSocket;
		TCP_PARAM tp;
		tp.pvKey = m_pInit->pvKey;
		tp.tcpEvent = TCPCOMM_CONNECT;
		tp.dwIP = pConnect->m_dwIP;
		tp.wPort = pConnect->m_wPort;
		tp.pvConnect = pConnect;
		
		m_dwTickCount ++;
		UGGCP_T ut;
		ut.ulSvrType = NET_MSG_CHECK_DATA_RETURN;
		ut.ulSvrParam = m_dwTickCount;
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
		p->m_pchSendBuffer = new UGCHAR[m_nWriteBuffer];
		p->m_pchRecvBuffer = new UGCHAR[m_nMessageBuffer];
		m_listConnectIdle.push_back(p);
	}
	m_pRecvBuffer = new UGCHAR[m_dwRecvBufferSize];
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
			delete[] (p->m_pchSendBuffer);
			p->m_pchSendBuffer = NULL;
			delete[] (p->m_pchRecvBuffer);
			p->m_pchRecvBuffer = NULL;
		}
	}
	UGDel<CTCPConnect*>(m_pConnect);
	UGDel<PINIT_SERCER>(m_pInit);
	
	if(m_pRecvBuffer)
	{
		delete[] m_pRecvBuffer;
		m_pRecvBuffer = NULL;
	}
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

UGDWORD CTCPServer::writeBuffer(PUGGCP_T pUGGcp,UG_PVOID pvConnect,UG_PCHAR pchBuffer,UGDWORD dwOverflow)
{
	if(!pvConnect)
	{
		return UG_ULONG(-1);
	}
	WORD wLen = pUGGcp->wDataLen + sizeof(UGGCP_T);
	CTCPConnect* pConnect = (CTCPConnect*)pvConnect;
	if(pConnect->m_ulSendPos + wLen + 2 + sizeof(UG_WORD) > dwOverflow)
	{
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

UGINT CTCPServer::setSocket(UGINT nSend,UGINT nRecv)
{
	m_nSocketSend = nSend;
	m_nSocketRecv = nRecv;
	return 0;
}

UGINT CTCPServer::setWriteBuffer(UGINT nWrite)
{
	m_nWriteBuffer = nWrite;
	return 0;
}

UGINT CTCPServer::setRecvBuffer(UGDWORD dwRecv)
{
	m_dwRecvBufferSize = dwRecv;
	return 0;
}

