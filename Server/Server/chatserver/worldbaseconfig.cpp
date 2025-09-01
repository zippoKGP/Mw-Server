// WorldbaseConfig.cpp: implementation of the CWorldbaseConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "incall.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWorldbaseConfig::CWorldbaseConfig()
{
	m_dwWorldbaseIP = 0;
	m_dwWorldbaseCheckID = 0;
	m_dwIPForPlayer = 0;
	m_dwPortForPlayer = 0;
	m_ulMaxPlayer = 0;
	Q_memset(m_szMailDBHost,0,261);
	Q_memset(m_szMailDBName,0,261);
	Q_memset(m_szMailDBUser,0,51);
	Q_memset(m_szMailDBPassword,0,51);
	m_ulSceneNum = 0;
	m_nFilterTrade = 0;
	m_ppchFilterTrade = NULL;
	m_nGMLogCount = 0;
	m_pGMLog = NULL;
	m_dwIPForGM = 0;
	m_dwPortForGM = 0;
}

CWorldbaseConfig::~CWorldbaseConfig()
{
	cleanup();
}

UG_LONG CWorldbaseConfig::init(CUGIni* pIni,UG_PCHAR pchApp)
{
	UG_PCHAR pchData = NULL;
	UG_ULONG ulRet = 0;
	pIni->getValue(pchApp,"WorldbaseIP",pchData);
	if(!pchData)
	{
		printf("get WorldbaseIP error.\n");
		g_pLog->UGLog("get WorldbaseIP error.");
		return -1;
	}
	m_dwWorldbaseIP = inet_addr(pchData);
	pIni->getValue(pchApp,"IPForPlayer",pchData);
	if(!pchData)
	{
		printf("get IPForPlayer error.\n");
		g_pLog->UGLog("get IPForPlayer error.");
		return -1;
	}
	m_dwIPForPlayer = inet_addr(pchData);
	pIni->getValue(pchApp,"PortForPlayer",m_dwPortForPlayer);
	if(!m_dwPortForPlayer)
	{
		printf("get PortForPlayer error.\n");
		g_pLog->UGLog("get PortForPlayer error.");
		return -1;
	}

	pIni->getValue(pchApp,"IPForGM",pchData);
	if(!pchData)
	{
		printf("get IPForGM error.\n");
		g_pLog->UGLog("get IPForGM error.");
		return -1;
	}
	m_dwIPForGM = inet_addr(pchData);
	pIni->getValue(pchApp,"PortForGM",m_dwPortForGM);
	if(!m_dwPortForPlayer)
	{
		printf("get PortForGM error.\n");
		g_pLog->UGLog("get PortForGM error.");
		return -1;
	}
	
	pIni->getValue(pchApp,"CheckID",m_dwWorldbaseCheckID);
	if(!m_dwWorldbaseCheckID)
	{
		printf("get CheckID error.\n");
		g_pLog->UGLog("get CheckID error.");
		return -1;
	}
	pIni->getValue(pchApp,"MaxPlayer",m_ulMaxPlayer);
	if(!m_ulMaxPlayer)
	{
		printf("get MaxPlayer error.\n");
		g_pLog->UGLog("get MaxPlayer error.");
		return -1;
	}
	
	pIni->getValue(pchApp,"SQLMailHost",pchData);
	if(!pchData)
	{
		printf("get SQL mail Host error.\n");
		g_pLog->UGLog("get SQL mail Host error.");
		return -1;
	}
	Q_strncpy(m_szMailDBHost,pchData,261);
	pIni->getValue(pchApp,"SQLMailDBName",pchData);
	if(!pchData)
	{
		printf("get SQL mail DBName error.\n");
		g_pLog->UGLog("get SQL mail DBName error.");
		return -1;
	}
	Q_strncpy(m_szMailDBName,pchData,261);
	pIni->getValue(pchApp,"SQLMailUser",pchData);
	if(!pchData)
	{
		printf("get SQL mail User error.\n");
		g_pLog->UGLog("get SQL mail User error.");
		return -1;
	}
	Q_strncpy(m_szMailDBUser,pchData,51);
	pIni->getValue(pchApp,"SQLMailPassword",pchData);
	if(!pchData)
	{
		printf("get SQL mail Password error.\n");
		g_pLog->UGLog("get SQL mail Password error.");
		return -1;
	}
	Q_strncpy(m_szMailDBPassword,pchData,51);
	
	pIni->getValue(pchApp,"TradeFilter",pchData);
	if(!pchData)
	{
		printf("get Trade Filter error.\n");
		g_pLog->UGLog("get Trade Filter error.");
		return -1;
	}
	initFilterTrade(pchData);
	
	pIni->getValue(pchApp,"GMIDS",pchData);
	if(pchData)
	{
		getGMLog(pchData);
	}
	return 0;
}

