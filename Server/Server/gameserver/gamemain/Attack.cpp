#include "stdafx.h"
#include "Attack.h"
#include "MyUtil.h"
#include "Player.h"
#include "SkillFld.h"
#include "ForceFld.h"
#include "UnitPartFld.h"
#include "MainThread.h"
#include "ErrorEventFromServer.h"


#define PTA(a)   (((double)(a)*360.0)/(2.0*3.141592f))

static int s_nLimitDist[mastery_level_num] = { 42, 56, 70, 84 };
static int s_nLimitAngle[EFFECT_CODE_NUM][mastery_level_num] = { {45,  90, 180, 270}, {15, 30, 45, 60} };	
static int s_nLimitRadius[mastery_level_num] = { 42, 56, 70, 84 };

_attack_param	CAttack::s_DefParam;
BYTE			CAttack::s_bySkillIndexMT[MELEE_TECH_NUM];
CRecordData*	CAttack::s_pSkillData = NULL;

CAttack::CAttack(CCharacter* pThis)
{
	m_pAttChar = pThis;
	m_bIsCrtAtt = false;
	m_pp = &s_DefParam;
	m_nDamagedObjNum = 0;
}

void CAttack::AttackGen(_attack_param* pParam)
{
	m_nDamagedObjNum = 0;
	m_bIsCrtAtt = false;

	m_pp = pParam;
	bool bSucc = true;
	m_nDamagedObjNum = 0;

	if(!m_pp->pDst)
		return;

	if(!m_pp->bMatchless)
	{
		if(m_pp->pDst->m_EP.GetEff_State(_EFF_STATE::Abs_Avd)) //## PARAMETER EDIT (����ȸ��)
			bSucc = false;

		else if(m_pp->pDst->m_EP.GetEff_State(_EFF_STATE::Res_Att) && m_pp->nClass == wp_class_close)//PARAMETER EDIT (���ݰݱ�)
			bSucc = false;

		else if(m_pAttChar->m_rtPer100.GetRand() >= m_pAttChar->GetGenAttackProb(m_pp->pDst, m_pp->nPart))
			bSucc = false;
	}

	if(!bSucc)
	{
		m_DamList[0].m_pChar = m_pp->pDst;
		m_DamList[0].m_nDamage = 0;//�̽��� �������� 0����..
		m_nDamagedObjNum = 1;
		return;
	}

	//��������Ʈ
	float fAttFc = 1.0f;
	if(!m_pp->bMatchless)
	{
		fAttFc = _CalcGenAttPnt() + m_pp->nAddAttPnt;;
		fAttFc *= m_pAttChar->m_EP.GetEff_Rate(_EFF_RATE::GE_AttFc_ + m_pp->nClass);//PARAMETER EDIT (���ݷ�)
	}

	// damaged char setting
	m_DamList[0].m_pChar = m_pp->pDst;
	if(!m_pp->bMatchless)
		m_DamList[0].m_nDamage = m_pAttChar->GetAttackDamPoint(fAttFc, m_pp->nPart, m_pp->nTol, m_pp->pDst);
	else
		m_DamList[0].m_nDamage = m_pp->pDst->GetHP();
	m_nDamagedObjNum = 1;
}

