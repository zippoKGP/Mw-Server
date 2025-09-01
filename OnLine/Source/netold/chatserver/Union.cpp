// Union.cpp: implementation of the CUnion class.
//
//////////////////////////////////////////////////////////////////////

#include "incall.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUnion::CUnion()
{
	m_mapPlayer.clear();
}

CUnion::~CUnion()
{
	m_mapPlayer.clear();
}

UG_PVOID CUnion::addPlayer(UG_INT32 n32Playerid,UG_PVOID pvPlayer)
{
	if(m_mapPlayer.size() < UNION_MAX_PLAYER)
	{
		m_mapPlayer[n32Playerid] = pvPlayer;
		return this;
	}
	return NULL;
}

UG_PVOID CUnion::delPlayer(UG_INT32 n32Playerid)
{
	map<UG_INT32,UG_PVOID>::iterator it = m_mapPlayer.find(n32Playerid);
	if(it != m_mapPlayer.end())
	{
		m_mapPlayer.erase(it);
		return this;
	}
	return NULL;
}

UG_ULONG CUnion::cout(FILE* p)
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

UG_LONG CUnion::clearupPlayer()
{
	map<UG_INT32,UG_PVOID>::iterator it;
	for(it = m_mapPlayer.begin(); it != m_mapPlayer.end(); it ++)
	{
		CPlayer* pPlayer = (CPlayer*)(it->second);
		pPlayer->m_pUnion = NULL;
	}
	return 0;
}
