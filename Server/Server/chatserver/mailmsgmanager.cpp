/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : MailMsgManager.cpp
*owner        : Ben
*description  : 短消息管理类。
*modified     : 2004/12/20
******************************************************************************/ 

#include "incall.h"
extern CWorldbaseManager		theApp;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMailMsgManager::CMailMsgManager()
{
	m_lSize = 0;
	m_lMaxCount = 0;
	m_pMailMsg = NULL;
	m_n32Playerid = 0;
}

CMailMsgManager::~CMailMsgManager()
{
	cleanup();
}

UG_LONG CMailMsgManager::init(UG_LONG lMaxCount)
{
	m_lMaxCount = lMaxCount;
	if(m_lMaxCount <= 0)
	{
		return -1;
	}
	m_pMailMsg = new CMailMsg[m_lMaxCount];
	m_lSize = 0;
	return 0;
}

UG_LONG CMailMsgManager::cleanup()
{
	ReleasePA<CMailMsg*>(m_pMailMsg);
	return 0;
}


CMailMsg* CMailMsgManager::pop()
{
	if(m_lSize > 0) //表示目前玩家有邮件
	{
		//弹出
		Q_memcpy(&m_mailMsg,m_pMailMsg,sizeof(CMailMsg));
		Q_memmove(m_pMailMsg,m_pMailMsg + 1,sizeof(CMailMsg) * (m_lSize - 1));
		m_lSize --;
		return &m_mailMsg;
	}
	else
	{
		g_pLog->UGLog("pop error mail count = %d, player id = %d.",m_lSize,m_n32Playerid);
		return NULL;
	}
}

UG_LONG CMailMsgManager::push(CMailMsg* pMail)
{
	if(m_lSize < m_lMaxCount)
	{
		Q_memcpy(m_pMailMsg + m_lSize,pMail,sizeof(CMailMsg));
		m_lSize ++;
		return 0;
	}
	return -1;
}

UG_LONG CMailMsgManager::size()
{
	return m_lSize;
}

UG_LONG CMailMsgManager::reset(UG_INT32 n32Playerid)
{
	m_n32Playerid = n32Playerid;
	return 0;
}

UG_LONG CMailMsgManager::loadMail(UG_INT32 n32Playerid)
{
	if(m_lSize > 0) //玩家内存列表必须没有邮件
	{
		return -1;
	}
	UG_LONG lRet = 0;
	static UG_CHAR szBuffer[1024];
	sprintf(szBuffer, "select * from `chatmail` where ownerid = %d order by id limit %d",n32Playerid,m_lMaxCount);
	CWorldbase* pWorldbase = theApp.m_pWorldbase;
	UG_PVOID pvResult = NULL;
	UG_ULONG ulRows = 0;
	pWorldbase->m_pSQLChatMail->query(szBuffer,pvResult,ulRows);
	for(UG_ULONG l = 0; l < ulRows; l ++)
	{
		UG_PCHAR pchCount = NULL;
		pWorldbase->m_pSQLChatMail->getQueryResult(pvResult,l,0,pchCount);
		if(pchCount)
		{
			lRet = atoi(pchCount);
			(m_pMailMsg + m_lSize)->m_n32MailID = lRet;
		}
		pWorldbase->m_pSQLChatMail->getQueryResult(pvResult,l,1,pchCount);
		if(pchCount)
		{
			lRet = atoi(pchCount);
			(m_pMailMsg + m_lSize)->m_n32RecvID = n32Playerid;
		}
		pWorldbase->m_pSQLChatMail->getQueryResult(pvResult,l,2,pchCount);
		if(pchCount)
		{
			lRet = atoi(pchCount);
			(m_pMailMsg + m_lSize)->m_n32SendID = lRet;
		}
		pWorldbase->m_pSQLChatMail->getQueryResult(pvResult,l,3,pchCount);
		if(pchCount)
		{
			Q_strncpy((m_pMailMsg + m_lSize)->m_szSendNickName,pchCount,min(MAX_NAME_LENGTH + 1,Q_strlen(pchCount) + 1));
		}
		pWorldbase->m_pSQLChatMail->getQueryResult(pvResult,l,4,pchCount);
		if(pchCount)
		{
			Q_strncpy((m_pMailMsg + m_lSize)->m_szContent,pchCount,min(MAX_CHAT_LENGTH + 1,Q_strlen(pchCount) + 1));
		}
		pWorldbase->m_pSQLChatMail->getQueryResult(pvResult,l,5,pchCount);
		if(pchCount)
		{
			lRet = atoi(pchCount);
			(m_pMailMsg + m_lSize)->m_lTimer = lRet;
		}
		m_lSize ++;
	}
	pWorldbase->m_pSQLChatMail->freeResult(pvResult);
	
	sprintf(szBuffer, "delete from `chatmail` where ownerid = %d",n32Playerid);
	pvResult = NULL;
	pWorldbase->m_pSQLChatMail->query(szBuffer,pvResult);
	pWorldbase->m_pSQLChatMail->freeResult(pvResult);
	return (UG_LONG)ulRows;
}

