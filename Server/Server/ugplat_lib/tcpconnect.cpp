/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : TCPConnect.cpp
*owner        : Ben
*description  : 每个连接的数据
*modified     : 2004/12/20
******************************************************************************/ 

#include "IncAll.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTCPConnect::CTCPConnect()
{
	m_hSocket = NULL;
	m_dwIP = 0;
	m_wPort = 0;
	m_pchSendBuffer = NULL;
	m_pchRecvBuffer = NULL;
	m_ulRecvCount = 0;
	m_pvPlayer = NULL;
	m_ulSendError = 0;
	
	m_pchRecvBuffer = NULL;
	m_ulRecvPos = 0;

	m_pchSendBuffer = NULL;
	m_ulSendPos = 0;

	m_ulWriteSize = 0;
	m_ulSendSize = 0;
	m_dbWrite = 0.0;
	m_dbBeginer = 0.0;
	m_dwSendBufferTimer = 0;
	m_dwEncryptKey = 0;
	m_dbLastestRecvData = 0.0;
	m_bChecked = FALSE;
	m_dwRecvAllDataSize = 0;
	m_dwSendAllDataSize = 0;
}

CTCPConnect::~CTCPConnect()
{

}

UG_ULONG CTCPConnect::init()
{
	if(m_hSocket)
	{
		if(INVALID_SOCKET != m_hSocket && SOCKET_ERROR != m_hSocket)
		{
			closesocket(m_hSocket);
		}
		m_hSocket = NULL;
	}
	m_dwIP = 0;
	m_wPort = 0;
	m_ulRecvCount = 0;
	m_pvPlayer = NULL;
	
	m_ulRecvPos = 0;
	m_ulSendError = 0;
	
	m_ulSendPos = 0;

	m_ulWriteSize = 0;
	m_ulSendSize = 0;
	m_dbWrite = 0.0;
	m_dbBeginer = Plat_FloatTime();
	m_dwSendBufferTimer = 0;
	m_dwEncryptKey = 0;
	m_dbLastestRecvData = Plat_FloatTime();
	m_bChecked = FALSE;
	m_dwRecvAllDataSize = 0;
	m_dwSendAllDataSize = 0;
	return 0;
}