void CAttack::AttackForce(_attack_param* pParam)
{
	m_nDamagedObjNum = 0;
	m_bIsCrtAtt = false;

	m_pp = pParam;
	_force_fld* pForceFld = (_force_fld*)m_pp->pFld;
	bool bSucc = true;

	if(m_pp->pDst)
	{
		if(m_pp->pDst->m_EP.GetEff_State(_EFF_STATE::Abs_Avd)) //## PARAMETER EDIT (����ȸ��)
			bSucc = false;

		else if(m_pp->pDst->m_EP.GetEff_State(_EFF_STATE::Res_Att) && m_pp->nClass == wp_class_close)//PARAMETER EDIT (���ݰݱ�)
			bSucc = false;
	}

	if(!bSucc)
	{
		if(m_pp->pDst)
		{
			m_DamList[0].m_pChar = m_pp->pDst;
			m_DamList[0].m_nDamage = 0;//�̽��� �������� 0����..
			m_nDamagedObjNum = 1;
		}
		return;
	}

	//���ݷ�..	
	float fAttPower = _CalcForceAttPnt() + m_pp->nAddAttPnt;
	fAttPower *= m_pAttChar->m_EP.GetEff_Rate(_EFF_RATE::FC_AttFc);//PARAMETER EDIT (����_����������)

	// �������̸� ������ damage�� �Դ� ĳ���͸� ã�Ƽ� ������ damage�� ���Ѵ�.
	switch(pForceFld->m_nEffectGroup)
	{
	case sf_effect_code_flash:
		FlashDamageProc(pForceFld->m_nLv, fAttPower, s_nLimitAngle[effect_code_force][pForceFld->m_nLv]);
		break;

	case sf_effect_code_self: case sf_effect_code_extent:
		AreaDamageProc(pForceFld->m_nLv, fAttPower, m_pp->fArea);		
		break;

	case sf_effect_code_one: case sf_effect_code_double: case sf_effect_code_combo:
		if(m_pp->pDst)
		{
			m_DamList[0].m_pChar = m_pp->pDst;
			m_DamList[0].m_nDamage = m_pAttChar->GetAttackDamPoint(fAttPower, m_pp->nPart, m_pp->nTol, m_pp->pDst);
			m_nDamagedObjNum = 1;
		}
		break;

	default:
		return;
	}
}

bool CAttack::_IsForceHit()
{
	if(m_pp->nMastery == 99)
		return true;

	float l_fHitRate;

	l_fHitRate = (50 + m_pp->nMastery * 0.5);// * m_pAttChar->GetEff_Rate(_EFF_RATE::FC_Hit); //PARAMETER EDIT (����Ȯ��)
	
	if(m_pAttChar->m_rtPer100.GetRand() < l_fHitRate)
		return true;

	return false;	
}

int CAttack::_CalcGenAttPnt()
{
	int nCrtAF = m_pp->nMaxAF * (2.5f/0.02f+m_pp->nMaxAF) / (1.0f/0.02f+m_pp->nMaxAF) + 0.5f;
	int nAFBlk = (m_pp->nMinAF + m_pp->nMaxAF)/2.0f + 0.5f;

	if(m_pAttChar->m_EP.GetEff_State(_EFF_STATE::Abs_Crt))//PARAMETER EDIT (������ũ��Ƽ��)
		return nCrtAF;

	int nProb[2];
	int nRan = ::rand()%100;

	nProb[0] = m_pp->nMinSel - m_pAttChar->m_EP.GetEff_Plus(_EFF_PLUS::GE_CrtExt);//PARAMETER EDIT (ũ��Ƽ�ü��÷�)
	if(nProb[0] < 0)		nProb[0] = 0;

	nProb[1] = m_pp->nMinSel + m_pp->nMaxSel - m_pAttChar->m_EP.GetEff_Plus(_EFF_PLUS::GE_CrtExt);
	if(nProb[1] < 0)		nProb[1] = 0;

	if(nRan <= nProb[0])
	{
		if(nAFBlk - m_pp->nMinAF >= 1)
			return m_pp->nMinAF + ::rand()%(nAFBlk - m_pp->nMinAF);
		else
			return m_pp->nMinAF;
	}

	else if(nRan <= nProb[1])
	{
		if(m_pp->nMaxAF - nAFBlk >= 1)
			return nAFBlk + ::rand()%(m_pp->nMaxAF - nAFBlk);
		else
			return nAFBlk;
	}

	m_bIsCrtAtt = true;
	return nCrtAF;
}

