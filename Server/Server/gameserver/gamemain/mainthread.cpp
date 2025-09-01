// MainThread.cpp: implementation of the CMainThread class.
//
//////////////////////////////////////////////////////////////////////
//
#include "stdafx.h"
#include "ZoneServer.h"
#include "MainThread.h"
#include "ZoneServerView.h"
#include "pt_account_world.h"
#include "ExpFld.h"
#include "ClassFld.h"
#include <process.h>
#include "EconomySystem.h"
#include "UnitPartFld.h"
#include "UnitBulletFld.h"
#include "UnitFrameFld.h"
#include "pt_world_msg.h"

CUserDB			g_UserDB[MAX_PLAYER];
CPartyPlayer	g_PartyPlayer[MAX_PLAYER];

CPlayer			g_Player[MAX_PLAYER];
CMonster		g_Monster[MAX_MONSTER];
CNPC			g_NPC[MAX_NPC];
CAnimus			g_Animus[MAX_ANIMUS];
CGuardTower		g_Tower[MAX_TOWER];
CItemBox		g_ItemBox[MAX_ITEMBOX];
CBattleDungeon	g_BattleDungeon[MAX_BATTLEDUNGEON];
CParkingUnit	g_ParkingUnit[MAX_PARKINGUNIT];

CMainThread		g_Main;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMainThread::CMainThread()
{	
	m_bServerClosing = false;
	m_pWorldDB = NULL;
	m_pdwKillMonNum = NULL;	
}

CMainThread::~CMainThread()
{
	m_bRuleThread = false;
	m_bDQSThread = false;

	if ( m_pWorldDB != NULL )
	{
		delete m_pWorldDB;
		m_pWorldDB = NULL;
	}

	if(m_pdwKillMonNum)
		delete [] m_pdwKillMonNum;
}

bool CMainThread::Init()
{
	m_bWorldOpen = false;
	m_bWorldService = false;
	m_bMsgrOpen = false;
	m_bUsingMsgr = false;

	char szLogFile[128];
	sprintf(szLogFile, ".\\Netlog\\SystemError%d.log", ::GetKorLocalTime());
	m_logSystemError.SetWriteLogFile(szLogFile, true);			
																
	sprintf(szLogFile, ".\\Netlog\\LoadingError.log");
	m_logLoadingError.SetWriteLogFile(szLogFile, true, false, true);	

	sprintf(szLogFile, ".\\Netlog\\Dungeon%d.log", ::GetKorLocalTime());
	m_logDungeon.SetWriteLogFile(szLogFile, true);	

	sprintf(szLogFile, ".\\Netlog\\KillMon%d.log", ::GetKorLocalTime());
	m_logKillMon.SetWriteLogFile(szLogFile, true, false, false, false);	

	sprintf(szLogFile, ".\\Netlog\\Cheat%d.log", ::GetKorLocalTime());
	m_logCheat.SetWriteLogFile(szLogFile, true, false, true);
	
	if(!DataFileInit())
		return false;

	if(!ObjectInit())
		return false;	

	if(!m_MapOper.Init())
	{
		::MyMessageBox("CGameServerDoc Error", "m_MapOper.Init() == false" );
		return false;
	}
	
	NetworkInit();

	m_GameMsg.Init(MAX_PLAYER);
	m_tmrCheckAvator.BeginTimer(1000);
	m_tmrAccountPing.BeginTimer(ACCOUNT_WORLD_PING_TERM);

	m_bRuleThread = true;
	::_beginthread(RuleThread, 0, (void*)this);

	m_bDQSThread = true;
	::_beginthread(DQSThread, 0, (void*)this);

	int nMonKindNum = m_tblMonster.GetRecordNum();
	if(nMonKindNum > 0)
	{
		m_pdwKillMonNum = new DWORD [nMonKindNum];
		memset(m_pdwKillMonNum, 0, sizeof(DWORD)*nMonKindNum);
		m_tmKillMon.BeginTimer(60*60*1000);//60분마다..
	}

	//동접자기록 초기화..
	m_MgrConnNum.Init();

	return true;
}

bool CMainThread::DataFileInit()
{
	//Data File..
	if(!::WriteTableData(item_tbl_num, m_tblItemData, NULL))
		return false;

	//효과데이터..
	char* pszEffectFileName[EFFECT_CODE_NUM] = { ".\\script\\skill.dat", ".\\script\\force.dat", ".\\script\\ClassSkill.dat" };
	int nEffectDataSize[EFFECT_CODE_NUM] = { sizeof(_skill_fld), sizeof(_force_fld), sizeof(_skill_fld) };
	for(int i = 0; i < EFFECT_CODE_NUM; i++)
	{
		if(!m_tblEffectData[i].ReadRecord(pszEffectFileName[i], nEffectDataSize[i]))	
			return false;
	}

	if(!m_tblMapData.ReadScript(".\\Map\\Map_Data.spt"))
		return false;

	if(!m_tblClass.ReadRecord(".\\Script\\Class.dat", sizeof(_class_fld)))
		return false;

	if(!m_tblPlayer.ReadRecord(".\\Script\\PlayerCharacter.dat", sizeof(_player_fld)))
		return false;
	
	if(!m_tblMonster.ReadRecord(".\\Script\\MonsterCharacter.dat", sizeof(_monster_fld)))	
		return false;

	if(!m_tblNPC.ReadRecord(".\\Script\\NPCharacter.dat", sizeof(_npc_fld)))	
		return false;

	if(!m_tblAnimus.ReadRecord(".\\Script\\AnimusItem.dat", sizeof(_AnimusItem_fld)))	
		return false;

	if(!m_tblExp.ReadRecord(".\\Script\\Exp.dat", sizeof(_exp_fld)))
		return false;

	if(!m_tblWeaponBulletLink.ReadRecord(".\\Script\\WeaponBulletLink.dat", &m_tblItemData[tbl_code_weapon]))
		return false;

	if(!m_tblItemLoot.ReadRecord(".\\Script\\ItemLooting.dat", m_tblItemData))
		return false;

	if(!m_tblOreCutting.ReadRecord(".\\Script\\OreCutting.dat", &m_tblItemData[tbl_code_ore], &m_tblItemData[tbl_code_res]))
		return false;

	if(!m_tblItemMakeData.ReadRecord(".\\Script\\ItemMakeData.dat", m_tblItemData))
		return false;

	if(!m_tblItemUpgrade.ReadRecord(".\\Script\\ItemUpgrade.dat", &m_tblItemData[tbl_code_res]))
		return false;

	//유닛데이터..
	char* pszUnitPartFileName[UNIT_PART_NUM] = {	".\\script\\UnitHead.dat", 
													".\\script\\UnitUpper.dat", 
													".\\script\\UnitLower.dat", 
													".\\script\\UnitArms.dat", 
													".\\script\\UnitShoulder.dat", 
													".\\script\\UnitBack.dat"		};
	for(i = 0; i < UNIT_PART_NUM; i++)
	{
		if(!m_tblUnitPart[i].ReadRecord(pszUnitPartFileName[i], sizeof(_UnitPart_fld)))	
			return false;
	}

	if(!m_tblUnitBullet.ReadRecord(".\\script\\UnitBullet.dat", sizeof(_UnitBullet_fld)))	
		return false;

	if(!m_tblUnitFrame.ReadRecord(".\\script\\UnitFrame.dat", sizeof(_UnitFrame_fld)))	
		return false;

	if(!check_loaded_data())
		return false;

	//계정서버주소
	char szAccountIPBuffer[128];
	GetPrivateProfileString("System", "AccountAddress", "X", szAccountIPBuffer, sizeof(szAccountIPBuffer), ".\\Initialize\\WorldSystem.ini");
	if(!strcmp(szAccountIPBuffer, "X"))
		return false;

	m_dwAccountIP = inet_addr(szAccountIPBuffer);

	//월드이름
	GetPrivateProfileString("System", "WorldName", "X", m_szWorldName, sizeof(m_szWorldName), ".\\Initialize\\WorldSystem.ini");
	if(!strcmp(m_szWorldName, "X"))
		return false;

	//messenger사용
	char szMsgrUse[32];
	GetPrivateProfileString("Messenger", "Use", "FALSE", szMsgrUse, sizeof(szMsgrUse), ".\\Initialize\\WorldSystem.ini");
	if(!strcmp(szMsgrUse, "TRUE"))
		m_bUsingMsgr = true;

	return true;
}

