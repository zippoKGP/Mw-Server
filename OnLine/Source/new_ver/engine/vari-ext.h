/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : vari-ext.h
  *owner  		: Stephen
  *description  : 
  *modified     : 2004/12/6
******************************************************************************/ 

#ifndef _VARIEXT_H_
#define _VARIEXT_H_

#include "system.h"
#include "utility.h"
#include "game.h"
#include "findpath.h"
#include "player_info.h"
#include "taskman.h"
#include "chat_msg.h"
#include "edit.h"
#include "fight.h"
#include "macrodef.h"
#include "data_proc.h"

// ------------------ System.cpp -----------------
extern DEBUG_INFO debug_info;

extern SYSTEM_CONFIG system_config;
extern CLIENT_CONFIG client_config;
extern SYSTEM_CONFIG backup_system_config;
extern COLOR_CONTROL color_control;
extern GAME_CONTROL game_control;
extern SERVERINFO *server_info;
extern MAP_BASE_DATA map_base_data;
extern MAP_CONTROL map_control;
extern MAP_NPC_GROUP *map_npc_group;
extern NPC_LAYER_SORT *npc_layer_sort;
extern SLONG total_npc_layer;
extern SYSTEM_CONTROL system_control;
extern UCHR r_full_palette[768];
extern UCHR g_full_palette[768];
extern UCHR b_full_palette[768];
extern UCHR yellow_full_palette[768];
extern UCHR black_full_palette[768];
extern UCHR white_full_palette[768];
extern UCHR mask_full_palette[768];
extern UCHR point_palette[768];
extern UCHR gray_palette[768];
extern SYSTEM_IMAGE system_image;
extern FIGHT_IMAGE fight_image;
extern MESSAGE_STACK message_stack[MAX_MESSAGE_STACK];
extern TEXT_OUT_DATA text_out_data;
extern CHANGE_MAP_INFO change_map_info;
extern CChatEdit s_editChat;
extern CFriendEdit s_FriendEdit;
extern CFindNickNameBox g_FindNickNameEdit;
extern CPetNameEdit g_PetNameEdit;
extern CFindIDBox g_FindIDEdit;
extern UCHR conform_text[80];
extern BMP *fight_background;
extern CNumberEdit g_GiveEdit;
extern CNumberEdit g_DealEdit;
extern CNumberEdit g_BankEdit;

// ------------- Screen control parameter
extern SLONG map_alpha_value;
extern SLONG screen_alpha_value;
extern UHINT map_alpha_color;


// ------------------ utility.cpp ----------------
extern VOICE_CONTROL voice_control;
extern SLONG total_mda_group;
extern MDA_GROUP *mda_group;
extern ULONG mda_handle_count;
extern UCHR *system_palette;
extern MAP_BASE_DATA map_base_data;
extern MAP_CONTROL map_control;
extern MAP_NPC_GROUP *map_npc_group;
extern SLONG total_map_npc;
extern MAP_BAR_LAYER map_bar_layer;
extern MAP_LAYER_TOUCH map_layer_touch;
extern EXTRA_MDA_DATA extra_mda_data[MAX_EXTRA_MDA];
extern EXTRA_MDA_DATA extra_fight_mda_data[MAX_EXTRA_FIGHT_MDA];


// ---------- Ani file use
extern FRAME_DATA *frame_data;
extern SLONG ani_total_frame;
extern SLONG ani_now_frame_no;
extern CONTROL_ANI control_ani_head;
extern CHAR_VIEW_DATA *char_view_data;
extern LAYER_VIEW_DATA *layer_view_data;
extern SLONG total_extra_frame_data;
extern EXTRA_FRAME_DATA *extra_frame_data;


// --------- fight camera control
extern SLONG goto_frame;
extern SLONG attack_start;
extern SLONG attack_value;
extern SLONG front_start;
extern SLONG magic_start;


// -------- GAME.CPP ------
extern MAP_MOVE map_move;
extern BASE_CHARACTER_INFO *base_character_info;
extern SLONG total_base_character;
extern UCHR character_phyle_text[4][12];
extern SCREEN_TEXT_DATA screen_text_data[MAX_SCREEN_TEXT];
extern SLONG screen_text_stack[MAX_SCREEN_TEXT];
extern PHIZ_DATA phiz_data[81];
extern CHAT_DATA_BUFFER chat_data_buffer;
extern CHAT_DATA_BUFFER hirstory_chat_data_buffer;
extern SLONG hirstory_channel;
extern CHAT_FRIEND_LIST chat_friend_list[MAX_CHAT_FRIEND_LIST];
extern CHAT_FRIEND_LIST display_friend_list[MAX_CHAT_FRIEND_LIST];
extern CHAT_FRIEND_LIST display_temp_list[MAX_CHAT_FRIEND_LIST];
extern CHAT_FRIEND_LIST display_mask_list[MAX_CHAT_FRIEND_LIST];
extern CHAT_FRIEND_LIST send_message_data;
extern CHAT_RECEIVE_DATA chat_receive_data;
extern bool chat_group_send_flag[MAX_CHAT_FRIEND_LIST];
extern SLONG chat_group_send_start_idx;
extern FRIEND_DATA_RECORD friend_data_record[MAX_FRIEND_DATA_RECORD];
extern SLONG friend_data_record_top_index;
extern SLONG friend_data_record_bottom_index;
extern FRIEND_DATA_RECORD display_friend_data_record[MAX_FRIEND_DATA_RECORD];
extern SLONG display_friend_data_record_top_index;
extern SLONG display_friend_data_record_bottom_index;
extern SLONG display_friend_data_record_display_index;
extern BASE_CHARACTER_INFO chat_search_data;
extern BASE_CHARACTER_INFO team_list_data[MAX_TEAM_PLAYER];
extern BASE_CHARACTER_INFO team_request_list_data[MAX_TEAM_PLAYER];
extern SLONG team_select_index;
extern SLONG team_request_select_index;
extern SLONG team_request_select_start_index;
extern SMALL_MAP_DATA small_map_data;
extern MISSION_DATA mission_data[MAX_MISSION];
extern SLONG total_mission_data;
extern SLONG mission_data_start_index;


