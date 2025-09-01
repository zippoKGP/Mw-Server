#include "stdafx.h"
#include "Player.h"
#include "MainThread.h"
#include "ErrorEventFromServer.h"
#include "UnitPartFld.h"
#include "UnitFrameFld.h"
#include "UnitBulletFld.h"

const int KEEPING_FEE = 10000;
const int PULLING_FEE = 10000;

static _UnitKeyItem_fld* gGetUnitKeyMatchFrame(BYTE byFrameCode);

//유닛구입
void CPlayer::pc_UnitFrameBuyRequest(BYTE byFrameCode)
{
	BYTE byRetCode = 0;
	BYTE byUnitSlotIndex = 0xFF;
	DWORD dwConsumMoney[MONEY_UNIT_NUM] = {0,};
	_UnitFrame_fld* pFrameFld = (_UnitFrame_fld*)g_Main.m_tblUnitFrame.GetRecord(byFrameCode);
	_UnitKeyItem_fld* pKeyFld = NULL;
	BYTE byBasePart[UNIT_PART_NUM];
	float fR = 1.0f + ::eGetTex(race_code_bellato);
	int i;

	//존재하는 프레임인지..
	if(!pFrameFld)
	{
		byRetCode = error_unit_not_exist_frame;
		goto RESULT;
	}

	//종족이 맞는지..
	if(m_Param.GetRaceSexCode()/2 != race_code_bellato)
	{
		byRetCode = error_unit_not_usable_race;
		goto RESULT;
	}

	//운행중인 유닛이 있는지..
	if(m_pUsingUnit)
	{
		byRetCode = error_unit_check_out;
		goto RESULT;
	}

	//빈 슬롯이 있는지..
	for(i = 0; i < unit_storage_num; i++)
	{
		if(m_Param.m_UnitDB.m_List[i].byFrame == 0xFF)
		{
			byUnitSlotIndex = i;
			break;
		}
	}
	if(byUnitSlotIndex == 0xFF)
	{
		byRetCode = error_unit_full_unit;
		goto RESULT;
	}

	//빈 인벤이 있는지..(키추가)
	if(m_Param.m_dbInven.GetNumEmptyCon() == 0)
	{
		byRetCode = error_unit_full_inven;
		goto RESULT;
	}

	//키종류찾기
	pKeyFld = ::gGetUnitKeyMatchFrame(byFrameCode);
	if(!pKeyFld)
	{
		byRetCode = error_unit_not_match_frame;
		goto RESULT;
	}

	//돈은 있는지..
	dwConsumMoney[pFrameFld->m_nMoney] = pFrameFld->m_nStdPrice * fR;
	if(dwConsumMoney[pFrameFld->m_nMoney] > GetMoney(pFrameFld->m_nMoney))
	{
		byRetCode = error_unit_lack_money;
		goto RESULT;
	}

	//기본파트세팅체크
	memset(byBasePart, 0xFF, sizeof(byBasePart));
	static _str_code* pStdPartCode = &pFrameFld->m_strDFHead;
	for(i = 0; i < UNIT_PART_NUM; i++)
	{	
		if(!strcmp(pStdPartCode[i] , "-1"))
			continue;

		_UnitPart_fld* pPartFld = (_UnitPart_fld*)g_Main.m_tblUnitPart[i].GetRecord(pStdPartCode[i]);
		if(!pPartFld)
		{
			byRetCode = error_unit_server_bin_error;
			goto RESULT;
		}
		byBasePart[i] = pPartFld->m_dwIndex;
	}

RESULT:
	
	WORD wKeySerial = 0xFFFF;
	WORD wKeyIndex = 0XFFFF;

	if(byRetCode == 0)
	{
		_UNIT_DB_BASE::_LIST* pEmptySlot = &m_Param.m_UnitDB.m_List[byUnitSlotIndex];

		//프레임장착
		pEmptySlot->byFrame = byFrameCode;

		//기본파트세팅
		for(i = 0; i < UNIT_PART_NUM; i++)
			pEmptySlot->byPart[i] = byBasePart[i];

		//내구도세팅
		pEmptySlot->dwGauge = pFrameFld->m_nUnit_HP;
		
		//월드에 갱신..	
		m_pUserDB->Update_UnitInsert(byUnitSlotIndex, pEmptySlot);
		
		//키 아이템 추가..
		wKeySerial = m_Param.GetNewItemSerial();
		wKeyIndex = pKeyFld->m_dwIndex;

		_STORAGE_LIST::_storage_con KeyItem (tbl_code_key, pKeyFld->m_dwIndex, 0, byUnitSlotIndex, wKeySerial);
		Emb_AddStorage(_STORAGE_POS::INVEN, &KeyItem);

		//돈계산..
		AlterMoney(-dwConsumMoney[money_unit_gold], -dwConsumMoney[money_unit_dalant]);

		//ITEM HISTORY..
		s_MgrItemHistory.buy_unit(byUnitSlotIndex, pEmptySlot, dwConsumMoney, m_Param.GetDalant(), m_Param.GetGold(), m_szItemHistoryFileName);

		//ECONOMY SET..
		::eAddGold(race_code_bellato, dwConsumMoney[money_unit_gold]);
		::eAddDalant(race_code_bellato, dwConsumMoney[money_unit_dalant]);
	}

	SendMsg_UnitFrameBuyResult(byRetCode, byFrameCode, byUnitSlotIndex, wKeyIndex, wKeySerial, dwConsumMoney);
}

