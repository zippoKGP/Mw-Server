// Character.cpp: implementation of the CCharacter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Character.h"
#include "R3Bsp.h"
#include <mmsystem.h>
#include "MyUtil.h"
#include "pt_zone_client.h"
#include "MainThread.h"
#include "CommonUtil.h"
#include "ErrorEventFromServer.h"

const double g_pi = 3.1415926535;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCharacter::CCharacter()
{
}

CCharacter::~CCharacter()
{

}

void CCharacter::Init(_object_id* pID)
{
	CGameObject::Init(pID);
	m_EP.Init();
	SFContInit();
}

bool CCharacter::Create(_character_create_setdata* pData)
{
	if(CGameObject::Create((_object_create_setdata*)pData))
	{
		memcpy(m_fTarPos, m_fCurPos, sizeof(float)*3);
		m_bObserver = false;

		m_dwNextAttackTime = 0;
		m_wLastContEffect = 0xFFFF;
		m_wLastContEffect_Old = 0xFFFF;
		m_nContEffectSec = -1;
		m_pLastContEffect = NULL;
		m_tmrSFCont.BeginTimer(500);
		SFContInit();
//		m_byJumpSectorCount = 0;	

		return true;
	}
	return false;
}

bool CCharacter::Destroy()
{
	CGameObject::Destroy();

	SFContInit();
	m_EP.Init();	//create���� �ʱ�ȭ�ؼ��¾ȉ´�..!!(CPlayer�ǰ�� create�������É�)

	return true;
}

int CCharacter::GetAttackDamPoint(int nAttPnt, int nAttPart, int nTolType, CCharacter* pDst)
{
	//����
	float fDefPnt = 0;
	float fProp = 1.1f;
	int nDamage;
	float fStdAttFc;
	float fSecDstFc;

	//�������..
	int nTolFc = pDst->GetTotalTol(nTolType, nAttPnt);

	if(!pDst->m_EP.GetEff_State(_EFF_STATE::Dst_No_Def))	//######### PARAMETER EDIT (����� ����)#################
		fDefPnt = (float)pDst->GetDefFC(nAttPart, this) * pDst->m_EP.GetEff_Rate(_EFF_RATE::Part_Def);//## PARAMETER EDIT (����)	
	else
		pDst->SetAttackPart(nAttPart);

	if(nTolType == TOL_CODE_NOTHING)
		fProp = 1.0f;

	//��հ���������.. (�������� ���Ⱓ�������� + ������� ����������)/2
	float fAveAdj = (GetWeaponAdjust() + pDst->GetDefGap())/2;

	//���ذ��ݷ� : {(�����°�*((����/2)*����������))-(����/2)}/(�����°�-1)
	float fDono1 = (pDst->GetDefFacing() - 1);
	if(fDono1 != 0)
		fStdAttFc = ((pDst->GetDefFacing()*fDefPnt/2*pDst->GetDefGap()) - (fDefPnt/2)) / fDono1;
	else
		fStdAttFc = 0.0f;
	
    //2�������� : (���ذ��ݷ�-(����/2))/(���ذ��ݷ�-((����/2)*��հ���������))
	float fDono2 = (fStdAttFc - (fDefPnt/2*fAveAdj));
	if(fDono2 != 0.0f)
		fSecDstFc = (fStdAttFc - fDefPnt/2) / (fStdAttFc - (fDefPnt/2*fAveAdj));
	else
		fSecDstFc = 0;

	//������������Ʈ
	nDamage = (nAttPnt * fProp + nTolFc - fDefPnt/2 * fAveAdj) * fSecDstFc;

	if(nDamage < 1)
		nDamage = 1;

	return nDamage;
}

