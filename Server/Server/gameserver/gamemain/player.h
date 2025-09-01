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

	bool				m_bLoad;		//����κ��� �����͸� �ε���������ΰ�.. m_bLive���� ���ܰ�
	bool				m_bOper;		//��ġ�� create() �´������� pc_ExitSaveDataRequest()����..
	bool				m_bMoveOut;		//���̵���û�� �޳�..
	bool				m_bFullMode;	//Ŭ���̾�Ʈ�� ��������..
	BYTE				m_byUserDgr;	//�������
	BYTE				m_bySubDgr;		//������
	
	bool				m_bBaseDownload, m_bInvenDownload, m_bForceDownload, m_bCumDownload, m_bSpecialDownload;
	
	CPartyPlayer*		m_pPartyMgr;
	CPlayerDB			m_Param;	//Player DB �׸� ���̴� parameter��
	_CLID				m_id;
	
	int					m_nMoveType, m_nModeType;	//�ȱ� or �ٱ�//���� or ��ȭ���	
	bool				m_bMineMode;	//ä�����
	bool				m_bPosMainBase;	//������ �ֳ�..
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
	WORD				m_wVisualVer;	//�ڽ��� 
	int					m_nLastBeatenPart;
	DWORD				m_dwLastState;
	WORD				m_wExpRate;

	int					m_nMaxHP, m_nMaxFP, m_nMaxSP;
	DWORD				m_dwTolValue[TOL_CODE_NUM];
	DWORD				m_dwMineStartTime, m_dwMineDelayTime;
	BYTE				m_byDelayErrorCount;

	//����..
	DWORD					m_dwLastTimeCheckUnitViewOver;
	DWORD					m_dwUnitViewOverTime;
	_UNIT_DB_BASE::_LIST*	m_pUsingUnit;
	CParkingUnit*			m_pParkingUnit;
	BYTE					m_byUsingWeaponPart;

	//��ȯ..
	__ITEM*				m_pRecalledAnimusItem;	//��ȯ�� �ִϸӽ��� ������������
	CAnimus*			m_pRecalledAnimusChar;	//��ȯ�� �ִϸӽ��� �ɸ���������	
	DWORD				m_dwLastRecallTime;		//�ֱٿ� ��ȯ�� ���ѽ�Ų�ð�
	bool				m_bNextRecallReturn;	//��ȯ���� ����

	CMyTimer			m_tmrAlterState;	//Ÿ�̸�
	CMyTimer			m_tmrIntervalSec;	//1�ʴ�.

	//������������ �̵��� �����ʰ� ��ǥ �������..
	float				m_fBeforeDungeonPos[3];
	CMapData*			m_pBeforeDungeonMap;

	//Far whisper�ߴ� ���..
	_MEM_PAST_WHISPER	m_PastWhiper[max_memory_past_whisper];

	//count attack..
	CCharacter*			m_pCountAttackDst;

	//�Ƶ巹���� ����
	DWORD				m_dwStartTime;
	DWORD				m_dwDurTime;
	float				m_fAFRate;
	float				m_fDefRate;

	//ȿ����������
	bool				m_bUpCheckEquipEffect;			// ����, ���õ����� �ö�����
	bool				m_bDownCheckEquipEffect;		// ����, ���õ����� ����������
	BYTE				m_byEffectEquipCode[equip_fix_num+embellish_fix_num];
	DWORD*				m_pdwLastForceActTime;	//���������� �ֱ� �����ð� 
	DWORD*				m_pdwLastSkillActTime;	//��ų������ �ֱ� �����ð� 

	char				m_szItemHistoryFileName[max_history_file_size];
	char				m_szLvHistoryFileName[max_history_file_size];
	DWORD				m_dwUMWHLastTime;

	//ġƮ���ú���.
	bool				m_bCheat_100SuccMake;	//����, ���׷��̵� 100%����
	bool				m_bCheat_Matchless;		//����,, �ѹ�.. ������..
	bool				m_bFreeRecallWaitTime;	//�ִϸӽ� ��ȯ���ð� ����

	static CMgrAvatorItemHistory	s_MgrItemHistory;
	static CMgrAvatorLvHistory		s_MgrLvHistory;
	
