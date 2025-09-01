#include "stdafx.h"
#include "Player.h"
#include "MainThread.h"
#include "SkillFld.h"
#include "ForceFld.h"
#include "Expfld.h"
#include "ErrorEventFromServer.h"
#include "pt_world_msg.h"
#include "ClassFld.h"

void CPlayer::SendMsg_NewViewOther(BYTE byViewType)
{
	_other_new_view_zocl Send;

	Send.wIndex = m_ObjID.m_wIndex;
	Send.dwSerial = m_dwObjSerial;
	Send.wEquipVer = GetVisualVer();
	::FloatToShort(m_fCurPos, Send.zPos, 3);
	Send.byRaceCode = m_Param.GetRaceSexCode();
	Send.byViewType = byViewType;
	Send.dwStateFlag = GetStateFlag();
	Send.wLastEffectCode = m_wLastContEffect;

	BYTE byType[msg_header_num] = {init_msg, other_new_view_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_AttackResult_Error(int nErrCode)
{
	_attack_error_result_zocl Send;

	Send.sFailureCode = nErrCode;

	BYTE byType[msg_header_num] = {fight_msg, attack_error_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_AttackResult_Gen(CAttack* pAt, BYTE byBulletIndex)
{
	_attack_gen_result_zocl Send;

	Send.idAtter.byID = m_ObjID.m_byID;
	Send.idAtter.wIndex = m_ObjID.m_wIndex;
	Send.idAtter.dwSerial = m_dwObjSerial;
	Send.byAttackPart = pAt->m_pp->nPart;
	Send.bCritical = pAt->m_bIsCrtAtt;
	Send.byBulletIndex = byBulletIndex;

	Send.byListNum = pAt->m_nDamagedObjNum;
	for(int i = 0; i < pAt->m_nDamagedObjNum; i++)
	{
		Send.DamList[i].idDster.byID		= pAt->m_DamList[i].m_pChar->m_ObjID.m_byID;
		Send.DamList[i].idDster.wIndex		= pAt->m_DamList[i].m_pChar->m_ObjID.m_wIndex;
		Send.DamList[i].idDster.dwSerial	= pAt->m_DamList[i].m_pChar->m_dwObjSerial;
		Send.DamList[i].wDamage				= pAt->m_DamList[i].m_nDamage;
		Send.DamList[i].wLeftHP				= max(int(pAt->m_DamList[i].m_pChar->GetHP() - pAt->m_DamList[i].m_nDamage), 0);
	}

	BYTE byTypeOther[msg_header_num] = {fight_msg, attack_gen_result_zocl};
	CircleReport(byTypeOther, (char*)&Send, Send.size(), true);
}

void CPlayer::SendMsg_AttackResult_Skill(BYTE byEffectCode, CPlayerAttack* pAt, BYTE byBulletIndex)
{
	_attack_skill_result_zocl Send; 

	Send.idAtter.byID = m_ObjID.m_byID;
	Send.idAtter.wIndex = m_ObjID.m_wIndex;
	Send.idAtter.dwSerial = m_dwObjSerial;
	Send.byEffectCode = byEffectCode;
	Send.wSkillIndex = pAt->m_pp->pFld->m_dwIndex;
	Send.bySkillLv = pAt->m_pp->nLevel;
	Send.byAttackPart = pAt->m_pp->nPart;
	Send.bCritical = pAt->m_bIsCrtAtt;
	Send.byBulletIndex = byBulletIndex;

	Send.byListNum = pAt->m_nDamagedObjNum;
	for(int i = 0; i < pAt->m_nDamagedObjNum; i++)
	{
		Send.DamList[i].idDster.byID		= pAt->m_DamList[i].m_pChar->m_ObjID.m_byID;
		Send.DamList[i].idDster.wIndex		= pAt->m_DamList[i].m_pChar->m_ObjID.m_wIndex;
		Send.DamList[i].idDster.dwSerial	= pAt->m_DamList[i].m_pChar->m_dwObjSerial;
		Send.DamList[i].wDamage				= pAt->m_DamList[i].m_nDamage;
		Send.DamList[i].wLeftHP				= max(int(pAt->m_DamList[i].m_pChar->GetHP() - pAt->m_DamList[i].m_nDamage), 0);
	}

	BYTE byTypeOther[msg_header_num] = {fight_msg, attack_skill_result_zocl};
	CircleReport(byTypeOther, (char*)&Send, Send.size(), true);
}

void CPlayer::SendMsg_AttackResult_Force(CAttack* pAt)
{
	_attack_force_result_zocl Send;

	Send.idAtter.byID = m_ObjID.m_byID;
	Send.idAtter.wIndex = m_ObjID.m_wIndex;
	Send.idAtter.dwSerial = m_dwObjSerial;
	Send.byForceIndex = pAt->m_pp->pFld->m_dwIndex;
	Send.byForceLv = pAt->m_pp->nLevel;
	Send.zAreaPos[0] = pAt->m_pp->fArea[0];
	Send.zAreaPos[1] = pAt->m_pp->fArea[2];
	Send.byAttackPart = pAt->m_pp->nPart;
	Send.bCritical = pAt->m_bIsCrtAtt;

	Send.byListNum = pAt->m_nDamagedObjNum;
	for(int i = 0; i < pAt->m_nDamagedObjNum; i++)
	{
		Send.DamList[i].idDster.byID		= pAt->m_DamList[i].m_pChar->m_ObjID.m_byID;
		Send.DamList[i].idDster.wIndex		= pAt->m_DamList[i].m_pChar->m_ObjID.m_wIndex;
		Send.DamList[i].idDster.dwSerial	= pAt->m_DamList[i].m_pChar->m_dwObjSerial;
		Send.DamList[i].wDamage				= pAt->m_DamList[i].m_nDamage;
		Send.DamList[i].wLeftHP				= max(int(pAt->m_DamList[i].m_pChar->GetHP() - pAt->m_DamList[i].m_nDamage), 0);
	}

	BYTE byTypeOther[msg_header_num] = {fight_msg, attack_force_result_zocl};
	CircleReport(byTypeOther, (char*)&Send, Send.size(), true);
}

void CPlayer::SendMsg_AttackResult_Unit(CAttack* pAt, BYTE byWeaponPart, BYTE byBulletIndex)
{
	_attack_unit_result_zocl Send;

	Send.idAtter.byID = m_ObjID.m_byID;
	Send.idAtter.wIndex = m_ObjID.m_wIndex;
	Send.idAtter.dwSerial = m_dwObjSerial;
	Send.byWeaponPart = byWeaponPart;
	Send.wWeaponIndex = pAt->m_pp->pFld->m_dwIndex;
	Send.byBulletIndex = byBulletIndex;
	Send.byAttackPart = pAt->m_pp->nPart;
	Send.bCritical = pAt->m_bIsCrtAtt;

	Send.byListNum = pAt->m_nDamagedObjNum;
	for(int i = 0; i < pAt->m_nDamagedObjNum; i++)
	{
		Send.DamList[i].idDster.byID		= pAt->m_DamList[i].m_pChar->m_ObjID.m_byID;
		Send.DamList[i].idDster.wIndex		= pAt->m_DamList[i].m_pChar->m_ObjID.m_wIndex;
		Send.DamList[i].idDster.dwSerial	= pAt->m_DamList[i].m_pChar->m_dwObjSerial;
		Send.DamList[i].wDamage				= pAt->m_DamList[i].m_nDamage;
		Send.DamList[i].wLeftHP				= max(int(pAt->m_DamList[i].m_pChar->GetHP() - pAt->m_DamList[i].m_nDamage), 0);
	}

	BYTE byTypeOther[msg_header_num] = {fight_msg, attack_unit_result_zocl};
	CircleReport(byTypeOther, (char*)&Send, Send.size(), true);
}

void CPlayer::SendMsg_AttackResult_Count(CAttack* pAt)
{
	_attack_count_result_zocl Send;

	Send.idAtter.byID = m_ObjID.m_byID;
	Send.idAtter.wIndex = m_ObjID.m_wIndex;
	Send.idAtter.dwSerial = m_dwObjSerial;
	Send.byAttackPart = pAt->m_pp->nPart;
	Send.bCritical = pAt->m_bIsCrtAtt;

	Send.byListNum = pAt->m_nDamagedObjNum;
	for(int i = 0; i < pAt->m_nDamagedObjNum; i++)
	{
		Send.DamList[i].idDster.byID		= pAt->m_DamList[i].m_pChar->m_ObjID.m_byID;
		Send.DamList[i].idDster.wIndex		= pAt->m_DamList[i].m_pChar->m_ObjID.m_wIndex;
		Send.DamList[i].idDster.dwSerial	= pAt->m_DamList[i].m_pChar->m_dwObjSerial;
		Send.DamList[i].wDamage				= pAt->m_DamList[i].m_nDamage;
		Send.DamList[i].wLeftHP				= max(int(pAt->m_DamList[i].m_pChar->GetHP() - pAt->m_DamList[i].m_nDamage), 0);
	}

	BYTE byTypeOther[msg_header_num] = {fight_msg, attack_count_result_zocl};
	CircleReport(byTypeOther, (char*)&Send, Send.size(), true);
}

void CPlayer::SendMsg_TestAttackResult(BYTE byEffectCode, BYTE byEffectIndex, WORD wBulletItemIndex, BYTE byEffectLv, BYTE byWeaponPart, short* pzTar)
{
	_attack_test_result_zocl Send;
	
	Send.idAtter.byID = m_ObjID.m_byID;
	Send.idAtter.wIndex = m_ObjID.m_wIndex;
	Send.idAtter.dwSerial = m_dwObjSerial;
	Send.byEffectCode = byEffectCode;
	Send.byEffectLv = byEffectLv;
	Send.byEffectIndex = byEffectIndex;
	Send.byBulletIndex = wBulletItemIndex;
	Send.byWeaponPart = byWeaponPart;
	Send.zTar[0] = pzTar[0];
	Send.zTar[1] = pzTar[1];

	BYTE byType[msg_header_num] = {fight_msg, attack_test_result_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send), true);
}

void CPlayer::SendMsg_DamageResult(__ITEM* pItem)
{
	_shield_equip_dur_dec_zocl Send;

	Send.byPartIndex = m_nLastBeatenPart;
	Send.wLeftDurPoint = pItem->m_dwDur;
	Send.wSerial = pItem->m_wSerial;

	BYTE byType[msg_header_num] = {fight_msg, shield_equip_dur_dec_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_EquipPartChange(BYTE byPart)
{
	_other_shape_change_zocl Send;

	Send.wIndex = m_ObjID.m_wIndex;
	Send.dwSerial = m_dwObjSerial;
	Send.wEquipVer = GetVisualVer();
	Send.byPartIndex = byPart;
	if(m_Param.m_dbEquip.m_pStorageList[byPart].m_bLoad)
	{
		Send.wItemIndex = ::GetItemUpgedLv(m_Param.m_dbEquip.m_pStorageList[byPart].m_dwLv);
		Send.wItemIndex <<= 12;
		Send.wItemIndex |= m_Param.m_dbEquip.m_pStorageList[byPart].m_wItemIndex;		
	}
	else
	{
		if(byPart < base_fix_num)
			Send.wItemIndex = m_Param.m_dbChar.m_byDftPart[byPart];
		else
			Send.wItemIndex = 0xFFFF;
	}

	BYTE byType[msg_header_num] = {init_msg, other_shape_change_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_UnitRideChange(bool bTake, CParkingUnit* pUnit)
{
	_other_unit_ride_change_zocl Send;

	Send.wIndex = m_ObjID.m_wIndex;
	Send.dwSerial = m_dwObjSerial;
	Send.wEquipVer = GetVisualVer();

	Send.bTake = bTake;
	Send.dwUnitSerial = pUnit->m_dwObjSerial;
	::FloatToShort(m_fCurPos, Send.zNewPos, 3);

	BYTE byType[msg_header_num] = {init_msg, other_unit_ride_change_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_OtherShapeAll(CPlayer* pDst)
{
	if(!m_bLive)
	{
		SendMsg_OtherShapeError(pDst, _other_shape_failure_zocl::NOT_LIVE);
//		::NetTrace("ShapAll..F: S:%d -> S:%d\n", m_dwObjSerial, pDst->m_dwObjSerial);
		return;
	}

//	::NetTrace("ShapAll: S:%d -> S:%d\n", m_dwObjSerial, pDst->m_dwObjSerial);
	
	_other_shape_all_zocl Send;

	Send.wIndex = m_ObjID.m_wIndex;
	Send.dwSerial = m_dwObjSerial;
	Send.wEquipVer = GetVisualVer();
	Send.byRecIndex = m_Param.GetRaceSexCode();
	Send.byFaceIndex = m_Param.m_dbChar.m_byDftPart_Face;
	Send.byHairIndex = m_Param.m_dbChar.m_byDftPart[tbl_code_helmet];
	for(int i = 0; i < equip_fix_num; i++)
	{
		if(m_Param.m_dbEquip.m_pStorageList[i].m_bLoad)
		{
			Send.wModelPerPart[i] = ::GetItemUpgedLv(m_Param.m_dbEquip.m_pStorageList[i].m_dwLv);
			Send.wModelPerPart[i] <<= 12;
			Send.wModelPerPart[i] |= m_Param.m_dbEquip.m_pStorageList[i].m_wItemIndex;
		}
		else
		{
			if(i < base_fix_num)
				Send.wModelPerPart[i] = m_Param.m_dbChar.m_byDftPart[i];
			else
				Send.wModelPerPart[i] = 0xFFFF;
		}
	}

	Send.byUserGrade = m_byUserDgr;

	strcpy(Send.szName, m_Param.GetCharName());

	Send.byUnitFrameIndex = 0xFF;
	if(IsRidingUnit())
	{
		Send.byUnitFrameIndex = m_pUsingUnit->byFrame;
		memcpy(Send.byUnitPartIndex, m_pUsingUnit->byPart, sizeof(BYTE)*UNIT_PART_NUM);
	}

	BYTE byType[msg_header_num] = {init_msg, other_shape_all_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(pDst->m_ObjID.m_wIndex, byType, (char*)&Send, Send.size());
}

void CPlayer::SendMsg_OtherShapePart(CPlayer* pDst)
{
	if(!m_bLive)
	{
		SendMsg_OtherShapeError(pDst, _other_shape_failure_zocl::NOT_LIVE);
		return;
	}

	_other_shape_part_zocl Send;

	Send.wIndex = m_ObjID.m_wIndex;
	Send.dwSerial = m_dwObjSerial;
	Send.wEquipVer = GetVisualVer();

	for(int i = 0; i < equip_fix_num; i++)
	{
		if(m_Param.m_dbEquip.m_pStorageList[i].m_bLoad)
		{
			Send.wModelPerPart[i] = ::GetItemUpgedLv(m_Param.m_dbEquip.m_pStorageList[i].m_dwLv);
			Send.wModelPerPart[i] <<= 12;
			Send.wModelPerPart[i] |= m_Param.m_dbEquip.m_pStorageList[i].m_wItemIndex;
		}
		else
		{
			if(i < base_fix_num)
				Send.wModelPerPart[i] = m_Param.m_dbChar.m_byDftPart[i];
			else
				Send.wModelPerPart[i] = 0xFFFF;		
		}
	}

	Send.byUnitFrameIndex = 0xFF;
	if(IsRidingUnit())
	{
		Send.byUnitFrameIndex = m_pUsingUnit->byFrame;
		memcpy(Send.byUnitPartIndex, m_pUsingUnit->byPart, sizeof(BYTE)*UNIT_PART_NUM);
	}

	BYTE byType[msg_header_num] = {init_msg, other_shape_part_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(pDst->m_ObjID.m_wIndex, byType, (char*)&Send, Send.size());
}

void CPlayer::SendMsg_OtherShapeError(CPlayer* pDst, BYTE byErrCode)
{
	_other_shape_failure_zocl Send;
	
	Send.wOtherIndex = m_ObjID.m_wIndex;
	Send.byErrCode = byErrCode;

	BYTE byType[msg_header_num] = {init_msg, other_shape_failure_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(pDst->m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_Destroy()
{
	_player_destroy_zocl Send;

	Send.wIndex = m_ObjID.m_wIndex;
	Send.dwSerial = m_dwObjSerial;
	Send.byState = m_bMoveOut;

	BYTE byType[msg_header_num] = {init_msg, player_destroy_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_Die()
{
	_player_die_zocl Send;

	Send.wIndex = m_ObjID.m_wIndex;
	Send.dwSerial = m_dwObjSerial;

	BYTE byType[msg_header_num] = {init_msg, player_die_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send), true);
}

void CPlayer::SendMsg_FixPosition(int n)
{
	_player_fixpositon_zocl Send;

	Send.wIndex = m_ObjID.m_wIndex;
	Send.dwSerial = m_dwObjSerial;
	Send.wEquipVer = GetVisualVer();

	::FloatToShort(m_fCurPos, Send.zCur, 3);
	Send.byRaceCode = m_Param.GetRaceSexCode();
	Send.dwStateFlag = GetStateFlag();
	Send.wLastEffectCode = m_wLastContEffect;

	BYTE byType[msg_header_num] = {position_msg, player_fixpositon_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(n, byType, (char*)&Send, sizeof(Send));


//	CPlayer* pDst = &g_Main.m_Player[n];

//	NetTrace("fixpos: I:%d, S:%d -> I:%d, S:%d\n", m_ObjID.m_wIndex, m_dwObjSerial, pDst->m_ObjID.m_wIndex, pDst->m_dwObjSerial);

}

void CPlayer::SendMsg_RealMovePoint(int n)	//특정인한테..
{
	_player_real_move_zocl Send;

	Send.byRaceCode = m_Param.GetRaceSexCode();
	Send.wIndex = m_ObjID.m_wIndex;
	Send.dwSerial = m_dwObjSerial;
	Send.dwEquipVer = GetVisualVer();

	::FloatToShort(m_fCurPos, Send.zCur, 3);
	Send.zTar[0] = m_fTarPos[0];
	Send.zTar[1] = m_fTarPos[2];
	Send.wLastEffectCode = m_wLastContEffect;
	Send.dwStateFlag = GetStateFlag();
	Send.byAddSpeed = (BYTE)m_EP.GetEff_Plus(_EFF_PLUS::Move_Run_Spd)*10;	//10을곱해서 소숫점을 끊는다..

	BYTE byType[msg_header_num] = {position_msg, player_real_move_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(n, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_MoveNext(bool bOtherSend)
{
	//bOtherSend=true면 다른 플레이어한테도 보낸다.
	if(bOtherSend)	//주위플레이어한테도 보낸다.
	{
		_player_move_zocl Send;	

		Send.byRaceCode = m_Param.GetRaceSexCode();
		Send.wIndex = m_ObjID.m_wIndex;
		Send.wEquipVer = GetVisualVer();
		Send.dwSerial = m_dwObjSerial;
		Send.wLastEffectCode = m_wLastContEffect;
		Send.dwStateFlag = GetStateFlag();

		::FloatToShort(m_fCurPos, Send.zCur, 3);
		Send.zTar[0] = (short)m_fTarPos[0];
		Send.zTar[1] = (short)m_fTarPos[2];
		Send.byAddSpeed = (BYTE)m_EP.GetEff_Plus(_EFF_PLUS::Move_Run_Spd)*10;	//10을곱해서 소숫점을 끊는다..
		
		BYTE byType[msg_header_num] = {position_msg, player_move_zocl};
		CircleReport(byType, (char*)&Send, sizeof(Send));
	}
}

void CPlayer::SendMsg_Revival(BYTE byRet, bool bEquialZone)
{
	_player_revival_zocl Send;

	Send.byRet = byRet;
	Send.byLevel = m_Param.GetLevel();
	Send.wCurFP = m_Param.GetFP();
	Send.wCurHP = m_Param.GetHP();
	Send.wCurST = m_Param.GetSP();
	Send.wStartMapIndex = g_Main.m_MapOper.GetMap(m_pCurMap);
	::FloatToShort(m_fCurPos, Send.zPos, 3);
	if(bEquialZone)
		Send.byZoneCode = 0;
	else 
		Send.byZoneCode = 1;

	BYTE byType[msg_header_num] = {init_msg, player_revival_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_Resurrect(BYTE byRet)
{
	_player_resurrect_zocl Send;

	Send.byRet = byRet;
	Send.byLevel = m_Param.GetLevel();
	Send.wCurFP = m_Param.GetFP();
	Send.wCurHP = m_Param.GetHP();
	Send.wCurST = m_Param.GetSP();

	BYTE byType[msg_header_num] = {init_msg, player_resurrect_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_Stop(bool bAll)
{
	_player_stop_zocl Send;

	Send.dwSerial = m_dwObjSerial;
	::FloatToShort(m_fCurPos, Send.zCur, 3);

	BYTE byType[msg_header_num] = {position_msg, player_stop_zocl};

	if(bAll)
		CircleReport(byType, (char*)&Send, sizeof(Send), true);
	else
		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_MoveError(BYTE byRetCode)
{
	_move_error_result_zocl Send;

	Send.byErrCode = byRetCode;
	::FloatToShort(m_fCurPos, Send.zCur, 3);

	BYTE byType[msg_header_num] = {position_msg, move_error_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_Level(int nLevel)
{
	_level_up_zocl Send;

	Send.dwSerial = m_dwObjSerial;
	Send.byLevel = nLevel;

	BYTE byType[msg_header_num] = {event_msg, level_up_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send), true);
}

void CPlayer::SendMsg_MaxHFSP()
{
	_max_hfsp_zocl Send;

	Send.wMaxHP = GetLimHP();
	Send.wMaxFP = GetLimFP();
	Send.wMaxST = GetLimSP();

	BYTE byType[msg_header_num] = {event_msg, max_hfsp_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));

	//파티원에게도 알린다.
	SendData_PartyMemberMaxHFSP();
}

void CPlayer::SendMsg_Recover()
{
	_recover_zocl Send;

	Send.wHP = m_Param.GetHP();
	Send.wFP = m_Param.GetFP();
	Send.wST = m_Param.GetSP();

	BYTE byType[msg_header_num] = {event_msg, recover_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_AlterBooster()
{
	_unit_booster_charge_inform_zocl Send;

	Send.bySlotIndex = m_pUsingUnit->bySlotIndex;
	Send.wBoosterGauge = m_pUsingUnit->wBooster;

	BYTE byType[msg_header_num] = {unit_msg, recover_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_TakeNewResult(BYTE byErrCode, __ITEM* pItem)
{
	_itembox_take_new_result_zocl Send;
	Send.sErrorCode = byErrCode;
	if(byErrCode == 0)
	{
		Send.byItemTableCode = pItem->m_byTableCode;
		Send.wItemIndex = pItem->m_wItemIndex;
		Send.dwCurDurPoint = pItem->m_dwDur;
		Send.wItemSerial = pItem->m_wSerial;
		Send.dwUptInfo = pItem->m_dwLv;
	}

	BYTE byType[msg_header_num] = {item_msg, itembox_take_new_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, Send.size());
}

void CPlayer::SendMsg_TakeAddResult(BYTE byErrCode, __ITEM* pItem)
{
	_itembox_take_add_result_zocl Send;

	Send.sErrorCode = byErrCode;

	if(byErrCode == 0)
	{
		Send.wItemSerial = pItem->m_wSerial;
		Send.byAmount = pItem->m_dwDur;	
	}

	BYTE byType[msg_header_num] = {item_msg, itembox_take_add_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, Send.size());
}

void CPlayer::SendMsg_ThrowStorageResult(BYTE byErrCode)
{
	_throw_storage_result_zocl Send;

	Send.byErrCode = byErrCode;

	BYTE byType[msg_header_num] = {item_msg, throw_storage_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_MapOut(BYTE byMapOutCode, BYTE byNextMapCode)
{
	_other_map_out_zocl Send;

	Send.byMapOutType = byMapOutCode;
	Send.dwSerial = m_dwObjSerial;
	Send.byNextMap = byNextMapCode;

	BYTE byType[msg_header_num] = {init_msg, other_map_out_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

/*
파티장이 바뀔때..
*/
void CPlayer::SendMsg_StateInform(DWORD dwStateFlag)
{
	_state_inform_zocl Send;
	
	Send.dwSerial = m_dwObjSerial;
	Send.dwState = dwStateFlag;

	BYTE byType[msg_header_num] = {position_msg, state_inform_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send), true);
}

void CPlayer::SendMsg_GotoBasePortalResult(BYTE byErrCode)
{
	_goto_baseportal_result_zocl Send;

	Send.byRet = byErrCode;
	Send.byMapCode = m_pCurMap->m_pMapSet->m_dwIndex;
	::FloatToShort(m_fCurPos, Send.zPos, 3);

	BYTE byType[msg_header_num] = {position_msg, goto_baseportal_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_GotoRecallResult(BYTE byErrCode, BYTE byMapCode, float* pfStartPos)
{
	_goto_avator_result_zocl Send;

	Send.byRet = byErrCode;
	Send.byMapCode = byMapCode;
	memcpy(Send.fStartPos, pfStartPos, sizeof(float)*3);

	BYTE byType[msg_header_num] = {position_msg, goto_avator_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_ExitSaveDataResult(BYTE byRetCode)
{
	_exit_save_data_result_zocl Send;

	Send.byRetCode = byRetCode;

	BYTE byType[msg_header_num] = {init_msg, exit_save_data_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_StatInform(BYTE byStatIndex, DWORD dwNewStat)
{
	_stat_inform_zocl Send;

	Send.byStatIndex = byStatIndex;
	Send.dwNewStat = dwNewStat; 

	BYTE byType[msg_header_num] = {init_msg, stat_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_FcitemInform(WORD wItemSerial, DWORD dwNewStat)
{
	_fcitem_inform_zocl Send;

	Send.wItemSerial = wItemSerial;
	Send.dwNewStat = dwNewStat;

	BYTE byType[msg_header_num] = {init_msg, fcitem_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_ItemStorageRefresh(BYTE byStorageCode)
{
	_STORAGE_LIST*	pList = m_Param.m_pStoragePtr[byStorageCode];

	_storage_refresh_inform_zocl Send;

	Send.byStorageCode = byStorageCode;
	Send.byItemNum = pList->GetNumUseCon();

	int nCnt = 0;
	for(int i = 0; i < Send.byItemNum; i++)
	{
		__ITEM* p = &pList->m_pStorageList[i];
		if(p->m_bLoad)
		{
			Send.wSerial[nCnt] = p->m_wSerial;
			nCnt++;
		}
	}

	BYTE byType[msg_header_num] = {init_msg, storage_refresh_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_BuyItemStoreResult(CItemStore* pStore, bool bSucc, int nOfferNum, _buy_offer* pCard, char* pszErrCode)
{
	if(bSucc)
	{
		_buy_store_success_zocl Send;

		Send.dwLeftDalant = m_Param.GetDalant();
		Send.dwLeftGold = m_Param.GetGold();
		Send.dwConsumDanlant = pStore->GetLastTradeDalant();
		Send.dwConsumGold = pStore->GetLastTradeGold();
		Send.byDiscountRate = (BYTE)m_EP.m_fEff_Have[_EFF_HAVE::Trade_Prof]*100;

		Send.byBuyNum = nOfferNum;		
		for(int i = 0; i < nOfferNum; i++)
		{
			Send.OfferList[i].wSerial = pCard[i].wSerial;
		}

		BYTE byType[msg_header_num] = {trade_msg, buy_store_success_zocl};
		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, Send.size());		
	}
	else
	{
		_buy_store_failure_zocl Send;
		
		Send.dwDalant = (DWORD)m_Param.GetDalant();
		Send.dwGold = (DWORD)m_Param.GetGold();

		Send.byErrCodeLen = strlen(pszErrCode);
		if(Send.byErrCodeLen > max_error_code_size)
			Send.byErrCodeLen = max_error_code_size;
		memcpy(Send.szErrCode, pszErrCode, Send.byErrCodeLen);

		BYTE byType[msg_header_num] = {trade_msg, buy_store_failure_zocl};
		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, Send.size());				
	}
}

void CPlayer::SendMsg_SellItemStoreResult(CItemStore* pStore, bool bSucc, char* pszErrCode)
{
	_sell_store_result_zocl Send;
	
	Send.bSucc = bSucc;
	Send.dwLeftDalant = m_Param.GetDalant();
	Send.dwLeftGold = m_Param.GetGold();
	Send.dwProfitDanlant = pStore->GetLastTradeDalant();
	Send.dwProfitGold = pStore->GetLastTradeGold();
	Send.byDiscountRate = (BYTE)m_EP.m_fEff_Have[_EFF_HAVE::Trade_Prof]*100;

	if(!bSucc)
	{
		Send.byErrCodeLen = strlen(pszErrCode);
		if(Send.byErrCodeLen > max_error_code_size)
			Send.byErrCodeLen = max_error_code_size;
		memcpy(Send.szErrCode, pszErrCode, Send.byErrCodeLen);
	}
	else
		Send.byErrCodeLen = 0;

	BYTE byType[msg_header_num] = {trade_msg, sell_store_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, Send.size());
}

void CPlayer::SendMsg_RepairItemStoreResult(CItemStore* pStore, int nOfferNum, _repair_offer* pCard, bool bSucc, char* pszErrCode)
{
	_repair_result_zocl Send;

	Send.bSucc = bSucc;
	Send.dwLeftDalant = m_Param.GetDalant();
	Send.dwLeftGold = m_Param.GetGold();
	Send.dwConsumDanlant = pStore->GetLastTradeDalant();
	Send.dwConsumGold = pStore->GetLastTradeGold();

	if(!bSucc)
	{
		Send.byErrCodeLen = strlen(pszErrCode);
		if(Send.byErrCodeLen > max_error_code_size)
			Send.byErrCodeLen = max_error_code_size;
		memcpy(Send.szErrCode, pszErrCode, Send.byErrCodeLen);
	}
	else
		Send.byErrCodeLen = 0;

	BYTE byType[msg_header_num] = {trade_msg, repair_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, Send.size());
}

void CPlayer::SendMsg_ExchangeMoneyResult(BYTE byErrCode)
{
	_exchange_money_result_zocl Send;

	Send.byErrCode = byErrCode;
	Send.dwLeftDalant = m_Param.GetDalant();
	Send.dwLeftGold = m_Param.GetGold();

	BYTE byType[msg_header_num] = {trade_msg, exchange_money_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

//void CPlayer::SendMsg_EconomyHistoryInform()
//{
//	_economy_history_inform_zocl Send;
//
//	//경제시스템 히스토리 정보
//	memcpy(Send.wEconomyGuide, ::eGetGuideHistory(m_Param.GetRaceCode()), sizeof(WORD)*MAX_ECONOMY_HISTORY);
//
//	BYTE byType[msg_header_num] = {trade_msg, economy_history_inform_zocl};
//	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
//}

//void CPlayer::SendMsg_EconomyRateInform()
//{
//	_economy_rate_inform_zocl Send;
//
//	Send.fPayExgRate = ::eGetRate(m_Param.GetRaceCode());
//	Send.fTexRate = ::eGetTex(m_Param.GetRaceCode());
//	Send.wEconomyGuide = ::eGetGuide(m_Param.GetRaceCode());
//
//	BYTE byType[msg_header_num] = {trade_msg, economy_rate_inform_zocl};
//	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
//}

// 속쵱변鑒
void CPlayer::SendMsg_EconomyHistoryInform()
{
	_economy_history_inform_zocl Send;

	//경제시스템 히스토리 정보
	_economy_history_data* pData = ::eGetGuideHistory();
	for(int r = 0; r < RACE_NUM; r++)
	{
		for(int h = 0; h < MAX_ECONOMY_HISTORY; h++)
		{
			Send.wEconomyGuide[r][h] = pData[h].wEconomyGuide[r];
		}
	}

	BYTE byType[msg_header_num] = {trade_msg, economy_history_inform_zocl};
	SEND_USER(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_EconomyRateInform(bool bStart)
{
	_economy_rate_inform_zocl Send;

	Send.bStart = bStart;
	Send.fPayExgRate = ::eGetRate(m_Param.GetRaceCode());
	Send.fTexRate = ::eGetTex(m_Param.GetRaceCode());
	Send.wMgrValue = ::eGetMgrValue();

	if(!bStart)
	{
		for(int r = 0; r < RACE_NUM; r++)
			Send.wEconomyGuide[r] = ::eGetGuide(r);
	}

	BYTE byType[msg_header_num] = {trade_msg, economy_rate_inform_zocl};
	SEND_USER(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_UsePotionResult(BYTE byErrCode, WORD wSerial)
{	
	_use_potion_result_zocl Send;

	Send.byErrCode = byErrCode;
	Send.wPotionSerial = wSerial;
	Send.wHP = m_Param.GetHP();
	Send.wFP = m_Param.GetFP();
	Send.wSP = m_Param.GetSP();

	BYTE byType[msg_header_num] = {item_msg, use_potion_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_EquipPartResult(BYTE byErrCode)
{
	_equip_part_result_zocl Send;

	Send.byErrCode = byErrCode;

	BYTE byType[msg_header_num] = {item_msg, equip_part_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_EmbellishResult(BYTE byErrCode)
{
	_embellish_result_zocl Send;

	Send.byErrCode = byErrCode;

	BYTE byType[msg_header_num] = {item_msg, embellish_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_OffPartResult(BYTE byErrCode)
{
	_off_part_result_zocl Send;

	Send.byErrCode = byErrCode;

	BYTE byType[msg_header_num] = {item_msg, off_part_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_MakeItemResult(BYTE byErrCode)
{
	_make_item_result_zocl Send;

	Send.byErrCode = byErrCode;

	BYTE byType[msg_header_num] = {item_msg, make_item_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_ItemUpgrade(BYTE byErrCode)
{
	_upgrade_item_result_zocl Send;

	Send.byErrCode = byErrCode;

	BYTE byType[msg_header_num] = {item_msg, upgrade_item_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_ItemDowngrade(BYTE byErrCode)
{
	_downgrade_item_result_zocl Send;

	Send.byErrCode = byErrCode;

	BYTE byType[msg_header_num] = {item_msg, downgrade_item_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_PotionSocketChange(BYTE byErrCode)
{
	_potionsocket_change_result_zocl Send;

	Send.byErrCode = byErrCode;

	BYTE byType[msg_header_num] = {ui_msg, potionsocket_change_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_PotionSeparation(WORD wParentSerial, BYTE byParentAmount, WORD wChildSerial, BYTE byChildAmount, int nRet)
{
	_potionsocket_separation_result_zocl Send;

	Send.wParentSerial = wParentSerial;
	Send.byParentAmount = byParentAmount;
	Send.wChildSerial = wChildSerial;
	Send.byChildAmount = byChildAmount;
	Send.sErrorCode = nRet;

	BYTE byType[msg_header_num] = {ui_msg, potionsocket_separation_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_PotionDivision(WORD wSerial, BYTE byParentAmount, WORD wChildSerial, BYTE byChildAmount, int nRet)
{
	_potionsocket_division_result_zocl Send;

	Send.wSerial = wSerial;
	Send.byParentAmount = byParentAmount;
	Send.wChildSerial = wChildSerial;
	Send.byChildAmount = byChildAmount;
	Send.sErrorCode = nRet;

	BYTE byType[msg_header_num] = {ui_msg, potionsocket_division_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_BaseDownloadResult()
{
	m_bBaseDownload = true;	//base down load flag turn off..

	_base_download_result_zocl Send;

	Send.byRetCode = 0;
	Send.wZoneIndex = m_ObjID.m_wIndex;
	Send.wExpRate = m_wExpRate;
	for(int i = 0; i < class_history_num; i++)
	{
		if(m_Param.m_pClassHistory[i])
			Send.wClassHistory[i] = m_Param.m_pClassHistory[i]->m_dwIndex;
		else
			Send.wClassHistory[i] = 0xFFFF;
	}
	Send.wHP = m_Param.GetHP();
	Send.wSP = m_Param.GetSP();
	Send.wFP = m_Param.GetFP();
	Send.wMaxHP = m_nMaxHP;
	Send.wMaxSP = m_nMaxSP;
	Send.wMaxFP = m_nMaxFP;

	_base_fld* pMapFld = g_Main.m_tblMapData.GetRecord(m_Param.GetMapCode());
	Send.wMapIndex = pMapFld->m_dwIndex;
	::FloatToShort(m_Param.GetCurPos(), Send.zPos, 3);

	//포션정보
	for(i = 0; i < potion_belt_num; i++)
	{
		__ITEM* pProp = &m_Param.m_dbBelt.m_pStorageList[i];
		if(pProp->m_bLoad)
		{
			Send.BeltList[i].zItemIndex = pProp->m_wItemIndex;
			Send.BeltList[i].byNum = pProp->m_dwDur;	
			Send.BeltList[i].sClientIndex = pProp->m_byClientIndex;
		}
		else
			Send.BeltList[i].zItemIndex = -1;
	}
	
	//장착정보
	for(i = 0; i < equip_fix_num; i++)
	{
		__ITEM* pProp = &m_Param.m_dbEquip.m_pStorageList[i];
		if(pProp->m_bLoad)
		{
			Send.EquipList[i].sTableCode = pProp->m_byTableCode;
			Send.EquipList[i].wItemIndex = pProp->m_wItemIndex;
			Send.EquipList[i].dwUptInfo = pProp->m_dwLv;
		}
		else
			Send.EquipList[i].sTableCode = -1;
	}

	//장식정보
	for(i= 0; i < embellish_fix_num; i++)
	{
		__ITEM* pProp = &m_Param.m_dbEmbellish.m_pStorageList[i];
		if(pProp->m_bLoad)
		{
			Send.EmbellishList[i].sTableCode = pProp->m_byTableCode;
			Send.EmbellishList[i].wItemIndex = pProp->m_wItemIndex;
			Send.EmbellishList[i].wNum = pProp->m_dwDur;
			Send.EmbellishList[i].sClientIndex = pProp->m_byClientIndex;
		}
		else
			Send.EmbellishList[i].sTableCode = -1;
	}

	//링크정보
	for(i = 0; i < sf_linker_num; i++)
	{
		_SF_LINK* pLinker = &m_Param.m_SFLink[i];

		if(!pLinker->m_bLoad)
			Send.LinkerList[i].byEffectCode = 0xFF;
		else
		{
			Send.LinkerList[i].byEffectCode = pLinker->m_byEffectCode;
			Send.LinkerList[i].wItemIndex = pLinker->m_wEffectIndex;
			Send.LinkerList[i].sClientIndex = pLinker->m_sClientIndex;
		}
	}

	BYTE byType[msg_header_num] = {init_msg, base_download_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, Send.size());
}

void CPlayer::SendMsg_InvenDownloadResult()
{
	m_bInvenDownload = true;	//inven down load flag turn off..

	_inven_download_result_zocl Send;

	Send.byRetCode = 0;
	Send.byBagNum = m_Param.m_dbChar.m_byUseBagNum;

	int nCnt = 0;
	for(int i = 0; i < m_Param.GetUseSlot(); i++)
	{
		__ITEM* pProp = &m_Param.m_dbInven.m_pStorageList[i];
		if(pProp->m_bLoad)
		{			
			Send.ItemSlotInfo[nCnt].byTableCode = pProp->m_byTableCode;
			Send.ItemSlotInfo[nCnt].sClientIndex = pProp->m_byClientIndex;
			Send.ItemSlotInfo[nCnt].wItemIndex = pProp->m_wItemIndex;
			Send.ItemSlotInfo[nCnt].dwUptInfo = pProp->m_dwLv;
			Send.ItemSlotInfo[nCnt].dwDurPoint = pProp->m_dwDur;				
			nCnt++;
		}
	}

	Send.bySlotNum = nCnt;

	BYTE byType[msg_header_num] = {init_msg, inven_download_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, Send.size());
}

void CPlayer::SendMsg_CumDownloadResult()
{
	m_bCumDownload  = true;

	_cum_download_result_zocl Send;

	Send.byRetCode = 0;
	memcpy(&Send.Stat, &m_pmMst.m_BaseCum, sizeof(_STAT_DB_BASE));

	BYTE byResNum = 0;
	for(int r = 0; r < ::GetMaxResKind(); r++)
	{
		if(m_Param.m_wCuttingResBuffer[r] > 0)
		{
			WORD wResInfo;

			((BYTE*)&wResInfo)[0] = r;
			((BYTE*)&wResInfo)[1] = m_Param.m_wCuttingResBuffer[r];

			Send.wleftResList[byResNum] = wResInfo;
			byResNum++;
		}
	}
	Send.byLeftCuttingResNum = byResNum;

	BYTE byType[msg_header_num] = {init_msg, cum_download_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, Send.size());
}

void CPlayer::SendMsg_ForceDownloadResult()
{
	m_bForceDownload  = true;

	_force_download_result_zocl Send;

	Send.byRetCode = 0;

	int nCnt = 0;

	for(int s = 0; s < force_storage_num; s++)
	{
		__ITEM* pItem = &m_Param.m_dbForce.m_pStorageList[s];
		if(pItem->m_bLoad)
		{
			Send.ItemSlotInfo[nCnt].dwCum = pItem->m_dwDur;
			Send.ItemSlotInfo[nCnt].wItemIndex = pItem->m_wItemIndex;

			nCnt++;				
		}
	}

	Send.bySlotNum = nCnt;

	BYTE byType[msg_header_num] = {init_msg, force_download_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, Send.size());	

}

void CPlayer::SendMsg_SpecialDownloadResult()
{
	m_bSpecialDownload = true;

	if(m_Param.GetRaceSexCode()/2 == race_code_bellato)
	{
		_unit_download_result_zocl Send;

		int cnt = 0;
		for(int i = 0; i < unit_storage_num; i++)
		{
			if(m_Param.m_UnitDB.m_List[i].byFrame != 0xFF)
			{
				Send.UnitList[cnt].bySlotIndex = i;
				memcpy(&Send.UnitList[cnt].UnitData, &m_Param.m_UnitDB.m_List[i], sizeof(_UNIT_DB_BASE::_LIST));
				cnt++;
			}
		}
		Send.byUnitNum = cnt;

		BYTE byType[msg_header_num] = {init_msg, unit_download_result_zocl};
		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, Send.size());	
	}
	else if(m_Param.GetRaceSexCode()/2 == race_code_cora)
	{
		_animus_download_result_zocl Send;

		int cnt = 0;
		for(int i= 0; i < animus_storage_num; i++)
		{
			__ITEM* pProp = &m_Param.m_dbAnimus.m_pStorageList[i];
			if(pProp->m_bLoad)
			{
				Send.AnimusList[cnt].sItemIndex = pProp->m_wItemIndex;
				Send.AnimusList[cnt].dwExp = pProp->m_dwDur;
				Send.AnimusList[cnt].dwParam = pProp->m_dwLv;
				cnt++;
			}
		}
		Send.byAnimusNum = cnt;

		BYTE byType[msg_header_num] = {init_msg, animus_download_result_zocl};
		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, Send.size());	
	}
	else
	{
		_animus_download_result_zocl Send;

		BYTE byType[msg_header_num] = {init_msg, animus_download_result_zocl};
		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, Send.size());	
	}
}

void CPlayer::SendMsg_StoreListResult()
{
	_store_list_result_zocl Send;

	CMapData* pMap = &g_Main.m_MapOper.m_Map[m_Param.GetMapCode()];

	for(int i = 0; i < pMap->m_nItemStoreDumNum; i++)
	{
		Send.StorePos[i].wStoreIndex = pMap->m_ItemStore[i].m_pRec->m_dwIndex;
		memcpy(Send.StorePos[i].fPos, pMap->m_pItemStoreDummy[i].m_pDumPos->m_fCenterPos, sizeof(float)*3);
	}

	Send.byStoreNum = pMap->m_nItemStoreDumNum;

	BYTE byType[msg_header_num] = {trade_msg, store_list_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, Send.size());
}

void CPlayer::SendMsg_ModeChange(BYTE byMode)
{
	_mode_change_result_zocl Send;

	Send.byModeCode = byMode;

	BYTE byType[msg_header_num] = {fight_msg, mode_change_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}	

void CPlayer::SendMsg_MovePortal(BYTE byRet, BYTE byMapIndex, BYTE byPotalIndex, float* pfStartPos, bool bEqualZone)
{
	_move_potal_result_zocl Send;

	Send.byRet = byRet;
	Send.byMapIndex = byMapIndex;
	memcpy(Send.fStartPos, pfStartPos, sizeof(float)*3);
	if(bEqualZone)
		Send.byZoneCode = 0;	//같은존..
	else
		Send.byZoneCode = 1;	//다른존..

	BYTE byType[msg_header_num] = {map_msg, move_potal_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_BattleDungeonOpenResult(BYTE byRetCode)
{
	_battledungeon_open_result_zocl Send;

	Send.byRetCode = byRetCode;

	BYTE byType[msg_header_num] = {dungeon_msg, battledungeon_open_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_GotoDungeon(BYTE byErrCode, BYTE byMapCode, float* fStartPos, CBattleDungeon* pDungeon)
{
	_goto_dungeon_result_zocl Send;

	Send.byRetCode = byErrCode;
	if(byErrCode == 0)
	{
		Send.byMapCode = byMapCode;
		::FloatToShort(fStartPos, Send.zPos, 3);
		Send.wLeftSec = (WORD)pDungeon->GetLeftTimeoutSec();
		Send.wTotalKillNum = (WORD)pDungeon->m_nTotalKillMonNum;
		Send.wMissionKillNum = (WORD)pDungeon->m_nMissionKillMonNum;
	}

	BYTE byType[msg_header_num] = {dungeon_msg, goto_dungeon_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_CloseDungeon(BYTE byCompleteCode)
{
	_close_dungeon_inform_zocl Send;

	Send.byCompleteCode = byCompleteCode;

	BYTE byType[msg_header_num] = {dungeon_msg, close_dungeon_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_OutofDungeon(BYTE byErrCode, BYTE byMapCode, float* fStartPos)
{
	_outof_dungeon_result_zocl Send;

	Send.byRetCode = byErrCode;
	Send.byMapCode = byMapCode;
	::FloatToShort(fStartPos, Send.zPos, 3);

	BYTE byType[msg_header_num] = {dungeon_msg, outof_dungeon_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_UnitFrameBuyResult(BYTE byRetCode, BYTE byFrameCode, BYTE byUnitSlotIndex, WORD wKeyIndex, WORD wKeySerial, DWORD* pdwConsumMoney)
{
	_unit_frame_buy_result_zocl Send;

	Send.byRetCode = byRetCode;
	Send.byFrameCode = byFrameCode;
	Send.byAddUnitSlot = byUnitSlotIndex;
	Send.byKeyIndex = wKeyIndex;
	Send.wKeySerial = wKeySerial;
	Send.dwLeftMoney[money_unit_dalant] = m_Param.GetDalant();
	Send.dwLeftMoney[money_unit_gold] = m_Param.GetGold();
	Send.dwConsumMoney[money_unit_dalant] = pdwConsumMoney[money_unit_dalant];
	Send.dwConsumMoney[money_unit_gold] = pdwConsumMoney[money_unit_gold];

	BYTE byType[msg_header_num] = {unit_msg, unit_frame_buy_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_UnitSellResult(BYTE byRetCode, BYTE bySlotIndex, WORD wKeySerial, DWORD* pdwAddMoney, DWORD dwTotalNonpay, DWORD dwSumDalant, DWORD dwSumGold)
{
	_unit_sell_result_zocl Send;

	Send.byRetCode = byRetCode;
	Send.bySlotIndex = bySlotIndex;
	Send.wKeySerial = wKeySerial;
	Send.dwNonPayDalant = dwTotalNonpay;
	Send.dwAddMoney[money_unit_dalant] = pdwAddMoney[money_unit_dalant];
	Send.dwAddMoney[money_unit_gold] = pdwAddMoney[money_unit_gold];
	Send.dwLeftMoney[money_unit_dalant] = dwSumDalant;
	Send.dwLeftMoney[money_unit_gold] = dwSumGold;

	BYTE byType[msg_header_num] = {unit_msg, unit_sell_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_UnitPartTuningResult(BYTE byRetCode, BYTE bySlotIndex, int* pnCost)
{
	_unit_part_tuning_result_zocl Send;

	Send.byRetCode = byRetCode;
	Send.bySlotIndex = bySlotIndex;
	memcpy(Send.byPart, m_Param.m_UnitDB.m_List[bySlotIndex].byPart, sizeof(BYTE)*UNIT_PART_NUM);
	Send.nCost[money_unit_dalant] = pnCost[money_unit_dalant];
	Send.nCost[money_unit_gold] = pnCost[money_unit_gold];
	Send.dwLeftMoney[money_unit_dalant] = m_Param.GetDalant();
	Send.dwLeftMoney[money_unit_gold] = m_Param.GetGold();

	BYTE byType[msg_header_num] = {unit_msg, unit_part_tuning_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_UnitFrameRepairResult(BYTE byRetCode, BYTE bySlotIndex, WORD wNewGauge, DWORD dwConsumDalant)
{
	_unit_frame_repair_result_zocl Send;

	Send.byRetCode = byRetCode;
	Send.bySlotIndex = bySlotIndex;
	Send.wNewGauge = wNewGauge;
	Send.dwConsumDalant = dwConsumDalant;
	Send.dwLeftDalant = m_Param.GetDalant();

	BYTE byType[msg_header_num] = {unit_msg, unit_frame_repair_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_UnitBulletFillResult(BYTE byRetCode, BYTE bySlotIndex, BYTE* pbyBulletIndex, DWORD* pdwConsumMoney)
{
	_unit_bullet_fill_result_zocl Send;

	Send.byRetCode = byRetCode;
	Send.bySlotIndex = bySlotIndex;
	memcpy(Send.byBulletIndex, pbyBulletIndex, sizeof(BYTE)*UNIT_BULLET_NUM);
	Send.dwComsumMoney[money_unit_dalant] = pdwConsumMoney[money_unit_dalant];
	Send.dwComsumMoney[money_unit_gold] = pdwConsumMoney[money_unit_gold];
	Send.dwLeftMoney[money_unit_dalant] = m_Param.GetDalant();
	Send.dwLeftMoney[money_unit_gold] = m_Param.GetGold();

	BYTE byType[msg_header_num] = {unit_msg, unit_bullet_fill_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_UnitPackFillResult(BYTE byRetCode, BYTE bySlotIndex, BYTE byFillNum, _unit_pack_fill_request_clzo::__list* pList, DWORD* pdwConsumMoney)
{
	_unit_pack_fill_result_zocl Send;

	Send.byRetCode = byRetCode;
	Send.bySlotIndex = bySlotIndex;

	Send.byFillNum = byFillNum;
	memcpy(Send.List, pList, sizeof(_unit_pack_fill_request_clzo::__list)*byFillNum);

	Send.dwComsumMoney[money_unit_dalant] = pdwConsumMoney[money_unit_dalant];
	Send.dwComsumMoney[money_unit_gold] = pdwConsumMoney[money_unit_gold];
	Send.dwLeftMoney[money_unit_dalant] = m_Param.GetDalant();
	Send.dwLeftMoney[money_unit_gold] = m_Param.GetGold();

	BYTE byType[msg_header_num] = {unit_msg, unit_pack_fill_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_UnitDeliveryResult(BYTE byRetCode, BYTE bySlotIndex, DWORD dwParkingUnitSerial, DWORD dwPayDalant)
{
	_unit_delivery_result_zocl Send;

	Send.byRetCode = byRetCode;
	Send.bySlotIndex = bySlotIndex;
	Send.dwParkingUnitSerial = dwParkingUnitSerial;
	Send.dwPayDalant = dwPayDalant;
	Send.dwLeftDalant = m_Param.GetDalant();

	BYTE byType[msg_header_num] = {unit_msg, unit_delivery_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_UnitReturnResult(BYTE byRetCode, DWORD dwPayDalant)
{
	_unit_return_result_zocl Send;

	Send.byRetCode = byRetCode;
	Send.dwPayDalant = dwPayDalant;
	Send.dwNewDalant = m_Param.GetDalant();

	BYTE byType[msg_header_num] = {unit_msg, unit_return_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_UnitTakeResult(BYTE byRetCode)
{
	_unit_take_result_zocl Send;

	Send.byRetCode = byRetCode;

	BYTE byType[msg_header_num] = {unit_msg, unit_take_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_UnitLeaveResult(BYTE byRetCode)
{
	_unit_leave_result_zocl Send;

	Send.byRetCode = byRetCode;

	BYTE byType[msg_header_num] = {unit_msg, unit_leave_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_UnitAlterFeeInform(BYTE bySlotIndex, DWORD dwKeepingFee, DWORD dwPullingFee)
{
	_unit_alter_fee_inform_zocl Send;

	Send.bySlotIndex = bySlotIndex;
//ydq	Send.dwKeepingFee = dwKeepingFee;
	Send.dwPullingFee = dwPullingFee;

	BYTE byType[msg_header_num] = {unit_msg, unit_alter_fee_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_UnitBulletReplaceResult(BYTE byRetCode)
{
	_unit_bullet_replace_result_zocl Send;

	Send.byRetCode = byRetCode;

	BYTE byType[msg_header_num] = {unit_msg, unit_alter_fee_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_AnimusRecallResult(BYTE byResultCode, WORD wLeftFP, CAnimus* pNewAnimus/* = NULL*/)
{
	_animus_recall_result_zocl Send;
	Send.byResultCode = byResultCode;
	Send.wLeftFP = wLeftFP;
	if(byResultCode == 0 && pNewAnimus)
	{
		Send.dwAnimusSerial = pNewAnimus->m_dwObjSerial;
		Send.wAnimusHP = pNewAnimus->m_nHP;
		Send.wAnimusFP = pNewAnimus->m_nFP;
		Send.wAnimusExp = pNewAnimus->m_dwExp;
	}

	BYTE byType[msg_header_num] = {recall_msg, animus_recall_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_AnimusReturnResult(BYTE byRetCode, WORD wAnimusItemSerial)
{
	_animus_return_result_zocl Send;

	Send.byResultCode = byRetCode;
	Send.wAnimusItemSerial = wAnimusItemSerial;

	BYTE byType[msg_header_num] = {recall_msg, animus_return_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_AnimusTargetResult(BYTE byRetCode)
{
	_animus_target_result_zocl Send;

	Send.byResultCode = byRetCode;

	BYTE byType[msg_header_num] = {recall_msg, animus_target_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_AnimusHPInform()
{
	if(!m_pRecalledAnimusChar || !m_pRecalledAnimusItem)
		return;

	_animus_hp_inform_zocl Send;

	Send.wAnimusItemSerial = m_pRecalledAnimusItem->m_wSerial;
	Send.wLeftHP = m_pRecalledAnimusChar->m_nHP;

	BYTE byType[msg_header_num] = {recall_msg, animus_hp_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_AnimusFPInform()
{
	if(!m_pRecalledAnimusChar || !m_pRecalledAnimusItem)
		return;

	_animus_fp_inform_zocl Send;

	Send.wAnimusItemSerial = m_pRecalledAnimusItem->m_wSerial;
	Send.wLeftFP = m_pRecalledAnimusChar->m_nFP;

	BYTE byType[msg_header_num] = {recall_msg, animus_fp_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_AnimusExpInform()
{
	if(!m_pRecalledAnimusChar || !m_pRecalledAnimusItem)
		return;

	_animus_exp_inform_zocl Send;

	Send.wAnimusItemSerial = m_pRecalledAnimusItem->m_wSerial;
	Send.dwExp = m_pRecalledAnimusChar->m_dwExp;

	BYTE byType[msg_header_num] = {recall_msg, animus_exp_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_AnimusModeInform(BYTE byMode)
{
	_animus_command_inform_zocl Send;

	Send.byStateCode = byMode;

	BYTE byType[msg_header_num] = {recall_msg, animus_command_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_StartNewPos(BYTE byMapInMode)
{
	_new_pos_start_result_zocl Send;

	Send.byMapInMode = byMapInMode;

	BYTE byType[msg_header_num] = {init_msg, new_pos_start_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_BreakdownEquipItem(BYTE byPartIndex, WORD wSerial)
{
	_breakdown_equip_item_zocl Send;

	Send.wPlayerIndex = m_ObjID.m_wIndex;
	Send.dwPlayerSerial = m_dwObjSerial;
	Send.dwEquipVer = GetVisualVer();

	Send.byPartIndex = byPartIndex;
	Send.wItemSerial = wSerial;

	BYTE byType[msg_header_num] = {fight_msg, breakdown_equip_item_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send), true);	
}

void CPlayer::SendMsg_ForceInvenChange(BYTE byErrCode)
{
	_force_inven_change_result_zocl Send;

	Send.byErrCode = byErrCode;
		
	BYTE byType[msg_header_num] = {ui_msg, force_inven_change_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_AnimusInvenChange(BYTE byErrCode)
{
	_animus_inven_change_result_zocl Send;

	Send.byErrCode = byErrCode;
		
	BYTE byType[msg_header_num] = {ui_msg, animus_inven_change_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_ResSeparation(BYTE byErrCode, __ITEM* pStartOre, __ITEM* pNewOre)
{
	_res_separation_result_zocl Send;

	Send.byErrCode = byErrCode;
	if(byErrCode == 0)
	{
		Send.wParentSerial = pStartOre->m_wSerial;
		Send.byParentAmount = pStartOre->m_dwDur;
		Send.wChildSerial = pNewOre->m_wSerial;
		Send.byChildAmount = pNewOre->m_dwDur;
	}

	BYTE byType[msg_header_num] = {ui_msg, res_separation_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_ResDivision(BYTE byErrCode, __ITEM* pStartOre, __ITEM* pTargetOre)
{
	_res_division_result_zocl Send;

	Send.byErrCode = byErrCode;
	if(byErrCode == 0)
	{
		Send.wParentSerial = pStartOre->m_wSerial;
		if(pStartOre->m_bLoad)
			Send.byParentAmount = pStartOre->m_dwDur;
		else
			Send.byParentAmount = 0;

		Send.wChildSerial = pTargetOre->m_wSerial;
		Send.byChildAmount = pTargetOre->m_dwDur;
	}

	BYTE byType[msg_header_num] = {ui_msg, res_division_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_AddBagResult(BYTE byErrCode)
{
	_add_bag_result_zocl Send;

	Send.byErrCode = byErrCode;

	BYTE byType[msg_header_num] = {item_msg, add_bag_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_DeleteStorageInform(BYTE byStorageCode, WORD wSerial)
{
	_delete_storage_inform_zocl Send;

	Send.byStorageCode = byStorageCode;
	Send.wSerial = wSerial;

	BYTE byType[msg_header_num] = {item_msg, delete_storage_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_AdjustAmountInform(BYTE byStorageCode, WORD wSerial, DWORD dwDur)
{
	_adjust_amount_inform_zocl Send;

	Send.byStorageCode = byStorageCode;
	Send.wSerial = wSerial;
	Send.dwNewDur = dwDur;

	BYTE byType[msg_header_num] = {sync_msg, adjust_amount_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_MineStartResult(BYTE byErrCode)
{
	_mine_start_result_zocl Send;

	Send.byErrCode = byErrCode;
	Send.dwMineDelayTime = m_dwMineDelayTime;

	BYTE byType[msg_header_num] = {resource_msg, mine_start_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	

	if(byErrCode == 0)//성공시 주변에도 돌린다..
	{
		_mine_start_result_other_zocl SendOther;

		SendOther.dwSerial = m_dwObjSerial;
		SendOther.wIndex = m_ObjID.m_wIndex;

		BYTE byTypeOther[msg_header_num] = {resource_msg, mine_start_result_other_zocl};
		CircleReport(byTypeOther, (char*)&SendOther, sizeof(SendOther));
	}
}

void CPlayer::SendMsg_MineCancle(BYTE byErrCode, WORD wBatteryLeftDurPoint)
{
	_mine_cancle_result_zocl Send;

	Send.byErrCode = byErrCode;
	Send.wBatteryLeftDur = wBatteryLeftDurPoint;
	Send.wEquipLeftDur = m_Param.m_dbEquip.m_pStorageList[tbl_code_weapon].m_dwDur;

	BYTE byType[msg_header_num] = {resource_msg, mine_cancle_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	

	//주변에..
	_mine_end_result_other_zocl SendOther;

	SendOther.dwSerial = m_dwObjSerial;
	SendOther.wIndex = m_ObjID.m_wIndex;

	BYTE byTypeOther[msg_header_num] = {resource_msg, mine_end_result_other_zocl};
	CircleReport(byTypeOther, (char*)&SendOther, sizeof(SendOther));

}

void CPlayer::SendMsg_MineCompleteResult(BYTE byErrCode, BYTE byNewOreIndex, WORD dwOreSerial, BYTE byOreDur, WORD dwBatteryLeftDurPoint)
{
	_mine_complete_result_zocl Send;

	Send.byErrCode = byErrCode;
	Send.byOreDur = byOreDur;
	Send.byOreIndex = byNewOreIndex;
	Send.wOreSerial = dwOreSerial;
	Send.wEquipLeftDur = m_Param.m_dbEquip.m_pStorageList[tbl_code_weapon].m_dwDur;
	Send.wBatteryLeftDur = dwBatteryLeftDurPoint;

	BYTE byType[msg_header_num] = {resource_msg, mine_complete_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	

	//주변에..
	_mine_end_result_other_zocl SendOther;

	SendOther.dwSerial = m_dwObjSerial;
	SendOther.wIndex = m_ObjID.m_wIndex;

	BYTE byTypeOther[msg_header_num] = {resource_msg, mine_end_result_other_zocl};
	CircleReport(byTypeOther, (char*)&SendOther, sizeof(SendOther));
}

void CPlayer::SendMsg_OreCuttingResult(BYTE byErrCode, BYTE byLeftOreNum, DWORD dwConsumDalant)
{
	_ore_cutting_result_zocl Send;

	Send.byErrCode = byErrCode;
	Send.byCuttingNum = 0;
	if(byErrCode == 0)
	{
		Send.byLeftNum = byLeftOreNum;
		Send.dwLeftDalant = m_Param.GetDalant();
		Send.dwConsumDalant = dwConsumDalant;

		int nCnt = 0;
		for(int i = 0; i < 80; i++)
		{
			if(m_Param.m_wCuttingResBuffer[i] > 0)
			{
				Send.ResList[nCnt].wResIndex = i;
				Send.ResList[nCnt].byAddAmount = m_Param.m_wCuttingResBuffer[i];
				nCnt++;
			}
		}
		Send.byCuttingNum = nCnt;
	}

	BYTE byType[msg_header_num] = {resource_msg, ore_cutting_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, Send.size());	
}

void CPlayer::SendMsg_OreIntoBagResult(BYTE byErrCode, WORD wNewSerial)
{
	_ore_into_bag_result_zocl Send;

	Send.byErrCode = byErrCode;
	Send.wNewSerial = wNewSerial;

	BYTE byType[msg_header_num] = {resource_msg, ore_into_bag_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_CuttingCompleteResult()
{
	_cutting_complete_result_zocl Send;

	Send.dwLeftGold = m_Param.GetGold();

	BYTE byType[msg_header_num] = {resource_msg, cutting_complete_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendData_ChatTrans(BYTE byChatType, DWORD dwSenderSerial, BYTE bySize, char* pszMessage)
{
	_chat_message_receipt_udp Data;

	Data.byMessageType = byChatType;
	Data.dwSenderSerial = dwSenderSerial;
	memcpy(Data.sChatData, pszMessage, bySize);
	Data.sChatData[bySize] = NULL;
	Data.bySize = bySize+1;//NULL포함..
	
	BYTE byType[msg_header_num] = {chat_msg, chat_message_receipt_udp};

	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Data, Data.size());	
}

void CPlayer::SendMsg_ChatFarFailure()
{
	_chat_far_failure_zocl Send;

	BYTE byType[msg_header_num] = {chat_msg, chat_far_failure_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));	
}

void CPlayer::SendMsg_PartyJoinInvitationQuestion(WORD wJoinerIndex)
{
	_party_join_invitation_question_zocl Send;

	Send.idBoss.dwSerial = m_dwObjSerial;
	Send.idBoss.wIndex = m_ObjID.m_wIndex;

	BYTE byType[msg_header_num] = {party_msg, party_join_invitation_question_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(wJoinerIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_PartyJoinMemberResult(CPartyPlayer* pJoiner)
{
	_party_join_member_result_zocl Send;

	Send.dwJoinerSerial = pJoiner->m_id.dwSerial;
	strcpy(Send.szJoinerName, pJoiner->m_szName);

	BYTE byType[msg_header_num] = {party_msg, party_join_member_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_PartyJoinJoinerResult()
{
	_party_join_joiner_result_zocl Send;

	CPartyPlayer** ppMem = m_pPartyMgr->GetPtrPartyMember();
	if(!ppMem)
		return;

	Send.byListNum = m_pPartyMgr->GetPopPartyMember()-1;

	for(int m = 0; m < Send.byListNum; m++)
	{
		if(ppMem[m])
		{
			if(ppMem[m] == m_pPartyMgr)	//자기데이터는 빼서준다..
				continue;

			Send.List[m].dwSerial = ppMem[m]->m_id.dwSerial;				
			strcpy(Send.List[m].szAvatorName, ppMem[m]->m_szName);
		}
		else
			break;
	}

	BYTE byType[msg_header_num] = {party_msg, party_join_joiner_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, Send.size());
}

void CPlayer::SendMsg_PartyJoinApplicationQuestion(CPlayer* pJoiner)
{
	_party_join_application_question_zocl Send;

	Send.idApplicant.dwSerial = pJoiner->m_dwObjSerial;
	Send.idApplicant.wIndex = pJoiner->m_ObjID.m_wIndex;

	BYTE byType[msg_header_num] = {party_msg, party_join_application_question_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_PartyLeaveSelfResult(CPartyPlayer* pLeaver, bool bWorldExit)
{
	_party_leave_self_result_zocl Send;

	Send.bWorldExit = bWorldExit;

	if(pLeaver)
		Send.dwExiterSerial = pLeaver->m_id.dwSerial;
	else
		Send.dwExiterSerial = 0xFFFFFFFF;

	BYTE byType[msg_header_num] = {party_msg, party_leave_self_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_PartyLeaveCompulsionResult(CPartyPlayer* pLeaver)
{
	_party_leave_compulsion_result_zocl Send;

	if(pLeaver)
		Send.dwExiterSerial = pLeaver->m_id.dwSerial;
	else
		Send.dwExiterSerial = 0xFFFFFFFF;

	BYTE byType[msg_header_num] = {party_msg, party_leave_compulsion_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_PartyDisjointResult(BYTE bSuccess)
{
	_party_disjoint_result_zocl Send;

	Send.bySuccess = bSuccess;

	BYTE byType[msg_header_num] = {party_msg, party_disjoint_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_PartySuccessResult(CPartyPlayer* pSuccessor)
{
	_party_succession_result_zocl Send;

	if(pSuccessor)
		Send.dwSuccessorSerial = pSuccessor->m_id.dwSerial;
	else
		Send.dwSuccessorSerial = 0xFFFFFFFF;

	BYTE byType[msg_header_num] = {party_msg, party_succession_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_PartyLockResult(BYTE byRet)
{
	_party_lock_result_zocl Send;

	Send.byLock = byRet;

	BYTE byType[msg_header_num] = {party_msg, party_lock_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendData_PartyMemberInfoToMembers()
{
	_party_member_info_upd Data;

	Data.dwMemSerial = m_dwObjSerial;
	Data.wHP = m_Param.GetHP();
	Data.wFP = m_Param.GetFP();
	Data.wSP = m_Param.GetSP();
	Data.wMaxHP = GetLimHP();
	Data.wMaxFP = GetLimFP();
	Data.wMaxSP = GetLimSP();
	Data.byLv = m_Param.GetLevel();
	Data.byMapCode = m_Param.GetMapCode();
	Data.zPos[0] = (short)m_fCurPos[0];
	Data.zPos[1] = (short)m_fCurPos[2];
	Data.byContEffectNum = 0;

	BYTE byEffectNum = 0;

	DWORD dwCurSec = _sf_continous::GetSFContCurTime();//(timeGetTime()/1000)%10000;

	for(int i = 0; i < SF_CONT_CODE_NUM; i++)
	{
		for(int c = 0; c < CONT_SF_SIZE; c++)
		{
			_sf_continous* p = &m_SFCont[i][c];		
			if(!p->m_bExist)
				continue;

			Data.Effect[byEffectNum].wEffectCode = CalcEffectBit(p->m_byEffectCode, p->m_wEffectIndex);
			Data.Effect[byEffectNum].byEffectLv = p->m_byLv;
			Data.Effect[byEffectNum].wLeftSec/*byTime,ydq*/ = (dwCurSec - p->m_wStartSec);
			byEffectNum++;
		}
	}
	Data.byContEffectNum = byEffectNum;

	CPartyPlayer** ppMem = m_pPartyMgr->GetPtrPartyMember();
	if(!ppMem)
		return;

	int nMemPop = m_pPartyMgr->GetPopPartyMember();
	BYTE byType[msg_header_num] = {party_msg, party_member_info_upd};

//	x_sockaddr_in Addr(PORT_UDP_PARTY);

	for(int m = 0; m < nMemPop; m++)
	{
		if(ppMem[m] == m_pPartyMgr)
			continue;

//		Addr.SetIP(ppMem[m]->m_ipAddr);
//		g_Main.m_UDPNetwork.LoadSendMsg(sizeof(Data), byType, (char*)&Data, &Addr);

		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(ppMem[m]->m_wZoneIndex, byType, (char*)&Data, Data.size());
	}
}

void CPlayer::SendData_PartyMemberInfo(WORD wDstIndex)
{
	_party_member_info_upd Data;

	Data.dwMemSerial = m_dwObjSerial;
	Data.wHP = m_Param.GetHP();
	Data.wFP = m_Param.GetFP();
	Data.wSP = m_Param.GetSP();
	Data.wMaxHP = GetLimHP();
	Data.wMaxFP = GetLimFP();
	Data.wMaxSP = GetLimSP();
	Data.byLv = m_Param.GetLevel();
	Data.byMapCode = m_Param.GetMapCode();
	Data.zPos[0] = (short)m_fCurPos[0];
	Data.zPos[1] = (short)m_fCurPos[2];
	Data.byContEffectNum = 0;

	BYTE byEffectNum = 0;
	DWORD dwCurSec = _sf_continous::GetSFContCurTime();//(timeGetTime()/1000)%10000;

	for(int i = 0; i < SF_CONT_CODE_NUM; i++)
	{
		for(int c = 0; c < CONT_SF_SIZE; c++)
		{
			_sf_continous* p = &m_SFCont[i][c];		
			if(!p->m_bExist)
				continue;
			
			Data.Effect[byEffectNum].wEffectCode = CalcEffectBit(p->m_byEffectCode, p->m_wEffectIndex);
			Data.Effect[byEffectNum].byEffectLv = p->m_byLv;
			Data.Effect[byEffectNum].wLeftSec/*byTime,ydq*/ = (dwCurSec - p->m_wStartSec);
			byEffectNum++;
		}
	}
			
	Data.byContEffectNum = byEffectNum;
//	x_sockaddr_in Addr(PORT_UDP_PARTY, dwDstIP);
	
	BYTE byType[msg_header_num] = {party_msg, party_member_info_upd};
//	g_Main.m_UDPNetwork.LoadSendMsg(sizeof(Data), byType, (char*)&Data, &Addr);
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(wDstIndex, byType, (char*)&Data, Data.size());
}

void CPlayer::SendData_PartyMemberHP()
{
	if(!m_pPartyMgr)
		return;

	CPartyPlayer** ppMem = m_pPartyMgr->GetPtrPartyMember();
	if(!ppMem)
		return;

	_party_member_hp_upd Data;

	Data.dwMemSerial = m_dwObjSerial;
	Data.wHP = m_Param.GetHP();

	int nMemPop = m_pPartyMgr->GetPopPartyMember();
	BYTE byType[msg_header_num] = {party_msg, party_member_hp_upd};
//	x_sockaddr_in Addr(PORT_UDP_PARTY);

	for(int m = 0; m < nMemPop; m++)
	{
		if(ppMem[m] == m_pPartyMgr)
			continue;

	//	Addr.SetIP(ppMem[m]->m_ipAddr);
	//	g_Main.m_UDPNetwork.LoadSendMsg(sizeof(Data), byType, (char*)&Data, &Addr);
		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(ppMem[m]->m_wZoneIndex, byType, (char*)&Data, sizeof(Data));
	}
}

void CPlayer::SendData_PartyMemberFP()
{
	if(!m_pPartyMgr)
		return;

	CPartyPlayer** ppMem = m_pPartyMgr->GetPtrPartyMember();
	if(!ppMem)
		return;

	_party_member_fp_upd Data;

	Data.dwMemSerial = m_dwObjSerial;
	Data.wFP = m_Param.GetFP();

	int nMemPop = m_pPartyMgr->GetPopPartyMember();
	BYTE byType[msg_header_num] = {party_msg, party_member_fp_upd};
//	x_sockaddr_in Addr(PORT_UDP_PARTY);

	for(int m = 0; m < nMemPop; m++)
	{
		if(ppMem[m] == m_pPartyMgr)
			continue;

//		Addr.SetIP(ppMem[m]->m_ipAddr);
//		g_Main.m_UDPNetwork.LoadSendMsg(sizeof(Data), byType, (char*)&Data, &Addr);
		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(ppMem[m]->m_wZoneIndex, byType, (char*)&Data, sizeof(Data));
	}
}

void CPlayer::SendData_PartyMemberSP()
{
	if(!m_pPartyMgr)
		return;

	CPartyPlayer** ppMem = m_pPartyMgr->GetPtrPartyMember();
	if(!ppMem)
		return;

	_party_member_sp_upd Data;

	Data.dwMemSerial = m_dwObjSerial;
	Data.wSP = m_Param.GetSP();

	int nMemPop = m_pPartyMgr->GetPopPartyMember();
	BYTE byType[msg_header_num] = {party_msg, party_member_sp_upd};
//	x_sockaddr_in Addr(PORT_UDP_PARTY);

	for(int m = 0; m < nMemPop; m++)
	{
		if(ppMem[m] == m_pPartyMgr)
			continue;

//		Addr.SetIP(ppMem[m]->m_ipAddr);
//		g_Main.m_UDPNetwork.LoadSendMsg(sizeof(Data), byType, (char*)&Data, &Addr);
		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(ppMem[m]->m_wZoneIndex, byType, (char*)&Data, sizeof(Data));
	}
}

void CPlayer::SendData_PartyMemberLv()
{
	if(!m_pPartyMgr)
		return;

	CPartyPlayer** ppMem = m_pPartyMgr->GetPtrPartyMember();
	if(!ppMem)
		return;

	_party_member_lv_upd Data;

	Data.dwMemSerial = m_dwObjSerial;
	Data.byLv = m_Param.GetLevel();

	int nMemPop = m_pPartyMgr->GetPopPartyMember();
	BYTE byType[msg_header_num] = {party_msg, party_member_lv_upd};
//	x_sockaddr_in Addr(PORT_UDP_PARTY);

	for(int m = 0; m < nMemPop; m++)
	{
		if(ppMem[m] == m_pPartyMgr)
			continue;

	//	Addr.SetIP(ppMem[m]->m_ipAddr);
	//	g_Main.m_UDPNetwork.LoadSendMsg(sizeof(Data), byType, (char*)&Data, &Addr);
		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(ppMem[m]->m_wZoneIndex, byType, (char*)&Data, sizeof(Data));
	}
}

void CPlayer::SendData_PartyMemberPos()
{
	if(!m_pPartyMgr)
		return;

	CPartyPlayer** ppMem = m_pPartyMgr->GetPtrPartyMember();
	if(!ppMem)
		return;

	_party_member_pos_upd Data;

	Data.dwMemSerial = m_dwObjSerial;
	Data.byMapCode = m_Param.GetMapCode();
	Data.zPos[0] = (short)m_fCurPos[0];
	Data.zPos[1] = (short)m_fCurPos[2];
	
	int nMemPop = m_pPartyMgr->GetPopPartyMember();
	BYTE byType[msg_header_num] = {party_msg, party_member_pos_upd};

	for(int m = 0; m < nMemPop; m++)
	{
		if(ppMem[m] == m_pPartyMgr)
			continue;

		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(ppMem[m]->m_wZoneIndex, byType, (char*)&Data, sizeof(Data));
	}
}

void CPlayer::SendData_PartyMemberMaxHFSP()
{
	if(!m_pPartyMgr)
		return;

	CPartyPlayer** ppMem = m_pPartyMgr->GetPtrPartyMember();
	if(!ppMem)
		return;

	_party_member_max_hfsp_upd Data;

	Data.dwMemSerial = m_dwObjSerial;
	Data.wMaxHP = GetLimHP();
	Data.wMaxFP = GetLimFP();	
	Data.wMaxSP = GetLimSP();
	
	int nMemPop = m_pPartyMgr->GetPopPartyMember();
	BYTE byType[msg_header_num] = {party_msg, party_member_max_hfsp_upd};
//	x_sockaddr_in Addr(PORT_UDP_PARTY);

	for(int m = 0; m < nMemPop; m++)
	{
		if(ppMem[m] == m_pPartyMgr)
			continue;

//		Addr.SetIP(ppMem[m]->m_ipAddr);
//		g_Main.m_UDPNetwork.LoadSendMsg(sizeof(Data), byType, (char*)&Data, &Addr);
		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(ppMem[m]->m_wZoneIndex, byType, (char*)&Data, sizeof(Data));
	}
}

void CPlayer::SendData_PartyMemberEffect(BYTE byAlterCode, WORD wEffectCode, BYTE byLv)
{
	if(!m_pPartyMgr)
		return;

	CPartyPlayer** ppMem = m_pPartyMgr->GetPtrPartyMember();
	if(!ppMem)
		return;

	_party_member_effect_upd Data;

	Data.dwMemSerial = m_dwObjSerial;
	Data.byAlterCode = byAlterCode;
	Data.wEffectCode = wEffectCode;
	Data.byEffectLv = byLv;

	int nMemPop = m_pPartyMgr->GetPopPartyMember();
	BYTE byType[msg_header_num] = {party_msg, party_member_effect_upd};
//	x_sockaddr_in Addr(PORT_UDP_PARTY);

	for(int m = 0; m < nMemPop; m++)
	{
		if(ppMem[m] == m_pPartyMgr)
			continue;

//		Addr.SetIP(ppMem[m]->m_ipAddr);
//		g_Main.m_UDPNetwork.LoadSendMsg(sizeof(Data), byType, (char*)&Data, &Addr);
		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(ppMem[m]->m_wZoneIndex, byType, (char*)&Data, sizeof(Data));
	}
}

void CPlayer::SendMsg_AlterExpInform()
{
	_exp_alter_zocl Send;

	Send.wExpRate = m_wExpRate;

	BYTE byType[msg_header_num] = {event_msg, exp_alter_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_AlterItemDurInform(BYTE byStorageCode, WORD wItemSerial, DWORD dwDur)
{
	_alter_item_dur_zocl Send;

	Send.byStorageCode = byStorageCode;
	Send.wItemSerial = wItemSerial;
	Send.dwDur = dwDur;

	BYTE byType[msg_header_num] = {item_msg, alter_item_dur_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_ChangeClassCommand()
{
	_change_class_command_zocl Send;

	Send.wCurClassIndex = m_Param.m_pClassData->m_dwIndex;

	BYTE byType[msg_header_num] = {event_msg, change_class_command_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_SelectClassResult(BYTE byErrCode, WORD wSelClassIndex)
{
	_select_class_result_zocl Send;

	Send.byRetCode = byErrCode;

	BYTE byType[msg_header_num] = {event_msg, select_class_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));

	if(byErrCode == 0)	//주변에 알림..
	{
		_change_class_other_zocl Send;

		Send.dwPlayerSerial = m_dwObjSerial;
		Send.wClassIndex = wSelClassIndex;

		BYTE byType[msg_header_num] = {effect_msg, change_class_other_zocl};
		CircleReport(byType, (char*)&Send, sizeof(Send));
	}
}

void CPlayer::SendMsg_RewardAddItem(__ITEM* pItem)
{
	_reward_add_item_zocl Send;

	Send.byTableCode = pItem->m_byTableCode;
	Send.wItemIndex = pItem->m_wItemIndex;
	Send.dwDur = pItem->m_dwDur; 
	Send.dwLv = pItem->m_dwLv;
	Send.wItemSerial = pItem->m_wSerial;

	BYTE byType[msg_header_num] = {event_msg, reward_add_item_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_AlterWeaponBulletInform(WORD wItemSerial, WORD wLeftNum)
{
	_alter_weapon_bullet_inform_zocl Send;

	Send.wItemSerial = wItemSerial;
	Send.wLeftNum = wLeftNum;

	BYTE byType[msg_header_num] = {fight_msg, alter_weapon_bullet_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_AlterUnitBulletInform(BYTE byPart, WORD wLeftNum)
{
	_alter_unit_bullet_inform_zocl Send;

	Send.byPart = byPart;
	Send.wLeftNum = wLeftNum;

	BYTE byType[msg_header_num] = {fight_msg, alter_unit_bullet_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_ForceResult(BYTE byErrCode, _CHRID* pidDst, __ITEM* pForceItem, int nSFLv)
{
	_force_result_one_zocl SendOne;

	SendOne.byErrCode = byErrCode;
	SendOne.dwLeftFP = m_Param.GetFP();
	if(pForceItem)
		SendOne.dwItemCum = pForceItem->m_dwDur;

	BYTE byTypeOne[msg_header_num] = {effect_msg, force_result_one_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byTypeOne, (char*)&SendOne, sizeof(SendOne));

	if(byErrCode == 0)
	{
		_force_result_other_zocl SendOther;

		SendOther.byRetCode = 	byErrCode;
		memcpy(&SendOther.idDster, pidDst, sizeof(_CHRID));
		SendOther.idPerformer.byID = m_ObjID.m_byID;
		SendOther.idPerformer.wIndex = m_ObjID.m_wIndex;
		SendOther.idPerformer.dwSerial = m_dwObjSerial;
		if(!pForceItem)
			SendOther.byForceIndex = 0xFF;
		else
			SendOther.byForceIndex = pForceItem->m_wItemIndex;

		SendOther.byForceLv = (BYTE)nSFLv;

		BYTE byTypeOther[msg_header_num] = {effect_msg, force_result_other_zocl};
		CircleReport(byTypeOther, (char*)&SendOther, sizeof(SendOther));
	}
}

void CPlayer::SendMsg_SkillResult(BYTE byErrCode, _CHRID* pidDst, BYTE bySkillIndex, int nSFLv)
{
	_skill_result_one_zocl SendOne;

	SendOne.byErrCode = byErrCode;
	SendOne.dwLeftFP = m_Param.GetFP();

	BYTE byTypeOne[msg_header_num] = {effect_msg, skill_result_one_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byTypeOne, (char*)&SendOne, sizeof(SendOne));

	_skill_result_other_zocl SendOther;

	SendOther.byRetCode = byErrCode;
	memcpy(&SendOther.idDster, pidDst, sizeof(_CHRID));
	SendOther.idPerformer.byID = m_ObjID.m_byID;
	SendOther.idPerformer.wIndex = m_ObjID.m_wIndex;
	SendOther.idPerformer.dwSerial = m_dwObjSerial;
	SendOther.bySkillIndex = bySkillIndex;
	SendOther.bySkillLv = nSFLv;

	BYTE byTypeOther[msg_header_num] = {effect_msg, skill_result_other_zocl};
	CircleReport(byTypeOther, (char*)&SendOther, sizeof(SendOther));
}

void CPlayer::SendMsg_ClassSkillResult(BYTE byErrCode, _CHRID* pidDst, WORD wSkillIndex)
{
	_class_skill_result_one_zocl SendOne;

	SendOne.byErrCode = byErrCode;
	SendOne.wLeftFP = m_Param.GetFP();

	BYTE byTypeOne[msg_header_num] = {effect_msg, class_skill_result_one_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byTypeOne, (char*)&SendOne, sizeof(SendOne));

	_class_skill_result_other_zocl SendOther;

	SendOther.byRetCode = byErrCode;
	memcpy(&SendOther.idDster, pidDst, sizeof(_CHRID));
	SendOther.idPerformer.byID = m_ObjID.m_byID;
	SendOther.idPerformer.wIndex = m_ObjID.m_wIndex;
	SendOther.idPerformer.dwSerial = m_dwObjSerial;
	SendOther.wSkillIndex = wSkillIndex;

	BYTE byTypeOther[msg_header_num] = {effect_msg, class_skill_result_other_zocl};
	CircleReport(byTypeOther, (char*)&SendOther, sizeof(SendOther));
}

void CPlayer::SendMsg_SetFPInform()
{
	_set_fp_inform_zocl Send;

	Send.wFP = m_Param.GetFP();

	BYTE byType[msg_header_num] = {effect_msg, set_fp_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_SetHPInform()
{
	_set_hp_inform_zocl Send;

	Send.wHP = m_Param.GetHP();

	BYTE byType[msg_header_num] = {effect_msg, set_hp_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_SetSPInform()
{
	_set_sp_inform_zocl Send;

	Send.wSP = m_Param.GetSP();

	BYTE byType[msg_header_num] = {effect_msg, set_sp_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_InsertItemInform(BYTE byStorageCode, __ITEM* pItem)
{
	_insert_item_inform_zocl Send;

	Send.byStorageCode = byStorageCode;
	Send.byTableCode = pItem->m_byTableCode;
	Send.wItemIndex = pItem->m_wItemIndex;
	Send.dwDurPoint = pItem->m_dwDur;
	Send.wSerial = pItem->m_wSerial;

	BYTE byType[msg_header_num] = {effect_msg, insert_item_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_CreateTowerResult(BYTE byErrCode, DWORD dwTowerObjSerial)
{
	_make_tower_result_zocl Send;

	Send.byErrCode = byErrCode;
	Send.dwTowerObjSerial = dwTowerObjSerial;

	BYTE byType[msg_header_num] = {effect_msg, make_tower_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_BackTowerResult(BYTE byErrCode, WORD wItemSerial, WORD wLeftHP)
{
	_back_tower_result_zocl Send;

	Send.byErrCode = byErrCode;
	Send.wItemSerial = wItemSerial;
	Send.wLeftHP = wLeftHP;

	BYTE byType[msg_header_num] = {effect_msg, back_tower_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_AlterTowerHP(WORD wItemSerial, WORD wLeftHP)
{
	_alter_tower_hp_zocl Send;

	Send.wItemSerial = wItemSerial;
	Send.wLeftHP = wLeftHP;

	BYTE byType[msg_header_num] = {effect_msg, alter_tower_hp_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_DTradeAskResult(BYTE byErrCode)
{	
	_d_trade_ask_result_clzo Send;

	Send.byErrCode = byErrCode;

	BYTE byType[msg_header_num] = {dtrade_msg, d_trade_ask_result_clzo};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_DTradeAskInform(CPlayer* pAsker)
{
	_d_trade_ask_inform_zocl Send;

	Send.idAsker.wIndex = pAsker->m_ObjID.m_wIndex;
	Send.idAsker.dwSerial = pAsker->m_dwObjSerial;

	BYTE byType[msg_header_num] = {dtrade_msg, d_trade_ask_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_DTradeAnswerResult(BYTE byErrCode)
{
	_d_trade_answer_result_zocl Send;

	Send.byErrCode = byErrCode;

	BYTE byType[msg_header_num] = {dtrade_msg, d_trade_answer_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_DTradeStartInform(CPlayer* pAsker, CPlayer* pAnswer)
{
	_d_trade_start_inform_zocl Send;

	Send.idAsker.dwSerial = pAsker->m_dwObjSerial;
	Send.idAsker.wIndex = pAsker->m_ObjID.m_wIndex;
	Send.byAskerEmptyNum = pAsker->m_pmTrd.byEmptyInvenNum;
	Send.idAnswer.dwSerial = pAnswer->m_dwObjSerial;
	Send.idAnswer.wIndex = pAnswer->m_ObjID.m_wIndex;
	Send.byAnswerEmptyNum = pAnswer->m_pmTrd.byEmptyInvenNum;

	BYTE byType[msg_header_num] = {dtrade_msg, d_trade_start_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_DTradeCancleResult(BYTE byErrCode)
{
	_d_trade_cancle_result_zocl Send;

	Send.byErrCode = byErrCode;

	BYTE byType[msg_header_num] = {dtrade_msg, d_trade_cancle_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_DTradeCancleInform()
{
	_d_trade_cancle_inform_zocl Send;

	BYTE byType[msg_header_num] = {dtrade_msg, d_trade_cancle_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_DTradeLockResult(BYTE byErrCode)
{
	_d_trade_lock_result_zocl Send;

	Send.byErrCode = byErrCode;

	BYTE byType[msg_header_num] = {dtrade_msg, d_trade_lock_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_DTradeLockInform()
{
	_d_trade_lock_inform_zocl Send;

	BYTE byType[msg_header_num] = {dtrade_msg, d_trade_lock_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_DTradeAddResult(BYTE byErrCode)
{
	_d_trade_add_result_zocl Send;

	Send.byErrCode = byErrCode;

	BYTE byType[msg_header_num] = {dtrade_msg, d_trade_add_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_DTradeAddInform(BYTE bySlotIndex, __ITEM* pItem, BYTE byAmount)
{
	_d_trade_add_inform_zocl Send;

	Send.bySlotIndex = bySlotIndex;
	Send.byTableCode = pItem->m_byTableCode;
	Send.wItemIndex = pItem->m_wItemIndex;
	Send.dwDurPoint = pItem->m_dwDur;
	Send.dwUdtInfo = pItem->m_dwLv;
	Send.byAmount = byAmount;
	Send.byEmptyInvenNum = m_pmTrd.byEmptyInvenNum;

	BYTE byType[msg_header_num] = {dtrade_msg, d_trade_add_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_DTradeDelResult(BYTE byErrCode)
{
	_d_trade_del_result_zocl Send;

	Send.byErrCode = byErrCode;

	BYTE byType[msg_header_num] = {dtrade_msg, d_trade_del_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_DTradeDelInform(BYTE bySlotIndex)
{
	_d_trade_del_inform_zocl Send;

	Send.bySlotIndex = bySlotIndex;
	Send.byEmptyInvenNum = m_pmTrd.byEmptyInvenNum;

	BYTE byType[msg_header_num] = {dtrade_msg, d_trade_del_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_DTradeBetResult(BYTE byErrCode)
{
	_d_trade_bet_result_zocl Send;

	Send.byErrCode = byErrCode;

	BYTE byType[msg_header_num] = {dtrade_msg, d_trade_bet_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_DTradeBetInform(BYTE byUnitCode, DWORD dwAmount)
{
	_d_trade_bet_inform_zocl Send;

	Send.byMoneyUnit = byUnitCode;
	Send.dwBetAmount = dwAmount;

	BYTE byType[msg_header_num] = {dtrade_msg, d_trade_bet_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_DTradeOKResult(BYTE byErrCode)
{
	_d_trade_ok_result_zocl Send;

	Send.byErrCode = byErrCode;

	BYTE byType[msg_header_num] = {dtrade_msg, d_trade_ok_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_DTradeOKInform()
{
	_d_trade_ok_inform_zocl Send;	

	BYTE byType[msg_header_num] = {dtrade_msg, d_trade_ok_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_DTradeAccomplishInform(bool bSucc, WORD wStartSerial)
{
	_d_trade_accomplish_inform_zocl Send;

	Send.bSucc = bSucc;
	Send.dwDalant = m_Param.GetDalant();
	Send.dwGold = m_Param.GetGold();
	Send.wStartSerial = wStartSerial;

	BYTE byType[msg_header_num] = {dtrade_msg, d_trade_accomplish_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_DTradeCloseInform(BYTE byCloseCode)
{
	_d_trade_close_inform_zocl Send;

	Send.byCloseCode = byCloseCode;

	BYTE byType[msg_header_num] = {dtrade_msg, d_trade_close_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_DTradeUnitInfoInform(BYTE byTradeSlotIndex, _UNIT_DB_BASE::_LIST* pUnitData)
{
	_d_trade_unit_info_inform_zocl Send;

	Send.byTradeSlotIndex = byTradeSlotIndex;
	Send.byFrame = pUnitData->byFrame;
	Send.dwGauge = pUnitData->dwGauge;
	memcpy(Send.byPart, pUnitData->byPart, sizeof(BYTE)*UNIT_PART_NUM);
	memcpy(Send.dwBullet, pUnitData->dwBullet, sizeof(DWORD)*UNIT_BULLET_NUM);
	memcpy(Send.dwSpare, pUnitData->dwSpare, sizeof(DWORD)*max_unit_spare);
	Send.nDebtFee = pUnitData->nKeepingFee + pUnitData->nPullingFee;

	BYTE byType[msg_header_num] = {dtrade_msg, d_trade_unit_info_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_DTradeUnitAddInform(WORD wUnitKeySerial, _UNIT_DB_BASE::_LIST* pUnitData)
{
	_d_trade_unit_add_inform_zocl Send;

	Send.wUnitKeySerial = wUnitKeySerial;
	Send.bySlotIndex = pUnitData->bySlotIndex;
	Send.byFrame = pUnitData->byFrame;
	Send.dwGauge = pUnitData->dwGauge;
	memcpy(Send.byPart, pUnitData->byPart, sizeof(BYTE)*UNIT_PART_NUM);
	memcpy(Send.dwBullet, pUnitData->dwBullet, sizeof(DWORD)*UNIT_BULLET_NUM);
	memcpy(Send.dwSpare, pUnitData->dwSpare, sizeof(DWORD)*max_unit_spare);
	Send.nKeepingFee = pUnitData->nKeepingFee;
	Send.nPullingFee = pUnitData->nPullingFee;

	BYTE byType[msg_header_num] = {dtrade_msg, d_trade_unit_add_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_InsertNewQuest(BYTE bySlotIndex, DWORD dwEventIndex, BYTE byEventNodeIndex, DWORD dwQuestIndex)
{
	_insert_new_quest_inform_zocl Send;

	Send.bySlotIndex = bySlotIndex;
	Send.dwEventIndex = dwEventIndex;
	Send.byEventNodeIndex = byEventNodeIndex;
	Send.dwQuestIndex = dwQuestIndex;

	BYTE byType[msg_header_num] = {quest_msg, insert_new_quest_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_SelectWaitedQuest(DWORD dwEventIndex, BYTE byEventNodeIndex)
{
	_select_waited_quest_command_zocl Send;

	Send.dwEventIndex = dwEventIndex;
	Send.byEventNodeIndex = byEventNodeIndex;

	BYTE byType[msg_header_num] = {quest_msg, select_waited_quest_command_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_InsertQuestFailure(DWORD dwEventIndex, BYTE byEventNodeIndex)
{
	_insert_quest_failure_zocl Send;

	Send.dwEventIndex = dwEventIndex;
	Send.byEventNodeIndex = byEventNodeIndex;

	BYTE byType[msg_header_num] = {quest_msg, insert_quest_failure_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_QuestComplete(BYTE byQuestDBSlot)
{
	_quest_complete_inform_zocl Send;

	Send.byQuestDBSlot = byQuestDBSlot;

	BYTE byType[msg_header_num] = {quest_msg, quest_complete_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_QuestProcess(BYTE byQuestDBSlot, BYTE byActIndex, WORD wCount)
{
	_quest_process_inform_zocl Send;

	Send.byQuestDBSlot = byQuestDBSlot;
	Send.byActIndex = byActIndex;
	Send.wCount = wCount;

	BYTE byType[msg_header_num] = {quest_msg, quest_process_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_QuestFailure(BYTE byFailCode, BYTE byQuestDBSlot)
{
	_quest_fail_inform_zocl Send;

	Send.byFailCode = byFailCode;
	Send.byQuestDBSlot = byQuestDBSlot;

	BYTE byType[msg_header_num] = {quest_msg, quest_fail_inform_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CPlayer::SendMsg_StartShape_Msgr()
{
	_start_shape_inform_wrmg Send;

	memcpy(&Send.idLocal, &m_pUserDB->m_idMessenger, sizeof(_CLID));
	Send.byDftPart_Face = m_Param.m_dbChar.m_byDftPart_Face;
	for(int i = 0; i < base_fix_num; i++)
		Send.byDftPart[i] = m_Param.m_dbChar.m_byDftPart[i];
	for(i = 0; i < equip_fix_num; i++)
	{
		if(m_Param.m_dbEquip.m_pStorageList[i].m_bLoad)
		{
			Send.wEquipPart[i] = ::GetItemUpgedLv(m_Param.m_dbEquip.m_pStorageList[i].m_dwLv);
			Send.wEquipPart[i] <<= 12;
			Send.wEquipPart[i] |= m_Param.m_dbEquip.m_pStorageList[i].m_wItemIndex;
		}
		else
			Send.wEquipPart[i] = 0xFFFF;
	}
	Send.wVisualVer = GetVisualVer();

	g_Main.m_Network.m_pProcess[messenger_line]->LoadSendMsg(m_ObjID.m_wIndex, Send.head(), (char*)&Send, Send.size());
}

void CPlayer::SendMsg_UpdateEquip_Msgr(BYTE	byPartCode)
{
	_update_equip_inform_wrmg Send;

	memcpy(&Send.idLocal, &m_pUserDB->m_idMessenger, sizeof(_CLID));
	Send.wVisualVer = GetVisualVer();
	Send.byPartCode = byPartCode;
	if(m_Param.m_dbEquip.m_pStorageList[byPartCode].m_bLoad)
	{
		Send.wEquipPart = ::GetItemUpgedLv(m_Param.m_dbEquip.m_pStorageList[byPartCode].m_dwLv);
		Send.wEquipPart <<= 12;
		Send.wEquipPart |= m_Param.m_dbEquip.m_pStorageList[byPartCode].m_wItemIndex;
	}
	else
		Send.wEquipPart = 0xFFFF;

	g_Main.m_Network.m_pProcess[messenger_line]->LoadSendMsg(m_ObjID.m_wIndex, Send.head(), (char*)&Send, Send.size());
}