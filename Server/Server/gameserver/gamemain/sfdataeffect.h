
#ifndef __SFDATAEFFECT_H__
#define __SFDATAEFFECT_H__

class CCharacter;

#include "PlayerDB.h"
#include "BaseFld.h"

#define temp_effect_num	22

/*---------------------------------------------------------------------
 [ SFDataEffectFunc ]

 - parameter info
  1. ����� ������ ��
    pActChar	  : ����� ����� player                
    pTargetChar      : ���� ��� character	
	fEffectValue  : ����� ����Ǵ� effect ��
	bApply        : TRUE

  2. ����� ������ �� ( bApply�� false�� �� )
    pActChar	  : NULL
    pTargetChar	  : ���� ��� character	
	fEffectValue  : ����� ����Ǵ� effect ��
	bApply        : FALSE

Return Values:

	������ �����͸��� �����ؾ��ϴ� ��� true 	

---------------------------------------------------------------------*/

#define __sf_func_p		CCharacter* pActChar, CCharacter* pTargetChar, float fEffectValue

typedef	bool	(*SFTempEffectFunc)	( __sf_func_p );	

extern SFTempEffectFunc	g_TempEffectFunc[temp_effect_num];

//////////////////////////////////////////////////////////////////
//
//		temp effect function
//
//////////////////////////////////////////////////////////////////

bool	DE_AttHPtoDstFP( __sf_func_p );
bool	DE_ContDamageTimeInc( __sf_func_p );
bool	DE_Recovery( __sf_func_p );
bool	DE_HPInc( __sf_func_p );
bool	DE_STInc( __sf_func_p );
bool	DE_ContHelpTimeInc( __sf_func_p );
bool	DE_OverHealing( __sf_func_p );
bool	DE_LateContHelpSkillRemove( __sf_func_p );
bool	DE_LateContHelpForceRemove( __sf_func_p );
bool	DE_LateContDamageForceRemove( __sf_func_p );
bool	DE_AllContHelpSkillRemove( __sf_func_p );
bool	DE_AllContHelpForceRemove( __sf_func_p );
bool	DE_AllContDamageForceRemove( __sf_func_p );
bool	DE_OthersContHelpSFRemove( __sf_func_p );
bool	DE_SkillContHelpTimeInc( __sf_func_p );
bool	DE_ConvertMonsterTarget( __sf_func_p );
bool	DE_TransMonsterHP( __sf_func_p );
bool	DE_ViewWeakPoint( __sf_func_p );
bool	DE_ReleaseMonsterTarget( __sf_func_p );
bool	DE_MakeGuardTower( __sf_func_p );
bool	DE_LayTrap( __sf_func_p );
bool	DE_DetectTrap( __sf_func_p );

#endif // __SFDATAEFFECT_H__
