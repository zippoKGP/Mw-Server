
#include "stdafx.h"
#include "SFDataEffect.h"
#include "Player.h"
#include "ForceFld.h"
#include "SkillFld.h"
#include "MainThread.h"
#include "MethodParameter.h"
#include "WeaponItemFld.h"

SFTempEffectFunc	g_TempEffectFunc[temp_effect_num] = 
{
	DE_AttHPtoDstFP,				//#define 0 �ڽ��� HP�� ����� FP�� ����� 
	DE_ContDamageTimeInc,			//#define 1 ���� ���غ��� ���� �ð� ���̱�
	DE_Recovery,					//#define 2 ���� ������ ����Ѵ�. 
	DE_HPInc,						//#define 3 ���� HP���� 
	DE_STInc,						//#define 4 ���� ST���� 
	DE_ContHelpTimeInc,				//#define 5 ���Ӻ����ð� ���� 
	DE_OverHealing,					//#define 6 overhealing..   MaxHP�� 200%��ŭ�� �����ϵ��� �Ѵ�. 
	DE_LateContHelpSkillRemove,		//#define 7 ���� �ֱ� �����̵溸�� ��ų ����	 
	DE_LateContHelpForceRemove,		//#define 8 ���� �ֱ� �����̵溸�� ���� ���� 
	DE_LateContDamageForceRemove,	//#define 9 ���� �ֱ� �������غ��� ���� ���� 
	DE_AllContHelpSkillRemove,		//#define 10 ��� �ɷ��ִ� �����̵溸�� ��ų ���� 
	DE_AllContHelpForceRemove,		//#define 11 ��� �ɷ��ִ� �����̵溸�� ���� ���� 
	DE_AllContDamageForceRemove,	//#define 12 ��� �ɷ��ִ� ���� ���� ���� ���� ���� 
	DE_OthersContHelpSFRemove,		//#define 13 ������ ���� �ٸ� �������� ���� �̵� ���� ��ų & ���� ���� 
	DE_SkillContHelpTimeInc,		//#define 14 skill�� ���� �������� �̵� ���� �ð� ���̱�
	DE_ConvertMonsterTarget,		//#define 15 �ݰ� 30 �̳� ������ Ÿ���� �����ڷ� �ٲ۴�.
	DE_TransMonsterHP,				//#define 16 �ݰ� 30 �̳� ��(����, Ÿ����)�� HP�� �������� HP�� �ٲ۴�.
	DE_ViewWeakPoint,				//#define 17 ����� ���� ������ ã�´�.
	DE_ReleaseMonsterTarget,		//#define 18 �ݰ� 30 �̳� ���� Ÿ������ �����Ѵ�.
	DE_MakeGuardTower,				//#define 19 ����Ÿ�� ��ġ ��ų
	DE_LayTrap,						//#define 20 Ʈ�� ��ġ
	DE_DetectTrap					//#define 21 �ֺ��� ������ �ʴ� Ʈ���� ����Ʈ �Ѵ�.
};

bool	DE_AttHPtoDstFP( __sf_func_p )
{	// �ڽ��� HP�� ����� FP�� �����
	return pTargetChar->F_AttHPtoDstFP_Once(pTargetChar);
}

bool	DE_ContDamageTimeInc( __sf_func_p )
{	// ���� ���غ��� ���� �ð� ���̱�
	return pTargetChar->F_ContDamageTimeInc_Once(pTargetChar, fEffectValue);	
}

bool	DE_Recovery( __sf_func_p )
{	// ���� ������ ����Ѵ�.
	return pTargetChar->F_Resurrect_Once(pTargetChar);
}

bool	DE_HPInc( __sf_func_p )
{	// ���� HP����
	return pTargetChar->F_HPInc_Once(pTargetChar, fEffectValue);
}

bool	DE_STInc( __sf_func_p )
{	// ���� ST����
	return pTargetChar->F_STInc_Once(pTargetChar, fEffectValue);
}

