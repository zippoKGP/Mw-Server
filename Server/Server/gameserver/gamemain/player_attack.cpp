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
{	//�ݵ�� ���ΰ���
	__ITEM* pBulletProp = NULL;
	_BulletItem_fld* pfldBullet = NULL;

	if(bCount)	//ī��Ʈ���.. �������� �ľ�..
	{
		if(m_pmWpn.byWpClass != wp_class_close)
			return;
	}

	char sErrCode = _pre_check_normal_attack(pDst, wBulletSerial, &pBulletProp, &pfldBullet);
	if(sErrCode != 0)
	{
		SendMsg_AttackResult_Error(sErrCode);	//�����޽���..
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
		//���� ���õ� ���� �� ���� ������ ����
		if(GetLevel() - pDst->GetLevel() < 6)	
		{
			int nDstLv = pDst->GetLevel();
			int nAddCum = GetMasteryCumAfterAttack(nDstLv);

			if(m_pmWpn.byWpType != type_luancher)	//���İ� �ƴ϶�� ������õ��� �ø���..
				Emb_AlterStat(mastery_code_weapon, m_pmWpn.byWpClass, nAddCum);
			else								//���Ķ�� ����Ưȭ���õ��� �ø���.
				Emb_AlterStat(mastery_code_special, 0, nAddCum);
		}

		_check_dst_param_after_attack(nTotalDam, pDst);
	}

	//���ݰݱ�üũ
	if(pDst)
		CheckDstCountAttack(pDst);				

	//�������
	if(!bCount)
		SendMsg_AttackResult_Gen(&at, byBulletIndex);
	else
		SendMsg_AttackResult_Count(&at);

	//������..(�״¸޽����� ���߿� �������ϹǷ�..�ǳ��߿��Ѵ�.)
	for(int i = 0; i < at.m_nDamagedObjNum; i++)
		at.m_DamList[i].m_pChar->SetDamage(at.m_DamList[i].m_nDamage, this, m_Param.GetLevel(), at.m_bIsCrtAtt);

	//�ִϸӽ� ��ȯ�����ΰ�� �ִϸӽ����� �˸���..
	if(pDst && m_pRecalledAnimusChar)
	{
		if(pDst->GetHP() > 0)
			m_pRecalledAnimusChar->MasterAttack_MasterInform(pDst);
	}

	//�Ѿ˰���..
	if(pBulletProp)
	{
		if(Emb_AlterDurPoint(_STORAGE_POS::EMBELLISH, pBulletProp->m_byStorageIndex, -1, false, true) == 0)//�Ҹ�
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
		SendMsg_AttackResult_Error(sErrCode);	//�����޽���..
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

	//�켱 FP�� �Ҹ��Ų��.
	float fNeedValue = pSkillFld->m_nNeedFP * m_EP.GetEff_Rate(_EFF_RATE::FP_Consum);	//PARAMETER EDIT (FP�Ҹ�)
	int l_nLeftFP = max(int(m_Param.GetFP() - fNeedValue), 0);
	SetFP(l_nLeftFP, true);
	SendMsg_SetFPInform();

	//����ġ
	int nTotalDam = _check_exp_after_attack(at.m_nDamagedObjNum, at.m_DamList);

	if(nTotalDam > 0)
	{
		//��ų ������ �Ѱ�ġ ����
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
						nAddCum *= 2;//3���� ��ų�迭�� 2���
					}

					Emb_AlterStat(mastery_code_skill, pSkillFld->m_dwIndex, nAddCum);
				}
			}
		}

		_check_dst_param_after_attack(nTotalDam, pTargetChar);
	}

	//���ݰݱ�üũ
	if(pTargetChar)
		CheckDstCountAttack(pTargetChar);				

	//�Ѿ˰���
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

	//���� ������� hp�� ���ҽ�Ű�� �׷α���·� ����..
	if(nAttType == sf_effect_code_sector)
	{
		SetHP(GetHP()*0.1f, true);
		m_pmGrg.StartGroggy(3000);//3�ʰ� �׷α� ���·�..
	}

	SendMsg_AttackResult_Skill(byEffectCode, &at, byBulletIndex); 

	//������..(�״¸޽����� ���߿� �������ϹǷ�..�ǳ��߿��Ѵ�.)
	for(int i = 0; i < at.m_nDamagedObjNum; i++)
		at.m_DamList[i].m_pChar->SetDamage(at.m_DamList[i].m_nDamage, this, m_Param.GetLevel(), at.m_bIsCrtAtt);

	//�ִϸӽ� ��ȯ�����ΰ�� �ִϸӽ����� �˸���..
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
		SendMsg_AttackResult_Error(sErrCode);	//�����޽���..
		return;
	}
	else
	{
		//self, extent�� Ÿ���ɸ��Ͱ� ���°�����..
		if(pForceFld->m_nEffectGroup == sf_effect_code_extent || pForceFld->m_nEffectGroup == sf_effect_code_self)
			pTargetChar = NULL;
	}

	CAttack at(this);

	_attack_param ap;
	make_force_attack_param(pTargetChar, pForceFld, pForceItem, pfAreaPos, &ap);

	at.AttackForce(&ap);
	
	//FP�Ҹ�
	float fNeedValue = pForceFld->m_nNeedFP * m_EP.GetEff_Rate(_EFF_RATE::FP_Consum);//PARAMETER EDIT (FP�Ҹ�)
	int l_nLeftFP = max(int(m_Param.GetFP() - fNeedValue), 0);
	SetFP( l_nLeftFP, true );
	SendMsg_SetFPInform();

	//set exp, and calc total damage
	int nTotalDam = _check_exp_after_attack(at.m_nDamagedObjNum, at.m_DamList);

	if(nTotalDam > 0)
	{
		// ���� ���� ������ ���ġ ����			
		if((DWORD)pForceFld->m_nMastIndex < force_mastery_num)
		{
			int	nDstLv = ::CalcDamagedCharAvgLv(at.m_DamList, at.m_nDamagedObjNum);

			if(GetLevel() - nDstLv < 6)	
			{
				int nAddCum = GetMasteryCumAfterAttack(nDstLv);

				//���������͸��� ���� ����ġ ����..
				Emb_AlterStat(mastery_code_force, pForceFld->m_nMastIndex, nAddCum);
				_STORAGE_LIST* pList = pForceItem->m_pInList;
				DWORD dwFcItemStat = Emb_AlterDurPoint(pList->m_nListCode, pForceItem->m_byStorageIndex, nAddCum, false, true);//����
				SendMsg_FcitemInform(wForceSerial, dwFcItemStat);		
			}					
		}

		_check_dst_param_after_attack(nTotalDam, pTargetChar);
	}

	//���ݰݱ�üũ
	if(pTargetChar)
		CheckDstCountAttack(pTargetChar);				

	SendMsg_AttackResult_Force(&at);

	//������..(�״¸޽����� ���߿� �������ϹǷ�..�ǳ��߿��Ѵ�.)
	for(int i = 0; i < at.m_nDamagedObjNum; i++)
		at.m_DamList[i].m_pChar->SetDamage(at.m_DamList[i].m_nDamage, this, m_Param.GetLevel(), at.m_bIsCrtAtt);

	//�ִϸӽ� ��ȯ�����ΰ�� �ִϸӽ����� �˸���..
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
		SendMsg_AttackResult_Error(sErrCode);	//�����޽���..
		return;
	}

	float fAddBulletFc = 1.0f;
	BYTE byBulletIndex = 0xFF;
	if(pBulletFld) 
	{
		fAddBulletFc += 0;	//��ź������ �߰� ���ݷ�
		byBulletIndex = pBulletFld->m_dwIndex;
	}
	int nLeftBulletNum = 0;
	
	CPlayerAttack at(this);

	_attack_param ap;
	make_unit_attack_param(pDst, pWeaponFld, fAddBulletFc, &ap);

	at.AttackUnit(&ap);

	_check_exp_after_attack(at.m_nDamagedObjNum, at.m_DamList);

	//�Ѿ˰���..
	if(pBulletParam->wLeftNum < pWeaponFld->m_nNeedBt)
		pBulletParam->wLeftNum = 0;
	else
		pBulletParam->wLeftNum -= pWeaponFld->m_nNeedBt;

	nLeftBulletNum = pBulletParam->wLeftNum;

	SendMsg_AlterUnitBulletInform(byWeaponPart, nLeftBulletNum);

	if(pBulletParam->wLeftNum == 0)	//ź�� ���� �Ҹ��ϸ� �ʱ�ȭ��Ų��.
		*(DWORD*)pBulletParam = 0xFFFFFFFF;		

	//�������
	SendMsg_AttackResult_Unit(&at, byWeaponPart, byBulletIndex);

	//������..(�״¸޽����� ���߿� �������ϹǷ�..�ǳ��߿��Ѵ�.)
	for(int i = 0; i < at.m_nDamagedObjNum; i++)
		at.m_DamList[i].m_pChar->SetDamage(at.m_DamList[i].m_nDamage, this, m_Param.GetLevel(), at.m_bIsCrtAtt);
}

