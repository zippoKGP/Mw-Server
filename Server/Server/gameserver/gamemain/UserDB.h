#ifndef _User_DB_h_
#define _User_DB_h_

#include "CharacterDB.h"
#include "MyUtil.h"
#include "MyClassUtil.h"

struct _SYNC_STATE
{
	bool			bEnter;
	bool			bReged;
	bool			bSelect;

	_SYNC_STATE()		{	Init();	}
	void Init()			{	memset(this, false, sizeof(*this));	}

	bool chk_enter()	{	if(bEnter)	return false; bEnter = true; return true;	}
	bool chk_reged()	{	if(bReged)	return false; bReged = true; return true;	}
	bool chk_select()	{	if(bSelect)	return false; bSelect = true; return true;	}

	void re_lobby()		{	bReged = bSelect = false; 	}
};

struct _MOVE_LOBBY_DELAY : public _DELAY_PROCESS
{
	_MOVE_LOBBY_DELAY() : _DELAY_PROCESS(MAX_PLAYER, 10000) {}

	virtual void Process(DWORD dwIndex, DWORD dwSerial);
};

struct _MESSENGER_KEY_DELAY : public _DELAY_PROCESS
{
	_MESSENGER_KEY_DELAY() : _DELAY_PROCESS(MAX_PLAYER, 1000) {}	//1초후에 보냄

	virtual void Process(DWORD dwIndex, DWORD dwSerial);
};

struct _MESSENGER_ENTER_CHECK : public _DELAY_PROCESS	//메신져서버에 정해진시간안에 들어왔는지 체크
{
	_MESSENGER_ENTER_CHECK() : _DELAY_PROCESS(MAX_PLAYER, 10000) {}	//10초안에 들어와야함..

	virtual void Process(DWORD dwIndex, DWORD dwSerial);
};

struct _EXIT_ALTER_PARAM				//아바타가 게임종료햇을때 마지막 세이브할 인자를 CAvatorDB에 넣어주는 파라메터 구조체
{
	DWORD dwHP;			
	DWORD dwFP;			
	DWORD dwSP;	
	double dExp;		
	BYTE byMapCode;		
	float fStartPos[3];	

	_BELT_DB_BASE::_LINK_LIST	Link[sf_linker_num];	//링크항목
};

class CUserDB
{
public:

	_GLBID				m_gidGlobal;	//전체계정..계정시리얼이다..
	_CLID				m_idWorld;		//월드. serial은 월드에 접속한 순서..
	_CLID				m_idMessenger;	//메신져서버에서의 id..

	DWORD				m_dwIP;
	DWORD				m_dwTotalPlayMin;

	char				m_szAccountID[max_id_len+1];
	DWORD				m_dwAccountSerial;
	DWORD				m_ipAddress;
	BYTE				m_byUserDgr;	//유저등급
	BYTE				m_bySubDgr;		//서브등급

	char				m_szAvatorName[max_name_len+1];
	DWORD				m_dwSerial;
	
	_REGED				m_RegedList[MAX_CHAR_PER_WORLDUSER];
	_AVATOR_DATA		m_AvatorData;
	_AVATOR_DATA		m_AvatorData_bk;

	bool				m_bActive;
	bool				m_bField;
	bool				m_bWndFullMode;
	bool				m_bDBWaitState;		//data base에 접근중인지.. therad..mode.
	bool				m_bChatLock;
	_SYNC_STATE			m_ss;
	DWORD				m_dwMessengerKey[CHECK_KEY_NUM];	//메신져서버 입장키

	CMyTimer			m_tmrCheckPlayMin;	//1분마다..
	CMyTimer			m_tmrCheckVariableUpdate;	//10초마다..

	//BK..
	bool				m_bFileUpdate;
	bool				m_bVariableUpdate;
	char				m_szFileName[64];
	HANDLE				m_hBKFile;
	HANDLE				m_hBKMap;
	_AVATOR_DATA*		m_pDataMapping;

	DWORD				m_dwReLobbyTime;
	bool				m_bNoneUpdateData;

	static _MOVE_LOBBY_DELAY		s_MoveLobbyDelay;
	static _MESSENGER_KEY_DELAY		s_MessengerKeyDelay;
	static _MESSENGER_ENTER_CHECK	s_MessengerEnterCheck;

	static int					s_nLoginNum;

public:

	CUserDB();
	~CUserDB(){};

