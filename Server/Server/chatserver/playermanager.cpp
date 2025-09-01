/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : PlayerManager.h
*owner        : Ben
*description  : 玩家管理类，只管理已经成功登入的玩家。
*modified     : 2004/12/20
******************************************************************************/ 

#include "incall.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlayerManager::CPlayerManager()
{
	m_mapPlayer.clear();
}

CPlayerManager::~CPlayerManager()
{
	m_mapPlayer.clear();
}

UG_LONG CPlayerManager::addPlayer(UG_INT32 n32Playerid,UG_PVOID pvPlayer)
{
	map<UG_INT32,UG_PVOID>::iterator it = m_mapPlayer.find(n32Playerid);
	if(it != m_mapPlayer.end())
	{
		CPlayer* pPlayer = (CPlayer*)(it->second);
		g_pLog->UGLog("add player failed, n32Playerid = %d.",n32Playerid);
		pPlayer->m_bOnline = FALSE; //将以前的玩家状态置为踢出状态。
	}
	m_mapPlayer[n32Playerid] = pvPlayer;
	return 0;
}

UG_PVOID CPlayerManager::delPlayer(UG_INT32 n32Playerid)
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

UG_PVOID CPlayerManager::findPlayer(UG_INT32 n32Playerid)
{
	map<UG_INT32,UG_PVOID>::iterator it = m_mapPlayer.find(n32Playerid);
	if(it != m_mapPlayer.end())
	{
		return it->second;
	}
	return NULL;
}

UG_LONG CPlayerManager::cout(FILE* p)
{
	fprintf(p,"player count = %d.\n",m_mapPlayer.size());
	map<UG_INT32,UG_PVOID>::iterator it;
	for(it = m_mapPlayer.begin(); it != m_mapPlayer.end(); it ++)
	{
		CPlayer* pPlayer = (CPlayer*)(it->second);
		pPlayer->cout(p);
	}
	return 0;
}

UG_LONG CPlayerManager::cleanup()
{
	m_mapPlayer.clear();
	return 0;
}
