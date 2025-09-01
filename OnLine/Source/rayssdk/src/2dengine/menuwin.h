/*
**      MENUWIN.H
**      Menuwindows functions.
**      ZJian,2000/09/07
*/
#ifndef MENUWIN_H_INCLUDE
#define MENUWIN_H_INCLUDE       1
#include "xgrafx.h"

#define MAX_OPTION_LENGTH       80
#define MAX_TITLE_LENGTH        80
#define MAX_MENUWIN_OPTION      16
#define MAX_EDIT_LENGTH         80

#define MENUWIN_SHOW_NORMAL     0
#define MENUWIN_SHOW_MINIMIZE   1
#define MENUWIN_FLAG_ACTIVE     0x01

#define MENUWIN_ACTION_STAND    0
#define MENUWIN_ACTION_MOVE     1

#define EDITWIN_EDIT_OVER       0
#define EDITWIN_EDIT_INS        1

#define BARWIN_HEIGHT           28


#define MAX_MENU_RECT       512
#define NULL_MENU_RECT      {0,0,0,0,0}
#define NULL_STRING         "\0"
#define NULL_KEY            0x00


#define MAX_KEY_CHOICE      256


typedef struct tagMENU_RECT
{
    SLONG sx,sy,xl,yl;
    ULONG id;
} MENU_RECT,* LPMENU_RECT;;


typedef struct  tagMENUOPT
{
    USTR    name[MAX_OPTION_LENGTH];
    SLONG   id;
} MENUOPT,*LPMENUOPT;

typedef struct  tagMENUWIN
{
    SLONG   flag;   // control flag
    SLONG   show;   // show flag
    USTR    title[MAX_TITLE_LENGTH];    // menu title
    MENUOPT option[MAX_MENUWIN_OPTION]; // menu options
    SLONG   total_option;   // total menu options
    SLONG   start_option;   // start menu options
    SLONG   active_option;  // now active options
    SLONG   win_sx; // window sx
    SLONG   win_sy; // window sy
    SLONG   win_xl; // window xl
    SLONG   win_yl; // window yl
    SLONG   opt_sx; // option relative sx(to win_sx)
    SLONG   opt_sy; // option relative sy
    SLONG   opt_xl; // option xl
    SLONG   opt_yl; // option yl
    SLONG   action; // now process action
    SLONG   org_sx; // store original position( when move the window )
    SLONG   org_sy; // store original position( when move the window )
} MENUWIN,*LPMENUWIN;

typedef struct  tagEDITWIN
{
    SLONG   flag;   //control flag
    SLONG   show;   //show flag
    USTR    title[MAX_TITLE_LENGTH];    //win title
    USTR    string[MAX_EDIT_LENGTH];    //edit string buffer
    SLONG   string_index;   // string edit index
    SLONG   string_max; // max length of edit string
    SLONG   edit_flag;  // edit flag
    SLONG   cursor_tick;    // for show flash cursor
    SLONG   win_sx; // window sx
    SLONG   win_sy; // window sy
    SLONG   win_xl; // window xl
    SLONG   win_yl; // window yl
    SLONG   opt_sx; // option relative sx(to win_sx)
    SLONG   opt_sy; // option relative sy(to win_sy)
    SLONG   opt_xl; // option xl
    SLONG   opt_yl; // option yl
    SLONG   action; // now process action
    SLONG   org_sx; // store original sx position( when move the window )
    SLONG   org_sy; // store original sy position( when move the window )
} EDITWIN,*LPEDITWIN;

typedef struct  tagPOPWIN
{
    MENUOPT option[MAX_MENUWIN_OPTION]; // menu options
    SLONG   total_option;   // total menu options
    SLONG   active_option;  // now active options
    SLONG   win_sx; // window sx
    SLONG   win_sy; // window sy
    SLONG   win_xl; // window xl
    SLONG   win_yl; // window yl
    SLONG   opt_sx; // option relative sx(to win_sx)
    SLONG   opt_sy; // option relative sy
    SLONG   opt_xl; // option xl
    SLONG   opt_yl; // option yl
} POPWIN,*LPPOPWIN;

