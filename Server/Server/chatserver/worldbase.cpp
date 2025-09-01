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
	m_pStopTalkManager = NULL;
	m_pWorldbaseConfig = NULL;
	m_dwRenderTimer = 0;
	m_pchSendBuf = NULL;
	m_pchPlayerCurrentBuffer = NULL;
	m_pchPlayerUnionBuffer = NULL;
	m_dwSendBufferTimer = 0;
	m_pGMTools = NULL;
	m_listGMToolsIdle.clear();
	m_mapGMTools.clear();
	m_pGMTCPComm = NULL;
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
	lRet = initMailSQL();
	if(lRet)
	{
		g_pLog->UGLog("initMailSQL error");
		return (UG_ULONG)(-1);
	}
	lRet = initPlayerTCPComm();
	if(lRet)
	{
		g_pLog->UGLog("initPlayerTCPComm error.");
		return (UG_ULONG)(-1);
	}
	lRet = initGMTCPComm();
	if(lRet)
	{
		g_pLog->UGLog("initGMTCPComm error.");
		return (UG_ULONG)(-1);
	}
	lRet = (UGLONG)(m_pStopTalkManager->loadDB());
	if(lRet)
	{
		g_pLog->UGLog("loadDB error.");
		return (UG_ULONG)(-1);
	}
	return 0;
}

UG_LONG CWorldbase::renderGMTools(UG_ULONG ulTimer)
{
	m_pGMTCPComm->acceptPlayer();
	map<UG_PVOID,CGMTools*>::iterator it;
	for(it = m_mapGMTools.begin(); it != m_mapGMTools.end();)
	{
		if(it->second->m_bOnline)
		{
			double db = 0;
			double db2 = 0;
			UG_ULONG ulRet = m_pGMTCPComm->recvData((it->second)->m_pvConnect,db,db2);
			if(-1 == ulRet)
			{
				it->second->m_bOnline = FALSE;
			}
			
			if(GM_CONNECT_STEP_OK != it->second->m_stepGM) //30秒没有验证即断开
			{
				if(ulTimer - it->second->m_dwTimer > 30000)
				{
					it->second->m_bOnline = FALSE;
				}
			}

			it ++;
		}
		else
		{
			m_pGMTCPComm->disconnectPlayer(it->first);
			m_listGMToolsIdle.push_back(it->second);
			it = m_mapGMTools.erase(it);
		}
	}
	for(it = m_mapGMTools.begin(); it != m_mapGMTools.end(); it ++)
	{
		if((it->second)->m_bOnline)
		{
			if(-1 == m_pGMTCPComm->sendBuffer((it->second)->m_pvConnect,500))
			{
				g_pLog->UGLog("gm tools send buffer error.ip = %s, port = %d.",convertip((it->second)->m_dwIP),(it->second)->m_wPort);
				(it->second)->m_bOnline = FALSE;
			}
		}
	}
	return 0;
}