void CMainThread::NetworkInit()
{
	_NET_TYPE_PARAM typeParam[NET_LINE_NUM];

	typeParam[client_line].m_bServer = TRUE;
	typeParam[client_line].m_wSocketMaxNum = MAX_PLAYER;
	typeParam[client_line].m_bRealSockCheck = TRUE;
//	typeParam[client_line].m_bRealClientCheck = TRUE;
	typeParam[client_line].m_bSendThread = TRUE;
	typeParam[client_line].m_bSystemLogFile = TRUE;
	typeParam[client_line].m_byRecvThreadNum = 4;
	typeParam[client_line].m_byRecvSleepTime = 10;
	typeParam[client_line].m_bySendSleepTime = 0;
	typeParam[client_line].m_wPort = PORT_SERVER_MAP;
	typeParam[client_line].m_bAcceptIPCheck = TRUE;
	typeParam[client_line].m_bKeyCheck = TRUE;
	typeParam[client_line].m_bSpeedHackCheck = TRUE;
	typeParam[client_line].m_bOddMsgWriteLog = TRUE;
	typeParam[client_line].m_bOddMsgDisconnect = FALSE;
	typeParam[client_line].m_dwSocketRecycleTerm = 60000;
//	typeParam[client_line].m_bOddMsgDisconnect = TRUE;
	typeParam[client_line].m_dwProcessMsgNumPerLoop = 4;	//클라이언트의 메시지는 한번에 최대 4개씩만 처리해준다.
	sprintf(typeParam[client_line].m_szModuleName, "ClientLine");

	typeParam[account_line].m_bServer = FALSE;
	typeParam[account_line].m_wSocketMaxNum = 1;
	typeParam[account_line].m_bSvrToS = TRUE;
	typeParam[account_line].m_bRealSockCheck = TRUE;
//	typeParam[account_line].m_bRealClientCheck = TRUE;
	typeParam[account_line].m_bSendThread = TRUE;
	typeParam[account_line].m_bSystemLogFile = TRUE;
	typeParam[account_line].m_byRecvThreadNum = 1;
	typeParam[account_line].m_byRecvSleepTime = 1;
	typeParam[account_line].m_bOddMsgWriteLog = TRUE;
	sprintf(typeParam[account_line].m_szModuleName, "AccountLine");

	typeParam[messenger_line].m_bServer = TRUE;
	typeParam[messenger_line].m_wSocketMaxNum = 1;
	typeParam[messenger_line].m_bSvrToS = TRUE;
	typeParam[messenger_line].m_bRealSockCheck = TRUE;
//	typeParam[messenger_line].m_bRealClientCheck = TRUE;
	typeParam[messenger_line].m_bSendThread = TRUE;
	typeParam[messenger_line].m_wPort = PORT_WORLD_MESSENGER;
	typeParam[messenger_line].m_bSystemLogFile = TRUE;
	typeParam[messenger_line].m_byRecvThreadNum = 1;
	typeParam[messenger_line].m_byRecvSleepTime = 1;
	typeParam[messenger_line].m_bOddMsgWriteLog = TRUE;
	sprintf(typeParam[messenger_line].m_szModuleName, "MessengerLine");

	m_Network.SetNetSystem(NET_LINE_NUM, typeParam, "GameServer");
}

bool CMainThread::ObjectInit()
{
	int i;

//아바타데이터
	for(i = 0; i < MAX_PLAYER; i++)
	{
		g_UserDB[i].Init(i);
	}

//월드아바타
	for(i = 0; i < MAX_PLAYER; i++)
	{
		g_PartyPlayer[i].Init(i);
	}

//플레이어
	for(i = 0; i < MAX_PLAYER; i++)
	{
		_object_id ID(obj_kind_char, obj_id_player, i);
		g_Player[i].Init(&ID);
	}

//몬스터
	for(i = 0; i < MAX_MONSTER; i++)
	{
		_object_id ID(obj_kind_char, obj_id_monster, i);
		g_Monster[i].Init(&ID); 
	}

//NPC
	for(i = 0; i < MAX_NPC; i++)
	{
		_object_id ID(obj_kind_char, obj_id_npc, i);
		g_NPC[i].Init(&ID);
	}

//Animus
	for(i = 0; i < MAX_ANIMUS; i++)
	{
		_object_id ID(obj_kind_char, obj_id_animus, i);
		g_Animus[i].Init(&ID);
	}

//GuardTower
	for(i = 0; i < MAX_TOWER; i++)
	{
		_object_id ID(obj_kind_char, obj_id_tower, i);
		g_Tower[i].Init(&ID);
	}

//아이템박스
	for(i = 0; i < MAX_ITEMBOX; i++)
	{
		_object_id ID(obj_kind_item, obj_id_itembox, i);
		g_ItemBox[i].Init(&ID);
	}

//던젼게이트
	for(i = 0; i < MAX_BATTLEDUNGEON; i++)
	{
		_object_id ID(obj_kind_item, obj_id_dungeongate, i);
		g_BattleDungeon[i].Init(&ID);
	}

//파킹유닛
	for(i = 0; i < MAX_PARKINGUNIT; i++)
	{
		_object_id ID(obj_kind_item, obj_id_parkingunit, i);
		g_ParkingUnit[i].Init(&ID);
	}

	//캐릭터 데이터리스트 초기화..
	CPlayer::SetStaticMember();
	CAnimus::SetStaticMember();
	CAttack::SetStaticMember(&m_tblEffectData[effect_code_skill]);
	_WEAPON_PARAM::SetStaticMember(&m_tblItemData[tbl_code_weapon]);
	_MASTERY_PARAM::SetStaticMember(&m_tblEffectData[effect_code_skill], &m_tblEffectData[effect_code_force]);

	m_listDQSData.SetList(max_db_syn_data_num);
	m_listDQSDataComplete.SetList(max_db_syn_data_num);
	m_listDQSDataEmpty.SetList(max_db_syn_data_num);
	for(i = 0; i < max_db_syn_data_num; i++)
		m_listDQSDataEmpty.PushNode_Back(i);

	//Lock Chat Init..
	for(i = 0; i < max_lock_chat; i++)
		m_LockChatData[i].Init();
	m_tmLockChatDataCheck.BeginTimer(60*1000);//1분

	return true;
}

