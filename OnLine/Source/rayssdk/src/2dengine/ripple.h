/*
**      RIPPLE.H
**      ripple effect functions
**      ZJian,2000/09/06
*/
#ifndef RIPPLE_H_INCLUDE
#define RIPPLE_H_INCLUDE    1

EXPORT  int     FNBACK  init_ripple(void);
EXPORT  void    FNBACK  free_ripple(void);
EXPORT  void    FNBACK  active_ripple(int active);
EXPORT  void    FNBACK  clear_ripple(void);
EXPORT  void    FNBACK  spread_ripple(void);
EXPORT  void    FNBACK  render_ripple(BMP *bitmap);
EXPORT  void    FNBACK  drop_stone(SLONG x,SLONG y,SLONG stone_size,SLONG stone_weight);

#endif//RIPPLE_H_INCLUDE
