// Worldbase.cpp: implementation of the CWorldbase class.
//
//////////////////////////////////////////////////////////////////////

#include "incall.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWorldbase::CWorldbase()
{
	m_pSQLChatMail = NULL;
	m_pWorldbaseTCPComm = NULL;
	m_pPlayerTCPComm = NULL;
	m_pSysModule = NULL;
	m_pWBConnect = NULL;
	m_pPlayer = NULL;
	m_listPlayerIdle.clear();
	m_mapPlayer.clear();
	m_pPlayerManager = NULL;
	m_pSceneManager = NULL;
	m_pTeamManager = NULL;
	m_pTradeRoomManager = NULL;
	m_pUnionManager = NULL;
	m_pWorldbaseConfig = NULL;
	m_dwRenderTimer = 0;
	m_pchSendBuf = NULL;
	m_pchPlayerCurrentBuffer = NULL;
}

CWorldbase::~CWorldbase()
{
	cleanup();
}

UG_LONG CWorldbase::init(CWorldbaseConfig* pWorldbaseConfig,CSysModule* pSysModule,INetServer* pWorldbaseTCPComm)
{
	m_pWorldbaseConfig = pWorldbaseConfig;
	m_pSysModule = pSysModule;
	m_pWorldbaseTCPComm = pWorldbaseTCPComm;

	m_listPlayerIdle.clear();
	m_mapPlayer.clear();
	UG_LONG lRet = mallocMemory();
	if(lRet)
	{
		g_pLog->UGLog("mallocMemory error");
		return (UG_ULONG)(-1);
	}
	printf("malloc memory succeed.\n");
	lRet = initMailSQL();
	if(lRet)
	{
		g_pLog->UGLog("initMailSQL error");
		return (UG_ULONG)(-1);
	}
	printf("initMailSQL succeed.\n");
	lRet = initPlayerTCPComm();
	if(lRet)
	{
		g_pLog->UGLog("initPlayerTCPComm error.");
		return (UG_ULONG)(-1);
	}
	printf("initPlayerTCPComm succeed.\n");
	return 0;
}

UG_LONG CWorldbase::render(UG_ULONG ulTimer)
{
	m_dwRenderTimer = ulTimer;
	CSyncEvent sys(&m_cs);
	m_pPlayerTCPComm->acceptPlayer();
	map<UG_PVOID,CPlayer*>::iterator it;
	for(it = m_mapPlayer.begin(); it != m_mapPlayer.end();)
	{
		UG_ULONG ulRet = m_pPlayerTCPComm->recvData((it->second)->m_pvConnect,0);
		if(-1 == ulRet)
		{
			g_pLog->UGLog("player disconnected in recvdata, ip = %s, port = %d.",convertip((it->second)->m_dwIP),(it->second)->m_wPort);
			it->second->m_bOnline = FALSE;
		}
		if(it->second->m_bOnline)
		{
			it ++;
		}
		else
		{
			g_pLog->UGLog("player disconnected in sendbuffer, ip = %s, port = %d.",convertip((it->second)->m_dwIP),(it->second)->m_wPort);
			it->second->disconnect();
			m_listPlayerIdle.push_back(it->second);
			it = m_mapPlayer.erase(it);
		}
	}
	for(it = m_mapPlayer.begin(); it != m_mapPlayer.end(); it ++)
	{
		if(it->second->m_bOnline)
		{
			m_pPlayerTCPComm->sendBuffer((it->second)->m_pvConnect,0);
		}
	}
	return 0;
}

UG_LONG CWorldbase::cleanup()
{
	if(m_pPlayerTCPComm)
	{
		m_pPlayerTCPComm->shutdown();
		m_pPlayerTCPComm = NULL;
	}
	m_listPlayerIdle.clear();
	m_mapPlayer.clear();
	freeMemory();
	return 0;
}

