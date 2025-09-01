// TeamManager.cpp: implementation of the CTeamManager class.
//
//////////////////////////////////////////////////////////////////////

#include "incall.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTeamManager::CTeamManager()
{
	m_pTeam = NULL;
	m_listTeamIdle.clear();
	m_mapTeam.clear();
	m_ulMaxTeam = 0;
}

CTeamManager::~CTeamManager()
{
	UGDel<CTeam*>(m_pTeam);
	m_listTeamIdle.clear();
	m_mapTeam.clear();
}

UG_LONG CTeamManager::init(UG_ULONG ulCount)
{
	m_ulMaxTeam = ulCount;
	m_listTeamIdle.clear();
	m_mapTeam.clear();
	m_pTeam = UGNew<CTeam>(ulCount,__FILE__,__LINE__);
	for(UG_ULONG i = 0; i < ulCount; i ++)
	{
		m_listTeamIdle.push_back(m_pTeam + i);
	}
	return 0;
}

UG_LONG CTeamManager::cleanup()
{
	m_listTeamIdle.clear();
	m_mapTeam.clear();
	for(UG_ULONG i = 0; i < m_ulMaxTeam; i ++)
	{
		m_listTeamIdle.push_back(m_pTeam + i);
	}
	return 0;
}

UG_PVOID CTeamManager::addTeam(INT32 n32Teamid)
{
	UG_PVOID pvTeam = NULL;
	list<CTeam*>::iterator it = m_listTeamIdle.begin();
	if(it == m_listTeamIdle.end())
	{
		return NULL;
	}
	else
	{
		pvTeam = *it;
		m_mapTeam[n32Teamid] = *it;
		m_listTeamIdle.erase(it);
	}
	return pvTeam;
}

UG_PVOID CTeamManager::delTeam(INT32 n32Teamid)
{
	UG_PVOID pvTeam = NULL;
	map<UG_INT32,CTeam*>::iterator it = m_mapTeam.find(n32Teamid);
	if(it == m_mapTeam.end())
	{
		return pvTeam;
	}
	else
	{
		pvTeam = it->second;
		it->second->clearupPlayer();
		m_listTeamIdle.push_back(it->second);
		m_mapTeam.erase(it);
	}
	return pvTeam;
}

UG_PVOID CTeamManager::addPlayer(INT32 n32Playerid,INT32 n32Teamid,UG_PVOID pvPlayer)
{
	map<UG_INT32,CTeam*>::iterator it = m_mapTeam.find(n32Teamid);
	if(it == m_mapTeam.end())
	{
		return NULL;
	}
	else
	{
		return (it->second)->addPlayer(n32Playerid,pvPlayer);
	}
}

UG_PVOID CTeamManager::delPlayer(INT32 n32Playerid,INT32 n32Teamid)
{
	map<UG_INT32,CTeam*>::iterator it = m_mapTeam.find(n32Teamid);
	if(it == m_mapTeam.end())
	{
		return NULL;
	}
	else
	{
		return (it->second)->delPlayer(n32Playerid);
	}
}

