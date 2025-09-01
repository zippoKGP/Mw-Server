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
	m_pWorldbaseSQL = NULL;
	m_nWorldbaseSQLCount = 0;
	m_dwLastRenderTimer = 0;
	m_dwSendOnlineTimer = 0;
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
		printf("malloc memory failed in class CWorldbaseManager.\n");
		g_pLog->UGLog("malloc memory failed in CWorldbaseManager.");
		return -1;
	}
	if(initSQL())
	{
		printf("initSQL failed in class CWorldbaseManager.\n");
		g_pLog->UGLog("initSQL failed in CWorldbaseManager.");
		return -1;
	}
	if(initTCPComm())
	{
		printf("initTCPComm failed in class CWorldbaseManager.\n");
		g_pLog->UGLog("initTCPComm failed in CWorldbaseManager.");
		return -1;
	}
	if(setAllWorldbaseNotOnline())
	{
		printf("setAllWorldbaseNotOnline failed in class CWorldbaseManager.\n");
		g_pLog->UGLog("setAllWorldbaseNotOnline failed in CWorldbaseManager.");
		return -1;
	}
	getAllWorldbase();
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
		double db = 0;
		double db1 = 0;
		UG_ULONG ulRet = m_pWorldbaseTCPComm->recvData(pWB->m_pvConnect,db,db1);
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
		
		ulRet = m_pWorldbaseTCPComm->sendBuffer(pWB->m_pvConnect,1000);
		if(-1 == ulRet)
		{
			g_pLog->UGLog("m_pWorldbaseTCPComm sendBuffer is -1, ip = %s.",m_pWorldbaseTCPComm->convertIP(pWB->m_dwPrivateIP));
			pWB->m_bOnline = FALSE;
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
		if(pWB->m_bOnline)
		{
			if(m_ulRenderTimer - m_dwLastRenderTimer > 18 * 60 * 1000)
			{
				pWB->resetCurrentMaxOnline();
				m_dwLastRenderTimer = m_ulRenderTimer;
			}
		}
	}
	return 0;
}

UG_LONG CWorldbaseManager::cleanup()
{
	setAllWorldbaseNotOnline();
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
	ReleasePA<CWorldbase*>(m_pWorldbaseSQL);
	ReleaseP<CUGSQL*>(m_pSQLBilling);
	ReleaseP<CUGSQL*>(m_pSQLDoorServer);
	return 0;
}