//유닛처분
void CPlayer::pc_UnitSellRequest(BYTE bySlotIndex)
{
	BYTE byRetCode = 0;
	BYTE byFrameCode = m_Param.m_UnitDB.m_List[bySlotIndex].byFrame;
	_UnitFrame_fld* pFrameFld = (_UnitFrame_fld*)g_Main.m_tblUnitFrame.GetRecord(byFrameCode);
	_UNIT_DB_BASE::_LIST* pUnitSlot = &m_Param.m_UnitDB.m_List[bySlotIndex];
	DWORD dwTotalNonpay = 0;

	//존재하는 프레임인지..
	if(!pFrameFld)
	{
		byRetCode = error_unit_not_exist_frame;
		goto RESULT;
	}

	//종족이 맞는지..
	if(m_Param.GetRaceSexCode()/2 != race_code_bellato)
	{
		byRetCode = error_unit_not_usable_race;
		goto RESULT;
	}
	
	//운행중인 유닛이 있는지..
	if(m_pUsingUnit)
	{
		byRetCode = error_unit_check_out;
		goto RESULT;
	}

	//해당 슬롯에 유닛이 있는지..
	if(byFrameCode == 0xFF)
	{
		byRetCode = error_unit_empty_slot;
		goto RESULT;
	}

	//미납요금이 보유달란트보다 큰지 검사..
	dwTotalNonpay = pUnitSlot->nKeepingFee + pUnitSlot->nPullingFee;
	if(dwTotalNonpay > m_Param.GetDalant())
	{
		byRetCode = error_unit_lack_non_pay;
		goto RESULT;
	}

RESULT:

	DWORD dwAddMoney[MONEY_UNIT_NUM] = {0,};
	WORD wKeySerial = 0xFFFF;

	if(byRetCode == 0)
	{
		_UNIT_DB_BASE::_LIST* pUnitSlot = &m_Param.m_UnitDB.m_List[bySlotIndex];

		//슬롯을 비운다..
		pUnitSlot->byFrame = 0xFF;

		//키도 삭제..
		wKeySerial = _DeleteUnitKey(bySlotIndex);

		//돈을 계산..
		_UnitFrame_fld* pFrameFld = (_UnitFrame_fld*)g_Main.m_tblUnitFrame.GetRecord(byFrameCode);

		float fR = 1.0f - ::eGetTex(race_code_bellato);
		dwAddMoney[pFrameFld->m_nMoney] = pFrameFld->m_nStdPrice/2 * fR + 0.5f;

		AlterMoney(dwAddMoney[money_unit_gold], (int)dwAddMoney[money_unit_dalant]-dwTotalNonpay);

		//SYNC DB..
		m_pUserDB->Update_UnitDelete(bySlotIndex);			

		//ITEM HISTORY..
		s_MgrItemHistory.sell_unit(bySlotIndex, byFrameCode, dwAddMoney, dwTotalNonpay, m_Param.GetDalant(), m_Param.GetGold(), m_szItemHistoryFileName);

		//ECONOMY SET..
		::eAddGold(race_code_bellato, dwAddMoney[money_unit_gold]);
		::eAddDalant(race_code_bellato, dwAddMoney[money_unit_dalant]);
	}

	SendMsg_UnitSellResult(byRetCode, bySlotIndex, wKeySerial, dwAddMoney, dwTotalNonpay, m_Param.GetDalant(), m_Param.GetGold());
}

