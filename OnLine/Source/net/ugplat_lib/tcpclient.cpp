/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd.
2004 ALL RIGHTS RESERVED.
*file name    : TCPClient.cpp
*owner        : Ben
*description  : TCPClient接口类的实现，用于客户端TCP数据传输
*modified     : 2004/12/20
******************************************************************************/

#include "IncAll.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern UG_PCHAR convertip(UG_DWORD dwIP);

CTCPClient::CTCPClient()
{
	m_hSocket = NULL;
	m_pInit = NULL;
	m_hRecvEvent = NULL;
	m_hThread = NULL;
	m_pchSendBuffer = NULL;
	m_pchSended = NULL;
	m_nSendBufferLen = 0;
	m_pchRecvBuffer = NULL;
	m_pchRecved = NULL;
	m_nRecvBufferLen = 0;
	m_ulErrorCount = 0;
	m_ulRecvCount = 0;
	m_nRecvPos = 0;
	m_hThreadExit = NULL;
	m_ulCheckDataOver = 0;
	m_dwHostIP = 0;
	m_wHostPort = 0;
	m_pchRecvBufferLzo = NULL;
	m_nRecvPosLzo = 0;
	m_bWSACleanup = FALSE;
	m_stepInit = NOT_INIT;
	m_bInitSucceed = FALSE;
	m_hInitThread = NULL;
	m_pchBufferTemp = NULL;
}

CTCPClient::~CTCPClient()
{
	cleanup();
	m_stepInit = NOT_INIT;
	terminateThread(m_hInitThread);
//	m_log.cleanup();
}

UG_ULONG CTCPClient::cleanup()
{
//	m_log.UGLog("CTCPClient cleanup.");
	if(m_pInit)
	{
		if(m_pInit->bAsynEvent) //异步模式
		{
			SetEvent(m_hThreadExit);
			terminateThread(m_hThread);
			ReleaseH(m_hRecvEvent);
			ReleaseH(m_hThreadExit);
		}
	}
	ReleaseHS(m_hSocket);
	freeMemory();
	if(m_bWSACleanup)
	{
		WSACleanup();
		m_bWSACleanup = FALSE;
	}
	return 0;
}

UG_DWORD CTCPClient::initThread(LPARAM l)
{
	return ((CTCPClient*)l)->initProc();
}

UG_DWORD CTCPClient::initProc()
{
	UG_ULONG ulRet = init(&m_init);
	m_stepInit = INIT_THREAD_END;
	if(ulRet)
	{
		m_bInitSucceed = FALSE;
	}
	else
	{
		m_bInitSucceed = TRUE;
	}
	return 0;
}

UG_ULONG CTCPClient::initNoBlock(PINIT_CLIENT pInit)
{
	if(NOT_INIT == m_stepInit)
	{
		m_bInitSucceed = FALSE;
		m_stepInit = INIT_CONTINUE;
		memcpy(&m_init,pInit,sizeof(INIT_CLIENT));
		m_hInitThread = createThread(LPTHREAD_START_ROUTINE(initThread),this);
		return 1;
	}
	else if(INIT_CONTINUE == m_stepInit)
	{
		return 1;
	}
	else if(INIT_THREAD_END == m_stepInit)
	{
		if(m_bInitSucceed)
		{
			m_stepInit = INIT_OK;
			return 0;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}
	return 0;
}

UG_ULONG CTCPClient::init(PINIT_CLIENT pInit)
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
	static int sss = 0;
	sss ++;
	char szPathName[1024];
	sprintf(szPathName,"./clientnet%d.log",sss);
//	m_log.init(szPathName);
	ulRet = initSocket();
//	m_log.UGLog("init socket ok.");
	if(ulRet)
	{
		cleanup();
		return ulRet;
	}
	if(pInit->dwBindPort > 0)
	{
//		m_log.UGLog("sendData begin.");
		UGGCP_T ut;
		ut.ulSvrType = NET_MSG_CHECK_DATA;
		ut.ulSvrParam = pInit->dwBindPort;
		ut.wDataLen = 0;
		sendData(&ut,NULL);
//		m_log.UGLog("sendData ok.");
		if(-1 == recvData())
		{
//			m_log.UGLog("recvData error.");
			cleanup();
			return -1;
		}
		else if(m_ulCheckDataOver)
		{
//			m_log.UGLog("m_ulCheckDataOver error.");
			cleanup();
			return -1;
		}
		else
		{
		}
//		m_log.UGLog("recvData ok.");
	}
//	m_log.UGLog("init ok.");
	setNoBlocking();
	if(m_pInit->bAsynEvent)
	{
		ulRet = initRecieveThread();
		if(ulRet)
		{
			cleanup();
			printf("setNoBlocking error.\n");
			return ulRet;
		}		
	}
	return 0;
}

