/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : Player.cpp
*owner        : Ben
*description  : 玩家基本类，包含玩家各种信息，操作等。
*modified     : 2004/12/20
******************************************************************************/ 

#include "incall.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlayer::CPlayer()
{
	m_dwIP = 0;
	m_wPort = 0;
	m_bOnline = FALSE;
	m_dwTimer = 0;
	m_pvConnect = NULL;
	m_pTCPComm = NULL;

	m_n32Playerid = 0;
	m_nAttribute = 0;
	m_nLevel = 0;
	Q_memset(m_szNickname,0,MAX_NAME_LENGTH + 1);
	
	m_pTeam = NULL;
	m_pUnion = NULL;
	m_pTradeRoom = NULL;
	m_pScene = NULL;
	m_stepPlayer = PLAYER_NOT_CONNECTED;
	m_pvParent = NULL;

	m_pchCurrentBuffer = NULL;
	m_ulCurrentTimer = 0;
	m_ulTeamTimer = 0;
	m_ulUnionTimer = 0;
	m_ulTradeTimer = 0;
	m_ulWorldTimer = 0;
	m_ulPrivateTimer = 0;
	m_lCurrentBufferPos = 0;
	m_n32CurrentPos = 0;
	m_pMailMsgManager = NULL;
	Q_memset(m_nTeamMember,0,sizeof(UGINT) * TEAM_MAX_PLAYER);
}

CPlayer::~CPlayer()
{
	cleanup();
}

UG_LONG CPlayer::cleanup()
{
	freeMemory();
	return 0;
}

UG_LONG CPlayer::dispatchMsg(UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR pchData,UG_ULONG ulLen)
{
	switch(dwMsgID)
	{
	case NET_MSG_WORLD_CHANNEL:
		{
			sendWorldChannel(pchData,ulLen);
			break;
		}
	case NET_MSG_PRIVATE_CHANNEL:
		{
			sendPrivateChannel(pchData,ulLen);
			break;
		}
	case NET_MSG_TRADE_CHANNEL:
		{
			sendTradeChannel(pchData,ulLen);
			break;
		}
	case NET_MSG_SYSTEM_CHANNEL:
		{
			break;
		}
	case NET_MSG_TEAM_CHANNEL:
		{
			sendTeamChannel((UG_INT32)dwParam,pchData,ulLen);
			break;
		}
	case NET_MSG_CURRENT_CHANNEL:
		{
			sendCurrentChannel((UG_INT32)dwParam,pchData,ulLen);
			break;
		}
	case NET_MSG_CHAT_GET_MAIL:
		{
			sendMail((UG_INT32)dwParam);
			break;
		}
	case NET_MSG_CHAT_SYS_TIME:
		{
			time_t lTimer;
			time(&lTimer);
			sendData(NET_MSG_CHAT_SYS_TIME_RETURN,(UG_DWORD)lTimer,NULL,0);
			break;
		}
	case NET_MSG_PLAYER_CLIENT_CONFIG:
		{
			setCommandConfig(pchData,ulLen);
			break;
		}
	case NET_MSG_CHAT_MAIL_COUNT:
		{
			m_pMailMsgManager->filterMail(this);
			m_pMailMsgManager->setMail(m_n32Playerid);
			m_pMailMsgManager->loadMail(m_n32Playerid);
			sendMailCount();
			break;
		}
	case NET_MSG_CHAT_SEND_MAIL:
		{
			recvMail(pchData,ulLen);
			break;
		}
	default:
		{
			break;
		}
	}
	return 0;
}

