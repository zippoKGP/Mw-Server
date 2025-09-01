// ClientNet.cpp: implementation of the CClientNet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#define SEND_RECV_SIZE	4500

EXPOSE_INTERFACE(CClientNet,IClientNet,NETCLIENT_INTERFACE_VERSION)

CClientNet::CClientNet()
{
	m_pTCPComm = new CTCPComm;
	m_pCallback = NULL;
	m_pThis = NULL;
	m_ulLastError = 0;
	m_pLog = NULL;
}

CClientNet::~CClientNet()
{
	cleanup();
}

UG_ULONG CClientNet::cleanup()
{
	ReleaseP<CTCPComm*>(m_pTCPComm);
	m_pCallback = NULL;
	ReleaseP<CUGLog*>(m_pLog);
	return 0;
}

UG_ULONG CClientNet::init(UG_DWORD dwServerIP,UG_WORD wServerPort,UG_DWORD dwBindIP,UG_DWORD dwBindPort,FnClientNet pfnCallback,UG_BOOL bAsyc)
{
	UG_CHAR szPath[1024];
	sprintf(szPath,"./log/netclient_%d.log",GetTickCount());
	m_pLog = new CUGLog;
	m_pLog->init(szPath);
	m_pLog->UGLog("log file begin!");
	
	m_pCallback = pfnCallback;
	if(!m_pTCPComm)
	{
		m_pTCPComm = new CTCPComm;
	}
	INIT_CLIENT ic;
	ic.bAsynEvent = bAsyc;
	ic.dwIP = dwServerIP;
	ic.dwPort = wServerPort;
	ic.dwBindIP = dwBindIP;
	ic.dwBindPort = dwBindPort;
	ic.pvCallback = (TCPCallback)TCPCltCallback;
	ic.pvKey = this;
	ic.ulRecvSocketBufferSize = SEND_RECV_SIZE;
	ic.ulSendSocketBufferSize = SEND_RECV_SIZE;
	m_pLog->UGLog("init begin.");
	if(m_pTCPComm->init(&ic))
	{
		cleanup();
		return UG_ULONG(-1);
	}
	return 0;
}

UG_ULONG CClientNet::init(UG_PCHAR pchPathName,FnClientNet pfnCallback,UG_BOOL bAsyc)
{
	UG_CHAR szPath[1024];
	sprintf(szPath,"./log/netclient_%d.log",GetTickCount());
	m_pLog = new CUGLog;
	m_pLog->init(szPath);
	
	m_pCallback = pfnCallback;
	CUGIni ini;
	if(ini.init(pchPathName))
	{
		return -1;
	}
	UG_PCHAR pchIP = NULL;
	UG_PCHAR pchPort = NULL;
	UG_PCHAR pchBindIP = NULL;
	UG_PCHAR pchBindPort = NULL;
	ini.getValue("NetConfig","IP",pchIP);
	ini.getValue("NetConfig","BindIP",pchBindIP);
	ini.getValue("NetConfig","BindPort",pchBindPort);
	ini.getValue("NetConfig","Port",pchPort);
	if(!m_pTCPComm)
	{
		m_pTCPComm = new CTCPComm;
	}
	INIT_CLIENT ic;
	ic.bAsynEvent = bAsyc;
	ic.dwIP = inet_addr(pchIP);
	if(pchBindIP)
	{
		ic.dwBindIP = inet_addr(pchBindIP);
	}
	else
	{
		ic.dwBindIP = 0;
	}
	ic.dwPort = atoi(pchPort);
	ic.dwBindPort = atoi(pchBindPort);
	ic.pvCallback = (TCPCallback)TCPCltCallback;
	ic.pvKey = this;
	ic.ulRecvSocketBufferSize = 4 * SEND_RECV_SIZE;
	ic.ulSendSocketBufferSize = SEND_RECV_SIZE;
	if(m_pTCPComm->init(&ic))
	{
		cleanup();
		return UG_ULONG(-1);
	}
	return 0;
}

UG_ULONG CClientNet::disconnect()
{
	return m_pTCPComm->disconnect();
}