void CCharacter::Move(float fSpeed)	
{
	if(m_bMove)
	{
		memcpy(m_fOldPos, m_fCurPos, sizeof(float)*3);

		int nAngle = ::GetYAngle(m_fCurPos, m_fTarPos);
		int nSqrt = (int)::GetSqrt(m_fCurPos, m_fTarPos);
		float fMoveSpeed = ::R3GetLoopTime() * STANDARD_MOVE_SPEED * fSpeed;
		float fOldCur[3];
		memcpy(fOldCur, m_fCurPos, sizeof(float)*3);

		if(nSqrt <= fMoveSpeed)
		{
			m_fTarPos[1] = m_fCurPos[1];
			if(!m_pCurMap->m_Level.GetNextYposForServer(m_fTarPos, &m_fTarPos[1]))
			{
				if(!m_pCurMap->m_Level.GetNextYposForServerFar(m_fCurPos,m_fTarPos, &m_fTarPos[1]))
				{
					memcpy(m_fCurPos, fOldCur, sizeof(float)*3);
					memcpy(m_fTarPos, m_fCurPos, sizeof(float)*3);
				}
			}
			else                    
			   memcpy(m_fCurPos, m_fTarPos, sizeof(float)*3);

			Stop();
			return;
		}
		else
		{
			double dAngleToPi = AngleToPi(nAngle);
			m_fCurPos[0] -= (float)sin(dAngleToPi)*fMoveSpeed;
			m_fCurPos[2] -= (float)cos(dAngleToPi)*fMoveSpeed;
		}

		BOOL bRet;

		m_fCurPos[1]=fOldCur[1];
		bRet = m_pCurMap->m_Level.GetNextYposForServerFar(fOldCur,m_fCurPos,&m_fCurPos[1]);

		if(!bRet) //��簡 ���Ұ�� �ü��ִ�.
		{
			if(!m_pCurMap->m_Level.GetNextYposFarProgress(fOldCur,m_fCurPos,&m_fCurPos[1]))
			{
				memcpy(m_fCurPos, fOldCur, sizeof(float)*3);
				memcpy(m_fTarPos, m_fCurPos, sizeof(float)*3);

				Stop();
			}
		}
	}
}

bool CCharacter::SetTarPos(float* fTarPos, bool bColl/* = true*/)
{
	if(!m_pCurMap->IsMapIn(fTarPos))
		return false;

	Vector3f fNew;

	//�����ִ� ���ΰ�?
	if(bColl)//�浹ó���Ѵ�.
	{
		if(m_pCurMap->m_Level.mBsp->CanYouGoThere(m_fCurPos, fTarPos, &fNew))
		{
			memcpy(m_fTarPos, fTarPos, sizeof(float)*3);
		}
		else	//�����°��̶�� ���� ������ fNew�� �����Ѵ�. 
		{
			fNew[1] = m_fCurPos[1];
			memcpy(m_fTarPos, fNew, sizeof(float)*3);
		}
	}
	else
		memcpy(m_fTarPos, fTarPos, sizeof(float)*3);

	Go();

	///*MAYUN
	ResetSlot();
	//*/
	return true;		
}

void CCharacter::Stop()
{
	memcpy(m_fTarPos, m_fCurPos, sizeof(float)*3);
	
	if(m_bMove)
	{
		m_bMove = false;		
	}
}

void CCharacter::Go()
{
	if(!m_bMove)
	{
		m_bMove = true;
	}
}

int CCharacter::GetTotalTol(BYTE byAttTolType, int nDamPoint)
{	 	
	float fTol[4] = {(float)GetFireTol()/100, (float)GetWaterTol()/100, (float)GetSoilTol()/100, (float)GetWindTol()/100};

	switch(byAttTolType)
	{
	case TOL_CODE_FIRE:
		return -(nDamPoint*fTol[TOL_CODE_FIRE]) -(nDamPoint*fTol[TOL_CODE_WATER]) +(nDamPoint*fTol[TOL_CODE_SOIL]);		
		
	case TOL_CODE_WATER:
		return +(nDamPoint*fTol[TOL_CODE_FIRE]) -(nDamPoint*fTol[TOL_CODE_WATER]) -(nDamPoint*fTol[TOL_CODE_WIND]);

	case TOL_CODE_SOIL:
		return -(nDamPoint*fTol[TOL_CODE_FIRE]) -(nDamPoint*fTol[TOL_CODE_SOIL]) +(nDamPoint*fTol[TOL_CODE_WIND]);		

	case TOL_CODE_WIND:
		return +(nDamPoint*fTol[TOL_CODE_WATER]) -(nDamPoint*fTol[TOL_CODE_SOIL]) -(nDamPoint*fTol[TOL_CODE_WIND]);
	}
	return 0;
}

