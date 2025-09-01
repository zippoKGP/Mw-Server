// MyNetWork_Chat.cpp: implementation of the CNetworkEX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyNetWorking.h"
#include "Player.h"
#include "protocol.h"
#include "pt_zone_client.h"
#include "MainThread.h"

bool CNetworkEX::ChatOperatorRequest(int n, char* pBuf)
{
	_chat_operator_request_clzo* pRecv = (_chat_operator_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper)
		return true;

	if(pRecv->bySize > max_message_size)
		return false;

	char szChat[max_message_size+1];
	memcpy(szChat, pRecv->sChatData, pRecv->bySize);
	szChat[pRecv->bySize] = NULL;

	pOne->pc_ChatOperatorRequest(pRecv->bySize, szChat);

	return true;
}

bool CNetworkEX::ChatCircleRequest(int n, char* pBuf)
{
	_chat_circle_request_clzo* pRecv = (_chat_circle_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper)
		return true;

	if(pRecv->bySize > max_message_size)
		return false;

	char szChat[max_message_size+1];
	memcpy(szChat, pRecv->sChatData, pRecv->bySize);
	szChat[pRecv->bySize] = NULL;

	pOne->pc_ChatCircleRequest(pRecv->bySize, szChat);

	return true;
}

bool CNetworkEX::ChatWhisperRequest(int n, char* pBuf)
{
	_chat_whisper_request_clzo* pRecv = (_chat_whisper_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper)
		return true;

	if(pRecv->wDstIndex >= MAX_PLAYER)
		return false;

	if(pRecv->bySize > max_message_size)
		return false;

	char szChat[max_message_size+1];
	memcpy(szChat, pRecv->sChatData, pRecv->bySize);
	szChat[pRecv->bySize] = NULL;

	pOne->pc_ChatWhsiperRequest(pRecv->wDstIndex, pRecv->bySize, szChat);

	return true;
}

bool CNetworkEX::ChatFarRequest(int n, char* pBuf)
{
	_chat_far_request_clzo* pRecv = (_chat_far_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper)
		return true;

	if(pRecv->bySize > max_message_size)
		return false;

	char szChat[max_message_size+1];
	memcpy(szChat, pRecv->sChatData, pRecv->bySize);
	szChat[pRecv->bySize] = NULL;

	pOne->pc_ChatFarRequest(pRecv->szName, pRecv->bySize, szChat);

	return true;
}

bool CNetworkEX::ChatPartyRequest(int n, char* pBuf)
{
	_chat_party_request_clzo* pRecv = (_chat_party_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper)
		return true;

	if(pRecv->bySize > max_message_size)
		return false;

	char szChat[max_message_size+1];
	memcpy(szChat, pRecv->sChatData, pRecv->bySize);
	szChat[pRecv->bySize] = NULL;

	pOne->pc_ChatPartyRequest(pRecv->bySize, szChat);

	return true;
}

bool CNetworkEX::ChatRaceRequest(int n, char* pBuf)
{
	_chat_race_request_clzo* pRecv = (_chat_race_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper)
		return true;

	if(pRecv->bySize > max_message_size)
		return false;

	char szChat[max_message_size+1];
	memcpy(szChat, pRecv->sChatData, pRecv->bySize);
	szChat[pRecv->bySize] = NULL;

	pOne->pc_ChatRaceRequest(pRecv->bySize, szChat);

	return true;
}

bool CNetworkEX::ChatCheatRequest(int n, char* pBuf)
{
	_chat_cheat_request_clzo* pRecv = (_chat_cheat_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper)
		return true;

	if(pRecv->bySize > max_message_size)
		return false;

	char szChat[max_message_size+1];
	memcpy(szChat, pRecv->sChatData, pRecv->bySize);
	szChat[pRecv->bySize] = NULL;

	pOne->pc_ChatCheatRequest(pRecv->bySize, szChat);

	return true;
}

bool CNetworkEX::ChatManageRequest(int n, char* pBuf)
{/*
	_chat_manage_request_clzo* pRecv = (_chat_manage_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper)
		return true;

	if(pRecv->bySize > max_message_size)
		return false;

	char szChat[max_message_size+1];
	memcpy(szChat, pRecv->sChatData, pRecv->bySize);
	szChat[pRecv->bySize] = NULL;

	pOne->pc_ChatManageRequest(pRecv->bySize, szChat);
*/
	return true;
}

bool CNetworkEX::ChatMgrWhisperRequest(int n, char* pBuf)
{
	_chat_mgr_whisper_request_clzo* pRecv = (_chat_mgr_whisper_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper)
		return true;

	if(pRecv->bySize > max_message_size)
		return false;

	char szChat[max_message_size+1];
	memcpy(szChat, pRecv->sChatData, pRecv->bySize);
	szChat[pRecv->bySize] = NULL;

	pOne->pc_ChatMgrWhisperRequest(pRecv->bySize, szChat);

	return true;
}
