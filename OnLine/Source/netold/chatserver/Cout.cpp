// Cout.cpp: implementation of the CCout class.
//
//////////////////////////////////////////////////////////////////////

#include "incall.h"
extern CWorldbaseManager	theApp;
extern UG_BOOL				g_bCoutFile;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCout::CCout()
{

}

CCout::~CCout()
{

}

UG_LONG CCout::coutConfig()
{
	try
	{
		if(g_bCoutFile)
		{
			FILE* pFile = fopen("./config.log","wt");
			if(pFile)
			{
				theApp.coutConfig(pFile);
				fclose(pFile);
			}
			pFile = NULL;
		}
		else
		{
			theApp.coutConfig(stdout);
			fprintf(stdout,"\n");
		}
	}
	catch(...)
	{
		g_pLog->UGLog("coutConfig error!");
	}
	return 0;
}

UG_LONG CCout::coutWorldbase()
{
	try
	{
		if(g_bCoutFile)
		{
			FILE* pFile = fopen("./worldbase.log","wt");
			if(pFile)
			{
				theApp.coutWorldbase(pFile);
				fclose(pFile);
			}
			pFile = NULL;
		}
		else
		{
			theApp.coutWorldbase(stdout);
			fprintf(stdout,"\n");
		}
	}
	catch(...)
	{
		g_pLog->UGLog("coutWorldbase error!");
	}
	return 0;
}

UG_LONG CCout::coutPlayerManager()
{
	try
	{
		if(g_bCoutFile)
		{
			FILE* pFile = fopen("./players.log","wt");
			if(pFile)
			{
				theApp.coutPlayerManager(pFile);
				fclose(pFile);
			}
			pFile = NULL;
		}
		else
		{
			theApp.coutPlayerManager(stdout);
			fprintf(stdout,"\n");
		}
	}
	catch(...)
	{
		g_pLog->UGLog("coutPlayerManager error!");
	}
	return 0;
}

UG_LONG CCout::coutSceneManager()
{
	try
	{
		if(g_bCoutFile)
		{
			FILE* pFile = fopen("./scene.log","wt");
			if(pFile)
			{
				theApp.coutSceneManager(pFile);
				fclose(pFile);
			}
			pFile = NULL;
		}
		else
		{
			theApp.coutSceneManager(stdout);
			fprintf(stdout,"\n");
		}
	}
	catch(...)
	{
		g_pLog->UGLog("coutSceneManager error!");
	}
	return 0;
}

UG_LONG CCout::coutAll()
{
	try
	{
		if(g_bCoutFile)
		{
			FILE* pFile = fopen("./all.log","wt");
			if(pFile)
			{
				theApp.m_pWorldbaseManagerConfig->cout(pFile);
				theApp.coutAll(pFile);
				fclose(pFile);
			}
			pFile = NULL;
		}
		else
		{
			theApp.m_pWorldbaseManagerConfig->cout(stdout);
			theApp.coutAll(stdout);
			fprintf(stdout,"\n");
		}
	}
	catch(...)
	{
		g_pLog->UGLog("coutAll error!");
	}
	return 0;
}

