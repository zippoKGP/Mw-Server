// chatserver.cpp : Defines the entry point for the console application.
//
#include "incall.h"

CWorldbaseManager		theApp;
UG_BOOL					g_bCoutFile;
CUGLog*					g_pLog;
CUGLog*					g_pLogWorld;
CUGLog*					g_pLogGM;

//static const char		g_szLastModify[] = {"last modify time = 2005-09-08 10:30:00\n"};
//2005-09-08 modify world chat times (500人加5s，最大为30s，最小5秒)
//static const char		g_szLastModify[] = {"last modify time = 2005-09-21 10:30:00\n"};
//添加log指定id的玩家的所有聊天记录
//static const char		g_szLastModify[] = {"last modify time = 2005-09-22 11:01:00\n"};
//添加发送场景信息功能
//static const char		g_szLastModify[] = {"last modify time = 2005-10-11 11:01:00\n"};
//修改解禁言功能，增加需要一个标志才能解除禁言功能
//static const char		g_szLastModify[] = {"last modify time = 2005-10-26 15:01:00\n"};
//修改顶号功能，使顶号要想客户端发送一个登入聊天的消息
//static const char		g_szLastModify[] = {"last modify time = 2005-10-30 10:20:00\n"};
//修改玩家连接没验证就断线的BUG,增加了及时将该玩家直接断开的功能
//static const char		g_szLastModify[] = {"last modify time = 2005-11-02 16:50:00\n"};
//修改心跳时间，该为100ms才心跳一次，降低CPU使用，为world和chat放在一起做准备
//static const char		g_szLastModify[] = {"last modify time = 2005-11-08 09:50:00\n"};
//修改了经济频道间隔时间，CHAT_CHANNEL_SELLBUY_TIMER = 1000*30
//static const char		g_szLastModify[] = {"last modify time = 2005-11-09 13:50:00\n"};
//修改了经济频道过滤，增加了#后面字移除过滤
//static const char		g_szLastModify[] = {"last modify time = 2005-11-10 12:05:00\n"};
//修改了经济频道过滤bug
//static const char		g_szLastModify[] = {"last modify time = 2005-11-16 16:05:00\n"};
//修改了禁言,增加提示客户端交罚金或禁言中提示
//static const char		g_szLastModify[] = {"last modify time = 2005-11-30 11:15:00\n"};
//增加拒绝陌生人消息功能,修改了短消息存储方式，改为登入时load数据库所有内容到内存，登出时在存入数据库
//static const char		g_szLastModify[] = {"last modify time = 2005-12-05 10:15:00\n"};
//修改了chatsvr的log文件输出
//static const char		g_szLastModify[] = {"last modify time = 2005-12-16 15:15:00\n"};
//修改了禁言功能，增加数据库存储玩家禁言功能。
//static const char		g_szLastModify[] = {"last modify time = 2005-12-20 13:15:00\n"};
//增加向监控程序发送世界频道,经济频道,GM频道,系统频道信息,增加了一个端口。
static const char		g_szLastModify[] = {"last modify time = 2006-01-10 13:15:00\n"};
//增加接受worldbase发过来的短信消息。


int main(int argc, char* argv[])
{
	char szVersion[1024];
	sprintf(szVersion,"the current chat server version is : %d.%d.%d.\n",GAME_VERSION0,GAME_VERSION1,GAME_VERSION2);

	char szLog[1024];
	long lTimer = 0;
	time(&lTimer);
	sprintf(szLog,"./log/charsvr%d.log",lTimer);
	g_pLog = new CUGLog;
	g_pLog->init(szLog);
	
	sprintf(szLog,"./log/worldcht%d.log",lTimer);
	g_pLogWorld = new CUGLog;
	g_pLogWorld->init(szLog);

	sprintf(szLog,"./log/gmcht%d.log",lTimer);
	g_pLogGM = new CUGLog;
	g_pLogGM->init(szLog);

	g_bCoutFile = FALSE;
	if(theApp.init(CONFIG_FILE))
	{
		printf(szVersion);
		printf(g_szLastModify);
		printf("init chat server failed!\n");
		getchar();
		theApp.cleanup();
		ReleaseP(g_pLog);
		ReleaseP(g_pLogWorld);
		return 0;
	}

	UG_ULONG ulBeginer = Plat_MSTime();
	UG_ULONG ulPauser = 0;
	UG_ULONG ulPauses = 0;
	UG_ULONG ulAPauses = 0;
	UG_ULONG ulBeginerRender = 0; 
	printf(szVersion);
	printf(g_szLastModify);
	printf("init chat server succeed, the server is running!\n");

	UG_ULONG ulHearter = Plat_MSTime();

	for(int i = 0;; i ++)
	{
		if(_kbhit())
		{
			int nHit = toupper(_getch());
			if(VK_ESCAPE == nHit)
			{
				break;
			}
			else if('C' == nHit)
			{
				CCout::coutConfig();
			}
			else if('S' == nHit)
			{
				CCout::coutSceneManager();
			}
			else if('P' == nHit)
			{
				CCout::coutPlayerManager();
			}
			else if('W' == nHit)
			{
				CCout::coutWorldbase();
			}
			else if('N' == nHit)
			{
				CCout::coutPlayerCount();
			}
			else if('F' == nHit)
			{
				g_bCoutFile = !g_bCoutFile;
			}
			else if('V' == nHit)
			{
				printf(szVersion);
				printf(g_szLastModify);
			}
			else if('R' == nHit)
			{
				ReleaseP(g_pLogWorld);
				printf("now server is pause, you can copy file worldcht.log.\n");
				printf("press any key to continue.\n");
				getchar();
				g_pLogWorld = new CUGLog;
				g_pLogWorld->init(LOG_WORLD);
			}
			else if('G' == nHit)
			{
				ReleaseP(g_pLogGM);
				printf("now server is pause, you can copy file gmcht.log.\n");
				printf("press any key to continue.\n");
				getchar();
				g_pLogGM = new CUGLog;
				g_pLogGM->init(LOG_GM);
			}
			else
			{
				CCout::coutAll();
			}
		}
		ulBeginerRender = Plat_MSTime();
		if(ulBeginerRender - ulHearter > 100)
		{
			theApp.render(ulBeginerRender);
			ulHearter = ulBeginerRender;
		}
		else
		{
			Sleep(10);
		}
	}

	printf("the chat server begin to quit!\n");
	theApp.cleanup();
	ReleaseP(g_pLog);
	ReleaseP(g_pLogWorld);
	ReleaseP(g_pLogGM);
	printf("the chat server quit ok, bye bye!\n");
	getchar();
	return 0;
}

UG_PCHAR convertip(UG_DWORD dwIP)
{
	static UG_CHAR szIP[256];
	try
	{
		sprintf(szIP,"%d.%d.%d.%d",dwIP & 0x000000ff,(dwIP & 0x0000ff00) >> 8,(dwIP & 0x00ff0000) >> 16,(dwIP & 0xff000000) >> 24);
		szIP[255] = '\0';
	}
	catch(...)
	{
		g_pLog->UGLog("convertip error.");
		szIP[255] = '\0';
	}
	return szIP;
}
