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
#include "raysime.h"


HWND        g_hDDWnd = NULL;            // program window handle
BOOL        g_bProgramActive;           // is program active ?
DWORD       g_dwScreenMode = FULL_SCREEN_MODE;  // screen mode
RECT        g_rcWindow;         //

BOOL		g_bCenterWindow = true;
SLONG		g_nScreenX = 0;
SLONG		g_nScreenY = 0;
SLONG       g_nScreenWidth;       //screen pixel width
SLONG       g_nScreenHeight;      //screen pixel height
BOOL        g_bIsInitialized = FALSE;   // is program initialized sucessfully?
BOOL        g_enable_f12_switch_screen = TRUE;

static  HINSTANCE   local_hInstance;  // program instance
static  LPCTSTR     local_DDWndClassName = "##RAYSDDWNDCLASS20010705##";// window class name
static  char        local_DDWindowName[256] = "RAYSDDWND";              // window name
static  HICON       local_hIcon = LoadIcon(NULL, IDI_APPLICATION);
static  HICON       local_hIconSm = LoadIcon(NULL, IDI_APPLICATION);
static  BOOL        local_bRunOnce = FALSE;
static  BOOL        local_bFinishProc = FALSE;  //
static  WNDPROC     local_pfnDDWndProc = (WNDPROC ) DefWindowProc;
static  BOOL        local_bEnableExit = TRUE;   //
static  void      (*local_pfnDoDropFiles)( int nFiles, int iFile, char *filename ) = NULL;
static  BOOL        local_bEnableDblClk = FALSE;

long FAR WINAPI RaysDDWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
static  BOOL    doInit(int);
static  BOOL    AlreadyRun(void);

extern  int     gameSet(void);
extern  int     gameInit(void);
extern  int     gameMain(void);
extern  void    gameExit(void);

#define KEY_DOWN(key) ((GetKeyState(key) & 0x8000) == 0x8000)

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
//                   LPSTR lpCmdLine, int nCmdShow)
int WINAPI RaysDDWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                         LPSTR lpCmdLine, int nCmdShow)
{
    local_hInstance = hInstance;
    local_bFinishProc = FALSE;

    //Jack, 2002.5.6.
    local_pfnDDWndProc = RaysDDWndProc;

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
        g_bIsInitialized = 0;
        return FALSE;
    }

    gameMain();
    //MessageBox( g_hDDWnd, "gameMain() end", "info", MB_OK);
    
    if(g_bIsInitialized)
    {
        gameExit();
        g_bIsInitialized=0;
    }
    FinishProc();
    //MessageBox( g_hDDWnd, "FinishProc() end", "info", MB_OK);

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
        {
            g_bProgramActive = wParam;
            if (g_bIsInitialized)
                active_modules(g_bProgramActive);
            break;
        }
    case WM_SYSCOMMAND:
        // Prevent moving/sizing and power loss in fullscreen mode
        {
            switch( wParam )
            {
            case SC_MOVE:
            case SC_SIZE:
            case SC_MAXIMIZE:
            case SC_MONITORPOWER:
                if( g_dwScreenMode == FULL_SCREEN_MODE)
                    return TRUE;
            }
            break;
        }
    case WM_GETMINMAXINFO:
        {
            // Don't allow resizing in windowed mode.  
            // Fix the size of the window to (screen width * screen height) (client size)
            MINMAXINFO* pMinMax = (MINMAXINFO*) lParam;
            DWORD   dwStyle, dwExStyle;
            RECT    rc;

            dwStyle = GetWindowLong( hWnd, GWL_STYLE );
            dwExStyle = GetWindowLong( hWnd, GWL_EXSTYLE );
            SetRect( &rc, 0, 0, g_nScreenWidth, g_nScreenHeight);
            AdjustWindowRectEx( &rc, dwStyle, NULL, dwExStyle );

            pMinMax->ptMinTrackSize.x = rc.right - rc.left;
            pMinMax->ptMinTrackSize.y = rc.bottom - rc.top;
            
            pMinMax->ptMaxTrackSize.x = pMinMax->ptMinTrackSize.x;
            pMinMax->ptMaxTrackSize.y = pMinMax->ptMinTrackSize.y;

            /*
            sprintf((char *)print_rec, "WM_GETMINMAXINFO (rc)= (%d, %d)-(%d, %d)=(%d x %d)", 
                rc.left, rc.top, rc.right, rc.bottom,
                rc.right - rc.left, rc.bottom - rc.top);
            MessageBox( NULL, (LPCTSTR)print_rec, "INFO", MB_OK | MB_ICONINFORMATION );
            */
            return 0L;
        }

    case WM_MOVE:
        {
            // Retrieve the window position after a move.
            update_display_bounds();
            return 0L;
        }
        
    case WM_SIZE:
        {
            // Check to see if we are losing our window...
            if( SIZE_MAXHIDE == wParam || SIZE_MINIMIZED == wParam )
                g_bProgramActive = FALSE;
            else
                g_bProgramActive = TRUE;

            update_display_bounds();

            if (g_bIsInitialized)
                active_modules(g_bProgramActive);
            break;
        }
        
    case WM_PAINT:
        {
			PAINTSTRUCT ps;
			BeginPaint(hWnd,&ps);
			EndPaint(hWnd,&ps);
			if( screen_buffer && g_bIsInitialized && g_hDDWnd )
				update_screen(screen_buffer);
			return 0;
        }

    case WM_CREATE:
        break;

    case WM_CLOSE:
