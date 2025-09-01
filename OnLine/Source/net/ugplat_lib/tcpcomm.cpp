/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : TCPComm.cpp
*owner        : Ben
*description  : TCPComm接口类的头文件，用于TCP数据传输，包含客户端和服务器端。
*modified     : 2004/12/20
******************************************************************************/ 

#include "IncAll.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTCPComm::CTCPComm()
{
	m_pServerComm = NULL;
	m_pClientComm = NULL;
	m_ulIPCount = 0;
	m_pdwIP = NULL;
	m_nRecv = 0;
	m_nSend = 0;
	m_nWrite = 0;
	m_dwRecv = 0;
}

CTCPComm::~CTCPComm()
{
	cleanup();
}

UG_ULONG CTCPComm::cleanup()
{
	CTCPServer* pServer = (CTCPServer*)m_pServerComm;
	ReleaseP<CTCPServer*>(pServer);
	m_pServerComm = NULL;
	CTCPClient* pClient = (CTCPClient*)m_pClientComm;
	ReleaseP<CTCPClient*>(pClient);
	m_pClientComm = NULL;
	m_nRecv = 0;
	m_nSend = 0;
	m_nWrite = 0;
	m_dwRecv = 0;
	return 0;
}

UG_ULONG CTCPComm::init(PINIT_SERCER pInit)
{
	if(m_pServerComm || m_pClientComm)
	{
		return -1;
	}
	CTCPServer* p = new CTCPServer;
	m_pServerComm = p;
	if(m_nRecv || m_nSend)
	{
		p->setSocket(m_nSend,m_nRecv);
	}
	if(m_nWrite)
	{
		p->setWriteBuffer(m_nWrite);
	}
	if(m_dwRecv)
	{
		p->setRecvBuffer(m_dwRecv);
	}
	return p->init(pInit);
}

UG_ULONG CTCPComm::disconnect(UG_PVOID pvConnect)
{
	if(m_pServerComm)
	{
		return ((CTCPServer*)m_pServerComm)->disconnect(pvConnect);
	}
	return -1;
}

UG_ULONG CTCPComm::writeBuffer(PUGGCP_T pUGGcp,UG_PVOID pvConnect,UG_PCHAR pchBuffer)
{
	if(m_pServerComm)
	{
		return ((CTCPServer*)m_pServerComm)->writeBuffer(pUGGcp,pvConnect,pchBuffer);
	}
	return -1;
}

UG_ULONG CTCPComm::writeBuffer(PUGGCP_T pUGGcp,UG_PVOID pvConnect,UG_PCHAR pchBuffer,DWORD dwOverflow)
{
	if(m_pServerComm)
	{
		return ((CTCPServer*)m_pServerComm)->writeBuffer(pUGGcp,pvConnect,pchBuffer,dwOverflow);
	}
	return -1;
}

UG_ULONG CTCPComm::sendData(PUGGCP_T pUGGcp,UG_PVOID pvConnect,UG_PCHAR pchBuffer)
{
	if(m_pServerComm)
	{
		return ((CTCPServer*)m_pServerComm)->sendData(pUGGcp,pvConnect,pchBuffer);
	}
	return -1;
}

UG_ULONG CTCPComm::sendBuffer(UG_PVOID pvConnect,UG_ULONG ulSendSize)
{
	if(m_pServerComm)
	{
		return ((CTCPServer*)m_pServerComm)->sendBuffer(pvConnect,ulSendSize);
	}
	return -1;
}

UG_ULONG CTCPComm::recvData(UG_PVOID pvConnect,double& dbRecv,double& dbRecvs)
{
	if(m_pServerComm)
	{
		return ((CTCPServer*)m_pServerComm)->recvData(pvConnect,dbRecv,dbRecvs);
	}
	return -1;
}


UG_ULONG CTCPComm::init(PINIT_CLIENT pInit)
{
	if(m_pServerComm || m_pClientComm)
	{
		return -1;
	}
	CTCPClient* p = new CTCPClient;
	m_pClientComm = p;
	return p->init(pInit);
}
	
UG_ULONG CTCPComm::initNoBlock(PINIT_CLIENT pInit)
{
	if(m_pClientComm)
	{
		CTCPClient* p = (CTCPClient*)m_pClientComm;
		return p->initNoBlock(pInit);
	}
	else
	{
		CTCPClient* p = new CTCPClient;
		m_pClientComm = p;
		return p->initNoBlock(pInit);
	}
	return 0;
}

UG_ULONG CTCPComm::disconnect()
{
	if(m_pClientComm)
	{
		return ((CTCPClient*)m_pClientComm)->disconnect();
	}
	return -1;
}

