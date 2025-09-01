#ifndef __INCLUDE_ALL__
#define __INCLUDE_ALL__
//-------------------------------------------------------------------------------------
#pragma warning(disable:4786)
#include <winsock2.h>
#include <list>
#include <map>
#include <vector>
using namespace std;
#include <time.h>
#include "lzo.h"
#include "zlib.h"

//---------------------------------------------------------------------------------------
#include "net/Bencommon.h"
#include "tier0/platform.h"

//-------------------------------------------------------------------------------------
#ifndef __NET_CONNECT_AND_DISCONNECT_MSG__
#define __NET_CONNECT_AND_DISCONNECT_MSG__

#define		NET_MSG_CONNECT						0x00000000 //����������Ϣ
#define		NET_MSG_DISCONNECT					0x00000001 //����Ͽ���Ϣ
#define		NET_MSG_CHECK_DATA					0x00000002 //�ͻ���У����Ϣ���ڲ�ʹ��

#endif//(__NET_CONNECT_AND_DISCONNECT_MSG__)
#define		NET_MSG_CHECK_DATA_RETURN			0x00000003
#define		NET_MSG_CHECK_ONLINE				0x00000012

enum INIT_STEP
{
	NOT_INIT,
	INIT_CONTINUE,
	INIT_THREAD_END,
	INIT_OK,
};

//-------------------------------------------------------------------------------------
typedef struct _tag_memory_pool
{
	UG_PCHAR	pchFile; //�ļ���
	UG_ULONG	ulLine; //�ļ��е�����
	time_t		timer; //�����ʱ��
	UG_PVOID	pvMem; //�ڴ�ָ��
	UG_ULONG	ulLen; //�ڴ��С
	MEM_TYPE	ulType; //�ڴ�����
	_tag_memory_pool()
	{
		pchFile = NULL;
		ulLine = 0;
		timer = 0;
		pvMem = NULL;
		ulLen  = 0;
		ulType = CHAR_MEM_ARRAY;
	}
	~_tag_memory_pool()
	{
		ReleasePA<UG_PCHAR>(pchFile);
		ulLine = 0;
		timer = 0;
		pvMem = NULL;
		ulLen  = 0;
		ulType = CHAR_MEM_ARRAY;
	}
	
} MEM, *PMEM;

typedef map<UG_PVOID,UG_PVOID>			MEM_MAP;

//-------------------------------------------------------------------------------------
extern CUGProgram*			g_pUGProgram;

//-------------------------------------------------------------------------------------
static	const	UG_PCHAR	g_pchTCPCommFlag				= "UG";

//-------------------------------------------------------------------------------------
#include "TCPClient.h"
#include "TCPConnect.h"
#include "TCPServer.h"
#include "mysql/mysql.h"
#include "UGMySql.h"
#define OP	OutputDebugString
//-------------------------------------------------------------------------------------
#pragma comment(lib,"Ws2_32.lib")

//-------------------------------------------------------------------------------------
#endif//(__INCLUDE_ALL__)
