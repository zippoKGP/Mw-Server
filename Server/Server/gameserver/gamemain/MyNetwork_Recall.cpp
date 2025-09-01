// MyNetWork_Recall.cpp: implementation of the CNetworkEX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyNetWorking.h"
#include "Player.h"
#include "pt_zone_client.h"
#include "MainThread.h"

bool CNetworkEX::AnimusRecallRequest(int n, char* pBuf)
{
	_animus_recall_request_clzo* pRecv = (_animus_recall_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse)
		return true;

	pOne->pc_AnimusRecallRequest(pRecv->dwAnimusItemSerial, pRecv->wAnimusClientHP, pRecv->wAnimusClientFP);

	return true;
}

bool CNetworkEX::AnimusReturnRequest(int n, char* pBuf)
{
	_animus_return_request_clzo* pRecv = (_animus_return_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse)
		return true;

	pOne->pc_AnimusReturnRequest();

	return true;
}

bool CNetworkEX::AnimusCommandRequest(int n, char* pBuf)
{
	_animus_command_request_clzo* pRecv = (_animus_command_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_bCorpse)
		return true;

	if(pRecv->byCommandCode >= ANIMUS_ACT_TYPE_NUM)
		return false;

	pOne->pc_AnimusCommandRequest(pRecv->byCommandCode);

	return true;
}

bool CNetworkEX::AnimusTargetRequest(int n, char* pBuf)
{
	_animus_target_request_clzo* pRecv = (_animus_target_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_bCorpse)
		return true;

	if(!g_Main.GetObject(obj_kind_char, pRecv->byObjectID, pRecv->wObjectIndex))
		return false;

	pOne->pc_AnimusTargetRequest(pRecv->byObjectID, pRecv->wObjectIndex, pRecv->dwObjectSerial);

	return true;
}

bool CNetworkEX::SelectClassRequest(int n, char* pBuf)
{
	_select_class_request_clzo* pRecv = (_select_class_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper)
		return true;

	if(pRecv->wSelClassIndex >= g_Main.m_tblClass.GetRecordNum())
		return false;

	pOne->pc_SelectClassRequest(pRecv->wSelClassIndex);

	return true;
}


