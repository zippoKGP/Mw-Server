// GM.cpp: implementation of the CGM class.
//
//////////////////////////////////////////////////////////////////////

#include "incall.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGM::CGM()
{
	m_stepGM = GM_NOT_CONNECTED;
	m_pvConnect = NULL;
	m_dwTimer = 0;
	m_bOnline = FALSE;
	m_dwIP = 0;
	m_dwPort = 0;
	m_dwCheckID = 0;
	m_pParent = NULL;
}

CGM::~CGM()
{

}

UG_LONG CGM::dispatchMsg(UG_DWORD dwMsgID,UG_DWORD wParam,UG_PCHAR pchData,UG_ULONG ulLen)
{
	switch(dwMsgID)
	{
	case NET_GM_CHECK_USER:
		{
			checkUser(pchData,ulLen);
		}
		break;
	case NET_GM_GETWORLDBASE:
		{
			if(GM_CHECK_OK == m_stepGM) //校验通过后才能发送
			{
				sendWorldbase();
			}
		}
		break;
	default:
		{
		}
		break;
	}
	return 0;
}

UG_LONG CGM::cout(FILE* p)
{
	fprintf(p,"beonline = %d, step = %d, login timer = %d, checkid = %d.\n",m_bOnline,m_stepGM,m_dwTimer,m_dwCheckID);
	fprintf(p,"ip = %s, port = %d.\n",convertip(m_dwIP),m_dwPort);
	fprintf(p,"\n");
	return 0;
}

UGLONG CGM::checkUser(UG_PCHAR pchData,UG_ULONG ulLen)
{
	CGMManager* pGMM = (CGMManager*)m_pParent;
	NAME_PASSWD np;
	UGLONG lRet = pGMM->m_pTCPComm->dencrypt(m_pvConnect,(UGPBYTE)pchData,ulLen,&np);
	if(lRet)
	{
		return lRet;
	}
	
	lRet = isGMName(np.szName);
	if(lRet)
	{
		sendWBData(NET_GM_CHECK_USER_RETURN,GM_LOGIN_CHECK_ID_ERROR,NULL,0);
		return lRet;
	}
	
	UGCHAR szCheckGM[1024];
	sprintf(szCheckGM,SQL_CHECK_GM,np.szName);
	UGPVOID pvResult = NULL;
	UGDWORD dwRows = 0;
	lRet = queryBilling(szCheckGM,pvResult,dwRows);
	if(lRet)
	{
		sendWBData(NET_GM_CHECK_USER_RETURN,GM_LOGIN_CHECK_ID_ERROR,NULL,0);
		freeResultBilling(pvResult);
		return lRet;
	}
	if(dwRows < 1)
	{
		sendWBData(NET_GM_CHECK_USER_RETURN,GM_LOGIN_CHECK_USER_ERROR,NULL,0);
		freeResultBilling(pvResult);
		return __LINE__;
	}
	UGPCHAR pchResult = NULL;
	lRet = getQueryResultBilling(pvResult,0,0,pchResult);
	if(lRet)
	{
		sendWBData(NET_GM_CHECK_USER_RETURN,GM_LOGIN_CHECK_USER_ERROR,NULL,0);
		freeResultBilling(pvResult);
		return lRet;
	}
	UGLONG lID = atoi(pchResult);
	pchResult = NULL;
	lRet = getQueryResultBilling(pvResult,0,1,pchResult);
	if(lRet)
	{
		sendWBData(NET_GM_CHECK_USER_RETURN,GM_LOGIN_CHECK_USER_ERROR,NULL,0);
		freeResultBilling(pvResult);
		return lRet;
	}
	if(strcmp(pchResult,np.szPasswd))
	{
		sendWBData(NET_GM_CHECK_USER_RETURN,GM_LOGIN_CHECK_PASSWD_ERROR,NULL,0);
		freeResultBilling(pvResult);
		return __LINE__;
	}
	freeResultBilling(pvResult);

	m_stepGM = GM_CHECK_OK;
	sendWBData(NET_GM_CHECK_USER_RETURN,GM_LOGIN_CHECK_OK,NULL,0);
	return 0;
}

UGLONG CGM::queryBilling(UGPCHAR pchQuery,UGPVOID& pvResult)
{
	CWorldbaseManager* pWBM = theApp.m_pWorldbaseManager;
	if(pWBM)
	{
		if(pWBM->m_pSQLBilling)
		{
			if(-1 == pWBM->m_pSQLBilling->query(pchQuery,pvResult))
			{
				g_pLog->UGLog("queryBilling error = %s, %s, %d.",pWBM->m_pSQLBilling->getError(),pchQuery,__LINE__);
				return -1;
			}
			return 0;
		}
		else
		{
			g_pLog->UGLog("queryBilling error, %s, %d.",pchQuery,__LINE__);
		}
	}
	else
	{
		g_pLog->UGLog("queryBilling error, %s, %d.",pchQuery,__LINE__);
	}
	return -1;
}

UGLONG CGM::queryBilling(UGPCHAR pchQuery,UGPVOID& pvResult,UGDWORD& dwRows)
{
	CWorldbaseManager* pWBM = theApp.m_pWorldbaseManager;
	if(pWBM)
	{
		if(pWBM->m_pSQLBilling)
		{
			if(-1 == pWBM->m_pSQLBilling->query(pchQuery,pvResult,dwRows))
			{
				g_pLog->UGLog("queryBilling error = %s, %s, %d.",pWBM->m_pSQLBilling->getError(),pchQuery,__LINE__);
				return -1;
			}
			return 0;
		}
		else
		{
			g_pLog->UGLog("queryBilling error, %s, %d.",pchQuery,__LINE__);
		}
	}
	else
	{
		g_pLog->UGLog("queryBilling error, %s, %d.",pchQuery,__LINE__);
	}
	return -1;
}

