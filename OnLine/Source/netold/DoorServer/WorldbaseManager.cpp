/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : WorldbaseManager.cpp
*owner        : Ben
*description  : worldbase管理者，管理一组worldbase的连接。
*modified     : 2004/12/20
******************************************************************************/ 

#include "incall.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWorldbaseManager::CWorldbaseManager()
{
	m_pDoorConfig = NULL;
	m_ulRenderTimer = 0;
	m_pSQLBilling = NULL;
	m_pSQLDoorServer = NULL;
	m_pWorldbaseTCPComm = NULL;
	m_mapWorldbase.clear();
	m_listWorldbaseIdle.clear();
	m_pWorldbaseArray = NULL;
}

CWorldbaseManager::~CWorldbaseManager()
{
	cleanup();
}

UG_LONG CWorldbaseManager::init(CDoorConfig* pDoorConfig)
{
	m_pDoorConfig = pDoorConfig;
	if(mallocMemory())
	{
		printf("malloc memory failed in CWorldbaseManager.\n");
		g_pLog->UGLog("malloc memory failed in CWorldbaseManager.");
		cleanup();
		return -1;
	}
	if(initSQL())
	{
		printf("initSQL failed in CWorldbaseManager.\n");
		g_pLog->UGLog("initSQL failed in CWorldbaseManager.");
		cleanup();
		return -1;
	}
	if(initTCPComm())
	{
		printf("initTCPComm failed in CWorldbaseManager.\n");
		g_pLog->UGLog("initTCPComm failed in CWorldbaseManager.");
		cleanup();
		return -1;
	}
	if(setAllWorldbaseNotOnline())
	{
		printf("setAllWorldbaseNotOnline failed in CWorldbaseManager.\n");
		g_pLog->UGLog("setAllWorldbaseNotOnline failed in CWorldbaseManager.");
		cleanup();
		return -1;
	}
	return 0;
}

UG_LONG CWorldbaseManager::render(UG_ULONG ulTimer)
{
	m_ulRenderTimer = ulTimer;
	m_pWorldbaseTCPComm->acceptPlayer();
	map<UG_PVOID,CWorldbase*>::iterator it;
	for(it = m_mapWorldbase.begin(); it != m_mapWorldbase.end();)
	{
		CWorldbase* pWB = (*it).second;
		UG_ULONG ulRet = m_pWorldbaseTCPComm->recvData(pWB->m_pvConnect,0);
		if(-1 == ulRet)
		{
			g_pLog->UGLog("m_pWorldbaseTCPComm recv data is -1, ip = %s.",m_pWorldbaseTCPComm->convertIP(pWB->m_dwPrivateIP));
			pWB->m_bOnline = FALSE;
		}
		if(pWB->m_stepWB == WB_CONNECTED_NOT_CHECK)
		{
			if(m_ulRenderTimer > pWB->m_dwTimer + 30000)
			{
				g_pLog->UGLog("m_pWorldbaseTCPComm WB_CONNECTED_NOT_CHECK timeout %d > %d + 30000.",m_ulRenderTimer,pWB->m_dwTimer);
				pWB->m_bOnline = FALSE;
			}
		}
		if(pWB->m_bOnline)
		{
			it ++;
		}
		else
		{
			if(WB_CHECK_OK == pWB->m_stepWB)
			{
				static UG_CHAR szBuffer[1024];
				sprintf(szBuffer,SQL_SET_WORLD_NO_ONLINE,pWB->m_wbData.ulID);
				UG_PVOID pvResult = NULL;
				m_pSQLDoorServer->query(szBuffer,pvResult);
				m_pSQLDoorServer->freeResult(pvResult);
			}
			disconnectWorldbase(pWB);
			m_listWorldbaseIdle.push_back(pWB);
			it = m_mapWorldbase.erase(it);
		}
	}
/*	for(it = m_mapWorldbase.begin(); it != m_mapWorldbase.end(); it ++)
	{
		CWorldbase* pWB = (*it).second;
		static UG_ULONG ulLastTimer = 0;
		if(m_ulRenderTimer - ulLastTimer > 1000)
		{
			ulLastTimer = m_ulRenderTimer;
			if(-1 == m_pWorldbaseTCPComm->writeBuffer(pWB->m_pvConnect,0x00000101,0))
			{
				g_pLog->UGLog("writeBuffer error ip = %s.",m_pWorldbaseTCPComm->convertIP(pWB->m_dwPrivateIP));
				pWB->m_bOnline = FALSE;
			}
		}
		if(-1 == m_pWorldbaseTCPComm->sendBuffer(pWB->m_pvConnect,0))
		{
			g_pLog->UGLog("sendBuffer error ip = %s.",m_pWorldbaseTCPComm->convertIP(pWB->m_dwPrivateIP));
			pWB->m_bOnline = FALSE;
		}
	}*/
	return 0;
}