CGameObject* CMainThread::GetObject(_object_id* pObjID)
{
	return GetObject(pObjID->m_byKind, pObjID->m_byID, pObjID->m_wIndex);
}

CGameObject* CMainThread::GetObject(int kind, int id, int index)
{
	switch(kind)
	{
	case obj_kind_char:
		switch(id)
		{
		case obj_id_player:
			if(index >= MAX_PLAYER || index < 0)
				return NULL;
			return &g_Player[index];
		
		case obj_id_monster:
			if(index >= MAX_MONSTER || index < 0)
				return NULL;
			return &g_Monster[index];

		case obj_id_npc:
			if(index >= MAX_NPC || index < 0)
				return NULL;
			return &g_NPC[index];

		case obj_id_animus:
			if(index >= MAX_ANIMUS || index < 0)
				return NULL;
			return &g_Animus[index];
		}
		break;

	case obj_kind_item:
		return NULL;
	}
	return NULL;
}

CGameObject* CMainThread::GetChar(char* szCharName)
{
	for(int i = 0; i < MAX_PLAYER; i++)
	{
		if(g_Player[i].m_bLive)
		{
			if(!strcmp(g_Player[i].m_Param.GetCharName(), szCharName))
				return &g_Player[i];
		}
	}

	return NULL;
}

WORD CMainThread::GetPlayerIndexFromSerial(DWORD dwSerial)
{
	for(int i = 0; i < MAX_PLAYER; i++)
	{
		if(!g_Player[i].m_bLive)
			continue;

		if(g_Player[i].m_dwObjSerial == dwSerial)
			return i;
	}

	return 0xFFFF;
}

bool CMainThread::IsChatLockAccount(DWORD dwAccountSerial)
{
	for(int i = 0; i < max_lock_chat; i++)
	{
		_LOCK_CHAT_DATA* p = &m_LockChatData[i];

		if(!p->bLock)
			continue;

		if(dwAccountSerial == p->dwAccountSerial)
			return true;
	}

	return false;
}

bool CMainThread::AddChatLockAccount(CUserDB* pUser, DWORD dwTime)
{
	for(int i = 0; i < max_lock_chat; i++)
	{
		_LOCK_CHAT_DATA* p = &m_LockChatData[i];

		if(p->bLock)
			continue;

		p->dwAccountSerial = pUser->m_dwAccountSerial;
		p->dwDurTime = dwTime;
		p->dwStartTime = timeGetTime();
		strcpy(p->szName, pUser->m_szAvatorName);
		strcpy(p->szID, pUser->m_szAccountID);

		p->bLock = true;

		pUser->SetChatLock(true);

		char szMsg[128];
		sprintf(szMsg, "%s 케릭터 %d시간동안 채팅금지", pUser->m_szAvatorName, dwTime);

		g_Player[pUser->m_idWorld.wIndex].SendData_ChatTrans(CHAT_TYPE_OPERATOR, 0xFFFFFFFF, strlen(szMsg), szMsg);

		return true;
	}

	return false;
}

bool CMainThread::DelChatLockAccount(char* pszAvatorName)
{
	_LOCK_CHAT_DATA* pLockData = NULL;
	for(int i = 0; i < max_lock_chat; i++)
	{
		_LOCK_CHAT_DATA* p = &m_LockChatData[i];

		if(!p->bLock)
			continue;

		if(!strcmp(p->szName, pszAvatorName))
		{
			pLockData = p;
			p->bLock = false;
			break;
		}
	}

	if(!pLockData)
		return false;

	//현재 접속중이면 바로 해제해준다..
	for(int a = 0; a < MAX_PLAYER; a++)
	{
		CUserDB* pUser = &g_UserDB[a];

		if(!pUser->m_bActive)
			continue;

		if(pUser->m_dwAccountSerial == pLockData->dwAccountSerial)
		{
			pUser->SetChatLock(false);

			char szMsg[128];
			sprintf(szMsg, "%s 케릭터 채팅금지 해제", pUser->m_szAvatorName, pUser->m_szAccountID);

			g_Player[pUser->m_idWorld.wIndex].SendData_ChatTrans(CHAT_TYPE_OPERATOR, 0xFFFFFFFF, strlen(szMsg), szMsg);

			break;
		}
	}

	return true;
}

bool CMainThread::DatabaseInit(char* pszDBName)
{
	/////////////////////////////////////

	if ( m_pWorldDB == NULL )
	{
		m_pWorldDB = new CRFWorldDatabase();

		if ( m_pWorldDB->StartDataBase(pszDBName, "odinWorld", "e3aksgdms*dnjfem" ) == false )
		{
			return false;
		}
		else
		{
			//저장안됀 케릭터 업데이트하기..
			AllAvatorDBRestore();

			DWORD dwDate = ::eGetLocalDate();
			int	nHisDataNum = 0;
			_economy_history_data EcoHisData[MAX_ECONOMY_HISTORY];
			_economy_history_data EcoCurData;
			int	nCurMgrValue = 1000;
			int	nNextMgrValue = 1000;

			//Economy History.. 읽어오기..
			db_Select_Economy_History(&EcoCurData, &nCurMgrValue, &nNextMgrValue, EcoHisData, &nHisDataNum, dwDate);

			::eInitEconomySystem(nCurMgrValue, nNextMgrValue, EcoHisData, nHisDataNum, EcoCurData.dTradeDalant, EcoCurData.dTradeGold);//경제시시템 초기화..
		}

		return true;
	}

	return true;
}

void CMainThread::RuleThread(void* pv)
{
	CMainThread* pDoc = (CMainThread*)pv;
	::srand((unsigned)time(NULL));
	int nSleepTimer = 0;

	while(pDoc->m_bRuleThread)
	{
		pDoc->m_MainFrameRate.CalcSpeedPerFrame();
		pDoc->OnRun();	

		if(++nSleepTimer > 1)
		{
			Sleep(1);
			nSleepTimer = 0;
		}
	}
	::_endthreadex(0);
}

