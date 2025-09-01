/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
 2003 ALL RIGHTS RESERVED.

*file name    : dbg_file.h
*owner        : Andy
*description  : 
*modified     : 2004/3/7
******************************************************************************/ 

#ifndef DBG_FILE_H
#define DBG_FILE_H

#include "tier0/platform.h"

UTILLIB_INTERFACE int openDebugFile(const char *strFileName);

UTILLIB_INTERFACE int writeDebugFile(char *msg, ...);

UTILLIB_INTERFACE int closeDebugFile(void);

#endif // DBG_FILE_H