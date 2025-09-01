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

//���ֱ���
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

	//�����ϴ� ����������..
	if(!pFrameFld)
	{
		byRetCode = error_unit_not_exist_frame;
		goto RESULT;
	}

	//������ �´���..
	if(m_Param.GetRaceSexCode()/2 != race_code_bellato)
	{
		byRetCode = error_unit_not_usable_race;
		goto RESULT;
	}

	//�������� ������ �ִ���..
	if(m_pUsingUnit)
	{
		byRetCode = error_unit_check_out;
		goto RESULT;
	}

	//�� ������ �ִ���..
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

	//�� �κ��� �ִ���..(Ű�߰�)
	if(m_Param.m_dbInven.GetNumEmptyCon() == 0)
	{
		byRetCode = error_unit_full_inven;
		goto RESULT;
	}

	//Ű����ã��
	pKeyFld = ::gGetUnitKeyMatchFrame(byFrameCode);
	if(!pKeyFld)
	{
		byRetCode = error_unit_not_match_frame;
		goto RESULT;
	}

	//���� �ִ���..
	dwConsumMoney[pFrameFld->m_nMoney] = pFrameFld->m_nStdPrice * fR;
	if(dwConsumMoney[pFrameFld->m_nMoney] > GetMoney(pFrameFld->m_nMoney))
	{
		byRetCode = error_unit_lack_money;
		goto RESULT;
	}

	//�⺻��Ʈ����üũ
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

		//����������
		pEmptySlot->byFrame = byFrameCode;

		//�⺻��Ʈ����
		for(i = 0; i < UNIT_PART_NUM; i++)
			pEmptySlot->byPart[i] = byBasePart[i];

		//����������
		pEmptySlot->dwGauge = pFrameFld->m_nUnit_HP;
		
		//���忡 ����..	
		m_pUserDB->Update_UnitInsert(byUnitSlotIndex, pEmptySlot);
		
		//Ű ������ �߰�..
		wKeySerial = m_Param.GetNewItemSerial();
		wKeyIndex = pKeyFld->m_dwIndex;

		_STORAGE_LIST::_storage_con KeyItem (tbl_code_key, pKeyFld->m_dwIndex, 0, byUnitSlotIndex, wKeySerial);
		Emb_AddStorage(_STORAGE_POS::INVEN, &KeyItem);

		//�����..
		AlterMoney(-dwConsumMoney[money_unit_gold], -dwConsumMoney[money_unit_dalant]);

		//ITEM HISTORY..
		s_MgrItemHistory.buy_unit(byUnitSlotIndex, pEmptySlot, dwConsumMoney, m_Param.GetDalant(), m_Param.GetGold(), m_szItemHistoryFileName);

		//ECONOMY SET..
		::eAddGold(race_code_bellato, dwConsumMoney[money_unit_gold]);
		::eAddDalant(race_code_bellato, dwConsumMoney[money_unit_dalant]);
	}

	SendMsg_UnitFrameBuyResult(byRetCode, byFrameCode, byUnitSlotIndex, wKeyIndex, wKeySerial, dwConsumMoney);
}

