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
	UG_LONG			checkConnect(UG_DWORD dwParam); //У������
	UG_LONG			checkUser(MW_ACCOUNT_ID& account); //У���˻�����
	UG_LONG			addUser(UG_ULONG ulCount); //����
	UG_LONG			dropUser(UG_ULONG ulCount); //����
	
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

};

#endif // !defined(AFX_WORLDBASE_H__108D4312_5D96_4906_8894_F324623E8251__INCLUDED_)
