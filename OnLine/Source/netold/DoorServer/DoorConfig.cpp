/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : DoorConfig.cpp
*owner        : Ben
*description  : 处理doorserver的配置文件，并将信息保存下来。
*modified     : 2004/12/20
******************************************************************************/ 

#include "incall.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDoorConfig::CDoorConfig()
{
	m_dwHostIPForWorldbaseConnect = 0;
	m_wPortForWorldbaseConnect = 0;
	m_dwHostIPForManagerConnect = 0;
	m_wPortForManagerConnect = 0;
	Q_memset(m_szSQLBillingHost,0,MAX_STRING_BUFFER);
	Q_memset(m_szSQLBillingDBName,0,MAX_STRING_BUFFER);
	Q_memset(m_szSQLDoorServerDBName,0,MAX_STRING_BUFFER);
	Q_memset(m_szSQLDoorServerHost,0,MAX_STRING_BUFFER);
	Q_memset(m_szSQLDoorServerUser,0,MAX_STRING_BUFFER);
	Q_memset(m_szSQLDoorServerPassword,0,MAX_STRING_BUFFER);
	Q_memset(m_szSQLBillingUser,0,MAX_STRING_BUFFER);
	Q_memset(m_szSQLBillingPassword,0,MAX_STRING_BUFFER);
}

CDoorConfig::~CDoorConfig()
{
	cleanup();
}

UG_LONG CDoorConfig::init(UG_PCHAR pchPathName)
{
	CUGIni ini;
	UG_PCHAR pchData = NULL;
	if(ini.init(pchPathName))
	{
		printf("ini init(%s) failed.\n",pchPathName);
		g_pLog->UGLog("ini init(%s) failed.",pchPathName);
		cleanup();
		return -1;
	}
	ini.getValue("server","HostIPForWorldbaseConnect",pchData);
	if(!pchData)
	{
		printf("ini HostIPForWorldbaseConnect failed.\n");
		g_pLog->UGLog("ini HostIPForWorldbaseConnect failed.");
		cleanup();
		return -1;
	}
	m_dwHostIPForWorldbaseConnect = inet_addr(pchData);
	ini.getValue("server","HostIPForManagerConnect",pchData);
	if(!pchData)
	{
		printf("ini HostIPForManagerConnect failed.\n");
		g_pLog->UGLog("ini HostIPForManagerConnect failed.");
		cleanup();
		return -1;
	}
	m_dwHostIPForManagerConnect = inet_addr(pchData);
	ini.getValue("server","PortForWorldbaseConnect",m_wPortForWorldbaseConnect);
	if(!m_wPortForWorldbaseConnect)
	{
		printf("ini PortForWorldbaseConnect failed.\n");
		g_pLog->UGLog("ini PortForWorldbaseConnect failed.");
		cleanup();
		return -1;
	}
	ini.getValue("server","PortForManagerConnect",m_wPortForManagerConnect);
	if(!m_wPortForManagerConnect)
	{
		printf("ini PortForManagerConnect failed.\n");
		g_pLog->UGLog("ini PortForManagerConnect failed.");
		cleanup();
		return -1;
	}
	
	ini.getValue("SQLBilling","host",pchData);
	if(!pchData)
	{
		printf("ini SQLBilling host failed.\n");
		g_pLog->UGLog("ini SQLBilling host failed.");
		cleanup();
		return -1;
	}
	Q_strncpy(m_szSQLBillingHost,pchData,MAX_STRING_BUFFER);
	ini.getValue("SQLBilling","user",pchData);
	if(!pchData)
	{
		printf("ini SQLBilling user failed.\n");
		g_pLog->UGLog("ini SQLBilling user failed.");
		cleanup();
		return -1;
	}
	Q_strncpy(m_szSQLBillingUser,pchData,MAX_STRING_BUFFER);
	ini.getValue("SQLBilling","password",pchData);
	if(!pchData)
	{
		printf("ini SQLBilling password failed.\n");
		g_pLog->UGLog("ini SQLBilling password failed.");
		cleanup();
		return -1;
	}
	Q_strncpy(m_szSQLBillingPassword,pchData,MAX_STRING_BUFFER);
	ini.getValue("SQLBilling","dbname",pchData);
	if(!pchData)
	{
		printf("ini SQLBilling dbname failed.\n");
		g_pLog->UGLog("ini SQLBilling dbname failed.");
		cleanup();
		return -1;
	}
	Q_strncpy(m_szSQLBillingDBName,pchData,MAX_STRING_BUFFER);
	
	ini.getValue("SQLDoorServer","host",pchData);
	if(!pchData)
	{
		printf("ini SQLDoorServer host failed.\n");
		g_pLog->UGLog("ini SQLDoorServer host failed.");
		cleanup();
		return -1;
	}
	Q_strncpy(m_szSQLDoorServerHost,pchData,MAX_STRING_BUFFER);
	ini.getValue("SQLDoorServer","user",pchData);
	if(!pchData)
	{
		printf("ini SQLDoorServer user failed.\n");
		g_pLog->UGLog("ini SQLDoorServer user failed.");
		cleanup();
		return -1;
	}
	Q_strncpy(m_szSQLDoorServerUser,pchData,MAX_STRING_BUFFER);
	ini.getValue("SQLDoorServer","password",pchData);
	if(!pchData)
	{
		printf("ini SQLDoorServer password failed.\n");
		g_pLog->UGLog("ini SQLDoorServer password failed.");
		cleanup();
		return -1;
	}
	Q_strncpy(m_szSQLDoorServerPassword,pchData,MAX_STRING_BUFFER);
	ini.getValue("SQLDoorServer","dbname",pchData);
	if(!pchData)
	{
		printf("ini SQLDoorServer dbname failed.\n");
		g_pLog->UGLog("ini SQLDoorServer dbname failed.");
		cleanup();
		return -1;
	}
	Q_strncpy(m_szSQLDoorServerDBName,pchData,MAX_STRING_BUFFER);
	cout(stdout);
	return 0;
}