void CPlayer::pc_UnitPartTuningRequest(BYTE bySlotIndex, BYTE byTuningNum, _tuning_data* pTuningData)
{
	BYTE byRetCode = 0;
	_UNIT_DB_BASE::_LIST* pUnitSlot = &m_Param.m_UnitDB.m_List[bySlotIndex];
	BYTE byFrameCode = m_Param.m_UnitDB.m_List[bySlotIndex].byFrame;
	int nTotalCost[MONEY_UNIT_NUM] = {0,};
	float fBuyR = 1.0f + ::eGetTex(race_code_bellato);
	float fSellR = 1.0f - ::eGetTex(race_code_bellato);
	int i;

	//종족이 맞는지..
	if(m_Param.GetRaceSexCode()/2 != race_code_bellato)
	{
		byRetCode = error_unit_not_usable_race;
		goto RESULT;
	}
	
	//운행중인 유닛이 있는지..
	if(m_pUsingUnit)
	{
		byRetCode = error_unit_check_out;
		goto RESULT;
	}

	//해당 슬롯에 유닛이 있는지..
	if(byFrameCode == 0xFF)
	{
		byRetCode = error_unit_empty_slot;
		goto RESULT;
	}

	//프레임이 맞는지..
	for(i = 0; i < byTuningNum; i++)
	{
		_UnitPart_fld* pFld = (_UnitPart_fld*)g_Main.m_tblUnitPart[pTuningData[i].byPartCode].GetRecord(pTuningData[i].byPartIndex);
		if(!pFld)
		{
			byRetCode = error_unit_not_exist_part;
			goto RESULT;
		}
		if(pFld->m_strDefFrame[byFrameCode] == '0')
		{
			byRetCode = error_unit_not_match_frame;
			goto RESULT;
		}

		//레벨체크..
		if(pFld->m_nLevelLim > m_Param.GetLevel())
		{
			byRetCode = error_unit_tuning_lv;
			goto RESULT;
		}

		//튜닝가능한 마스터리인지..
		_EQUIP_MASTERY_LIM* pML = (_EQUIP_MASTERY_LIM*)&pFld->m_nExpertID1;

		for(int m = 0; m < EQUIP_MASTERY_CHECK_NUM; m++)
		{
			int code = pML[m].nMasteryCode;

			if(code == equip_mastery_nothing)
				continue;

			if(m_pmMst.GetEquipMastery(code) < pML[m].nLimMastery)
			{
				byRetCode = error_unit_tuning_mastery;
				goto RESULT;
			}
		}

		//가격합산
		nTotalCost[pFld->m_nMoney] += (pFld->m_nStdPrice * fBuyR);

		//백팩이라면 담겨있는거 가격에서 빼준다..
		if(pTuningData[i].byPartCode == unit_part_back)
		{
			for(int s = 0; s < max_unit_spare; s++)
			{
				if(pUnitSlot->dwSpare[s] == 0xFFFFFFFF)
					continue;

				_unit_bullet_param* p = (_unit_bullet_param*)&pUnitSlot->dwSpare[s];

				_UnitBullet_fld* pButFld = (_UnitBullet_fld*)g_Main.m_tblUnitBullet.GetRecord(p->wBulletIndex);
				if(pButFld)
					nTotalCost[pButFld->m_nMoney] -= (pButFld->m_nStdPrice/2 * fSellR + 0.5f);
			}
		}
	}

	//돈은 있는지..
	for(i = 0; i < MONEY_UNIT_NUM; i++)
	{
		if(nTotalCost[i] > GetMoney(i))
		{
			byRetCode = error_unit_lack_money;
			goto RESULT;
		}	
	}

RESULT:

	if(byRetCode == 0)
	{
		//파트를 갈아끼운다..
		for(i = 0; i < byTuningNum; i++)
		{
			pUnitSlot->byPart[pTuningData[i].byPartCode] = pTuningData[i].byPartIndex;

			//백팩이라면 sqare를 비운다..
			if(pTuningData[i].byPartCode == unit_part_back)
				memset(pUnitSlot->dwSpare, 0xFFFFFFFF, sizeof(DWORD)*max_unit_spare);
		}

		//돈계산..		
		AlterMoney(-nTotalCost[money_unit_gold], -nTotalCost[money_unit_dalant]);

		//SYNC DB..
		m_pUserDB->Update_UnitData(bySlotIndex, pUnitSlot);

		//ITEM HISTORY..
		s_MgrItemHistory.tuning_unit(bySlotIndex, pUnitSlot, nTotalCost, m_Param.GetDalant(), m_Param.GetGold(), m_szItemHistoryFileName);

		//ECONOMY SET..
		::eAddGold(race_code_bellato, nTotalCost[money_unit_gold]);
		::eAddDalant(race_code_bellato, nTotalCost[money_unit_dalant]);
	}

	SendMsg_UnitPartTuningResult(byRetCode, bySlotIndex, nTotalCost);
}

void CPlayer::pc_UnitFrameRepairRequest(BYTE bySlotIndex)
{
	BYTE byRetCode = 0;
	_UNIT_DB_BASE::_LIST* pUnitSlot = &m_Param.m_UnitDB.m_List[bySlotIndex];
	BYTE byFrameCode = m_Param.m_UnitDB.m_List[bySlotIndex].byFrame;
	_UnitFrame_fld* pFrameFld = (_UnitFrame_fld*)g_Main.m_tblUnitFrame.GetRecord(byFrameCode);
	DWORD dwConsumDalant = 0;
	DWORD dwNewGauge = 0;
	float fR = 1.0f + ::eGetTex(race_code_bellato);
	int i;

	//종족이 맞는지..
	if(m_Param.GetRaceSexCode()/2 != race_code_bellato)
	{
		byRetCode = error_unit_not_usable_race;
		goto RESULT;
	}
	
	//운행중인 유닛이 있는지..
	if(m_pUsingUnit)
	{
		byRetCode = error_unit_check_out;
		goto RESULT;
	}

	if(!pFrameFld)
	{
		byRetCode = error_unit_not_exist_frame;
		goto RESULT;
	}

	//해당 슬롯에 유닛이 있는지..
	if(byFrameCode == 0xFF)
	{
		byRetCode = error_unit_empty_slot;
		goto RESULT;
	}

	//수리할수있는 상태인지..
	dwNewGauge = pFrameFld->m_nUnit_HP;
	if(pUnitSlot->dwGauge >= dwNewGauge)
	{
		byRetCode = error_unit_full_gauge;
		goto RESULT;
	}

	//돈은 있는지..
	dwConsumDalant += (pFrameFld->m_nRepPrice * fR);
	for(i = 0; i < UNIT_PART_NUM; i++)
	{
		_UnitPart_fld* pPartFld = (_UnitPart_fld*)g_Main.m_tblUnitPart[i].GetRecord(pUnitSlot->byPart[i]);
		if(!pPartFld)
			continue;
		dwConsumDalant += pPartFld->m_nRepPrice;
	}
	dwConsumDalant *= (dwNewGauge - pUnitSlot->dwGauge);
	if(dwConsumDalant > m_Param.GetDalant())
	{
		byRetCode = error_unit_lack_money;
		goto RESULT;
	}

RESULT:

	if(byRetCode == 0)
	{
		//게이지를 채운다..
		pUnitSlot->dwGauge = dwNewGauge;

		//돈계산..
		AlterMoney(0, -dwConsumDalant);

		//SYNC DB..
		m_pUserDB->Update_UnitData(bySlotIndex, pUnitSlot);

		//ITEM HISTORY..
		if(dwConsumDalant > 0)
			s_MgrItemHistory.pay_money("유닛수리",  dwConsumDalant, 0, m_Param.GetGold(), m_Param.GetDalant(), m_szItemHistoryFileName);

		//ECONOMY SET..		
		::eAddDalant(race_code_bellato, dwConsumDalant);
	}

	SendMsg_UnitFrameRepairResult(byRetCode, bySlotIndex, dwNewGauge, dwConsumDalant);
}

