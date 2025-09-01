#ifndef _ATTACK_H_
#define _ATTACK_H_

#include "CharacterDB.h"
#include "BaseFld.h"
#include "c_vector.h"

class CCharacter;
class CPlayer;

// ��������
#define sf_tech_code_attack			0	// ����
#define sf_tech_code_temp_help		1	// �Ͻ����� ����

#define sf_effect_code_one			0
#define sf_effect_code_double		1
#define sf_effect_code_combo		2
#define sf_effect_code_penalty		3
#define sf_effect_code_self			4
#define sf_effect_code_flash		5
#define sf_effect_code_extent		6
#define sf_effect_code_sector		7
#define sf_effect_code_support		8

// skill melee ���
#define	melee_tech_none				0		// ����
#define	melee_tech_cut				1		// ����
#define	melee_tech_hit				2		// ������
#define	melee_tech_prick			3		// ���
#define	melee_tech_velocity			4		// �ӵ�
#define	melee_tech_power			5		// ��
#define	melee_tech_accuracy 		6		// ��Ȯ��
#define	melee_tech_self	 			7		// self
#define	melee_tech_flash 			8		// flash
#define	melee_tech_perforate		9		// 1Ÿ ����
#define	melee_tech_high_self		10		// ��� self
#define	melee_tech_high_flash		11		// ��� flash
#define	melee_tech_high_perforate	12		// ���1Ÿ����
#define MELEE_TECH_NUM				melee_tech_high_perforate+1	

#define max_damaged 30

struct _be_damaged_char 
{
	CCharacter*		m_pChar;
	int				m_nDamage;
};

//CAttack åü������ ���� �ʱ�ȭ�ؼ� ���ڷγѱ�
struct _attack_param
{
	CCharacter*				pDst;		// ���� ���
	int						nPart;		// ���� ����
	int						nTol;		// ���� �Ӽ�
	int						nClass;		// ���� ����	
	int						nMinAF;		// �ּ� ����
	int						nMaxAF;		// �ִ� ����	
	int						nMinSel;	// �ּ� ����Ȯ��
	int						nMaxSel;	// �ִ� ����Ȯ��
	int						nWpRange;	// ���� �����Ÿ�
	int						nShotNum;	// �߻��				//��ų.. (sector..)
	int						nAddAttPnt;	// �ΰ� ��������Ʈ		//��ų.. (penalty..)	

	_base_fld*				pFld;		// Ư������ ���ڵ� ������
	float					fArea[3];	// �������ݽ� ��ǥ
	int						nLevel;		// Ư������� ����	
	int						nMastery;	// Ư������� ���õ�
	
	bool					bMatchless;	// ��������..

	_attack_param()	{	memset(this, 0, sizeof(*this));		}
};

//�Ϲ� �� ��������. ��� �ɸ��� ��밡��
class CAttack
{
public:

	//���ݰ��ĵ ���Ŵ� �Ķ����
	_attack_param*			m_pp;

	//������
	CCharacter*				m_pAttChar;

	//����ɹ�
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
	BOOL	_IsCharInSector(Vector3f chkpos, Vector3f src, Vector3f dest, float angle, float radius);         // ��ä�þȿ� �ִ°�?
};

//��ų����.. �÷��̾ �����ִ�
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