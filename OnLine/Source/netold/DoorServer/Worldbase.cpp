// Worldbase.cpp: implementation of the CWorldbase class.
//
//////////////////////////////////////////////////////////////////////

#include "incall.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWorldbase::CWorldbase()
{
	m_pvConnect = NULL;
	m_dwTimer = 0;
	m_bOnline = FALSE;
	m_dwPrivateIP = 0;
	m_dwPrivatePort = 0;
	m_dwCurrentMaxOnline = 0;
	m_dwCheckID = 0;
	m_stepWB = WB_NOT_CONNECTED;
	m_pParent = NULL;
	Q_memset(&m_wbData,0,sizeof(WORLD_BASE));
}

CWorldbase::~CWorldbase()
{

}

UG_LONG CWorldbase::dispatchMsg(UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR pchData,UG_ULONG ulLen)
{
	switch(dwMsgID)
	{
	case NET_MSG_DOOR_WORLD:
		{
			if(DOOR_WORLD_ADD_USER == dwParam)
			{
				try
				{
					addUser(1);
				}
				catch(...)
				{
					g_pLog->UGLog("addUser function error.");
				}
			}
			else if(DOOR_WORLD_DROP_USER == dwParam)
			{
				try
				{
					dropUser(1);
				}
				catch(...)
				{
					g_pLog->UGLog("dropUser function error.");
				}
			}
			else
			{
				g_pLog->UGLog("NET_MSG_DOOR_WORLD user count error type is error.");
			}
		}
		break;
	case NET_MSG_CHECK_USER:
		{
			if(sizeof(MW_ACCOUNT_ID) == ulLen)
			{
				try
				{
					static MW_ACCOUNT_ID account;
					Q_memcpy(&account,pchData,sizeof(MW_ACCOUNT_ID));
					checkUser(account);
				}
				catch(...)
				{
					g_pLog->UGLog("checkUser function error.");
				}
			}
			else
			{
				g_pLog->UGLog("check user error because ulLen = %d != sizeof(MW_ACCOUNT_ID) = %d.",ulLen,sizeof(MW_ACCOUNT_ID));
			}
		}
		break;
	case NET_MSG_CHECK_DATA: //worldbase 校验信息
		{
			try
			{
				checkConnect(dwParam);
			}
			catch(...)
			{
				g_pLog->UGLog("checkConnect function error.");
			}
		}
		break;
	default:
		{
			g_pLog->UGLog("worldbase message id = %d, param = %d. this is default.",dwMsgID,dwParam);
		}
	}
	return 0;
}

UG_LONG CWorldbase::checkUser(MW_ACCOUNT_ID& account)
{
	static UG_INT32 n32CheckUserr = 0;
	n32CheckUserr += 1;
	g_pLog->UGLog("checkUser num = %d.",n32CheckUserr);
	
	CWorldbaseManager* pWBM = (CWorldbaseManager*)m_pParent;
	static UG_CHAR szBuffer[1024];
	static UG_CHAR pchPwd[33]; //MD5加密后为32字节
	account.name[sizeof(account.name) - 1] = '\0';
	account.passwd[sizeof(account.passwd) - 1] = '\0';
	g_pLog->UGLog("check name = %s, pwd = %s",account.name,account.passwd);
	CUGEncrypt::MD5Encrypt(account.passwd,pchPwd);
	pchPwd[32] = '\0';
	g_pLog->UGLog("check md5 pwd = %s",pchPwd);
	sprintf(szBuffer,SQL_CHECK_USER,account.name);
	UG_PVOID pvResult = NULL;
	UG_ULONG ulRet = pWBM->m_pSQLBilling->query(szBuffer,pvResult);
	if(-1 == ulRet)//no user
	{
		g_pLog->UGLog("query(%s) error, error is %s.",szBuffer,pWBM->m_pSQLBilling->getError());
		pWBM->m_pWorldbaseTCPComm->sendData(m_pvConnect,NET_MSG_CHECK_USER_RETURN,CHECK_USER_ERROR,UG_PCHAR(&(account.player_id)),sizeof(int));
	}
	else
	{
		UG_PCHAR pchRes = NULL;
		pWBM->m_pSQLBilling->getQueryResult(pvResult,0,0,pchRes);
		if(pchRes)
		{
			if(!strcmp(pchPwd,pchRes))
			{
				pWBM->m_pSQLBilling->getQueryResult(pvResult,0,1,pchRes);
				MW_BILLING nb;
				nb.billing_id = atoi(pchRes);
				pWBM->m_pSQLBilling->getQueryResult(pvResult,0,2,pchRes);
				nb.money = atoi(pchRes);
				nb.player_id = account.player_id;
				pWBM->m_pWorldbaseTCPComm->sendData(m_pvConnect,NET_MSG_CHECK_USER_RETURN,CHECK_USER_OK,(UG_PCHAR)(&nb),sizeof(MW_BILLING));
				g_pLog->UGLog("check user is ok.");
			}
			else //Not Pwd
			{
				pWBM->m_pWorldbaseTCPComm->sendData(m_pvConnect,NET_MSG_CHECK_USER_RETURN,CHECK_PWD_NOT,UG_PCHAR(&(account.player_id)),sizeof(int));
				g_pLog->UGLog("check user is password error");
			}
		}
		else //no user
		{
			pWBM->m_pWorldbaseTCPComm->sendData(m_pvConnect,NET_MSG_CHECK_USER_RETURN,CHECK_USER_NOT,UG_PCHAR(&(account.player_id)),sizeof(int));
			g_pLog->UGLog("check user is user error");
		}
	}
	pWBM->m_pSQLBilling->freeResult(pvResult);
	return 0;
}

