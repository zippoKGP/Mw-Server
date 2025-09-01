/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : TCPComm.h
*owner        : Ben
*description  : TCPComm�ӿ����ͷ�ļ�������TCP���ݴ��䣬�����ͻ��˺ͷ������ˡ�
*modified     : 2004/12/20
******************************************************************************/ 

#if !defined(AFX_TCPCOMM_H__6FC703EB_8E4A_44FE_9160_A1E49F80EDD6__INCLUDED_)
#define AFX_TCPCOMM_H__6FC703EB_8E4A_44FE_9160_A1E49F80EDD6__INCLUDED_

#include "UGBenDef.h"

enum TCPCOMM_EVENT
{
	TCPCOMM_CONNECT, //�����¼�
	TCPCOMM_DISCONNECT, //�Ͽ��¼�
	TCPCOMM_RECVDATA, //���������¼�
	TCPCOMM_ERROR //�����¼�

};

typedef struct _tag_tcp_callback_param
{
	UG_PVOID			pvKey; //��ʼ��ʱ����Ĳ������̶��Ĳ���
	UG_PVOID			pvFlag; //�������ݻ��������ʱ���봫���Ĳ���
	UG_PVOID			pvConnect; //���ӱ�־���������¼�ʱ�᷵�ظ��û���ͬʱ�ڷ������˷��ͽ���ʱ��Ҫ�õ��ñ�־��
	UG_DWORD			dwIP; //���ӵĿͻ���IP��ֻ�������¼�ʱʹ��
	UG_WORD				wPort; //���ӵĿͻ���Port��ֻ�������¼�ʱʹ��
	TCPCOMM_EVENT		tcpEvent; //�����¼�
	UGGCP_T				ugGcp;
	UG_PCHAR			pchData; //���ӶϿ��¼�ʱΪNULL�����������¼�ʱΪ����ָ�룬�����¼�ʱ�Ǵ�������(Ӣ��)
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
//����ص�����
typedef UG_ULONG (UG_FUN_CALLBACK *TCPCallback)(PTCP_PARAM pvParam);

typedef struct _tag_init_server
{
	UG_DWORD			dwIP; //������IP
	UG_DWORD			dwPort; //������Port
	UG_ULONG			ulMaxConnect; //������������������������ʱ�����ӽ����������ر�
	TCPCallback			pvCallback; //�ص�����
	UG_PVOID			pvKey; //���ӻص���������ָ��
	UG_ULONG			ulSendSocketBufferSize; //Socket���㷢�ͻ����С
	UG_ULONG			ulRecvSocketBufferSize; //Socket������ջ����С������������͵����ݴ�С���Ӧ
	_tag_init_server()
	{
		dwIP = 0;
		dwPort = 0;
		ulMaxConnect = 0;
		pvCallback = NULL;
		pvKey = NULL;
		ulSendSocketBufferSize = 0;
		ulRecvSocketBufferSize = 0;
	}

} INIT_SERCER, *PINIT_SERCER;

typedef struct _tag_init_client
{
	UG_DWORD			dwIP; //������IP
	UG_DWORD			dwPort; //������Port
	UG_DWORD			dwBindIP; //�Լ��󶨵�IP
	UG_DWORD			dwBindPort; //��˿�
	UG_BOOL				bAsynEvent; //�첽����ģʽ�������Ľ������ݻ������ݵ���ʱ�Զ�����һ���߳��Զ�����
	TCPCallback			pvCallback; //�ص�����
	UG_PVOID			pvKey; //�ص���������ָ��
	UG_ULONG			ulSendSocketBufferSize; //Socket���㷢�ͻ����С��һ������²���̫�󣬷����Ӱ������������ܣ�������2KB��á�
	UG_ULONG			ulRecvSocketBufferSize; //Socket������ջ����С��ֻҪ���ڷ������˷��͵����ݴ�С����.
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
	
public: //��ȡIP
	UG_ULONG		initGetIP(); //���ظ�����-1��ʾʧ��
	UG_ULONG		getIPCount(); //���ظ���
	UG_DWORD		getIP(UG_ULONG ulIndex); //���IP
	UG_PCHAR		convertIP(UG_DWORD dwIP);
	UG_DWORD		convertIP(UG_PCHAR pchIP);
	
public: //Server
	UG_ULONG		init(PINIT_SERCER pInit); //����0��ʾ��ȷ���������
	UG_ULONG		disconnect(UG_PVOID pvConnect); //�Ͽ��ͻ�������,����0��ʾ��ȷ���������
	//д�����ͻ����У��������ݣ��������ݴ�СΪulSendSocketBufferSize������0��ȷ,�������
	UG_ULONG		writeBuffer(PUGGCP_T pUGGcp,UG_PVOID pvConnect,UG_PCHAR pchBuffer);
	//ֱ�ӽ����ݷ��ͳ�ȥ������0��ȷ��-1���󣬸�����Ӧ�ùرգ������ʾ�Է��͵���������Ҳ�Ǵ��󣬸�Ҳ����Ӧ�ùر�
	UG_ULONG		sendData(PUGGCP_T pUGGcp,UG_PVOID pvConnect,UG_PCHAR pchBuffer);
	//���ͻ������ݣ�����0��ȷ��-1���󣬸�����Ӧ�ùرգ������ʾ�Է��͵���������Ҳ�Ǵ��󣬸�Ҳ����Ӧ�ùر�
	UG_ULONG		sendBuffer(UG_PVOID pvConnect,UG_ULONG ulSendSize); //���ͻ����е�����
	//���ջ������ݣ�����0��ȷ��-1���󣬸�����Ӧ�ùرգ������ʾ�Է��͵�������
	UG_ULONG		recvData(UG_PVOID pvConnect,UG_LONG lRecvCount,double& dbRecv,double& dbRecvs);
	UG_ULONG		getConnect(UG_PVOID pvConnect,UG_DWORD& dwIP,UG_WORD& wPort,SOCKET& hSocket);
	UG_ULONG		setPlayer(UG_PVOID pvConnect,UG_PVOID pvPlayer);
	UG_ULONG		acceptPlayer();
	UG_ULONG		sendProc(void);
		
public: //Client
	UG_ULONG		init(PINIT_CLIENT pInit); //����0��ʾ��ȷ���������
	UG_ULONG		disconnect(); //�Ͽ��������������
	UG_ULONG		reconnect(PINIT_CLIENT pInit); //�ú���һ�㲻���õ������½�������
	//д�����ͻ����У��������ݣ��������ݴ�СΪulSendSocketBufferSize������0��ȷ,�������
	UG_ULONG		writeBuffer(PUGGCP_T pUGGcp,UG_PCHAR pchBuffer); //д�����ͻ����У��������ݣ��������ݴ�СΪulSendSocketBufferSize
	//ֱ�ӽ����ݷ��ͳ�ȥ������0��ȷ��-1���󣬸�����Ӧ�ùرգ������ʾ�Է��͵���������Ҳ�Ǵ��󣬸�Ҳ����Ӧ�ùر�
	UG_ULONG		sendData(PUGGCP_T pUGGcp,UG_PCHAR pchBuffer); //ֱ�ӽ����ݷ��ͳ�ȥ
	//���ͻ������ݣ�����0��ȷ��-1���󣬸�����Ӧ�ùرգ������ʾ�Է��͵���������Ҳ�Ǵ��󣬸�Ҳ����Ӧ�ùر�
	UG_ULONG		sendBuffer(); //���ͻ����е�����
	//���ջ������ݣ�����0��ȷ��-1���󣬸�����Ӧ�ùرգ������ʾ�Է��͵�������
	UG_ULONG		recvData(UG_LONG lRecvCount); //pvFlagΪ�ص�����������ָ��tp.pvFlag
	UG_ULONG		setPlayer(UG_PVOID pvPlayer);
	UG_ULONG		getServer(UG_DWORD& dwIP,UG_WORD& wPort);
	UG_ULONG		getHost(UG_DWORD& dwIP,UG_WORD& wPort);

//	UG_ULONG		sendProc(void);
	
private:
	UG_PVOID		m_pServerComm; //CTCPServer*
	UG_PVOID		m_pClientComm; //CTCPClient*
	UG_ULONG		m_ulIPCount; //CTCPClient*
	UG_PDWORD		m_pdwIP; //CTCPClient*
	
};

#endif // !defined(AFX_TCPCOMM_H__6FC703EB_8E4A_44FE_9160_A1E49F80EDD6__INCLUDED_)
