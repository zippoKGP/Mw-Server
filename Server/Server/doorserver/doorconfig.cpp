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
	m_nTimes = 15;
	m_nMoney = 4;
	m_nGMNameCount = 0;
	m_ppchGMName = NULL;
	
	m_dwIPAginLogin = 0;
	m_wPortAginLogin = 3601;
	
	m_dwIPAginLogout = 0;
	m_wPortAginLogout = 3602;
	
	m_dwIPAginBalance = 0;
	m_wPortAginBalance = 3603;
	
	m_dwIPAginPayment = 0;
	m_wPortAginPayment = 3604;

	m_dwIPAginState = 0;
	m_wPortAginState = 3605;
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
		return -1;
	}
	ini.getValue("server","HostIPForWorldbaseConnect",pchData);
	if(!pchData)
	{
		printf("getValue HostIPForWorldbaseConnect in ini file failed.\n");
		g_pLog->UGLog("ini HostIPForWorldbaseConnect failed.");
		return -1;
	}
	m_dwHostIPForWorldbaseConnect = inet_addr(pchData);
	ini.getValue("server","HostIPForManagerConnect",pchData);
	if(!pchData)
	{
		printf("getValue HostIPForManagerConnect in ini file failed.\n");
		g_pLog->UGLog("ini HostIPForManagerConnect failed.");
		return -1;
	}
	m_dwHostIPForManagerConnect = inet_addr(pchData);
	ini.getValue("server","PortForWorldbaseConnect",m_wPortForWorldbaseConnect);
	if(!m_wPortForWorldbaseConnect)
	{
		printf("getValue PortForWorldbaseConnect in ini file failed.\n");
		g_pLog->UGLog("ini PortForWorldbaseConnect failed.");
		return -1;
	}
	ini.getValue("server","PortForManagerConnect",m_wPortForManagerConnect);
	if(!m_wPortForManagerConnect)
	{
		printf("getValue PortForManagerConnect in ini file failed.\n");
		g_pLog->UGLog("ini PortForManagerConnect failed.");
		return -1;
	}


	ini.getValue("SQLBilling","host",pchData);
	if(!pchData)
	{
		printf("getValue host from SQLBilling in ini file failed.\n");
		g_pLog->UGLog("ini SQLBilling host failed.");
		return -1;
	}
	Q_strncpy(m_szSQLBillingHost,pchData,MAX_STRING_BUFFER);
	ini.getValue("SQLBilling","user",pchData);
	if(!pchData)
	{
		printf("getValue user from SQLBilling in ini file failed.\n");
		g_pLog->UGLog("ini SQLBilling user failed.");
		return -1;
	}
	Q_strncpy(m_szSQLBillingUser,pchData,MAX_STRING_BUFFER);
	ini.getValue("SQLBilling","password",pchData);
	if(!pchData)
	{
		printf("getValue password from SQLBilling in ini file failed.\n");
		g_pLog->UGLog("ini SQLBilling password failed.");
		return -1;
	}
	Q_strncpy(m_szSQLBillingPassword,pchData,MAX_STRING_BUFFER);
	ini.getValue("SQLBilling","dbname",pchData);
	if(!pchData)
	{
		printf("getValue dbname from SQLBilling in ini file failed.\n");
		g_pLog->UGLog("ini SQLBilling dbname failed.");
		return -1;
	}
	Q_strncpy(m_szSQLBillingDBName,pchData,MAX_STRING_BUFFER);
	
	ini.getValue("SQLDoorServer","host",pchData);
	if(!pchData)
	{
		printf("getValue host from SQLDoorServer in ini file failed.\n");
		g_pLog->UGLog("ini SQLDoorServer host failed.");
		return -1;
	}
	Q_strncpy(m_szSQLDoorServerHost,pchData,MAX_STRING_BUFFER);
	ini.getValue("SQLDoorServer","user",pchData);
	if(!pchData)
	{
		printf("getValue user from SQLDoorServer in ini file failed.\n");
		g_pLog->UGLog("ini SQLDoorServer user failed.");
		return -1;
	}
	Q_strncpy(m_szSQLDoorServerUser,pchData,MAX_STRING_BUFFER);
	ini.getValue("SQLDoorServer","password",pchData);
	if(!pchData)
	{
		printf("getValue password from SQLDoorServer in ini file failed.\n");
		g_pLog->UGLog("ini SQLDoorServer password failed.");
		return -1;
	}
	Q_strncpy(m_szSQLDoorServerPassword,pchData,MAX_STRING_BUFFER);
	ini.getValue("SQLDoorServer","dbname",pchData);
	if(!pchData)
	{
		printf("getValue dbname from SQLDoorServer in ini file failed.\n");
		g_pLog->UGLog("ini SQLDoorServer dbname failed.");
		return -1;
	}
	Q_strncpy(m_szSQLDoorServerDBName,pchData,MAX_STRING_BUFFER);
	
	ini.getValue("server","Times",m_nTimes);
	if(m_nTimes < 1)
	{
		printf("getValue Times from server in ini file failed.\n");
		g_pLog->UGLog("ini SQLDoorServer Times failed.");
		return -1;
	}
	ini.getValue("server","Moneys",m_nMoney);
	if(m_nMoney < 1)
	{
		printf("getValue Moneys from server in ini file failed.\n");
		g_pLog->UGLog("ini SQLDoorServer Money failed.");
		return -1;
	}
	
	ini.getValue("server","GMName",pchData);
	if(!pchData)
	{
		printf("getValue GMName from server in ini file failed.\n");
		g_pLog->UGLog("ini SQLDoorServer GMName failed.");
		return -1;
	}
	
	initGMID(pchData);
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
	m_dwIPAginLogin = 0;
	m_wPortAginLogin = 0;
	m_dwIPAginLogout = 0;
	m_wPortAginLogout = 0;
	m_dwIPAginBalance = 0;
	m_wPortAginBalance = 0;
	m_dwIPAginPayment = 0;
	m_wPortAginPayment = 0;
	m_dwIPAginState = 0;
	m_wPortAginState = 0;
	return 0;
}

