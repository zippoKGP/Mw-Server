// WBConnect.cpp: implementation of the CWBConnect class.
//
//////////////////////////////////////////////////////////////////////

#include "incall.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWBConnect::CWBConnect()
{
	m_dwIP = 0;
	m_wPort = 0;
	m_bOnline = FALSE;
	m_dwTimer = 0;
	m_pvConnect = NULL;
	m_pTCPComm = NULL;
	m_stepWB = WB_NOT_CONNECTED;
	m_pvParent = NULL;
}

CWBConnect::~CWBConnect()
{

}

UG_LONG CWBConnect::dispatchMsg(UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR pchData,UG_ULONG ulLen)
{
	switch(dwMsgID)
	{
	case NET_MSG_WBCHAT_SCENE_ENTER:
		{
			if(sizeof(CHAT_SCENE) == ulLen)
			{
				CHAT_SCENE c;
				Q_memcpy(&c,pchData,sizeof(CHAT_SCENE));
				if(!addPlayerInScene(&c))
				{
					m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_WBCHAT_SCENE_ENTER_RETURN,(UG_DWORD)CHECK_OK,NULL,0,WRITE_WORLDBASE_OVERFLOW);
				}
				else
				{
					m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_WBCHAT_SCENE_ENTER_RETURN,(UG_DWORD)CHECK_ERROR,NULL,0,WRITE_WORLDBASE_OVERFLOW);
				}
			}
			else
			{
				m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_WBCHAT_SCENE_ENTER_RETURN,(UG_DWORD)CHECK_DATA_LEN_ERROR,NULL,0,WRITE_WORLDBASE_OVERFLOW);
				g_pLog->UGLog("msg NET_MSG_WBCHAT_SCENE_ENTER_RETURN data len error, len = %d.",ulLen);
			}
			break;
		}
	case NET_MSG_WBCHAT_SCENE_EXIT:
		{
			if(sizeof(CHAT_SCENE) == ulLen)
			{
				CHAT_SCENE c;
				Q_memcpy(&c,pchData,sizeof(CHAT_SCENE));
				if(!delPlayerInScene(&c))
				{
					m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_WBCHAT_SCENE_EXIT_RETURN,(UG_DWORD)CHECK_OK,NULL,0,WRITE_WORLDBASE_OVERFLOW);
				}
				else
				{
					m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_WBCHAT_SCENE_EXIT_RETURN,(UG_DWORD)CHECK_ERROR,NULL,0,WRITE_WORLDBASE_OVERFLOW);
				}
			}
			else
			{
				m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_WBCHAT_SCENE_EXIT_RETURN,(UG_DWORD)CHECK_DATA_LEN_ERROR,NULL,0,WRITE_WORLDBASE_OVERFLOW);
				g_pLog->UGLog("msg NET_MSG_WBCHAT_SCENE_EXIT_RETURN data len error, len = %d.",ulLen);
			}
			break;
		}
	case NET_MSG_WBCHAT_DISCONNECT:
		{
			disconnectPlayer(UG_INT32(dwParam));
		}
		break;
	case NET_MSG_WORLD_CHANNEL_CHECK:
		{
			if(ulLen == sizeof(int))
			{
				CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
				CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(int(dwParam)));
				if(pPlayer)
				{
					return pPlayer->sendWorldChannel(*(UGPINT(pchData)));
				}
			}
			return -1;
		}
		break;
	case NET_MSG_SYSTEM_CHANNEL:
		{
			CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
			
			char szSys[2048];
			if(ulLen < 2047)
			{
				strncpy(szSys,pchData,ulLen);
				szSys[ulLen] = '\0';
				pWorldbase->sendGMMsg(GM_MSG_SYSTEM,0,NULL,szSys);
			}

			map<UG_INT32,UG_PVOID>::iterator it;
			for(it = pWorldbase->m_pPlayerManager->m_mapPlayer.begin(); it != pWorldbase->m_pPlayerManager->m_mapPlayer.end(); it ++)
			{
				CPlayer* pPlayer = (CPlayer*)(it->second);
				if(pPlayer)
				{
					if(-1 == pPlayer->sendData(NET_MSG_SYSTEM_CHANNEL_RETURN,dwParam,pchData,ulLen,0))
					{
//						g_pLog->UGLog("send NET_MSG_SYSTEM_CHANNEL_RETURN error, player id = %d.",pPlayer->m_n32Playerid);
					}
				}
			}
		}
		break;