int CAttack::_CalcForceAttPnt()
{
	_force_fld* pForceFld = (_force_fld*)m_pp->pFld;

	static float fR = 0.847f;		// (1-(1/7+1/99))
	static float fRLf = 0.86472f;	// ���LV ����
	static float fRMf = 0.28824f;	// ���Mastery ����
	static float fRLVf = (float)m_pp->nLevel + (7 - (float)m_pp->nLevel) * 0.5f;	// ��Lv�϶��� ȿ���� ����
	
	float l_fConst = pForceFld->m_fAttFormulaConstant;	// ���� ���

	int l_nMinAf = (m_pp->nMinAF * (fR + (fRLVf/7 * fRLf) + ((float)m_pp->nMastery/99 * fRMf)) * l_fConst ) + 0.5;	// �ּ� ���ݷ�
	int l_nMaxAf = (m_pp->nMaxAF * (fR + (fRLVf/7 * fRLf) + ((float)m_pp->nMastery/99 * fRMf)) * l_fConst ) + 0.5;	// �ִ� ���ݷ�
	int l_nCrtAf = l_nMaxAf * ((125 + l_nMaxAf) / (50 + l_nMaxAf)) + 0.5;	// ũ��Ƽ�� ���ݷ�

	int l_nAttBlk = (float)(l_nMinAf + l_nMaxAf)/2 + 0.5;	// Min,Max ���ݷ� ���� �߰���

	int l_nAttProb = ::rand()%100;	// ���ݷ� ���� ����
	
	if(l_nAttProb < m_pp->nMinSel)
	{		
		if(l_nAttBlk - l_nMinAf > 0)
			return l_nMinAf + ::rand()%(l_nAttBlk - l_nMinAf);
		else
			return l_nMinAf;
	}
	else if(l_nAttProb < m_pp->nMinSel + m_pp->nMaxSel)
	{		
		if(l_nMaxAf - l_nAttBlk > 0)
			return l_nAttBlk + ::rand()%(l_nMaxAf - l_nAttBlk);
		else 
			return l_nAttBlk;
	}

	m_bIsCrtAtt = true;
	return l_nCrtAf;
}

void CAttack::FlashDamageProc(int nSkillLv, int nAttPower, int nAngle)
{
	float l_fInterval = s_nLimitDist[nSkillLv];

	if(!m_pp->pDst)
		return;

	m_DamList[0].m_pChar = m_pp->pDst;
	if(!m_pp->bMatchless)
		m_DamList[0].m_nDamage = m_pAttChar->GetAttackDamPoint(nAttPower, m_pp->nPart, m_pp->nTol, m_pp->pDst);
	else
		m_DamList[0].m_nDamage = m_pp->pDst->GetHP();
	m_nDamagedObjNum = 1;

	// ����� �߽����� ���ѵ� �Ÿ� ���� sector�� �����´�.
	if(m_pp->pDst->GetCurSecNum() == 0xFFFFFFFF)
		return;

	CMapData* pMap = m_pp->pDst->m_pCurMap;
	_pnt_rect l_rtArea;
	pMap->GetRectInRadius(&l_rtArea, 1, m_pp->pDst->GetCurSecNum());

	for(int h = l_rtArea.nStarty; h <= l_rtArea.nEndy; h++)
	{
		for(int w = l_rtArea.nStartx; w <= l_rtArea.nEndx; w++)
		{
			int l_nSecNum = w + h * pMap->GetSecInfo()->m_nSecNumW;			

			CObjectList* pList = m_pAttChar->m_pCurMap->GetSectorListObj(m_pAttChar->m_wMapLayerIndex, l_nSecNum);
			if(!pList)
				continue;
			
			_object_list_point* l_pObjPtr = pList->m_Head.m_pNext;
			while(l_pObjPtr != &pList->m_Tail)
			{
				CGameObject* l_pObj = l_pObjPtr->m_pItem;
				l_pObjPtr = l_pObjPtr->m_pNext;

				if(m_nDamagedObjNum >= max_damaged)
					return;

				//���ݰ��ɴ������..
				if(l_pObj == m_pAttChar || l_pObj == m_pp->pDst)
					continue;
				if(!l_pObj->IsBeAttackedAble())
					continue;
				if(l_pObj->GetObjRace() == m_pAttChar->GetObjRace())
					continue;
				
				//���� üũ..
				float fYGap = abs(m_pAttChar->m_fCurPos[1] - l_pObj->m_fCurPos[1]);
				if(m_pAttChar->AttackableHeight() < fYGap)
					continue;

				//�Ÿ�üũ..
				int l_nDistToDstChar = ::GetSqrt(m_pp->pDst->m_fCurPos, l_pObj->m_fCurPos);
				if(l_nDistToDstChar >= l_fInterval)
					continue;

				if(!_IsCharInSector(l_pObj->m_fCurPos, m_pAttChar->m_fCurPos, m_pp->pDst->m_fCurPos, nAngle, l_fInterval))
					continue;

				m_DamList[m_nDamagedObjNum].m_pChar = (CCharacter*)l_pObj;
				if(!m_pp->bMatchless)
				{
					int l_nAttPower = nAttPower * (float)(l_fInterval - l_nDistToDstChar) / l_fInterval;
					m_DamList[m_nDamagedObjNum].m_nDamage = m_pAttChar->GetAttackDamPoint(l_nAttPower, ((CCharacter*)l_pObj)->GetAttackRandomPart(), m_pp->nTol, (CCharacter*)l_pObj);
				}
				else
				{
					m_DamList[m_nDamagedObjNum].m_nDamage = ((CCharacter*)l_pObj)->GetHP();
				}
				m_nDamagedObjNum++;					
			}
		}
	}
}