void CPlayer::pc_UnitBulletFillRequest(BYTE bySlotIndex, BYTE* pbyBulletIndex)
{
	BYTE byRetCode = 0;
	_UNIT_DB_BASE::_LIST* pUnitSlot = &m_Param.m_UnitDB.m_List[bySlotIndex];
	BYTE byFrameCode = m_Param.m_UnitDB.m_List[bySlotIndex].byFrame;
	_UnitBullet_fld* ppBulletFld[UNIT_BULLET_NUM] = { (_UnitBullet_fld*)g_Main.m_tblUnitBullet.GetRecord(pbyBulletIndex[0]), (_UnitBullet_fld*)g_Main.m_tblUnitBullet.GetRecord(pbyBulletIndex[1]) };
	DWORD dwConsumMoney[MONEY_UNIT_NUM] = {0,};
	float fR = 1.0f + ::eGetTex(race_code_bellato);
	int i;

	//종족이 맞는지..
	if(m_Param.GetRaceSexCode()/2 != race_code_bellato)
	{
		byRetCode = error_unit_not_usable_race;
		goto RESULT;
	}
	
	//운행중인 유닛이 있는지..
	if(m_pUsingUnit)
	{
		byRetCode = error_unit_check_out;
		goto RESULT;
	}

	//해당 슬롯에 유닛이 있는지..
	if(byFrameCode == 0xFF)
	{
		byRetCode = error_unit_empty_slot;
		goto RESULT;
	}

	//돈은 있는지..
	for(i = 0; i < UNIT_BULLET_NUM; i++)
	{
		if(ppBulletFld[i])
			dwConsumMoney[ppBulletFld[i]->m_nMoney] += (ppBulletFld[i]->m_nStdPrice * fR);
	}
	for(i = 0; i < MONEY_UNIT_NUM; i++)
	{
		if(dwConsumMoney[i] > GetMoney(i))
		{
			byRetCode = error_unit_lack_money;
			goto RESULT;
		}	
	}

RESULT:

	if(byRetCode == 0)
	{
		//탄창을 채운다..
		for(i = 0; i < UNIT_BULLET_NUM; i++)
		{
			if(ppBulletFld[i])
			{
				_unit_bullet_param* p = (_unit_bullet_param*)&pUnitSlot->dwBullet[i];

				p->wBulletIndex = pbyBulletIndex[i];
				p->wLeftNum = ppBulletFld[i]->m_nDurUnit;
			}
		}

		//돈계산..
		AlterMoney(-dwConsumMoney[money_unit_gold], -dwConsumMoney[money_unit_dalant]);

		//SYNC DB..
		m_pUserDB->Update_UnitData(bySlotIndex, pUnitSlot);

		//ITEM HISTORY..
		if(dwConsumMoney[money_unit_dalant] > 0 || dwConsumMoney[money_unit_gold] > 0)
			s_MgrItemHistory.pay_money("유닛탄창충전",  dwConsumMoney[money_unit_dalant], dwConsumMoney[money_unit_gold], m_Param.GetGold(), m_Param.GetDalant(), m_szItemHistoryFileName);

		//ECONOMY SET..
		::eAddGold(race_code_bellato, dwConsumMoney[money_unit_gold]);
		::eAddDalant(race_code_bellato, dwConsumMoney[money_unit_dalant]);
	}

	SendMsg_UnitBulletFillResult(byRetCode, bySlotIndex, pbyBulletIndex, dwConsumMoney);
}