/***************************************************************************************
Function:		addUser	
Describe:		增加用户，写入数据库
----------------------------------------------------------------------------------
parameter:		CWorldbase* p, //用户信息指针
				UG_ULONG ulCount //增加个数
Return:			UG_ULONG //0表示正确，否则错误
Author:			Ben
Create Timer:	2004/12/29 17:56:00
Modify Timer:	
/**************************************************************************************/
UG_LONG CWorldbase::addUser(UG_ULONG ulCount)
{
	static UG_INT32 n32AddUser = 0;
	n32AddUser += (UG_INT32)ulCount;
	g_pLog->UGLog("addUser num = %d.",n32AddUser);
	
	CWorldbaseManager* pWBM = (CWorldbaseManager*)m_pParent;
	static UG_CHAR szBuffer[1024];
	m_wbData.ulOnlineCount += ulCount;
	if(m_dwCurrentMaxOnline < m_wbData.ulOnlineCount)
	{
		if(m_wbData.ulHistoryMaxCount < m_wbData.ulOnlineCount)
		{
			g_pLog->UGLog("HistoryMaxCount = %d.",m_wbData.ulHistoryMaxCount);
			m_wbData.ulHistoryMaxCount = m_wbData.ulOnlineCount;
			sprintf(szBuffer,SQL_SET_MAX_NO_ONLINE,m_wbData.ulHistoryMaxCount,m_wbData.ulID);
			UG_PVOID pvResult = NULL;
			pWBM->m_pSQLDoorServer->query(szBuffer,pvResult);
			pWBM->m_pSQLDoorServer->freeResult(pvResult);
		}
		m_dwCurrentMaxOnline = m_wbData.ulOnlineCount;
	}
	g_pLog->UGLog("online num = %d.",m_wbData.ulOnlineCount);
	sprintf(szBuffer,SQL_ADD_USER,m_wbData.ulOnlineCount,m_wbData.ulID);
	UG_PVOID pvResult = NULL;
	UG_ULONG ulRet = pWBM->m_pSQLDoorServer->query(szBuffer,pvResult);
	pWBM->m_pSQLDoorServer->freeResult(pvResult);
	if(ulRet)
	{
		g_pLog->UGLog("add uses error query(%s) error, error is %s.",szBuffer,pWBM->m_pSQLDoorServer->getError());
	}
	return 0;
}

/***************************************************************************************
Function:		dropUser	
Describe:		减少用户，写入数据库
----------------------------------------------------------------------------------
parameter:		CWorldbase* p, //用户信息指针
				UG_ULONG ulCount //减少个数
Return:			UG_ULONG //0表示正确，否则错误
Author:			Ben
Create Timer:	2004/12/29 17:56:00
Modify Timer:	
/**************************************************************************************/
UG_LONG CWorldbase::dropUser(UG_ULONG ulCount)
{
	static UG_INT32 n32DropUser = 0;
	n32DropUser += (UG_INT32)ulCount;
	g_pLog->UGLog("dropUser num = %d.",n32DropUser);
	
	CWorldbaseManager* pWBM = (CWorldbaseManager*)m_pParent;
	if(m_wbData.ulOnlineCount >= ulCount)
	{
		m_wbData.ulOnlineCount -= ulCount;
	}
	else
	{
		g_pLog->UGLog("drop error because current num = %d < drop num = %d.",m_wbData.ulOnlineCount,ulCount);
	}
	static UG_CHAR szBuffer[1024];
	sprintf(szBuffer,SQL_DROP_USER,m_wbData.ulOnlineCount,m_wbData.ulID);
	UG_PVOID pvResult = NULL;
	UG_ULONG ulRet = pWBM->m_pSQLDoorServer->query(szBuffer,pvResult);
	pWBM->m_pSQLDoorServer->freeResult(pvResult);
	if(ulRet)
	{
		g_pLog->UGLog("drop uses error query(%s) error, error is %s.",szBuffer,pWBM->m_pSQLDoorServer->getError());
	}
	return 0;
}

