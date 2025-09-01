/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : MailMsgManager.cpp
*owner        : Ben
*description  : ����Ϣ�����ࡣ
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
	m_lSQLMailCount = 0;
	m_n32Playerid = 0;
}

CMailMsgManager::~CMailMsgManager()
{
	cleanup();
}

UG_LONG CMailMsgManager::init(UG_LONG lMaxCount)
{
	m_list.clear();
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


/*************************************************************************************
Function:		pop	
Describe:		����һ���ʼ��������ͻ��ˣ����id !- 0�������ݿ���ɾ����
----------------------------------------------------------------------------------
parameter:		
Return:			UG_LONG 
Author:			Ben
Create Timer:	2005/01/04 10:56:00
Modify Timer:	
/************************************************************************************/
CMailMsg* CMailMsgManager::pop()
{
	if(m_lSize > 0) //��ʾĿǰ������ʼ�
	{
		//����
		Q_memcpy(&m_mailMsg,m_pMailMsg,sizeof(CMailMsg));
		Q_memmove(m_pMailMsg,m_pMailMsg + 1,sizeof(CMailMsg) * (m_lSize - 1));
		m_lSize --;
		m_lSQLMailCount --;

		static UG_CHAR szBuffer[1024];
		if(m_mailMsg.m_n32MailID) //id != 0��ʾ�ʼ������ݿ���
		{
			sprintf(szBuffer, "delete from `chatmail` where id = %d",m_mailMsg.m_n32MailID);
			CWorldbase* pWorldbase = theApp.m_pWorldbase;
			UG_PVOID pvResult = NULL;
			pWorldbase->m_pSQLChatMail->query(szBuffer,pvResult);
		}
		g_pLog->UGLog("pop ok.");
		if(!m_lSize) //�ڴ�û���ʼ���
		{
			if(m_lSQLMailCount > 0) //���ݿ⻹���ʼ�
			{
				loadMail(m_n32Playerid);
			}
		}
		return &m_mailMsg;
	}
	else
	{
		g_pLog->UGLog("pop error.");
		return NULL;
	}
}

/*************************************************************************************
Function:		push	
Describe:		����һ���ʼ������嵽�ڴ���id = 0���������ʼ�ʱ�Ͳ���ȥ���ݿ�ɾ�ˣ�
				����˳�ʱ��Ҫ������ʼ���push��ʱ���ʼ������ǹ��˹��ġ�
----------------------------------------------------------------------------------
parameter:		CMailMsg* pMail
Return:			UG_LONG 
Author:			Ben
Create Timer:	2005/01/04 10:56:00
Modify Timer:	
/************************************************************************************/
UG_LONG CMailMsgManager::push(CMailMsg* pMail)
{
	if(m_lSQLMailCount >= MAX_MAIL_MSG_COUNT)
	{
		g_pLog->UGLog("push mail error %d > %d.",m_lSQLMailCount,MAX_MAIL_MSG_COUNT);
		return -1;
	}
	if(m_lSize == m_lSQLMailCount)
	{
		if(m_lSize < m_lMaxCount)
		{
			pMail->m_n32MailID = 0; //id = 0��ʾ���ݿ���û�и��ʼ�
			Q_memcpy(m_pMailMsg + m_lSize,pMail,sizeof(CMailMsg));
			m_lSize ++;
			m_lSQLMailCount ++;
			return 0;
		}
	}
	sqlWriteMail(pMail);
	m_lSQLMailCount ++;
	return 0;
}

/*************************************************************************************
Function:		size	
Describe:		������ҵ�ǰ�ʼ�������
----------------------------------------------------------------------------------
parameter:		
Return:			UG_LONG 
Author:			Ben
Create Timer:	2005/01/04 10:56:00
Modify Timer:	
/************************************************************************************/
UG_LONG CMailMsgManager::size()
{
	return m_lSQLMailCount;
}

/*************************************************************************************
Function:		reset	
Describe:		����ҵ���ʱʹ�ã����������ʼ�����
----------------------------------------------------------------------------------
parameter:		UG_INT32 n32Playerid //���id
Return:			UG_LONG 
Author:			Ben
Create Timer:	2005/01/04 10:56:00
Modify Timer:	
/************************************************************************************/
UG_LONG CMailMsgManager::reset(UG_INT32 n32Playerid)
{
	m_lSize = 0;
	m_n32Playerid = n32Playerid;
	return 0;
}

/*************************************************************************************
Function:		loadMail	
Describe:		�����ݿ⵹������ʼ���ֻ������ڴ��б�û���ʼ�ʱ�ŵ��롣
----------------------------------------------------------------------------------
parameter:		UG_INT32 n32Playerid //���id
Return:			UG_LONG 
Author:			Ben
Create Timer:	2005/01/04 10:56:00
Modify Timer:	
/************************************************************************************/
UG_LONG CMailMsgManager::loadMail(UG_INT32 n32Playerid)
{
	if(m_lSize > 0) //����ڴ��б����û���ʼ�
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
			lRet = atoi(pchCount);
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
	return (UG_LONG)ulRows;
}

/*************************************************************************************
Function:		getMailCount	
Describe:		��ȡ����ʼ�������Ϊ��̬���������߷�������ҽԿ�
----------------------------------------------------------------------------------
parameter:		UG_INT32 n32Playerid //���id
Return:			UG_LONG 
Author:			Ben
Create Timer:	2005/01/04 10:56:00
Modify Timer:	
/************************************************************************************/
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
				sqlDelFlowMail(n32Playerid);
				lRet = MAX_MAIL_MSG_COUNT;
			}
		}
	}
	pWorldbase->m_pSQLChatMail->freeResult(pvResult);
	return lRet;
}