typedef struct  tagBARWIN
{
    MENUOPT option[MAX_MENUWIN_OPTION]; // menu options
    SLONG   total_option;   // total menu options
    SLONG   active_option;  // now active options
    SLONG   win_sx; // window sx
    SLONG   win_sy; // window sy
    SLONG   win_xl; // window xl
    SLONG   win_yl; // window yl
    SLONG   opt_sx[MAX_MENUWIN_OPTION]; // option relative sx(to win_sx)
    SLONG   opt_sy[MAX_MENUWIN_OPTION]; // option relative sy
    SLONG   opt_xl[MAX_MENUWIN_OPTION]; // option xl
    SLONG   opt_yl[MAX_MENUWIN_OPTION]; // option yl
} BARWIN,*LPBARWIN;


typedef struct  tagKEY_CHOICE
{
    UCHR    key;
    SLONG   choice;
} KEY_CHOICE,*LPKEY_CHOICE;

EXPORT  void    FNBACK  clear_key_choice(void);
EXPORT  void    FNBACK  register_key_choice(KEY_CHOICE kc);
EXPORT  void    FNBACK  kill_key_choice(UCHR key);
EXPORT  void    FNBACK  setup_key_choice(int nn,KEY_CHOICE *kc);
EXPORT  SLONG   FNBACK  get_key_choice(UCHR key);


EXPORT  void    FNBACK  clear_menu_rect(void);
EXPORT  void    FNBACK  register_menu_rect(MENU_RECT mr);
EXPORT  void    FNBACK  kill_menu_rect(ULONG id);
EXPORT  void    FNBACK  setup_menu_rect(int nn,MENU_RECT *mr);
EXPORT  ULONG   FNBACK  get_menu_id(SLONG x,SLONG y);


EXPORT  void    FNBACK  init_menuwin(MENUWIN *mw);
EXPORT  void    FNBACK  redraw_menuwin(MENUWIN *mw,BMP *bitmap);
EXPORT  void    FNBACK  set_menuwin_title(MENUWIN *mw,USTR *title);
EXPORT  SLONG   FNBACK  add_menuwin_option(MENUWIN *mw,MENUOPT *opt);
EXPORT  void    FNBACK  clear_menuwin_option(MENUWIN *mw);
EXPORT  void    FNBACK  set_menuwin_show(MENUWIN *mw,SLONG show);
EXPORT  void    FNBACK  adjust_menuwin_start(MENUWIN *mw,SLONG adjust_flag,SLONG sx,SLONG sy);
EXPORT  SLONG   FNBACK  process_menuwin_choice(MENUWIN *mw);
EXPORT  void    FNBACK  set_menuwin_active_option(MENUWIN *mw,SLONG option_id);

EXPORT  void    FNBACK  init_editwin(EDITWIN *ew);
EXPORT  void    FNBACK  redraw_editwin(EDITWIN *ew,BMP *bitmap);
EXPORT  void    FNBACK  set_editwin_title(EDITWIN *ew,USTR *title);
EXPORT  void    FNBACK  set_editwin_string(EDITWIN *ew,USTR *string);
EXPORT  void    FNBACK  adjust_editwin_start(EDITWIN *ew,SLONG adjust_flag,SLONG sx,SLONG sy);
EXPORT  SLONG   FNBACK  process_editwin_input(EDITWIN *ew);
EXPORT  USTR *  FNBACK  get_editwin_string(EDITWIN *ew);

EXPORT  void    FNBACK  init_popwin(POPWIN *p);
EXPORT  void    FNBACK  redraw_popwin(POPWIN *p,BMP *bitmap);
EXPORT  SLONG   FNBACK  add_popwin_option(POPWIN *p,MENUOPT *opt);
EXPORT  void    FNBACK  adjust_popwin_start(POPWIN *p,SLONG adjust_flag,SLONG sx,SLONG sy);
EXPORT  SLONG   FNBACK  process_popwin_choice(POPWIN *p);
EXPORT  void    FNBACK  set_popwin_active_option(POPWIN *p,SLONG option_id);

EXPORT  void    FNBACK  init_barwin(BARWIN *p);
EXPORT  void    FNBACK  redraw_barwin(BARWIN *p,BMP *bitmap);
EXPORT  SLONG   FNBACK  add_barwin_option(BARWIN *p,MENUOPT *opt);
EXPORT  void    FNBACK  change_barwin_option_string(BARWIN *p,SLONG index,USTR *string);
EXPORT  void    FNBACK  adjust_barwin_start(BARWIN *p,SLONG adjust_flag,SLONG sx,SLONG sy);
EXPORT  SLONG   FNBACK  process_barwin_choice(BARWIN *p);
EXPORT  void    FNBACK  set_barwin_active_option(BARWIN *p,SLONG option_id);
 
#endif//MENUWIN_H_INCLUDE
