// GM.h: interface for the CGM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GM_H__5670271A_1F5D_418E_9A43_72D450B15D16__INCLUDED_)
#define AFX_GM_H__5670271A_1F5D_418E_9A43_72D450B15D16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGM  
{
public:
	CGM();
	virtual ~CGM();

public:
	UG_LONG			dispatchMsg(UG_DWORD dwMsgID,UG_DWORD wParam,UG_PCHAR pchData,UG_ULONG ulLen);
	
public:
	UG_LONG			cout(FILE* p);

protected:
	UGLONG			checkUser(UG_PCHAR pchData,UG_ULONG ulLen);
	UGLONG			sendWorldbase();
	UGLONG			isGMName(UGPCHAR pchGMName);
	
protected:
	UGLONG			queryBilling(UGPCHAR pchQuery,UGPVOID& pvResult);
	UGLONG			queryBilling(UGPCHAR pchQuery,UGPVOID& pvResult,UGDWORD& dwRows);
	UGLONG			getQueryResultBilling(UGPVOID pvResult,UGDWORD dwRowIndex,UGDWORD dwFieldIndex,UGPCHAR& pchResult);
	UGLONG			freeResultBilling(UGPVOID pvResult);
	UGLONG			sendWBData(UGDWORD dwMsgID,UGDWORD dwParam,UGPCHAR pchData,UGDWORD dwLen);
	UGLONG			getWBData(PSERVERINFO& pWBData,UGINT& nCount);
	
public:
	UG_PVOID		m_pvConnect; //连接标志
	UG_DWORD		m_dwTimer; //开始时间
	UG_BOOL			m_bOnline; //是否在线，在接收数据时如果不掉线则设置，在用户处理完数据后发送数据时处理断开事件
	UG_DWORD		m_dwIP;
	UG_DWORD		m_dwPort;
	UG_DWORD		m_dwCheckID;
	GM_STEP			m_stepGM;
	UG_PVOID		m_pParent;

};

#endif // !defined(AFX_GM_H__5670271A_1F5D_418E_9A43_72D450B15D16__INCLUDED_)
