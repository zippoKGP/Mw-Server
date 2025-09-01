
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
	DE_AttHPtoDstFP,				//#define 0 자신의 HP를 대상의 FP로 만든다 
	DE_ContDamageTimeInc,			//#define 1 지속 피해보조 남은 시간 늘이기
	DE_Recovery,					//#define 2 죽은 유저를 재생한다. 
	DE_HPInc,						//#define 3 현재 HP증가 
	DE_STInc,						//#define 4 현재 ST증가 
	DE_ContHelpTimeInc,				//#define 5 지속보조시간 증가 
	DE_OverHealing,					//#define 6 overhealing..   MaxHP의 200%만큼만 가능하도록 한다. 
	DE_LateContHelpSkillRemove,		//#define 7 가장 최근 지속이득보조 스킬 해제	 
	DE_LateContHelpForceRemove,		//#define 8 가장 최근 지속이득보조 포스 해제 
	DE_LateContDamageForceRemove,	//#define 9 가장 최근 지속피해보조 포스 해제 
	DE_AllContHelpSkillRemove,		//#define 10 대상에 걸려있는 지속이득보조 스킬 제거 
	DE_AllContHelpForceRemove,		//#define 11 대상에 걸려있는 지속이득보조 포스 제거 
	DE_AllContDamageForceRemove,	//#define 12 대상에 걸려있는 지속 피해 보조 포스 해제 
	DE_OthersContHelpSFRemove,		//#define 13 시전자 주위 다른 종족들의 지속 이득 보조 스킬 & 포스 해제 
	DE_SkillContHelpTimeInc,		//#define 14 skill에 의한 지속적인 이득 남은 시간 늘이기
	DE_ConvertMonsterTarget,		//#define 15 반경 30 이내 몬스터의 타겟을 시전자로 바꾼다.
	DE_TransMonsterHP,				//#define 16 반경 30 이내 적(몬스터, 타종족)의 HP를 시전자의 HP로 바꾼다.
	DE_ViewWeakPoint,				//#define 17 대상의 약한 부위를 찾는다.
	DE_ReleaseMonsterTarget,		//#define 18 반경 30 이내 적의 타게팅을 해제한다.
	DE_MakeGuardTower,				//#define 19 가드타워 설치 스킬
	DE_LayTrap,						//#define 20 트랩 설치
	DE_DetectTrap					//#define 21 주변의 보이지 않는 트랩을 디텍트 한다.
};

bool	DE_AttHPtoDstFP( __sf_func_p )
{	// 자신의 HP를 대상의 FP로 만든다
	return pTargetChar->F_AttHPtoDstFP_Once(pTargetChar);
}

bool	DE_ContDamageTimeInc( __sf_func_p )
{	// 지속 피해보조 남은 시간 늘이기
	return pTargetChar->F_ContDamageTimeInc_Once(pTargetChar, fEffectValue);	
}

bool	DE_Recovery( __sf_func_p )
{	// 죽은 유저를 재생한다.
	return pTargetChar->F_Resurrect_Once(pTargetChar);
}

bool	DE_HPInc( __sf_func_p )
{	// 현재 HP증가
	return pTargetChar->F_HPInc_Once(pTargetChar, fEffectValue);
}

bool	DE_STInc( __sf_func_p )
{	// 현재 ST증가
	return pTargetChar->F_STInc_Once(pTargetChar, fEffectValue);
}

bool	DE_ContHelpTimeInc( __sf_func_p )
{	//지속보조시간 증가
	return pTargetChar->F_ContHelpTimeInc_Once(pTargetChar, fEffectValue);
}

bool	DE_OverHealing( __sf_func_p )
{	// overhealing..   MaxHP의 200%만큼만 가능하도록 한다.
	return pTargetChar->F_OverHealing_Once(pTargetChar, fEffectValue);	
}

bool	DE_LateContHelpSkillRemove( __sf_func_p )
{	// 가장 최근 지속이득보조 스킬 해제	
	return pTargetChar->F_LateContHelpSkillRemove_Once(pTargetChar);
}

bool	DE_LateContHelpForceRemove( __sf_func_p )
{	// 가장 최근 지속이득보조 포스 해제
	return pTargetChar->F_LateContHelpForceRemove_Once(pTargetChar);	
}

bool	DE_LateContDamageForceRemove( __sf_func_p )
{	// 가장 최근 지속피해보조 포스 해제
	return pTargetChar->F_LateContDamageForceRemove_Once(pTargetChar);	
}

bool	DE_AllContHelpSkillRemove( __sf_func_p )
{	// 대상에 걸려있는 지속이득보조 스킬 제거
	return pTargetChar->F_AllContHelpSkillRemove_Once(pTargetChar);	
}

bool	DE_AllContHelpForceRemove( __sf_func_p )
{	// 대상에 걸려있는 지속이득보조 포스 제거
	return pTargetChar->F_AllContHelpForceRemove_Once(pTargetChar);
}

bool	DE_AllContDamageForceRemove( __sf_func_p )
{	// 대상에 걸려있는 지속 피해 보조 포스 해제
	return pTargetChar->F_AllContDamageForceRemove_Once(pTargetChar);
}

bool	DE_OthersContHelpSFRemove( __sf_func_p )
{	// 시전자 주위 다른 종족들의 지속 이득 보조 스킬 & 포스 해제
	return pTargetChar->F_OthersContHelpSFRemove_Once(fEffectValue);
}

bool	DE_SkillContHelpTimeInc( __sf_func_p )
{	// skill에 의한 지속적인 이득 남은 시간 늘이기
	return pTargetChar->S_SkillContHelpTimeInc_Once(pTargetChar, fEffectValue);
}

bool	DE_ConvertMonsterTarget( __sf_func_p )
{	//반경 30 이내 몬스터의 타겟을 시전자로 바꾼다.
	return pTargetChar->S_ConvertMonsterTarget(pTargetChar, fEffectValue);
}

bool	DE_TransMonsterHP( __sf_func_p )
{	//반경 30 이내 적(몬스터, 타종족)의 HP를 시전자의 HP로 바꾼다.
	return pTargetChar->S_TransMonsterHP(pTargetChar, fEffectValue);
}

bool	DE_ViewWeakPoint( __sf_func_p )
{	//대상의 약한 부위를 찾는다.
	return true;	//클라이언트에서 처리..
}

bool	DE_ReleaseMonsterTarget( __sf_func_p )
{	//반경 30 이내 적의 타게팅을 해제한다.
	return pTargetChar->S_ReleaseMonsterTarget(pTargetChar, fEffectValue);
}

bool	DE_MakeGuardTower( __sf_func_p )
{	//가드타워 설치 스킬
	return true;	//따로처리..
}

bool	DE_LayTrap( __sf_func_p )
{	//트랩 설치
	return true;	//띠로처리..	
}

bool	DE_DetectTrap( __sf_func_p )
{	//주변의 보이지 않는 트랩을 디텍트 한다.
	return true;	//클라이언트에서 처리..
}

