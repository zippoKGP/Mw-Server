/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : ibcclientnet.h
  *owner  		: bob
  *description  : 
  *modified     : 2005/5/9
******************************************************************************/ 


#ifndef BC_CLIENT_NET
#define  BC_CLIENT_NET

#include "tier0/platform.h"



typedef UGDWORD (STDCALL *FnClientNet)(UGDWORD dwMsgID,UGDWORD dwParam,const UGPCHAR pchData,UGDWORD ulLen);



DLL_EXPORT	int initNetModule(UGVOID **pClientNet);

DLL_EXPORT	int shutdownNetModule(void);

DLL_EXPORT	int	connectServerIdle(UGVOID *pClientNet, UGDWORD ip, int port, FnClientNet fnMsgFunc, UGDWORD checkid = 0);

DLL_EXPORT	int	disconnectServer(UGVOID *pClientNet);

DLL_EXPORT	int	postNetMessage(UGVOID *pClientNet, UGDWORD dwMsgID, UGDWORD dwParam, char *pchData = NULL, long lData_Len = 0);

DLL_EXPORT	int	netFrame(UGVOID *pClientNet);


//////////////////////////////////////////////////////////////////////////
//
//启动网络消息处理模块，同步处理模块，只有与netFrame联合时才能取到数据，属于单线程，返回值int,0表示正确，-1表示错误，该函数与上面函数DWORD dwSuspend = 1一样。


//断开连接。


//把数据发送给服务器，返回值int,-3网络句柄无效，-2表示加密错误。-1表示发送错误，否则返回发送的数据量

//处理游戏消息，返回值int,0表示网络关闭，1返回一个网络消息，-3表示网络句柄无效。




 
#endif	//BC_CLIENT_NET