void CPlayer::pc_UnitPackFillRequest(BYTE bySlotIndex, BYTE byFillNum, _unit_pack_fill_request_clzo::__list* pList)
{
	BYTE byRetCode = 0;
	_UNIT_DB_BASE::_LIST* pUnitSlot = &m_Param.m_UnitDB.m_List[bySlotIndex];
	BYTE byFrameCode = m_Param.m_UnitDB.m_List[bySlotIndex].byFrame;
	_UnitPart_fld* pPackFld = (_UnitPart_fld*)g_Main.m_tblUnitPart[unit_part_back].GetRecord(pUnitSlot->byPart[unit_part_back]);	
	_UnitBullet_fld* ppBulletFld[max_unit_spare] = {0, };	
	DWORD dwConsumMoney[MONEY_UNIT_NUM] = {0,};
	float fR = 1.0f + ::eGetTex(race_code_bellato);
	int i;

	//종족이 맞는지..
	if(m_Param.GetRaceSexCode()/2 != race_code_bellato)
	{
		byRetCode = error_unit_not_usable_race;
		goto RESULT;
	}

	//해당 슬롯에 유닛이 있는지..
	if(byFrameCode == 0xFF)
	{
		byRetCode = error_unit_empty_slot;
		goto RESULT;
	}

	//pack이 있는지..
	if(!pPackFld)
	{
		byRetCode = error_unit_no_have_back;
		goto RESULT;
	}

	for(i = 0; i < byFillNum; i++)
	{
		if(pList[i].bySpareIndex >= pPackFld->m_nBackSlt)
		{	//pack spare범위에 맞는지..
			byRetCode = error_unit_no_spare;
			goto RESULT;
		}
		ppBulletFld[pList[i].bySpareIndex] = (_UnitBullet_fld*)g_Main.m_tblUnitBullet.GetRecord(pList[i].byBulletIndex);
		if(!ppBulletFld[pList[i].bySpareIndex])
		{
			byRetCode = error_unit_no_spare;
			goto RESULT;
		}
	}
	
	//운행중인 유닛이 있는지..
	if(m_pUsingUnit)
	{
		byRetCode = error_unit_check_out;
		goto RESULT;
	}

	//돈은 있는지..
	for(i = 0; i < max_unit_spare; i++)
	{
		if(ppBulletFld[i])
			dwConsumMoney[ppBulletFld[i]->m_nMoney] += (ppBulletFld[i]->m_nStdPrice * fR);
	}
	for(i = 0; i < MONEY_UNIT_NUM; i++)
	{
		if(dwConsumMoney[i] > GetMoney(i))
		{
			byRetCode = error_unit_lack_money;
			goto RESULT;
		}	
	}	
	
RESULT:

	if(byRetCode == 0)
	{
		//탄창을 채운다..
		for(i = 0; i < max_unit_spare; i++)
		{
			if(ppBulletFld[i])
			{
				_unit_bullet_param* p = (_unit_bullet_param*)&pUnitSlot->dwSpare[i];

				p->wBulletIndex = ppBulletFld[i]->m_dwIndex;
				p->wLeftNum = ppBulletFld[i]->m_nDurUnit;
			}
		}		

		//돈계산..
		AlterMoney(-dwConsumMoney[money_unit_gold], -dwConsumMoney[money_unit_dalant]);

		//SYNC DB..
		m_pUserDB->Update_UnitData(bySlotIndex, pUnitSlot);

		//ITEM HISTORY..
		if(dwConsumMoney[money_unit_gold] > 0 || dwConsumMoney[money_unit_dalant] > 0)
			s_MgrItemHistory.pay_money("유닛확장팩충전",  dwConsumMoney[money_unit_dalant], dwConsumMoney[money_unit_gold], m_Param.GetGold(), m_Param.GetDalant(), m_szItemHistoryFileName);

		//ECONOMY SET..
		::eAddGold(race_code_bellato, dwConsumMoney[money_unit_gold]);
		::eAddDalant(race_code_bellato, dwConsumMoney[money_unit_dalant]);
	}

	SendMsg_UnitPackFillResult(byRetCode, bySlotIndex, byFillNum, pList, dwConsumMoney);
}

void CPlayer::pc_UnitBulletReplaceRequest(BYTE bySlotIndex, BYTE byPackIndex, BYTE byBulletPart)
{
	BYTE byRetCode = 0;
	_UNIT_DB_BASE::_LIST* pUnitSlot = &m_Param.m_UnitDB.m_List[bySlotIndex];
	_unit_bullet_param* pBulletParam = (_unit_bullet_param*)&pUnitSlot->dwBullet[byBulletPart];
	_unit_bullet_param* pSpareParam = (_unit_bullet_param*)&pUnitSlot->dwSpare[byPackIndex];

	//있는 유닛인지..
	if(pUnitSlot->byFrame == 0xFF)
	{
		byRetCode = error_unit_empty_slot;
		goto RESULT;
	}

	//채워진 백팩인가
	if(pSpareParam->IsFill())
	{
		byRetCode = error_unit_empty_spare;
		goto RESULT;
	}

RESULT:

	if(byRetCode == 0)
	{
		//탄창을 채운다
		memcpy(pBulletParam, pSpareParam, sizeof(_unit_bullet_param));

		//백팩을 지운다
		*(DWORD*)pSpareParam = 0xFFFFFFFF;

		//SYNC DB..
		m_pUserDB->Update_UnitData(bySlotIndex, pUnitSlot);
	}

	SendMsg_UnitBulletReplaceResult(byRetCode);
}

