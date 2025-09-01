// Worldbase.h: interface for the CWorldbase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORLDBASE_H__108D4312_5D96_4906_8894_F324623E8251__INCLUDED_)
#define AFX_WORLDBASE_H__108D4312_5D96_4906_8894_F324623E8251__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWorldbase  
{
public:
	CWorldbase();
	virtual ~CWorldbase();

public:
	UG_LONG			dispatchMsg(UG_DWORD dwMsgID,UG_DWORD wParam,UG_PCHAR pchData,UG_ULONG ulLen);

public:
	UG_LONG			cout(FILE* p);

public:
	UG_LONG			checkConnect(UG_DWORD dwParam); //校验连接
	UG_LONG			checkUser(MW_ACCOUNT_ID& account); //校验账户密码
	UG_LONG			addUser(UG_ULONG ulCount); //加人
	UG_LONG			dropUser(UG_ULONG ulCount); //减人
	
public:
	UG_PVOID		m_pvConnect; //连接标志
	UG_DWORD		m_dwTimer; //开始时间
	UG_BOOL			m_bOnline; //是否在线，在接收数据时如果不掉线则设置，在用户处理完数据后发送数据时处理断开事件
	WORLD_BASE		m_wbData;
	UG_DWORD		m_dwPrivateIP;
	UG_DWORD		m_dwPrivatePort;
	UG_DWORD		m_dwCheckID;
	UG_DWORD		m_dwCurrentMaxOnline; //目前运行时的曾经有过的最大人数
	WORLDBASE_STEP	m_stepWB;
	UG_PVOID		m_pParent;

};

#endif // !defined(AFX_WORLDBASE_H__108D4312_5D96_4906_8894_F324623E8251__INCLUDED_)