void CMainThread::DQSThread(void* pv)
{
	CMainThread* pDoc = (CMainThread*)pv;
	DWORD dwDataIndex;

	while(pDoc->m_bDQSThread)
	{
		pDoc->m_DBFrameRate.CalcSpeedPerFrame();

		while(pDoc->m_listDQSData.PopNode_Front(&dwDataIndex))
		{
			_DB_QRY_SYN_DATA* p = &pDoc->m_DBQrySynData[dwDataIndex];
			switch(p->m_byQryCase)
			{
			case _DB_QRY_SYN_DATA::qry_case_reged:{
				_qry_sheet_reged* pSheet = (_qry_sheet_reged*)p->m_sData;
				p->m_byResult = pDoc->db_Reged_Avator(pSheet->dwAccountSerial, pSheet->RegedData);
			  }break;

			case _DB_QRY_SYN_DATA::qry_case_insert:{
				_qry_sheet_insert* pSheet = (_qry_sheet_insert*)p->m_sData;
				p->m_byResult = pDoc->db_Insert_Avator(pSheet->dwAccountSerial, pSheet->szAccountID, &pSheet->InsertData, &pSheet->InsertData.m_dwRecordNum);
			  }break;

			case _DB_QRY_SYN_DATA::qry_case_delete:{
				_qry_sheet_delete* pSheet = (_qry_sheet_delete*)p->m_sData;
				p->m_byResult = pDoc->db_Delete_Avator(pSheet->dwAvatorSerial, pSheet->byRaceCode);
			  }break;

			case _DB_QRY_SYN_DATA::qry_case_load:{
				_qry_sheet_load* pSheet = (_qry_sheet_load*)p->m_sData;
				p->m_byResult = pDoc->db_Load_Avator(pSheet->dwAvatorSerial, &pSheet->LoadData, false);
				if(p->m_byResult == RET_CODE_SUCCESS)
				{//로드에 성공햇으면 hbk파일을 만든다..
					CUserDB::ReRangeClientIndex(&pSheet->LoadData);
					sprintf(pSheet->szHBKFileName, ".\\hbk\\%d.hbk", pSheet->LoadData.dbAvator.m_dwRecordNum);
					if(!CUserDB::SetBKFile(pSheet->szHBKFileName, &pSheet->LoadData))
						p->m_byResult = RET_CODE_HBKFILE_ERROR;
				}
			  }break;

			case _DB_QRY_SYN_DATA::qry_case_logout:{
				_qry_sheet_logout* pSheet = (_qry_sheet_logout*)p->m_sData;
				p->m_byResult = pDoc->db_Update_Avator(pSheet->dwAvatorSerial, &pSheet->NewData, &pSheet->OldData, pSheet->bCheckLowHigh);
				CUserDB::ReleaseBKFile(pSheet->szHBKFileName);
			  }break;

			case _DB_QRY_SYN_DATA::qry_case_lobby:{
				_qry_sheet_lobby* pSheet = (_qry_sheet_lobby*)p->m_sData;
				p->m_byResult = pDoc->db_Update_Avator(pSheet->dwAvatorSerial, &pSheet->NewData, &pSheet->OldData, true);
				CUserDB::ReleaseBKFile(pSheet->szHBKFileName);
			  }break;

			case _DB_QRY_SYN_DATA::log_case_lv:{
				_log_sheet_lv* pSheet = (_log_sheet_lv*)p->m_sData;
				pDoc->db_Log_AvatorLevel(pSheet->dwTotalPlayMin, pSheet->dwAvatorSerial, pSheet->byLv);
			  }break;

			case _DB_QRY_SYN_DATA::log_case_usernum:{
				_log_sheet_usernum* pSheet = (_log_sheet_usernum*)p->m_sData;
				pDoc->db_Log_UserNum(pSheet->nAveragePerHour, pSheet->nMaxPerHour);
			  }break;

			case _DB_QRY_SYN_DATA::log_case_economy:{
				_log_sheet_economy* pSheet = (_log_sheet_economy*)p->m_sData;
				pDoc->db_Insert_Economy_History(pSheet->dwDate, pSheet->dTradeDalant, pSheet->dTradeGold, pSheet->nMgrValue);
			  }break;
			}

			if(p->m_byResult != RET_CODE_SUCCESS)
			{
				if(p->m_byResult != RET_CODE_REGED_AVATOR)
					pDoc->m_logSystemError.Write("DQSThread.. DB fail.. account: %d, case: %d, ret: %d", p->m_dwAccountSerial, p->m_byQryCase, p->m_byResult);
			}
			
			p->m_bLoad = true;
			pDoc->m_listDQSDataComplete.PushNode_Back(dwDataIndex);
			Sleep(0);
		}
		Sleep(1);
	}

	::_endthreadex(0);
}

void CMainThread::DQSCompleteProcess()
{
	DWORD dwDataIndex;
	while(m_listDQSDataComplete.PopNode_Front(&dwDataIndex))
	{
		_DB_QRY_SYN_DATA* p = &m_DBQrySynData[dwDataIndex];
		switch(p->m_byQryCase)
		{
		case _DB_QRY_SYN_DATA::qry_case_reged:{
			Reged_Avator_Complete(p);
		  }break;

		case _DB_QRY_SYN_DATA::qry_case_insert:{
			Insert_Avator_Complete(p);
		  }break;

		case _DB_QRY_SYN_DATA::qry_case_delete:{
			Delete_Avator_Complete(p);
		  }break;

		case _DB_QRY_SYN_DATA::qry_case_load:{
			Select_Avator_Complete(p);
		  }break;

		case _DB_QRY_SYN_DATA::qry_case_logout:{
			Logout_Account_Complete(p);
		  }break;

		case _DB_QRY_SYN_DATA::qry_case_lobby:{
			Lobby_Account_Complete(p);
		  }break;
		}

		p->m_bUse = false;
		p->m_bLoad = false;
		m_listDQSDataEmpty.PushNode_Back(dwDataIndex);
	}
}

void CMainThread::Reged_Avator_Complete(_DB_QRY_SYN_DATA* pData)
{
	BYTE byRet = RET_CODE_SUCCESS;
	CUserDB* pObj = &g_UserDB[pData->m_idWorld.wIndex];

	if(!pObj->m_bActive || pObj->m_idWorld.dwSerial != pData->m_idWorld.dwSerial)
		return;

	_qry_sheet_reged* pQryData =  (_qry_sheet_reged*)pData->m_sData;

	pObj->Reged_Char_Complete(pData->m_byResult, pQryData->RegedData);
}

void CMainThread::Insert_Avator_Complete(_DB_QRY_SYN_DATA* pData)
{
	BYTE byRet = RET_CODE_SUCCESS;
	CUserDB* pObj = &g_UserDB[pData->m_idWorld.wIndex];

	if(!pObj->m_bActive || pObj->m_idWorld.dwSerial != pData->m_idWorld.dwSerial)
		return;

	_qry_sheet_insert* pQryData = (_qry_sheet_insert*)pData->m_sData;

	pObj->Insert_Char_Complete(pData->m_byResult, &pQryData->InsertData);
}

void CMainThread::Delete_Avator_Complete(_DB_QRY_SYN_DATA* pData)
{
	BYTE byRet = RET_CODE_SUCCESS;
	CUserDB* pObj = &g_UserDB[pData->m_idWorld.wIndex];

	if(!pObj->m_bActive || pObj->m_idWorld.dwSerial != pData->m_idWorld.dwSerial)
		return;

	_qry_sheet_delete* pQryData = (_qry_sheet_delete*)pData->m_sData;

	pObj->Delete_Char_Complete(pData->m_byResult, pQryData->bySlotIndex);	
}

void CMainThread::Select_Avator_Complete(_DB_QRY_SYN_DATA* pData)
{
	BYTE byRet = RET_CODE_SUCCESS;
	CUserDB* pObj = &g_UserDB[pData->m_idWorld.wIndex];

	if(!pObj->m_bActive || pObj->m_idWorld.dwSerial != pData->m_idWorld.dwSerial)
		return;

	_qry_sheet_load* pQryData = (_qry_sheet_load*)pData->m_sData;
	
	pObj->Select_Char_Complete(pData->m_byResult, &pQryData->LoadData, pQryData->szHBKFileName);
}

