/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2004 ALL RIGHTS RESERVED.

  *file name    : net.h
  *owner  		: Andy
  *description  : 
  *modified     : 2004/12/2
******************************************************************************/ 

#ifndef ENGINE_NET_H
#define ENGINE_NET_H

#include "net/iclientnet.h"

//////////////////////////////////////////////////////////////////////////
//
int initNetModule(void);

int shutdownNetModule(void);

//////////////////////////////////////////////////////////////////////////
//
//启动网络消息处理模块，同步处理模块，只有与netFrame联合时才能取到数据，属于单线程，返回值int,0表示正确，-1表示错误，该函数与上面函数DWORD dwSuspend = 1一样。
int	connectServerIdle(DWORD ip, int port, FnClientNet fnMsgFunc);
  
//断开连接。
int	disconnectServer();

//把数据发送给服务器，返回值int,-3网络句柄无效，-2表示加密错误。-1表示发送错误，否则返回发送的数据量
int	postNetMessage(DWORD dwMsgID, DWORD dwParam, char *pchData = NULL, long lData_Len = 0);

//处理游戏消息，返回值int,0表示网络关闭，1返回一个网络消息，-3表示网络句柄无效。
int	netFrame(void);

//////////////////////////////////////////////////////////////////////////
//
int	ConnectChatServer(DWORD ip, int port, DWORD check_id, FnClientNet fnMsgFunc);

int	DisconnectChatServer();

//把数据发送给服务器，返回值int,-3网络句柄无效，-2表示加密错误。-1表示发送错误，否则返回发送的数据量
int	PostChatNetMessage(DWORD dwMsgID, DWORD dwParam, char *pchData = NULL, long lData_Len = 0);

//处理游戏消息，返回值int,0表示网络关闭，1返回一个网络消息，-3表示网络句柄无效。
int	NetChatFrame(void);

//////////////////////////////////////////////////////////////////////////
//
int updateNetData(void);

#endif