UG_LONG CPlayer::sendTradeChannel(UG_PCHAR p,UG_ULONG ulLen)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	static TRADE_CHANNEL tc;
	if(ulLen <= sizeof(tc.base))
	{
		g_pLog->UGLog("id = %d, sendTradeChannel error data len = %d <= sizeof(tc.base).",m_n32Playerid,ulLen);
		return -1;
	}
	Q_memcpy(&tc.base, p, sizeof(tc.base));
	if(tc.base.byNameLen != 0)
	{
		g_pLog->UGLog("id = %d, sendTradeChannel error name data len = %d != 0.",m_n32Playerid,tc.base.byNameLen);
		return -1;
	}
	if(tc.base.byContentLen <= 0)
	{
		g_pLog->UGLog("id = %d, sendTradeChannel error content_len = 0.",m_n32Playerid);
		return -1;
	}
	if(tc.base.byContentLen + sizeof(tc.base) != ulLen)
	{
		g_pLog->UGLog("id = %d, sendTradeChannel error tc.base.content_len = %d + sizeof(tc.base) = ulLen = %d.",m_n32Playerid,tc.base.byContentLen,ulLen);
		return -1;
	}
	if(tc.base.n32Playerid != m_n32Playerid)
	{
		g_pLog->UGLog("sendTradeChannel error player id = %d.",m_n32Playerid);
		return -1;
	}
	
	if(CHAT_CHANNEL_SELLBUY_TIMER > pWorldbase->m_dwRenderTimer - m_ulTradeTimer)
	{
		g_pLog->UGLog("id = %d, sendTradeChannel error time error %d, %d.",m_n32Playerid,pWorldbase->m_dwRenderTimer,m_ulTradeTimer);
		sendData(NET_MSG_TRADE_CHANNEL_RETURN,(UG_DWORD)CHECK_DATA_TIMEOUT,NULL,0);
		return UG_ULONG(-1);
	}
	
	m_ulTradeTimer = pWorldbase->m_dwRenderTimer;
	tc.base.byContentLen = min(MAX_CHAT_LENGTH,tc.base.byContentLen);
	tc.base.byNameLen = Q_strlen(m_szNickname);
	UG_PCHAR m_pchSendBuf = pWorldbase->m_pchSendBuf;
	Q_memcpy(m_pchSendBuf,&(tc.base),sizeof(tc.base));
	Q_memcpy(m_pchSendBuf + sizeof(tc.base),m_szNickname,tc.base.byNameLen);
	Q_memcpy(m_pchSendBuf + sizeof(tc.base) + tc.base.byNameLen,p + sizeof(tc.base),tc.base.byContentLen);
	UG_ULONG ulRet = 0;
	map<UG_INT32,UG_PVOID>::iterator it;
	for(it = pWorldbase->m_pPlayerManager->m_mapPlayer.begin(); it != pWorldbase->m_pPlayerManager->m_mapPlayer.end(); it ++)
	{
		CPlayer* pPlayer = (CPlayer*)(it->second);
		if(pPlayer->m_clConfig.channel_sellbuy_flag)
		{
			ulRet = pPlayer->sendData(NET_MSG_TRADE_CHANNEL_RETURN,0,m_pchSendBuf,sizeof(tc.base) + tc.base.byNameLen + tc.base.byContentLen);
		}
		if(-1 == ulRet)
		{
			g_pLog->UGLog("sendTradeChannel error player id = %d.",pPlayer->m_n32Playerid);
		}
	}
	return 0;
}

UG_LONG CPlayer::sendData(UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR p,UG_ULONG ulLen)
{
	if(m_pTCPComm)
	{
		if(m_pvConnect)
		{
			m_pTCPComm->sendData(m_pvConnect,dwMsgID,dwParam,p,ulLen);
			return 0;
		}
	}
	return UG_ULONG(-1);
}

UG_LONG CPlayer::sendWorldChannel(UG_PCHAR p,UG_ULONG ulLen)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	static WORLD_CHANNEL wc;
	if(ulLen <= sizeof(wc.base))
	{
		g_pLog->UGLog("id = %d, sendWorldChannel error data len = %d <= sizeof(wc.base).",m_n32Playerid,ulLen);
		return UG_ULONG(-1);
	}
	Q_memcpy(&wc.base, p, sizeof(wc.base));
	if(wc.base.name_len != 0)
	{
		g_pLog->UGLog("id = %d, sendWorldChannel error name data len = %d != 0.",m_n32Playerid,wc.base.name_len);
		return UG_ULONG(-1);
	}
	if(wc.base.content_len <= 0)
	{
		g_pLog->UGLog("id = %d, sendWorldChannel error content_len = 0.",m_n32Playerid);
		return UG_ULONG(-1);
	}
	if(wc.base.content_len + sizeof(wc.base) != ulLen)
	{
		g_pLog->UGLog("id = %d, sendWorldChannel error wc.base.content_len = %d + sizeof(wc.base) = ulLen = %d.",m_n32Playerid,wc.base.content_len,ulLen);
		return UG_ULONG(-1);
	}
	if(wc.base.id != m_n32Playerid)
	{
		g_pLog->UGLog("sendWorldChannel error player id = %d.",m_n32Playerid);
		return UG_ULONG(-1);
	}

	if(CHAT_CHANNEL_WORLD_TIMER > pWorldbase->m_dwRenderTimer - m_ulWorldTimer)
	{
		g_pLog->UGLog("id = %d, sendWorldChannel error time error %d, %d.",m_n32Playerid,pWorldbase->m_dwRenderTimer,m_ulWorldTimer);
		sendData(NET_MSG_WORLD_CHANNEL_RETURN,(UG_DWORD)CHECK_DATA_TIMEOUT,NULL,0);
		return UG_ULONG(-1);
	}

	m_ulWorldTimer = pWorldbase->m_dwRenderTimer;
	wc.base.content_len = min(MAX_CHAT_LENGTH,wc.base.content_len);
	wc.base.name_len = Q_strlen(m_szNickname);
	UG_PCHAR m_pchSendBuf = pWorldbase->m_pchSendBuf;
	Q_memcpy(m_pchSendBuf,&(wc.base),sizeof(wc.base));
	Q_memcpy(m_pchSendBuf + sizeof(wc.base),m_szNickname,wc.base.name_len);
	Q_memcpy(m_pchSendBuf + sizeof(wc.base) + wc.base.name_len,p + sizeof(wc.base),wc.base.content_len);

	if(m_nLevel >= WORLD_LEVEL) //发送全世界
	{
		UG_ULONG ulRet = 0;
		map<UG_INT32,UG_PVOID>::iterator it;
		for(it = pWorldbase->m_pPlayerManager->m_mapPlayer.begin(); it != pWorldbase->m_pPlayerManager->m_mapPlayer.end(); it ++)
		{
			CPlayer* pPlayer = (CPlayer*)(it->second);
			if(pPlayer->m_clConfig.channel_world_flag)
			{
				ulRet = pPlayer->sendData(NET_MSG_WORLD_CHANNEL_RETURN,0,m_pchSendBuf,sizeof(wc.base) + wc.base.name_len + wc.base.content_len);
			}
			if(-1 == ulRet)
			{
				g_pLog->UGLog("id = %d, sendWorldChannel error player id = %d.",m_n32Playerid,pPlayer->m_n32Playerid);
			}
		}
	}
	else //没有资格
	{
		g_pLog->UGLog("id = %d, sendWorldChannel level is %d < 10.",m_n32Playerid,m_nLevel);
	}
	return 0;
}