UG_LONG CWorldbase::mallocMemory()
{
	m_pchPlayerCurrentBuffer = UGMalloc<UG_CHAR>((m_pWorldbaseConfig->m_ulMaxPlayer) * 10 * (MAX_CHAT_LENGTH + 1),__FILE__,__LINE__);
	m_pPlayer = UGNew<CPlayer>(m_pWorldbaseConfig->m_ulMaxPlayer,__FILE__,__LINE__);
	for(UG_INT32 i = 0; i < UG_INT32(m_pWorldbaseConfig->m_ulMaxPlayer); i ++)
	{
		(m_pPlayer + i)->m_pchCurrentBuffer = m_pchPlayerCurrentBuffer + (10 * (MAX_CHAT_LENGTH + 1)) * i;
		(m_pPlayer + i)->setParent(this);
		(m_pPlayer + i)->init();
		m_listPlayerIdle.push_back(m_pPlayer + i);
	}
	m_pSQLChatMail = UGNew<CUGSQL>(0,__FILE__,__LINE__);
	m_pWBConnect = UGNew<CWBConnect>(0,__FILE__,__LINE__);
	m_pWBConnect->setParent(this);
	m_pPlayerManager = UGNew<CPlayerManager>(0,__FILE__,__LINE__);
	m_pSceneManager = UGNew<CSceneManager>(0,__FILE__,__LINE__);
	m_pTeamManager = UGNew<CTeamManager>(0,__FILE__,__LINE__);
	m_pTeamManager->init(m_pWorldbaseConfig->m_ulMaxTeam);
	m_pTradeRoomManager = UGNew<CTradeRoomManager>(0,__FILE__,__LINE__);
	m_pTradeRoomManager->init(m_pWorldbaseConfig->m_ulMaxTradeRoom);
	m_pUnionManager = UGNew<CUnionManager>(0,__FILE__,__LINE__);
	m_pUnionManager->init(m_pWorldbaseConfig->m_ulMaxUnion);
	m_pchSendBuf = UGMalloc<UG_CHAR>(10240,__FILE__,__LINE__);
	return 0;
}

UG_LONG CWorldbase::freeMemory()
{
	UGDel<CPlayer*>(m_pPlayer);
	UGDel<CUGSQL*>(m_pSQLChatMail);
	UGDel<CWBConnect*>(m_pWBConnect);
	UGDel<CPlayerManager*>(m_pPlayerManager);
	UGDel<CSceneManager*>(m_pSceneManager);
	UGDel<CTeamManager*>(m_pTeamManager);
	UGDel<CTradeRoomManager*>(m_pTradeRoomManager);
	UGDel<CUnionManager*>(m_pUnionManager);
	UGDel<UG_PCHAR>(m_pchSendBuf);
	UGDel<UG_PCHAR>(m_pchPlayerCurrentBuffer);
	return 0;
}

UG_LONG CWorldbase::initMailSQL()
{
	UG_ULONG ulRet = m_pSQLChatMail->openDB(m_pWorldbaseConfig->m_szMailDBHost,m_pWorldbaseConfig->m_szMailDBUser,m_pWorldbaseConfig->m_szMailDBPassword,m_pWorldbaseConfig->m_szMailDBName);
	if(ulRet)
	{
		g_pLog->UGLog("open db error.");
		return -1;
	}
	return 0;
}

UG_LONG CWorldbase::initPlayerTCPComm()
{
	CreateInterfaceFn create_interface = ::Sys_GetFactory(m_pSysModule);
	m_pPlayerTCPComm = (INetServer*)create_interface(NETSERVER_INTERFACE_VERSION,NULL);
	if(!m_pPlayerTCPComm)
	{
		g_pLog->UGLog("create PlayerTCPComm interface failed.");
		return (UG_ULONG)(-1);
	}
	INIT_NET_SERCER is;
	is.dwIP = m_pWorldbaseConfig->m_dwIPForPlayer;
	is.dwPort = m_pWorldbaseConfig->m_dwPortForPlayer;
	is.ulMaxConnect = m_pWorldbaseConfig->m_ulMaxPlayer;
	UG_ULONG ulRet = m_pPlayerTCPComm->init(&is,FnNetSvr(PlayerCB),this);
	if(ulRet)
	{
		g_pLog->UGLog("init PlayerTCPComm failed.");
		return -1;
	}
	return 0;
}
	
