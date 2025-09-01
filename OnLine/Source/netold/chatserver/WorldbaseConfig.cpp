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
	m_ulMaxTeam = 0;
	m_ulMaxTradeRoom = 0;
	m_ulMaxUnion = 0;
	Q_memset(m_szMailDBHost,0,261);
	Q_memset(m_szMailDBName,0,261);
	Q_memset(m_szMailDBUser,0,51);
	Q_memset(m_szMailDBPassword,0,51);
	m_ulSceneNum = 0;
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
		g_pLog->UGLog("get WorldbaseIP error.");
		return -1;
	}
	m_dwWorldbaseIP = inet_addr(pchData);
	pIni->getValue(pchApp,"IPForPlayer",pchData);
	if(!pchData)
	{
		g_pLog->UGLog("get IPForPlayer error.");
		return -1;
	}
	m_dwIPForPlayer = inet_addr(pchData);
	pIni->getValue(pchApp,"PortForPlayer",m_dwPortForPlayer);
	if(!m_dwPortForPlayer)
	{
		g_pLog->UGLog("get PortForPlayer error.");
		return -1;
	}
	
	pIni->getValue(pchApp,"CheckID",m_dwWorldbaseCheckID);
	if(!m_dwWorldbaseCheckID)
	{
		g_pLog->UGLog("get CheckID error.");
		return -1;
	}
	pIni->getValue(pchApp,"MaxPlayer",m_ulMaxPlayer);
	if(!m_ulMaxPlayer)
	{
		g_pLog->UGLog("get MaxPlayer error.");
		return -1;
	}
	pIni->getValue(pchApp,"MaxTeam",m_ulMaxTeam);
	if(!m_ulMaxTeam)
	{
		g_pLog->UGLog("get MaxTeam error.");
		return -1;
	}
	pIni->getValue(pchApp,"MaxTradeRoom",m_ulMaxTradeRoom);
	if(!m_ulMaxTradeRoom)
	{
		g_pLog->UGLog("get MaxTradeRoom error.");
		return -1;
	}
	pIni->getValue(pchApp,"MaxUnion",m_ulMaxUnion);
	if(!m_ulMaxUnion)
	{
		g_pLog->UGLog("get MaxUnion error.");
		return -1;
	}
	
	pIni->getValue(pchApp,"SQLMailHost",pchData);
	if(!pchData)
	{
		g_pLog->UGLog("get SQL mail Host error.");
		return -1;
	}
	Q_strncpy(m_szMailDBHost,pchData,261);
	pIni->getValue(pchApp,"SQLMailDBName",pchData);
	if(!pchData)
	{
		g_pLog->UGLog("get SQL mail DBName error.");
		return -1;
	}
	Q_strncpy(m_szMailDBName,pchData,261);
	pIni->getValue(pchApp,"SQLMailUser",pchData);
	if(!pchData)
	{
		g_pLog->UGLog("get SQL mail User error.");
		return -1;
	}
	Q_strncpy(m_szMailDBUser,pchData,51);
	pIni->getValue(pchApp,"SQLMailPassword",pchData);
	if(!pchData)
	{
		g_pLog->UGLog("get SQL mail Password error.");
		return -1;
	}
	Q_strncpy(m_szMailDBPassword,pchData,51);
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
	fprintf(p,"player max = %d, team max = %d.\n",m_ulMaxPlayer,m_ulMaxTeam);
	fprintf(p,"union max = %d, traderoom max = %d.\n",m_ulMaxUnion,m_ulMaxTradeRoom);
	return 0;
}
