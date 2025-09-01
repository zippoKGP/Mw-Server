/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : entry.cpp
  *owner  		: Andy
  *description  : 
  *modified     : 2004/12/2
******************************************************************************/ 

#include "engine_global.h"
#include "gamemain.h"
#include "utility.h"
#include "system.h"
#include "weather.h"
#include "vari-ext.h"
#include "tier0/dbg.h"

#include "net.h"
#include "net_msg.h"
#include "login.h"
#include "player_info.h"
#include "utillib/utillib.h"
#include "data.h"
#include "weather.h"
#include "chatwindow.h"
#include "resource.h"

static HINSTANCE   g_hInstance;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
    g_hInstance = hInstance;

//	extern void mda_test(void);
	
//	mda_test();
	
    return RaysDDWinMain (hInstance, hPrevInstance,
        szCmdLine, iCmdShow);
}


int	gameSet(void)
{
char version_text[255];
char window_name[255];

	get_version_string(version_text,GAME_VERSION0,GAME_VERSION1,GAME_VERSION2,__DATE__,__TIME__);

	sprintf(window_name,"%s( version %s )",GAME_NAME,version_text);
	SetDDWndName( (char *)window_name );
	SetDDWndIcon( LoadIcon(g_hInstance, NULL), LoadIcon(g_hInstance, NULL) );
	SetDDWndRunOnce( TRUE );

	initialize_system_config();

	if(system_config.window_mode==0)
		SetDDWndScreenMode( WINDOWS_SCREEN_MODE );
	else
		SetDDWndScreenMode( FULL_SCREEN_MODE );
		
    SetDDWndEnableExit( TRUE ); //enable exit the main process when user exit the DDWnd
    SetDDWndScreenSize( SCREEN_WIDTH, SCREEN_HEIGHT );
	SetDDWndScreenPos(0, 0);

//    SetDDWndScreenMode( FULL_SCREEN_MODE );
	
	return 0;
}


int gameInit(void)
{
	SpewOutputFunc(MySpewFunc);
	
	CoInitialize(NULL);

	// add by andy
	if (initNetModule() == -1)
	{
		return -1;
	}
	
	HICON hIcon = ::LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_MAINFRAME));
//	::SendMessage(g_hDDWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(g_hDDWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	
    if( FAILED( install(system)			)) FailMsg("install system failed");
    if( FAILED( install(draw)			)) FailMsg("install draw failed");
    if( FAILED( install(timer)			)) FailMsg("install timer failed");
    if( FAILED( install(input)			)) FailMsg("install input failed");


#ifdef  BIG5_VERSION
    set_winfont_name((USTR*)GAME_FONT_NAME);
    set_winfont_support_system(SUPPORT_FONT_SYSTEM_BIG5 | SUPPORT_FONT_SYSTEM_GB );
    set_winfont_weight(FW_NORMAL);
    set_winfont_convert_big5_to_gb(true);
#endif//BIG5_VERSION
	
#ifdef  GB_VERSION
    set_winfont_name((USTR*)GAME_FONT_NAME);
    set_winfont_support_system(SUPPORT_FONT_SYSTEM_GB );
    set_winfont_weight(FW_NORMAL);
    set_winfont_convert_big5_to_gb(false);
#endif	

//    set_winfont_support_system(SUPPORT_FONT_SYSTEM_BIG5 | SUPPORT_FONT_SYSTEM_GB );
	

    if( FAILED( install(winfont)		)) FailMsg("install winfont failed");

//	if( FAILED( install(net)			)) FailMsg("install net failed");

//    if( FAILED( install(sound)			)) FailMsg("install sound failed");
	install(sound);
	
//    if( FAILED( install(mp3)			)) FailMsg("install mp3 failed");
	if( FAILED( install(weather)		)) FailMsg("install weather failed");

//	if( FAILED( install(cdrom_music)	)) FailMsg("install cdrom music failed");


//	set_winfont_support_system(SUPPORT_FONT_SYSTEM_BIG5);		
//	init_ime();
//	char *pname = get_ime_name();
//	sharp_ime(g_hDDWnd);


	if(getChatWindow()->WinInit(g_hInstance)!=0)
		return(-1);



	sprintf((char *)print_rec,"Game Start OK");
	log_error(1,print_rec);


    return 0;
}

int	gameMain(void)
{
/*******************************************************************************************}
{								main game entry												}
{*******************************************************************************************/
	
	enter_game_main();
	return(1);
}

void gameExit(void)
{
	CoUninitialize();

	// add by andy
	if (shutdownNetModule() == -1)
	{
		sprintf((char *)print_rec,"shutdown net error");
		log_error(1,print_rec);		
	}
	
	sprintf((char *)print_rec,"Game Exit OK");
	log_error(1,print_rec);
}


//============================================================================