UG_LONG CWorldbase::WorldbaseCBProc(PNET_SERVER_PARAM p)
{
	if(NET_MSG_CHECK_DATA == p->dwMsgID)
	{
		checkWorldbase(p);
	}
	else if(NET_MSG_WBCHAT_BASEINFO == p->dwMsgID) //发送场景，目前工会个数，数据
	{
		if(WB_CHECKED_NOT_SENDBASEINFO == m_pWBConnect->m_stepWB)
		{
			if(getWorldbaseBaseinfo(p))
			{
				m_pWorldbaseTCPComm->sendData(m_pWBConnect->m_pvConnect,NET_MSG_WBCHAT_BASEINFO_RETURN,(UG_DWORD)CHECK_ERROR);
			}
			else
			{
				UG_DWORD dw[2];
				dw[0] = m_pWorldbaseConfig->m_dwIPForPlayer;
				dw[1] = m_pWorldbaseConfig->m_dwPortForPlayer;
				m_pWorldbaseTCPComm->sendData(m_pWBConnect->m_pvConnect,NET_MSG_WBCHAT_BASEINFO_RETURN,(UG_DWORD)CHECK_OK,(UG_PCHAR)(dw),sizeof(UG_DWORD) * 2);
			}
		}
		else
		{
			m_pWorldbaseTCPComm->sendData(m_pWBConnect->m_pvConnect,NET_MSG_WBCHAT_BASEINFO_RETURN,(UG_DWORD)CHECK_ERROR);
			g_pLog->UGLog("WorldbaseCBProc getWorldbaseBaseinfo error because step is not WB_CHECKED_NOT_SENDBASEINFO.");
		}
	}
	else if(NET_MSG_WBCHAT_CHECK == p->dwMsgID)
	{
		if(WB_CONNECT_STEP_OK == m_pWBConnect->m_stepWB)
		{
			recvCheckPlayerid(p);
		}
		else
		{
			m_pWorldbaseTCPComm->sendData(m_pWBConnect->m_pvConnect,NET_MSG_WBCHAT_CHECK_RETURN,(UG_DWORD)CHECK_ERROR);
			g_pLog->UGLog("WorldbaseCBProc dealWithData error because step is not WB_CONNECT_STEP_OK.");
		}
	}
	else
	{
		if(WB_CONNECT_STEP_OK == m_pWBConnect->m_stepWB)
		{
			m_pWBConnect->dispatchMsg(p->dwMsgID,p->dwParam,p->pchData,p->ulLen);
		}
		else
		{
			g_pLog->UGLog("WorldbaseCBProc dealWithData error because step is not WB_CONNECT_STEP_OK.");
		}
	}
	return 0;
}

UG_ULONG CWorldbase::PlayerCB(PNET_SERVER_PARAM p)
{
	return ((CWorldbase*)(p->pvKey))->PlayerCBProc(p);
}

UG_ULONG CWorldbase::PlayerCBProc(PNET_SERVER_PARAM p)
{
	if(NET_MSG_CONNECT == p->dwMsgID) //connect
	{
		if(WB_CONNECT_STEP_OK != m_pWBConnect->m_stepWB)
		{
			g_pLog->UGLog("player disconnect because worldbase not connected.");
			m_pPlayerTCPComm->disconnectPlayer(p->pvConnect);
		}
		else
		{
			recvPlayer(p);
		}
	}
	else if(NET_MSG_CHECK_DATA == p->dwMsgID)
	{
		CPlayer* pPlayer = (CPlayer*)(p->pvPlayer);
		if(PLAYER_CONNECTED_NOT_CHECK == pPlayer->m_stepPlayer)
		{
			CHAT_CHECK cc;
			cc.nPlayerid = int(p->dwParam);
			cc.nPlayerPointer = int(pPlayer->m_pvConnect);
			g_pLog->UGLog("recv player check id = %d, ip = %s, port = %d and send to worldbase to check.",p->dwParam,convertip(pPlayer->m_dwIP),pPlayer->m_wPort);
			m_pWorldbaseTCPComm->sendData(m_pWBConnect->m_pvConnect,NET_MSG_WBCHAT_CHECK_RETURN,0,(UG_PCHAR)(&cc),sizeof(CHAT_CHECK));
		}
		else
		{
			g_pLog->UGLog("Client check id error because step != PLAYER_CONNECTED_NOT_CHECK.");
		}
	}
	else
	{
		CPlayer* pPlayer = (CPlayer*)(p->pvPlayer);
		pPlayer->dispatchMsg(p->dwMsgID,p->dwParam,p->pchData,p->ulLen);
	}
	return 0;
}

