// TradeRoomManager.h: interface for the CTradeRoomManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRADEROOMMANAGER_H__4D1C3C69_75DE_4504_920D_CFEAC3DCC1EA__INCLUDED_)
#define AFX_TRADEROOMMANAGER_H__4D1C3C69_75DE_4504_920D_CFEAC3DCC1EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTradeRoomManager  
{
public:
	CTradeRoomManager();
	virtual ~CTradeRoomManager();

public:
	UG_ULONG		init(UG_ULONG ulCount);
	UG_ULONG		cleanup();
	
public:
	UG_PVOID		addTradeRoom(INT32 n32Playerid,INT32 n32TradeRoomid,UG_PVOID pvPlayer);
	UG_ULONG		delTradeRoom(INT32 n32TradeRoomid);
	UG_PVOID		addPlayer(INT32 n32Playerid,INT32 n32TradeRoomid,UG_PVOID pvPlayer);
	UG_ULONG		delPlayer(INT32 n32Playerid,INT32 n32TradeRoomid,UG_PVOID pvPlayer);
	
public:
	CTradeRoom*					m_pTradeRoom;
	list<CTradeRoom*>			m_listTradeRoomIdle;
	map<UG_INT32,CTradeRoom*>	m_mapTradeRoom;
	UG_INT32					m_n32TradeRoomOwner;
	UG_PVOID					m_pvTradeRoomOwner;
	UG_ULONG					m_ulMaxTradeRoom;
	
};

#endif // !defined(AFX_TRADEROOMMANAGER_H__4D1C3C69_75DE_4504_920D_CFEAC3DCC1EA__INCLUDED_)
