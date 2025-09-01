/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
  *file name    : IClientNet.h
  *owner        : Ben
  *description  : IClientNet接口类的头文件，用于客户端网络的连接，数据发送等。
  *modified     : 2004/12/20
******************************************************************************/ 

#if !defined(AFX_ICLIENTNET_H__C11C684C_DAEA_4187_B552_5D4FA814FCA0__INCLUDED_)
#define AFX_ICLIENTNET_H__C11C684C_DAEA_4187_B552_5D4FA814FCA0__INCLUDED_

#include "net/UGBenDef.h"
//----------------Add by ben in 2005-01-06---------------------------------------------
#ifndef __NET_CONNECT_AND_DISCONNECT_MSG__
#define __NET_CONNECT_AND_DISCONNECT_MSG__

#define		NET_MSG_CONNECT						0x00000000 //网络连接消息
#define		NET_MSG_DISCONNECT					0x00000001 //网络断开消息
//客户端校验消息，内部使用,服务器端接收到该数据后发送发送发送返回的参数为0表示通过接收，否则不接受
#define		NET_MSG_CHECK_DATA					0x00000002

#endif//(__NET_CONNECT_AND_DISCONNECT_MSG__)

//-------------------------------------------------------------------------------------
#define		NETCLIENT_INTERFACE_VERSION			"INetClient_003"

typedef UG_ULONG (UG_FUN_CALLBACK *FnClientNet)(UG_DWORD dwMsgID,UG_DWORD dwParam,const UG_PCHAR pchData,UG_ULONG ulLen);

class IClientNet  
{
public: //获取IP
	virtual UG_ULONG		initGetIP() = 0; //返回个数，-1表示失败
	virtual UG_ULONG		getIPCount() = 0; //返回个数
	virtual UG_DWORD		getIP(UG_ULONG ulIndex) = 0; //获得IP
	virtual UG_PCHAR		convertIP(UG_DWORD dwIP) = 0;
	virtual UG_DWORD		convertIP(UG_PCHAR pchIP) = 0;
	
public:
	virtual UG_ULONG		getErrorCode() = 0;
	virtual UG_ULONG		getServer(UG_DWORD& dwIP,UG_WORD& wPort) = 0;
	virtual UG_ULONG		getHost(UG_DWORD& dwIP,UG_WORD& wPort) = 0;
	
public:
	//init初始化函数，dwIP服务器IP,wPort服务器端口,dwBindIP为客户端本地邦定的IP,,pfnCallback回调函数指针
	//bAsyc为非真时表示必须主动调用netFrame才能通过回调函数取得数据，netFrame和回调函数在同一线程，
	//bAsyc为真时表示有数据时回调函数将自动运行，回调函数在另一线程，
	//表示客户端需要服务器端的校验wClientCheckID，服务器端接收到消息为NET_MSG_CHECK_DATA，
	//如果校验通过则返回同样的消息，参数为0，否则返回同样的消息，但是参数不为0。
	virtual UG_ULONG		init(UG_DWORD dwServerIP,UG_WORD wServerPort,UG_DWORD dwBindIP,UG_DWORD dwClientCheckID,FnClientNet pfnCallback,UG_BOOL bAsyc = FALSE) = 0;
	//pchPathName配置文件，格式为ini文件，有下面的格式即可[NetConfig](ip= 192.168.0.14,bindip = 192.168.0.14,port = 34567)
	virtual UG_ULONG		init(UG_PCHAR pchPathName,FnClientNet pfnCallback,UG_BOOL bAsyc = FALSE) = 0;
	virtual UG_ULONG		disconnect() = 0;
	virtual UG_ULONG		shutdown() = 0;
	//发送数据
	virtual UG_ULONG		postNetMessage(UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR pchData = NULL,UG_ULONG ulLen = 0) = 0;
	//将数据写进发送缓存，不发数据，参数位数据指针，长度。
	virtual UG_ULONG		writeBuffer(UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR pchData = NULL,UG_ULONG ulLen = 0) = 0;
	//发送缓存数据。
	virtual UG_ULONG		sendBuffer() = 0;
	//主动接收数据数据，在bAsyc为非真时时才能用。
	//lRecvCount 回调个数，<1表示全部，否则表示个数
	virtual UG_ULONG		netFrame(UG_LONG lRecvCount = 0) = 0;
	
};

//一次发送的数据大小不能超过4KB。

#endif // !defined(AFX_ICLIENTNET_H__C11C684C_DAEA_4187_B552_5D4FA814FCA0__INCLUDED_)
