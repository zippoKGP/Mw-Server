#include "stdafx.h"
#include "Player.h"
#include "MainThread.h"
#include "ForceFld.h"
#include "SkillFld.h"
#include "ClassFld.h"
#include "ErrorEventFromServer.h"
#include "CharacterDB.h"

#define	dst_anyone				0
#define	dst_same_race			1
#define	dst_enemy				2
#define	dst_mob					3
#define	dst_map					4	

void CPlayer::pc_ForceRequest(WORD wForceSerial, _CHRID* pidDst)
{
	BYTE byErrCode = 0;
	__ITEM* pForceItem = NULL;
	_force_fld* pForceFld = NULL;
	CCharacter* pDst = NULL;
	WORD wLeftFP = 0;
	int nSFLv;
	DWORD dwCurTime = timeGetTime();

	if(IsRidingUnit())
	{
		byErrCode = error_sf_riding_unit;	//유닛탑승상태
		goto RESULT;
	}

	//포스아이템확인
	pForceItem = m_Param.m_dbForce.GetPtrFromSerial(wForceSerial);
	if(!pForceItem)
	{
		byErrCode = error_sf_no_have_force;//없는포스아이템
		goto RESULT;
	}
	pForceFld = (_force_fld*)g_Main.m_tblEffectData[effect_code_force].GetRecord(s_pnLinkForceItemToEffect[pForceItem->m_wItemIndex]);
	if(!pForceFld)
	{
		byErrCode = error_sf_no_have_force;//없는포스아이템
		goto RESULT;
	}
	
	//시전대상확인
	pDst = (CCharacter*)g_Main.GetObject(obj_kind_char, pidDst->byID, pidDst->wIndex);	
	if(!pDst)
	{
		byErrCode = error_sf_no_exist_dst;//없는대상
		goto RESULT;
	}

	//시전대상확인
	if(!pDst->m_bLive || pDst->m_pCurMap != m_pCurMap)
	{
		byErrCode = error_sf_no_exist_dst;
		goto RESULT;
	}

	if(pForceFld->m_nTempEffectType == -1 && pForceFld->m_nContEffectType == -1)
	{
		byErrCode = error_sf_data_fail;	//일시, 지속 둘중 하나도 아님
		goto RESULT;		
	}

	if(pForceFld->m_nContEffectType != -1)
	{
		if(!pDst->IsRecvableContEffect())
		{
			byErrCode = error_sf_non_recvalbe_dst;	//지속효과 시전불가능한 대상
			goto RESULT;
		}
	}

	if(!IsSFUsableSFMastery(mastery_code_force, pForceFld->m_nMastIndex, pForceFld->m_nLv))
	{
		byErrCode = error_sf_shortage_mastery;//마스터리부족
		goto RESULT;
	}

	if(!IsSFUseableRace(effect_code_force, pForceFld->m_dwIndex))
	{
		byErrCode = error_sf_not_actable_race;//사용가능종족아님
		goto RESULT;
	}

	if(!IsSFUseableTarget(effect_code_force, pForceFld->m_dwIndex, pDst))
	{
		byErrCode = error_sf_not_actable_dst;//시전대상이 올바르지않음
		goto RESULT;
	}

	if(!IsSFUsableFP(effect_code_force, pForceFld->m_dwIndex, &wLeftFP))
	{
		byErrCode = error_sf_shortage_fp;//FP부족
		goto RESULT;
	}

	//시전딜레이 체크
	if(dwCurTime - m_pdwLastForceActTime[pForceFld->m_dwIndex] < pForceFld->m_fActDelay - 300)
	{
		byErrCode = error_sf_act_delay;
		goto RESULT;
	}
	else
		m_pdwLastForceActTime[pForceFld->m_dwIndex] = dwCurTime;

	//효과레벨
	nSFLv = m_pmMst.GetSFLevel(pForceFld->m_nLv, pForceItem->m_dwDur);

RESULT:

	if(byErrCode == 0)
	{
		bool bUpMty = false;
		if(CCharacter::AssistForce(pDst, pForceFld, nSFLv, &byErrCode, &bUpMty))
		{	//데이터 갱신..
			SetFP(wLeftFP, true);
		}
		if(bUpMty)	
		{//마스터리갱신
			if((DWORD)pForceFld->m_nMastIndex < force_mastery_num)
				Emb_AlterStat(mastery_code_force, pForceFld->m_nMastIndex, 1);
			DWORD dwNewStat = Emb_AlterDurPoint(pForceItem->m_pInList->m_nListCode, pForceItem->m_byStorageIndex, 1, false);//누적
			SendMsg_FcitemInform(wForceSerial, dwNewStat);		
		}
	}

	SendMsg_ForceResult(byErrCode, pidDst, pForceItem, nSFLv);
}

