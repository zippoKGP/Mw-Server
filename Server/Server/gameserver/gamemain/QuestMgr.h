#ifndef _QuestMgr_h_
#define _QuestMgr_h_

#include "CharacterDB.h"
#include "QuestEventFld.h"
#include "QuestFld.h"

class CPlayer;
typedef _QUEST_DB_BASE::_LIST	__QUEST;

//퀘스트이벤트코드
#define quest_event_code_dummy			0
#define quest_event_code_npc			1
#define quest_event_code_pk				2
#define quest_event_code_lv				3
#define quest_event_code_class			4
#define quest_event_code_grade			5
#define quest_event_code_item			6
#define	QUEST_EVENT_CODE_NUM			7

//퀘스트발생 조건코드
#define quest_condition_code_lv			0
#define quest_condition_code_grade		1
#define quest_condition_code_dalant		2
#define quest_condition_code_gold		3
#define quest_condition_code_party		4
#define quest_condition_code_guild		5
#define quest_condition_code_nation		6
#define quest_coddition_code_equip		7
#define quest_condition_code_have		8
#define quest_condition_code_mastery	9
#define quest_condition_code_dummy		10
#define quest_condition_code_race		11

//요구행위조건
#define request_action_code_dummy		0
#define request_action_code_dialog		1
#define request_action_code_pk			2
#define request_action_code_mk			3
#define request_action_code_loot		4
#define request_action_code_make		5
#define request_action_code_lv			6
#define request_action_code_mastery		7
#define request_action_code_skilllv		8
#define request_action_code_forcelv		9

struct _event_node_point
{
	_event_node*	m_pEvent;
	int				m_nEventIndex;		//이벤트 테이블 레코드 인덱스
	int				m_nEventNodeIndex;	//한 레코드상의	병렬필드인덱스

	_event_node_point()	{	init();	}
	void init()	{	memset(this, 0, sizeof(*this));	}
	void set(_event_node* pPoint, int nEventIndex, int nEventNodeIndex) 
	{
		m_pEvent = pPoint;
		m_nEventIndex = nEventIndex;
		m_nEventNodeIndex = nEventNodeIndex;
	}
	bool isset()
	{
		if(m_pEvent)return true;
		return false;
	}
};

struct _quest_check_result
{
	struct _node
	{
		BYTE byQuestDBSlot;
		BYTE byActIndex;
		WORD wCount;
	};

	BYTE	m_byCheckNum;
	_node	m_List[quest_storage_num];

	void init()	{	m_byCheckNum = 0;	}
};

struct _quest_fail_result
{
	struct _node
	{
		BYTE byQuestDBSlot;
	};
	
	BYTE	m_byCheckNum;
	_node	m_List[quest_storage_num];

	void init()	{	m_byCheckNum = 0;	}
};

class CQuestMgr
{
public:

	CPlayer*			m_pMaster;
	_QUEST_DB_BASE*		m_pQuestData;
	DWORD				m_dwOldTime;
	_event_node_point	m_LastSelectEvent;
	BYTE				m_byCurQuestNum[QUEST_TYPE_NUM];

	static CRecordData*	s_tblQuest;
	static CRecordData*	s_tblQuestEvent;

	static _quest_check_result s_QuestCKRet;
	static _quest_fail_result  s_QuestFTRet;

public:

	CQuestMgr();
	~CQuestMgr();

	void InitMgr(CPlayer* pMaster, _QUEST_DB_BASE* pQuestData);
	bool AddQuestType(int nType);
	bool DelQuestType(int nType);

	//플레이중 미리 정의한 퀘스트발생타입의 행위를 했을때..
	_event_node_point* CheckCreateEvent(int nEventType, char* pszEventCode);

	bool _CheckCondition(_event_condition_node* pCond);

	bool __CheckCond_LV(BOOL bOver, int nLv);
	bool __CheckCond_Grade(BOOL bOver, int nGrade);
	bool __CheckCond_Dalant(BOOL bOver, int nDalant);
	bool __CheckCond_Gold(BOOL bOver, int nGold);
	bool __CheckCond_Party(BOOL belong);
	bool __CheckCond_Guild(BOOL belong);
	bool __CheckCond_Nation(BOOL belong);
	bool __CheckCond_Equip(char* pszItemCode);
	bool __CheckCond_Have(int nAmonut, char* pszItemCode);
	bool __CheckCond_Mastery(int nMasteryCode, int nMasteryIndex, int nMastery);
	bool __CheckCond_Dummy(BOOL bInclude, char* pszDummyCode);
	bool __CheckCond_Race(char* pszRaceSexFlag);

	//플레이중 미리 정의한 퀘스트수행타입의 행위를 했을때..
	_quest_check_result* CheckReqAct(int nActCode, char* pszReqCode);//, BYTE* OUT pbyActIndex, WORD* OUT pwCount, BYTE* OUT pbyQuestDBSlot);

	//발생한 퀘스트 이벤트에대해 퀘스트를 선택함
	_Quest_fld* GetQuestFromEvent(BYTE bySelect = 0xFF);

	//제한레벨체크
	_quest_fail_result* CheckLimLv(int nNewLv);

	_event_node_point* GetLastEvent() {	if(m_LastSelectEvent.isset())return &m_LastSelectEvent;	return NULL;	}

};

#endif