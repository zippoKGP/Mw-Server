/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
  *file name    : IClientNet.h
  *owner        : Ben
  *description  : IClientNet�ӿ����ͷ�ļ������ڿͻ�����������ӣ����ݷ��͵ȡ�
  *modified     : 2004/12/20
******************************************************************************/ 

#if !defined(AFX_ICLIENTNET_H__C11C684C_DAEA_4187_B552_5D4FA814FCA0__INCLUDED_)
#define AFX_ICLIENTNET_H__C11C684C_DAEA_4187_B552_5D4FA814FCA0__INCLUDED_

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
#define		NETCLIENT_INTERFACE_VERSION			"INetClient_003"

typedef UG_ULONG (UG_FUN_CALLBACK *FnClientNet)(UG_DWORD dwMsgID,UG_DWORD dwParam,const UG_PCHAR pchData,UG_ULONG ulLen);

class IClientNet  
{
public: //��ȡIP
	virtual UG_ULONG		initGetIP() = 0; //���ظ�����-1��ʾʧ��
	virtual UG_ULONG		getIPCount() = 0; //���ظ���
	virtual UG_DWORD		getIP(UG_ULONG ulIndex) = 0; //���IP
	virtual UG_PCHAR		convertIP(UG_DWORD dwIP) = 0;
	virtual UG_DWORD		convertIP(UG_PCHAR pchIP) = 0;
	
public:
	virtual UG_ULONG		getErrorCode() = 0;
	virtual UG_ULONG		getServer(UG_DWORD& dwIP,UG_WORD& wPort) = 0;
	virtual UG_ULONG		getHost(UG_DWORD& dwIP,UG_WORD& wPort) = 0;
	
public:
	//init��ʼ��������dwIP������IP,wPort�������˿�,dwBindIPΪ�ͻ��˱��ذ��IP,,pfnCallback�ص�����ָ��
	//bAsycΪ����ʱ��ʾ������������netFrame����ͨ���ص�����ȡ�����ݣ�netFrame�ͻص�������ͬһ�̣߳�
	//bAsycΪ��ʱ��ʾ������ʱ�ص��������Զ����У��ص���������һ�̣߳�
	//��ʾ�ͻ�����Ҫ�������˵�У��wClientCheckID���������˽��յ���ϢΪNET_MSG_CHECK_DATA��
	//���У��ͨ���򷵻�ͬ������Ϣ������Ϊ0�����򷵻�ͬ������Ϣ�����ǲ�����Ϊ0��
	virtual UG_ULONG		init(UG_DWORD dwServerIP,UG_WORD wServerPort,UG_DWORD dwBindIP,UG_DWORD dwClientCheckID,FnClientNet pfnCallback,UG_BOOL bAsyc = FALSE) = 0;
	//pchPathName�����ļ�����ʽΪini�ļ���������ĸ�ʽ����[NetConfig](ip= 192.168.0.14,bindip = 192.168.0.14,port = 34567)
	virtual UG_ULONG		init(UG_PCHAR pchPathName,FnClientNet pfnCallback,UG_BOOL bAsyc = FALSE) = 0;
	virtual UG_ULONG		disconnect() = 0;
	virtual UG_ULONG		shutdown() = 0;
	//��������
	virtual UG_ULONG		postNetMessage(UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR pchData = NULL,UG_ULONG ulLen = 0) = 0;
	//������д�����ͻ��棬�������ݣ�����λ����ָ�룬���ȡ�
	virtual UG_ULONG		writeBuffer(UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR pchData = NULL,UG_ULONG ulLen = 0) = 0;
	//���ͻ������ݡ�
	virtual UG_ULONG		sendBuffer() = 0;
	//���������������ݣ���bAsycΪ����ʱʱ�����á�
	//lRecvCount �ص�������<1��ʾȫ���������ʾ����
	virtual UG_ULONG		netFrame(UG_LONG lRecvCount = 0) = 0;
	
};

//һ�η��͵����ݴ�С���ܳ���4KB��

#endif // !defined(AFX_ICLIENTNET_H__C11C684C_DAEA_4187_B552_5D4FA814FCA0__INCLUDED_)