UG_LONG CWorldbase::recvWorldbase(PNET_SERVER_PARAM p)
{
	UG_DWORD dwIP = 0;
	UG_WORD wPort = 0;
	m_pWorldbaseTCPComm->getConnect(p->pvConnect,dwIP,wPort);
	if(WB_NOT_CONNECTED != m_pWBConnect->m_stepWB)
	{
		return -1;
	}
	if(m_pWorldbaseConfig->m_dwWorldbaseIP != dwIP)
	{
		g_pLog->UGLog("ip error recv %s != %s, %d worldbase.",convertip(dwIP),convertip(m_pWorldbaseConfig->m_dwWorldbaseIP),m_pWorldbaseConfig->m_dwWorldbaseIP);
		return -1;
	}
	m_pWBConnect->m_dwIP = dwIP;
	m_pWBConnect->m_wPort = wPort;
	m_pWBConnect->m_bOnline = TRUE;
	m_pWBConnect->m_dwTimer = m_dwRenderTimer;
	m_pWBConnect->m_pvConnect = p->pvConnect;
	m_pWBConnect->m_pTCPComm = m_pWorldbaseTCPComm;
	m_pWorldbaseTCPComm->setPlayer(p->pvConnect,this);
	m_pWorldbaseTCPComm->sendData(m_pWBConnect->m_pvConnect,NET_MSG_CHECK_DATA,0);
	m_pWBConnect->m_stepWB = WB_CONNECTED_NOT_CHECK;
	g_pLog->UGLog("recvWorldbase is ok, ip = %s, port = %d.",convertip(m_pWBConnect->m_dwIP),m_pWBConnect->m_wPort);
	return 0;
}

UG_LONG CWorldbase::checkWorldbase(PNET_SERVER_PARAM p)
{
	m_pWorldbaseTCPComm->sendData(m_pWBConnect->m_pvConnect,NET_MSG_CHECK_DATA,0);
	if(WB_CONNECT_STEP_OK != m_pWBConnect->m_stepWB)
	{
		m_pWBConnect->m_stepWB = WB_CHECKED_NOT_SENDBASEINFO;
	}
	g_pLog->UGLog("checkWorldbase is ok, check id = %d.",p->dwParam);
	return 0;
}

UG_LONG CWorldbase::getWorldbaseBaseinfo(PNET_SERVER_PARAM p)
{
	static CHAT_BASEINFO cb;
	if(p->ulLen > sizeof(cb.base))
	{
		Q_memcpy(&(cb.base),p->pchData,sizeof(cb.base));
		if(cb.base.nSceneNum > 0)
		{
			if(cb.base.nUnionCurrentNum >= 0)
			{
				if((sizeof(cb.base) + (cb.base.nSceneNum + cb.base.nUnionCurrentNum) * sizeof(int)) == p->ulLen)
				{
					m_pWorldbaseConfig->m_ulSceneNum = cb.base.nSceneNum;
					m_pSceneManager->init(cb.base.nSceneNum,(UG_PINT32)(p->pchData + sizeof(cb.base)));
					for(UG_INT32 i = 0; i < cb.base.nUnionCurrentNum; i ++)
					{
						m_pUnionManager->addUnion(*((UG_PINT32)(p->pchData + sizeof(cb.base) + (cb.base.nSceneNum + i) * sizeof(int))));
					}
					m_pWBConnect->m_stepWB = WB_CONNECT_STEP_OK;
					g_pLog->UGLog("getWorldbaseBaseinfo is ok, Scene num = %d, union num = %d.",cb.base.nSceneNum,cb.base.nUnionCurrentNum);
					return 0;
				}
				else
				{
					g_pLog->UGLog("data len error %d != %d",sizeof(cb.base) + (cb.base.nSceneNum + cb.base.nUnionCurrentNum) * sizeof(int),p->ulLen);
				}
			}
			else
			{
				g_pLog->UGLog("getWorldbaseBaseinfo failed UnionCurrentNum = %d.",cb.base.nUnionCurrentNum);
			}
		}
		else
		{
			g_pLog->UGLog("getWorldbaseBaseinfo failed scene num = %d.",cb.base.nSceneNum);
		}
	}
	else
	{
		g_pLog->UGLog("getWorldbaseBaseinfo failed ulLen  = %d <= sizeof(cb.base).",p->ulLen);
	}
	return -1;
}

