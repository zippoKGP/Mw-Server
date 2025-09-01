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
	READ_CONTINUE, //读出的数据有效，继续循环
	READ_BREAK, //读出的数据无效，不能循环
	READ_ERROR, //出错了
};

//-------------------------------------------------------------------------------------
#define		SIZE_MAX					1024

//-玩家新手卡记录状态-----------------------------------------------------------------
#define		USERCARD_STATUE_UNLOCK		0x00000000 //玩家的卡记录可以用
#define		USERCARD_STATUE_USED		0x00000001 //玩家的卡记录被用掉了，在必要时该记录会被移到另外的表中作日志用。
#define		USERCARD_STATUE_LOCKED		0x00000002 //玩家的卡记录被锁住了，不能用

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
#define			LOOP_TIMES					5 //ms，每次轮询的时间的最小间隔
#define			MAX_CONNECT_WORLDBASE		200 //worldbase服务器的最大连结数
#define			MAX_CONNECT_MANAGER			20 //manager服务器的最大连结数
#define			LOG_FILE					"./log/doorsvr.log" //log文件
#define			CONFIG_FILE					"./doorsvr.ini" //配置文件
#define			TCPCOMM_SVR_DLL				"./server_net.dll" //TCP传输服务器端动态库
#define			MAX_WORLD					300


//-------------------------------------------------------------------------------------
#endif//(__INCLUDE_ALL__)