/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
 2003 ALL RIGHTS RESERVED.

*file name    : code_trans.h
*owner        : Andy
*description  : 
*modified     : 2004/4/17
******************************************************************************/ 

#ifndef __CODE_TRANS_H__
#define __CODE_TRANS_H__

#include <windows.h>
#include "tier0/platform.h"

//////////////////////////////////////////////////////////////////////////
//
#define CODE_PAGE_GBK  0x00000001
#define CODE_PAGE_BIG5 0x00000002
#define CODE_PAGE_EN   0x00000003

//////////////////////////////////////////////////////////////////////////
//
UTILLIB_INTERFACE HRESULT initCodePage(void);

UTILLIB_INTERFACE int getCodePage(void);

UTILLIB_INTERFACE void closeCodePage(void);

#if 0
UTILLIB_INTERFACE WORD convert_word_gb2big(WORD gb_code);

UTILLIB_INTERFACE WORD convert_word_big2gb(WORD big_code);

UTILLIB_INTERFACE void make_gbk_2_big5_list(void);

UTILLIB_INTERFACE void make_big5_2_gbk_list(void);

UTILLIB_INTERFACE WORD gbk_2_big5(WORD gbk);

UTILLIB_INTERFACE WORD big5_2_gbk(WORD big5);

#endif

//////////////////////////////////////////////////////////////////////////
// cp : 1 gb
// cp : 2 big5

UTILLIB_INTERFACE char* transCodePage(char* str, int cp);

UTILLIB_INTERFACE char* transCodePageEx(char* str, int cp, int tp);

UTILLIB_INTERFACE LPCTSTR unicode_2_string(LPCTSTR str_unicode);

UTILLIB_INTERFACE LPCTSTR loadString(DWORD id);

UTILLIB_INTERFACE HMODULE getResouceHandle(void);

#endif //#ifndef __CODE_TRANS_H__