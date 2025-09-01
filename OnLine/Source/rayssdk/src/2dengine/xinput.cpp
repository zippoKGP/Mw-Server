/*
**      XINPUT.CPP
**      Input functions,include mouse & keyboard.
**
**      ZJian, 2000.7.8.
**          Created.
**      ZJian, 2001.4.26.
**          Update to support Windows2000.
**      ZJian, 2001.6.5.
**          Make some functions for export :
**              fnCheckShiftKey()
**              fnCheckCtrlKey()...
**      ZJian, 2001.7.4.
**          Add element "MouseData.bInClientArea" for control 
**              mouse cursor between window client area and non-client area.
**
**
*/
#include "rays.h"
#include "winmain.h"
#include "xsystem.h"
#include "xinput.h"
#include "xtimer.h"
#include "xgrafx.h"
#include "xcak.h"
#include "xrle.h"
#include "xdraw.h"
#include "Windows.h"

#define CURSOR_REFRESH_TICKS    4   //8  // 1/100 second
#define MAX_GAME_CURSOR         64


typedef struct tagMOUSE_DATA
{
    SLONG    xPos;
    SLONG    yPos;
    UCHR     cShow;
    UCHR     cEvent;
    SHINT    nCursorType;
    HCURSOR  hNowCursor;
    HCURSOR  hGameCursor[MAX_GAME_CURSOR];
    SHINT    nWindowXOffset;
    SHINT    nWindowYOffset;
    SLONG    xSpot[MAX_GAME_CURSOR];
    SLONG    ySpot[MAX_GAME_CURSOR];
    //
    USHORT  fwKeys;
    SHORT   zDelta;
    //
    //geaan, 2001.7.4. add this for show mouse cursor in not-client area
    BOOL    bInClientArea;

} MOUSE_DATA,*LPMOUSE_DATA;


CAKE_FRAME_ANI  *cursor_ani[MAX_GAME_CURSOR];
CAKE_FRAME_ANI  *cursor_p[MAX_GAME_CURSOR];


//-------------------------------------------------------------------------------------------
// static functions & variables
//-------------------------------------------------------------------------------------------
static  void    fnResetKeyBoard(void);
static  void    fnResetNormalKey(void);
static  UCHR    fnCheckKeyValue(UCHR ScanCode);
static  SHINT   fnCheckUpKey(void);
static  SHINT   fnCheckDownKey(void);
static  SHINT   fnCheckLeftKey(void);
static  SHINT   fnCheckRightKey(void);
static  SHINT   fnCheckSpaceKey(void);
static  SHINT   fnCheckEnterKey(void);

//ZJian, 2001.6.5.
//marked for make these functions for export.
//static  SHINT   fnCheckCtrlKey(void);
//static  SHINT   fnCheckLeftCtrlKey(void);
//static  SHINT   fnCheckRightCtrlKey(void);
//static  SHINT   fnCheckAltKey(void);
//static  SHINT   fnCheckLeftAltKey(void);
//static  SHINT   fnCheckRightAltKey(void);
//static  SHINT   fnCheckShiftKey(void);
//static  SHINT   fnCheckLeftShiftKey(void);
//static  SHINT   fnCheckRightShiftKey(void);
static  SHINT   fnCheckInsertKey(void);
static  SHINT   fnCheckDeleteKey(void);
static  SHINT   fnCheckHomeKey(void);
static  SHINT   fnCheckEndKey(void);
static  SHINT   fnCheckPgUpKey(void);
static  SHINT   fnCheckPgDnKey(void);
static  SHINT   fnCheckTabKey(void);
static  SHINT   fnCheckEscKey(void);
static  UCHR    fnCheckAlphaNumberKey(void);
static  UCHR    fnCheckFunctionKey(void);
static  SHINT   fnScanKeyPress(void);
static  SHINT   fnCheckPauseKey(void);
static  SHINT   fnCheckBackKey(void);

static  void    fnMouseInit(void);

static void backupMouseImageRect(int w, int h, PIXEL *line, int sx, int sy, RLE* rle);

static  SHINT   EscKeyFlag=0;
static  USTR    NormalKeyBuffer[89];
static  USTR    ExtendKeyBuffer[89];
static  UCHR    ExtendKeyCode=0x00;
static  MOUSE_DATA MouseData;
static  ULONG   _start_time;
static  BMP*	cursor_backup = NULL;
static  RECT	back_rect = {0};

//--------------------------------------------------------------------------------------------------
// functions ...
//--------------------------------------------------------------------------------------------------
EXPORT  int     FNBACK  init_input(void)
{
    sprintf((char *)print_rec,"INPUT module initialize starting...");
    log_error(1,print_rec);

    fnResetKeyBoard();
    fnMouseInit();
    init_mouse_image_cursor();
    show_mouse(SHOW_WINDOW_CURSOR);

    sprintf((char *)print_rec,"INPUT module initialized OKay.");
    log_error(1,print_rec);
    sprintf((char *)print_rec,"\n");
    log_error(1,print_rec);

    return 0;
}

