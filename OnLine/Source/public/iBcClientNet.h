/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : ibcclientnet.h
  *owner  		: bob
  *description  : 
  *modified     : 2005/5/9
******************************************************************************/ 


#ifndef BC_CLIENT_NET
#define  BC_CLIENT_NET

#include "tier0/platform.h"



typedef UGDWORD (STDCALL *FnClientNet)(UGDWORD dwMsgID,UGDWORD dwParam,const UGPCHAR pchData,UGDWORD ulLen);



DLL_EXPORT	int initNetModule(UGVOID **pClientNet);

DLL_EXPORT	int shutdownNetModule(void);

DLL_EXPORT	int	connectServerIdle(UGVOID *pClientNet, UGDWORD ip, int port, FnClientNet fnMsgFunc, UGDWORD checkid = 0);

DLL_EXPORT	int	disconnectServer(UGVOID *pClientNet);

DLL_EXPORT	int	postNetMessage(UGVOID *pClientNet, UGDWORD dwMsgID, UGDWORD dwParam, char *pchData = NULL, long lData_Len = 0);

DLL_EXPORT	int	netFrame(UGVOID *pClientNet);


//////////////////////////////////////////////////////////////////////////
//
//����������Ϣ����ģ�飬ͬ������ģ�飬ֻ����netFrame����ʱ����ȡ�����ݣ����ڵ��̣߳�����ֵint,0��ʾ��ȷ��-1��ʾ���󣬸ú��������溯��DWORD dwSuspend = 1һ����


//�Ͽ����ӡ�


//�����ݷ��͸�������������ֵint,-3��������Ч��-2��ʾ���ܴ���-1��ʾ���ʹ��󣬷��򷵻ط��͵�������

//������Ϸ��Ϣ������ֵint,0��ʾ����رգ�1����һ��������Ϣ��-3��ʾ��������Ч��




 
#endif	//BC_CLIENT_NET