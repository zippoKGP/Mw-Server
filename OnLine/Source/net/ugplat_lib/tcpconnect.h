/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : TCPConnect.h
*owner        : Ben
*description  : 每个连接的数据的头文件
*modified     : 2004/12/20
******************************************************************************/ 

#if !defined(AFX_TCPCONNECT_H__DE58FD7A_6293_4000_8FC2_5624E761FA92__INCLUDED_)
#define AFX_TCPCONNECT_H__DE58FD7A_6293_4000_8FC2_5624E761FA92__INCLUDED_

#include "net/UGBenDef.h"

class CTCPConnect  
{
public:
	CTCPConnect();
	virtual ~CTCPConnect();

public:
	UG_ULONG		init();

public:
	SOCKET			m_hSocket;
	UG_DWORD		m_dwIP;
	UG_WORD			m_wPort;
	UG_ULONG		m_ulRecvCount;
	UG_PVOID		m_pvPlayer;

public:
	UG_PCHAR		m_pchRecvBuffer;
	UG_ULONG		m_ulRecvPos;
	UG_ULONG		m_ulSendError;
	
public:
	UG_PCHAR		m_pchSendBuffer;
	UG_ULONG		m_ulSendPos;

	UG_ULONG		m_ulWriteSize;
	UG_ULONG		m_ulSendSize;
	double			m_dbWrite;
	double			m_dbBeginer;

public:
	UGDWORD			m_dwSendBufferTimer;
	
};

#endif // !defined(AFX_TCPCONNECT_H__DE58FD7A_6293_4000_8FC2_5624E761FA92__INCLUDED_)
