#include "paradise.h"
#include "resource.h"
#include "mainfun.h"
#include <vfw.h>
#include "dxraw.h"
#include "dsound.h"
#include <windowsx.h>
#include "Muldiv32.h"


//
//  #define         DXDEBUG         1

//====================== define for avi ======================================
#define         MAXSTREAMS      25
#define         gfVideoFound (giFirstVideo >= 0)
#define         gfAudioFound (giFirstAudio >= 0)
HRESULT         hr;
PAVIFILE        pfile;
SLONG           gcpavi=0;
PAVISTREAM      pavi;
PAVISTREAM  gapavi[MAXSTREAMS];
PGETFRAME       gapgf[MAXSTREAMS];
HDRAWDIB        ghdd[MAXSTREAMS];
LPBITMAPINFOHEADER              lpbi=NULL;
AVICOMPRESSOPTIONS              gaAVIOptions[MAXSTREAMS];
LPAVICOMPRESSOPTIONS    galpAVIOptions[MAXSTREAMS];
AVISTREAMINFO                   avis;
DWORD   gdwMicroSecPerPixel = 1000L;
SLONG   giFirstAudio=-1;
SLONG   giFirstVideo=-1;
SLONG   timeStart,timeEnd,timeLength;
SLONG   start_time,end_time;
SLONG   lTime=0;
BOOL    gfPlaying;
// ------------------------ avi audio use ------------------------------------
static  HWAVEOUT        shWaveOut = 0;
static  LONG            slBegin;
static  LONG            slCurrent;
static  LONG            slEnd;
static  BOOL            sfLooping;
static  BOOL            sfPlaying = FALSE;

//#define MAX_AUDIO_BUFFERS       2048
//#define MIN_AUDIO_BUFFERS       2
//#define AUDIO_BUFFER_SIZE       16384

//#define MAX_AUDIO_BUFFERS       1024
#define MAX_AUDIO_BUFFERS       2048+200
#define MIN_AUDIO_BUFFERS       2
#define AUDIO_BUFFER_SIZE       16384

static  WORD            swBuffers;          // total # buffers
static  WORD            swBuffersOut;       // buffers device has
static  WORD            swNextBuffer;       // next buffer to fill
static  LPWAVEHDR       salpAudioBuf[MAX_AUDIO_BUFFERS];

static  PAVISTREAM      spavi;              // stream we're playing
static  LONG            slSampleSize;       // size of an audio sample

static  LONG            sdwBytesPerSec;
static  LONG            sdwSamplesPerSec;

void CALLBACK aviaudioStop(void);
BOOL CALLBACK aviaudioPlay(HWND hwnd, PAVISTREAM pavi, LONG lStart, LONG lEnd, BOOL fWait);
BOOL GetRGBBitsMask(UHINT *rbits,UHINT *gbits,UHINT *bbits);
//============================================================================
HCURSOR hNowCursor;

//=================以下數据可修改==============================================
#ifdef BIG5_COPYRIGHT_CHECK
#define  PROGRAM_NAME     "敦煌之大漠傳奇"    //程序名
#define  CLASS_NAME       "敦煌之大漠傳奇"    //窗口類名
#else
#define  PROGRAM_NAME     "嗟銓"    //程序名
#define  CLASS_NAME       "嗟銓"    //窗口類名
#endif
#define  SCREEN_W          SCREEN_WIDTH       //屏寬(可修 paradise.h)
#define  SCREEN_H          SCREEN_HEIGHT      //屏高
#define  SCREEN_D          16                 //顯示表面像素的位數
#define  MAX_VOICE_BUFFER  MAX_VOICE_CHANNEL  //語音道數(0《NUM《10)
// #define  SHOW_WINDOW_MOUSE                 //顯示WINDOWS cursor
// #define  DEBUG_MODE                        //調顯程序用

//=============================以下數据請不要改動==============================
HINSTANCE              g_hInst=NULL;
HWND                   g_hMainWnd=NULL;
BOOL                   g_bActWin=FALSE;
BOOL                   g_bActKeyInput=FALSE;
BOOL                   g_bActMouseInput=FALSE;
BOOL                   g_bDirectDraw=FALSE;
LPDIRECTINPUT          g_lpInput=NULL;
LPDIRECTINPUTDEVICE2   g_lpKeyInput=NULL;
LPDIRECTINPUTDEVICE2   g_lpMouseInput=NULL;

ULONG        g_nloRedBit=11;                //Red   的起始位
ULONG        g_nloGreenBit=5;               //Green 的起始位
ULONG        g_nloBlueBit=0;                //Blue  的起始
ULONG        g_nRedBits=5;                  //Red   占的位數
ULONG        g_nGreenBits=6;                //Green 占的位數
ULONG        g_nBlueBits=5;                 //Blue  占的位數
LPGUID       g_lpDevice=NULL;
ULONG        g_nDeviceNum=0;

UCHR  KeyBoards[256];
SLONG MouseXpos=SCREEN_W/2;
SLONG MouseYpos=SCREEN_H/2;
ULONG MouseButton=0;

#define INITGUID
#define WIN32_LEAN_AND_MEAN
#define INITGUID

#define  KEY_INPUT   0
#define  MOUSE_INPUT 1

//==========================SYSTEM=============================================
#define  TIMER_PROC01      1
#define  TIMER_PROC02      2
#define  TIMER_PROC03      3
#define  MAX_QUIT_PROCESS 32

UHINT    *SVGA_SCREEN_BUFFER;
ULONG    SystemCounter01=0;      // 1/18 second  delay count
ULONG    SystemCounter02=0;      // 1/18 second  flash palette
ULONG    SystemCounter03=0;      // 1/18 second  Mouse ani
ULONG    SystemCounter04=0;      // 1/18 second  AP control
ULONG    SystemCounter05=0;      // 1/18 second  CD-ROM check
ULONG    SystemCounter06=0;      // 1/18 second
ULONG    SystemCounter07=0;      // 1/18 second


//=============================MIDI VARI====================================
#define    MIDI_STOP               0
#define    MIDI_PLAY               1
#define    MIDI_ONE_TIMES          0
#define    MIDI_LOOP               1

BOOL       g_bMidiDevice      =TRUE ;
BOOL       g_bDSDevice        =TRUE ;
BOOL       g_nMidiLoad        =FALSE ;
BOOL       g_bPlayEnd         =TRUE ;
int        g_nMidiStatus      =MIDI_STOP;
int        g_nMidiPlayFlag    =MIDI_ONE_TIMES;
UINT       g_wMidiDeviceID    =0;


//==================== FUNCTION DEFINE =================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message,WPARAM wParam, LPARAM lParam);
BOOL    SeachRunProgram(void);
BOOL    RegisterWindows(HINSTANCE hInst);
BOOL    CreateWindows(HINSTANCE hInst);

BOOL    CreateDirectDraw(GUID *lpdevice);
void    DrawScreen(void) ;
void    DrawPutScreen(void);
void    SelectDriveDraw(SLONG select_type);

BOOL    fnEnableTimerProcess(void);
void    fnDisableTimerProcess(void);
void    CALLBACK TimerProc01(HWND hwnd,UINT message,UINT itimerid,ULONG dwtime);
void    CALLBACK TimerProc02(HWND hwnd,UINT message,UINT itimerid,ULONG dwtime);
void    CALLBACK TimerProc03(HWND hwnd,UINT message,UINT itimerid,ULONG dwtime);
void    fnErrorExit(const char *err_msg);
void    fnExit(void);
void    fnCheckMessage(void);

static  int   TotalQuitProcess=0;
void    (*QuitProcess[MAX_QUIT_PROCESS])(void);
void    fnExecQuitProcess(void);
BOOL    fnRegisterQuitProcess(void (*function)(void));

int     Convert_to_dxinput(void);
UCHR    read_key_data(void);
void    read_mouse_data(void);
BOOL    SetAcquireState(BOOL acq,int input_type);
BOOL    CleanDxinput( void );
BOOL    InitInput( void );
BOOL    InitKeyInput(void);
BOOL    InitMouseInput(void);
LPDIRECTINPUTDEVICE2 CreateDevice2( LPDIRECTINPUT lpdi, GUID* pguid );

int     Midi_Replay( void );
void    Midi_Stop(void);
void    OnMCINotify(WPARAM  wFlag, UINT nID);
BOOL    StorePixelFormatData(void);


// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void window_idle_loop(void);
KEY read_window_key(void);


// ====================== Extern subroutine define =====================
extern VGA_CONTROL vga_control;
extern void MainWnd(void);
extern void log_error(UCHR *message,SLONG flag);
void reset_window_key(ULONG delay_time);
//=======================system define==================================



//============================ WINDOWS Program Start ================================
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevent,LPSTR lpCmdLine,int nCmdShow)
{
MSG msg;

        g_hInst=hInstance;
        if(SeachRunProgram())
            return 1;    //ok find

        if(!RegisterWindows(g_hInst))
            return 1;    //faile

        if(!CreateWindows(g_hInst))
            return 1;    //faile

// select display drive
        ShowWindow(g_hMainWnd, nCmdShow);
        UpdateWindow(g_hMainWnd);

        g_cDDraw.SetRelativeHwnd(g_hMainWnd);
#ifdef DXDEBUG
        g_cDDraw.SetDisplayType(WINDOWS_MODE);
#else
        g_cDDraw.SetDisplayType(FULLSCREEN_MODE);
#endif
        g_cDDraw.Create();

//        if(!StorePixelFormatData())
//            return 1;

/* stephen
        if(!Convert_to_dxinput())
           return 1;
*/
        g_cDSound.SetRelativeHwnd(g_hMainWnd);
        if(g_cDSound.Create()!=TTN_OK)
           MessageBox(g_hMainWnd,"DSOUND Init Failed!","Message",MB_OK);

        if(!fnEnableTimerProcess())
           {
           fnErrorExit("Create System Times is failed!");
           return 1;
           }

        MainWnd();                              // 進入AP主程序

        Midi_Stop();
        PostQuitMessage(WM_QUIT);
        while(GetMessage(&msg,NULL,NULL,NULL))
           {
           TranslateMessage(&msg);
           DispatchMessage(&msg);
           }

        CleanDxinput();
        fnExecQuitProcess();

        return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,WPARAM wParam, LPARAM lParam)
{
        switch(message)
           {
           case WM_SYSCOMMAND:
              if((int)wParam==SC_CLOSE)
                 return 0;
           break;
    case WM_KEYUP:
          fnKeyBoardInterrupt((UCHR)((( HIWORD(lParam))%256 ) | 0x80));
          return 0;
    case WM_KEYDOWN:
          fnKeyBoardInterrupt((UCHR)(( HIWORD(lParam))%256 ) );
          return 0;
    case WM_SETCURSOR:
          SetCursor(hNowCursor);
          return 0;
    case WM_MOUSEMOVE:
          SetCursor(hNowCursor);
          return 0;
    case WM_LBUTTONDOWN:
          SetCursor(hNowCursor);
          MouseButton |= 0x01;
          return 0;
    case WM_LBUTTONUP:
          SetCursor(hNowCursor);
          MouseButton &= 0xFE;
          return 0;
    case WM_LBUTTONDBLCLK:
          SetCursor(hNowCursor);
          MouseButton |= 0x01;
          return 0;
    case WM_RBUTTONDOWN:
          SetCursor(hNowCursor);
          MouseButton |= 0x02;
          return 0;
    case WM_RBUTTONUP:
          SetCursor(hNowCursor);
          MouseButton &= 0xFD;
          return 0;
    case WM_RBUTTONDBLCLK:
          SetCursor(hNowCursor);
          MouseButton |= 0x02;
          return 0;
    case WM_MBUTTONDOWN:
          SetCursor(hNowCursor);
          MouseButton |= 0x04;
          return 0;
    case WM_MBUTTONUP:
          SetCursor(hNowCursor);
          MouseButton &= 0xFB;
          return 0;
    case WM_MBUTTONDBLCLK:
          SetCursor(hNowCursor);
          MouseButton |= 0x04;
          return 0;

            case WM_DESTROY:
               fnExit();
               return 0;
            case WM_CREATE:
               g_bActWin=TRUE;
               return 0;
            case WM_ACTIVATEAPP:
               if(g_bDirectDraw)
                  {
                  if(wParam==TRUE && GetActiveWindow()==g_hMainWnd )
                     {
                     g_bActWin=TRUE;
                     SetAcquireState(TRUE,KEY_INPUT);//ACTIVE KEY
                     SetAcquireState(TRUE,MOUSE_INPUT); //ACTIVE_MOUSE
                     }
                  else
                     {
                     g_bActWin=FALSE;
                     SetAcquireState(FALSE,KEY_INPUT);//DEACTIVE KEY
                     SetAcquireState(FALSE,MOUSE_INPUT); //DEACTIVE_MOUSE
                     }
                  }
               return 0;
               break;
            case WM_NCPAINT:            //攔劫 WINDOWS  重畫視窗骨架的動作
               return 0;
            case WM_NCCALCSIZE:         // 阻止 user 改變 window style
               return 0;
            case MM_MCINOTIFY :
               OnMCINotify( wParam, ( UINT )lParam ) ;
               return 0 ;

#ifdef DEBUG_MODE
            case WM_KEYDOWN:
               if(wParam==VK_F12)
                  fnExit();
               return 0;
#endif
           }
        return DefWindowProc(hWnd, message, wParam, lParam);
}


//============================ DATA Init =====================================
BOOL SeachRunProgram(void)
{
HWND hWnd;

        hWnd=FindWindow(CLASS_NAME,PROGRAM_NAME);
        if(hWnd!=NULL)
           {
           BringWindowToTop(hWnd);
           SetActiveWindow(hWnd);
           if(IsIconic(hWnd))
             ShowWindow(hWnd,SW_RESTORE);
           return TRUE;
           }
        return FALSE;
}


BOOL RegisterWindows(HINSTANCE hInst)
{
WNDCLASSEX wndClass;
int regval;

        wndClass.cbSize        = sizeof(wndClass);
        wndClass.style         = 0;
        wndClass.lpfnWndProc   = WndProc;
        wndClass.cbClsExtra    = 0;
        wndClass.cbWndExtra    = 0;
        wndClass.hInstance     = hInst;
        wndClass.hIcon         = LoadIcon(hInst,"DH_ICON");
        wndClass.hIconSm       = LoadIcon(hInst,"DH_ICON");
        wndClass.hCursor       = NULL;
        wndClass.lpszMenuName  = NULL;
        wndClass.lpszClassName = CLASS_NAME;
        wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

        regval=RegisterClassEx(&wndClass) ;
        if(regval!=0)
           return TRUE;
        else
           return FALSE;
}

BOOL CreateWindows(HINSTANCE hInst)
{

        g_hMainWnd=CreateWindow(CLASS_NAME  ,
                                PROGRAM_NAME,
                                WS_MAXIMIZE,
                                0,
                                0,
                                GetSystemMetrics(SM_CXSCREEN),
                                GetSystemMetrics(SM_CYSCREEN),
                                NULL,
                                NULL,
                                hInst,
                                NULL
                               );
        if(!g_hMainWnd)
           return FALSE;
        else
           return TRUE;
}



/*

BOOL StorePixelFormatData()
{
DDPIXELFORMAT format;
ULONG  hiRedBit,hiGreenBit,hiBlueBit;

        g_nloRedBit=11;
        g_nloGreenBit=5;
        g_nloBlueBit=0;
        g_nRedBits=5;
        g_nGreenBits=6;
        g_nBlueBits=5;

        ZeroMemory( &format, sizeof(format) );
        format.dwSize=sizeof(format);
        if(g_lpDDSPrimary->GetPixelFormat( &format )!=DD_OK)
           {
           log_error((UCHR *)"StorePixelFormatData() failed\n",0);
           return FALSE;
           }

        g_nloRedBit = LowBitPos( format.dwRBitMask );
        hiRedBit = HighBitPos( format.dwRBitMask );
        g_nRedBits=(hiRedBit-g_nloRedBit+1);

        g_nloGreenBit = LowBitPos( format.dwGBitMask );
        hiGreenBit = HighBitPos( format.dwGBitMask );
        g_nGreenBits=(hiGreenBit-g_nloGreenBit+1);

        g_nloBlueBit  = LowBitPos( format.dwBBitMask );
        hiBlueBit = HighBitPos( format.dwBBitMask );
        g_nBlueBits=(hiBlueBit-g_nloBlueBit+1);

// ================= Update to system config ==============
        vga_control.r_start_bit=g_nloRedBit;
        vga_control.g_start_bit=g_nloGreenBit;
        vga_control.b_start_bit=g_nloBlueBit;

        if(g_nRedBits==6)
           vga_control.r_shift=0;
        else
           vga_control.r_shift=1;

        if(g_nGreenBits==6)
           vga_control.g_shift=0;
        else
           vga_control.g_shift=1;

        if(g_nBlueBits==6)
           vga_control.b_shift=0;
        else
           vga_control.b_shift=1;


        if(GetRGBBitsMask(&vga_control.r_mask,
                          &vga_control.g_mask,
                          &vga_control.b_mask)!=TRUE)
            {
            vga_control.r_mask=0xFC00;
            vga_control.g_mask=0x07E0;
            vga_control.b_mask=0x001F;
            }



        vga_control.r_bit_num   = hiRedBit-
                                      vga_control.r_start_bit+1;
        vga_control.g_bit_num   = hiGreenBit-
                                      vga_control.g_start_bit+1;

        vga_control.b_bit_num   = hiBlueBit-
                                      vga_control.b_start_bit+1;


        return TRUE;
}


*/


