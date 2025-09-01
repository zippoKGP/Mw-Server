// WorldbaseManagerConfig.cpp: implementation of the CWorldbaseManagerConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "incall.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWorldbaseManagerConfig::CWorldbaseManagerConfig()
{
	m_dwIPForWorldbase = 0;
	m_dwPortForWorldbase = 0;
	m_lWorldbaseCount = 0;
	m_pWorldbaseConfig = NULL;
}

CWorldbaseManagerConfig::~CWorldbaseManagerConfig()
{
	cleanup();
}

UG_LONG CWorldbaseManagerConfig::init(UG_PCHAR pchPathName)
{
	CUGIni ini;
	UG_PCHAR pchData = NULL;
	ReleasePA<CWorldbaseConfig*>(m_pWorldbaseConfig);
	UG_ULONG ulRet = ini.init(pchPathName);
	if(ulRet)
	{
		printf("init ini file error. %s\n",pchPathName);
		return -1;
	}
	ulRet = ini.getValue("WorldbaseManager","IPForWorldbase",pchData);
	if(!pchData)
	{
		printf("init ini getValue IPForWorldbase.\n");
		return -1;
	}
	m_dwIPForWorldbase = inet_addr(pchData);
	ulRet = ini.getValue("WorldbaseManager","PortForWorldbase",m_dwPortForWorldbase);
	if(ulRet)
	{
		printf("init ini getValue PortForWorldbase.\n");
		return -1;
	}
	m_lWorldbaseCount = ini.getCount("WorldbaseManager") - 2;
	if(m_lWorldbaseCount < 1)
	{
		printf("init ini getCount error %d.\n",m_lWorldbaseCount);
		return -1;
	}
	UG_PCHAR pchKey = NULL;
	UG_PCHAR pchValue = NULL;
	m_pWorldbaseConfig = new CWorldbaseConfig[m_lWorldbaseCount];
	UG_INT32 i = 0;
	for(UG_LONG l = 0; l < m_lWorldbaseCount + 2; l ++)
	{
		if(ini.getKeyValue(l,"WorldbaseManager",pchKey,pchValue))
		{
			continue;
		}
		if((!strcmp(pchKey,"ipforworldbase")) || (!strcmp(pchKey,"portforworldbase")))
		{
			continue;
		}
		if((m_pWorldbaseConfig + i)->init(&ini,pchValue))
		{
			printf("init chat %d failed.\n",i);
			ReleasePA<CWorldbaseConfig*>(m_pWorldbaseConfig);
			return -1;
		}
		i ++;
	}
	return 0;
}

UG_LONG CWorldbaseManagerConfig::cleanup()
{
	ReleasePA<CWorldbaseConfig*>(m_pWorldbaseConfig);
	return 0;
}

UG_LONG CWorldbaseManagerConfig::cout(FILE* p)
{
	fprintf(p,"the chat count is %d.\n",m_lWorldbaseCount);
	for(UG_LONG l = 0; l < m_lWorldbaseCount; l ++)
	{
		(m_pWorldbaseConfig + l)->cout(p);
		fprintf(p,"\n");
	}
	return 0;
}

