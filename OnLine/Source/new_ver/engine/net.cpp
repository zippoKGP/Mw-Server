/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2004 ALL RIGHTS RESERVED.

  *file name    : net.cpp
  *owner  		: Andy
  *description  : 
  *modified     : 2004/12/2
******************************************************************************/ 

#include "engine_global.h"
#include "interface.h"
#include "net/iclientnet.h"
#include "net.h"
#include "tier0/dbg.h"
#include "utility.h"

const char* module_name = "client_net.dll";

static IClientNet *s_pClientNet = NULL;
static IClientNet *s_pChatNet = NULL;

static bool s_bNetConnected = false;
static bool s_bChatConnected = false;

//////////////////////////////////////////////////////////////////////////
//
int initNetModule(void)
{
	CSysModule* pModule = ::Sys_LoadModule(module_name);

	if (!pModule)
	{
		Error("could not load %s", module_name);

		return -1;
	}
	
	CreateInterfaceFn net_interface = ::Sys_GetFactory(pModule);

	if (!net_interface)
	{
		Error("could not get %s's interface", module_name);
		
		return -1;
	}

	s_pClientNet = (IClientNet*) net_interface(NETCLIENT_INTERFACE_VERSION, NULL);

	if (!s_pClientNet)
	{
		Error("could not get %s's interface %s", module_name, NETCLIENT_INTERFACE_VERSION);

		return -1;
	}

	s_pChatNet = (IClientNet*) net_interface(NETCLIENT_INTERFACE_VERSION, NULL);	
	
	if (!s_pChatNet)
	{
		Error("could not get %s's interface %s", module_name, NETCLIENT_INTERFACE_VERSION);
		
		return -1;
	}
	
	return 0;
}

int shutdownNetModule(void)
{
	HMODULE hModule = ::GetModuleHandle(module_name); 
	
	if (hModule) 
	{ 
		::FreeLibrary(hModule); 
	}

	return 0;
}

//����������Ϣ����ģ�飬ͬ������ģ�飬ֻ����netFrame����ʱ����ȡ�����ݣ����ڵ��̣߳�����ֵint,0��ʾ��ȷ��-1��ʾ���󣬸ú��������溯��DWORD dwSuspend = 1һ����
int	connectServerIdle(DWORD ip, int port, FnClientNet fnMsgFunc)
{
	if (!s_bNetConnected && s_pClientNet)
	{
		if (s_pClientNet->init(ip, port, 0, 0, (FnClientNet)fnMsgFunc) == -1)
		{
			return -1;
		}
		else
		{
			
		}
		
		s_bNetConnected = true;

		return 0;
	}

	return -1;
}

//�Ͽ����ӡ�
int	disconnectServer(void)
{
	if (s_pClientNet && s_bNetConnected)
	{
		s_bNetConnected = false;

		return s_pClientNet->shutdown();
	}

	return DisconnectChatServer();
}

//�����ݷ��͸�������������ֵint,-3��������Ч��-2��ʾ���ܴ���-1��ʾ���ʹ��󣬷��򷵻ط��͵�������
int	postNetMessage(DWORD dwMsgID, DWORD dwParam, char *pchData, long lData_Len)
{
	if (s_pClientNet && s_bNetConnected)
	{
		return s_pClientNet->postNetMessage(dwMsgID, dwParam, pchData, lData_Len);
	}

	return -1;
}

//������Ϸ��Ϣ������ֵint,0��ʾ����رգ�1����һ��������Ϣ��-3��ʾ��������Ч��
int	netFrame(void)
{
	extern bool net_lock;
	
	if (s_pClientNet && s_bNetConnected && !net_lock)
	{
		return s_pClientNet->netFrame();
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//
int	ConnectChatServer(DWORD ip, int port, DWORD check_id, FnClientNet fnMsgFunc)
{
	if (!s_bChatConnected && s_pChatNet)
	{
		if (s_pChatNet->init(ip, port, 0, check_id, (FnClientNet)fnMsgFunc) == -1)
		{
			return -1;
		}
		
		s_bChatConnected = true;
		
		return 0;
	}
	
	return -1;
}

int	DisconnectChatServer(void)
{
	if (s_pChatNet && s_bChatConnected)
	{
		s_bChatConnected = false;
		
		return s_pChatNet->shutdown();
	}
	
	return -1;
}

int	PostChatNetMessage(DWORD dwMsgID, DWORD dwParam, char *pchData, long lData_Len)
{
	if (s_pChatNet && s_bChatConnected)
	{
		return s_pChatNet->postNetMessage(dwMsgID, dwParam, pchData, lData_Len);
	}
	
	return -1;
}

//������Ϸ��Ϣ������ֵint,0��ʾ����رգ�1����һ��������Ϣ��-3��ʾ��������Ч��
int	NetChatFrame(void)
{
	extern bool chat_net_lock;

	if (s_pChatNet && s_bChatConnected && !chat_net_lock)
	{
		return s_pChatNet->netFrame();
	}

	return 0;
}

int updateNetData(void)
{
	static bool net_frame = true;
	if (net_frame)
	{
		if (netFrame() == -1)
		{
			net_frame = false;
			
			display_error_message((UCHR*)MSG_LAN_DISCONNECT, true);
			exit(0);
		}
	}
	
	static bool chat_net_frame = true;
	
	if (chat_net_frame)
	{
		if (NetChatFrame() == -1)
		{
			chat_net_frame = false;
			
			display_error_message((UCHR*)MSG_LAN_DISCONNECT, true);

			exit(0);
		}		
	}

	return 0;
}
