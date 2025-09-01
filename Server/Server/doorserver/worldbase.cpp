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
	m_mapMoneyLog.clear();
	m_dwLoginCount = 0;
	m_dwLogoutCount = 0;
	m_dwLastRecvDataTimer = 0;
#ifdef JP_VERSION
	m_nServerCode = 0;
	memset(m_szPortalID,0,3);
#endif
}

CWorldbase::~CWorldbase()
{
	m_mapMoneyLog.clear();
}

UG_LONG CWorldbase::dispatchMsg(UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR pchData,UG_ULONG ulLen)
{
	m_dwLastRecvDataTimer = theApp.m_pWorldbaseManager->m_ulRenderTimer;
	switch(dwMsgID)
	{
	case NET_MSG_DROP_MONEY:
		{
			if(ulLen == sizeof(MW_BILLING))
			{
				if(BEGIN_DROP == dwParam)
				{
					dropMoney((MW_BILLING*)pchData);
				}
				else if(DROP_MONEY == dwParam)
				{
					dropMoney((MW_BILLING*)pchData);
				}
				else if(SAVE_MONEY == dwParam)
				{
					saveMoney((MW_BILLING*)pchData);
				}
				else
				{
					g_pLog->UGLog("NET_MSG_DROP_MONEY error param is error %d, %s.",dwParam,m_wbData.szName);
				}
			}
			else
			{
				g_pLog->UGLog("NET_MSG_DROP_MONEY error, sizeof(MW_BILLING) != %d, %s",ulLen,m_wbData.szName);
			}
		}
		break;
	case NET_MSG_DOOR_WORLD_PLAYER_NUMBER:
		{
			setUserNum(UGINT(dwParam));
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
					g_pLog->UGLog("checkUser function error, %s.",m_wbData.szName);
				}
			}
			else
			{
				g_pLog->UGLog("check user error because ulLen = %d != sizeof(MW_ACCOUNT_ID) = %d, %s.",ulLen,sizeof(MW_ACCOUNT_ID),m_wbData.szName);
			}
			break;
		}
	case NET_MSG_GM_MONITOR:
		{
			if(ulLen == sizeof(GM_MONITOR))
			{
				logGMMonitor((GM_MONITOR*)pchData);
			}
			else
			{
				g_pLog->UGLog("NET_MSG_GM_MONITOR log error, data size = %d != sizeof(GM_MONITOR) = %d.",ulLen,sizeof(GM_MONITOR));
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
				g_pLog->UGLog("checkConnect function error, %s.",m_wbData.szName);
			}
		}
		break;
	case NET_MSG_SET_ACTIVE_TIME:
		{
			if(ulLen == sizeof(int))
			{
				long lCur = 0;
				time(&lCur);
				char szBuffer[1024];
				sprintf(szBuffer,SQL_SET_ACTIVE_TIME_UPDATE,*((int*)pchData) + lCur,dwParam);
				UGPVOID pvResult = NULL;
				UGDWORD dwRet = queryBilling(szBuffer,pvResult);
				freeResultBilling(pvResult);
				if(dwRet)
				{
					sendWBData(NET_MSG_SET_ACTIVE_TIME_RETURN,-1,NULL,0);
				}
				else
				{
					sendWBData(NET_MSG_SET_ACTIVE_TIME_RETURN,0,NULL,0);
				}
			}
			else
			{
				sendWBData(NET_MSG_SET_ACTIVE_TIME_RETURN,-1,NULL,0);
			}
		}
	case NET_MSG_USERCARD_CHECK:
		{
			if(ulLen == sizeof(USERCARD))
			{
				checkUsercard((USERCARD*)pchData);
			}
			else
			{
				g_pLog->UGLog("NET_MSG_USERCARD_CHECK error ulLen = %d, %s.",ulLen,m_wbData.szName);
			}
		}
		break;
	case NET_MSG_USERCARD_UNLOCKED:
		{
			if(ulLen == sizeof(USERCARD))
			{
				unlockUsercard((USERCARD*)pchData);
			}
			else
			{
				g_pLog->UGLog("NET_MSG_USERCARD_UNLOCKED error ulLen = %d, %s.",ulLen,m_wbData.szName);
			}
		}
		break;
	case NET_MSG_USERCARD_DEL:
		{
			if(ulLen == sizeof(USERCARD))
			{
				delUsercard((USERCARD*)pchData);
			}
			else
			{
				g_pLog->UGLog("NET_MSG_USERCARD_DEL error ulLen = %d, %s.",ulLen,m_wbData.szName);
			}
		}
		break;
	default:
		{
		}
	}
	return 0;
}

