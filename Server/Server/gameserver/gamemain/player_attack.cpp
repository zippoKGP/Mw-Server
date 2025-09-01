#include "stdafx.h"
#include "Player.h"
#include "SkillFld.h"
#include "ForceFld.h"
#include "WeaponItemFld.h"
#include "MainThread.h"
#include "SFDataEffect.h"
#include "BulletItemFld.h"
#include "ErrorEventFromServer.h"
#include "UnitPartFld.h"
#include "UnitBulletFld.h"

static int CalcDamagedCharAvgLv(_be_damaged_char* pCharList, int nCharNum);

////////////////////////////////////////////////////////////////////////////////////
//	pc_PlayAttack_Gen
////////////////////////////////////////////////////////////////////////////////////

void CPlayer::pc_PlayAttack_Gen(CCharacter* pDst, BYTE byAttPart, WORD wBulletSerial, bool bCount)// = false)
{	//반드시 대인공격
	__ITEM* pBulletProp = NULL;
	_BulletItem_fld* pfldBullet = NULL;

	if(bCount)	//카운트라면.. 근접인지 파악..
	{
		if(m_pmWpn.byWpClass != wp_class_close)
			return;
	}

	char sErrCode = _pre_check_normal_attack(pDst, wBulletSerial, &pBulletProp, &pfldBullet);
	if(sErrCode != 0)
	{
		SendMsg_AttackResult_Error(sErrCode);	//에러메시지..
		return;
	}

	float fAddBulletFc = 1.0f;
	BYTE byBulletIndex = 0xFF;
	if(pBulletProp) 
	{
		fAddBulletFc = pfldBullet->m_fGAAF;
		byBulletIndex = pBulletProp->m_wItemIndex;
	}
	
	CAttack at(this);

	_attack_param ap;
	make_gen_attack_param(pDst, byAttPart, fAddBulletFc, &ap);

	at.AttackGen(&ap);

	int nTotalDam = _check_exp_after_attack(at.m_nDamagedObjNum, at.m_DamList);

	if(nTotalDam > 0)
	{
		//무기 숙련도 증가 및 무기 내구성 감소
		if(GetLevel() - pDst->GetLevel() < 6)	
		{
			int nDstLv = pDst->GetLevel();
			int nAddCum = GetMasteryCumAfterAttack(nDstLv);

			if(m_pmWpn.byWpType != type_luancher)	//런쳐가 아니라면 무기숙련도로 올리고..
				Emb_AlterStat(mastery_code_weapon, m_pmWpn.byWpClass, nAddCum);
			else								//런쳐라면 종종특화숙련도로 올린다.
				Emb_AlterStat(mastery_code_special, 0, nAddCum);
		}

		_check_dst_param_after_attack(nTotalDam, pDst);
	}

	//상대반격기체크
	if(pDst)
		CheckDstCountAttack(pDst);				

	//결과전송
	if(!bCount)
		SendMsg_AttackResult_Gen(&at, byBulletIndex);
	else
		SendMsg_AttackResult_Count(&at);

	//데미지..(죽는메시지를 나중에 보내야하므로..맨나중에한다.)
	for(int i = 0; i < at.m_nDamagedObjNum; i++)
		at.m_DamList[i].m_pChar->SetDamage(at.m_DamList[i].m_nDamage, this, m_Param.GetLevel(), at.m_bIsCrtAtt);

	//애니머스 소환상태인경우 애니머스에게 알린다..
	if(pDst && m_pRecalledAnimusChar)
	{
		if(pDst->GetHP() > 0)
			m_pRecalledAnimusChar->MasterAttack_MasterInform(pDst);
	}

	//총알감소..
	if(pBulletProp)
	{
		if(Emb_AlterDurPoint(_STORAGE_POS::EMBELLISH, pBulletProp->m_byStorageIndex, -1, false, true) == 0)//소모
		{	//ITEM HISTORY..
			s_MgrItemHistory.consume_del_item(pBulletProp, m_szItemHistoryFileName);
		}
		else
		{
			SendMsg_AlterWeaponBulletInform(pBulletProp->m_wSerial, pBulletProp->m_dwDur);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////
//	pc_PlayAttack_Skill
////////////////////////////////////////////////////////////////////////////////////

void CPlayer::pc_PlayAttack_Skill(CCharacter* pDst, BYTE byEffectCode, WORD wSkillIndex, WORD wBulletSerial)
{
	__ITEM* pBulletProp = NULL;
	_BulletItem_fld* pfldBullet = NULL;
	_skill_fld* pSkillFld = (_skill_fld*)g_Main.m_tblEffectData[byEffectCode].GetRecord(wSkillIndex);
	int nAttType = pSkillFld->m_nAttType[m_pmMst.GetSkillLv(pSkillFld->m_dwIndex)-1];
	CCharacter* pTargetChar = pDst;

	if(nAttType == sf_effect_code_self)
		pTargetChar = NULL;

	char sErrCode = _pre_check_skill_attack(pTargetChar, byEffectCode, pSkillFld, wBulletSerial, &pBulletProp, &pfldBullet, nAttType);
	if(sErrCode != 0)
	{
		SendMsg_AttackResult_Error(sErrCode);	//에러메시지..
		return;
	}

	float fAddBulletFc = 1.0f;
	BYTE byBulletIndex = 0xFF;
	if(pBulletProp) 
	{
		fAddBulletFc = pfldBullet->m_fGAAF;
		byBulletIndex = pBulletProp->m_wItemIndex;
	}

	CPlayerAttack at(this);

	_attack_param ap;
	make_skill_attack_param(pTargetChar, pSkillFld, nAttType, pBulletProp, fAddBulletFc, &ap);

	at.AttackSkill(&ap);

	//우선 FP를 소모시킨다.
	float fNeedValue = pSkillFld->m_nNeedFP * m_EP.GetEff_Rate(_EFF_RATE::FP_Consum);	//PARAMETER EDIT (FP소모량)
	int l_nLeftFP = max(int(m_Param.GetFP() - fNeedValue), 0);
	SetFP(l_nLeftFP, true);
	SendMsg_SetFPInform();

	//경험치
	int nTotalDam = _check_exp_after_attack(at.m_nDamagedObjNum, at.m_DamList);

	if(nTotalDam > 0)
	{
		//스킬 누적수 총계치 증가
		if(byEffectCode == effect_code_skill)
		{
			if((DWORD)pSkillFld->m_nMastIndex < skill_mastery_num)
			{
				int nDstLv = ::CalcDamagedCharAvgLv(at.m_DamList, at.m_nDamagedObjNum);

				if(GetLevel() - nDstLv < 6)	
				{					
					int nAddCum = GetMasteryCumAfterAttack(nDstLv);
					if(pSkillFld->m_nClass == sk_class_bow || 
					   pSkillFld->m_nClass == sk_class_firearm || 
					   pSkillFld->m_nClass == sk_class_throw)
					{
						nAddCum *= 2;//3개의 스킬계열은 2배로
					}

					Emb_AlterStat(mastery_code_skill, pSkillFld->m_dwIndex, nAddCum);
				}
			}
		}

		_check_dst_param_after_attack(nTotalDam, pTargetChar);
	}

	//상대반격기체크
	if(pTargetChar)
		CheckDstCountAttack(pTargetChar);				

	//총알감소
	if(ap.nShotNum > 0)
	{
		if(Emb_AlterDurPoint(_STORAGE_POS::EMBELLISH, pBulletProp->m_byStorageIndex, -ap.nShotNum, false, true) == 0)
		{	//ITEM HISTORY..
			s_MgrItemHistory.consume_del_item(pBulletProp, m_szItemHistoryFileName);
		}
		else
		{
			SendMsg_AlterWeaponBulletInform(pBulletProp->m_wSerial, pBulletProp->m_dwDur);
		}
	}

	//섹터 지역기는 hp를 감소시키고 그로기상태로 만듬..
	if(nAttType == sf_effect_code_sector)
	{
		SetHP(GetHP()*0.1f, true);
		m_pmGrg.StartGroggy(3000);//3초간 그로기 상태로..
	}

	SendMsg_AttackResult_Skill(byEffectCode, &at, byBulletIndex); 

	//데미지..(죽는메시지를 나중에 보내야하므로..맨나중에한다.)
	for(int i = 0; i < at.m_nDamagedObjNum; i++)
		at.m_DamList[i].m_pChar->SetDamage(at.m_DamList[i].m_nDamage, this, m_Param.GetLevel(), at.m_bIsCrtAtt);

	//애니머스 소환상태인경우 애니머스에게 알린다..
	if(pTargetChar)
	{
		if(m_pRecalledAnimusChar && pTargetChar->GetHP() > 0)
			m_pRecalledAnimusChar->MasterAttack_MasterInform(pTargetChar);
	}
}

////////////////////////////////////////////////////////////////////////////////////
//	pc_PlayAttack_Force  
////////////////////////////////////////////////////////////////////////////////////

void CPlayer::pc_PlayAttack_Force(CCharacter* pDst, float* pfAreaPos, WORD wForceSerial)
{
	__ITEM* pForceItem = NULL;
	_force_fld* pForceFld = NULL;
	float fArea[3] = { pfAreaPos[0], m_fCurPos[1], pfAreaPos[1] };
	CCharacter* pTargetChar = pDst;

	char sErrCode = _pre_check_force_attack(pTargetChar, fArea, wForceSerial, &pForceFld, &pForceItem);
	if(sErrCode != 0)
	{
		SendMsg_AttackResult_Error(sErrCode);	//에러메시지..
		return;
	}
	else
	{
		//self, extent는 타겟케릭터가 없는것으로..
		if(pForceFld->m_nEffectGroup == sf_effect_code_extent || pForceFld->m_nEffectGroup == sf_effect_code_self)
			pTargetChar = NULL;
	}

	CAttack at(this);

	_attack_param ap;
	make_force_attack_param(pTargetChar, pForceFld, pForceItem, pfAreaPos, &ap);

	at.AttackForce(&ap);
	
	//FP소모
	float fNeedValue = pForceFld->m_nNeedFP * m_EP.GetEff_Rate(_EFF_RATE::FP_Consum);//PARAMETER EDIT (FP소모량)
	int l_nLeftFP = max(int(m_Param.GetFP() - fNeedValue), 0);
	SetFP( l_nLeftFP, true );
	SendMsg_SetFPInform();

	//set exp, and calc total damage
	int nTotalDam = _check_exp_after_attack(at.m_nDamagedObjNum, at.m_DamList);

	if(nTotalDam > 0)
	{
		// 포스 성공 누적수 통계치 증가			
		if((DWORD)pForceFld->m_nMastIndex < force_mastery_num)
		{
			int	nDstLv = ::CalcDamagedCharAvgLv(at.m_DamList, at.m_nDamagedObjNum);

			if(GetLevel() - nDstLv < 6)	
			{
				int nAddCum = GetMasteryCumAfterAttack(nDstLv);

				//포스마스터리랑 리버 누적치 증가..
				Emb_AlterStat(mastery_code_force, pForceFld->m_nMastIndex, nAddCum);
				_STORAGE_LIST* pList = pForceItem->m_pInList;
				DWORD dwFcItemStat = Emb_AlterDurPoint(pList->m_nListCode, pForceItem->m_byStorageIndex, nAddCum, false, true);//누적
				SendMsg_FcitemInform(wForceSerial, dwFcItemStat);		
			}					
		}

		_check_dst_param_after_attack(nTotalDam, pTargetChar);
	}

	//상대반격기체크
	if(pTargetChar)
		CheckDstCountAttack(pTargetChar);				

	SendMsg_AttackResult_Force(&at);

	//데미지..(죽는메시지를 나중에 보내야하므로..맨나중에한다.)
	for(int i = 0; i < at.m_nDamagedObjNum; i++)
		at.m_DamList[i].m_pChar->SetDamage(at.m_DamList[i].m_nDamage, this, m_Param.GetLevel(), at.m_bIsCrtAtt);

	//애니머스 소환상태인경우 애니머스에게 알린다..
	if(pTargetChar)
	{
		if(m_pRecalledAnimusChar && pTargetChar->GetHP() > 0)
			m_pRecalledAnimusChar->MasterAttack_MasterInform(pTargetChar);
	}
}

////////////////////////////////////////////////////////////////////////////////////
//	pc_PlayAttack_Unit
////////////////////////////////////////////////////////////////////////////////////

void CPlayer::pc_PlayAttack_Unit(CCharacter* pDst, BYTE byWeaponPart)
{
	_unit_bullet_param* pBulletParam = NULL;
	_UnitBullet_fld* pBulletFld = NULL;
	_UnitPart_fld* pWeaponFld = NULL;
	
	char sErrCode = _pre_check_unit_attack(pDst, byWeaponPart, &pWeaponFld, &pBulletFld, &pBulletParam);
	if(sErrCode != 0)
	{
		SendMsg_AttackResult_Error(sErrCode);	//에러메시지..
		return;
	}

	float fAddBulletFc = 1.0f;
	BYTE byBulletIndex = 0xFF;
	if(pBulletFld) 
	{
		fAddBulletFc += 0;	//총탄에대한 추가 공격력
		byBulletIndex = pBulletFld->m_dwIndex;
	}
	int nLeftBulletNum = 0;
	
	CPlayerAttack at(this);

	_attack_param ap;
	make_unit_attack_param(pDst, pWeaponFld, fAddBulletFc, &ap);

	at.AttackUnit(&ap);

	_check_exp_after_attack(at.m_nDamagedObjNum, at.m_DamList);

	//총알감소..
	if(pBulletParam->wLeftNum < pWeaponFld->m_nNeedBt)
		pBulletParam->wLeftNum = 0;
	else
		pBulletParam->wLeftNum -= pWeaponFld->m_nNeedBt;

	nLeftBulletNum = pBulletParam->wLeftNum;

	SendMsg_AlterUnitBulletInform(byWeaponPart, nLeftBulletNum);

	if(pBulletParam->wLeftNum == 0)	//탄을 전부 소모하면 초기화시킨다.
		*(DWORD*)pBulletParam = 0xFFFFFFFF;		

	//결과전송
	SendMsg_AttackResult_Unit(&at, byWeaponPart, byBulletIndex);

	//데미지..(죽는메시지를 나중에 보내야하므로..맨나중에한다.)
	for(int i = 0; i < at.m_nDamagedObjNum; i++)
		at.m_DamList[i].m_pChar->SetDamage(at.m_DamList[i].m_nDamage, this, m_Param.GetLevel(), at.m_bIsCrtAtt);
}

void CPlayer::pc_PlayAttack_Test(BYTE byEffectCode, BYTE byEffectIndex, WORD wBulletItemSerial, BYTE byWeaponPart, short* pzTar)
{
	BYTE byRetCode = 0;	//1:틀린 BULLET
	__ITEM* pBulletItem = NULL;
	WORD wBulletItemIndex = 0xFFFF;
	WORD wLeftBullet = 0;
	BYTE byEffectLv = 0;

	if(!IsRidingUnit())
	{
		//bullet잇나..
		if(wBulletItemSerial != 0xFFFF)
		{
			pBulletItem = IsBulletValidity(wBulletItemSerial);
			if(pBulletItem)
			{
				wBulletItemIndex = pBulletItem->m_wItemIndex;

				wLeftBullet = Emb_AlterDurPoint(_STORAGE_POS::EMBELLISH, pBulletItem->m_byStorageIndex, -1, false, true);
				if(wLeftBullet == 0)
				{	//ITEM HISTORY..
					s_MgrItemHistory.consume_del_item(pBulletItem, m_szItemHistoryFileName);
				}
				else
				{
					SendMsg_AlterWeaponBulletInform(pBulletItem->m_wSerial, pBulletItem->m_dwDur);
				}
			}
		}

		if(byEffectCode == effect_code_skill)
		{
			_skill_fld* pFld = (_skill_fld*)g_Main.m_tblEffectData[effect_code_skill].GetRecord(byEffectIndex);
			byEffectLv = m_pmMst.GetSkillLv(pFld->m_dwIndex);
		}
		else if(byEffectCode == effect_code_force)
		{
			_force_fld* pFld = (_force_fld*)g_Main.m_tblEffectData[effect_code_force].GetRecord(byEffectIndex);
			__ITEM* pForce = NULL;
			for(int i = 0; i < force_storage_num; i++)
			{
				__ITEM* p = (__ITEM*)&m_Param.m_dbForce.m_pStorageList[i];
				if(!p->m_bLoad)
					continue;

				if(s_pnLinkForceItemToEffect[p->m_wItemIndex] == byEffectIndex)
				{
					pForce = p;
					break;
				}
			}
			if(!pForce)
				byEffectLv = 0;
			else
				byEffectLv = m_pmMst.GetSFLevel(pFld->m_nLv, pForce->m_dwDur);		
		}	
	}
	else
	{
		_unit_bullet_param* pBulletParam = (_unit_bullet_param*)&m_pUsingUnit->dwBullet[byWeaponPart];
		if(pBulletParam->wLeftNum == 0 || pBulletParam->wLeftNum == 0xFFFF)
			return;

		wBulletItemIndex = pBulletParam->wBulletIndex;

		_UnitPart_fld* pWeaponFld = NULL;
		if(byWeaponPart == unit_bullet_arms)
			pWeaponFld = (_UnitPart_fld*)g_Main.m_tblUnitPart[unit_part_arms].GetRecord(m_pUsingUnit->byPart[unit_part_arms]);
		else
			pWeaponFld = (_UnitPart_fld*)g_Main.m_tblUnitPart[unit_part_shoulder].GetRecord(m_pUsingUnit->byPart[unit_part_shoulder]);
		if(!pWeaponFld)
			return;

		//총알감소..
		if(pBulletParam->wLeftNum < pWeaponFld->m_nNeedBt)
			pBulletParam->wLeftNum = 0;
		else
			pBulletParam->wLeftNum -= pWeaponFld->m_nNeedBt;

		int nLeftBulletNum = pBulletParam->wLeftNum;

		SendMsg_AlterUnitBulletInform(byWeaponPart, nLeftBulletNum);

		if(pBulletParam->wLeftNum == 0)	//탄을 전부 소모하면 초기화시킨다.
			*(DWORD*)pBulletParam = 0xFFFFFFFF;		
	}

	SendMsg_TestAttackResult(byEffectCode, byEffectIndex, wBulletItemIndex, byEffectLv, byWeaponPart, pzTar);
}

int CPlayer::_pre_check_normal_attack(CCharacter* pDst, WORD wBulletSerial, __ITEM** OUT ppBulletProp, _BulletItem_fld** OUT ppfldBullet)
{
	__ITEM* l_pBulletProp = NULL;
	_BulletItem_fld* l_pfldBullet = NULL;

	if(!pDst)	//반드시 대인공격이이야함..
		return error_attack_invalid_dst;

	if(IsRidingUnit())
		return error_attack_unit_state;	//유닛탑승상태

	//Bullet가 유효한지 검사
	if(wBulletSerial != 0xFFFF)
	{
		l_pBulletProp = IsBulletValidity(wBulletSerial);
		if(!l_pBulletProp)
		{
			SendMsg_AdjustAmountInform(_STORAGE_POS::EMBELLISH, wBulletSerial, 0);//REFRESH..	
			return  error_attack_not_have_bullet;
		}
		else 
			l_pfldBullet = (_BulletItem_fld*)g_Main.m_tblItemData[tbl_code_bullet].GetRecord(l_pBulletProp->m_wItemIndex);
	}

	//상대의 유효성체크
	if(!pDst->m_bLive || pDst->m_bCorpse || pDst->m_pCurMap != m_pCurMap || pDst->GetCurSecNum() == 0xFFFFFFFF)
		return error_attack_invalid_dst;

	if(pDst == this)	//자신을 공격..
		return error_attack_invalid_dst;

	//공격가능대상
	if(GetObjRace() == pDst->GetObjRace())
		return error_attack_invalid_dst;

	if(!IsBeAttackedAble())
		return error_attack_invalid_dst;

	DWORD dwCurTime = timeGetTime();
	//공격딜레이 체크
	if(dwCurTime < GetNextAttTime())
	{
		m_byDelayErrorCount++;
		if(m_byDelayErrorCount > 1)
		{
			m_byDelayErrorCount = 0;
			return error_attack_delay;
		}
	}

	DWORD dwNextTime = dwCurTime + 
						m_pmWpn.GetAttackDelay(GetLevel(), CalcEquipAttackDelay()) + 
						m_EP.GetEff_Plus(_EFF_PLUS::GE_Att_Spd_+ m_pmWpn.byWpClass);
	SetNextAttTime(dwNextTime - 300);	//여유로 0.3초 빼준다..

	//사정거리체크
	float fDist = ::GetSqrt(pDst->m_fCurPos, m_fCurPos);
	if(fDist > m_pmWpn.wGaAttRange + m_EP.GetEff_Plus(_EFF_PLUS::GE_Att_Dist_+m_pmWpn.byWpClass))
	{//사정거리를 벗어나면 좀전의 좌표로도 구해본다..
		fDist = ::GetSqrt(pDst->m_fOldPos, m_fCurPos);
		if(fDist > m_pmWpn.wGaAttRange + m_EP.GetEff_Plus(_EFF_PLUS::GE_Att_Dist_+m_pmWpn.byWpClass))
			return error_attack_radius;
	}

	*ppBulletProp = l_pBulletProp;
	*ppfldBullet = l_pfldBullet;

	return 0;
}

int CPlayer::_pre_check_skill_attack(CCharacter* pDst, BYTE byEffectCode, _skill_fld* pSkillFld, WORD wBulletSerial, __ITEM** OUT ppBulletProp, _BulletItem_fld** OUT ppfldBullet, int nEffectGroup)
{
	__ITEM* l_pBulletProp = NULL;
	_BulletItem_fld* l_pfldBullet = NULL;

	//상대의 유효성체크
	if(pDst)
	{
		if(pDst == this)	
			return error_attack_invalid_dst;
		if(!pDst->m_bLive || pDst->m_bCorpse || pDst->m_pCurMap != m_pCurMap || pDst->GetCurSecNum() == 0xFFFFFFFF)
			return error_attack_invalid_dst;
		if(GetObjRace() == pDst->GetObjRace())	//공격가능대상
			return error_attack_invalid_dst;
		if(!pDst->IsBeAttackedAble())
			return error_attack_invalid_dst;
	}

	if(!pSkillFld)
		return error_attack_data;

	//시전가능한 마스터리인지..
	if(byEffectCode == effect_code_skill)
	{
		if((DWORD)pSkillFld->m_nMastIndex > skill_mastery_num)
			return error_attack_data;
		if(!IsSFUsableSFMastery(mastery_code_skill, pSkillFld->m_nMastIndex, pSkillFld->m_nLv))
			return error_attack_few_mastery;
	}
	else
	{	//자신이 쓸수있는 클래스인가..
		if(!m_Param.IsActableClassSkill(pSkillFld->m_strCode))
			return error_attack_not_have_skill;
	}

	if(IsRidingUnit())
		return error_attack_unit_state;	//유닛탑승상태

	if(m_EP.GetEff_State(_EFF_STATE::SK_Att_Lck))//PARAMETER EDIT (공격스킬제한)
		return error_attack_rule;
	
	//Bullet가 유효한지 검사
	if(wBulletSerial != 0xFFFF)
	{
		l_pBulletProp = IsBulletValidity(wBulletSerial);
		if(!l_pBulletProp)
		{
			SendMsg_AdjustAmountInform(_STORAGE_POS::EMBELLISH, wBulletSerial, 0);//REFRESH..	
			return error_attack_not_have_bullet;
		}
		else
			l_pfldBullet = (_BulletItem_fld*)g_Main.m_tblItemData[tbl_code_bullet].GetRecord(l_pBulletProp->m_wItemIndex);
	}

	bool bWpAble = false;
	//올바른 무기인가
	if(m_pmWpn.GetAttackToolType() == weapon_equip_weapon)
	{
		if(pSkillFld->m_strFixWeapon[m_pmWpn.byWpType] == '1')
			bWpAble = true;
	}
	else
	{
		if(!strstr(pSkillFld->m_strFixWeapon, "1"))// m_strFixEquip이 모두 0으로 세팅되어있으면 무기가 없어도 된다.
			bWpAble = true;
	}
	if(!bWpAble)
		return error_attack_correctweapon;

	//공격인지 확인
	if(!pSkillFld->m_bAttackable)
		return error_attack_rule;

	DWORD dwCurTime = timeGetTime();
	//시전딜레이 체크
	if(dwCurTime - m_pdwLastSkillActTime[pSkillFld->m_dwIndex] < pSkillFld->m_fActDelay - 500)
		return error_attack_delay;
	else
		m_pdwLastSkillActTime[pSkillFld->m_dwIndex] = dwCurTime;

	//사정거리체크 .. self area인경우는 제외..
	if(pDst)
	{
		if(::GetSqrt(pDst->m_fCurPos, m_fCurPos) > m_pmWpn.wGaAttRange + m_EP.GetEff_Plus(_EFF_PLUS::SK_Att_Dist))
		{
			if(::GetSqrt(pDst->m_fOldPos, m_fCurPos) > m_pmWpn.wGaAttRange + m_EP.GetEff_Plus(_EFF_PLUS::SK_Att_Dist))
				return error_attack_radius;
		}
	}

	//공격딜레이 체크
	if(dwCurTime <= GetNextAttTime())
	{
		m_byDelayErrorCount++;
		if(m_byDelayErrorCount > 1)
		{
			m_byDelayErrorCount = 0;
			return error_attack_delay;
		}
	}

	DWORD dwNextTime = dwCurTime + 1000 + m_EP.GetEff_Plus(_EFF_PLUS::GE_Att_Spd_+ m_pmWpn.byWpClass);		
	SetNextAttTime(dwNextTime-300);

	*ppBulletProp = l_pBulletProp;
	*ppfldBullet = l_pfldBullet;

	return 0;
}

int CPlayer::_pre_check_force_attack(CCharacter* pDst, float* IN OUT pfTarPos, WORD wForceItemSerial, _force_fld** OUT ppForceFld, __ITEM** OUT ppForceItem)
{
	_force_fld* l_pForceFld = NULL;
	__ITEM* l_pForceItem = NULL;

	if(IsRidingUnit())
		return error_attack_unit_state;	//유닛탑승상태

	l_pForceItem = m_Param.m_dbForce.GetPtrFromSerial(wForceItemSerial);
	if(!l_pForceItem)
		return error_attack_not_have_force;
	l_pForceFld = (_force_fld *)g_Main.m_tblEffectData[effect_code_force].GetRecord(s_pnLinkForceItemToEffect[l_pForceItem->m_wItemIndex]);
	if(!l_pForceFld)
		return error_attack_not_have_force;

	//지역기인경우 대상이 없는걸루한다..
	if(l_pForceFld->m_nEffectGroup == sf_effect_code_self || l_pForceFld->m_nEffectGroup == sf_effect_code_extent)
		pDst = NULL;	

	//상대의 유효성체크
	if(pDst)
	{	
		if(pDst == this)
			return error_attack_invalid_dst;
		if(!pDst->m_bLive || pDst->m_bCorpse || pDst->m_pCurMap != m_pCurMap || pDst->GetCurSecNum() == 0xFFFFFFFF)
			return error_attack_invalid_dst;
		if(GetObjRace() == pDst->GetObjRace())	//공격가능대상
			return error_attack_invalid_dst;
		if(!pDst->IsBeAttackedAble())
			return error_attack_invalid_dst;
	}
	
	//시전가능한 마스터리인지..
	if(!IsSFUsableSFMastery(mastery_code_force, l_pForceFld->m_nMastIndex, l_pForceFld->m_nLv))
		return error_attack_few_mastery;

	if(m_EP.GetEff_State(_EFF_STATE::FC_Att_Lck))//PARAMETER EDIT (공격포스제한)
		return error_attack_rule;

	//올바른 무기인가..
	if(m_pmWpn.byWpType != type_staff)
		return error_attack_correctweapon;

	//공격인지 확인
	if(!l_pForceFld->m_bAttackable)
		return error_attack_rule;

	if(l_pForceFld->m_nEffectGroup == sf_effect_code_extent)
	{
		if(!m_pCurMap->IsMapIn(pfTarPos))
			return error_attack_rule;			
	}

	DWORD dwCurTime = timeGetTime();
	//시전딜레이 체크
	if(dwCurTime - m_pdwLastForceActTime[l_pForceFld->m_dwIndex] < l_pForceFld->m_fActDelay - 500)
		return error_attack_delay;
	else
		m_pdwLastForceActTime[l_pForceFld->m_dwIndex] = dwCurTime;

	//사정거리체크
	if(pDst)	
	{
		int nLim = l_pForceFld->m_nActDistance+40 + m_EP.GetEff_Plus(_EFF_PLUS::FC_Att_Dist);//여유로 40을 더준다..
		if(::GetSqrt(pDst->m_fCurPos, m_fCurPos) > nLim)
		{
			if(::GetSqrt(pDst->m_fOldPos, m_fCurPos) > nLim)
				return error_attack_radius;
		}
	}
	else 
	{
		if(l_pForceFld->m_nEffectGroup != sf_effect_code_self)//셀프는 뺀다..
		{
			int nLim = l_pForceFld->m_nActDistance+40;
			if(::GetSqrt(pfTarPos, m_fCurPos) > nLim)
			{
				if(::GetSqrt(pfTarPos, m_fOldPos) > nLim)
					return error_attack_radius;
			}
		}
	}

	//공격딜레이 체크
	if(dwCurTime <= GetNextAttTime())
	{
		m_byDelayErrorCount++;
		if(m_byDelayErrorCount > 1)
		{
			m_byDelayErrorCount = 0;
			return error_attack_delay;
		}
	}

	DWORD dwNextTime = dwCurTime + 1000 + m_EP.GetEff_Plus(_EFF_PLUS::GE_Att_Spd_+ m_pmWpn.byWpClass);		
	SetNextAttTime(dwNextTime-300);

	*ppForceFld = l_pForceFld;
	*ppForceItem = l_pForceItem;

	return 0;
}

int CPlayer::_pre_check_unit_attack(CCharacter* pDst, BYTE byWeaponPart, _UnitPart_fld** OUT ppWeaponFld, _UnitBullet_fld** OUT ppBulletFld, _unit_bullet_param** OUT ppBulletParam)
{
	_UnitPart_fld* l_pWeaponFld = NULL;
	_UnitBullet_fld* l_pBulletFld = NULL;
	_unit_bullet_param* l_pBulletParam = NULL;

	//유닛탑승중인지..
	if(!IsRidingUnit())
		return error_attack_unit_state;

	//Bullet이 있는지..
	l_pBulletParam = (_unit_bullet_param*)&m_pUsingUnit->dwBullet[byWeaponPart];
	if(l_pBulletParam->wLeftNum == 0 || l_pBulletParam->wLeftNum == 0xFFFF)
		return error_attack_not_have_bullet;
	l_pBulletFld = (_UnitBullet_fld*)g_Main.m_tblUnitBullet.GetRecord(l_pBulletParam->wBulletIndex);
	if(!l_pBulletFld)
		return error_attack_data;

	if(byWeaponPart == unit_bullet_arms)
		l_pWeaponFld = (_UnitPart_fld*)g_Main.m_tblUnitPart[unit_part_arms].GetRecord(m_pUsingUnit->byPart[unit_part_arms]);
	else
		l_pWeaponFld = (_UnitPart_fld*)g_Main.m_tblUnitPart[unit_part_shoulder].GetRecord(m_pUsingUnit->byPart[unit_part_shoulder]);
	if(!l_pWeaponFld)
		return error_attack_data;

	//무기와 구경이같은 총탄인지..
	if(l_pBulletFld->m_nWPType != l_pWeaponFld->m_nWPType)
		return error_attack_match_bullet;
	
	//상대의 유효성체크
	if(!pDst->m_bLive || pDst->m_bCorpse || pDst->m_pCurMap != m_pCurMap || pDst->GetCurSecNum() == 0xFFFFFFFF)
		return error_attack_invalid_dst;

	//공격가능대상
	if(GetObjRace() == pDst->GetObjRace())
		return error_attack_invalid_dst;

	if(!IsBeAttackedAble())
		return error_attack_invalid_dst;

	//사정거리체크	
	if(::GetSqrt(pDst->m_fCurPos, m_fCurPos) > l_pWeaponFld->m_fAttackRange+40)
	{//사정거리를 벗어나면 좀전의 좌표로도 구해본다..		
		if(::GetSqrt(pDst->m_fOldPos, m_fCurPos) > l_pWeaponFld->m_fAttackRange+40)
			return error_attack_radius;
	}

	DWORD dwCurTime = timeGetTime();
	//공격딜레이 체크
	if(dwCurTime < GetNextAttTime())
	{
		m_byDelayErrorCount++;
		if(m_byDelayErrorCount > 1)
		{
			m_byDelayErrorCount = 0;
			return error_attack_delay;
		}
	}

	DWORD dwNextTime = dwCurTime + l_pWeaponFld->m_nAttackDel;
	SetNextAttTime(dwNextTime-300);

	*ppWeaponFld = l_pWeaponFld;
	*ppBulletFld = l_pBulletFld;
	*ppBulletParam = l_pBulletParam;

	return 0;
}

void CPlayer::make_gen_attack_param(CCharacter* pDst, BYTE byPart, float fAddBulletFc, _attack_param* OUT pAP)
{
	pAP->pDst = pDst;
	pAP->nPart = byPart;
	pAP->nTol = m_pmWpn.byAttTolType;
	pAP->nClass = m_pmWpn.byWpClass;
	pAP->nMinAF = m_pmWpn.nGaMinAF * GetAdrenAF() * fAddBulletFc;	//아드레날린효과 + 총알부가효과
	pAP->nMaxAF = (m_pmWpn.nGaMaxAF + m_pmMst.GetMasteryPerMast(mastery_code_weapon, m_pmWpn.byWpClass)) * GetAdrenAF() * fAddBulletFc;
	pAP->nMinSel = m_pmWpn.byGaMinSel;
	pAP->nMaxSel = m_pmWpn.byGaMaxSel;
	pAP->bMatchless = m_bCheat_Matchless;
}

void CPlayer::make_skill_attack_param(CCharacter* pDst, _skill_fld* pSkillFld, int nAttType, __ITEM* pBulletItem, float fAddBulletFc, _attack_param* OUT pAP)
{
	pAP->pDst = pDst;
	pAP->nPart = GetAttackRandomPart();
	pAP->nTol = m_pmWpn.byAttTolType;
	pAP->nClass = m_pmWpn.byWpClass;
	pAP->nMinAF = m_pmWpn.nGaMinAF * GetAdrenAF() * fAddBulletFc;
	pAP->nMaxAF = (m_pmWpn.nGaMaxAF + m_pmMst.GetMasteryPerMast(mastery_code_weapon, m_pmWpn.byWpClass)) * GetAdrenAF() * fAddBulletFc;
	pAP->nMinSel = m_pmWpn.byGaMinSel;
	pAP->nMaxSel = m_pmWpn.byGaMaxSel;
	pAP->nWpRange = GetWeaponRange();

	if(pBulletItem)
	{	//한방에 나가는 탄갯수..
		pAP->nShotNum = pSkillFld->m_nAttNeedBt;
		if(pBulletItem->m_dwDur < pSkillFld->m_nAttNeedBt)
			pAP->nShotNum = pBulletItem->m_dwDur;		
	}
	if(nAttType == sf_effect_code_sector)
		pAP->nAddAttPnt = GetHP()*0.9f;

	pAP->pFld = (_base_fld*)pSkillFld;
	pAP->nLevel = m_pmMst.GetSkillLv(pSkillFld->m_dwIndex);
	pAP->nMastery = m_pmMst.GetMasteryPerMast(mastery_code_skill, pSkillFld->m_nMastIndex);
	
	if(nAttType == sf_effect_code_self)
		memcpy(pAP->fArea, m_fCurPos, sizeof(float)*3);	//자신의 좌표로..
	else
		memcpy(pAP->fArea, pDst->m_fCurPos, sizeof(float)*3);	//타겟의 좌표로..
}

void CPlayer::make_force_attack_param(CCharacter* pDst, _force_fld* pForceFld, __ITEM* pForceItem, float* pTar, _attack_param* OUT pAP)
{
	pAP->pDst = pDst;
	pAP->nPart = GetAttackRandomPart();
	pAP->nTol = pForceFld->m_nProperty;
	pAP->nClass = m_pmWpn.byWpClass;
	pAP->nMinAF = m_pmWpn.nMaMinAF;
	pAP->nMaxAF = m_pmWpn.nMaMaxAF;
	pAP->nMinSel = m_pmWpn.byMaMinSel;
	pAP->nMaxSel = m_pmWpn.byMaMaxSel;
	pAP->pFld = (_base_fld*)pForceFld;
	if(pForceFld->m_nEffectGroup == sf_effect_code_self)
		memcpy(pAP->fArea, m_fCurPos, sizeof(float)*3);
	else
		memcpy(pAP->fArea, pTar, sizeof(float)*3);
	pAP->nLevel = m_pmMst.GetSFLevel(pForceFld->m_nLv, pForceItem->m_dwDur);
	pAP->nMastery = m_pmMst.GetMasteryPerMast(mastery_code_force, pForceFld->m_nMastIndex);
}

void CPlayer::make_unit_attack_param(CCharacter* pDst, _UnitPart_fld* pWeaponFld, float fAddBulletFc, _attack_param* OUT pAP)
{
	pAP->pDst = pDst;
	pAP->nPart = GetAttackRandomPart();
	pAP->nTol = TOL_CODE_NOTHING;
	pAP->nClass = pWeaponFld->m_nWPType;
	pAP->nMinAF = pWeaponFld->m_nGAMinAF + fAddBulletFc;
	pAP->nMaxAF = pWeaponFld->m_nGAMaxAF + m_pmMst.GetMasteryPerMast(mastery_code_special, 0) + fAddBulletFc;
	pAP->nMinSel = pWeaponFld->m_nGAMinSelProb;
	pAP->nMaxSel = pWeaponFld->m_nGAMaxSelProb;
	pAP->nWpRange = pWeaponFld->m_fAttackRange;
	pAP->pFld = (_base_fld*)pWeaponFld;
}

int CPlayer::_check_exp_after_attack(int nDamagedObjNum, _be_damaged_char* pList)
{
	int nTotalDam = 0;
	bool bUnitState = IsRidingUnit();

	for(int i = 0; i < nDamagedObjNum; i++)
	{
		_be_damaged_char* p = &pList[i];

		nTotalDam += p->m_nDamage;

		//대상이 플레이어라면 경험치넘어감
		if(p->m_pChar->IsRewardExp() && p->m_nDamage > 0)
			CalcExp(p->m_pChar, p->m_nDamage);

		//스텔스 상태에서 공격받으면 스텔스 풀린다
		if(p->m_pChar->m_EP.GetEff_State(_EFF_STATE::Stealth))	//PARAMETER EDIT (STEALTH)
			p->m_pChar->RemoveSFContHelpByEffect(cont_param_state, _EFF_STATE::Stealth);

		//투명 상태에서 공격받으면 잠시 풀린다.
		if(p->m_pChar->m_EP.GetEff_Plus(_EFF_PLUS::Transparency) > 0 && !p->m_pChar->m_bBreakTranspar)	//PARAMETER EDIT (Transparency)
			p->m_pChar->SetBreakTranspar(true);
	}

	return nTotalDam;
}

void CPlayer::_check_dst_param_after_attack(int nTotalDam, CCharacter* pTarget)
{
	if(m_EP.GetEff_Rate(_EFF_RATE::FP_Absorb) > 1.0f)	//PARAMETER EDIT (FP흡수도)
	{
		int nFPAbs = nTotalDam * (m_EP.GetEff_Rate(_EFF_RATE::FP_Absorb)-1);
		int nCurFP = m_Param.GetFP();
		nCurFP += nFPAbs;
		if(nCurFP > GetLimFP())
			nCurFP = GetLimFP();

		SetFP(nCurFP, false);
	}
	
	if(m_EP.GetEff_Rate(_EFF_RATE::HP_Absorb) > 1.0f)	//PARAMETER EDIT (HP흡수도)
	{
		int nHPAbs = nTotalDam * (m_EP.GetEff_Rate(_EFF_RATE::HP_Absorb)-1);
		int nCurHP = m_Param.GetHP();

		SetHP(nCurHP+nHPAbs, false);
	}

	//첫방에 맞은 사람에게 STUN.. 적용..
	if(pTarget)
	{
		if(m_EP.GetEff_State(_EFF_STATE::Dst_Make_Stun))	//PARAMETER EDIT (STUN)
		{
			if(pTarget->GetHP() > 0)
			{
				pTarget->SetStun(true);
				pTarget->SendMsg_StunInform();
			}
		}
		else if(m_EP.GetEff_Rate(_EFF_RATE::GE_Stun) > 1.0f)	//PARAMETER EDIT (GE_Stun)
		{
			WORD wStunRate = (m_EP.GetEff_Rate(_EFF_RATE::GE_Stun)-1) * 100;
			if(wStunRate > rand()%1000)
			{
				if(pTarget->GetHP() > 0)
				{
					pTarget->SetStun(true);
					pTarget->SendMsg_StunInform();
				}
			}
		}
	}
}

__ITEM* CPlayer::IsBulletValidity(WORD wBulletSerial)
{
	if(wBulletSerial != 0xFF)
	{	//착용창에서 찾는다.
		__ITEM* pBulletProp = m_Param.m_dbEmbellish.GetPtrFromSerial(wBulletSerial);
		if(pBulletProp)
		{	
			__ITEM* pWpProp = &m_Param.m_dbEquip.m_pStorageList[tbl_code_weapon];
			if(!pWpProp->m_bLoad)
				return NULL;
			if(pBulletProp->m_byTableCode != tbl_code_bullet)
				return NULL;

			_weapon_bullet_link_fld* pBulletRec = (_weapon_bullet_link_fld*)g_Main.m_tblWeaponBulletLink.m_tblBullet.GetRecord(pBulletProp->m_wItemIndex);

			bool bFind = false;
			for(int i = 0; i < CWeaponBulletLinkTable::max_link_num; i++)
			{
				int nWpIdx = g_Main.m_tblWeaponBulletLink.GetWPIndexFromLinkSlot(pBulletProp->m_wItemIndex, i);
				if(nWpIdx == pWpProp->m_wItemIndex)
					return pBulletProp;
			}
		}
	}

	return NULL;
}

bool CPlayer::CheckDstCountAttack(CCharacter* pDst)
{
	//상대반격기체크
	if(!pDst)
		return false;

	if(pDst->m_EP.GetEff_State(_EFF_STATE::Res_Att))//PARAMETER EDIT (상대반격기)
	{	//상대방과 자신의 거리가 나의 사정거리의 1.5배이내인경우만..
		double dDist = ::GetSqrt(pDst->m_fCurPos, m_fCurPos);
		if(dDist < pDst->GetAttackRange() * 1.5)
		{
			if(pDst->m_ObjID.m_byKind == obj_kind_char && pDst->m_ObjID.m_byID == obj_id_player)
				((CPlayer*)pDst)->m_pCountAttackDst = this;

			//반격기를 해제한다.
			pDst->RemoveSFContHelpByEffect(cont_param_state, _EFF_STATE::Res_Att);

			return true;
		}
	}

	return false;
}

int CalcDamagedCharAvgLv(_be_damaged_char* pCharList, int nCharNum)
{
	int nLvCum = 0;
	for(int i = 0; i < nCharNum; i++)
		nLvCum += pCharList[i].m_pChar->GetLevel();

	return nLvCum/nCharNum;
}