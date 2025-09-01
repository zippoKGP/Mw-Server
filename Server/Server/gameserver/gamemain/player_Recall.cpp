#include "stdafx.h"
#include "Player.h"
#include "MainThread.h"
#include "ErrorEventFromServer.h"

void CPlayer::pc_AnimusRecallRequest(WORD wAnimusItemSerial, WORD wAnimusClientHP, WORD wAnimusClientFP)
{//애니머스 소환..
	BYTE byRetCode = 0;
	__ITEM* pAnimusItem = NULL;
	_animus_fld* pAnimusFld = NULL;
	_animus_param* pAnimusParam = NULL;
	CAnimus* pEmptyAnimus = NULL;
	WORD wAdjustHP, wAdjustFP;
	int i;

	//소환가능 종족인지..
	if(m_Param.GetRaceSexCode()/2 != race_code_cora)
	{
		byRetCode = error_recall_not_actable_race;
		goto RESULT;
	}

	//소환가능한 상태인지..
	if(GetCurSecNum() == 0xFFFFFFFF || m_bMapLoading)
	{
		byRetCode = error_recall_map_move;
		goto RESULT;
	}

	//소환지연시간 확인
	if(!m_bFreeRecallWaitTime)
	{
		if(timeGetTime() - m_dwLastRecallTime < 60000)//60초간 소환못함..
		{
			byRetCode = error_recall_delay;
			goto RESULT;
		}
	}

	//현재 소환물이 있는지 확인..
	if(m_pRecalledAnimusItem)
	{
		byRetCode = error_recall_already_recalled;
		goto RESULT;
	}

	//해당 시리얼의 아이템 찾기..
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

	//소환가능할만틈 회복이 된 상태인지..	
	pAnimusFld = ::GetAnimusFldFromExp(pAnimusItem->m_wItemIndex, pAnimusItem->m_dwDur);
	if(pAnimusFld)
	{
		pAnimusParam = (_animus_param*)&pAnimusItem->m_dwLv;
		wAdjustHP = pAnimusParam->wHP;
		wAdjustFP = pAnimusParam->wFP;

		//클라이언트의 파라메터와 비교해서 큰차이(+-4)가없으면 그것으로 인정해주기..
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
		return;//있을수 없는 에러..

	//소환가능한 마스터리인지..
	if(pAnimusFld->m_nLevel > sqrt((float)m_pmMst.GetCumPerMast(mastery_code_special, 0)*20)+12)
	{
		byRetCode = error_recal_few_mastery;
		goto RESULT;
	}

	//FP는 충분한지..
	if(GetFP() < 60)//애니머스 소환때마다 60씩 소모..
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

		//FP깍고..
		SetFP(GetFP()-nConsumeFPPoint, true);

		pAnimusParam->wHP = wAdjustHP;
		pAnimusParam->wFP = wAdjustFP;

		//소환물 생성하고..
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

	//소환한 애니머스가 있는지..
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

	//소환한 애니머스가 있는지..
	if(!m_pRecalledAnimusItem || !m_pRecalledAnimusChar)
	{
		byRetCode = error_recall_not_recalled;
		goto RESULT;
	}

RESULT:

	if(byRetCode == 0)
	{
		//소환된 애니머스에게 알리기
		m_pRecalledAnimusChar->ChangeMode_MasterCommand(byCommandCode);
	}
}

void CPlayer::pc_AnimusTargetRequest(BYTE byObjectID, WORD wObjectIndex, DWORD dwObjectSerial)
{
	BYTE byRetCode = 0;
	CCharacter* pTargetChar = (CCharacter*)g_Main.GetObject(obj_kind_char, byObjectID, wObjectIndex);

	//소환한 애니머스가 있는지..
	if(!m_pRecalledAnimusItem || !m_pRecalledAnimusChar)
	{
		byRetCode = error_recall_not_recalled;
		goto RESULT;
	}

	//타겟이 제대로 됀건지..
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
		//소환된 애니머스에게 알리기
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

		//월드에 알림..
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

		//월드에 알림..
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

			//종족특화스킬도 동반상승..
			Emb_AlterStat(mastery_code_special, 0, nAlterExp);
		}
		else
		{	//현재 경험치보다 깍일것을 대비..
			if(m_pRecalledAnimusItem->m_dwDur >= nAlterExp)
				m_pRecalledAnimusItem->m_dwDur += nAlterExp;
			else
				m_pRecalledAnimusItem->m_dwDur = 0;
		}

		SendMsg_AnimusExpInform();

		//월드에 알림..
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
{	//소환한 애니머스가 있는지..
	if(!m_pRecalledAnimusItem || !m_pRecalledAnimusChar)
		return;

	m_bNextRecallReturn = true;
}

void CPlayer::_AnimusReturn()
{	//소환물이 있다면 귀환시킨다..
	if(m_pRecalledAnimusChar)
	{
		m_pRecalledAnimusChar->Destroy();
		SendMsg_AnimusReturnResult(0, m_pRecalledAnimusItem->m_wSerial);

		m_pRecalledAnimusItem = NULL;
		m_pRecalledAnimusChar = NULL;
		m_dwLastRecallTime = timeGetTime();

		//혹시 타이밍 중이라면 타이머에서 삭제한다..
		s_AnimusReturnDelay.Delete(m_ObjID.m_wIndex, m_dwObjSerial);
	}
}



