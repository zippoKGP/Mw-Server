// Player.h: interface for the CPlayer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef	_Player_h_
#define _Player_h_

#include "Character.h"
#include "PlayerDB.h"
#include "NetUtil.h"
#include "ItemBox.h"
#include "ItemStore.h"
#include "CharacterDB.h"
#include "Attack.h"
#include "PartyPlayer.h"
#include "PlayerCharacterFld.h"
#include "PlayerStruct.h"
#include "MgrAvatorItemHistory.h"
#include "MgrAvatorLvHistory.h"
#include "QuestMgr.h"

struct _MEM_PAST_WHISPER
{
	bool			bMemory;
	char			szName[max_name_len+1];
	CPlayer*		pDst;
	DWORD			dwMemoryTime;
};

class CUserDB;class CBattleDungeon;class CAnimus;class CParkingUnit;struct _BulletItem_fld;struct _UnitPart_fld;struct _UnitBullet_fld;

struct _ANIMUS_RETURN_DELAY : public _DELAY_PROCESS
{
	_ANIMUS_RETURN_DELAY() : _DELAY_PROCESS(MAX_PLAYER, 5000) {}

	virtual void Process(DWORD dwIndex, DWORD dwSerial);
};

#define download_total_count 2

class CPlayer: public CCharacter
{
public:

	enum			{	timer_alter_state			= 1000	};
	enum			{	move_potal_dist				= 100	};
	enum			{   max_memory_past_whisper		= 30	};
	enum			{	item_upgrade_up = 0, item_upgrade_down, item_upgrade_init };
	enum			{	effect_none = 0, effect_apply, effect_wait	};
	enum			{	extra_stoppos_gap			= 20	};	
	enum			{	max_history_file_size		= 64	};

	bool				m_bLoad;		//월드로부터 데이터를 로드받은상태인가.. m_bLive보다 전단계
	bool				m_bOper;		//위치에 create() 됀다음부터 pc_ExitSaveDataRequest()까지..
	bool				m_bMoveOut;		//존이동신청을 햇나..
	bool				m_bFullMode;	//클라이언트의 윈도우모드..
	BYTE				m_byUserDgr;	//유저등급
	BYTE				m_bySubDgr;		//서브등급
	
	bool				m_bBaseDownload, m_bInvenDownload, m_bForceDownload, m_bCumDownload, m_bSpecialDownload;
	
	CPartyPlayer*		m_pPartyMgr;
	CPlayerDB			m_Param;	//Player DB 항목에 쓰이는 parameter들
	_CLID				m_id;
	
	int					m_nMoveType, m_nModeType;	//걷기 or 뛰기//공격 or 평화모드	
	bool				m_bMineMode;	//채굴모드
	bool				m_bPosMainBase;	//마을에 있나..
	_WEAPON_PARAM		m_pmWpn;
	_DTRADE_PARAM		m_pmTrd;
	_MASTERY_PARAM		m_pmMst;
	_GROGGY_PARAM		m_pmGrg;
	_TOWER_PARAM		m_pmTwr;
	CQuestMgr			m_QuestMgr;

	static int			s_nLiveNum;
	static int			s_nRaceNum[RACE_NUM];
	static DWORD		s_dwItemUpgSucRate[MAX_ITEM_LV];
	static float		s_fExpDivUnderParty_Dam[member_per_party];
	static float		s_fExpDivUnderParty_Kill[member_per_party];
	static float		s_fWPGapAdjust[wp_type_num];
	static double		s_dExpLimPerLv[max_level];	
	static int*			s_pnLinkForceItemToEffect;
	static _ANIMUS_RETURN_DELAY s_AnimusReturnDelay;

private:

	CUserDB*			m_pUserDB;
	WORD				m_wVisualVer;	//자신이 
	int					m_nLastBeatenPart;
	DWORD				m_dwLastState;
	WORD				m_wExpRate;

	int					m_nMaxHP, m_nMaxFP, m_nMaxSP;
	DWORD				m_dwTolValue[TOL_CODE_NUM];
	DWORD				m_dwMineStartTime, m_dwMineDelayTime;
	BYTE				m_byDelayErrorCount;

	//유닛..
	DWORD					m_dwLastTimeCheckUnitViewOver;
	DWORD					m_dwUnitViewOverTime;
	_UNIT_DB_BASE::_LIST*	m_pUsingUnit;
	CParkingUnit*			m_pParkingUnit;
	BYTE					m_byUsingWeaponPart;

	//소환..
	__ITEM*				m_pRecalledAnimusItem;	//소환한 애니머스의 아이템포인터
	CAnimus*			m_pRecalledAnimusChar;	//소환한 애니머스의 케릭터포인터	
	DWORD				m_dwLastRecallTime;		//최근에 소환을 귀한시킨시간
	bool				m_bNextRecallReturn;	//소환리턴 예약

	CMyTimer			m_tmrAlterState;	//타이머
	CMyTimer			m_tmrIntervalSec;	//1초당.

	//랜덤던젼으로 이동시 이전맵과 좌표 백업변수..
	float				m_fBeforeDungeonPos[3];
	CMapData*			m_pBeforeDungeonMap;

	//Far whisper했던 목록..
	_MEM_PAST_WHISPER	m_PastWhiper[max_memory_past_whisper];

	//count attack..
	CCharacter*			m_pCountAttackDst;

	//아드레날린 포션
	DWORD				m_dwStartTime;
	DWORD				m_dwDurTime;
	float				m_fAFRate;
	float				m_fDefRate;

	//효과장착상태
	bool				m_bUpCheckEquipEffect;			// 레벨, 숙련도등이 올랐을때
	bool				m_bDownCheckEquipEffect;		// 레벨, 숙련도등이 떨어졌을때
	BYTE				m_byEffectEquipCode[equip_fix_num+embellish_fix_num];
	DWORD*				m_pdwLastForceActTime;	//포스시전별 최근 시전시간 
	DWORD*				m_pdwLastSkillActTime;	//스킬시전별 최근 시전시간 

