// TCPClient.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

CUGProgram *  g_pUGProgram;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    if(DLL_PROCESS_ATTACH == ul_reason_for_call)
	{
		g_pUGProgram = new CUGProgram;
	}
	else if(DLL_PROCESS_DETACH == ul_reason_for_call)
	{
		ReleaseP<CUGProgram*>(g_pUGProgram);
	}
	else
	{
	}
	return TRUE;
}

