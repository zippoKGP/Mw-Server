// WorldbaseManager.h: interface for the CWorldbaseManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORLDBASEMANAGER_H__D2B5A64D_3BC5_4DEF_B3D7_F561A3F39839__INCLUDED_)
#define AFX_WORLDBASEMANAGER_H__D2B5A64D_3BC5_4DEF_B3D7_F561A3F39839__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWorldbaseManager  
{
public:
	CWorldbaseManager();
	virtual ~CWorldbaseManager();

public:
	UG_LONG					init(UG_PCHAR pchPathName); //初始化函数
	UG_LONG					render(UG_ULONG ulTimer); //循环帧
	UG_LONG					cleanup(); //清空，调用初始化就必须调用清空函数
	
public:
	UG_LONG					coutConfig(FILE* p);
	UG_LONG					coutWorldbase(FILE* p);
	UG_LONG					coutPlayerManager(FILE* p);
	UG_LONG					coutSceneManager(FILE* p);
	UG_LONG					coutAll(FILE* p);
	UG_LONG					coutPlayerCount(FILE* p);

protected:
	UG_LONG					initWorldbaseTCPComm();
	
protected:
	static	UG_ULONG UG_FUN_CALLBACK	WorldbaseCB(PNET_SERVER_PARAM p);
	UG_ULONG 							WorldbaseCBProc(PNET_SERVER_PARAM p);

private:
	UG_DWORD					m_dwSendBufferTimer;
	
public:
	INetServer*					m_pWorldbaseTCPComm;
	CWorldbaseManagerConfig*	m_pWorldbaseManagerConfig;
	CSysModule*					m_pSysModule;
	
public:
	CWorldbase*				m_pWorldbase;
	list<CWorldbase*>		m_listWorldbase;

};

#endif // !defined(AFX_WORLDBASEMANAGER_H__D2B5A64D_3BC5_4DEF_B3D7_F561A3F39839__INCLUDED_)
