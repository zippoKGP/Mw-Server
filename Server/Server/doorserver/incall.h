#ifndef __INCLUDE_ALL__
#define __INCLUDE_ALL__
//-------------------------------------------------------------------------------------
enum WORLDBASE_STEP
{
	WB_NOT_CONNECTED,
	WB_CONNECTED_NOT_CHECK,
	WB_CHECK_OK
};

enum JP_CHECK
{
	JP_CHECK_LOGIN,
	JP_CHECK_LOGOUT,
	JP_CHECK_BALANCE,
	JP_CHECK_PAYMENT,
	JP_CHECK_STATE
};

enum GM_STEP
{
	GM_NOT_CONNECTED,
	GM_CONNECTED_NOT_CHECK,
	GM_CHECK_OK
};

enum READ_RET
{
	READ_CONTINUE, //������������Ч������ѭ��
	READ_BREAK, //������������Ч������ѭ��
	READ_ERROR, //������
};

//-------------------------------------------------------------------------------------
#define		SIZE_MAX					1024

//-������ֿ���¼״̬-----------------------------------------------------------------
#define		USERCARD_STATUE_UNLOCK		0x00000000 //��ҵĿ���¼������
#define		USERCARD_STATUE_USED		0x00000001 //��ҵĿ���¼���õ��ˣ��ڱ�Ҫʱ�ü�¼�ᱻ�Ƶ�����ı�������־�á�
#define		USERCARD_STATUE_LOCKED		0x00000002 //��ҵĿ���¼����ס�ˣ�������

//-------------------------------------------------------------------------------------
#pragma warning(disable:4245)
#pragma warning(disable:4786)
#pragma warning(disable:4100)
#pragma warning(disable:4512)
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <map>
#include <list>
using namespace std;
#include "resource.h"
typedef signed char        SCHR;
typedef unsigned char      UCHR;
typedef signed char        SSTR;
typedef unsigned char      USTR;
typedef signed short int   SHINT;
typedef unsigned short int UHINT;
typedef signed long int    SLONG;
typedef signed __int64     SHYPER;

//-------------------------------------------------------------------------------------
#include "tier0/platform.h"
#include "vstdlib/strtools.h"
#include "interface.h"
#include "UGSQL.h"
#include "net/UGBenini.h"
#include "net/BenSyncEvent.h"
#include "net/INetServer.h"
#include "net/UGBenGlobalFun.h"
#include "net/UGBenlog.h"
#include "net/UGBenEncrypt.h"
#include "login.h"
#include "net/worldbaseofdoor.h"
#include "SQLDoor.h"
#include "basetypes.h"
#include "gm_monitor.h"
#include "data.h"
#include "door_info.h"

//-------------------------------------------------------------------------------------
UG_PCHAR convertip(UG_DWORD dwIP);

//-------------------------------------------------------------------------------------
#include "DoorConfig.h"
#include "GM.h"
#include "Worldbase.h"
#include "GMManager.h"
#include "WorldbaseManager.h"
#include "DoorApp.h"

//-------------------------------------------------------------------------------------
extern CDoorApp				theApp;
extern CUGLog*				g_pLog;

//-------------------------------------------------------------------------------------
#define			LOOP_TIMES					5 //ms��ÿ����ѯ��ʱ�����С���
#define			MAX_CONNECT_WORLDBASE		200 //worldbase�����������������
#define			MAX_CONNECT_MANAGER			20 //manager�����������������
#define			LOG_FILE					"./log/doorsvr.log" //log�ļ�
#define			CONFIG_FILE					"./doorsvr.ini" //�����ļ�
#define			TCPCOMM_SVR_DLL				"./server_net.dll" //TCP����������˶�̬��
#define			MAX_WORLD					300


//-------------------------------------------------------------------------------------
#endif//(__INCLUDE_ALL__)