UG_LONG CWorldbaseManager::initSQL()
{
	UG_ULONG ulRet = m_pSQLBilling->openDB(m_pDoorConfig->m_szSQLBillingHost,m_pDoorConfig->m_szSQLBillingUser,m_pDoorConfig->m_szSQLBillingPassword,m_pDoorConfig->m_szSQLBillingDBName);
	if(ulRet)
	{
		printf("open Billing db error, error = %s.\n",m_pSQLBilling->getError());
		g_pLog->UGLog("open m_pSQLBillingdb error.%s.",m_pSQLBilling->getError());
		return ulRet;
	}
	ulRet = m_pSQLDoorServer->openDB(m_pDoorConfig->m_szSQLDoorServerHost,m_pDoorConfig->m_szSQLDoorServerUser,m_pDoorConfig->m_szSQLDoorServerPassword,m_pDoorConfig->m_szSQLDoorServerDBName);
	if(ulRet)
	{
		printf("open DoorServer db error. error = %s.\n",m_pSQLDoorServer->getError());
		g_pLog->UGLog("open m_pSQLDoorServer error.%s,%s,%s,%s,%s,%d.",m_pDoorConfig->m_szSQLDoorServerHost,m_pDoorConfig->m_szSQLDoorServerUser,m_pDoorConfig->m_szSQLDoorServerPassword,m_pDoorConfig->m_szSQLDoorServerDBName,m_pSQLDoorServer->getError(),m_pSQLDoorServer->getErrorCode());
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
		g_pLog->UGLog("create WorldbaseTCPComm interface failed.");
		return (UG_ULONG)(-1);
	}

	m_pWorldbaseTCPComm->setSocket(5 * 1024 * 1024,5 * 1024 * 1024);
	m_pWorldbaseTCPComm->setRecvBuffer(1024 * 500);
	m_pWorldbaseTCPComm->setWriteBuffer(500 * 1024);
	
	INIT_NET_SERCER is;
	is.dwIP = m_pDoorConfig->m_dwHostIPForWorldbaseConnect;
	is.dwPort = m_pDoorConfig->m_wPortForWorldbaseConnect;
	is.ulMaxConnect = MAX_CONNECT_WORLDBASE;
	g_pLog->UGLog("init WorldbaseTCPComm begin, %s, %d.",m_pWorldbaseTCPComm->convertIP(is.dwIP),is.dwPort);
	UG_ULONG ulRet = m_pWorldbaseTCPComm->init(&is,(FnNetSvr)WorldbaseCB,this);
	if(ulRet)
	{
		printf("server init failed in server_net.dll, ip = %s, port = %d.\n",convertip(is.dwIP),is.dwPort);
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
	UGDWORD dwIP = 0;
	UGWORD wPort = 0;
	m_pWorldbaseTCPComm->getConnect(p->pvConnect,dwIP,wPort);
	g_pLog->UGLog("recvWorldbase begin, ip = %s, port = %d.",m_pWorldbaseTCPComm->convertIP(dwIP),wPort);
	list<CWorldbase*>::iterator it = m_listWorldbaseIdle.begin();
	if(it != m_listWorldbaseIdle.end())
	{
		(*it)->m_dwPrivateIP = dwIP;
		(*it)->m_dwPrivatePort = wPort;
		(*it)->m_bOnline = TRUE;
		(*it)->m_dwTimer = Plat_MSTime();
		(*it)->m_dwLastRecvDataTimer = Plat_MSTime();
		(*it)->m_pvConnect = p->pvConnect;
		(*it)->m_stepWB = WB_CONNECTED_NOT_CHECK;
		(*it)->m_pParent = this;
		(*it)->m_wbData.ulCPU = 0;
		(*it)->m_wbData.ulOnlineCount = 0;
		m_pWorldbaseTCPComm->setPlayer(p->pvConnect,(*it));
		m_mapWorldbase[p->pvConnect] = *it;
		m_listWorldbaseIdle.erase(it);
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
		p->disconnect();
		m_pWorldbaseTCPComm->disconnectPlayer(p->m_pvConnect);
	}
	else
	{
		g_pLog->UGLog("disconnectWorldbase error %d.",__LINE__);
	}
	return 0;
}

UG_LONG CWorldbaseManager::setAllWorldbaseNotOnline()
{
	UG_ULONG ulRet = UG_ULONG(-1);
	if(m_pSQLDoorServer)
	{
		UG_PVOID pvResult = NULL;
		ulRet = m_pSQLDoorServer->query(SQL_SET_ALL_NO_ONLINE,pvResult);
		if(-1 == ulRet)
		{
			g_pLog->UGLog("error in setAllWorldbaseNotOnline, %d.",__LINE__);
		}
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

UGLONG CWorldbaseManager::getWBData(PSERVERINFO pWBData,UGINT& nCount)
{
	nCount = m_mapWorldbase.size();
	if(nCount > MAX_WORLD)
	{
		nCount = 0;
		return __LINE__;
	}
	map<UG_PVOID,CWorldbase*>::iterator it;
	UGINT nIndex = 0;
	for(it = m_mapWorldbase.begin(); it != m_mapWorldbase.end(); it ++)
	{
		CWorldbase* pWB = (*it).second;
		Q_strncpy((pWBData + nIndex)->szServerName,pWB->m_wbData.szName,20);
		(pWBData + nIndex)->nServerID = pWB->m_wbData.ulID;
		(pWBData + nIndex)->nIP = pWB->m_wbData.dwIP;
		(pWBData + nIndex)->nPort = pWB->m_wbData.dwPort;
		(pWBData + nIndex)->bLine = 1;
		(pWBData + nIndex)->nLineNum = pWB->m_wbData.ulOnlineCount;
		(pWBData + nIndex)->nLoginNum = pWB->m_dwLoginCount;
		(pWBData + nIndex)->nLogoutNum = pWB->m_dwLogoutCount;
		nIndex ++;
	}		
	return 0;
}

UGLONG CWorldbaseManager::getAllWorldbase()
{
	UGDWORD dwRows = 0;
	char szQuery[1024];
	UGPVOID pvResult = NULL;
	sprintf(szQuery,SQL_GET_ALL_WORLDBASE);
	UGDWORD dwRet = m_pSQLDoorServer->query(szQuery,pvResult,dwRows);
	if(dwRet)
	{
		if(pvResult)
		{
			m_pSQLDoorServer->freeResult(pvResult);
		}
		return dwRet;
	}
	if(dwRows < 1)
	{
		m_pSQLDoorServer->freeResult(pvResult);
		return __LINE__;
	}
	m_pWorldbaseSQL = new CWorldbase[dwRows];
	m_nWorldbaseSQLCount = dwRows;
	
	for(int i = 0; i < int(dwRows); i ++)
	{
		UGPCHAR pchData = NULL;
		m_pSQLDoorServer->getQueryResult(pvResult,i,0,pchData);
		strcpy((m_pWorldbaseSQL + i)->m_wbData.szName,pchData);

		pchData = NULL;
		m_pSQLDoorServer->getQueryResult(pvResult,i,1,pchData);
		(m_pWorldbaseSQL + i)->m_wbData.ulID = atoi(pchData);

		pchData = NULL;
		m_pSQLDoorServer->getQueryResult(pvResult,i,2,pchData);
		(m_pWorldbaseSQL + i)->m_wbData.dwIP = inet_addr(pchData);
		
		pchData = NULL;
		m_pSQLDoorServer->getQueryResult(pvResult,i,3,pchData);
		(m_pWorldbaseSQL + i)->m_dwPrivateIP = inet_addr(pchData);
		
		pchData = NULL;
		m_pSQLDoorServer->getQueryResult(pvResult,i,4,pchData);
		(m_pWorldbaseSQL + i)->m_wbData.dwPort = atoi(pchData);
		
		pchData = NULL;
		m_pSQLDoorServer->getQueryResult(pvResult,i,5,pchData);
		(m_pWorldbaseSQL + i)->m_dwPrivatePort = atoi(pchData);
		
		pchData = NULL;
		m_pSQLDoorServer->getQueryResult(pvResult,i,6,pchData);
		(m_pWorldbaseSQL + i)->m_wbData.ulOnlineCount = atoi(pchData);
		
		
		pchData = NULL;
		m_pSQLDoorServer->getQueryResult(pvResult,i,7,pchData);
		(m_pWorldbaseSQL + i)->m_dwCurrentMaxOnline = atoi(pchData);
		
		pchData = NULL;
		m_pSQLDoorServer->getQueryResult(pvResult,i,8,pchData);
		(m_pWorldbaseSQL + i)->m_bOnline = FALSE;

		pchData = NULL;
		m_pSQLDoorServer->getQueryResult(pvResult,i,9,pchData);
		(m_pWorldbaseSQL + i)->m_wbData.ulHistoryMaxCount = atoi(pchData);
		
		g_pLog->UGLog("private ip = %s, current = %d, history = %d.",convertip((m_pWorldbaseSQL + i)->m_dwPrivateIP),(m_pWorldbaseSQL + i)->m_dwCurrentMaxOnline,(m_pWorldbaseSQL + i)->m_wbData.ulHistoryMaxCount);
		
	}
	for(i = 0; i < int(dwRows - 1); i ++)
	{
		CWorldbase* pI = m_pWorldbaseSQL + i;
		for(int j = i + 1; j < int(dwRows); j ++)
		{
			CWorldbase* pJ = m_pWorldbaseSQL + j;
			if((pI->m_dwPrivateIP == pJ->m_dwPrivateIP) && (pI->m_dwPrivatePort == pJ->m_dwPrivatePort))
			{
				printf("init error because has two ip = %s and port = %d is EQ. line = %d.\n",convertip(pI->m_dwPrivateIP),pI->m_dwPrivatePort,__LINE__);
				g_pLog->UGLog("init error because has two ip = %s and port = %d is EQ. line = %d.",convertip(pI->m_dwPrivateIP),pI->m_dwPrivatePort,__LINE__);
				return __LINE__;
			}
		}
	}

	m_pSQLDoorServer->freeResult(pvResult);
	return 0;
}