UG_LONG CWorldbase::render(UG_ULONG ulTimer)
{
	m_dwRenderTimer = ulTimer;
	
	renderGMTools(ulTimer);
	
	m_pPlayerTCPComm->acceptPlayer();
	map<UG_PVOID,CPlayer*>::iterator it;
	for(it = m_mapPlayer.begin(); it != m_mapPlayer.end();)
	{
		if(it->second->m_bOnline)
		{
			double db = 0;
			double db2 = 0;
			UG_ULONG ulRet = m_pPlayerTCPComm->recvData((it->second)->m_pvConnect,db,db2);
			if(-1 == ulRet)
			{
				it->second->m_bOnline = FALSE;
			}
		}
		if(PLAYER_CONNECT_STEP_OK != (it->second)->m_stepPlayer)
		{
			if(!(it->second->m_bOnline)) //没有校验,而且已经断线,则直接关闭该玩家
			{
				g_pLog->UGLog("player disconnected not check but disconnect, ip = %s, port = %d.",convertip((it->second)->m_dwIP),(it->second)->m_wPort);
				m_pPlayerTCPComm->disconnectPlayer(it->first);
				m_listPlayerIdle.push_back(it->second);
				it = m_mapPlayer.erase(it);
				continue;
			}
			if(m_dwRenderTimer - it->second->m_dwTimer > 20000) //20s没有确认则认为非法连接
			{
				g_pLog->UGLog("player disconnected timeout, ip = %s, port = %d.",convertip((it->second)->m_dwIP),(it->second)->m_wPort);
				m_pPlayerTCPComm->writeBuffer(it->first,NET_MSG_PLAYER_LOGIN_CHAT,PLAYER_LOGIN_CHAT_TIMEOUT);
				m_pPlayerTCPComm->disconnectPlayer(it->first);
				m_listPlayerIdle.push_back(it->second);
				it = m_mapPlayer.erase(it);
				continue;
			}
		}
		if(it->second->m_bReconnect) //
		{
			m_listPlayerIdle.push_back(it->second);
			it = m_mapPlayer.erase(it);
			continue;
		}
		it ++;
	}
//网络流量控制，通过控制网络发送量保证客户端连接的稳定.	
	DWORD dwTimes = 1000;
	int nSize = 100;
	if(m_mapPlayer.size() > 4000) //4000人以上 400
	{
		dwTimes = 1000;
		nSize = 400;
	}
	else if(m_mapPlayer.size() > 3000) //3000 - 4000人 470
	{
		dwTimes = 750;
		nSize = 350;
	}
	else if(m_mapPlayer.size() > 2000) //2000 - 3000人 500
	{
		dwTimes = 500;
		nSize = 250;
	}
	else if(m_mapPlayer.size() > 1000) //1000 - 2000人 600
	{
		dwTimes = 500;
		nSize = 300;
	}
	else //1000人以下 834
	{
		dwTimes = 300;
		nSize = 250;
	}
//ben add 2005-07-20 for no test
	dwTimes = 400;
	nSize = 500;
//----
	if(m_dwRenderTimer - m_dwSendBufferTimer > dwTimes)
	{
		for(it = m_mapPlayer.begin(); it != m_mapPlayer.end(); it ++)
		{
			if((it->second)->m_bOnline)
			{
				if(-1 == m_pPlayerTCPComm->sendBuffer((it->second)->m_pvConnect,nSize))
				{
					g_pLog->UGLog("send buffer error.ip = %s, port = %d.",convertip((it->second)->m_dwIP),(it->second)->m_wPort);
					(it->second)->m_bOnline = FALSE;
				}
			}
		}
		m_dwSendBufferTimer = m_dwRenderTimer;
	}
	m_pStopTalkManager->render(m_dwRenderTimer);
	return 0;
}

UG_LONG CWorldbase::cleanup()
{
	map<UG_PVOID,CPlayer*>::iterator it;
	for(it = m_mapPlayer.begin(); it != m_mapPlayer.end(); it ++)
	{
		if((it->second)->m_pvConnect)
		{
			g_pLog->UGLog("disconnect ip = %s, id = %d.",convertip((it->second)->m_dwIP),(it->second)->m_n32Playerid);
			m_pPlayerTCPComm->disconnectPlayer((it->second)->m_pvConnect);
		}
	}
	if(m_pPlayerTCPComm)
	{
		m_pPlayerTCPComm->shutdown();
		m_pPlayerTCPComm = NULL;
	}
	
	map<UG_PVOID,CGMTools*>::iterator itGMTools;
	for(itGMTools = m_mapGMTools.begin(); itGMTools != m_mapGMTools.end(); itGMTools ++)
	{
		if((itGMTools->second)->m_pvConnect)
		{
			g_pLog->UGLog("disconnect ip = %s.",convertip((itGMTools->second)->m_dwIP));
			m_pGMTCPComm->disconnectPlayer((itGMTools->second)->m_pvConnect);
		}
	}
	if(m_pGMTCPComm)
	{
		m_pGMTCPComm->shutdown();
		m_pGMTCPComm = NULL;
	}

	m_listGMToolsIdle.clear();
	m_mapGMTools.clear();
	
	m_listPlayerIdle.clear();
	m_mapPlayer.clear();
	freeMemory();
	return 0;
}