/***************************************************************************************
	Function:		initSocket	
	Describe:		初始化socket。
	----------------------------------------------------------------------------------
	parameter:		
	Return:			UG_ULONG
	Author:			Ben
	Create Timer:	2004/12/20 17:56:00
	Modify Timer:	
/**************************************************************************************/
UG_ULONG CTCPClient::initSocket()
{
	m_hSocket = socket(AF_INET,SOCK_STREAM,0);
	if(INVALID_SOCKET == m_hSocket)
	{
		printf("socket error code is %d.\n",WSAGetLastError());
		cleanup();
		return -1;
	}
	if(m_pInit->dwBindIP)
	{
		SOCKADDR_IN sockaddr;
		memset(&sockaddr,0,sizeof(SOCKADDR_IN));
		sockaddr.sin_family = AF_INET;
		sockaddr.sin_addr.S_un.S_addr = m_pInit->dwBindIP;
		sockaddr.sin_port = 0;
		INT32 n32Ret = bind(m_hSocket,(LPSOCKADDR)(&sockaddr),sizeof(SOCKADDR_IN));
		if(SOCKET_ERROR == n32Ret)
		{
			cleanup();
			return -1;
		}
	}


	SOCKADDR_IN sockaddr;
	memset(&sockaddr,0,sizeof(SOCKADDR_IN));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.S_un.S_addr = m_pInit->dwIP;
	sockaddr.sin_port = htons(m_pInit->dwPort);
//	m_log.UGLog("connect begin ip = %s, port = %d, id = %d.",convertip(m_pInit->dwIP),m_pInit->dwPort,m_pInit->dwBindPort);
	INT32 n32Ret = connect(m_hSocket,(LPSOCKADDR)(&sockaddr),sizeof(SOCKADDR_IN));
	if(SOCKET_ERROR == n32Ret)
	{
//		m_log.UGLog("connect error.");
		printf("connect error code is %d.\n",WSAGetLastError());
		cleanup();
		return -1;
	}
//	m_log.UGLog("connect ok.");
	
	memset(&sockaddr,0,sizeof(SOCKADDR_IN));
	UG_INT32 nSockAddr = sizeof(SOCKADDR_IN);
	getsockname(m_hSocket,(LPSOCKADDR)(&sockaddr),&nSockAddr);
	m_dwHostIP = sockaddr.sin_addr.S_un.S_addr;
	m_wHostPort = sockaddr.sin_port;
	printf("initSocket socket %d, ip = %s.\n",m_hSocket,convertip(m_pInit->dwIP));
	m_dbBeginer = Plat_FloatTime();
	
	setSocketOpt(m_hSocket,FALSE);
	
	return 0;
}

UG_ULONG CTCPClient::disconnect()
{
	cleanup();
	return 0;
}

/***************************************************************************************
	Function:		sendData
	Describe:		发送数据。
	----------------------------------------------------------------------------------
	parameter:		UG_PCHAR pchBuffer,
					UG_WORD wLen,
					UG_PVOID pvFlag //回调时用到该参数
	Return:			UG_ULONG
	Author:			Ben
	Create Timer:	2004/12/20 17:56:00
	Modify Timer:	
/**************************************************************************************/
UG_ULONG CTCPClient::sendData(PUGGCP_T pUGGcp,UG_PCHAR pchBuffer)
{
	CSyncEvent sys(&m_cs);
	UG_WORD wLen = sizeof(UGGCP_T) + pUGGcp->wDataLen;
	if(wLen > m_pInit->ulSendSocketBufferSize)
	{
//		m_log.UGLog("CTCPClient, sendData error code.");
		return -1;
	}
	memcpy(m_pchSended,g_pchTCPCommFlag,2);
	memcpy(m_pchSended + 2,&wLen,sizeof(UG_WORD));
	memcpy(m_pchSended + 2 + sizeof(UG_WORD),pUGGcp,sizeof(UGGCP_T));
	if(pUGGcp->wDataLen)
	{
		memcpy(m_pchSended + 2 + sizeof(UG_WORD) + sizeof(UGGCP_T),pchBuffer,pUGGcp->wDataLen);
	}
	UG_ULONG ulRet = 0;
	for(int i = 0; i < 3; i ++)
	{
		ulRet = sendData(m_pchSended,wLen + 2 + sizeof(UG_WORD));
		if(-2 == ulRet)
		{
			Sleep(10);
			continue;
		}
		return ulRet;
	}
	if(ulRet)
	{
//		m_log.UGLog("CTCPClient, send sendData error return = %d.\n",ulRet);
	}
	return ulRet;
}