//����ó��
void CPlayer::pc_UnitSellRequest(BYTE bySlotIndex)
{
	BYTE byRetCode = 0;
	BYTE byFrameCode = m_Param.m_UnitDB.m_List[bySlotIndex].byFrame;
	_UnitFrame_fld* pFrameFld = (_UnitFrame_fld*)g_Main.m_tblUnitFrame.GetRecord(byFrameCode);
	_UNIT_DB_BASE::_LIST* pUnitSlot = &m_Param.m_UnitDB.m_List[bySlotIndex];
	DWORD dwTotalNonpay = 0;

	//�����ϴ� ����������..
	if(!pFrameFld)
	{
		byRetCode = error_unit_not_exist_frame;
		goto RESULT;
	}

	//������ �´���..
	if(m_Param.GetRaceSexCode()/2 != race_code_bellato)
	{
		byRetCode = error_unit_not_usable_race;
		goto RESULT;
	}
	
	//�������� ������ �ִ���..
	if(m_pUsingUnit)
	{
		byRetCode = error_unit_check_out;
		goto RESULT;
	}

	//�ش� ���Կ� ������ �ִ���..
	if(byFrameCode == 0xFF)
	{
		byRetCode = error_unit_empty_slot;
		goto RESULT;
	}

	//�̳������ �����޶�Ʈ���� ū�� �˻�..
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

		//������ ����..
		pUnitSlot->byFrame = 0xFF;

		//Ű�� ����..
		wKeySerial = _DeleteUnitKey(bySlotIndex);

		//���� ���..
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

	//������ �´���..
	if(m_Param.GetRaceSexCode()/2 != race_code_bellato)
	{
		byRetCode = error_unit_not_usable_race;
		goto RESULT;
	}
	
	//�������� ������ �ִ���..
	if(m_pUsingUnit)
	{
		byRetCode = error_unit_check_out;
		goto RESULT;
	}

	//�ش� ���Կ� ������ �ִ���..
	if(byFrameCode == 0xFF)
	{
		byRetCode = error_unit_empty_slot;
		goto RESULT;
	}

	//�������� �´���..
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

		//����üũ..
		if(pFld->m_nLevelLim > m_Param.GetLevel())
		{
			byRetCode = error_unit_tuning_lv;
			goto RESULT;
		}

		//Ʃ�װ����� �����͸�����..
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

		//�����ջ�
		nTotalCost[pFld->m_nMoney] += (pFld->m_nStdPrice * fBuyR);

		//�����̶�� ����ִ°� ���ݿ��� ���ش�..
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

	//���� �ִ���..
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
		//��Ʈ�� ���Ƴ����..
		for(i = 0; i < byTuningNum; i++)
		{
			pUnitSlot->byPart[pTuningData[i].byPartCode] = pTuningData[i].byPartIndex;

			//�����̶�� sqare�� ����..
			if(pTuningData[i].byPartCode == unit_part_back)
				memset(pUnitSlot->dwSpare, 0xFFFFFFFF, sizeof(DWORD)*max_unit_spare);
		}

		//�����..		
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

	//������ �´���..
	if(m_Param.GetRaceSexCode()/2 != race_code_bellato)
	{
		byRetCode = error_unit_not_usable_race;
		goto RESULT;
	}
	
	//�������� ������ �ִ���..
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

	//�ش� ���Կ� ������ �ִ���..
	if(byFrameCode == 0xFF)
	{
		byRetCode = error_unit_empty_slot;
		goto RESULT;
	}

	//�����Ҽ��ִ� ��������..
	dwNewGauge = pFrameFld->m_nUnit_HP;
	if(pUnitSlot->dwGauge >= dwNewGauge)
	{
		byRetCode = error_unit_full_gauge;
		goto RESULT;
	}

	//���� �ִ���..
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
		//�������� ä���..
		pUnitSlot->dwGauge = dwNewGauge;

		//�����..
		AlterMoney(0, -dwConsumDalant);

		//SYNC DB..
		m_pUserDB->Update_UnitData(bySlotIndex, pUnitSlot);

		//ITEM HISTORY..
		if(dwConsumDalant > 0)
			s_MgrItemHistory.pay_money("���ּ���",  dwConsumDalant, 0, m_Param.GetGold(), m_Param.GetDalant(), m_szItemHistoryFileName);

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

	//������ �´���..
	if(m_Param.GetRaceSexCode()/2 != race_code_bellato)
	{
		byRetCode = error_unit_not_usable_race;
		goto RESULT;
	}
	
	//�������� ������ �ִ���..
	if(m_pUsingUnit)
	{
		byRetCode = error_unit_check_out;
		goto RESULT;
	}

	//�ش� ���Կ� ������ �ִ���..
	if(byFrameCode == 0xFF)
	{
		byRetCode = error_unit_empty_slot;
		goto RESULT;
	}

	//���� �ִ���..
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
		//źâ�� ä���..
		for(i = 0; i < UNIT_BULLET_NUM; i++)
		{
			if(ppBulletFld[i])
			{
				_unit_bullet_param* p = (_unit_bullet_param*)&pUnitSlot->dwBullet[i];

				p->wBulletIndex = pbyBulletIndex[i];
				p->wLeftNum = ppBulletFld[i]->m_nDurUnit;
			}
		}

		//�����..
		AlterMoney(-dwConsumMoney[money_unit_gold], -dwConsumMoney[money_unit_dalant]);

		//SYNC DB..
		m_pUserDB->Update_UnitData(bySlotIndex, pUnitSlot);

		//ITEM HISTORY..
		if(dwConsumMoney[money_unit_dalant] > 0 || dwConsumMoney[money_unit_gold] > 0)
			s_MgrItemHistory.pay_money("����źâ����",  dwConsumMoney[money_unit_dalant], dwConsumMoney[money_unit_gold], m_Param.GetGold(), m_Param.GetDalant(), m_szItemHistoryFileName);

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

	//������ �´���..
	if(m_Param.GetRaceSexCode()/2 != race_code_bellato)
	{
		byRetCode = error_unit_not_usable_race;
		goto RESULT;
	}

	//�ش� ���Կ� ������ �ִ���..
	if(byFrameCode == 0xFF)
	{
		byRetCode = error_unit_empty_slot;
		goto RESULT;
	}

	//pack�� �ִ���..
	if(!pPackFld)
	{
		byRetCode = error_unit_no_have_back;
		goto RESULT;
	}

	for(i = 0; i < byFillNum; i++)
	{
		if(pList[i].bySpareIndex >= pPackFld->m_nBackSlt)
		{	//pack spare������ �´���..
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
	
	//�������� ������ �ִ���..
	if(m_pUsingUnit)
	{
		byRetCode = error_unit_check_out;
		goto RESULT;
	}

	//���� �ִ���..
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
		//źâ�� ä���..
		for(i = 0; i < max_unit_spare; i++)
		{
			if(ppBulletFld[i])
			{
				_unit_bullet_param* p = (_unit_bullet_param*)&pUnitSlot->dwSpare[i];

				p->wBulletIndex = ppBulletFld[i]->m_dwIndex;
				p->wLeftNum = ppBulletFld[i]->m_nDurUnit;
			}
		}		

		//�����..
		AlterMoney(-dwConsumMoney[money_unit_gold], -dwConsumMoney[money_unit_dalant]);

		//SYNC DB..
		m_pUserDB->Update_UnitData(bySlotIndex, pUnitSlot);

		//ITEM HISTORY..
		if(dwConsumMoney[money_unit_gold] > 0 || dwConsumMoney[money_unit_dalant] > 0)
			s_MgrItemHistory.pay_money("����Ȯ��������",  dwConsumMoney[money_unit_dalant], dwConsumMoney[money_unit_gold], m_Param.GetGold(), m_Param.GetDalant(), m_szItemHistoryFileName);

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

	//�ִ� ��������..
	if(pUnitSlot->byFrame == 0xFF)
	{
		byRetCode = error_unit_empty_slot;
		goto RESULT;
	}

	//ä���� �����ΰ�
	if(pSpareParam->IsFill())
	{
		byRetCode = error_unit_empty_spare;
		goto RESULT;
	}

RESULT:

	if(byRetCode == 0)
	{
		//źâ�� ä���
		memcpy(pBulletParam, pSpareParam, sizeof(_unit_bullet_param));

		//������ �����
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

	//������ �´���..
	if(m_Param.GetRaceSexCode()/2 != race_code_bellato)
	{
		byRetCode = error_unit_not_usable_race;
		goto RESULT;
	}

	//�ִ� ��������..
	if(pUnitSlot->byFrame == 0xFF)
	{
		byRetCode = error_unit_empty_slot;
		goto RESULT;
	}
	
	//�������� ������ �ִ���..
	if(m_pUsingUnit)
	{
		byRetCode = error_unit_check_out;
		goto RESULT;
	}

	//������ ���ǵǾ������� �ٰŸ� ���..		
	if(pStore)
	{	//���ֻ����� �´��� Ȯ��..
		if(pStore->m_pRec->m_nStore_trade != store_code_unit)
		{
			byRetCode = error_unit_not_unit_store;
			goto RESULT;
		}	
		//���ֻ��ΰ� �Ÿ���..
		if(::GetSqrt(m_fCurPos, pStore->GetStorePos()) > STANDARD_VIEW_SIZE)
		{
			byRetCode = error_unit_dist_store;
			goto RESULT;
		}
	}
	else
		byTransDistCode = unit_trans_long_range;

	//�̳��п����� ���� �մ���..
	dwPayDalant = pUnitSlot->nKeepingFee + pUnitSlot->nPullingFee;
	if(dwPayDalant > m_Param.GetDalant())
	{
		byRetCode = error_unit_lack_non_pay;
		goto RESULT;
	}

	//���Ÿ������̶�� ���ۺ�� �ִ���..
	if(byTransDistCode == unit_trans_long_range)
	{
		dwPayDalant += PULLING_FEE;
		if(dwPayDalant > m_Param.GetDalant())
		{
			byRetCode = error_unit_lack_pay;
			goto RESULT;
		}
	}

	//������ġ�� 20�̳�����..
	if(::GetSqrt(m_fCurPos, pfNewPos) > 40)
	{
		byRetCode = error_unit_leave_newpos;
		goto RESULT;
	}

	//�� object��������..
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
		//���ֻ���..
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

		//�ν��� ����
		_UnitPart_fld* pBackFld = (_UnitPart_fld*)g_Main.m_tblUnitPart[unit_part_back].GetRecord(pUnitSlot->byPart[unit_part_back]);
		if(pBackFld)
			pUnitSlot->wBooster = pBackFld->m_nBstCha;

		//�÷��̾ ��ŷ..
		m_pUsingUnit = pUnitSlot;
		m_pParkingUnit = pEmptyParkingUnit;
		m_dwUnitViewOverTime = 0xFFFFFFFF;

		//�����..
		AlterMoney(0, -dwPayDalant);

		//ITEM HISTORY..
		if(dwPayDalant > 0)
			s_MgrItemHistory.pay_money("���ֹ̳����¡��",  dwPayDalant, 0, m_Param.GetGold(), m_Param.GetDalant(), m_szItemHistoryFileName);
	}

	SendMsg_UnitDeliveryResult(byRetCode, bySlotIndex, dwParkingUnitSerial, dwPayDalant);
}

void CPlayer::pc_UnitReturnRequest()
{
	BYTE byRetCode = 0;
	BYTE byTransDistCode = unit_trans_long_range;
	CItemStore* pStore = NULL;
	int i;

	//������ �´���..
	if(m_Param.GetRaceSexCode()/2 != race_code_bellato)
	{
		byRetCode = error_unit_not_usable_race;
		goto RESULT;
	}

	//�������� ������ �ִ���..
	if(!m_pParkingUnit)
	{
		byRetCode = error_unit_check_out;
		goto RESULT;
	}

	//�������� ������ �´���..
	if(m_pParkingUnit->m_dwOwnerSerial != m_dwObjSerial)
	{	//����ȵŴ� ����..
		byRetCode = error_unit_check_out;
		goto RESULT;
	}

	//���ְ� �ڽ��� ��ȿ�� �Ÿ��� �����ϴ���..
	if(::GetSqrt(m_fCurPos, m_pParkingUnit->m_fCurPos) > STANDARD_VIEW_SIZE)
	{
		byRetCode = error_unit_dist_parkingunit;
		goto RESULT;
	}

	//�������� �����԰����� �Ǵ�..
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
		//���� �Ҹ�..
		m_pParkingUnit->Destroy(unit_destory_type_return);

		//������ ���� .. 
		dwPayKeep = KEEPING_FEE;
		if(byTransDistCode == unit_trans_long_range)
			dwPayPull = PULLING_FEE;

		if(m_Param.GetDalant() >= dwPayKeep+dwPayPull)
			AlterMoney(0, -(dwPayKeep+dwPayPull));
		else//���� ���ڸ��� ä���� �״´�..
			_UpdateUnitDebt(m_pUsingUnit->bySlotIndex, dwPayKeep, dwPayPull);

		//�÷��̾ ��ŷ..
		m_pUsingUnit = NULL;
		m_pParkingUnit = NULL;

		//ITEM HISTORY..
		if(dwPayKeep+dwPayPull > 0)
			s_MgrItemHistory.pay_money("���ְ��ο��¡��",  dwPayKeep+dwPayPull, 0, m_Param.GetGold(), m_Param.GetDalant(), m_szItemHistoryFileName);
	}

	SendMsg_UnitReturnResult(byRetCode, dwPayKeep+dwPayPull);
}

void CPlayer::pc_UnitTakeRequest()
{
	BYTE byRetCode = 0;

	//������ �´���..
	if(m_Param.GetRaceSexCode()/2 != race_code_bellato)
	{
		byRetCode = error_unit_not_usable_race;
		goto RESULT;
	}

	//�������� ������ �ִ���..
	if(!(m_pParkingUnit && m_pUsingUnit))
	{
		byRetCode = error_unit_check_out;
		goto RESULT;
	}

	//�������� ������ �´���..
	if(m_pParkingUnit->m_dwOwnerSerial != m_dwObjSerial)
	{	//����ȵŴ� ����..
		byRetCode = error_unit_check_out;
		goto RESULT;
	}

	//���ְ� �ڽ��� ��ȿ�� �Ÿ��� �����ϴ���..
	if(::GetSqrt(m_fCurPos, m_pParkingUnit->m_fCurPos) > 100)
	{
		byRetCode = error_unit_dist_parkingunit;
		goto RESULT;
	}

RESULT:

	if(byRetCode == 0)
	{
		//�÷��̾� ��ġ ����..
		float fNewPos[3] = {m_pParkingUnit->m_fCurPos[0], m_pParkingUnit->m_fCurPos[1], m_pParkingUnit->m_fCurPos[2]};
		m_pCurMap->GetRandPosInRange(m_pParkingUnit->m_fCurPos, 10, fNewPos);

		memcpy(m_fOldPos, m_fCurPos, sizeof(float)*3);
		memcpy(m_fCurPos, fNewPos, sizeof(float)*3);//������ǥ ����..

		//���� �Ҹ�..
		m_pParkingUnit->Destroy(unit_destory_type_take);

		Emb_RidindUnit(true, NULL);	
	}

	SendMsg_UnitTakeResult(byRetCode);
}

void CPlayer::pc_UnitLeaveRequest(float* pfNewPos)
{
	BYTE byRetCode = 0;
	CParkingUnit* pEmptyParkingUnit = NULL;

	//������ �´���..
	if(m_Param.GetRaceSexCode()/2 != race_code_bellato)
	{
		byRetCode = error_unit_not_usable_race;
		goto RESULT;
	}

	//����������..
	if(!(m_pUsingUnit && !m_pParkingUnit))
	{
		byRetCode = error_unit_not_riding;
		goto RESULT;
	}

	//������ġ�� 20�̳�����..
	if(::GetSqrt(m_fCurPos, pfNewPos) > 40)
	{
		byRetCode = error_unit_leave_newpos;
		goto RESULT;
	}
	
	//�� object��������..
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
		//���ֻ���..
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
	{	//�α׾ƿ��� �ƴѰ��.. �ݵ�� ���������̾����..
		if(!m_pParkingUnit)
			return;
	}
	
	if(m_pParkingUnit)
	{	//�������� �Ҹ�..
		m_pParkingUnit->Destroy(unit_destory_type_return);
	}

	//������ ����
	DWORD dwPayDalant = 0;
	if(bOneSelf)	//�ڹ��� ����
	{
		if(m_Param.GetDalant() >= KEEPING_FEE + PULLING_FEE)
		{
			dwPayDalant = KEEPING_FEE + PULLING_FEE;
			AlterMoney(0, -dwPayDalant);
//			SetMoney(m_Param.GetGold(), m_Param.GetDalant()-dwPayDalant);
		}
		else
		{	//���� ���ڸ��� ä���� �״´�..
			_UpdateUnitDebt(m_pUsingUnit->bySlotIndex, KEEPING_FEE, PULLING_FEE);
		}
	}
	else			//���ڹ��� ����
	{		//������ ä���� �״´�.
		_UpdateUnitDebt(m_pUsingUnit->bySlotIndex, KEEPING_FEE, PULLING_FEE);
	}

	if(bLogout && !m_bOper)	//������ �����ΰ��..
	{	//cuttime ����..
		m_pUsingUnit->dwCutTime = ::GetKorLocalTime();
		if(m_pUserDB)//���忡 ������Ʈ..
			m_pUserDB->Update_UnitData(m_pUsingUnit->bySlotIndex, m_pUsingUnit);
	}

	//�÷��̾ ��ŷ..
	m_pUsingUnit = NULL;
	m_pParkingUnit = NULL;

	//ITEM HISTORY..
	if(dwPayDalant > 0)
		s_MgrItemHistory.pay_money("���ְ��ο��¡��",  dwPayDalant, 0, m_Param.GetGold(), m_Param.GetDalant(), m_szItemHistoryFileName);
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

	//���忡 �˸�..
	m_pUserDB->Update_UnitData(bySlotIndex, pUnitSlot);
}

void CPlayer::_CheckForcePullUnit()
{
	if(!m_pParkingUnit)
		return;

	DWORD dwCurTime = timeGetTime();
	if(dwCurTime - m_dwLastTimeCheckUnitViewOver > 2000)//2�ʸ���..
	{
		m_dwLastTimeCheckUnitViewOver = dwCurTime;

		const int ViewOverStdTime = 60000;

		if(m_pCurMap == m_pParkingUnit->m_pCurMap && 
			::GetSqrt(m_fCurPos, m_pParkingUnit->m_fCurPos) < STANDARD_VIEW_SIZE &&
			abs(m_fCurPos[1] - m_pParkingUnit->m_fCurPos[1]) < 100)	//y��ǥ�� 100�̸�..
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
				{//��������..
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

		if(dwCurLocalTime - p->dwCutTime < 5)	//������������ 5���̳� �����ΰ��..
		{
			//�̳���ä ����..
			p->nKeepingFee = 0;
			p->nPullingFee = 0;
			
			SendMsg_UnitAlterFeeInform(i, 0, 0);

			//���忡 ������Ʈ..
			m_pUserDB->Update_UnitData(i, p);

			//��ŷ���ֻ���..
			float fNewPos[3] = {m_fCurPos[0], m_fCurPos[1], m_fCurPos[2]};
			m_pCurMap->GetRandPosInRange(m_fCurPos, 20, fNewPos);
			pc_UnitDeliveryRequest(i, NULL, fNewPos);
			break;
		}
	}

	//cuttime �ʱ�ȭ..
	for(i = 0; i < unit_storage_num; i++)
	{
		_UNIT_DB_BASE::_LIST* p = &m_Param.m_UnitDB.m_List[i];

		if(p->byFrame == 0xFF)
			continue;

		if(p->dwCutTime == 0)
			continue;

		p->dwCutTime = 0;
		//���忡 ������Ʈ..
		m_pUserDB->Update_UnitData(i, p);
	}
}

bool CPlayer::IsRidingUnit()
{
	//������� ������ �ְ�, ��ŷ���°� �ƴϸ�..
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