#if 0
		return 1;
		break;
#endif
    case WM_DESTROY:
        //MessageBox(hWnd, "(DDWND) WM_DESTROY", "MSG", MB_OK);
        if( local_bEnableExit )
        {
            if (g_bIsInitialized)
            {
                gameExit();
                g_bIsInitialized = 0;
            }
            FinishProc();
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

	case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
        {
            /*
            lParam : Specifies the repeat count, scan code, extended-key flag, context code, previous key-state 
                     flag, and transition-state flag, as shown in the following table. 
            Value Description :
                0每15   Specifies the repeat count for the current message. The value is the number of times the 
                        keystroke is autorepeated as a result of the user holding down the key. If the keystroke 
                        is held long enough, multiple messages are sent. However, the repeat count is not cumulative. 
                16每23  Specifies the scan code. The value depends on the original equipment manufacturer (OEM). 
                24      Specifies whether the key is an extended key, such as the right-hand ALT and CTRL keys 
                        that appear on an enhanced 101- or 102-key keyboard. The value is 1 if it is an extended key; 
                        otherwise, it is 0. 
                25每28  Reserved; do not use. 
                29      Specifies the context code. The value is always 0 for a WM_KEYDOWN message. 
                30      Specifies the previous key state. The value is 1 if the key is down before the message is sent, 
                        or it is zero if the key is up. 
                31      Specifies the transition state. The value is always zero for a WM_KEYDOWN message. 
            */
            switch( wParam )
            {
            case VK_RETURN:
				{
					short key = GetKeyState(VK_MENU);

					if (key & 0x8000)
					{
						if(g_enable_f12_switch_screen)
						{
							if( g_dwScreenMode == WINDOWS_SCREEN_MODE )
							GetWindowRect( hWnd, &g_rcWindow );
						
							switch_screen_mode();

							return  1;
						}						
					}
					else
					{
						fnKeyBoardInterrupt((UCHR)(( HIWORD(lParam))%256 ));
					}
                }
                break;

			default:
				{
					fnKeyBoardInterrupt((UCHR)(( HIWORD(lParam))%256 ));
				}
                break;
            }
        }
        break;

    case WM_KEYUP:
        {
            /*
            lParam : Specifies the repeat count, scan code, extended-key flag, context code, previous key-state 
                    flag, and transition-state flag, as shown in the following table. 
            Value Description :
                0每15   Specifies the repeat count for the current message. The value is the number of times the 
                        keystroke is autorepeated as a result of the user holding down the key. The repeat count 
                        is always one for a WM_KEYUP message. 
                16每23  Specifies the scan code. The value depends on the original equipment manufacturer (OEM). 
                24      Specifies whether the key is an extended key, such as the right-hand ALT and CTRL keys 
                        that appear on an enhanced 101- or 102-key keyboard. The value is 1 if it is an extended 
                        key; otherwise, it is 0. 
                25每28  Reserved; do not use. 
                29      Specifies the context code. The value is always 0 for a WM_KEYUP message. 
                30      Specifies the previous key state. The value is always 1 for a WM_KEYUP message. 
                31      Specifies the transition state. The value is always 1 for a WM_KEYUP message. 
            */
            switch (wParam)
            {
				
			case VK_SNAPSHOT:
				capture_screen();
				return true;
				break;
				
            default:
                fnKeyBoardInterrupt((UCHR)((( HIWORD(lParam))%256 ) | 0x80));
                return  0;
                //break;
            }
        }
        break;

    case WM_DROPFILES:
        {
            HDROP   hDrop = (HDROP) wParam;
            POINT   pt;
            UINT    i, nFiles;
            CHAR    szFile[_MAX_FNAME];

            //sprintf((char *)print_rec, "Drop files start -------------------------");
            //log_error(1, print_rec);

            DragQueryPoint( hDrop, &pt);
            nFiles = DragQueryFile( hDrop, 0xFFFFFFFF, NULL, 0 );
            for( i = 0; i < nFiles; i ++ )
            {
                DragQueryFile( hDrop, i, szFile, _MAX_FNAME );
                if( local_pfnDoDropFiles )
                {
                    local_pfnDoDropFiles( nFiles, i, szFile );
                }
                //sprintf((char *)print_rec, "  [%d] %s", i, szFile );
                //log_error(1, print_rec);
            }
            DragFinish( hDrop );

            //sprintf((char *)print_rec, "Drop files end ---------------------------");
            //log_error(1, print_rec);
        }
        break;

        // Jack, add for IME. [14:53,10/8/2002]
    case WM_CHAR:
        interrupt_char_key(wParam);
        return  0;
        //break;

        //IME ----------------------------------------------------------------------
    case WM_IME_SETCONTEXT:		
        if( ime_on_WM_IME_SETCONTEXT( hWnd, message, wParam, lParam) )
            return  0;
        break;

    case WM_INPUTLANGCHANGEREQUEST:
        if( ime_on_WM_INPUTLANGCHANGEREQUEST( hWnd, message, wParam, lParam ) )
            return  0;
        break;

    case WM_INPUTLANGCHANGE:
        if( ime_on_WM_INPUTLANGCHANGE( hWnd, message, wParam, lParam ) )
            return  0;
        break;

    case WM_IME_STARTCOMPOSITION:
        if( ime_on_WM_IME_STARTCOMPOSITION( hWnd, message, wParam, lParam ) )
            return  0;
        break;

    case WM_IME_ENDCOMPOSITION:
        if( ime_on_WM_IME_ENDCOMPOSITION( hWnd, message, wParam, lParam ))
            return  0;
        break;

    case WM_IME_NOTIFY:		
        if( ime_on_WM_IME_NOTIFY( hWnd, message, wParam, lParam ))
            return  0;
        break;

    case WM_IME_COMPOSITION: //怀�賳覺�
        if( ime_on_WM_IME_COMPOSITION( hWnd, message, wParam, lParam ) )
            return  0;
        break;
        // IME end ----------------------------------------------------------------

	case WM_USER+265:
		mp3_loop_flag = 1;
		break;
		
    default:
        break;
    }
    return DefWindowProc(hWnd,message,wParam,lParam);
} 


