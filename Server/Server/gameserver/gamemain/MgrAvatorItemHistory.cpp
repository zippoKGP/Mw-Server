#include "stdafx.h"
#include "MgrAvatorItemHistory.h"
#include "MyUtil.h"
#include "CharacterDB.h"
#include "MainThread.h"
#include "ErrorEventFromServer.h"
#include <process.h>

static char sBuf[128];
static char sData[10000];

static void _IOFileWrite(char* pszFileName, int nLen, char* pszData);

CMgrAvatorItemHistory::CMgrAvatorItemHistory()
{
	m_dwLastLocalDate = 0;
	m_dwLastLocalHour = 0;

	sprintf(m_szStdPath, ".\\History\\Item");
	::CreateDirectory(m_szStdPath, NULL);

	_strtime(m_szCurTime);
	m_szCurTime[5] = NULL;//�б����� ���
	m_tmrUpdateTime.BeginTimer(60*1000);	//1��

	m_listLogData_10K.SetList(max_log_data_10K);
	m_listLogDataEmpty_10K.SetList(max_log_data_10K);
	for(int i = 0; i < max_log_data_10K; i++)
		m_listLogDataEmpty_10K.PushNode_Back(i);

	m_listLogData_1K.SetList(max_log_data_1K);
	m_listLogDataEmpty_1K.SetList(max_log_data_1K);
	for(i = 0; i < max_log_data_1K; i++)
		m_listLogDataEmpty_1K.PushNode_Back(i);

	m_listLogData_200.SetList(max_log_data_200);
	m_listLogDataEmpty_200.SetList(max_log_data_200);
	for(i = 0; i < max_log_data_200; i++)
		m_listLogDataEmpty_200.PushNode_Back(i);

	m_bIOThread = true;
	::_beginthread(IOThread, 0, (void*)this);
}

CMgrAvatorItemHistory::~CMgrAvatorItemHistory()
{
	m_bIOThread = false;
}

void CMgrAvatorItemHistory::GetNewFileName(DWORD dwAvatorSerial, char* OUT pszFileName)
{	
	DWORD dwNewDate = ::GetLocalDate();
	if(m_dwLastLocalDate != dwNewDate)
	{//���� ���丮����
		char szPath[128];
		sprintf(szPath, "%s\\%d", m_szStdPath, dwNewDate);
		::CreateDirectory(szPath, NULL);
		m_dwLastLocalDate = dwNewDate;
	}

	DWORD dwNewHour = ::GetCurrentHour();
	if(m_dwLastLocalHour != dwNewHour)
	{//���� ���丮����
		char szPath[128];
		sprintf(szPath, "%s\\%d\\%d", m_szStdPath, m_dwLastLocalDate, dwNewHour);
		::CreateDirectory(szPath, NULL);
		m_dwLastLocalHour = dwNewHour;
	}

	//�ð�.
	char szTime[7], szHour[3], szMin[3], szSec[3];
	if(dwNewHour > 9)
		sprintf(szHour, "%d", dwNewHour);
	else
		sprintf(szHour, "0%d", dwNewHour);

	DWORD dwMin = ::GetCurrentMin();		
	if(dwMin > 9)
		sprintf(szMin, "%d", dwMin);
	else
		sprintf(szMin, "0%d", dwMin);

	DWORD dwSec = ::GetCurrentSec();
	if(dwSec > 9)
		sprintf(szSec, "%d", dwSec);
	else
		sprintf(szSec, "0%d", dwSec);

	sprintf(szTime, "%s%s%s", szHour, szMin, szSec);

	sprintf(pszFileName, "%s\\%d\\%d\\%d_%s.his", m_szStdPath, m_dwLastLocalDate, m_dwLastLocalHour, dwAvatorSerial, szTime);
}

void CMgrAvatorItemHistory::OnLoop()
{
	if(m_tmrUpdateTime.CountingTimer())
	{
		_strtime(m_szCurTime);
		m_szCurTime[5] = NULL;//�б����� ���
	}
}

