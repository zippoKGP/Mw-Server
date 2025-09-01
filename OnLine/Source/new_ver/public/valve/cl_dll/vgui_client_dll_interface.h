//=========== (C) Copyright 1999 Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Client DLL VGUI2 Viewport
//
// $Workfile:     $
// $Date: 2004/12/02 04:29:16 $
//
//-----------------------------------------------------------------------------
// $Log: vgui_client_dll_interface.h,v $
// Revision 1.1  2004/12/02 04:29:16  andy
// *** empty log message ***
//
// Revision 1.1  2004/11/12 12:36:29  andy
// *** empty log message ***
//
// Revision 1.1  2004/04/16 02:25:07  bob
// *** empty log message ***
//
// Revision 1.2  2004/03/07 09:36:40  andy
// *** empty log message ***
//
//
// $NoKeywords: $
//=============================================================================
#ifndef VGUICLIENTDLLINTERFACE_H
#define VGUICLIENTDLLINTERFACE_H

#ifdef _WIN32
#pragma once
#endif

typedef int   (*ININTERMISSION)();
typedef void  (*SPECTATOR_FINDNEXTPLAYER)( bool reverse );
typedef void  (*SPECTATOR_FINDPLAYER)( const char *playerName );
typedef int   (*SPECTATOR_PIPVALUE)();
typedef void  (*SPECTATOR_CHECKSETTINGS)();
typedef int   (*SPECTATOR_NUMBER)();
typedef float (*HUDTIME)();
typedef void  (*MESSAGE_ADD)( void * newMessage );
typedef void  (*MESSAGE_HUD)( const char *pszName, int iSize, void *pbuf );
typedef int   (*TEAMPLAY)();
typedef void  (*CLIENTCMD)( const char *cmd );
typedef int   (*TEAMNUMBER)();
typedef const char *(*GETLEVELNAME)();
typedef char * (*COMPARSEFILE)(char *data, char *token);
typedef void * (*COMLOADFILE)(char *path, int usehunk, int *pLength);
typedef void  (*COMFREEFILE)( void *buffer );
typedef void  (*CONDPRINTF)( char *fmt, ...);

typedef struct {
	ININTERMISSION InIntermission;
	
	SPECTATOR_FINDNEXTPLAYER FindNextPlayer;
	SPECTATOR_FINDPLAYER FindPlayer;
	SPECTATOR_PIPVALUE PipValue;
	SPECTATOR_CHECKSETTINGS CheckSettings;
	SPECTATOR_NUMBER SpectatorNumber;

	HUDTIME HudTime;
	MESSAGE_ADD MessageAdd;
	MESSAGE_HUD MessageHud;
	TEAMPLAY TeamPlay;

	CLIENTCMD ClientCmd;
	TEAMNUMBER TeamNumber;
	GETLEVELNAME GetLevelName;

	COMPARSEFILE COM_ParseFile;
	COMLOADFILE  COM_LoadFile;
	COMFREEFILE  COM_FreeFile;
	CONDPRINTF	Con_DPrintf;

} VGuiLibraryInterface_t; 

#endif //VGUICLIENTDLLINTERFACE_H