UG_LONG CPlayer::sendSystemChannel(UG_PCHAR p,UG_ULONG ulLen)
{
	return 0;
}

UG_LONG CPlayer::sendPrivateChannel(UG_PCHAR p,UG_ULONG ulLen)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	static PRIVATE_CHANNEL pc;
	if(ulLen < sizeof(pc.base))
	{
		g_pLog->UGLog("id = %d, sendPrivateChannel error data len = %d <= sizeof(pc.base).",m_n32Playerid,ulLen);
		return UG_ULONG(-1);
	}
	Q_memcpy(&(pc.base),p,sizeof(pc.base));
	if(pc.base.name_len > MAX_NAME_LENGTH)
	{
		g_pLog->UGLog("id = %d, sendPrivateChannel error nickname len = %d > 16.",m_n32Playerid,pc.base.name_len);
		return UG_ULONG(-1);
	}
	if(pc.base.content_len + pc.base.name_len + sizeof(pc.base) != ulLen)
	{
		g_pLog->UGLog("id = %d, sendPrivateChannel error data len = %d != pc.base.content_len = %d + pc.base.name_len = %d + sizeof(pc.base).",m_n32Playerid,ulLen,pc.base.name_len,pc.base.content_len);
		return UG_ULONG(-1);
	}
	if(pc.base.rec_id == m_n32Playerid)
	{
		g_pLog->UGLog("sendPrivateChannel error send to self = %d.",m_n32Playerid);
		return UG_ULONG(-1);
	}
	if(pc.base.id != m_n32Playerid)
	{
		g_pLog->UGLog("sendPrivateChannel error player id = %d.",m_n32Playerid);
		return UG_ULONG(-1);
	}
	if(pc.base.content_len < 1)
	{
		g_pLog->UGLog("id = %d, sendPrivateChannel error len = 0.",m_n32Playerid);
		return UG_ULONG(-1);
	}

	if(CHAT_CHANNEL_PERSON_TIMER > pWorldbase->m_dwRenderTimer - m_ulPrivateTimer)
	{
		g_pLog->UGLog("id = %d, sendPrivateChannel error time error %d, %d.",m_n32Playerid,pWorldbase->m_dwRenderTimer,m_ulPrivateTimer);
		sendData(NET_MSG_PRIVATE_CHANNEL_RETURN,(UG_DWORD)CHECK_DATA_TIMEOUT,NULL,0);
		return UG_ULONG(-1);
	}
	
	if(filterFriend(pc.base.rec_id))
	{
		g_pLog->UGLog("id = %d, sendPrivateChannel error is friend mask.",m_n32Playerid);
//		sendData(NET_MSG_PRIVATE_CHANNEL_RETURN,CHECK_DATA_FRIEND_MASK,NULL,0);
		return UG_ULONG(-1);
	}

	m_ulPrivateTimer = pWorldbase->m_dwRenderTimer;

	UG_INT32 nRemoved = pc.base.content_len - MAX_CHAT_LENGTH;
	if(nRemoved < 0) //不需要切除
	{
		nRemoved = 0;
	}
	map<UG_INT32,UG_PVOID>::iterator it;
	it = pWorldbase->m_pPlayerManager->m_mapPlayer.find(pc.base.rec_id);
	if(it != pWorldbase->m_pPlayerManager->m_mapPlayer.end())
	{
		unsigned char byTmp = pc.base.name_len;
		UG_PCHAR m_pchSendBuf = pWorldbase->m_pchSendBuf;
		pc.base.name_len = Q_strlen(m_szNickname);
		pc.base.content_len = min(MAX_CHAT_LENGTH,pc.base.content_len);
		Q_memcpy(m_pchSendBuf,&(pc.base),sizeof(pc.base));
		Q_memcpy(m_pchSendBuf + sizeof(pc.base),m_szNickname,pc.base.name_len);
		Q_memcpy(m_pchSendBuf + sizeof(pc.base) + pc.base.name_len,p + sizeof(pc.base) + byTmp,pc.base.content_len);
		CPlayer* pPlayer = (CPlayer*)(it->second);
		UG_ULONG ulRet = 0;
		if(pPlayer->m_clConfig.channel_person_flag)
		{
			ulRet = pPlayer->sendData(NET_MSG_PRIVATE_CHANNEL_RETURN,0,m_pchSendBuf,sizeof(pc.base) + pc.base.name_len + pc.base.content_len);
		}
		if(-1 == ulRet)
		{
			g_pLog->UGLog("sendPrivateChannel error player id = %d.",pPlayer->m_n32Playerid);
		}
		
		pc.base.name_len = Q_strlen(pPlayer->m_szNickname);
		Q_memcpy(m_pchSendBuf,&(pc.base),sizeof(pc.base));
		Q_memcpy(m_pchSendBuf + sizeof(pc.base),pPlayer->m_szNickname,pc.base.name_len);
		Q_memcpy(m_pchSendBuf + sizeof(pc.base) + pc.base.name_len,p + sizeof(pc.base) + byTmp,pc.base.content_len);
		
		if(m_clConfig.channel_person_flag)
		{
			ulRet = sendData(NET_MSG_PRIVATE_CHANNEL_RETURN,0,m_pchSendBuf,sizeof(pc.base) + pc.base.name_len + pc.base.content_len);
		}
		if(-1 == ulRet)
		{
			g_pLog->UGLog("sendPrivateChannel error player id = %d.",m_n32Playerid);
		}
	}
	else
	{
		g_pLog->UGLog("id = %d, sendPrivateChannel recv id not find.",m_n32Playerid);
	}
	return 0;
}