EXPORT  void    FNBACK  free_input(void)
{
    free_mouse_image_cursor();

    sprintf((char *)print_rec,"INPUT module free OKay.");
    log_error(1,print_rec);
}

EXPORT  void    FNBACK  active_input(int active)
{
    if( !active ) return;
    if(active)
    {
    }
}

//---------------------------------------------------------------------------------------------------
// keyboard functions ...
//---------------------------------------------------------------------------------------------------
EXPORT  UCHR    FNBACK  read_system_key(void)
{
    UCHR ch=(UCHR)NULL;
    ULONG ret;
    
    ret=fnScanKeyPress();
    if(1==(ret=fnScanKeyPress()))
    {
        if(fnCheckUpKey())
            ch=Up;
        else if(fnCheckDownKey())
            ch=Dn;
        else if(fnCheckLeftKey())
            ch=Left;
        else if(fnCheckRightKey())
            ch=Right;
        else if(fnCheckSpaceKey())
            ch=' ';
        else if(fnCheckEnterKey())
            ch=Enter;
        else if(fnCheckEscKey())
            ch=Esc;
        else if(fnCheckTabKey())
            ch=Tab;
        else if(fnCheckInsertKey())
            ch=Ins;
        else if(fnCheckDeleteKey())
            ch=Del;
        else if(fnCheckPgUpKey())
            ch=PgUp;
        else if(fnCheckPgDnKey())
            ch=PgDn;
        else if(fnCheckHomeKey())
            ch=Home;
        else if(fnCheckEndKey())
            ch=End;
        else if (fnCheckBackKey())
			ch=BackTab;
		else
            ch=fnCheckFunctionKey();
    }
    switch(ch)
    {
    case Tab:
//        capture_screen();
        //fnResetKeyBoard();
        break;
    default:
        break;
    }
    return(ch);
}

EXPORT  UCHR    FNBACK  read_data_key(void)
{
    UCHR ch=(UCHR)NULL;
    ULONG ret;
    SLONG shift_flag=0;
    
    if(1==(ret=fnScanKeyPress()))
    {
        shift_flag=fnCheckShiftKey();
        
        if(fnCheckUpKey())
            ch=S_Up;
        else if(fnCheckDownKey())
            ch=S_Dn;
        else if(fnCheckLeftKey())
            ch=S_Left;
        else if(fnCheckRightKey())
            ch=S_Right;
        else if(fnCheckInsertKey())
            ch=S_Ins;
        else if(fnCheckDeleteKey())
            ch=S_Del;
        else if(fnCheckHomeKey())
            ch=S_Home;
        else if(fnCheckEndKey())
            ch=S_End;
        else if(fnCheckPgUpKey())
            ch=S_PgUp;
        else if(fnCheckPgDnKey())
            ch=S_PgDn;
        else if(fnCheckSpaceKey())
            ch=' ';
        else if(fnCheckEnterKey())
            ch=S_Enter;
        else if(fnCheckEscKey())
            ch=S_Esc;
        else if(fnCheckTabKey())
            ch=S_Tab;
        else
        {
            ch=fnCheckFunctionKey();
            if(ch!=0x00)
            {
                switch(ch)
                {
                case KB_F1: ch=S_KB_F1; break;
                case KB_F2: ch=S_KB_F2; break;
                case KB_F3: ch=S_KB_F3; break;
                case KB_F4: ch=S_KB_F4; break;
                case KB_F5: ch=S_KB_F5; break;
                case KB_F6: ch=S_KB_F6; break;
                case KB_F7: ch=S_KB_F7; break;
                case KB_F8: ch=S_KB_F8; break;
                case KB_F9: ch=S_KB_F9; break;
                case KB_F10: ch=S_KB_F10; break;
                case KB_F11: ch=S_KB_F11; break;
                case KB_F12: ch=S_KB_F12; break;
                default: break;
                }
            }
            else
                ch=fnCheckAlphaNumberKey();
        }
    }
    if(shift_flag==1)
    {
        switch(ch)
        {
        case '1': ch='!'; break;
        case '2': ch='@'; break;
        case '3': ch='#'; break;
        case '4': ch='$'; break;
        case '5': ch='%'; break;
        case '6': ch='^'; break;
        case '7': ch='&'; break;
        case '8': ch='*'; break;
        case '9': ch='('; break;
        case '0': ch=')'; break;
        case '-': ch='_'; break;
        case '=': ch='+'; break;
        case '[': ch='{'; break;
        case ']': ch='}'; break;
        case ';': ch=':'; break;
        case '\'': ch='\"'; break;
        case ',': ch='<'; break;
        case '.': ch='>'; break;
        case '/': ch='\?'; break;
        case '\\': ch='|'; break;
        case '`': ch='~'; break;
        default: 
            //ZJian, add for chat string, 2001.4.21.
            if(ch>='A' && ch<='Z')
            {
                ch = (UCHR)( ch - 'A' + 'a' );
            }
            else if(ch>='a' && ch<='z')
            {
                ch = (UCHR)( ch -'a' + 'A' );
            }
            break;
        }
    }
    switch(ch)
    {
    case S_Tab:
//        capture_screen();
//        fnResetNormalKey();
        break;
    default:
        break;
    }
    return(ch);
}