void CPlayer::pc_SkillRequest(BYTE bySkillIndex, _CHRID* pidDst)
{
	int nSFLv = 1;
	BYTE byErrCode = skill_process(effect_code_skill, bySkillIndex, pidDst, &nSFLv);

	SendMsg_SkillResult(byErrCode, pidDst, bySkillIndex, nSFLv);
}

void CPlayer::pc_ClassSkillRequest(WORD wSkillIndex, _CHRID* pidDst)
{
	BYTE byErrCode = skill_process(effect_code_class, wSkillIndex, pidDst, NULL);

	SendMsg_ClassSkillResult(byErrCode, pidDst, wSkillIndex);
}
	
BYTE CPlayer::skill_process(int nEffectCode, int nSkillIndex, _CHRID* pidDst, int* OUT pnLv)
{
	_skill_fld* pSkillFld = (_skill_fld *)g_Main.m_tblEffectData[nEffectCode].GetRecord(nSkillIndex);
	WORD wLeftFP = 0;
	int nSFLv = 1;
	CCharacter* pDst = NULL;
	DWORD dwCurTime = timeGetTime();

	if(nEffectCode == effect_code_skill)
	{
		if((DWORD)pSkillFld->m_nMastIndex > skill_mastery_num)
		{
			g_Main.m_logSystemError.Write("data error: 잘못된 스킬 마스터리..(%d스킬의 %d마스터리)", nSkillIndex, pSkillFld->m_nMastIndex);
			return error_sf_data_fail;
		}
	}
	else
	{	//자신이 쓸수있는 클래스인가..
		if(!m_Param.IsActableClassSkill(pSkillFld->m_strCode))
			return error_sf_no_use_calss;
	}

	if(IsRidingUnit())
	{
		return error_sf_riding_unit;	//유닛탑승상태		
	}

	//시전대상확인
	pDst = (CCharacter*)g_Main.GetObject(obj_kind_char, pidDst->byID, pidDst->wIndex);	
	if(!pDst)
		return error_sf_no_exist_dst;//없는대상

	//시전대상확인
	if(!pDst->m_bLive || pDst->m_pCurMap != m_pCurMap)
		return error_sf_no_exist_dst;

	if(pSkillFld->m_nTempEffectType == -1 && pSkillFld->m_nContEffectType == -1)
		return error_sf_data_fail;	//일시, 지속 둘중 하나도 아님

	if(pSkillFld->m_nContEffectType != -1)
	{
		if(!pDst->IsRecvableContEffect())
			return error_sf_non_recvalbe_dst;	//지속효과 시전불가능한 대상
	}

	if(nEffectCode == effect_code_skill)
	{
		if(!IsSFUsableSFMastery(nEffectCode, pSkillFld->m_nMastIndex, pSkillFld->m_nLv))
			return error_sf_shortage_mastery;//마스터리부족
	}

	if(!IsSFUseableRace(nEffectCode, pSkillFld->m_dwIndex))
		return error_sf_not_actable_race;//사용가능종족아님

	if(!IsSFUseableTarget(nEffectCode, pSkillFld->m_dwIndex, pDst))
		return error_sf_not_actable_dst;//시전대상이 올바르지않음

	if(!IsSFUsableFP(nEffectCode, pSkillFld->m_dwIndex, &wLeftFP))
		return error_sf_shortage_fp;//FP부족

	//시전딜레이 체크
	if(nEffectCode == effect_code_skill)
	{
		if(dwCurTime - m_pdwLastSkillActTime[pSkillFld->m_dwIndex] < pSkillFld->m_fActDelay - 300)
			return error_sf_act_delay;
		else
			m_pdwLastSkillActTime[pSkillFld->m_dwIndex] = dwCurTime;
	}

	//효과레벨
	if(nEffectCode == effect_code_skill)
		nSFLv = m_pmMst.GetSkillLv(nSkillIndex) + m_EP.GetEff_Plus(_EFF_PLUS::SK_LvUp); //## PARAMETER EDIT (스킬_레벨증가)

	if(pnLv)
		*pnLv = nSFLv;

	//스킬시전..
	BYTE byErrCode = 0;
	bool bUpMty = false;
	if(CCharacter::AssistSkill(pDst, nEffectCode, pSkillFld, nSFLv, &byErrCode, &bUpMty))
	{	//fp감소			
		SetFP(wLeftFP, true);
	}	
	
	//마스터리갱신
	if(bUpMty && nEffectCode == effect_code_skill)	//일반스킬만 마스터리갱신..
	{
		if((DWORD)pSkillFld->m_nMastIndex < skill_mastery_num)
		{
			int nAddCum = 1;
			if(pSkillFld->m_nClass == sk_class_bow || pSkillFld->m_nClass == sk_class_firearm || pSkillFld->m_nClass == sk_class_throw)
				nAddCum = 2;//3개의 스킬계열은 2배로

			Emb_AlterStat(mastery_code_skill, pSkillFld->m_dwIndex, nAddCum);
		}
	}

	return byErrCode;
}
		
