#include "stdafx.h"
#include "Player.h"
#include "MainThread.h"
#include "ErrorEventFromServer.h"

void CPlayer::pc_AnimusRecallRequest(WORD wAnimusItemSerial, WORD wAnimusClientHP, WORD wAnimusClientFP)
{//�ִϸӽ� ��ȯ..
	BYTE byRetCode = 0;
	__ITEM* pAnimusItem = NULL;
	_animus_fld* pAnimusFld = NULL;
	_animus_param* pAnimusParam = NULL;
	CAnimus* pEmptyAnimus = NULL;
	WORD wAdjustHP, wAdjustFP;
	int i;

	//��ȯ���� ��������..
	if(m_Param.GetRaceSexCode()/2 != race_code_cora)
	{
		byRetCode = error_recall_not_actable_race;
		goto RESULT;
	}

	//��ȯ������ ��������..
	if(GetCurSecNum() == 0xFFFFFFFF || m_bMapLoading)
	{
		byRetCode = error_recall_map_move;
		goto RESULT;
	}

	//��ȯ�����ð� Ȯ��
	if(!m_bFreeRecallWaitTime)
	{
		if(timeGetTime() - m_dwLastRecallTime < 60000)//60�ʰ� ��ȯ����..
		{
			byRetCode = error_recall_delay;
			goto RESULT;
		}
	}

	//���� ��ȯ���� �ִ��� Ȯ��..
	if(m_pRecalledAnimusItem)
	{
		byRetCode = error_recall_already_recalled;
		goto RESULT;
	}

	//�ش� �ø����� ������ ã��..
	for(i = 0; i < animus_storage_num; i++)
	{
		__ITEM* p = &m_Param.m_dbAnimus.m_pStorageList[i];

		if(!p->m_bLoad)
			continue;

		if(p->m_wSerial == wAnimusItemSerial)
		{
			pAnimusItem = p;
			break;
		}
	}
	if(!pAnimusItem)
	{
		byRetCode = error_recall_nothing_item;
		goto RESULT;
	}

	//��ȯ�����Ҹ�ƴ ȸ���� �� ��������..	
	pAnimusFld = ::GetAnimusFldFromExp(pAnimusItem->m_wItemIndex, pAnimusItem->m_dwDur);
	if(pAnimusFld)
	{
		pAnimusParam = (_animus_param*)&pAnimusItem->m_dwLv;
		wAdjustHP = pAnimusParam->wHP;
		wAdjustFP = pAnimusParam->wFP;

		//Ŭ���̾�Ʈ�� �Ķ���Ϳ� ���ؼ� ū����(+-4)�������� �װ����� �������ֱ�..
		if(abs(pAnimusParam->wHP - wAnimusClientHP) <= 4)
			wAdjustHP = wAnimusClientHP;
		if(abs(pAnimusParam->wFP - wAnimusClientFP) <= 4)
			wAdjustFP = wAnimusClientFP;

		if((float)wAdjustHP / pAnimusFld->m_nMaxHP < 0.3f)
		{
			byRetCode = error_recall_hp_under_30;
			goto RESULT;
		}		
	}
	else
		return;//������ ���� ����..

	//��ȯ������ �����͸�����..
	if(pAnimusFld->m_nLevel > sqrt((float)m_pmMst.GetCumPerMast(mastery_code_special, 0)*20)+12)
	{
		byRetCode = error_recal_few_mastery;
		goto RESULT;
	}

	//FP�� �������..
	if(GetFP() < 60)//�ִϸӽ� ��ȯ������ 60�� �Ҹ�..
	{
		byRetCode = error_recall_fp_lack;
		goto RESULT;
	}

	pEmptyAnimus = ::FindEmptyAnimus(g_Animus, MAX_ANIMUS);
	if(!pEmptyAnimus)
	{
		byRetCode = error_recall_max_animus;
		goto RESULT;
	}


RESULT:

	if(byRetCode == 0)
	{
		static int nConsumeFPPoint = 60;

		//FP���..
		SetFP(GetFP()-nConsumeFPPoint, true);

		pAnimusParam->wHP = wAdjustHP;
		pAnimusParam->wFP = wAdjustFP;

		//��ȯ�� �����ϰ�..
		_animus_create_setdata Data;

		Data.m_pMap = m_pCurMap;
		Data.m_nLayerIndex = m_wMapLayerIndex;	
		Data.m_pRecordSet = g_Main.m_tblAnimus.GetRecord(pAnimusItem->m_wItemIndex);
		memcpy(Data.m_fStartPos, m_fCurPos, sizeof(float)*3);
		Data.nHP = pAnimusParam->wHP;
		Data.nFP = pAnimusParam->wFP;
		Data.dwExp = pAnimusItem->m_dwDur;
		Data.pMaster = this;

		pEmptyAnimus->Create(&Data);

		m_pRecalledAnimusItem = pAnimusItem;
		m_pRecalledAnimusChar = pEmptyAnimus;
	}

	SendMsg_AnimusRecallResult(byRetCode, GetFP(), pEmptyAnimus);
}

void CPlayer::pc_AnimusReturnRequest()
{
	BYTE byRetCode = 0;

	//��ȯ�� �ִϸӽ��� �ִ���..
	if(!m_pRecalledAnimusItem || !m_pRecalledAnimusChar)
	{
		byRetCode = error_recall_not_recalled;
		goto RESULT;
	}

RESULT:

	if(byRetCode == 0)
	{
		_AnimusReturn();
	}
	else
	{
		SendMsg_AnimusReturnResult(byRetCode, 0xFFFF);
	}
}

