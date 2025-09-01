// MainThread.h: interface for the CMainThread class.
//
//////////////////////////////////////////////////////////////////////

#ifndef	_MainThread_h_
#define _MainThread_h_

#include "Player.h"
#include "PartyPlayer.h"
#include "UserDB.h"
#include "Monster.h"
#include "Animus.h"
#include "GuardTower.h"
#include "BattleDungeon.h"
#include "MapOperate.h"
#include "MapDisplay.h"
#include "MsgProcess.h"
#include "MyNetWorking.h"
#include "ItemBox.h"
#include "NPC.h"
#include "ParkingUnit.h"
#include "RFWorldDatabase.h"
#include "DBQrySyn.h"
#include "EconomySystem.h"
#include "ItemLootTable.h"
#include "WeaponBulletLinkTable.h"
#include "OreCuttingTable.h"
#include "MapDataTable.h"
#include "ItemMakeDataTable.h"
#include "ItemUpgradeTable.h"
#include "ConnNumPHMgr.h"

#define MAX_WAIT_NETMOVE_FROM_WORLD		1280	//월드서버에서 계정->월드로 이동중일대 최대 대기가능한 수

struct _WAIT_ENTER_ACCOUNT
{
	bool	m_bLoad;
	DWORD   m_dwAccountSerial;
	char	m_szAccountID[max_id_len+1];
	BYTE	m_byUserDgr;
	BYTE	m_bySubDgr;
	_GLBID	m_gidGlobal;
	DWORD	m_dwKey[CHECK_KEY_NUM];
	DWORD	m_dwLoadTime;

	_WAIT_ENTER_ACCOUNT()
	{
		m_bLoad = false;
		m_byUserDgr = USER_DEGREE_STD;
	}

	void SetData(DWORD dwAccountSerial, char* pszAccountID, BYTE byUserDgr, BYTE bySubDgr, _GLBID* pgidGlobal, DWORD* pdwKey)
	{
		m_bLoad = true;
		m_dwAccountSerial = dwAccountSerial;
		strcpy(m_szAccountID, pszAccountID);
		m_byUserDgr = byUserDgr;
		m_bySubDgr = bySubDgr;
		memcpy(&m_gidGlobal, pgidGlobal, sizeof(_GLBID));
		memcpy(m_dwKey, pdwKey, sizeof(m_dwKey));
		m_dwLoadTime = timeGetTime();	
	}
	
	void Release()
	{
		m_bLoad = false;	
	}
};

struct _LOCK_CHAT_DATA
{
	bool			bLock;
	DWORD			dwAccountSerial;
	DWORD			dwStartTime;
	DWORD			dwDurTime;
	char			szName[max_name_len+1];
	char			szID[max_id_len+1];

	_LOCK_CHAT_DATA()	{	Init();	}
	void Init()			{	bLock = false;	}
};	

extern CUserDB				g_UserDB[MAX_PLAYER];
extern CPartyPlayer			g_PartyPlayer[MAX_PLAYER];

extern CPlayer				g_Player[MAX_PLAYER];
extern CMonster				g_Monster[MAX_MONSTER];
extern CNPC					g_NPC[MAX_NPC];
extern CAnimus				g_Animus[MAX_ANIMUS];
extern CGuardTower			g_Tower[MAX_TOWER];
extern CItemBox				g_ItemBox[MAX_ITEMBOX];
extern CBattleDungeon		g_BattleDungeon[MAX_BATTLEDUNGEON];
extern CParkingUnit			g_ParkingUnit[MAX_PARKINGUNIT];

#define SEND_USER(_w, _x, _y, _z)		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(_w, _x, _y, _z);
#define SEND_ACCOUNT(_w, _x, _y, _z)	g_Main.m_Network.m_pProcess[account_line]->LoadSendMsg(_w, _x, _y, _z);

class CMainThread  
{
public:

	enum			{		max_lock_chat	=	1000	};
	
	_SRAND					m_Rand;

	_WAIT_ENTER_ACCOUNT		m_WaitEnterAccount[MAX_WAIT_NETMOVE_FROM_WORLD];
	
	CRFWorldDatabase*		m_pWorldDB;
	
	CMapDisplay				m_MapDisplay;
	CMapOperation			m_MapOper;
	CFrameRate				m_MainFrameRate;
	CFrameRate				m_DBFrameRate;
	CNetworkEX				m_Network;
	CMsgProcess				m_GameMsg;
	CConnNumPHMgr			m_MgrConnNum;
	CMyTimer				m_tmEconomyState;
			
