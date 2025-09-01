/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
 2003 ALL RIGHTS RESERVED.

*file name    : chatdef.h
*owner        : Andy
*description  : 
*modified     : 2004/02/11
******************************************************************************/ 

#ifndef __CHATDEF_H__
#define __CHATDEF_H__

#pragma warning(disable:4100)   //unreferenced formal parameter
#pragma warning(disable:4201)   //nonstandard extension used : nameless struct/union
#pragma warning(disable:4505)   //unreferenced local function has been removed
#pragma warning(disable:4786)   

#include <objbase.h>
#include <basetsd.h>
#include <windows.h>
#include <windowsx.h>
#include <math.h>
#include <string.h>
#include <process.h>
#include <direct.h>
#include <stdlib.h>
#include <ctype.h>
#include <io.h>
#include <time.h>
#include <fcntl.h>
#include <malloc.h>
#include <dos.h>
#include <stdio.h>
#include <stdarg.h>
#include <lmcons.h>
#include <mmsystem.h>
#include <string>
#include <vector>
#include <map>
#include "d3d9types.h"



using namespace std;

#include "engine_global.h"
#include "utility.h"

#include "xvga.h"

#define  UINT8          byte
#define  UINT16         unsigned short

//////////////////////////////////////////////////////////////////////////
//
#define CHAT_SYS_RGB	D3DCOLOR_XRGB(7,245,232)	//系统聊天消息颜色
#define CHAT_WARN_RGB	D3DCOLOR_XRGB(253,41,71)	//警告消息颜色
#define CHAT_DEF_RGB	D3DCOLOR_XRGB(255,255,255)	//默认消息颜色
#define CHAT_NAME_RGB	D3DCOLOR_XRGB(95,155,207)	//人名输入栏消息颜色
#define CHAT_BR_ARGB	D3DCOLOR_ARGB(178,22,38,101)	//背景消息颜色

#define CHAT_OTHER_RGB	D3DCOLOR_XRGB(255,0,0)	//表示>> : 的消息颜色

#define CHAT_BR_SIZE	CRect(4,443, 4+616,468+83+20)	//背景区域
#define CHAT_STRING_SIZE CRect(10,555,4+592,570)			//画字符串区域

#define CHAT_GM_SIZE    CRect(635,351-7,796,541+23)
#define CHAT_GM_STRING_SIZE CRect(639,353,795,540)

//#define CHAT_STRING_SIZE 100,555,400,571			//画字符串区域
//#define CHAT_BR_SIZE	CRect(4,468, 500,468+83+20)	//背景区域

#define CHAT_NOTE_COUNT	6
#define CHAT_NOTE_NUM	6

#define	CHAT_PRIVATE	" >> "
#define CHAT_PUBLIC		" : "

#define MAX_CHAT_HISTORY 28
#define MAX_CHAT_DISPLAY 10


#define CHAT_RED                                              32005//32418//'红'
#define CHAT_GREEN                                            32160//32511//'绿'
#define CHAT_BLUE                                             34253//34013//'蓝'
#define CHAT_BLACK                                            40657//'黑'
#define CHAT_YELLOW                                           30970//40644//'黄'
#define CHAT_WHITE                                            30333//'白'
#define CHAT_PINK                                             31881//'粉'
#define CHAT_FLASH                                            38275//38378//'闪'


#define		SELECTCHATRGB									0x80ffffff	//高亮
#define		DEFAULTCHATRGB									0xffffffff	
#define		DEFAULTCHATHEADRGB								0xffffffff
#define		FLASHTIMEOPEN									1000 //flashTime
#define		FLASHTIMEOFF									500 //flashTime
#define		FLASHCHATTIME									3 //flashchatTime
#define		SCROLLTIME										2 //flashTime
#define     SHOWHEADTIME									10


#define     CHATFACEREFRESH                                 100   //表情刷新速度

//////////////////////////////////////////////////////////////////////////
// struct


enum chat_type 
{
		CHATTYPE_WORLD = 0 ,		//0  世界频道
		CHATTYPE_CURRENT   ,	    //1  当前频道;
		CHATTYPE_PRIVATE   ,		//2  私聊;
		CHATTYPE_SYSTEM			    //3  系统;
};

struct chat_word_t                   
{
	wstring   str_word;
	UINT32    userID;
	BYTE      chat_type_id;
	int       pixel_width;
	int       pixel_height;
	DWORD     font_color;
	DWORD     font_type;
	RECT      rectChat;
	BYTE      showFlag;
	BYTE      showHeadFlag;
	float     showHeadTime;
	char      userName[30];
	RECT      rectShow;
};

struct chat_faceandstring_t{
	BYTE       faceFlag;
	short int  picFlag; //聊天类别图片
	string     facename;
	POINT      pt;
	BYTE       faceW;
	BYTE       faceH;
	wstring    w_str;
	RECT       rect;
	UINT16     dwColor;    //字体颜色
	BYTE       flash;      //字体闪烁
	BYTE       underline;  //下划线
	BYTE       linenum;
};

typedef struct _BMP24_PIXEL
{
	UCHR buff[3];
}BMP24_PIXEL;

typedef struct tagBMP24
{
    SLONG w,h,pitch;       //BMP width,height,pitch(bytes per line)
    BMP24_PIXEL  *line[1];      //BMP line address
} BMP24,*LPBMP24;


typedef map<UINT8,UINT8,less<UINT8> >       LINEINFO;
typedef vector<chat_faceandstring_t>        FACEANDSTRINGLIST;
typedef map<UINT,FACEANDSTRINGLIST>         ARFACEANDSTRINGLIST;
typedef map<UINT,chat_word_t>               CHATWORDLIST;
typedef map<UINT8,LINEINFO>                 LINEINFOLIST;

#include "ichat.h"
#include "chatdata.h"
#include "ugdraw.h"
#include "chatface.h"
#include "chatpanel.h"

extern MDA_GROUP *mda_group;

ULONG                   getCurrentTime(void);
CChatFace*              GetChatFace();
CChatPanel*             GetChatPanel();
bool                    getChatFlash(void); 
bool                    setChatFlash(bool bFlash);
vector<chat_word_t>*    getChatList(void);
CChat*                  getChatObject();
void                    display_img256_buffer_24(SLONG x,SLONG y,UCHR *data_buffer,BMP24 *bitmap, BITMAP* pbrbm,RECT* prt);
SLONG                   mda_group_exec_24(SLONG handle,UCHR *command,SLONG x,SLONG y,BMP24 *bitmap,SLONG mda_command,SLONG value,SLONG alpha_value,
										  SLONG frame_value, BITMAP* pbrbm,RECT* prt);
void                    clear_bitmap24(BMP24 *bmp);
BMP24*                  create_bitmap24(SLONG w,SLONG h);
void                    destroy_bitmap24(BMP24 **bmp);


void					_oper_alpha_color_any_24(BMP24_PIXEL *fore_color,BMP24_PIXEL *back_color,SLONG alpha);

#endif //#ifndef __CHAT_H__