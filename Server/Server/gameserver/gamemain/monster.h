// Monster.h: interface for the CMonster class.
//
//////////////////////////////////////////////////////////////////////

#ifndef	_Monster_h_
#define _Monster_h_

#include "Character.h"
#include "MyClassUtil.h"
#include "PlayerDB.h"
#include "MonsterAI.h"
#include "Player.h"
#include "MonsterCharacterFld.h"
#include "MonsterLootingMgr.h"

class CBattleDungeon;

struct _monster_create_setdata : public _character_create_setdata
{
	_mon_active*		pActiveRec;
	_dummy_position*	pDumPosition;
	CBattleDungeon*		pBattleDungeon;	//battle dungeon에서 생긴 몬스터라면 세팅..

	_monster_create_setdata(){
		pActiveRec = NULL;
		pDumPosition = NULL;
		pBattleDungeon = NULL;	//battle dungeon에서 생긴 몬스터라면 세팅..
	}
};  

class CPlayer;
struct _mon_active;

class CMonster : public CCharacter
{
public:
	_mon_active*		m_pActiveRec;	//시작할 dummy의 pointer
	_dummy_position*	m_pDumPosition;
	CBattleDungeon*		m_pBattleDungeon;	//battle dungeon에서 생긴 몬스터라면 세팅..

	int m_nHP;
	DWORD m_dwLastDestroyTime;
	bool  m_bTurn;

	///*	MAYUN
	enum	{  max_part = 5};
	enum	{  DEFUP , DEFDOWN ,DEFHAND ,DEFFOOT, DEFHEAD };
	enum	{  max_path = 16};
	enum	{  max_attack_player = 3};
	enum	{  max_area_attack_object = 30	};
	//AI 상태 변수들 
	float		m_AroundX,m_AroundY;
	MONSTERAI	m_MonAI;
	CCharacter	*m_pPlayerTarget;//타겟 플레이어
	CMonster	*m_pMonTarget;//타겟 몬스터 
	CCharacter	*m_pSearchPlayer;//새로 찾은 캐릭터, 현재 타겟아님!
	CCharacter	*m_pAssistPlayer;//공격요청이 들어온 타겟 플레이어 
	CMonster	*m_pSearchMon;//새로 찾은 몬스터, 현재 타겟아님!
	CMonster	*m_pHelpMonster;//누가 나에게 도움을 요청을 했는가?
	float		m_MoveTarget[3];//이해야할 새 좌표//현재의 이동 타겟점 아님!
	float		m_PlayerPos[max_path][3];//플레이어의 타겟의 위치 좌표 노드  
	//int			m_CurNode;
	DWORD		m_CurNode;
	DWORD		m_EndNode;
	
	BOOL		m_bNode;//노드가 추가되었는가?
	DWORD		m_CurrentSkill;//현재 사용중인 스킬
	DWORD		m_SearchSkill;//새로 선택 된 스킬 
	BOOL		m_RunAway;//도망 상태인가?
	
	
	//능력치 
	DWORD		m_LifeMax;
	DWORD		m_LifeCicle;
	float		m_Scale;
	float		m_Mightiness;
	DWORD		m_ChaseDist;
	DWORD		m_MaxSkill;//최대 스킬 
	SKILL		m_Skill[MAX_SKILL];//사용가능 스킬 
	int			m_DefPart[max_part];//방어력 

	CCharacter	*m_AttackedPlayer[max_attack_player];
	CLootingMgr m_LootMgr;

	//// AI 함수들 
	void		InitAI();
	CCharacter* SearchNearPlayer();//공격할 주위의 player를 찾는다
	CCharacter* SearchNearMonster();////공격할 주위의 Monster를 찾는다
	
	void		GetTarPosChase();
	void		InitMonster();
	int			GetAttackPart();
	void		Attack();
	float		GetAttackProb();
	BOOL		IsFront(float *cur,float *tar,float x,float y);//현재 위치에서 타겟까지의 위치가 앞쪽인가 뒤쪽인가?
	void		GetPlayerPos();//플레이어 위치 + 슬롯 위치 
	void		GetTargetPosProcess();
	BOOL		IsValidPlayer()	{
		if (m_pPlayerTarget)return m_pPlayerTarget->m_bLive&&
			(!((CPlayer*)m_pPlayerTarget)->m_bCorpse);
		else return false;
	}
	BOOL		IsValidPlayer(CCharacter*pl)	{
		if (pl)return pl->m_bLive&&
			(!((CPlayer*)pl)->m_bCorpse);
		else return false;
	}
	float		GetAngle(float mon[3],float plr[3]);
	void		CheckCondition();
	void		InsertAttackedPlayer(CGameObject* pOri);
	bool		MoveLater( float later);
	DWORD		m_StunTime;
	//*/	MAYUN

