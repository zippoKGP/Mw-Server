#include "stdafx.h"
#include "Player.h"
#include "MainThread.h"
#include "ErrorEventFromServer.h"

void CPlayer::Emb_CreateQuestEvent(int nEventType, char* pszEventCode)
{
	_event_node_point* pCreateEvent = m_QuestMgr.CheckCreateEvent(nEventType, pszEventCode);
	if(!pCreateEvent)
		return;

	if(!pCreateEvent->m_pEvent->m_bSelectQuestManual)
	{//�ڵ�����..
		_Quest_fld* pQuestFld = m_QuestMgr.GetQuestFromEvent(0xFF);
		if(!pQuestFld)
			return;
		//����Ʈ�߰�..
		//�� ������ �ִ���..
		int nEmptyNode = -1;
		for(int i = 0; i < quest_storage_num; i++)
		{
			if(m_Param.m_QuestDB.m_List[i].byQuestType == 0xFF)
			{
				nEmptyNode = i;
				break;
			}
		}
		if(nEmptyNode != -1)
		{//�߰�..
			_QUEST_DB_BASE::_LIST* pEmptyList = &m_Param.m_QuestDB.m_List[nEmptyNode];

			pEmptyList->byQuestType = pCreateEvent->m_pEvent->m_nQTType;
			pEmptyList->wIndex = pQuestFld->m_dwIndex;
			if(pQuestFld->m_nLimMin == -1)
				pEmptyList->wLeftMin = 0xFFFF;//�������ѽð����ξ���..
			else
				pEmptyList->wLeftMin = pQuestFld->m_nLimMin;

			//�䱸�ל� Ƚ������..
			for(int a = 0; a < max_quest_act_num; a++)
			{
				if(pQuestFld->m_ActionNode[a].m_nActType != -1)
					pEmptyList->wNum[a] = 0;
			}

			//���ѽð�����..
			if(pQuestFld->m_nLimMin != -1)
				pEmptyList->wLeftMin = pQuestFld->m_nLimMin;

			//quest mgr���� �˸���..
			m_QuestMgr.AddQuestType(pCreateEvent->m_pEvent->m_nQTType);

			//���忡 ����..	
			m_pUserDB->Update_QuestInsert(nEmptyNode, pEmptyList);

			//�߰������˸�..
			SendMsg_InsertNewQuest(nEmptyNode, pCreateEvent->m_nEventIndex, pCreateEvent->m_nEventNodeIndex, pQuestFld->m_dwIndex);
		}
	}
	else
	{//���ù��� ������.. �亯�� ����
		SendMsg_SelectWaitedQuest(pCreateEvent->m_nEventIndex, pCreateEvent->m_nEventNodeIndex);
	}
}

void CPlayer::Emb_SelectQuestEvent(BYTE bySelectIndex)
{
	_event_node_point* pCreateEvent = m_QuestMgr.GetLastEvent();
	if(!pCreateEvent)
		return;

	_Quest_fld* pQuestFld = m_QuestMgr.GetQuestFromEvent(bySelectIndex);
	if(!pQuestFld)
		return;

	//����Ʈ�߰�..
	//�� ������ �ִ���..
	int nEmptyNode = -1;
	for(int i = 0; i < quest_storage_num; i++)
	{
		if(m_Param.m_QuestDB.m_List[i].byQuestType == 0xFF)
		{
			nEmptyNode = i;
			break;
		}
	}
	if(nEmptyNode != -1)
	{//�߰�..
		_QUEST_DB_BASE::_LIST* pEmptyList = &m_Param.m_QuestDB.m_List[nEmptyNode];

		pEmptyList->byQuestType = pCreateEvent->m_pEvent->m_nQTType;
		pEmptyList->wIndex = pQuestFld->m_dwIndex;
		if(pQuestFld->m_nLimMin == -1)
			pEmptyList->wLeftMin = 0xFFFF;//�������ѽð����ξ���..
		else
			pEmptyList->wLeftMin = pQuestFld->m_nLimMin;

		//���ѽð�����..
		if(pQuestFld->m_nLimMin != -1)
			pEmptyList->wLeftMin = pQuestFld->m_nLimMin;

		//quest mgr���� �˸���..
		m_QuestMgr.AddQuestType(pCreateEvent->m_pEvent->m_nQTType);

		//quest mgr���� �˸���..
		m_QuestMgr.AddQuestType(pCreateEvent->m_pEvent->m_nQTType);

		//���忡 ����..	
		m_pUserDB->Update_QuestInsert(nEmptyNode, pEmptyList);

		//�߰������˸�..
		SendMsg_InsertNewQuest(nEmptyNode, pCreateEvent->m_nEventIndex, pCreateEvent->m_nEventNodeIndex, pQuestFld->m_dwIndex);
	}
	else
	{
		//�߰�����..
		SendMsg_InsertQuestFailure(pCreateEvent->m_nEventIndex, pCreateEvent->m_nEventNodeIndex);
	}
}

