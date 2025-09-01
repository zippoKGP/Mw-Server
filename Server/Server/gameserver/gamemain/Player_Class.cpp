#include "stdafx.h"
#include "Player.h"
#include "MainThread.h"
#include "ClassFld.h"

void CPlayer::pc_SelectClassRequest(WORD wSelClassIndex)
{
	BYTE byErrCode = 0;
	_class_fld* pClassFld = m_Param.m_pClassData;
	_class_fld* pSelClassFld = (_class_fld*)g_Main.m_tblClass.GetRecord(wSelClassIndex);
	BYTE byHistoryRecordNum = 0xFF;
	BYTE byBranchIndex = 0xFF;
	int i;

	//전직가능한 레벨인지..
	if(!m_Param.IsClassChangeableLv())
	{
		byErrCode = 1;
		goto RESULT;
	}

	//선택가능한 클래스인지
	if(!pSelClassFld)
	{
		byErrCode = 2;
		goto RESULT;
	}
	for(i = 0; i < max_branch_class; i++)
	{
		if(!strcmp(pClassFld->m_strCh_Class[i], "-1"))
			break;

		if(!strcmp(pClassFld->m_strCh_Class[i], pSelClassFld->m_strCode))
		{
			byBranchIndex = i;
			break;
		}
	}
	if(byBranchIndex == 0xFF)
	{
		byErrCode = 2;
		goto RESULT;
	}

	//전직횟수 초과 확인
	for(i = 0; i < class_history_num; i++)
	{
		if(!m_Param.m_pClassHistory[i])
		{
			byHistoryRecordNum = i;
			break;
		}
	}
	if(byHistoryRecordNum == 0xFF)
	{
		byErrCode = 3;
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		m_Param.m_pClassHistory[byHistoryRecordNum] = m_Param.m_pClassData;
		m_Param.m_pClassData = pSelClassFld;

		//보상..
		RewardChangeClass(pSelClassFld);

		//월드 업데이트..
		m_pUserDB->Update_Class(pSelClassFld->m_strCode, byHistoryRecordNum, m_Param.m_pClassHistory[byHistoryRecordNum]->m_dwIndex);
	}

	SendMsg_SelectClassResult(byErrCode, wSelClassIndex);
}

void CPlayer::RewardChangeClass(_class_fld* pClassFld)
{
		//무기숙련도
	for(int i = 0; i < WP_CLASS_NUM; i++)
	{
		if(pClassFld->m_nBnsMMastery[i] == 0)	continue;
		Emb_AlterStat(mastery_code_weapon, i, pClassFld->m_nBnsMMastery[i]);
	}

		//종족별 특수 숙련도
	if(pClassFld->m_nBnsSMastery != 0)
		Emb_AlterStat(mastery_code_special, 0, pClassFld->m_nBnsSMastery);

		//방어숙련도
	if(pClassFld->m_nBnsDefMastery != 0)
		Emb_AlterStat(mastery_code_defence, 0, pClassFld->m_nBnsDefMastery);

		//방패 숙련도
	if(pClassFld->m_nBnsPryMastery != 0)
		Emb_AlterStat(mastery_code_shield, 0, pClassFld->m_nBnsPryMastery);

		//제작 숙련도
	for(i = 0; i < MI_CLASS_NUM; i++)
	{
		if(pClassFld->m_nBnsMakeMastery[i] == 0)	continue;
		Emb_AlterStat(mastery_code_make, i, pClassFld->m_nBnsMakeMastery[i]);
	}

		//포스 숙련도
	for(i = 0; i < force_mastery_num; i++)
	{
		if(pClassFld->m_nBnsForceMastery[i] == 0)	continue;
		Emb_AlterStat(mastery_code_force, i, pClassFld->m_nBnsForceMastery[i]);
	}

		//아이템 보너스..	
	for(i = 0; i < max_class_bns_item; i++)
	{
		if(!strcmp(pClassFld->m_strDefaultItem[i], "0"))
			continue;

		__ITEM NewItem;

		int nTableCode = ::GetItemTableCode(pClassFld->m_strDefaultItem[i]);
		if(nTableCode == -1)
		{
			g_Main.m_logSystemError.Write("전직 보너스 아이템 에러.., class: %s, 테이블코드에러", pClassFld->m_strCode, pClassFld->m_strDefaultItem[i]);		
			continue;
		}

		if(nTableCode == tbl_code_key)//유닛키는 줄수없다..
		{
			g_Main.m_logSystemError.Write("전직 보너스 아이템 에러.., class: %s, 유닛키는줄수없다", pClassFld->m_strCode, pClassFld->m_strDefaultItem[i]);		
			continue;
		}

		_base_fld* pFld = g_Main.m_tblItemData[nTableCode].GetRecord(pClassFld->m_strDefaultItem[i]);
		if(!pFld)
		{
			g_Main.m_logSystemError.Write("전직 보너스 아이템 에러.., class: %s, 테이블에 없음", pClassFld->m_strCode, pClassFld->m_strDefaultItem[i]);		
			continue;
		}

		DWORD dwAmount = ::GetItemDurPoint(nTableCode, pFld->m_dwIndex);
		DWORD dwUpd = 0;	
		//아이템 타입에 따라 업그레이드 맴버 분기..
		BYTE byItemKindCode = ::GetItemKindCode(nTableCode);
		if(byItemKindCode == item_kind_std)//일반..
			dwUpd = __DF_UPT;
		else if(byItemKindCode == item_kind_ani)//애니머스..
			dwUpd = ::GetMaxParamFromExp(pFld->m_dwIndex, dwAmount);
		else
			continue;//다른건 줄수없다..

		NewItem.m_byTableCode = nTableCode;
		NewItem.m_wItemIndex = pFld->m_dwIndex;
		NewItem.m_dwDur = dwAmount;
		NewItem.m_dwLv = dwUpd;

		if(m_Param.m_dbInven.GetIndexEmptyCon() != __NSI)
		{//빈릇롯이 있으면 인벤에 삽입	
			NewItem.m_wSerial = m_Param.GetNewItemSerial();

			Emb_AddStorage(_STORAGE_POS::INVEN, &NewItem);
			SendMsg_RewardAddItem(&NewItem);

			//ITEM HISTORY..
			s_MgrItemHistory.reward_add_item("전직", &NewItem, m_szItemHistoryFileName);
		}
		else	//인벤공간이 모자르면 떨어뜨려주기..
		{//없으면 땅에 떨구기
			::CreateItemBox(&NewItem, this, NULL, itembox_create_code_reward, m_pCurMap, m_wMapLayerIndex, m_fCurPos, false);
		}
	}

	//LV HISTORY..	
	s_MgrLvHistory.update_mastery(m_Param.GetLevel(), m_Param.GetExp(), &m_pmMst.m_BaseCum, m_Param.m_bAlterMastery, m_szLvHistoryFileName);
}
