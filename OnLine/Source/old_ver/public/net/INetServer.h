/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.

  *file name    : INetServer.h
  *owner        : Ben
  *description  : INetServer�ӿ����ͷ�ļ������ڷ���������������ӣ����ݷ��ͣ����յȡ�
				  ����ͨ��һ������͵�����Ϊ2048BYTE
  *modified     : 2004/12/06
******************************************************************************/ 
#ifndef __INET_SERVER__
#define __INET_SERVER__
//-------------------------------------------------------------------------------------
#include "net/UGBenDef.h"

//----------------Add by ben in 2005-01-06---------------------------------------------
#ifndef __NET_CONNECT_AND_DISCONNECT_MSG__
#define __NET_CONNECT_AND_DISCONNECT_MSG__

#define		NET_MSG_CONNECT						0x00000000 //����������Ϣ
#define		NET_MSG_DISCONNECT					0x00000001 //����Ͽ���Ϣ
//�ͻ���У����Ϣ���ڲ�ʹ��,�������˽��յ������ݺ��ͷ��ͷ��ͷ��صĲ���Ϊ0��ʾͨ�����գ����򲻽���
#define		NET_MSG_CHECK_DATA					0x00000002

#endif//(__NET_CONNECT_AND_DISCONNECT_MSG__)

//-------------------------------------------------------------------------------------
typedef struct _tag_net_server_callback_param
{
	UG_PVOID			pvKey; //��ʼ��ʱ����Ĳ������̶��Ĳ���
	UG_PVOID			pvPlayer; //�������ݻ��������ʱ���봫���Ĳ���
	UG_PVOID			pvConnect; //���ӱ�־���������¼�ʱ�᷵�ظ��û���ͬʱ�ڷ������˷��ͽ���ʱ��Ҫ�õ��ñ�־��
	UG_DWORD			dwMsgID;
	UG_DWORD			dwParam;
	UG_PCHAR			pchData; //���ӶϿ��¼�ʱΪNULL�����������¼�ʱΪ����ָ�룬�����¼�ʱ�Ǵ�������(Ӣ��)
	UG_ULONG			ulLen; //���������¼�ʱΪ���ݴ�С�������¼�ʱ�Ǵ�����룬����Ϊ0
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
//����ص�����
typedef UG_ULONG (UG_FUN_CALLBACK *FnNetSvr)(const PNET_SERVER_PARAM p);

typedef struct _tag_init_net_server
{
	UG_DWORD			dwIP; //������IP
	UG_DWORD			dwPort; //������Port
	UG_ULONG			ulMaxConnect; //������������������������ʱ�����ӽ����������ر�
	_tag_init_net_server()
	{
		dwIP = 0;
		dwPort = 0;
		ulMaxConnect = 0;
	}
	
} INIT_NET_SERCER, *PINIT_NET_SERCER;


#define		NETSERVER_INTERFACE_VERSION			"INetServer_001"

//TCPCallback*		pvCallback; //�ص�����
//UG_PVOID			pvKey; //���ӻص���������ָ��

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
	virtual UG_ULONG		init(PINIT_NET_SERCER pInit,FnNetSvr pfnCB,UG_PVOID pvKey) = 0; //����0��ʾ��ȷ���������
	virtual UG_ULONG		init(UG_PCHAR pchPathName,FnNetSvr pfnCB,UG_PVOID pvKey) = 0; //����0��ʾ��ȷ���������
	virtual UG_ULONG		shutdown() = 0; //����0��ʾ��ȷ���������
	virtual UG_ULONG		disconnectPlayer(UG_PVOID pvConnect) = 0; //�Ͽ��ͻ�������,����0��ʾ��ȷ���������
	//д�����ͻ����У��������ݣ��������ݴ�СΪulSendSocketBufferSize������0��ȷ,�������
	virtual UG_ULONG		writeBuffer(UG_PVOID pvConnect,UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR pchData = NULL,UG_ULONG ulLen = 0) = 0;
	//ֱ�ӽ����ݷ��ͳ�ȥ������0��ȷ��-1���󣬸�����Ӧ�ùرգ������ʾ�Է��͵���������Ҳ�Ǵ��󣬸�Ҳ����Ӧ�ùر�
	virtual UG_ULONG		sendData(UG_PVOID pvConnect,UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR pchData = NULL,UG_ULONG ulLen = 0) = 0;
	//���ͻ������ݣ�����0��ȷ��-1���󣬸�����Ӧ�ùرգ������ʾ�Է��͵���������Ҳ�Ǵ��󣬸�Ҳ����Ӧ�ùر�
	virtual UG_ULONG		sendBuffer(UG_PVOID pvConnect,UG_ULONG ulSendSize) = 0; //���ͻ����е�����
	//���ջ������ݣ�����0��ȷ��-1���󣬸�����Ӧ�ùرգ������ʾ�Է��͵�������
	//lRecvCount �ص�������<1��ʾȫ���������ʾ����
	virtual UG_ULONG		recvData(UG_PVOID pvConnect,UG_LONG lRecvCount,double& dbRecv,double& dbRecvs) = 0;
	virtual UG_ULONG		setPlayer(UG_PVOID pvConnect,UG_PVOID pvPlayer) = 0;
//	virtual UG_ULONG		sendProc(void) = 0;
	virtual UG_ULONG		acceptPlayer() = 0;
};

//-------------------------------------------------------------------------------------
#endif//(__INET_SERVER__)
