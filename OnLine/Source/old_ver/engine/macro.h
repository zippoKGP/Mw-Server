/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : macro.h
  *owner  		: Stephen
  *description  : 
  *modified     : 2005/2/21
******************************************************************************/ 

#ifndef _MACRO_H_
#define _MACRO_H_

#include "data.h"


typedef struct MENU_ITEM_DATA_STRUCT
{
	RECT pos;
	UCHR text[80];
} MENU_ITEM_DATA;





void exec_macro(UCHR command,UCHR *data,SLONG data_length);
void clear_macro_var(void);
void decode_menu_item(UCHR *data,SLONG *total,MENU_ITEM_DATA *item);
void clear_store_data(void);


// ---------- Macro exec routinue
void mc_exec_say(void);
void mc_exec_ask(void);
void exec_conform(void);
void mc_exec_store_buy(void);
void mc_exec_store_sell(void);
void mc_exec_bank_store(void);
void mc_exec_bank_withdraw(void);
void mc_exec_popshop_push(void);
void mc_exec_popshop_pop(void);




void send_macro_return(UCHR command,SLONG ret_val);






#endif