void CPlayer::pc_MakeTowerRequest(WORD wSkillIndex, WORD wTowerItemSerial, BYTE byMaterialNum, _make_tower_request_clzo::__material* pMaterial, float* pfPos)
{
	BYTE byErrCode = 0;
	_GuardTowerItem_fld* pTowerFld = NULL;
	__ITEM* pTowerItem = NULL;
	__ITEM* pMaterialItem[_make_tower_request_clzo::max_material] = {0, };
	BYTE	byMtrBufferNum[max_tower_material] = {0, };
	_skill_fld* pSkillFld = (_skill_fld*)g_Main.m_tblEffectData[effect_code_class].GetRecord(wSkillIndex);
	int i;

	//남은 가드타워가 잇는지
	if(CGuardTower::GetEmpty() == 0)
	{
		byErrCode = error_tower_full_tower;
		goto RESULT;
	}

	//자신이 지을수있는 제한수가 넘는지..
	if(m_pmTwr.IsEmpty() == 0)
	{
		byErrCode = error_tower_limit_num;
		goto RESULT;
	}

	//가드타워 클래스 인덱스가 맞는지..
	if(pSkillFld->m_nTempEffectType != -1)
	{
		SFTempEffectFunc pFunc = g_TempEffectFunc[pSkillFld->m_nTempEffectType];
		if(pFunc != DE_MakeGuardTower)
		{
			byErrCode = error_tower_data_fail;
			goto RESULT;
		}
	}
	else
	{
		byErrCode = error_tower_data_fail;
		goto RESULT;
	}

	//자신이 사용가능한건지 체크
	if(!m_Param.IsActableClassSkill(pSkillFld->m_strCode))
	{
		byErrCode = error_tower_no_act_skill;
		goto RESULT;
	}

	//타워아이템 체크
	pTowerItem = m_Param.m_dbInven.GetPtrFromSerial(wTowerItemSerial);
	if(!pTowerItem)
	{
		byErrCode = error_tower_no_have_item;
		goto RESULT;
	}
	//타워아이템맞는지..
	if(pTowerItem->m_byTableCode != tbl_code_tower)
	{
		byErrCode = error_tower_no_tower_item;
		goto RESULT;
	}
	pTowerFld = (_GuardTowerItem_fld*)g_Main.m_tblItemData[tbl_code_tower].GetRecord(pTowerItem->m_wItemIndex);
	
	//재료의 중복 및 유무 체크..
	for(i = 0; i < byMaterialNum; i++)
	{
		WORD wSrc = pMaterial[i].wItemSerial;

		//중복해서 올린게 있는지..
		for(int j = 0; j < i; j++)
		{
			if(wSrc == pMaterial[j].wItemSerial)
			{
				byErrCode = error_tower_overlap_material;
				goto RESULT;
			}
		}		

		//실재 재료가 있는가..
		pMaterialItem[i] = m_Param.m_dbInven.GetPtrFromSerial(wSrc);
		if(!pMaterialItem[i])
		{
			byErrCode = error_tower_no_have_material;
			goto RESULT;
		}

		//선택재료가 정해진 재료인가..
		_base_fld* pMaterialFld = g_Main.m_tblItemData[pMaterialItem[i]->m_byTableCode].GetRecord(pMaterialItem[i]->m_wItemIndex);
		if(strcmp(pMaterialFld->m_strCode, pTowerFld->m_Material[pMaterial[i].byMaterSlotIndex].strMaterialCode))
		{
			byErrCode = error_tower_no_match_material;
			goto RESULT;
		}

		//재고수량과 맞는가..
		if(pMaterial[i].byAmount > pMaterialItem[i]->m_dwDur)
		{
			byErrCode = error_tower_lack_material;
			goto RESULT;
		}

		byMtrBufferNum[pMaterial[i].byMaterSlotIndex] += pMaterial[i].byAmount;
	}	

	//수량체크
	for(i = 0; i < max_tower_material; i++)
	{
		if(byMtrBufferNum[i] != pTowerFld->m_Material[i].nMaterialNum)
		{
			byErrCode = error_tower_no_match_amount;
			goto RESULT;
		}
	}

	//위치체크 40이내..
	if(::GetSqrt(m_fCurPos, pfPos) > 40)
	{
		byErrCode = error_tower_loge_distance;
		goto RESULT;
	}

RESULT:

	DWORD dwTowerObjSerial = 0xFFFFFFFF;
	if(byErrCode == 0)
	{
		//가드타워생성..
		CGuardTower* pTwr = ::CreateGuardTower(m_pCurMap, m_wMapLayerIndex, pfPos, pTowerItem, this);
		if(!pTwr)
		{
			byErrCode = error_tower_full_tower;
		}
		else
		{//재료제거		
			for(i = 0; i < byMaterialNum; i++)
				Emb_AlterDurPoint(_STORAGE_POS::INVEN, pMaterialItem[i]->m_byStorageIndex, -pMaterial[i].byAmount, false, false); 				

			//타워아이템에 락을 건다.
			m_Param.m_dbInven.SetLock(pTowerItem->m_byStorageIndex, true);

			//타워아이템관리자에 알림..
			m_pmTwr.PushList(pTowerItem, pTwr);

			dwTowerObjSerial = pTwr->m_dwObjSerial;
		}
	}

	SendMsg_CreateTowerResult(byErrCode, dwTowerObjSerial);
}

