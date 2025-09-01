/*
**    ENGINE.CPP
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


#if (!defined(MAKELIB) && !defined(MAKEDLL))



int gameSet(void)
{
    //SetAppName( char *name );
    //SetAppIcon( HICON hIcon, HICON hIconSm );
    //SetRunOnce( BOOL bFlag );
    //SetScreenMode( FULL_SCREEN_MODE );
    return 0;
}



int gameInit(void)
{
    if( FAILED( install(system)         )) FailMsg("install system failed");
    if( FAILED( install(draw)           )) FailMsg("install draw failed");
    if( FAILED( install(timer)          )) FailMsg("install timer failed");
    if( FAILED( install(input)          )) FailMsg("install input failed");
    if( FAILED( install(winfont)        )) FailMsg("install winfont failed");

    //if( FAILED( install(net)          )) FailMsg("install net failed");
    if( FAILED( install(sound)          )) FailMsg("install sound failed");

    //if( FAILED( install(cdrom_music)  )) FailMsg("install cdrom music failed");

    //load_mouse_image_cursor(0,(USTR *)"CURSOR.ANI");
    //show_mouse(SHOW_IMAGE_CURSOR);
    show_mouse(SHOW_WINDOW_CURSOR);

    sprintf((char *)print_rec," ");
    log_error(1,print_rec);

    return 0;
}

int gameMain(void)
{
    some_test();
    return 1;
}

void gameExit(void)
{
    sprintf((char *)print_rec," ");
    log_error(1,print_rec);
}


#endif//
//============================================================================
