// MyNetWork_System.cpp: implementation of the CNetworkEX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyNetWorking.h"
#include "protocol.h"
#include "pt_zone_client.h"
#include "MainThread.h"

bool CNetworkEX::EnterWorldRequest(int n, char* pBuf)
{
	_enter_world_request_zone* pRecv = (_enter_world_request_zone*)pBuf;
	
	//마스터키는 암호화한것으로...
	if(!m_pProcess[client_line]->FindKeyFromWaitList(n, pRecv->dwAccountSerial, pRecv->dwMasterKey, CHECK_KEY_NUM))
		Close(client_line, n, false);

	_socket* pSocket = GetSocket(client_line, n);
	if(!g_UserDB[n].Enter_Account(pRecv->dwAccountSerial, pSocket->m_Addr.sin_addr.S_un.S_addr, pRecv->bFullMode))
		Close(client_line, n, false);

	m_pProcess[client_line]->StartSpeedHackCheck(n, pRecv->dwAccountSerial);

	return true;
}

bool CNetworkEX::RegedCharRequest(int n, char* pBuf)
{
	_reged_char_request_zone* pRecv = (_reged_char_request_zone*)pBuf;

	if(!g_UserDB[n].Reged_Char_Request())
		Close(client_line, n, false);

	return true;
}

bool CNetworkEX::AddCharRequest(int n, char* pBuf)
{
	_add_char_request_zone* pRecv = (_add_char_request_zone*)pBuf;

	if(strlen(pRecv->szCharName) > max_name_len)
		return false;

	if(pRecv->bySlotIndex >= MAX_CHAR_PER_WORLDUSER)
		return false;

	if(pRecv->byRaceSexCode >= race_sex_kind_num)
		return false;

	if(strlen(pRecv->szClassCode) > class_code_len)
		return false;

	if(!g_UserDB[n].Insert_Char_Request(pRecv->szCharName, pRecv->bySlotIndex, pRecv->byRaceSexCode, pRecv->szClassCode, pRecv->dwBaseShape))
		Close(client_line, n, false);

	return true;
}

bool CNetworkEX::DelCharRequest(int n, char* pBuf)
{
	_del_char_request_zone* pRecv = (_del_char_request_zone*)pBuf;

	if(pRecv->bySlotIndex >= MAX_CHAR_PER_WORLDUSER)
		return false;

	if(!g_UserDB[n].Delete_Char_Request(pRecv->bySlotIndex))
		Close(client_line, n, false);

	return true;
}

bool CNetworkEX::SelCharRequest(int n, char* pBuf)
{
	_sel_char_request_zone* pRecv = (_sel_char_request_zone*)pBuf;

	if(pRecv->bySlotIndex >= MAX_CHAR_PER_WORLDUSER)
		return false;

	if(!g_UserDB[n].Select_Char_Request(pRecv->bySlotIndex))
		Close(client_line, n, false);

	return true;
}

bool CNetworkEX::MoveOutUserRequest(int n, char* pBuf)
{
	_moveout_user_request_zone* pRecv = (_moveout_user_request_zone*)pBuf;

	if(pRecv->byNextZoneType != ZONE_TYPE_GATE)
		return false;

	if(!g_UserDB[n].Lobby_Char_Request())
		Close(client_line, n, false);

	return true;
}
