/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : GMManager.h
*owner        : Ben
*description  : manager管理者，管理一组manager的连接。
*modified     : 2004/12/20
******************************************************************************/ 

#if !defined(AFX_GMMANAGER_H__3455284F_0BA1_451A_B9D3_F57C99E3D58E__INCLUDED_)
#define AFX_GMMANAGER_H__3455284F_0BA1_451A_B9D3_F57C99E3D58E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGMManager  
{
public:
	CGMManager();
	virtual ~CGMManager();

public:
	UG_LONG			cout(FILE* p);

public:
	UG_LONG			init(CDoorConfig* pDoorConfig);
	UG_LONG			render(UG_ULONG ulTimer);
	UG_LONG			cleanup();

protected:
	UG_LONG			mallocMemory();
	UG_LONG			freeMemory();
	UG_LONG			initTCPComm();
	
protected:
	UG_LONG			recvGM(PNET_SERVER_PARAM p);
	UG_LONG			disconnectGM(CGM* p);
	
protected:
	static	UG_ULONG UG_FUN_CALLBACK	GMCB(PNET_SERVER_PARAM p);
	UG_ULONG 							GMCBProc(PNET_SERVER_PARAM p);

public:
	UG_ULONG		m_ulRenderTimer;
	INetServer*		m_pTCPComm;

private:
	CDoorConfig*	m_pDoorConfig;
	CCritical		m_cs;
	
private:
	map<UG_PVOID,CGM*>		m_mapGM;
	list<CGM*>				m_listGMIdle;
	CGM*					m_pGMArray;

};

#endif // !defined(AFX_GMMANAGER_H__3455284F_0BA1_451A_B9D3_F57C99E3D58E__INCLUDED_)
