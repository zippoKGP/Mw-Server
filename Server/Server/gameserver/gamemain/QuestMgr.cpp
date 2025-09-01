#include "stdafx.h"
#include "QuestMgr.h"
#include "Player.h"
#include "MainThread.h"

CRecordData* CQuestMgr::s_tblQuest = NULL;
CRecordData* CQuestMgr::s_tblQuestEvent = NULL;
_quest_check_result CQuestMgr::s_QuestCKRet;
_quest_fail_result  CQuestMgr::s_QuestFTRet;

CQuestMgr::CQuestMgr()
{
	m_pMaster = NULL;
	m_pQuestData = NULL;

	if(!s_tblQuestEvent)
	{
		s_tblQuestEvent = new CRecordData [QUEST_EVENT_CODE_NUM];

		char* pszQuestEventFileName[QUEST_EVENT_CODE_NUM] = {	".\\script\\QuestDummyEvent.dat", 
															".\\script\\QuestNPCEvent.dat", 
															".\\script\\QuestKillOtherRaceEvent.dat", 
															".\\script\\QuestLvUpEvent.dat", 
															".\\script\\QuestPromoteEvent.dat", 
															".\\script\\QuestGradeEvent.dat", 
															".\\script\\QuestGainItemEvent.dat"	}; 

		for(int i = 0; i < QUEST_EVENT_CODE_NUM; i++)
		{
			if(!s_tblQuestEvent[i].ReadRecord(pszQuestEventFileName[i], sizeof(_QuestEvent_fld)))	
				exit(0);
		}
	}
	if(!s_tblQuest)
	{
		s_tblQuest = new CRecordData;
		if(!s_tblQuest->ReadRecord(".\\script\\Quest.dat", sizeof(_Quest_fld)))	
			exit(0);
	}
}

CQuestMgr::~CQuestMgr()
{
	if(s_tblQuestEvent)
	{
		delete [] s_tblQuestEvent;
		s_tblQuestEvent = NULL;
	}
	if(s_tblQuest)
	{
		delete s_tblQuest;
		s_tblQuest = NULL;
	}
}

void CQuestMgr::InitMgr(CPlayer* pMaster, _QUEST_DB_BASE* pQuestData)
{
	m_pMaster = pMaster;
	m_pQuestData = pQuestData;
	m_dwOldTime = timeGetTime();
	m_LastSelectEvent.init();
	memset(m_byCurQuestNum, 0, sizeof(m_byCurQuestNum));

	for(int i = 0; i < quest_storage_num; i++)
	{
		__QUEST* pQuest = &pQuestData->m_List[i];
		if(pQuest->byQuestType == 0xFF)
			continue;

		if(pQuest->byQuestType >= QUEST_TYPE_NUM)
			continue;

		m_byCurQuestNum[pQuest->byQuestType]++;
	}
}

bool CQuestMgr::AddQuestType(int nType)
{
	if(m_byCurQuestNum[nType] >= max_quest_per_type)
		return false;

	m_byCurQuestNum[nType]++;
	return true;
}

bool CQuestMgr::DelQuestType(int nType)
{
	if(m_byCurQuestNum[nType] == 0)
		return false;

	m_byCurQuestNum[nType]--;
	return true;
}

_event_node_point* CQuestMgr::CheckCreateEvent(int nEventType, char* pszEventCode)
{
	_QuestEvent_fld* pEvent = (_QuestEvent_fld*)s_tblQuestEvent[nEventType].GetRecord(pszEventCode);
	if(!pEvent)
		return NULL;

	for(int n = 0; n < quest_node_per_event; n++)
	{
		_event_node* pN = &pEvent->m_Node[n];

		if(!pN->m_bActiveType)
			break;

		if(m_byCurQuestNum[pN->m_nQTType] >= max_quest_per_type)	//퀘스트분류별 제한수가 꽉찼다..
			continue;

		//수락조건
		bool bPass = true;
		for(int c = 0; c < quest_cont_per_node; c++)
		{
			_event_condition_node* pC = &pN->m_CondNode[c];

			if(pC->m_nAcepIDXType == -1)
				break;

			if(!_CheckCondition(pC))
			{
				bPass = false;
				break;
			}
		}

		if(bPass)
		{
			//확률
			if(pN->m_nAcepProDen != pN->m_nAcepProNum)
			{
				if(rand()%pN->m_nAcepProDen >= pN->m_nAcepProNum)
					return NULL;
			}

			m_LastSelectEvent.set(pN, pEvent->m_dwIndex, n);
			return &m_LastSelectEvent;
		}
	}

	return NULL;
}