UG_LONG CMailMsgManager::sqlWriteMail(CMailMsg* pMail)
{
	CWorldbase* pWorldbase = (CWorldbase*)(theApp.m_pWorldbase);
	UGCHAR szNick[1024];
	UGCHAR szContent[1024];
	UG_CHAR szBuffer[1024];
	
	pWorldbase->m_pSQLChatMail->convertString(szNick,pMail->m_szSendNickName);
	pWorldbase->m_pSQLChatMail->convertString(szContent,pMail->m_szContent);

	sprintf(szBuffer,"insert into `chatmail` (ownerid,senderid,sendernickname,content,systimer) values (%d,%d,\'%s\',\'%s\',%d)",pMail->m_n32RecvID,pMail->m_n32SendID,szNick,szContent,pMail->m_lTimer);
	UG_PVOID pvResult = NULL;
	UG_ULONG ulRows = 0;
	UG_ULONG ulRet = pWorldbase->m_pSQLChatMail->query(szBuffer,pvResult,ulRows);
	if(ulRet)
	{
		g_pLog->UGLog("sqlWriteMail failed, query(%s).",szBuffer);
	}
	pWorldbase->m_pSQLChatMail->freeResult(pvResult);
	return 0;
}
 
UG_LONG CMailMsgManager::saveMail()
{
	for(UG_LONG l = 0; l < m_lSize; l ++)
	{
		sqlWriteMail((m_pMailMsg + l));
	}
	m_lSize = 0;
	return 0;
}

UG_LONG CMailMsgManager::filterMail(UG_PVOID pvPlayer)
{
	CPlayer* pPlayer = (CPlayer*)pvPlayer;
	map<INT32,INT32>::iterator it;
	for(it = pPlayer->m_friends.m_mapFriends.begin(); it != pPlayer->m_friends.m_mapFriends.end(); it ++)
	{
		if(FRIEND_MASK == it->second) //屏蔽的好友
		{
			filterMail(it->first,pPlayer->m_n32Playerid);
		}
	}
	return 0;
}
 
UG_LONG CMailMsgManager::filterMail(UG_INT32 n32Sendid,UG_INT32 n32Recvid)
{
	for(UGLONG l = 0; l < m_lSize;)
	{
		CMailMsg* pMail = m_pMailMsg + l;
		if((pMail->m_n32RecvID == n32Recvid) && (pMail->m_n32SendID == n32Sendid))
		{
			Q_memmove(m_pMailMsg + l,m_pMailMsg + l + 1,m_lSize - l - 1);
			m_lSize --;
		}
		else
		{
			l ++;
		}
	}
	return 0;
}

UG_LONG CMailMsgManager::getMailCount(UG_INT32 n32Playerid)
{
	UG_LONG lRet = 0;
	static UG_CHAR szBuffer[1024];
	sprintf(szBuffer,"select count(id) as num from chatmail where ownerid = %d",n32Playerid);
	CWorldbase* pWorldbase = theApp.m_pWorldbase;
	UG_PVOID pvResult = NULL;
	UG_ULONG ulRows = 0;
	pWorldbase->m_pSQLChatMail->query(szBuffer,pvResult,ulRows);
	if(ulRows > 0)
	{
		UG_PCHAR pchCount = NULL;
		pWorldbase->m_pSQLChatMail->getQueryResult(pvResult,0,0,pchCount);
		if(pchCount)
		{
			lRet = atoi(pchCount);
			if(lRet > MAX_MAIL_MSG_COUNT)
			{
				g_pLog->UGLog("getMailCount = %d > %d, ownerid = %d.",lRet,MAX_MAIL_MSG_COUNT,n32Playerid);
			}
		}
	}
	pWorldbase->m_pSQLChatMail->freeResult(pvResult);
	return lRet;
}
