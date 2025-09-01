// TradeRoom.h: interface for the CTradeRoom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRADEROOM_H__386C88CB_AECB_4DA6_9C4D_9EFE78814A81__INCLUDED_)
#define AFX_TRADEROOM_H__386C88CB_AECB_4DA6_9C4D_9EFE78814A81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTradeRoom  
{
public:
	CTradeRoom();
	virtual ~CTradeRoom();

public:
	UG_PVOID		addPlayer(UG_INT32 n32Playerid,UG_PVOID pvPlayer);
	UG_ULONG		delPlayer(UG_INT32 n32Playerid);
	
public:
	map<UG_INT32,UG_PVOID>		m_mapPlayer;
	
};

#endif // !defined(AFX_TRADEROOM_H__386C88CB_AECB_4DA6_9C4D_9EFE78814A81__INCLUDED_)