//====================================DXINPUT=====================================

BOOL InitInput( void )
{
HRESULT     hr;

        hr = DirectInputCreate(g_hInst, DIRECTINPUT_VERSION, &g_lpInput, NULL );
        if(FAILED( hr ) )
           {
           MessageBox( g_hMainWnd, "Failed to initialize DirectInput.", "Error",
               MB_ICONERROR | MB_OK );
           return FALSE;
           }
        return TRUE;
}


LPDIRECTINPUTDEVICE2 CreateDevice2( LPDIRECTINPUT lpdi, GUID* pguid )
{
HRESULT hr, hr2;
LPDIRECTINPUTDEVICE  lpdid1;  // Temporary.
LPDIRECTINPUTDEVICE2 lpdid2;  // The keeper.

        hr = lpdi->CreateDevice( *pguid, &lpdid1, NULL );

        if(SUCCEEDED( hr ) )
           {
           hr2 = lpdid1->QueryInterface(IID_IDirectInputDevice2,( void ** )&lpdid2);
           lpdid1->Release();
           }
        else
           {
#ifdef DEBUG_MODE
           OutputDebugString("Could not create IDirectInputDevice device\n" );
#endif
           return NULL;
           }

        if(FAILED( hr2 ) )
           {
#ifdef DEBUG_MODE
           OutputDebugString( "Could not create IDirectInputDevice2 device\n" );
#endif
           return NULL;
           }
        return lpdid2;
}  // CreateDevice2


BOOL InitMouseInput(void)
{
DWORD cl, cl1;
HRESULT hr;

        if (g_lpMouseInput)
           {
           SetAcquireState(FALSE,MOUSE_INPUT);
           g_lpMouseInput->Release();
           g_lpMouseInput=NULL;
           }

        g_lpMouseInput = CreateDevice2(g_lpInput,(GUID*)&GUID_SysMouse );
        if ( ! g_lpMouseInput ) return FALSE;

        hr =g_lpMouseInput->SetDataFormat( &c_dfDIMouse);

        if ( FAILED( hr ) ) return FALSE;

        cl = DISCL_EXCLUSIVE;
        cl1 = DISCL_FOREGROUND;

#ifdef _DEBUG
        cl1 = DISCL_BACKGROUND;
        cl =  DISCL_NONEXCLUSIVE;
#endif

#ifdef SHOW_WINDOW_MOUSE
        cl =  DISCL_NONEXCLUSIVE;
#endif

        if (FAILED(g_lpMouseInput->SetCooperativeLevel(g_hMainWnd, cl | cl1 ) ) )
           {
#ifdef DEBUG_MODE
           OutputDebugString( "Failed to set game device cooperative level.\n" );
#endif

           g_lpMouseInput->Release();
           g_lpMouseInput = NULL;
           return FALSE;
           }

#ifdef BUFFER_DATA
#define BUFFERSIZE 16

// =========================== Set up the data buffer.
        DIPROPDWORD dipdw =
           {
// =========================== The header.
              {
              sizeof( DIPROPDWORD ),      // diph.dwSize
              sizeof( DIPROPHEADER ),     // diph.dwHeaderSize
              0,                          // diph.dwObj
              DIPH_DEVICE,                // diph.dwHow
              },
// =========================== Number of elements in data buffer.
            BUFFERSIZE,              // dwData
           };

        hr=g_lpMouseInput->SetProperty( DIPROP_BUFFERSIZE, &dipdw.diph );
        if(FAILED(hr)) return FALSE;

#endif

        return TRUE;
}


BOOL InitKeyInput(void)
{
DWORD cl, cl1;
HRESULT hr;

        if (g_lpKeyInput)
           {
           SetAcquireState(FALSE,KEY_INPUT);
           g_lpKeyInput->Release();
           g_lpKeyInput=NULL;
           }

        g_lpKeyInput = CreateDevice2(g_lpInput,(GUID*)&GUID_SysKeyboard  );
        if ( ! g_lpKeyInput ) return FALSE;

        hr = g_lpKeyInput->SetDataFormat( &c_dfDIKeyboard );

        if ( FAILED( hr ) ) return FALSE;

        cl = DISCL_NONEXCLUSIVE;
        cl1 = DISCL_FOREGROUND;

#ifdef DEBUG_MODE
        cl1 = DISCL_BACKGROUND;
#endif

       if (FAILED(g_lpKeyInput->SetCooperativeLevel(g_hMainWnd, cl | cl1 ) ) )
          {
#ifdef _DEBUG
          OutputDebugString( "Failed to set game device cooperative level.\n" );
#endif

          g_lpKeyInput->Release();
          g_lpKeyInput = NULL;
          return FALSE;
          }

        DIPROPDWORD dipdw =
          {
// ============================ The header.
             {
             sizeof( DIPROPDWORD ),      // diph.dwSize
             sizeof( DIPROPHEADER ),     // diph.dwHeaderSize
             0,                          // diph.dwObj
             DIPH_DEVICE,                // diph.dwHow
             },
// ============================ Number of elements in data buffer.
             256,              // dwData
          };

        hr=g_lpKeyInput->SetProperty( DIPROP_BUFFERSIZE, &dipdw.diph );
        if(FAILED(hr)) return FALSE;

        return TRUE;
        }


BOOL SetAcquireState(BOOL acq,int input_type)
{
HRESULT hr;

        if(! g_bActWin ) return FALSE;
        if(! g_lpInput ) return FALSE;

        if(input_type==KEY_INPUT)
           {
           if ( !acq )  // Unacquire.
              {
              hr =g_lpKeyInput->Unacquire();
              }
           else       // Acquire.
              {
              hr =g_lpKeyInput->Acquire();
              }
           }
        else
           {
          if ( !acq )  // Unacquire.
             {
             hr =g_lpMouseInput->Unacquire();
             }
          else       // Acquire.
             {
             hr =g_lpMouseInput->Acquire();
             }
          }

        return ( SUCCEEDED( hr ) );

}


BOOL CleanDxinput( void )
{
        if(g_lpKeyInput != NULL )
          {
          SetAcquireState(FALSE,KEY_INPUT);
          g_lpKeyInput->Release();
          g_lpKeyInput=NULL;
          }

        if(g_lpMouseInput!=NULL)
          {
          SetAcquireState(FALSE,MOUSE_INPUT);
          g_lpMouseInput->Release();
          g_lpKeyInput=NULL;
          }

        if(g_lpInput != NULL )
          {
          g_lpInput->Release();
          g_lpInput = NULL;
          }

        return TRUE;
}


int Convert_to_dxinput(void)
{
int val;

        val=0;
        g_bActKeyInput=FALSE;
        g_bActMouseInput=FALSE;

        if( InitInput() )
           {
           if(InitKeyInput() )
              {
              val=0x0001;
              SetAcquireState(TRUE,KEY_INPUT);
              g_bActKeyInput=TRUE;
              }
           if( InitMouseInput() )
              {
              val|=0x0010;
              SetAcquireState(TRUE,MOUSE_INPUT);
              g_bActMouseInput=TRUE;

#ifdef SHOW_WINDOW_MOUSE
              POINT mousepos;
              GetCursorPos(&mousepos);
              MouseXpos=mousepos.x;
              MouseYpos=mousepos.y;
#endif
              }
           }
        return (val);
}

