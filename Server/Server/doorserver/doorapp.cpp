// DoorApp.cpp: implementation of the CDoorApp class.
//
//////////////////////////////////////////////////////////////////////
#include "incall.h"
extern UGBOOL				g_bCoutFile;

CDoorApp::CDoorApp()
{
	m_pWorldbaseManager = NULL;
	m_pGMManager = NULL;
	m_pSysTCPComm = NULL;
}

CDoorApp::~CDoorApp()
{
	cleanup();
}

UG_LONG CDoorApp::init(UG_PCHAR pchPathName)
{
	if(m_config.init(pchPathName))
	{
		printf("init config error, file = %s.\n",pchPathName);
		g_pLog->UGLog("init config error.");
		return -1;
	}
	if(mallocMemory())
	{
		g_pLog->UGLog("malloc memory error.");
		return -1;
	}
	m_pSysTCPComm = ::Sys_LoadModule(TCPCOMM_SVR_DLL);
	if(!m_pSysTCPComm)
	{
		printf("load tcpserver %s failed.\n",TCPCOMM_SVR_DLL);
		g_pLog->UGLog("load tcpserver %s failed.",TCPCOMM_SVR_DLL);
		return -1;
	}
	if(m_pWorldbaseManager->init(&m_config))
	{
		printf("init WorldbaseManager failed.\n");
		g_pLog->UGLog("init WorldbaseManager failed.");
		return -1;
	}
	if(m_pGMManager->init(&m_config))
	{
		printf("init GMManager failed.\n");
		g_pLog->UGLog("init GMManager failed.");
		return -1;
	}
	return 0;
}

UG_LONG CDoorApp::cleanup()
{
	if(m_pWorldbaseManager)
	{
		m_pWorldbaseManager->cleanup();
	}
	if(m_pGMManager)
	{
		m_pGMManager->cleanup();
	}
	freeMemory();
	return 0;
}

UG_LONG CDoorApp::mallocMemory()
{
	m_pWorldbaseManager = new CWorldbaseManager;
	m_pGMManager = new CGMManager;
	return 0;
}

UG_LONG CDoorApp::freeMemory()
{
	ReleaseP<CWorldbaseManager*>(m_pWorldbaseManager);
	ReleaseP<CGMManager*>(m_pGMManager);
	if(m_pSysTCPComm)
	{
		Sys_UnloadModule(m_pSysTCPComm);
		m_pSysTCPComm = NULL;
	}
	return 0;
}

UG_LONG CDoorApp::render(UG_ULONG ulTimer)
{
	if(m_pWorldbaseManager)
	{
		m_pWorldbaseManager->render(ulTimer);
	}
	if(m_pGMManager)
	{
		m_pGMManager->render(ulTimer);
	}
	return 0;
}

UG_LONG CDoorApp::coutGM()
{
	if(m_pGMManager)
	{
		try
		{
			if(g_bCoutFile)
			{
				FILE* pFile = fopen("./log/gmmanager.log","wt");
				if(pFile)
				{
					m_pGMManager->cout(pFile);
					fclose(pFile);
				}
				pFile = NULL;
			}
			else
			{
				m_pGMManager->cout(stdout);
			}
		}
		catch(...)
		{
			g_pLog->UGLog("coutGM error.");
		}
	}
	return 0;
}

UG_LONG CDoorApp::coutWB()
{
	if(m_pWorldbaseManager)
	{
		try
		{
			if(g_bCoutFile)
			{
				FILE* pFile = fopen("./log/worldbase.log","wt");
				if(pFile)
				{
					m_pWorldbaseManager->cout(pFile);
					fclose(pFile);
				}
				pFile = NULL;
			}
			else
			{
				m_pWorldbaseManager->cout(stdout);
			}
		}
		catch(...)
		{
			g_pLog->UGLog("coutWB error.");
		}
	}
	return 0;
}

UG_LONG CDoorApp::coutConfig()
{
	try
	{
		if(g_bCoutFile)
		{
			FILE* pFile = fopen("./log/config.log","wt");
			if(pFile)
			{
				m_config.cout(pFile);
				fclose(pFile);
			}
			pFile = NULL;
		}
		else
		{
			m_config.cout(stdout);
		}
	}
	catch(...)
	{
		g_pLog->UGLog("coutConfig error.");
	}
	return 0;
}

UG_LONG CDoorApp::coutAll()
{
	if(g_bCoutFile)
	{
		FILE* pFile = fopen("./log/all.log","wt");
		if(pFile)
		{
			m_config.cout(pFile);
			m_pWorldbaseManager->cout(pFile);
			m_pGMManager->cout(pFile);
			fclose(pFile);
		}
		pFile = NULL;
	}
	else
	{
		coutConfig();
		coutWB();
		coutGM();
	}
	return 0;
}