void CPlayer::pc_PlayAttack_Test(BYTE byEffectCode, BYTE byEffectIndex, WORD wBulletItemSerial, BYTE byWeaponPart, short* pzTar)
{
	BYTE byRetCode = 0;	//1:Ʋ�� BULLET
	__ITEM* pBulletItem = NULL;
	WORD wBulletItemIndex = 0xFFFF;
	WORD wLeftBullet = 0;
	BYTE byEffectLv = 0;

	if(!IsRidingUnit())
	{
		//bullet�ճ�..
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

		//�Ѿ˰���..
		if(pBulletParam->wLeftNum < pWeaponFld->m_nNeedBt)
			pBulletParam->wLeftNum = 0;
		else
			pBulletParam->wLeftNum -= pWeaponFld->m_nNeedBt;

		int nLeftBulletNum = pBulletParam->wLeftNum;

		SendMsg_AlterUnitBulletInform(byWeaponPart, nLeftBulletNum);

		if(pBulletParam->wLeftNum == 0)	//ź�� ���� �Ҹ��ϸ� �ʱ�ȭ��Ų��.
			*(DWORD*)pBulletParam = 0xFFFFFFFF;		
	}

	SendMsg_TestAttackResult(byEffectCode, byEffectIndex, wBulletItemIndex, byEffectLv, byWeaponPart, pzTar);
}

