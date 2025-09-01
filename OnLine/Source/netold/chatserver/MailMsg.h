/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : MailMsg.h
*owner        : Ben
*description  : ����Ϣ���ʼ��Ľṹ�������ʾ��
*modified     : 2004/12/20
******************************************************************************/ 

#if !defined(AFX_MAILMSG_H__83E37426_D4E9_4F83_B7B9_15D0C48B13F9__INCLUDED_)
#define AFX_MAILMSG_H__83E37426_D4E9_4F83_B7B9_15D0C48B13F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMailMsg  
{
public:
	CMailMsg();
	virtual ~CMailMsg();

public:
	UG_INT32	m_n32MailID; //�������ݿ��е�id�ţ�idΪ0ʱ��ʾ��û�д������ݿ���
	UG_INT32	m_n32SendID; //�����ߵ�id
	UG_INT32	m_n32RecvID; //�����ߵ�id
	UG_LONG		m_lTimer; //���͵�ʱ�䣬Ϊ�����������ʱ��
	UG_CHAR		m_szSendNickName[MAX_NAME_LENGTH + 1]; //�������ǳ�
	UG_CHAR		m_szContent[MAX_CHAT_LENGTH + 1]; //����Ϣ����
	
};

#endif // !defined(AFX_MAILMSG_H__83E37426_D4E9_4F83_B7B9_15D0C48B13F9__INCLUDED_)
