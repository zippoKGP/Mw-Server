/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : WorldbaseManager.h
*owner        : Ben
*description  : worldbase管理者，管理一组worldbase的连接。
*modified     : 2004/12/20
******************************************************************************/ 

#if !defined(AFX_WORLDBASEMANAGER_H__7CF7484A_9383_41D7_B89E_E0ACBC3EC79C__INCLUDED_)
#define AFX_WORLDBASEMANAGER_H__7CF7484A_9383_41D7_B89E_E0ACBC3EC79C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWorldbaseManager  
{
public:
	CWorldbaseManager();
	virtual ~CWorldbaseManager();

public:
	UG_LONG			init(CDoorConfig* pDoorConfig);
	UG_LONG			render(UG_ULONG ulTimer);
	UG_LONG			cleanup();

public:
	UG_LONG			cout(FILE* p);
	
protected:
	UG_LONG			initSQL();
	UG_LONG			initTCPComm();
	UG_LONG			mallocMemory();
	UG_LONG			freeMemory();
	UG_LONG			setAllWorldbaseNotOnline();

protected:
	UG_LONG			recvWorldbase(PNET_SERVER_PARAM p);
	UG_LONG			disconnectWorldbase(CWorldbase* p);
	
protected:
	static	UG_ULONG UG_FUN_CALLBACK	WorldbaseCB(PNET_SERVER_PARAM p);
	UG_ULONG 							WorldbaseCBProc(PNET_SERVER_PARAM p);
	
private:
	CDoorConfig*	m_pDoorConfig;
	
private:
	map<UG_PVOID,CWorldbase*>			m_mapWorldbase;
	list<CWorldbase*>					m_listWorldbaseIdle;
	CWorldbase*							m_pWorldbaseArray;
	
public:
	UG_ULONG		m_ulRenderTimer;
	CUGSQL*			m_pSQLBilling;
	CUGSQL*			m_pSQLDoorServer;
	INetServer*		m_pWorldbaseTCPComm;
	
};

#endif // !defined(AFX_WORLDBASEMANAGER_H__7CF7484A_9383_41D7_B89E_E0ACBC3EC79C__INCLUDED_)