//add by ben in 2005-09-23
	case NET_MSG_WBCHAT_SCENE:
		{
			if(ulLen > 0)
			{
				CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
				map<UG_INT32,CScene*>::iterator it = pWorldbase->m_pSceneManager->m_mapScene.find((UGINT)dwParam);
				if(it != pWorldbase->m_pSceneManager->m_mapScene.end())
				{
					CScene* pScene = it->second;
					map<UG_INT32,UG_PVOID>::iterator it;
					for(it = pScene->m_mapPlayer.begin(); it != pScene->m_mapPlayer.end(); it ++)
					{
						map<UG_INT32,UG_PVOID>::iterator itPlayer = pWorldbase->m_pPlayerManager->m_mapPlayer.find(it->first);
						if(itPlayer != pWorldbase->m_pPlayerManager->m_mapPlayer.end())
						{
							CPlayer* pPlayer = (CPlayer*)(itPlayer->second);
							if(pPlayer)
							{
								pPlayer->sendData(NET_MSG_SYSTEM_CHANNEL_RETURN,dwParam,pchData,ulLen,0);
							}
						}
					}
					m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_WBCHAT_SCENE_RETURN,(UG_DWORD)CHECK_OK,NULL,0,0);
				}
				else
				{
					m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_WBCHAT_SCENE_RETURN,(UG_DWORD)CHECK_ERROR,NULL,0,0);
					g_pLog->UGLog("NET_MSG_WBCHAT_SCENE error, scene id = %d not find.",dwParam);
				}
			}
			else
			{
				m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_WBCHAT_SCENE_RETURN,(UG_DWORD)CHECK_ERROR,NULL,0,0);
				g_pLog->UGLog("NET_MSG_WBCHAT_SCENE error, ulLen = %d <= 0.",ulLen);
			}
			break;
		}
