// GMTools.cpp: implementation of the CGMTools class.
//
//////////////////////////////////////////////////////////////////////

#include "incall.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGMTools::CGMTools()
{
	m_dwIP = 0;
	m_wPort = 0;
	m_bOnline = FALSE;
	m_dwTimer = 0;
	m_pvConnect = NULL;
	m_pTCPComm = NULL;
	m_pvParent = NULL;
	m_stepGM = DWORD(GM_NOT_CONNECTED);
}

CGMTools::~CGMTools()
{

}

UGLONG CGMTools::dispatchMsg(UGDWORD dwMsgID,UGDWORD dwParam,UGPCHAR pchData,UGDWORD dwLen)
{
	switch(dwMsgID)
	{
	case NET_MSG_GM_MONITOR_CHANNEL:
		{
			if(dwLen == sizeof(GMMSG))
			{
				sendGM((GMMSG*)pchData);
			}
			else
			{
				g_pLog->UGLog("NET_MSG_GM_MONITOR_CHANNEL error len %d != %d",dwLen,sizeof(GMMSG));
			}
			break;
		}
	default:
		{
			break;
		}
	}
	return 0;
}

UGLONG CGMTools::sendMessage(UGDWORD dwMsgID,UGDWORD dwParam,UGPCHAR pchData,UGDWORD dwLen)
{
	if(m_pvConnect)
	{
		if(m_pTCPComm)
		{
			return m_pTCPComm->writeBuffer(m_pvConnect,dwMsgID,dwParam,pchData,dwLen);
		}
	}
	return 0;
}

UGLONG CGMTools::sendGM(GMMSG* p)
{
	CWorldbase* pWorldbase = (CWorldbase*)(m_pvParent);
	if(pWorldbase)
	{
		CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(p->nPlayerid));
		if(pPlayer)
		{
			pPlayer->sendGMChannel(p);
			sendMessage(NET_MSG_GM_MONITOR_CLIENT_RETURN,CHECK_OK);
			return 0;
		}
	}
	sendMessage(NET_MSG_GM_MONITOR_CLIENT_RETURN,CHECK_ERROR);
	return 0;
}