	char				m_szItemHistoryFileName[max_history_file_size];
	char				m_szLvHistoryFileName[max_history_file_size];
	DWORD				m_dwUMWHLastTime;

	//치트관련변수.
	bool				m_bCheat_100SuccMake;	//제작, 업그레이드 100%성공
	bool				m_bCheat_Matchless;		//무적,, 한방.. 안죽음..
	bool				m_bFreeRecallWaitTime;	//애니머스 소환대기시간 해제

	static CMgrAvatorItemHistory	s_MgrItemHistory;
	static CMgrAvatorLvHistory		s_MgrLvHistory;
	
public:

	CPlayer();
	virtual ~CPlayer();

	//DATA SET..
	bool	Init(_object_id* pID);				//정적으로 사용할려면 미리 호출하는것..
	bool	Load(CUserDB* pUser);
	bool	Create();							//게임을 정식으로 시작함
	void	CreateComplete();					//게임시작처리가 모두 끝난후..
	void	NetClose();							//접속종료
	void    ExitUpdateDataToWorld();
	void	PastWhisperInit();

	//INIT EVENT..
	void	DTradeInit();
	bool	Corpse(CCharacter* pAtter);			//게임중 시체가됨
	void	Resurrect();		//부활	
	void    CheckUnitCutTime();

	//ITEM EVENT
	void	Emb_AddStorage(BYTE byStorageCode, _STORAGE_LIST::_storage_con* pCon);
	void	Emb_DelStorage(BYTE byStorageCode, BYTE byStorageIndex, bool bEquipChange);
	DWORD	Emb_AlterDurPoint(BYTE byStorageCode, BYTE byStorageIndex, int nAlter, bool bUpdate, bool bSend = false); 
	void	Emb_ItemUpgrade(BYTE byUpgradeType, BYTE byStorageCode, BYTE byStorageIndex, DWORD dwGradeInfo); 
	void	Emb_UpdateStat(DWORD dwStatIndex, DWORD dwNewData);
	void	Emb_AlterStat(BYTE byMasteryClass, BYTE byIndex, DWORD dwAlter);
	void	Emb_RidindUnit(bool bRiding, CParkingUnit* pCreateUnit);
	
	//QUEST EVENT
	void	Emb_CreateQuestEvent(int nEventType, char* pszEventCode);
	void	Emb_SelectQuestEvent(BYTE bySelectIndex);	//이벤트발생후 첫 퀘스트 선택
	void	Emb_CheckActForQuest(int nActCode, char* pszReqCode);
	void	Emb_CheckLvUpForQuest();
	void	Emb_CheckTimeoutForQuest();

	//TIMER EVENT..
	void	SenseState();
	void	AutoRecover();
	void	AutoRecover_Animus();	//애니머스 자동 회복
	void	AutoCharge_Booster();	//유닛 부스터 충전
	void	UpdatedMasteryWriteHistory();
	void	_CheckForcePullUnit();

	//MAIN..
	void	NewViewCircleObject();
	void	SetLevel(BYTE byNewLevel);
	void	AlterMoney(int nAlterGold, int nAlterDalant);
	bool	SetTarPos(float* fTarPos, bool bColl = true);
	void	SetEquipEffect(_STORAGE_LIST::_storage_con* pItem, bool bEquip);
	void	SetHaveEffect();//소지효과있는 보석을 인벤내용변경 이벤트발생마다 모두검색
	DWORD	SetStateFlag(bool bUpdate);
	bool	OutOfMap(CMapData* pIntoMap, WORD wLayerIndex, BYTE byMapOutType, float* pfStartPos);		//위치이동..일단..현재맵에서 뺀다
	bool	IntoMap(BYTE byMapInMode);		//새로운 위치세팅후 맵으로 집어넣는다.
	void	UpdateVisualVer();		//m_wVisualVer을 증가시킴
	
	//PARAMETER..
	virtual int		GetMaxHP( void )	{ return m_nMaxHP; }
	int				GetMaxSP( void )	{ return m_nMaxSP; }
	virtual int		GetHP()				{ return m_Param.GetHP(); }
	virtual void	SetHP(int nHP, bool bOver);
	int				GetFP()				{ return m_Param.GetFP(); }
	void			SetFP(int nFP, bool bOver);
	int				GetSP()				{ return m_Param.GetSP(); }
	void			SetSP(int nSP, bool bOver);

	//PROCESS EVENT..
	DWORD	GetMoney(BYTE byMoneyCode);
	bool	ConsumEquipItem(int nTableCode, int nDam, __ITEM* poutItem = NULL);
	void	AlterExp(float fAlterExp);
	WORD	_DeleteUnitKey(BYTE bySlotIndex);
	void	SetEffectEquipCode(BYTE byStorageCode, BYTE bySlotIndex, BYTE byCode);
	BYTE	GetEffectEquipCode(BYTE byStorageCode, BYTE bySlotIndex);
	int		GetVisualVer();			//m_wVisualVer을 리턴
	DWORD	GetStateFlag() {	return m_dwLastState; }	
	DWORD   GetLastStateFlag();
	float	GetAdrenAF();
	float	GetAdrenDef();
	void	AlterHP_Animus(int nNewHP);		//HP가 바뀌면 CALL		//애니머스..
	void	AlterFP_Animus(int nNewFP);		//FP가 바뀌면 CALL
	void	AlterExp_Animus(int nAlterExp);	//Exp가 바뀌면 CALL
	void	AlterMode_Animus(BYTE byMode);	//AI적으로 자동으로모드가 바뀔때 호출
	void	Return_AnimusAsk();
	void	_AnimusReturn();
	void	_TowerDestroy(CGuardTower* pTowerObj);
	WORD	_TowerReturn(__ITEM* pTowerItem);
	void	_TowerAllReturn();
	void	ForcePullUnit(bool bOneSelf, bool bLogout);	//유닛의 강제견인
	void	_UpdateUnitDebt(BYTE bySlotIndex, DWORD dwKeep, DWORD dwPull);
	int		_check_exp_after_attack(int nDamagedObjNum, _be_damaged_char* pList);
	void	RewardChangeClass(_class_fld* pClasFld);
	BYTE	skill_process(int nEffectCode, int nSkillIndex, _CHRID* pidDst, int* OUT pnLv);	//스킬, 클래스스킬 통합..