void CPlayer::pc_BackTowerRequest(DWORD dwTowerObjSerial)
{
	BYTE byErrCode = 0;
	__ITEM* pTowerItem = NULL;

	//자신이 설치한 가드타워인가..
	for(int i = 0; i < _TOWER_PARAM::max_param; i++)
	{
		if(!m_pmTwr.m_List[i].m_pTowerItem)
			continue;

		if(m_pmTwr.m_List[i].m_pTowerObj->m_dwObjSerial == dwTowerObjSerial)
		{
			__ITEM* pItem = m_pmTwr.m_List[i].m_pTowerItem;

			//타워아이템 체크
			pTowerItem = m_Param.m_dbInven.GetPtrFromSerial(pItem->m_wSerial);
			if(!pTowerItem)
			{
				byErrCode = error_tower_no_have_item;
				goto RESULT;
			}
			//타워아이템맞는지..
			if(pTowerItem->m_byTableCode != tbl_code_tower)
			{
				byErrCode = error_tower_no_tower_item;
				goto RESULT;
			}
			break;
		}
	}

RESULT:

	WORD wItemSerial = 0xFFFF;
	WORD wLeftHP = 0;
	if(byErrCode == 0)
	{
		wLeftHP = _TowerReturn(pTowerItem);
		wItemSerial = pTowerItem->m_wSerial;
	}

	SendMsg_BackTowerResult(byErrCode, wItemSerial, wLeftHP);
}

// 자신의 HP를 대상의 FP에 더한다
bool CPlayer::F_AttHPtoDstFP_Once(CCharacter* pDstObj)
{
	bool bSucc = false;

	if(GetHP() == 0)
		goto RESULT;

	if(pDstObj->m_ObjID.m_byID != obj_id_player)
		goto RESULT;

	bSucc = true;

RESULT:

	if(bSucc)
	{
		// 자신의 HP를 1만 남기고 나머지를 대상의 FP에 더한다
		int nAlterFP = GetHP()-1;
		int nOldFP = ((CPlayer *)pDstObj)->GetFP();

		((CPlayer *)pDstObj)->SetFP(nOldFP + nAlterFP, false);			
		SetHP(1, false);

		//자신에게 HP알림..
		SendMsg_SetHPInform();

		//메크로 방지검사
		if(nOldFP == ((CPlayer *)pDstObj)->GetFP())
			return false;
		
		//대상에게 FP알림..
		((CPlayer *)pDstObj)->SendMsg_SetFPInform();

		return true;
	}

	return false;
}

// 지속 피해보조 남은 시간 늘이기
bool CPlayer::F_ContDamageTimeInc_Once(CCharacter* pDstObj, float fEffectValue)
{
	int nCnt = 0;

	DWORD dwCurTime = timeGetTime();

	for(int i = 0; i < CONT_SF_SIZE; i++)
	{
		_sf_continous* pCon = &pDstObj->m_SFCont[sf_cont_code_damage][i];
		if(!pCon->m_bExist)
			continue;

		_base_fld* pFld = g_Main.m_tblEffectData[pCon->m_byEffectCode].GetRecord(pCon->m_wEffectIndex);
		DWORD dwEffTime = 0;
		if(pCon->m_byEffectCode == effect_code_skill)
			dwEffTime = ((_skill_fld*)pFld)->m_nContEffectSec[pCon->m_byLv-1];
		else if(pCon->m_byEffectCode == effect_code_force)
			dwEffTime = ((_force_fld*)pFld)->m_nContEffectSec[pCon->m_byLv-1];

		pCon->m_wDurSec += (dwEffTime*fEffectValue);

		nCnt++;
	}	

	//메크로 방지검사
	if(nCnt > 0)
		return true;
	
	return false;
}

// 죽은 유저를 재생한다.
bool CPlayer::F_Resurrect_Once(CCharacter* pDstObj)
{
	bool bSucc = false;

	if(pDstObj->m_ObjID.m_byID != obj_id_player)
		goto RESULT;

	if(pDstObj->GetHP() == 0)
		goto RESULT;

	bSucc = true;

RESULT:

	if(bSucc)
	{
		((CPlayer *)pDstObj)->Resurrect();//재생메시지..
		return true;
	}

	return false;
}