void CPlayer::pc_UnitDeliveryRequest(BYTE bySlotIndex, CItemStore* pStore, float* pfNewPos)
{
	BYTE byRetCode = 0;
	_UNIT_DB_BASE::_LIST* pUnitSlot = &m_Param.m_UnitDB.m_List[bySlotIndex];
	DWORD dwPayDalant = 0;
	CParkingUnit* pEmptyParkingUnit = NULL;
	BYTE byTransDistCode = unit_trans_short_range;

	//종족이 맞는지..
	if(m_Param.GetRaceSexCode()/2 != race_code_bellato)
	{
		byRetCode = error_unit_not_usable_race;
		goto RESULT;
	}

	//있는 슬롯인지..
	if(pUnitSlot->byFrame == 0xFF)
	{
		byRetCode = error_unit_empty_slot;
		goto RESULT;
	}
	
	//운행중인 유닛이 있는지..
	if(m_pUsingUnit)
	{
		byRetCode = error_unit_check_out;
		goto RESULT;
	}

	//상점이 정의되어있으면 근거리 출고..		
	if(pStore)
	{	//유닛상인이 맞는지 확인..
		if(pStore->m_pRec->m_nStore_trade != store_code_unit)
		{
			byRetCode = error_unit_not_unit_store;
			goto RESULT;
		}	
		//유닛상인과 거리비교..
		if(::GetSqrt(m_fCurPos, pStore->GetStorePos()) > STANDARD_VIEW_SIZE)
		{
			byRetCode = error_unit_dist_store;
			goto RESULT;
		}
	}
	else
		byTransDistCode = unit_trans_long_range;

	//미납분에대한 돈은 잇는지..
	dwPayDalant = pUnitSlot->nKeepingFee + pUnitSlot->nPullingFee;
	if(dwPayDalant > m_Param.GetDalant())
	{
		byRetCode = error_unit_lack_non_pay;
		goto RESULT;
	}

	//원거리수송이라면 수송비는 있는지..
	if(byTransDistCode == unit_trans_long_range)
	{
		dwPayDalant += PULLING_FEE;
		if(dwPayDalant > m_Param.GetDalant())
		{
			byRetCode = error_unit_lack_pay;
			goto RESULT;
		}
	}

	//현재위치와 20이내인지..
	if(::GetSqrt(m_fCurPos, pfNewPos) > 40)
	{
		byRetCode = error_unit_leave_newpos;
		goto RESULT;
	}

	//빈 object가져오기..
	pEmptyParkingUnit = ::FindEmptyParkingUnit(g_ParkingUnit, MAX_PARKINGUNIT);
	if(!pEmptyParkingUnit)
	{
		byRetCode = error_unit_full_max_unit;
		goto RESULT;
	}	

RESULT:

	DWORD dwParkingUnitSerial = 0xFFFFFFFF;

	if(byRetCode == 0)
	{
		//유닛생성..
		_parkingunit_create_setdata Data;

		Data.byFrame = pUnitSlot->byFrame;
		memcpy(&Data.byPartCode, pUnitSlot->byPart, sizeof(BYTE)*UNIT_PART_NUM);
		Data.m_pRecordSet = g_Main.m_tblUnitFrame.GetRecord(pUnitSlot->byFrame);
		Data.pOwner = this;
		Data.byCreateType = unit_create_type_delivery;
		Data.m_pMap = m_pCurMap;
		Data.m_nLayerIndex = m_wMapLayerIndex;		
		memcpy(Data.m_fStartPos, pfNewPos, sizeof(float)*3);
		Data.byTransDistCode = byTransDistCode;

		pEmptyParkingUnit->Create(&Data);
		dwParkingUnitSerial = pEmptyParkingUnit->m_dwObjSerial;

		//부스터 충전
		_UnitPart_fld* pBackFld = (_UnitPart_fld*)g_Main.m_tblUnitPart[unit_part_back].GetRecord(pUnitSlot->byPart[unit_part_back]);
		if(pBackFld)
			pUnitSlot->wBooster = pBackFld->m_nBstCha;

		//플레이어에 마킹..
		m_pUsingUnit = pUnitSlot;
		m_pParkingUnit = pEmptyParkingUnit;
		m_dwUnitViewOverTime = 0xFFFFFFFF;

		//돈계산..
		AlterMoney(0, -dwPayDalant);

		//ITEM HISTORY..
		if(dwPayDalant > 0)
			s_MgrItemHistory.pay_money("유닛미납요금징수",  dwPayDalant, 0, m_Param.GetGold(), m_Param.GetDalant(), m_szItemHistoryFileName);
	}

	SendMsg_UnitDeliveryResult(byRetCode, bySlotIndex, dwParkingUnitSerial, dwPayDalant);
}

void CPlayer::pc_UnitReturnRequest()
{
	BYTE byRetCode = 0;
	BYTE byTransDistCode = unit_trans_long_range;
	CItemStore* pStore = NULL;
	int i;

	//종족이 맞는지..
	if(m_Param.GetRaceSexCode()/2 != race_code_bellato)
	{
		byRetCode = error_unit_not_usable_race;
		goto RESULT;
	}

	//정차중인 유닛이 있는지..
	if(!m_pParkingUnit)
	{
		byRetCode = error_unit_check_out;
		goto RESULT;
	}

	//정차중인 유닛이 맞는지..
	if(m_pParkingUnit->m_dwOwnerSerial != m_dwObjSerial)
	{	//나면안돼는 오류..
		byRetCode = error_unit_check_out;
		goto RESULT;
	}

	//유닛과 자신이 유효한 거리를 유지하는지..
	if(::GetSqrt(m_fCurPos, m_pParkingUnit->m_fCurPos) > STANDARD_VIEW_SIZE)
	{
		byRetCode = error_unit_dist_parkingunit;
		goto RESULT;
	}

	//견인인지 직접입고인지 판단..
	for(i = 0; i < m_pCurMap->m_nItemStoreDumNum; i++)
	{
		CItemStore* p = &m_pCurMap->m_ItemStore[i];
		if(p->m_pRec->m_nStore_trade == store_code_unit)
		{
			if(::GetSqrt(m_fCurPos, p->GetStorePos()) < STANDARD_VIEW_SIZE)
			{
				pStore = p;
				break;
			}
		}
	}
	if(pStore)
	{
		byTransDistCode = unit_trans_short_range;
	}

RESULT:

	DWORD dwPayKeep = 0;
	DWORD dwPayPull = 0;

	if(byRetCode == 0)
	{
		//유닛 소멸..
		m_pParkingUnit->Destroy(unit_destory_type_return);

		//보관료 산정 .. 
		dwPayKeep = KEEPING_FEE;
		if(byTransDistCode == unit_trans_long_range)
			dwPayPull = PULLING_FEE;

		if(m_Param.GetDalant() >= dwPayKeep+dwPayPull)
			AlterMoney(0, -(dwPayKeep+dwPayPull));
		else//돈이 모자르면 채무로 쌓는다..
			_UpdateUnitDebt(m_pUsingUnit->bySlotIndex, dwPayKeep, dwPayPull);

		//플레이어에 마킹..
		m_pUsingUnit = NULL;
		m_pParkingUnit = NULL;

		//ITEM HISTORY..
		if(dwPayKeep+dwPayPull > 0)
			s_MgrItemHistory.pay_money("유닛견인요금징수",  dwPayKeep+dwPayPull, 0, m_Param.GetGold(), m_Param.GetDalant(), m_szItemHistoryFileName);
	}

	SendMsg_UnitReturnResult(byRetCode, dwPayKeep+dwPayPull);
}