//end by ben
	case NET_MSG_CHAT_CHANG_FRIEND:
		{
			if(sizeof(friend_t) == ulLen)
			{
				friend_t f;
				memcpy(&f,pchData,sizeof(friend_t));
				if(!changeFriend(UG_INT32(dwParam),f))
				{
					m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_CHAT_CHANG_FRIEND_RETURN,(UG_DWORD)CHECK_OK,NULL,0,WRITE_WORLDBASE_OVERFLOW);
				}
				else
				{
					m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_CHAT_CHANG_FRIEND_RETURN,(UG_DWORD)CHECK_ERROR,NULL,0,WRITE_WORLDBASE_OVERFLOW);
				}
			}
			else
			{
				m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_CHAT_ADD_FRIEND_RETURN,(UG_DWORD)CHECK_DATA_LEN_ERROR,NULL,0,WRITE_WORLDBASE_OVERFLOW);
				g_pLog->UGLog("msg NET_MSG_CHAT_CHANG_FRIEND data len error, len = %d.",ulLen);
			}
			break;
		}
	case NET_MSG_CHAT_ADD_FRIEND:
		{
			if(sizeof(UG_INT32) == ulLen)
			{
				UG_INT32 n32Friendid = 0;
				memcpy(&n32Friendid,pchData,sizeof(UG_INT32));
				if(!addFriend(UG_INT32(dwParam),n32Friendid))
				{
					m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_CHAT_ADD_FRIEND_RETURN,(UG_DWORD)CHECK_OK,NULL,0,WRITE_WORLDBASE_OVERFLOW);
				}
				else
				{
					m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_CHAT_ADD_FRIEND_RETURN,(UG_DWORD)CHECK_ERROR,NULL,0,WRITE_WORLDBASE_OVERFLOW);
				}
			}
			else
			{
				m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_CHAT_ADD_FRIEND_RETURN,(UG_DWORD)CHECK_DATA_LEN_ERROR,NULL,0,WRITE_WORLDBASE_OVERFLOW);
				g_pLog->UGLog("msg NET_MSG_CHAT_ADD_FRIEND_RETURN data len error, len = %d.",ulLen);
			}
			break;
		}
	case NET_MSG_CHAT_SUB_FRIEND:
		{
			if(sizeof(UG_INT32) == ulLen)
			{
				UG_INT32 n32Friendid = 0;
				memcpy(&n32Friendid,pchData,sizeof(UG_INT32));
				if(!delFriend(UG_INT32(dwParam),n32Friendid))
				{
					m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_CHAT_SUB_FRIEND_RETURN,(UG_DWORD)CHECK_OK,NULL,0,WRITE_WORLDBASE_OVERFLOW);
				}
				else
				{
					m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_CHAT_SUB_FRIEND_RETURN,(UG_DWORD)CHECK_ERROR,NULL,0,WRITE_WORLDBASE_OVERFLOW);
				}
			}
			else
			{
				m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_CHAT_SUB_FRIEND_RETURN,(UG_DWORD)CHECK_DATA_LEN_ERROR,NULL,0,WRITE_WORLDBASE_OVERFLOW);
				g_pLog->UGLog("msg NET_MSG_CHAT_SUB_FRIEND_RETURN data len error, len = %d.",ulLen);
			}
			break;
		}
	case NET_MSG_BASE_FRIEND_LIST:
		{
			BASE_FRIEDN_LIST bf;
			if(sizeof(int) <= ulLen)
			{
				Q_memcpy(&(bf.num),pchData,sizeof(int));
				if(bf.num * sizeof(friend_t) + sizeof(int) == ulLen)
				{
					if(bf.num <= MAX_CHAT_FRIEND_LIST)
					{
						bf.friendlist = (friend_t*)(pchData + sizeof(int));
						if(!initFriends(UG_INT32(dwParam),&bf))
						{
							m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_BASE_FRIEND_LIST_RETURN,(UG_DWORD)CHECK_OK,NULL,0,WRITE_WORLDBASE_OVERFLOW);
						}
						else
						{
							m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_BASE_FRIEND_LIST_RETURN,(UG_DWORD)CHECK_ERROR,NULL,0,WRITE_WORLDBASE_OVERFLOW);
							g_pLog->UGLog("msg NET_MSG_BASE_FRIEND_LIST_RETURN error.");
						}
					}
					else
					{
						m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_BASE_FRIEND_LIST_RETURN,(UG_DWORD)CHECK_DATA_LEN_ERROR,NULL,0,WRITE_WORLDBASE_OVERFLOW);
						g_pLog->UGLog("msg NET_MSG_BASE_FRIEND_LIST_RETURN error bf.num = %d > %d.",bf.num,MAX_CHAT_FRIEND_LIST);
					}
				}
				else
				{
					m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_BASE_FRIEND_LIST_RETURN,(UG_DWORD)CHECK_DATA_LEN_ERROR,NULL,0,WRITE_WORLDBASE_OVERFLOW);
					g_pLog->UGLog("msg NET_MSG_BASE_FRIEND_LIST_RETURN error len = %d.",ulLen);
				}
			}
			else
			{
				g_pLog->UGLog("msg NET_MSG_BASE_FRIEND_LIST_RETURN error len = %d.",ulLen);
				m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_BASE_FRIEND_LIST_RETURN,(UG_DWORD)CHECK_DATA_LEN_ERROR,NULL,0,WRITE_WORLDBASE_OVERFLOW);
			}
			break;
		}
	case NET_MSG_CHAT_CURRENT:
		{
			getCurrentID((UG_INT32)dwParam,pchData,ulLen);
			break;
		}
	case NET_MSG_CHAT_UNION:
		{
			getUnionID((UG_INT32)dwParam,pchData,ulLen);
			break;
		}
	case NET_MSG_CHAT_STOP_TALK:
		{
			if(sizeof(UGDWORD) != ulLen)
			{
				g_pLog->UGLog("NET_MSG_CHAT_STOP_TALK error line = %d.",__LINE__);
			}
			else
			{
				CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
				UGLONG lBeginer = 0;
				time(&lBeginer);
				UGINT nRet = pWorldbase->m_pStopTalkManager->addPlayer((UG_INT32)dwParam,(UGDWORD)lBeginer,*((UGPDWORD)pchData));
			}
			break;
		}
