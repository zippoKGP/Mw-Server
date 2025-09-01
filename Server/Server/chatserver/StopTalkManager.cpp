// StopTalkManager.cpp: implementation of the CStopTalkManager class.
//
//////////////////////////////////////////////////////////////////////

#include "incall.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStopTalkManager::CStopTalkManager()
{
	m_map.clear();
	m_list.clear();
	m_dwLastRenderTimer = 0;
}

CStopTalkManager::~CStopTalkManager()
{
	cleanup();
}

UGINT CStopTalkManager::init()
{
	m_pStopTalk = new CStopTalk[MAX_MUTE];
	for(int i = 0; i < MAX_MUTE; i ++)
	{
		m_list.push_back(m_pStopTalk + i);
	}
	return 0;
}

UGINT CStopTalkManager::render(UGDWORD dwTimer)
{
	if(m_dwLastRenderTimer + 1000 < dwTimer)
	{
		UGLONG lTimer = 0;
		time(&lTimer);
		map<UGINT,CStopTalk*>::iterator it;
		for(it = m_map.begin(); it != m_map.end();)
		{
			CStopTalk* p = it->second;
			if(UGLONG((p->m_dwBeginer + p->m_dwTimes) < lTimer) && (p->m_bRemove))
			{
				deleteDB(p->m_nPlayerID);
				m_list.push_back(p);
				it = m_map.erase(it);
			}
			else
			{
				it ++;
			}
		}
		m_dwLastRenderTimer = dwTimer;
	}
	return 0;
}

UGINT CStopTalkManager::cleanup()
{
	m_map.clear();
	m_list.clear();
	ReleasePA(m_pStopTalk);
	return 0;
}

UGINT CStopTalkManager::addPlayer(UGINT nPlayerID,UGDWORD dwBeginer,UGDWORD dwTimes)
{
	map<UGINT,CStopTalk*>::iterator it = m_map.find(nPlayerID);
	if(it != m_map.end())
	{
		it->second->m_dwBeginer = dwBeginer;
		it->second->m_dwTimes = dwTimes;
		it->second->m_bRemove = FALSE;
		updateDB(nPlayerID,dwBeginer,dwTimes,0);
		return 0;
	}
	else
	{
		if(dwTimes)
		{
			list<CStopTalk*>::iterator it = m_list.begin();
			if(it != m_list.end())
			{
				CStopTalk* p = *it;
				p->m_dwBeginer = dwBeginer;
				p->m_nPlayerID = nPlayerID;
				p->m_dwTimes = dwTimes;
				p->m_bRemove = FALSE;
				m_map[p->m_nPlayerID] = p;
				m_list.erase(it);
				insertDB(nPlayerID,dwBeginer,dwTimes);
				return 0;
			}
			else
			{
				g_pLog->UGLog("CStopTalkManager addPlayer(%d,%d,%d) error line = %d.",nPlayerID,dwBeginer,dwTimes,__LINE__);
				return __LINE__;
			}
		}
	}
	return 1;
}

UGINT CStopTalkManager::findPlayer(UGINT nPlayerID,UGDWORD dwTimer)
{
	map<UGINT,CStopTalk*>::iterator it = m_map.find(nPlayerID);
	if(it != m_map.end())
	{
		UGLONG lTimer = 0;
		time(&lTimer);
		CStopTalk* p = it->second;
		if(UGLONG(p->m_dwBeginer + p->m_dwTimes) < lTimer) //Î´½»Ç®
		{
			return 1;
		}
		else //Î´½â³ý½ûÑÔ
		{
			return -1;
		}
	}
	return 0;
}

UGINT CStopTalkManager::setPlayer(UGINT nPlayerID,BOOL bRemove)
{
	g_pLog->UGLog("CStopTalkManager = %d, %d.",nPlayerID,bRemove);
	map<UGINT,CStopTalk*>::iterator it = m_map.find(nPlayerID);
	if(it != m_map.end())
	{
		CStopTalk* p = it->second;
		if(p)
		{
			p->m_bRemove = bRemove;
			updateDB(nPlayerID,bRemove);
		}
		return 0;
	}
	return __LINE__;
}

UGINT CStopTalkManager::insertDB(UGINT nPlayerID,UGDWORD dwBeginer,UGDWORD dwTimes)
{
	char szQuery[1024];
	sprintf(szQuery,"insert into chatmute (playerid,beginer,times,removed) values (%d,%d,%d,0)",nPlayerID,dwBeginer,dwTimes);
	return queryDB(szQuery);
}

UGINT CStopTalkManager::updateDB(UGINT nPlayerID,UGDWORD dwBeginer,UGDWORD dwTimes,BOOL bRemove)
{
	char szQuery[1024];
	sprintf(szQuery,"update chatmute set beginer = %d, times = %d, removed = %d where playerid = %d",dwBeginer,dwTimes,bRemove,nPlayerID);
	return queryDB(szQuery);
}

