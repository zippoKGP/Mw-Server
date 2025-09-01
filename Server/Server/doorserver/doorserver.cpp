// doorserver.cpp : Defines the entry point for the console application.
//
#include "incall.h"

CDoorApp		theApp;
CUGLog*			g_pLog;
UGBOOL			g_bCoutFile;

//static const char		g_szLastModify[] = {"last modify time = 2005-09-12 13:45:00\n"};
//����worldserver��������־��log��Ϣ
//static const char		g_szLastModify[] = {"last modify time = 2005-10-14 11:45:00\n"};
//����worldserver��������־��log��Ϣ
//static const char		g_szLastModify[] = {"last modify time = 2005-10-18 16:45:00\n"};
//����doorsvr��usercard�����ݿ�ѡȡʱ���packet_id��pocket������
//static const char		g_szLastModify[] = {"last modify time = 2005-10-21 10:45:00\n"};
//�޸���doorsvr����������Կ���ʱ�䳬ʱ�жϵ�bug
//static const char		g_szLastModify[] = {"last modify time = 2005-10-25 10:45:00\n"};
//����doorsvr�����������¼�յ��������ݵ����ʱ�䣬Ϊ�Ժ�೤ʱ��û�յ����ݾ��жϿͻ��˶�����׼��
//static const char		g_szLastModify[] = {"last modify time = 2005-11-07 10:45:00\n"};
//��doorsvr��������ÿ��������sleep��1ms��Ϊ5ms
//static const char		g_szLastModify[] = {"last modify time = 2005-12-05 10:45:00\n"};
//�޸���doorsvr��������log�ļ�
//static const char		g_szLastModify[] = {"last modify time = 2005-12-22 18:00:00\n"};
//����gm monitor���߲�����¼��Ϣ������һ�ű�gmmonitor��
static const char		g_szLastModify[] = {"last modify time = 2006-01-24 11:30:00\n"};
//�����޸��˿���unlock��delcard�ķ��ظ�worldsvr�Ĵ���

int main(int argc, char* argv[])
{
	char szVersion[1024];
	sprintf(szVersion,"the current door server version is : %d.%d.%d.\n",GAME_VERSION0,GAME_VERSION1,GAME_VERSION2);

	g_pLog = new CUGLog;

	char szLog[1024];
	long lTimer = 0;
	time(&lTimer);
	sprintf(szLog,"./log/doorsvr%d.log",lTimer);
	g_pLog->init(szLog);

	g_bCoutFile = FALSE;

	if(theApp.init(CONFIG_FILE))
	{
		printf(szVersion);
		printf(g_szLastModify);
		printf("door server init failed!\n");
		theApp.cleanup();
		ReleaseP(g_pLog);
		getchar();
		return 0;
	}

	UG_ULONG ulBeginerRender = 0;
	printf(szVersion);
	printf(g_szLastModify);
	printf("door server init succeed, door server is running!\n");
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
				theApp.coutConfig();
			}
			else if('G' == nHit)
			{
				theApp.coutGM();
			}
			else if('W' == nHit)
			{
				theApp.coutWB();
			}
			else if('S' == nHit)
			{
				theApp.coutWB();
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
			else
			{
				theApp.coutAll();
			}
		}
		ulBeginerRender = Plat_MSTime();
		theApp.render(ulBeginerRender);
		Sleep(LOOP_TIMES);
	}

	printf("door server begin to quit!\n");
	theApp.cleanup();
	ReleaseP(g_pLog);
	printf("door server quit ok, bye bye!\n");
	getchar();
	return 0;
}

UGPCHAR convertip(UG_DWORD dwIP)
{
	static UGCHAR szIP[256];
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
