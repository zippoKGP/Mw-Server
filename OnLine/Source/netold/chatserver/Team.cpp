// Team.cpp: implementation of the CTeam class.
//
//////////////////////////////////////////////////////////////////////

#include "incall.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTeam::CTeam()
{
	m_mapPlayer.clear();
}

CTeam::~CTeam()
{
	m_mapPlayer.clear();
}

UG_PVOID CTeam::addPlayer(UG_INT32 n32Playerid,UG_PVOID pvPlayer)
{
	if(m_mapPlayer.size() < TEAM_MAX_PLAYER)
	{
		m_mapPlayer[n32Playerid] = pvPlayer;
		return this;
	}
	return NULL;
}

UG_PVOID CTeam::delPlayer(UG_INT32 n32Playerid)
{
	map<UG_INT32,UG_PVOID>::iterator it = m_mapPlayer.find(n32Playerid);
	if(it != m_mapPlayer.end())
	{
		m_mapPlayer.erase(it);
		return this;
	}
	return NULL;
}

UG_LONG CTeam::cout(FILE* p)
{
	fprintf(p,"union count = %d.\n",m_mapPlayer.size());
	map<UG_INT32,UG_PVOID>::iterator it;
	for(it = m_mapPlayer.begin(); it != m_mapPlayer.end(); it ++)
	{
		CPlayer* pPlayer = (CPlayer*)(it->second);
		pPlayer->cout(p);
	}
	return 0;
}

UG_LONG CTeam::clearupPlayer()
{
	map<UG_INT32,UG_PVOID>::iterator it;
	for(it = m_mapPlayer.begin(); it != m_mapPlayer.end(); it ++)
	{
		CPlayer* pPlayer = (CPlayer*)(it->second);
		pPlayer->m_pTeam = NULL;
	}
	return 0;
}

UG_LONG CTeam::sendData(UG_INT32 n32Senderid,UG_PCHAR p,UG_ULONG ulLen)
{
	map<UG_INT32,UG_PVOID>::iterator it;
	for(it = m_mapPlayer.begin(); it != m_mapPlayer.end(); it ++)
	{
		CPlayer* pPlayer = (CPlayer*)(it->second);
		if(pPlayer)
		{
			if(pPlayer->m_clConfig.channel_team_flag)
			{
				pPlayer->sendData(NET_MSG_TEAM_CHANNEL_RETURN,n32Senderid,p,ulLen);
			}
		}
	}
	return 0;
}
