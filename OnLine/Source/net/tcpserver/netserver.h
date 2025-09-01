// NetServer.h: interface for the CNetServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETSERVER_H__921BF04E_5BDC_4207_B3C7_A13AFE84A555__INCLUDED_)
#define AFX_NETSERVER_H__921BF04E_5BDC_4207_B3C7_A13AFE84A555__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CNetServer : public INetServer  
{
public:
	CNetServer();
	virtual ~CNetServer();

public:
	virtual UG_ULONG		initGetIP();
	virtual UG_ULONG		getIPCount();
	virtual UG_DWORD		getIP(UG_ULONG ulIndex);
	virtual UG_PCHAR		convertIP(UG_DWORD dwIP);
	virtual UG_DWORD		convertIP(UG_PCHAR pchIP);

public:
	virtual UG_ULONG		getErrorCode();
	virtual UG_ULONG		getConnect(UG_PVOID pvConnect,UG_DWORD& dwIP,UG_WORD& wPort);
	virtual UG_ULONG		getHost(UG_DWORD& dwIP,UG_WORD& wPort);
	
public:
	virtual UG_ULONG		init(PINIT_NET_SERCER pInit,FnNetSvr pfnCB,UG_PVOID pvKey);
	virtual UG_ULONG		shutdown();
	virtual UG_ULONG		disconnectPlayer(UG_PVOID pvConnect);
	virtual UG_ULONG		writeBuffer(UG_PVOID pvConnect,UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR pchData,UG_ULONG ulLen);
	virtual UG_ULONG		writeBuffer(UG_PVOID pvConnect,UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR pchData,UG_ULONG ulLen,DWORD dwOverflow);
	virtual UG_ULONG		sendData(UG_PVOID pvConnect,UG_DWORD dwMsgID,UG_DWORD dwParam,UG_PCHAR pchData = NULL,UG_ULONG ulLen = 0);
	virtual UG_ULONG		sendBuffer(UG_PVOID pvConnect,UG_ULONG ulSendSize);
	virtual UG_ULONG		recvData(UG_PVOID pvConnect,double& dbRecv,double& dbRecvs);
	virtual UG_ULONG		setPlayer(UG_PVOID pvConnect,UG_PVOID pvPlayer);
	virtual UG_ULONG		acceptPlayer();
	
public:
	virtual UG_INT32		setSocket(UG_INT32 nSend,UG_INT32 nRecv);
	virtual UG_INT32		setWriteBuffer(UG_INT32 nWrite);
	virtual UG_INT32		setRecvBuffer(UG_DWORD dwRecv);

protected:
	static	UG_ULONG	UG_FUN_CALLBACK	TCPSvrCallback(PTCP_PARAM pvParam);
	UG_ULONG							TCPSvrCallbackProc(PTCP_PARAM pvParam);

protected:
	UG_ULONG				cleanup();

private:
	CTCPComm*				m_pTCPComm;
	FnNetSvr				m_pCallback;
	UG_PVOID				m_pThis;
	UG_ULONG				m_ulLastError;
	UG_PVOID				m_pvPlayer;
	CUGLog*					m_pLog;
	UG_DWORD				m_dwIP;
	UG_WORD					m_wPort;
};

#endif // !defined(AFX_NETSERVER_H__921BF04E_5BDC_4207_B3C7_A13AFE84A555__INCLUDED_)