int CCharacter::GetAttackRandomPart()
{
	//23, 22, 18, 17, 20
	int nRate[base_fix_num] = {23, 45, 63, 80, 100};
	int nRand = ::rand()%100;
	for(int i = 0; i < base_fix_num; i++)
	{
		if(nRand < nRate[i])
			return i;
	}
	
	return -1;
}

///*MAYUN


int CCharacter::GetNearEmptySlot(int pos,float dist,Vector3f cur)
{
	const double pi = 3.1415926535;
	//int max = slot_max;
	int n = pos;
	int m = pos;
	float fpos[3];
	Vector3f fNew;
	for (int i=0;i<slot_max;i++)
	{		
		if (n>=slot_max) n = 0;
		if (m<0) m = slot_max-1;
		
		if (!m_AroundSlot[n] ) 
		{
			memcpy(fpos , m_fTarPos, sizeof(float)*3);
			float rad2 = (2*pi)*(float(n+1))/slot_max;
			fpos[0] += cos(rad2)*dist;
			fpos[2] += sin(rad2)*dist;
			if(m_pCurMap->m_Level.mBsp->CanYouGoThere(cur, (Vector3f)fpos, &fNew))return n;
		}
		if (!m_AroundSlot[m] ) 
		{
			memcpy(fpos , m_fTarPos, sizeof(float)*3);
			float rad2 = (2*pi)*(float(m+1))/slot_max;
			fpos[0] += cos(rad2)*dist;
			fpos[2] += sin(rad2)*dist;
			if(m_pCurMap->m_Level.mBsp->CanYouGoThere(cur, (Vector3f)fpos, &fNew))return m;
		}
		n++;
		m--;

	}
	return -1;
}
int CCharacter::InsertSlot(CCharacter *p,int pos)
{
	if (m_AroundSlot[pos]==p) return true;
	if (m_AroundSlot[pos]) return false;
	
	m_AroundSlot[pos] = p;
	m_AroundNum++;
	return true;
}
int CCharacter::RemoveSlot(CCharacter *p)
{
	for (int i = 0;i<slot_max;i++)
	{
		if (m_AroundSlot[i]== p) 
		{
			m_AroundSlot[i] = NULL;
			m_AroundNum--;
			return true;
		}
	}
	return false;
}

int CCharacter::GetSlot(CCharacter *p)
{
	for (int i = 0;i<slot_max;i++)
	{
		if (m_AroundSlot[i]== p) 
		{
			return i;
		}
	}
	return -1;
}
void CCharacter::ResetSlot()
{
	if(m_AroundNum)
	{
		memset(m_AroundSlot, 0, sizeof(CCharacter*)*slot_max);
		m_AroundNum = 0;
	}
}
//*/MAYUN