UG_LONG CWorldbase::recvPlayer(PNET_SERVER_PARAM p)
{
	CSyncEvent sys(&m_cs);
	list<CPlayer*>::iterator it = m_listPlayerIdle.begin();
	if(it != m_listPlayerIdle.end())
	{
		(*it)->m_bOnline = TRUE;
		(*it)->m_dwTimer = m_dwRenderTimer;
		(*it)->m_pvConnect = p->pvConnect;
		(*it)->m_stepPlayer = PLAYER_CONNECTED_NOT_CHECK;
		m_pPlayerTCPComm->getConnect(p->pvConnect,(*it)->m_dwIP,(*it)->m_wPort);
		m_pPlayerTCPComm->setPlayer(p->pvConnect,(*it));
		(*it)->m_pTCPComm = m_pPlayerTCPComm;
		g_pLog->UGLog("recv player connect ok, ip = %s, port = %d.",convertip((*it)->m_dwIP),(*it)->m_wPort);
		m_mapPlayer[p->pvConnect] = *it;
		m_listPlayerIdle.erase(it);
	}
	else
	{
		g_pLog->UGLog("recv player connect failed because there is no idle manager memory for connect.");
	}
	return 0;
}

UG_LONG CWorldbase::recvCheckPlayerid(PNET_SERVER_PARAM p)
{
	if(CHECK_OK == p->dwParam)
	{
		if(p->ulLen == sizeof(CHAT_CHECK_RETURN))
		{
			CHAT_CHECK_RETURN cc;
			Q_memcpy(&cc,p->pchData,sizeof(CHAT_CHECK_RETURN));
			map<UG_PVOID,CPlayer*>::iterator it = m_mapPlayer.find(UG_PVOID(cc.nPlayerPointer));
			if(it != m_mapPlayer.end())
			{
				g_pLog->UGLog("check playerid ok.");
				CPlayer* pPlayer = (CPlayer*)(it->second);
				pPlayer->playerLogin(&cc);
				m_pPlayerTCPComm->sendData(pPlayer->m_pvConnect,NET_MSG_CHECK_DATA,0);
				m_pWorldbaseTCPComm->sendData(m_pWBConnect->m_pvConnect,NET_MSG_WBCHAT_RECV_CHECK_RETURN,(UG_DWORD)CHECK_OK,(UG_PCHAR(&(cc.nPlayerid))),sizeof(int));
				return 0;
			}
			else
			{
				g_pLog->UGLog("check playerid ok but player pointer returned from worldbase error.");
			}
			m_pWorldbaseTCPComm->sendData(m_pWBConnect->m_pvConnect,NET_MSG_WBCHAT_RECV_CHECK_RETURN,(UG_DWORD)CHECK_ERROR,(UG_PCHAR(&(cc.nPlayerid))),sizeof(int));
			return -1;
		}
		else
		{
			g_pLog->UGLog("check playerid ok but data len <= sizeof(CHAT_CHECK_RETURN).");
		}
	}
	else if(CHECK_ERROR == p->dwParam)
	{
		g_pLog->UGLog("check playerid error because check return error.");
	}
	else
	{
		g_pLog->UGLog("check playerid error because param error = %d.",p->dwParam);
	}
	m_pWorldbaseTCPComm->sendData(m_pWBConnect->m_pvConnect,NET_MSG_WBCHAT_RECV_CHECK_RETURN,(UG_DWORD)CHECK_ERROR);
	disconnectPlayer(m_pWBConnect->m_pvConnect);
	return -1;
}

UG_LONG CWorldbase::disconnectAll()
{
	m_pWBConnect->m_bOnline = FALSE;
	m_pWBConnect->m_stepWB = WB_NOT_CONNECTED;
	m_pWorldbaseTCPComm->disconnectPlayer(m_pWBConnect->m_pvConnect);
	map<UG_PVOID,CPlayer*>::iterator it;
	for(it = m_mapPlayer.begin(); it != m_mapPlayer.end();)
	{
		m_pPlayerTCPComm->disconnectPlayer(it->second->m_pvConnect);
		m_listPlayerIdle.push_back(it->second);
		it = m_mapPlayer.erase(it);
	}
	m_pPlayerManager->cleanup();
	m_pSceneManager->cleanup();
	m_pTeamManager->cleanup();
	m_pTradeRoomManager->cleanup();
	m_pUnionManager->cleanup();
	return 0;
}

UG_LONG CWorldbase::disconnectPlayer(UG_PVOID pvConnect)
{
	map<UG_PVOID,CPlayer*>::iterator it = m_mapPlayer.find(pvConnect);
	if(it != m_mapPlayer.end())
	{
		g_pLog->UGLog("disconnectPlayer ok in CWorldbaseok.");
		it->second->disconnect();
		m_listPlayerIdle.push_back(it->second);
		it = m_mapPlayer.erase(it);
	}
	else
	{
		g_pLog->UGLog("disconnectPlayer failed in CWorldbaseok.");
	}
	return 0;
}
