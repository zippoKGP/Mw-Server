/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : Player.h
*owner        : Ben
*description  : 玩家基本类，包含玩家各种信息，操作等。
*modified     : 2004/12/20
******************************************************************************/ 

#if !defined(AFX_PLAYER_H__CBDB2645_2710_40AC_98E6_1FC696B1850D__INCLUDED_)
#define AFX_PLAYER_H__CBDB2645_2710_40AC_98E6_1FC696B1850D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPlayer  
{
public:
	CPlayer();
	virtual ~CPlayer();

public:
	UG_LONG			init();
	UG_LONG			mallicMemory();
	UG_LONG			freeMemory();
	UG_LONG			cleanup();
	
public:
	UG_LONG			dispatchMsg(UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR pchData,UG_ULONG ulLen);
	UG_LONG			setParent(UG_PVOID pvParent);
	
public:
	UG_LONG			intoScene(INT32 n32Sceneid);
	UG_LONG			outScene(INT32 n32Sceneid);

public:
	UG_LONG			createUnion(INT32 n32Unionid);
	UG_LONG			intoUnion(INT32 n32Unionid);
	UG_LONG			outUnion(INT32 n32Unionid);
	
public:
	UG_LONG			createTeam(INT32 n32Teamid);
	UG_LONG			intoTeam(INT32 n32Teamid);
	UG_LONG			outTeam(INT32 n32Teamid);
	
public:
	UG_LONG			initFriends(PBASE_FRIEDN_LIST p);
	UG_LONG			addFriend(UG_INT32 n32Friendid,UG_INT32 n32Attribute);
	UG_LONG			delFriend(UG_INT32 n32Friendid);

public:
	UG_LONG			sendMail(UG_INT32 n32Playerid);
	UG_LONG			sendMailCount();
	UG_LONG			recvMail(UG_PCHAR p,UG_ULONG ulLen);
	
public:
	UG_LONG			sendCurrentChannel(UG_INT32 n32id[],UG_INT32 nCount,UG_INT32 n32Pos);

public:
	UG_LONG			playerLogin(PCHAT_CHECK_RETURN p);
	UG_LONG			disconnect();
	
public:
	UG_LONG			cout(FILE* p);
	UG_LONG			sendData(UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR p,UG_ULONG ulLen);
	
protected:
	UG_LONG			filterFriend(UG_INT32 n32Playerid);
	UG_LONG			setCommandConfig(UG_PCHAR pchData,UG_ULONG ulLen);
	
protected:
	UG_LONG			sendCurrentChannel(UG_INT32 n32Playerid,UG_PCHAR p,UG_ULONG ulLen);
	UG_LONG			sendTeamChannel(UG_INT32 n32Playerid,UG_PCHAR p,UG_ULONG ulLen);
	UG_LONG			sendTeamMember(UG_INT32 nSenderid,UG_PCHAR p,UG_ULONG ulLen);
//	UG_ULONG		sendUnionChannel(PCHAT_CHANNEL p,UG_PCHAR pchData,UG_ULONG ulLen);
	UG_LONG			sendTradeChannel(UG_PCHAR p,UG_ULONG ulLen);
	UG_LONG			sendWorldChannel(UG_PCHAR p,UG_ULONG ulLen);
	UG_LONG			sendPrivateChannel(UG_PCHAR p,UG_ULONG ulLen);
	UG_LONG			sendSystemChannel(UG_PCHAR p,UG_ULONG ulLen);
	

public:
	UG_DWORD		m_dwIP;
	UG_WORD			m_wPort;
	UG_BOOL			m_bOnline;
	UG_DWORD		m_dwTimer;
	UG_PVOID		m_pvConnect;
	INetServer*		m_pTCPComm;

public:
	UG_INT32		m_n32Playerid;
	UG_CHAR			m_szNickname[MAX_NAME_LENGTH + 1];
	UG_INT32		m_nAttribute; //玩家属性
	UG_INT32		m_nLevel; //玩家等级
	
public:
	CTeam*			m_pTeam;
	CUnion*			m_pUnion;
	CTradeRoom*		m_pTradeRoom;
	CScene*			m_pScene;

public:
	CFriends		m_friends;

public:
	PLAYER_CONNECT_STEP			m_stepPlayer;
	
private:
	UG_PVOID		m_pvParent;

public:
	UG_PCHAR		m_pchCurrentBuffer; //(10 * (MAX_CHAT_LENGTH + 1))BYTE
	UG_LONG			m_lCurrentBufferPos;
	UG_INT32		m_n32CurrentPos;
	
	UG_ULONG		m_ulCurrentTimer; //3000ms
	UG_ULONG		m_ulTeamTimer; //2000ms
	UG_ULONG		m_ulUnionTimer; //3000ms
	UG_ULONG		m_ulTradeTimer; //30000ms
	UG_ULONG		m_ulWorldTimer; //10000ms
	UG_ULONG		m_ulPrivateTimer; //2000ms
	CLIENT_CONFIG	m_clConfig;
	
public:
	CMailMsgManager*		m_pMailMsgManager;
	
private:
	UGINT					m_nTeamMember[TEAM_MAX_PLAYER];

};

#endif // !defined(AFX_PLAYER_H__CBDB2645_2710_40AC_98E6_1FC696B1850D__INCLUDED_)