void CAttack::AreaDamageProc(int nSkillLv, int nAttPower, float* pTar)
{
	float l_fInterval = s_nLimitRadius[nSkillLv];

	CMapData* pMap = m_pAttChar->m_pCurMap;
	int nSecNum = pMap->GetSectorIndex(pTar);
	if(nSecNum == -1)
		return;

	_pnt_rect l_rtArea;
	pMap->GetRectInRadius(&l_rtArea, 1, nSecNum);

	for(int h = l_rtArea.nStarty; h <= l_rtArea.nEndy; h++)
	{
		for (int w = l_rtArea.nStartx; w <= l_rtArea.nEndx; w++)
		{
			int l_nSecNum = w + h * pMap->GetSecInfo()->m_nSecNumW;

			CObjectList* pList = m_pAttChar->m_pCurMap->GetSectorListObj(m_pAttChar->m_wMapLayerIndex, l_nSecNum);
			if(!pList)
				continue;
			
			_object_list_point* l_pObjPtr = pList->m_Head.m_pNext;
			while(l_pObjPtr != &pList->m_Tail)
			{
				CGameObject* l_pObj = l_pObjPtr->m_pItem;
				l_pObjPtr = l_pObjPtr->m_pNext;
			
				if(m_nDamagedObjNum >= max_damaged)
					return;

				//���ݰ��ɴ������..
				if(l_pObj == m_pAttChar)
					continue;
				if(!l_pObj->IsBeAttackedAble())
					continue;
				if(l_pObj->GetObjRace() == m_pAttChar->GetObjRace())
					continue;

				//���� üũ..
				float fYGap = abs(m_pAttChar->m_fCurPos[1] - l_pObj->m_fCurPos[1]);
				if(attack_max_height < fYGap)
					continue;

				int l_nDistToAttChar = ::GetSqrt( pTar, l_pObj->m_fCurPos );
				if( l_nDistToAttChar > s_nLimitRadius[nSkillLv] )
					continue;

				m_DamList[m_nDamagedObjNum].m_pChar = (CCharacter *)l_pObj;
				if(!m_pp->bMatchless)
				{
					int l_nAttPower = nAttPower * (float)(l_fInterval - l_nDistToAttChar) / l_fInterval;
					m_DamList[m_nDamagedObjNum].m_nDamage = m_pAttChar->GetAttackDamPoint(l_nAttPower, ((CCharacter*)l_pObj)->GetAttackRandomPart(), m_pp->nTol, (CCharacter *)l_pObj);
				}
				else
				{
					m_DamList[m_nDamagedObjNum].m_nDamage = ((CCharacter *)l_pObj)->GetHP();
				}
				m_nDamagedObjNum++;					
			}
		}
	}	
}