void CPlayer::pc_UnitTakeRequest()
{
	BYTE byRetCode = 0;

	//종족이 맞는지..
	if(m_Param.GetRaceSexCode()/2 != race_code_bellato)
	{
		byRetCode = error_unit_not_usable_race;
		goto RESULT;
	}

	//정차중인 유닛이 있는지..
	if(!(m_pParkingUnit && m_pUsingUnit))
	{
		byRetCode = error_unit_check_out;
		goto RESULT;
	}

	//정차중인 유닛이 맞는지..
	if(m_pParkingUnit->m_dwOwnerSerial != m_dwObjSerial)
	{	//나면안돼는 오류..
		byRetCode = error_unit_check_out;
		goto RESULT;
	}

	//유닛과 자신이 유효한 거리를 유지하는지..
	if(::GetSqrt(m_fCurPos, m_pParkingUnit->m_fCurPos) > 100)
	{
		byRetCode = error_unit_dist_parkingunit;
		goto RESULT;
	}

RESULT:

	if(byRetCode == 0)
	{
		//플레이어 위치 세팅..
		float fNewPos[3] = {m_pParkingUnit->m_fCurPos[0], m_pParkingUnit->m_fCurPos[1], m_pParkingUnit->m_fCurPos[2]};
		m_pCurMap->GetRandPosInRange(m_pParkingUnit->m_fCurPos, 10, fNewPos);

		memcpy(m_fOldPos, m_fCurPos, sizeof(float)*3);
		memcpy(m_fCurPos, fNewPos, sizeof(float)*3);//세팅좌표 저장..

		//유닛 소멸..
		m_pParkingUnit->Destroy(unit_destory_type_take);

		Emb_RidindUnit(true, NULL);	
	}

	SendMsg_UnitTakeResult(byRetCode);
}

void CPlayer::pc_UnitLeaveRequest(float* pfNewPos)
{
	BYTE byRetCode = 0;
	CParkingUnit* pEmptyParkingUnit = NULL;

	//종족이 맞는지..
	if(m_Param.GetRaceSexCode()/2 != race_code_bellato)
	{
		byRetCode = error_unit_not_usable_race;
		goto RESULT;
	}

	//운행중인지..
	if(!(m_pUsingUnit && !m_pParkingUnit))
	{
		byRetCode = error_unit_not_riding;
		goto RESULT;
	}

	//현재위치와 20이내인지..
	if(::GetSqrt(m_fCurPos, pfNewPos) > 40)
	{
		byRetCode = error_unit_leave_newpos;
		goto RESULT;
	}
	
	//빈 object가져오기..
	pEmptyParkingUnit = ::FindEmptyParkingUnit(g_ParkingUnit, MAX_PARKINGUNIT);
	if(!pEmptyParkingUnit)
	{
		byRetCode = error_unit_full_max_unit;
		goto RESULT;
	}		

RESULT:

	DWORD dwParkingUnitSerial = 0xFFFFFFFF;

	if(byRetCode == 0)
	{
		//유닛생성..
		_parkingunit_create_setdata Data;

		Data.byFrame = m_pUsingUnit->byFrame;
		memcpy(&Data.byPartCode, m_pUsingUnit->byPart, sizeof(BYTE)*UNIT_PART_NUM);
		Data.m_pRecordSet = g_Main.m_tblUnitFrame.GetRecord(m_pUsingUnit->byFrame);
		Data.pOwner = this;
		Data.byCreateType = unit_create_type_leave;
		Data.m_pMap = m_pCurMap;
		Data.m_nLayerIndex = m_wMapLayerIndex;
		memcpy(Data.m_fStartPos, m_fCurPos, sizeof(float)*3);
	
		pEmptyParkingUnit->Create(&Data);

		dwParkingUnitSerial = pEmptyParkingUnit->m_dwObjSerial;

		Emb_RidindUnit(false, pEmptyParkingUnit);	
	}

	SendMsg_UnitLeaveResult(byRetCode);
}

void CPlayer::ForcePullUnit(bool bOneSelf, bool bLogout)
{
	if(!m_pUsingUnit)
		return;

	if(!bLogout)
	{	//로그아웃이 아닌경우.. 반드시 하차상태이어야함..
		if(!m_pParkingUnit)
			return;
	}
	
	if(m_pParkingUnit)
	{	//주차유닛 소멸..
		m_pParkingUnit->Destroy(unit_destory_type_return);
	}

	//보관료 산정
	DWORD dwPayDalant = 0;
	if(bOneSelf)	//자발적 견인
	{
		if(m_Param.GetDalant() >= KEEPING_FEE + PULLING_FEE)
		{
			dwPayDalant = KEEPING_FEE + PULLING_FEE;
			AlterMoney(0, -dwPayDalant);
//			SetMoney(m_Param.GetGold(), m_Param.GetDalant()-dwPayDalant);
		}
		else
		{	//돈이 모자르면 채무로 쌓는다..
			_UpdateUnitDebt(m_pUsingUnit->bySlotIndex, KEEPING_FEE, PULLING_FEE);
		}
	}
	else			//비자발적 견인
	{		//무조건 채무로 쌓는다.
		_UpdateUnitDebt(m_pUsingUnit->bySlotIndex, KEEPING_FEE, PULLING_FEE);
	}

	if(bLogout && !m_bOper)	//비정상 종료인경우..
	{	//cuttime 세팅..
		m_pUsingUnit->dwCutTime = ::GetKorLocalTime();
		if(m_pUserDB)//월드에 업데이트..
			m_pUserDB->Update_UnitData(m_pUsingUnit->bySlotIndex, m_pUsingUnit);
	}

	//플레이어에 마킹..
	m_pUsingUnit = NULL;
	m_pParkingUnit = NULL;

	//ITEM HISTORY..
	if(dwPayDalant > 0)
		s_MgrItemHistory.pay_money("유닛견인요금징수",  dwPayDalant, 0, m_Param.GetGold(), m_Param.GetDalant(), m_szItemHistoryFileName);
}

