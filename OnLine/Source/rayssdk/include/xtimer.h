/*
**    XTIMER.H
**    timer functions header.
**    ZJian,2000/7/10.
*/
#ifndef _XTIMER_H_INCLUDE_
#define _XTIMER_H_INCLUDE_      1

extern  ULONG   timer_tick00;
extern  ULONG   timer_tick01;
extern  ULONG   timer_tick02;
extern  ULONG   timer_tick03;
extern  ULONG   timer_tick04;
extern  ULONG   timer_tick05;
extern  ULONG   timer_tick06;
extern  ULONG   timer_tick07;
extern  ULONG   timer_tick08;
extern  ULONG   timer_tick09;
extern  ULONG   cdrom_timer_tick;
extern  ULONG   cursor_timer_tick;
extern  ULONG   system_timer_tick;


EXPORT int     FNBACK   init_timer(void);
EXPORT void    FNBACK   free_timer(void);
EXPORT void    FNBACK   active_timer(int bActive);

#endif//_XTIMER_H_INCLUDE_