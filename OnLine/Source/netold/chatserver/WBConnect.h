// WBConnect.h: interface for the CWBConnect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WBCONNECT_H__66E2CCCE_8457_429F_AE0A_8C1963EF8B49__INCLUDED_)
#define AFX_WBCONNECT_H__66E2CCCE_8457_429F_AE0A_8C1963EF8B49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWBConnect  
{
public:
	CWBConnect();
	virtual ~CWBConnect();

public:
	UG_LONG			dispatchMsg(UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR pchData,UG_ULONG ulLen);
	UG_LONG			setParent(UG_PVOID pvParent);
	
protected:
	UG_LONG			disconnectPlayer(UG_INT32 n32Playerid);
	
protected:
	UG_LONG			addPlayerInScene(PCHAT_SCENE p);
	UG_LONG			delPlayerInScene(PCHAT_SCENE p);
	
public:
	UG_LONG			initFriends(UG_INT32 n32Playerid,PBASE_FRIEDN_LIST p);
	UG_LONG			addFriend(UG_INT32 n32Playerid,UG_INT32 n32Friendid);
	UG_LONG			changeFriend(UG_INT32 n32Playerid,friend_t f);
	UG_LONG			delFriend(UG_INT32 n32Playerid,UG_INT32 n32Friendid);
	
public:
	UG_LONG			getCurrentID(UG_INT32 n32Playerid,UG_PCHAR p,UG_ULONG ulLen);
	
public:
	UG_LONG			createUnion(UG_INT32 n32Playerid,UG_INT32 n32Unionid);
	UG_LONG			dropUnion(UG_INT32 n32Playerid,UG_INT32 n32Unionid);
	UG_LONG			intoUnion(UG_INT32 n32Playerid,INT32 n32Unionid);
	UG_LONG			outUnion(UG_INT32 n32Playerid,INT32 n32Unionid);
	
public:
	UG_LONG			createTeam(UG_INT32 n32Playerid);
//	UG_LONG			dropTeam(UG_INT32 n32Playerid,UG_INT32 n32Teamid);
	UG_LONG			intoTeam(UG_INT32 n32Playerid,INT32 n32Teamid);
	UG_LONG			outTeam(UG_INT32 n32Playerid,INT32 n32Teamid);
	
public:
	UG_LONG			cout(FILE* p);
	
public:
	UG_DWORD		m_dwIP;
	UG_WORD			m_wPort;
	UG_BOOL			m_bOnline;
	UG_DWORD		m_dwTimer;
	UG_PVOID		m_pvConnect;
	INetServer*		m_pTCPComm;
	
public:
	WB_CONNECT_STEP				m_stepWB;
	
private:
	UG_PVOID		m_pvParent;
	
};

#endif // !defined(AFX_WBCONNECT_H__66E2CCCE_8457_429F_AE0A_8C1963EF8B49__INCLUDED_)