void read_mouse_data(void)
{
HRESULT hr;

        if(! g_bActWin) return ;
        if(! g_bActMouseInput) return;

POLLING:
        hr=g_lpMouseInput->Poll();
        if(hr==DIERR_INPUTLOST)
          {
#ifdef DEBUG_MODE
          log_error((UCHR *)"MouseInput DIERR_INPUTLOST",0);
#endif

          hr =g_lpMouseInput->Acquire();
          if (SUCCEEDED(hr))
             goto POLLING;
          }
        else if(hr==DIERR_NOTACQUIRED || hr==DIERR_NOTINITIALIZED)
          {
#ifdef DEBUG_MODE
          log_error((UCHR *)"MouseInput DIERR_NOTACQUIRED",0);
#endif
          return;
          }



#ifdef BUFFER_DATA

int main_pass;
ULONG elements;
static DIDEVICEOBJECTDATA data;


        main_pass=0;
        while(main_pass==0)
           {
           elements=1;
           hr=g_lpMouseInput->GetDeviceData(sizeof(data),&data,&elements,0);
           if(hr==DI_OK && elements==1)
              {
              switch(data.dwOfs)
                 {
                 case DIMOFS_X:
                    MouseXpos+=data.dwData ;
                    break;
                 case DIMOFS_Y:
                    MouseYpos+=data.dwData;
                    break;
                 case DIMOFS_BUTTON0:  //LEFT BUTTON
                    if(data.dwData & 0x80)
                        MouseButton|=0x0001;
                    else
                        MouseButton&=0xfffE;
                    break;
                 case DIMOFS_BUTTON1:   //RIGHT BUTTON
                    if(data.dwData & 0x80)
                       MouseButton|=0x0002;
                    else
                       MouseButton&=0xfffD;
                    break;
                 case DIMOFS_BUTTON2:   //MIDDLE BUTTON
                    if(data.dwData & 0x80)
                       MouseButton|=0x0004;
                    else
                       MouseButton&=0xfffB;
                    break;
                 }
              }
           else if(elements==0)
              main_pass=1;
           }
#else

static DIMOUSESTATE dims;

AGAIN:
        hr =g_lpMouseInput->GetDeviceState(sizeof(DIMOUSESTATE), &dims);
        if (hr == DIERR_INPUTLOST)
           {
           hr =g_lpMouseInput->Acquire();
           if (SUCCEEDED(hr))
               goto AGAIN;
           }

        if (SUCCEEDED(hr))
           {
            MouseXpos+=dims.lX;
            MouseYpos+=dims.lY;

            if(dims.rgbButtons[0] & 0x80)
               MouseButton|=0x0001;
            else
               MouseButton&=0xfffE;

            if(dims.rgbButtons[1] & 0x80)
               MouseButton|=0x0002;
            else
               MouseButton&=0xfffD;
           }
#endif


#ifdef SHOW_WINDOW_MOUSE
        {
        static POINT mouseplace;
        GetCursorPos(&mouseplace);
        MouseXpos=mouseplace.x;
        MouseYpos=mouseplace.y;
        }
#endif


        if(MouseXpos<=0)
           MouseXpos=0;
        else if(MouseXpos >=SCREEN_W)
           MouseXpos=SCREEN_W-1;

        if(MouseYpos<=0)
           MouseYpos=0;
        else if(MouseYpos>=SCREEN_H)
           MouseYpos=SCREEN_H-1;
}


UCHR read_key_data(void)
{
static  DIDEVICEOBJECTDATA data[256];
ULONG   dwitems;
UCHR    i,ch;
HRESULT hr;
ch=NULL;

        if ( !g_bActWin) return (ch);
        if ( !g_bActKeyInput) return (ch);


REGETKEYDATA:
        dwitems=256;
        hr=g_lpKeyInput->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),data,&dwitems,0);
        if(hr == DIERR_INPUTLOST )
           {
           if(SetAcquireState(TRUE,KEY_INPUT) )
              goto REGETKEYDATA;
           }

        if(hr == DIERR_NOTACQUIRED )
           {
#ifdef _DEBUG
           OutputDebugString( "Device not acquired!" );
#endif
           return (NULL);
           }

        ZeroMemory(KeyBoards,sizeof(KeyBoards));

        if(SUCCEEDED(hr))
          {
           for(i=0;i<dwitems;i++)
              if(data[i].dwData & 0x80)
                 {
                 KeyBoards[data[i].dwOfs]++;
                 ch=(UCHR)data[i].dwOfs;
                 }
          }
        return (ch);
}


UCHR read_keyboard_data(void)
{
UCHR    i,ch;
HRESULT hr;

        ch=NULL;

        if (! g_bActWin)      return (ch);
        if (! g_bActKeyInput) return (ch);

REGETKEYDATA:
        memset(KeyBoards,0,256);
        hr=g_lpKeyInput->GetDeviceState(256,KeyBoards);

        if(hr == DIERR_INPUTLOST )
          {
          if(SetAcquireState(TRUE,KEY_INPUT) )
             goto REGETKEYDATA;
          }

        if(hr == DIERR_NOTACQUIRED )
          {
#ifdef _DEBUG
          OutputDebugString( "Device not acquired!\n" );
#endif
          return (NULL);
          }
        for(i=0;i<255;i++)
          if(KeyBoards[i]!=0)
             return i;

        return (NULL);
}



