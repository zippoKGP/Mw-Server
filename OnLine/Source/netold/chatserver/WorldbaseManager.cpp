// WorldbaseManager.cpp: implementation of the CWorldbaseManager class.
//
//////////////////////////////////////////////////////////////////////

#include "incall.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWorldbaseManager::CWorldbaseManager()
{
	m_pWorldbaseTCPComm = NULL;
	m_pWorldbaseManagerConfig = NULL;
	m_pWorldbase = NULL;
	m_pSysModule = NULL;
	m_listWorldbase.clear();
}

CWorldbaseManager::~CWorldbaseManager()
{
	cleanup();
}

UG_LONG CWorldbaseManager::init(UG_PCHAR pchPathName)
{
	m_pWorldbaseManagerConfig = new CWorldbaseManagerConfig;
	UG_LONG lRet = m_pWorldbaseManagerConfig->init(pchPathName);
	if(lRet)
	{
		printf("init config file failed.\n");
		g_pLog->UGLog("init config file failed.");
		cleanup();
		return lRet;
	}
	lRet = initWorldbaseTCPComm();
	if(lRet)
	{
		printf("initWorldbaseTCPComm failed.\n");
		g_pLog->UGLog("initWorldbaseTCPComm failed.");
		cleanup();
		return lRet;
	}
	m_pWorldbase = new CWorldbase[m_pWorldbaseManagerConfig->m_lWorldbaseCount];
	m_listWorldbase.clear();
	for(UG_LONG l = 0; l < m_pWorldbaseManagerConfig->m_lWorldbaseCount; l ++)
	{
		CWorldbase* pWorldbase = (m_pWorldbase + l);
		lRet = pWorldbase->init((m_pWorldbaseManagerConfig->m_pWorldbaseConfig) + l,m_pSysModule,m_pWorldbaseTCPComm);
		if(lRet)
		{
			printf("init chat failed.\n");
			g_pLog->UGLog("init chat failed.");
			pWorldbase->cleanup();
		}
		else
		{
			printf("init chat succeed.\n");
			g_pLog->UGLog("init chat succeed.");
			m_listWorldbase.push_back(pWorldbase);
		}
	}
	if(m_listWorldbase.size() < 1)
	{
		printf("init chat all failed.\n");
		g_pLog->UGLog("init chat all failed.");
		cleanup();
		return lRet;
	}
	return 0;
}

UG_LONG CWorldbaseManager::render(UG_ULONG ulTimer)
{
	CSyncEvent sys(&m_cs);
	m_pWorldbaseTCPComm->acceptPlayer();
	list<CWorldbase*>::iterator it;
	for(it = m_listWorldbase.begin(); it != m_listWorldbase.end(); it ++)
	{
		CWorldbase* pWorldbase = *it;
		if(pWorldbase->m_pWBConnect->m_bOnline)
		{
			UG_ULONG ulRet = m_pWorldbaseTCPComm->recvData(pWorldbase->m_pWBConnect->m_pvConnect,0);
			if(-1 == ulRet)
			{
				g_pLog->UGLog("WorldbaseTCPComm disconnected in render ip = %s, port = %d, timer is %s.",convertip(pWorldbase->m_pWBConnect->m_dwIP),pWorldbase->m_pWBConnect->m_wPort,g_pLog->getTime());
				pWorldbase->disconnectAll();
				continue;
			}
			else
			{
				pWorldbase->render(ulTimer);
			}
		}
	}
	return 0;
}

UG_LONG CWorldbaseManager::cleanup()
{
	m_listWorldbase.clear();
	ReleasePA<CWorldbase*>(m_pWorldbase);
	ReleaseP<CWorldbaseManagerConfig*>(m_pWorldbaseManagerConfig);
	if(m_pWorldbaseTCPComm)
	{
		m_pWorldbaseTCPComm->shutdown();
		m_pWorldbaseTCPComm = NULL;
	}
	if(m_pSysModule)
	{
		Sys_UnloadModule(m_pSysModule);
		m_pSysModule = NULL;
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
		CSyncEvent sys(&m_cs);
		UG_ULONG ulRet = UG_ULONG(-1);
		list<CWorldbase*>::iterator it;
		for(it = m_listWorldbase.begin(); it != m_listWorldbase.end(); it ++)
		{
			CWorldbase* pWorldbase = *it;
			ulRet = pWorldbase->recvWorldbase(p);
			if(!ulRet)
			{
				break;
			}
		}
		if(ulRet)
		{
			m_pWorldbaseTCPComm->disconnectPlayer(p->pvConnect);
		}
	}
	else
	{
		CWorldbase* pWorldbase = (CWorldbase*)(p->pvPlayer);
		pWorldbase->WorldbaseCBProc(p);
	}
	return 0;
}

