// MyNetWork_Message.cpp: implementation of the CNetworkEX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyNetWorking.h"
#include "Player.h"
#include "protocol.h"
#include "pt_world_msg.h"
#include "MainThread.h"

bool CNetworkEX::OpenMsgrRequest(DWORD n, char* pMsg)
{
	_open_msgr_request_mgwr* pRecv = (_open_msgr_request_mgwr*)pMsg;

	g_Main.pc_OpenMsgrRequest(pRecv->szWorldName, pRecv->dwLocalIP);

	return true;
}

bool CNetworkEX::EnterUserReport(DWORD n, char* pMsg)
{
	_enter_user_report_mgwr* pRecv = (_enter_user_report_mgwr*)pMsg;

	if(pRecv->idWorld.wIndex >= MAX_PLAYER)
		return false;

	if(pRecv->idLocal.wIndex >= MAX_PLAYER)
		return false;

	g_Main.pc_EnterUserMessengerReport(&pRecv->idWorld, &pRecv->idLocal);

	return true;
}

bool CNetworkEX::ExitUserReport(DWORD n, char* pMsg)
{
	_exit_user_report_mgwr* pRecv = (_exit_user_report_mgwr*)pMsg;

	if(pRecv->idWorld.wIndex >= MAX_PLAYER)
		return false;

	g_Main.pc_ExitUserMessengerReport(&pRecv->idWorld);

	return true;
}