EXPORT  void    FNBACK  reset_key(void)
{
    fnResetKeyBoard();
}

EXPORT  void    FNBACK  reset_data_key(void)
{
    fnResetNormalKey();
}

EXPORT  void    FNBACK  wait_tick(ULONG no)
{
    system_timer_tick = 0;
    while(system_timer_tick < no)
    {
        idle_loop();
    }
}

EXPORT  void    FNBACK  wait_key(SLONG key)
{
    UCHR ch=(UCHR)NULL;
    fnResetKeyBoard();
    while(ch!=key)
    {
        idle_loop();
        ch=read_data_key();
        if(ch) fnResetKeyBoard();
    }
}

EXPORT  UCHR    FNBACK  wait_any_key(void)
{
    UCHR ch=(UCHR)NULL;
    fnResetKeyBoard();
    while(ch==(UCHR)NULL)
    {
        idle_loop();
        ch=read_data_key();
    }
    fnResetKeyBoard();
    return(ch);
}

EXPORT  void    FNBACK  wait_key_time(SLONG key,SLONG no)
{
    UCHR ch=(UCHR)NULL;
    system_timer_tick = 0;
    fnResetKeyBoard();
    do {
        idle_loop();
        ch = read_data_key();
        if(ch) fnResetKeyBoard();
        if(ch==key) break;
    } while (system_timer_tick < (ULONG)no);
    fnResetKeyBoard();
}

EXPORT  void    FNBACK  clear_time_delay(void)
{
    _start_time = timeGetTime();
}

EXPORT  ULONG   FNBACK  get_time_delay(void)
{
    return (timeGetTime() - _start_time);
}

EXPORT  void    FNBACK  wait_time_delay(ULONG count)
{
    while(timeGetTime() < _start_time+count)
        idle_loop();
}


EXPORT  void    FNBACK  fnKeyBoardInterrupt(UCHR keycode)
{
    if(ExtendKeyCode == 0xE0)
    {
        if(keycode & 0x80)
            ExtendKeyBuffer[keycode & 0x7F]=0x00;
        else
            ExtendKeyBuffer[keycode]++;
        ExtendKeyCode=0x00;
    }
    else if(ExtendKeyCode == 0xE1)                // Pause/Break
    {
        if(keycode == 0xC5)
        {
            ExtendKeyBuffer[keycode & 0x7F]=0x00;
            ExtendKeyCode=0x00;
        }
        else if(keycode == 0x45)
        {
            ExtendKeyBuffer[keycode]++;
            ExtendKeyCode=0x00;
        }
    }
    else
    {
        if((keycode == 0xE0) || (keycode == 0xE1))
            ExtendKeyCode = keycode;
        else
        {
            if(keycode & 0x80)                         // Break code
                NormalKeyBuffer[keycode & 0x7F]=0x00;   // Reset keybuffer
            else                                       // Make code
            {
                NormalKeyBuffer[keycode]++;             // Set keybuffer
                if(keycode == 0x01)
                    EscKeyFlag=1;
            }
        }
    }
}

/////////////////////////////////////////////////////////////
// reset all keyboard buffer
static  void    fnResetKeyBoard(void)
{
    memset(NormalKeyBuffer,0x00,89);
    memset(ExtendKeyBuffer,0x00,89);
    ExtendKeyCode=0x00;
    EscKeyFlag=0;
}
////////////////////////////////////////////////////////////
// reset keyboard buffer exclude Shift,Ctrl,Alt
static  void    fnResetNormalKey(void)
{
    UCHR    ShiftFlag[2];
    UCHR    CtrlFlag[2];
    UCHR    AltFlag[2];
    
    ShiftFlag[0]=NormalKeyBuffer[0x2A];
    ShiftFlag[1]=NormalKeyBuffer[0x36];
    CtrlFlag[0]=NormalKeyBuffer[0x1D];
    CtrlFlag[1]=ExtendKeyBuffer[0x1D];
    AltFlag[0]=NormalKeyBuffer[0x38];
    AltFlag[1]=ExtendKeyBuffer[0x38];
    memset(NormalKeyBuffer,0x00,89);
    memset(ExtendKeyBuffer,0x00,89);
    NormalKeyBuffer[0x2A]=ShiftFlag[0];
    NormalKeyBuffer[0x36]=ShiftFlag[1];
    // NormalKeyBuffer[0x1D]=CtrlFlag[0];
    // ExtendKeyBuffer[0x1D]=CtrlFlag[1];
    // NormalKeyBuffer[0x38]=AltFlag[0];
    // ExtendKeyBuffer[0x38]=AltFlag[1];
    ExtendKeyCode=0x00;
    EscKeyFlag=0;
}

//////////////////////////////////////////////////////////////////
// check if press key whose scancode is ScanCode
static  UCHR    fnCheckKeyValue(UCHR ScanCode)
{
    if(ScanCode & 0x80)
        return(ExtendKeyBuffer[ScanCode & 0x7f]);   // extend code 1XXXXXXX
    else
        return(NormalKeyBuffer[ScanCode]);          // normal code 0XXXXXXX
}