// 현재 HP증가
bool CPlayer::F_HPInc_Once(CCharacter* pDstObj, float fEffectValue)
{
	bool bSucc = false;

	if(pDstObj->m_ObjID.m_byID != obj_id_player)
		goto RESULT;

	bSucc = true;

RESULT:

	if(bSucc)
	{
		int nOldHP = pDstObj->GetHP();
		int nMaxHP = ((CPlayer *)pDstObj)->GetMaxHP();

		pDstObj->SetHP(nOldHP + nMaxHP * fEffectValue, false);

		//메크로 방지검사
		if(nOldHP == pDstObj->GetHP())
			return false;	

		//hp일림..
		pDstObj->SendMsg_SetHPInform();

		return true;
	}
	return false;
}

// 현재 ST증가
bool CPlayer::F_STInc_Once(CCharacter* pDstObj, float fEffectValue)
{
	bool bSucc = false;

	if(pDstObj->m_ObjID.m_byID != obj_id_player)
		goto RESULT;

	bSucc = true;

RESULT:

	if(bSucc)
	{
		int nOldSP = ((CPlayer *)pDstObj)->GetSP();
		int nMaxSP = ((CPlayer *)pDstObj)->GetMaxSP();

		((CPlayer *)pDstObj)->SetSP(nOldSP + nMaxSP * fEffectValue, false);

		//메크로 방지검사
		if(nOldSP == ((CPlayer *)pDstObj)->GetSP())
			return false;	

		//ST알림..
		((CPlayer *)pDstObj)->SendMsg_SetSPInform();

		return true;
	}

	return false;
}

// 지속 이득보조의 남은 시간 늘리기	
bool CPlayer::F_ContHelpTimeInc_Once(CCharacter* pDstObj, float fEffectValue)
{
	int nCnt = 0;

	DWORD dwCurTime = timeGetTime();

	for(int i = 0; i < CONT_SF_SIZE; i++)
	{
		_sf_continous* pCon = &pDstObj->m_SFCont[sf_cont_code_help][i];
		if(!pCon->m_bExist)
			continue;

		_base_fld* pFld = g_Main.m_tblEffectData[pCon->m_byEffectCode].GetRecord(pCon->m_wEffectIndex);
		DWORD dwEffTime = 0;
		if(pCon->m_byEffectCode == effect_code_skill)
			dwEffTime = ((_skill_fld*)pFld)->m_nContEffectSec[pCon->m_byLv-1];
		else if(pCon->m_byEffectCode == effect_code_force)
			dwEffTime = ((_force_fld*)pFld)->m_nContEffectSec[pCon->m_byLv-1];

		pCon->m_wDurSec += (dwEffTime*fEffectValue);

		nCnt++;
	}	
	
	//메크로 방지검사
	if(nCnt > 0)
		return true;

	return false;
}

// overhealing
bool CPlayer::F_OverHealing_Once(CCharacter* pDstObj, float fEffectValue)
{
	bool bSucc = false;

	if(pDstObj->m_ObjID.m_byID != obj_id_player)
		goto RESULT;

	bSucc = true;

RESULT:
	
	if(bSucc)
	{
		int nOldHP = pDstObj->GetHP();
		int nMaxHP = ((CPlayer *)pDstObj)->GetMaxHP();

		pDstObj->SetHP(nOldHP + nMaxHP * fEffectValue, true);

		//메크로 방지검사
		if(nOldHP == pDstObj->GetHP())
			return false;	

		//HP알리기..
		pDstObj->SendMsg_SetHPInform();

		return true;
	}

	return false;
}

// 가장 최근 지속이득보조 스킬 해제	
bool CPlayer::F_LateContHelpSkillRemove_Once(CCharacter* pDstObj)
{
	int nLastIndex = -1;
	_sf_continous* pLast = NULL;

	for(int i = 0; i < CONT_SF_SIZE; i++)
	{
		_sf_continous* p = &pDstObj->m_SFCont[sf_cont_code_help][i];
		if(!p->m_bExist)
			continue;

		if(p->m_byEffectCode != attack_type_skill)
			continue;

		if(nLastIndex == -1)
		{
			nLastIndex = i;
			pLast = p;
			continue;
		}

		else if(p->m_wStartSec > pLast->m_wStartSec)
		{
			nLastIndex = i;
			pLast = p;
		}
	}

	if(nLastIndex != -1)
	{
		pDstObj->RemoveSFContEffect(sf_cont_code_help, nLastIndex);

		//메크로 방지검사
		return true;
	}

	return false;
}