UG_LONG CWorldbaseManager::initWorldbaseTCPComm()
{
	m_pSysModule = ::Sys_LoadModule(TCPCOMM_SVR_DLL);
	if(!m_pSysModule)
	{
		printf("load tcpserver %s failed.\n");
		g_pLog->UGLog("load tcpserver %s failed.",TCPCOMM_SVR_DLL);
		return -1;
	}
	CreateInterfaceFn create_interface = ::Sys_GetFactory(m_pSysModule);
	m_pWorldbaseTCPComm = (INetServer*)create_interface(NETSERVER_INTERFACE_VERSION,NULL);
	if(!m_pWorldbaseTCPComm)
	{
		printf("create WorldbaseTCPComm interface failed.\n");
		g_pLog->UGLog("create WorldbaseTCPComm interface failed.");
		return -1;
	}
	INIT_NET_SERCER is;
	is.dwIP = m_pWorldbaseManagerConfig->m_dwIPForWorldbase;
	is.dwPort = m_pWorldbaseManagerConfig->m_dwPortForWorldbase;
	is.ulMaxConnect = m_pWorldbaseManagerConfig->m_lWorldbaseCount;
	UG_ULONG ulRet = m_pWorldbaseTCPComm->init(&is,FnNetSvr(WorldbaseCB),this);
	if(ulRet)
	{
		CCout::coutAll();
		printf("init WorldbaseTCPComm failed ip = %s, port = %d connect count = %d.\n",convertip(is.dwIP),is.dwPort,is.ulMaxConnect);
		g_pLog->UGLog("init WorldbaseTCPComm failed.");
		return -1;
	}
	return 0;
}

UG_LONG CWorldbaseManager::coutWorldbase(FILE* p)
{
	CSyncEvent sys(&m_cs);
	list<CWorldbase*>::iterator it;
	for(it = m_listWorldbase.begin(); it != m_listWorldbase.end(); it ++)
	{
		CWorldbase* pWorldbase = *it;
		pWorldbase->m_pWBConnect->cout(p);
		fprintf(p,"\n");
	}
	return 0;
}

UG_LONG CWorldbaseManager::coutPlayerManager(FILE* p)
{
	CSyncEvent sys(&m_cs);
	list<CWorldbase*>::iterator it;
	for(it = m_listWorldbase.begin(); it != m_listWorldbase.end(); it ++)
	{
		CWorldbase* pWorldbase = *it;
		pWorldbase->m_pPlayerManager->cout(p);
		fprintf(p,"\n");
	}
	return 0;
}

UG_LONG CWorldbaseManager::coutSceneManager(FILE* p)
{
	CSyncEvent sys(&m_cs);
	list<CWorldbase*>::iterator it;
	for(it = m_listWorldbase.begin(); it != m_listWorldbase.end(); it ++)
	{
		CWorldbase* pWorldbase = *it;
		pWorldbase->m_pSceneManager->cout(p);
		fprintf(p,"\n");
	}
	return 0;
}

UG_LONG CWorldbaseManager::coutAll(FILE* p)
{
	CSyncEvent sys(&m_cs);
	list<CWorldbase*>::iterator it;
	for(it = m_listWorldbase.begin(); it != m_listWorldbase.end(); it ++)
	{
		CWorldbase* pWorldbase = *it;
		pWorldbase->m_pWBConnect->cout(p);
		pWorldbase->m_pPlayerManager->cout(p);
		pWorldbase->m_pSceneManager->cout(p);
		fprintf(p,"\n");
	}
	return 0;
}

UG_LONG CWorldbaseManager::coutConfig(FILE* p)
{
	m_pWorldbaseManagerConfig->cout(p);
	return 0;
}