static  SHINT   fnCheckUpKey(void)
{
    return(ExtendKeyBuffer[0x48] || NormalKeyBuffer[0x48]);
}

static SHINT  fnCheckDownKey(void)
{
    return (ExtendKeyBuffer[0x50] || NormalKeyBuffer[0x50]);
}

static  SHINT   fnCheckLeftKey(void)
{
    return (ExtendKeyBuffer[0x4B] || NormalKeyBuffer[0x4B]);
}

static  SHINT   fnCheckRightKey(void)
{
    return(ExtendKeyBuffer[0x4D] || NormalKeyBuffer[0x4D]);
}

static  SHINT   fnCheckSpaceKey(void)
{
    return(0!=NormalKeyBuffer[0x39]);
}

static  SHINT   fnCheckEnterKey(void)
{
    return(NormalKeyBuffer[0x1C] || ExtendKeyBuffer[0x1C]);
}

EXPORT  SHINT   FNBACK  fnCheckCtrlKey(void)
{
    return(NormalKeyBuffer[0x1D] || ExtendKeyBuffer[0x1D]);
}

EXPORT  SHINT   FNBACK  fnCheckLeftCtrlKey(void)
{
    return(0!=NormalKeyBuffer[0x1D]);
}

EXPORT  SHINT   FNBACK  fnCheckRightCtrlKey(void)
{
    return(0!=ExtendKeyBuffer[0x1D]);
}

EXPORT  SHINT   FNBACK  fnCheckAltKey(void)
{
    return(NormalKeyBuffer[0x38] || ExtendKeyBuffer[0x38]);
}

EXPORT  SHINT   FNBACK  fnCheckLeftAltKey(void)
{
    return(0!=NormalKeyBuffer[0x38]);
}

EXPORT  SHINT   FNBACK  fnCheckRightAltKey(void)
{
    return(0!=ExtendKeyBuffer[0x38]);
}

EXPORT  SHINT   FNBACK  fnCheckShiftKey(void)
{
    return(NormalKeyBuffer[0x2A] || NormalKeyBuffer[0x36]);
}

EXPORT  SHINT   FNBACK  fnCheckLeftShiftKey(void)
{
    return(0!=NormalKeyBuffer[0x2A]);
}

EXPORT  SHINT   FNBACK  fnCheckRightShiftKey(void)
{
    return(0!=NormalKeyBuffer[0x36]);
}

static  SHINT   fnCheckInsertKey(void)
{
    return(ExtendKeyBuffer[0x52] || NormalKeyBuffer[0x52]);
}

static  SHINT   fnCheckDeleteKey(void)
{
    return(ExtendKeyBuffer[0x53] || NormalKeyBuffer[0x53]);
}

static  SHINT   fnCheckHomeKey(void)
{
    return(ExtendKeyBuffer[0x47] || NormalKeyBuffer[0x47]);
}

static  SHINT   fnCheckEndKey(void)
{
    return(ExtendKeyBuffer[0x4F] || NormalKeyBuffer[0x4F]);
}

static  SHINT   fnCheckPgUpKey(void)
{
    return(ExtendKeyBuffer[0x49] || NormalKeyBuffer[0x49]);
}

static  SHINT   fnCheckPgDnKey(void)
{
    return(ExtendKeyBuffer[0x51] || NormalKeyBuffer[0x51]);
}

static  SHINT   fnCheckTabKey(void)
{
    return(0!=NormalKeyBuffer[0x0F]);
}

static  SHINT   fnCheckBackKey(void)
{
    return(0!=NormalKeyBuffer[0x0E]);
}

static  SHINT   fnCheckEscKey(void)
{
    return(EscKeyFlag);
}

static  UCHR    fnCheckAlphaNumberKey(void)
{
    enum {ExtBackSpace=0x0E};
    static UCHR AlphaNumberTable[2][49]=
    {
        {   0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,
            0x1E,0x30,0x2E,0x20,0x12,0x21,0x22,0x23,0x17,0x24,
            0x25,0x26,0x32,0x31,0x18,0x19,0x10,0x13,0x1F,0x14,
            0x16,0x2F,0x11,0x2D,0x15,0x2C,0x0C,0x0D,0x1A,0x1B,
            0x27,0x28,0x33,0x34,0x35,0x37,0x29,0x2B,ExtBackSpace
        },
        {   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
		'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
		'u', 'v', 'w', 'x', 'y', 'z', '-', '=', '[', ']',
		';', '\'', ',', '.', '/', '*', '`','\\', 0x08   
        }
        /*
        {   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
            'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
            'U', 'V', 'W', 'X', 'Y', 'Z', '-', '=', '[', ']',
            ';', '\'', ',', '.', '/', '*', '`','\\', 0x08   
        }
        */
    };
    SHINT   i;
    
    for(i=0;i<49;i++)
    {
        if(NormalKeyBuffer[AlphaNumberTable[0][i]])
        {
            return(AlphaNumberTable[1][i]);
        }
    }
    return(0x00);
}