KEY read_window_key(void)
{
KEY    ch;
HRESULT hr;

        ch=TTN_KEY;
        if (! g_bActWin)  return (ch);
        if (! g_bActKeyInput) return (ch);

REGETKEYDATA:
        memset(KeyBoards,0,256);
        hr=g_lpKeyInput->GetDeviceState(256,KeyBoards);

        if(hr == DIERR_INPUTLOST )
          {
          log_error((UCHR *)"KEYinput DIERR_LOST",0);
          if(SetAcquireState(TRUE,KEY_INPUT) )
             goto REGETKEYDATA;
          }

        if(hr == DIERR_NOTACQUIRED )
          {
#ifdef _DEBUG
          OutputDebugString( "Device not acquired!\n" );
#endif
          log_error((UCHR *)"KEYinput DIERR_NOTACQUIRED",0);
          return (TTN_KEY);
          }

#ifdef DEBUG_MODE
        if(KeyBoards[DIK_F12]&0X80)
          fnExit();
#endif


        if(KeyBoards[DIK_LSHIFT] &0x80 || KeyBoards[DIK_RSHIFT] &0x80 )
          {
          if(KeyBoards[DIK_1] & 0x80)
             ch=Shift_1;
          else if(KeyBoards[DIK_2] &0x80)
             ch=Shift_2;
          else if(KeyBoards[DIK_3] &0x80)
             ch=Shift_3;
          if(KeyBoards[DIK_4] & 0x80)
             ch=Shift_1;
          else if(KeyBoards[DIK_5] &0x80)
             ch=Shift_2;
          else if(KeyBoards[DIK_6] &0x80)
             ch=Shift_3;
          return (ch);
          }
       else if(KeyBoards[DIK_LCONTROL]&0x80 || KeyBoards[DIK_RCONTROL]&0x80)
          {
          return (TTN_KEY);
          ;
          }
       else if(KeyBoards[DIK_LALT]&0x80 || KeyBoards[DIK_RALT]&0x80)
          {
          return (TTN_KEY);
          ;
          }
       else
          {
          if(KeyBoards[DIK_ESCAPE]&0x80)
             ch=Esc;
          else if(KeyBoards[DIK_TAB]&0x80)
             ch=Tab;
          else if(KeyBoards[DIK_BACK]&0x80)
             ch=BackSpace;
          else if(KeyBoards[DIK_RETURN]&0x80)
             ch=Enter;
          else if(KeyBoards[DIK_SPACE]&0x80)
             ch=Blank;
          else if(KeyBoards[DIK_HOME]&0x80)
             ch=Home ;
          else if(KeyBoards[DIK_UP]&0x80)
             ch=Up   ;
          else if(KeyBoards[DIK_PRIOR]&0x80)
             ch=PgUp;
          else if(KeyBoards[DIK_LEFT]&0x80)
             ch=Left;
          else if(KeyBoards[DIK_RIGHT]&0x80)
             ch=Right;
          else if(KeyBoards[DIK_END]&0x80)
             ch=End;
          else if(KeyBoards[DIK_DOWN]&0x80)
             ch=Dn;
          else if(KeyBoards[DIK_NEXT]&0x80)
             ch=PgDn;
          else if(KeyBoards[DIK_INSERT]&0x80)
             ch=Ins;
          else if(KeyBoards[DIK_DELETE]&0x80)
             ch=Del;
          else if(KeyBoards[DIK_A]&0x80)
             ch=KB_A;
          else if(KeyBoards[DIK_B]&0x80)
             ch=KB_B;
          else if(KeyBoards[DIK_C]&0x80)
             ch=KB_C;
          else if(KeyBoards[DIK_D]&0x80)
             ch=KB_D;
          else if(KeyBoards[DIK_E]&0x80)
             ch=KB_E;
          else if(KeyBoards[DIK_F]&0x80)
             ch=KB_F;
          else if(KeyBoards[DIK_G]&0x80)
             ch=KB_G;
          else if(KeyBoards[DIK_H]&0x80)
             ch=KB_H;
          else if(KeyBoards[DIK_I]&0x80)
             ch=KB_I;
          else if(KeyBoards[DIK_J]&0x80)
             ch=KB_J;
          else if(KeyBoards[DIK_K]&0x80)
             ch=KB_K;
          else if(KeyBoards[DIK_L]&0x80)
             ch=KB_L;
          else if(KeyBoards[DIK_M]&0x80)
             ch=KB_M;
          else if(KeyBoards[DIK_N]&0x80)
             ch=KB_N;
          else if(KeyBoards[DIK_O]&0x80)
             ch=KB_O;
          else if(KeyBoards[DIK_P]&0x80)
             ch=KB_P;
          else if(KeyBoards[DIK_Q]&0x80)
             ch=KB_Q;
          else if(KeyBoards[DIK_R]&0x80)
             ch=KB_R;
          else if(KeyBoards[DIK_S]&0x80)
             ch=KB_S;
          else if(KeyBoards[DIK_T]&0x80)
             ch=KB_T;
          else if(KeyBoards[DIK_U]&0x80)
             ch=KB_U;
          else if(KeyBoards[DIK_V]&0x80)
             ch=KB_V;
          else if(KeyBoards[DIK_W]&0x80)
             ch=KB_W;
          else if(KeyBoards[DIK_X]&0x80)
             ch=KB_X;
          else if(KeyBoards[DIK_Y]&0x80)
             ch=KB_Y;
          else if(KeyBoards[DIK_Z]&0x80)
             ch=KB_Z;
          else if(KeyBoards[DIK_1]&0x80)
             ch=KB_1;
          else if(KeyBoards[DIK_2]&0x80)
             ch=KB_2;
          else if(KeyBoards[DIK_3]&0x80)
             ch=KB_3;
          else if(KeyBoards[DIK_4]&0x80)
             ch=KB_4;
          else if(KeyBoards[DIK_5]&0x80)
             ch=KB_5;
          else if(KeyBoards[DIK_6]&0x80)
             ch=KB_6;
          else if(KeyBoards[DIK_7]&0x80)
             ch=KB_7;
          else if(KeyBoards[DIK_8]&0x80)
             ch=KB_8;
          else if(KeyBoards[DIK_9]&0x80)
             ch=KB_9;
          else if(KeyBoards[DIK_0]&0x80)
             ch=KB_0;
          return (ch);
          }
        return (TTN_KEY);

}


void reset_window_key(ULONG delay_time)
{
SLONG i;
ULONG now_time;
        if(!g_bActWin) return;
        if(!g_lpKeyInput) return;

        memset(KeyBoards,0,256);
        now_time=GetTickCount();

GETKEYDATA:
        g_lpKeyInput->GetDeviceState(256,KeyBoards);
        for(i=1;i<256;i++)
           {
           if(GetTickCount()-now_time>delay_time)
              break;
           if(KeyBoards[i] & 0x80)
              {
              window_idle_loop();
              goto GETKEYDATA;
              }
           }
}

/*
void reset_window_key(void)
{
int i;

        if(!g_bActWin) return;
        if(!g_lpKeyInput) return;

        memset(KeyBoards,0,256);

KEYAGAIN:
        g_lpKeyInput->GetDeviceState(256,KeyBoards);
        for(i=1;i<256;i++)
        if(KeyBoards[i] & 0x80)
            goto KEYAGAIN;
}
*/


// =================================== SYSTEM USE =================================
void  fnDisableTimerProcess(void)
{
        KillTimer(g_hMainWnd,TIMER_PROC01);
        KillTimer(g_hMainWnd,TIMER_PROC02);
        KillTimer(g_hMainWnd,TIMER_PROC03);
}


void CALLBACK TimerProc01(HWND hwnd,UINT message,UINT itimerid,DWORD dwtime)
{
        SystemCounter01++;
        SystemCounter04++;
        SystemCounter07++;
}


void CALLBACK TimerProc02(HWND hwnd,UINT message,UINT itimerid,DWORD dwtime)
{
        SystemCounter02++;
        SystemCounter06++;
}

void CALLBACK TimerProc03(HWND hwnd,UINT message,UINT itimerid,DWORD dwtime)
{
        SystemCounter03++;
        SystemCounter05++;
}



BOOL  fnEnableTimerProcess(void)
{

        if(SetTimer(g_hMainWnd,TIMER_PROC01,5,(TIMERPROC)TimerProc01) == 0)
           return (FALSE);

        if(SetTimer(g_hMainWnd,TIMER_PROC02,5,(TIMERPROC)TimerProc02) == 0)
           {
           KillTimer(g_hMainWnd,TIMER_PROC01);
           return(FALSE);
           }

        if(SetTimer(g_hMainWnd,TIMER_PROC03,5,(TIMERPROC)TimerProc03) == 0)
           {
           KillTimer(g_hMainWnd,TIMER_PROC01);
           KillTimer(g_hMainWnd,TIMER_PROC02);
           return(FALSE);
           }

        if(! fnRegisterQuitProcess(fnDisableTimerProcess))
           {
           KillTimer(g_hMainWnd,TIMER_PROC01);
           KillTimer(g_hMainWnd,TIMER_PROC02);
           KillTimer(g_hMainWnd,TIMER_PROC03);
           return(FALSE);
           }

        return(TRUE);
}


void fnCheckMessage(void)
{
MSG   msg;

        while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
           {
           TranslateMessage(&msg);
           DispatchMessage(&msg);
           }
}


BOOL fnRegisterQuitProcess(void (*function)(void))
{
        if(TotalQuitProcess >=MAX_QUIT_PROCESS)
           return (FALSE);

        QuitProcess[TotalQuitProcess]=function;
        TotalQuitProcess++;
        return(TRUE);
}


void fnExecQuitProcess(void)
{
int i;

        if(TotalQuitProcess == 0)
           return;
        for(i=TotalQuitProcess-1;i>=0;i--)
          QuitProcess[i]();
}


void fnErrorExit(const char *err_msg)
{
MSG msg;

        Midi_Stop();
        MessageBox(g_hMainWnd,err_msg,"ERROR",
                 MB_ICONEXCLAMATION | MB_TASKMODAL | MB_OK);

        PostQuitMessage(0);
        while(GetMessage(&msg,NULL,0,0))
           {
           TranslateMessage(&msg);
           DispatchMessage(&msg);
           }

        CleanDxinput();
        fnExecQuitProcess();
        exit(1);
}


void fnExit(void)
{
MSG msg;

        Midi_Stop();
        PostQuitMessage(0);
        while(GetMessage(&msg,NULL,0,0))
          {
          TranslateMessage(&msg);
          DispatchMessage(&msg);
          }

        CleanDxinput();
        fnExecQuitProcess();
        exit(1);
}


void window_idle_loop(void)
{
MSG msg;

        while( PeekMessage(&msg,NULL,0,0,PM_REMOVE) )
          {
          TranslateMessage(&msg);
          DispatchMessage(&msg);
          }
        get_mouse_position();

}


void wait_window_any_key(void)
{
UCHR ch;

        while(MouseButton!=0)
           window_idle_loop();

        ch=NULL;
        while(ch==NULL && MouseButton==0)
           {
           window_idle_loop();
           ch=read_keyboard_data();
           }
}


