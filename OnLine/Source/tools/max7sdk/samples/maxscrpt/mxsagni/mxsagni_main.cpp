#include "MAXScrpt.h"
#include "MXSAgni.h"

extern void le_init();
extern void sf_init();
extern void AngleCtrlInit();
extern void GroupBoxInit();
extern void ImgTagInit();
extern void LinkCtrlInit();
extern void rk_init();
extern void MXSAgni_init1();
extern void MXSAgni_init2();
extern void install_i3_custom_controls();
extern void avg_init();

HMODULE hInstance = NULL;
HINSTANCE g_hInst;
bool bRunningW2K;

// following are for methods defined in W2K, but not NT
transparentBlt TransparentBlt_i;
alphaBlend AlphaBlend_i;

BOOL APIENTRY
DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(icex);
	icex.dwICC  = ICC_BAR_CLASSES;

	static BOOL controlsInit = FALSE;
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			// Hang on to this DLL's instance handle.
			hInstance = hModule;
			g_hInst = hModule;
			DisableThreadLibraryCalls(hModule);
			if ( !controlsInit )
			{
				controlsInit = TRUE;				
				// Initialize Win95 controls
 				InitCommonControls();
				InitCommonControlsEx(&icex);
				InitCustomControls(hModule);
			}
			break;
	}
		
	return(TRUE);
}

__declspec( dllexport ) void
LibInit() { 
	// do any setup here
// check to see if we are running under W2K. 
	OSVERSIONINFO os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&os);
	bRunningW2K = (os.dwPlatformId == VER_PLATFORM_WIN32_NT && os.dwMajorVersion > 4);

// following are for methods defined in W2K, but not NT
	HMODULE msimg32 = LoadLibrary("msimg32.dll");
	if (msimg32) {
		TransparentBlt_i = (transparentBlt)GetProcAddress(msimg32,"TransparentBlt");
		AlphaBlend_i = (alphaBlend)GetProcAddress(msimg32,"AlphaBlend");
	}

	le_init();
	sf_init();
	AngleCtrlInit();
	GroupBoxInit();
	ImgTagInit();
	LinkCtrlInit();
	rk_init();
	MXSAgni_init1();
	MXSAgni_init2();
	install_i3_custom_controls();
	avg_init();
}


__declspec( dllexport ) const TCHAR *
LibDescription() { return _T("QE MAXScript Extension DLL"); }

__declspec( dllexport ) ULONG
LibVersion() {  return VERSION_3DSMAX; }

