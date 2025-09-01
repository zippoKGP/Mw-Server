/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : data_proc.h
  *owner  		: Stephen
  *description  : 
  *modified     : 2005/2/26
******************************************************************************/ 


#ifndef _DATA_PROC_H_
#define _DATA_PROC_H_


#include "data.h"

#define FIGHT_COMMAND_CHARACTER						0
#define FIGHT_COMMAND_BAOBAO						1




void display_character_quick_status(BMP *buffer);
void display_baobao_quick_status(BMP *buffer);
void display_character_quick_exp(BMP *buffer);
void get_near_position(SLONG sx,SLONG sy,SLONG range,SLONG dir,SLONG *tx,SLONG *ty);
SLONG get_map_npc_follow_conut(ULONG follow_id);		// 计算目前有多少人跟随这个id
SLONG get_random_near_offset_position(SLONG sx,SLONG sy,SLONG *tx,SLONG *ty);
void check_screen_zoom(void);
void auto_close_map_npc_handle(SLONG handle);
void get_money_string(SLONG value,UCHR *text);
void set_give_item_index(void);
void set_give_item_index_max(void);
void set_give_item_index_del(SLONG idx,SLONG number);
void set_deal_item_index(void);
void set_deal_item_index_max(void);
void set_deal_item_index_del(SLONG idx,SLONG number);
void clear_detail_item_inst(void);
void make_detail_inst(void);
void display_fight_character_quick_status(BMP *buffer);
void display_fight_baobao_quick_status(BMP *buffer);
void clear_fight_command(void);
void clear_fight_control(void);
void charge_control(void );
void charge_time_control(SLONG idx,SLONG code);
void check_fight_main_command(void);
void set_fight_main_command(SLONG idx,char command);
void check_fight_command_parameter(SLONG npc,char command,ULONG source_id,ULONG *target_id,char *type_no);
SLONG get_fight_npc_index(ULONG id);
SLONG get_character_skill_index(SLONG skill_id);
SLONG get_baobao_skill_index(SLONG skill_id);
void close_character_fight_all_window(void);
void close_baobao_fight_all_window(void);
SLONG gium_strlen(char *title);
void update_fight_status_result(FIGHT_STATUS_RESULT data);
void update_fight_action_result(FIGHT_ACTION_RESULT data);
void update_fight_value_result(FIGHT_VALUE_RESULT data);






#endif
