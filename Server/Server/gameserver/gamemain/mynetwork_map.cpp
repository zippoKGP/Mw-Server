// MyNetWork_Init.cpp: implementation of the CNetworkEX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyNetWorking.h"
#include "Player.h"
#include "protocol.h"
#include "pt_zone_client.h"
#include "MainThread.h"

bool CNetworkEX::MovePortalRequest(int n, char* pBuf)
{
	_move_potal_request_clzo* pRecv = (_move_potal_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	pOne->pc_MovePortal(pRecv->byPotalIndex);

	return true;
}