void CAttack::SectorDamageProc(int nSkillLv, int nAttPower, int nAngle, int nShotNum, int nWeaponRange)
{
	int	l_nInterval = nWeaponRange;
	int	l_nOuterval = nWeaponRange/4;

	if(!m_pp->pDst)
		return;

	m_DamList[0].m_pChar = m_pp->pDst;
	if(!m_pp->bMatchless)
		m_DamList[0].m_nDamage = m_pAttChar->GetAttackDamPoint(nAttPower, m_pp->nPart, m_pp->nTol, m_pp->pDst);
	else
		m_DamList[0].m_nDamage = m_pp->pDst->GetHP();
	m_nDamagedObjNum = 1;

	// ����� �߽����� ���ѵ� �Ÿ� ���� sector�� �����´�.
	if(m_pAttChar->GetCurSecNum() == 0xFFFFFFFF)
		return;

	if(nShotNum > 1)
	{
		CMapData* pMap = m_pAttChar->m_pCurMap;
		_pnt_rect l_rtArea;
		pMap->GetRectInRadius(&l_rtArea, 1, m_pAttChar->GetCurSecNum());

		for(int h = l_rtArea.nStarty; h <= l_rtArea.nEndy; h++)
		{
			for(int w = l_rtArea.nStartx; w <= l_rtArea.nEndx; w++)
			{
				int l_nSecNum = w + h * pMap->GetSecInfo()->m_nSecNumW;			

				CObjectList* pList = pMap->GetSectorListObj(m_pAttChar->m_wMapLayerIndex, l_nSecNum);
				if(!pList)
					continue;
				
				_object_list_point* l_pObjPtr = pList->m_Head.m_pNext;
				while(l_pObjPtr != &pList->m_Tail)
				{
					CGameObject* l_pObj = l_pObjPtr->m_pItem;
					l_pObjPtr = l_pObjPtr->m_pNext;

					if(m_nDamagedObjNum >= max_damaged)
						return;

					if(m_nDamagedObjNum >= nShotNum)
						return;

					//���ݰ��ɴ������..
					if(l_pObj == m_pAttChar || l_pObj == m_pp->pDst)
						continue;
					if(!l_pObj->IsBeAttackedAble())
						continue;
					if(l_pObj->GetObjRace() == m_pAttChar->GetObjRace())
						continue;
					
					//���� üũ..
					float fYGap = abs(m_pAttChar->m_fCurPos[1] - l_pObj->m_fCurPos[1]);
					if(100 < fYGap)
						continue;

					//�Ÿ�üũ..
					int l_nDistToDstChar = ::GetSqrt(m_pAttChar->m_fCurPos, l_pObj->m_fCurPos);
					if(l_nDistToDstChar > l_nInterval || l_nDistToDstChar < l_nOuterval)
						continue;

					if(!_IsCharInSector(l_pObj->m_fCurPos, m_pAttChar->m_fCurPos, m_pp->pDst->m_fCurPos, nAngle, l_nInterval))
						continue;

					m_DamList[m_nDamagedObjNum].m_pChar = (CCharacter *)l_pObj;
					if(!m_pp->bMatchless)
					{
						int l_nAttPower = nAttPower * (float)(l_nInterval - l_nDistToDstChar) / l_nInterval;
						m_DamList[m_nDamagedObjNum].m_nDamage = m_pAttChar->GetAttackDamPoint(l_nAttPower, ((CCharacter*)l_pObj)->GetAttackRandomPart(), m_pp->nTol, (CCharacter*)l_pObj);
					}
					else
					{
						m_DamList[m_nDamagedObjNum].m_nDamage = ((CCharacter*)l_pObj)->GetHP();
					}
					m_nDamagedObjNum++;					
				}
			}
		}
	}
}

void CAttack::SetStaticMember(CRecordData* pSkillData)
{
	s_pSkillData = pSkillData;

	s_bySkillIndexMT[0] = 0xFF;
	for(int i = 1; i < MELEE_TECH_NUM; i++)
		s_bySkillIndexMT[i] = GetMeleeSkillIndex(i);

}

int CAttack::GetMeleeSkillIndex(int nMeleeTechCode)
{
	int nRecNum = s_pSkillData->GetRecordNum();

	for(int i = 0; i < nRecNum; i++)
	{
		_skill_fld* l_pFld = (_skill_fld*)s_pSkillData->GetRecord(i);

		if(l_pFld->m_nClass != wp_class_close)
			continue;
		
		if(l_pFld->m_nEtc == nMeleeTechCode)		
			return i;
	}

	return -1;
}

BOOL CAttack::_IsCharInSector(Vector3f chkpos, Vector3f src, Vector3f dest, float angle, float radius)         // ��ä�þȿ� �ִ°�?
{
	if( src[0] == dest[0] && src[2] == dest[2] )
		return FALSE;      //���� �ȵȴ�.

	if( chkpos[0] == dest[0] && chkpos[2] == dest[2] )
		return TRUE;       //���� �ȿ� �ִ°ɷ� ģ��.

	Vector3f a,b;
	float temp;

	// ���� ������ �ȿ� ���Դ���..
	Vector3fSub(chkpos, dest, a);
	
	temp = Vector3fDist(a);
	if( temp > radius )                          //�Ÿ��� �����.
		return FALSE;

	// cos0���� �̿��Ѵ�.
	Vector3fSub(dest, src, b);
	Normalize(a);
	Normalize(b);
	temp = Vector3fDot(a, b);   //a,b���̺����� cos0���̴�.

	float theta = acos(temp);

	theta = PTA(theta);                 //������ ��ȯ

	if(theta < angle/2)            //�����ȿ� ���.
		return TRUE;

	return FALSE;
}