UG_LONG CDoorConfig::cleanup()
{
	m_dwHostIPForWorldbaseConnect = 0;
	m_wPortForWorldbaseConnect = 0;
	m_dwHostIPForManagerConnect = 0;
	m_wPortForManagerConnect = 0;
	Q_memset(m_szSQLBillingHost,0,MAX_STRING_BUFFER);
	Q_memset(m_szSQLBillingDBName,0,MAX_STRING_BUFFER);
	Q_memset(m_szSQLDoorServerDBName,0,MAX_STRING_BUFFER);
	Q_memset(m_szSQLDoorServerHost,0,MAX_STRING_BUFFER);
	Q_memset(m_szSQLDoorServerUser,0,MAX_STRING_BUFFER);
	Q_memset(m_szSQLDoorServerPassword,0,MAX_STRING_BUFFER);
	Q_memset(m_szSQLBillingUser,0,MAX_STRING_BUFFER);
	Q_memset(m_szSQLBillingPassword,0,MAX_STRING_BUFFER);
	return 0;
}

UG_LONG CDoorConfig::cout(FILE* p)
{
	fprintf(p,"worldbase ip = %s, port = %d.\n",convertip(m_dwHostIPForWorldbaseConnect),m_wPortForWorldbaseConnect);
	fprintf(p,"GM ip = %s, port = %d.\n",convertip(m_dwHostIPForManagerConnect),m_wPortForManagerConnect);
	fprintf(p,"sql billing host = %s, db = %s.\n",m_szSQLBillingHost,m_szSQLBillingDBName);
	fprintf(p,"sql billing user = %s, password = %s.\n",m_szSQLBillingUser,m_szSQLBillingPassword);
	fprintf(p,"sql doorserver host = %s, db = %s\n",m_szSQLDoorServerHost,m_szSQLDoorServerDBName);
	fprintf(p,"sql doorserver user = %s, password = %s.\n",m_szSQLDoorServerUser,m_szSQLDoorServerPassword);
	fprintf(p,"\n");
	return 0;
}
