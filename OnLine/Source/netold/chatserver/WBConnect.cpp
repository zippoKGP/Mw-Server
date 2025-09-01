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
					m_pTCPComm->sendData(m_pvConnect,NET_MSG_WBCHAT_SCENE_ENTER_RETURN,(UG_DWORD)CHECK_OK);
				}
				else
				{
					m_pTCPComm->sendData(m_pvConnect,NET_MSG_WBCHAT_SCENE_ENTER_RETURN,(UG_DWORD)CHECK_ERROR);
				}
			}
			else
			{
				m_pTCPComm->sendData(m_pvConnect,NET_MSG_WBCHAT_SCENE_ENTER_RETURN,(UG_DWORD)CHECK_DATA_LEN_ERROR);
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
					m_pTCPComm->sendData(m_pvConnect,NET_MSG_WBCHAT_SCENE_EXIT_RETURN,(UG_DWORD)CHECK_OK);
				}
				else
				{
					m_pTCPComm->sendData(m_pvConnect,NET_MSG_WBCHAT_SCENE_EXIT_RETURN,(UG_DWORD)CHECK_ERROR);
				}
			}
			else
			{
				m_pTCPComm->sendData(m_pvConnect,NET_MSG_WBCHAT_SCENE_EXIT_RETURN,(UG_DWORD)CHECK_DATA_LEN_ERROR);
				g_pLog->UGLog("msg NET_MSG_WBCHAT_SCENE_EXIT_RETURN data len error, len = %d.",ulLen);
			}
			break;
		}
	case NET_MSG_WBCHAT_DISCONNECT:
		{
			disconnectPlayer(UG_INT32(dwParam));
			break;
		}
	case NET_MSG_CHAT_CHANG_FRIEND:
		{
			if(sizeof(friend_t) == ulLen)
			{
				friend_t f;
				memcpy(&f,pchData,sizeof(friend_t));
				if(!changeFriend(UG_INT32(dwParam),f))
				{
					m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_CHANG_FRIEND_RETURN,(UG_DWORD)CHECK_OK);
				}
				else
				{
					m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_CHANG_FRIEND_RETURN,(UG_DWORD)CHECK_ERROR);
				}
			}
			else
			{
				m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_ADD_FRIEND_RETURN,(UG_DWORD)CHECK_DATA_LEN_ERROR);
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
					m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_ADD_FRIEND_RETURN,(UG_DWORD)CHECK_OK);
				}
				else
				{
					m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_ADD_FRIEND_RETURN,(UG_DWORD)CHECK_ERROR);
				}
			}
			else
			{
				m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_ADD_FRIEND_RETURN,(UG_DWORD)CHECK_DATA_LEN_ERROR);
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
					m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_SUB_FRIEND_RETURN,(UG_DWORD)CHECK_OK);
				}
				else
				{
					m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_SUB_FRIEND_RETURN,(UG_DWORD)CHECK_ERROR);
				}
			}
			else
			{
				m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_SUB_FRIEND_RETURN,(UG_DWORD)CHECK_DATA_LEN_ERROR);
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
							m_pTCPComm->sendData(m_pvConnect,NET_MSG_BASE_FRIEND_LIST_RETURN,(UG_DWORD)CHECK_OK);
						}
						else
						{
							m_pTCPComm->sendData(m_pvConnect,NET_MSG_BASE_FRIEND_LIST_RETURN,(UG_DWORD)CHECK_ERROR);
							g_pLog->UGLog("msg NET_MSG_BASE_FRIEND_LIST_RETURN error.");
						}
					}
					else
					{
						m_pTCPComm->sendData(m_pvConnect,NET_MSG_BASE_FRIEND_LIST_RETURN,(UG_DWORD)CHECK_DATA_LEN_ERROR);
						g_pLog->UGLog("msg NET_MSG_BASE_FRIEND_LIST_RETURN error bf.num = %d > %d.",bf.num,MAX_CHAT_FRIEND_LIST);
					}
				}
				else
				{
					m_pTCPComm->sendData(m_pvConnect,NET_MSG_BASE_FRIEND_LIST_RETURN,(UG_DWORD)CHECK_DATA_LEN_ERROR);
					g_pLog->UGLog("msg NET_MSG_BASE_FRIEND_LIST_RETURN error len = %d.",ulLen);
				}
			}
			else
			{
				g_pLog->UGLog("msg NET_MSG_BASE_FRIEND_LIST_RETURN error len = %d.",ulLen);
				m_pTCPComm->sendData(m_pvConnect,NET_MSG_BASE_FRIEND_LIST_RETURN,(UG_DWORD)CHECK_DATA_LEN_ERROR);
			}
			break;
		}
	case NET_MSG_CHAT_CURRENT:
		{
			getCurrentID((UG_INT32)dwParam,pchData,ulLen);
			break;
		}
	case NET_MSG_CHAT_ADD_UNION:
		{
			if(sizeof(UG_INT32) == ulLen)
			{
				UG_PINT32 p = (UG_PINT32)pchData;
				if(!createUnion((UG_INT32)dwParam,*p))
				{
					g_pLog->UGLog("msg NET_MSG_CHAT_ADD_UNION ok.");
					m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_ADD_UNION_RETURN,(UG_DWORD)CHECK_OK);
				}
				else
				{
					g_pLog->UGLog("msg NET_MSG_CHAT_ADD_UNION error.");
					m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_ADD_UNION_RETURN,(UG_DWORD)CHECK_ERROR);
				}
			}
			else
			{
				g_pLog->UGLog("msg NET_MSG_CHAT_ADD_UNION error len = %d.",ulLen);
				m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_ADD_UNION_RETURN,(UG_DWORD)CHECK_DATA_LEN_ERROR);
			}
			break;
		}
	case NET_MSG_CHAT_SUB_UNION:
		{
			if(sizeof(UG_INT32) == ulLen)
			{
				UG_PINT32 p = (UG_PINT32)pchData;
				if(!dropUnion((UG_INT32)dwParam,*p))
				{
					g_pLog->UGLog("msg NET_MSG_CHAT_SUB_UNION ok.");
					m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_SUB_UNION_RETURN,(UG_DWORD)CHECK_OK);
				}
				else
				{
					g_pLog->UGLog("msg NET_MSG_CHAT_SUB_UNION error.");
					m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_SUB_UNION_RETURN,(UG_DWORD)CHECK_ERROR);
				}
			}
			else
			{
				g_pLog->UGLog("msg NET_MSG_CHAT_SUB_UNION error len = %d.",ulLen);
				m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_SUB_UNION_RETURN,(UG_DWORD)CHECK_DATA_LEN_ERROR);
			}
			break;
		}
	case NET_MSG_CHAT_UNION_ENTER:
		{
			if(sizeof(UG_INT32) == ulLen)
			{
				UG_PINT32 p = (UG_PINT32)pchData;
				if(!intoUnion((UG_INT32)dwParam,*p))
				{
					g_pLog->UGLog("msg NET_MSG_CHAT_UNION_ENTER ok.");
					m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_UNION_ENTER_RETURN,(UG_DWORD)CHECK_OK);
				}
				else
				{
					g_pLog->UGLog("msg NET_MSG_CHAT_UNION_ENTER error.");
					m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_UNION_ENTER_RETURN,(UG_DWORD)CHECK_ERROR);
				}
			}
			else
			{
				g_pLog->UGLog("msg NET_MSG_CHAT_UNION_ENTER error len = %d.",ulLen);
				m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_UNION_ENTER_RETURN,(UG_DWORD)CHECK_DATA_LEN_ERROR);
			}
			break;
		}
	case NET_MSG_CHAT_UNION_EXIT:
		{
			if(sizeof(UG_INT32) == ulLen)
			{
				UG_PINT32 p = (UG_PINT32)pchData;
				if(!outUnion((UG_INT32)dwParam,*p))
				{
					g_pLog->UGLog("msg NET_MSG_CHAT_UNION_EXIT ok.");
					m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_UNION_EXIT_RETURN,(UG_DWORD)CHECK_OK);
				}
				else
				{
					g_pLog->UGLog("msg NET_MSG_CHAT_UNION_EXIT error.");
					m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_UNION_EXIT_RETURN,(UG_DWORD)CHECK_ERROR);
				}
			}
			else
			{
				g_pLog->UGLog("msg NET_MSG_CHAT_UNION_EXIT error len = %d.",ulLen);
				m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_UNION_EXIT_RETURN,(UG_DWORD)CHECK_DATA_LEN_ERROR);
			}
			break;
		}
	case NET_MSG_CHAT_CREATE_TEAM:
		{
			if(!createTeam((UG_INT32)dwParam))
			{
				g_pLog->UGLog("msg NET_MSG_CHAT_ADD_TEAM ok.");
				m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_CREATE_TEAM_RETURN,(UG_DWORD)CHECK_OK);
			}
			else
			{
				g_pLog->UGLog("msg NET_MSG_CHAT_ADD_TEAM error.");
				m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_CREATE_TEAM_RETURN,(UG_DWORD)CHECK_ERROR);
			}
		}
		break;