UG_LONG CPlayer::cout(FILE* p)
{
	fprintf(p,"id = %d, online = %d, login timer = %d.\n",m_n32Playerid,m_bOnline,m_dwTimer);
	fprintf(p,"ip = %s, port = %d, step = %d.\n",convertip(m_dwIP),m_wPort,m_stepPlayer);
	*(m_szNickname + MAX_NAME_LENGTH) = '\0';
	fprintf(p,"name = %s, attribute = %d, level = %d.\n",m_szNickname,m_nAttribute,m_nLevel);
	
	fprintf(p,"m_clConfig.channel_group_flag = %d.\n",m_clConfig.channel_group_flag);
	fprintf(p,"m_clConfig.channel_person_flag = %d.\n",m_clConfig.channel_person_flag);
	fprintf(p,"m_clConfig.channel_screen_flag = %d.\n",m_clConfig.channel_screen_flag);
	fprintf(p,"m_clConfig.channel_sellbuy_flag = %d.\n",m_clConfig.channel_sellbuy_flag);
	fprintf(p,"m_clConfig.channel_team_flag = %d.\n",m_clConfig.channel_team_flag);
	fprintf(p,"m_clConfig.channel_world_flag = %d.\n",m_clConfig.channel_world_flag);
	fprintf(p,"m_clConfig.fight_flag = %d.\n",m_clConfig.fight_flag);
	fprintf(p,"m_clConfig.friend_flag = %d.\n",m_clConfig.friend_flag);
	fprintf(p,"m_clConfig.item_flag = %d.\n",m_clConfig.item_flag);
	fprintf(p,"m_clConfig.mail_flag = %d.\n",m_clConfig.mail_flag);
	fprintf(p,"m_clConfig.strange_flag = %d.\n",m_clConfig.strange_flag);
	fprintf(p,"m_clConfig.team_flag = %d.\n",m_clConfig.team_flag);
	
	m_friends.cout(p);
	fprintf(p,"\n");
	return 0;
}

UG_LONG CPlayer::disconnect()
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	m_pTCPComm->disconnectPlayer(m_pvConnect);
	pWorldbase->m_pPlayerManager->delPlayer(m_n32Playerid);
	if(m_pTeam)
	{
		m_pTeam->delPlayer(m_n32Playerid);
		m_pTeam = NULL;
	}
	if(m_pUnion)
	{
		m_pUnion->delPlayer(m_n32Playerid);
		m_pUnion = NULL;
	}
	if(m_pTradeRoom)
	{
		m_pTradeRoom->delPlayer(m_n32Playerid);
		m_pTradeRoom = NULL;
	}
	if(m_pScene)
	{
		m_pScene->delPlayer(m_n32Playerid);
		m_pScene = NULL;
	}
	m_pMailMsgManager->saveMail();
	m_dwIP = 0;
	m_wPort = 0;
	m_bOnline = FALSE;
	m_dwTimer = 0;
	m_pvConnect = NULL;
	m_pTCPComm = NULL;
	m_n32Playerid = 0;
	m_nAttribute = 0;
	m_nLevel = 0;
	Q_memset(m_szNickname,0,MAX_NAME_LENGTH + 1);
	m_stepPlayer = PLAYER_NOT_CONNECTED;
	return 0;
}

