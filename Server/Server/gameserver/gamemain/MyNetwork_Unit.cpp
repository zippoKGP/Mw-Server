// MyNetWork_Unit.cpp: implementation of the CNetworkEX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyNetWorking.h"
#include "protocol.h"
#include "pt_zone_client.h"
#include "MainThread.h"

bool CNetworkEX::UnitFrameBuyRequest(int n, char* pBuf)
{
	_unit_frame_buy_request_clzo* pRecv = (_unit_frame_buy_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse)
		return true;

	if(pRecv->byFrameCode >= UNIT_FRAME_CODE_NUM)
		return false;

	pOne->pc_UnitFrameBuyRequest(pRecv->byFrameCode);

	return true;
}

bool CNetworkEX::UnitSellRequest(int n, char* pBuf)
{
	_unit_sell_request_clzo* pRecv = (_unit_sell_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse)
		return true;

	if(pRecv->bySlotIndex >= unit_storage_num)
		return false;	

	pOne->pc_UnitSellRequest(pRecv->bySlotIndex);

	return true;
}

bool CNetworkEX::UnitPartTuningRequest(int n, char* pBuf)
{
	_unit_part_tuning_request_clzo* pRecv = (_unit_part_tuning_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse)
		return true;

	if(pRecv->bySlotIndex >= unit_storage_num)
		return false;	

	if(pRecv->byTuningNum > UNIT_PART_NUM)
		return false;

	for(int i = 0; i < pRecv->byTuningNum; i++)
	{
		if(pRecv->TuningList[i].byPartCode >= UNIT_PART_NUM)
			return false;
	}

	pOne->pc_UnitPartTuningRequest(pRecv->bySlotIndex, pRecv->byTuningNum, pRecv->TuningList);

	return true;
}

bool CNetworkEX::UnitFrameRepairRequest(int n, char* pBuf)
{
	_unit_frame_repair_request_clzo* pRecv = (_unit_frame_repair_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse)
		return true;

	if(pRecv->bySlotIndex >= unit_storage_num)
		return false;	

	pOne->pc_UnitFrameRepairRequest(pRecv->bySlotIndex);

	return true;
}

bool CNetworkEX::UnitBulletFillRequest(int n, char* pBuf)
{
	_unit_bullet_fill_request_clzo* pRecv = (_unit_bullet_fill_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse)
		return true;

	if(pRecv->bySlotIndex >= unit_storage_num)
		return false;	

	BYTE byFill = 0x00;
	for(int i = 0; i < UNIT_BULLET_NUM; i++)
	{
		if(pRecv->byBulletIndex[i] != 0xFF)
		{
			if(!g_Main.m_tblUnitBullet.GetRecord(pRecv->byBulletIndex[i]))
				return false;	
			byFill |= (0x01 << i);
		}
	}
	if(!byFill)
	{	//한개도 안산경우..
		return false;
	}
	
	pOne->pc_UnitBulletFillRequest(pRecv->bySlotIndex, pRecv->byBulletIndex);

	return true;
}

bool CNetworkEX::UnitPackFillRequest(int n, char* pBuf)
{
	_unit_pack_fill_request_clzo* pRecv = (_unit_pack_fill_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse)
		return true;

	if(pRecv->bySlotIndex >= unit_storage_num)
		return false;	

	if(pRecv->byFillNum > max_unit_spare)
		return false;

	bool bSpare[max_unit_spare] = {0, };
	for(int i = 0; i < pRecv->byFillNum; i++)
	{
		if(pRecv->List[i].bySpareIndex >= max_unit_spare)
			return false;
		
		if(!g_Main.m_tblUnitBullet.GetRecord(pRecv->List[i].byBulletIndex))
			return false;	

		if(bSpare[pRecv->List[i].bySpareIndex])
			return false;	//같은 공간에 두번이상 채웠는지..
		else
			bSpare[pRecv->List[i].bySpareIndex] = true;
	}

	pOne->pc_UnitPackFillRequest(pRecv->bySlotIndex, pRecv->byFillNum, pRecv->List);
	
	return true;
}

bool CNetworkEX::UnitDeliveryRequest(int n, char* pBuf)
{
	_unit_delivery_request_clzo* pRecv = (_unit_delivery_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	if(pRecv->bySlotIndex >= unit_storage_num)
		return false;

	CItemStore* pStore = NULL;

	if(pRecv->byStoreIndex != 0xFF)
	{
		for(int i = 0; i < pOne->m_pCurMap->m_nItemStoreDumNum; i++)
		{
			if(pOne->m_pCurMap->m_ItemStore[i].m_pRec->m_dwIndex == pRecv->byStoreIndex)
			{
				pStore = &pOne->m_pCurMap->m_ItemStore[i];
				break;
			}
		}
		if(!pStore)
			return false;
	}
	
	float fNewPos[3];
	::ShortToFloat(pRecv->zUnitNewPos, fNewPos, 3);

	pOne->pc_UnitDeliveryRequest(pRecv->bySlotIndex, pStore, fNewPos);

	return true;
}

bool CNetworkEX::UnitReturnRequest(int n, char* pBuf)
{
	_unit_return_request_clzo* pRecv = (_unit_return_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	pOne->pc_UnitReturnRequest();

	return true;
}

bool CNetworkEX::UnitTakeRequest(int n, char* pBuf)
{
	_unit_take_request_clzo* pRecv = (_unit_take_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	pOne->pc_UnitTakeRequest();

	return true;
}

bool CNetworkEX::UnitLeaveRequest(int n, char* pBuf)
{
	_unit_leave_request_clzo* pRecv = (_unit_leave_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	float fNewPos[3];
	::ShortToFloat(pRecv->zPlayerNewPos, fNewPos, 3);

	pOne->pc_UnitLeaveRequest(fNewPos);

	return true;
}

bool CNetworkEX::UnitBulletReplaceRequest(int n, char* pBuf)
{
	_unit_bullet_replace_request_clzo* pRecv = (_unit_bullet_replace_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	if(pRecv->bySlotIndex >= unit_storage_num)
		return false;

	if(pRecv->byPackIndex >= max_unit_spare)
		return false;

	if(pRecv->byBulletPart >= UNIT_BULLET_NUM)
		return false;

	pOne->pc_UnitBulletReplaceRequest(pRecv->bySlotIndex, pRecv->byPackIndex, pRecv->byBulletPart);

	return true;
}