	_DB_QRY_SYN_DATA		m_DBQrySynData[max_db_syn_data_num];
	CNetIndexList			m_listDQSData;
	CNetIndexList			m_listDQSDataComplete;
	CNetIndexList			m_listDQSDataEmpty;	

	_LOCK_CHAT_DATA			m_LockChatData[max_lock_chat];
	CMyTimer				m_tmLockChatDataCheck;

	//Service..
	char					m_szWorldName[max_world_name_len+1];
	BYTE					m_byWorldCode;
	bool					m_bWorldOpen;	//계정서버로부터 인증을 마친 당장 서비스가능한 상태..
	bool					m_bWorldService;
	bool					m_bUsingMsgr;	//메신져서버를 사용할것인가..
	bool					m_bMsgrOpen;	//메신져서버 오픈..
	DWORD					m_dwMessengerIP;	//메신져서버 주소,,
	DWORD					m_dwAccountIP;	
	DWORD					m_dwCheckAccountOldTick;
	CMyTimer				m_tmrCheckAvator;	//1초마다..
	CMyTimer				m_tmrAccountPing;	//ACCOUNT_WORLD_PING_TERM

	CMapDataTable			m_tblMapData;

	CRecordData				m_tblPlayer;
	CRecordData				m_tblMonster;
	CRecordData				m_tblNPC;
	CRecordData				m_tblAnimus;

	CRecordData				m_tblClass;
	CRecordData				m_tblExp;

	//ReProcess Table
	CWeaponBulletLinkTable  m_tblWeaponBulletLink;
	CItemLootTable			m_tblItemLoot;
	COreCuttingTable		m_tblOreCutting;
	CItemMakeDataTable		m_tblItemMakeData;
	CItemUpgradeTable		m_tblItemUpgrade;

	CRecordData				m_tblItemData[item_tbl_num];
	CRecordData				m_tblEffectData[EFFECT_CODE_NUM];

	//unit data
	CRecordData				m_tblUnitPart[UNIT_PART_NUM];
	CRecordData				m_tblUnitBullet;
	CRecordData				m_tblUnitFrame;

	bool					m_bRuleThread;
	bool					m_bDQSThread;

	CLogFile				m_logSystemError;
	CLogFile				m_logLoadingError;
	CLogFile				m_logDungeon;
	CLogFile				m_logKillMon;
	CLogFile				m_logCheat;

	DWORD*					m_pdwKillMonNum;
	CMyTimer				m_tmKillMon;

	//서버닫기시 접속자를 순차적으로 강제종료매시지를 보내는 변수
	CMyTimer				m_tmForceUserExit;
	int						m_nForceExitSocketIndexOffset;
	bool					m_bServerClosing;

public:

	CMainThread();
	virtual ~CMainThread();
	bool Init();

	CGameObject* GetObject(_object_id* pObjID);
	CGameObject* GetObject(int kind, int id, int index);	
	CGameObject* GetChar(char* szCharName);

	WORD GetPlayerIndexFromSerial(DWORD dwSerial);
	bool IsChatLockAccount(DWORD dwAccountSerial);	
	bool AddChatLockAccount(CUserDB* pUser, DWORD dwTime);
	bool DelChatLockAccount(char* pszAvatorName);

	static void RuleThread(void* pv);
	static void DQSThread(void* pv);

	void OnRun();

	void pc_OpenWorldSuccessResult(BYTE byWorldCode, bool bControlOpen, DWORD dwControlIP, WORD wControlPort, DWORD* pdwControlMasterKey, char* pszDBName);//, DWORD dwDBIP, char* pszDBAccount, char* pszDBPassword);
	void pc_TransIPKeyInform(DWORD dwAccountSerial, char* pszAccountID, BYTE byUserDgr, BYTE bySubDgr, DWORD* pdwKey, _GLBID* pgidGlobal, DWORD dwClientIP);
	void pc_ForceCloseCommand(_CLID* pidWorld, bool bDirectly, bool bPushClose, DWORD dwPushIP);
	void pc_AllUserKickInform();
	void pc_AllUserMsgInform(BYTE byMsgSize, char* szMsg);
	void pc_AlterWorldService(bool bSerivce);
	
	void pc_OpenMsgrRequest(char* pszWorldName, DWORD dwLocalIP);
	void pc_EnterUserMessengerReport(_CLID* pidWorld, _CLID* pidLocal);
	void pc_ExitUserMessengerReport(_CLID* pidWorld);

	void AccountServerLogin();
	void SerivceSelfStart();
	void SerivceSelfStop();
	void SerivceForceSet(bool bService);

