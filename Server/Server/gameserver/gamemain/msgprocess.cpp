// MsgProcess.cpp: implementation of the CMsgProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MsgProcess.h"
#include "MainThread.h"
#include "ZoneServerDoc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMsgProcess::CMsgProcess(int nObjNum)
	:CMsgData(nObjNum)
{

}

CMsgProcess::CMsgProcess()
{
}

CMsgProcess::~CMsgProcess()
{

}

void CMsgProcess::ProcessMessage(_message* pMsg)
{
	_message* pMsgBuf = NULL;
	CCharacter* pChar = NULL;
	CMapData* pMap;

	switch(pMsg->GetMessage())
	{
	case gm_interface_displaymode_change:
		g_Main.gm_DisplaymodeChange();
		break;

	case gm_interface_map_change:
		pMap = g_Main.m_MapOper.GetMap(pMsg->GetKey1());
		g_Main.gm_MapChange(pMap);
		break;

	case gm_interface_monster_int:
		g_Main.gm_MonsterInit();
		break;

	case gm_interface_character_select:
		g_Main.gm_ObjectSelect();
		break;

	case gm_interface_update_server:
		g_Main.gm_UpdateServer();
		break;

	case gm_interface_update_object:
		g_Main.gm_UpdateObject();
		break;

	case gm_interface_update_map:
		g_Main.gm_UpdateMap();
		break;

	case gm_interface_display_all:
		g_Main.gm_DisplayAll();
		break;

	case gm_interface_PreClose:
		g_Main.gm_PreCloseAnn();
		break;

	case gm_interface_ServerClose:
		g_Main.gm_ServerClose();
		break;

	case gm_interface_UserExit:
		g_Main.gm_UserExit();
		break;

	case gm_system_close:
		g_Main.m_MapDisplay.OffDisplay();
		::SendMessage(g_pDoc->m_pwndMainFrame->m_hWnd, WM_CLOSE, 0, 0);
		break;
	}
}
	
