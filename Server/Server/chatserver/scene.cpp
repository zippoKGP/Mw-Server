// Scene.cpp: implementation of the CScene class.
//
//////////////////////////////////////////////////////////////////////

#include "incall.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScene::CScene()
{
	m_mapPlayer.clear();
}

CScene::~CScene()
{
	m_mapPlayer.clear();
}

UG_PVOID CScene::addPlayer(UG_INT32 n32Playerid,UG_PVOID pvPlayer)
{
	map<UG_INT32,UG_PVOID>::iterator it = m_mapPlayer.find(n32Playerid);
	if(it == m_mapPlayer.end())
	{
		m_mapPlayer[n32Playerid] = pvPlayer;
		return this;
	}
	return NULL;
}

UG_PVOID CScene::delPlayer(UG_INT32 n32Playerid)
{
	map<UG_INT32,UG_PVOID>::iterator it = m_mapPlayer.find(n32Playerid);
	if(it != m_mapPlayer.end())
	{
		UG_PVOID pvPlayer = it->second;
		m_mapPlayer.erase(it);
		return pvPlayer;
	}
	return NULL;
}

UG_LONG CScene::cout(FILE* p)
{
	fprintf(p,"scene player count = %d.\n",m_mapPlayer.size());
	map<UG_INT32,UG_PVOID>::iterator it;
	for(it = m_mapPlayer.begin(); it != m_mapPlayer.end(); it ++)
	{
		CPlayer* pPlayer = (CPlayer*)(it->second);
		pPlayer->cout(p);
	}
	return 0;
}
