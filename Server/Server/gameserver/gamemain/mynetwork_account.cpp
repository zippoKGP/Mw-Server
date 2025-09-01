// MyNetWork_Account.cpp: implementation of the CNetworkEX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyNetWorking.h"
#include "Player.h"
#include "MyUtil.h"
#include "protocol.h"
#include "pt_zone_client.h"
#include "pt_account_world.h"
#include "MainThread.h"

bool CNetworkEX::OpenWorldSuccessResult(DWORD n, char* pMsg)
{
	_open_world_success_acwr* pRecv = (_open_world_success_acwr*)pMsg;

	g_Main.pc_OpenWorldSuccessResult(pRecv->byWorldCode,   
									pRecv->bControlOpen, pRecv->dwControlIP, pRecv->wControlPort, pRecv->dwControlMasterKey,
									pRecv->szDBName);//, pRecv->dwDBIP, pRecv->szDBAccount, pRecv->szDBPassword);
	return true;
}

bool CNetworkEX::OpenWorldFailureResult(DWORD n, char* pMsg)
{
	return true;
}

bool CNetworkEX::OpenControlInform(DWORD n, char* pMsg)
{
	return true;
}

bool CNetworkEX::TransAccountInform(DWORD n, char* pMsg)
{
	_trans_account_inform_acwr* pRecv = (_trans_account_inform_acwr*)pMsg;

	g_Main.pc_TransIPKeyInform(pRecv->dwAccountSerial, pRecv->szAccountID, pRecv->byUserDgr, pRecv->bySubDgr, pRecv->dwKey, &pRecv->gidGlobal, pRecv->dwClientIP);

	return true;
}

bool CNetworkEX::EnterWorldResult(DWORD n, char* pMsg)
{
	return true;
}

bool CNetworkEX::ForceCloseCommand(DWORD n, char* pMsg)
{
	_force_close_command_acwr* pRecv = (_force_close_command_acwr*)pMsg;

	if(pRecv->idLocal.wIndex > MAX_PLAYER)
		return false;

	g_Main.pc_ForceCloseCommand(&pRecv->idLocal, pRecv->bDirectly, pRecv->byKickType/*bPushClose,ydq*/, pRecv->dwPushIP);

	return true;
}

bool CNetworkEX::WorldServiceInform(DWORD n, char* pMsg)
{
	_world_service_inform_acwr* pRecv = (_world_service_inform_acwr*)pMsg;

	g_Main.pc_AlterWorldService(pRecv->bService);

	return true;
}

bool CNetworkEX::WorldExitInform(DWORD n, char* pMsg)
{
	_world_exit_inform_acwr* pRecv = (_world_exit_inform_acwr*)pMsg;

	g_Main.pc_AllUserKickInform();

	return true;
}

bool CNetworkEX::WorldMsgInform(DWORD n, char* pMsg)
{
	_world_msg_inform_acwr* pRecv = (_world_msg_inform_acwr*)pMsg;

	g_Main.pc_AllUserMsgInform(pRecv->byMsgSize, pRecv->szMsg);

	return true;
}