void wait_any_key_time(ULONG count)
{
UCHR ch;
ULONG now_time;

        while(MouseButton!=0)
           window_idle_loop();
        ch=NULL;
        now_time=GetTickCount();
        while(ch==NULL && MouseButton==0)
           {
           window_idle_loop();
           ch=read_keyboard_data();
           if( GetTickCount() - now_time >count)
              break;
           }

}


void log_msg(int flag, char *msg,...)
{
va_list va;
char buf[256];

        va_start(va,msg);
        wvsprintf(buf,msg,va);
        va_end(va);
        log_error((UCHR *)buf,flag);
}


//=================================DXVOICE===================================

// ===========================MIDI=========================================
void InitMidiPlay( void )
{
        if( g_bMidiDevice == FALSE ) return ;

char szFileName[ MAX_PATH ] ;

        MCI_OPEN_PARMS  mciOpen ;
        mciOpen.wDeviceID = NULL ;
        mciOpen.lpstrDeviceType  = ( LPCSTR )MCI_DEVTYPE_SEQUENCER ;
        mciOpen.lpstrElementName = szFileName ;

        if( mciSendCommand( NULL, MCI_OPEN,
                            MCI_WAIT | MCI_OPEN_TYPE |
                            MCI_OPEN_ELEMENT | MCI_OPEN_TYPE_ID,
                           ( DWORD )( LPVOID )&mciOpen ) != 0L )
           {
           return ;
           }

        g_wMidiDeviceID = mciOpen.wDeviceID ;

        MCI_SEEK_PARMS  mciSeek ;
        if( mciSendCommand( g_wMidiDeviceID, MCI_SEEK,
                            MCI_SEEK_TO_START | MCI_WAIT,
                            ( DWORD )( LPVOID )&mciSeek     ) != 0L )
            {
            goto Stop ;
            }

MCIERROR mciErr ;
MCI_PLAY_PARMS mciPlay ;

        mciPlay.dwCallback = ( DWORD )g_hMainWnd ;
        mciErr = mciSendCommand( g_wMidiDeviceID, MCI_PLAY, MCI_NOTIFY,
                                 ( DWORD )( LPVOID )&mciPlay ) ;

Stop :
        MCI_GENERIC_PARMS       mciGene ;
        mciSendCommand( g_wMidiDeviceID, MCI_STOP, MCI_WAIT,
                                        ( DWORD )( LPVOID )&mciGene ) ;

        mciSendCommand( g_wMidiDeviceID, MCI_CLOSE, MCI_WAIT, 0L ) ;

}


int Midi_Play(char *lpFilename,int play_type)
{

        if( g_bMidiDevice == FALSE ) return TTN_NOT_OK;

        if( g_nMidiStatus == MIDI_PLAY )
           {
            Midi_Stop() ;
           }

        g_bPlayEnd = FALSE ;

        if(play_type==0)
           g_nMidiPlayFlag=MIDI_ONE_TIMES;
        else
           g_nMidiPlayFlag=MIDI_LOOP;

        MCI_OPEN_PARMS  mciOpen ;
        mciOpen.wDeviceID = NULL ;
        mciOpen.lpstrDeviceType  = ( LPCSTR )MCI_DEVTYPE_SEQUENCER ;
        mciOpen.lpstrElementName =lpFilename  ;

        if( mciSendCommand( NULL, MCI_OPEN,
                            MCI_WAIT | MCI_OPEN_TYPE |
                            MCI_OPEN_ELEMENT | MCI_OPEN_TYPE_ID,
                            ( DWORD )( LPVOID )&mciOpen ) != 0L )
           {
              log_msg(0,"%s %s",lpFilename,"play falid!");
              return (TTN_NOT_OK) ;
           }

       g_wMidiDeviceID = mciOpen.wDeviceID ;

       g_nMidiLoad = TRUE ;

       return Midi_Replay() ;
}

MCI_SEEK_PARMS  g_mciSeek ;
MCIERROR        g_mciErr ;
MCI_PLAY_PARMS  g_mciPlay ;

int Midi_Replay( void )
{
        if( mciSendCommand( g_wMidiDeviceID, MCI_SEEK,
                           MCI_SEEK_TO_START | MCI_WAIT,
                          ( DWORD )( LPVOID )&g_mciSeek     ) != 0L )
            {
              Midi_Stop() ;
              return TTN_NOT_OK ;
            }

        g_mciPlay.dwCallback = (DWORD)g_hMainWnd ;
        g_mciErr = mciSendCommand( g_wMidiDeviceID, MCI_PLAY, MCI_NOTIFY,
                                 ( DWORD )( LPVOID )&g_mciPlay ) ;

        if(g_mciErr != 0L )
           {
             g_bMidiDevice = FALSE ;
             Midi_Stop() ;
             return TTN_NOT_OK ;
          }

        g_nMidiStatus = MIDI_PLAY ;

        return TTN_OK ;
}


void Midi_Stop( void )
{
        if( g_nMidiStatus == MIDI_STOP )
           {
            return ;
           }

        MCI_GENERIC_PARMS       mciGene ;

        mciSendCommand( g_wMidiDeviceID, MCI_STOP, MCI_WAIT,
                       ( DWORD )( LPVOID )&mciGene ) ;

        mciSendCommand( g_wMidiDeviceID, MCI_CLOSE, MCI_WAIT, 0L ) ;

        g_nMidiStatus = MIDI_STOP ;
}

void Midi_Pause( void )
{
        if(g_nMidiStatus == MIDI_STOP )
            {
            return ;
            }

        MCI_GENERIC_PARMS       mciGene ;

        mciSendCommand( g_wMidiDeviceID, MCI_PAUSE, MCI_WAIT,
                        ( DWORD )( LPVOID )&mciGene ) ;

        g_nMidiStatus = MIDI_STOP ;
}


int Midi_Continue( void )
{

        MCIERROR          mciErr ;
        MCI_PLAY_PARMS    mciPlay ;

        mciPlay.dwCallback = ( DWORD )g_hMainWnd ;
        mciErr = mciSendCommand( g_wMidiDeviceID, MCI_PLAY, MCI_NOTIFY,
                               ( DWORD )( LPVOID )&mciPlay ) ;

        if( mciErr != 0L )
           {
              g_bMidiDevice = FALSE ;
              Midi_Stop() ;
              return TTN_NOT_OK ;
           }

        g_nMidiStatus = MIDI_PLAY ;

        return TTN_OK ;
}


void OnMCINotify(WPARAM  wFlag, UINT nID)
{
        if( nID != g_wMidiDeviceID )
          {
             return ;
          }

        if( ( g_nMidiStatus != MIDI_PLAY ) )
           {
                return ;
           }

        if( ( wFlag & MCI_NOTIFY_SUCCESSFUL ) != 0 )
           {
                if( g_nMidiPlayFlag == MIDI_ONE_TIMES )
                   {
                     Midi_Stop();
                     g_bPlayEnd = TRUE ;
                     g_nMidiLoad=FALSE;
                   }
                else if( g_nMidiLoad )
                   {
                      Midi_Replay() ;
                   }
          }
}


void get_mouse_position(void)
{
 POINT mouseplace;
        GetCursorPos(&mouseplace);
        MouseXpos=mouseplace.x;
        MouseYpos=mouseplace.y;
}

