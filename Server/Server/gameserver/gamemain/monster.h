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
	CBattleDungeon*		pBattleDungeon;	//battle dungeon���� ���� ���Ͷ�� ����..

	_monster_create_setdata(){
		pActiveRec = NULL;
		pDumPosition = NULL;
		pBattleDungeon = NULL;	//battle dungeon���� ���� ���Ͷ�� ����..
	}
};  

class CPlayer;
struct _mon_active;

class CMonster : public CCharacter
{
public:
	_mon_active*		m_pActiveRec;	//������ dummy�� pointer
	_dummy_position*	m_pDumPosition;
	CBattleDungeon*		m_pBattleDungeon;	//battle dungeon���� ���� ���Ͷ�� ����..

	int m_nHP;
	DWORD m_dwLastDestroyTime;
	bool  m_bTurn;

	///*	MAYUN
	enum	{  max_part = 5};
	enum	{  DEFUP , DEFDOWN ,DEFHAND ,DEFFOOT, DEFHEAD };
	enum	{  max_path = 16};
	enum	{  max_attack_player = 3};
	enum	{  max_area_attack_object = 30	};
	//AI ���� ������ 
	float		m_AroundX,m_AroundY;
	MONSTERAI	m_MonAI;
	CCharacter	*m_pPlayerTarget;//Ÿ�� �÷��̾�
	CMonster	*m_pMonTarget;//Ÿ�� ���� 
	CCharacter	*m_pSearchPlayer;//���� ã�� ĳ����, ���� Ÿ�پƴ�!
	CCharacter	*m_pAssistPlayer;//���ݿ�û�� ���� Ÿ�� �÷��̾� 
	CMonster	*m_pSearchMon;//���� ã�� ����, ���� Ÿ�پƴ�!
	CMonster	*m_pHelpMonster;//���� ������ ������ ��û�� �ߴ°�?
	float		m_MoveTarget[3];//���ؾ��� �� ��ǥ//������ �̵� Ÿ���� �ƴ�!
	float		m_PlayerPos[max_path][3];//�÷��̾��� Ÿ���� ��ġ ��ǥ ���  
	//int			m_CurNode;
	DWORD		m_CurNode;
	DWORD		m_EndNode;
	
	BOOL		m_bNode;//��尡 �߰��Ǿ��°�?
	DWORD		m_CurrentSkill;//���� ������� ��ų
	DWORD		m_SearchSkill;//���� ���� �� ��ų 
	BOOL		m_RunAway;//���� �����ΰ�?
	
	
	//�ɷ�ġ 
	DWORD		m_LifeMax;
	DWORD		m_LifeCicle;
	float		m_Scale;
	float		m_Mightiness;
	DWORD		m_ChaseDist;
	DWORD		m_MaxSkill;//�ִ� ��ų 
	SKILL		m_Skill[MAX_SKILL];//��밡�� ��ų 
	int			m_DefPart[max_part];//���� 

	CCharacter	*m_AttackedPlayer[max_attack_player];
	CLootingMgr m_LootMgr;

	//// AI �Լ��� 
	void		InitAI();
	CCharacter* SearchNearPlayer();//������ ������ player�� ã�´�
	CCharacter* SearchNearMonster();////������ ������ Monster�� ã�´�
	
	void		GetTarPosChase();
	void		InitMonster();
	int			GetAttackPart();
	void		Attack();
	float		GetAttackProb();
	BOOL		IsFront(float *cur,float *tar,float x,float y);//���� ��ġ���� Ÿ�ٱ����� ��ġ�� �����ΰ� �����ΰ�?
	void		GetPlayerPos();//�÷��̾� ��ġ + ���� ��ġ 
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
	void SendMsg_Move();	//�̵������ ������ ������ Player���� Move Message�� Send
	void SendMsg_Attack_Gen(CCharacter* pTarget, int nDamage, int nAttPart, BOOL bCritical);
	void SendMsg_Attack_Force(CAttack* pAtt);
	virtual void SendMsg_FixPosition(int n);	//�������� ������ ������ Player���� �ǽð����� Fix Position Message�� Send
	virtual void SendMsg_RealMovePoint(int n);	//Ư��������..

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
	virtual int	AttackableHeight() {	return attack_able_height;	}	//���ݰ��ɳ���..������ ���..
	virtual int GetGenAttackProb(CCharacter* pDst, int nPart) {	return (int)GetAttackProb(); }	//������ ���..
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