	//CALC..
	void	CalcExp(CCharacter* pDst, int nDam);
	void	CalcDefTol();
	void	ReCalcMaxHFSP(bool bSend = true);
	int		_CalcMaxHP();
	int		_CalcMaxFP();
	int		_CalcMaxSP();	
	float	CalcEquipSpeed();
	int		CalcEquipAttackDelay();
	int	    GetMasteryCumAfterAttack(int nDstLv);
	int		GetLimHP();
	int		GetLimFP();
	int		GetLimSP();
	float	GetMoveSpeed();
	int		GetWeaponRange();
	void	make_gen_attack_param(CCharacter* pDst, BYTE byPart, float fAddBulletFc, _attack_param* OUT pAP);
	void	make_skill_attack_param(CCharacter* pDst, _skill_fld* pSkillFld, int nAttType, __ITEM* pBulletItem, float fAddBulletFc, _attack_param* OUT pAP);
	void	make_force_attack_param(CCharacter* pDst, _force_fld* pForceFld, __ITEM* pForceItem, float* pfAreaPos, _attack_param* OUT pAP);
	void	make_unit_attack_param(CCharacter* pDst, _UnitPart_fld* pWeaponFld, float fAddBulletFc, _attack_param* OUT pAP);

	//GET DATA
	BYTE	_GetPartyMemberInCircle(CPlayer** out_ppMember, int nMax); 
	_ITEM_EFFECT* _GetItemEffect(__ITEM* pItem);

	//CHECK..
	int		_pre_check_normal_attack(CCharacter* pDst, WORD wBulletSerial, __ITEM** OUT ppBulletProp, _BulletItem_fld** OUT ppfldBullet);
	int		_pre_check_skill_attack(CCharacter* pDst, BYTE byEffectCode, _skill_fld* pSkillFld, WORD wBulletSerial, __ITEM** OUT ppBulletProp, _BulletItem_fld** OUT ppfldBullet, int nEffectGroup);
	int		_pre_check_force_attack(CCharacter* pDst, float* IN OUT pfTarPos, WORD wForceItemSerial, _force_fld** OUT ppForceFld, __ITEM** OUT ppForceItem);
	int		_pre_check_unit_attack(CCharacter* pDst, BYTE byWeaponPart, _UnitPart_fld** OUT ppWeaponFld, _UnitBullet_fld** OUT ppBulletFld, _unit_bullet_param** OUT ppBulletParam);
	void	_check_dst_param_after_attack(int nTotalDam, CCharacter* pTarget);
	bool	CheckDstCountAttack(CCharacter* pDst);

	//YES OR NO
	bool	IsOutExtraStopPos(float* pfStopPos);//멈춰야할곳과 이동타겟지점이 20미만일경우 FALSE
	bool	IsEffectableEquip(_STORAGE_LIST::_storage_con* pCon);
	bool	IsRidingUnit();	//유닛탑승중인지..
	bool	IsSFUsableSFMastery(BYTE byMasteryCode, int nMasteryIndex, BYTE byLv);
	bool	IsSFUseableRace(BYTE byEffectCode, WORD wEffectIndex);
	bool	IsSFUseableTarget(BYTE byEffectCode, WORD wEffectIndex, CCharacter* pTargetChar);
	bool	IsSFUsableFP(BYTE byEffectCode, WORD wEffectIndex, WORD* OUT pLeftFP);
	__ITEM* IsBulletValidity(WORD wBulletSerial);	

	virtual void Loop();
	virtual void AlterSec();

	static	float GetExpRate(BOOL bKill, int nPartyNum);
	static	void  SetStaticMember();
	static	void  OnLoop_Static();

	//attack..
	void	pc_PlayAttack_Gen(CCharacter* pDst, BYTE byAttPart, WORD wBulletSerial, bool bCount = false);
	void	pc_PlayAttack_Skill(CCharacter* pDst, BYTE byEffectCode, WORD wSkillIndex, WORD wBulletSerial);
	void	pc_PlayAttack_Force(CCharacter* pDst, float* pfAreaPos, WORD wForceSerial);
	void	pc_PlayAttack_Unit(CCharacter* pDst, BYTE byWeaponPart);
	void	pc_PlayAttack_Test(BYTE byEffectCode, BYTE byEffectIndex, WORD wBulletItemSerial, BYTE byWeaponPart, short* pzTar);

	//chat..
	void	pc_ChatOperatorRequest(BYTE bySize, char* psChatData);
	void	pc_ChatCircleRequest(BYTE bySize, char* psChatData);
	void	pc_ChatWhsiperRequest(WORD wDstIndex, BYTE bySize, char* psChatData);
	void	pc_ChatFarRequest(char* pszName, BYTE bySize, char* psChatData);
	void	pc_ChatPartyRequest(BYTE bySize, char* psChatData);
	void	pc_ChatRaceRequest(BYTE bySize, char* psChatData);
	void	pc_ChatCheatRequest(BYTE bySize, char* psChatData);
	void	pc_ChatMgrWhisperRequest(BYTE bySize, char* psChatData);//운영자간의 귓말

