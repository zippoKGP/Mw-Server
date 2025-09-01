/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : MailMsg.h
*owner        : Ben
*description  : 短消息或邮件的结构，用类表示。
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
	UG_INT32	m_n32MailID; //存在数据库中的id号，id为0时表示还没有存入数据库中
	UG_INT32	m_n32SendID; //发送者的id
	UG_INT32	m_n32RecvID; //接收者的id
	UG_LONG		m_lTimer; //发送的时间，为聊天服务器的时间
	UG_CHAR		m_szSendNickName[MAX_NAME_LENGTH + 1]; //发送者昵称
	UG_CHAR		m_szContent[MAX_CHAT_LENGTH + 1]; //短消息内容
	
};

#endif // !defined(AFX_MAILMSG_H__83E37426_D4E9_4F83_B7B9_15D0C48B13F9__INCLUDED_)
