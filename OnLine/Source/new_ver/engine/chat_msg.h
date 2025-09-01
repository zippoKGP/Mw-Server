/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2004 ALL RIGHTS RESERVED.

  *file name    : chat_msg.h
  *owner  		: Andy
  *description  : 
  *modified     : 2005/1/17
******************************************************************************/ 

#ifndef ENGINE_CHAT_MSG_H
#define ENGINE_CHAT_MSG_H

#include "net/iclientnet.h"

UG_ULONG UG_FUN_CALLBACK ChatNetHandle(UG_DWORD dwMsgID,UG_DWORD dwParam,
									   const UG_PCHAR pchData,UG_ULONG ulLen);

HRESULT WorldChat(const char *str, int id);

HRESULT PrivateChat(const char* str, int id, int rev_id);

HRESULT FriendChat(const char* str, int id, int rev_id);

HRESULT CurrentChat(const char *str, int id);

HRESULT TradeChat(const char *str, int id);

HRESULT GroupChat(const char *str, int id);

#endif