/*	case NET_MSG_CHAT_SUB_TEAM:
		{
			if(sizeof(UG_INT32) == ulLen)
			{
				UG_PINT32 p = (UG_PINT32)pchData;
				if(!dropTeam((UG_INT32)dwParam,*p))
				{
					g_pLog->UGLog("msg NET_MSG_CHAT_SUB_TEAM ok.");
					m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_SUB_TEAM_RETURN,(UG_DWORD)CHECK_OK);
				}
				else
				{
					g_pLog->UGLog("msg NET_MSG_CHAT_SUB_TEAM error.");
					m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_SUB_TEAM_RETURN,(UG_DWORD)CHECK_ERROR);
				}
			}
			else
			{
				g_pLog->UGLog("msg NET_MSG_CHAT_SUB_TEAM error len = %d.",ulLen);
				m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_SUB_TEAM_RETURN,(UG_DWORD)CHECK_DATA_LEN_ERROR);
			}
			break;
		}*/
	case NET_MSG_CHAT_ADD_TO_TEAM:
		{
			if(sizeof(UG_INT32) == ulLen)
			{
				UG_PINT32 p = (UG_PINT32)pchData;
				if(!intoTeam(*p,(UG_INT32)dwParam))
				{
					g_pLog->UGLog("msg NET_MSG_CHAT_TEAM_ENTER ok.");
					m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_ADD_TO_TEAM_RETURN,(UG_DWORD)CHECK_OK);
				}
				else
				{
					g_pLog->UGLog("msg NET_MSG_CHAT_TEAM_ENTER error.");
					m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_ADD_TO_TEAM_RETURN,(UG_DWORD)CHECK_ERROR);
				}
			}
			else
			{
				g_pLog->UGLog("msg NET_MSG_CHAT_TEAM_ENTER error len = %d.",ulLen);
				m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_ADD_TO_TEAM_RETURN,(UG_DWORD)CHECK_DATA_LEN_ERROR);
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
					g_pLog->UGLog("msg NET_MSG_CHAT_TEAM_EXIT ok.");
					m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_EXIT_TEAM_RETURN,(UG_DWORD)CHECK_OK);
				}
				else
				{
					g_pLog->UGLog("msg NET_MSG_CHAT_TEAM_EXIT error.");
					m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_EXIT_TEAM_RETURN,(UG_DWORD)CHECK_ERROR);
				}
			}
			else
			{
				g_pLog->UGLog("msg NET_MSG_CHAT_TEAM_EXIT error len = %d.",ulLen);
				m_pTCPComm->sendData(m_pvConnect,NET_MSG_CHAT_EXIT_TEAM_RETURN,(UG_DWORD)CHECK_DATA_LEN_ERROR);
			}
			break;
		}
	default:
		{
			g_pLog->UGLog("default msg id = %d, param = %d, datalen = %d.",dwMsgID,dwParam,ulLen);
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
		g_pLog->UGLog("disconnectPlayer in CWBConnect ok playerid = %d.",n32Playerid);
		pPlayer->m_bOnline = FALSE;
		pWorldbase->disconnectPlayer(pPlayer->m_pvConnect);
	}
	else
	{
		g_pLog->UGLog("disconnectPlayer in CWBConnect failed because playerid = %d not find.",n32Playerid);
	}
	m_pTCPComm->sendData(m_pvConnect,NET_MSG_WBCHAT_DISCONNECT_RETURN,(UG_DWORD)n32Playerid);
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
	else
	{
		g_pLog->UGLog("addPlayerInScene in CWBConnect failed because playerid = %d not find.",p->nPlayerid);
		return -1;
	}
}