void CMainThread::Logout_Account_Complete(_DB_QRY_SYN_DATA* pData)
{
	BYTE byRet = RET_CODE_SUCCESS;
	CUserDB* pObj = &g_UserDB[pData->m_idWorld.wIndex];

	if(!pObj->m_bActive || pObj->m_idWorld.dwSerial != pData->m_idWorld.dwSerial)
		return;

	pObj->Exit_Account_Complete(pData->m_byResult);
}

void CMainThread::Lobby_Account_Complete(_DB_QRY_SYN_DATA* pData)
{
	BYTE byRet = RET_CODE_SUCCESS;
	CUserDB* pObj = &g_UserDB[pData->m_idWorld.wIndex];

	if(!pObj->m_bActive || pObj->m_idWorld.dwSerial != pData->m_idWorld.dwSerial)
		return;

	pObj->Lobby_Char_Complete(pData->m_byResult);
}

bool CMainThread::PushDQSData(DWORD dwAccountSerial, _CLID* pidWorld, BYTE byQryCase, char* pQryData, int nSize)
{
	DWORD dwEmptyIndex;
	if(!m_listDQSDataEmpty.PopNode_Front(&dwEmptyIndex))
	{
		m_logSystemError.Write("%d.. m_listDQSDataEmpty.PopNode_Front() => failed ", byQryCase);
		return false;
	}

	_DB_QRY_SYN_DATA* pData = &m_DBQrySynData[dwEmptyIndex];
	
	pData->m_dwAccountSerial = dwAccountSerial;
	if(pidWorld)
		memcpy(&pData->m_idWorld, pidWorld, sizeof(_CLID));
	pData->m_byQryCase = byQryCase;
	if(pQryData)
		memcpy(pData->m_sData, pQryData, nSize);
	
	if(!m_listDQSData.PushNode_Back(dwEmptyIndex))
	{
		m_logSystemError.Write("%d : m_listDQSData.PushNode_Back() => failed ", byQryCase);
		return false;
	}

	pData->m_bUse = true;
	pData->m_bLoad = false;

	return true;
}

void CMainThread::OnRun()
{
	m_MapDisplay.DrawDisplay();
	m_MapOper.OnLoop();
	m_Network.OnLoop();
	DQSCompleteProcess();
	CheckAvatorState();
		
	CheckAccountLineState();
	ForceCloseUserInTiming();
	::eUpdateEconomySystem();//경제시스템.. 경제지표 체크..
	CheckConnNumLog();

	LogMonKillCount();

	m_GameMsg.PumpMsgList();
}

void CMainThread::pc_OpenWorldSuccessResult(BYTE byWorldCode, bool bControlOpen, DWORD dwControlIP, WORD wControlPort, DWORD* pdwControlMasterKey, char* pszDBName)//, DWORD dwDBIP, char* pszDBAccount, char* pszDBPassword)
{
	char szDate[128], szTime[128];
	_strdate(szDate);
	_strtime(szTime);

	::__trace("%s-%s: Open World", szDate, szTime);

	m_byWorldCode = byWorldCode;

	if(DatabaseInit(pszDBName))
	{
		m_bWorldOpen = true;

		if(!(m_bUsingMsgr && !m_bMsgrOpen))
			SerivceSelfStart();	//메신져서버를 사용하는데 메신져가 로그인한 상태가 아닌경우를 제외하고
	}
}

void CMainThread::pc_TransIPKeyInform(DWORD dwAccountSerial, char* pszAccountID, BYTE byUserDgr, BYTE bySubDgr, DWORD* pdwKey, _GLBID* pgidGlobal, DWORD dwClientIP)
{
	//못끈고 유령으로 남은 자신의 캐릭이 있는지 확인..
	for(int i = 0; i < MAX_PLAYER; i++)
	{
		CUserDB* p = &g_UserDB[i];

		if(!p->m_bActive)
			continue;
		
		if(p->m_dwAccountSerial == dwAccountSerial)	//발견..
		{
			//존에 강제종료알림..
			p->ForceCloseCommand(true, dwClientIP);
			m_logSystemError.Write("pc_TransIPKeyInform : 유령계정이 존재..(Account: %d))", dwAccountSerial);
			return;
		}
	}

	//대기 리스트에 삽입..
	for(i = 0; i < MAX_WAIT_NETMOVE_FROM_WORLD; i++)
	{
		_WAIT_ENTER_ACCOUNT* p = &m_WaitEnterAccount[i];
		if(p->m_bLoad)
			continue;

		p->SetData(dwAccountSerial, pszAccountID, byUserDgr, bySubDgr, pgidGlobal, pdwKey);
		
		//push ip..
		if(!m_Network.m_Process[client_line].m_NetSocket.PushIPCheckList(dwClientIP))
			return;
		if(!m_Network.m_Process[client_line].PushKeyCheckList(dwAccountSerial, dwClientIP, pdwKey, CHECK_KEY_NUM))
			return;
		break;
	}
	
	_trans_account_report_wrac Send;

	Send.byRetCode = 0;
	memcpy(&Send.gidGlobal, pgidGlobal, sizeof(_GLBID));

	BYTE byType[msg_header_num] = {system_msg, trans_account_report_wrac};
	m_Network.m_pProcess[account_line]->LoadSendMsg(0, byType, (char*)&Send, Send.size());
}

void CMainThread::pc_ForceCloseCommand(_CLID* pidWorld, bool bDirectly, bool bPushClose, DWORD dwPushIP)
{
	CUserDB* pUser = &g_UserDB[pidWorld->wIndex];

	if(!pUser->m_bActive)
	{
//		m_logSystemError.Write("pc_ForceCloseCommand : 이미 나감.. IP(%s)", inet_ntoa(*(in_addr*)&dwPushIP));
		return;
	}

	if(pUser->m_idWorld.dwSerial != pidWorld->dwSerial)
	{
//		m_logSystemError.Write("pc_ForceCloseCommand : 월드시리얼틀림.. IP(%s)", inet_ntoa(*(in_addr*)&dwPushIP));
		return;
	}

	pUser->ForceCloseCommand(bPushClose, dwPushIP);
}

void CMainThread::pc_AllUserKickInform()
{
	if(m_bServerClosing)
		return;

	m_bServerClosing = true;

	m_tmForceUserExit.BeginTimer(100);
	m_nForceExitSocketIndexOffset = 0;
}

void CMainThread::pc_AllUserMsgInform(BYTE byMsgSize, char* szMsg)
{
	for(int i= 0; i < MAX_PLAYER; i++)
	{
		if(g_Player[i].m_bLive)
			g_Player[i].SendData_ChatTrans(CHAT_TYPE_OPERATOR, 0xFFFFFFFF, byMsgSize, szMsg);
	}
}

void CMainThread::pc_AlterWorldService(bool bSerivce)
{
	SerivceForceSet(bSerivce);
}

