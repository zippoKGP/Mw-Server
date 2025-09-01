/*
**    XINPUT.H
**    DirectInput functions header.
**    ZJian,2000/7/10.
*/
#ifndef XINPUT_H_INCLUDE
#define XINPUT_H_INCLUDE        1
#include "xcak.h"


#define SHOW_WINDOW_CURSOR      0x01
#define SHOW_IMAGE_CURSOR       0x02


EXPORT  int     FNBACK  init_input(void);
EXPORT  void    FNBACK  free_input(void);
EXPORT  void    FNBACK  active_input(int bActive);
//
EXPORT  UCHR    FNBACK  read_system_key(void);
EXPORT  UCHR    FNBACK  read_data_key(void);
EXPORT  void    FNBACK  reset_key(void);
EXPORT  void    FNBACK  reset_data_key(void);
EXPORT  void    FNBACK  wait_tick(ULONG no);
EXPORT  void    FNBACK  wait_key(SLONG key);
EXPORT  UCHR    FNBACK  wait_any_key(void);
EXPORT  void    FNBACK  wait_key_time(SLONG key,SLONG no);
EXPORT  void    FNBACK  clear_time_delay(void);
EXPORT  ULONG   FNBACK  get_time_delay(void);
EXPORT  void    FNBACK  wait_time_delay(ULONG count);
EXPORT  void    FNBACK  fnKeyBoardInterrupt(UCHR keycode);
//
EXPORT  SLONG   FNBACK  fnMouseInterrupt(UINT message,WPARAM wParam,LPARAM lParam);
EXPORT  void    FNBACK  show_mouse(SLONG flag);
EXPORT  void    FNBACK  set_mouse_cursor(SLONG type);
EXPORT  void    FNBACK  set_mouse_position(SLONG xpos,SLONG ypos);
EXPORT  void    FNBACK  get_mouse_position(SLONG *xpos,SLONG *ypos);
EXPORT  UCHR    FNBACK  get_mouse_key(void);
EXPORT  UCHR    FNBACK  read_mouse_key(void);       // it's same as get_mouse_key()
EXPORT  void    FNBACK  wait_mouse_any_key(void);
EXPORT  void    FNBACK  wait_mouse_key(UCHR key);
EXPORT  SLONG   FNBACK  check_mouse_shift(void);
EXPORT  SLONG   FNBACK  check_mouse_control(void);
EXPORT  void    FNBACK  reset_mouse(void);
EXPORT  void    FNBACK  reset_mouse_key(void);      // it's same as reset_mouse()
EXPORT  SLONG   FNBACK  load_mouse_cursor(SLONG index,HCURSOR hCursor);
EXPORT  void    FNBACK  set_mouse_spot(SLONG index,SLONG x,SLONG y);
//
EXPORT  SLONG   FNBACK  init_mouse_image_cursor(void);
EXPORT  void    FNBACK  free_mouse_image_cursor(void);
EXPORT  SLONG   FNBACK  load_mouse_image_cursor(SLONG index,USTR *filename);
EXPORT  SLONG   FNBACK  make_mouse_image_cursor(SLONG index, CAKE_FRAME_ANI *image_cfa, SLONG frames);
EXPORT  void    FNBACK  redraw_mouse_image_cursor(char *pbuffer,long pitch,long width,long height);
//
EXPORT  SHINT   FNBACK  fnCheckCtrlKey(void);
EXPORT  SHINT   FNBACK  fnCheckLeftCtrlKey(void);
EXPORT  SHINT   FNBACK  fnCheckRightCtrlKey(void);
EXPORT  SHINT   FNBACK  fnCheckAltKey(void);
EXPORT  SHINT   FNBACK  fnCheckLeftAltKey(void);
EXPORT  SHINT   FNBACK  fnCheckRightAltKey(void);
EXPORT  SHINT   FNBACK  fnCheckShiftKey(void);
EXPORT  SHINT   FNBACK  fnCheckLeftShiftKey(void);
EXPORT  SHINT   FNBACK  fnCheckRightShiftKey(void);
//
EXPORT  void    FNBACK  interrupt_char_key(WPARAM wParam);
EXPORT  void    FNBACK  reset_char_key(void);
EXPORT  UCHR    FNBACK  get_char_key(int i);
EXPORT  UCHR    FNBACK  get_char_key_num(void);

//

#endif//XINPUT_H_INCLUDE