void SCR_play_avi(UCHR *filename,SLONG x,SLONG y,SLONG xl,SLONG yl,SLONG *realplay,SLONG *realframe,SLONG skip_flag)
{
SLONG i,l,lTemp,j;
SLONG lEnd,lEndTime;
SLONG lSamp,lCurSamp;
SLONG iFrameWidth;
SLONG temp_i;
UCHR key=' ';
HDC hdc;

        g_cDSound.Release();

        *realplay=0;
        *realframe=0;
        AVIFileInit();
        hr=AVIFileOpen(&pfile,(LPCTSTR)filename,0,0L);
        if(hr!=0)
                fnErrorExit("Load Avi File Error!");

        for(i=gcpavi;i<MAXSTREAMS;i++)
        {
                if(AVIFileGetStream(pfile,&pavi,0L,i-gcpavi) != AVIERR_OK)
                        break;
                if (i == MAXSTREAMS)
                {
                    AVIStreamRelease(pavi);
                    break;
                }
                if (CreateEditableStream(&gapavi[i], pavi) != AVIERR_OK)
                {
                        AVIStreamRelease(pavi);
                        break;
                }
                AVIStreamRelease(pavi);
        }
        AVIFileRelease(pfile);

    if (gcpavi == i && i != MAXSTREAMS)
                fnErrorExit("Error!");

    temp_i = i;
    aviaudioStop();
    for (i = 0; i < gcpavi; i++)
        {
                if (gapgf[i])
                {
                        AVIStreamGetFrameClose(gapgf[i]);
                        gapgf[i] = NULL;
                }
                if (ghdd[i])
                {
                        DrawDibClose(ghdd[i]);
                    ghdd[i] = 0;
                }
    }
    giFirstVideo = giFirstAudio = -1;

        gcpavi = temp_i;
        timeStart=0x7FFFFFFF;
        timeEnd=0;

        for(i=0;i<gcpavi;i++)
        {
                AVIStreamInfo(gapavi[i], &avis, sizeof(avis));
                galpAVIOptions[i] = &gaAVIOptions[i];
                memset(galpAVIOptions[i], 0, sizeof(AVICOMPRESSOPTIONS));
                galpAVIOptions[i]->fccType = avis.fccType;
                switch(avis.fccType)
                {
                        case streamtypeVIDEO:
                                galpAVIOptions[i]->dwFlags = AVICOMPRESSF_VALID |
                                        AVICOMPRESSF_KEYFRAMES | AVICOMPRESSF_DATARATE;
                                galpAVIOptions[i]->fccHandler = 0;
                                galpAVIOptions[i]->dwQuality = (DWORD)ICQUALITY_DEFAULT;
                                galpAVIOptions[i]->dwKeyFrameEvery = (DWORD)-1; // Default
                                galpAVIOptions[i]->dwBytesPerSecond = 0;
                                galpAVIOptions[i]->dwInterleaveEvery = 1;
                                break;
                        case streamtypeAUDIO:
                                galpAVIOptions[i]->dwFlags |= AVICOMPRESSF_VALID;
                                galpAVIOptions[i]->dwInterleaveEvery = 1;
                                AVIStreamReadFormat(gapavi[i],
                                                        AVIStreamStart(gapavi[i]),
                                                        NULL,
                                                        &lTemp);
                                galpAVIOptions[i]->cbFormat = lTemp;
                                if (lTemp)
                                        galpAVIOptions[i]->lpFormat = GlobalAllocPtr(GHND, lTemp);

                                if (galpAVIOptions[i]->lpFormat)
                                        AVIStreamReadFormat(gapavi[i],
                                                        AVIStreamStart(gapavi[i]),
                                                        galpAVIOptions[i]->lpFormat,
                                                        &lTemp);
                                break;
                        default:
                                break;
                }

                timeStart = min(timeStart, AVIStreamStartTime(gapavi[i]));
                timeEnd   = max(timeEnd, AVIStreamEndTime(gapavi[i]));

                //
                // Draw a VIDEO stream
                //
                if (avis.fccType == streamtypeVIDEO)
                {
                        gapgf[i] = AVIStreamGetFrameOpen(gapavi[i], NULL);
                        *realframe += AVIStreamLength(gapavi[i]);
                        if(gapgf[i] == NULL)
                                continue;

                        ghdd[i] = DrawDibOpen();
                }
                else if (avis.fccType == streamtypeAUDIO)
                {
                    gapgf[i] = NULL;
                        ghdd[i] = NULL;

                    if (!gfAudioFound)
                                giFirstAudio = i;
                }
        }

        timeLength=timeEnd-timeStart;

        if(timeLength==0)
                timeLength=1;
// -------- the program of before finish the avi file init -----------
        end_time=0;
        lTime=0;
                            if (gfAudioFound)
                                {
                                aviaudioPlay(g_hMainWnd,
                                                                gapavi[giFirstAudio],
                                                                AVIStreamTimeToSample(gapavi[giFirstAudio], 0),
                                                                AVIStreamEnd(gapavi[giFirstAudio]),
                                         FALSE);
//                                      giFirstAudio=-1;

                                        clear_time_delay();
                                        wait_time_delay1(750);
                                }

        gfPlaying = TRUE;

//
        for (i=0; i<gcpavi; i++)
        {
                start_time=timeGetTime();
        while(end_time-start_time<timeLength)
        {
                end_time=timeGetTime();
                lTime=end_time-start_time;
                idle_loop();
                if(skip_flag==1)
                   {
                   key=read_system_key();
                   if(key==Esc)
                      {
                      fnResetKeyBoard();
                      break;
                      }
                   }
                AVIStreamInfo(gapavi[i], &avis, sizeof(avis));
                if (avis.fccType == streamtypeVIDEO)
                {
                    if (gapgf[i] == NULL)
                                continue;
                    lEndTime = AVIStreamEndTime(gapavi[i]);
                    if (lTime <= lEndTime)
                                lSamp = AVIStreamTimeToSample(gapavi[i], lTime);
                        else
                        {
                                lEnd = AVIStreamEnd(gapavi[i]);
                                lSamp = lEnd + AVIStreamTimeToSample(gapavi[i],
                                                     lTime - lEndTime);
                        }
                    iFrameWidth = (avis.rcFrame.right - avis.rcFrame.left);
                }
                else break;
//
                if (i == giFirstVideo)
                {
                    lCurSamp = lSamp+1;
                    l = AVIStreamSampleToTime(gapavi[i], lCurSamp);
                }
                else
                {
                    l = lTime + MulDiv32((iFrameWidth),gdwMicroSecPerPixel, 1000);
                    lCurSamp = AVIStreamTimeToSample(gapavi[i], l);
                    l = AVIStreamSampleToTime(gapavi[i], lCurSamp);
                }

                if (gapgf[i] && lCurSamp >= AVIStreamStart(gapavi[i])) // &&
                    lpbi = (BITMAPINFOHEADER *)AVIStreamGetFrame(gapgf[i], lCurSamp);
                else
                    lpbi = NULL;

                if(lpbi)
                {
                        g_cDDraw.m_pcDDSPrimary->GetDC(&hdc);
                        DrawDibDraw(ghdd[i], hdc,
                                    x, y,
                                        xl,
                                        yl,
                                        lpbi, NULL,
                                        0, 0, -1, -1,
                                        (i == giFirstVideo) ? 0 :DDF_BACKGROUNDPAL);
                        g_cDDraw.m_pcDDSPrimary->ReleaseDC(hdc);
                        *realplay+=1;
                }
        }
        }
        if(!*realplay)
                wait_any_key();
        //____________________FreeDraw___________________
        aviaudioStop();
        for(i=0;i<gcpavi;i++)
        {
                if(gapgf[i])
                {
                        AVIStreamGetFrameClose(gapgf[i]);
                        gapgf[i]=NULL;
                }
                if(ghdd[i])
                {
                        DrawDibClose(ghdd[i]);
                        ghdd[i]=0;
                }
        }
        giFirstVideo = giFirstAudio = -1;
        //____________________FreeStream___________________
        for(i=gcpavi-1;i>=0;i--)
        {
                if(AVIStreamLength(gapavi[i])==0)
                {
                        AVIStreamRelease(gapavi[i]);
                    if (galpAVIOptions[i]->lpFormat)
                        {
                                GlobalFreePtr(galpAVIOptions[i]->lpFormat);
                        }
                        if (gapgf[i])
                        {
                                AVIStreamGetFrameClose(gapgf[i]);
                                gapgf[i] = NULL;
                        }
                        if (ghdd[i])
                        {
                                DrawDibClose(ghdd[i]);
                                ghdd[i] = 0;
                        }

                    for (j = i; j < gcpavi - 1; j++)
                        {
                                gapavi[j] = gapavi[j+1];
                                galpAVIOptions[j] = galpAVIOptions[j+1];
                                gapgf[j] = gapgf[j+1];
                                ghdd[j] = ghdd[j+1];
                        }
                }
        }
        //____________________FreeAviFile__________________
        AVISaveOptionsFree(gcpavi, galpAVIOptions);
    for (i = 0; i < gcpavi; i++) {
        AVIStreamRelease(gapavi[i]);
    }

    // Good a place as any to make sure audio data gets freed
    gcpavi = 0;

        g_cDSound.Create();
}

