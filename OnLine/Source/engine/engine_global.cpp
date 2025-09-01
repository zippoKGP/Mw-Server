/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : engine_global.cpp
  *owner  		: Andy
  *description  : 
  *modified     : 2004/12/2
******************************************************************************/ 

#include "engine_global.h"

//////////////////////////////////////////////////////////////////////////
//

#ifdef	_DEBUG
#pragma comment(lib,"2denginedbg.lib")
#pragma	comment(lib,"mp3dbg.lib")
#pragma	comment(lib,"jpegdbg.lib")
#else//!DEBUG
#pragma comment(lib,"2denginedbg.lib")
//#pragma comment(lib,"2dengine.lib")
#pragma	comment(lib,"mp3.lib")
#pragma	comment(lib,"jpeg.lib")
#endif//DEBUG
