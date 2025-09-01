// TradeRoomManager.cpp: implementation of the CTradeRoomManager class.
//
//////////////////////////////////////////////////////////////////////

#include "incall.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTradeRoomManager::CTradeRoomManager()
{
	m_pTradeRoom = NULL;
	m_listTradeRoomIdle.clear();
	m_mapTradeRoom.clear();
	m_n32TradeRoomOwner = 0;
	m_pvTradeRoomOwner = NULL;
	m_ulMaxTradeRoom = 0;
}

CTradeRoomManager::~CTradeRoomManager()
{
	UGDel<CTradeRoom*>(m_pTradeRoom);
	m_listTradeRoomIdle.clear();
	m_mapTradeRoom.clear();
}

UG_ULONG CTradeRoomManager::init(UG_ULONG ulCount)
{
	m_ulMaxTradeRoom = ulCount;
	m_listTradeRoomIdle.clear();
	m_mapTradeRoom.clear();
	m_pTradeRoom = UGNew<CTradeRoom>(ulCount,__FILE__,__LINE__);
	for(UG_ULONG i = 0; i < ulCount; i ++)
	{
		m_listTradeRoomIdle.push_back(m_pTradeRoom + i);
	}
	return 0;
}

UG_ULONG CTradeRoomManager::cleanup()
{
	m_listTradeRoomIdle.clear();
	m_mapTradeRoom.clear();
	for(UG_ULONG i = 0; i < m_ulMaxTradeRoom; i ++)
	{
		m_listTradeRoomIdle.push_back(m_pTradeRoom + i);
	}
	return 0;
}

UG_PVOID CTradeRoomManager::addTradeRoom(INT32 n32Playerid,INT32 n32TradeRoomid,UG_PVOID pvPlayer)
{
	list<CTradeRoom*>::iterator it = m_listTradeRoomIdle.begin();
	if(it == m_listTradeRoomIdle.end())
	{
		return NULL;
	}
	else
	{
		(*it)->addPlayer(n32Playerid,pvPlayer);
		m_mapTradeRoom[n32TradeRoomid] = *it;
		m_listTradeRoomIdle.erase(it);
		m_n32TradeRoomOwner = n32Playerid;
		m_pvTradeRoomOwner = pvPlayer;
		return pvPlayer;
	}
}

UG_ULONG CTradeRoomManager::delTradeRoom(INT32 n32TradeRoomid)
{
	map<UG_INT32,CTradeRoom*>::iterator it = m_mapTradeRoom.find(n32TradeRoomid);
	if(it == m_mapTradeRoom.end())
	{
		return UG_ULONG(-1);
	}
	else
	{
		m_listTradeRoomIdle.push_back(it->second);
		m_mapTradeRoom.erase(it);
	}
	return 0;
}

UG_PVOID CTradeRoomManager::addPlayer(INT32 n32Playerid,INT32 n32TradeRoomid,UG_PVOID pvPlayer)
{
	map<UG_INT32,CTradeRoom*>::iterator it = m_mapTradeRoom.find(n32TradeRoomid);
	if(it == m_mapTradeRoom.end())
	{
		return NULL;
	}
	else
	{
		return (it->second)->addPlayer(n32Playerid,pvPlayer);
	}
}

UG_ULONG CTradeRoomManager::delPlayer(INT32 n32Playerid,INT32 n32TradeRoomid,UG_PVOID pvPlayer)
{
	map<UG_INT32,CTradeRoom*>::iterator it = m_mapTradeRoom.find(n32TradeRoomid);
	if(it == m_mapTradeRoom.end())
	{
		return UG_ULONG(-1);
	}
	else
	{
		return (it->second)->delPlayer(n32Playerid);
	}
}