UG_LONG CDoorConfig::cout(FILE* p)
{
	fprintf(p,"worldbase ip = %s, port = %d.\n",convertip(m_dwHostIPForWorldbaseConnect),m_wPortForWorldbaseConnect);
	fprintf(p,"GM ip = %s, port = %d.\n",convertip(m_dwHostIPForManagerConnect),m_wPortForManagerConnect);
	fprintf(p,"sql billing host = %s, db = %s.\n",m_szSQLBillingHost,m_szSQLBillingDBName);
	fprintf(p,"sql billing user = %s, password = %s.\n",m_szSQLBillingUser,m_szSQLBillingPassword);
	fprintf(p,"sql doorserver host = %s, db = %s\n",m_szSQLDoorServerHost,m_szSQLDoorServerDBName);
//	fprintf(p,"sql doorserver user = %s, password = %s.\n",m_szSQLDoorServerUser,m_szSQLDoorServerPassword);
	fprintf(p,"sql doorserver money = %d, times = %d.\n",m_nMoney,m_nTimes);
	fprintf(p,"\n");
	return 0;
}

UGLONG CDoorConfig::initGMID(UG_PCHAR pchGMID)
{
	m_nGMNameCount = 0;
	UGPCHAR pchTemp = pchGMID;
	UGPCHAR pchFind = NULL;
	for(;;)
	{
		pchFind = strstr(pchTemp,"@");
		if(pchFind)
		{
			UGINT nFind = pchFind - pchTemp;
			if(nFind > 0)
			{
				m_nGMNameCount ++;
			}
			pchTemp = pchFind + 1;
		}
		else
		{
			m_nGMNameCount ++;
			break;
		}
	}
	if(m_nGMNameCount)
	{
		pchTemp = pchGMID;
		pchFind = NULL;
		m_ppchGMName = new UGPCHAR[m_nGMNameCount];
		int i = 0;
		for(;;)
		{
			pchFind = strstr(pchTemp,"@");
			if(pchFind)
			{
				UGINT nFind = pchFind - pchTemp;
				if(nFind > 0)
				{
					*(m_ppchGMName + i) = new UGCHAR[nFind + 1];
					strncpy(*(m_ppchGMName + i),pchTemp,nFind);
					*(*(m_ppchGMName + i) + nFind) = '\0';
					i ++;
				}
				pchTemp = pchFind + 1;
			}
			else
			{
				*(m_ppchGMName + i) = new UGCHAR[strlen(pchTemp) + 1];
				strcpy(*(m_ppchGMName + i),pchTemp);
				i ++;
				break;
			}
		}
	}
	return 0;
}
