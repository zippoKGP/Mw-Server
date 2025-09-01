#ifndef __INCLUDE_ALL__
#define __INCLUDE_ALL__
//-------------------------------------------------------------------------------------
#include <windows.h>
#pragma warning(disable:4786)
#pragma warning(disable:4100)
#pragma warning(disable:4512)
#pragma warning(disable:4245)
#include <stdio.h>
#include <time.h>
#include <conio.h>
#include <list>
#include <map>
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
#define			WORLD_LEVEL						50		//向世界频道发送数据的等级
#define			WRITE_OVERFLOW					20000
#define			WRITE_WORLDBASE_OVERFLOW		400000
#define			MAXNUM_GMTOOLS					50
#define			MAX_MUTE						500000

//-------------------------------------------------------------------------------------
#define			LOG_FILE			"./log/chatsvr.log" //log文件
#define			LOG_WORLD			"./log/worldcht.log" //log文件
#define			LOG_GM				"./log/gmcht.log" //log文件
#define			CONFIG_FILE			"./chatsvr.ini" //配置文件
#define			TCPCOMM_SVR_DLL		"./server_net.dll"

//-------------------------------------------------------------------------------------
//worldbase 连接的次序
enum WB_CONNECT_STEP
{
	WB_NOT_CONNECTED,
	WB_CONNECTED_NOT_CHECK,
	WB_CHECKED_NOT_SENDBASEINFO,
	WB_CONNECT_STEP_OK
	
};

//GMTools 连接的次序
enum GM_CONNECT_STEP
{
	GM_NOT_CONNECTED,
	GM_CONNECTED_NOT_CHECK,
	GM_CONNECT_STEP_OK
		
};

//worldbase 连接的次序
enum PLAYER_CONNECT_STEP
{
	PLAYER_NOT_CONNECTED,
	PLAYER_CONNECTED_NOT_CHECK,
	PLAYER_CONNECT_STEP_OK
	
};

//-------------------------------------------------------------------------------------
#include "tier0/platform.h"
#include "vstdlib/strtools.h"
#include "interface.h"
#include "net/INetServer.h"
#include "UGSQL.h"
#include "net/BenSyncEvent.h"
#include "net/UGBenGlobalFun.h"
#include "net/UGBenlog.h"
#include "net/UGBenIni.h"
#include "basetypes.h"
#include "player_info.h"
#include "chat_info.h"
#include "data.h"

//-------------------------------------------------------------------------------------
#include "Cout.h"
#include "StopTalk.h"
#include "StopTalkManager.h"
#include "MailMsg.h"
#include "MailMsgManager.h"
#include "Scene.h"
#include "Friends.h"
#include "SceneManager.h"
#include "WorldbaseConfig.h"
#include "WorldbaseManagerConfig.h"
#include "WBConnect.h"
#include "Player.h"
#include "PlayerManager.h"
#include "GMTools.h"
#include "Worldbase.h"
#include "WorldbaseManager.h"

//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
UG_PCHAR					convertip(UG_DWORD dwIP);

//------ ben add in 2005-09-21---------------------------------------------------------
#define		LOG_GM_TYPE_WORLD				1
#define		LOG_GM_TYPE_CURRENT				2
#define		LOG_GM_TYPE_UNION				3
#define		LOG_GM_TYPE_TEAM				4
#define		LOG_GM_TYPE_PRIVATE				5
#define		LOG_GM_TYPE_MAIL				6

//-------------------------------------------------------------------------------------
extern CWorldbaseManager	theApp;
extern CUGLog*				g_pLog;
extern CUGLog*				g_pLogWorld;
extern CUGLog*				g_pLogGM;

//-------------------------------------------------------------------------------------
#endif//(__INCLUDE_ALL__)