UG_ULONG CTCPClient::sendData(UG_PCHAR pchBuffer,UG_WORD wLen)
{
	INT32 n32Ret = send(m_hSocket,pchBuffer,wLen,0);
	if(SOCKET_ERROR == n32Ret)
	{
		DWORD dwError = WSAGetLastError();
		if((WSAEWOULDBLOCK == dwError) || (WSAENOBUFS == dwError))//非堵塞
		{
//			m_log.UGLog("CTCPClient, send sendData code = %d.\n",dwError);
			return -2;
		}
		else
		{
//			m_log.UGLog("CTCPClient, send error code = %d.",WSAGetLastError());
			return -1;
		}
	}
	else if(!n32Ret) //已关闭
	{
//		m_log.UGLog("send is 0, closed.");
		return -1;
	}
	else if(n32Ret != wLen) //没有全部发送出去，在非堵塞下一般关闭该连接
	{
//		m_log.UGLog("CTCPClient, send error because %d != %d.\n",n32Ret,wLen);
		return -1;
	}
	return 0;
}

UG_ULONG CTCPClient::recvData()
{
	INT32 n32Ret = recv(m_hSocket,m_pchRecvBuffer + m_nRecvPos,m_nRecvBufferLen - m_nRecvPos,0);
	if(SOCKET_ERROR == n32Ret)
	{
		if(WSAEWOULDBLOCK == WSAGetLastError())
		{
			return 0;
		}
		else
		{
//			m_log.UGLog("CTCPClient, recv error code is %d, %s.",GetLastError(),convertip(m_pInit->dwIP));
			return -1;
		}
	}
	else if(!n32Ret)
	{
//		m_log.UGLog("CTCPClient, recv 0 error code is %d, ip = %s, port = %d.",GetLastError(),convertip(m_pInit->dwIP),m_pInit->dwPort);
		return -1;
	}
	else
	{
		m_nRecvPos += n32Ret;
		for(UG_INT32 i = 0; ; i ++)
		{
			if(m_nRecvPos < 2 + sizeof(UG_WORD)) //数据太小
			{
				return 0;
			}
			if((*g_pchTCPCommFlag != *m_pchRecvBuffer) || (*(g_pchTCPCommFlag + 1) != *(m_pchRecvBuffer + 1))) //数据不对
			{
//				m_log.UGLog("CTCPClient, data is not UG.");
				m_nRecvPos = 0;
				return 0;
			}
			UG_WORD wLen = 0;
			memcpy(&wLen,m_pchRecvBuffer + 2,sizeof(UG_WORD));
			UG_ULONG ulBufLen = wLen + 2 + sizeof(UG_WORD);
			if(ulBufLen > m_pInit->ulRecvSocketBufferSize) //数据过大
			{
//				m_log.UGLog("CTCPClient, data is not long.");
				m_nRecvPos = 0;
				return 0;
			}
			if(m_nRecvPos > ulBufLen) //数据大
			{
				TCP_PARAM tp;
				tp.pvFlag = m_pvPlayer;
				tp.pvKey = m_pInit->pvKey;
				tp.tcpEvent = TCPCOMM_RECVDATA;
				memcpy(&(tp.ugGcp),m_pchRecvBuffer + 2 + sizeof(UG_WORD),sizeof(UGGCP_T));
				memcpy(m_pchBufferTemp,m_pchRecvBuffer + 2 + sizeof(UG_WORD) + sizeof(UGGCP_T),tp.ugGcp.wDataLen);
				if(tp.ugGcp.wDataLen)
				{
					tp.pchData = m_pchBufferTemp;
				}
				else
				{
					tp.pchData = NULL;
				}
				if(NET_MSG_CHECK_DATA_RETURN == tp.ugGcp.ulSvrType)
				{
					m_ulCheckDataOver = 0;//tp.ugGcp.ulSvrParam;
				}
				else if(NET_MSG_CHECK_ONLINE == tp.ugGcp.ulSvrType)
				{
//					m_log.UGLog("CTCPClient NET_MSG_CHECK_ONLINE, ip = %s, port = %d.",convertip(m_pInit->dwIP),m_pInit->dwPort);
					static UGGCP_T ut;
					ut.ulSvrType = NET_MSG_CHECK_ONLINE;
					ut.ulSvrParam = 0;
					ut.wDataLen = 0;
					sendData(&ut,NULL);
				}
				else
				{
					(m_pInit->pvCallback)(&tp);
				}
				m_nRecvPos -= ulBufLen;
				memmove(m_pchRecvBuffer,m_pchRecvBuffer + ulBufLen,m_nRecvPos);
				continue;
			}
			else if(m_nRecvPos == ulBufLen) //完整数据
			{
				TCP_PARAM tp;
				tp.pvFlag = m_pvPlayer;
				tp.pvKey = m_pInit->pvKey;
				tp.tcpEvent = TCPCOMM_RECVDATA;
				memcpy(&(tp.ugGcp),m_pchRecvBuffer + 2 + sizeof(UG_WORD),sizeof(UGGCP_T));
				memcpy(m_pchBufferTemp,m_pchRecvBuffer + 2 + sizeof(UG_WORD) + sizeof(UGGCP_T),tp.ugGcp.wDataLen);
				if(tp.ugGcp.wDataLen)
				{
					tp.pchData = m_pchBufferTemp;
				}
				else
				{
					tp.pchData = NULL;
				}
				if(NET_MSG_CHECK_DATA_RETURN == tp.ugGcp.ulSvrType)
				{
					m_ulCheckDataOver = 0;//tp.ugGcp.ulSvrParam;
				}
				else if(NET_MSG_CHECK_ONLINE == tp.ugGcp.ulSvrType)
				{
//					m_log.UGLog("CTCPClient NET_MSG_CHECK_ONLINE, ip = %s, port = %d.",convertip(m_pInit->dwIP),m_pInit->dwPort);
					static UGGCP_T ut;
					ut.ulSvrType = NET_MSG_CHECK_ONLINE;
					ut.ulSvrParam = 0;
					ut.wDataLen = 0;
					sendData(&ut,NULL);
				}
				else
				{
					(m_pInit->pvCallback)(&tp);
				}
				m_nRecvPos = 0;
				break;
			}
			else
			{
				break;
			}
		}
	}
	return 0;
}