void CMainThread::pc_OpenMsgrRequest(char* pszWorldName, DWORD dwLocalIP)
{
	bool bSucc = true;
	char szErrMsg[128];

	if(!m_bUsingMsgr)
	{
		bSucc = false;
		sprintf(szErrMsg, "메신져서버를 사용하지 않는다.");
		goto RESULT;
	}

	if(m_bMsgrOpen)
	{	
		bSucc = false;
		sprintf(szErrMsg, "이미 접속한 상태임");
		goto RESULT;
	}

RESULT:

	if(!bSucc)
	{	//실패..
		_open_msgr_failure_wrmg Send;

		strcpy(Send.szErrCode, szErrMsg);

		m_Network.m_pProcess[account_line]->LoadSendMsg(0, Send.head(), (char*)&Send, Send.size());	
	}
	else
	{	//성공
		m_bMsgrOpen = true;
		m_dwMessengerIP = dwLocalIP;

		_open_msgr_success_wrmg Send;

		Send.wMapNum = m_MapOper.m_nMapNum;

		for(int i = 0; i < m_MapOper.m_nMapNum; i++)
		{
			CMapData* p = m_MapOper.GetMap(i);
			_sec_info* pSecInfo = p->GetSecInfo();

			Send.MapList[i].wLayerNum = p->m_pMapSet->m_nLayerNum;
			Send.MapList[i].dwSectorNum = pSecInfo->m_nSecNum;
			Send.MapList[i].wSecNumW = pSecInfo->m_nSecNumW;
		}
		
		m_Network.m_pProcess[messenger_line]->LoadSendMsg(0, Send.head(), (char*)&Send, Send.size());
	}
}

void CMainThread::pc_EnterUserMessengerReport(_CLID* pidWorld, _CLID* pidLocal)
{
	CUserDB* pUser = &g_UserDB[pidWorld->wIndex];

	pUser->EnterMessenger(pidWorld, pidLocal);
}

void CMainThread::pc_ExitUserMessengerReport(_CLID* pidWorld)
{
	CUserDB* pUser = &g_UserDB[pidWorld->wIndex];

	pUser->ExitMessenger(pidWorld);
}

void CMainThread::ForceCloseUserInTiming()
{
	if(!m_tmForceUserExit.CountingTimer())
		return;

	int nSocketIndex = -1;
	for(int i = m_nForceExitSocketIndexOffset; i < MAX_PLAYER; i++)
	{
		_socket* p = m_Network.GetSocket(client_line, i);
		if(p->m_bAccept)
		{
			nSocketIndex = i;
			break;
		}
	}
	
	if(nSocketIndex != -1)
	{
		g_UserDB[nSocketIndex].ForceCloseCommand(false, 0);
		m_nForceExitSocketIndexOffset = nSocketIndex + 1;
	}
	else
	{
		m_tmForceUserExit.StopTimer();
		m_nForceExitSocketIndexOffset = 0;
		m_bServerClosing = false;
	}
}

void CMainThread::CheckAccountLineState()
{
	if(m_bWorldOpen)
	{
		if(m_tmrAccountPing.CountingTimer())
			PingToAccount();
	}
	else
	{
		//메신져서버 사용 월드라면 메신져가 로그인됐는지 확인하고 커넥션을 시도함
		if(m_bUsingMsgr && !m_bMsgrOpen)
			return;

		_socket* pSocket = m_Network.GetSocket(account_line, 0);
		if(!pSocket->m_bAccept)
		{
			DWORD dwNewTick = timeGetTime();

			DWORD dwGap = dwNewTick-m_dwCheckAccountOldTick;
			if(dwGap > 5000)//5초마다..
			{
				m_dwCheckAccountOldTick = dwNewTick;

				if(m_Network.Connect(account_line, 0, m_dwAccountIP, PORT_ACCOUNT_WORLD))
					::__trace("Account Server Connectting Success");
				else
					::__trace("Account Server Connectting Fail");
			}
		}
	}
}

void CMainThread::CheckAvatorState()
{
	if(!m_tmrCheckAvator.CountingTimer())
		return;

	DWORD dwTime = timeGetTime();

	//플레이시간 및 bk파일 정리 체크..
	for(DWORD i = 0; i < MAX_PLAYER; i++)
	{
		CUserDB* p = &g_UserDB[i];

		if(!p->m_bActive)
			continue;

		p->TotalPlayMinCheck();
		p->VariableUpdateCheck();

		if(!p->UpdateBKFile())
			m_Network.Close(client_line, p->m_idWorld.wIndex, false);
	}

	//로비로 들어올때 일정시간 딜레이를주기
	CUserDB::s_MoveLobbyDelay.CheckOnLoop();

	//케릭터 선택후 메신져서버 입장키알려줌을 일정시간 딜레이후 주기
	CUserDB::s_MessengerKeyDelay.CheckOnLoop();

	//메신져서버로 접속대기시간안에 들어왔는지체크
	CUserDB::s_MessengerEnterCheck.CheckOnLoop();

	//계정에서 월드로 올 유져 체크
	for(i = 0; i < MAX_WAIT_NETMOVE_FROM_WORLD; i++)
	{
		_WAIT_ENTER_ACCOUNT* p = &m_WaitEnterAccount[i];

		if(!p->m_bLoad)
			continue;

		if(dwTime - p->m_dwLoadTime > NETMOVE_WAIT_TERM)		//NETMOVE_WAIT_TERM초 안에 안들어오면..자른다..
			p->Release();
	}

	CPlayer::OnLoop_Static();
}

void CMainThread::CheckLockChatData()
{
	if(!m_tmLockChatDataCheck.CountingTimer())
		return;

	DWORD dwCurTime = timeGetTime();

	for(int i = 0; i < max_lock_chat; i++)
	{
		_LOCK_CHAT_DATA* p = &m_LockChatData[i];

		if(!p->bLock)
			continue;

		if(dwCurTime - p->dwStartTime > p->dwDurTime)
		{//해제
			for(int a = 0; a < MAX_PLAYER; a++)
			{
				CUserDB* pUser = &g_UserDB[i];
				if(!pUser->m_bActive)
					continue;

				if(pUser->m_dwAccountSerial == p->dwAccountSerial)
				{
					pUser->SetChatLock(false);
				}
			}
			p->Init();
		}
	}
}

void CMainThread::CheckConnNumLog()
{
	_USER_NUM_SHEET* pSheet = m_MgrConnNum.Check(CUserDB::s_nLoginNum);
	if(pSheet)
	{
		_log_sheet_usernum Sheet;

		Sheet.nAveragePerHour = pSheet->nAveragePerHour;
		Sheet.nMaxPerHour = pSheet->nMaxPerHour;

		g_Main.PushDQSData(0xFFFFFFFF, NULL, _DB_QRY_SYN_DATA::log_case_usernum, (char*)&Sheet, Sheet.size());
	}
}