static  BOOL    doInit(int nCmdShow)
{
    WNDCLASSEX  wc;
    DWORD   dwStyle, dwExStyle;
    RECT    rc, rcWork, rcWin;

    // register DirectDraw Window class --------------------------------------
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW ;
    if(local_bEnableDblClk)
        wc.style |= CS_DBLCLKS ;

    //Jack, 2002.5.6.
    //wc.lpfnWndProc = RaysDDWndProc;
    wc.lpfnWndProc = local_pfnDDWndProc;

    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = local_hInstance;
    wc.hIcon = local_hIcon;
    wc.hIconSm = local_hIconSm;
    wc.hCursor = NULL;
    wc.hbrBackground =  (HBRUSH) GetStockObject( BLACK_BRUSH );
    wc.lpszMenuName = NULL;//local_MenuName;
    wc.lpszClassName = local_DDWndClassName;
    if(! RegisterClassEx(&wc) )
    {
        LastErrorMsg("RegisterClassEx");
        return FALSE;
    }

#ifdef  DEBUG
    dwExStyle = 0;
#else//!DEBUG
    dwExStyle = WS_EX_TOPMOST;
#endif//DEBUG

    dwExStyle |= WS_EX_ACCEPTFILES;

    if( g_dwScreenMode == WINDOWS_SCREEN_MODE )
    {
        dwStyle = WS_POPUP | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU ;
    }
    else //if( g_dwScreenMode == FULL_SCREEN_MODE )
    {
        dwStyle = WS_POPUP;
    }


    SetRect( &rc, 0, 0, g_nScreenWidth, g_nScreenHeight);

    AdjustWindowRectEx( &rc, dwStyle, FALSE, dwExStyle );
    SystemParametersInfo( SPI_GETWORKAREA, 0, &rcWork, 0 );

	if (g_bCenterWindow)
	{
		SetRect( &rcWin, rcWork.left + ( (rcWork.right - rcWork.left) - (rc.right - rc.left) ) / 2,
			rcWork.top + ( (rcWork.bottom - rcWork.top) - (rc.bottom - rc.top) ) / 2,
			rcWork.left + ( (rcWork.right - rcWork.left) - (rc.right - rc.left) ) / 2 + (rc.right - rc.left),
			rcWork.top + ( (rcWork.bottom - rcWork.top) - (rc.bottom - rc.top) ) / 2 + (rc.bottom - rc.top) );

		g_nScreenX = rcWin.left;
		g_nScreenY = rcWin.top;
	}
	else
	{
		SetRect( &rcWin, g_nScreenX, g_nScreenY, rc.right - rc.left+g_nScreenX, rc.bottom - rc.top+g_nScreenY );
	}

    g_hDDWnd = CreateWindowEx(
        dwExStyle,              // extended window style
        local_DDWndClassName,   // registered class name
        local_DDWindowName,          // window name
        dwStyle,
        rcWin.left,
        rcWin.top,
        rcWin.right - rcWin.left,
        rcWin.bottom - rcWin.top,
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

    // Save the window size/pos for switching modes
    GetWindowRect( g_hDDWnd, &g_rcWindow );

    /*
    GetClientRect( g_hDDWnd, &rcWin);
    sprintf((char *)print_rec, "ClientRect = (%d, %d)-(%d, %d)=(%d x %d)", rcWin.left, rcWin.top,
        rcWin.right, rcWin.bottom, rcWin.right - rcWin.left, rcWin.bottom - rcWin.top);
    MessageBox( NULL, (LPCTSTR)print_rec, "INFO", MB_OK | MB_ICONINFORMATION );
    */

    init_modules();
    g_bIsInitialized = TRUE;

    return TRUE;
} 


EXPORT  void    FNBACK  FinishProc()
{
    if(! local_bFinishProc )
    {
        free_modules();
        local_bFinishProc = TRUE;
    }
} 



EXPORT   int   FNBACK   FailMsg(char *s)
{
    if (g_bIsInitialized)
    {
        gameExit();
        g_bIsInitialized = 0;
    }
    FinishProc();
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


EXPORT  void    FNBACK  SetDDWndDoDropFiles( void func(int nFiles, int iFile, char *filename) )
{
    local_pfnDoDropFiles = func;
}


EXPORT  void    FNBACK  SetDDWndScreenSize( SLONG width, SLONG height )
{
    BOOL    valid_flag;

    valid_flag = 0;
    if(width == 640 && height == 480)
        valid_flag = 1;
    if(width == 800 && height == 600)
        valid_flag = 1;
    if(width == 1024 && height == 768)
        valid_flag = 1;
    if(width == 1152 && height == 864)
        valid_flag = 1;
    if(width == 1280 && height == 1024)
        valid_flag = 1;
    if(valid_flag)
    {
        g_nScreenWidth = width;
        g_nScreenHeight = height;
    }
    /*
    else
    {
        //g_nScreenWidth = 640;
        //g_nScreenHeight = 480;
    }
    */
}


EXPORT  void    FNBACK  SetDDWndEnableSwitchScreenMode( SLONG flag )
{
    if(flag)
        g_enable_f12_switch_screen = TRUE;
    else
        g_enable_f12_switch_screen = FALSE;
}


EXPORT  void    FNBACK  SetDDWndEnableDblClk( SLONG flag)
{
    if(flag)
        local_bEnableDblClk = TRUE;
    else
        local_bEnableDblClk = FALSE;
}

//===========================================================================================================
// add 2004/12/6 andy
EXPORT  void    FNBACK  SetDDWndScreenPos( SLONG x, SLONG y )
{
	g_nScreenX = x;
	g_nScreenY = y;

	g_bCenterWindow = false;
}

