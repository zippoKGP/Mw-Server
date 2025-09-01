#ifndef _ATTACK_H_
#define _ATTACK_H_

#include "CharacterDB.h"
#include "BaseFld.h"
#include "c_vector.h"

class CCharacter;
class CPlayer;

// 시전구분
#define sf_tech_code_attack			0	// 공격
#define sf_tech_code_temp_help		1	// 일시적인 보조

#define sf_effect_code_one			0
#define sf_effect_code_double		1
#define sf_effect_code_combo		2
#define sf_effect_code_penalty		3
#define sf_effect_code_self			4
#define sf_effect_code_flash		5
#define sf_effect_code_extent		6
#define sf_effect_code_sector		7
#define sf_effect_code_support		8

// skill melee 기술
#define	melee_tech_none				0		// 없음
#define	melee_tech_cut				1		// 베기
#define	melee_tech_hit				2		// 때리기
#define	melee_tech_prick			3		// 찌르기
#define	melee_tech_velocity			4		// 속도
#define	melee_tech_power			5		// 힘
#define	melee_tech_accuracy 		6		// 정확도
#define	melee_tech_self	 			7		// self
#define	melee_tech_flash 			8		// flash
#define	melee_tech_perforate		9		// 1타 관통
#define	melee_tech_high_self		10		// 고급 self
#define	melee_tech_high_flash		11		// 고급 flash
#define	melee_tech_high_perforate	12		// 고급1타관통
#define MELEE_TECH_NUM				melee_tech_high_perforate+1	

#define max_damaged 30

struct _be_damaged_char 
{
	CCharacter*		m_pChar;
	int				m_nDamage;
};

//CAttack 책체생성시 먼저 초기화해서 인자로넘김
struct _attack_param
{
	CCharacter*				pDst;		// 공격 대상
	int						nPart;		// 공격 부위
	int						nTol;		// 공격 속성
	int						nClass;		// 무기 종류	
	int						nMinAF;		// 최소 공격
	int						nMaxAF;		// 최대 공격	
	int						nMinSel;	// 최소 선택확률
	int						nMaxSel;	// 최대 선택확률
	int						nWpRange;	// 무기 사정거리
	int						nShotNum;	// 발사수				//스킬.. (sector..)
	int						nAddAttPnt;	// 부가 공격포인트		//스킬.. (penalty..)	

	_base_fld*				pFld;		// 특수공격 레코드 포인터
	float					fArea[3];	// 지역공격시 좌표
	int						nLevel;		// 특수기술의 레벨	
	int						nMastery;	// 특수기술의 숙련도
	
	bool					bMatchless;	// 무적인지..

	_attack_param()	{	memset(this, 0, sizeof(*this));		}
};

//일반 및 포스공격. 모든 케릭터 사용가능
class CAttack
{
public:

	//공격공식등에 사용돼는 파라메터
	_attack_param*			m_pp;

	//공격자
	CCharacter*				m_pAttChar;

	//결과맴버
	bool					m_bIsCrtAtt;
	int						m_nDamagedObjNum;
	_be_damaged_char		m_DamList[max_damaged];
	
	static _attack_param	s_DefParam;
	static BYTE				s_bySkillIndexMT[MELEE_TECH_NUM];
	static CRecordData*		s_pSkillData;

public:

	CAttack(CCharacter* pThis);

	void	AttackGen(_attack_param* pParam);
	void	AttackForce(_attack_param* pParam);

	static  void	SetStaticMember(CRecordData* pSkillData);
	static	int		GetMeleeSkillIndex(int nMeleeTechCode);

protected:

	void	FlashDamageProc(int nSkillLv, int nAttPower, int nAngle);
	void	AreaDamageProc(int nSkillLv, int nAttPower, float* pTar);
	void	SectorDamageProc(int nSkillLv, int nAttPower, int nAngle, int nShotNum, int nWeaponRange);
	
//private:

	bool	_IsForceHit();
	int		_CalcGenAttPnt();
	int		_CalcForceAttPnt();
	BOOL	_IsCharInSector(Vector3f chkpos, Vector3f src, Vector3f dest, float angle, float radius);         // 부채꼴안에 있는가?
};

//스킬공격.. 플레이어만 쓸수있다
class CPlayerAttack : public CAttack
{
public:
	
	CPlayer*	m_pAttPlayer;
//	int			m_nEffectGroup;

public:

	CPlayerAttack(CCharacter* pThis) : CAttack(pThis)
	{
		m_pAttPlayer = (CPlayer*)pThis;
	}
	
	void	AttackSkill(_attack_param* pParam);
	void	AttackUnit(_attack_param* pParam);

private:

	bool	_IsSkillHit();
	int		_CalcSkillAttPnt();
};

#endif