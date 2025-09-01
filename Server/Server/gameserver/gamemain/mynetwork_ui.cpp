// MyNetWork_UI.cpp: implementation of the CNetworkEX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyNetWorking.h"
#include "protocol.h"
#include "pt_zone_client.h"
#include "MainThread.h"

bool CNetworkEX::ForceInvenChangeRequest(int n, char* pBuf)
{
	_force_inven_change_request_clzo* pRecv = (_force_inven_change_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode)
		return true;

	if(pRecv->Item.byStorageCode != _STORAGE_POS::INVEN && pRecv->Item.byStorageCode != _STORAGE_POS::FORCE)
		return false;

	pOne->pc_ForceInvenChange(&pRecv->Item, pRecv->wReplaceSerial);

	return true;
}

bool CNetworkEX::AnimusInvenChangeRequest(int n, char* pBuf)
{
	_animus_inven_change_request_clzo* pRecv = (_animus_inven_change_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode)
		return true;

	if(pRecv->Item.byStorageCode != _STORAGE_POS::INVEN && pRecv->Item.byStorageCode != _STORAGE_POS::ANIMUS)
		return false;

	pOne->pc_AnimusInvenChange(&pRecv->Item, pRecv->wReplaceSerial);

	return true;

}

bool CNetworkEX::ResSeparationRequest(int n, char* pBuf)
{
	_res_separation_request_clzo* pRecv = (_res_separation_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode)
		return true;

	if(pRecv->byMoveAmount >= max_overlap_num)
	{
		m_LogFile.Write("odd.. %s: ResSeparationRequest() : pRecv->byMoveAmount(%d) >= max_overlap_num",
			pOne->m_Param.GetCharName(), pRecv->byMoveAmount);				
		return false;	
	}

	pOne->pc_ResSeparation(pRecv->wStartSerial, pRecv->byMoveAmount);
	return true;
}

bool CNetworkEX::ResDivisionRequest(int n, char* pBuf)
{
	_res_division_request_clzo* pRecv = (_res_division_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode)
		return true;

	if(pRecv->byMoveAmount >= max_overlap_num)
		return false;

	pOne->pc_ResDivision(pRecv->wStartSerial, pRecv->wTarSerial, pRecv->byMoveAmount);
	return true;
}

bool CNetworkEX::PotionSocketChangeRequest(int n, char* pBuf)
{
	_potionsocket_change_request_clzo* pRecv = (_potionsocket_change_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode)
		return true;

	if(pRecv->Item.byStorageCode != _STORAGE_POS::INVEN && pRecv->Item.byStorageCode != _STORAGE_POS::BELT)
		return false;

	if(pRecv->TarPos.byStorageCode != _STORAGE_POS::INVEN && pRecv->TarPos.byStorageCode != _STORAGE_POS::BELT)
		return false;

	if(pRecv->Item.byStorageCode == pRecv->TarPos.byStorageCode)
		return false;

	pOne->pc_PotionSocketChange(&pRecv->Item, &pRecv->TarPos);

	return true;
}

bool CNetworkEX::PotionSocketSeparationRequest(int n, char* pBuf)
{
	_potionsocket_separation_request_clzo* pRecv = (_potionsocket_separation_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode)
		return true;

	if(pRecv->StartPos.byPositon != _STORAGE_POS::INVEN && pRecv->StartPos.byPositon != _STORAGE_POS::BELT)
		return false;
	
	if(pRecv->TargetPos.byPositon != _STORAGE_POS::INVEN && pRecv->TargetPos.byPositon != _STORAGE_POS::BELT)
		return false;

	if(pRecv->byMoveAmount > 99)
		return false;

	pOne->pc_PotionSeparation(pRecv->StartPos.byPositon, pRecv->TargetPos.byPositon, pRecv->wStartSerial, pRecv->byMoveAmount);

	return true;
}

bool CNetworkEX::PotionSocketDivisionRequest(int n, char* pBuf)
{
	_potionsocket_division_request_clzo* pRecv = (_potionsocket_division_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];
	
	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode)
		return true;

	if(pRecv->StartPos.byPositon !=_STORAGE_POS::INVEN && pRecv->StartPos.byPositon != _STORAGE_POS::BELT)
		return false;

	if(pRecv->TargetPos.byPositon !=_STORAGE_POS::INVEN && pRecv->TargetPos.byPositon != _STORAGE_POS::BELT)
		return false;	

	if(pRecv->byMoveAmount > 99)
		return false;

	pOne->pc_PotionDivision(pRecv->StartPos.byPositon, pRecv->TargetPos.byPositon, pRecv->wStartSerial, pRecv->wTarSerial, pRecv->byMoveAmount);

	return true;
}