UG_LONG CPlayer::playerLogin(PCHAT_CHECK_RETURN p)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	m_stepPlayer = PLAYER_CONNECT_STEP_OK;
	m_friends.cleanup();
	m_n32Playerid = p->nPlayerid;
	Q_strncpy(m_szNickname,p->szNickName,MAX_NAME_LENGTH + 1);
	m_nAttribute = p->nAttribute;
	m_nLevel = p->nLevel;
	pWorldbase->m_pPlayerManager->addPlayer(p->nPlayerid,this);
	m_pScene = (CScene*)(pWorldbase->m_pSceneManager->addPlayer(m_n32Playerid,p->nSceneID,this));
	g_pLog->UGLog("recvCheckPlayerid is ok, ip = %s, port = %d, id = %d.",convertip(m_dwIP),m_wPort,m_n32Playerid);
	
	m_ulCurrentTimer = 0;
	m_ulTeamTimer = 0;
	m_ulUnionTimer = 0;
	m_ulTradeTimer = 0;
	m_ulWorldTimer = 0;
	m_ulPrivateTimer = 0;

	m_lCurrentBufferPos = 0;
	m_n32CurrentPos = 0;
	
	m_clConfig.channel_group_flag = 1;
	m_clConfig.channel_person_flag = 1;
	m_clConfig.channel_screen_flag = 1;
	m_clConfig.channel_sellbuy_flag = 1;
	m_clConfig.channel_team_flag = 1;
	m_clConfig.channel_world_flag = 1;
	m_clConfig.fight_flag = 1;
	m_clConfig.friend_flag = 1;
	m_clConfig.item_flag = 1;
	m_clConfig.mail_flag = 1;
	m_clConfig.strange_flag = 1;
	m_clConfig.team_flag = 1;
	
	m_pMailMsgManager->reset(m_n32Playerid);

	Q_memset(m_nTeamMember,0,sizeof(UGINT) * TEAM_MAX_PLAYER);
	return 0;
}

UG_LONG CPlayer::intoScene(INT32 n32Sceneid)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	UG_PVOID pvScene = pWorldbase->m_pSceneManager->addPlayer(m_n32Playerid,n32Sceneid,this);
	if(pvScene)
	{
		g_pLog->UGLog("intoScene playerid = %d, scene id = %d.",m_n32Playerid,n32Sceneid);
		m_pScene = (CScene*)pvScene;
		return 0;
	}
	else
	{
		return UG_ULONG(-1);
	}
}

UG_LONG CPlayer::outScene(INT32 n32Sceneid)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	UG_PVOID pvScene = pWorldbase->m_pSceneManager->delPlayer(m_n32Playerid,n32Sceneid);
	if(pvScene)
	{
		g_pLog->UGLog("outScene playerid = %d, scene id = %d.",m_n32Playerid,n32Sceneid);
		m_pScene = NULL;
		return 0;
	}
	else
	{
		return UG_ULONG(-1);
	}
}

UG_LONG CPlayer::setParent(UG_PVOID pvParent)
{
	m_pvParent = pvParent;
	return 0;
}

UG_LONG CPlayer::addFriend(UG_INT32 n32Friendid,UG_INT32 n32Attribute)
{
	return m_friends.addFriends(n32Friendid,n32Attribute);
}

UG_LONG CPlayer::delFriend(UG_INT32 n32Friendid)
{
	return m_friends.delFriends(n32Friendid);
}

UG_LONG CPlayer::initFriends(PBASE_FRIEDN_LIST p)
{
	if(p->num > MAX_CHAT_FRIEND_LIST)
	{
		g_pLog->UGLog("initFriends failed num error, playerid = %d, num = %d.",m_n32Playerid,p->num);
		return -1;
	}
	for(UG_INT32 i = 0; i < p->num; i ++)
	{
		friend_t f = *(p->friendlist + i);
		if(m_friends.addFriends(f.id,(UG_INT32)(f.type)))
		{
			return -1;
		}
	}
	return 0;
}

UG_LONG CPlayer::sendCurrentChannel(UG_INT32 n32Playerid,UG_PCHAR p,UG_ULONG ulLen)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	if(n32Playerid != m_n32Playerid)
	{
		g_pLog->UGLog("sendCurrentChannel error because id %d != id %d.",n32Playerid,m_n32Playerid);
		return -1;
	}
	if(CHAT_CHANNEL_SCREEN_TIMER > pWorldbase->m_dwRenderTimer - m_ulCurrentTimer)
	{
		g_pLog->UGLog("id = %d, sendCurrentChannel error time error %d, %d.",m_n32Playerid,pWorldbase->m_dwRenderTimer,m_ulCurrentTimer);
		sendData(NET_MSG_CURRENT_CHANNEL_RETURN,(UG_DWORD)CHECK_DATA_TIMEOUT,NULL,0);
		return -1;
	}
	m_ulCurrentTimer = pWorldbase->m_dwRenderTimer;

	if(m_lCurrentBufferPos > 9)
	{
		g_pLog->UGLog("id = %d, sendCurrentChannel error because m_lCurrentBufferPos = %d.",m_n32Playerid,m_lCurrentBufferPos);
		Q_memmove(m_pchCurrentBuffer,m_pchCurrentBuffer + (MAX_CHAT_LENGTH + 1),(MAX_CHAT_LENGTH + 1) * 9);
		m_lCurrentBufferPos = 9;
	}
	UG_LONG lLen = UG_LONG(min(ulLen,MAX_CHAT_LENGTH));
	Q_strncpy(m_pchCurrentBuffer + m_lCurrentBufferPos * (MAX_CHAT_LENGTH + 1),p,lLen + 1);
	m_lCurrentBufferPos ++;
	m_n32CurrentPos ++;
	CURRENT_ID cd;
	cd.base.idx = m_n32CurrentPos;
	cd.base.num = 0;
	pWorldbase->m_pWorldbaseTCPComm->sendData(pWorldbase->m_pWBConnect->m_pvConnect,NET_MSG_CHAT_CURRENT_RETURN,n32Playerid,(UG_PCHAR)(&(cd.base)),sizeof(cd.base));
	return 0;
}

