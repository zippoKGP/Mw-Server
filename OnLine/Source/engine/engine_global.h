/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2004 ALL RIGHTS RESERVED.

  *file name    : engine_global.h
  *owner  		: Andy
  *description  : 
  *modified     : 2004/12/2
******************************************************************************/ 

#ifndef ENGINE_GLOBAL_H
#define ENGINE_GLOBAL_H

// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers


#define GB_VERSION	1

#include <windows.h>

#pragma warning(disable: 4201)

#include <mmsystem.h>

//////////////////////////////////////////////////////////////////////////
// rayssdk
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

#include "vtga.h"
#include "vpcx.h"
#include "vbmp.h"
#include "vpsd.h"
#include "vflic.h"
#include "mp3.h"
#include "jpeg.h"
#include "raysime.h"
#include "textmsg.h"


#include <wchar.h>

//////////////////////////////////////////////////////////////////////////
//
//#include "net.h"
#include "control_src/control_src.h"

#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   600


#define MYTH_DEBUG	1

#endif