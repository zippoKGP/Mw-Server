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

	//���������� ��������..
	if(!m_Param.IsClassChangeableLv())
	{
		byErrCode = 1;
		goto RESULT;
	}

	//���ð����� Ŭ��������
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

	//����Ƚ�� �ʰ� Ȯ��
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

		//����..
		RewardChangeClass(pSelClassFld);

		//���� ������Ʈ..
		m_pUserDB->Update_Class(pSelClassFld->m_strCode, byHistoryRecordNum, m_Param.m_pClassHistory[byHistoryRecordNum]->m_dwIndex);
	}

	SendMsg_SelectClassResult(byErrCode, wSelClassIndex);
}

void CPlayer::RewardChangeClass(_class_fld* pClassFld)
{
		//������õ�
	for(int i = 0; i < WP_CLASS_NUM; i++)
	{
		if(pClassFld->m_nBnsMMastery[i] == 0)	continue;
		Emb_AlterStat(mastery_code_weapon, i, pClassFld->m_nBnsMMastery[i]);
	}

		//������ Ư�� ���õ�
	if(pClassFld->m_nBnsSMastery != 0)
		Emb_AlterStat(mastery_code_special, 0, pClassFld->m_nBnsSMastery);

		//�����õ�
	if(pClassFld->m_nBnsDefMastery != 0)
		Emb_AlterStat(mastery_code_defence, 0, pClassFld->m_nBnsDefMastery);

		//���� ���õ�
	if(pClassFld->m_nBnsPryMastery != 0)
		Emb_AlterStat(mastery_code_shield, 0, pClassFld->m_nBnsPryMastery);

		//���� ���õ�
	for(i = 0; i < MI_CLASS_NUM; i++)
	{
		if(pClassFld->m_nBnsMakeMastery[i] == 0)	continue;
		Emb_AlterStat(mastery_code_make, i, pClassFld->m_nBnsMakeMastery[i]);
	}

		//���� ���õ�
	for(i = 0; i < force_mastery_num; i++)
	{
		if(pClassFld->m_nBnsForceMastery[i] == 0)	continue;
		Emb_AlterStat(mastery_code_force, i, pClassFld->m_nBnsForceMastery[i]);
	}

		//������ ���ʽ�..	
	for(i = 0; i < max_class_bns_item; i++)
	{
		if(!strcmp(pClassFld->m_strDefaultItem[i], "0"))
			continue;

		__ITEM NewItem;

		int nTableCode = ::GetItemTableCode(pClassFld->m_strDefaultItem[i]);
		if(nTableCode == -1)
		{
			g_Main.m_logSystemError.Write("���� ���ʽ� ������ ����.., class: %s, ���̺��ڵ忡��", pClassFld->m_strCode, pClassFld->m_strDefaultItem[i]);		
			continue;
		}

		if(nTableCode == tbl_code_key)//����Ű�� �ټ�����..
		{
			g_Main.m_logSystemError.Write("���� ���ʽ� ������ ����.., class: %s, ����Ű���ټ�����", pClassFld->m_strCode, pClassFld->m_strDefaultItem[i]);		
			continue;
		}

		_base_fld* pFld = g_Main.m_tblItemData[nTableCode].GetRecord(pClassFld->m_strDefaultItem[i]);
		if(!pFld)
		{
			g_Main.m_logSystemError.Write("���� ���ʽ� ������ ����.., class: %s, ���̺� ����", pClassFld->m_strCode, pClassFld->m_strDefaultItem[i]);		
			continue;
		}

		DWORD dwAmount = ::GetItemDurPoint(nTableCode, pFld->m_dwIndex);
		DWORD dwUpd = 0;	
		//������ Ÿ�Կ� ���� ���׷��̵� �ɹ� �б�..
		BYTE byItemKindCode = ::GetItemKindCode(nTableCode);
		if(byItemKindCode == item_kind_std)//�Ϲ�..
			dwUpd = __DF_UPT;
		else if(byItemKindCode == item_kind_ani)//�ִϸӽ�..
			dwUpd = ::GetMaxParamFromExp(pFld->m_dwIndex, dwAmount);
		else
			continue;//�ٸ��� �ټ�����..

		NewItem.m_byTableCode = nTableCode;
		NewItem.m_wItemIndex = pFld->m_dwIndex;
		NewItem.m_dwDur = dwAmount;
		NewItem.m_dwLv = dwUpd;

		if(m_Param.m_dbInven.GetIndexEmptyCon() != __NSI)
		{//�󸩷��� ������ �κ��� ����	
			NewItem.m_wSerial = m_Param.GetNewItemSerial();

			Emb_AddStorage(_STORAGE_POS::INVEN, &NewItem);
			SendMsg_RewardAddItem(&NewItem);

			//ITEM HISTORY..
			s_MgrItemHistory.reward_add_item("����", &NewItem, m_szItemHistoryFileName);
		}
		else	//�κ������� ���ڸ��� ����߷��ֱ�..
		{//������ ���� ������
			::CreateItemBox(&NewItem, this, NULL, itembox_create_code_reward, m_pCurMap, m_wMapLayerIndex, m_fCurPos, false);
		}
	}

	//LV HISTORY..	
	s_MgrLvHistory.update_mastery(m_Param.GetLevel(), m_Param.GetExp(), &m_pmMst.m_BaseCum, m_Param.m_bAlterMastery, m_szLvHistoryFileName);
}
