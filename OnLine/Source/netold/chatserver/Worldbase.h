// Worldbase.h: interface for the CWorldbase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORLDBASE_H__641710E0_073D_4EB1_A79B_7B510CBF0560__INCLUDED_)
#define AFX_WORLDBASE_H__641710E0_073D_4EB1_A79B_7B510CBF0560__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWorldbase  
{
public:
	CWorldbase();
	virtual ~CWorldbase();

public:
	UG_LONG			init(CWorldbaseConfig* pWorldbaseConfig,CSysModule* pSysModule,INetServer* pWorldbaseTCPComm); //初始化函数
	UG_LONG			render(UG_ULONG ulTimer); //循环帧
	UG_LONG			recvWorldbase(PNET_SERVER_PARAM p);
	UG_LONG 		WorldbaseCBProc(PNET_SERVER_PARAM p);
	UG_LONG			disconnectAll();
	UG_LONG			disconnectPlayer(UG_PVOID pvConnect);
	UG_LONG			cleanup(); //清空，调用初始化就必须调用清空函数
	
protected:
	UG_LONG			mallocMemory();
	UG_LONG			freeMemory();
	UG_LONG			initMailSQL();
	UG_LONG			initPlayerTCPComm();
	
protected:
	UG_LONG			checkWorldbase(PNET_SERVER_PARAM p);
	UG_LONG			getWorldbaseBaseinfo(PNET_SERVER_PARAM p);
	UG_LONG			recvCheckPlayerid(PNET_SERVER_PARAM p);
	
protected:
	UG_LONG			recvPlayer(PNET_SERVER_PARAM p);
	
protected:
	static	UG_ULONG UG_FUN_CALLBACK	PlayerCB(PNET_SERVER_PARAM p);
	UG_ULONG							PlayerCBProc(PNET_SERVER_PARAM p);
	
public:
	CUGSQL*				m_pSQLChatMail; //邮件数据库指针
	INetServer*			m_pWorldbaseTCPComm;
	INetServer*			m_pPlayerTCPComm;
	CWorldbaseConfig*	m_pWorldbaseConfig;
	
public:
	UG_DWORD		m_dwRenderTimer;
	
public:
	//	CWBConnect*					m_pWorldbase;
	CWBConnect*					m_pWBConnect;
	
private:
	CSysModule*					m_pSysModule;
	CCritical					m_cs;
	CPlayer*					m_pPlayer;
	list<CPlayer*>				m_listPlayerIdle;
	map<UG_PVOID,CPlayer*>		m_mapPlayer;
	
public:
	CPlayerManager*				m_pPlayerManager;
	CSceneManager*				m_pSceneManager;
	CTeamManager*				m_pTeamManager;
	CTradeRoomManager*			m_pTradeRoomManager;
	CUnionManager*				m_pUnionManager;
	
public:
	UG_PCHAR					m_pchSendBuf; //只要和render函数在同一线程时，都可以使用该10KB的内存。

protected:
	UG_PCHAR					m_pchPlayerCurrentBuffer; //玩家当前频道缓冲
	
};

#endif // !defined(AFX_WORLDBASE_H__641710E0_073D_4EB1_A79B_7B510CBF0560__INCLUDED_)