/*************************************************************************************
Function:		sqlWriteMail	
Describe:		д�ʼ������ݿ�
----------------------------------------------------------------------------------
parameter:		CMailMsg* pMail
Return:			UG_LONG 
Author:			Ben
Create Timer:	2005/01/04 10:56:00
Modify Timer:	
/************************************************************************************/
UG_LONG CMailMsgManager::sqlWriteMail(CMailMsg* pMail)
{
	CWorldbase* pWorldbase = (CWorldbase*)(theApp.m_pWorldbase);
	static UG_CHAR szBuffer[1024];
	sprintf(szBuffer,"insert into `chatmail` (ownerid,senderid,sendernickname,content,systimer) values (%d,%d,'%s','%s',%d)",pMail->m_n32RecvID,pMail->m_n32SendID,pMail->m_szSendNickName,pMail->m_szContent,pMail->m_lTimer);
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

/*************************************************************************************
Function:		saveMail	
Describe:		����˳�ʱ�ű����ʼ���������ʼ�������id = 0
----------------------------------------------------------------------------------
parameter:		
Return:			UG_LONG 
Author:			Ben
Create Timer:	2005/01/04 10:56:00
Modify Timer:	
/************************************************************************************/
UG_LONG CMailMsgManager::saveMail()
{
	for(UG_LONG l = 0; l < m_lSize; l ++)
	{
		if(!((m_pMailMsg + l)->m_n32MailID)) //ֻ��id = 0���ʼ�����û��д�����ݿ���ʼ�
		{
			sqlWriteMail((m_pMailMsg + l));
		}
	}
	m_lSize = 0;
	return 0;
}

/***************************************************************************************
Function:		filterMail	
Describe:		���˵�ǰ����ʼ����������εĺ���������ˣ�����������Ҫ�ʼ���������ʱ
				�ȹ����ʼ����ٻ�ȡ�ʼ�������������ʼ�(filterMail,setMail,loadMail)��
----------------------------------------------------------------------------------
parameter:		UG_PVOID pvPlayer���ָ��
Return:			UG_LONG 
Author:			Ben
Create Timer:	2005/01/04 10:56:00
Modify Timer:	
/**************************************************************************************/
UG_LONG CMailMsgManager::filterMail(UG_PVOID pvPlayer)
{
	CPlayer* pPlayer = (CPlayer*)pvPlayer;
	map<INT32,INT32>::iterator it;
	for(it = pPlayer->m_friends.m_mapFriends.begin(); it != pPlayer->m_friends.m_mapFriends.end(); it ++)
	{
		if(FRIEND_MASK == it->second) //���εĺ���
		{
			filterMail(it->first,pPlayer->m_n32Playerid);
		}
	}
	return 0;
}

/***************************************************************************************
Function:		filterMail	
Describe:		���˵�ǰ����ʼ��������ݿ��й��ˡ�
----------------------------------------------------------------------------------
parameter:		UG_INT32 n32Sendid, //���˵�����ҷ��͵��ʼ�
				UG_INT32 n32Recvid //���id
Return:			UG_LONG //0��ʾ��ȷ���������
Author:			Ben
Create Timer:	2005/01/04 10:56:00
Modify Timer:	
/**************************************************************************************/
UG_LONG CMailMsgManager::filterMail(UG_INT32 n32Sendid,UG_INT32 n32Recvid)
{
	static UG_CHAR szBuffer[1024];
	sprintf(szBuffer, "delete from `chatmail` where ownerid = %d and senderid = %d",n32Recvid,n32Sendid);
	CWorldbase* pWorldbase = theApp.m_pWorldbase;
	UG_PVOID pvResult = NULL;
	pWorldbase->m_pSQLChatMail->query(szBuffer,pvResult);
	pWorldbase->m_pSQLChatMail->freeResult(pvResult);
	return 0;
}

/***************************************************************************************
Function:		setMail	
Describe:		���õ�ǰ��ҵ��ʼ�����������������Ҫ�ʼ���������ʱ�ȹ����ʼ�����
				��ȡ�ʼ�������������ʼ�(filterMail,setMail,loadMail)��
----------------------------------------------------------------------------------
parameter:		UG_INT32 n32Playerid
Return:			UG_LONG //�ʼ�����
Author:			Ben
Create Timer:	2005/01/04 10:56:00
Modify Timer:	
/**************************************************************************************/
UG_LONG CMailMsgManager::setMail(UG_INT32 n32Playerid)
{
	m_lSQLMailCount = CMailMsgManager::getMailCount(n32Playerid);
	return m_lSQLMailCount;
}

UGLONG CMailMsgManager::sqlDelFlowMail(UGINT nOwnerid) //ɾ��������ʼ�
{
	static UG_CHAR szBuffer[1024];
	sprintf(szBuffer,"select id	from `chatmail` where ownerid = %d limit %d, 1",nOwnerid,MAX_MAIL_MSG_COUNT);
	CWorldbase* pWorldbase = theApp.m_pWorldbase;
	UG_PVOID pvResult = NULL;
	UG_ULONG ulRows = 0;
	pWorldbase->m_pSQLChatMail->query(szBuffer,pvResult,ulRows);
	if(ulRows > 0)
	{
		UG_PCHAR pchID = NULL;
		pWorldbase->m_pSQLChatMail->getQueryResult(pvResult,0,0,pchID);
		if(pchID)
		{
			sprintf(szBuffer,"delete from chatmail where id > %d and ownerid = %d",atoi(pchID),nOwnerid);
			pWorldbase->m_pSQLChatMail->query(szBuffer,pvResult,ulRows);
		}
	}
	pWorldbase->m_pSQLChatMail->freeResult(pvResult);
	return 0;
}