static  UCHR    fnCheckFunctionKey(void)
{
    static UCHR FunctionTable[2][12]=
    {
        0x3B,  0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43,  0x44,  0x57,  0x58,
            KB_F1,KB_F2,KB_F3,KB_F4,KB_F5,KB_F6,KB_F7,KB_F8,KB_F9,KB_F10,KB_F11,KB_F12
    };
    SHINT i;
    
    for(i=0;i<12;i++)
    {
        if(NormalKeyBuffer[FunctionTable[0][i]])
        {
            return(FunctionTable[1][i]);
        }
    }
    return(0x00);
}

static  SHINT   fnScanKeyPress(void)
{
    SHINT   i;
    
    for(i=0;i<89;i++)
    {
        if((NormalKeyBuffer[i] != 0) || (ExtendKeyBuffer[i] != 0))
            return(1);
    }
    return(0);
}

static  SHINT   fnCheckPauseKey(void)
{
    // return(0!=ExtendKeyBuffer[0x45]);
    return(0!=NormalKeyBuffer[0x19]); // 'p' , for windows edition convenience
}



//---------------------------------------------------------------------------------------------------
// mouse functions ...
//---------------------------------------------------------------------------------------------------
static  void    fnMouseInit(void)
{
    SLONG   i;
    
    MouseData.xPos=0;
    MouseData.yPos=0;
    MouseData.cShow=SHOW_WINDOW_CURSOR;
    MouseData.cEvent=MS_Dummy;
    MouseData.nCursorType=0;
    MouseData.hNowCursor=LoadCursor(NULL,IDC_ARROW);
    for(i=0;i<MAX_GAME_CURSOR;i++)
    {
        MouseData.hGameCursor[i]=LoadCursor(NULL,IDC_ARROW);
        MouseData.xSpot[i]=0;
        MouseData.ySpot[i]=0;
    }
    MouseData.nWindowXOffset=0;
    MouseData.nWindowYOffset=0;
    //
    MouseData.fwKeys = 0;
    MouseData.zDelta = 0;
    //
    MouseData.bInClientArea = TRUE;
}

