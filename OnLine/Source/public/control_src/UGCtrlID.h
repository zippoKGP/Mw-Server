/************************************************************************* 
*****
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.
*file name    : UGCtrlID.h
*owner        : Matrol
*description  : flag for control 
*modified     : 1. created on 2004/6/9 by Matrol
*************************************************************************
****/  
#ifndef UG_CTRL_ID__H
#define UG_CTRL_ID__H

#define UG_CTRL_BUTTON			0x00000001
#define UG_CTRL_DIALOG			0x00000006
#define UG_CTRL_EDIT			0x00000002
#define UG_CTRL_SLIDER			0x00000010
#define UG_CTRL_MENU			0x00000003
#define UG_CTRL_MENUITEM		0x00000004

//for UGCONTRL
#define UG_STYLE_ENABLE			0x0001L
#define UG_STYLE_VISUALE		0x0002L
#define UG_STYLE_FOCUS			0x0004L

//for   UGBUTTON
#define BTN_STYLE_DOWN			0x0010L		//按钮按下标志
//for   UGMENU
#define MENU_STYLE_CASCADING	0x0010L		//级联菜单打开
//for SLIDER 
#define SLIDER_STYLE_VERTICAL   0x0020L		//背景剪取方向方式 true为横,false为竖
#define SLIDER_STYLE_TRACK      0x0040L		//表示鼠标是否按在滑块上

//for UGHELPBUTTON
#define HELPBTN_STYLE_DRAWHELP  0x0080L		//是否显示按钮提示帮助
#define HELPBTN_STYLE_MOUSEON   0x0100L		//鼠标是否在按钮上
#define HELPBTN_STYLE_DRAWING   0x0200L

//for UGHSBUTTON
#define HSBTN_STYLE_SOUND       0X0400L		//是否有声音

#endif //UG_CTRL_ID__H
