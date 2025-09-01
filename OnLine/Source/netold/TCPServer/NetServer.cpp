// NetServer.cpp: implementation of the CNetServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#define SEND_RECV_SIZE	4500

EXPOSE_INTERFACE(CNetServer,INetServer,NETSERVER_INTERFACE_VERSION)

CNetServer::CNetServer()
{
	m_pTCPComm = new CTCPComm;
	m_pCallback = NULL;
	m_pThis = NULL;
	m_ulLastError = 0;
	m_pvPlayer = NULL;
	m_pLog = NULL;
	m_dwIP = 0;
	m_wPort = 0;
}

CNetServer::~CNetServer()
{
	cleanup();
}

UG_ULONG CNetServer::init(PINIT_NET_SERCER pInit,FnNetSvr pfnCB,UG_PVOID pvKey)
{
	UG_CHAR szPath[1024];
	sprintf(szPath,"./log/netserver_%d.log",GetTickCount());
//	m_pLog = new CUGLog;
//	m_pLog->init(szPath);
	m_dwIP = pInit->dwIP;
	m_wPort = pInit->dwPort;
	
	m_pCallback = pfnCB;
	m_pThis = pvKey;
	if(!m_pTCPComm)
	{
		m_pTCPComm = new CTCPComm;
	}
	INIT_SERCER is;
	is.dwIP = pInit->dwIP;
	is.dwPort = pInit->dwPort;
	is.pvCallback = (TCPCallback)TCPSvrCallback;
	is.pvKey = this;
	is.ulMaxConnect = pInit->ulMaxConnect;
	is.ulRecvSocketBufferSize = SEND_RECV_SIZE;
	is.ulSendSocketBufferSize = SEND_RECV_SIZE;
	m_pTCPComm = new CTCPComm;
	if(m_pTCPComm->init(&is))
	{
		cleanup();
		return UG_ULONG(-1);
	}
	return 0;
}

UG_ULONG CNetServer::init(UG_PCHAR pchPathName,FnNetSvr pfnCB,UG_PVOID pvKey)
{
	UG_CHAR szPath[1024];
	sprintf(szPath,"./log/netserver_%d.log",GetTickCount());
//	m_pLog = new CUGLog;
//	m_pLog->init(szPath);
	
	m_pCallback = pfnCB;
	m_pThis = pvKey;
	CUGIni ini;
	if(ini.init(pchPathName))
	{
		return -1;
	}
	UG_PCHAR pchIP = NULL;
	UG_PCHAR pchPort = NULL;
	UG_PCHAR pchMaxCount = NULL;
	UG_PCHAR pchSocketSend = NULL;
	UG_PCHAR pchSocketRecv = NULL;
	ini.getValue("NetConfig","IP",pchIP);
	ini.getValue("NetConfig","Port",pchPort);
	ini.getValue("NetConfig","MaxConnectCount",pchMaxCount);
	if(!m_pTCPComm)
	{
		m_pTCPComm = new CTCPComm;
	}
	INIT_SERCER is;
	is.dwIP = inet_addr(pchIP);
	is.dwPort = atoi(pchPort);
	is.pvCallback = (TCPCallback)TCPSvrCallback;
	is.pvKey = this;
	is.ulMaxConnect = atoi(pchMaxCount);
	is.ulRecvSocketBufferSize = SEND_RECV_SIZE;
	is.ulSendSocketBufferSize = SEND_RECV_SIZE;
	m_pTCPComm = new CTCPComm;
	if(m_pTCPComm->init(&is))
	{
		cleanup();
		return UG_ULONG(-1);
	}
	return 0;
}

UG_ULONG CNetServer::shutdown()
{
	cleanup();
	return 0;
}

UG_ULONG CNetServer::cleanup()
{
	ReleaseP<CTCPComm*>(m_pTCPComm);
	ReleaseP<CUGLog*>(m_pLog);
	return 0;
}

UG_ULONG CNetServer::disconnectPlayer(UG_PVOID pvConnect)
{
	return m_pTCPComm->disconnect(pvConnect);
}

UG_ULONG CNetServer::sendBuffer(UG_PVOID pvConnect,UG_ULONG ulSendSize)
{
	UG_ULONG ulRet = m_pTCPComm->sendBuffer(pvConnect,ulSendSize);
	return ulRet;
}

UG_ULONG CNetServer::recvData(UG_PVOID pvConnect,UG_LONG lRecvCount,double& dbRecv,double& dbRecvs)
{
	UG_ULONG ulRet = m_pTCPComm->recvData(pvConnect,lRecvCount,dbRecv,dbRecvs);
	return ulRet;
}