void CPlayer::pc_AnimusCommandRequest(BYTE byCommandCode)
{
	BYTE byRetCode = 0;

	//��ȯ�� �ִϸӽ��� �ִ���..
	if(!m_pRecalledAnimusItem || !m_pRecalledAnimusChar)
	{
		byRetCode = error_recall_not_recalled;
		goto RESULT;
	}

RESULT:

	if(byRetCode == 0)
	{
		//��ȯ�� �ִϸӽ����� �˸���
		m_pRecalledAnimusChar->ChangeMode_MasterCommand(byCommandCode);
	}
}

void CPlayer::pc_AnimusTargetRequest(BYTE byObjectID, WORD wObjectIndex, DWORD dwObjectSerial)
{
	BYTE byRetCode = 0;
	CCharacter* pTargetChar = (CCharacter*)g_Main.GetObject(obj_kind_char, byObjectID, wObjectIndex);

	//��ȯ�� �ִϸӽ��� �ִ���..
	if(!m_pRecalledAnimusItem || !m_pRecalledAnimusChar)
	{
		byRetCode = error_recall_not_recalled;
		goto RESULT;
	}

	//Ÿ���� ����� �°���..
	if(!pTargetChar->m_bLive || pTargetChar->m_bCorpse)
	{
		byRetCode = error_recall_target_invalid;
		goto RESULT;
	}
	if(m_pRecalledAnimusChar->GetObjRace() == pTargetChar->GetObjRace())
	{
		byRetCode = error_recall_target_invalid;
		goto RESULT;
	}
	if(::GetSqrt(m_pRecalledAnimusChar->m_fCurPos, pTargetChar->m_fCurPos) > 400)
	{
		byRetCode = error_recall_target_invalid;
		goto RESULT;
	}

RESULT:

	if(byRetCode == 0)
	{
		//��ȯ�� �ִϸӽ����� �˸���
		m_pRecalledAnimusChar->ChangeTarget_MasterCommand(pTargetChar);
	}

	SendMsg_AnimusTargetResult(byRetCode);
}

void CPlayer::AlterHP_Animus(int nNewHP)
{
	if(m_pRecalledAnimusItem)
	{
		_animus_param* pParam = (_animus_param*)&m_pRecalledAnimusItem->m_dwLv;
		pParam->wHP = (WORD)nNewHP;

		SendMsg_AnimusHPInform();

		//���忡 �˸�..
		if(m_pUserDB)
			m_pUserDB->Update_ItemUpgrade(_STORAGE_POS::ANIMUS, m_pRecalledAnimusItem->m_byStorageIndex, m_pRecalledAnimusItem->m_dwLv, false);
	}
}

void CPlayer::AlterFP_Animus(int nNewFP)
{
	if(m_pRecalledAnimusItem)
	{
		_animus_param* pParam = (_animus_param*)&m_pRecalledAnimusItem->m_dwLv;
		pParam->wFP = (WORD)nNewFP;

		SendMsg_AnimusFPInform();

		//���忡 �˸�..
		if(m_pUserDB)
		{
			m_pUserDB->Update_ItemUpgrade(_STORAGE_POS::ANIMUS, m_pRecalledAnimusItem->m_byStorageIndex, m_pRecalledAnimusItem->m_dwLv, false);
		}
	}
}

void CPlayer::AlterExp_Animus(int nAlterExp)
{
	if(m_pRecalledAnimusItem)
	{
		if(nAlterExp > 0)
		{
			m_pRecalledAnimusItem->m_dwDur += nAlterExp;

			//����Ưȭ��ų�� ���ݻ��..
			Emb_AlterStat(mastery_code_special, 0, nAlterExp);
		}
		else
		{	//���� ����ġ���� ���ϰ��� ���..
			if(m_pRecalledAnimusItem->m_dwDur >= nAlterExp)
				m_pRecalledAnimusItem->m_dwDur += nAlterExp;
			else
				m_pRecalledAnimusItem->m_dwDur = 0;
		}

		SendMsg_AnimusExpInform();

		//���忡 �˸�..
		if(m_pUserDB)
		{
			m_pUserDB->Update_ItemDur(_STORAGE_POS::ANIMUS, m_pRecalledAnimusItem->m_byStorageIndex, m_pRecalledAnimusItem->m_dwDur, false);
		}
	}
}

void CPlayer::AlterMode_Animus(BYTE byMode)
{
	if(m_pRecalledAnimusItem)
	{
		SendMsg_AnimusModeInform(byMode);
	}
}

void CPlayer::Return_AnimusAsk()
{	//��ȯ�� �ִϸӽ��� �ִ���..
	if(!m_pRecalledAnimusItem || !m_pRecalledAnimusChar)
		return;

	m_bNextRecallReturn = true;
}

void CPlayer::_AnimusReturn()
{	//��ȯ���� �ִٸ� ��ȯ��Ų��..
	if(m_pRecalledAnimusChar)
	{
		m_pRecalledAnimusChar->Destroy();
		SendMsg_AnimusReturnResult(0, m_pRecalledAnimusItem->m_wSerial);

		m_pRecalledAnimusItem = NULL;
		m_pRecalledAnimusChar = NULL;
		m_dwLastRecallTime = timeGetTime();

		//Ȥ�� Ÿ�̹� ���̶�� Ÿ�̸ӿ��� �����Ѵ�..
		s_AnimusReturnDelay.Delete(m_ObjID.m_wIndex, m_dwObjSerial);
	}
}



