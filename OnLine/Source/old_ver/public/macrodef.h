/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : macrodef.h
  *owner  		: Stephen
  *description  : 
  *modified     : 2005/2/21
******************************************************************************/ 

#ifndef _MACRODEF_H_
#define _MACRODEF_H_


#define MAX_MACRO_COMMAND_DEFINE			20
#define MAX_MACRO_PARAMETER					6



typedef struct MACRO_COMMAND_DEFINE_STRUCT
{
	UCHR command_code;
	SLONG var_type[MAX_MACRO_PARAMETER];					// 参数形态  0 -> None  1 -> Number  2 -> String

} MACRO_COMMAND_DEFINE;


enum 
{
	MC_NONE							=	0x00,			// 空的 command
// --------------------- window 部分 -----------------------
	MC_SAY							=	0x20,			// 广播 window 
	MC_ASK							=	0x21,			// 询问 window 

	MC_STORE_BUY					=	0x22,			// 商店买
	MC_STORE_SELL					=	0x23,			// 商店卖
	MC_STORE_OPEN					=	0x24,			// 开店面
	MC_POPSHOP_PUSH					=	0x25,			// 当物品
	MC_POPSHOP_POP					=	0x26,			// 赎回物品
	

	MC_BANK_STORE					=	0x30,			// 银行存钱
	MC_BANK_WITHDRAW				=	0x31,			// 银行取钱

	MC_SYSTEM_MESSAGE				=	0x40,			// 显示系统消息
	MC_CHAT_MESSAGE					=	0x41			// 显示聊天版消息

// --------------------- 数值部分 --------------------------

};





#endif