void CPlayerAttack::AttackSkill(_attack_param* pParam)
{
	m_nDamagedObjNum = 0;
	m_bIsCrtAtt = false;

	m_pp = pParam;
	_skill_fld* pSkillFld = (_skill_fld*)m_pp->pFld;
	bool bSucc = true;

	int nAttType = pSkillFld->m_nAttType[m_pp->nLevel-1];
	
	if(m_pp->pDst)
	{
		if(m_pp->pDst->m_EP.GetEff_State(_EFF_STATE::Abs_Avd)) //## PARAMETER EDIT (����ȸ��)
			bSucc = false;

		else if(m_pp->pDst->m_EP.GetEff_State(_EFF_STATE::Res_Att) && m_pp->nClass == wp_class_close)//PARAMETER EDIT (���ݰݱ�)
			bSucc = false;

		if(!bSucc)
		{
			m_DamList[0].m_pChar = m_pp->pDst;
			m_DamList[0].m_nDamage = 0;//�̽��� �������� 0����..
			m_nDamagedObjNum = 1;
			return;
		}
	}

	// ���ݷ�
	int l_nAttPower = _CalcSkillAttPnt() + m_pp->nAddAttPnt;
	l_nAttPower *= m_pAttChar->m_EP.GetEff_Rate(_EFF_RATE::SK_AttFc);//PARAMETER EDIT (��ų_����������)

	switch(nAttType)
	{
	case sf_effect_code_flash:// �������̸� ������ damage�� �Դ� ĳ���͸� ã�Ƽ� list�� �߰��Ѵ�.
		FlashDamageProc(pSkillFld->m_nLv, l_nAttPower, s_nLimitAngle[effect_code_skill][pSkillFld->m_nLv]);
		break;

	case sf_effect_code_self: case sf_effect_code_extent:
		AreaDamageProc(pSkillFld->m_nLv, l_nAttPower, m_pp->fArea);	
		break;
	
	case sf_effect_code_sector:
		SectorDamageProc(pSkillFld->m_nLv, l_nAttPower, s_nLimitAngle[effect_code_skill][pSkillFld->m_nLv], m_pp->nShotNum, m_pp->nWpRange);
		break;
	
	case sf_effect_code_one: case sf_effect_code_double: case sf_effect_code_combo: case sf_effect_code_penalty:
		if(m_pp->pDst)
		{
			m_DamList[0].m_pChar = m_pp->pDst;
			m_DamList[0].m_nDamage = m_pAttChar->GetAttackDamPoint(l_nAttPower, m_pp->nPart, m_pp->nTol, m_pp->pDst);
			m_nDamagedObjNum = 1;		
		}
		break;

	default:
		return;
	}
}

bool CPlayerAttack::_IsSkillHit()
{
	if(m_pp->nMastery == 99)
		return true;

	float l_fHitRate = (50 + m_pp->nMastery * 0.5);// * m_pAttChar->GetEff_Rate(_EFF_RATE::SK_Hit);	//PARAMETER EDIT (����Ȯ��)
	
	if(m_pAttChar->m_rtPer100.GetRand() < l_fHitRate)
		return true;

	return false;
}

