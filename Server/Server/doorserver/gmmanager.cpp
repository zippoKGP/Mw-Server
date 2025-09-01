/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : GMManager.cpp
*owner        : Ben
*description  : manager管理者，管理一组manager的连接。
*modified     : 2004/12/20
******************************************************************************/

#include "incall.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGMManager::CGMManager()
{
	m_ulRenderTimer = 0;
	m_pTCPComm = NULL;
	m_pDoorConfig = NULL;
	m_mapGM.clear();
	m_listGMIdle.clear();
	m_pGMArray = NULL;
	m_pWBData = NULL;
}

CGMManager::~CGMManager()
{
	cleanup();
}

UG_LONG CGMManager::init(CDoorConfig* pDoorConfig)
{
	m_pDoorConfig = pDoorConfig;
	if(mallocMemory())
	{
		printf("malloc memory error in class CGMManager.\n");
		g_pLog->UGLog("malloc memory error in CGMManager.");
		return -1;
	}
	if(initTCPComm())
	{
		printf("initTCPComm error in class CGMManager.\n");
		g_pLog->UGLog("initTCPComm error in CGMManager.");
		return -1;
	}
	return 0;
}

UG_LONG CGMManager::render(UG_ULONG ulTimer)
{
	m_ulRenderTimer = ulTimer;
	m_pTCPComm->acceptPlayer();
	map<UG_PVOID,CGM*>::iterator it;
	for(it = m_mapGM.begin(); it != m_mapGM.end();)
	{
		CGM* p = (*it).second;
		double db = 0;
		double db1 = 0;
		UG_ULONG ulRet = m_pTCPComm->recvData(p->m_pvConnect,db,db1);
		if(-1 == ulRet)
		{
			g_pLog->UGLog("m_pWorldbaseTCPComm recv data is -1.");
			p->m_bOnline = FALSE;
		}
		if(p->m_stepGM == GM_CONNECTED_NOT_CHECK)
		{
			if(UGLONG(m_ulRenderTimer - p->m_dwTimer) > 30000)
			{
				g_pLog->UGLog("m_pTCPComm GM_CONNECTED_NOT_CHECK timeout %d > %d + 30000.",m_ulRenderTimer,p->m_dwTimer);
				p->m_bOnline = FALSE;
			}
		}
		if(p->m_bOnline)
		{
			it ++;
		}
		else
		{
			if(GM_CHECK_OK == p->m_stepGM)
			{
			}
			disconnectGM(p);
			it = m_mapGM.erase(it);
		}
	}
	for(it = m_mapGM.begin(); it != m_mapGM.end(); it ++)
	{
		CGM* p = (*it).second;
		m_pTCPComm->sendBuffer(p->m_pvConnect,1500);
	}
	return 0;
}

UG_LONG CGMManager::cleanup()
{
	freeMemory();
	return 0;
}

UG_LONG CGMManager::mallocMemory()
{
	m_pWBData = new SERVERINFO[MAX_WORLD];
	m_mapGM.clear();
	m_listGMIdle.clear();
	m_pGMArray = new CGM[MAX_CONNECT_MANAGER];
	for(UG_INT32 i = 0; i < MAX_CONNECT_MANAGER; i ++)
	{
		m_listGMIdle.push_back(m_pGMArray + i);
	}
	return 0;
}

UG_LONG CGMManager::freeMemory()
{
	m_mapGM.clear();
	m_listGMIdle.clear();
	ReleasePA<CGM*>(m_pGMArray);
	ReleasePA(m_pWBData);
	return 0;
}


