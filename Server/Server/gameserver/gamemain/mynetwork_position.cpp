// MyNetWork_Position.cpp: implementation of the CNetworkEX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyNetWorking.h"
#include "Player.h"
#include "MyUtil.h"
#include "protocol.h"
#include "pt_zone_client.h"
#include "MainThread.h"

bool CNetworkEX::NextPoint(int n, char* pBuf)
{
	_move_request_clzo* pRecv = (_move_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bLive)
		return true;

//	if(pOne->m_pmTrd.bDTradeMode)	//안에서 처리..
//		return true;

//	if(pOne->m_bCorpse)
//		return true;

	if(pRecv->byMoveType != move_type_walk && pRecv->byMoveType != move_type_run)
		return false;

	if(pRecv->byModeType != mode_type_demili && pRecv->byModeType != mode_type_mili)
		return false;

	float fCur[3];
	::ShortToFloat(pRecv->zCur, fCur, 3);
	float fTar[3] = { (float)pRecv->zTar[0], fCur[1], (float)pRecv->zTar[1] };

	pOne->pc_MoveNext(pRecv->byMoveType, pRecv->byModeType, fCur, fTar);

	return true;
}

bool CNetworkEX::RealMovPosRequest(int n, char* pBuf)
{
	_real_movpos_request_clzo* pRecv = (_real_movpos_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bLive)
		return true;

//	if(pOne->m_pmTrd.bDTradeMode)//안에서 처리..
//		return true;

//	if(pOne->m_bCorpse)
//		return true;

	float fCur[3];
	::ShortToFloat(pRecv->zCur, fCur, 3);

	pOne->pc_RealMovPos(fCur);

	return true;
}

bool CNetworkEX::Stop(int n, char* pBuf)
{
	_player_stop_clzo* pRecv = (_player_stop_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bLive)
		return true;

//	if(pOne->m_pmTrd.bDTradeMode)//안에서 처리..
//		return true;

//	if(pOne->m_bCorpse)
//		return true;

	float fCur[3];
	::ShortToFloat(pRecv->zCur, fCur, 3);

	pOne->pc_MoveStop(fCur);

	return true;
}

bool CNetworkEX::GotoBasePortalRequest(int n, char* pBuf)
{
	_goto_baseportal_request_clzo* pRecv = (_goto_baseportal_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	pOne->pc_GotoBasePortalRequest(pRecv->wItemSerial);

	return true;
}

bool CNetworkEX::GotoAvatorRequest(int n, char* pBuf)
{
	_goto_avator_request_clzo* pRecv = (_goto_avator_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse)
		return true;

	if(strlen(pRecv->szAvatorName) > max_name_len)
		return false;

	if(pOne->m_byUserDgr < USER_DEGREE_MGR)
		return true;

	if(pOne->m_byUserDgr == USER_DEGREE_MGR)
	{
		if(pOne->m_bySubDgr < MGR_DEGREE_MASTER)
			return true;
	}

	pOne->pc_GotoAvatorRequest(pRecv->szAvatorName);

	return true;
}

bool CNetworkEX::MoveTypeChangeRequeset(int n, char* pBuf)
{
	_move_type_change_request_clzo* pRecv = (_move_type_change_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	if(pRecv->byMoveType != move_type_walk && pRecv->byMoveType != move_type_run)
		return false;

	pOne->pc_MoveModeChangeRequest(pRecv->byMoveType);

	return true;
}

bool CNetworkEX::ObjectServerPosRequest(int n, char* pBuf)
{
	_object_server_pos_request_clzo* pRecv = (_object_server_pos_request_clzo*)pBuf;

//0: 성공..1: 배열범위초과, 2: 죽은오브젝트, 3: 시리얼이 틀림
	
	BYTE byRetCode = 0;
	CGameObject* pObj = g_Main.GetObject(pRecv->byObjKind, pRecv->byObjID, pRecv->wObjIndex);
	if(!pObj)
	{
		byRetCode = 1;
		goto RESULT;
	}
	if(!pObj->m_bLive)
	{
		byRetCode = 2;
		goto RESULT;
	}
	if(pObj->m_dwObjSerial != pRecv->dwObjSerial)
	{
		byRetCode = 3;
		goto RESULT;
	}

RESULT:

	_object_server_pos_result_zocl Send;

	Send.byErrCode = byRetCode;
	Send.byObjKind = pRecv->byObjKind;
	Send.byObjID = pRecv->byObjID;
	Send.wObjIndex = pRecv->wObjIndex;
	Send.dwObjSerial = pRecv->dwObjSerial;

	if(byRetCode == 0)
		memcpy(Send.fServerPos, pObj->m_fCurPos, sizeof(float)*3);

	BYTE byType[msg_header_num] = {sync_msg, object_server_pos_result_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(n, byType, (char*)&Send, sizeof(Send));

	return true;
}