//add by ben in 2005-10-11
	case NET_MSG_CHAT_SET_TALK:
		{
			CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
			UGINT nRet = pWorldbase->m_pStopTalkManager->setPlayer((UG_INT32)dwParam,TRUE);
			break;
		}
//end by ben
	case NET_MSG_CHAT_CREATE_TEAM:
		{
			if(!createTeam((UG_INT32)dwParam))
			{
				m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_CHAT_CREATE_TEAM_RETURN,(UG_DWORD)CHECK_OK,NULL,0,WRITE_WORLDBASE_OVERFLOW);
			}
			else
			{
				m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_CHAT_CREATE_TEAM_RETURN,(UG_DWORD)CHECK_ERROR,NULL,0,WRITE_WORLDBASE_OVERFLOW);
			}
		}
		break;
	case NET_MSG_CHAT_ADD_TO_TEAM:
		{
			if(sizeof(UG_INT32) == ulLen)
			{
				UG_PINT32 p = (UG_PINT32)pchData;
				if(!intoTeam(*p,(UG_INT32)dwParam))
				{
					m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_CHAT_ADD_TO_TEAM_RETURN,(UG_DWORD)CHECK_OK,NULL,0,WRITE_WORLDBASE_OVERFLOW);
				}
				else
				{
					m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_CHAT_ADD_TO_TEAM_RETURN,(UG_DWORD)CHECK_ERROR,NULL,0,WRITE_WORLDBASE_OVERFLOW);
				}
			}
			else
			{
				m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_CHAT_ADD_TO_TEAM_RETURN,(UG_DWORD)CHECK_DATA_LEN_ERROR,NULL,0,WRITE_WORLDBASE_OVERFLOW);
			}
			break;
		}
	case NET_MSG_CHAT_EXIT_TEAM:
		{
			if(sizeof(UG_INT32) == ulLen)
			{
				UG_PINT32 p = (UG_PINT32)pchData;
				if(!outTeam(*p,(UG_INT32)dwParam))
				{
					m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_CHAT_EXIT_TEAM_RETURN,(UG_DWORD)CHECK_OK,NULL,0,WRITE_WORLDBASE_OVERFLOW);
				}
				else
				{
					m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_CHAT_EXIT_TEAM_RETURN,(UG_DWORD)CHECK_ERROR,NULL,0,WRITE_WORLDBASE_OVERFLOW);
				}
			}
			else
			{
				g_pLog->UGLog("msg NET_MSG_CHAT_TEAM_EXIT error len = %d.",ulLen);
				m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_CHAT_EXIT_TEAM_RETURN,(UG_DWORD)CHECK_DATA_LEN_ERROR,NULL,0,WRITE_WORLDBASE_OVERFLOW);
			}
			break;
		}
//ben add in 2006-01-10
	case NET_MSG_WB_CHAT_SEND_MAIL:
		{
			if(recvSysMail(pchData,ulLen))
			{
				m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_WB_CHAT_SEND_MAIL_RETURN,(UG_DWORD)CHECK_ERROR,NULL,0,WRITE_WORLDBASE_OVERFLOW);
			}
			else
			{
				m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_WB_CHAT_SEND_MAIL_RETURN,(UG_DWORD)CHECK_OK,NULL,0,WRITE_WORLDBASE_OVERFLOW);
			}
			break;
		}