UG_LONG CGMManager::recvGM(PNET_SERVER_PARAM p)
{
	UG_DWORD dwIP = 0;
	UG_WORD wPort = 0;
	m_pTCPComm->getConnect(p->pvConnect,dwIP,wPort);
	g_pLog->UGLog("recvGM begin, ip = %s, port = %d.",m_pTCPComm->convertIP(dwIP),wPort);
	list<CGM*>::iterator it = m_listGMIdle.begin();
	if(it != m_listGMIdle.end())
	{
		(*it)->m_dwIP = dwIP;
		(*it)->m_dwPort = wPort;
		(*it)->m_bOnline = TRUE;
		(*it)->m_dwTimer = Plat_MSTime();
		(*it)->m_pvConnect = p->pvConnect;
		(*it)->m_stepGM = GM_CONNECTED_NOT_CHECK;
		(*it)->m_pParent = this;
		m_pTCPComm->setPlayer(p->pvConnect,(*it));
		m_mapGM[p->pvConnect] = *it;
		m_listGMIdle.erase(it);
		g_pLog->UGLog("recv gm connect ok, ip = %s, port = %d.",m_pTCPComm->convertIP(dwIP),wPort);
	}
	else
	{
		m_pTCPComm->disconnectPlayer(p->pvConnect);
		g_pLog->UGLog("recv gm connect failed because there is no idle gm memory for connect.");
	}
	return 0;
}

UG_LONG CGMManager::disconnectGM(CGM* p)
{
	if(p->m_pvConnect)
	{
		m_pTCPComm->disconnectPlayer(p->m_pvConnect);
	}
	g_pLog->UGLog("disconnectGM.");
	m_listGMIdle.push_back(p);
	return 0;
}

UG_LONG CGMManager::initTCPComm()
{
	CreateInterfaceFn create_interface = ::Sys_GetFactory(theApp.m_pSysTCPComm);
	m_pTCPComm = (INetServer*)create_interface(NETSERVER_INTERFACE_VERSION,NULL);
	if(!m_pTCPComm)
	{
		g_pLog->UGLog("create TCPComm interface failed.");
		return (UG_ULONG)(-1);
	}

	m_pTCPComm->setSocket(5 * 1024 * 1024,5 * 1024 * 1024);
	m_pTCPComm->setRecvBuffer(1024 * 500);
	m_pTCPComm->setWriteBuffer(500 * 1024);
	
	INIT_NET_SERCER is;
	is.dwIP = m_pDoorConfig->m_dwHostIPForManagerConnect;
	is.dwPort = m_pDoorConfig->m_wPortForManagerConnect;
	is.ulMaxConnect = MAX_CONNECT_MANAGER;
	g_pLog->UGLog("init TCPComm begin, %s, %d.",m_pTCPComm->convertIP(is.dwIP),is.dwPort);
	UG_ULONG ulRet = m_pTCPComm->init(&is,(FnNetSvr)GMCB,this);
	if(ulRet)
	{
		printf("init TCPComm failed ip = %s, port = %d.\n",convertip(is.dwIP),is.dwPort);
		g_pLog->UGLog("init TCPComm failed.");
		return ulRet;
	}
	return 0;
}

UG_ULONG CGMManager::GMCB(PNET_SERVER_PARAM p)
{
	return ((CGMManager*)(p->pvKey))->GMCBProc(p);
}

UG_ULONG CGMManager::GMCBProc(PNET_SERVER_PARAM p)
{
	if(NET_MSG_CONNECT == p->dwMsgID) //connect
	{
		recvGM(p);
	}
	else
	{
		CGM* pGM = (CGM*)(p->pvPlayer);
		pGM->dispatchMsg(p->dwMsgID,p->dwParam,p->pchData,p->ulLen);
	}
	return 0;
}

UG_LONG CGMManager::cout(FILE* p)
{
	fprintf(p,"gm manager count = %d.\n",m_mapGM.size());
	map<UG_PVOID,CGM*>::iterator it;
	for(it = m_mapGM.begin(); it != m_mapGM.end(); it ++)
	{
		CGM* pGM = (*it).second;
		pGM->cout(p);
	}		
	fprintf(p,"\n");
	return 0;
}

UGLONG CGMManager::getWBData(PSERVERINFO& pWBData,UGINT& nCount)
{
	pWBData = m_pWBData;
	return theApp.m_pWorldbaseManager->getWBData(pWBData,nCount);
}