// 가장 최근 지속이득보조 포스 해제
bool CPlayer::F_LateContHelpForceRemove_Once(CCharacter* pDstObj)
{
	int nLastIndex = -1;
	_sf_continous* pLast = NULL;

	for(int i = 0; i < CONT_SF_SIZE; i++)
	{
		_sf_continous* p = &pDstObj->m_SFCont[sf_cont_code_help][i];
		if(!p->m_bExist)
			continue;

		if(p->m_byEffectCode != attack_type_force)
			continue;

		if(nLastIndex == -1)
		{
			nLastIndex = i;
			pLast = p;
			continue;
		}

		else if(p->m_wStartSec > pLast->m_wStartSec)
		{
			nLastIndex = i;
			pLast = p;
		}
	}

	if(nLastIndex != -1)
	{
		pDstObj->RemoveSFContEffect(sf_cont_code_help, nLastIndex);

		//메크로 방지검사
		return true;
	}

	return false;
}

// 가장 최근 지속피해보조 포스 해제
bool CPlayer::F_LateContDamageForceRemove_Once(CCharacter* pDstObj)
{
	int nLastIndex = -1;
	_sf_continous* pLast = NULL;

	for(int i = 0; i < CONT_SF_SIZE; i++)
	{
		_sf_continous* p = &pDstObj->m_SFCont[sf_cont_code_damage][i];
		if(!p->m_bExist)
			continue;

		if(p->m_byEffectCode != attack_type_force)
			continue;

		if(nLastIndex == -1)
		{
			nLastIndex = i;
			pLast = p;
			continue;
		}

		else if(p->m_wStartSec > pLast->m_wStartSec)
		{
			nLastIndex = i;
			pLast = p;
		}
	}

	if(nLastIndex != -1)
	{
		pDstObj->RemoveSFContEffect(sf_cont_code_help, nLastIndex);

		//메크로 방지검사
		return true;
	}

	return false;
}

// 대상에 걸려있는 지속이득보조 스킬 제거
bool CPlayer::F_AllContHelpSkillRemove_Once(CCharacter* pDstObj)
{
	int nCnt = 0;
	for(int i = 0; i < CONT_SF_SIZE; i++)
	{
		_sf_continous* p = &pDstObj->m_SFCont[sf_cont_code_help][i];
		if(!p->m_bExist)
			continue;

		if(p->m_byEffectCode != attack_type_skill)
			continue;

		pDstObj->RemoveSFContEffect(sf_cont_code_help, i);
		nCnt++;		
	}

	//메크로 방지검사
	if(nCnt > 0)
		return true;

	return false;
}

// 대상에 걸려있는 지속이득보조 포스 제거
bool CPlayer::F_AllContHelpForceRemove_Once(CCharacter* pDstObj)
{
	int nCnt = 0;
	for(int i = 0; i < CONT_SF_SIZE; i++)
	{
		_sf_continous* p = &pDstObj->m_SFCont[sf_cont_code_help][i];
		if(!p->m_bExist)
			continue;

		if(p->m_byEffectCode != attack_type_force)
			continue;

		pDstObj->RemoveSFContEffect(sf_cont_code_help, i);
		nCnt++;		
	}
	
	//메크로 방지검사
	if(nCnt > 0)
		return true;

	return false;
}

// 대상에 걸려있는 지속 피해 보조 포스 해제
bool CPlayer::F_AllContDamageForceRemove_Once(CCharacter* pDstObj)
{
	int nCnt = 0;
	for(int i = 0; i < CONT_SF_SIZE; i++)
	{
		_sf_continous* p = &pDstObj->m_SFCont[sf_cont_code_damage][i];
		if(!p->m_bExist)
			continue;

		if(p->m_byEffectCode != attack_type_force)
			continue;

		pDstObj->RemoveSFContEffect(sf_cont_code_damage, i);
		nCnt++;		
	}

	//메크로 방지검사
	if(nCnt > 0)
		return true;

	return false;
}

// 시전자 주위 다른 종족들의 지속 이득 보조 스킬 & 포스 해제
bool CPlayer::F_OthersContHelpSFRemove_Once(float fEffectValue)
{
	int nCnt = 0;

	_pnt_rect l_rtArea;
	m_pCurMap->GetRectInRadius(&l_rtArea, fEffectValue, GetCurSecNum());

	int h, w, l_nSecNum;
	_object_list_point* l_pObjPtr;
	CGameObject* l_pObj;

	for(h = l_rtArea.nStarty; h < l_rtArea.nEndy; h++)
	{
		for(w = l_rtArea.nStartx; w < l_rtArea.nEndx; w++)
		{
			l_nSecNum = w+h*m_pCurMap->GetSecInfo()->m_nSecNumW;

			CObjectList* pList = m_pCurMap->GetSectorListObj(m_wMapLayerIndex, l_nSecNum);
			if(!pList)
				continue;
			
			l_pObjPtr = pList->m_Head.m_pNext;
			while(l_pObjPtr != &pList->m_Tail)
			{
				l_pObj = l_pObjPtr->m_pItem;
				l_pObjPtr = l_pObjPtr->m_pNext;

				// 자신이거나 item이면 skip
				if(l_pObj == this)
					continue;

				if(l_pObj->m_ObjID.m_byKind != obj_kind_char)
					continue;

				if(l_pObj->m_ObjID.m_byID != obj_id_player)
					continue;
					
				int l_nDistToAttChar = ::GetSqrt(m_fCurPos, l_pObj->m_fCurPos);
				if(l_nDistToAttChar > fEffectValue)
					continue;
				
				// 해제 함수를 호출하고 모두 지운다.
				for(int i = 0; i < CONT_SF_SIZE; i++)
				{
					if(((CCharacter*)l_pObj)->m_SFCont[sf_cont_code_help][i].m_bExist)
					{
						((CCharacter*)l_pObj)->RemoveSFContEffect(sf_cont_code_help, i);
						nCnt++;
					}
				}				
			}
		}
	}	

	//메크로 방지검사
	if(nCnt > 0)
		return true;

	return false;
}