UG_LONG CWorldbase::checkUser(MW_ACCOUNT_ID& account)
{
	static UG_INT32 n32CheckUserr = 0;
	n32CheckUserr += 1;
	
	static UG_CHAR szBuffer[1024];
	static UG_CHAR pchPwd[33]; //MD5加密后为32字节
	account.name[sizeof(account.name) - 1] = '\0';
	account.passwd[sizeof(account.passwd) - 1] = '\0';
	g_pLog->UGLog("check name = %s, pwd = %s, %s",account.name,account.passwd,m_wbData.szName);
	strcpy(pchPwd,account.passwd);
	pchPwd[32] = '\0';
	sprintf(szBuffer,SQL_CHECK_USER,account.name);
	UG_PVOID pvResult = NULL;
	UGLONG lRet = queryBilling(szBuffer,pvResult);
	if(-1 == lRet)//no user
	{
		sendWBData(NET_MSG_CHECK_USER_RETURN,CHECK_USER_NOT,UG_PCHAR(&(account.player_id)),sizeof(int));
		g_pLog->UGLog("check user error, %s.",m_wbData.szName);
	}
	else
	{
		UG_PCHAR pchRes = NULL;
		getQueryResultBilling(pvResult,0,0,pchRes);
		if(pchRes)
		{
			if(!strcmp(pchPwd,pchRes))
			{
				getQueryResultBilling(pvResult,0,3,pchRes);
				if(pchRes)
				{
					BOOL bActivate = atoi(pchRes);
					if(bActivate)
					{
						getQueryResultBilling(pvResult,0,4,pchRes);
						long lCur = 0;
						time(&lCur);
						if(lCur < atoi(pchRes))
						{
							sendWBData(NET_MSG_CHECK_USER_RETURN,CHECK_USER_CANNOT_USE,UG_PCHAR(&(account.player_id)),sizeof(int));
							g_pLog->UGLog("check user is error, can not use line = %d, %s.",__LINE__,m_wbData.szName);
						}
						else
						{
							getQueryResultBilling(pvResult,0,1,pchRes);
							MW_BILLING nb;
							nb.billing_id = atoi(pchRes);
							getQueryResultBilling(pvResult,0,2,pchRes);
							int nMoney = getMoney(nb.billing_id);
							if(nMoney >= theApp.m_config.m_nMoney)
							{
								nb.money = moneyToTime(theApp.m_config.m_nMoney);
								dropMoney(nb.billing_id,theApp.m_config.m_nMoney);
							}
							else
							{
								nb.money = 0;
							}
							nb.player_id = account.player_id;
							sendWBData(NET_MSG_CHECK_USER_RETURN,CHECK_USER_OK,(UG_PCHAR)(&nb),sizeof(MW_BILLING));
						}
					}
					else
					{
						sendWBData(NET_MSG_CHECK_USER_RETURN,CHECK_USER_NOT_ACTIVE,UG_PCHAR(&(account.player_id)),sizeof(int));
						g_pLog->UGLog("check user is error, not activate line = %d, %s.",__LINE__,m_wbData.szName);
					}
				}
				else
				{
					sendWBData(NET_MSG_CHECK_USER_RETURN,CHECK_USER_NOT_ACTIVE,UG_PCHAR(&(account.player_id)),sizeof(int));
					g_pLog->UGLog("check user is error, not activate = %d, %s.",__LINE__,m_wbData.szName);
				}
			}
			else //Not Pwd
			{
				sendWBData(NET_MSG_CHECK_USER_RETURN,CHECK_PWD_NOT,UG_PCHAR(&(account.player_id)),sizeof(int));
				g_pLog->UGLog("check user is password error, %s",m_wbData.szName);
			}
		}
		else //no user
		{
			sendWBData(NET_MSG_CHECK_USER_RETURN,CHECK_USER_NOT,UG_PCHAR(&(account.player_id)),sizeof(int));
			g_pLog->UGLog("check user is user error, %s",m_wbData.szName);
		}
	}
	freeResultBilling(pvResult);
	return 0;
}

UGLONG CWorldbase::setUserNum(UGINT nCount)
{
	int nSize = nCount - m_wbData.ulOnlineCount;
	if(nSize >= 0)
	{
		m_dwLoginCount += nSize;
	}
	else
	{
		m_dwLogoutCount -= nSize;
	}

	if(m_wbData.ulOnlineCount != nCount)
	{
		UG_CHAR szBuffer[1024];
		g_pLog->UGLog("setUserNum num = %d, m_dwCurrentMaxOnline = %d, %s.",nCount,m_dwCurrentMaxOnline,m_wbData.szName);
		m_wbData.ulOnlineCount = nCount;
		UGINT nCurNum = getCurrentMaxOnline();
		if(0 == nCurNum)
		{
			m_dwCurrentMaxOnline = m_wbData.ulOnlineCount;
		}
		else
		{
			if(int(m_dwCurrentMaxOnline) < int(m_wbData.ulOnlineCount))
			{
				m_dwCurrentMaxOnline = m_wbData.ulOnlineCount;
				g_pLog->UGLog("m_dwCurrentMaxOnline = %d, %s.",m_dwCurrentMaxOnline,m_wbData.szName);
			}
		}
		if(int(m_wbData.ulHistoryMaxCount) < int(m_wbData.ulOnlineCount))
		{
			m_wbData.ulHistoryMaxCount = m_wbData.ulOnlineCount;
			g_pLog->UGLog("m_wbData.ulHistoryMaxCount = %d, %s.",m_wbData.ulHistoryMaxCount,m_wbData.szName);
		}
		sprintf(szBuffer,SQL_SET_USER_NUM,m_wbData.ulOnlineCount,m_dwCurrentMaxOnline,m_wbData.ulHistoryMaxCount,m_wbData.ulID);
		UG_PVOID pvResult = NULL;
		UGLONG lRet = queryDoor(szBuffer,pvResult);
		if(lRet)
		{
			g_pLog->UGLog("queryDoor error %s, %s.",szBuffer,m_wbData.szName);
		}
		freeResultDoor(pvResult);
	}
	return 0;
}

