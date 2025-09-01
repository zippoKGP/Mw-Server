/*
**      WINMAIN.CPP
**      Windows main functions.
**
**      ZJian, 2000.7.8.
*/
#include "rays.h"
#include "2dlocal.h"
#include "xsystem.h"
#include "xkiss.h"
#include "xmodule.h"
#include "xgrafx.h"
#include "xinput.h"
#include "winmain.h"
#include "xdraw.h"


HWND        g_hDDWnd = NULL;              // program window handle
BOOL        g_bProgramActive;           // is program active ?
DWORD       g_dwScreenMode = FULL_SCREEN_MODE;  // screen mode


SCREENMODE_INFO screenmode_info[NUM_SCREENMODE];


static  HINSTANCE   local_hInstance;  // program instance
static  LPCTSTR     local_DDWndClassName = "##RAYSDDWNDCLASS20010705##";// window class name
static  char        local_DDWindowName[256] = "RAYSDDWND";              // window name
static  HICON       local_hIcon = LoadIcon(NULL, IDI_APPLICATION);
static  HICON       local_hIconSm = LoadIcon(NULL, IDI_APPLICATION);
static  BOOL        local_bRunOnce = FALSE;
static  BOOL        local_bIsInitialized = FALSE;   // is program initialized sucessfully?
static  BOOL        local_bFinishProc = FALSE;  //
static  WNDPROC     local_pfnDDWndProc = (WNDPROC ) DefWindowProc;
static  BOOL        local_bEnableExit = TRUE;   //


long FAR WINAPI RaysDDWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
static  BOOL    doInit(int);
static  void    finishProc(void);
static  BOOL    AlreadyRun(void);

extern  int     gameSet(void);
extern  int     gameInit(void);
extern  int     gameMain(void);
extern  void    gameExit(void);


//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
//                   LPSTR lpCmdLine, int nCmdShow)
int WINAPI RaysDDWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                         LPSTR lpCmdLine, int nCmdShow)
{
    local_hInstance = hInstance;
    local_bFinishProc = FALSE;

    if(gameSet())
        return FALSE;

    if(local_bRunOnce)
    {
        if(!AlreadyRun())
            return FALSE;
    }

    if( ! doInit( nCmdShow ) )
        return FALSE;
    if(gameInit()) 
    {
        local_bIsInitialized = 0;
        return FALSE;
    }

    gameMain();
    //MessageBox( g_hDDWnd, "gameMain() end", "info", MB_OK);
    
    if(local_bIsInitialized)
    {
        gameExit();
        local_bIsInitialized=0;
    }
    finishProc();
    //MessageBox( g_hDDWnd, "finishProc() end", "info", MB_OK);

    DestroyWindow( g_hDDWnd );
    //MessageBox( g_hDDWnd, "DestroyWindow() end", "info", MB_OK);

    /*
    MSG msg;

    PostQuitMessage(0);
    while (GetMessage(&msg, NULL, 0, 0) )
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    */
    //MessageBox( g_hDDWnd, "message loop end", "info", MB_OK);

    if( ! UnregisterClass(local_DDWndClassName, local_hInstance) )
        LastErrorMsg("UnregisterClass");
    //MessageBox( g_hDDWnd, "UnregisterClass() end", "info", MB_OK);

    return 0;
} 


