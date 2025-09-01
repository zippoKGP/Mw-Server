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
	g_pLog->UGLog("addPlayer in CScene error because n32Playerid = %d exist.",n32Playerid);
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
	g_pLog->UGLog("delPlayer in CScene error because not find player = %d.",n32Playerid);
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
