/*
**    WINMAIN.H
**    Windows main functions header.
**    ZJian,2001.5.20.
*/
#ifndef WINMAIN_H_INCLUDE
#define WINMAIN_H_INCLUDE  1


/* screen mode enums ---------------------------------------------------------------------- */
typedef enum    SCREENMODE_ENUM
{   FULL_SCREEN_MODE        =   0,      //full screen mode
WINDOWS_SCREEN_MODE         =   1,      //windows screen mode
} SCREENMODE_ENUM;



/* globals -------------------------------------------------------------------------------- */
extern  HWND        g_hDDWnd;           /* system window handle         */
extern  BOOL        g_bProgramActive;   /* system program active flag   */
extern  DWORD       g_dwScreenMode;     /* system screen mode           */
extern  RECT        g_rcWindow;         /* system window rect           */
extern  SLONG       g_nScreenWidth;     /* screen pixel width           */
extern  SLONG       g_nScreenHeight;    /* screen pixel height          */
extern  BOOL        g_bIsInitialized;   /* is program initialized sucessfully? */
extern  BOOL        g_enable_f12_switch_screen;     /* is enable f12 switch screen mode? */

extern SLONG		g_nScreenX;
extern SLONG		g_nScreenY;
extern BOOL			g_bImeActive;
extern BOOL         g_bImeCandidate;     //modified by coolly;

/* function prototypes -------------------------------------------------------------------- */
EXPORT  int         FNBACK  FailMsg(char *s);
EXPORT  void        FNBACK  LastErrorMsg(LPCTSTR szTitle);
EXPORT  int WINAPI  FNBACK  RaysDDWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                                          LPSTR lpCmdLine, int nCmdShow);
EXPORT  void        FNBACK  FinishProc();


/*------------------------------------------------------------------------------------------*/
/* The following functions you must use it in your gameSet() for set your application.      */
/*------------------------------------------------------------------------------------------*/
EXPORT  void    FNBACK  SetDDWndName( char *name );
EXPORT  void    FNBACK  SetDDWndIcon( HICON hIcon, HICON hIconSm );
EXPORT  void    FNBACK  SetDDWndRunOnce( BOOL bFlag );
EXPORT  void    FNBACK  SetDDWndScreenMode( DWORD dwMode );
EXPORT  void    FNBACK  SetDDWndProc( WNDPROC pfn );
EXPORT  void    FNBACK  SetDDWndEnableExit( BOOL bEnable );
EXPORT  void    FNBACK  SetDDWndDoDropFiles( void func(int nFiles, int iFile, char *filename) );
EXPORT  void    FNBACK  SetDDWndScreenSize( SLONG width, SLONG height );
EXPORT  void    FNBACK  SetDDWndEnableSwitchScreenMode( SLONG flag );
EXPORT  void    FNBACK  SetDDWndEnableDblClk( SLONG flag);

EXPORT  void    FNBACK  SetDDWndScreenPos( SLONG x, SLONG y);

#endif/*WINMAIN_H_INCLUDE   */
//=======================================================================