	bool PushDQSData(DWORD dwAccountSerial, _CLID* pidWorld, BYTE byQryCase, char* pQryData, int nSize);

	void Reged_Avator_Complete(_DB_QRY_SYN_DATA* pData);
	void Insert_Avator_Complete(_DB_QRY_SYN_DATA* pData);
	void Delete_Avator_Complete(_DB_QRY_SYN_DATA* pData);
	void Select_Avator_Complete(_DB_QRY_SYN_DATA* pData);
	void Logout_Account_Complete(_DB_QRY_SYN_DATA* pData);
	void Lobby_Account_Complete(_DB_QRY_SYN_DATA* pData);

	void ForceCloseUserInTiming();

	void gm_DisplaymodeChange();
	void gm_MapChange(CMapData* pMap);
	bool gm_MonsterInit();
	void gm_ObjectSelect();
	void gm_UpdateServer();
	void gm_UpdateObject();
	void gm_UpdateMap();
	void gm_DisplayAll();
	void gm_PreCloseAnn();
	void gm_ServerClose();
	void gm_UserExit();

	BYTE db_Reged_Avator(DWORD dwAccountSerial, _REGED* OUT pRegedList);
	BYTE db_Insert_Avator(DWORD dwAccountSerial, char* pszAccount, _REGED_AVATOR_DB* pCharDB, DWORD* OUT pdwAvatorSerial);
	BYTE db_Delete_Avator(DWORD dwSerial, BYTE byRaceCode);
	BYTE db_Max_AvatorSerial(DWORD* OUT pdwAvatorSerial);

	BYTE db_Load_Avator(DWORD dwSerial, _AVATOR_DATA* OUT pData, bool bAll);
	BYTE db_Update_Avator(DWORD dwSerial, _AVATOR_DATA* pNewData, _AVATOR_DATA* pOldData, bool bCheckLowHigh);

	BYTE db_Select_Economy_History(_economy_history_data* OUT pCurData, int* OUT pnCurMgrValue, int* OUT pnNextMgrValue, _economy_history_data* OUT pHisData, int* OUT pHistoryNum, DWORD dwDate);
	BYTE db_Insert_Economy_History(DWORD dwDate, double* pdTradeDalant, double* pdTradeGold, int nMgrValue);
	BYTE db_Log_AvatorLevel(DWORD dwTotalPlayMin, DWORD dwSerial, BYTE byLv);
	BYTE db_Log_UserNum(int nAveragePerHour, int nMaxPerHour);

private:

	BYTE _db_Load_Base(DWORD dwSerial, _AVATOR_DATA* OUT pCon);
	BYTE _db_Load_General(DWORD dwSerial, BYTE byRaceCode, _AVATOR_DATA* OUT pCon);
	BYTE _db_Load_Inven(DWORD dwSerial, int nBagNum, _INVEN_DB_BASE* OUT pCon);
	BYTE _db_Load_Unit(DWORD dwSerial, _UNIT_DB_BASE* OUT pCon);

	bool _db_Update_Base(DWORD dwSerial, _AVATOR_DATA* pNewData, _AVATOR_DATA* pOldData, char* OUT pSzQuery, bool bCheckLowHigh);
	bool _db_Update_General(DWORD dwSerial, _AVATOR_DATA* pNewData, _AVATOR_DATA* pOldData, char* OUT pSzQuery, bool bCheckLowHigh);
	bool _db_Update_Inven(DWORD dwSerial, _AVATOR_DATA* pNewData, _AVATOR_DATA* pOldData, char* OUT pSzQuery);
	bool _db_Update_Unit(DWORD dwSerial, _AVATOR_DATA* pNewData, _AVATOR_DATA* pOldData, char* OUT pSzQuery);

	bool DataFileInit();
	void NetworkInit();
	bool ObjectInit();
	bool DatabaseInit(char* pszDBName);

	void CheckAccountLineState();
	void CheckAvatorState();
	void CheckLockChatData();
	void CheckConnNumLog();
	void DQSCompleteProcess();

	void AllAvatorDBRestore();
	bool _AvatorDBUpdateWithRecFile(DWORD dwSerial, bool* OUT pbNextDelete);

	void PingToAccount();
	void LogMonKillCount();

	bool check_loaded_data();
};

extern int GetMaxResKind();
extern CMainThread g_Main;

#endif // !defined(AFX_MAINTHREAD_H__CD50286D_6690_4B9F_8622_BB5426275AB2__INCLUDED_)
