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
#define BTN_STYLE_DOWN			0x0010L		//��ť���±�־
//for   UGMENU
#define MENU_STYLE_CASCADING	0x0010L		//�����˵���
//for SLIDER 
#define SLIDER_STYLE_VERTICAL   0x0020L		//������ȡ����ʽ trueΪ��,falseΪ��
#define SLIDER_STYLE_TRACK      0x0040L		//��ʾ����Ƿ��ڻ�����

//for UGHELPBUTTON
#define HELPBTN_STYLE_DRAWHELP  0x0080L		//�Ƿ���ʾ��ť��ʾ����
#define HELPBTN_STYLE_MOUSEON   0x0100L		//����Ƿ��ڰ�ť��
#define HELPBTN_STYLE_DRAWING   0x0200L

//for UGHSBUTTON
#define HSBTN_STYLE_SOUND       0X0400L		//�Ƿ�������

#endif //UG_CTRL_ID__H