UG_ULONG CTCPClient::startupWinsock2()
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

HANDLE CTCPClient::createThread(LPTHREAD_START_ROUTINE pfnProc,UG_PVOID pvKey,UG_INT32 n32Priority)
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

UG_ULONG CTCPClient::terminateThread(HANDLE& hThread,UG_DWORD dwMilliseconds)
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

UG_VOID CTCPClient::ReleaseH(HANDLE& h)
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

UG_VOID CTCPClient::ReleaseHS(SOCKET& h)
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

/***************************************************************************************
	Function:		setSocketOpt	
	Describe:		设置socket最基层的发送接收缓冲，有winsock自己控制。
	----------------------------------------------------------------------------------
	parameter:		SOCKET hSocket,
					UG_BOOL bReuseAddr //该socket是否可以被其他的进程bind，一般在服务器端是不可以的，客户端是可以的
	Return:			UG_ULONG
	Author:			Ben
	Create Timer:	2004/12/20 17:56:00
	Modify Timer:	
/**************************************************************************************/
UG_ULONG CTCPClient::setSocketOpt(SOCKET hSocket,UG_BOOL bReuseAddr)
{
	UG_INT32 n32Send = 200 * m_pInit->ulSendSocketBufferSize; //客户端缓冲大小可以开大
	UG_INT32 n32Recv = 200 * m_pInit->ulRecvSocketBufferSize;
	setsockopt(hSocket,SOL_SOCKET,SO_RCVBUF,(UG_PCHAR)(&n32Recv),sizeof(UG_INT32));
//	setsockopt(hSocket,SOL_SOCKET,SO_SNDBUF,(UG_PCHAR)(&n32Send),sizeof(UG_INT32));
	setsockopt(hSocket,SOL_SOCKET,SO_REUSEADDR,(UG_PCHAR)(&bReuseAddr),sizeof(UG_BOOL));
	return 0;
}

UG_ULONG CTCPClient::checkInitData(PINIT_CLIENT pInit)
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

UG_DWORD CTCPClient::recvThread(LPARAM l)
{
	return ((CTCPClient*)l)->recvProc();
}

