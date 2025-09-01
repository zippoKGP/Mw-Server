// MyNetWork_Attack.cpp: implementation of the CNetworkEX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyNetWorking.h"
#include "Player.h"
#include "MyUtil.h"
#include "protocol.h"
#include "pt_zone_client.h"
#include "MainThread.h"

bool CNetworkEX::AttackPersonalRequest(int n, char* pBuf)
{
	_attack_gen_request_clzo* pRecv = (_attack_gen_request_clzo*)pBuf;
	
	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	CCharacter* pDst = (CCharacter*)g_Main.GetObject(obj_kind_char, pRecv->byID, pRecv->wIndex);
	if(!pDst)
		return false;

	if(pRecv->byAttPart >= base_fix_num)
		return false;

	pOne->pc_PlayAttack_Gen(pDst, pRecv->byAttPart, pRecv->wBulletSerial);

	return true;
}

bool CNetworkEX::AttackSkillRequest(int n, char* pBuf)
{
	_attack_skill_request_clzo* pRecv = (_attack_skill_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	CCharacter* pDst = (CCharacter*)g_Main.GetObject(obj_kind_char, pRecv->byID, pRecv->wIndex);

	if(pRecv->byEffectCode != effect_code_skill && pRecv->byEffectCode != effect_code_class)
		return false;

	if(!g_Main.m_tblEffectData[pRecv->byEffectCode].GetRecord(pRecv->wSkillIndex))
		return false;

	pOne->pc_PlayAttack_Skill(pDst, pRecv->byEffectCode, pRecv->wSkillIndex, pRecv->wBulletSerial);

	return true;
}

bool CNetworkEX::AttackForceRequest(int n, char* pBuf)
{
	_attack_force_request_clzo* pRecv = (_attack_force_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	//지역기인경우 pDst가 NULL이고 fPos가 세팅..
	CCharacter* pDst = (CCharacter*)g_Main.GetObject(obj_kind_char, pRecv->byID, pRecv->wIndex);
	float fPos[] = { pRecv->zAreaPos[0], pRecv->zAreaPos[1] };

	pOne->pc_PlayAttack_Force(pDst, fPos, pRecv->wForceSerial);

	return true;
}

bool CNetworkEX::AttackUnitRequest(int n, char* pBuf)
{
	_attack_unit_request_clzo* pRecv = (_attack_unit_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	CCharacter* pDst = (CCharacter*)g_Main.GetObject(obj_kind_char, pRecv->byID, pRecv->wIndex);
	if(!pDst)
		return false;

	if(pRecv->byWeaponPart >= UNIT_BULLET_NUM)
		return false;

	pOne->pc_PlayAttack_Unit(pDst, pRecv->byWeaponPart);

	return true;
}

bool CNetworkEX::AttackTestRequest(int n, char* pBuf)
{
	_attack_test_request_clzo* pRecv = (_attack_test_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	if(pRecv->byEffectCode != 0xFF && pRecv->byEffectCode != effect_code_skill && pRecv->byEffectCode != effect_code_force)
		return false;

	if(pRecv->byEffectCode == effect_code_skill)
	{
		if(!g_Main.m_tblEffectData[effect_code_skill].GetRecord(pRecv->byEffectIndex))
			return false;
	}
	else if(pRecv->byEffectCode == effect_code_force)
	{
		if(!g_Main.m_tblEffectData[effect_code_force].GetRecord(pRecv->byEffectIndex))
			return false;
	}

	if(pRecv->byWeaponPart != 0xFF)
	{
		if(pRecv->byWeaponPart >= UNIT_BULLET_NUM)
			return false;
	}

	pOne->pc_PlayAttack_Test(pRecv->byEffectCode, pRecv->byEffectIndex, pRecv->wBulletSerial, pRecv->byWeaponPart, pRecv->zTar);

	return true;
}

bool CNetworkEX::ModeChangeRequest(int n, char* pBuf)
{
	_mode_change_request_clzo* pRecv = (_mode_change_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	if(pRecv->byModeCode != mode_type_demili && pRecv->byModeCode != mode_type_mili)
		return false;

	pOne->pc_ChangeModeType(pRecv->byModeCode);

	return true;
}

bool CNetworkEX::ForceRequest(int n, char* pBuf)
{
	_force_request_clzo* pRecv = (_force_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	if(!g_Main.GetObject(obj_kind_char, pRecv->idDst.byID, pRecv->idDst.wIndex))
		return false;
	
	pOne->pc_ForceRequest(pRecv->wForceSerial, &pRecv->idDst);

	return true;
}

bool CNetworkEX::SkillRequest(int n, char* pBuf)
{
	_skill_request_clzo* pRecv = (_skill_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	if(!g_Main.GetObject(obj_kind_char, pRecv->idDst.byID, pRecv->idDst.wIndex))
		return false;

	if(pRecv->bySkillIndex >= g_Main.m_tblEffectData[effect_code_skill].GetRecordNum())
		return false;

	pOne->pc_SkillRequest(pRecv->bySkillIndex, &pRecv->idDst);

	return true;
}

bool CNetworkEX::ClassSkillRequest(int n, char* pBuf)
{
	_class_skill_request_clzo* pRecv = (_class_skill_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	if(!g_Main.GetObject(obj_kind_char, pRecv->idDst.byID, pRecv->idDst.wIndex))
		return false;

	if(pRecv->wSkillIndex >= g_Main.m_tblEffectData[effect_code_class].GetRecordNum())
		return false;

	pOne->pc_ClassSkillRequest(pRecv->wSkillIndex, &pRecv->idDst);

	return true;
}

bool CNetworkEX::MakeTowerRequest(int n, char* pBuf)
{
	_make_tower_request_clzo* pRecv = (_make_tower_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	if(pRecv->wSkillIndex >= g_Main.m_tblEffectData[effect_code_class].GetRecordNum())
		return false;

	if(pRecv->byMaterialNum > _make_tower_request_clzo::max_material)
		return false;

	for(int i = 0; i < pRecv->byMaterialNum; i++)
	{
		if(pRecv->Material[i].byMaterSlotIndex >= max_tower_material)
			return false;
		if(pRecv->Material[i].byAmount > max_overlap_num)
			return false;
	}

	float fPos[3];
	::ShortToFloat(pRecv->zPos, fPos, 3);

	pOne->pc_MakeTowerRequest(pRecv->wSkillIndex, pRecv->wTowerItemSerial, pRecv->byMaterialNum, pRecv->Material, fPos);

	return true;
}

bool CNetworkEX::BackTowerRequest(int n, char* pBuf)
{
	_back_tower_request_clzo* pRecv = (_back_tower_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	pOne->pc_BackTowerRequest(pRecv->dwTowerObjSerial);

	return true;
}


