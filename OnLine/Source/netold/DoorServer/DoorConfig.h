/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : DoorConfig.h
*owner        : Ben
*description  : 处理doorserver的配置文件，并将信息保存下来。
*modified     : 2004/12/20
******************************************************************************/ 

#if !defined(AFX_DOORCONFIG_H__4120F8E4_0667_4B36_B825_B7CC54A768C0__INCLUDED_)
#define AFX_DOORCONFIG_H__4120F8E4_0667_4B36_B825_B7CC54A768C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define		MAX_STRING_BUFFER		1024

class CDoorConfig  
{
public:
	CDoorConfig();
	virtual ~CDoorConfig();

public:
	UG_LONG		init(UG_PCHAR pchPathName);
	UG_LONG		cleanup();

public:
	UG_LONG			cout(FILE* p);
	
public:
	UG_DWORD	m_dwHostIPForWorldbaseConnect; //供worldbase连接的ip
	UG_WORD		m_wPortForWorldbaseConnect; //供worldbase连接的port
	UG_DWORD	m_dwHostIPForManagerConnect; //供管理员连接的ip，目前没有用
	UG_WORD		m_wPortForManagerConnect; //供管理员连接的port，目前没有用

public:
	UG_CHAR		m_szSQLBillingHost[MAX_STRING_BUFFER]; //billing数据库所在的电脑
	UG_CHAR		m_szSQLBillingDBName[MAX_STRING_BUFFER]; //billing数据库名称
	UG_CHAR		m_szSQLBillingUser[MAX_STRING_BUFFER]; //billing数据库用户名
	UG_CHAR		m_szSQLBillingPassword[MAX_STRING_BUFFER]; //billing数据库用户密码

public:
	UG_CHAR		m_szSQLDoorServerHost[MAX_STRING_BUFFER]; //door_server数据库所在的电脑
	UG_CHAR		m_szSQLDoorServerDBName[MAX_STRING_BUFFER]; //door_server数据库名称
	UG_CHAR		m_szSQLDoorServerUser[MAX_STRING_BUFFER]; //door_server数据库用户名
	UG_CHAR		m_szSQLDoorServerPassword[MAX_STRING_BUFFER]; //door_server数据库用户密码

};

#endif // !defined(AFX_DOORCONFIG_H__4120F8E4_0667_4B36_B825_B7CC54A768C0__INCLUDED_)