void CMainThread::AllAvatorDBRestore()
{
	//아바타 history의 파일을 열어 저장못한 아바타의 데이터를 저장한다.
	char*			pszFileName[5000];
	int				nFileNum = ::FindAllFile(".\\HBK\\*.hbk", pszFileName, 5000);
	int				nSuccCount = 0;

	if(nFileNum == 0)
		return;
	
	char szMessage[128];
	sprintf(szMessage, "%d개 hbk파일발견.. 복구OK?", nFileNum);
	int nRet = ::MessageBox(NULL, szMessage, "warning", MB_OKCANCEL);
	if(nRet != IDOK)
		return;

	for(int i = 0; i < nFileNum; i++)
	{
		char szSerial[32];
		int nNameSize = strlen(pszFileName[i])-strlen(".hbk");
		memcpy(szSerial, pszFileName[i], nNameSize);
		szSerial[nNameSize] = NULL;
		bool bFileDelete;
		DWORD dwSerial = atoi(szSerial);

		if(_AvatorDBUpdateWithRecFile(dwSerial, &bFileDelete))
		{
			nSuccCount++;
			bFileDelete = true;
		}
		else
		{
			m_logSystemError.Write("%s : 복구실패", pszFileName[i]);
		}

		if(bFileDelete)
		{	
			char szFile[128];
			sprintf(szFile, ".\\HBK\\%s", pszFileName[i]);
			if(!DeleteFile(szFile))
			{
				DWORD dwErr = GetLastError();
				m_logSystemError.Write("*.hbk파일 복구후 제거 실패(%s).. CODE : %d", pszFileName[i], dwErr);
			}
		}

		Sleep(1);
		delete [] pszFileName[i];
	}

	m_logSystemError.Write("%d개 복구 파일중 %d개 파일 복구 성공", nFileNum, nSuccCount);
}

bool CMainThread::_AvatorDBUpdateWithRecFile(DWORD dwSerial, bool* OUT pbNextDelete)
{
	char szFile[128];
	sprintf(szFile, ".\\HBK\\%d.hbk", dwSerial);
	bool bComplete = false;
	*pbNextDelete = false;
	DWORD dwReadSize = 0;
	int nFileSize = ::CalcFileSize(szFile);
	_AVATOR_DATA	BKData;
	_AVATOR_DATA	DBData;

	if(nFileSize != sizeof(_AVATOR_DATA))
	{
		*pbNextDelete = true;
		return false;
	}
	
	HANDLE hFile = ::CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		m_logSystemError.Write("%d 아바타 데이터 복구 실패 (파일 열기 실패)", dwSerial);				
		return false;
	}

	::ReadFile(hFile, &BKData, sizeof(_AVATOR_DATA), &dwReadSize, NULL);
	::CloseHandle(hFile);

	if(sizeof(_AVATOR_DATA) != dwReadSize)
	{
		*pbNextDelete = true;
		return false;
	}

	//아바타 데이터 가져옴..
	BYTE byRet = db_Load_Avator(dwSerial, &DBData, true);
	if(byRet != RET_CODE_SUCCESS)
	{
		if(byRet == RET_CODE_NONEXIST_DB_RECORD)	//디비에 없는 아바타..
		{
			m_logSystemError.Write("%d 아바타 데이터 복구 실패 (DB에 없는 레코드 : CODE %d)", dwSerial, byRet);
			*pbNextDelete = true;
		}
		else
		{
			m_logSystemError.Write("%d 아바타 데이터 복구 실패 (DB 열기 실패 : CODE %d)", dwSerial, byRet);
		}
		return false;

	}

	//시리얼체크..
	if(dwSerial != BKData.dbAvator.m_dwRecordNum)
	{
		m_logSystemError.Write("%d 아바타 데이터 복구 실패 (시리얼이 다름 : SavedSerial : %d)", dwSerial, BKData.dbAvator.m_dwRecordNum);
		*pbNextDelete = true;	//저장하는데 실패.. 지운다
		return false;
	}


	//저장..
	BYTE byUptRet = db_Update_Avator(dwSerial, &BKData, &DBData, true);
	if(byUptRet != RET_CODE_SUCCESS)
	{
		m_logSystemError.Write("%d 아바타 데이터 복구 실패 (DB 저장 실패 : CODE %d)", dwSerial, byUptRet);
		return false;
	}

	return true;
}

void CMainThread::gm_DisplaymodeChange()
{
	if(!m_MapDisplay.m_bDisplayMode)
	{ 
		CMapData*	pMap;
		WORD		wLayerIndex;

		if(m_MapDisplay.m_pOldActMap)
		{
			pMap = m_MapDisplay.m_pOldActMap;
			wLayerIndex = m_MapDisplay.m_wOldLayerIndex;
		}
		else if(CGameObject::s_pSelectObject)
		{
			pMap = CGameObject::s_pSelectObject->m_pCurMap;
			wLayerIndex = CGameObject::s_pSelectObject->m_wMapLayerIndex;
		}
		else
		{
			pMap = m_MapOper.GetMap(0);
			wLayerIndex = 0;
		}

		if(!m_MapDisplay.OnDisplay(pMap, wLayerIndex))
		{
			::MyMessageBox("Display Error", "OnDisplay() Failure");			
			return;
		}
	}
	else
	{
		if(!m_MapDisplay.OffDisplay())
		{
			::MyMessageBox("Display Error", "OffDisplay() Failure");	
			return;
		}
		else
		{
			g_pDoc->m_DisplayView.ShowWindow(SW_HIDE);
			g_pDoc->m_DisplayView.ShowWindow(SW_SHOW);
		}
	}
}

void CMainThread::gm_MapChange(CMapData* pMap)
{
	m_MapDisplay.m_MapExtend.m_bExtendMode = false;
	g_pDoc->m_DisplayView.SetExtendMode(FALSE);
	m_MapDisplay.ChangeMap(pMap);
}

bool CMainThread::gm_MonsterInit()
{
/*	float fPos[3] = {0, 0, 0};
	__ITEM Item;
	::CreateGuardTower(&m_MapOper.m_Map[0], 0, fPos, &Item, &g_Player[0]);*/

	m_MapOper.m_bReSpawnMonster = !m_MapOper.m_bReSpawnMonster;

	if(!m_MapOper.m_bReSpawnMonster)
	{
		for(int i = 0; i < MAX_MONSTER; i++)
		{
			if(g_Monster[i].m_bLive)
				g_Monster[i].Destroy(mon_destroy_code_respawn, NULL);
		}
	}

	if(g_pDoc->m_InfoSheet.GetActiveIndex() == CInfoSheet::OBJECT_TAB)
		g_Main.m_GameMsg.PackingMsg(gm_interface_update_object);

	return m_MapOper.m_bReSpawnMonster;
}

void CMainThread::gm_DisplayAll()
{
	gm_DisplaymodeChange();
	if(m_MapDisplay.m_bDisplayMode)
	{
		((CGameServerView*)g_pDoc->m_pwndMainView)->OnButtonDummy();
		((CGameServerView*)g_pDoc->m_pwndMainView)->OnButtonCollline();
	}
}

void CMainThread::gm_PreCloseAnn()
{
	char szMessage[128] = "30초내 서버를 닫습니다. 5분후에 재접속해주세요.";
	
	for(int i= 0; i < MAX_PLAYER; i++)
	{
		if(g_Player[i].m_bLive)
			g_Player[i].SendData_ChatTrans(CHAT_TYPE_OPERATOR, 0xFFFFFFFF, strlen(szMessage), szMessage);
	}
}