	static int		s_nLiveNum;
	static DWORD    s_dwSerialCnt;
	static CNetIndexList s_listEmpty;

public:
	CMonster();
	virtual ~CMonster();

	bool Init(_object_id* pID);
	bool Create(_monster_create_setdata* pData);
	bool Destroy(BYTE byDestroyCode, CGameObject* pAttObj);

	virtual void Loop();

	void LootingItem(CPlayer* pOwner);	

public:

//Send Message Load..
	void SendMsg_Create();
	void SendMsg_Destroy(BYTE byDestroyCode);
	void SendMsg_Move();	//이동명령을 받으면 주위의 Player에게 Move Message를 Send
	void SendMsg_Attack_Gen(CCharacter* pTarget, int nDamage, int nAttPart, BOOL bCritical);
	void SendMsg_Attack_Force(CAttack* pAtt);
	virtual void SendMsg_FixPosition(int n);	//움직이지 않을때 주위의 Player에게 실시간으로 Fix Position Message를 Send
	virtual void SendMsg_RealMovePoint(int n);	//특정인한테..

//Get Paramter..
	DWORD	GetEmotionState();
	DWORD	GetConditionState();
	DWORD	GetActionState();
	DWORD	GetScale(){	return m_Scale; }
	
	float	GetMoveSpeed();
	int		AttackObject(int nDmg , CGameObject* pOri/* = NULL*/);
	CCharacter* GetAttackTarget()	{	return m_pPlayerTarget;	}

	virtual int GetHP();
	virtual void SetHP(int nHP, bool bOver);
	virtual int	GetMaxHP( void );
	virtual int SetDamage(int nDam, CCharacter* pDst, int nDstLv, bool bCrt);
	virtual float GetWidth();
	virtual float GetAttackRange();
	virtual	int GetDefFC(int nAttactPart, CCharacter* pAttChar);
	virtual int GetLevel() {	
		return ((_monster_fld*)m_pRecordSet)->m_fLevel;
	}
	virtual int GetDefSkill(){
		return ((_monster_fld*)m_pRecordSet)->m_fDefSklUnit;
	}

	virtual int GetFireTol() { return ((_monster_fld*)m_pRecordSet)->m_fFireTol;}
	virtual int	GetWaterTol(){ return ((_monster_fld*)m_pRecordSet)->m_fWaterTol; }
	virtual int	GetSoilTol() { return ((_monster_fld*)m_pRecordSet)->m_fSoilTol; }
	virtual int	GetWindTol() { return ((_monster_fld*)m_pRecordSet)->m_fWindTol; }
	virtual float GetDefGap() { return ((_monster_fld*)m_pRecordSet)->m_fDefGap; }
	virtual float GetDefFacing() { return ((_monster_fld*)m_pRecordSet)->m_fDefFacing; }
	virtual float GetWeaponAdjust() { return ((_monster_fld*)m_pRecordSet)->m_fDefGap; }
	virtual int	AttackableHeight() {	return attack_able_height;	}	//공격가능높이..재정의 요망..
	virtual int GetGenAttackProb(CCharacter* pDst, int nPart) {	return (int)GetAttackProb(); }	//재정의 요망..
	virtual bool IsBeAttackedAble()	{	return true;	}
	virtual bool IsRewardExp()	{	return true;	}
	virtual	int GetObjRace()	{	return obj_race_monster;		}

	static DWORD GetNewMonSerial(){
		return s_dwSerialCnt++;
	}
};

//create monster under respawn system..
CMonster* CreateRespawnMonster(CMapData* pMap, WORD wLayer, int nMonsterIndex, _mon_active* pActiveRec, _dummy_position* pDumPosition, CBattleDungeon* pDungeon);
//create monster free from respawn system..
bool CreateRepMonster(CMapData* pMap, WORD wLayer, float* fPos, char* pszMonsterCode);

#endif