BYTE CCharacter::InsertSFContEffect(BYTE byContCode, BYTE byEffectCode, DWORD dwEffectIndex, WORD wDurSec, BYTE byLv, bool* OUT pbUpMty)
{
	*pbUpMty = true;	//0���ϻ��¿����� *pbUpMty�� �����Ѵ�..

	if(byContCode == sf_cont_code_damage)
	{//PARAMETER EDIT (�������غ��� �Ȱɸ�)
		if(m_EP.GetEff_Plus(_EFF_PLUS::Anti_CtDm) > 0)
			return error_sf_anti_cont_damage;
	}

	DWORD dwCurSec = _sf_continous::GetSFContCurTime();//(timeGetTime()/1000)%10000;

	//���� ȿ���� ������ ����
	for(int i = 0; i < CONT_SF_SIZE; i++)
	{
		_sf_continous* p = &m_SFCont[byContCode][i];
		if(!p->m_bExist)
			continue;
		
		if(p->m_byEffectCode == byEffectCode && p->m_wEffectIndex == dwEffectIndex)
		{//�����Ŷ�� ���������� �߰��ҷ��°ͺ��� Ŭ���� �Ѿ��..
			if(p->m_byLv > byLv)
				return error_sf_existing_lv_better;

			//�����ð��� 10%�̻��̶��..����
			//�����ð�..
			int nLeftSec = p->m_wDurSec - (dwCurSec - p->m_wStartSec);			
			if((float)nLeftSec/p->m_wDurSec > 0.1f)//10%
				return error_sf_existing_dur_10pro;

			RemoveSFContEffect(byContCode, i);
			break;
		}
	}

	//�󽽷�ã��
	_sf_continous* pCont = NULL;
	for(i = 0; i < CONT_SF_SIZE; i++)
	{
		_sf_continous* p = &m_SFCont[byContCode][i];
		if(!p->m_bExist)
		{
			pCont = p;
			break;
		}		
	}
	if(!pCont)
	{//������ �����Ȱ� push..		
		pCont = &m_SFCont[byContCode][0];
		int nOldIndex = 0;
		for(i = 1; i < CONT_SF_SIZE; i++)
		{
			_sf_continous* p = &m_SFCont[byContCode][i];
			if(p->m_wStartSec < pCont->m_wStartSec)
			{
				pCont = p;
				nOldIndex = i;
			}
		}

		//ã������ 10%�̻� �����Ŷ�� �����͸��� ������..
		int nLeftSec = pCont->m_wDurSec - (dwCurSec - pCont->m_wStartSec);			
		if((float)nLeftSec/pCont->m_wDurSec > 0.1f)//10%
			*pbUpMty = false;

		RemoveSFContEffect(byContCode, nOldIndex, false);
	}

	float fAddTime = 1.0f;
	if(byContCode == sf_cont_code_damage)
	{//PARAMETER EDIT (����_�������غ���_�ð�_������)
		if(byEffectCode == effect_code_force)
			fAddTime = m_EP.GetEff_Rate(_EFF_RATE::FC_CtDm_Tm);
	}

	// insert new continous damage
	pCont->m_bExist = true;
	pCont->m_byEffectCode = byEffectCode;
	pCont->m_wEffectIndex = (WORD)dwEffectIndex;
	pCont->m_byLv = byLv;
	pCont->m_wStartSec = dwCurSec;

	if(m_nContEffectSec == -1)
		pCont->m_wDurSec = wDurSec * fAddTime;
	else//ȿ������ ġƮ���̶��..
		pCont->m_wDurSec = m_nContEffectSec;

	//�ɷ�ġ����..
	_base_fld* pEffectFld = g_Main.m_tblEffectData[byEffectCode].GetRecord(dwEffectIndex);
	_cont_param_list* pfEffectValue = NULL;
	if(byEffectCode == effect_code_skill)
		pfEffectValue = ((_skill_fld*)pEffectFld)->m_ContParamList;
	else if(byEffectCode == effect_code_force)
		pfEffectValue = ((_force_fld*)pEffectFld)->m_ContParamList;
	else if(byEffectCode == effect_code_class)
		pfEffectValue = ((_skill_fld*)pEffectFld)->m_ContParamList;

	for(i = 0; i < max_cont_param; i++)
	{
		_cont_param_list* p = &pfEffectValue[i];
		if(p->m_nContParamCode == -1)
			break;

		switch(p->m_nContParamCode)
		{
		case cont_param_rate:
			m_EP.SetEff_Rate(p->m_nContParamIndex, p->m_fContValue[byLv-1], true);	break;
		case cont_param_plus:
			m_EP.SetEff_Plus(p->m_nContParamIndex, p->m_fContValue[byLv-1], true);	break;
		case cont_param_state:
			m_EP.SetEff_State(p->m_nContParamIndex, true);		break;
		}
	}

	//�ֱٰɸ� ȿ�� ������Ʈ	
	WORD wBufferLastContEffect = CalcEffectBit(byEffectCode, dwEffectIndex);
	m_pLastContEffect = pCont;

	m_wLastContEffect = wBufferLastContEffect;

	SendMsg_AddEffect(m_wLastContEffect, byLv);

	return 0;
}