bool CQuestMgr::_CheckCondition(_event_condition_node* pCond)
{
	switch(pCond->m_nAcepIDXType)
	{
	case quest_condition_code_lv:	
		return __CheckCond_LV(pCond->m_nAcepSub, atoi(pCond->m_strAcepVal));

	case quest_condition_code_grade:
		return __CheckCond_Grade(pCond->m_nAcepSub, atoi(pCond->m_strAcepVal));

	case quest_condition_code_dalant:
		return __CheckCond_Dalant(pCond->m_nAcepSub, atoi(pCond->m_strAcepVal));

	case quest_condition_code_gold:
		return __CheckCond_Gold(pCond->m_nAcepSub, atoi(pCond->m_strAcepVal));

	case quest_condition_code_party:
		return __CheckCond_Party(pCond->m_nAcepSub);

	case quest_condition_code_guild:
		return __CheckCond_Guild(pCond->m_nAcepSub);

	case quest_condition_code_nation:
		return __CheckCond_Nation(pCond->m_nAcepSub);

	case quest_coddition_code_equip:
		return __CheckCond_Equip(pCond->m_strAcepVal);

	case quest_condition_code_have:
		return __CheckCond_Have(pCond->m_nAcepSub, pCond->m_strAcepVal);

	case quest_condition_code_mastery:
		return __CheckCond_Mastery(pCond->m_nAcepSub/1000, pCond->m_nAcepSub%1000, atoi(pCond->m_strAcepVal));

	case quest_condition_code_dummy:
		return __CheckCond_Dummy(pCond->m_nAcepSub, pCond->m_strAcepVal);

	case quest_condition_code_race:
		return __CheckCond_Race(pCond->m_strAcepVal);
	}

	return false;
}

bool CQuestMgr::__CheckCond_LV(BOOL bOver, int nLv)
{
	if(bOver)
		return (m_pMaster->m_Param.GetLevel() >= nLv) ? true : false;
	else
		return (m_pMaster->m_Param.GetLevel() <= nLv) ? true : false;

	return false;
}

bool CQuestMgr::__CheckCond_Grade(BOOL bOver, int nGrade)
{
	return true;
}

bool CQuestMgr::__CheckCond_Dalant(BOOL bOver, int nDalant)
{
	if(bOver)
		return (m_pMaster->m_Param.GetDalant() >= nDalant) ? true : false;
	else
		return (m_pMaster->m_Param.GetDalant() <= nDalant) ? true : false;

	return false;
}

bool CQuestMgr::__CheckCond_Gold(BOOL bOver, int nGold)
{
	if(bOver)
		return (m_pMaster->m_Param.GetGold() >= nGold) ? true : false;
	else
		return (m_pMaster->m_Param.GetGold() <= nGold) ? true : false;

	return false;
}

bool CQuestMgr::__CheckCond_Party(BOOL belong)
{
	if(belong)
		return (m_pMaster->m_pPartyMgr->IsPartyMode()) ? true : false;
	else
		return (m_pMaster->m_pPartyMgr->IsPartyMode()) ? false : true;

	return false;
}


bool CQuestMgr::__CheckCond_Guild(BOOL belong)
{
	return true;
}

bool CQuestMgr::__CheckCond_Nation(BOOL belong)
{
	return true;
}

bool CQuestMgr::__CheckCond_Equip(char* pszItemCode)
{
	int nTableCode = ::GetItemTableCode(pszItemCode);
	if(nTableCode == -1)
		return false;

	__ITEM* pItem = &m_pMaster->m_Param.m_dbEquip.m_pStorageList[nTableCode];

	if(!pItem->m_bLoad)
		return false;

	_base_fld* pFld = g_Main.m_tblItemData[nTableCode].GetRecord(pItem->m_wItemIndex);
	if(!pFld)
		return false;

	if(!strcmp(pFld->m_strCode, pszItemCode))
		return true;
	
	return false;
}

bool CQuestMgr::__CheckCond_Have(int nAmonut, char* pszItemCode)
{
	int nStdTableCode = ::GetItemTableCode(pszItemCode);
	if(nStdTableCode == -1)
		return false;

	int nCount = 0;

	for(int i = 0; i < m_pMaster->m_Param.GetBagNum() * one_bag_store_num; i++)
	{
		__ITEM* p = &m_pMaster->m_Param.m_dbInven.m_pStorageList[i];
		if(!p->m_bLoad)
			continue;

		if(p->m_byTableCode != nStdTableCode)
			continue;

		_base_fld* pFld = g_Main.m_tblItemData[p->m_byTableCode].GetRecord(p->m_wItemIndex);
		if(!pFld)
			continue;

		if(strcmp(pFld->m_strCode, pszItemCode))
			continue;

		if(::IsOverLapItem(p->m_byTableCode))
			nCount += p->m_dwDur;
		else
			nCount++;

		if(nCount >= nAmonut)
			return true;	
	}

	return false;
}

