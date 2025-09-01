/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.

  *file name    : INetServer.h
  *owner        : Ben
  *description  : INetServer接口类的头文件，用于服务器端网络的连接，数据发送，接收等。
				  网络通信一次最大发送的数据为2048BYTE
  *modified     : 2004/12/06
******************************************************************************/ 
#ifndef __INET_SERVER__
#define __INET_SERVER__
//-------------------------------------------------------------------------------------
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
typedef struct _tag_net_server_callback_param
{
	UG_PVOID			pvKey; //初始化时传入的参数，固定的参数
	UG_PVOID			pvPlayer; //发送数据或接收数据时传入传出的参数
	UG_PVOID			pvConnect; //连接标志，在连接事件时会返回给用户，同时在服务器端发送接收时需要用到该标志。
	UG_DWORD			dwMsgID;
	UG_DWORD			dwParam;
	UG_PCHAR			pchData; //连接断开事件时为NULL，接收数据事件时为数据指针，错误事件时是错误描述(英文)
	UG_ULONG			ulLen; //接收数据事件时为数据大小，错误事件时是错误代码，否则为0
	_tag_net_server_callback_param()
	{
		pvKey = NULL;
		pvPlayer = NULL;
		pvConnect = NULL;
		dwMsgID = 0;
		dwParam = 0;
		pchData = NULL;
		ulLen = 0;
	}
	
} NET_SERVER_PARAM, *PNET_SERVER_PARAM;
//网络回调函数
typedef UG_ULONG (UG_FUN_CALLBACK *FnNetSvr)(const PNET_SERVER_PARAM p);

typedef struct _tag_init_net_server
{
	UG_DWORD			dwIP; //服务器IP
	UG_DWORD			dwPort; //服务器Port
	UG_ULONG			ulMaxConnect; //最大连接数，到达最大连接数时，连接将会立即被关闭
	_tag_init_net_server()
	{
		dwIP = 0;
		dwPort = 0;
		ulMaxConnect = 0;
	}
	
} INIT_NET_SERCER, *PINIT_NET_SERCER;


#define		NETSERVER_INTERFACE_VERSION			"INetServer_001"

//TCPCallback*		pvCallback; //回调函数
//UG_PVOID			pvKey; //连接回调函数传出指针

class INetServer
{
public:
	virtual UG_ULONG		initGetIP() = 0;
	virtual UG_ULONG		getIPCount() = 0;
	virtual UG_DWORD		getIP(UG_ULONG ulIndex) = 0;
	virtual UG_PCHAR		convertIP(UG_DWORD dwIP) = 0;
	virtual UG_DWORD		convertIP(UG_PCHAR pchIP) = 0;

public:
	virtual UG_ULONG		getErrorCode() = 0;
	virtual UG_ULONG		getConnect(UG_PVOID pvConnect,UG_DWORD& dwIP,UG_WORD& wPort) = 0;
	virtual UG_ULONG		getHost(UG_DWORD& dwIP,UG_WORD& wPort) = 0;
	
public:
	virtual UG_ULONG		init(PINIT_NET_SERCER pInit,FnNetSvr pfnCB,UG_PVOID pvKey) = 0; //返回0表示正确，否则错误
	virtual UG_ULONG		init(UG_PCHAR pchPathName,FnNetSvr pfnCB,UG_PVOID pvKey) = 0; //返回0表示正确，否则错误
	virtual UG_ULONG		shutdown() = 0; //返回0表示正确，否则错误
	virtual UG_ULONG		disconnectPlayer(UG_PVOID pvConnect) = 0; //断开客户端连接,返回0表示正确，否则错误
	//写到发送缓冲中，不发数据，缓冲数据大小为ulSendSocketBufferSize，返回0正确,否则错误
	virtual UG_ULONG		writeBuffer(UG_PVOID pvConnect,UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR pchData = NULL,UG_ULONG ulLen = 0) = 0;
	//直接将数据发送出去，返回0正确，-1错误，该连接应该关闭，否则表示以发送的数据量，也是错误，该也连接应该关闭
	virtual UG_ULONG		sendData(UG_PVOID pvConnect,UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR pchData = NULL,UG_ULONG ulLen = 0) = 0;
	//发送缓冲数据，返回0正确，-1错误，该连接应该关闭，否则表示以发送的数据量，也是错误，该也连接应该关闭
	virtual UG_ULONG		sendBuffer(UG_PVOID pvConnect,UG_ULONG ulSendSize) = 0; //发送缓冲中的数据
	//接收缓冲数据，返回0正确，-1错误，该连接应该关闭，否则表示以发送的数据量
	//lRecvCount 回调个数，<1表示全部，否则表示个数
	virtual UG_ULONG		recvData(UG_PVOID pvConnect,UG_LONG lRecvCount,double& dbRecv,double& dbRecvs) = 0;
	virtual UG_ULONG		setPlayer(UG_PVOID pvConnect,UG_PVOID pvPlayer) = 0;
//	virtual UG_ULONG		sendProc(void) = 0;
	virtual UG_ULONG		acceptPlayer() = 0;
};

//-------------------------------------------------------------------------------------
#endif//(__INET_SERVER__)