void CCharacter::RemoveSFContEffect(BYTE byContCode, WORD wListIndex, bool bInit)
{
	_sf_continous* pCon = &m_SFCont[byContCode][wListIndex];
	if(!pCon->m_bExist)
		return;

	//�ɷ�ġ����..
	_base_fld* pEffectFld = g_Main.m_tblEffectData[pCon->m_byEffectCode].GetRecord(pCon->m_wEffectIndex);
	_cont_param_list* pfEffectValue = NULL;
	if(pCon->m_byEffectCode == effect_code_skill)
		pfEffectValue = ((_skill_fld*)pEffectFld)->m_ContParamList;
	else if(pCon->m_byEffectCode == effect_code_force)
		pfEffectValue = ((_force_fld*)pEffectFld)->m_ContParamList;
	else if(pCon->m_byEffectCode == effect_code_class)
		pfEffectValue = ((_skill_fld*)pEffectFld)->m_ContParamList;

	for(int i = 0; i < max_cont_param; i++)
	{
		_cont_param_list* p = &pfEffectValue[i];
		if(p->m_nContParamCode == -1)
			break;

		switch(p->m_nContParamCode)
		{
		case cont_param_rate:
			m_EP.SetEff_Rate(p->m_nContParamIndex, p->m_fContValue[pCon->m_byLv-1], false);	break;
		case cont_param_plus:
			m_EP.SetEff_Plus(p->m_nContParamIndex, p->m_fContValue[pCon->m_byLv-1], false);	break;
		case cont_param_state:
			m_EP.SetEff_State(p->m_nContParamIndex, false);		break;
		}
	}
	// remove
	pCon->m_bExist = false;
	
	if(bInit)	//�ʱ�ȭ��� ���ϸ� �������� �ʴ´�.
		return;

	SendMsg_DelEffect(pCon->m_byEffectCode, pCon->m_wEffectIndex, pCon->m_byLv);

	//�ֱٰ��� ������Ŷ�� ���� �ֱٰ� ã��..
	if(m_pLastContEffect != pCon)
		return;

	int nStartSec = 0;
	int nLastIndex = -1;
	_sf_continous* pLastCon = NULL;

	for(i = 0; i < SF_CONT_CODE_NUM; i++)
	{
		for(int c = 0; c < CONT_SF_SIZE; c++)
		{
			_sf_continous* p = &m_SFCont[c][i];
			if(p->m_bExist)
				continue;

			if(p->m_wStartSec > nStartSec)
			{
				nStartSec = p->m_wStartSec;
				pLastCon = p;
			}
		}
	}
	
	WORD wBufferLastContEffect = 0xFFFF;

	if(nLastIndex == -1)
	{
		m_pLastContEffect = NULL;		
	}
	else	
	{
		wBufferLastContEffect = CalcEffectBit(pLastCon->m_byEffectCode, pLastCon->m_wEffectIndex); 
		m_pLastContEffect = pLastCon;
	}

	m_wLastContEffect = wBufferLastContEffect;
}

void CCharacter::RemoveSFContHelpByEffect(int nContParamCode, int nContParamIndex)
{
	//���ԵȰ��� ã�Ƽ� ���ش�..
	for(int i = 0; i < CONT_SF_SIZE; i++)
	{
		_sf_continous* pCon = &m_SFCont[sf_cont_code_help][i];
		if(!pCon->m_bExist)
			continue;

		_base_fld* pEffFld = g_Main.m_tblEffectData[pCon->m_byEffectCode].GetRecord(pCon->m_wEffectIndex);
		_cont_param_list* pfEffectValue = NULL;
		if(pCon->m_byEffectCode == effect_code_skill)
			pfEffectValue = ((_skill_fld*)pEffFld)->m_ContParamList;
		else if(pCon->m_byEffectCode == effect_code_force)
			pfEffectValue = ((_force_fld*)pEffFld)->m_ContParamList;
		else if(pCon->m_byEffectCode == effect_code_class)
			pfEffectValue = ((_skill_fld*)pEffFld)->m_ContParamList;

		for(int i = 0; i < max_cont_param; i++)
		{
			_cont_param_list* p = &pfEffectValue[i];
			if(p->m_nContParamCode == -1)
				break;

			if(p->m_nContParamCode == nContParamCode && p->m_nContParamIndex == nContParamIndex)
			{
				// effect�� �����Ѵ�.
				RemoveSFContEffect(sf_cont_code_help, i);
				break;
			}
		}
	}
}