UG_LONG CPlayer::sendTeamChannel(UG_INT32 n32Playerid,UG_PCHAR p,UG_ULONG ulLen)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	if(n32Playerid != m_n32Playerid)
	{
		g_pLog->UGLog("sendTeamChannel error because id %d != id %d.",n32Playerid,m_n32Playerid);
		return -1;
	}
	if(CHAT_CHANNEL_TEAM_TIMER > pWorldbase->m_dwRenderTimer - m_ulTeamTimer)
	{
		g_pLog->UGLog("id = %d, sendTeamChannel error time error %d, %d.",m_n32Playerid,pWorldbase->m_dwRenderTimer,m_ulTeamTimer);
		sendData(NET_MSG_TEAM_CHANNEL_RETURN,(UG_DWORD)CHECK_DATA_TIMEOUT,NULL,0);
		return -1;
	}
	if(ulLen > MAX_CHAT_LENGTH)
	{
		g_pLog->UGLog("id = %d, sendTeamChannel error ulLen = %d > %d.",m_n32Playerid,ulLen,MAX_CHAT_LENGTH);
		return -1;
	}
	m_ulTeamTimer = pWorldbase->m_dwRenderTimer;
/*	if(m_pTeam)
	{
		m_pTeam->sendData(m_n32Playerid,p,ulLen);
	}
	else
	{
		g_pLog->UGLog("id = %d, sendTeamChannel error because m_pTeam is null.",m_n32Playerid);
	}*/
	if(m_nTeamMember[0] == n32Playerid) //是队长
	{
		sendTeamMember(n32Playerid,p,ulLen);
	}
	else
	{
		if(m_nTeamMember[0])
		{
			CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
			CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(m_nTeamMember[0]));
			if(pPlayer)
			{
				pPlayer->sendTeamMember(n32Playerid,p,ulLen);
			}
		}
	}
	return 0;
}

UG_LONG CPlayer::sendTeamMember(UG_INT32 nSenderid,UG_PCHAR p,UG_ULONG ulLen)
{
	sendData(NET_MSG_TEAM_CHANNEL_RETURN,nSenderid,p,ulLen);
	for(int i = 1; i < 5; i ++)
	{
		if(m_nTeamMember[i])
		{
			CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
			CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(m_nTeamMember[i]));
			if(pPlayer)
			{
				pPlayer->sendData(NET_MSG_TEAM_CHANNEL_RETURN,nSenderid,p,ulLen);
			}
		}
	}
	return 0;
}

UG_LONG CPlayer::sendCurrentChannel(UG_INT32 n32id[],UG_INT32 nCount,UG_INT32 n32Pos)
{
	UG_INT32 n32Offset = m_n32CurrentPos - n32Pos;
	UG_LONG lPos = m_lCurrentBufferPos - n32Offset - 1;
	if(lPos < 0)
	{
		g_pLog->UGLog("id = %d, sendCurrentChannel error pos = %d, lPos = %d, m_lCurrentBufferPos = %d.",m_n32Playerid,n32Pos,lPos,m_lCurrentBufferPos);
		return -1;
	}
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	if(m_lCurrentBufferPos > 0)
	{
		for(UG_INT32 i = 0; i < nCount; i ++)
		{
			CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(n32id[i]));
			if(pPlayer)
			{
				if(pPlayer->m_clConfig.channel_screen_flag)
				{
					pPlayer->sendData(NET_MSG_CURRENT_CHANNEL_RETURN,m_n32Playerid,m_pchCurrentBuffer + lPos * (MAX_CHAT_LENGTH + 1),Q_strlen(m_pchCurrentBuffer + lPos * (MAX_CHAT_LENGTH + 1)));
				}
				else
				{
					g_pLog->UGLog("id = %d, sendCurrentChannel is close.",m_n32Playerid);
				}
			}
			else
			{
				g_pLog->UGLog("id = %d, sendCurrentChannel error because id not finded = %d.",m_n32Playerid,n32id[i]);
			}
		}
		Q_memmove(m_pchCurrentBuffer,m_pchCurrentBuffer + (lPos + 1) * (MAX_CHAT_LENGTH + 1),(10 - lPos - 1) * (MAX_CHAT_LENGTH + 1));
		m_lCurrentBufferPos = m_lCurrentBufferPos - lPos - 1;
	}
	else
	{
		g_pLog->UGLog("id = %d, sendCurrentChannel error because m_lCurrentBufferPos <= 0.",m_n32Playerid);
		return -1;
	}
	return 0;
}

