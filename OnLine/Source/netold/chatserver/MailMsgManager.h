/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : MailMsgManager.h
*owner        : Ben
*description  : ����Ϣ�����ࡣ
*modified     : 2004/12/20
******************************************************************************/ 

#if !defined(AFX_MAILMSGMANAGER_H__88BB260B_00F4_4C51_89B7_900178BBB89E__INCLUDED_)
#define AFX_MAILMSGMANAGER_H__88BB260B_00F4_4C51_89B7_900178BBB89E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMailMsgManager  
{
public:
	CMailMsgManager();
	virtual ~CMailMsgManager();

public:
	UG_LONG		init(UG_LONG lMaxCount);
	UG_LONG		cleanup();

//���loginʱ���ã���ոýṹ�Թ������ʹ��
public:
	UG_LONG		reset(UG_INT32 n32Playerid);

//logoutʱ���ã���ʣ����ʼ�д�����ݿ�
public:
	UG_LONG		saveMail();

//��ȡ�ʼ�����ʱ����
public:
	UG_LONG		filterMail(UG_PVOID pvPlayer); //�������κ��ѵ��ʼ�
	UG_LONG		setMail(UG_INT32 n32Playerid); //��ȡ���˺���ʼ�����
	UG_LONG		loadMail(UG_INT32 n32Playerid); //�����ݿ⵹���ʼ�

public:
	static UG_LONG		getMailCount(UG_INT32 n32Playerid); //��ȡû�й��˵��ʼ�����
	static UG_LONG		sqlWriteMail(CMailMsg* pMail); //д�ʼ�
	
protected:
	static UGLONG		sqlDelFlowMail(UGINT nOwnerid); //ɾ��������ʼ�

public:
	//���ڴ��е���һ���ʼ���������ݿ������Ҵ����ݿ���ɾ�������������ڴ�û���ʼ���
	//�����ݿ��е���(������ݿ����ʼ��Ļ�)��
	CMailMsg*	pop();
	UG_LONG		push(CMailMsg* pMail); //����һ���Ѿ����˵��ʼ�
	UG_LONG		size(); //Ŀǰ(�ѹ��˺�)���ʼ�����
	
protected:
	UG_LONG		filterMail(UG_INT32 n32Sendid,UG_INT32 n32Recvid);
	
private:
	UG_INT32			m_n32Playerid; //���id����resetʱ����
	UG_LONG				m_lSQLMailCount; //���˺���ʼ�����
	
private:
	list<CMailMsg*>		m_list;
	UG_LONG				m_lMaxCount; //�ʼ������
	UG_LONG				m_lSize; //�ڴ��ʼ���
	CMailMsg*			m_pMailMsg;
	CMailMsg			m_mailMsg;

};

#endif // !defined(AFX_MAILMSGMANAGER_H__88BB260B_00F4_4C51_89B7_900178BBB89E__INCLUDED_)
