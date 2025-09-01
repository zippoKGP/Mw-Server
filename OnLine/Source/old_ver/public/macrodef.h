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
	SLONG var_type[MAX_MACRO_PARAMETER];					// ������̬  0 -> None  1 -> Number  2 -> String

} MACRO_COMMAND_DEFINE;


enum 
{
	MC_NONE							=	0x00,			// �յ� command
// --------------------- window ���� -----------------------
	MC_SAY							=	0x20,			// �㲥 window 
	MC_ASK							=	0x21,			// ѯ�� window 

	MC_STORE_BUY					=	0x22,			// �̵���
	MC_STORE_SELL					=	0x23,			// �̵���
	MC_STORE_OPEN					=	0x24,			// ������
	MC_POPSHOP_PUSH					=	0x25,			// ����Ʒ
	MC_POPSHOP_POP					=	0x26,			// �����Ʒ
	

	MC_BANK_STORE					=	0x30,			// ���д�Ǯ
	MC_BANK_WITHDRAW				=	0x31,			// ����ȡǮ

	MC_SYSTEM_MESSAGE				=	0x40,			// ��ʾϵͳ��Ϣ
	MC_CHAT_MESSAGE					=	0x41			// ��ʾ�������Ϣ

// --------------------- ��ֵ���� --------------------------

};





#endif