	//map..pos..
	void	pc_NewPosStart(BYTE byMapInMode);		//위치를 잡으면 보낸다.. 첫시작, 포탈이동, 부활..
	void	pc_Revival();			//재생
	void	pc_MovePortal(int nPortalIndex);
	void	pc_MoveNext(BYTE byMoveType, BYTE byModeType, float* pfCur, float* pfTar);
	void	pc_RealMovPos(float* pfCur);
	void	pc_MoveStop(float* pfCur);
	void	pc_GotoBasePortalRequest(WORD wItemSerial);
	void	pc_GotoAvatorRequest(char* pszAvatorName);
	void    pc_MoveModeChangeRequest(BYTE byMoveType);

	//trade..
	void	pc_BuyItemStore(CItemStore* pStore, BYTE byOfferNum, _buy_store_request_clzo::_list* pList);
	void	pc_SellItemStore(CItemStore* pStore, BYTE byOfferNum, _sell_store_request_clzo::_list* pList);
	void	pc_RepairItemStore(CItemStore* pStore, BYTE byOfferNum, _repair_request_clzo::_list* pList);
	void	pc_ExchangeDalantForGold(DWORD dwDalant);	
	void	pc_ExchangeGoldForDalant(DWORD dwGold);

	//item..
	void	pc_ThrowStorageItem(_STORAGE_POS_INDIV* pItem);
	void	pc_UsePotionItem(_STORAGE_POS_INDIV* pItem);
	void	pc_EquipPart(_STORAGE_POS_INDIV* pItem);
	void	pc_EmbellishPart(_STORAGE_POS_INDIV* pItem, WORD wChangeSerial);
	void	pc_OffPart(_STORAGE_POS_INDIV* pItem);
	void	pc_MakeItem(_STORAGE_POS_INDIV* pipMakeTool, WORD wManualIndex, BYTE byMaterialNum, _STORAGE_POS_INDIV* pipMaterials);
	void	pc_UpgradeItem(_STORAGE_POS_INDIV* pposTalik, _STORAGE_POS_INDIV* pposToolItem, _STORAGE_POS_INDIV* pposUpgItem, BYTE byJewelNum, _STORAGE_POS_INDIV* pposUpgJewel);
	void	pc_DowngradeItem(_STORAGE_POS_INDIV* pposTalik, _STORAGE_POS_INDIV* pposToolItem, _STORAGE_POS_INDIV* pposUpgItem);
	void	pc_TakeLootingItem(CItemBox* pBox, WORD wAddSerial);
	void	pc_AddBag(WORD wBagItemSerial);
	void	pc_ExitSaveDataRequest(WORD wDataSize, char* psData);

	//ui..
	void	pc_SFLinkChange(char sInvenTable, WORD wInvenIndex, char sLinkTable, WORD wLinkIndex);
	void	pc_ForceInvenChange(_STORAGE_POS_INDIV* pItem, WORD wReplaceSerial);
	void	pc_AnimusInvenChange(_STORAGE_POS_INDIV* pItem, WORD wReplaceSerial);
	void	pc_ResSeparation(WORD wStartSerial, BYTE byMoveAmount);
	void	pc_ResDivision(WORD wStartSerial, WORD wTarSerial, BYTE byMoveAmount);
	void	pc_PotionSocketChange(_STORAGE_POS_INDIV* pItem, _STORAGE_POS_INDIV* pTarPos);
	void	pc_PotionSeparation(BYTE byStartPos, BYTE byTargetPos, WORD wSerial, BYTE byAmount);
	void	pc_PotionDivision(BYTE byStartPos, BYTE byTargetPos, WORD wSerial, WORD wTarSerial, BYTE byAmount);

	//resource..
	void	pc_MineStart(BYTE byMineIndex);
	void	pc_MineCancle(WORD wBatterySerial);
	void	pc_MineComplete(BYTE byOreIndex, WORD wBatterySerial);
	void	pc_OreCutting(WORD wOreSerial, BYTE byProcessNum);
	void	pc_OreIntoBag(WORD wResIndex, WORD wSerial, BYTE byAddAmount);
	void	pc_CuttingComplete(BYTE byNpcRace);

	//party..
	void	pc_PartyJoinInvitation(WORD wDstIndex);
	void	pc_PartyJoinInvitationAnswer(_CLID* pidBoss);
	void	pc_PartyJoinApplication(WORD wBossIndex);
	void	pc_PartyJoinApplicationAnswer(_CLID* pidApplicant);
	void	pc_PartyLeaveSelfReqeuest();
	void	pc_PartyLeaveCompulsionReqeuest(DWORD dwExiterSerial);
	void	pc_PartyDisJointReqeuest();
	void	pc_PartySuccessionReqeuest(DWORD dwSuccessorSerial);
	void	pc_PartyLockReqeuest(bool bLock);

	//effect..
	void	pc_ForceRequest(WORD wForceSerial, _CHRID* pidDst);
	void	pc_SkillRequest(BYTE bySkillIndex, _CHRID* pidDst);
	void	pc_ClassSkillRequest(WORD wSkillIndex, _CHRID* pidDst);
	void	pc_MakeTowerRequest(WORD wSkillIndex, WORD wTowerItemSerial, BYTE byMaterialNum, _make_tower_request_clzo::__material* pMaterial, float* pfPos);
	void	pc_BackTowerRequest(DWORD dwTowerObjSerial);

	//DTrade..
	void	pc_DTradeAskRequest(WORD wDstIndex);
	void	pc_DTradeAnswerRequest(_CLID* pidAsker);
	void	pc_DTradeCancleRequest();
	void	pc_DTradeLockRequest();
	void	pc_DTradeAddRequest(BYTE bySlotIndex, BYTE byStorageCode, DWORD dwSerial, BYTE byAmount);
	void	pc_DTradeDelRequest(BYTE bySlotIndex);
	void	pc_DTradeBetRequest(BYTE byMoneyUnit, DWORD dwBetAmount);
	void	pc_DTradeOKRequest();

	//Dungeon..
	void 	pc_OpenBattleDungeon(DWORD dwItemSerial);
	void   	pc_GotoDungeon(WORD wGateIndex, DWORD dwGateSerial);
	void 	pc_OutofDungeon(BYTE byCompleteCode, CBattleDungeon* pDungeon = NULL);

