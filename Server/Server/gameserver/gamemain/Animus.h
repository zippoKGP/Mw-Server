#ifndef	_Animus_h_
#define _Animus_h_

#include "Character.h"
#include "AnimusFld.h"
#include "AnimusItemFld.h"
#include "MonsterAI.h"

struct _animus_create_setdata : public _character_create_setdata
{
	int			nHP;
	int			nFP;
	DWORD		dwExp;
	CPlayer*	pMaster;

	_animus_create_setdata()
	{
		pMaster = NULL;
	}
};  

class CPlayer;

class CAnimus : public CCharacter
{
public:

	BYTE		m_byClassCode;	//소환수 종류
	int			m_nHP;		
	int			m_nFP;
	DWORD		m_dwExp;
	CPlayer*	m_pMaster;

	DWORD		m_dwLastDestroyTime;

	static int		s_nLiveNum;
	static DWORD    s_dwSerialCnt;

	static 	CRecordData	s_tblParameter[animus_storage_num];

public:

	CAnimus();
	virtual ~CAnimus();

	bool Init(_object_id* pID);
	bool Create(_animus_create_setdata* pData);
	bool Destroy();

	virtual void Loop();

	void ChangeMode_MasterCommand(int nMode);				//mode변경 명령.. 명령을 받거나 자동으로 mode가 바뀌면 AlterMode_MasterReport()를 호출함..
	void ChangeTarget_MasterCommand(CCharacter* pTarget);	//target변경 명령

	void MasterAttack_MasterInform(CCharacter* pDst);		//master가 어떤 대상을 공격함을 알림
	void MasterBeAttacked_MasterInform(CCharacter* pDst);	//master가 어떤 대상에게 공격당함을 알림]

	void AlterHP_MasterReport();		//HP가 바뀌면 CALL
	void AlterFP_MasterReport();		//FP가 바뀌면 CALL
	void AlterExp_MasterReport(int nAlterExp);	//Exp가 바뀌면 CALL (증가또는 감소량)
	void AlterMode_MasterReport(BYTE byMode);	//AI적으로 자동으로모드가 바뀔때 호출
	void Return_MasterRequest();

	virtual int GetHP();
	virtual int	GetMaxHP( void );
	virtual int SetDamage(int nDam, CCharacter* pDst, int nDstLv, bool bCrt);
	virtual float GetWidth();
	virtual float GetAttackRange();
	virtual	int GetDefFC(int nAttactPart, CCharacter* pAttChar);
	virtual int GetLevel();
	virtual int GetDefSkill();

	virtual int GetFireTol();
	virtual int	GetWaterTol();
	virtual int	GetSoilTol();
	virtual int	GetWindTol();
	virtual float GetDefGap();
	virtual float GetDefFacing();
	virtual float GetWeaponAdjust();
	virtual int	AttackableHeight();
	virtual int GetGenAttackProb(CCharacter* pDst, int nPart);
	virtual bool IsBeAttackedAble()	{	return true;	}

	virtual	int GetObjRace();//		{	return m_pMaster->GetObjRace();		}

	static DWORD GetNewMonSerial()	{		return s_dwSerialCnt++;	}
	static bool SetStaticMember();

	void SendMsg_Create();
	void SendMsg_Destroy();
	void SendMsg_Move();	
	void SendMsg_Attack_Gen(CCharacter* pTarget, int nDamage, int nAttPart, BOOL bCritical);
	virtual void SendMsg_FixPosition(int n);	
	virtual void SendMsg_RealMovePoint(int n);	
	void SendMsg_LevelUp();
	
	//MAYUN
#define		MAX_ANI_SKILL	2
enum	{  ani_mode_action = 0, ani_mode_follow = 1 };
enum	{  DEFUP , DEFDOWN ,DEFHAND ,DEFFOOT, DEFHEAD };
enum	{  max_part = 5};
public:

	_animus_fld*m_pRecord;
	int			m_nMaxHP;
	int			m_nMaxFP;

	float		m_Mightiness;
	int			m_DefPart[max_part];//방어력 
	
	DWORD		m_dwAIMode;	// 현재의 소환수 AI 모드 // Action  = 0 ,Follow Me = 1
	CCharacter*	m_pTarget;  // 
	Vector3f	m_BefTarget;
	
	//DWORD		m_dwActionDelay;
#define			MAX_AI_TIMER	3
	enum	{	AIT_ACTION,	AIT_MOVE, AIT_LIFE };
	CAITimer	m_AITimer[MAX_AI_TIMER];
	CAIPathFinder	m_Path;	

public:
	CCharacter* SearchNearPlayerAttack();//적을 찾는다. 플레이어를 공격한 적이 있는 넘들이다!
	CCharacter* SearchNearEnemy();//자신을 공격하는 적!


	float		GetAttackProb();
	int			GetAttackPart();
	void		GetTarget();
	void		Action();// 각 행동 
	void		GetMoveTarget( CCharacter *target );
	void		ActionMove();
	
	void		Attack( DWORD skill);
	SKILL		m_Skill[MAX_ANI_SKILL];//사용가능 스킬 
	void		Process();
	void		AIInit();
	void		SendMove();
	void		ChangeMode( DWORD mode );
	void		LifeTimeCheck();
	//
};

CAnimus* FindEmptyAnimus(CAnimus* pObjArray, int nMax);
bool CreateAnimus(CMapData* pMap, WORD wLayer, float* fPos, BYTE byClass, int nHP, int nFP, DWORD dwExp, CPlayer* pMaster);
_animus_fld* GetAnimusFldFromExp(int nAnimusClass, DWORD dwExp);
DWORD GetMaxParamFromExp(int nAnimusClass, DWORD dwExp);

#endif