/***************************************************************************************
Function:		checkConnect	
Describe:		校验连接是否合法
----------------------------------------------------------------------------------
parameter:		PNET_SERVER_PARAM p连接参数
				UG_ULONG ulCount //减少个数
Return:			UG_ULONG //0表示正确，否则错误
Author:			Ben
Create Timer:	2004/12/29 17:56:00
Modify Timer:	
/**************************************************************************************/
UG_LONG CWorldbase::checkConnect(UG_DWORD dwParam)
{
	CWorldbaseManager* pWBM = (CWorldbaseManager*)m_pParent;
	static UG_CHAR szBuffer[1024];
	sprintf(szBuffer,SQL_GET_WORLDBASE,pWBM->m_pWorldbaseTCPComm->convertIP(m_dwPrivateIP),dwParam); //查数据库是否有该IP存在
	UG_PVOID pvResult = NULL;
	UG_ULONG ulCount = 0;
	UG_ULONG ulRet = pWBM->m_pSQLDoorServer->query(szBuffer,pvResult,ulCount);
	if(-1 == ulRet)
	{
		m_bOnline = FALSE;
		g_pLog->UGLog("query(%s) check connect error.",szBuffer);
	}
	else
	{
		if(1 == ulCount) //is worldbase server
		{
			m_dwCheckID = dwParam;
			UG_PCHAR pchData = NULL;
			pWBM->m_pSQLDoorServer->getQueryResult(pvResult,0,0,pchData);
			if(pchData)
			{
				Q_strncpy(m_wbData.szName,pchData,WORLD_BASE_NAME_SIZE + 1);
			}
			else
			{
				g_pLog->UGLog("get worldbase name is null.");
			}
			pWBM->m_pSQLDoorServer->getQueryResult(pvResult,0,1,pchData);
			m_wbData.ulID = atoi(pchData);
			pWBM->m_pSQLDoorServer->getQueryResult(pvResult,0,2,pchData);
			m_wbData.dwIP = inet_addr(pchData);
			pWBM->m_pSQLDoorServer->getQueryResult(pvResult,0,3,pchData);
			m_wbData.dwPort = atoi(pchData);
			pWBM->m_pSQLDoorServer->getQueryResult(pvResult,0,5,pchData);
			m_wbData.ulHistoryMaxCount = atoi(pchData);
			m_wbData.ulCPU = 0;
			m_wbData.ulOnlineCount = 0;
			m_bOnline = TRUE;
			m_dwTimer = Plat_MSTime();
			m_dwCurrentMaxOnline = 0;
			m_stepWB = WB_CHECK_OK;
			pWBM->m_pSQLDoorServer->freeResult(pvResult);
			//更新数据库纪录
			sprintf(szBuffer,SQL_UPDATE_ONLINE,m_wbData.ulID);
			pWBM->m_pSQLDoorServer->query(szBuffer,pvResult);
			pWBM->m_pWorldbaseTCPComm->sendData(m_pvConnect,NET_MSG_CHECK_DATA,0);
			g_pLog->UGLog("worldbase connect ok ip = %s,port = %d.",pWBM->m_pWorldbaseTCPComm->convertIP(m_dwPrivateIP),m_dwPrivatePort);
		}
		else
		{
			m_bOnline = FALSE;
			g_pLog->UGLog("worldbase connect failed ip = %s,port = %d, check id = %d.",pWBM->m_pWorldbaseTCPComm->convertIP(m_dwPrivateIP),m_dwPrivatePort,dwParam);
		}
	}
	return 0;
}

UG_LONG CWorldbase::cout(FILE* p)
{
	CWorldbaseManager* pWBM = (CWorldbaseManager*)m_pParent;
	fprintf(p,"beonline = %d, checkid = %d, step = %d.\n",m_bOnline,m_dwCheckID,m_stepWB);
	fprintf(p,"CurrentMaxOnline = %d, login timer = %d, ip = %s, port = %d.\n",m_dwCurrentMaxOnline,m_dwTimer,pWBM->m_pWorldbaseTCPComm->convertIP(m_dwPrivateIP),m_dwPrivatePort);
	fprintf(p,"name = %s, history = %d, online = %d.\n",m_wbData.szName,m_wbData.ulHistoryMaxCount,m_wbData.ulOnlineCount);
	fprintf(p,"id = %d, ip = %s, port = %d, cpu = %d.\n",m_wbData.ulID,pWBM->m_pWorldbaseTCPComm->convertIP(m_wbData.dwIP),m_wbData.dwPort,m_wbData.ulCPU);
	fprintf(p,"\n");
	return 0;
}
