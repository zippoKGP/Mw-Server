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
	UGLONG			disconnect();
	
public:
	UG_LONG			checkConnect(UG_DWORD dwParam); //У������
	UG_LONG			checkUser(MW_ACCOUNT_ID& account); //У���˻�����
	UGLONG			setUserNum(UGINT nCount); //����
	UGINT			resetCurrentMaxOnline();
	UGINT			getCurrentMaxOnline();
	UGLONG			sendWBData(UGDWORD dwMsgID,UGDWORD dwParam,UGPCHAR pchData,UGDWORD dwLen);
	
protected:
	UGLONG			unlockWorldserverUsercard();
	UGLONG			checkUsercard(USERCARD* p);
	UGLONG			unlockUsercard(USERCARD* p);
	UGLONG			delUsercard(USERCARD* p);
	UGLONG			convertTimer(UGPCHAR pchTimer,UGINT& nYear,UGINT& nMonth,UGINT& nDay);
	UGLONG			compareTimer(UGINT nYear,UGINT nMonth,UGINT nDay,UGINT nCYear,UGINT nCMonth,UGINT nCDay);
	
protected:
	UGINT			moneyToTime(UGINT nMoney);
	UGINT			timeToMoney(UGINT nTimes);
	UGINT			dropMoney(MW_BILLING* nb);
	UGINT			saveMoney(MW_BILLING* nb);
	UGINT			dropMoney(UGINT nBillingID,UGINT nMoney);
	UGINT			getMoney(UGINT nBillingID);
	UGINT			setMoney(UGINT nBillingID,UGINT nMoney);
	UGINT			beginDropMoney(MW_BILLING* nb);
	UGINT			insertLog(MW_BILLING* nb);
	UGINT			dropLog(MW_BILLING* nb);
	UGINT			saveLog(MW_BILLING* nb);
	
protected:
	UGINT			logGMMonitor(GM_MONITOR* p);
	
protected:
	UGLONG			queryDoor(UGPCHAR pchQuery,UGPVOID& pvResult);
	UGLONG			queryBilling(UGPCHAR pchQuery,UGPVOID& pvResult);
	UGLONG			queryDoor(UGPCHAR pchQuery,UGPVOID& pvResult,UGDWORD& dwRows);
	UGLONG			queryBilling(UGPCHAR pchQuery,UGPVOID& pvResult,UGDWORD& dwRows);
	UGLONG			getQueryResultDoor(UGPVOID pvResult,UGDWORD dwRowIndex,UGDWORD dwFieldIndex,UGPCHAR& pchResult);
	UGLONG			getQueryResultBilling(UGPVOID pvResult,UGDWORD dwRowIndex,UGDWORD dwFieldIndex,UGPCHAR& pchResult);
	UGLONG			freeResultDoor(UGPVOID pvResult);
	UGLONG			freeResultBilling(UGPVOID pvResult);
	
public:
	UG_PVOID		m_pvConnect; //���ӱ�־
	UG_DWORD		m_dwTimer; //��ʼʱ��
	UG_BOOL			m_bOnline; //�Ƿ����ߣ��ڽ�������ʱ��������������ã����û����������ݺ�������ʱ����Ͽ��¼�
	WORLD_BASE		m_wbData;
	UG_DWORD		m_dwPrivateIP;
	UG_DWORD		m_dwPrivatePort;
	UG_DWORD		m_dwCheckID;
	UG_DWORD		m_dwCurrentMaxOnline; //Ŀǰ����ʱ�������й����������
	WORLDBASE_STEP	m_stepWB;
	UG_PVOID		m_pParent;
	UGDWORD			m_dwLoginCount;
	UGDWORD			m_dwLogoutCount;
	
public:
	UGDWORD			m_dwLastRecvDataTimer;

private:
	map<int,int>	m_mapMoneyLog;

};

#endif // !defined(AFX_WORLDBASE_H__108D4312_5D96_4906_8894_F324623E8251__INCLUDED_)
