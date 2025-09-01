// WorldbaseConfig.h: interface for the CWorldbaseConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORLDBASECONFIG_H__C1EAE866_546C_451A_882C_97012D6B6B5C__INCLUDED_)
#define AFX_WORLDBASECONFIG_H__C1EAE866_546C_451A_882C_97012D6B6B5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWorldbaseConfig  
{
public:
	CWorldbaseConfig();
	virtual ~CWorldbaseConfig();

public:
	UG_LONG			init(CUGIni* pIni,UG_PCHAR pchApp);
	UG_LONG			cleanup();
	
public:
	UG_LONG			cout(FILE* p);

protected:	
	UG_LONG			initFilterTrade(UG_PCHAR pchFilter);
	UGLONG			getGMLog(UGPCHAR pchGMIDS);
	
public:
	UG_DWORD		m_dwWorldbaseIP; //必须为该ip的worldbase才能连接
	UG_DWORD		m_dwWorldbaseCheckID; //该ip的worldbase的id，目前没有使用
	UG_DWORD		m_dwIPForPlayer; //供玩家连接的聊天服务器的ip
	UG_DWORD		m_dwPortForPlayer; //供玩家连接的聊天服务器的port
	UG_ULONG		m_ulMaxPlayer; //最大玩家数
	UG_DWORD		m_dwIPForGM; //供GM连接的聊天服务器的ip
	UG_DWORD		m_dwPortForGM; //供GM连接的聊天服务器的port
	
public:
	UG_CHAR			m_szMailDBHost[261]; //邮件数据库的地址
	UG_CHAR			m_szMailDBName[261]; //邮件数据库的名称
	UG_CHAR			m_szMailDBUser[51]; //邮件数据库的用户名
	UG_CHAR			m_szMailDBPassword[51]; //邮件数据库的用户密码

public:
	UGINT			m_nFilterTrade;
	UGCHAR**		m_ppchFilterTrade;

public:
	UG_ULONG		m_ulSceneNum; //场景数据，由worldbase传过来

//add by ben 2005-09-21	
public:
	UGINT			m_nGMLogCount;
	UGPINT			m_pGMLog;
	
};

#endif // !defined(AFX_WORLDBASECONFIG_H__C1EAE866_546C_451A_882C_97012D6B6B5C__INCLUDED_)
