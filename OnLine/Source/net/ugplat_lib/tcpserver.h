/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : TCPServer.h
*owner        : Ben
*description  : �������˵�ͷ�ļ�
*modified     : 2004/12/20
******************************************************************************/ 

#if !defined(AFX_TCPSERVER_H__EA056FD0_1A9D_4892_9A2F_72605A2A4870__INCLUDED_)
#define AFX_TCPSERVER_H__EA056FD0_1A9D_4892_9A2F_72605A2A4870__INCLUDED_

#include "net/UGBenDef.h"

class CTCPServer  
{
public:
	CTCPServer();
	virtual ~CTCPServer();

public: //Server
	UG_ULONG		init(PINIT_SERCER pInit);
	UG_ULONG		disconnect(UG_PVOID pvConnect);
	
	UGDWORD			writeBuffer(PUGGCP_T pUGGcp,UG_PVOID pvConnect,UG_PCHAR pchBuffer,UGDWORD dwOverflow);
	
	UG_ULONG		writeBuffer(PUGGCP_T pUGGcp,UG_PVOID pvConnect,UG_PCHAR pchBuffer);
	UG_ULONG		sendData(PUGGCP_T pUGGcp,UG_PVOID pvConnect,UG_PCHAR pchBuffer);
	UG_ULONG		sendBuffer(UG_PVOID pvConnect,int nSize);
	UG_ULONG		recvData(UG_PVOID pvConnect,double& dbRecv,double& dbRecvs);
	UG_ULONG		getConnect(UG_PVOID pvConnect,UG_DWORD& dwIP,UG_WORD& wPort,SOCKET& hSocket);
	UG_ULONG		setPlayer(UG_PVOID pvConnect,UG_PVOID pvPlayer);
	UG_ULONG		acceptPlayer();
	
public: //Server
	UGINT			setSocket(UGINT nSend,UGINT nRecv);
	UGINT			setWriteBuffer(UGINT nWrite);
	UGINT			setRecvBuffer(UGDWORD dwRecv);
	
protected:
	UG_ULONG		startupWinsock2();
	UG_ULONG		checkInitData(PINIT_SERCER pInit);
	UG_ULONG		initSocket();
	UG_ULONG		setSocketOpt(SOCKET hSocket,UG_BOOL bReuseAddr = TRUE);
	HANDLE			createThread(LPTHREAD_START_ROUTINE pfnProc,UG_PVOID pvKey,UG_INT32 n32Priority = THREAD_PRIORITY_NORMAL);
	UG_ULONG		terminateThread(HANDLE& hThread,UG_DWORD dwMilliseconds = 5000);
	UG_VOID			ReleaseH(HANDLE& h);
	UG_VOID			ReleaseHS(SOCKET& h);
	UG_ULONG		mallocMemory(PINIT_SERCER pInit);
	UG_ULONG		freeMemory();
	UG_ULONG		cleanup();
	
protected:
	UG_DWORD			listenProc();

private:
	SOCKET			m_hSocket;
	PINIT_SERCER	m_pInit;

private:
	CTCPConnect*				m_pConnect;
	list<CTCPConnect*>			m_listConnectIdle;
	map<UG_PVOID,CTCPConnect*>	m_mapConnect;
	CUGLog						m_log;

private:
	BOOL		m_bWSACleanup;

private:
	UGPCHAR		m_pRecvBuffer; //��������ʱ�Ĵ�С,Ĭ��Ϊ10KB
	UGDWORD		m_dwRecvBufferSize;
	
private:
	UGINT		m_nSocketSend; //���ͽ���socket�ײ�Ĵ�С,Ĭ��Ϊ20KB
	UGINT		m_nSocketRecv; //Ĭ��Ϊ10KB

private:
	UGINT		m_nWriteBuffer; //���ͻ���Ĵ�С,Ĭ��Ϊ32KB
	UGINT		m_nMessageBuffer; //�����Ϣ�Ĵ�С,Ĭ��Ϊ8KB
	UGDWORD		m_dwTickCount;

};

#endif // !defined(AFX_TCPSERVER_H__EA056FD0_1A9D_4892_9A2F_72605A2A4870__INCLUDED_)
