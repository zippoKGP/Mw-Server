/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
 2003 ALL RIGHTS RESERVED.

*file name    : utillib.h
*owner        : Andy
*description  : 
*modified     : 2004/9/3
******************************************************************************/ 

#ifndef UTILLIB_H
#define UTILLIB_H

#include "tier0/platform.h"

//-----------------------------------------------------------------------------
// dll export stuff
//-----------------------------------------------------------------------------
#ifdef UTILLIB_DLL_EXPORT
#define UTILLIB_INTERFACE	DLL_EXPORT
#define UTILLIB_OVERLOAD	DLL_GLOBAL_EXPORT
#define UTILLIB_CLASS		DLL_CLASS_EXPORT
#define UTILLIB_GLOBAL		DLL_GLOBAL_EXPORT
#else
#define UTILLIB_INTERFACE	DLL_IMPORT
#define UTILLIB_OVERLOAD	DLL_GLOBAL_IMPORT
#define UTILLIB_GLOBAL		DLL_GLOBAL_IMPORT
#define UTILLIB_CLASS		DLL_CLASS_IMPORT
#endif

#include "sound.h"
#include "iconfig.h"
#include "code_trans.h"
#include "dbg_file.h"

#endif
