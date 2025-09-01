/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : TCPComm.h
*owner        : Ben
*description  : TCPComm接口类的头文件，用于TCP数据传输，包含客户端和服务器端。
*modified     : 2004/12/20
******************************************************************************/ 

#if !defined(AFX_TCPCOMM_H__6FC703EB_8E4A_44FE_9160_A1E49F80EDD6__INCLUDED_)
#define AFX_TCPCOMM_H__6FC703EB_8E4A_44FE_9160_A1E49F80EDD6__INCLUDED_

#include "UGBenDef.h"

enum TCPCOMM_EVENT
{
	TCPCOMM_CONNECT, //连接事件
	TCPCOMM_DISCONNECT, //断开事件
	TCPCOMM_RECVDATA, //接收数据事件
	TCPCOMM_ERROR //错误事件

};

typedef struct _tag_tcp_callback_param
{
	UG_PVOID			pvKey; //初始化时传入的参数，固定的参数
	UG_PVOID			pvFlag; //发送数据或接收数据时传入传出的参数
	UG_PVOID			pvConnect; //连接标志，在连接事件时会返回给用户，同时在服务器端发送接收时需要用到该标志。
	UG_DWORD			dwIP; //连接的客户端IP，只在连接事件时使用
	UG_WORD				wPort; //连接的客户端Port，只在连接事件时使用
	TCPCOMM_EVENT		tcpEvent; //网络事件
	UGGCP_T				ugGcp;
	UG_PCHAR			pchData; //连接断开事件时为NULL，接收数据事件时为数据指针，错误事件时是错误描述(英文)
	UG_ULONG			ulLen;
	_tag_tcp_callback_param()
	{
		pvKey = NULL;
		pvFlag = NULL;
		pvConnect = NULL;
		dwIP = 0;
		wPort = 0;
		tcpEvent = TCPCOMM_ERROR;
		pchData = NULL;
	}
	
} TCP_PARAM, *PTCP_PARAM;
//网络回调函数
typedef UG_ULONG (UG_FUN_CALLBACK *TCPCallback)(PTCP_PARAM pvParam);

typedef struct _tag_init_server
{
	UG_DWORD			dwIP; //服务器IP
	UG_DWORD			dwPort; //服务器Port
	UG_ULONG			ulMaxConnect; //最大连接数，到达最大连接数时，连接将会立即被关闭
	TCPCallback			pvCallback; //回调函数
	UG_PVOID			pvKey; //连接回调函数传出指针
	_tag_init_server()
	{
		dwIP = 0;
		dwPort = 0;
		ulMaxConnect = 0;
		pvCallback = NULL;
		pvKey = NULL;
	}

} INIT_SERCER, *PINIT_SERCER;

typedef struct _tag_init_client
{
	UG_DWORD			dwIP; //服务器IP
	UG_DWORD			dwPort; //服务器Port
	UG_DWORD			dwBindIP; //自己绑定的IP
	UG_DWORD			dwBindPort; //邦定端口
	UG_BOOL				bAsynEvent; //异步处理模式，主动的接收数据或有数据到达时自动在另一个线程自动调用
	TCPCallback			pvCallback; //回调函数
	UG_PVOID			pvKey; //回调函数传出指针
	UG_ULONG			ulSendSocketBufferSize; //Socket基层发送缓存大小，一般情况下不能太大，否则会影响服务器的性能，不大于2KB最好。
	UG_ULONG			ulRecvSocketBufferSize; //Socket基层接收缓存大小，只要大于服务器端发送的数据大小即可.
	_tag_init_client()
	{
		dwIP = 0;
		dwPort = 0;
		dwBindIP = 0;
		dwBindPort = 0;
		bAsynEvent = FALSE;
		pvCallback = NULL;
		pvKey = NULL;
		ulSendSocketBufferSize = 0;
		ulRecvSocketBufferSize = 0;
	}

} INIT_CLIENT, *PINIT_CLIENT;