UG_LONG CWorldbase::mallocMemory()
{
	m_pchPlayerCurrentBuffer = new UGCHAR[(m_pWorldbaseConfig->m_ulMaxPlayer) * 10 * (MAX_CHAT_LENGTH + 1)];
	m_pchPlayerUnionBuffer = new UGCHAR[(m_pWorldbaseConfig->m_ulMaxPlayer) * 10 * (MAX_CHAT_LENGTH + 1)];
	m_pPlayer = new CPlayer[m_pWorldbaseConfig->m_ulMaxPlayer];
	for(UG_INT32 i = 0; i < UG_INT32(m_pWorldbaseConfig->m_ulMaxPlayer); i ++)
	{
		(m_pPlayer + i)->m_pchCurrentBuffer = m_pchPlayerCurrentBuffer + (10 * (MAX_CHAT_LENGTH + 1)) * i;
		(m_pPlayer + i)->m_pchUnionBuffer = m_pchPlayerUnionBuffer + (10 * (MAX_CHAT_LENGTH + 1)) * i;
		(m_pPlayer + i)->setParent(this);
		(m_pPlayer + i)->init();
		m_listPlayerIdle.push_back(m_pPlayer + i);
	}
	m_pSQLChatMail = new CUGSQL;
	m_pWBConnect = new CWBConnect;
	m_pWBConnect->setParent(this);
	m_pPlayerManager = new CPlayerManager;
	m_pSceneManager = new CSceneManager;
	m_pchSendBuf = new UGCHAR[10240];
	m_pStopTalkManager = new CStopTalkManager;
	m_pStopTalkManager->init();
	
	m_pGMTools = new CGMTools[MAXNUM_GMTOOLS];
	for(i = 0; i < MAXNUM_GMTOOLS; i ++)
	{
		m_listGMToolsIdle.push_back(m_pGMTools + i);
	}
	return 0;
}

UG_LONG CWorldbase::freeMemory()
{
	ReleasePA(m_pPlayer);
	ReleaseP(m_pSQLChatMail);
	ReleaseP(m_pWBConnect);
	ReleaseP(m_pPlayerManager);
	ReleaseP(m_pSceneManager);
	ReleasePA(m_pchSendBuf);
	ReleasePA(m_pchPlayerCurrentBuffer);
	ReleasePA(m_pchPlayerUnionBuffer);
	ReleaseP(m_pStopTalkManager);
	ReleasePA(m_pGMTools);
	return 0;
}

