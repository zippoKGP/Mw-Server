// TradeRoom.cpp: implementation of the CTradeRoom class.
//
//////////////////////////////////////////////////////////////////////

#include "incall.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTradeRoom::CTradeRoom()
{
	m_mapPlayer.clear();
}

CTradeRoom::~CTradeRoom()
{
	m_mapPlayer.clear();
}

UG_PVOID CTradeRoom::addPlayer(UG_INT32 n32Playerid,UG_PVOID pvPlayer)
{
	if(m_mapPlayer.size() < CHAT_ROOM_MAX_PLAYER)
	{
		m_mapPlayer[n32Playerid] = pvPlayer;
		return this;
	}
	return NULL;
}

UG_ULONG CTradeRoom::delPlayer(UG_INT32 n32Playerid)
{
	map<UG_INT32,UG_PVOID>::iterator it = m_mapPlayer.find(n32Playerid);
	if(it != m_mapPlayer.end())
	{
		m_mapPlayer.erase(it);
		return 0;
	}
	return UG_ULONG(-1);
}