UG_LONG CPlayer::createUnion(INT32 n32Unionid)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	UG_PVOID pvUnion = pWorldbase->m_pUnionManager->addUnion(n32Unionid);
	if(pvUnion)
	{
		CUnion* pUnion = (CUnion*)pvUnion;
		m_pUnion = (CUnion*)(pUnion->addPlayer(m_n32Playerid,this));
		return 0;
	}
	else
	{
		return -1;
	}
}

UG_LONG CPlayer::intoUnion(INT32 n32Unionid)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	UG_PVOID pvUnion = pWorldbase->m_pUnionManager->addPlayer(m_n32Playerid,n32Unionid,this);
	m_pUnion = (CUnion*)pvUnion;
	if(pvUnion)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

UG_LONG CPlayer::outUnion(INT32 n32Unionid)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	UG_PVOID pvUnion = pWorldbase->m_pUnionManager->delPlayer(m_n32Playerid,n32Unionid);
	m_pUnion = NULL;
	if(pvUnion)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}


UG_LONG CPlayer::createTeam(INT32 n32Teamid)
{
	Q_memset(m_nTeamMember,0,sizeof(UGINT) * TEAM_MAX_PLAYER);
	m_nTeamMember[0] = n32Teamid;
/*	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	UG_PVOID pvTeam = pWorldbase->m_pTeamManager->addTeam(n32Teamid);
	if(pvTeam)
	{
		CTeam* pTeam = (CTeam*)pvTeam;
		m_pTeam = (CTeam*)(pTeam->addPlayer(m_n32Playerid,this));
		return 0;
	}
	else
	{
		return -1;
	}*/
	return 0;
}

UG_LONG CPlayer::intoTeam(INT32 n32Teamid) //n32Teamid为队长id
{
/*	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	UG_PVOID pvTeam = pWorldbase->m_pTeamManager->addPlayer(m_n32Playerid,n32Teamid,this);
	m_pTeam = (CTeam*)pvTeam;
	if(pvTeam)
	{
		return 0;
	}
	else
	{
		return -1;
	}*/
	m_nTeamMember[0] = n32Teamid;
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(n32Teamid));
	if(pPlayer)
	{
		if(n32Teamid == (pPlayer->m_nTeamMember)[0]) //是队长
		{
			for(int i = 1; i < 5; i ++)
			{
				if(!((pPlayer->m_nTeamMember)[i]))
				{
					(pPlayer->m_nTeamMember)[i] = m_n32Playerid;
					return 0;
				}
			}
		}
	}
	return -1;
}

UG_LONG CPlayer::outTeam(INT32 n32Teamid)
{
/*	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	UG_PVOID pvTeam = pWorldbase->m_pTeamManager->delPlayer(m_n32Playerid,n32Teamid);
	m_pTeam = NULL;
	if(pvTeam)
	{
		return 0;
	}
	else
	{
		return -1;
	}*/
	if(m_nTeamMember[0] == m_n32Playerid) //是队长
	{
		for(int i = 1; i < 5; i ++)
		{
			CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
			if((m_nTeamMember)[i])
			{
				CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(m_nTeamMember[i]));
				if(pPlayer)
				{
					Q_memset((pPlayer->m_nTeamMember),0,sizeof(UGINT) * TEAM_MAX_PLAYER);
				}
			}
		}
		Q_memset(m_nTeamMember,0,sizeof(UGINT) * TEAM_MAX_PLAYER);
		return 0;
	}
	else
	{
		Q_memset(m_nTeamMember,0,sizeof(UGINT) * TEAM_MAX_PLAYER);
		CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
		CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(n32Teamid));
		if(pPlayer)
		{
			if(n32Teamid == (pPlayer->m_nTeamMember)[0]) //是队长
			{
				for(int i = 1; i < 5; i ++)
				{
					if(m_n32Playerid == (pPlayer->m_nTeamMember)[i])
					{
						(pPlayer->m_nTeamMember)[i] = 0;
						return 0;
					}
				}
			}
		}
		return -1;
	}
}

UG_LONG CPlayer::init()
{
	mallicMemory();
	return 0;
}

UG_LONG CPlayer::sendMail(UG_INT32 n32Playerid)
{
	CMailMsg* pMail = m_pMailMsgManager->pop();
	if(pMail)
	{
		CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
		UG_PCHAR pchSendBuffer = pWorldbase->m_pchSendBuf;
		static MAIL_MSG ms;
		ms.base.lTimer = pMail->m_lTimer;
		ms.base.byNameLen = Q_strlen(pMail->m_szSendNickName);
		ms.base.nSendid = pMail->m_n32SendID;
		ms.base.nRecvid = pMail->m_n32RecvID;
		ms.base.byContentLen = Q_strlen(pMail->m_szContent);
		Q_memcpy(pchSendBuffer,&(ms.base),sizeof(ms.base));
		Q_memcpy(pchSendBuffer + sizeof(ms.base),pMail->m_szSendNickName,ms.base.byNameLen);
		Q_memcpy(pchSendBuffer + sizeof(ms.base) + ms.base.byNameLen,pMail->m_szContent,ms.base.byContentLen);
		sendData(NET_MSG_CHAT_GET_MAIL_RETURN,0,pchSendBuffer,sizeof(ms.base) + ms.base.byNameLen + ms.base.byContentLen);
	}
	else
	{
		g_pLog->UGLog("CMailMsg pop is NULL.");
		return -1;
	}
	return 0;
}