bool	DE_ContHelpTimeInc( __sf_func_p )
{	//���Ӻ����ð� ����
	return pTargetChar->F_ContHelpTimeInc_Once(pTargetChar, fEffectValue);
}

bool	DE_OverHealing( __sf_func_p )
{	// overhealing..   MaxHP�� 200%��ŭ�� �����ϵ��� �Ѵ�.
	return pTargetChar->F_OverHealing_Once(pTargetChar, fEffectValue);	
}

bool	DE_LateContHelpSkillRemove( __sf_func_p )
{	// ���� �ֱ� �����̵溸�� ��ų ����	
	return pTargetChar->F_LateContHelpSkillRemove_Once(pTargetChar);
}

bool	DE_LateContHelpForceRemove( __sf_func_p )
{	// ���� �ֱ� �����̵溸�� ���� ����
	return pTargetChar->F_LateContHelpForceRemove_Once(pTargetChar);	
}

bool	DE_LateContDamageForceRemove( __sf_func_p )
{	// ���� �ֱ� �������غ��� ���� ����
	return pTargetChar->F_LateContDamageForceRemove_Once(pTargetChar);	
}

bool	DE_AllContHelpSkillRemove( __sf_func_p )
{	// ��� �ɷ��ִ� �����̵溸�� ��ų ����
	return pTargetChar->F_AllContHelpSkillRemove_Once(pTargetChar);	
}

bool	DE_AllContHelpForceRemove( __sf_func_p )
{	// ��� �ɷ��ִ� �����̵溸�� ���� ����
	return pTargetChar->F_AllContHelpForceRemove_Once(pTargetChar);
}

bool	DE_AllContDamageForceRemove( __sf_func_p )
{	// ��� �ɷ��ִ� ���� ���� ���� ���� ����
	return pTargetChar->F_AllContDamageForceRemove_Once(pTargetChar);
}

bool	DE_OthersContHelpSFRemove( __sf_func_p )
{	// ������ ���� �ٸ� �������� ���� �̵� ���� ��ų & ���� ����
	return pTargetChar->F_OthersContHelpSFRemove_Once(fEffectValue);
}

bool	DE_SkillContHelpTimeInc( __sf_func_p )
{	// skill�� ���� �������� �̵� ���� �ð� ���̱�
	return pTargetChar->S_SkillContHelpTimeInc_Once(pTargetChar, fEffectValue);
}

bool	DE_ConvertMonsterTarget( __sf_func_p )
{	//�ݰ� 30 �̳� ������ Ÿ���� �����ڷ� �ٲ۴�.
	return pTargetChar->S_ConvertMonsterTarget(pTargetChar, fEffectValue);
}

bool	DE_TransMonsterHP( __sf_func_p )
{	//�ݰ� 30 �̳� ��(����, Ÿ����)�� HP�� �������� HP�� �ٲ۴�.
	return pTargetChar->S_TransMonsterHP(pTargetChar, fEffectValue);
}

bool	DE_ViewWeakPoint( __sf_func_p )
{	//����� ���� ������ ã�´�.
	return true;	//Ŭ���̾�Ʈ���� ó��..
}

bool	DE_ReleaseMonsterTarget( __sf_func_p )
{	//�ݰ� 30 �̳� ���� Ÿ������ �����Ѵ�.
	return pTargetChar->S_ReleaseMonsterTarget(pTargetChar, fEffectValue);
}

bool	DE_MakeGuardTower( __sf_func_p )
{	//����Ÿ�� ��ġ ��ų
	return true;	//����ó��..
}

bool	DE_LayTrap( __sf_func_p )
{	//Ʈ�� ��ġ
	return true;	//���ó��..	
}

bool	DE_DetectTrap( __sf_func_p )
{	//�ֺ��� ������ �ʴ� Ʈ���� ����Ʈ �Ѵ�.
	return true;	//Ŭ���̾�Ʈ���� ó��..
}