public:

	CPlayer();
	virtual ~CPlayer();

	//DATA SET..
	bool	Init(_object_id* pID);				//�������� ����ҷ��� �̸� ȣ���ϴ°�..
	bool	Load(CUserDB* pUser);
	bool	Create();							//������ �������� ������
	void	CreateComplete();					//���ӽ���ó���� ��� ������..
	void	NetClose();							//��������
	void    ExitUpdateDataToWorld();
	void	PastWhisperInit();

	//INIT EVENT..
	void	DTradeInit();
	bool	Corpse(CCharacter* pAtter);			//������ ��ü����
	void	Resurrect();		//��Ȱ	
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
	void	Emb_SelectQuestEvent(BYTE bySelectIndex);	//�̺�Ʈ�߻��� ù ����Ʈ ����
	void	Emb_CheckActForQuest(int nActCode, char* pszReqCode);
	void	Emb_CheckLvUpForQuest();
	void	Emb_CheckTimeoutForQuest();

	//TIMER EVENT..
	void	SenseState();
	void	AutoRecover();
	void	AutoRecover_Animus();	//�ִϸӽ� �ڵ� ȸ��
	void	AutoCharge_Booster();	//���� �ν��� ����
	void	UpdatedMasteryWriteHistory();
	void	_CheckForcePullUnit();

	//MAIN..
	void	NewViewCircleObject();
	void	SetLevel(BYTE byNewLevel);
	void	AlterMoney(int nAlterGold, int nAlterDalant);
	bool	SetTarPos(float* fTarPos, bool bColl = true);
	void	SetEquipEffect(_STORAGE_LIST::_storage_con* pItem, bool bEquip);
	void	SetHaveEffect();//����ȿ���ִ� ������ �κ����뺯�� �̺�Ʈ�߻����� ��ΰ˻�
	DWORD	SetStateFlag(bool bUpdate);
	bool	OutOfMap(CMapData* pIntoMap, WORD wLayerIndex, BYTE byMapOutType, float* pfStartPos);		//��ġ�̵�..�ϴ�..����ʿ��� ����
	bool	IntoMap(BYTE byMapInMode);		//���ο� ��ġ������ ������ ����ִ´�.
	void	UpdateVisualVer();		//m_wVisualVer�� ������Ŵ
	
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
	int		GetVisualVer();			//m_wVisualVer�� ����
	DWORD	GetStateFlag() {	return m_dwLastState; }	
	DWORD   GetLastStateFlag();
	float	GetAdrenAF();
	float	GetAdrenDef();
	void	AlterHP_Animus(int nNewHP);		//HP�� �ٲ�� CALL		//�ִϸӽ�..
	void	AlterFP_Animus(int nNewFP);		//FP�� �ٲ�� CALL
	void	AlterExp_Animus(int nAlterExp);	//Exp�� �ٲ�� CALL
	void	AlterMode_Animus(BYTE byMode);	//AI������ �ڵ����θ�尡 �ٲ� ȣ��
	void	Return_AnimusAsk();
	void	_AnimusReturn();
	void	_TowerDestroy(CGuardTower* pTowerObj);
	WORD	_TowerReturn(__ITEM* pTowerItem);
	void	_TowerAllReturn();
	void	ForcePullUnit(bool bOneSelf, bool bLogout);	//������ ��������
	void	_UpdateUnitDebt(BYTE bySlotIndex, DWORD dwKeep, DWORD dwPull);
	int		_check_exp_after_attack(int nDamagedObjNum, _be_damaged_char* pList);
	void	RewardChangeClass(_class_fld* pClasFld);
	BYTE	skill_process(int nEffectCode, int nSkillIndex, _CHRID* pidDst, int* OUT pnLv);	//��ų, Ŭ������ų ����..

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
	bool	IsOutExtraStopPos(float* pfStopPos);//������Ұ��� �̵�Ÿ�������� 20�̸��ϰ�� FALSE
	bool	IsEffectableEquip(_STORAGE_LIST::_storage_con* pCon);
	bool	IsRidingUnit();	//����ž��������..
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
	void	pc_ChatMgrWhisperRequest(BYTE bySize, char* psChatData);//��ڰ��� �Ӹ�

	//map..pos..
	void	pc_NewPosStart(BYTE byMapInMode);		//��ġ�� ������ ������.. ù����, ��Ż�̵�, ��Ȱ..
	void	pc_Revival();			//���
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

	//����, ��ų �Ͻ�ȿ��
	virtual bool	F_AttHPtoDstFP_Once(CCharacter* pDstObj);		// �ڽ��� HP�� ����� FP�� �����
	virtual bool	F_ContDamageTimeInc_Once(CCharacter* pDstObj, float fEffectValue);	// ���� ���غ��� ���� �ð� ���̱�
	virtual	bool	F_Resurrect_Once(CCharacter* pDstObj);	// ���� ������ ����Ѵ�.
	virtual bool	F_HPInc_Once(CCharacter* pDstObj, float fEffectValue);	// ���� HP����
	virtual bool	F_STInc_Once(CCharacter* pDstObj, float fEffectValue);	// ���� ST����
	virtual bool	F_ContHelpTimeInc_Once(CCharacter* pDstObj, float fEffectValue);	// ���� �̵溸���� ���� �ð� �ø���	
	virtual bool	F_OverHealing_Once(CCharacter* pDstObj, float fEffectValue);	// overhealing
	virtual bool	F_LateContHelpSkillRemove_Once(CCharacter* pDstObj);	// ���� �ֱ� �����̵溸�� ��ų ����	
	virtual bool	F_LateContHelpForceRemove_Once(CCharacter* pDstObj);	// ���� �ֱ� �����̵溸�� ���� ����
	virtual bool	F_LateContDamageForceRemove_Once(CCharacter* pDstObj);	// ���� �ֱ� �������غ��� ���� ����
	virtual bool	F_AllContHelpSkillRemove_Once(CCharacter* pDstObj);	// ��� �ɷ��ִ� �����̵溸�� ��ų ����
	virtual bool	F_AllContHelpForceRemove_Once(CCharacter* pDstObj);	// ��� �ɷ��ִ� �����̵溸�� ���� ����
	virtual bool	F_AllContDamageForceRemove_Once(CCharacter* pDstObj);	// ��� �ɷ��ִ� ���� ���� ���� ���� ����
	virtual bool	F_OthersContHelpSFRemove_Once(float fEffectValue);	// ������ ���� �ٸ� �������� ���� �̵� ���� ��ų & ���� ����
	virtual bool	S_SkillContHelpTimeInc_Once(CCharacter* pDstObj, float fEffectValue);	// skill�� ���� �������� �̵� ���� �ð� ���̱�
	virtual bool	S_ConvertMonsterTarget(CCharacter* pDstObj, float fEffectValue);
	virtual bool	S_TransMonsterHP(CCharacter* pDstObj, float fEffectValue);
	virtual bool	S_ReleaseMonsterTarget(CCharacter* pDstObj, float fEffectValue);

	//MANAGE..
	void	mgr_tracing(bool bOn);		//����.. 
	void	mgr_matchless(bool bMatchless);	//����
	void    mgr_kick(char* pszCharName);					//����..
	void	mgr_recall_player(char* pszCharName);	//��ȯ..
	void	mgr_recall_mon(char* pszMonCode, int nCreateNum);	//���� ��ȯ..
	void	mgr_dumb(char* pszCharName, DWORD dwDurHour);	//ä�ñ���..
	void	mgr_whisper(int nSize, char* pszMsg);	//��ڱӸ�
	void	mgr_query(int nWordNum, char* pszMsg0, char* pszMsg1, char* pszMsg2, char* pszMsg3, char* pszMsg4);	//����	
	void	dev_lv(int nLv);			//��������
	void	dev_die();					//�ڻ�
	void    dev_up_forceitem(int nCum);	//��� ������������ ��봩������ ����
	void	dev_up_skill(char* pszSkillCode, int nCum);	//��ų�� ��봩������ ����
	void	dev_up_forcemastery(int nCum);	//��� ���� �����͸��� ������������ ����
	void	dev_up_mastery(int nMasteryCode, int nMasteryIndex, int nCum);	//Ư�� �����͸�����
	void	dev_up_all(int nCum);	//�����õ�����
	void	dev_animus_recall_time_free(bool bFree);	//�ִϸӽ��� ��ȯ���ð��� ����.
	void	dev_set_animus_exp(int nExpPoint);	//�ִϸӽ��� ����ġ����Ʈ�� �ø�..
	void	dev_init_monster();	//���� �ʱ�ȭ
	void	dev_money();		//���� �޶�Ʈ õ������ ����
	void	dev_full_force();	//����â ����ä��
	void	dev_loot_material();		//������
	void	dev_loot_item(char* pszItemCode, int nNum);	//�������� ����
	void	dev_loot_item_lv(char* pszPart, int nLv);	//���ϴ� ��Ʈ�� �������� �������� ���� ����
	void	dev_inven_empty();	//�κ�����
	void	dev_avator_copy(char* pszDstName);	//�ɸ���ī��
	void	dev_make_succ(bool bSucc);	//���� ���׿��� ������ ����
	void	dev_cont_effect_time(DWORD dwSec);	//�ɸ� ����ȿ���� �ð� ����
	void	dev_cont_effect_del();	//�ɸ� ����ȿ���� ���� ���ش�

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
	virtual void SendMsg_FixPosition(int n);	//�������� ������ ������ Player���� �ǽð����� Fix Position Message�� Send
	virtual void SendMsg_RealMovePoint(int n);	//Ư��������..
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
	void	SendData_PartyMemberInfoToMembers();	//��Ƽ�� ������ ������..
	void	SendData_PartyMemberInfo(WORD wDstIndex);//(DWORD dwDstIP);//���ΰ����� ��Ƽ������..
	void	SendData_PartyMemberHP();	//��Ƽ�� HP, FP�� ��ȭ�� ������..
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