	void Init(DWORD dwIndex);
	void ParamInit();
	void DummyCreate(DWORD dwSerial);
	void SetWorldCLID(DWORD dwSerial, IP_ADDR* pipAddr);	//소켓시리얼을 얻어옴..
	void StartFieldMode();
	void ForceCloseCommand(bool bPushClose, DWORD dwPushIP);
	void DirectClose();
	void SetChatLock(bool bLock);
	void ChatLockCheck();
	void GenerateMessengerKey();
	void EnterMessenger(_CLID* pidWorld, _CLID* pidLocal);
	void ExitMessenger(_CLID* pidWorld);

	bool Enter_Account(DWORD dwAccountSerial, DWORD dwIP, bool bFullMode);

	void Exit_Account_Request();
	void Exit_Account_Complete(BYTE byRetCode);

	bool Reged_Char_Request();
	void Reged_Char_Complete(BYTE byRetCode, _REGED* pRegedList);

	bool Insert_Char_Request(char* pszCharName, BYTE bySlotIndex, BYTE byRaceSexCode, char* pszClassCode, DWORD dwBaseShape);
	void Insert_Char_Complete(BYTE byRetCode, _REGED_AVATOR_DB* pInsertData);

	bool Delete_Char_Request(BYTE bySlotIndex);
	void Delete_Char_Complete(BYTE byRetCode, BYTE bySlotIndex);

	bool Select_Char_Request(BYTE bySlotIndex);
	void Select_Char_Complete(BYTE byRetCode, _AVATOR_DATA* pLoadData, char* pszHBKFileName);

	bool Lobby_Char_Request();
	void Lobby_Char_Complete(BYTE byRetCode);

	void TotalPlayMinCheck();
	void VariableUpdateCheck();
	void WriteLog_Level(BYTE byLv);
//	void WriteLog_Mastery(BYTE byMasteryCode, BYTE byCodeIndex, BYTE byMastery);

	bool StartBKFileMapping();
	bool EndBKFileMapping();
	bool UpdateBKFile(bool bDirect = false);
	
	//AVATOR DATA UPDATE..
	bool Update_ItemAdd(BYTE storage, BYTE slot, BYTE table, WORD index, DWORD amount, DWORD upg);
	bool Update_ItemDelete(BYTE storage, BYTE slot);
	bool Update_ItemDur(BYTE storage, BYTE slot, DWORD amount, bool bUpdate = true);
	bool Update_ItemUpgrade(BYTE storage, BYTE slot, DWORD upg, bool bUpdate = true);	
	bool Update_UnitInsert(BYTE bySlotIndex, _UNIT_DB_BASE::_LIST* pSlotData);
	bool Update_UnitDelete(BYTE bySlotIndex);
	bool Update_UnitData(BYTE bySlotIndex, _UNIT_DB_BASE::_LIST* pData);
	bool Update_QuestInsert(BYTE bySlotIndex, _QUEST_DB_BASE::_LIST* pSlotData);
	bool Update_QuestDelete(BYTE bySlotIndex);
	bool Update_QuestUpdate(BYTE bySlotIndex, _QUEST_DB_BASE::_LIST* pSlotData);
	bool Update_ItemSlot(BYTE storage, BYTE slot, BYTE clientpos);
	bool Update_Level(BYTE lv, double exp);
	bool Update_Exp(double exp);
	bool Update_Class(char* pszClassCode, BYTE byHistoryRecordNum, WORD wHistoryClassIndex);
	bool Update_Money(DWORD dalant, DWORD gold);
	bool Update_Map(BYTE map, float* pos, bool bUpdate = true);
	bool Update_BagNum(BYTE bagnum);
	bool Update_CuttingPush(BYTE resnum, _CUTTING_DB_BASE::_LIST* plist);
	bool Update_CuttingTrans(BYTE byResItemIndex, BYTE byLeftAmt);
	bool Update_CuttingEmpty();
	bool Update_Stat(BYTE byStatIndex, DWORD dwNewCum, bool bUpdate = true);
	bool Update_PlayTime(DWORD dwTotalTimeMin);
	bool Update_Param(_EXIT_ALTER_PARAM* pCon);

	bool Update_CopyAll(_AVATOR_DATA* pSrc);

	static void ReRangeClientIndex(_AVATOR_DATA* pData);
	static void ItemValidCheckRevise(_AVATOR_DATA* pData);
	static bool SetBKFile(char* pszHBKFileName, _AVATOR_DATA* pData);
	static void ReleaseBKFile(char* pszHBKFileName);

private:

	bool FirstSettingData();
};

CUserDB* SearchAvatorWithName(CUserDB* pList, int nMax, char* pName);

#endif