UG_LONG CWorldbase::checkConnect(UG_DWORD dwParam)
{
	CWorldbaseManager* pWBM = (CWorldbaseManager*)m_pParent;
	if(pWBM)
	{
		for(int i = 0; i < pWBM->m_nWorldbaseSQLCount; i ++)
		{
			CWorldbase* pTemp = pWBM->m_pWorldbaseSQL + i;
			if((pTemp->m_dwPrivateIP == m_dwPrivateIP) && (pTemp->m_dwPrivatePort == dwParam) && !(pTemp->m_bOnline))
			{
				m_dwCheckID = dwParam;
				Q_strncpy(m_wbData.szName,pTemp->m_wbData.szName,WORLD_BASE_NAME_SIZE + 1);
				m_wbData.ulID = pTemp->m_wbData.ulID;
				m_wbData.dwIP = pTemp->m_wbData.dwIP;
				m_wbData.dwPort = pTemp->m_wbData.dwPort;
				m_wbData.ulHistoryMaxCount = pTemp->m_wbData.ulHistoryMaxCount;
				m_wbData.ulCPU = 0;
				m_wbData.ulOnlineCount = 0;
				m_bOnline = TRUE;
				pTemp->m_bOnline = TRUE;
				m_dwTimer = Plat_MSTime();
				m_dwCurrentMaxOnline = pTemp->m_dwCurrentMaxOnline;
				m_stepWB = WB_CHECK_OK;
				UGCHAR szBuffer[1024];
				sprintf(szBuffer,SQL_UPDATE_ONLINE,m_wbData.ulID);
				UGPVOID pvResult = NULL;
				queryDoor(szBuffer,pvResult);
				freeResultDoor(pvResult);
				sendWBData(NET_MSG_CHECK_DATA,0,NULL,0);
				unlockWorldserverUsercard();
				m_dwLoginCount = 0;
				m_dwLogoutCount = 0;
				g_pLog->UGLog("worldbase connect ok ip = %s,port = %d, check id = %d, current = %d, history = %d, %s.",convertip(m_dwPrivateIP),m_dwPrivatePort,dwParam,pTemp->m_dwCurrentMaxOnline,pTemp->m_wbData.ulHistoryMaxCount,m_wbData.szName);
				return 0;
			}
		}
	}
	g_pLog->UGLog("worldbase connect failed ip = %s,port = %d, check id = %d, %s.",convertip(m_dwPrivateIP),m_dwPrivatePort,dwParam,m_wbData.szName);
	m_bOnline = FALSE;
	return __LINE__;
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

UGINT CWorldbase::moneyToTime(UGINT nMoney)
{
	return nMoney * (theApp.m_config.m_nTimes);
}

UGINT CWorldbase::timeToMoney(UGINT nTimes)
{
	return (nTimes / (theApp.m_config.m_nTimes));
}

UGINT CWorldbase::saveMoney(MW_BILLING* nb)
{
	return 0;
	int nMoney = getMoney(nb->billing_id);
	int nLeave = timeToMoney(nb->money);
	int nBillingMoney = nMoney + nLeave;
	setMoney(nb->billing_id,nBillingMoney);
	return 0;
}

UGINT CWorldbase::dropMoney(MW_BILLING* nb)
{
	return 0;
	int nMoney = getMoney(nb->billing_id);
	int nLeave = timeToMoney(nb->money);
	int nBillingMoney = nMoney + nLeave;
	setMoney(nb->billing_id,nBillingMoney);
	
	int nDrop = 0;
	if(nBillingMoney == 0) //接束
	{
		nDrop = 0;
	}
	else if(nBillingMoney < 0)
	{
		nDrop = nBillingMoney;
	}
	else if(nBillingMoney >= (theApp.m_config.m_nMoney)) //全扣
	{
		nDrop = (theApp.m_config.m_nMoney);
		setMoney(nb->billing_id,nBillingMoney - (theApp.m_config.m_nMoney));
	}
	else
	{
		nDrop = nBillingMoney;
		setMoney(nb->billing_id,0);
	}
	
	nb->money = moneyToTime(nDrop);
	sendWBData(NET_MSG_DROP_MONEY_RETURN,0,(UG_PCHAR)(nb),sizeof(MW_BILLING));
	return 0;
}

UGINT CWorldbase::getMoney(UGINT nBillingID)
{
	return 1000;
	CWorldbaseManager* pWBM = (CWorldbaseManager*)m_pParent;
	static UG_CHAR szBuffer[1024];
	sprintf(szBuffer,SQL_GET_MONEY,nBillingID);
	UG_PVOID pvResult = NULL;
	UG_ULONG ulCount = 0;
	UGLONG lRet = queryDoor(szBuffer,pvResult,ulCount);
	if(-1 == lRet) //如果是第一次则返回0表示不能等入，否则该纪录一定存在
	{
		g_pLog->UGLog("query(%s) getMoney error, %s.",szBuffer,m_wbData.szName);
		return 0;
	}
	UG_PCHAR pchData = NULL;
	getQueryResultDoor(pvResult,0,0,pchData);
	if(pchData)
	{
		int nMoney = atoi(pchData);
		freeResultDoor(pvResult);
		return nMoney;
	}
	else
	{
		freeResultDoor(pvResult);
		return 0;
	}
	return 0;
}

UGINT CWorldbase::setMoney(UGINT nBillingID,UGINT nMoney)
{
	return 0;
	CWorldbaseManager* pWBM = (CWorldbaseManager*)m_pParent;
	static UG_CHAR szBuffer[1024];
	sprintf(szBuffer,SQL_SET_MONEY,nMoney,nBillingID);
	UG_PVOID pvResult = NULL;
	UG_ULONG ulCount = 0;
	UGLONG lRet = queryDoor(szBuffer,pvResult,ulCount);
	if(-1 == lRet)
	{
		g_pLog->UGLog("query(%s) setMoney error.",szBuffer);
		return -1;
	}
	freeResultDoor(pvResult);
	return 0;
}

UGINT CWorldbase::dropMoney(UGINT nBillingID,UGINT nMoney)
{
	return 0;
	int nBillingMoney = getMoney(nBillingID);
	int nLeave = nBillingMoney - nMoney;
	return setMoney(nBillingID,nLeave);
}

UGLONG CWorldbase::queryDoor(UGPCHAR pchQuery,UGPVOID& pvResult)
{
	CWorldbaseManager* pWBM = (CWorldbaseManager*)m_pParent;
	if(pWBM)
	{
		if(pWBM->m_pSQLDoorServer)
		{
			if(-1 == pWBM->m_pSQLDoorServer->query(pchQuery,pvResult))
			{
				g_pLog->UGLog("queryDoor error = %s, %s, %d, %s.",pWBM->m_pSQLDoorServer->getError(),pchQuery,__LINE__,m_wbData.szName);
				return -1;
			}
			return 0;
		}
		else
		{
			g_pLog->UGLog("queryDoor error, %s, %d, %s.",pchQuery,__LINE__,m_wbData.szName);
		}
	}
	else
	{
		g_pLog->UGLog("queryDoor error, %s, %d, %s.",pchQuery,__LINE__,m_wbData.szName);
	}
	return -1;
}

UGLONG CWorldbase::queryBilling(UGPCHAR pchQuery,UGPVOID& pvResult)
{
	CWorldbaseManager* pWBM = (CWorldbaseManager*)m_pParent;
	if(pWBM)
	{
		if(pWBM->m_pSQLBilling)
		{
			if(-1 == pWBM->m_pSQLBilling->query(pchQuery,pvResult))
			{
				g_pLog->UGLog("queryBilling error = %s, %s, %d, %s.",pWBM->m_pSQLBilling->getError(),pchQuery,__LINE__,m_wbData.szName);
				return -1;
			}
			return 0;
		}
		else
		{
			g_pLog->UGLog("queryBilling error, %s, %d, %s.",pchQuery,__LINE__,m_wbData.szName);
		}
	}
	else
	{
		g_pLog->UGLog("queryBilling error, %s, %d, %s.",pchQuery,__LINE__,m_wbData.szName);
	}
	return -1;
}

UGLONG CWorldbase::queryDoor(UGPCHAR pchQuery,UGPVOID& pvResult,UGDWORD& dwRows)
{
	CWorldbaseManager* pWBM = (CWorldbaseManager*)m_pParent;
	if(pWBM)
	{
		if(pWBM->m_pSQLDoorServer)
		{
			if(-1 == pWBM->m_pSQLDoorServer->query(pchQuery,pvResult,dwRows))
			{
				g_pLog->UGLog("queryDoor error = %s, %s, %d, %s.",pWBM->m_pSQLDoorServer->getError(),pchQuery,__LINE__,m_wbData.szName);
				return -1;
			}
			return 0;
		}
		else
		{
			g_pLog->UGLog("queryDoor error, %s, %d, %s.",pchQuery,__LINE__,m_wbData.szName);
		}
	}
	else
	{
		g_pLog->UGLog("queryDoor error, %s, %d.",pchQuery,__LINE__);
	}
	return -1;
}

UGLONG CWorldbase::queryBilling(UGPCHAR pchQuery,UGPVOID& pvResult,UGDWORD& dwRows)
{
	CWorldbaseManager* pWBM = (CWorldbaseManager*)m_pParent;
	if(pWBM)
	{
		if(pWBM->m_pSQLBilling)
		{
			if(-1 == pWBM->m_pSQLBilling->query(pchQuery,pvResult,dwRows))
			{
				g_pLog->UGLog("queryBilling error = %s, %s, %d, %s.",pWBM->m_pSQLBilling->getError(),pchQuery,__LINE__,m_wbData.szName);
				return -1;
			}
			return 0;
		}
		else
		{
			g_pLog->UGLog("queryBilling error, %s, %d, %s.",pchQuery,__LINE__,m_wbData.szName);
		}
	}
	else
	{
		g_pLog->UGLog("queryBilling error, %s, %d, %s.",pchQuery,__LINE__,m_wbData.szName);
	}
	return -1;
}

UGLONG CWorldbase::sendWBData(UGDWORD dwMsgID,UGDWORD dwParam,UGPCHAR pchData,UGDWORD dwLen)
{
	CWorldbaseManager* pWBM = (CWorldbaseManager*)m_pParent;
	if(pWBM)
	{
		if(pWBM->m_pWorldbaseTCPComm)
		{
			if(m_pvConnect)
			{
				if(-1 == pWBM->m_pWorldbaseTCPComm->writeBuffer(m_pvConnect,dwMsgID,dwParam,pchData,dwLen))
				{
					g_pLog->UGLog("sendWBData error %d, %d, %d, %s",dwMsgID,dwParam,__LINE__,m_wbData.szName);
					return -1;
				}
				return 0;
			}
		}
		else
		{
			g_pLog->UGLog("sendWBData error %d, %d, %d, %s",dwMsgID,dwParam,__LINE__,m_wbData.szName);
		}
	}
	else
	{
		g_pLog->UGLog("sendWBData error %d, %d, %d, %s",dwMsgID,dwParam,__LINE__,m_wbData.szName);
	}
	return -1;
}

UGLONG CWorldbase::getQueryResultDoor(UGPVOID pvResult,UGDWORD dwRowIndex,UGDWORD dwFieldIndex,UGPCHAR& pchResult)
{
	CWorldbaseManager* pWBM = (CWorldbaseManager*)m_pParent;
	if(pWBM)
	{
		if(pWBM->m_pSQLDoorServer)
		{
			if(-1 == pWBM->m_pSQLDoorServer->getQueryResult(pvResult,dwRowIndex,dwFieldIndex,pchResult))
			{
				g_pLog->UGLog("getQueryResultDoor error %d, %s",__LINE__,m_wbData.szName);
				return -1;
			}
			return 0;
		}
		else
		{
			g_pLog->UGLog("getQueryResultDoor error %d, %s",__LINE__,m_wbData.szName);
		}
	}
	else
	{
		g_pLog->UGLog("getQueryResultDoor error %d, %s",__LINE__,m_wbData.szName);
	}
	return -1;
}

UGLONG CWorldbase::getQueryResultBilling(UGPVOID pvResult,UGDWORD dwRowIndex,UGDWORD dwFieldIndex,UGPCHAR& pchResult)
{
	CWorldbaseManager* pWBM = (CWorldbaseManager*)m_pParent;
	if(pWBM)
	{
		if(pWBM->m_pSQLBilling)
		{
			if(-1 == pWBM->m_pSQLBilling->getQueryResult(pvResult,dwRowIndex,dwFieldIndex,pchResult))
			{
				g_pLog->UGLog("getQueryResultBilling error %d, %s",__LINE__,m_wbData.szName);
				return -1;
			}
			return 0;
		}
		else
		{
			g_pLog->UGLog("getQueryResultBilling error %d, %s",__LINE__,m_wbData.szName);
		}
	}
	else
	{
		g_pLog->UGLog("getQueryResultBilling error %d, %s",__LINE__,m_wbData.szName);
	}
	return -1;
}

UGLONG CWorldbase::freeResultDoor(UGPVOID pvResult)
{
	CWorldbaseManager* pWBM = (CWorldbaseManager*)m_pParent;
	if(pWBM)
	{
		if(pWBM->m_pSQLDoorServer)
		{
			pWBM->m_pSQLDoorServer->freeResult(pvResult);
			return 0;
		}
		else
		{
			g_pLog->UGLog("freeResultDoor error %d, %s",__LINE__,m_wbData.szName);
		}
	}
	else
	{
		g_pLog->UGLog("freeResultDoor error %d, %s",__LINE__,m_wbData.szName);
	}
	return -1;
}

UGLONG CWorldbase::freeResultBilling(UGPVOID pvResult)
{
	CWorldbaseManager* pWBM = (CWorldbaseManager*)m_pParent;
	if(pWBM)
	{
		if(pWBM->m_pSQLBilling)
		{
			pWBM->m_pSQLBilling->freeResult(pvResult);
			return 0;
		}
		else
		{
			g_pLog->UGLog("freeResultBilling error %d, %s",__LINE__,m_wbData.szName);
		}
	}
	else
	{
		g_pLog->UGLog("freeResultBilling error %d, %s",__LINE__,m_wbData.szName);
	}
	return -1;
}

UGLONG CWorldbase::disconnect()
{
	if(WB_CHECK_OK == m_stepWB)
	{
		CWorldbaseManager* pWBM = (CWorldbaseManager*)m_pParent;
		if(pWBM)
		{
			for(int i = 0; i < pWBM->m_nWorldbaseSQLCount; i ++)
			{
				CWorldbase* pTemp = pWBM->m_pWorldbaseSQL + i;
				if((pTemp->m_dwPrivateIP == m_dwPrivateIP) && (pTemp->m_dwPrivatePort == m_dwCheckID))
				{
//ben add in 2005-09-12 13:40:00 for set CurrentMaxOnline
					pTemp->m_dwCurrentMaxOnline = m_dwCurrentMaxOnline;
					pTemp->m_wbData.ulHistoryMaxCount = m_wbData.ulHistoryMaxCount;
					pTemp->m_wbData.ulOnlineCount = 0;
//end by ben
					pTemp->m_bOnline = FALSE;
					return 0;
				}
			}
		}
	}
	g_pLog->UGLog("disconnect in CWorldbase is error, %s.",m_wbData.szName);
	return __LINE__;
}

UGINT CWorldbase::beginDropMoney(MW_BILLING* nb)
{
	int nMoney = getMoney(nb->billing_id);
	int nLeave = timeToMoney(nb->money);
	int nBillingMoney = nMoney + nLeave;
	g_pLog->UGLog("drop money money = %d, id = %d.",nMoney,nb->billing_id);
	setMoney(nb->billing_id,nBillingMoney);
	
	int nDrop = 0;
	if(nBillingMoney == 0) //接束
	{
		nDrop = 0;
	}
	else if(nBillingMoney < 0)
	{
		nDrop = nBillingMoney;
	}
	else if(nBillingMoney >= (theApp.m_config.m_nMoney)) //全扣
	{
		nDrop = (theApp.m_config.m_nMoney);
		setMoney(nb->billing_id,nBillingMoney - (theApp.m_config.m_nMoney));
	}
	else
	{
		nDrop = nBillingMoney;
		setMoney(nb->billing_id,0);
	}
	nb->money = moneyToTime(nDrop);
	sendWBData(NET_MSG_DROP_MONEY_RETURN,0,(UG_PCHAR)(nb),sizeof(MW_BILLING));
	return 0;
}

UGINT CWorldbase::insertLog(MW_BILLING* nb)
{
	return 0;
}

UGINT CWorldbase::dropLog(MW_BILLING* nb)
{
	return 0;
}

UGINT CWorldbase::saveLog(MW_BILLING* nb)
{
	return 0;
}

UGINT CWorldbase::resetCurrentMaxOnline()
{
	int nRet = getCurrentMaxOnline();
	UGCHAR szBuffer[1024];
	if(0 == nRet)
	{
		m_dwCurrentMaxOnline = m_wbData.ulOnlineCount;
		sprintf(szBuffer,SQL_SET_MAX_ONLINE,m_dwCurrentMaxOnline,m_wbData.ulID);
		UG_PVOID pvResult = NULL;
		queryDoor(szBuffer,pvResult);
		freeResultDoor(pvResult);
	}
	return nRet;
}

UGINT CWorldbase::getCurrentMaxOnline()
{
	int nRet = -1;
	UGCHAR szBuffer[1024];
	sprintf(szBuffer,SQL_GET_MAX_ONLINE,m_wbData.ulID);
	UGPVOID pvResult = NULL;
	queryDoor(szBuffer,pvResult);
	UGPCHAR pchMaxOnline = NULL;
	getQueryResultDoor(pvResult,0,0,pchMaxOnline);
	if(pchMaxOnline)
	{
		nRet = atoi(pchMaxOnline);
	}
	else
	{
		g_pLog->UGLog("getQueryResultDoor error in resetCurrentMaxOnline, %s.",m_wbData.szName);
	}
	freeResultDoor(pvResult);
	return nRet;
}

UGLONG CWorldbase::unlockWorldserverUsercard()
{
	char szQuery[1024];
	sprintf(szQuery,SQL_SELECT_UNLOCK_USERCARD,m_wbData.ulID,USERCARD_STATUE_LOCKED);
	UGPVOID pvRet = NULL;
	UGDWORD dwRows = 0;
	UGLONG lRet = queryDoor(szQuery,pvRet,dwRows);
	if(!lRet)
	{
		g_pLog->UGLog("unlockWorldserverUsercard ok query(%s).",szQuery);
		for(int i = 0; i < int(dwRows); i ++)
		{
			UGPCHAR pchData = NULL;
			getQueryResultDoor(pvRet,i,0,pchData);
			int nID = atoi(pchData);
			getQueryResultDoor(pvRet,i,1,pchData);
			int nBillingID = atoi(pchData);
			getQueryResultDoor(pvRet,i,2,pchData);
			int nPlayerID = atoi(pchData);
			char szAccount[1024];
			getQueryResultDoor(pvRet,i,3,pchData);
			strcpy(szAccount,pchData);
			char szPasswd[1024];
			getQueryResultDoor(pvRet,i,4,pchData);
			strcpy(szPasswd,pchData);
			g_pLog->UGLog("unlockWorldserverUsercard id = %d, billingid = %d, playerid = %d, account = %s, passwd = %d, %s.",nID,nBillingID,nPlayerID,szAccount,szPasswd,m_wbData.szName);
		}
		freeResultDoor(pvRet);
		sprintf(szQuery,SQL_UNLOCK_WORLDBASE_USERCARD,USERCARD_STATUE_UNLOCK,m_wbData.ulID,USERCARD_STATUE_LOCKED);
		queryDoor(szQuery,pvRet,dwRows);
		freeResultDoor(pvRet);
		g_pLog->UGLog("unlockWorldserverUsercard ok query(%s), %s.",szQuery,m_wbData.szName);
	}
	else
	{
		g_pLog->UGLog("unlockWorldserverUsercard error query(%s), %s.",szQuery,m_wbData.szName);
		freeResultDoor(pvRet);
	}
	return 0;
}
//单线程,不会有并发同步问题
UGLONG CWorldbase::checkUsercard(USERCARD* p)
{
	(p->account)[sizeof(p->account) - 1] = '\0';
	(p->passwd)[sizeof(p->passwd) - 1] = '\0';
	char szQuery[1024];
	sprintf(szQuery,SQL_SELECT_USERCARD,p->account);
	UGPVOID pvRet = NULL;
	UGDWORD dwRows = 0;
	UGLONG lRet = queryDoor(szQuery,pvRet,dwRows);
	UGINT nRet = USERCARD_NOFINDED;
	g_pLog->UGLog("checkUsercard query(%s).",szQuery);
	if(lRet)
	{
		g_pLog->UGLog("checkUsercard error USERCARD_NOFINDED line = %d, %s.",__LINE__,m_wbData.szName);
		nRet = USERCARD_NOFINDED;
	}
	else
	{
		if(1 != dwRows)
		{
			g_pLog->UGLog("checkUsercard error USERCARD_NOFINDED rows = %d,line = %d, %s.",dwRows,__LINE__,m_wbData.szName);
			nRet = USERCARD_NOFINDED;
		}
		else
		{
			UGPCHAR pchData = NULL;
			getQueryResultDoor(pvRet,0,0,pchData); //account id
			p->account_id = atoi(pchData);
			getQueryResultDoor(pvRet,0,1,pchData); //account
			if(strcmp(p->account,pchData))
			{
				g_pLog->UGLog("checkUsercard USERCARD_NOFINDED card = %s, account = %s, line = %d, %s.",pchData,p->account,__LINE__,m_wbData.szName);
				nRet = USERCARD_NOFINDED;
			}
			else
			{
				getQueryResultDoor(pvRet,0,2,pchData); //passwd
				UGCHAR szPwd[33]; //MD5加密后为32字节
				CUGEncrypt::MD5Encrypt(p->passwd,szPwd);
				szPwd[32] = '\0';
				if(strcmp(szPwd,pchData))
				{
					g_pLog->UGLog("checkUsercard USERCARD_PASSWD_ERROR card = %d, passwd = %d, line = %d, %s.",pchData,p->passwd,__LINE__,m_wbData.szName);
					nRet = USERCARD_PASSWD_ERROR;
				}
				else
				{
					getQueryResultDoor(pvRet,0,3,pchData); //status
					UGINT nStatus = atoi(pchData);

					getQueryResultDoor(pvRet,0,4,pchData); //end time
					char szCTimer[1024];
					strcpy(szCTimer,pchData);
					int nCYear = 0;
					int nCMonth = 0;
					int nCDay = 0;
					convertTimer(pchData,nCYear,nCMonth,nCDay);
//add by ben in 2005-10-18
					getQueryResultDoor(pvRet,0,5,pchData); //pocket
					p->pocket_id = atoi(pchData);
					getQueryResultDoor(pvRet,0,6,pchData); //pocket
					strncpy(p->pocket,pchData,64);
//end by ben					
					
					if(USERCARD_STATUE_LOCKED == nStatus)
					{
						g_pLog->UGLog("checkUsercard USERCARD_LOCKED line = %d, %s.",__LINE__,m_wbData.szName);
						nRet = USERCARD_LOCKED;
					}
					else if(USERCARD_STATUE_UNLOCK == nStatus)
					{
						long lTimer = 0;
						time(&lTimer);
						struct tm* pTimer = NULL;
						pTimer = localtime(&lTimer);
						
						int nYear = pTimer->tm_year + 1900;
						int nMonth = pTimer->tm_mon + 1;
						int nDay = pTimer->tm_mday;

						if(compareTimer(nCYear,nCMonth,nCDay,nYear,nMonth,nDay) >= 0)
						{
							g_pLog->UGLog("checkUsercard USERCARD_CANUSE line = %d, %s.",__LINE__,m_wbData.szName);
							nRet = USERCARD_CANUSE;
						}
						else
						{
							g_pLog->UGLog("checkUsercard USERCARD_TIMEOUT end = %d-%d-%d, cur = %d-%d-%d, line = %d, %s.",nCYear,nCMonth,nCDay,nYear,nMonth,nDay,__LINE__,m_wbData.szName);
							nRet = USERCARD_TIMEOUT;
						}
					}
					else
					{
						g_pLog->UGLog("checkUsercard USERCARD_NOFINDED = %d, line = %d, %s.",nStatus,__LINE__,m_wbData.szName);
						nRet = USERCARD_NOFINDED;
					}
				}
			}
		}
	}
	freeResultDoor(pvRet);
	if(USERCARD_CANUSE == nRet)
	{
		long lTimer = 0;
		time(&lTimer);
		char szTimer[1024];
		struct tm* pTimer = NULL;
		pTimer = localtime(&lTimer);
		sprintf(szTimer,"%d-%d-%d %d:%d:%d",pTimer->tm_year + 1900,pTimer->tm_mon + 1,pTimer->tm_mday,pTimer->tm_hour,pTimer->tm_min,pTimer->tm_sec);
		
		sprintf(szQuery,SQL_OPERATE_USERCARD,p->billing_id,m_wbData.ulID,p->player_id,szTimer,USERCARD_STATUE_LOCKED,p->account_id);
		queryDoor(szQuery,pvRet);
		freeResultDoor(pvRet);
		g_pLog->UGLog("checkUsercard ok query(%s), %s.",szQuery,m_wbData.szName);
	}
	sendWBData(NET_MSG_USERCARD_CHECK_RETURN,(UGDWORD)nRet,UGPCHAR(p),sizeof(USERCARD));
	return 0;
}

UGLONG CWorldbase::unlockUsercard(USERCARD* p)
{
	char szQuery[1024];
	
	long lTimer = 0;
	time(&lTimer);
	char szTimer[1024];
	struct tm* pTimer = NULL;
	pTimer = localtime(&lTimer);
	sprintf(szTimer,"%d-%d-%d %d:%d:%d",pTimer->tm_year + 1900,pTimer->tm_mon + 1,pTimer->tm_mday,pTimer->tm_hour,pTimer->tm_min,pTimer->tm_sec);

	sprintf(szQuery,SQL_OPERATE_USERCARD,p->billing_id,m_wbData.ulID,p->player_id,szTimer,USERCARD_STATUE_UNLOCK,p->account_id);
	UGPVOID pvRet = NULL;
	UGDWORD dwRows = 0;
	queryDoor(szQuery,pvRet,dwRows);
	freeResultDoor(pvRet);
	if(1 == dwRows)
	{
		g_pLog->UGLog("unlockUsercard ok, query(%s).",szQuery);
		sendWBData(NET_MSG_USERCARD_UNLOCKED_RETURN,(UGDWORD)USERCARD_OK,UGPCHAR(p),sizeof(USERCARD));
	}
	else
	{
		g_pLog->UGLog("unlockUsercard error, query(%s).",szQuery);
		sendWBData(NET_MSG_USERCARD_UNLOCKED_RETURN,(UGDWORD)USERCARD_ERROR,UGPCHAR(p),sizeof(USERCARD));
	}
	return 0;
}

UGLONG CWorldbase::delUsercard(USERCARD* p)
{
	char szQuery[1024];
	
	long lTimer = 0;
	time(&lTimer);
	char szTimer[1024];
	struct tm* pTimer = NULL;
	pTimer = localtime(&lTimer);
	sprintf(szTimer,"%d-%d-%d %d:%d:%d",pTimer->tm_year + 1900,pTimer->tm_mon + 1,pTimer->tm_mday,pTimer->tm_hour,pTimer->tm_min,pTimer->tm_sec);
	
	sprintf(szQuery,SQL_OPERATE_USERCARD,p->billing_id,m_wbData.ulID,p->player_id,szTimer,USERCARD_STATUE_USED,p->account_id);
	UGPVOID pvRet = NULL;
	UGDWORD dwRows = 0;
	queryDoor(szQuery,pvRet,dwRows);
	freeResultDoor(pvRet);
	if(1 == dwRows)
	{
		g_pLog->UGLog("delUsercard ok, query(%s), %s.",szQuery,m_wbData.szName);
		sendWBData(NET_MSG_USERCARD_DEL_RETURN,(UGDWORD)USERCARD_OK,UGPCHAR(p),sizeof(USERCARD));
	}
	else
	{
		g_pLog->UGLog("delUsercard error, query(%s), %s.",szQuery,m_wbData.szName);
		sendWBData(NET_MSG_USERCARD_DEL_RETURN,(UGDWORD)USERCARD_ERROR,UGPCHAR(p),sizeof(USERCARD));
	}
	return 0;
}

UGLONG CWorldbase::convertTimer(UGPCHAR pchTimer,UGINT& nYear,UGINT& nMonth,UGINT& nDay)
{
	nYear = 0;
	nMonth = 0;
	nDay = 0;
	if(pchTimer)
	{
		char* pchFind = strstr(pchTimer,"-");
		if(!pchFind)
		{
			nYear = atoi(pchTimer);
			return 0;
		}
		*pchFind = '\0';
		nYear = atoi(pchTimer);
		pchTimer = pchFind + 1;
		if(pchTimer)
		{
			pchFind = strstr(pchTimer,"-");
			if(!pchFind)
			{
				nMonth = atoi(pchTimer);
				return 0;
			}
			*pchFind = '\0';
			nMonth = atoi(pchTimer);
			pchTimer = pchFind + 1;
		}
		if(pchTimer)
		{
			pchFind = strstr(pchTimer,"-");
			if(!pchFind)
			{
				nDay = atoi(pchTimer);
				return 0;
			}
			*pchFind = '\0';
			nDay = atoi(pchTimer);
		}
	}
	return 0;
}

UGLONG CWorldbase::compareTimer(UGINT nYear,UGINT nMonth,UGINT nDay,UGINT nCYear,UGINT nCMonth,UGINT nCDay)
{
	if(nYear < nCYear)
	{
		return -1;
	}
	else if(nYear > nCYear)
	{
		return 1;
	}
	else
	{
		if(nMonth < nCMonth)
		{
			return -1;
		}
		else if(nMonth > nCMonth)
		{
			return 1;
		}
		else
		{
			if(nDay < nCDay)
			{
				return -1;
			}
			else if(nDay > nCDay)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	return 0;
}

UGINT CWorldbase::logGMMonitor(GM_MONITOR* p)
{
	static char szQuery[1024];
	p->szGMName[MAX_NAME_LENGTH] = '\0';
	p->szhContent[255] = '\0';
	sprintf(szQuery,SQL_GM_MONITR_LOG,m_wbData.ulID,p->nOPTimer,p->nOPType,p->szhContent);
	UGPVOID pvRet = NULL;
	queryDoor(szQuery,pvRet);
	freeResultDoor(pvRet);
	return 0;
}