bool CQuestMgr::__CheckCond_Mastery(int nMasteryCode, int nMasteryIndex, int nMastery)
{
	if(!_MASTERY_PARAM::IsValidMasteryCode(nMasteryCode, nMasteryIndex))
		return false;

	if(m_pMaster->m_pmMst.GetMasteryPerMast(nMasteryCode, nMasteryIndex) >= nMastery)
		return true;

	return false;
}

bool CQuestMgr::__CheckCond_Dummy(BOOL bInclude, char* pszDummyCode)
{
	return true;
}

bool CQuestMgr::__CheckCond_Race(char* pszRaceSexFlag)
{
	if(pszRaceSexFlag[m_pMaster->m_Param.GetRaceSexCode()] == '1')
		return true;

	return false;
}

_Quest_fld* CQuestMgr::GetQuestFromEvent(BYTE bySelect)// = 0xFF)
{
	if(!m_LastSelectEvent.isset())
		return NULL;

	_str_code* pLinkQuest = NULL;

	int nQuestNum = 0;
	for(int i = 0; i < quest_link_per_node; i++)
	{
		if(!strcmp(m_LastSelectEvent.m_pEvent->m_strLink[i], "-1"))
			break;

		nQuestNum++;
	}

	if(nQuestNum > 0)
	{
		if(!m_LastSelectEvent.m_pEvent->m_bSelectQuestManual) //랜덤..
			pLinkQuest = &m_LastSelectEvent.m_pEvent->m_strLink[rand()%nQuestNum];
		else
		{
			if(bySelect < nQuestNum)
				pLinkQuest = &m_LastSelectEvent.m_pEvent->m_strLink[bySelect];
		}
	}

	m_LastSelectEvent.init();

	if(!pLinkQuest)
		return NULL;

	return (_Quest_fld*)s_tblQuest->GetRecord(*pLinkQuest);	
}

_quest_check_result* CQuestMgr::CheckReqAct(int nActCode, char* pszReqCode)//, BYTE* OUT pbyActIndex, WORD* OUT pwCount, BYTE* OUT pbyQuestDBSlot)
{
	s_QuestCKRet.init();

	int nCheckCount = 0;
	for(int i = 0; i < quest_storage_num; i++)
	{
		__QUEST* pQuest = &m_pQuestData->m_List[i];
		if(pQuest->wIndex == 0xFFFF)
			continue;

		_Quest_fld* pFld = (_Quest_fld*)s_tblQuest->GetRecord(pQuest->wIndex);
		if(!pFld)
			continue;

		for(int n = 0; n < max_quest_act_num; n++)
		{
			_action_node* pNode = &pFld->m_ActionNode[n];
			if(pNode->m_nActType == -1)
				break;

			if(pNode->m_nActType != nActCode)
				continue;

			if(!strcmp(pNode->m_strActSub, pszReqCode))
			{
				s_QuestCKRet.m_List[nCheckCount].byQuestDBSlot = i;
				s_QuestCKRet.m_List[nCheckCount].byActIndex = n;
				s_QuestCKRet.m_List[nCheckCount].wCount = pQuest->wNum[n]+1;
				
				if(pQuest->wNum[n]+1 >= pNode->m_nReqAct)
					s_QuestCKRet.m_List[nCheckCount].wCount = 0xFFFF;	//요구행위를 완수했다면 0xFFFF

				nCheckCount++;
				break;				
			}
		}
	}

	if(nCheckCount > 0)
	{
		s_QuestCKRet.m_byCheckNum = nCheckCount;
		return &s_QuestCKRet;
	}
	return NULL;
}

_quest_fail_result* CQuestMgr::CheckLimLv(int nNewLv)
{
	s_QuestFTRet.init();

	int nCheckCount = 0;
	for(int i = 0; i < quest_storage_num; i++)
	{
		__QUEST* pQuest = &m_pQuestData->m_List[i];
		if(pQuest->wIndex == 0xFFFF)
			continue;

		_Quest_fld* pFld = (_Quest_fld*)s_tblQuest->GetRecord(pQuest->wIndex);
		if(!pFld)
			continue;

		if(pFld->m_nLimLv < nNewLv)
		{
			s_QuestFTRet.m_List[nCheckCount].byQuestDBSlot = i;
			nCheckCount++;
		}
	}

	if(nCheckCount > 0)
	{
		s_QuestFTRet.m_byCheckNum = nCheckCount;
		return &s_QuestFTRet;
	}
	return NULL;
}

