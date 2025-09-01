// MsgProcess.h: interface for the CMsgProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSGPROCESS_H__0234982C_B784_4FFE_832F_FE6FF21884DF__INCLUDED_)
#define AFX_MSGPROCESS_H__0234982C_B784_4FFE_832F_FE6FF21884DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MsgData.h"

#define gm_game	0
#define gm_interface 1000
#define gm_system 10000

#define gm_game_char_create		gm_game+1 

#define gm_interface_displaymode_change		gm_interface+1
#define gm_interface_map_change				gm_interface+2
#define gm_interface_monster_int			gm_interface+3
#define gm_interface_character_select		gm_interface+4
#define gm_interface_update_server			gm_interface+5
#define gm_interface_update_object			gm_interface+6
#define gm_interface_update_map				gm_interface+7
#define gm_interface_npc_int				gm_interface+8
#define gm_interface_display_all            gm_interface+9
#define gm_interface_PreClose				gm_interface+10
#define gm_interface_FileReload				gm_interface+11
#define gm_interface_ServerClose			gm_interface+12
#define gm_interface_UserExit				gm_interface+13

#define gm_system_close						gm_system+1

class CMsgProcess : public CMsgData
{
public:
	CMsgProcess(int nObjNum);
	CMsgProcess();
	virtual ~CMsgProcess();

private:
	/*virtual*/ void ProcessMessage(_message* pMsg);
};

#endif // !defined(AFX_MSGPROCESS_H__0234982C_B784_4FFE_832F_FE6FF21884DF__INCLUDED_)