UG_ULONG CNetServer::writeBuffer(UG_PVOID pvConnect,UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR pchData,UG_ULONG ulLen)
{
	UGGCP_T ugGcp;
	ugGcp.ulSvrParam = dwParam;
	ugGcp.ulSvrType = dwMsgID;
	ugGcp.wDataLen = ulLen;
	return m_pTCPComm->writeBuffer(&ugGcp,pvConnect,pchData);
}

UG_ULONG CNetServer::sendData(UG_PVOID pvConnect,UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR pchData,UG_ULONG ulLen)
{
	UGGCP_T ugGcp;
	ugGcp.ulSvrParam = dwParam;
	ugGcp.ulSvrType = dwMsgID;
	ugGcp.wDataLen = ulLen;
	UG_ULONG ulRet = m_pTCPComm->sendData(&ugGcp,pvConnect,pchData);
	return ulRet;
}

UG_ULONG CNetServer::TCPSvrCallback(PTCP_PARAM pvParam)
{
	return ((CNetServer*)(pvParam->pvKey))->TCPSvrCallbackProc(pvParam);
}

UG_ULONG CNetServer::TCPSvrCallbackProc(PTCP_PARAM pvParam)
{
	if(TCPCOMM_RECVDATA == pvParam->tcpEvent)
	{
		NET_SERVER_PARAM nsp;
		nsp.dwMsgID = pvParam->ugGcp.ulSvrType;
		if(NET_MSG_CONNECT == nsp.dwMsgID)
		{
			return 0;
		}
		nsp.dwParam = pvParam->ugGcp.ulSvrParam;
		nsp.pchData = pvParam->pchData;
		nsp.ulLen = pvParam->ugGcp.wDataLen;
		nsp.pvConnect = pvParam->pvConnect;
		nsp.pvKey = m_pThis;
		nsp.pvPlayer = pvParam->pvFlag;
		assert(nsp.pvPlayer);
		((FnNetSvr)m_pCallback)(&nsp);
	}
	else if(TCPCOMM_CONNECT == pvParam->tcpEvent)
	{
		NET_SERVER_PARAM nsp;
		nsp.pchData = NULL;
		nsp.pvConnect = pvParam->pvConnect;
		nsp.pvKey = m_pThis;
		nsp.pvPlayer = pvParam->pvFlag;
		nsp.dwMsgID = NET_MSG_CONNECT;
		nsp.dwParam = -1;
		nsp.ulLen = 0;
		((FnNetSvr)m_pCallback)(&nsp);
	}
	else if(TCPCOMM_ERROR == pvParam->tcpEvent)
	{
		if(m_pLog)
		{
			m_pLog->UGLog(TRUE,pvParam->pchData);
		}
	}
	else
	{
	}
	return 0;
}

UG_ULONG CNetServer::getErrorCode()
{
	return m_ulLastError;
}

UG_ULONG CNetServer::initGetIP()
{
	if(!m_pTCPComm)
	{
		m_pTCPComm = new CTCPComm;
	}
	return m_pTCPComm->initGetIP();
}

UG_ULONG CNetServer::getIPCount()
{
	if(m_pTCPComm)
	{
		return m_pTCPComm->getIPCount();
	}
	return -1;
}

UG_DWORD CNetServer::getIP(UG_ULONG ulIndex)
{
	if(m_pTCPComm)
	{
		return m_pTCPComm->getIP(ulIndex);
	}
	return -1;
}

UG_PCHAR CNetServer::convertIP(UG_DWORD dwIP)
{
	if(m_pTCPComm)
	{
		return m_pTCPComm->convertIP(dwIP);
	}
	return NULL;
}

UG_DWORD CNetServer::convertIP(UG_PCHAR pchIP)
{
	if(m_pTCPComm)
	{
		return m_pTCPComm->convertIP(pchIP);
	}
	return -1;
}

UG_ULONG CNetServer::getConnect(UG_PVOID pvConnect,UG_DWORD& dwIP,UG_WORD& wPort)
{
	SOCKET hSocket = NULL;
	return m_pTCPComm->getConnect(pvConnect,dwIP,wPort,hSocket);
}

UG_ULONG CNetServer::setPlayer(UG_PVOID pvConnect,UG_PVOID pvPlayer)
{
	if(m_pTCPComm)
	{
		m_pvPlayer = pvPlayer;
		return m_pTCPComm->setPlayer(pvConnect,pvPlayer);
	}
	return -1;
}

UG_ULONG CNetServer::getHost(UG_DWORD& dwIP,UG_WORD& wPort)
{
	dwIP = m_dwIP;
	wPort = m_wPort;
	return -1;
}

UG_ULONG CNetServer::sendProc(void)
{
	if (m_pTCPComm)
	{
		return m_pTCPComm->sendProc();
	}

	return -1;
}

UG_ULONG CNetServer::acceptPlayer()
{
	if(m_pTCPComm)
	{
		return m_pTCPComm->acceptPlayer();
	}
	return -1;
}