void CCharacter::UpdateSFCont()
{	
	if(!m_tmrSFCont.CountingTimer())
		return;

	DWORD dwCurSec = _sf_continous::GetSFContCurTime();//(timeGetTime()/1000)%10000;

	for(int i = 0; i < SF_CONT_CODE_NUM; i++)
	{
		for(int c = 0; c < CONT_SF_SIZE; c++)
		{
			_sf_continous* p = &m_SFCont[i][c];
			if(!p->m_bExist)
				continue;

			if(!IsRecvableContEffect())	//����ȿ�����������϶��� ȿ��Ÿ�ֵ̹� �����´�..
				p->m_wStartSec += (m_tmrSFCont.GetTerm()/1000);
			else if(dwCurSec - p->m_wStartSec >= p->m_wDurSec)
				RemoveSFContEffect(i, c);
		}
	}

	if(m_wLastContEffect != m_wLastContEffect_Old)//�ֱٰɸ� ȿ���� �ٲ�ٸ�..
	{
		SendMsg_LastEffectChangeInform();
		m_wLastContEffect_Old = m_wLastContEffect;
	}
}

bool CCharacter::GetStealth() 
{ 
	if(m_bObserver)
		return true;

	if(m_bBreakTranspar)
		return false;

	if(m_EP.GetEff_Plus(_EFF_PLUS::Transparency) > 0 || m_EP.GetEff_State(_EFF_STATE::Stealth))
		return true;

	return false; 
}

void CCharacter::SFContInit()
{
	for(int i = 0; i < SF_CONT_CODE_NUM; i++)
	{
		for(int c = 0; c < CONT_SF_SIZE; c++)
		{
			if(m_SFCont[i][c].m_bExist)
				RemoveSFContEffect(i, c, true);
		}
	}

	m_wLastContEffect = 0xFFFF;
	m_wLastContEffect_Old = 0xFFFF;
}

bool CCharacter::AssistForce(CCharacter* pDstChar, _force_fld* pForceFld, int nForceLv, BYTE* OUT pbyErrorCode, bool* OUT pbUpMty)
{	//return true �̸� fp����.. *pbUpMty true�� ���ô���ġ����
	*pbyErrorCode = 0;
	*pbUpMty = false;

	//�Ͻ�ó��..
	if(pForceFld->m_nTempEffectType != -1)
	{
		SFTempEffectFunc pFunc = g_TempEffectFunc[pForceFld->m_nTempEffectType];
		*pbUpMty = (*pFunc)(this, pDstChar, pForceFld->m_fTempValue[nForceLv-1]);
	}

	//����ó��..
	if(pForceFld->m_nContEffectType != -1)
	{
		if(m_EP.GetEff_State(_EFF_STATE::FC_CtDm_Lck)) //## PARAMETER EDIT (�����������غ�������)
		{
			if(pForceFld->m_nContEffectType == sf_cont_code_damage)
			{
				*pbyErrorCode = error_sf_act_limit;
				return false;
			}
		}

		if(m_EP.GetEff_State(_EFF_STATE::FC_CtHp_Lck))	//## PARAMETER EDIT (���������̵溸������)
		{
			if(pForceFld->m_nContEffectType == sf_cont_code_help)
			{
				*pbyErrorCode = error_sf_act_limit;
				return false;
			}
		}		
		//���ӱ���� ť�� �߰�
		bool bMup = false;
		*pbyErrorCode = pDstChar->InsertSFContEffect(pForceFld->m_nContEffectType, effect_code_force, pForceFld->m_dwIndex, 
							 pForceFld->m_nContEffectSec[nForceLv-1], nForceLv, &bMup);

		if(!*pbUpMty && *pbyErrorCode == 0 && bMup)
			*pbUpMty = true;
	}

	if(*pbyErrorCode == 0)
		return true;

	if(*pbyErrorCode == error_sf_existing_lv_better)	
		return true;

	return false;
}