WORD CPlayer::_DeleteUnitKey(BYTE bySlotIndex)
{
	for(int i = 0; i < m_Param.m_dbInven.m_nUsedNum; i++)
	{
		__ITEM* p = &m_Param.m_dbInven.m_pStorageList[i];
		if(!p->m_bLoad)
			continue;

		if(p->m_dwLv == bySlotIndex)
		{
			Emb_DelStorage(_STORAGE_POS::INVEN, i, false);
			return p->m_wSerial;	
		}
	}
	return 0xFFFF;
}

void CPlayer::_UpdateUnitDebt(BYTE bySlotIndex, DWORD dwKeep, DWORD dwPull)
{
	_UNIT_DB_BASE::_LIST* pUnitSlot = &m_Param.m_UnitDB.m_List[bySlotIndex];

	pUnitSlot->nKeepingFee = dwKeep;
	pUnitSlot->nPullingFee = dwPull;

	SendMsg_UnitAlterFeeInform(bySlotIndex, dwKeep, dwPull);

	//월드에 알림..
	m_pUserDB->Update_UnitData(bySlotIndex, pUnitSlot);
}

void CPlayer::_CheckForcePullUnit()
{
	if(!m_pParkingUnit)
		return;

	DWORD dwCurTime = timeGetTime();
	if(dwCurTime - m_dwLastTimeCheckUnitViewOver > 2000)//2초마다..
	{
		m_dwLastTimeCheckUnitViewOver = dwCurTime;

		const int ViewOverStdTime = 60000;

		if(m_pCurMap == m_pParkingUnit->m_pCurMap && 
			::GetSqrt(m_fCurPos, m_pParkingUnit->m_fCurPos) < STANDARD_VIEW_SIZE &&
			abs(m_fCurPos[1] - m_pParkingUnit->m_fCurPos[1]) < 100)	//y좌표도 100미만..
		{
			m_dwUnitViewOverTime = 0xFFFFFFFF;
		}
		else
		{		
			if(m_dwUnitViewOverTime == 0xFFFFFFFF)
				m_dwUnitViewOverTime = dwCurTime;
			else
			{
				if(dwCurTime - m_dwUnitViewOverTime > ViewOverStdTime)
				{//강제견인..
					ForcePullUnit(false, false);
				}
			}
		}
	}
}

void CPlayer::CheckUnitCutTime()
{
	DWORD dwCurLocalTime = ::GetKorLocalTime();

	for(int i = 0; i < unit_storage_num; i++)
	{
		_UNIT_DB_BASE::_LIST* p = &m_Param.m_UnitDB.m_List[i];

		if(p->byFrame == 0xFF)
			continue;

		if(p->dwCutTime == 0)
			continue;

		if(dwCurLocalTime - p->dwCutTime < 5)	//비정상종료후 5분이내 접속인경우..
		{
			//미납부채 탕감..
			p->nKeepingFee = 0;
			p->nPullingFee = 0;
			
			SendMsg_UnitAlterFeeInform(i, 0, 0);

			//월드에 업데이트..
			m_pUserDB->Update_UnitData(i, p);

			//파킹유닛생성..
			float fNewPos[3] = {m_fCurPos[0], m_fCurPos[1], m_fCurPos[2]};
			m_pCurMap->GetRandPosInRange(m_fCurPos, 20, fNewPos);
			pc_UnitDeliveryRequest(i, NULL, fNewPos);
			break;
		}
	}

	//cuttime 초기화..
	for(i = 0; i < unit_storage_num; i++)
	{
		_UNIT_DB_BASE::_LIST* p = &m_Param.m_UnitDB.m_List[i];

		if(p->byFrame == 0xFF)
			continue;

		if(p->dwCutTime == 0)
			continue;

		p->dwCutTime = 0;
		//월드에 업데이트..
		m_pUserDB->Update_UnitData(i, p);
	}
}

bool CPlayer::IsRidingUnit()
{
	//사용중인 유닛은 있고, 파킹상태가 아니면..
	if(m_pUsingUnit && !m_pParkingUnit)
		return true;
	return false;
}

_UnitKeyItem_fld* gGetUnitKeyMatchFrame(BYTE byFrameCode)
{
	for(int i = 0; i < g_Main.m_tblItemData[tbl_code_key].GetRecordNum(); i++)
	{
		_UnitKeyItem_fld* p = (_UnitKeyItem_fld*)g_Main.m_tblItemData[tbl_code_key].GetRecord(i);
		if(p->m_nFRAType == byFrameCode)
			return p;
	}
	return NULL;
}