	//unit..
	void    pc_UnitFrameBuyRequest(BYTE byFrameCode);
	void    pc_UnitSellRequest(BYTE bySlotIndex);
	void    pc_UnitPartTuningRequest(BYTE bySlotIndex, BYTE byTuningNum, _tuning_data* pTuningData);
	void    pc_UnitFrameRepairRequest(BYTE bySlotIndex);
	void    pc_UnitBulletFillRequest(BYTE bySlotIndex, BYTE* pbyBulletIndex);
	void    pc_UnitPackFillRequest(BYTE bySlotIndex, BYTE byFillNum, _unit_pack_fill_request_clzo::__list* pList);
	void    pc_UnitBulletReplaceRequest(BYTE bySlotIndex, BYTE byPackIndex, BYTE byBulletPart);
	void    pc_UnitDeliveryRequest(BYTE bySlotIndex, CItemStore* pStore, float* pfNewPos);
	void    pc_UnitReturnRequest();
	void 	pc_UnitTakeRequest();
	void	pc_UnitLeaveRequest(float* pfNewPos);

	//recall..
	void	pc_AnimusRecallRequest(WORD wAnimusItemSerial, WORD wAnimusClientHP, WORD wAnimusClientFP);
	void	pc_AnimusReturnRequest();
	void	pc_AnimusCommandRequest(BYTE byCommandCode);
	void	pc_AnimusTargetRequest(BYTE byObjectID, WORD wObjectIndex, DWORD dwObjectSerial);

	//event..
	void	pc_SelectClassRequest(WORD wSelClassIndex);

	//quest..

	bool	pc_ChangeModeType(int nModeType);

	virtual int		SetDamage(int nDam, CCharacter* pDst, int nDstLv, bool bCrt);
	virtual float	GetWidth();
	virtual float	GetAttackRange();
	virtual int		GetDefFC(int nAttactPart, CCharacter* pAttChar);
	virtual void	SetAttackPart(int nAttactPart)		{	m_nLastBeatenPart = nAttactPart;	}
	virtual int		GetLevel()							{	return m_Param.GetLevel();	}
	virtual int		GetDefSkill();
	virtual int		GetFireTol();
	virtual int		GetWaterTol();
	virtual int		GetSoilTol();
	virtual int		GetWindTol();
	virtual float	GetWeaponAdjust();
	virtual float	GetDefGap()							{	return ((_player_fld*)m_pRecordSet)->m_fDefGap; }
	virtual float	GetDefFacing()						{	return ((_player_fld*)m_pRecordSet)->m_fDefFacing; }
	virtual int		AttackableHeight();
	virtual int		GetGenAttackProb(CCharacter* pDst, int nPart);
	virtual	int		GetObjRace()						{	return m_Param.GetRaceSexCode()/2;		}
	virtual bool	IsRecvableContEffect(); 
	virtual bool	IsBeAttackedAble()					{	if(m_bObserver)	return false;	return true;	}

	//포스, 스킬 일시효과
	virtual bool	F_AttHPtoDstFP_Once(CCharacter* pDstObj);		// 자신의 HP를 대상의 FP로 만든다
	virtual bool	F_ContDamageTimeInc_Once(CCharacter* pDstObj, float fEffectValue);	// 지속 피해보조 남은 시간 늘이기
	virtual	bool	F_Resurrect_Once(CCharacter* pDstObj);	// 죽은 유저를 재생한다.
	virtual bool	F_HPInc_Once(CCharacter* pDstObj, float fEffectValue);	// 현재 HP증가
	virtual bool	F_STInc_Once(CCharacter* pDstObj, float fEffectValue);	// 현재 ST증가
	virtual bool	F_ContHelpTimeInc_Once(CCharacter* pDstObj, float fEffectValue);	// 지속 이득보조의 남은 시간 늘리기	
	virtual bool	F_OverHealing_Once(CCharacter* pDstObj, float fEffectValue);	// overhealing
	virtual bool	F_LateContHelpSkillRemove_Once(CCharacter* pDstObj);	// 가장 최근 지속이득보조 스킬 해제	
	virtual bool	F_LateContHelpForceRemove_Once(CCharacter* pDstObj);	// 가장 최근 지속이득보조 포스 해제
	virtual bool	F_LateContDamageForceRemove_Once(CCharacter* pDstObj);	// 가장 최근 지속피해보조 포스 해제
	virtual bool	F_AllContHelpSkillRemove_Once(CCharacter* pDstObj);	// 대상에 걸려있는 지속이득보조 스킬 제거
	virtual bool	F_AllContHelpForceRemove_Once(CCharacter* pDstObj);	// 대상에 걸려있는 지속이득보조 포스 제거
	virtual bool	F_AllContDamageForceRemove_Once(CCharacter* pDstObj);	// 대상에 걸려있는 지속 피해 보조 포스 해제
	virtual bool	F_OthersContHelpSFRemove_Once(float fEffectValue);	// 시전자 주위 다른 종족들의 지속 이득 보조 스킬 & 포스 해제
	virtual bool	S_SkillContHelpTimeInc_Once(CCharacter* pDstObj, float fEffectValue);	// skill에 의한 지속적인 이득 남은 시간 늘이기
	virtual bool	S_ConvertMonsterTarget(CCharacter* pDstObj, float fEffectValue);
	virtual bool	S_TransMonsterHP(CCharacter* pDstObj, float fEffectValue);
	virtual bool	S_ReleaseMonsterTarget(CCharacter* pDstObj, float fEffectValue);

