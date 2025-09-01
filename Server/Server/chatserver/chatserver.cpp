// chatserver.cpp : Defines the entry point for the console application.
//
#include "incall.h"

CWorldbaseManager		theApp;
UG_BOOL					g_bCoutFile;
CUGLog*					g_pLog;
CUGLog*					g_pLogWorld;
CUGLog*					g_pLogGM;

//static const char		g_szLastModify[] = {"last modify time = 2005-09-08 10:30:00\n"};
//2005-09-08 modify world chat times (500�˼�5s�����Ϊ30s����С5��)
//static const char		g_szLastModify[] = {"last modify time = 2005-09-21 10:30:00\n"};
//���logָ��id����ҵ����������¼
//static const char		g_szLastModify[] = {"last modify time = 2005-09-22 11:01:00\n"};
//��ӷ��ͳ�����Ϣ����
//static const char		g_szLastModify[] = {"last modify time = 2005-10-11 11:01:00\n"};
//�޸Ľ���Թ��ܣ�������Ҫһ����־���ܽ�����Թ���
//static const char		g_szLastModify[] = {"last modify time = 2005-10-26 15:01:00\n"};
//�޸Ķ��Ź��ܣ�ʹ����Ҫ��ͻ��˷���һ�������������Ϣ
//static const char		g_szLastModify[] = {"last modify time = 2005-10-30 10:20:00\n"};
//�޸��������û��֤�Ͷ��ߵ�BUG,�����˼�ʱ�������ֱ�ӶϿ��Ĺ���
//static const char		g_szLastModify[] = {"last modify time = 2005-11-02 16:50:00\n"};
//�޸�����ʱ�䣬��Ϊ100ms������һ�Σ�����CPUʹ�ã�Ϊworld��chat����һ����׼��
//static const char		g_szLastModify[] = {"last modify time = 2005-11-08 09:50:00\n"};
//�޸��˾���Ƶ�����ʱ�䣬CHAT_CHANNEL_SELLBUY_TIMER = 1000*30
//static const char		g_szLastModify[] = {"last modify time = 2005-11-09 13:50:00\n"};
//�޸��˾���Ƶ�����ˣ�������#�������Ƴ�����
//static const char		g_szLastModify[] = {"last modify time = 2005-11-10 12:05:00\n"};
//�޸��˾���Ƶ������bug
//static const char		g_szLastModify[] = {"last modify time = 2005-11-16 16:05:00\n"};
//�޸��˽���,������ʾ�ͻ��˽�������������ʾ
//static const char		g_szLastModify[] = {"last modify time = 2005-11-30 11:15:00\n"};
//���Ӿܾ�İ������Ϣ����,�޸��˶���Ϣ�洢��ʽ����Ϊ����ʱload���ݿ��������ݵ��ڴ棬�ǳ�ʱ�ڴ������ݿ�
//static const char		g_szLastModify[] = {"last modify time = 2005-12-05 10:15:00\n"};
//�޸���chatsvr��log�ļ����
//static const char		g_szLastModify[] = {"last modify time = 2005-12-16 15:15:00\n"};
//�޸��˽��Թ��ܣ��������ݿ�洢��ҽ��Թ��ܡ�
//static const char		g_szLastModify[] = {"last modify time = 2005-12-20 13:15:00\n"};
//�������س���������Ƶ��,����Ƶ��,GMƵ��,ϵͳƵ����Ϣ,������һ���˿ڡ�
static const char		g_szLastModify[] = {"last modify time = 2006-01-10 13:15:00\n"};
//���ӽ���worldbase�������Ķ�����Ϣ��


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