//end by ben
	default:
		{
			break;
		}
	}
	return 0;
}

UG_LONG CWBConnect::disconnectPlayer(UG_INT32 n32Playerid)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->delPlayer(n32Playerid));
	if(pPlayer)
	{
		pWorldbase->disconnectPlayer(pPlayer->m_pvConnect);
	}
	else
	{
		g_pLog->UGLog("disconnectPlayer in CWBConnect failed because playerid = %d not find.",n32Playerid);
	}
	m_pTCPComm->writeBuffer(m_pvConnect,NET_MSG_WBCHAT_DISCONNECT_RETURN,(UG_DWORD)n32Playerid);
	return 0;
}

UG_LONG CWBConnect::addPlayerInScene(PCHAT_SCENE p)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(p->nPlayerid));
	if(pPlayer)
	{
		return pPlayer->intoScene(p->nSceneid);
	}
	return -1;
}

UG_LONG CWBConnect::delPlayerInScene(PCHAT_SCENE p)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(p->nPlayerid));
	if(pPlayer)
	{
		return pPlayer->outScene(p->nSceneid);
	}
	return -1;
}


UG_LONG CWBConnect::cout(FILE* p)
{
	fprintf(p,"step = %d, online = %d.\n",m_stepWB,m_bOnline),
	fprintf(p,"worldbase ip = %s, port = %d, login timer = %d.\n",convertip(m_dwIP),m_wPort,m_dwTimer);
	return 0;
}

UG_LONG CWBConnect::setParent(UG_PVOID pvParent)
{
	m_pvParent = pvParent;
	return 0;
}

UG_LONG CWBConnect::initFriends(UG_INT32 n32Playerid,PBASE_FRIEDN_LIST p)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(n32Playerid));
	if(pPlayer)
	{
		return pPlayer->initFriends(p);
	}
	return -1;
}

UG_LONG CWBConnect::addFriend(UG_INT32 n32Playerid,UG_INT32 n32Friendid)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(n32Playerid));
	if(pPlayer)
	{
		return pPlayer->addFriend(n32Friendid,0);
	}
	return -1;
}

UG_LONG CWBConnect::changeFriend(UG_INT32 n32Playerid,friend_t f)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(n32Playerid));
	if(pPlayer)
	{
		return pPlayer->addFriend(f.id,(UG_INT32)(f.type));
	}
	return -1;
}

UG_LONG CWBConnect::delFriend(UG_INT32 n32Playerid,UG_INT32 n32Friendid)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(n32Playerid));
	if(pPlayer)
	{
		return pPlayer->delFriend(n32Friendid);
	}
	return -1;
}

UG_LONG CWBConnect::getCurrentID(UG_INT32 n32Playerid,UG_PCHAR p,UG_ULONG ulLen)
{
	static UG_INT32 n32ID[500];
	static CURRENT_ID cd;
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	if(ulLen < sizeof(cd.base))
	{
		g_pLog->UGLog("getCurrentID failed because ulLen = %d.",ulLen);
		return -1;
	}
	Q_memcpy(&(cd.base),p,sizeof((cd.base)));
	if(cd.base.num > 500)
	{
		g_pLog->UGLog("getCurrentID failed because n32Count = %d.",cd.base.num);
		return -1;
	}
	if(cd.base.num * sizeof(UG_INT32) + sizeof(cd.base) != ulLen)
	{
		g_pLog->UGLog("getCurrentID failed because n32Count = %d, ulLen = %d.",cd.base.num,ulLen);
		return -1;
	}
	Q_memcpy(n32ID,p + sizeof(cd.base),sizeof(UG_INT32) * cd.base.num);
	CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(n32Playerid));
	if(pPlayer)
	{
		return pPlayer->sendCurrentChannel(n32ID,cd.base.num,cd.base.idx);
	}
	else
	{
		g_pLog->UGLog("getCurrentID failed because pPlayer = %d not finded.",n32Playerid);
		return -1;
	}
}

