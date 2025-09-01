/*
**    HELLO.CPP
*/
#include "rays.h"
#include "winmain.h"
#include "xsystem.h"
#include "xmodule.h"
#include "xgrafx.h"
#include "xdraw.h"
#include "xinput.h"
#include "xtimer.h"
#include "xcdrom.h"
#include "xsound.h"
#include "xplay.h"
#include "xkiss.h"

#include "xfont.h"
//
#include "vtga.h"
#include "vpcx.h"
#include "vbmp.h"
#include "vpsd.h"
#include "vflic.h"
#include "mp3.h"
#include "jpeg.h"
#include "raysime.h"

//#include "resource.h"

#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480



#ifdef	DEBUG
#pragma comment(lib,"2denginedbg.lib")
//#pragma	comment(lib,"mp3dbg.lib")
#pragma	comment(lib,"jpegdbg.lib")
#else//!DEBUG
#pragma comment(lib,"2dengine.lib")
//#pragma	comment(lib,"mp3.lib")
#pragma	comment(lib,"jpeg.lib")
#endif//DEBUG


HINSTANCE   g_hInstance;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
    g_hInstance = hInstance;

    return RaysDDWinMain (hInstance, hPrevInstance,
        szCmdLine, iCmdShow);
}


int	gameSet(void)
{
	SetDDWndName( (char *)"GAME" );
	SetDDWndIcon( LoadIcon(g_hInstance, NULL), LoadIcon(g_hInstance, NULL) );
	SetDDWndRunOnce( TRUE );
    SetDDWndScreenMode( WINDOWS_SCREEN_MODE );
    SetDDWndEnableExit( TRUE ); //enable exit the main process when user exit the DDWnd
    SetDDWndScreenSize( SCREEN_WIDTH, SCREEN_HEIGHT );
	return 0;
}


int gameInit(void)
{
    if( FAILED( install(system)			)) FailMsg("install system failed");
    if( FAILED( install(draw)			)) FailMsg("install draw failed");
    if( FAILED( install(timer)			)) FailMsg("install timer failed");
    if( FAILED( install(input)			)) FailMsg("install input failed");
    if( FAILED( install(winfont)		)) FailMsg("install winfont failed");
    //	if( FAILED( install(net)			)) FailMsg("install net failed");
    //	if( FAILED( install(sound)			)) FailMsg("install sound failed");
    //	if( FAILED( install(mp3)			)) FailMsg("install mp3 failed");
    //	if( FAILED( install(cdrom_music)	)) FailMsg("install cdrom music failed");

    show_mouse(SHOW_WINDOW_CURSOR); // SHOW_IMAGE_CURSOR

//	init_ime();

//	char *pname = get_ime_name();

//	sharp_ime(g_hDDWnd);

	sprintf((char *)print_rec,"Game Start OK");
	log_error(1,print_rec);

    return 0;
}

int	gameMain(void)
{
    UCHR    ch;
    SLONG   main_pass = 0;
    SLONG   x = 100, y = 100;

    while(0 == main_pass)
    {
        idle_loop();y = 100;
        // fill_bitmap(screen_buffer, SYSTEM_WHITE);
        print16(x, y, (USTR*)"Press F12 to switch the screen mode.", PEST_PUT, screen_buffer);
        y += 20;
        print16(x, y, (USTR*)"Enter for play mp3 music, Space for stop.", PEST_PUT, screen_buffer);
        update_screen(screen_buffer);
        ch = read_data_key();
        if(ch) reset_data_key();
        switch(ch)
        {
        case S_Esc: 
            main_pass = 1; 
            break;
        }
    }
    return 1;
}

void gameExit(void)
{
	sprintf((char *)print_rec,"Game Exit OK");
	log_error(1,print_rec);
}


//============================================================================