UG_LONG CWBConnect::delPlayerInScene(PCHAT_SCENE p)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(p->nPlayerid));
	if(pPlayer)
	{
		return pPlayer->outScene(p->nSceneid);
	}
	else
	{
		g_pLog->UGLog("delPlayerInScene in CWBConnect failed because playerid = %d not find.",p->nPlayerid);
		return -1;
	}
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
	else
	{
		g_pLog->UGLog("initFriends in CWBConnect failed.");
		return -1;
	}
}

UG_LONG CWBConnect::addFriend(UG_INT32 n32Playerid,UG_INT32 n32Friendid)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(n32Playerid));
	if(pPlayer)
	{
		return pPlayer->addFriend(n32Friendid,0);
	}
	else
	{
		g_pLog->UGLog("addFriend in CWBConnect failed because playerid = %d not find.",n32Playerid);
		return -1;
	}
}

UG_LONG CWBConnect::changeFriend(UG_INT32 n32Playerid,friend_t f)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(n32Playerid));
	if(pPlayer)
	{
		return pPlayer->addFriend(f.id,(UG_INT32)(f.type));
	}
	else
	{
		g_pLog->UGLog("changeFriend in CWBConnect failed because playerid = %d not find.",n32Playerid);
		return -1;
	}
}

UG_LONG CWBConnect::delFriend(UG_INT32 n32Playerid,UG_INT32 n32Friendid)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(n32Playerid));
	if(pPlayer)
	{
		return pPlayer->delFriend(n32Friendid);
	}
	else
	{
		g_pLog->UGLog("delFriend in CWBConnect failed because playerid = %d not find.",n32Playerid);
		return -1;
	}
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