	//MANAGE..
	void	mgr_tracing(bool bOn);		//투명.. 
	void	mgr_matchless(bool bMatchless);	//무적
	void    mgr_kick(char* pszCharName);					//강퇴..
	void	mgr_recall_player(char* pszCharName);	//소환..
	void	mgr_recall_mon(char* pszMonCode, int nCreateNum);	//몬스터 소환..
	void	mgr_dumb(char* pszCharName, DWORD dwDurHour);	//채팅금지..
	void	mgr_whisper(int nSize, char* pszMsg);	//운영자귓말
	void	mgr_query(int nWordNum, char* pszMsg0, char* pszMsg1, char* pszMsg2, char* pszMsg3, char* pszMsg4);	//질의	
	void	dev_lv(int nLv);			//레벨변경
	void	dev_die();					//자살
	void    dev_up_forceitem(int nCum);	//모든 포스아이템의 사용누적수를 변경
	void	dev_up_skill(char* pszSkillCode, int nCum);	//스킬의 사용누적수를 변경
	void	dev_up_forcemastery(int nCum);	//모든 포스 마스터리의 성공누적수를 변경
	void	dev_up_mastery(int nMasteryCode, int nMasteryIndex, int nCum);	//특정 마스터리증가
	void	dev_up_all(int nCum);	//모든숙련도변경
	void	dev_animus_recall_time_free(bool bFree);	//애니머스의 소환대기시간을 없앰.
	void	dev_set_animus_exp(int nExpPoint);	//애니머스의 경험치포인트를 올림..
	void	dev_init_monster();	//몬스터 초기화
	void	dev_money();		//골드와 달란트 천만으로 변경
	void	dev_full_force();	//포스창 가득채움
	void	dev_loot_material();		//재료루팅
	void	dev_loot_item(char* pszItemCode, int nNum);	//아이템을 루팅
	void	dev_loot_item_lv(char* pszPart, int nLv);	//원하는 파트의 지정레벨 아이템을 전부 루팅
	void	dev_inven_empty();	//인벤비우기
	void	dev_avator_copy(char* pszDstName);	//케릭터카피
	void	dev_make_succ(bool bSucc);	//제작 업그에드 무조건 성공
	void	dev_cont_effect_time(DWORD dwSec);	//걸린 지속효과의 시간 조정
	void	dev_cont_effect_del();	//걸린 지속효과를 전부 없앤다

	//Send Message Load..
	void	SendMsg_NewViewOther(BYTE byViewType);
	void	SendMsg_EquipPartChange(BYTE byPart);
	void	SendMsg_UnitRideChange(bool bTake, CParkingUnit* pUnit);
	void	SendMsg_OtherShapeAll(CPlayer* pOther);
	void	SendMsg_OtherShapePart(CPlayer* pOther);
	void	SendMsg_OtherShapeError(CPlayer* pOther, BYTE byErrCode);
	void    SendMsg_AttackResult_Error(int nErrCode);
	void    SendMsg_AttackResult_Gen(CAttack* pAt, BYTE byBulletIndex);
	void	SendMsg_AttackResult_Skill(BYTE byEffectCode, CPlayerAttack* pAt, BYTE byBulletIndex);
	void	SendMsg_AttackResult_Force(CAttack* pAt);
	void	SendMsg_AttackResult_Unit(CAttack* pAt, BYTE byWeaponPart, BYTE byBulletIndex);
	void	SendMsg_AttackResult_Count(CAttack* pAt);
	void	SendMsg_TestAttackResult(BYTE byEffectCode, BYTE byEffectIndex, WORD wBulletItemIndex, BYTE byEffectLv, BYTE byWeaponPart, short* pzTar);
	void	SendMsg_DamageResult(__ITEM* pItem);
	void	SendMsg_Destroy();
	void	SendMsg_Die();
	void	SendMsg_Revival(BYTE byRet, bool bEquialZone);
	void	SendMsg_Resurrect(BYTE byRet);
	virtual void SendMsg_FixPosition(int n);	//움직이지 않을때 주위의 Player에게 실시간으로 Fix Position Message를 Send
	virtual void SendMsg_RealMovePoint(int n);	//특정인한테..
	void	SendMsg_MoveNext(bool bOtherSend = true);
	void	SendMsg_Stop(bool bAll = true);
	void	SendMsg_MoveError(BYTE byRetCode);
	void	SendMsg_Level(int nLevel);
	void	SendMsg_MaxHFSP();
	void	SendMsg_Recover();
	void	SendMsg_AlterBooster();
	void	SendMsg_TakeNewResult(BYTE byErrCode, __ITEM* pItem);
	void	SendMsg_TakeAddResult(BYTE byErrCode, __ITEM* pItem);
	void	SendMsg_ThrowStorageResult(BYTE byErrCode);
	void	SendMsg_MapOut(BYTE byMapOutCode, BYTE byNextMapCode);
	void	SendMsg_StateInform(DWORD dwStateFlag);
	void	SendMsg_GotoBasePortalResult(BYTE byErrCode);
	void	SendMsg_GotoRecallResult(BYTE byErrCode, BYTE byMapCode, float* pfStartPos);
	void	SendMsg_ExitSaveDataResult(BYTE byRetCode);
	void	SendMsg_StatInform(BYTE byStatIndex, DWORD dwNewStat);
	void	SendMsg_FcitemInform(WORD wItemSerial, DWORD dwNewStat);
	void	SendMsg_ItemStorageRefresh(BYTE byStorageCode);

	void	SendMsg_BuyItemStoreResult(CItemStore* pStore, bool bSucc, int nOfferNum, _buy_offer* pCard, char* pszErrCode);
	void	SendMsg_SellItemStoreResult(CItemStore* pStore, bool bSucc, char* pszErrCode);
	void	SendMsg_RepairItemStoreResult(CItemStore* pStore, int nOfferNum, _repair_offer* pCard, bool bSucc, char* pszErrCode);
	void	SendMsg_ExchangeMoneyResult(BYTE byErrCode);
	void	SendMsg_EconomyHistoryInform();
//	void	SendMsg_EconomyRateInform();
	void	SendMsg_EconomyRateInform(bool bStart);