UG_LONG	CWBConnect::createTeam(UG_INT32 n32Playerid)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(n32Playerid));
	if(pPlayer)
	{
		return pPlayer->createTeam(n32Playerid);
	}
	return -1;
}

UG_LONG	CWBConnect::intoTeam(UG_INT32 n32Playerid,INT32 n32Teamid) //n32Teamid为队长id
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(n32Playerid));
	if(pPlayer)
	{
		return pPlayer->intoTeam(n32Teamid);
	}
	return -1;
}

UG_LONG	CWBConnect::outTeam(UG_INT32 n32Playerid,INT32 n32Teamid) //n32Teamid为队长id
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(n32Playerid));
	if(pPlayer)
	{
		return pPlayer->outTeam(n32Teamid);
	}
	return -1;
}

UG_LONG CWBConnect::getUnionID(UG_INT32 n32Playerid,UG_PCHAR p,UG_ULONG ulLen)
{
	static UG_INT32 n32ID[500];
	static CURRENT_ID cd;
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	if(ulLen < sizeof(cd.base))
	{
		g_pLog->UGLog("getUnionID failed because ulLen = %d.",ulLen);
		return -1;
	}
	Q_memcpy(&(cd.base),p,sizeof((cd.base)));
	if(cd.base.num > 500)
	{
		g_pLog->UGLog("getUnionID failed because n32Count = %d.",cd.base.num);
		return -1;
	}
	if(cd.base.num * sizeof(UG_INT32) + sizeof(cd.base) != ulLen)
	{
		g_pLog->UGLog("getUnionID failed because n32Count = %d, ulLen = %d.",cd.base.num,ulLen);
		return -1;
	}
	Q_memcpy(n32ID,p + sizeof(cd.base),sizeof(UG_INT32) * cd.base.num);
	CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(n32Playerid));
	if(pPlayer)
	{
		return pPlayer->sendUnionChannel(n32ID,cd.base.num,cd.base.idx);
	}
	else
	{
		g_pLog->UGLog("getUnionID failed because pPlayer = %d not finded.",n32Playerid);
		return -1;
	}
	return 0;
}

UGINT CWBConnect::recvSysMail(UGPCHAR pchData,UGDWORD dwLen)
{
	static MAIL_MSG ms;
	static CMailMsg mm;
	if(dwLen < sizeof(ms.base))
	{
		g_pLog->UGLog("recvSysMail failed because ulLen = %d.",dwLen);
		return -1;
	}
	Q_memcpy(&(ms.base),pchData,sizeof(ms.base));
	if(ms.base.byNameLen + ms.base.byContentLen + sizeof(ms.base) != dwLen)
	{
		g_pLog->UGLog("recvSysMail failed because ulLen = %d.",dwLen);
		return -1;
	}
	time(&(mm.m_lTimer));
	mm.m_n32RecvID = ms.base.nRecvid;
	mm.m_n32SendID = ms.base.nSendid;
	Q_strncpy(mm.m_szSendNickName,pchData + sizeof(ms.base),min(MAX_NAME_LENGTH + 1,ms.base.byNameLen + 1));
	Q_strncpy(mm.m_szContent,pchData + sizeof(ms.base) + ms.base.byNameLen,min(MAX_CHAT_LENGTH,ms.base.byContentLen) + 1);
	
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	
	CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(mm.m_n32RecvID));
	if(pPlayer)
	{
		pPlayer->m_pMailMsgManager->push(&mm);
		pPlayer->sendMailCount();
	}
	else
	{
		UG_LONG lCount = CMailMsgManager::getMailCount(mm.m_n32RecvID);
		if(lCount >= MAX_MAIL_MSG_COUNT)
		{
			g_pLog->UGLog("getMailCount is %d > %d.",lCount,MAX_MAIL_MSG_COUNT);
			return __LINE__;
		}
		else
		{
			CMailMsgManager::sqlWriteMail(&mm);
		}
	}
	return 0;
}
