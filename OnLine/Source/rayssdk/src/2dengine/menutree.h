/*
**      MENUTREE.H
**      MenuTree functions header.
**      Jian,2000/8/17.
*/
#ifndef MENUTREE_H_INCLUDE
#define MENUTREE_H_INCLUDE   1
#include "xgrafx.h"

#define MAX_MENU_NAME_LENGTH    40

#define MENU_SON                0
#define MENU_BROTHER            1

#define MENU_TITLE              1
#define MENU_POPUP              2

typedef struct  tagMENU_CELL        MENU_CELL,*LPMENU_CELL;
typedef struct  tagMENU_TREE        MENU_TREE,*LPMENU_TREE;

struct  tagMENU_CELL
{
    SLONG   self_id;                    // self node identify
    SLONG   father_id;                  // father node identify
    SLONG   sx,sy,xl,yl;                // node show position
    USTR    name[MAX_MENU_NAME_LENGTH]; // node show name
    UCHR    key;                        // node control key
    SLONG   (* func )(void);            // node function
};
#define ENDOFMENUCELL   {-1,-1,0,0,0,0,"ENDMENU",0x00,NULL}

struct  tagMENU_TREE
{
    MENU_CELL * mc;             // point to menu cell 
    MENU_TREE * father;         // point to father menu
    MENU_TREE * left;           // point to sub menu(son)
    MENU_TREE * right;          // point to next menu(brother)
};


EXPORT  void    FNBACK  init_menutree(MENU_TREE **mt);
EXPORT  void    FNBACK  destroy_menutree(MENU_TREE **mt);
EXPORT  void    FNBACK  draw_menutree(MENU_TREE *mt,MENU_TREE *p,BMP *bit_map);
EXPORT  void    FNBACK  setup_menutree(MENU_TREE *mt,MENU_CELL *mc);
EXPORT  void    FNBACK  exec_menutree(MENU_TREE *mt);
EXPORT  void    FNBACK  set_menutree_redraw(void (*func)(BMP *bit_map));
EXPORT  void    FNBACK  set_menutree_exit(SLONG (*func)(void));
EXPORT  SLONG   FNBACK  exit_menutree(void);

#endif//MENUTREE_H_INCLUDE