UG_LONG CWorldbaseManager::cleanup()
{
	freeMemory();
	return 0;
}

UG_LONG CWorldbaseManager::mallocMemory()
{
	m_mapWorldbase.clear();
	m_listWorldbaseIdle.clear();
	m_pWorldbaseArray = new CWorldbase[MAX_CONNECT_WORLDBASE];
	for(UG_INT32 i = 0; i < MAX_CONNECT_WORLDBASE; i ++)
	{
		m_listWorldbaseIdle.push_back(m_pWorldbaseArray + i);
	}
	m_pSQLBilling = new CUGSQL;
	m_pSQLDoorServer = new CUGSQL;
	return 0;
}

UG_LONG CWorldbaseManager::freeMemory()
{
	m_mapWorldbase.clear();
	m_listWorldbaseIdle.clear();
	ReleasePA<CWorldbase*>(m_pWorldbaseArray);
	ReleaseP<CUGSQL*>(m_pSQLBilling);
	ReleaseP<CUGSQL*>(m_pSQLDoorServer);
	return 0;
}


UG_LONG CWorldbaseManager::initSQL()
{
	UG_ULONG ulRet = m_pSQLBilling->openDB(m_pDoorConfig->m_szSQLBillingHost,m_pDoorConfig->m_szSQLBillingUser,m_pDoorConfig->m_szSQLBillingPassword,m_pDoorConfig->m_szSQLBillingDBName);
	if(ulRet)
	{
		printf("open m_pSQLBillingdb error.\n");
		g_pLog->UGLog("open m_pSQLBillingdb error.");
		return ulRet;
	}
	ulRet = m_pSQLDoorServer->openDB(m_pDoorConfig->m_szSQLDoorServerHost,m_pDoorConfig->m_szSQLDoorServerUser,m_pDoorConfig->m_szSQLDoorServerPassword,m_pDoorConfig->m_szSQLDoorServerDBName);
	if(ulRet)
	{
		printf("open m_pSQLDoorServer error.\n");
		g_pLog->UGLog("open m_pSQLDoorServer error.");
		return ulRet;
	}
	return 0;
}

UG_LONG CWorldbaseManager::initTCPComm()
{
	CreateInterfaceFn create_interface = ::Sys_GetFactory(theApp.m_pSysTCPComm);
	m_pWorldbaseTCPComm = (INetServer*)create_interface(NETSERVER_INTERFACE_VERSION,NULL);
	if(!m_pWorldbaseTCPComm)
	{
		printf("create WorldbaseTCPComm interface failed.\n");
		g_pLog->UGLog("create WorldbaseTCPComm interface failed.");
		return (UG_ULONG)(-1);
	}
	INIT_NET_SERCER is;
	is.dwIP = m_pDoorConfig->m_dwHostIPForWorldbaseConnect;
	is.dwPort = m_pDoorConfig->m_wPortForWorldbaseConnect;
	is.ulMaxConnect = MAX_CONNECT_WORLDBASE;
	g_pLog->UGLog("init WorldbaseTCPComm begin, %s, %d.",m_pWorldbaseTCPComm->convertIP(is.dwIP),is.dwPort);
	UG_ULONG ulRet = m_pWorldbaseTCPComm->init(&is,(FnNetSvr)WorldbaseCB,this);
	if(ulRet)
	{
		printf("init WorldbaseTCPComm failed.\n");
		g_pLog->UGLog("init WorldbaseTCPComm failed.");
		return ulRet;
	}
	return 0;
}

