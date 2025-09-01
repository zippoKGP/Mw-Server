// MyNetWork_Resource.cpp: implementation of the CNetworkEX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyNetWorking.h"
#include "protocol.h"
#include "pt_zone_client.h"
#include "Player.h"
#include "MainThread.h"

bool CNetworkEX::MineStartRequest(int n, char* pBuf)
{
	_mine_start_request_clzo* pRecv = (_mine_start_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	if(pRecv->byMineIndex >= pOne->m_pCurMap->m_nResDumNum)
	{
		m_LogFile.Write("odd.. %s: MineStartRequest() : pRecv->byMineIndex(%d) >= pOne->m_pCurMap->m_nResDumNum(%d)", 
			pOne->m_Param.GetCharName(), pRecv->byMineIndex, pOne->m_pCurMap->m_nResDumNum);
		return false;
	}

	pOne->pc_MineStart(pRecv->byMineIndex);

	return true;
}

bool CNetworkEX::MineCancleRequest(int n, char* pBuf)
{
	_mine_cancle_request_clzo* pRecv = (_mine_cancle_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];
	
	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse)
		return true;

	pOne->pc_MineCancle(pRecv->wBatterySerial);

	return true;
}

bool CNetworkEX::MineCompleteRequest(int n, char* pBuf)
{
	_mine_complete_request_clzo* pRecv = (_mine_complete_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];
	
	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	if(pRecv->byOreIndex >= g_Main.m_tblItemData[tbl_code_ore].GetRecordNum())
		return false;

	pOne->pc_MineComplete(pRecv->byOreIndex, pRecv->wBatterySerial);

	return true;
}

bool CNetworkEX::ResCuttingRequest(int n, char* pBuf)
{
	_ore_cutting_request_clzo* pRecv = (_ore_cutting_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse)
		return true;

	if(pRecv->byProcessNum > max_overlap_num)
		return false;

	pOne->pc_OreCutting(pRecv->wOreSerial, pRecv->byProcessNum);

	return true;
}

bool CNetworkEX::OreIntoBagRequest(int n, char* pBuf)
{
	_ore_into_bag_request_clzo* pRecv = (_ore_into_bag_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse)
		return true;

	if(pRecv->byAddAmount > max_overlap_num)
		return false;

	if(pRecv->wResIndex >= g_Main.m_tblItemData[tbl_code_res].GetRecordNum())
		return false;

	pOne->pc_OreIntoBag(pRecv->wResIndex, pRecv->wSerial, pRecv->byAddAmount);

	return true;
}

bool CNetworkEX::CuttingCompleteRequest(int n, char* pBuf)
{
	_cutting_complete_request_clzo* pRecv = (_cutting_complete_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse)
		return true;

	if(pRecv->byNpcRace > 3)
		return false;

	pOne->pc_CuttingComplete(pRecv->byNpcRace);

	return true;
}
