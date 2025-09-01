// UnionManager.h: interface for the CUnionManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNIONMANAGER_H__86C343AB_108F_4FE0_B0CD_4A4E3D3DEC1B__INCLUDED_)
#define AFX_UNIONMANAGER_H__86C343AB_108F_4FE0_B0CD_4A4E3D3DEC1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CUnionManager  
{
public:
	CUnionManager();
	virtual ~CUnionManager();

public:
	UG_ULONG		init(UG_ULONG ulCount);
	UG_ULONG		cleanup();
	
public:
	UG_PVOID		addUnion(INT32 n32Unionid);
	UG_PVOID		delUnion(INT32 n32Unionid);
	UG_PVOID		addPlayer(INT32 n32Playerid,INT32 n32Unionid,UG_PVOID pvPlayer);
	UG_PVOID		delPlayer(INT32 n32Playerid,INT32 n32Unionid);

public:
	UG_ULONG		cout(FILE* p);
	
public:
	CUnion*						m_pUnion;
	list<CUnion*>				m_listUnionIdle;
	map<UG_INT32,CUnion*>		m_mapUnion;
	UG_ULONG					m_ulMaxUnion;
	
};

#endif // !defined(AFX_UNIONMANAGER_H__86C343AB_108F_4FE0_B0CD_4A4E3D3DEC1B__INCLUDED_)