//스킬..일시..

// skill에 의한 지속적인 이득 남은 시간 늘이기
bool CPlayer::S_SkillContHelpTimeInc_Once(CCharacter* pDstObj, float fEffectValue)
{
	int nCnt = 0;

	DWORD dwCurTime = timeGetTime();

	for(int i = 0; i < CONT_SF_SIZE; i++)
	{
		_sf_continous* pCon = &pDstObj->m_SFCont[sf_cont_code_help][i];
		if(!pCon->m_bExist)
			continue;

		if(pCon->m_byEffectCode != effect_code_skill)
			continue;

		_base_fld* pFld = g_Main.m_tblEffectData[pCon->m_byEffectCode].GetRecord(pCon->m_wEffectIndex);
		DWORD dwEffTime = 0;
		if(pCon->m_byEffectCode == effect_code_skill)
			dwEffTime = ((_skill_fld*)pFld)->m_nContEffectSec[pCon->m_byLv-1];
		else if(pCon->m_byEffectCode == effect_code_force)
			dwEffTime = ((_force_fld*)pFld)->m_nContEffectSec[pCon->m_byLv-1];

		pCon->m_wDurSec += (dwEffTime*fEffectValue);

		nCnt++;
	}

	//메크로 방지검사
	if(nCnt > 0)
		return true;

	return false;	
}

bool CPlayer::S_ConvertMonsterTarget(CCharacter* pDstObj, float fEffectValue)
{
	return false;
}

bool CPlayer::S_TransMonsterHP(CCharacter* pDstObj, float fEffectValue)
{
	return false;
}

bool CPlayer::S_ReleaseMonsterTarget(CCharacter* pDstObj, float fEffectValue)
{
	return false;
}

bool CPlayer::IsSFUsableSFMastery(BYTE byMasteryCode, int nMasteryIndex, BYTE byLv)
{
	static int nForceLimMasteryPerLv[mastery_level_num] = {0, 0, 60, 90};
	static int nSkillLimMasteryPerLv[mastery_level_num] = {0, 30, 60, 90};

	if(nMasteryIndex == -1)	//플레이어는 사용불가
		return false;

	int nMPL = 0;
	if(byMasteryCode == mastery_code_force)
		nMPL = nForceLimMasteryPerLv[byLv];
	else
		nMPL = nSkillLimMasteryPerLv[byLv];

	int m = nMasteryIndex;
	while(m % mastery_level_num > 0)
	{
		if(m_pmMst.GetMasteryPerMast(byMasteryCode, m-1) < nMPL)
			return false;
		--m;		
	}
		
	return true;
}

bool CPlayer::IsSFUseableRace(BYTE byEffectCode, WORD wEffectIndex)
{	
	BYTE byRaceCode = m_Param.GetRaceSexCode();
	if(byEffectCode == effect_code_skill)
	{
		_skill_fld* pSkillFld = (_skill_fld*)g_Main.m_tblEffectData[byEffectCode].GetRecord(wEffectIndex);

		if(pSkillFld->m_strUsableRace[byRaceCode] == '1')
			return true;
	}
	else
	{
		_force_fld* pForceFld = (_force_fld*)g_Main.m_tblEffectData[byEffectCode].GetRecord(wEffectIndex);
	
		if(pForceFld->m_strUsableRace[byRaceCode] == '1')
			return true;
	}
	return false;
}

