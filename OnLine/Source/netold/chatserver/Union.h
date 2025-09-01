// Union.h: interface for the CUnion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNION_H__14D6A659_6613_47A7_AA51_E037D16683A8__INCLUDED_)
#define AFX_UNION_H__14D6A659_6613_47A7_AA51_E037D16683A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CUnion  
{
public:
	CUnion();
	virtual ~CUnion();

public:
	UG_PVOID		addPlayer(UG_INT32 n32Playerid,UG_PVOID pvPlayer);
	UG_PVOID		delPlayer(UG_INT32 n32Playerid);
	UG_LONG			clearupPlayer();
	
public:
	UG_ULONG		cout(FILE* p);
	
public:
	map<UG_INT32,UG_PVOID>		m_mapPlayer;
	
};

#endif // !defined(AFX_UNION_H__14D6A659_6613_47A7_AA51_E037D16683A8__INCLUDED_)