long FAR WINAPI RaysDDWndProc(HWND hWnd,UINT message, 
                              WPARAM wParam,LPARAM lParam )
{

    if(fnMouseInterrupt(message,wParam,lParam))
        return 0;

    switch(message)
    {
    case WM_ACTIVATEAPP:
        g_bProgramActive = wParam;
        if (local_bIsInitialized)
            active_modules(g_bProgramActive);
        break;
        /*
    case WM_SIZE:
        {
            // check to see if we are losing our window...
            if( SIZE_MAXHIDE == wParam || SIZE_MINIMIZED == wParam )
                g_bProgramActive = FALSE;
            else
                g_bProgramActive = TRUE;

            if (local_bIsInitialized)
                active_modules(g_bProgramActive);
            break;
        }
        */
    case WM_PAINT:
        {
            /*
            HDC hDC;
            PAINTSTRUCT ps;
            
            hDC = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            */
            if( screen_buffer && local_bIsInitialized && g_hDDWnd )
                update_screen(screen_buffer);
            break;
        }
    case WM_CREATE:
        break;
    case WM_CLOSE:
    case WM_DESTROY:
        //MessageBox(hWnd, "(DDWND) WM_DESTROY", "MSG", MB_OK);
        if( local_bEnableExit )
        {
            if (local_bIsInitialized)
            {
                gameExit();
                local_bIsInitialized = 0;
            }
            finishProc();
            PostQuitMessage(0);
            exit(0);
        }
        else
        {
            g_hDDWnd = NULL;
            g_bProgramActive = FALSE;
        }
        break;
    case WM_NCDESTROY:
        //MessageBox(hWnd, "(DDWND) WM_NCDESTROY", "MSG", MB_OK);
        break;
        /*
    case WM_SETCURSOR:
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MBUTTONDBLCLK:
    case WM_MOUSEWHEEL:
    case WM_XBUTTONDBLCLK:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
        fnMouseInterrupt(message,wParam,lParam);
        return 0;
        //break;
    case WM_NCMOUSEMOVE:
        fnMouseInterrupt(message,wParam,lParam);
        break;
        */

    case WM_KEYDOWN:
        {
            switch( wParam )
            {
            case VK_F12:
                return 0;
                //break;
            default:
                fnKeyBoardInterrupt((UCHR)(( HIWORD(lParam))%256 ));
                return 0;
                //break;
            }
            break;
        }
    case WM_KEYUP:
        {
            switch (wParam)
            {
            case VK_F12:
                switch_screen_mode();
                return 0;
                //break;
            default:
                fnKeyBoardInterrupt((UCHR)((( HIWORD(lParam))%256 ) | 0x80));
                break;
            }
            break;
        }
    }
    if( local_pfnDDWndProc )
        return local_pfnDDWndProc(hWnd,message,wParam,lParam);
    else
        return DefWindowProc(hWnd,message,wParam,lParam);
} 


static  BOOL    doInit(int nCmdShow)
{
    WNDCLASSEX  wc;
    DWORD   dwExStyle;

    // init screen mode information for varia modes --------------------------
    RaysDDInitScreenModeInfo();

    // register DirectDraw Window class --------------------------------------
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS ;
    wc.lpfnWndProc = RaysDDWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = local_hInstance;
    wc.hIcon = local_hIcon;
    wc.hIconSm = local_hIconSm;
    wc.hCursor = NULL;
    wc.hbrBackground =  screenmode_info[g_dwScreenMode].hbrBackground ;
    wc.lpszMenuName = NULL;//local_MenuName;
    wc.lpszClassName = local_DDWndClassName;
    if(! RegisterClassEx(&wc) )
    {
        LastErrorMsg("RegisterClassEx");
        return FALSE;
    }

    // review screen mode info for our seted screen mode --------------------
    RaysDDReviewScreenModeInfo( g_dwScreenMode );

#ifdef  DEBUG
    dwExStyle = 0;
#else//!DEBUG
    dwExStyle = WS_EX_TOPMOST;
#endif//DEBUG

    g_hDDWnd = CreateWindowEx(
        dwExStyle,              // extended window style
        local_DDWndClassName,   // registered class name
        local_DDWindowName,          // window name
        screenmode_info[g_dwScreenMode].dwWindowStyle,
        screenmode_info[g_dwScreenMode].rcWindow.left,
        screenmode_info[g_dwScreenMode].rcWindow.top,
        screenmode_info[g_dwScreenMode].rcWindow.right - screenmode_info[g_dwScreenMode].rcWindow.left,
        screenmode_info[g_dwScreenMode].rcWindow.bottom - screenmode_info[g_dwScreenMode].rcWindow.top,
        NULL,           // parent
        NULL,           // menu
        local_hInstance,    // instance
        NULL            // lpPara
        );
    if(!g_hDDWnd)
    {
        LastErrorMsg("CreateWindowEx");
        return FALSE;
    }

    ShowWindow(g_hDDWnd, nCmdShow);
    UpdateWindow(g_hDDWnd);

    init_modules();
    local_bIsInitialized = TRUE;
    return TRUE;
} 


static  void    finishProc()
{
    if(! local_bFinishProc )
    {
        free_modules();
        local_bFinishProc = TRUE;
    }
} 



EXPORT   int   FNBACK   FailMsg(char *s)
{
    if (local_bIsInitialized)
    {
        gameExit();
        local_bIsInitialized = 0;
    }
    finishProc();
    MessageBox(g_hDDWnd, s, local_DDWindowName, MB_OK);
    DestroyWindow( g_hDDWnd );
    PostQuitMessage(0);
    // return -1;
    exit(-1);
}