UG_LONG CPlayer::sendMailCount()
{
	UG_LONG lCount = m_pMailMsgManager->size();
	if(lCount > 0)
	{
		g_pLog->UGLog("sendMailCount count is %d.",lCount);
		sendData(NET_MSG_CHAT_MAIL_COUNT_RETURN,(UG_DWORD)lCount,NULL,0);
	}
	return 0;
}

UG_LONG CPlayer::recvMail(UG_PCHAR p,UG_ULONG ulLen)
{
	static MAIL_MSG ms;
	static CMailMsg mm;
	if(ulLen < sizeof(ms.base))
	{
		g_pLog->UGLog("recvMail failed because ulLen = %d.",ulLen);
		return -1;
	}
	Q_memcpy(&(ms.base),p,sizeof(ms.base));
	if(ms.base.byNameLen + ms.base.byContentLen + sizeof(ms.base) != ulLen)
	{
		g_pLog->UGLog("recvMail failed because ulLen = %d.",ulLen);
		return -1;
	}
	time(&(mm.m_lTimer));
	mm.m_n32RecvID = ms.base.nRecvid;
	mm.m_n32SendID = m_n32Playerid;
	Q_strncpy(mm.m_szSendNickName,m_szNickname,MAX_NAME_LENGTH + 1);
	Q_strncpy(mm.m_szContent,p + sizeof(ms.base) + ms.base.byNameLen,min(MAX_CHAT_LENGTH,ms.base.byContentLen) + 1);

	UG_LONG lRet = 0;
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(mm.m_n32RecvID));
	if(pPlayer)
	{
		if(!(pPlayer->m_clConfig.mail_flag))
		{
			g_pLog->UGLog("recvMail failed because mail_flag is closed.");
			return -1;
		}
		if(pPlayer->filterFriend(m_n32Playerid))
		{
			g_pLog->UGLog("recvMail friend is mask.");
//			sendData(NET_MSG_CHAT_SEND_MAIL_RETURN,CHECK_DATA_FRIEND_MASK,NULL,0);
			return -1;
		}
		if(pPlayer->m_pMailMsgManager->push(&mm))
		{
			g_pLog->UGLog("recvMail sqlWriteMail id = %d.",mm.m_n32RecvID);
			lRet = 0;
		}
		else
		{
			g_pLog->UGLog("recvMail push id = %d.",mm.m_n32RecvID);
			lRet = -1;
		}
		pPlayer->sendMailCount();
	}
	else
	{
		g_pLog->UGLog("recvMail player not finded id = %d.",mm.m_n32RecvID);
		UG_LONG lCount = CMailMsgManager::getMailCount(mm.m_n32RecvID);
		if(lCount >= MAX_MAIL_MSG_COUNT)
		{
			g_pLog->UGLog("getMailCount is %d > %d.",lCount,MAX_MAIL_MSG_COUNT);
			lRet = -1;
		}
		else
		{
			CMailMsgManager::sqlWriteMail(&mm);
			lRet = 0;
		}
	}
	if(lRet)
	{
		sendData(NET_MSG_CHAT_SEND_MAIL_RETURN,(UG_DWORD)CHECK_DATA_MAILOUT,NULL,0);
	}
	else
	{
		sendData(NET_MSG_CHAT_SEND_MAIL_RETURN,(UG_DWORD)CHECK_OK,NULL,0);
	}
	return 0;
}

UG_LONG CPlayer::filterFriend(UG_INT32 n32Playerid)
{
	CWorldbase* pWorldbase = (CWorldbase*)m_pvParent;
	CPlayer* pPlayer = (CPlayer*)(pWorldbase->m_pPlayerManager->findPlayer(n32Playerid));
	if(pPlayer)
	{
		UG_INT32 n32Attribute = 0;
		UG_BOOL bFind = pPlayer->m_friends.findFriends(m_n32Playerid,n32Attribute);
		if(bFind)
		{
			if(FRIEND_MASK == n32Attribute)
			{
				return -1;
			}
			else
			{
				return 0;
			}
		}
		return 0;
	}
	return -1;
}

UG_LONG CPlayer::setCommandConfig(UG_PCHAR pchData,UG_ULONG ulLen)
{
	if(ulLen != sizeof(CLIENT_CONFIG))
	{
		g_pLog->UGLog("setCommandConfig is error ulLen = %d.",ulLen);
		return -1;
	}
	Q_memcpy(&m_clConfig,pchData,ulLen);
	return 0;
}

UG_LONG CPlayer::mallicMemory()
{
	m_pMailMsgManager = new CMailMsgManager;
	m_pMailMsgManager->init(MAX_MAIL_MSG_SQL);
	return 0;
}

UG_LONG CPlayer::freeMemory()
{
	ReleaseP<CMailMsgManager*>(m_pMailMsgManager);
	return 0;
}






