EXPORT SLONG FNBACK fnMouseInterrupt(UINT message,WPARAM wParam,LPARAM lParam)
{
    static  POINT   pt;
    SLONG   ret=1;

    
    switch(message)
    {
    case WM_SETCURSOR:
		{
			RECT rectClient;
			GetClientRect(g_hDDWnd, &rectClient);
			
			POINT ptCursor;
			GetCursorPos(&ptCursor);
			ScreenToClient(g_hDDWnd, &ptCursor);

			if (PtInRect(&rectClient, ptCursor))			
			{
				SetCursor(NULL);
			}
			else
			{
				SetCursor(MouseData.hNowCursor);
			}
			
			return true;		
		}
        break;

    case WM_MOUSEMOVE:
        {
            MouseData.fwKeys = GET_KEYSTATE_WPARAM(wParam);
            MouseData.xPos = GET_X_LPARAM(lParam)-MouseData.nWindowXOffset;
            MouseData.yPos = GET_Y_LPARAM(lParam)-MouseData.nWindowYOffset;
			if(wParam & MK_LBUTTON) MouseData.cEvent = MS_LDrag;
			else if(wParam & MK_RBUTTON) MouseData.cEvent = MS_RDrag;
			else if(wParam & MK_MBUTTON) MouseData.cEvent = MS_MDrag;
			else MouseData.cEvent = MS_Move;
//            SetCursor(MouseData.hNowCursor);
            if(! MouseData.bInClientArea )
            {
                MouseData.bInClientArea = TRUE;
                if(MouseData.cShow & SHOW_WINDOW_CURSOR)
                    ShowCursor(TRUE);
                else
                    ShowCursor(FALSE);
            }
        }
        break;
        
    case WM_LBUTTONDOWN:
        {
            MouseData.fwKeys = GET_KEYSTATE_WPARAM(wParam);
            MouseData.xPos = GET_X_LPARAM(lParam)-MouseData.nWindowXOffset;
            MouseData.yPos = GET_Y_LPARAM(lParam)-MouseData.nWindowYOffset;
            MouseData.cEvent=MS_LDn;
//            SetCursor(MouseData.hNowCursor);
            if(! MouseData.bInClientArea )
            {
                MouseData.bInClientArea = TRUE;
                if(MouseData.cShow & SHOW_WINDOW_CURSOR)
                    ShowCursor(TRUE);
                else
                    ShowCursor(FALSE);
            }
        }
        break;
        
    case WM_LBUTTONUP:
        {
            MouseData.fwKeys = GET_KEYSTATE_WPARAM(wParam);
            MouseData.xPos = GET_X_LPARAM(lParam)-MouseData.nWindowXOffset;
            MouseData.yPos = GET_Y_LPARAM(lParam)-MouseData.nWindowYOffset;
            MouseData.cEvent=MS_LUp;
//            SetCursor(MouseData.hNowCursor);
        }
        break;

    case WM_LBUTTONDBLCLK:
        {
            MouseData.fwKeys = GET_KEYSTATE_WPARAM(wParam);
            MouseData.xPos = GET_X_LPARAM(lParam)-MouseData.nWindowXOffset;
            MouseData.yPos = GET_Y_LPARAM(lParam)-MouseData.nWindowYOffset;
            MouseData.cEvent=MS_LDblClk;
//            SetCursor(MouseData.hNowCursor);
        }
        break;

    case WM_RBUTTONDOWN:
        {
            MouseData.fwKeys = GET_KEYSTATE_WPARAM(wParam);
            MouseData.xPos = GET_X_LPARAM(lParam)-MouseData.nWindowXOffset;
            MouseData.yPos = GET_Y_LPARAM(lParam)-MouseData.nWindowYOffset;
            MouseData.cEvent=MS_RDn;
//            SetCursor(MouseData.hNowCursor);
        }
        break;

    case WM_RBUTTONUP:
        {
            MouseData.fwKeys = GET_KEYSTATE_WPARAM(wParam);
            MouseData.xPos = GET_X_LPARAM(lParam)-MouseData.nWindowXOffset;
            MouseData.yPos = GET_Y_LPARAM(lParam)-MouseData.nWindowYOffset;
            MouseData.cEvent=MS_RUp;
//            SetCursor(MouseData.hNowCursor);
        }
        break;

    case WM_RBUTTONDBLCLK:
        {
            MouseData.fwKeys = GET_KEYSTATE_WPARAM(wParam);
            MouseData.xPos = GET_X_LPARAM(lParam)-MouseData.nWindowXOffset;
            MouseData.yPos = GET_Y_LPARAM(lParam)-MouseData.nWindowYOffset;
            MouseData.cEvent=MS_RDblClk;
//            SetCursor(MouseData.hNowCursor);
        }
        break;

    case WM_MBUTTONDOWN:
        {
            MouseData.fwKeys = GET_KEYSTATE_WPARAM(wParam);
            MouseData.xPos = GET_X_LPARAM(lParam)-MouseData.nWindowXOffset;
            MouseData.yPos = GET_Y_LPARAM(lParam)-MouseData.nWindowYOffset;
            MouseData.cEvent=MS_MDn;
//            SetCursor(MouseData.hNowCursor);
        }
        break;

    case WM_MBUTTONUP:
        {
            MouseData.fwKeys = GET_KEYSTATE_WPARAM(wParam);
            MouseData.xPos = GET_X_LPARAM(lParam)-MouseData.nWindowXOffset;
            MouseData.yPos = GET_Y_LPARAM(lParam)-MouseData.nWindowYOffset;
            MouseData.cEvent=MS_MUp;
//            SetCursor(MouseData.hNowCursor);
        }
        break;

    case WM_MBUTTONDBLCLK:
        {
            MouseData.fwKeys = GET_KEYSTATE_WPARAM(wParam);
            MouseData.xPos = GET_X_LPARAM(lParam)-MouseData.nWindowXOffset;
            MouseData.yPos = GET_Y_LPARAM(lParam)-MouseData.nWindowYOffset;
            MouseData.cEvent=MS_MDblClk;
//            SetCursor(MouseData.hNowCursor);
        }
        break;

    case WM_MOUSEWHEEL:
        {
            /*
            ** Jack, 2002.3.22.
            ** ----------------
            ** PS: 
            **  (1) We can receive this message even if mouse is not in the client area.
            **  (2) Different as other messages, the position is not the client area position,
            **      but the screen position.
            **  (3) Above all, we need convert screen position to client area position.
            */
            MouseData.fwKeys = GET_KEYSTATE_WPARAM(wParam);
            MouseData.zDelta += GET_WHEEL_DELTA_WPARAM(wParam);
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            ScreenToClient(g_hDDWnd, &pt);
            MouseData.xPos = pt.x - MouseData.nWindowXOffset;
            MouseData.yPos = pt.y - MouseData.nWindowYOffset;
            if(MouseData.zDelta >= WHEEL_DELTA)
            {
                MouseData.zDelta -= WHEEL_DELTA;
                MouseData.cEvent = MS_Forward;
            }
            if(MouseData.zDelta <= -WHEEL_DELTA)
            {
                MouseData.zDelta += WHEEL_DELTA;
                MouseData.cEvent = MS_Backward;
            }
//            SetCursor(MouseData.hNowCursor);
        }
        break;

//    case WM_XBUTTONDBLCLK:
        //sprintf((char *)print_rec,"WM_XBUTTONDBLCLK %d, %d", lParam, wParam);
        //log_error(1, print_rec);
        break;
//    case WM_XBUTTONDOWN:
        //sprintf((char *)print_rec,"WM_XBUTTONDOWN %d, %d", lParam, wParam);
        //log_error(1, print_rec);
        break;
//    case WM_XBUTTONUP:
        //sprintf((char *)print_rec,"WM_XBUTTONUP %d, %d", lParam, wParam);
        //log_error(1, print_rec);
        break;
        /*
    case WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONDBLCLK:
    case WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONUP:
    case WM_NCMBUTTONDBLCLK:
    case WM_NCMBUTTONDOWN:
    case WM_NCMBUTTONUP:
    case WM_NCRBUTTONDBLCLK:
    case WM_NCRBUTTONDOWN:
    case WM_NCRBUTTONUP:
    case WM_NCMOUSEHOVER:
    case WM_NCMOUSELEAVE:
        */
    case WM_NCMOUSEMOVE:
        {
            // Jack, added. [7/10/2002]
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            ScreenToClient(g_hDDWnd, &pt);
            MouseData.xPos = pt.x - MouseData.nWindowXOffset;
            MouseData.yPos = pt.y - MouseData.nWindowYOffset;

            if( MouseData.bInClientArea )
            {

                MouseData.bInClientArea = FALSE;
                ShowCursor(TRUE);
            }
            ret = 0;
            break;
        }
    default:
//        SetCursor(MouseData.hNowCursor);
        ret = 0;
        break;
    }
    return(ret);
}


