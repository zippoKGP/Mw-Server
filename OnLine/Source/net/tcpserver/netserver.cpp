// NetServer.cpp: implementation of the CNetServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

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
	if(!m_pTCPComm)
	{
		m_pTCPComm = new CTCPComm;
	}
	m_dwIP = pInit->dwIP;
	m_wPort = pInit->dwPort;
	m_pCallback = pfnCB;
	m_pThis = pvKey;
	INIT_SERCER is;
	is.dwIP = pInit->dwIP;
	is.dwPort = pInit->dwPort;
	is.pvCallback = (TCPCallback)TCPSvrCallback;
	is.pvKey = this;
	is.ulMaxConnect = pInit->ulMaxConnect;
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
	try
	{
		UG_ULONG ulRet = m_pTCPComm->sendBuffer(pvConnect,ulSendSize);
		return ulRet;
	}
	catch(...)
	{
		printf("CNetServer sendBuffer error.\n");
		return -1;
	}
}

UG_ULONG CNetServer::recvData(UG_PVOID pvConnect,double& dbRecv,double& dbRecvs)
{
	try
	{
		UG_ULONG ulRet = m_pTCPComm->recvData(pvConnect,dbRecv,dbRecvs);
		return ulRet;
	}
	catch(...)
	{
		printf("CNetServer recvData error.\n");
		return -1;
	}
}

UG_ULONG CNetServer::writeBuffer(UG_PVOID pvConnect,UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR pchData,UG_ULONG ulLen)
{
	try
	{
		UGGCP_T ugGcp;
		ugGcp.ulSvrParam = dwParam;
		ugGcp.ulSvrType = dwMsgID;
		ugGcp.wDataLen = ulLen;
		return m_pTCPComm->writeBuffer(&ugGcp,pvConnect,pchData);
	}
	catch(...)
	{
		printf("CNetServer writeBuffer error.\n");
		return -1;
	}
}

UG_ULONG CNetServer::writeBuffer(UG_PVOID pvConnect,UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR pchData,UG_ULONG ulLen,DWORD dwOverflow)
{
	try
	{
		UGGCP_T ugGcp;
		ugGcp.ulSvrParam = dwParam;
		ugGcp.ulSvrType = dwMsgID;
		ugGcp.wDataLen = ulLen;
		return m_pTCPComm->writeBuffer(&ugGcp,pvConnect,pchData,dwOverflow);
	}
	catch(...)
	{
		printf("CNetServer writeBuffer error overflow.\n");
		return -1;
	}
}

UG_ULONG CNetServer::sendData(UG_PVOID pvConnect,UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR pchData,UG_ULONG ulLen)
{
	try
	{
		UGGCP_T ugGcp;
		ugGcp.ulSvrParam = dwParam;
		ugGcp.ulSvrType = dwMsgID;
		ugGcp.wDataLen = ulLen;
		UG_ULONG ulRet = m_pTCPComm->sendData(&ugGcp,pvConnect,pchData);
		return ulRet;
	}
	catch(...)
	{
		printf("CNetServer sendData error overflow.\n");
		return -1;
	}
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
		try
		{
			((FnNetSvr)m_pCallback)(&nsp);
		}
		catch(...)
		{
			printf("CNetServer TCPCltCallbackProc 1 error id = %d, param = %d, len = %d.\n",nsp.dwMsgID,nsp.dwParam,nsp.ulLen);
		}
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
		try
		{
			((FnNetSvr)m_pCallback)(&nsp);
		}
		catch(...)
		{
			printf("CNetServer TCPCltCallbackProc 2 error id = %d, param = %d, len = %d.\n",nsp.dwMsgID,nsp.dwParam,nsp.ulLen);
		}
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

UG_ULONG CNetServer::acceptPlayer()
{
	try
	{
		if(m_pTCPComm)
		{
			return m_pTCPComm->acceptPlayer();
		}
		return -1;
	}
	catch(...)
	{
		printf("CNetServer acceptPlayer error overflow.\n");
		return -1;
	}
}

UG_INT32 CNetServer::setSocket(UG_INT32 nSend,UG_INT32 nRecv)
{
	if(m_pTCPComm)
	{
		return m_pTCPComm->setSocket(nSend,nRecv);
	}
	return -1;
}


UG_INT32 CNetServer::setWriteBuffer(UG_INT32 nWrite)
{
	if(m_pTCPComm)
	{
		return m_pTCPComm->setWriteBuffer(nWrite);
	}
	return -1;
}

UG_INT32 CNetServer::setRecvBuffer(UG_DWORD dwRecv)
{
	if(m_pTCPComm)
	{
		return m_pTCPComm->setRecvBuffer(dwRecv);
	}
	return -1;
}