void CMainThread::gm_ObjectSelect()
{
	CGameObject* pObj = NULL;
	
	pObj = m_MapDisplay.SelectObject(&g_pDoc->m_DisplayView.m_ptLBtn);//, m_MapDisplay.m_pActMap);

	if(pObj)
	{
		CGameObject::s_pSelectObject = pObj;
		if(g_pDoc->m_InfoSheet.GetActiveIndex() == CInfoSheet::OBJECT_TAB)
			g_Main.m_GameMsg.PackingMsg(gm_interface_update_object);
	}
//	else
//		CGameObject::s_pSelectObject = NULL;
}

void CMainThread::gm_UpdateServer()
{
	g_pDoc->m_InfoSheet.m_tabServer.UpdateServerTab();
}

void CMainThread::gm_UpdateObject()
{
	g_pDoc->m_InfoSheet.m_tabObject.UpdateTab();
}

void CMainThread::gm_UpdateMap()
{
	g_pDoc->m_InfoSheet.m_tabMap.UpdateTab();
}

void CMainThread::gm_ServerClose()
{
	if(m_bServerClosing)
		return;

	m_bServerClosing = true;

	m_tmForceUserExit.BeginTimer(100);
	m_nForceExitSocketIndexOffset = 0;
}

void CMainThread::gm_UserExit()
{
	CGameObject* pObj = CGameObject::s_pSelectObject;

	if(!pObj)
		return;

	if(pObj->m_ObjID.m_byKind == obj_kind_char)
	{
		if(pObj->m_ObjID.m_byID == obj_id_monster)
		{
			((CMonster*)pObj)->Destroy(mon_destroy_code_respawn, NULL);
			pObj = NULL;
			return;
		}
		else if(pObj->m_ObjID.m_byID == obj_id_player)
		{
			if(((CPlayer*)pObj)->m_bLive)
			{
				g_UserDB[pObj->m_ObjID.m_wIndex].ForceCloseCommand(false, 0);				
				pObj = NULL;
				return;
			}
		}
	}	
}

void CMainThread::AccountServerLogin()
{	//계정서버에 오픈요청
	_open_world_request_wrac Send;

	strcpy(Send.szWorldName, m_szWorldName);
	Send.dwWorldServerIP = ::GetIPAddress();

	BYTE byType[msg_header_num] = {system_msg, open_world_request_wrac};
	m_Network.m_pProcess[account_line]->LoadSendMsg(0, byType, (char*)&Send, Send.size());
}

void CMainThread::SerivceSelfStart()
{
	if(g_Main.m_bWorldOpen && !g_Main.m_bWorldService)
	{
		m_bWorldService = true;

		_start_world_request_wrac Send;

		Send.dwGateIP = ::GetIPAddress();
		Send.wGatePort = PORT_SERVER_MAP;

		BYTE byType[msg_header_num] = {system_msg, start_world_request_wrac};
		m_Network.m_pProcess[account_line]->LoadSendMsg(0, byType, (char*)&Send, Send.size());
	}
}

void CMainThread::SerivceSelfStop()
{
	if(g_Main.m_bWorldOpen && g_Main.m_bWorldService)
	{
		m_bWorldService = false;

		_stop_world_request_wrac Send;

		BYTE byType[msg_header_num] = {system_msg, stop_world_request_wrac};
		m_Network.m_pProcess[account_line]->LoadSendMsg(0, byType, (char*)&Send, Send.size());
	}
}

void CMainThread::SerivceForceSet(bool bService)
{
	m_bWorldService = bService;
}

void CMainThread::PingToAccount()
{
	if(m_bWorldOpen)
	{
		_world_account_ping_wrac Send;

		BYTE byType[msg_header_num] = {sync_msg, world_account_ping_wrac};
		m_Network.m_pProcess[account_line]->LoadSendMsg(0, byType, (char*)&Send, Send.size());
	}
}

void CMainThread::LogMonKillCount()
{
	if(!m_tmKillMon.CountingTimer())
		return;

	char szTime[128];
	_strtime(szTime);

	m_logKillMon.Write("%s", szTime);

	int nMonKindNum = m_tblMonster.GetRecordNum();
	for(int i = 0; i < nMonKindNum; i++)
	{
		if(m_pdwKillMonNum[i] > 0)
		{
			_monster_fld* p = (_monster_fld*)m_tblMonster.GetRecord(i);

			m_logKillMon.Write("%s	: %d", p->m_strName, m_pdwKillMonNum[i]);
		}
	}
}

bool CMainThread::check_loaded_data()
{
	//일반스킬 레코드수 체크..
	if(m_tblEffectData[effect_code_skill].GetRecordNum() > max_skill_num)
	{
		::MyMessageBox("데이터 오류", "m_tblEffectData[effect_code_skill].ReadRecordNum() > max_skill_num");
		return false;
	}

	//포스 레코드수 체크..
	if(m_tblEffectData[effect_code_force].GetRecordNum() > force_storage_num)
	{
		::MyMessageBox("데이터 오류", "m_tblEffectData[effect_code_force].ReadRecordNum() > force_storage_num");
		return false;
	}

	//몬스터 체크..
	for(int r = 0; r < m_tblMonster.GetRecordNum(); r++)
	{
		_monster_fld* p = (_monster_fld*)m_tblMonster.GetRecord(r);
		if(p->m_fLevel <= 0)
		{
			::MyMessageBox("데이터 오류", "m_tblMonster, %d Rec.. Level == %d", r, (int)p->m_fLevel);
			return false;
		}
	}

	//유닛프레임 체크..
	for(r = 0; r < m_tblUnitFrame.GetRecordNum(); r++)
	{
		_UnitFrame_fld* p = (_UnitFrame_fld*)m_tblUnitFrame.GetRecord(r);
		if((DWORD)p->m_nMoney >= MONEY_UNIT_NUM || p->m_nMoney < 0)
		{
			::MyMessageBox("데이터 오류", "m_tblUnitFrame, %d Rec.. MoneyCode == %d", r, p->m_nMoney);
			return false;
		}
	}

	//유닛파트 체크..
	for(int t = 0; t < UNIT_PART_NUM; t++)
	{
		for(r = 0; r < m_tblUnitPart[t].GetRecordNum(); r++)
		{
			_UnitPart_fld* p = (_UnitPart_fld*)m_tblUnitPart[t].GetRecord(r);
			if((DWORD)p->m_nMoney >= MONEY_UNIT_NUM || p->m_nMoney < 0)
			{
				::MyMessageBox("데이터 오류", "m_tblUnitPart[%d], %d Rec.. MoneyCode == %d", t, r, p->m_nMoney);
				return false;
			}
		}
	}

	//유닛총알 체크..
	for(r = 0; r < m_tblUnitBullet.GetRecordNum(); r++)
	{
		_UnitBullet_fld* p = (_UnitBullet_fld*)m_tblUnitBullet.GetRecord(r);
		if((DWORD)p->m_nMoney >= MONEY_UNIT_NUM || p->m_nMoney < 0)
		{
			::MyMessageBox("데이터 오류", "m_tblUnitBullet[%d], %d Rec.. MoneyCode == %d", t, r, p->m_nMoney);
			return false;
		}
	}

	return true;
}

int GetMaxResKind()
{
	return g_Main.m_tblItemData[tbl_code_res].GetRecordNum();
}