EXPORT  void    FNBACK  show_mouse(SLONG flag)
{
    if((UCHR)flag==MouseData.cShow)
        return;
    if(flag & SHOW_WINDOW_CURSOR)
        ShowCursor(TRUE);
    else
        ShowCursor(FALSE);
    MouseData.cShow=(UCHR)flag;
}

EXPORT  void    FNBACK  set_mouse_cursor(SLONG type)
{
    if( (type<0)||(type>=MAX_GAME_CURSOR))
        return;
    if(MouseData.nCursorType==(SHINT)type)
        return;
    MouseData.hNowCursor=MouseData.hGameCursor[type];
//    SetCursor(MouseData.hNowCursor);
    MouseData.nCursorType=(SHINT)type;
}

EXPORT  void    FNBACK  set_mouse_position(SLONG xpos,SLONG ypos)
{
    POINT   pp;
    
    pp.x=(LONG)xpos;
    pp.y=(LONG)ypos;
    ClientToScreen(g_hDDWnd,&pp);
    SetCursorPos(pp.x,pp.y);
    MouseData.xPos=xpos;
    MouseData.yPos=ypos;
}


EXPORT  void    FNBACK  get_mouse_position(SLONG *xpos,SLONG *ypos)
{
//	POINT ptCursor;

//	::GetCursorPos(&ptCursor);
	
//	ScreenToClient(g_hDDWnd, &ptCursor);

    *xpos=MouseData.xPos;
    *ypos=MouseData.yPos;

//	*xpos = ptCursor.x;
//	*ypos = ptCursor.y;
}


EXPORT  UCHR    FNBACK  get_mouse_key(void)
{
    //idle_loop();
    if(MouseData.cEvent != MS_Dummy)
        return MouseData.cEvent;
    else if(MouseData.fwKeys & MK_LBUTTON)
        return MS_LDrag;
    else if(MouseData.fwKeys & MK_RBUTTON)
        return MS_RDrag;
    return(MouseData.cEvent);
}

EXPORT  UCHR    FNBACK  read_mouse_key(void)
{
    //idle_loop();
    if(MouseData.cEvent != MS_Dummy)
        return MouseData.cEvent;
    else if(MouseData.fwKeys & MK_LBUTTON)
        return MS_LDrag;
    else if(MouseData.fwKeys & MK_RBUTTON)
        return MS_RDrag;
    return(MouseData.cEvent);
}

EXPORT  void    FNBACK  wait_mouse_any_key(void)
{
    int     pass=0;
    while(pass==0)
    {
        idle_loop();
        if(MouseData.cEvent & 0x80)
            pass=1;
    }
}

EXPORT  void    FNBACK  wait_mouse_key(UCHR key)
{
    int     pass=0;
    while(pass==0)
    {
        idle_loop();
        if(MouseData.cEvent==key)
            pass=1;
    }
}

EXPORT  SLONG   FNBACK  check_mouse_shift(void)
{
    return (MouseData.fwKeys & MK_SHIFT) ? TRUE : FALSE ;
}

EXPORT  SLONG   FNBACK  check_mouse_control(void)
{
    return (MouseData.fwKeys & MK_CONTROL) ? TRUE : FALSE ;
}

EXPORT  void    FNBACK  reset_mouse(void)
{
    MouseData.cEvent = MS_Dummy;
    MouseData.zDelta = 0;
}


EXPORT  void    FNBACK  reset_mouse_key(void)
{
    MouseData.cEvent = MS_Dummy;
    MouseData.zDelta = 0;
}


EXPORT  SLONG   FNBACK  load_mouse_cursor(SLONG index,HCURSOR hCursor)
{
    if(index<0 || index>=MAX_GAME_CURSOR)
        return -1;
    MouseData.hGameCursor[index] = hCursor;
    return 0;
}


EXPORT  void    FNBACK  set_mouse_spot(SLONG index,SLONG x,SLONG y)
{
    if(index<0 || index>=MAX_GAME_CURSOR)
        return;
    MouseData.xSpot[index]=x;
    MouseData.ySpot[index]=y;
}

EXPORT  SLONG   FNBACK  init_mouse_image_cursor(void)
{
    SLONG i;

    for(i=0;i<MAX_GAME_CURSOR;i++)
    {
        cursor_ani[i] = NULL;
        cursor_p[i] = NULL;
    }

	if(NULL==(cursor_backup=create_bitmap(64, 64)))
        return -1;
	
    return TTN_OK;
}


