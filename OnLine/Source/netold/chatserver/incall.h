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
#define			WORLD_LEVEL			0		//向世界频道发送数据的等级

//-------------------------------------------------------------------------------------
#define			LOOP_TIMES			300 //ms，每次轮询的时间的最小间隔
#define			LOG_FILE			"./chatsvr.log" //log文件
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

//worldbase 连接的次序
enum PLAYER_CONNECT_STEP
{
	PLAYER_NOT_CONNECTED,
	PLAYER_CONNECTED_NOT_CHECK,
	PLAYER_CONNECT_STEP_OK
	
};

//-------------------------------------------------------------------------------------
#include "UGSQL.h"
#include "net/INetServer.h"
#include "net/BenSyncEvent.h"
#include "net/UGBenGlobalFun.h"
#include "net/UGBenlog.h"
#include "net/UGBenIni.h"
#include "net/UGBenMemoryPool.h"
#include "net/UGBenProgram.h"
#include "tier0/platform.h"
#include "vstdlib/strtools.h"
#include "interface.h"
#include "player_info.h"
#include "data.h"
#include "chat_info.h"
#include "Cout.h"

//-------------------------------------------------------------------------------------
#include "MailMsg.h"
#include "MailMsgManager.h"
#include "Team.h"
#include "Union.h"
#include "TradeRoom.h"
#include "Scene.h"
#include "Friends.h"
#include "SceneManager.h"
#include "UnionManager.h"
#include "PlayerManager.h"
#include "TeamManager.h"
#include "TradeRoomManager.h"
#include "WorldbaseConfig.h"
#include "WorldbaseManagerConfig.h"
#include "WBConnect.h"
#include "Player.h"
#include "Worldbase.h"
#include "WorldbaseManager.h"

//-------------------------------------------------------------------------------------
extern CUGProgram*			g_pUGProgram;
extern CUGLog*				g_pLog;

//-------------------------------------------------------------------------------------
UG_PCHAR					convertip(UG_DWORD dwIP);


//-------------------------------------------------------------------------------------
#endif//(__INCLUDE_ALL__)