	void	SendMsg_UsePotionResult(BYTE byErrCode, WORD wSerial);
	void	SendMsg_EquipPartResult(BYTE byErrCode);
	void	SendMsg_EmbellishResult(BYTE byErrCode);
	void	SendMsg_OffPartResult(BYTE byErrCode);
	void	SendMsg_MakeItemResult(BYTE byErrCode);
	void	SendMsg_ItemUpgrade(BYTE byErrCode);
	void	SendMsg_ItemDowngrade(BYTE byErrCode);
	void	SendMsg_PotionSocketChange(BYTE byErrCode);
	void	SendMsg_PotionSeparation(WORD wParentSerial, BYTE byParentAmount, WORD wChildSerial, BYTE byChildAmount, int nRet);
	void	SendMsg_PotionDivision(WORD wSerial, BYTE byParentAmount, WORD wChildSerial, BYTE byChildAmount, int nRet);
	void	SendMsg_BaseDownloadResult();
	void	SendMsg_InvenDownloadResult();
	void	SendMsg_CumDownloadResult();
	void	SendMsg_ForceDownloadResult();
	void	SendMsg_SpecialDownloadResult();
	void	SendMsg_StoreListResult();
	void	SendMsg_ModeChange(BYTE byMode);
	void	SendMsg_MovePortal(BYTE byRet, BYTE byMapIndex, BYTE byPotalIndex, float* pfStartPos, bool bEqualZone);

	void	SendMsg_StartNewPos(BYTE byMapInMode);
	void	SendMsg_BreakdownEquipItem(BYTE byPartIndex, WORD wItemSerial);
	void	SendMsg_ForceInvenChange(BYTE byErrCode);
	void	SendMsg_AnimusInvenChange(BYTE byErrCode);
	void	SendMsg_UnitGetOnResult(BYTE byErrCode);
	void	SendMsg_UnitGetOffResult(BYTE byErrCode);
	void	SendMsg_ResSeparation(BYTE byErrCode, __ITEM* pStartOre, __ITEM* pNewOre);
	void	SendMsg_ResDivision(BYTE byErrCode, __ITEM* pStartOre, __ITEM* pTargetOre);
	void	SendMsg_AddBagResult(BYTE byErrCode);
	void	SendMsg_DeleteStorageInform(BYTE byStorageCode, WORD wSerial);

	void	SendMsg_MineStartResult(BYTE byErrCode);
	void	SendMsg_MineCancle(BYTE byErrCode, WORD wBatteryLeftDurPoint);
	void	SendMsg_MineCompleteResult(BYTE byErrCode, BYTE byOreIndex, WORD dwOreSerial, BYTE byOreDur, WORD dwBatteryLeftDur);
	void	SendMsg_OreCuttingResult(BYTE byErrCode, BYTE byLeftOreNum, DWORD dwConsumDalant);
	void	SendMsg_OreIntoBagResult(BYTE byErrCode, WORD wNewSerial);
	void	SendMsg_CuttingCompleteResult();

	//party..
	void	SendMsg_PartyJoinInvitationQuestion(WORD wJoinerIndex);
	void	SendMsg_PartyJoinMemberResult(CPartyPlayer* pJoiner);
	void	SendMsg_PartyJoinJoinerResult();
	void	SendMsg_PartyJoinApplicationQuestion(CPlayer* pJoiner);
	void	SendMsg_PartyLeaveSelfResult(CPartyPlayer* pLeaver, bool bWorldExit);
	void	SendMsg_PartyLeaveCompulsionResult(CPartyPlayer* pLeaver);
	void	SendMsg_PartyDisjointResult(BYTE bSuccess);
	void	SendMsg_PartySuccessResult(CPartyPlayer* pSuccessor);
	void	SendMsg_PartyLockResult(BYTE byRet);
	void	SendData_PartyMemberInfoToMembers();	//파티에 가입을 했을때..
	void	SendData_PartyMemberInfo(WORD wDstIndex);//(DWORD dwDstIP);//새로가입한 파티원에게..
	void	SendData_PartyMemberHP();	//파티중 HP, FP에 변화가 있을때..
	void	SendData_PartyMemberFP();
	void	SendData_PartyMemberSP();
	void	SendData_PartyMemberLv();
	void	SendData_PartyMemberPos();
	void	SendData_PartyMemberMaxHFSP();
	void	SendData_PartyMemberEffect(BYTE byAlterCode, WORD wEffectCode, BYTE byLv);

	//event..
	void	SendMsg_AlterExpInform();
	void	SendMsg_AlterItemDurInform(BYTE byStorageCode, WORD wItemSerial, DWORD dwDur);
	void 	SendMsg_ChangeClassCommand();
	void    SendMsg_SelectClassResult(BYTE byErrCode, WORD wSelClassIndex);
	void	SendMsg_RewardAddItem(__ITEM* pItem);
	void	SendMsg_AlterWeaponBulletInform(WORD wItemSerial, WORD wLeftNum);
	void	SendMsg_AlterUnitBulletInform(BYTE byPart, WORD wLeftNum);
	
	//effect..
	void	SendMsg_ForceResult(BYTE byErrCode, _CHRID* pidDst, __ITEM* pForceItem, int nSFLv);
	void	SendMsg_SkillResult(BYTE byErrCode, _CHRID* pidDst, BYTE bySkillIndex, int nSFLv);
	void	SendMsg_ClassSkillResult(BYTE byErrCode, _CHRID* pidDst, WORD wSkillIndex);
	void	SendMsg_SetFPInform();
	virtual void SendMsg_SetHPInform();
	void	SendMsg_SetSPInform();
	void	SendMsg_InsertItemInform(BYTE byStorageCode, __ITEM* pItem);
	void	SendMsg_CreateTowerResult(BYTE byErrCode, DWORD dwTowerObjSerial);
	void	SendMsg_BackTowerResult(BYTE byErrCode, WORD wItemSerial, WORD wLeftHP);
	void	SendMsg_AlterTowerHP(WORD wItemSerial, WORD wLeftHP);	