UG_LONG	CWBConnect::createUnion(UG_INT32 n32Playerid,INT32 n32Unionid)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(n32Playerid));
	if(pPlayer)
	{
		return pPlayer->createUnion(n32Unionid);
	}
	else
	{
		g_pLog->UGLog("createUnion in CWBConnect failed.");
		return -1;
	}
}

UG_LONG CWBConnect::dropUnion(UG_INT32 n32Playerid,UG_INT32 n32Unionid)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	UG_PVOID pvUnion = pWorldbase->m_pUnionManager->delUnion(n32Unionid);
	if(pvUnion)
	{
		return 0;
	}
	else
	{
		g_pLog->UGLog("dropUnion in CWBConnect failed.");
		return -1;
	}
}

UG_LONG	CWBConnect::intoUnion(UG_INT32 n32Playerid,INT32 n32Unionid)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(n32Playerid));
	if(pPlayer)
	{
		return pPlayer->intoUnion(n32Unionid);
	}
	else
	{
		g_pLog->UGLog("intoUnion in CWBConnect failed.");
		return -1;
	}
}

UG_LONG	CWBConnect::outUnion(UG_INT32 n32Playerid,INT32 n32Unionid)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(n32Playerid));
	if(pPlayer)
	{
		return pPlayer->outUnion(n32Unionid);
	}
	else
	{
		g_pLog->UGLog("outUnion in CWBConnect failed.");
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
	else
	{
		g_pLog->UGLog("createTeam in CWBConnect failed.");
		return -1;
	}
}
/*
UG_LONG CWBConnect::dropTeam(UG_INT32 n32Playerid,UG_INT32 n32Teamid)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	UG_PVOID pvUnion = pWorldbase->m_pTeamManager->delTeam(n32Teamid);
	if(pvUnion)
	{
		return 0;
	}
	else
	{
		g_pLog->UGLog("dropTeam in CWBConnect failed.");
		return -1;
	}
}
*/
UG_LONG	CWBConnect::intoTeam(UG_INT32 n32Playerid,INT32 n32Teamid) //n32Teamid为队长id
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(n32Playerid));
	if(pPlayer)
	{
		return pPlayer->intoTeam(n32Teamid);
	}
	else
	{
		g_pLog->UGLog("intoTeam in CWBConnect failed.");
		return -1;
	}
}

UG_LONG	CWBConnect::outTeam(UG_INT32 n32Playerid,INT32 n32Teamid) //n32Teamid为队长id
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(n32Playerid));
	if(pPlayer)
	{
		return pPlayer->outTeam(n32Teamid);
	}
	else
	{
		g_pLog->UGLog("outTeam in CWBConnect failed.");
		return -1;
	}
}

