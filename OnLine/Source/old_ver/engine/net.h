/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2004 ALL RIGHTS RESERVED.

  *file name    : net.h
  *owner  		: Andy
  *description  : 
  *modified     : 2004/12/2
******************************************************************************/ 

#ifndef ENGINE_NET_H
#define ENGINE_NET_H

#include "net/iclientnet.h"

//////////////////////////////////////////////////////////////////////////
//
int initNetModule(void);

int shutdownNetModule(void);

//////////////////////////////////////////////////////////////////////////
//
//����������Ϣ����ģ�飬ͬ������ģ�飬ֻ����netFrame����ʱ����ȡ�����ݣ����ڵ��̣߳�����ֵint,0��ʾ��ȷ��-1��ʾ���󣬸ú��������溯��DWORD dwSuspend = 1һ����
int	connectServerIdle(DWORD ip, int port, FnClientNet fnMsgFunc);
  
//�Ͽ����ӡ�
int	disconnectServer();

//�����ݷ��͸�������������ֵint,-3��������Ч��-2��ʾ���ܴ���-1��ʾ���ʹ��󣬷��򷵻ط��͵�������
int	postNetMessage(DWORD dwMsgID, DWORD dwParam, char *pchData = NULL, long lData_Len = 0);

//������Ϸ��Ϣ������ֵint,0��ʾ����رգ�1����һ��������Ϣ��-3��ʾ��������Ч��
int	netFrame(void);

//////////////////////////////////////////////////////////////////////////
//
int	ConnectChatServer(DWORD ip, int port, DWORD check_id, FnClientNet fnMsgFunc);

int	DisconnectChatServer();

//�����ݷ��͸�������������ֵint,-3��������Ч��-2��ʾ���ܴ���-1��ʾ���ʹ��󣬷��򷵻ط��͵�������
int	PostChatNetMessage(DWORD dwMsgID, DWORD dwParam, char *pchData = NULL, long lData_Len = 0);

//������Ϸ��Ϣ������ֵint,0��ʾ����رգ�1����һ��������Ϣ��-3��ʾ��������Ч��
int	NetChatFrame(void);

//////////////////////////////////////////////////////////////////////////
//
int updateNetData(void);

#endif