UGLONG CGM::getQueryResultBilling(UGPVOID pvResult,UGDWORD dwRowIndex,UGDWORD dwFieldIndex,UGPCHAR& pchResult)
{
	CWorldbaseManager* pWBM = theApp.m_pWorldbaseManager;
	if(pWBM)
	{
		if(pWBM->m_pSQLBilling)
		{
			if(-1 == pWBM->m_pSQLBilling->getQueryResult(pvResult,dwRowIndex,dwFieldIndex,pchResult))
			{
				g_pLog->UGLog("getQueryResultBilling error %d",__LINE__);
				return -1;
			}
			return 0;
		}
		else
		{
			g_pLog->UGLog("getQueryResultBilling error %d",__LINE__);
		}
	}
	else
	{
		g_pLog->UGLog("getQueryResultBilling error %d",__LINE__);
	}
	return -1;
}

UGLONG CGM::freeResultBilling(UGPVOID pvResult)
{
	CWorldbaseManager* pWBM = theApp.m_pWorldbaseManager;
	if(pWBM)
	{
		if(pWBM->m_pSQLBilling)
		{
			pWBM->m_pSQLBilling->freeResult(pvResult);
			return 0;
		}
		else
		{
			g_pLog->UGLog("freeResultBilling error %d",__LINE__);
		}
	}
	else
	{
		g_pLog->UGLog("freeResultBilling error %d",__LINE__);
	}
	return -1;
}

UGLONG CGM::sendWBData(UGDWORD dwMsgID,UGDWORD dwParam,UGPCHAR pchData,UGDWORD dwLen)
{
	CGMManager* pGMM = (CGMManager*)m_pParent;
	if(pGMM)
	{
		if(pGMM->m_pTCPComm)
		{
			if(m_pvConnect)
			{
				if(-1 == pGMM->m_pTCPComm->writeBuffer(m_pvConnect,dwMsgID,dwParam,pchData,dwLen))
				{
					g_pLog->UGLog("sendWBData error %d, %d, %d",dwMsgID,dwParam,__LINE__);
					return -1;
				}
				return 0;
			}
		}
		else
		{
			g_pLog->UGLog("sendWBData error %d, %d, %d",dwMsgID,dwParam,__LINE__);
		}
	}
	else
	{
		g_pLog->UGLog("sendWBData error %d, %d, %d",dwMsgID,dwParam,__LINE__);
	}
	return -1;
}

UGLONG CGM::getWBData(PSERVERINFO& pWBData,UGINT& nCount)
{
	CGMManager* pGMM = (CGMManager*)m_pParent;
	if(pGMM)
	{
		return pGMM->getWBData(pWBData,nCount);
	}
	return __LINE__;
}

UGLONG CGM::sendWorldbase()
{
	PSERVERINFO pWBData = NULL;
	UGINT nCount = 0;
	UGLONG lRet = getWBData(pWBData,nCount);
	if(lRet)
	{
		return __LINE__;
	}
	if(theApp.m_pWorldbaseManager->m_nWorldbaseSQLCount + nCount > MAX_WORLD)
	{
		return __LINE__;
	}
	
	for(int i = 0; i < nCount; i ++)
	{
		PSERVERINFO pTemp = pWBData + i;
		if(pTemp->nLoginNum < pTemp->nLineNum) //因为在线人数是登入到服务器的人数,而登入人数为验证通过的人数.
		{
			pTemp->nLoginNum = pTemp->nLineNum;
			pTemp->nLogoutNum = 0;
		}
	}

	UGINT nWBCount = 0;
	for(i = 0; i < theApp.m_pWorldbaseManager->m_nWorldbaseSQLCount; i ++)
	{
		PSERVERINFO pTemp = pWBData + nCount + nWBCount;
		CWorldbase* pW = theApp.m_pWorldbaseManager->m_pWorldbaseSQL + i;
		if(!(pW->m_bOnline))
		{
			strcpy(pTemp->szServerName,pW->m_wbData.szName);
			pTemp->nIP = pW->m_wbData.dwIP;
			pTemp->nLineNum = pW->m_wbData.ulOnlineCount;
			pTemp->nPort = pW->m_wbData.dwPort;
			pTemp->nServerID = pW->m_wbData.ulID;
			pTemp->bLine = 0;
			pTemp->nLoginNum = 0;
			pTemp->nLogoutNum = 0;
			nWBCount ++;
		}
	}
	sendWBData(NET_GM_GETWORLDBASE_RETURN,nWBCount + nCount,(UGPCHAR)pWBData,sizeof(SERVERINFO) * (nWBCount + nCount));
	return 0;
}

UGLONG CGM::isGMName(UGPCHAR pchGMName)
{
	for(int i = 0; i < theApp.m_config.m_nGMNameCount; i ++)
	{
		UGPCHAR pchName = *(theApp.m_config.m_ppchGMName + i);
		if(pchName)
		{
			if(!strcmp(pchName,pchGMName))
			{
				return 0;
			}
		}
	}
	return __LINE__;
}