	//dtrade..
	void	SendMsg_DTradeAskResult(BYTE byErrCode);
	void	SendMsg_DTradeAskInform(CPlayer* pAsker);
	void	SendMsg_DTradeAnswerResult(BYTE byErrCode);
	void	SendMsg_DTradeStartInform(CPlayer* pAsker, CPlayer* pAnswer);
	void	SendMsg_DTradeCancleResult(BYTE byErrCode);
	void	SendMsg_DTradeCancleInform();
	void	SendMsg_DTradeLockResult(BYTE byErrCode);
	void	SendMsg_DTradeLockInform();
	void	SendMsg_DTradeAddResult(BYTE byErrCode);
	void	SendMsg_DTradeAddInform(BYTE bySlotIndex, __ITEM* pItem, BYTE byAmount);
	void	SendMsg_DTradeDelResult(BYTE byErrCode);
	void	SendMsg_DTradeDelInform(BYTE bySlotIndex);
	void	SendMsg_DTradeBetResult(BYTE byErrCode);
	void	SendMsg_DTradeBetInform(BYTE byUnitCode, DWORD dwAmount);
	void	SendMsg_DTradeOKResult(BYTE byErrCode);
	void	SendMsg_DTradeOKInform();
	void	SendMsg_DTradeAccomplishInform(bool bSucc, WORD wStartSerial);
	void	SendMsg_DTradeCloseInform(BYTE byCloseCode);
	void	SendMsg_DTradeUnitInfoInform(BYTE byTradeSlotIndex, _UNIT_DB_BASE::_LIST* pUnitData);
	void	SendMsg_DTradeUnitAddInform(WORD wUnitKeySerial, _UNIT_DB_BASE::_LIST* pUnitData);

	//dungeon..
	void	SendMsg_BattleDungeonOpenResult(BYTE byRetCode);
	void	SendMsg_GotoDungeon(BYTE byErrCode, BYTE byMapCode, float* fStartPos, CBattleDungeon* pDungeon);
	void	SendMsg_CloseDungeon(BYTE byCompleteCode);
	void	SendMsg_OutofDungeon(BYTE byErrCode, BYTE byMapCode, float* fStartPos);

	//unit..
	void	SendMsg_UnitFrameBuyResult(BYTE byRetCode, BYTE byFrameCode, BYTE byUnitSlotIndex, WORD wKeyIndex, WORD wKeySerial, DWORD* pdwConsumMoney);
	void	SendMsg_UnitSellResult(BYTE byRetCode, BYTE bySlotIndex, WORD wKeySerial, DWORD* pdwAddMoney, DWORD dwTotalNonpay, DWORD dwSumDalant, DWORD dwSumGold);
	void	SendMsg_UnitPartTuningResult(BYTE byRetCode, BYTE bySlotIndex, int* pnCost);
	void	SendMsg_UnitFrameRepairResult(BYTE byRetCode, BYTE bySlotIndex, WORD wNewGauge, DWORD dwConsumDalant);
	void	SendMsg_UnitBulletFillResult(BYTE byRetCode, BYTE bySlotIndex, BYTE* pbyBulletIndex, DWORD* pdwConsumMoney);
	void	SendMsg_UnitPackFillResult(BYTE byRetCode, BYTE bySlotIndex, BYTE byFillNum, _unit_pack_fill_request_clzo::__list* pList, DWORD* pdwConsumMoney);
	void	SendMsg_UnitDeliveryResult(BYTE byRetCode, BYTE bySlotIndex, DWORD dwParkingUnitSerial, DWORD dwPayDalant);
	void	SendMsg_UnitReturnResult(BYTE byRetCode, DWORD dwPayDalant);
	void	SendMsg_UnitTakeResult(BYTE byRetCode);
	void	SendMsg_UnitLeaveResult(BYTE byRetCode);
	void	SendMsg_UnitAlterFeeInform(BYTE bySlotIndex, DWORD dwKeepingFee, DWORD dwPullingFee);
	void	SendMsg_UnitBulletReplaceResult(BYTE byRetCode);

	//recall..
	void	SendMsg_AnimusRecallResult(BYTE byResultCode, WORD wLeftFP, CAnimus* pNewAnimus = NULL);
	void	SendMsg_AnimusReturnResult(BYTE byRetCode, WORD wAnimusItemSerial);
	void	SendMsg_AnimusTargetResult(BYTE byRetCode);
	void	SendMsg_AnimusHPInform();
	void	SendMsg_AnimusFPInform();
	void	SendMsg_AnimusExpInform();
	void	SendMsg_AnimusModeInform(BYTE byMode);

	void	SendData_ChatTrans(BYTE byChatType, DWORD dwSenderSerial, BYTE bySize, char* pszMessage);
	void	SendMsg_ChatFarFailure();

	//quest..
	void	SendMsg_InsertNewQuest(BYTE bySlotIndex, DWORD dwEventIndex, BYTE byEventNodeIndex, DWORD dwQuestIndex);
	void 	SendMsg_SelectWaitedQuest(DWORD dwEventIndex, BYTE byEventNodeIndex);
	void	SendMsg_InsertQuestFailure(DWORD dwEventIndex, BYTE byEventNodeIndex);	
	void	SendMsg_QuestComplete(BYTE byQuestDBSlot);
	void	SendMsg_QuestProcess(BYTE byQuestDBSlot, BYTE byActIndex, WORD wCount);
	void	SendMsg_QuestFailure(BYTE byFailCode, BYTE byQuestDBSlot);

	//adjust..
	void	SendMsg_AdjustAmountInform(BYTE byStorageCode, WORD wSerial, DWORD dwDur);

	//MESSENGER 
	void    SendMsg_StartShape_Msgr();
	void	SendMsg_UpdateEquip_Msgr(BYTE byPartCode);


};

#endif 