// -------- TaskMan --------
extern SLONG now_exec_window_handle;
extern SLONG now_delete_window_handle;
extern SLONG now_active_window_handle;
extern SLONG now_window_move_handle;
extern SLONG now_window_move_xoffset;
extern SLONG now_window_move_yoffset;


extern void (*main_loop_active_task)(void);
extern void (*ask_yes_exec_task)(void);
extern void (*ask_no_exec_task)(void);


extern SLONG now_execute_task_id;
extern SYSTEM_TASK_TABLE system_task_table[MAX_SYSTEM_TASK];
	


// -------- Fight --------------
extern CAMERA_ZOOM camera_zoom[MAX_CAMERA_ZOOM];
extern SLONG now_camera_zoom;
extern SLONG now_camera_pan_x;
extern SLONG now_camera_pan_y;
extern IMG256_GROUP *fight_image_index[MAX_FIGHT_IMAGE_NO];
	


/*******************************************************************************************************************}
{																													}
{	人 物 数 值 部 分																								}
{																													}
********************************************************************************************************************/
extern BASE_CHARACTER_DATA base_character_data;
extern CLIENT_CHARACTER_POINT_BACK character_point_back;
extern CLIENT_CHARACTER_POINT_BACK baobao_point_back[MAX_BAOBAO_STACK];
extern BASE_BAOBAO_DATA base_baobao_data;
extern SLONG total_baobao_stack;
extern SLONG top_baobao_stack;
extern SLONG baobao_stack_index;
extern SERVER_CHARACTER_FASTNESS character_fastness_data;
extern SERVER_BAOBAO_FASTNESS baobao_fastness_data[MAX_BAOBAO_STACK];
extern SERVER_CHARACTER_POCKER character_pocket_data; 
extern SERVER_CHARACTER_EQUIP character_equip_data;
extern SERVER_CHARACTER_BANK character_bank_data;
extern SERVER_CHARACTER_POPSHOP character_popshop_data;
extern SERVER_STORE_DATA store_data;
extern CLIENT_ITEM_INST detail_item_inst;

extern FIGHT_BAOBAO_SKILL_DATA baobao_skill_data[MAX_BAOBAO_SKILL_NO];
extern PUBLIC_SKILL_DATA character_skill_data[MAX_CHARACTER_SKILL_NO];

extern MACRO_COMMAND_DEFINE macro_command_define[MAX_MACRO_COMMAND_DEFINE];
	
// --------------- Fight -------------------
extern FIGHT_NPC_GROUP fight_npc_group[MAX_FIGHT_UNIT];
extern POSITION fight_position[MAX_FIGHT_UNIT];
extern CAMERA_DATA fight_default_pos;
extern FIGHT_COMMAND_DATA character_fight_command_data;
extern FIGHT_COMMAND_DATA baobao_fight_command_data;
extern FIGHT_CONTROL_DATA fight_control_data;
extern FIGHT_CHARGE_TIME_DATA character_charge_time_data;
extern FIGHT_CHARGE_TIME_DATA baobao_charge_time_data;
extern FIGHT_INST_DATA fight_inst_data;
extern FIGHT_SCREEN_TEXT_DATA fight_screen_text_data[MAX_FIGHT_SCREEN_TEXT];


// --------------------- Fight Action 所需要的资料结构
extern FIGHT_ACTION_SINGLE_PHYSICS_DATA fight_action_single_physics_data;
extern FIGHT_ACTION_SINGLE_PHYSICS_DOUBLE_DATA fight_action_single_physics_double_data;
extern FIGHT_ACTION_SINGLE_MAGIC_DATA fight_action_single_magic_data;
extern FIGHT_ACTION_SINGLE_MAGIC_DOUBLE_DATA fight_action_single_magic_double_data;
extern FIGHT_ACTION_CHANGE_BAOBAO_DATA fight_action_change_baobao_data;

extern FIGHT_ACTION_MULTI_MAGIC_DATA fight_action_multi_magic_data;
extern FIGHT_ACTION_MULTI_MAGIC_DOUBLE_DATA fight_action_multi_magic_double_data;




// --- emote
extern EMOTE_DATA emote_data[TOTAL_EMOTE];
	

#endif