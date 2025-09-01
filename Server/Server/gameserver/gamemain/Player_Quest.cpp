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
	{//자동선택..
		_Quest_fld* pQuestFld = m_QuestMgr.GetQuestFromEvent(0xFF);
		if(!pQuestFld)
			return;
		//퀘스트추가..
		//빈 슬롯이 있는지..
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
		{//추가..
			_QUEST_DB_BASE::_LIST* pEmptyList = &m_Param.m_QuestDB.m_List[nEmptyNode];

			pEmptyList->byQuestType = pCreateEvent->m_pEvent->m_nQTType;
			pEmptyList->wIndex = pQuestFld->m_dwIndex;
			if(pQuestFld->m_nLimMin == -1)
				pEmptyList->wLeftMin = 0xFFFF;//수행제한시간따로없다..
			else
				pEmptyList->wLeftMin = pQuestFld->m_nLimMin;

			//요구액쑌 횟수세팅..
			for(int a = 0; a < max_quest_act_num; a++)
			{
				if(pQuestFld->m_ActionNode[a].m_nActType != -1)
					pEmptyList->wNum[a] = 0;
			}

			//제한시간세팅..
			if(pQuestFld->m_nLimMin != -1)
				pEmptyList->wLeftMin = pQuestFld->m_nLimMin;

			//quest mgr에도 알린다..
			m_QuestMgr.AddQuestType(pCreateEvent->m_pEvent->m_nQTType);

			//월드에 갱신..	
			m_pUserDB->Update_QuestInsert(nEmptyNode, pEmptyList);

			//추가됌을알림..
			SendMsg_InsertNewQuest(nEmptyNode, pCreateEvent->m_nEventIndex, pCreateEvent->m_nEventNodeIndex, pQuestFld->m_dwIndex);
		}
	}
	else
	{//선택문을 날린다.. 답변이 오기
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

	//퀘스트추가..
	//빈 슬롯이 있는지..
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
	{//추가..
		_QUEST_DB_BASE::_LIST* pEmptyList = &m_Param.m_QuestDB.m_List[nEmptyNode];

		pEmptyList->byQuestType = pCreateEvent->m_pEvent->m_nQTType;
		pEmptyList->wIndex = pQuestFld->m_dwIndex;
		if(pQuestFld->m_nLimMin == -1)
			pEmptyList->wLeftMin = 0xFFFF;//수행제한시간따로없다..
		else
			pEmptyList->wLeftMin = pQuestFld->m_nLimMin;

		//제한시간세팅..
		if(pQuestFld->m_nLimMin != -1)
			pEmptyList->wLeftMin = pQuestFld->m_nLimMin;

		//quest mgr에도 알린다..
		m_QuestMgr.AddQuestType(pCreateEvent->m_pEvent->m_nQTType);

		//quest mgr에도 알린다..
		m_QuestMgr.AddQuestType(pCreateEvent->m_pEvent->m_nQTType);

		//월드에 갱신..	
		m_pUserDB->Update_QuestInsert(nEmptyNode, pEmptyList);

		//추가됌을알림..
		SendMsg_InsertNewQuest(nEmptyNode, pCreateEvent->m_nEventIndex, pCreateEvent->m_nEventNodeIndex, pQuestFld->m_dwIndex);
	}
	else
	{
		//추가실패..
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

		//완수햇는지..
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
		{//보상
			_Quest_fld* pQuestFld = (_Quest_fld*)CQuestMgr::s_tblQuest->GetRecord(pActQuest->wIndex);

			//경험치보상
			if(pQuestFld->m_nConsExp > 0)
				AlterExp(pQuestFld->m_nConsExp);

			//달란트, 골드보상
			if(pQuestFld->m_nConsDalant > 0 || pQuestFld->m_nConsGold > 0)
			{
				AlterMoney(pQuestFld->m_nConsGold, pQuestFld->m_nConsDalant);
				SendMsg_ExchangeMoneyResult(0);	//보내기..
			}

			//아이템보상
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
					s_MgrItemHistory.reward_add_item("퀘스트", &NewItem, m_szItemHistoryFileName);
				}
				else	//인벤공간이 모자르면 떨어뜨려주기..
				{
					::CreateItemBox(&NewItem, this, NULL, itembox_create_code_reward, 
									m_pCurMap, m_wMapLayerIndex, m_fCurPos, false);
				}
			}

			//마스터리보상
			for(i = 0; i < reward_mastery_num; i++)
			{
				_quest_reward_mastery* p = &pQuestFld->m_RewardMastery[i];
				if(p->m_nConsMasteryID == -1)
					break;

				Emb_AlterStat(p->m_nConsMasteryID, p->m_nConsMasterySubID, p->m_nConsMasteryCnt);
			}

			//스킬보상
			if(strcmp(pQuestFld->m_strConsSkillCode, "-1"))
			{
				_base_fld* p = g_Main.m_tblEffectData[effect_code_skill].GetRecord(pQuestFld->m_strConsSkillCode);
				if(p)
					Emb_AlterStat(mastery_code_skill, p->m_dwIndex, pQuestFld->m_nConsSkillCnt);
			}

			//포스보상
			if(strcmp(pQuestFld->m_strConsForceCode, "-1"))
			{
				_base_fld* p = g_Main.m_tblEffectData[effect_code_force].GetRecord(pQuestFld->m_strConsSkillCode);
				if(p)
				{
					//포스리버가 포스창에 있는지 확인..
					for(int f = 0; f < force_storage_num; f++)
					{
						__ITEM* pItem = &m_Param.m_dbForce.m_pStorageList[i];
						if(!pItem->m_bLoad)
							continue;
						if(pItem->m_wItemIndex == p->m_dwIndex)
						{
							DWORD dwNewStat = Emb_AlterDurPoint(_STORAGE_POS::FORCE, f, pQuestFld->m_nConsForceCnt, false);//누적
							SendMsg_FcitemInform(pItem->m_wSerial, dwNewStat);		//보내기..	
							break;
						}
					}				
				}
			}

			//완수메시지..
			SendMsg_QuestComplete(pRet->byQuestDBSlot);

			//mgr보고..
			m_QuestMgr.DelQuestType(pActQuest->byQuestType);

			//월드에갱신
			m_pUserDB->Update_QuestDelete(pRet->byQuestDBSlot);

			//초기화..
			pActQuest->Init();

			//LV HISTORY..	
			s_MgrLvHistory.update_mastery(m_Param.GetLevel(), m_Param.GetExp(), &m_pmMst.m_BaseCum, m_Param.m_bAlterMastery, m_szLvHistoryFileName);

			//다음단계 퀘스트 연결은 다음기회에.....
		}
		else
		{//액션카운트 증가..
			SendMsg_QuestProcess(pRet->byQuestDBSlot, pRet->byActIndex, pRet->wCount);

			//월드에갱신
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

		//실패메시지..
		SendMsg_QuestFailure(quest_fail_code_lv, pRet->byQuestDBSlot);

		//mgr보고..
		m_QuestMgr.DelQuestType(pActQuest->byQuestType);

		//월드에갱신
		m_pUserDB->Update_QuestDelete(pRet->byQuestDBSlot);

		//초기화..
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
			//실패메시지..
			SendMsg_QuestFailure(quest_fail_code_timeout, i);

			//mgr보고..
			m_QuestMgr.DelQuestType(pActQuest->byQuestType);

			//월드에갱신
			m_pUserDB->Update_QuestDelete(i);

			//초기화..
			pActQuest->Init();
		}
	}
}
