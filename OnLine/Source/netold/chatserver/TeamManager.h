// TeamManager.h: interface for the CTeamManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEAMMANAGER_H__B0BCACEE_ACA6_4FC8_B196_A9C602786B64__INCLUDED_)
#define AFX_TEAMMANAGER_H__B0BCACEE_ACA6_4FC8_B196_A9C602786B64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTeamManager  
{
public:
	CTeamManager();
	virtual ~CTeamManager();

public:
	UG_LONG			init(UG_ULONG ulCount);
	UG_LONG			cleanup();

public:
	UG_PVOID		addTeam(INT32 n32Teamid);
	UG_PVOID		delTeam(INT32 n32Teamid);
	UG_PVOID		addPlayer(INT32 n32Playerid,INT32 n32Teamid,UG_PVOID pvPlayer);
	UG_PVOID		delPlayer(INT32 n32Playerid,INT32 n32Teamid);
	
public:
	CTeam*						m_pTeam;
	list<CTeam*>				m_listTeamIdle;
	map<UG_INT32,CTeam*>		m_mapTeam;
	UG_ULONG					m_ulMaxTeam;
	
};

#endif // !defined(AFX_TEAMMANAGER_H__B0BCACEE_ACA6_4FC8_B196_A9C602786B64__INCLUDED_)