bool CCharacter::AssistSkill(CCharacter* pDstChar, int nEffectCode, _skill_fld* pSkillFld, int nSkillLv, BYTE* OUT pbyErrorCode, bool* OUT pbUpMty)
{	//return true �̸� fp����.. *pbUpMty true�� ���ô���ġ����
	*pbyErrorCode = 0;
	*pbUpMty = false;

	//�Ͻ�ó��..
	if(pSkillFld->m_nTempEffectType != -1)
	{
		SFTempEffectFunc pFunc = g_TempEffectFunc[pSkillFld->m_nTempEffectType];
		*pbUpMty = (*pFunc)(this, pDstChar, pSkillFld->m_fTempValue[nSkillLv-1]);
	}

	//����ó��..
	if(pSkillFld->m_nContEffectType != -1)
	{
		if(m_EP.GetEff_State(_EFF_STATE::SK_CtHp_Lck)) //## PARAMETER EDIT (�����̵溸�� ��ų���)
		{
			if(pSkillFld->m_nContEffectType == sf_cont_code_help)
			{
				*pbyErrorCode = error_sf_act_limit;
				return false;
			}
		}
		
		//���ӱ���� ť�� �߰�
		bool bMup = false;
		*pbyErrorCode = pDstChar->InsertSFContEffect(pSkillFld->m_nContEffectType, nEffectCode, pSkillFld->m_dwIndex, 
							 pSkillFld->m_nContEffectSec[nSkillLv-1], nSkillLv, &bMup);

		if(!*pbUpMty && *pbyErrorCode == 0 && bMup)
			*pbUpMty = true;
	}

	if(*pbyErrorCode == 0)
		return true;

	if(*pbyErrorCode == error_sf_existing_lv_better)	
		return true;

	return false;
}

void CCharacter::SendMsg_StunInform()
{
	_stun_inform_zocl Send;

	Send.idStun.byID = m_ObjID.m_byID;
	Send.idStun.dwSerial = m_dwObjSerial;
	Send.idStun.wIndex = m_ObjID.m_wIndex;

	BYTE byType[msg_header_num] = {effect_msg, stun_inform_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

void CCharacter::SendMsg_LastEffectChangeInform()
{
	_last_effect_change_inform_zocl Send;

	Send.byObjID = (BYTE)m_ObjID.m_byID;	
	Send.dwSerial = m_dwObjSerial;
	Send.wLastContEffect = m_wLastContEffect;

	BYTE byType[msg_header_num] = {effect_msg, last_effect_change_inform_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

void CCharacter::SendMsg_AddEffect(WORD wEffectCode, BYTE byLv)
{
	if(m_ObjID.m_byID == obj_id_player)
	{
		_effect_add_inform_zocl Send;

		Send.wEffectCode = wEffectCode;
		Send.byLv = byLv;	

		BYTE byType[msg_header_num] = {effect_msg, effect_add_inform_zocl};
		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));

		((CPlayer*)this)->SendData_PartyMemberEffect(0, wEffectCode, byLv);	//��Ƽ���鿡�Ե�.. 0�̸� �߰�
	}
}

void CCharacter::SendMsg_DelEffect(BYTE byEffectCode, WORD wEffectIndex, BYTE byLv)
{
	if(m_ObjID.m_byID == obj_id_player)
	{
		WORD wEffectBit = CalcEffectBit(byEffectCode, wEffectIndex);

		_effect_remove_inform_zocl Send;

		Send.wEffectCode = wEffectBit;
		
		BYTE byType[msg_header_num] = {effect_msg, effect_remove_inform_zocl};
		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));

		((CPlayer*)this)->SendData_PartyMemberEffect(1, wEffectBit, byLv);	//��Ƽ���鿡�Ե�.. 1�̸� ����
	}
}