void CMgrAvatorItemHistory::init_load_item(_AVATOR_DATA* pLoadData, char* pszFileName)
{
	sData[0] = NULL;

	//�⺻����
	sprintf(sBuf, "%s [%d] /lv:%d /exp:%f /D:%d /G:%d /B:%d [%s]\n", 
		pLoadData->dbAvator.m_szAvatorName, pLoadData->dbAvator.m_dwRecordNum,
		pLoadData->dbAvator.m_byLevel, pLoadData->dbAvator.m_dExp,
		pLoadData->dbAvator.m_dwDalant, pLoadData->dbAvator.m_dwGold,
		pLoadData->dbAvator.m_byBagNum, m_szCurTime);
	strcat(sData, sBuf);

	//��������������
	//����..
	sprintf(sBuf, "����\n"); 
	strcat(sData, sBuf);
	for(int i = 0; i < equip_fix_num; i++)
	{
		_EQUIPKEY* pKey = &pLoadData->dbAvator.m_EquipKey[i];
		if(!pKey->IsFilled())
			continue;

		sprintf(sBuf, "\t%d:%d %s\n", i, pKey->zItemIndex, ::DisplayItemUpgInfo(i, pLoadData->dbAvator.m_dwFixEquipLv[i])); 
		strcat(sData, sBuf);
	}
		
	//��ű�..
	sprintf(sBuf, "���\n"); 
	strcat(sData, sBuf);
	for(i = 0; i < embellish_fix_num; i++)	
	{
		_EQUIP_DB_BASE::_EMBELLISH_LIST* pList = &pLoadData->dbEquip.m_EmbellishList[i];
		if(!pList->Key.IsFilled())
			continue;
		
		sprintf(sBuf, "\t%d:%d\n", pList->Key.byTableCode, pList->Key.wItemIndex); 
		strcat(sData, sBuf);
	}

	//�κ�..
	sprintf(sBuf, "�κ�\n"); 
	strcat(sData, sBuf);
	for(i = 0; i < one_bag_store_num*pLoadData->dbAvator.m_byBagNum; i++)
	{
		_INVEN_DB_BASE::_LIST* pList = &pLoadData->dbInven.m_List[i];
		if(!pList->Key.IsFilled())
			continue;

		if(!::IsProtectItem(pList->Key.byTableCode))
			continue;

		sprintf(sBuf, "\t%d:%d %d %s\n", pList->Key.byTableCode, pList->Key.wItemIndex, pList->dwDur, ::DisplayItemUpgInfo(pList->Key.byTableCode, pList->dwUpt)); 
		strcat(sData, sBuf);
	}

	//����â..
	sprintf(sBuf, "����\n"); 
	strcat(sData, sBuf);
	for(i = 0; i < force_storage_num; i++)	
	{
		_FORCE_DB_BASE::_LIST* pList = &pLoadData->dbForce.m_List[i];
		if(!pList->Key.IsFilled())
			continue;
		
		sprintf(sBuf, "\t%d:%d %d\n", tbl_code_fcitem, pList->Key.GetIndex(), pList->Key.GetStat()); 
		strcat(sData, sBuf);
	}

	//����â..
	sprintf(sBuf, "����\n"); 
	strcat(sData, sBuf);
	for(i = 0; i < pLoadData->dbCutting.m_byLeftNum; i++)	
	{
		_CUTTING_DB_BASE::_LIST* pList = &pLoadData->dbCutting.m_List[i];

		if(pList->byResIndex == 0xFF)
			continue;

		sprintf(sBuf, "\t%d:%d %d\n", tbl_code_res, pList->byResIndex, pList->byAmt); 
		strcat(sData, sBuf);
	}

	if(pLoadData->dbAvator.m_byRaceSexCode/2 == race_code_bellato)
	{	//����..
		sprintf(sBuf, "����\n"); 
		strcat(sData, sBuf);
		for(i = 0; i < unit_storage_num; i++)
		{
			_UNIT_DB_BASE::_LIST* pList = &pLoadData->dbUnit.m_List[i];
			if(pList->byFrame == 0xFF)
				continue;
			
			sprintf(sBuf, "\t%d> %d %d/%d/%d/%d/%d/%d\n", i, pList->byFrame, 
				(int)pList->byPart[unit_part_head], (int)pList->byPart[unit_part_upper], (int)pList->byPart[unit_part_lower], (int)pList->byPart[unit_part_arms], (int)pList->byPart[unit_part_shoulder], (int)pList->byPart[unit_part_back]); 
			strcat(sData, sBuf);
		}
	}
	else if(pLoadData->dbAvator.m_byRaceSexCode/2 == race_code_cora)
	{	//��ȯ..
		sprintf(sBuf, "��ȯ\n"); 
		strcat(sData, sBuf);
		for(i = 0; i < animus_storage_num; i++)
		{
			_ANIMUS_DB_BASE::_LIST* pList = &pLoadData->dbAnimus.m_List[i];
			if(!pList->Key.IsFilled())
				continue;
			
			sprintf(sBuf, "\t%d:%d %d\n", tbl_code_animus, pList->Key.byItemIndex, pList->dwExp); 
			strcat(sData, sBuf);
		}
	}

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::close(char* pCloseCode, char* pszFileName)
{
	sprintf(sData, "CLOSE %s [%s]\n", pCloseCode, m_szCurTime);

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::pay_money(char* pszClause, DWORD dwPayDalant, DWORD dwPayGold, DWORD dwNewDalant, DWORD dwNewGold, char* pszFileName)
{
	sprintf(sData, "���� %s D:%d G:%d => D:%d G:%d\n", pszClause, dwPayDalant, dwPayGold, dwNewDalant, dwNewGold);

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::take_ground_item(__ITEM* pItem, DWORD dwThrower, char* pszFileName)
{
	if(!::IsProtectItem(pItem->m_byTableCode))
		return;

	if(dwThrower == 0xFFFFFFFF)
	{
		sprintf(sData, "���� %d:%d %d %s [%s]\n", pItem->m_byTableCode, pItem->m_wItemIndex, pItem->m_dwDur, ::DisplayItemUpgInfo(pItem->m_byTableCode, pItem->m_dwLv), m_szCurTime);
	}
	else
	{
		sprintf(sData, "���� %d:%d %d %s tw:%d [%s]\n", pItem->m_byTableCode, pItem->m_wItemIndex, pItem->m_dwDur, ::DisplayItemUpgInfo(pItem->m_byTableCode, pItem->m_dwLv), dwThrower, m_szCurTime);
	}

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::throw_ground_item(__ITEM* pItem, char* pszFileName)
{
	if(!::IsProtectItem(pItem->m_byTableCode))
		return;

	sprintf(sData, "����: %d:%d %d %s [%s]\n", pItem->m_byTableCode, pItem->m_wItemIndex, pItem->m_dwDur, ::DisplayItemUpgInfo(pItem->m_byTableCode, pItem->m_dwLv), m_szCurTime);

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::buy_item(_buy_offer* pOffer, BYTE byOfferNum, DWORD dwCostDalant, DWORD dwCostGold, DWORD dwNewDalant, DWORD dwNewGold, char* pszFileName)
{
	sData[0] = NULL;
	sprintf(sBuf, "����(%d -%d/%d), D:%d G:%d [%s]\n", byOfferNum, dwCostDalant, dwCostGold, dwNewDalant, dwNewGold, m_szCurTime);
	strcat(sData, sBuf);

	for(int i = 0; i < byOfferNum; i++)
	{
		__ITEM* pItem = &pOffer[i].Item;

		sprintf(sBuf, "\t+ %d:%d %d %s\n", pItem->m_byTableCode, pItem->m_wItemIndex, pItem->m_dwDur, ::DisplayItemUpgInfo(pItem->m_byTableCode, pItem->m_dwLv));
		strcat(sData, sBuf);
	}

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::sell_item(_sell_offer* pOffer, BYTE byOfferNum, DWORD dwIncomeDalant, DWORD dwIncomeGold, DWORD dwNewDalant, DWORD dwNewGold, char* pszFileName)
{
	sData[0] = NULL;

	sprintf(sBuf, "�Ű�(%d +%d/%d), D:%d G:%d [%s]\n", byOfferNum, dwIncomeDalant, dwIncomeGold, dwNewDalant, dwNewGold, m_szCurTime);
	strcat(sData, sBuf);

	for(int i = 0; i < byOfferNum; i++)
	{
		__ITEM* pItem = pOffer[i].pItem;

		if(!::IsOverLapItem(pItem->m_byTableCode))
		{
			sprintf(sBuf, "\t- %d:%d %d %s\n", pItem->m_byTableCode, pItem->m_wItemIndex, pItem->m_dwDur, ::DisplayItemUpgInfo(pItem->m_byTableCode, pItem->m_dwLv));
			strcat(sData, sBuf);
		}
		else
		{
			sprintf(sBuf, "\t- %d:%d %d\n", pItem->m_byTableCode, pItem->m_wItemIndex, pOffer[i].byAmount);
			strcat(sData, sBuf);
		}
	}

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::repair_item(_repair_offer* pOffer, BYTE byOfferNum, DWORD dwCostDalant, DWORD dwCostGold, DWORD dwNewDalant, DWORD dwNewGold, char* pszFileName)
{
	sData[0] = NULL;

	sprintf(sBuf, "����(%d -%d/%d), D:%d G:%d [%s]\n", byOfferNum, dwCostDalant, dwCostGold, dwNewDalant, dwNewGold, m_szCurTime);
	strcat(sData, sBuf);

	for(int i = 0; i < byOfferNum; i++)
	{
		__ITEM* pItem = pOffer[i].pItem;

		sprintf(sBuf, "\t%d:%d /%d->%d %s\n", pItem->m_byTableCode, pItem->m_wItemIndex, pItem->m_dwDur, pOffer[i].wNewDurPoint, ::DisplayItemUpgInfo(pItem->m_byTableCode, pItem->m_dwLv));
		strcat(sData, sBuf);
	}

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::trade(__ITEM* pOutItem, int nOutItemNum, DWORD dwOutDalant, DWORD dwOutGold, 
										__ITEM* pInItem, int nInItemNum, DWORD dwInDalant, DWORD dwInGold, 
										DWORD dwDstSerial, DWORD dwSumDalant, DWORD dwSumGold, char* pszFileName)
{
	sData[0] = NULL;

	sprintf(sBuf, "�ŷ�(dst:%d -%d/%d +%d/%d D:%d G:%d) [%s]\n", 
		dwDstSerial, dwOutDalant, dwOutGold, dwInDalant, dwInGold, dwSumDalant, dwSumGold, m_szCurTime);
	strcat(sData, sBuf);

	//�����°� ó��..
	for(int i = 0; i < nOutItemNum; i++)
	{
		sprintf(sBuf, "\t- %d:%d %d %s\n", pOutItem[i].m_byTableCode, pOutItem[i].m_wItemIndex, pOutItem[i].m_dwDur, ::DisplayItemUpgInfo(pOutItem[i].m_byTableCode, pOutItem[i].m_dwLv));
		strcat(sData, sBuf);
	}

	//�����°� ó��..
	for(i = 0; i < nInItemNum; i++)
	{
		sprintf(sBuf, "\t+ %d:%d %d %s\n", pInItem[i].m_byTableCode, pInItem[i].m_wItemIndex, pInItem[i].m_dwDur, ::DisplayItemUpgInfo(pInItem[i].m_byTableCode, pInItem[i].m_dwLv));
		strcat(sData, sBuf);
	}

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::del_unit_after_trade(DWORD dwDstSerial, _UNIT_DB_BASE::_LIST* pUnitData, char* pszFileName)
{
	sprintf(sData, "\t���ְŷ�- dst:%d: %d>fr:%d\n", dwDstSerial, pUnitData->bySlotIndex, pUnitData->byFrame);

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::add_unit_after_trade(DWORD dwDstSerial, _UNIT_DB_BASE::_LIST* pUnitData, char* pszFileName)
{
	BYTE* pPart = pUnitData->byPart;
	sprintf(sData, "\t���ְŷ�+ dst:%d: %d>fr:%d %d/%d/%d/%d/%d/%d\n", dwDstSerial, pUnitData->bySlotIndex, pUnitData->byFrame, 
		(int)pPart[unit_part_head], (int)pPart[unit_part_upper], (int)pPart[unit_part_lower], (int)pPart[unit_part_arms], (int)pPart[unit_part_shoulder], (int)pPart[unit_part_back]);
		

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::grade_up_item(__ITEM* pItem, __ITEM* pTalik, __ITEM* pJewel, BYTE byJewelNum, BYTE byErrCode, DWORD dwAfterLv, char* pszFileName)
{
	sData[0] = NULL;

	char szBeforeLv[32];
	strcpy(szBeforeLv, ::DisplayItemUpgInfo(pItem->m_byTableCode, pItem->m_dwLv));

	if(byErrCode == 0)
	{
		sprintf(sBuf, "����(����): %d:%d %d %s->%s [%s]\n", pItem->m_byTableCode, pItem->m_wItemIndex, pItem->m_dwDur, szBeforeLv, ::DisplayItemUpgInfo(pItem->m_byTableCode, dwAfterLv), m_szCurTime);
		strcat(sData, sBuf);
	}
	else if(byErrCode == miss_upgrade_random)
	{
		sprintf(sBuf, "����(����): %d:%d %d %s [%s]\n", pItem->m_byTableCode, pItem->m_wItemIndex, pItem->m_dwDur, szBeforeLv, m_szCurTime);
		strcat(sData, sBuf);
	}
	else if(byErrCode == miss_upgrade_destroy_talik)
	{
		sprintf(sBuf, "����(����): %d:%d %d %s [%s]\n", pItem->m_byTableCode, pItem->m_wItemIndex, pItem->m_dwDur, szBeforeLv, ::DisplayItemUpgInfo(pItem->m_byTableCode, dwAfterLv), m_szCurTime);
		strcat(sData, sBuf);
	}
	else if(byErrCode == miss_upgrade_destroy_item)
	{
		sprintf(sBuf, "����(�ı�): %d:%d %d %s [%s]\n", pItem->m_byTableCode, pItem->m_wItemIndex, pItem->m_dwDur, szBeforeLv, m_szCurTime);
		strcat(sData, sBuf);
	}

	sprintf(sBuf, "\t- T %d:%d \n", pTalik->m_byTableCode, pTalik->m_wItemIndex);
	strcat(sData, sBuf);

	for(int i = 0; i < byJewelNum; i++)
	{
		sprintf(sBuf, "\t- R %d:%d\n", pJewel[i].m_byTableCode, pJewel[i].m_wItemIndex);
		strcat(sData, sBuf);
	}

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::grade_down_item(__ITEM* pItem, __ITEM* pTalik, DWORD dwAfterLv, char* pszFileName)
{
	sData[0] = NULL;

	char szBeforeLv[32];
	strcpy(szBeforeLv, ::DisplayItemUpgInfo(pItem->m_byTableCode, pItem->m_dwLv));

	sprintf(sBuf, "�ٱ�: %d:%d %d %s->%s [%s]\n", pItem->m_byTableCode, pItem->m_wItemIndex, pItem->m_dwDur, szBeforeLv, ::DisplayItemUpgInfo(pItem->m_byTableCode, dwAfterLv), m_szCurTime);
	strcat(sData, sBuf);

	sprintf(sBuf, "\t- T %d:%d\n", pTalik->m_byTableCode, pTalik->m_wItemIndex);
	strcat(sData, sBuf);

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::exchange_money(DWORD dwCurDalant, DWORD dwCurGold, DWORD dwNewDalant, DWORD dwNewGold, char* pszFileName)
{
	sData[0] = NULL;

	sprintf(sData, "ȯ��: D:%d->%d G:%d->%d [%s]\n", dwCurDalant, dwNewDalant, dwCurGold, dwNewGold, m_szCurTime);

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::cut_item(__ITEM* pOreItem, int nOreNum, WORD* pwCuttingResBuffer, DWORD dwCostDalant, DWORD dwNewDalant, char* pszFileName)
{
	sData[0] = NULL;

	sprintf(sBuf, "����: %d:%d * %d -D:%d D:%d [%s]\n", pOreItem->m_byTableCode, pOreItem->m_wItemIndex, nOreNum, dwCostDalant, dwNewDalant, m_szCurTime);
	strcat(sData, sBuf);

	for(int i = 0; i < ::GetMaxResKind(); i++)
	{
		if(pwCuttingResBuffer[i] > 0)
		{
			sprintf(sBuf, "\t+ %d:%d * %d\n", tbl_code_res, i, pwCuttingResBuffer[i]);
			strcat(sData, sBuf);
		}
	}

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::make_item(__ITEM* pMaterial, BYTE* pbyMtrNum, BYTE byMaterialNum, BYTE byRetCode, __ITEM* pMakeItem, char* pszFileName)
{
	sData[0] = NULL;

	if(byRetCode == 0)
	{
		sprintf(sBuf, "����(����): %d:%d %d %s [%s]\n", pMakeItem->m_byTableCode, pMakeItem->m_wItemIndex, pMakeItem->m_dwDur, ::DisplayItemUpgInfo(pMakeItem->m_byTableCode, pMakeItem->m_dwLv), m_szCurTime);
		strcat(sData, sBuf);
	}
	else
	{
		sprintf(sBuf, "����(����) [%s]\n", m_szCurTime);
		strcat(sData, sBuf);
	}

	for(int i = 0; i < byMaterialNum; i++)
	{
		if(::IsProtectItem(pMaterial[i].m_byTableCode))
		{
			sprintf(sBuf, "\t- %d:%d * %d\n", pMaterial[i].m_byTableCode, pMaterial[i].m_wItemIndex,  pbyMtrNum[i]);
			strcat(sData, sBuf);
		}
	}

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::cheat_alter_money(DWORD dwNewDalant, DWORD dwNewGold, char* pszFileName)
{
	sprintf(sData, "ġƮ(��): D:%d G:%d\n", dwNewDalant, dwNewGold);

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::cheat_add_item(__ITEM* pItem, BYTE byAddNum, char* pszFileName)
{
	sData[0] = NULL;

	sprintf(sBuf, "ġƮ(����+) [%s]\n", m_szCurTime);
	strcat(sData, sBuf);

	for(int i = 0; i < byAddNum; i++)
	{
		if(::IsProtectItem(pItem[i].m_byTableCode))
		{
			sprintf(sBuf, "\t+ %d:%d %d %s\n", pItem[i].m_byTableCode, pItem[i].m_wItemIndex, pItem[i].m_dwDur, ::DisplayItemUpgInfo(pItem[i].m_byTableCode, pItem[i].m_dwLv));
			strcat(sData, sBuf);
		}
	}

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::cheat_del_item(__ITEM* pItem, BYTE byDelNum, char* pszFileName)
{
	sData[0] = NULL;

	sprintf(sBuf, "ġƮ(����-) [%s]\n", m_szCurTime);
	strcat(sData, sBuf);

	for(int i = 0; i < byDelNum; i++)
	{
		if(::IsProtectItem(pItem[i].m_byTableCode))
		{
			sprintf(sBuf, "\t- %d:%d %d %s\n", pItem[i].m_byTableCode, pItem[i].m_wItemIndex,  pItem[i].m_dwDur, ::DisplayItemUpgInfo(pItem[i].m_byTableCode, pItem[i].m_dwLv));
			strcat(sData, sBuf);
		}
	}

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::cut_clear_item(WORD* pwCuttingResBuffer, DWORD dwAddGold, DWORD dwNewGold, char* pszFileName)
{
	sData[0] = NULL;

	sprintf(sBuf, "����ó��: +G:%d G:%d [%s]\n", dwAddGold, dwNewGold, m_szCurTime);
	strcat(sData, sBuf);

	for(int i = 0; i < ::GetMaxResKind(); i++)
	{
		if(pwCuttingResBuffer[i] > 0)
		{
			sprintf(sBuf, "\t- %d:%d * %d\n", tbl_code_res, i, pwCuttingResBuffer[i]);
			strcat(sData, sBuf);
		}
	}

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::consume_del_item(__ITEM* pItem, char* pszFileName)
{
	sData[0] = NULL;

	sprintf(sData, "�Ҹ�: %d:%d %s [%s]\n", pItem->m_byTableCode,  pItem->m_wItemIndex, ::DisplayItemUpgInfo(pItem->m_byTableCode, pItem->m_dwLv), m_szCurTime);

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::reward_add_money(char* pszClause, DWORD dwAddDalant, DWORD dwAddGold, DWORD dwSumDalant, DWORD dwSumGold, char* pszFileName)
{
	sprintf(sData, "����(%s:��): +(D:%d G%d), (D:%d G%d)\n", pszClause, dwAddDalant, dwAddGold, dwSumDalant, dwSumGold);

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::reward_add_item(char* pszClause, __ITEM* pItem, char* pszFileName)
{
	sprintf(sData, "����(%s:������): d:%d %d %s\n", pszClause, pItem->m_byTableCode, pItem->m_dwDur, ::DisplayItemUpgInfo(pItem->m_byTableCode, pItem->m_dwLv));

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::buy_unit(BYTE bySlotIndex, _UNIT_DB_BASE::_LIST* pData, DWORD* pdwConsumMoney, DWORD dwNewDalant, DWORD dwNewGold, char* pszFileName)
{
	BYTE* pPart = pData->byPart;
	sprintf(sData, "���ֱ���: %d>fr:%d %d/%d/%d/%d/%d/%d -(D:%d G%d), (D:%d G%d)\n", bySlotIndex, pData->byFrame, 
		(int)pPart[unit_part_head], (int)pPart[unit_part_upper], (int)pPart[unit_part_lower], (int)pPart[unit_part_arms], (int)pPart[unit_part_shoulder], (int)pPart[unit_part_back],
		pdwConsumMoney[money_unit_dalant], pdwConsumMoney[money_unit_gold], dwNewDalant, dwNewGold);

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::sell_unit(BYTE bySlotIndex, BYTE byFrameCode, DWORD* pdwAddMoney, DWORD dwPayDalant, DWORD dwNewDalant, DWORD dwNewGold, char* pszFileName)
{
	sprintf(sData, "����ó��: %d>fr:%d +(D:%d G:%d), -(D:%d), (D:%d G%d)\n", 
		bySlotIndex, byFrameCode, pdwAddMoney[money_unit_dalant], pdwAddMoney[money_unit_gold], dwPayDalant, dwNewDalant, dwNewGold);

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::destroy_unit(BYTE bySlotIndex, BYTE byFrameCode, char* pszFileName)
{
	sprintf(sData, "�����ı�: %d>fr:%d\n", 	bySlotIndex, byFrameCode);

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::tuning_unit(BYTE bySlotIndex, _UNIT_DB_BASE::_LIST* pData, int* pnPayMoney, DWORD dwNewDalant, DWORD dwNewGold, char* pszFileName)
{
	BYTE* pPart = pData->byPart;
	sprintf(sData, "����Ʃ��: %d>fr:%d %d/%d/%d/%d/%d/%d -(D:%d G:%d), (D:%d, G:%d)\n", bySlotIndex, pData->byFrame, 
		(int)pPart[unit_part_head], (int)pPart[unit_part_upper], (int)pPart[unit_part_lower], (int)pPart[unit_part_arms], (int)pPart[unit_part_shoulder], (int)pPart[unit_part_back],
		pnPayMoney[money_unit_dalant], pnPayMoney[money_unit_gold], dwNewDalant, dwNewGold);

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::char_copy(DWORD dwDstSerial, char* pszFileName)
{
	sprintf(sData, "ĳ����ī��: dst: %d\n", dwDstSerial);

	WriteFile(pszFileName, sData);
}

void CMgrAvatorItemHistory::WriteFile(char* pszFileName, char* pszLog)
{
	static DWORD dwNodeIndex;
		
	//����� ���
	DWORD	dwLen = strlen(pszLog);	

	if(dwLen < LOG_SIZE_200)
	{
		if(m_listLogDataEmpty_200.PopNode_Front(&dwNodeIndex))
		{
			__LOG_DATA_200* p =  &m_LogData_200[dwNodeIndex];

			strcpy(p->szFileName, pszFileName);
			p->nLen = dwLen;
			memcpy(p->sData, pszLog, dwLen);
			p->sData[dwLen] = NULL;

			m_listLogData_200.PushNode_Back(dwNodeIndex);
			return;
		}
	}
	else if(dwLen < LOG_SIZE_1K)
	{
		if(m_listLogDataEmpty_1K.PopNode_Front(&dwNodeIndex))
		{
			__LOG_DATA_1K* p =  &m_LogData_1K[dwNodeIndex];

			strcpy(p->szFileName, pszFileName);
			p->nLen = dwLen;
			memcpy(p->sData, pszLog, dwLen);
			p->sData[dwLen] = NULL;

			m_listLogData_1K.PushNode_Back(dwNodeIndex);
			return;
		}
	}
	else if(dwLen < LOG_SIZE_10K)
	{
		if(m_listLogDataEmpty_10K.PopNode_Front(&dwNodeIndex))
		{
			__LOG_DATA_10K* p =  &m_LogData_10K[dwNodeIndex];

			strcpy(p->szFileName, pszFileName);
			p->nLen = dwLen;
			memcpy(p->sData, pszLog, dwLen);
			p->sData[dwLen] = NULL;

			m_listLogData_10K.PushNode_Back(dwNodeIndex);
			return;
		}
	}

	//�ٷ�..
	_IOFileWrite(pszFileName, dwLen, pszLog);
}

void CMgrAvatorItemHistory::IOThread(void* pv)
{
	CMgrAvatorItemHistory* pDoc = (CMgrAvatorItemHistory*)pv;
	DWORD dwNodeIndex;

	while(pDoc->m_bIOThread)
	{
		while(pDoc->m_listLogData_10K.PopNode_Front(&dwNodeIndex))
		{
			__LOG_DATA_10K* p =  &pDoc->m_LogData_10K[dwNodeIndex];

			_IOFileWrite(p->szFileName, p->nLen, p->sData);

			pDoc->m_listLogDataEmpty_10K.PushNode_Back(dwNodeIndex);
			Sleep(0);
		}

		while(pDoc->m_listLogData_1K.PopNode_Front(&dwNodeIndex))
		{
			__LOG_DATA_1K* p =  &pDoc->m_LogData_1K[dwNodeIndex];

			_IOFileWrite(p->szFileName, p->nLen, p->sData);

			pDoc->m_listLogDataEmpty_1K.PushNode_Back(dwNodeIndex);
			Sleep(0);
		}

		while(pDoc->m_listLogData_200.PopNode_Front(&dwNodeIndex))
		{
			__LOG_DATA_200* p =  &pDoc->m_LogData_200[dwNodeIndex];

			_IOFileWrite(p->szFileName, p->nLen, p->sData);

			pDoc->m_listLogDataEmpty_200.PushNode_Back(dwNodeIndex);
			Sleep(0);
		}

		Sleep(1);
	}
	::_endthreadex(0);
}

void _IOFileWrite(char* pszFileName, int nLen, char* pszData)
{
	HANDLE hFile = ::CreateFile(pszFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		::SetFilePointer(hFile, 0, NULL, FILE_END);

		DWORD dwWrite;
		::WriteFile(hFile, pszData, nLen, &dwWrite, NULL);

		::CloseHandle(hFile);	
	}	
}