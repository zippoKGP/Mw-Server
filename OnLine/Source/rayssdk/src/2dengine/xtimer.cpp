/*
**      TIMER.CPP
**      Timer functions.
**
**      ZJian,2000.7.13.
*/
#include "rays.h"
#include "xtimer.h"
#include "xsystem.h"

#pragma comment(lib,"winmm")

ULONG   timer_tick00    = 0 ;
ULONG   timer_tick01    = 0 ;
ULONG   timer_tick02    = 0 ;
ULONG   timer_tick03    = 0 ;
ULONG   timer_tick04    = 0 ;
ULONG   timer_tick05    = 0 ;
ULONG   timer_tick06    = 0 ;
ULONG   timer_tick07    = 0 ;
ULONG   timer_tick08    = 0 ;
ULONG   timer_tick09    = 0 ;
ULONG   cdrom_timer_tick    = 0 ;
ULONG   cursor_timer_tick   = 0 ;
ULONG   system_timer_tick   = 0 ;

static UINT timer_id    = 0 ;


static  void    CALLBACK TimerFunc(UINT wID,UINT wUser,DWORD dwUser,DWORD dw1,DWORD dw2)
{
/*
** if you want post message to your app , just add the following
** line, which para hMainWnd is the app window handler :
**
** ::PostMessage(hMainWnd,WM_USER,0,0);
*/
   timer_tick00 ++ ;
   timer_tick01 ++ ;
   timer_tick02 ++ ;
   timer_tick03 ++ ;
   timer_tick04 ++ ;
   timer_tick05 ++ ;
   timer_tick06 ++ ;
   timer_tick07 ++ ;
   timer_tick08 ++ ;
   timer_tick09 ++ ;
   cdrom_timer_tick++;
   cursor_timer_tick++;
   system_timer_tick++;
}

EXPORT   int      FNBACK   init_timer(void)
{
    TIMECAPS caps;
    
    sprintf((char *)print_rec,"TIMER module initialize starting...");
    log_error(1,print_rec);
    
    timeGetDevCaps( &caps,sizeof(caps) );
    sprintf((char *)print_rec,"       Period Max : %d", caps.wPeriodMax );
    log_error(1,print_rec);
    sprintf((char *)print_rec,"       Period Min : %d", caps.wPeriodMin );
    log_error(1,print_rec);

    timeBeginPeriod( caps.wPeriodMin );
    sprintf((char *)print_rec,"     Begin Period : %d", caps.wPeriodMin);
    log_error(1,print_rec);

    timer_id = timeSetEvent( 10/caps.wPeriodMin, caps.wPeriodMin,
        TimerFunc, 0, (UINT)TIME_PERIODIC );
    sprintf((char *)print_rec,"Timer Event Delay : %d ms", 10/caps.wPeriodMin );
    log_error(1,print_rec);
    
    sprintf((char *)print_rec,"TIMER module initialized OKay.");
    log_error(1,print_rec);
    sprintf((char *)print_rec,"\n");
    log_error(1,print_rec);

    return(0);
}

EXPORT   void     FNBACK   free_timer(void)
{
   TIMECAPS caps;
   
   if( timer_id != 0 )
   {
      timeKillEvent( timer_id );
      timer_id = 0 ;
      timeGetDevCaps( &caps, sizeof(caps) );
      timeEndPeriod( caps.wPeriodMin );

      sprintf((char *)print_rec,"TIMER module free OKay.");
      log_error(1,print_rec);
   }
}

EXPORT   void     FNBACK   active_timer(int active)
{
    if(active)
    {
    }
}


//=========================================================================