int CPlayer::_pre_check_normal_attack(CCharacter* pDst, WORD wBulletSerial, __ITEM** OUT ppBulletProp, _BulletItem_fld** OUT ppfldBullet)
{
	__ITEM* l_pBulletProp = NULL;
	_BulletItem_fld* l_pfldBullet = NULL;

	if(!pDst)	//�ݵ�� ���ΰ������̾���..
		return error_attack_invalid_dst;

	if(IsRidingUnit())
		return error_attack_unit_state;	//����ž�»���

	//Bullet�� ��ȿ���� �˻�
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

	//����� ��ȿ��üũ
	if(!pDst->m_bLive || pDst->m_bCorpse || pDst->m_pCurMap != m_pCurMap || pDst->GetCurSecNum() == 0xFFFFFFFF)
		return error_attack_invalid_dst;

	if(pDst == this)	//�ڽ��� ����..
		return error_attack_invalid_dst;

	//���ݰ��ɴ��
	if(GetObjRace() == pDst->GetObjRace())
		return error_attack_invalid_dst;

	if(!IsBeAttackedAble())
		return error_attack_invalid_dst;

	DWORD dwCurTime = timeGetTime();
	//���ݵ����� üũ
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
	SetNextAttTime(dwNextTime - 300);	//������ 0.3�� ���ش�..

	//�����Ÿ�üũ
	float fDist = ::GetSqrt(pDst->m_fCurPos, m_fCurPos);
	if(fDist > m_pmWpn.wGaAttRange + m_EP.GetEff_Plus(_EFF_PLUS::GE_Att_Dist_+m_pmWpn.byWpClass))
	{//�����Ÿ��� ����� ������ ��ǥ�ε� ���غ���..
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

	//����� ��ȿ��üũ
	if(pDst)
	{
		if(pDst == this)	
			return error_attack_invalid_dst;
		if(!pDst->m_bLive || pDst->m_bCorpse || pDst->m_pCurMap != m_pCurMap || pDst->GetCurSecNum() == 0xFFFFFFFF)
			return error_attack_invalid_dst;
		if(GetObjRace() == pDst->GetObjRace())	//���ݰ��ɴ��
			return error_attack_invalid_dst;
		if(!pDst->IsBeAttackedAble())
			return error_attack_invalid_dst;
	}

	if(!pSkillFld)
		return error_attack_data;

	//���������� �����͸�����..
	if(byEffectCode == effect_code_skill)
	{
		if((DWORD)pSkillFld->m_nMastIndex > skill_mastery_num)
			return error_attack_data;
		if(!IsSFUsableSFMastery(mastery_code_skill, pSkillFld->m_nMastIndex, pSkillFld->m_nLv))
			return error_attack_few_mastery;
	}
	else
	{	//�ڽ��� �����ִ� Ŭ�����ΰ�..
		if(!m_Param.IsActableClassSkill(pSkillFld->m_strCode))
			return error_attack_not_have_skill;
	}

	if(IsRidingUnit())
		return error_attack_unit_state;	//����ž�»���

	if(m_EP.GetEff_State(_EFF_STATE::SK_Att_Lck))//PARAMETER EDIT (���ݽ�ų����)
		return error_attack_rule;
	
	//Bullet�� ��ȿ���� �˻�
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
	//�ùٸ� �����ΰ�
	if(m_pmWpn.GetAttackToolType() == weapon_equip_weapon)
	{
		if(pSkillFld->m_strFixWeapon[m_pmWpn.byWpType] == '1')
			bWpAble = true;
	}
	else
	{
		if(!strstr(pSkillFld->m_strFixWeapon, "1"))// m_strFixEquip�� ��� 0���� ���õǾ������� ���Ⱑ ��� �ȴ�.
			bWpAble = true;
	}
	if(!bWpAble)
		return error_attack_correctweapon;

	//�������� Ȯ��
	if(!pSkillFld->m_bAttackable)
		return error_attack_rule;

	DWORD dwCurTime = timeGetTime();
	//���������� üũ
	if(dwCurTime - m_pdwLastSkillActTime[pSkillFld->m_dwIndex] < pSkillFld->m_fActDelay - 500)
		return error_attack_delay;
	else
		m_pdwLastSkillActTime[pSkillFld->m_dwIndex] = dwCurTime;

	//�����Ÿ�üũ .. self area�ΰ��� ����..
	if(pDst)
	{
		if(::GetSqrt(pDst->m_fCurPos, m_fCurPos) > m_pmWpn.wGaAttRange + m_EP.GetEff_Plus(_EFF_PLUS::SK_Att_Dist))
		{
			if(::GetSqrt(pDst->m_fOldPos, m_fCurPos) > m_pmWpn.wGaAttRange + m_EP.GetEff_Plus(_EFF_PLUS::SK_Att_Dist))
				return error_attack_radius;
		}
	}

	//���ݵ����� üũ
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
		return error_attack_unit_state;	//����ž�»���

	l_pForceItem = m_Param.m_dbForce.GetPtrFromSerial(wForceItemSerial);
	if(!l_pForceItem)
		return error_attack_not_have_force;
	l_pForceFld = (_force_fld *)g_Main.m_tblEffectData[effect_code_force].GetRecord(s_pnLinkForceItemToEffect[l_pForceItem->m_wItemIndex]);
	if(!l_pForceFld)
		return error_attack_not_have_force;

	//�������ΰ�� ����� ���°ɷ��Ѵ�..
	if(l_pForceFld->m_nEffectGroup == sf_effect_code_self || l_pForceFld->m_nEffectGroup == sf_effect_code_extent)
		pDst = NULL;	

	//����� ��ȿ��üũ
	if(pDst)
	{	
		if(pDst == this)
			return error_attack_invalid_dst;
		if(!pDst->m_bLive || pDst->m_bCorpse || pDst->m_pCurMap != m_pCurMap || pDst->GetCurSecNum() == 0xFFFFFFFF)
			return error_attack_invalid_dst;
		if(GetObjRace() == pDst->GetObjRace())	//���ݰ��ɴ��
			return error_attack_invalid_dst;
		if(!pDst->IsBeAttackedAble())
			return error_attack_invalid_dst;
	}
	
	//���������� �����͸�����..
	if(!IsSFUsableSFMastery(mastery_code_force, l_pForceFld->m_nMastIndex, l_pForceFld->m_nLv))
		return error_attack_few_mastery;

	if(m_EP.GetEff_State(_EFF_STATE::FC_Att_Lck))//PARAMETER EDIT (������������)
		return error_attack_rule;

	//�ùٸ� �����ΰ�..
	if(m_pmWpn.byWpType != type_staff)
		return error_attack_correctweapon;

	//�������� Ȯ��
	if(!l_pForceFld->m_bAttackable)
		return error_attack_rule;

	if(l_pForceFld->m_nEffectGroup == sf_effect_code_extent)
	{
		if(!m_pCurMap->IsMapIn(pfTarPos))
			return error_attack_rule;			
	}

	DWORD dwCurTime = timeGetTime();
	//���������� üũ
	if(dwCurTime - m_pdwLastForceActTime[l_pForceFld->m_dwIndex] < l_pForceFld->m_fActDelay - 500)
		return error_attack_delay;
	else
		m_pdwLastForceActTime[l_pForceFld->m_dwIndex] = dwCurTime;

	//�����Ÿ�üũ
	if(pDst)	
	{
		int nLim = l_pForceFld->m_nActDistance+40 + m_EP.GetEff_Plus(_EFF_PLUS::FC_Att_Dist);//������ 40�� ���ش�..
		if(::GetSqrt(pDst->m_fCurPos, m_fCurPos) > nLim)
		{
			if(::GetSqrt(pDst->m_fOldPos, m_fCurPos) > nLim)
				return error_attack_radius;
		}
	}
	else 
	{
		if(l_pForceFld->m_nEffectGroup != sf_effect_code_self)//������ ����..
		{
			int nLim = l_pForceFld->m_nActDistance+40;
			if(::GetSqrt(pfTarPos, m_fCurPos) > nLim)
			{
				if(::GetSqrt(pfTarPos, m_fOldPos) > nLim)
					return error_attack_radius;
			}
		}
	}

	//���ݵ����� üũ
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

	//����ž��������..
	if(!IsRidingUnit())
		return error_attack_unit_state;

	//Bullet�� �ִ���..
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

	//����� �����̰��� ��ź����..
	if(l_pBulletFld->m_nWPType != l_pWeaponFld->m_nWPType)
		return error_attack_match_bullet;
	
	//����� ��ȿ��üũ
	if(!pDst->m_bLive || pDst->m_bCorpse || pDst->m_pCurMap != m_pCurMap || pDst->GetCurSecNum() == 0xFFFFFFFF)
		return error_attack_invalid_dst;

	//���ݰ��ɴ��
	if(GetObjRace() == pDst->GetObjRace())
		return error_attack_invalid_dst;

	if(!IsBeAttackedAble())
		return error_attack_invalid_dst;

	//�����Ÿ�üũ	
	if(::GetSqrt(pDst->m_fCurPos, m_fCurPos) > l_pWeaponFld->m_fAttackRange+40)
	{//�����Ÿ��� ����� ������ ��ǥ�ε� ���غ���..		
		if(::GetSqrt(pDst->m_fOldPos, m_fCurPos) > l_pWeaponFld->m_fAttackRange+40)
			return error_attack_radius;
	}

	DWORD dwCurTime = timeGetTime();
	//���ݵ����� üũ
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
	pAP->nMinAF = m_pmWpn.nGaMinAF * GetAdrenAF() * fAddBulletFc;	//�Ƶ巹����ȿ�� + �Ѿ˺ΰ�ȿ��
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
	{	//�ѹ濡 ������ ź����..
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
		memcpy(pAP->fArea, m_fCurPos, sizeof(float)*3);	//�ڽ��� ��ǥ��..
	else
		memcpy(pAP->fArea, pDst->m_fCurPos, sizeof(float)*3);	//Ÿ���� ��ǥ��..
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

		//����� �÷��̾��� ����ġ�Ѿ
		if(p->m_pChar->IsRewardExp() && p->m_nDamage > 0)
			CalcExp(p->m_pChar, p->m_nDamage);

		//���ڽ� ���¿��� ���ݹ����� ���ڽ� Ǯ����
		if(p->m_pChar->m_EP.GetEff_State(_EFF_STATE::Stealth))	//PARAMETER EDIT (STEALTH)
			p->m_pChar->RemoveSFContHelpByEffect(cont_param_state, _EFF_STATE::Stealth);

		//���� ���¿��� ���ݹ����� ��� Ǯ����.
		if(p->m_pChar->m_EP.GetEff_Plus(_EFF_PLUS::Transparency) > 0 && !p->m_pChar->m_bBreakTranspar)	//PARAMETER EDIT (Transparency)
			p->m_pChar->SetBreakTranspar(true);
	}

	return nTotalDam;
}

void CPlayer::_check_dst_param_after_attack(int nTotalDam, CCharacter* pTarget)
{
	if(m_EP.GetEff_Rate(_EFF_RATE::FP_Absorb) > 1.0f)	//PARAMETER EDIT (FP�����)
	{
		int nFPAbs = nTotalDam * (m_EP.GetEff_Rate(_EFF_RATE::FP_Absorb)-1);
		int nCurFP = m_Param.GetFP();
		nCurFP += nFPAbs;
		if(nCurFP > GetLimFP())
			nCurFP = GetLimFP();

		SetFP(nCurFP, false);
	}
	
	if(m_EP.GetEff_Rate(_EFF_RATE::HP_Absorb) > 1.0f)	//PARAMETER EDIT (HP�����)
	{
		int nHPAbs = nTotalDam * (m_EP.GetEff_Rate(_EFF_RATE::HP_Absorb)-1);
		int nCurHP = m_Param.GetHP();

		SetHP(nCurHP+nHPAbs, false);
	}

	//ù�濡 ���� ������� STUN.. ����..
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
	{	//����â���� ã�´�.
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
	//���ݰݱ�üũ
	if(!pDst)
		return false;

	if(pDst->m_EP.GetEff_State(_EFF_STATE::Res_Att))//PARAMETER EDIT (���ݰݱ�)
	{	//����� �ڽ��� �Ÿ��� ���� �����Ÿ��� 1.5���̳��ΰ�츸..
		double dDist = ::GetSqrt(pDst->m_fCurPos, m_fCurPos);
		if(dDist < pDst->GetAttackRange() * 1.5)
		{
			if(pDst->m_ObjID.m_byKind == obj_kind_char && pDst->m_ObjID.m_byID == obj_id_player)
				((CPlayer*)pDst)->m_pCountAttackDst = this;

			//�ݰݱ⸦ �����Ѵ�.
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