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
	UGBOOL bKeepAlive = TRUE;
	setsockopt(hSocket,SOL_SOCKET,SO_KEEPALIVE,(UG_PCHAR)(&bKeepAlive),sizeof(UG_BOOL));
	return 0;;
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
	m_logDis.cleanup();
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
	char szName[1024];
	long lTime = 0;
	time(&lTime);
	
	static int i = 0;
	sprintf(szName,"./log/%d_server_net_%d.log",i,lTime);
	m_log.init(szName);
	
	sprintf(szName,"./log/%d_net_%d.log",i,lTime);
	m_logDis.init(szName);
	i ++;
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
	double ddRecvTime = Plat_FloatTime() - pConnect->m_dbLastestRecvData;
	if(dbTimes > 0.0)
	{
		writeDisconnect(pConnect->m_dwIP,pConnect->m_wPort,ddRecvTime,1,dbTimes,pConnect->m_ulSendSize / dbTimes);
	}
	else
	{
		writeDisconnect(pConnect->m_dwIP,pConnect->m_wPort,ddRecvTime,1,dbTimes,0.0f);
	}
	
	ReleaseHS(pConnect->m_hSocket);
	map<UG_PVOID,CTCPConnect*>::iterator it = m_mapConnect.find(pvConnect);
	if(it != m_mapConnect.end())
	{
		m_mapConnect.erase(it);
		m_listConnectIdle.push_back(pConnect);
		m_log.UGLog("disconnect socket = %d, ip = %s, port = %d, this = %d",pConnect->m_hSocket,convertip(pConnect->m_dwIP),pConnect->m_wPort,pConnect);
	}
	else
	{
		m_log.UGLog("disconnect player error because socket handle is error ip = %s, port = %d, pos = %d, size = %d, error = %d.",convertip(pConnect->m_dwIP),pConnect->m_wPort,pConnect->m_ulSendPos,pConnect->m_ulSendSize,0);
	}
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
	if(m_mapConnect.find(pvConnect) == m_mapConnect.end())
	{
		m_log.UGLog("writeBuffer error because socket handle is error ip = %s, port = %d, pos = %d, size = %d, error = %d.",convertip(pConnect->m_dwIP),pConnect->m_wPort,pConnect->m_ulSendPos,pConnect->m_ulSendSize,0);
	}
	if(pConnect->m_ulSendPos + wLen + 2 + sizeof(UG_WORD) > m_nWriteBuffer)
	{
		m_log.UGLog("writeBuffer error ip = %s, port = %d, pos = %d, size = %d, error = %d, outflowed %d > %d.",convertip(pConnect->m_dwIP),pConnect->m_wPort,pConnect->m_ulSendPos,pConnect->m_ulSendSize,0,pConnect->m_ulSendPos + wLen + 2 + sizeof(UG_WORD),m_nWriteBuffer);
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
		return UG_ULONG(-1);
	}
	UGDWORD dwSendBufferTimer = GetTickCount();
	CTCPConnect* pConnect = (CTCPConnect*)pvConnect;
	if(m_mapConnect.find(pvConnect) == m_mapConnect.end())
	{
		m_log.UGLog("writeBuffer error because socket handle is error ip = %s, port = %d, pos = %d, size = %d, error = %d.",convertip(pConnect->m_dwIP),pConnect->m_wPort,pConnect->m_ulSendPos,pConnect->m_ulSendSize,0);
	}
	if((pConnect->m_ulSendPos < 1) || (nSize < 1))
	{
		if(dwSendBufferTimer - pConnect->m_dwSendBufferTimer > 300)
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
	if(!(pConnect->m_hSocket))
	{
		m_log.UGLog("send socket = NULL ip = %s, port = %d, pos = %d, size = %d, error = %d",convertip(pConnect->m_dwIP),pConnect->m_wPort,pConnect->m_ulSendPos,pConnect->m_ulSendSize,WSAGetLastError());
	}
	n32Ret = send(pConnect->m_hSocket,pConnect->m_pchSendBuffer,send_len,0);
	if(SOCKET_ERROR == n32Ret)
	{
		double dbTimes = Plat_FloatTime() - pConnect->m_dbBeginer;
		DWORD dwError = WSAGetLastError();
		if(WSAEWOULDBLOCK == dwError)
		{
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
			closesocket(pConnect->m_hSocket);//先关闭,其后在recv中获得-1错误
			return -1;
		}
	}
	else if(!n32Ret)
	{
		closesocket(pConnect->m_hSocket);//先关闭,其后在recv中获得-1错误
		return -1;
	}
	else
	{
		pConnect->m_ulSendSize += n32Ret;
		pConnect->m_ulSendPos -= n32Ret;
		pConnect->m_dwSendAllDataSize += n32Ret;
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
	if(m_mapConnect.find(pvConnect) == m_mapConnect.end())
	{
		m_log.UGLog("recvData error because socket handle is error ip = %s, port = %d, pos = %d, size = %d, error = %d.",convertip(pConnect->m_dwIP),pConnect->m_wPort,pConnect->m_ulSendPos,pConnect->m_ulSendSize,0);
		return -1;
	}
	
	double dbBeginer = Plat_FloatTime();
	if(!(pConnect->m_bChecked))
	{
		if(dbBeginer - pConnect->m_dbBeginer > 20)
		{
			return -1;
		}
	}
				
	int nPos = pConnect->m_ulRecvPos;
	memcpy(m_pRecvBuffer,pConnect->m_pchRecvBuffer,pConnect->m_ulRecvPos);
	char* pchRecv = m_pRecvBuffer;
	if(!(pConnect->m_hSocket))
	{
		m_log.UGLog("recv socket = NULL ip = %s, port = %d, pos = %d, size = %d, error = %d",convertip(pConnect->m_dwIP),pConnect->m_wPort,pConnect->m_ulSendPos,pConnect->m_ulSendSize,WSAGetLastError());
	}
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
			double ddRecvTime = Plat_FloatTime() - pConnect->m_dbLastestRecvData;
			if(dbTimes > 0.0)
			{
				writeDisconnect(pConnect->m_dwIP,pConnect->m_wPort,ddRecvTime,GetLastError(),dbTimes,pConnect->m_ulSendSize / dbTimes);
			}
			else
			{
				writeDisconnect(pConnect->m_dwIP,pConnect->m_wPort,ddRecvTime,GetLastError(),dbTimes,0.0f);
			}
			DWORD dwBuffer = pConnect->m_dwSendAllDataSize - pConnect->m_dwRecvAllDataSize;
			m_log.UGLog("ip = %s, port = %d, %d - %d = %d, error = %d.",convertip(pConnect->m_dwIP),pConnect->m_wPort,pConnect->m_dwSendAllDataSize,pConnect->m_dwRecvAllDataSize,dwBuffer,GetLastError());
			return -1;
		}
	}
	else if(!n32Ret)
	{
		double dbTimes = Plat_FloatTime() - pConnect->m_dbBeginer;
		double ddRecvTime = Plat_FloatTime() - pConnect->m_dbLastestRecvData;
		if(dbTimes > 0.0)
		{
			writeDisconnect(pConnect->m_dwIP,pConnect->m_wPort,ddRecvTime,0,dbTimes,pConnect->m_ulSendSize / dbTimes);
		}
		else
		{
			writeDisconnect(pConnect->m_dwIP,pConnect->m_wPort,ddRecvTime,0,dbTimes,0.0f);
		}
		return -1;
	}
	else
	{
		pConnect->m_dbLastestRecvData = Plat_FloatTime();
		pConnect->m_ulRecvPos += n32Ret;
		for(UG_INT32 i = 0; ; i ++)
		{
			if(pConnect->m_ulRecvPos < 2 + sizeof(UG_WORD)) //数据太小，不能处理
			{
				return 0;
			}
			if((*g_pchTCPCommFlag != *pchRecv) || (*(g_pchTCPCommFlag + 1) != *(pchRecv + 1))) //数据标志不对，丢弃
			{
				m_log.UGLog("message error because flag is not UG error ip = %s, port = %d, pos = %d, size = %d, error = %d.",convertip(pConnect->m_dwIP),pConnect->m_wPort,pConnect->m_ulSendPos,pConnect->m_ulSendSize,0);
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
				tp.pvConnect = pConnect;
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
				
				if(pConnect->m_bChecked)
				{
					if(NET_MSG_CHECK_ONLINE == tp.ugGcp.ulSvrType)
					{
						pConnect->m_dwRecvAllDataSize = tp.ugGcp.ulSvrParam;
					}
					(m_pInit->pvCallback)(&tp);
				}
				else
				{
					if(NET_MSG_CHECK_DATA == tp.ugGcp.ulSvrType)
					{
						pConnect->m_bChecked = TRUE;
						(m_pInit->pvCallback)(&tp);
					}
					else
					{
						return -1;
					}
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
				tp.pvConnect = pConnect;
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
				
				if(pConnect->m_bChecked)
				{
					if(NET_MSG_CHECK_ONLINE == tp.ugGcp.ulSvrType)
					{
						pConnect->m_dwRecvAllDataSize = tp.ugGcp.ulSvrParam;
					}
					(m_pInit->pvCallback)(&tp);
				}
				else
				{
					if(NET_MSG_CHECK_DATA == tp.ugGcp.ulSvrType)
					{
						pConnect->m_bChecked = TRUE;
						(m_pInit->pvCallback)(&tp);
					}
					else
					{
						return -1;
					}
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
			return 1;
		}
		DWORD dwError = GetLastError();
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
			return 1;
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
		pConnect->m_dwEncryptKey = m_dwTickCount;
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
		m_log.UGLog("accept socket = %d, ip = %s, port = %d, this = %d",pConnect->m_hSocket,convertip(pConnect->m_dwIP),pConnect->m_wPort,pConnect);
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
	UGDWORD dwRet = 0;
	for(;;)
	{
		dwRet = listenProc();
		if(0 == dwRet) //可能还有其他玩家连接
		{
			continue;
		}
		else if(1 == dwRet) //没有连接者
		{
			return 0;
		}
		else //关闭或错误了
		{
			break;
		}
	}
	return dwRet;
}

UGDWORD CTCPServer::writeBuffer(PUGGCP_T pUGGcp,UG_PVOID pvConnect,UG_PCHAR pchBuffer,UGDWORD dwOverflow)
{
	if(!pvConnect)
	{
		return UG_ULONG(-1);
	}
	if(m_mapConnect.find(pvConnect) == m_mapConnect.end())
	{
		m_log.UGLog("writeBuffer error because socket handle is error!");
	}
	WORD wLen = pUGGcp->wDataLen + sizeof(UGGCP_T);
	CTCPConnect* pConnect = (CTCPConnect*)pvConnect;
	if(pConnect->m_ulSendPos + wLen + 2 + sizeof(UG_WORD) > dwOverflow)
	{
//		m_log.UGLog("writeBuffer error ip = %s, port = %d, pos = %d, size = %d, error = %d, flowed = %d.",convertip(pConnect->m_dwIP),pConnect->m_wPort,pConnect->m_ulSendPos,pConnect->m_ulSendSize,0,dwOverflow);
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

UGINT CTCPServer::getEncrypt(UG_PVOID pvConnect,UG_DWORD& dwEncrypt)
{
	if(!pvConnect)
	{
		return UG_ULONG(-1);
	}
	CTCPConnect* pConnect = (CTCPConnect*)pvConnect;
	dwEncrypt = pConnect->m_dwEncryptKey;
	return 0;
}

UGDWORD CTCPServer::writeDisconnect(DWORD dwIP,DWORD dwPort,double dbTimes,DWORD dwError,double dbOnlineTimes,int nAverage)
{
	m_logDis.UGLog("ip = %s, port = %d, times = %f, error = %d, online = %f, average = %d.",convertip(dwIP),dwPort,dbTimes,dwError,dbOnlineTimes,nAverage);
	return 0;
}
