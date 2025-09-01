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

#define		PLAT_WIN2K		2000 //2000
#define		PLAT_WIN98		98 //98及以下
#define		PLAT_WINXP		2003 //XP
#define		PLAT_OTHER		1003 //XP

class CTCPClient  
{
public:
	CTCPClient();
	virtual ~CTCPClient();

public: //Client
	UG_ULONG		init(PINIT_CLIENT pInit);
	UG_ULONG		initNoBlock(PINIT_CLIENT pInit);
	UG_ULONG		disconnect();
	UG_ULONG		sendData(PUGGCP_T pUGGcp,UG_PCHAR pchBuffer);
	UG_ULONG		recvData();
	UG_ULONG		setPlayer(UG_PVOID pvPlayer);
	UG_ULONG		getServer(UG_DWORD& dwIP,UG_WORD& wPort);
	UG_ULONG		getHost(UG_DWORD& dwIP,UG_WORD& wPort);

public: //
	UG_INT32		getEncrypt(UG_DWORD& dwEncrypt);

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
	UG_ULONG		m_ulCheckDataOver;
	
	UG_ULONG		m_nRecvPosLzo;
	
private:
	UGPCHAR			m_pchBufferTemp;

private:
	UG_ULONG		m_ulErrorCount;
	UG_ULONG		m_ulRecvCount;
	UG_PVOID		m_pvPlayer;

private:
	UG_DWORD		m_dwHostIP;	
	UG_WORD			m_wHostPort;	
	CUGLog			m_log;
	double			m_dbBeginer;

private:
	UGBOOL			m_bWSACleanup;
	UGBOOL			m_bDisconnected;
	
protected:
	static	UG_DWORD		initThread(LPARAM l);
	UG_DWORD				initProc();
	
private:
	INIT_STEP		m_stepInit;
	BOOL			m_bInitSucceed;
	HANDLE			m_hInitThread;
	INIT_CLIENT		m_init;
	CCritical		m_cs;
	
private:
	DWORD			m_dwRecvAllDataSize;
	
};

#endif // !defined(AFX_TCPCLIENT_H__5CAFA014_0532_4D0C_AF56_45905A76CF36__INCLUDED_)