EXPORT  void    FNBACK  LastErrorMsg(LPCTSTR szTitle)
{
    LPVOID  lpMsgBuf;
    
    FormatMessage( 
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM | 
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR) &lpMsgBuf,
        0,
        NULL 
        );
    // Process any inserts in lpMsgBuf.
    // ...
    // Display the string.
    if(szTitle)
        MessageBox( NULL, (LPCTSTR)lpMsgBuf, szTitle, MB_OK | MB_ICONINFORMATION );
    else
        MessageBox( NULL, (LPCTSTR)lpMsgBuf, "INFO", MB_OK | MB_ICONINFORMATION );
    // Free the buffer.
    LocalFree( lpMsgBuf );
}


BOOL    AlreadyRun(void)
{
    HWND FirsthWnd, FirstChildhWnd;
    if((FirsthWnd = FindWindow(NULL, local_DDWindowName)) != NULL)
    {
        FirstChildhWnd = GetLastActivePopup(FirsthWnd);
        BringWindowToTop(FirsthWnd);
        if(FirsthWnd != FirstChildhWnd)
        {
            BringWindowToTop(FirstChildhWnd);
        }
        ShowWindow(FirsthWnd, SW_SHOWNORMAL);
        return FALSE;
    }
    return TRUE;
}



EXPORT  void    FNBACK  SetDDWndName( char *name )
{
    strcpy((char *)local_DDWindowName, (const char *)name);
}


EXPORT  void    FNBACK  SetDDWndIcon( HICON hIcon, HICON hIconSm )
{
    local_hIcon = hIcon;
    local_hIconSm = hIconSm;
}


EXPORT  void    FNBACK  SetDDWndRunOnce( BOOL bFlag )
{
    local_bRunOnce = bFlag;
}


EXPORT  void    FNBACK  SetDDWndScreenMode( DWORD dwMode )
{
    g_dwScreenMode = dwMode;
}


EXPORT  void    FNBACK  SetDDWndProc( WNDPROC pfn )
{
    if( pfn )   local_pfnDDWndProc = pfn;
}


EXPORT  void    FNBACK  SetDDWndEnableExit( BOOL bEnable)
{
    local_bEnableExit = bEnable;
}

//INTERNAL FUNCTIONS =======================================================================================
//
extern  void    RaysDDInitScreenModeInfo(void)
{
    LONG    nMode;

    //full screen mode
    nMode = FULL_SCREEN_MODE;
    screenmode_info[nMode].hbrBackground = NULL;
    screenmode_info[nMode].dwWindowStyle = WS_POPUP;
    //the (rcWindow)'s value must be set dynamically when run

    //windows screen mode
    nMode = WINDOWS_SCREEN_MODE;
    screenmode_info[nMode].hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
    screenmode_info[nMode].dwWindowStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX ;
    //the (rcWindow)'s value must be set dynamically when run
}


extern  void    RaysDDReviewScreenModeInfo( DWORD dwMode )
{
    RECT    rcl;

    SetRect( &rcl, 0, 0, SCREEN_WIDTH,SCREEN_HEIGHT);
    AdjustWindowRect( &rcl, 
        screenmode_info[dwMode].dwWindowStyle, 
        FALSE );
    SetRect( &screenmode_info[dwMode].rcWindow,
        rcl.left + (GetSystemMetrics(SM_CXSCREEN)-SCREEN_WIDTH)/2,
        rcl.top + (GetSystemMetrics(SM_CYSCREEN)-SCREEN_HEIGHT)/2,
        rcl.right + (GetSystemMetrics(SM_CXSCREEN)-SCREEN_WIDTH)/2,
        rcl.bottom + (GetSystemMetrics(SM_CYSCREEN)-SCREEN_HEIGHT)/2 
        );
}


extern  void    RaysDDFitWindowSize(DWORD dwMode)
{
    SetClassLong( g_hDDWnd, GCL_HBRBACKGROUND, (LONG)screenmode_info[dwMode].hbrBackground );
    SetWindowLong( g_hDDWnd, GWL_STYLE, screenmode_info[dwMode].dwWindowStyle );
    RaysDDReviewScreenModeInfo( dwMode );
    SetWindowPos( g_hDDWnd, NULL, 
        screenmode_info[dwMode].rcWindow.left,
        screenmode_info[dwMode].rcWindow.top,
        screenmode_info[dwMode].rcWindow.right - screenmode_info[dwMode].rcWindow.left,
        screenmode_info[dwMode].rcWindow.bottom - screenmode_info[dwMode].rcWindow.top,
        SWP_SHOWWINDOW );
}

//===========================================================================================================