bool CPlayer::IsSFUseableTarget(BYTE byEffectCode, WORD wEffectIndex, CCharacter* pTargetChar)
{
	char* psDstData;
	int nDstType;

	if( byEffectCode == effect_code_skill )
	{
		_skill_fld* pSkillFld = (_skill_fld*)g_Main.m_tblEffectData[byEffectCode].GetRecord(wEffectIndex);
		psDstData = pSkillFld->m_strActableDst;
	}
	else
	{
		_force_fld* pForceFld = (_force_fld*)g_Main.m_tblEffectData[byEffectCode].GetRecord(wEffectIndex);
		psDstData = pForceFld->m_strActableDst;		
	}

	if(psDstData[dst_anyone] == '1' )
		return true;	

	if(!pTargetChar)	// map
	{
		nDstType = dst_map;
	}
	else if(pTargetChar->m_ObjID.m_byID == obj_id_monster)	// mob
	{
		nDstType = dst_mob;
	}
	else if(pTargetChar->m_ObjID.m_byID == obj_id_player) // player
	{
		if( m_Param.GetRaceSexCode()/2 == ((CPlayer*)pTargetChar)->m_Param.GetRaceSexCode()/2 )//성별은 뺀다..
			nDstType = dst_same_race;
		else 
			nDstType = dst_enemy;
	}
	else
		return false;

	if( psDstData[nDstType] == '1' )
		return true;

	return false;
}

bool CPlayer::IsSFUsableFP(BYTE byEffectCode, WORD wEffectIndex, WORD* OUT pLeftFP)
{
	int nNeedFP;

	if(byEffectCode == effect_code_skill)
	{
		_skill_fld* pSkillFld = (_skill_fld*)g_Main.m_tblEffectData[byEffectCode].GetRecord(wEffectIndex);
		nNeedFP = pSkillFld->m_nNeedFP;
	}
	else
	{
		_force_fld* pForceFld = (_force_fld*)g_Main.m_tblEffectData[byEffectCode].GetRecord(wEffectIndex);
		nNeedFP = pForceFld->m_nNeedFP;
	}

	if(GetFP() < nNeedFP * m_EP.GetEff_Rate(_EFF_RATE::FP_Consum))  //## PARAMETER EDIT (FP소모량)
		return false;

	if(pLeftFP)
		*pLeftFP = GetFP() - nNeedFP * m_EP.GetEff_Rate(_EFF_RATE::FP_Consum);

	return true;
}

void CPlayer::_TowerDestroy(CGuardTower* pTowerObj)
{	//가드타워가 죽을때 destory에서 호출.. 
	__ITEM* pTowerItem = NULL;
	for(int i = 0; i < _TOWER_PARAM::max_param; i++)
	{
		if(m_pmTwr.m_List[i].m_pTowerObj != pTowerObj)
			continue;

		pTowerItem = m_pmTwr.m_List[i].m_pTowerItem;

		m_pmTwr.m_List[i].init();
		break;
	}

	if(!pTowerItem)
		return;

	pTowerItem->m_bLock = false;

	//send.. 파괴..
	SendMsg_AlterTowerHP(pTowerItem->m_wSerial, 0);

	//삭제..			
	Emb_DelStorage(_STORAGE_POS::INVEN, pTowerItem->m_byStorageIndex, false);

	//ITEM HISTORY..
	s_MgrItemHistory.consume_del_item(pTowerItem, m_szItemHistoryFileName);	
}

WORD CPlayer::_TowerReturn(__ITEM* pTowerItem)
{	//가드타워 해체
	for(int i = 0; i < _TOWER_PARAM::max_param; i++)
	{
		if(m_pmTwr.m_List[i].m_pTowerItem != pTowerItem)
			continue;

		m_pmTwr.m_List[i].m_pTowerItem->m_bLock = false;

		//삭제..
		int nAlterHP = m_pmTwr.m_List[i].m_pTowerObj->m_nHP - m_pmTwr.m_List[i].m_pTowerItem->m_dwDur;
		WORD dwLeftHP = Emb_AlterDurPoint(_STORAGE_POS::INVEN, m_pmTwr.m_List[i].m_pTowerItem->m_byStorageIndex, nAlterHP, false, false); 

		//tower obj destroy..
		m_pmTwr.m_List[i].m_pTowerObj->Destroy(tower_des_type_back);
	
		m_pmTwr.m_List[i].init();
		return dwLeftHP;
	}
	return 0;
}

void CPlayer::_TowerAllReturn()
{	//접속종료
	for(int i = 0; i < _TOWER_PARAM::max_param; i++)
	{
		if(!m_pmTwr.m_List[i].m_pTowerItem)
			continue;

		m_pmTwr.m_List[i].m_pTowerItem->m_bLock = false;

		//삭제..
		int nAlterHP = m_pmTwr.m_List[i].m_pTowerObj->m_nHP - m_pmTwr.m_List[i].m_pTowerItem->m_dwDur;
		Emb_AlterDurPoint(_STORAGE_POS::INVEN, m_pmTwr.m_List[i].m_pTowerItem->m_byStorageIndex, nAlterHP, false, false); 

		//tower obj destroy..
		m_pmTwr.m_List[i].m_pTowerObj->Destroy(tower_des_type_disconnect);
	
		m_pmTwr.m_List[i].init();
	}
}