EXPORT  void    FNBACK  free_mouse_image_cursor(void)
{
    SLONG i;
    for(i=0;i<MAX_GAME_CURSOR;i++)
    {
        destroy_cake_frame_ani(&cursor_ani[i]);
    }

	destroy_bitmap(&cursor_backup);
}


EXPORT  SLONG   FNBACK  load_mouse_image_cursor(SLONG index,USTR *filename)
{
    if(index<0 || index>MAX_GAME_CURSOR-1)
        return TTN_NOT_OK;
    destroy_cake_frame_ani(&cursor_ani[index]);
    if(TTN_OK == load_cake_frame_ani_from_file((USTR *)filename,&cursor_ani[index]) )
    {
        cursor_p[index]=cursor_ani[index]->next;
        return TTN_OK;
    }
    return TTN_NOT_OK;
}


EXPORT  SLONG   FNBACK  make_mouse_image_cursor(SLONG index, CAKE_FRAME_ANI *image_cfa, SLONG frames)
{
    CAKE_FRAME_ANI *p, *s, *src;

    if(index<0 || index>MAX_GAME_CURSOR-1)
        return TTN_NOT_OK;
    destroy_cake_frame_ani(&cursor_ani[index]);

    src = image_cfa;
    cursor_ani[index] = create_cake_frame_ani();
    p = cursor_ani[index];
    while(p && src && frames)
    {
        s = create_cake_frame_ani();
        if(s)
        {
            s->cake_frame = duplicate_cake_frame(src->cake_frame);
            s->last = p;
            s->next = NULL;

            p->next = s;

            src = src->next;
            p = p->next;
        }
        frames --;
    }
    cursor_p[index] = cursor_ani[index]->next;
    return TTN_OK;
}


EXPORT  void    FNBACK  redraw_mouse_image_cursor(char *pbuffer,long pitch,long width,long height)
{
    SLONG sx,sy;
    SLONG ox,oy;
    SHINT index;
    CAKE_FRAME *p=NULL;
    RLE *rle=NULL;

    if(! (MouseData.cShow & SHOW_IMAGE_CURSOR) )
        return;
    index=MouseData.nCursorType;
    if(index<0) return;
    if(! cursor_ani[index]) return;
    if(! cursor_p[index]) return;
    p = cursor_p[index]->cake_frame;
    if(!p) return;
    rle = p->rle;
    if(!rle) return;
    ox=p->frame_rx;
    oy=p->frame_ry;
    sx=MouseData.xPos-MouseData.xSpot[index] + ox;
    sy=MouseData.yPos-MouseData.ySpot[index] + oy;

	backupMouseImageRect(width, height, (PIXEL*)pbuffer, sx, sy, rle);

    put_rle_to_buffer(sx,sy,rle,pbuffer,pitch, width, height);
    
    if(cursor_timer_tick>CURSOR_REFRESH_TICKS)
    {
        if(cursor_p[index]->next != NULL)
            cursor_p[index]=cursor_p[index]->next;
        else
            cursor_p[index]=cursor_ani[index]->next;
        cursor_timer_tick=0;
    }
}

/******************************************************************************************************************/
/* CHAR KEY FUNCTIONS                                                                                             */
/******************************************************************************************************************/
static  UCHR    g_char_key[128];
static  UCHR	g_char_key_num = 0;

EXPORT  void    FNBACK  interrupt_char_key(WPARAM wParam)
{
	if (g_char_key_num < 100)
	{
		g_char_key[g_char_key_num ++] = (UCHR)wParam;
	}
}


EXPORT  void    FNBACK  reset_char_key(void)
{
    g_char_key_num = 0;
}


EXPORT  UCHR    FNBACK  get_char_key(int i)
{
	if (i < 0 || i >= g_char_key_num)
	{
		return 0;
	}

	return  g_char_key[i];
}

EXPORT  UCHR    FNBACK  get_char_key_num(void)
{
	return g_char_key_num;
}

/******************************************************************************************************************/

void backupMouseImageRect(int w, int h, PIXEL *line, int sx, int sy, RLE* rle)
{
	sx = max(sx, 0);
	sx = min(sx, w);

	sy = max(sy, 0);
	sy = min(sy, h);

	int width = min(rle->w, w-sx);
	width = min(width, cursor_backup->w);

	int height = min(rle->h, h-sy);
	height = min(height, cursor_backup->h);
	
	if (width > 0)
	{
		for (int y = 0; y < height; y ++)
		{
			memcpy(cursor_backup->line[y], &line[(y+sy)*w+sx], width*sizeof(PIXEL));
		}		

		SetRect(&back_rect, sx, sy, sx+width, sy+height);		
	}
}

void recoverMouseImageRect(BMP *img)
{
	int width = back_rect.right-back_rect.left;
	int height = back_rect.bottom-back_rect.top;

	if (width > 0)
	{
		for (int y = 0; y < height; y ++)
		{		
			memcpy(img->line[y+back_rect.top]+back_rect.left, cursor_backup->line[y], width*sizeof(PIXEL));
		}		
	}

	SetRect(&back_rect, 0, 0, 0, 0);
}