UG_ULONG CWorldbaseManager::WorldbaseCB(PNET_SERVER_PARAM p)
{
	return ((CWorldbaseManager*)(p->pvKey))->WorldbaseCBProc(p);
}

UG_ULONG CWorldbaseManager::WorldbaseCBProc(PNET_SERVER_PARAM p)
{
	if(NET_MSG_CONNECT == p->dwMsgID) //connect
	{
		try
		{
			recvWorldbase(p);
		}
		catch(...)
		{
			g_pLog->UGLog("recvWorldbase function error.");
		}
	}
	else
	{
		CWorldbase* pWB = (CWorldbase*)(p->pvPlayer);
		pWB->dispatchMsg(p->dwMsgID,p->dwParam,p->pchData,p->ulLen);
	}
	return 0;
}

UG_LONG CWorldbaseManager::recvWorldbase(PNET_SERVER_PARAM p)
{
	UG_DWORD dwIP = 0;
	UG_WORD wPort = 0;
	m_pWorldbaseTCPComm->getConnect(p->pvConnect,dwIP,wPort);
	g_pLog->UGLog("recvWorldbase begin, ip = %s, port = %d.",m_pWorldbaseTCPComm->convertIP(dwIP),wPort);
	list<CWorldbase*>::iterator it = m_listWorldbaseIdle.begin();
	if(it != m_listWorldbaseIdle.end())
	{
		(*it)->m_dwPrivateIP = dwIP;
		(*it)->m_dwPrivatePort = wPort;
		(*it)->m_bOnline = TRUE;
		(*it)->m_dwTimer = Plat_MSTime();
		(*it)->m_pvConnect = p->pvConnect;
		(*it)->m_dwCurrentMaxOnline = 0;
		(*it)->m_stepWB = WB_CONNECTED_NOT_CHECK;
		(*it)->m_pParent = this;
		(*it)->m_wbData.ulCPU = 0;
		(*it)->m_wbData.ulOnlineCount = 0;
		m_pWorldbaseTCPComm->setPlayer(p->pvConnect,(*it));
		m_mapWorldbase[p->pvConnect] = *it;
		m_listWorldbaseIdle.erase(it);
		g_pLog->UGLog("recv worldbase connect ok, ip = %s, port = %d.",m_pWorldbaseTCPComm->convertIP(dwIP),wPort);
	}
	else
	{
		m_pWorldbaseTCPComm->disconnectPlayer(p->pvConnect);
		g_pLog->UGLog("recv worldbase connect failed because there is no idle worldbase memory for connect.");
	}
	return 0;
}

UG_LONG CWorldbaseManager::disconnectWorldbase(CWorldbase* p)
{
	if(p->m_pvConnect)
	{
		m_pWorldbaseTCPComm->disconnectPlayer(p->m_pvConnect);
	}
	g_pLog->UGLog("disconnectWorldbase.");
	return 0;
}

UG_LONG CWorldbaseManager::setAllWorldbaseNotOnline()
{
	UG_ULONG ulRet = UG_ULONG(-1);
	if(m_pSQLDoorServer)
	{
		UG_PVOID pvResult = NULL;
		ulRet = m_pSQLDoorServer->query(SQL_SET_ALL_NO_ONLINE,pvResult);
		m_pSQLDoorServer->freeResult(pvResult);
	}
	return (UG_LONG)ulRet;
}

UG_LONG CWorldbaseManager::cout(FILE* p)
{
	fprintf(p,"worldbase count = %d.\n",m_mapWorldbase.size());
	map<UG_PVOID,CWorldbase*>::iterator it;
	for(it = m_mapWorldbase.begin(); it != m_mapWorldbase.end(); it ++)
	{
		CWorldbase* pWB = (*it).second;
		pWB->cout(p);
	}		
	fprintf(p,"\n");
	return 0;
}