void CPlayer::Emb_CheckActForQuest(int nActCode, char* pszReqCode)
{
	_quest_check_result* pResult = m_QuestMgr.CheckReqAct(nActCode, pszReqCode);
	if(!pResult)
		return;

	for(int q = 0; q < pResult->m_byCheckNum; q++)
	{
		_quest_check_result::_node* pRet = &pResult->m_List[q];
		__QUEST* pActQuest = &m_Param.m_QuestDB.m_List[pRet->byQuestDBSlot];

		pActQuest->wNum[pRet->byActIndex] = pRet->wCount;

		//�ϼ��޴���..
		bool bComplete = true;
		for(int i = 0; i < max_quest_act_num; i++)
		{
			if(pActQuest->wNum[i] != 0xFFFF) 
			{
				bComplete = false;
				break;
			}
		}

		if(bComplete)
		{//����
			_Quest_fld* pQuestFld = (_Quest_fld*)CQuestMgr::s_tblQuest->GetRecord(pActQuest->wIndex);

			//����ġ����
			if(pQuestFld->m_nConsExp > 0)
				AlterExp(pQuestFld->m_nConsExp);

			//�޶�Ʈ, ��庸��
			if(pQuestFld->m_nConsDalant > 0 || pQuestFld->m_nConsGold > 0)
			{
				AlterMoney(pQuestFld->m_nConsGold, pQuestFld->m_nConsDalant);
				SendMsg_ExchangeMoneyResult(0);	//������..
			}

			//�����ۺ���
			for(int i = 0; i < reward_item_num; i++)
			{
				_quest_reward_item* p = &pQuestFld->m_RewardItem[i];
				if(!strcmp(p->m_strConsITCode, "-1"))
					break;

				int nTable = ::GetItemTableCode(p->m_strConsITCode);
				if(nTable == -1)
					continue;

				_base_fld* pFld = g_Main.m_tblItemData[nTable].GetRecord(p->m_strConsITCode);
				if(!pFld)
					continue;

				__ITEM NewItem;

				BYTE byLimSocketNum = ::GetDefItemUpgSocketNum(nTable, pFld->m_dwIndex);
				
				NewItem.m_byTableCode = nTable;
				NewItem.m_wItemIndex = pFld->m_dwIndex;
				if(::IsOverLapItem(nTable))
					NewItem.m_dwDur = p->m_nConsITCnt;
				else
					NewItem.m_dwDur = ::GetItemDurPoint(nTable, pFld->m_dwIndex);
				NewItem.m_dwLv = ::GetBitAfterSetLimSocket(byLimSocketNum);

				//add item to inven..
				if(m_Param.m_dbInven.GetIndexEmptyCon() != __NSI)
				{	
					NewItem.m_wSerial = m_Param.GetNewItemSerial();
					Emb_AddStorage(_STORAGE_POS::INVEN, &NewItem);

					//ITEM HISTORY..
					s_MgrItemHistory.reward_add_item("����Ʈ", &NewItem, m_szItemHistoryFileName);
				}
				else	//�κ������� ���ڸ��� ����߷��ֱ�..
				{
					::CreateItemBox(&NewItem, this, NULL, itembox_create_code_reward, 
									m_pCurMap, m_wMapLayerIndex, m_fCurPos, false);
				}
			}

			//�����͸�����
			for(i = 0; i < reward_mastery_num; i++)
			{
				_quest_reward_mastery* p = &pQuestFld->m_RewardMastery[i];
				if(p->m_nConsMasteryID == -1)
					break;

				Emb_AlterStat(p->m_nConsMasteryID, p->m_nConsMasterySubID, p->m_nConsMasteryCnt);
			}

			//��ų����
			if(strcmp(pQuestFld->m_strConsSkillCode, "-1"))
			{
				_base_fld* p = g_Main.m_tblEffectData[effect_code_skill].GetRecord(pQuestFld->m_strConsSkillCode);
				if(p)
					Emb_AlterStat(mastery_code_skill, p->m_dwIndex, pQuestFld->m_nConsSkillCnt);
			}

			//��������
			if(strcmp(pQuestFld->m_strConsForceCode, "-1"))
			{
				_base_fld* p = g_Main.m_tblEffectData[effect_code_force].GetRecord(pQuestFld->m_strConsSkillCode);
				if(p)
				{
					//���������� ����â�� �ִ��� Ȯ��..
					for(int f = 0; f < force_storage_num; f++)
					{
						__ITEM* pItem = &m_Param.m_dbForce.m_pStorageList[i];
						if(!pItem->m_bLoad)
							continue;
						if(pItem->m_wItemIndex == p->m_dwIndex)
						{
							DWORD dwNewStat = Emb_AlterDurPoint(_STORAGE_POS::FORCE, f, pQuestFld->m_nConsForceCnt, false);//����
							SendMsg_FcitemInform(pItem->m_wSerial, dwNewStat);		//������..	
							break;
						}
					}				
				}
			}

			//�ϼ��޽���..
			SendMsg_QuestComplete(pRet->byQuestDBSlot);

			//mgr����..
			m_QuestMgr.DelQuestType(pActQuest->byQuestType);

			//���忡����
			m_pUserDB->Update_QuestDelete(pRet->byQuestDBSlot);

			//�ʱ�ȭ..
			pActQuest->Init();

			//LV HISTORY..	
			s_MgrLvHistory.update_mastery(m_Param.GetLevel(), m_Param.GetExp(), &m_pmMst.m_BaseCum, m_Param.m_bAlterMastery, m_szLvHistoryFileName);

			//�����ܰ� ����Ʈ ������ ������ȸ��.....
		}
		else
		{//�׼�ī��Ʈ ����..
			SendMsg_QuestProcess(pRet->byQuestDBSlot, pRet->byActIndex, pRet->wCount);

			//���忡����
			m_pUserDB->Update_QuestUpdate(pRet->byQuestDBSlot, pActQuest);
		}
	}
}