void aviaudioCloseDevice(void)
{
    if (shWaveOut)
    {
                while (swBuffers > 0)
                {
                --swBuffers;
                waveOutUnprepareHeader(shWaveOut, salpAudioBuf[swBuffers],
                                        sizeof(WAVEHDR));
                GlobalFreePtr((LPSTR) salpAudioBuf[swBuffers]);
                }
        waveOutClose(shWaveOut);

        shWaveOut = NULL;
    }
}

BOOL CALLBACK aviaudioOpenDevice(HWND hwnd, PAVISTREAM pavi)
{
    MMRESULT            mmResult;
    LPVOID              lpFormat;
    LONG                cbFormat;
    AVISTREAMINFO       strhdr;

    if (!pavi)          // no wave data to play
                return FALSE;

    if (shWaveOut)      // already something playing
                return TRUE;

    spavi = pavi;

    AVIStreamInfo(pavi, &strhdr, sizeof(strhdr));

    slSampleSize = (LONG) strhdr.dwSampleSize;
    if (slSampleSize <= 0 || slSampleSize > AUDIO_BUFFER_SIZE)
                return FALSE;

    AVIStreamFormatSize(pavi, 0, &cbFormat);

    lpFormat = GlobalAllocPtr(GHND, cbFormat);
    if (!lpFormat)
                return FALSE;

    AVIStreamReadFormat(pavi, 0, lpFormat, &cbFormat);

    sdwSamplesPerSec = ((LPWAVEFORMAT) lpFormat)->nSamplesPerSec;
    sdwBytesPerSec = ((LPWAVEFORMAT) lpFormat)->nAvgBytesPerSec;

    mmResult = waveOutOpen(&shWaveOut, (UINT)WAVE_MAPPER, (WAVEFORMATEX *)lpFormat,
                        (DWORD) (UINT) hwnd, 0L, CALLBACK_WINDOW);

    //
    // Maybe we failed because someone is playing sound already.
    // Shut any sound off, and try once more before giving up.
    //
    if (mmResult) {
        sndPlaySound(NULL, 0);
        mmResult = waveOutOpen(&shWaveOut, (UINT)WAVE_MAPPER, (WAVEFORMATEX *)lpFormat,
                        (DWORD) (UINT)hwnd, 0L, CALLBACK_WINDOW);
    }

    if (mmResult != 0)
    {
        // Show error message here?
        return FALSE;
    }

    for (swBuffers = 0; swBuffers < MAX_AUDIO_BUFFERS; swBuffers++)
    {
                if (!(salpAudioBuf[swBuffers] =
                                (LPWAVEHDR)GlobalAllocPtr(GMEM_MOVEABLE | GMEM_SHARE,
                                (DWORD)(sizeof(WAVEHDR) + AUDIO_BUFFER_SIZE))))
                break;
                salpAudioBuf[swBuffers]->dwFlags = WHDR_DONE;
                salpAudioBuf[swBuffers]->lpData = (LPSTR) salpAudioBuf[swBuffers]
                                                    + sizeof(WAVEHDR);
                salpAudioBuf[swBuffers]->dwBufferLength = AUDIO_BUFFER_SIZE;
                if (!waveOutPrepareHeader(shWaveOut, salpAudioBuf[swBuffers],
                                        sizeof(WAVEHDR)))
                continue;

                GlobalFreePtr((LPSTR) salpAudioBuf[swBuffers]);
                break;
    }

    if (swBuffers < MIN_AUDIO_BUFFERS)
    {
                aviaudioCloseDevice();
                return FALSE;
    }

    swBuffersOut = 0;
    swNextBuffer = 0;

    sfPlaying = FALSE;

    return TRUE;
}

LONG CALLBACK aviaudioTime(void)
{
    MMTIME      mmtime;

    if (!sfPlaying)
                return -1;

    mmtime.wType = TIME_SAMPLES;

    waveOutGetPosition(shWaveOut, &mmtime, sizeof(mmtime));

    if (mmtime.wType == TIME_SAMPLES)
                return AVIStreamSampleToTime(spavi, slBegin)
                                + muldiv32(mmtime.u.sample, 1000, sdwSamplesPerSec);
    else if (mmtime.wType == TIME_BYTES)
                return AVIStreamSampleToTime(spavi, slBegin)
                                + muldiv32(mmtime.u.cb, 1000, sdwBytesPerSec);
    else
                return -1;
}

BOOL aviaudioiFillBuffers(void)
{
    LONG                lRead;
        MMRESULT        mmResult;
    LONG                lSamplesToPlay;

    // We're not playing, so do nothing.
    if (!sfPlaying)
                return TRUE;

    while (swBuffersOut < swBuffers)
    {
                if (slCurrent >= slEnd)
                {
                if (sfLooping)
                {
                        // Looping, so go to the beginning.
                        slCurrent = slBegin;
                }
            else
                        break;
                }

        // Figure out how much data should go in this buffer
        lSamplesToPlay = slEnd - slCurrent;
        if (lSamplesToPlay > AUDIO_BUFFER_SIZE / slSampleSize)
            lSamplesToPlay = AUDIO_BUFFER_SIZE / slSampleSize;

        AVIStreamRead(spavi, slCurrent, lSamplesToPlay,
                      salpAudioBuf[swNextBuffer]->lpData,
                      AUDIO_BUFFER_SIZE,
                      (LPLONG)&salpAudioBuf[swNextBuffer]->dwBufferLength,
                      &lRead);

        if (lRead != lSamplesToPlay)
            return FALSE;

        slCurrent += lRead;

        mmResult = waveOutWrite(shWaveOut, salpAudioBuf[swNextBuffer],sizeof(WAVEHDR));

        if (mmResult != 0)
            return FALSE;

        ++swBuffersOut;
        ++swNextBuffer;
        if (swNextBuffer >= swBuffers)
            swNextBuffer = 0;
    }//while

    if (swBuffersOut == 0 && slCurrent >= slEnd)
                aviaudioStop();

    // We've filled all of the buffers we can or want to.
    return TRUE;
}

BOOL CALLBACK aviaudioPlay(HWND hwnd, PAVISTREAM pavi, LONG lStart, LONG lEnd, BOOL fWait)
{
    if (lStart < 0)
                lStart = AVIStreamStart(pavi);

    if (lEnd < 0)
                lEnd = AVIStreamEnd(pavi);

    if (lStart >= lEnd)
                return FALSE;

    if (!aviaudioOpenDevice(hwnd, pavi))
                return FALSE;

    if (!sfPlaying)
    {

                // We're beginning play, so pause until we've filled the buffers
                // for a seamless start
                //
                waveOutPause(shWaveOut);

                slBegin = lStart;
                slCurrent = lStart;
                slEnd = lEnd;
                sfPlaying = TRUE;
    }
    else
    {
                slEnd = lEnd;
    }

    aviaudioiFillBuffers();

    //
    // Now unpause the audio and away it goes!
    //
    waveOutRestart(shWaveOut);

    //
    // Caller wants us not to return until play is finished
    //
    if(fWait)
    {
                while (swBuffersOut > 0)
                Yield();
    }

    return TRUE;
}

void CALLBACK aviaudioMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == MM_WOM_DONE) {
        --swBuffersOut;
        aviaudioiFillBuffers();
    }
}

void CALLBACK aviaudioStop(void)
{
        MMRESULT        mmResult;

    if (shWaveOut != 0)
    {

                mmResult = waveOutReset(shWaveOut);

                sfPlaying = FALSE;

                aviaudioCloseDevice();
    }
}

BOOL GetRGBBitsMask(UHINT *rbits,UHINT *gbits,UHINT *bbits)
{
  DDPIXELFORMAT format;

        if(g_bDirectDraw!=TRUE)
           return FALSE;

        ZeroMemory( &format, sizeof(format) );
        format.dwSize=sizeof(format);
        if(g_cDDraw.m_pcDDSPrimary->GetPixelFormat( &format )!=DD_OK)
           {
            log_error((UCHR *)"StorePixelFormatData() failed",1);
            return FALSE;
           }

       *rbits=(UHINT)format.dwRBitMask ;
       *gbits=(UHINT)format.dwGBitMask ;
       *bbits=(UHINT)format.dwBBitMask;
       return TRUE;
}
