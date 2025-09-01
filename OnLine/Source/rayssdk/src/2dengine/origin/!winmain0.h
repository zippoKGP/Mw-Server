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

/* function prototypes -------------------------------------------------------------------- */
EXPORT  int         FNBACK  FailMsg(char *s);
EXPORT  void        FNBACK  LastErrorMsg(LPCTSTR szTitle);
EXPORT  int WINAPI  FNBACK  RaysDDWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                                          LPSTR lpCmdLine, int nCmdShow);


/*------------------------------------------------------------------------------------------*/
/* The following functions you must use it in your gameSet() for set your application.      */
/*------------------------------------------------------------------------------------------*/
EXPORT  void    FNBACK  SetDDWndName( char *name );
EXPORT  void    FNBACK  SetDDWndIcon( HICON hIcon, HICON hIconSm );
EXPORT  void    FNBACK  SetDDWndRunOnce( BOOL bFlag );
EXPORT  void    FNBACK  SetDDWndScreenMode( DWORD dwMode );
EXPORT  void    FNBACK  SetDDWndProc( WNDPROC pfn );
EXPORT  void    FNBACK  SetDDWndEnableExit( BOOL bEnable );


#endif/*WINMAIN_H_INCLUDE   */
//=======================================================================
