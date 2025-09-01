#ifndef __INCLUDE_ALL__
#define __INCLUDE_ALL__
//-------------------------------------------------------------------------------------
#pragma warning(disable:4245)
#pragma warning(disable:4786)
#pragma warning(disable:4100)
#pragma warning(disable:4512)
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <map>
#include <list>
using namespace std;
#include "resource.h"

//-------------------------------------------------------------------------------------
#include "UGSQL.h"
#include "net/UGBenini.h"
#include "net/BenSyncEvent.h"
#include "net/INetServer.h"
#include "net/UGBenGlobalFun.h"
#include "net/UGBenlog.h"
#include "net/UGBenMemoryPool.h"
#include "net/UGBenProgram.h"
#include "net/UGBenEncrypt.h"
#include "door_srv.h"
#include "login.h"
#include "net/worldbaseofdoor.h"
#include "SQLDoor.h"
#include "tier0/platform.h"
#include "vstdlib/strtools.h"
#include "interface.h"

//-------------------------------------------------------------------------------------
enum WORLDBASE_STEP
{
	WB_NOT_CONNECTED,
	WB_CONNECTED_NOT_CHECK,
	WB_CHECK_OK
};

enum GM_STEP
{
	GM_NOT_CONNECTED,
	GM_CONNECTED_NOT_CHECK,
	GM_CHECK_OK
};

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
extern CUGProgram*			g_pUGProgram;
extern CUGLog*				g_pLog;

//-------------------------------------------------------------------------------------
#define			LOOP_TIMES					10 //ms，每次轮询的时间的最小间隔
#define			MAX_CONNECT_WORLDBASE		100 //worldbase服务器的最大连结数
#define			MAX_CONNECT_MANAGER			100 //manager服务器的最大连结数
#define			LOG_FILE					"./doorsvr.log" //log文件
#define			CONFIG_FILE					"./doorsvr.ini" //配置文件
#define			TCPCOMM_SVR_DLL				"./server_net.dll" //TCP传输服务器端动态库

//-------------------------------------------------------------------------------------
#endif//(__INCLUDE_ALL__)