
#ifndef __SFDATAEFFECT_H__
#define __SFDATAEFFECT_H__

class CCharacter;

#include "PlayerDB.h"
#include "BaseFld.h"

#define temp_effect_num	22

/*---------------------------------------------------------------------
 [ SFDataEffectFunc ]

 - parameter info
  1. 기술을 적용할 때
    pActChar	  : 기술을 사용한 player                
    pTargetChar      : 시전 대상 character	
	fEffectValue  : 기술에 적용되는 effect 값
	bApply        : TRUE

  2. 기술을 해제할 때 ( bApply가 false일 때 )
    pActChar	  : NULL
    pTargetChar	  : 시전 대상 character	
	fEffectValue  : 기술에 적용되는 effect 값
	bApply        : FALSE

Return Values:

	수행후 마스터리를 증가해야하는 경우 true 	

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
