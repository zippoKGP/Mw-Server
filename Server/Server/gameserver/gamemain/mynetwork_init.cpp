// MyNetWork_Init.cpp: implementation of the CNetworkEX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyNetWorking.h"
#include "Player.h"
#include "MyUtil.h"
#include "protocol.h"
#include "pt_zone_client.h"
#include "MainThread.h"

bool CNetworkEX::NewPosStartRequest(int n, char* pBuf)
{
	_new_pos_start_request_clzo* pRecv = (_new_pos_start_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bLoad)
		return true;

	if(!pOne->m_bMapLoading)
		return true;	

	if(pRecv->byMapInMode >= mapin_type_num)
		return false;

	pOne->pc_NewPosStart(pRecv->byMapInMode);

	return true;
}

bool CNetworkEX::BaseDownloadRequest(int n, char* pBuf)
{
	_base_download_request_clzo* pRecv = (_base_download_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bLoad)
		return true;

	if(pOne->m_bBaseDownload)
		return true;
	
	pOne->SendMsg_BaseDownloadResult();

	return true;
}

bool CNetworkEX::InvenDownloadRequest(int n, char* pBuf)
{
	_inven_download_request_clzo* pRecv = (_inven_download_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bLoad)
		return true;

	if(pOne->m_bInvenDownload)
		return true;

	pOne->SendMsg_InvenDownloadResult();

	return true;
}

bool CNetworkEX::SpecialDownloadRequest(int n, char* pBuf)
{
	_special_download_request_clzo* pRecv = (_special_download_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bLoad)
		return true;

	if(pOne->m_bSpecialDownload)
		return true;

	pOne->SendMsg_SpecialDownloadResult();

	return true;
}

bool CNetworkEX::ForceDownloadRequest(int n, char* pBuf)
{
	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bLoad)
		return true;

	if(pOne->m_bForceDownload)
		return true;

	pOne->SendMsg_ForceDownloadResult();

	return true;
}

bool CNetworkEX::CumDownloadRequest(int n, char* pBuf)
{
	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bLoad)
		return true;

	if(pOne->m_bCumDownload)
		return true;

	pOne->SendMsg_CumDownloadResult();

	return true;
}

bool CNetworkEX::OtherShapeRequest(int n, char* pBuf)
{
	_other_shape_request_clzo* pRecv = (_other_shape_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper)
		return true;

	if(pRecv->byReqType != _other_shape_request_clzo::SHAPE_ALL && pRecv->byReqType != _other_shape_request_clzo::SHAPE_PART)
		return false;

	if(pRecv->wIndex >= MAX_PLAYER)
		return false;

	CPlayer* pOther = &g_Player[pRecv->wIndex];

	if(pRecv->byReqType == _other_shape_request_clzo::SHAPE_ALL)
	{
		pOther->SendMsg_OtherShapeAll(pOne);
	}
	else if(pRecv->byReqType == _other_shape_request_clzo::SHAPE_PART)
	{
		pOther->SendMsg_OtherShapePart(pOne);	
	}
	
	return true;
}

bool CNetworkEX::Revival(int n, char* pBuf)
{
	_player_revival_request_clzo* pRecv = (_player_revival_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper)
		return true;

	pOne->pc_Revival();

	return true;
}

bool CNetworkEX::ExitSaveDataRequest(int n, char* pBuf)
{
	_exit_save_data_request_clzo* pRecv = (_exit_save_data_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper)
		return true;

	if(pRecv->wDataSize > 0x0FFF)
		return false;

	pOne->pc_ExitSaveDataRequest(pRecv->wDataSize, pRecv->sData);

	return true;
}