int CPlayerAttack::_CalcSkillAttPnt()
{
	_skill_fld* pSkillFld = (_skill_fld*)m_pp->pFld;
	
	static float fR = 0.847f;		// (1-(1/7+1/99))
	static float fRLf = 0.86472f;	// ���LV ����
	static float fRMf = 0.28824f;	// ���Mastery ����
	static float fRLVf = (float)m_pp->nLevel + (7 - (float)m_pp->nLevel) * 0.5f;	// ��Lv�϶��� ȿ���� ����

	float l_fConst = pSkillFld->m_fAttFormulaConstant;	// ���� ���
	int l_nLvConst = pSkillFld->m_nAttConstant[m_pp->nLevel-1];	// ���� ���

	int l_nMinAf = (float)l_nLvConst/788 * (m_pp->nMinAF * (fR + (fRLVf/7 * fRLf) + ((float)m_pp->nMastery/99 * fRMf)) * l_fConst ) + 0.5;	// �ּ� ���ݷ�
	int l_nMaxAf = (float)l_nLvConst/788 * (m_pp->nMaxAF * (fR + (fRLVf/7 * fRLf) + ((float)m_pp->nMastery/99 * fRMf)) * l_fConst ) + 0.5;	// �ִ� ���ݷ�
	int l_nCrtAf = l_nMaxAf * ((125 + l_nMaxAf) / (50 + l_nMaxAf)) + 0.5;	// ũ��Ƽ�� ���ݷ�

	int l_nAttBlk = (float)(l_nMinAf + l_nMaxAf)/2 + 0.5;	// Min,Max ���ݷ� ���� �߰���

	int l_nAttProb = ::rand()%100;	// ���ݷ� ���� ����
	
	if(l_nAttProb < m_pp->nMinSel)
	{		
		if(l_nAttBlk - l_nMinAf > 0)
			return l_nMinAf + ::rand()%(l_nAttBlk - l_nMinAf);
		else
			return l_nMinAf;
	}
	else if(l_nAttProb < m_pp->nMinSel + m_pp->nMaxSel)
	{		
		if(l_nMaxAf - l_nAttBlk > 0)
			return l_nAttBlk + ::rand()%(l_nMaxAf - l_nAttBlk);
		else 
			return l_nAttBlk;
	}

	m_bIsCrtAtt = true;
	return l_nCrtAf;
}

void CPlayerAttack::AttackUnit(_attack_param* pParam)
{
	m_nDamagedObjNum = 0;
	m_bIsCrtAtt = false;

	m_pp = pParam;
	_UnitPart_fld* pWeaponFld = (_UnitPart_fld*)m_pp->pFld;
	bool bSucc = true;

	if(!m_pp->pDst)
		return;	//����� �������������� �׳� �Ѿ..

	// effectgroup
	int nEffectGroup = pWeaponFld->m_nEffectGroup;
	if(nEffectGroup == sf_effect_code_self)
		return;	//���ֿ��� self������ ����..

	if(m_pp->pDst)
	{
		if(m_pp->pDst->m_EP.GetEff_State(_EFF_STATE::Abs_Avd)) //## PARAMETER EDIT (����ȸ��)
			bSucc = false;

		else if(m_pp->pDst->m_EP.GetEff_State(_EFF_STATE::Res_Att) && m_pp->nClass == wp_class_close)//PARAMETER EDIT (���ݰݱ�)
			bSucc = false;

		else if(m_pAttChar->m_rtPer100.GetRand() >= m_pAttChar->GetGenAttackProb(m_pp->pDst, m_pp->nPart))
			bSucc = false;
	}

	if(!bSucc)
	{
		m_DamList[0].m_pChar = m_pp->pDst;
		m_DamList[0].m_nDamage = 0;//�̽��� �������� 0����..
		m_nDamagedObjNum = 1;
		return;
	}

	// ���ݷ�
	int l_nAttPower = _CalcGenAttPnt() + m_pp->nAddAttPnt;

	//�����͸������� ������ 3���� ����..

	switch(nEffectGroup)
	{
	case sf_effect_code_flash:// �������̸� ������ damage�� �Դ� ĳ���͸� ã�Ƽ� list�� �߰��Ѵ�.
		FlashDamageProc(3, l_nAttPower, s_nLimitAngle[effect_code_skill][3]);
		break;

	case sf_effect_code_extent:
		AreaDamageProc(3, l_nAttPower, m_pp->fArea);	
		break;
	
	case sf_effect_code_sector:
		SectorDamageProc(3, l_nAttPower, s_nLimitAngle[effect_code_skill][3], m_pp->nShotNum, m_pp->nWpRange);
		break;
	
	case sf_effect_code_one: case sf_effect_code_double: case sf_effect_code_combo: case sf_effect_code_penalty:
		if(m_pp->pDst)
		{
			m_DamList[0].m_pChar = m_pp->pDst;
			m_DamList[0].m_nDamage = m_pAttChar->GetAttackDamPoint(l_nAttPower, m_pp->nPart, m_pp->nTol, m_pp->pDst);
			m_nDamagedObjNum = 1;		
		}
		break;

	default:
		return;
	}
}

