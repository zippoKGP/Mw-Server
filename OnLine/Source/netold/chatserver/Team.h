// Team.h: interface for the CTeam class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEAM_H__E0BDFD19_EF87_46A4_947C_CCA9CCFEDBBA__INCLUDED_)
#define AFX_TEAM_H__E0BDFD19_EF87_46A4_947C_CCA9CCFEDBBA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTeam  
{
public:
	CTeam();
	virtual ~CTeam();

public:
	UG_PVOID		addPlayer(UG_INT32 n32Playerid,UG_PVOID pvPlayer);
	UG_PVOID		delPlayer(UG_INT32 n32Playerid);
	UG_LONG			clearupPlayer();
	UG_LONG			sendData(UG_INT32 n32Senderid,UG_PCHAR p,UG_ULONG ulLen);
	
public:
	UG_LONG			cout(FILE* p);
	
public:
	map<UG_INT32,UG_PVOID>		m_mapPlayer;
	
};

#endif // !defined(AFX_TEAM_H__E0BDFD19_EF87_46A4_947C_CCA9CCFEDBBA__INCLUDED_)