UG_ULONG CTCPComm::reconnect(PINIT_CLIENT pInit)
{
	CTCPClient* p = (CTCPClient*)m_pClientComm;
	ReleaseP<CTCPClient*>(p);
	p = new CTCPClient;
	m_pClientComm = p;
	return p->init(pInit);
}

UG_ULONG CTCPComm::sendData(PUGGCP_T pUGGcp,UG_PCHAR pchBuffer)
{
	if(m_pClientComm)
	{
		return ((CTCPClient*)m_pClientComm)->sendData(pUGGcp,pchBuffer);
	}
	return -1;
}

UG_ULONG CTCPComm::recvData()
{
	if(m_pClientComm)
	{
		return ((CTCPClient*)m_pClientComm)->recvData();
	}
	return -1;
}

UG_ULONG CTCPComm::initGetIP()
{
	UG_WORD wVersionRequested = MAKEWORD(2,2);
	WSADATA wsaData;
	memset(&wsaData,0,sizeof(WSADATA));
	WSAStartup(wVersionRequested,&wsaData);
	UG_CHAR szHostName[1024] = {0};
	gethostname(szHostName,1024);
	struct hostent* pHt = gethostbyname(szHostName);
	m_ulIPCount = 0;
	ReleasePA(m_pdwIP);
	for(;;)
	{
		UG_PCHAR p = (pHt->h_addr_list)[m_ulIPCount];
		if(p)
		{
			m_ulIPCount ++;
		}
		else
		{
			break;
		}
	}
	if(m_ulIPCount)
	{
		m_pdwIP = new UG_DWORD[m_ulIPCount];
		for(UG_INT32 i = 0; ; i ++)
		{
			LPIN_ADDR pIn = (LPIN_ADDR)((pHt->h_addr_list)[i]);
			if(pIn)
			{
				*(m_pdwIP + i) = pIn->S_un.S_addr;
			}
			else
			{
				break;
			}
		}
	}
	WSACleanup();
	return 0;
}

UG_DWORD CTCPComm::getIP(UG_ULONG ulIndex)
{
	if(ulIndex < m_ulIPCount)
	{
		return *(m_pdwIP + ulIndex);
	}
	return 0;
}

UG_PCHAR CTCPComm::convertIP(UG_DWORD dwIP)
{
	IN_ADDR in;
	in.S_un.S_addr = dwIP;
	return inet_ntoa(in);
}

UG_DWORD CTCPComm::convertIP(UG_PCHAR pchIP)
{
	return inet_addr(pchIP);
}

UG_ULONG CTCPComm::getIPCount()
{
	return m_ulIPCount;
}

UG_ULONG CTCPComm::getConnect(UG_PVOID pvConnect,UG_DWORD& dwIP,UG_WORD& wPort,SOCKET& hSocket)
{
	if(m_pServerComm)
	{
		return ((CTCPServer*)m_pServerComm)->getConnect(pvConnect,dwIP,wPort,hSocket);
	}
	return -1;
}

UG_ULONG CTCPComm::setPlayer(UG_PVOID pvConnect,UG_PVOID pvPlayer)
{
	if(m_pServerComm)
	{
		return ((CTCPServer*)m_pServerComm)->setPlayer(pvConnect,pvPlayer);
	}
	return -1;
}

UG_ULONG CTCPComm::setPlayer(UG_PVOID pvPlayer)
{
	if(m_pClientComm)
	{
		return ((CTCPClient*)m_pClientComm)->setPlayer(pvPlayer);
	}
	return -1;
}

UG_ULONG CTCPComm::getServer(UG_DWORD& dwIP,UG_WORD& wPort)
{
	if(m_pClientComm)
	{
		return ((CTCPClient*)m_pClientComm)->getServer(dwIP,wPort);
	}
	return UG_ULONG(-1);
}

UG_ULONG CTCPComm::getHost(UG_DWORD& dwIP,UG_WORD& wPort)
{
	if(m_pClientComm)
	{
		return ((CTCPClient*)m_pClientComm)->getHost(dwIP,wPort);
	}
	return UG_ULONG(-1);
}


UG_ULONG CTCPComm::acceptPlayer()
{
	if(m_pServerComm)
	{
		return ((CTCPServer*)m_pServerComm)->acceptPlayer();
	}
	return -1;
}

UGINT CTCPComm::setSocket(UG_INT32 nSend,UG_INT32 nRecv)
{
	m_nRecv = nRecv;
	m_nSend = nSend;
	return 0;
}

UGINT CTCPComm::setWriteBuffer(UG_INT32 nWrite)
{
	m_nWrite = nWrite;
	return 0;
}

UGINT CTCPComm::setRecvBuffer(UG_DWORD dwRecv)
{
	m_dwRecv = dwRecv;
	return 0;
}

