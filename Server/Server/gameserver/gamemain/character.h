// Character.h: interface for the CCharacter class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _Character_h_
#define _Character_h_

#include "GameObject.h"
#include "pt_zone_client.h"
#include "SFDataEffect.h"
#include "MethodParameter.h"
#include "ForceFld.h"
#include "SkillFld.h"

struct _sf_continous
{
	bool				m_bExist;
	BYTE				m_byEffectCode;	// skill or force or class?
	WORD				m_wEffectIndex;	// skill(or force) Index
	BYTE				m_byLv;
	WORD				m_wStartSec;	//sf 발동 시간(초)
	WORD				m_wDurSec;	// 지속시간(초)

	_sf_continous()
	{
		m_bExist = false;
	}

	static WORD GetSFContCurTime()
	{
		return (timeGetTime()/1000)%10000;
	}
};

struct _effect_parameter
{
	float		m_fEff_Rate[_EFF_RATE::NUM];
	short		m_zEff_Plus[_EFF_PLUS::NUM];
	bool		m_bEff_State[_EFF_STATE::NUM];
	float		m_fEff_Have[_EFF_HAVE::NUM];

	bool		m_bEffectable;

	_effect_parameter()
	{
		Init();
	}

	void Init()
	{
		int i;
		for(i = 0; i < _EFF_RATE::NUM; i++)
			m_fEff_Rate[i] = 1.0f;
		for(i = 0; i < _EFF_PLUS::NUM; i++)
			m_zEff_Plus[i] = 0;
		for(i = 0; i < _EFF_STATE::NUM; i++)
			m_bEff_State[i] = false;
		for(i = 0; i < _EFF_HAVE::NUM; i++)
			m_fEff_Have[i] = 0.0f;	
		m_bEffectable = true;
	}

	void SetEff_Rate(int nParamIndex, float fVar, bool bAdd)
	{
		if(bAdd)
			m_fEff_Rate[nParamIndex] += fVar;
		else
			m_fEff_Rate[nParamIndex] -= fVar;
	}

	void SetEff_Plus(int nParamIndex, int nVar, bool bAdd)
	{
		if(bAdd)
			m_zEff_Plus[nParamIndex] += nVar;
		else
			m_zEff_Plus[nParamIndex] -= nVar;
	}

	void SetEff_State(int nParamIndex, bool bVar)
	{
		m_bEff_State[nParamIndex] = bVar;
	}

	float GetEff_Rate(int nParamIndex)
	{
		if(!m_bEffectable)
			return 1.0f;
		return m_fEff_Rate[nParamIndex];
	}

	int GetEff_Plus(int nParamIndex)
	{
		if(!m_bEffectable)
			return 0;
		return m_zEff_Plus[nParamIndex];
	}

	bool GetEff_State(int nParamIndex)
	{
		if(!m_bEffectable)
			return false;
		return m_bEff_State[nParamIndex];
	}

	float GetEff_Have(int nParamIndex)
	{
		if(!m_bEffectable)
			return false;
		return m_fEff_Have[nParamIndex];
	}

	void SetEffectable(bool bEffect)
	{
		m_bEffectable = bEffect;
	}
};

struct _character_create_setdata : public _object_create_setdata
{
};

class CCharacter : public CGameObject
{			
public:	
	
	enum {	slot_max = 5	};	
			
	float		m_fTarPos[3];	

	///*MAYUN
	int			m_AroundNum;
	CCharacter*	m_AroundSlot[slot_max];// 몬스터가 얼마나 몰려 있는가?
	//*/
	DWORD		m_dwNextAttackTime;

	// -------------------------------------------------------------------
	// 지속 보조 리스트
	_sf_continous	m_SFCont[SF_CONT_CODE_NUM][CONT_SF_SIZE];	
	WORD			m_wLastContEffect;
	WORD			m_wLastContEffect_Old;
	_sf_continous*  m_pLastContEffect;

	_effect_parameter	m_EP;

	//치트관련변수.
	bool		m_bObserver;	//옵저버모드
	int			m_nContEffectSec;	//지속효과의 시간세팅

private:
	
	CMyTimer		m_tmrSFCont;	//SF 검사 타이머
			
public:
	CCharacter();
	virtual ~CCharacter();

	void Init(_object_id* pID);
	bool Create(_character_create_setdata* pData);
	bool Destroy();

	///*MAYUN
	int GetSlot(CCharacter *p);
	int GetNearEmptySlot(int pos, float dist, Vector3f cur);
	int InsertSlot(CCharacter *p, int pos);
	int RemoveSlot(CCharacter *p);
	void ResetSlot();
	//*/

	//SF Effect..
	bool AssistForce(CCharacter* pDstChar, _force_fld* pForceFld, int nForceLv, BYTE* OUT pbyErrorCode, bool* OUT pbUpMty);
	bool AssistSkill(CCharacter* pDstChar, int nEffectCode, _skill_fld* pSkillFld, int nSkillLv, BYTE* OUT pbyErrorCode, bool* OUT pbUpMty);

	// -------------------------------------------------------------------
	// 지속 보조 큐
	// 새로운 skill force가 적용되었을 때 queue에 추가시킨다.
	void SFContInit();
	BYTE InsertSFContEffect(BYTE byContCode, BYTE byEffectCode, DWORD dwEffectIndex, WORD wDurSec, BYTE byLv, bool* OUT pbUpMty);						
	void RemoveSFContEffect(BYTE byContCode, WORD wListIndex, bool bInit = false);
	void RemoveSFContHelpByEffect(int nContParamCode, int nContParamIndex);       
	void UpdateSFCont();
	WORD CalcEffectBit(WORD wEffectCode, WORD wEffectIndex)	{		return (wEffectCode << 12) | wEffectIndex;		}

	int	 GetAttackDamPoint(int nAttPnt, int nAttPart, int nTolType, CCharacter* pDst);
	void Move(float fSpeed);	
	bool SetTarPos(float* fTarPos, bool bColl = true);	//새로운 target point를 setting한다. 
	void Stop();
	void Go();

	int GetTotalTol(BYTE byAttTolType, int nDamPoint);
	int GetAttackRandomPart();
	void SetAttackDelay();
	DWORD GetNextAttTime() { return m_dwNextAttackTime; }
	void SetNextAttTime(DWORD dwNextTime){	m_dwNextAttackTime = dwNextTime;	}

	bool GetStun() { return m_bStun; }
	bool GetStealth();

	//effect..send..
	void SendMsg_AddEffect(WORD wEffectCode, BYTE byLv);
	void SendMsg_DelEffect(BYTE byEffectCode, WORD wEffectIndex, BYTE byLv);
	void SendMsg_StunInform();
	void SendMsg_LastEffectChangeInform();
};

#endif 