UG_LONG CWorldbase::initMailSQL()
{
	UG_ULONG ulRet = m_pSQLChatMail->openDB(m_pWorldbaseConfig->m_szMailDBHost,m_pWorldbaseConfig->m_szMailDBUser,m_pWorldbaseConfig->m_szMailDBPassword,m_pWorldbaseConfig->m_szMailDBName);
	if(ulRet)
	{
		const char* pchError = m_pSQLChatMail->getError();
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

UG_LONG CWorldbase::initGMTCPComm()
{
	CreateInterfaceFn create_interface = ::Sys_GetFactory(m_pSysModule);
	m_pGMTCPComm = (INetServer*)create_interface(NETSERVER_INTERFACE_VERSION,NULL);
	if(!m_pPlayerTCPComm)
	{
		g_pLog->UGLog("create initGMTCPComm interface failed.");
		return (UG_ULONG)(-1);
	}
	INIT_NET_SERCER is;
	is.dwIP = m_pWorldbaseConfig->m_dwIPForGM;
	is.dwPort = m_pWorldbaseConfig->m_dwPortForGM;
	is.ulMaxConnect = MAXNUM_GMTOOLS;
	UG_ULONG ulRet = m_pGMTCPComm->init(&is,FnNetSvr(GMCB),this);
	if(ulRet)
	{
		g_pLog->UGLog("init initGMTCPComm failed.");
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
				m_pWorldbaseTCPComm->writeBuffer(m_pWBConnect->m_pvConnect,NET_MSG_WBCHAT_BASEINFO_RETURN,(UG_DWORD)CHECK_ERROR);
			}
			else
			{
				UG_DWORD dw[4];
				dw[0] = m_pWorldbaseConfig->m_dwIPForPlayer;
				dw[1] = m_pWorldbaseConfig->m_dwPortForPlayer;
				dw[2] = m_pWorldbaseConfig->m_dwIPForGM;
				dw[3] = m_pWorldbaseConfig->m_dwPortForGM;
				m_pWorldbaseTCPComm->writeBuffer(m_pWBConnect->m_pvConnect,NET_MSG_WBCHAT_BASEINFO_RETURN,(UG_DWORD)CHECK_OK,(UG_PCHAR)(dw),sizeof(UG_DWORD) * 4);
			}
		}
		else
		{
			m_pWorldbaseTCPComm->writeBuffer(m_pWBConnect->m_pvConnect,NET_MSG_WBCHAT_BASEINFO_RETURN,(UG_DWORD)CHECK_ERROR,NULL,0,WRITE_WORLDBASE_OVERFLOW);
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
			m_pWorldbaseTCPComm->writeBuffer(m_pWBConnect->m_pvConnect,NET_MSG_WBCHAT_CHECK_RETURN,(UG_DWORD)CHECK_ERROR,NULL,0,WRITE_WORLDBASE_OVERFLOW);
			g_pLog->UGLog("WorldbaseCBProc dealWithData error because step is not WB_CONNECT_STEP_OK.");
		}
	}
	else if(NET_MSG_WBCHAT_GM_CHECK == p->dwMsgID)
	{
		if(p->ulLen == sizeof(CHAT_CHECK))
		{
			CHAT_CHECK* pCC =  (CHAT_CHECK*)(p->pchData);
			checkGM(pCC);
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

UGLONG CWorldbase::checkGM(CHAT_CHECK* pCC)
{
	map<UG_PVOID,CGMTools*>::iterator it = m_mapGMTools.find(UGPVOID(pCC->nPlayerPointer));
	if(it != m_mapGMTools.end())
	{
		CGMTools* pGMTools = it->second;
		if(pGMTools)
		{
			if(GM_CONNECTED_NOT_CHECK == pGMTools->m_stepGM)
			{
				if(pCC->nPlayerid) //不通过
				{
					pGMTools->m_bOnline = FALSE;
					pGMTools->sendMessage(NET_MSG_GM_CHECK_RETURN,CHECK_ERROR);
					g_pLog->UGLog("check gm error.");
				}
				else //通过
				{
					pGMTools->sendMessage(NET_MSG_GM_CHECK_RETURN,CHECK_OK);
					pGMTools->m_stepGM = GM_CONNECT_STEP_OK;
				}
			}
		}
	}
	return __LINE__;
}

UG_ULONG CWorldbase::GMCB(PNET_SERVER_PARAM p)
{
	return ((CWorldbase*)(p->pvKey))->GMCBProc(p);
}

UG_ULONG CWorldbase::GMCBProc(PNET_SERVER_PARAM p)
{
	if(NET_MSG_CONNECT == p->dwMsgID) //connect
	{
		if(WB_CONNECT_STEP_OK != m_pWBConnect->m_stepWB)
		{
			g_pLog->UGLog("gm disconnect because worldbase not connected.");
			m_pGMTCPComm->disconnectPlayer(p->pvConnect);
		}
		else
		{
			recvGM(p);
		}
	}
	else if(NET_MSG_CHECK_DATA == p->dwMsgID)
	{
		CGMTools* pGMTools = (CGMTools*)(p->pvPlayer);
		if(pGMTools)
		{
			if(GM_CONNECTED_NOT_CHECK == pGMTools->m_stepGM)
			{
				CHAT_CHECK cc;
				cc.nPlayerid = p->dwParam;
				cc.nPlayerPointer = int(pGMTools->m_pvConnect);
				if(-1 == m_pWorldbaseTCPComm->writeBuffer(m_pWBConnect->m_pvConnect,NET_MSG_WBCHAT_GM_CHECK_RETURN,0,(UG_PCHAR)(&cc),sizeof(CHAT_CHECK)))
				{
					g_pLog->UGLog("gm check write buffer error overflows.");
				}
			}
		}
	}
	else
	{
		CGMTools* pGMTools = (CGMTools*)(p->pvPlayer);
		if(pGMTools)
		{
			if(GM_CONNECT_STEP_OK == pGMTools->m_stepGM)
			{
				pGMTools->dispatchMsg(p->dwMsgID,p->dwParam,p->pchData,p->ulLen);
			}
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
			UGINT nPlayerID = int(p->dwParam);
			CPlayer* pFind = (CPlayer*)(m_pPlayerManager->findPlayer(nPlayerID));
			if(pFind)
			{
				if(!(p->pvConnect))
				{
					g_pLog->UGLog("player reconnected, error.");
				}
				m_pPlayerTCPComm->disconnectPlayer(pFind->m_pvConnect);
				pFind->m_pvConnect = pPlayer->m_pvConnect;
				pFind->m_bOnline = TRUE;
				pPlayer->m_bReconnect = TRUE;
				m_pPlayerTCPComm->getConnect(pFind->m_pvConnect,pFind->m_dwIP,pFind->m_wPort);
				m_pPlayerTCPComm->setPlayer(pFind->m_pvConnect,pFind);
				g_pLog->UGLog("player reconnected, id = %d, line = %d.",nPlayerID,__LINE__);
				m_pPlayerTCPComm->writeBuffer(pPlayer->m_pvConnect,NET_MSG_PLAYER_LOGIN_CHAT,PLAYER_LOGIN_CHAT_OK);
			}
			else
			{
				CHAT_CHECK cc;
				cc.nPlayerid = int(p->dwParam);
				cc.nPlayerPointer = int(pPlayer->m_pvConnect);
				pPlayer->m_n32Playerid = cc.nPlayerid;
				if(-1 == m_pWorldbaseTCPComm->writeBuffer(m_pWBConnect->m_pvConnect,NET_MSG_WBCHAT_CHECK_RETURN,0,(UG_PCHAR)(&cc),sizeof(CHAT_CHECK)))
				{
					g_pLog->UGLog("write buffer error overflows.");
				}
			}
		}
		else
		{
			g_pLog->UGLog("Client check id error because step != PLAYER_CONNECTED_NOT_CHECK.");
		}
	}
	else
	{
		CPlayer* pPlayer = (CPlayer*)(p->pvPlayer);
		if(PLAYER_CONNECT_STEP_OK == pPlayer->m_stepPlayer)
		{
			pPlayer->dispatchMsg(p->dwMsgID,p->dwParam,p->pchData,p->ulLen);
		}
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
		char sz[1024];
		strcpy(sz,convertip(dwIP));
		g_pLog->UGLog("ip error recv %s != %s, %d worldbase.",sz,convertip(m_pWorldbaseConfig->m_dwWorldbaseIP),m_pWorldbaseConfig->m_dwWorldbaseIP);
		return -1;
	}
	m_pWBConnect->m_dwIP = dwIP;
	m_pWBConnect->m_wPort = wPort;
	m_pWBConnect->m_bOnline = TRUE;
	m_pWBConnect->m_dwTimer = m_dwRenderTimer;
	m_pWBConnect->m_pvConnect = p->pvConnect;
	m_pWBConnect->m_pTCPComm = m_pWorldbaseTCPComm;
	m_pWorldbaseTCPComm->setPlayer(p->pvConnect,this);
	m_pWorldbaseTCPComm->writeBuffer(m_pWBConnect->m_pvConnect,NET_MSG_CHECK_DATA,0);
	m_pWBConnect->m_stepWB = WB_CONNECTED_NOT_CHECK;
	return 0;
}

UG_LONG CWorldbase::checkWorldbase(PNET_SERVER_PARAM p)
{
	m_pWorldbaseTCPComm->writeBuffer(m_pWBConnect->m_pvConnect,NET_MSG_CHECK_DATA,0);
	if(WB_CONNECT_STEP_OK != m_pWBConnect->m_stepWB)
	{
		m_pWBConnect->m_stepWB = WB_CHECKED_NOT_SENDBASEINFO;
	}
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
					m_pWBConnect->m_stepWB = WB_CONNECT_STEP_OK;
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

UG_LONG CWorldbase::recvGM(PNET_SERVER_PARAM p)
{
	list<CGMTools*>::iterator it = m_listGMToolsIdle.begin();
	if(it != m_listGMToolsIdle.end())
	{
		(*it)->m_bOnline = TRUE;
		(*it)->m_dwTimer = m_dwRenderTimer;
		(*it)->m_pvConnect = p->pvConnect;
		(*it)->m_pvParent = this;
		(*it)->m_stepGM = GM_CONNECTED_NOT_CHECK;
		m_pGMTCPComm->getConnect(p->pvConnect,(*it)->m_dwIP,(*it)->m_wPort);
		m_pGMTCPComm->setPlayer(p->pvConnect,(*it));
		(*it)->m_pTCPComm = m_pPlayerTCPComm;
		m_mapGMTools[p->pvConnect] = *it;
		m_listGMToolsIdle.erase(it);
	}
	else
	{
		g_pLog->UGLog("recv gmtools connect failed because there is no idle manager memory for connect.");
	}
	return 0;
}

UG_LONG CWorldbase::recvPlayer(PNET_SERVER_PARAM p)
{
	list<CPlayer*>::iterator it = m_listPlayerIdle.begin();
	if(it != m_listPlayerIdle.end())
	{
		(*it)->m_bOnline = TRUE;
		(*it)->m_dwTimer = m_dwRenderTimer;
		(*it)->m_pvConnect = p->pvConnect;
		(*it)->m_stepPlayer = PLAYER_CONNECTED_NOT_CHECK;
		(*it)->m_bReconnect = FALSE;
		m_pPlayerTCPComm->getConnect(p->pvConnect,(*it)->m_dwIP,(*it)->m_wPort);
		m_pPlayerTCPComm->setPlayer(p->pvConnect,(*it));
		(*it)->m_pTCPComm = m_pPlayerTCPComm;
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
				CPlayer* pPlayer = (CPlayer*)(it->second);
				pPlayer->playerLogin(&cc);
				if(pPlayer->m_pvConnect)
				{
					m_pPlayerTCPComm->writeBuffer(pPlayer->m_pvConnect,NET_MSG_PLAYER_LOGIN_CHAT,PLAYER_LOGIN_CHAT_OK);
				}
				m_pWorldbaseTCPComm->writeBuffer(m_pWBConnect->m_pvConnect,NET_MSG_WBCHAT_RECV_CHECK_RETURN,(UG_DWORD)CHECK_OK,(UG_PCHAR(&(cc.nPlayerid))),sizeof(int));
				return 0;
			}
			else
			{
				g_pLog->UGLog("check playerid ok but player pointer returned from worldbase error.");
			}
			m_pWorldbaseTCPComm->writeBuffer(m_pWBConnect->m_pvConnect,NET_MSG_WBCHAT_RECV_CHECK_RETURN,(UG_DWORD)CHECK_ERROR,(UG_PCHAR(&(cc.nPlayerid))),sizeof(int),WRITE_WORLDBASE_OVERFLOW);
			return -1;
		}
		else
		{
			g_pLog->UGLog("check playerid ok but data len <= sizeof(CHAT_CHECK_RETURN).");
		}
	}
	else if(CHECK_ERROR == p->dwParam)
	{
		if(p->ulLen == sizeof(CHAT_CHECK_RETURN))
		{
			CHAT_CHECK_RETURN cc;
			Q_memcpy(&cc,p->pchData,sizeof(CHAT_CHECK_RETURN));
			map<UG_PVOID,CPlayer*>::iterator it = m_mapPlayer.find(UG_PVOID(cc.nPlayerPointer));
			if(it != m_mapPlayer.end())
			{
				g_pLog->UGLog("check playerid error.");
				CPlayer* pPlayer = (CPlayer*)(it->second);
				if(pPlayer->m_pvConnect)
				{
					m_pPlayerTCPComm->writeBuffer(pPlayer->m_pvConnect,NET_MSG_PLAYER_LOGIN_CHAT,PLAYER_LOGIN_CHAT_ERROR);
				}
				m_pPlayerTCPComm->disconnectPlayer(UG_PVOID(cc.nPlayerPointer));
				m_listPlayerIdle.push_back(it->second);
				m_mapPlayer.erase(it);
				return -1;
			}
			else
			{
				g_pLog->UGLog("check playerid error but player pointer returned from worldbase error.");
			}
			return -1;
		}
		else
		{
			g_pLog->UGLog("check playerid error but data len = %d <= sizeof(CHAT_CHECK_RETURN).",p->ulLen);
		}
	}
	else
	{
		g_pLog->UGLog("check playerid error because param error = %d.",p->dwParam);
	}
	return -1;
}

UG_LONG CWorldbase::disconnectAll()
{
	if(m_pWBConnect)
	{
		m_pWBConnect->m_bOnline = FALSE;
		m_pWBConnect->m_stepWB = WB_NOT_CONNECTED;
	}
	if(m_pWorldbaseTCPComm)
	{
		if(m_pWBConnect)
		{
			m_pWorldbaseTCPComm->disconnectPlayer(m_pWBConnect->m_pvConnect);
		}
	}
	map<UG_PVOID,CPlayer*>::iterator it;
	for(it = m_mapPlayer.begin(); it != m_mapPlayer.end();)
	{
		if(PLAYER_CONNECT_STEP_OK == it->second->m_stepPlayer)
		{
			it->second->disconnect();
		}
		m_pPlayerTCPComm->disconnectPlayer(it->first);
		m_listPlayerIdle.push_back(it->second);
		it = m_mapPlayer.erase(it);
	}
	m_pPlayerManager->cleanup();
	m_pSceneManager->cleanup();
	return 0;
}
//供非player端口传过来的消息使用，用于断开一个连接，不删除已登入的玩家。
UG_LONG CWorldbase::disconnectPlayer(UG_PVOID pvConnect)
{
	map<UG_PVOID,CPlayer*>::iterator it = m_mapPlayer.find(pvConnect);
	if(it != m_mapPlayer.end())
	{
		m_pPlayerTCPComm->disconnectPlayer(pvConnect);
		if(PLAYER_CONNECT_STEP_OK == it->second->m_stepPlayer)
		{
			it->second->disconnect();
		}
		m_listPlayerIdle.push_back(it->second);
		m_mapPlayer.erase(it);
	}
	return 0;
}

UG_LONG CWorldbase::sendGMMsg(UGINT nChannel,UGINT nPlayerid,UGPCHAR pchNickName,UGPCHAR pchContent)
{
	if(pchNickName)
	{
		if(pchContent)
		{
			g_pLogWorld->UGLog("%d, %d, %s, %s.",nChannel,nPlayerid,pchNickName,pchContent);
		}
		else
		{
			g_pLogWorld->UGLog("%d, %d, %s, .",nChannel,nPlayerid,pchNickName);
		}
	}
	else
	{
		if(pchContent)
		{
			g_pLogWorld->UGLog("%d, %d, , .",nChannel,nPlayerid);
		}
		else
		{
			g_pLogWorld->UGLog("%d, %d, ,%s.",nChannel,nPlayerid,pchContent);
		}
	}
	static GMMSG gmmsg;
	if(pchNickName)
	{
		strncpy(gmmsg.szNickName,pchNickName,MAX_NAME_LENGTH);
		(gmmsg.szNickName)[MAX_NAME_LENGTH] = '\0';
	}
	if(pchContent)
	{
		strncpy(gmmsg.szContent,pchContent,256);
		(gmmsg.szContent)[255] = '\0';
	}
	gmmsg.nPlayerid = nPlayerid;
	time(&(gmmsg.lTimer));
	map<UG_PVOID,CGMTools*>::iterator it;
	for(it = m_mapGMTools.begin(); it != m_mapGMTools.end(); it ++)
	{
		if(it->second->m_bOnline)
		{
			m_pGMTCPComm->writeBuffer((it->second)->m_pvConnect,NET_MSG_GM_MSG_RETURN,(DWORD)nChannel,(UGPCHAR)(&gmmsg),sizeof(GMMSG));
		}
	}
	return 0;
}