void CPlayer::Emb_CheckLvUpForQuest()
{
	_quest_fail_result* pResult = m_QuestMgr.CheckLimLv(GetLevel());
	if(!pResult)
		return;

	for(int q = 0; q < pResult->m_byCheckNum; q++)
	{
		_quest_fail_result::_node* pRet = &pResult->m_List[q];
		__QUEST* pActQuest = &m_Param.m_QuestDB.m_List[pRet->byQuestDBSlot];

		//���и޽���..
		SendMsg_QuestFailure(quest_fail_code_lv, pRet->byQuestDBSlot);

		//mgr����..
		m_QuestMgr.DelQuestType(pActQuest->byQuestType);

		//���忡����
		m_pUserDB->Update_QuestDelete(pRet->byQuestDBSlot);

		//�ʱ�ȭ..
		pActQuest->Init();
	}
}

void CPlayer::Emb_CheckTimeoutForQuest()
{
	for(int i = 0; i < quest_storage_num; i++)
	{
		__QUEST* pActQuest = &m_Param.m_QuestDB.m_List[i];
		if(pActQuest->wIndex == 0xFFFF)
			continue;

		if(pActQuest->wLeftMin == 0xFFFF)
			continue;

		if(--pActQuest->wLeftMin == 0)
		{
			//���и޽���..
			SendMsg_QuestFailure(quest_fail_code_timeout, i);

			//mgr����..
			m_QuestMgr.DelQuestType(pActQuest->byQuestType);

			//���忡����
			m_pUserDB->Update_QuestDelete(i);

			//�ʱ�ȭ..
			pActQuest->Init();
		}
	}
}