UG_ULONG CClientNet::postNetMessage(UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR pchData,UG_ULONG ulLen)
{
	UGGCP_T ugGcp;
	ugGcp.wDataLen = ulLen;
	ugGcp.ulSvrParam = dwParam;
	ugGcp.ulSvrType = dwMsgID;
	UG_ULONG ulRet = m_pTCPComm->sendData(&ugGcp,pchData);
	return ulRet;
}

UG_ULONG CClientNet::writeBuffer(UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR pchData,UG_ULONG ulLen)
{
	UGGCP_T ugGcp;
	ugGcp.wDataLen = ulLen;
	ugGcp.ulSvrParam = dwParam;
	ugGcp.ulSvrType = dwMsgID;
	return m_pTCPComm->writeBuffer(&ugGcp,pchData);
}

UG_ULONG CClientNet::sendBuffer()
{
	UG_ULONG ulRet = m_pTCPComm->sendBuffer();
	return ulRet;
}

UG_ULONG CClientNet::netFrame(UG_LONG lRecvCount)
{
	UG_ULONG ulRet = m_pTCPComm->recvData(lRecvCount);
	return ulRet;
}

UG_ULONG CClientNet::TCPCltCallback(PTCP_PARAM pvParam)
{
	return ((CClientNet*)(pvParam->pvKey))->TCPCltCallbackProc(pvParam);
}

UG_ULONG CClientNet::TCPCltCallbackProc(PTCP_PARAM pvParam)
{
	if(TCPCOMM_RECVDATA == pvParam->tcpEvent)
	{
		if(NET_MSG_DISCONNECT == pvParam->ugGcp.ulSvrType)
		{
			return 0;
		}
		((FnClientNet)m_pCallback)(pvParam->ugGcp.ulSvrType,pvParam->ugGcp.ulSvrParam,pvParam->pchData,pvParam->ugGcp.wDataLen);
	}
	else if(TCPCOMM_DISCONNECT == pvParam->tcpEvent)
	{
		((FnClientNet)m_pCallback)(NET_MSG_DISCONNECT,-1,NULL,0);
	}
	else if(TCPCOMM_ERROR == pvParam->tcpEvent)
	{
		if(m_pLog)
		{
			m_pLog->UGLog(pvParam->pchData);
		}
	}
	else
	{
	}
	return 0;
}

UG_ULONG CClientNet::initGetIP()
{
	if(!m_pTCPComm)
	{
		m_pTCPComm = new CTCPComm;
	}
	return m_pTCPComm->initGetIP();
}

UG_ULONG CClientNet::getIPCount()
{
	if(m_pTCPComm)
	{
		return m_pTCPComm->getIPCount();
	}
	return -1;
}

UG_DWORD CClientNet::getIP(UG_ULONG ulIndex)
{
	if(m_pTCPComm)
	{
		return m_pTCPComm->getIP(ulIndex);
	}
	return -1;
}

UG_PCHAR CClientNet::convertIP(UG_DWORD dwIP)
{
	if(m_pTCPComm)
	{
		return m_pTCPComm->convertIP(dwIP);
	}
	return NULL;
}

UG_DWORD CClientNet::convertIP(UG_PCHAR pchIP)
{
	if(m_pTCPComm)
	{
		return m_pTCPComm->convertIP(pchIP);
	}
	return -1;
}

UG_ULONG CClientNet::getErrorCode()
{
	return m_ulLastError;
}

UG_ULONG CClientNet::shutdown()
{
	cleanup();
	return 0;
}

UG_ULONG CClientNet::getServer(UG_DWORD& dwIP,UG_WORD& wPort)
{
	if(m_pTCPComm)
	{
		return m_pTCPComm->getServer(dwIP,wPort);
	}
	return -1;
}

UG_ULONG CClientNet::getHost(UG_DWORD& dwIP,UG_WORD& wPort)
{
	if(m_pTCPComm)
	{
		return m_pTCPComm->getHost(dwIP,wPort);
	}
	return -1;
}