/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2004 ALL RIGHTS RESERVED.

  *file name    : net_msg.h
  *owner  		: Andy
  *description  : 
  *modified     : 2005/1/6
******************************************************************************/ 

#ifndef ENGINE_ENT_MSG_H
#define ENGINE_ENT_MSG_H

#include "net/iclientnet.h"

UG_ULONG UG_FUN_CALLBACK ClientNetHandle(UG_DWORD dwMsgID,UG_DWORD dwParam,const UG_PCHAR pchData,UG_ULONG ulLen);

#endif