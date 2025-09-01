/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : TCPClient.h
*owner        : Ben
*description  : TCPClient接口类的头文件，用于客户端TCP数据传输
*modified     : 2004/12/20
******************************************************************************/ 

#if !defined(AFX_TCPCLIENT_H__5CAFA014_0532_4D0C_AF56_45905A76CF36__INCLUDED_)
#define AFX_TCPCLIENT_H__5CAFA014_0532_4D0C_AF56_45905A76CF36__INCLUDED_

#include "net/UGBenDef.h"


class CTCPClient  
{
public:
	CTCPClient();
	virtual ~CTCPClient();

public: //Client
	UG_ULONG		init(PINIT_CLIENT pInit);
	UG_ULONG		disconnect();
	UG_ULONG		writeBuffer(PUGGCP_T pUGGcp,UG_PCHAR pchBuffer);
	UG_ULONG		sendData(PUGGCP_T pUGGcp,UG_PCHAR pchBuffer);
	UG_ULONG		sendBuffer();
	UG_ULONG		recvData();
	UG_ULONG		setPlayer(UG_PVOID pvPlayer);
	UG_ULONG		getServer(UG_DWORD& dwIP,UG_WORD& wPort);
	UG_ULONG		getHost(UG_DWORD& dwIP,UG_WORD& wPort);
	
protected:
	UG_ULONG		sendData(UG_PCHAR pchBuffer,UG_WORD wLen);
	UG_ULONG		startupWinsock2(); //初始化winsock2.2
	UG_ULONG		checkInitData(PINIT_CLIENT pInit);
	UG_ULONG		initSocket();
	UG_ULONG		setSocketOpt(SOCKET hSocket,UG_BOOL bReuseAddr = TRUE);
	HANDLE			createThread(LPTHREAD_START_ROUTINE pfnProc,UG_PVOID pvKey,UG_INT32 n32Priority = THREAD_PRIORITY_NORMAL);
#ifdef _DEBUG
	UG_ULONG		terminateThread(HANDLE& hThread,UG_DWORD dwMilliseconds = INFINITE);
#else
	UG_ULONG		terminateThread(HANDLE& hThread,UG_DWORD dwMilliseconds = 5000);
#endif
	UG_VOID			ReleaseH(HANDLE& h);
	UG_VOID			ReleaseHS(SOCKET& h);
	UG_ULONG		mallocMemory(PINIT_CLIENT pInit);
	UG_ULONG		freeMemory();
	UG_ULONG		cleanup();
	UG_ULONG		initRecieveThread(void);
	UG_ULONG		setNoBlocking(UG_BOOL bNoBlocking = TRUE);
		
private:
	SOCKET			m_hSocket;
	PINIT_CLIENT	m_pInit;

protected:
	static	UG_DWORD		recvThread(LPARAM l);
	UG_DWORD				recvProc();

private:
	HANDLE			m_hThread;
	HANDLE			m_hRecvEvent;
	HANDLE			m_hThreadExit;
	
private:
	UG_PCHAR		m_pchSendBuffer;
	UG_PCHAR		m_pchSended;
	UG_ULONG		m_nSendBufferLen;
	
private:
	UG_PCHAR		m_pchRecvBuffer;
	UG_PCHAR		m_pchRecved;
	UG_ULONG		m_nRecvBufferLen;
	UG_ULONG		m_nRecvPos;
	UG_ULONG		m_ulEncrypted;
	UG_ULONG		m_ulCheckDataOver;
	
	UG_PCHAR		m_pchRecvBufferLzo;
	UG_ULONG		m_nRecvPosLzo;
	
private:
	UG_ULONG		m_ulErrorCount;
	UG_ULONG		m_ulRecvCount;
	UG_PVOID		m_pvPlayer;

private:
	UG_DWORD		m_dwHostIP;	
	UG_WORD			m_wHostPort;	
	
};

#endif // !defined(AFX_TCPCLIENT_H__5CAFA014_0532_4D0C_AF56_45905A76CF36__INCLUDED_)