UG_DWORD CTCPClient::recvProc()
{
	HANDLE hWait[] = {m_hThreadExit,m_hRecvEvent};
	for(;;)
	{
		UG_DWORD dwWait = WaitForMultipleObjects(2,hWait,FALSE,INFINITE);
		if(WAIT_OBJECT_0 == dwWait)
		{
			break;
		}
		else if(WAIT_OBJECT_0 + 1 == dwWait)
		{
			UG_ULONG ulRet = recvData();
			if(ulRet)
			{
				TCP_PARAM tp;
				tp.pvKey = m_pInit->pvKey;
				tp.tcpEvent = TCPCOMM_DISCONNECT;
				(m_pInit->pvCallback)(&tp);
//				disconnect();//死锁
				break;
			}
		}
		else
		{
			break;
		}
	}
	return 0;
}

UG_ULONG CTCPClient::mallocMemory(PINIT_CLIENT pInit)
{
	m_pInit = UGNew<INIT_CLIENT>(0,__FILE__,__LINE__);
	memcpy(m_pInit,pInit,sizeof(INIT_CLIENT));
	m_pInit->ulRecvSocketBufferSize = m_pInit->ulRecvSocketBufferSize + 2 + sizeof(UG_WORD); //接收缓冲需要扩充
	m_pchSendBuffer = UGMalloc<UG_CHAR>(m_pInit->ulSendSocketBufferSize * 3,__FILE__,__LINE__);
	if(!m_pchSendBuffer)
	{
		return -1;
	}
	m_pchSended = UGMalloc<UG_CHAR>(m_pInit->ulSendSocketBufferSize * 3,__FILE__,__LINE__);
	if(!m_pchSended)
	{
		return -1;
	}
	m_nSendBufferLen = 0;
	m_nRecvBufferLen = m_pInit->ulRecvSocketBufferSize * 10;
	m_pchRecvBuffer = UGMalloc<UG_CHAR>(m_nRecvBufferLen,__FILE__,__LINE__);
	m_pchRecvBufferLzo = UGMalloc<UG_CHAR>(m_nRecvBufferLen,__FILE__,__LINE__);
	if(!m_pchRecvBuffer)
	{
		return -1;
	}
	m_pchRecved = UGMalloc<UG_CHAR>(m_pInit->ulRecvSocketBufferSize * 3,__FILE__,__LINE__);
	if(!m_pchRecved)
	{
		return -1;
	}
	m_pchBufferTemp = new UGCHAR[m_nRecvBufferLen * 2];
	m_nRecvPos = 0;
	m_nRecvPosLzo = 0;
	m_ulErrorCount = 0;
	m_ulRecvCount = 0;
	return 0;
}

UG_ULONG CTCPClient::freeMemory()
{
	UGDel<PINIT_CLIENT>(m_pInit);
	UGDel<UG_PCHAR>(m_pchSendBuffer);
	UGDel<UG_PCHAR>(m_pchSended);
	UGDel<UG_PCHAR>(m_pchRecvBuffer);
	UGDel<UG_PCHAR>(m_pchRecved);
	ReleasePA(m_pchBufferTemp);
	return 0;
}

UG_ULONG CTCPClient::setPlayer(UG_PVOID pvPlayer)
{
	m_pvPlayer = pvPlayer;
	return 0;
}

UG_ULONG CTCPClient::initRecieveThread(void)
{
	m_hRecvEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	m_hThreadExit = CreateEvent(NULL,FALSE,FALSE,NULL);
	UG_INT32 n32Ret = WSAEventSelect(m_hSocket,m_hRecvEvent,FD_READ | FD_CLOSE);
	if(SOCKET_ERROR == n32Ret)
	{
		cleanup();
		return -1;
	}
	m_hThread = createThread((LPTHREAD_START_ROUTINE)recvThread,this);
	if(!m_hThread)
	{
		cleanup();
		return -1;
	}
	return 0;
}

UG_ULONG CTCPClient::setNoBlocking(UG_BOOL bNoBlocking)
{
	UG_ULONG ulNonblocking = bNoBlocking;
	UG_INT32 n32Ret = ioctlsocket(m_hSocket,FIONBIO,&ulNonblocking); //非诸塞
	if(SOCKET_ERROR == n32Ret)
	{
		cleanup();
		return -1;
	}
	return 0;
}

UG_ULONG CTCPClient::getServer(UG_DWORD& dwIP,UG_WORD& wPort)
{
	dwIP = m_pInit->dwIP;
	wPort = m_pInit->dwPort;
	return 0;
}

UG_ULONG CTCPClient::getHost(UG_DWORD& dwIP,UG_WORD& wPort)
{
	dwIP = m_dwHostIP;
	wPort = m_wHostPort;
	return 0;
}
