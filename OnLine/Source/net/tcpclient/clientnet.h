// ClientNet.h: interface for the CClientNet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENTNET_H__471A64AD_329C_486F_AAB2_D99977FBD697__INCLUDED_)
#define AFX_CLIENTNET_H__471A64AD_329C_486F_AAB2_D99977FBD697__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CClientNet : public IClientNet  
{
public:
	CClientNet();
	virtual ~CClientNet();

public:
	virtual UG_ULONG		initGetIP();
	virtual UG_ULONG		getIPCount();
	virtual UG_DWORD		getIP(UG_ULONG ulIndex);
	virtual UG_PCHAR		convertIP(UG_DWORD dwIP);
	virtual UG_DWORD		convertIP(UG_PCHAR pchIP);

public:
	virtual UG_ULONG		getErrorCode();
	virtual UG_ULONG		getServer(UG_DWORD& dwIP,UG_WORD& wPort);
	virtual UG_ULONG		getHost(UG_DWORD& dwIP,UG_WORD& wPort);
	
public:
	virtual UG_ULONG		init(UG_DWORD dwServerIP,UG_WORD wServerPort,UG_DWORD dwBindIP,UG_DWORD dwBindPort,FnClientNet pfnCallback,UG_BOOL bAsyc);
	virtual UG_ULONG		initNoBlock(UG_DWORD dwServerIP,UG_WORD wServerPort,UG_DWORD dwBindIP,UG_DWORD dwBindPort,FnClientNet pfnCallback,UG_BOOL bAsyc);
	virtual UG_ULONG		disconnect();
	virtual UG_ULONG		shutdown();
	virtual UG_ULONG		postNetMessage(UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR pchData = NULL,UG_ULONG ulLen = 0);
	virtual UG_ULONG		netFrame();

	virtual UG_ULONG		sendBuffer();
	//主动接收数据数据，在bAsyc为非真时时才能用。
	//lRecvCount 回调个数，<1表示全部，否则表示个数
	virtual UG_ULONG		netFrame(UG_LONG lRecvCount);
	virtual UG_ULONG		init(UG_PCHAR pchPathName,FnClientNet pfnCallback,UG_BOOL bAsyc);
	
protected:
	static	UG_ULONG	UG_FUN_CALLBACK	TCPCltCallback(PTCP_PARAM pvParam);
	UG_ULONG							TCPCltCallbackProc(PTCP_PARAM pvParam);
	
protected:
	UG_ULONG				cleanup();

private:
	CTCPComm*				m_pTCPComm;
	FnClientNet				m_pCallback;
	UG_PVOID				m_pThis;
	UG_ULONG				m_ulLastError;
//	CUGLog*					m_pLog;
	
};

#endif // !defined(AFX_CLIENTNET_H__471A64AD_329C_486F_AAB2_D99977FBD697__INCLUDED_)