UG_LONG CWorldbaseConfig::cleanup()
{
	return 0;
}

UG_LONG CWorldbaseConfig::cout(FILE* p)
{
	fprintf(p,"server for player connect ip = %s, port = %d.\n",convertip(m_dwIPForPlayer),m_dwPortForPlayer);
	fprintf(p,"only the worldbase ip = %s can connect.\n",convertip(m_dwWorldbaseIP));
	fprintf(p,"mail sql host = %s.\n",m_szMailDBHost);
	fprintf(p,"mail sql db = %s.\n",m_szMailDBName);
	fprintf(p,"mail sql user = %s.\n",m_szMailDBUser);
	fprintf(p,"mail sql password = %s.\n",m_szMailDBPassword);
	return 0;
}

UG_LONG CWorldbaseConfig::initFilterTrade(UG_PCHAR pchFilter)
{
	m_nFilterTrade = 0;
	UGPCHAR pchTemp = pchFilter;
	UGPCHAR pchFind = NULL;
	for(;;)
	{
		pchFind = strstr(pchTemp,"@");
		if(pchFind)
		{
			UGINT nFind = pchFind - pchTemp;
			if(nFind > 0)
			{
				m_nFilterTrade ++;
			}
			pchTemp = pchFind + 1;
		}
		else
		{
			if(pchTemp)
			{
				m_nFilterTrade ++;
			}
			break;
		}
	}
	if(m_nFilterTrade)
	{
		pchTemp = pchFilter;
		pchFind = NULL;
		m_ppchFilterTrade = new UGPCHAR[m_nFilterTrade];
		int i = 0;
		for(;;)
		{
			pchFind = strstr(pchTemp,"@");
			if(pchFind)
			{
				UGINT nFind = pchFind - pchTemp;
				if(nFind > 0)
				{
					*(m_ppchFilterTrade + i) = new UGCHAR[nFind + 1];
					strncpy(*(m_ppchFilterTrade + i),pchTemp,nFind);
					*(*(m_ppchFilterTrade + i) + nFind) = '\0';
					i ++;
				}
				pchTemp = pchFind + 1;
			}
			else
			{
				if(pchTemp)
				{
					*(m_ppchFilterTrade + i) = new UGCHAR[strlen(pchTemp) + 1];
					strcpy(*(m_ppchFilterTrade + i),pchTemp);
				}
				break;
			}
		}
	}
	int nSpaces = 0;
	for(int i = 0; i < m_nFilterTrade; i ++)
	{
		if('\0' == *(*(m_ppchFilterTrade + i)))
		{
			nSpaces ++;
		}
	}
	UGCHAR** ppchTemp = m_ppchFilterTrade;
	int nTemp = m_nFilterTrade;
	m_nFilterTrade = m_nFilterTrade - nSpaces;
	
	nSpaces = 0;
	if(m_nFilterTrade > 0)
	{
		m_ppchFilterTrade = new UGPCHAR[m_nFilterTrade];
		for(int i = 0; i < nTemp; i ++)
		{
			if('\0' != *(*(ppchTemp + i)))
			{
				*(m_ppchFilterTrade + nSpaces) = *(ppchTemp + i);
				nSpaces ++;
			}
			else
			{
				ReleasePA(*(ppchTemp + i));
			}
		}
	}
	ReleasePA(ppchTemp);
	return 0;
}

UGLONG CWorldbaseConfig::getGMLog(UGPCHAR pchGMIDS)
{
	m_nGMLogCount = 0;
	UGPCHAR pchTemp = pchGMIDS;
	UGPCHAR pchFind = NULL;
	for(;;)
	{
		pchFind = strstr(pchTemp,"@");
		if(pchFind)
		{
			UGINT nFind = pchFind - pchTemp;
			if(nFind > 0)
			{
				m_nGMLogCount ++;
			}
			pchTemp = pchFind + 1;
		}
		else
		{
			if(pchTemp)
			{
				m_nGMLogCount ++;
			}
			break;
		}
	}
	if(m_nGMLogCount)
	{
		pchTemp = pchGMIDS;
		pchFind = NULL;
		m_pGMLog = new UGINT[m_nGMLogCount];
		int i = 0;
		for(;;)
		{
			pchFind = strstr(pchTemp,"@");
			if(pchFind)
			{
				UGINT nFind = pchFind - pchTemp;
				if(nFind > 0)
				{
					*pchFind = '\0';
					*(m_pGMLog + i) = atoi(pchTemp);
					i ++;
				}
				pchTemp = pchFind + 1;
			}
			else
			{
				if(pchTemp)
				{
					*(m_pGMLog + i) = atoi(pchTemp);
				}
				break;
			}
		}
	}
	return 0;
}
