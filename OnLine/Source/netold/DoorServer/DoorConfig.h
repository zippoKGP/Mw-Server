/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : DoorConfig.h
*owner        : Ben
*description  : ����doorserver�������ļ���������Ϣ����������
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
	UG_DWORD	m_dwHostIPForWorldbaseConnect; //��worldbase���ӵ�ip
	UG_WORD		m_wPortForWorldbaseConnect; //��worldbase���ӵ�port
	UG_DWORD	m_dwHostIPForManagerConnect; //������Ա���ӵ�ip��Ŀǰû����
	UG_WORD		m_wPortForManagerConnect; //������Ա���ӵ�port��Ŀǰû����

public:
	UG_CHAR		m_szSQLBillingHost[MAX_STRING_BUFFER]; //billing���ݿ����ڵĵ���
	UG_CHAR		m_szSQLBillingDBName[MAX_STRING_BUFFER]; //billing���ݿ�����
	UG_CHAR		m_szSQLBillingUser[MAX_STRING_BUFFER]; //billing���ݿ��û���
	UG_CHAR		m_szSQLBillingPassword[MAX_STRING_BUFFER]; //billing���ݿ��û�����

public:
	UG_CHAR		m_szSQLDoorServerHost[MAX_STRING_BUFFER]; //door_server���ݿ����ڵĵ���
	UG_CHAR		m_szSQLDoorServerDBName[MAX_STRING_BUFFER]; //door_server���ݿ�����
	UG_CHAR		m_szSQLDoorServerUser[MAX_STRING_BUFFER]; //door_server���ݿ��û���
	UG_CHAR		m_szSQLDoorServerPassword[MAX_STRING_BUFFER]; //door_server���ݿ��û�����

};

#endif // !defined(AFX_DOORCONFIG_H__4120F8E4_0667_4B36_B825_B7CC54A768C0__INCLUDED_)
