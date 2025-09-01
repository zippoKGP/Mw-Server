// chatserver.cpp : Defines the entry point for the console application.
//
#include "incall.h"

CWorldbaseManager		theApp;
CUGProgram*				g_pUGProgram;
CUGLog*					g_pLog;
UG_BOOL					g_bCoutFile;

UG_ULONG UG_FUN_CALLBACK memoryLeakedProc(UG_PVOID pvKey,UG_PCHAR pchError);

int main(int argc, char* argv[])
{
	g_pUGProgram = new CUGProgram;
	g_pLog = new CUGLog;
	g_pLog->init(LOG_FILE);
	g_pUGProgram->init(UGMemoryLeaked(memoryLeakedProc),g_pLog);
	g_bCoutFile = FALSE;
	if(theApp.init(CONFIG_FILE))
	{
		printf("theApp init failed!\n");
		getchar();
		theApp.cleanup();
		ReleaseP(g_pUGProgram);
		ReleaseP(g_pLog);
		return 0;
	}

	UG_ULONG ulBeginer = Plat_MSTime();
	UG_ULONG ulPauser = 0;
	UG_ULONG ulPauses = 0;
	UG_ULONG ulAPauses = 0;
	UG_ULONG ulBeginerRender = 0;
	printf("press c or C to exit theApp!\n");
	printf("press d or D to pause!\n");
	printf("theApp init succeed, now is loop!\n");
	for(int i = 0;; i ++)
	{
		if(_kbhit())
		{
			int nHit = _getch();
			if(VK_ESCAPE == nHit)
			{
				break;
			}
			else if('d' == nHit || 'D' == nHit)
			{
				printf("\nnow is paused!\n");
				ulPauser = Plat_MSTime();
				printf("theApp loop numers is %d.\n",i);
				printf("theApp loop timers is %d.\n",ulPauser - ulBeginer - ulPauses);
				printf("theApp milliseconds/frame is %d.\n",(ulPauser - ulBeginer - ulPauses) / i);
				printf("press enter for continue!\n");
				
				getchar();
				ulAPauses = Plat_MSTime() - ulPauser;
				ulPauses += ulAPauses;
				printf("\nthis pause times is %d!\n",ulAPauses);
				
				printf("press d or D to pause!\n");
				printf("theApp continue looping!\n");
			}
			else if('c' == nHit || 'C' == nHit)
			{
				CCout::coutConfig();
			}
			else if('s' == nHit || 'S' == nHit)
			{
				CCout::coutSceneManager();
			}
			else if('p' == nHit || 'P' == nHit)
			{
				CCout::coutPlayerManager();
			}
			else if('w' == nHit || 'W' == nHit)
			{
				CCout::coutWorldbase();
			}
			else if('f' == nHit || 'F' == nHit)
			{
				g_bCoutFile = !g_bCoutFile;
			}
			else
			{
				CCout::coutAll();
			}
		}
		ulBeginerRender = Plat_MSTime();
		theApp.render(ulBeginerRender);
		ulBeginerRender = Plat_MSTime() - ulBeginerRender;
		if(ulBeginerRender < LOOP_TIMES)
		{
			Sleep(LOOP_TIMES - ulBeginerRender);
		}
	}

	printf("theApp cleanup begin!\n");
	theApp.cleanup();
	ReleaseP(g_pUGProgram);
	ReleaseP(g_pLog);
	printf("theApp cleanup end!\n");
	getchar();
	return 0;
}

UG_ULONG UG_FUN_CALLBACK memoryLeakedProc(UG_PVOID pvKey,UG_PCHAR pchError)
{
	g_pLog->UGLog(pchError);
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