class CTCPComm  
{
public:
	CTCPComm();
	virtual ~CTCPComm();

public:
	UG_ULONG		cleanup();
	
public: //获取IP
	UG_ULONG		initGetIP(); //返回个数，-1表示失败
	UG_ULONG		getIPCount(); //返回个数
	UG_DWORD		getIP(UG_ULONG ulIndex); //获得IP
	UG_PCHAR		convertIP(UG_DWORD dwIP);
	UG_DWORD		convertIP(UG_PCHAR pchIP);
	
public: //Server
	UG_ULONG		init(PINIT_SERCER pInit); //返回0表示正确，否则错误
	UG_ULONG		initNoBlock(PINIT_CLIENT pInit);
	UG_ULONG		disconnect(UG_PVOID pvConnect); //断开客户端连接,返回0表示正确，否则错误
	//写到发送缓冲中，不发数据，缓冲数据大小为ulSendSocketBufferSize，返回0正确,否则错误
	UG_ULONG		writeBuffer(PUGGCP_T pUGGcp,UG_PVOID pvConnect,UG_PCHAR pchBuffer);
	UG_ULONG		writeBuffer(PUGGCP_T pUGGcp,UG_PVOID pvConnect,UG_PCHAR pchBuffer,DWORD dwOverflow);
	//直接将数据发送出去，返回0正确，-1错误，该连接应该关闭，否则表示以发送的数据量，也是错误，该也连接应该关闭
	UG_ULONG		sendData(PUGGCP_T pUGGcp,UG_PVOID pvConnect,UG_PCHAR pchBuffer);
	//发送缓冲数据，返回0正确，-1错误，该连接应该关闭，否则表示以发送的数据量，也是错误，该也连接应该关闭
	UG_ULONG		sendBuffer(UG_PVOID pvConnect,UG_ULONG ulSendSize); //发送缓冲中的数据
	//接收缓冲数据，返回0正确，-1错误，该连接应该关闭，否则表示以发送的数据量
	UG_ULONG		recvData(UG_PVOID pvConnect,double& dbRecv,double& dbRecvs);
	UG_ULONG		getConnect(UG_PVOID pvConnect,UG_DWORD& dwIP,UG_WORD& wPort,SOCKET& hSocket);
	UG_ULONG		setPlayer(UG_PVOID pvConnect,UG_PVOID pvPlayer);
	UG_ULONG		acceptPlayer();
	UG_ULONG		sendProc(void);

public: //Server
	UG_INT32		setSocket(UG_INT32 nSend,UG_INT32 nRecv);
	UG_INT32		setWriteBuffer(UG_INT32 nWrite);
	UG_INT32		setRecvBuffer(UG_DWORD dwRecv);
	
public: //Client
	UG_ULONG		init(PINIT_CLIENT pInit); //返回0表示正确，否则错误
	UG_ULONG		disconnect(); //断开与服务器端连接
	UG_ULONG		reconnect(PINIT_CLIENT pInit); //该函数一般不会用到，重新建立连接
	//直接将数据发送出去，返回0正确，-1错误，该连接应该关闭，否则表示以发送的数据量，也是错误，该也连接应该关闭
	UG_ULONG		sendData(PUGGCP_T pUGGcp,UG_PCHAR pchBuffer); //直接将数据发送出去
	//接收缓冲数据，返回0正确，-1错误，该连接应该关闭，否则表示以发送的数据量
	UG_ULONG		recvData(); //pvFlag为回调函数传出的指针tp.pvFlag
	UG_ULONG		setPlayer(UG_PVOID pvPlayer);
	UG_ULONG		getServer(UG_DWORD& dwIP,UG_WORD& wPort);
	UG_ULONG		getHost(UG_DWORD& dwIP,UG_WORD& wPort);

//	UG_ULONG		sendProc(void);
	
private:
	UG_PVOID		m_pServerComm; //CTCPServer*
	UG_PVOID		m_pClientComm; //CTCPClient*
	UG_ULONG		m_ulIPCount; //CTCPClient*
	UG_PDWORD		m_pdwIP; //CTCPClient*

private:
	int		m_nRecv;
	int		m_nSend;
	int		m_nWrite;
	DWORD	m_dwRecv;
	
};

#endif // !defined(AFX_TCPCOMM_H__6FC703EB_8E4A_44FE_9160_A1E49F80EDD6__INCLUDED_)
