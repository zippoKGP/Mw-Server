/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : MailMsgManager.h
*owner        : Ben
*description  : 短消息管理类。
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

//玩家login时调用，清空该结构以供新玩家使用
public:
	UG_LONG		reset(UG_INT32 n32Playerid);

//logout时调用，将剩余的邮件写入数据库
public:
	UG_LONG		saveMail();

//获取邮件个数时进行
public:
	UG_LONG		filterMail(UG_PVOID pvPlayer); //过滤屏蔽好友的邮件
	UG_LONG		setMail(UG_INT32 n32Playerid); //获取过滤后的邮件总数
	UG_LONG		loadMail(UG_INT32 n32Playerid); //从数据库倒入邮件

public:
	static UG_LONG		getMailCount(UG_INT32 n32Playerid); //获取没有过滤的邮件总数
	static UG_LONG		sqlWriteMail(CMailMsg* pMail); //写邮件
	
protected:
	static UGLONG		sqlDelFlowMail(UGINT nOwnerid); //删除溢出的邮件

public:
	//从内存中弹出一封邮件，如果数据库有则并且从数据库中删除，弹完后如果内存没有邮件则
	//从数据库中导入(如果数据库有邮件的话)。
	CMailMsg*	pop();
	UG_LONG		push(CMailMsg* pMail); //插入一封已经过滤的邮件
	UG_LONG		size(); //目前(已过滤后)的邮件总数
	
protected:
	UG_LONG		filterMail(UG_INT32 n32Sendid,UG_INT32 n32Recvid);
	
private:
	UG_INT32			m_n32Playerid; //玩家id，在reset时设置
	UG_LONG				m_lSQLMailCount; //过滤后的邮件总数
	
private:
	list<CMailMsg*>		m_list;
	UG_LONG				m_lMaxCount; //邮件最大数
	UG_LONG				m_lSize; //内存邮件数
	CMailMsg*			m_pMailMsg;
	CMailMsg			m_mailMsg;

};

#endif // !defined(AFX_MAILMSGMANAGER_H__88BB260B_00F4_4C51_89B7_900178BBB89E__INCLUDED_)