UGINT CStopTalkManager::updateDB(UGINT nPlayerID,UGDWORD dwBeginer,UGDWORD dwTimes)
{
	char szQuery[1024];
	sprintf(szQuery,"update chatmute set beginer = %d, times = %d where playerid = %d",dwBeginer,dwTimes,nPlayerID);
	return queryDB(szQuery);
}

UGINT CStopTalkManager::updateDB(UGINT nPlayerID,BOOL bRemove)
{
	char szQuery[1024];
	sprintf(szQuery,"update chatmute set removed = %d where playerid = %d",bRemove,nPlayerID);
	return queryDB(szQuery);
}

UGINT CStopTalkManager::deleteDB(UGINT nPlayerID)
{
	char szQuery[1024];
	sprintf(szQuery,"delete from chatmute where playerid = %d",nPlayerID);
	return queryDB(szQuery);
}

UGINT CStopTalkManager::queryDB(UGPCHAR pchQuery)
{
	if(!pchQuery)
	{
		g_pLog->UGLog("queryDB error line = %d.",__LINE__);
		return __LINE__;
	}
	CUGSQL* pSQL = (theApp.m_pWorldbase)->m_pSQLChatMail;
	if(!pSQL)
	{
		g_pLog->UGLog("queryDB error line = %d.",__LINE__);
		return __LINE__;
	}
	LPVOID pvRet = NULL;
	DWORD dwRet = pSQL->query(pchQuery,pvRet);
	pSQL->freeResult(pvRet);
	if(dwRet)
	{
		g_pLog->UGLog("query(%s) error.",pchQuery);
	}
	return UGINT(dwRet);
}

UGINT CStopTalkManager::loadDB()
{
	CUGSQL* pSQL = (theApp.m_pWorldbase)->m_pSQLChatMail;
	if(!pSQL)
	{
		g_pLog->UGLog("loadDB error line = %d.",__LINE__);
		return __LINE__;
	}
	LPVOID pvRet = NULL;
	DWORD dwRows = 0;
	DWORD dwRet = pSQL->query("select playerid, beginer, times, removed from chatmute",pvRet,dwRows);
	if(dwRet)
	{
		pSQL->freeResult(pvRet);
		g_pLog->UGLog("loadDB error line = %d.",__LINE__);
		return __LINE__;
	}
	
	DWORD dwCount = min(MAX_MUTE,dwRows);
	g_pLog->UGLog("loadDB count = %d.",dwCount);
	char* pchData = NULL;
	for(DWORD i = 0; i < dwCount; i ++)
	{
		pSQL->getQueryResult(pvRet,i,0,pchData);
		if(!pchData)
		{
			pSQL->freeResult(pvRet);
			return __LINE__;
		}
		int nPlayerid = atoi(pchData);

		pSQL->getQueryResult(pvRet,i,1,pchData);
		if(!pchData)
		{
			pSQL->freeResult(pvRet);
			return __LINE__;
		}
		int nBeginer = atoi(pchData);
	
		pSQL->getQueryResult(pvRet,i,2,pchData);
		if(!pchData)
		{
			pSQL->freeResult(pvRet);
			return __LINE__;
		}
		int nTimes = atoi(pchData);
		
		pSQL->getQueryResult(pvRet,i,3,pchData);
		if(!pchData)
		{
			pSQL->freeResult(pvRet);
			return __LINE__;
		}
		int nRemoved = atoi(pchData);
		UGINT nRet = addPlayer(nPlayerid,nBeginer,nTimes,nRemoved);
		if(nRet)
		{
			pSQL->freeResult(pvRet);
			return nRet;
		}
	}
	pSQL->freeResult(pvRet);
	return 0;
}

UGINT CStopTalkManager::addPlayer(UGINT nPlayerID,UGINT nBeginer,UGINT nTimes,UGINT nRemoved)
{
	list<CStopTalk*>::iterator it = m_list.begin();
	if(it != m_list.end())
	{
		CStopTalk* p = *it;
		p->m_dwBeginer = DWORD(nBeginer);
		p->m_nPlayerID = nPlayerID;
		p->m_dwTimes = (DWORD)nTimes;
		p->m_bRemove = nRemoved;
		m_map[p->m_nPlayerID] = p;
		m_list.erase(it);
		return 0;
	}
	else
	{
		g_pLog->UGLog("CStopTalkManager addPlayer(%d,%d,%d,%d) error line = %d.",nPlayerID,nBeginer,nTimes,nRemoved,__LINE__);
		return __LINE__;
	}
	return 0;
}
