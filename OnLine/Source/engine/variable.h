/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : variable.h
  *owner  		: Stephen
  *description  : 
  *modified     : 2004/12/6
******************************************************************************/ 

#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include "system.h"
#include "utility.h"
#include "game.h"
#include "findpath.h"
#include "polling.h"
#include "ugdef.h"
#include "player_info.h"
#include "taskman.h"
#include "data.h"
#include "fight.h"
#include "macrodef.h"
#include "data_proc.h"

// ---------------- System.cpp -------------------
DEBUG_INFO debug_info;

SYSTEM_CONFIG system_config;
CLIENT_CONFIG client_config;
SYSTEM_CONFIG backup_system_config;
COLOR_CONTROL color_control;
GAME_CONTROL game_control;
SERVERINFO *server_info = NULL;
SYSTEM_CONTROL system_control;

UCHR r_full_palette[768];
UCHR g_full_palette[768];
UCHR b_full_palette[768];
UCHR yellow_full_palette[768];
UCHR black_full_palette[768];
UCHR white_full_palette[768];
UCHR mask_full_palette[768];
UCHR point_palette[768];
UCHR gray_palette[768];

SYSTEM_IMAGE system_image;
FIGHT_IMAGE fight_image;
TEXT_OUT_DATA text_out_data;
UCHR conform_text[80];
BMP *fight_background=NULL;

// ------------- Screen control parameter
SLONG map_alpha_value=0;
SLONG screen_alpha_value=0;
UHINT map_alpha_color=0;


// ---------------- Uility.cpp -------------------
VOICE_CONTROL voice_control;
SLONG total_mda_group=0;
MDA_GROUP *mda_group=NULL;
ULONG mda_handle_count=0;
UCHR *system_palette=NULL;
MAP_BASE_DATA map_base_data;
MAP_CONTROL map_control;
MAP_NPC_GROUP *map_npc_group=NULL;
SLONG total_map_npc=0;
NPC_LAYER_SORT *npc_layer_sort=NULL;
SLONG total_npc_layer=0;
MAP_BAR_LAYER map_bar_layer;
MAP_LAYER_TOUCH map_layer_touch;
MESSAGE_STACK message_stack[MAX_MESSAGE_STACK];
EXTRA_MDA_DATA extra_mda_data[MAX_EXTRA_MDA];
EXTRA_MDA_DATA extra_fight_mda_data[MAX_EXTRA_FIGHT_MDA];


// ---------- Ani file use
FRAME_DATA *frame_data=NULL;
SLONG ani_total_frame=0;
SLONG ani_now_frame_no=0;
CONTROL_ANI control_ani_head;
CHAR_VIEW_DATA *char_view_data=NULL;
LAYER_VIEW_DATA *layer_view_data=NULL;
SLONG total_extra_frame_data=0;
EXTRA_FRAME_DATA *extra_frame_data=NULL;


// --------- fight camera control
SLONG goto_frame=-1;
SLONG attack_start=0;
SLONG attack_value=0;
SLONG front_start=0;
SLONG magic_start=0;


// --------- GAME.CPP -----------
MAP_MOVE map_move;
BASE_CHARACTER_INFO *base_character_info=NULL;
SLONG total_base_character=0;
CHANGE_MAP_INFO change_map_info;
UCHR character_phyle_text[4][12]={
	CHARACTER_PHYLE_HUMAN_TEXT,
	CHARACTER_PHYLE_GENUS_TEXT,
	CHARACTER_PHYLE_MAGIC_TEXT,
	CHARACTER_PHYLE_ROBET_TEXT
};


CHAT_DATA_BUFFER chat_data_buffer;
CHAT_DATA_BUFFER hirstory_chat_data_buffer;
SLONG hirstory_channel;
SCREEN_TEXT_DATA screen_text_data[MAX_SCREEN_TEXT];
SLONG screen_text_stack[MAX_SCREEN_TEXT];
CHAT_FRIEND_LIST chat_friend_list[MAX_CHAT_FRIEND_LIST];

CHAT_FRIEND_LIST display_friend_list[MAX_CHAT_FRIEND_LIST];
CHAT_FRIEND_LIST display_temp_list[MAX_CHAT_FRIEND_LIST];
CHAT_FRIEND_LIST display_mask_list[MAX_CHAT_FRIEND_LIST];
CHAT_FRIEND_LIST send_message_data;
CHAT_RECEIVE_DATA chat_receive_data;
bool chat_group_send_flag[MAX_CHAT_FRIEND_LIST];
SLONG chat_group_send_start_idx=0;
FRIEND_DATA_RECORD friend_data_record[MAX_FRIEND_DATA_RECORD];
SLONG friend_data_record_top_index=0;
SLONG friend_data_record_bottom_index=0;


FRIEND_DATA_RECORD display_friend_data_record[MAX_FRIEND_DATA_RECORD];
SLONG display_friend_data_record_top_index=0;
SLONG display_friend_data_record_bottom_index=0;
SLONG display_friend_data_record_display_index=0;

BASE_CHARACTER_INFO chat_search_data;
BASE_CHARACTER_INFO team_list_data[MAX_TEAM_PLAYER];
BASE_CHARACTER_INFO team_request_list_data[MAX_TEAM_REQUEST];
SLONG team_select_index=-1;
SLONG team_request_select_index=-1;
SLONG team_request_select_start_index=0;

SMALL_MAP_DATA small_map_data;

MISSION_DATA mission_data[MAX_MISSION];
SLONG total_mission_data;
SLONG mission_data_start_index;

// --------- TaskMan -----------
SLONG now_exec_window_handle=-1;
SLONG now_delete_window_handle=-1;
SLONG now_active_window_handle=-1;
SLONG now_window_move_handle=-1;
SLONG now_window_move_xoffset=0;
SLONG now_window_move_yoffset=0;


SLONG now_execute_task_id=0;


void (*main_loop_active_task)(void);
void (*ask_yes_exec_task)(void);
void (*ask_no_exec_task)(void);



PHIZ_DATA phiz_data[81]={
	{ { 0+PHIZ_FONT_X*0 ,0+PHIZ_FONT_Y*0 }, "#0" },
	{ { 0+PHIZ_FONT_X*0 ,0+PHIZ_FONT_Y*1 }, "#1" },
	{ { 0+PHIZ_FONT_X*0 ,0+PHIZ_FONT_Y*2 }, "#2" },
	{ { 0+PHIZ_FONT_X*0 ,0+PHIZ_FONT_Y*3 }, "#3" },
	{ { 0+PHIZ_FONT_X*0 ,0+PHIZ_FONT_Y*4 }, "#4" },
	{ { 0+PHIZ_FONT_X*0 ,0+PHIZ_FONT_Y*5 }, "#5" },
	{ { 0+PHIZ_FONT_X*0 ,0+PHIZ_FONT_Y*6 }, "#6" },
	{ { 0+PHIZ_FONT_X*0 ,0+PHIZ_FONT_Y*7 }, "#7" },
	{ { 0+PHIZ_FONT_X*0 ,0+PHIZ_FONT_Y*8 }, "#8" },
	
	{ { 0+PHIZ_FONT_X*1 ,0+PHIZ_FONT_Y*0 }, "#9" },
	{ { 0+PHIZ_FONT_X*1 ,0+PHIZ_FONT_Y*1 }, "#10" },
	{ { 0+PHIZ_FONT_X*1 ,0+PHIZ_FONT_Y*2 }, "#11" },
	{ { 0+PHIZ_FONT_X*1 ,0+PHIZ_FONT_Y*3 }, "#12" },
	{ { 0+PHIZ_FONT_X*1 ,0+PHIZ_FONT_Y*4 }, "#13" },
	{ { 0+PHIZ_FONT_X*1 ,0+PHIZ_FONT_Y*5 }, "#14" },
	{ { 0+PHIZ_FONT_X*1 ,0+PHIZ_FONT_Y*6 }, "#15" },
	{ { 0+PHIZ_FONT_X*1 ,0+PHIZ_FONT_Y*7 }, "#16" },
	{ { 0+PHIZ_FONT_X*1 ,0+PHIZ_FONT_Y*8 }, "#17" },
	
	{ { 0+PHIZ_FONT_X*2 ,0+PHIZ_FONT_Y*0 }, "#18" },
	{ { 0+PHIZ_FONT_X*2 ,0+PHIZ_FONT_Y*1 }, "#19" },
	{ { 0+PHIZ_FONT_X*2 ,0+PHIZ_FONT_Y*2 }, "#20" },
	{ { 0+PHIZ_FONT_X*2 ,0+PHIZ_FONT_Y*3 }, "#21" },
	{ { 0+PHIZ_FONT_X*2 ,0+PHIZ_FONT_Y*4 }, "#22" },
	{ { 0+PHIZ_FONT_X*2 ,0+PHIZ_FONT_Y*5 }, "#23" },
	{ { 0+PHIZ_FONT_X*2 ,0+PHIZ_FONT_Y*6 }, "#24" },
	{ { 0+PHIZ_FONT_X*2 ,0+PHIZ_FONT_Y*7 }, "#25" },
	{ { 0+PHIZ_FONT_X*2 ,0+PHIZ_FONT_Y*8 }, "#26" },
	
	{ { 0+PHIZ_FONT_X*3 ,0+PHIZ_FONT_Y*0 }, "#27" },
	{ { 0+PHIZ_FONT_X*3 ,0+PHIZ_FONT_Y*1 }, "#28" },
	{ { 0+PHIZ_FONT_X*3 ,0+PHIZ_FONT_Y*2 }, "#29" },
	{ { 0+PHIZ_FONT_X*3 ,0+PHIZ_FONT_Y*3 }, "#30" },
	{ { 0+PHIZ_FONT_X*3 ,0+PHIZ_FONT_Y*4 }, "#31" },
	{ { 0+PHIZ_FONT_X*3 ,0+PHIZ_FONT_Y*5 }, "#32" },
	{ { 0+PHIZ_FONT_X*3 ,0+PHIZ_FONT_Y*6 }, "#33" },
	{ { 0+PHIZ_FONT_X*3 ,0+PHIZ_FONT_Y*7 }, "#34" },
	{ { 0+PHIZ_FONT_X*3 ,0+PHIZ_FONT_Y*8 }, "#35" },
	
	{ { 0+PHIZ_FONT_X*4 ,0+PHIZ_FONT_Y*0 }, "#36" },
	{ { 0+PHIZ_FONT_X*4 ,0+PHIZ_FONT_Y*1 }, "#37" },
	{ { 0+PHIZ_FONT_X*4 ,0+PHIZ_FONT_Y*2 }, "#38" },
	{ { 0+PHIZ_FONT_X*4 ,0+PHIZ_FONT_Y*3 }, "#39" },
	{ { 0+PHIZ_FONT_X*4 ,0+PHIZ_FONT_Y*4 }, "#40" },
	{ { 0+PHIZ_FONT_X*4 ,0+PHIZ_FONT_Y*5 }, "#41" },
	{ { 0+PHIZ_FONT_X*4 ,0+PHIZ_FONT_Y*6 }, "#42" },
	{ { 0+PHIZ_FONT_X*4 ,0+PHIZ_FONT_Y*7 }, "#43" },
	{ { 0+PHIZ_FONT_X*4 ,0+PHIZ_FONT_Y*8 }, "#44" },
	
	{ { 0+PHIZ_FONT_X*5 ,0+PHIZ_FONT_Y*0 }, "#45" },
	{ { 0+PHIZ_FONT_X*5 ,0+PHIZ_FONT_Y*1 }, "#46" },
	{ { 0+PHIZ_FONT_X*5 ,0+PHIZ_FONT_Y*2 }, "#47" },
	{ { 0+PHIZ_FONT_X*5 ,0+PHIZ_FONT_Y*3 }, "#48" },
	{ { 0+PHIZ_FONT_X*5 ,0+PHIZ_FONT_Y*4 }, "#49" },
	{ { 0+PHIZ_FONT_X*5 ,0+PHIZ_FONT_Y*5 }, "#50" },
	{ { 0+PHIZ_FONT_X*5 ,0+PHIZ_FONT_Y*6 }, "#51" },
	{ { 0+PHIZ_FONT_X*5 ,0+PHIZ_FONT_Y*7 }, "#52" },
	{ { 0+PHIZ_FONT_X*5 ,0+PHIZ_FONT_Y*8 }, "#53" },
	
	{ { 0+PHIZ_FONT_X*6 ,0+PHIZ_FONT_Y*0 }, "#54" },
	{ { 0+PHIZ_FONT_X*6 ,0+PHIZ_FONT_Y*1 }, "#74" },			// ---
	{ { 0+PHIZ_FONT_X*6 ,0+PHIZ_FONT_Y*2 }, "#75" },
	{ { 0+PHIZ_FONT_X*6 ,0+PHIZ_FONT_Y*3 }, "#76" },
	{ { 0+PHIZ_FONT_X*6 ,0+PHIZ_FONT_Y*4 }, "#77" },
	{ { 0+PHIZ_FONT_X*6 ,0+PHIZ_FONT_Y*5 }, "#78" },
	{ { 0+PHIZ_FONT_X*6 ,0+PHIZ_FONT_Y*6 }, "#79" },
	{ { 0+PHIZ_FONT_X*6 ,0+PHIZ_FONT_Y*7 }, "#80" },
	{ { 0+PHIZ_FONT_X*6 ,0+PHIZ_FONT_Y*8 }, "#81" },
	
	{ { 0+PHIZ_FONT_X*7 ,0+PHIZ_FONT_Y*0 }, "#82" },
	{ { 0+PHIZ_FONT_X*7 ,0+PHIZ_FONT_Y*1 }, "#83" },
	{ { 0+PHIZ_FONT_X*7 ,0+PHIZ_FONT_Y*2 }, "#84" },
	{ { 0+PHIZ_FONT_X*7 ,0+PHIZ_FONT_Y*3 }, "#85" },
	{ { 0+PHIZ_FONT_X*7 ,0+PHIZ_FONT_Y*4 }, "#86" },
	{ { 0+PHIZ_FONT_X*7 ,0+PHIZ_FONT_Y*5 }, "#87" },
	{ { 0+PHIZ_FONT_X*7 ,0+PHIZ_FONT_Y*6 }, "#88" },
	{ { 0+PHIZ_FONT_X*7 ,0+PHIZ_FONT_Y*7 }, "#89" },
	{ { 0+PHIZ_FONT_X*7 ,0+PHIZ_FONT_Y*8 }, "#90" },
	
	{ { 0+PHIZ_FONT_X*8 ,0+PHIZ_FONT_Y*0 }, "#91" },
	{ { 0+PHIZ_FONT_X*8 ,0+PHIZ_FONT_Y*1 }, "#92" },
	{ { 0+PHIZ_FONT_X*8 ,0+PHIZ_FONT_Y*2 }, "#93" },
	{ { 0+PHIZ_FONT_X*8 ,0+PHIZ_FONT_Y*3 }, "#94" },
	{ { 0+PHIZ_FONT_X*8 ,0+PHIZ_FONT_Y*4 }, "#95" },
	{ { 0+PHIZ_FONT_X*8 ,0+PHIZ_FONT_Y*5 }, "#96" },
	{ { 0+PHIZ_FONT_X*8 ,0+PHIZ_FONT_Y*6 }, "#97" },
	{ { 0+PHIZ_FONT_X*8 ,0+PHIZ_FONT_Y*7 }, "#98" },
	{ { 0+PHIZ_FONT_X*8 ,0+PHIZ_FONT_Y*8 }, "#99" }
	
};



SYSTEM_TASK_TABLE system_task_table[MAX_SYSTEM_TASK]={
// ------------------------------- 0 
	{	TASK_SYSTEM_INITIALIZE,				// 系统启动   
		"TASK_SYSTEM_INITIALIZE",
		&initialize_system,
		NULL,
		TASK_SHOW_TITLE,					
//		TASK_SERVER_SELECT,		
//		TASK_SELECT_MAINFUNCTION,

		TASK_SYSTEM_QUIT,
		TASK_SYSTEM_QUIT 
	},
// ------------------------------- 1 
	{	TASK_SHOW_TITLE,					// show title 
		"TASK_SHOW_TITLE",
		&show_title,
		NULL,
		TASK_SELECT_MAINFUNCTION,			
		TASK_SYSTEM_QUIT,
		TASK_SYSTEM_QUIT 
	},
// ------------------------------- 2 	
	{	TASK_SELECT_MAINFUNCTION,			// 主登入界面 
		"TASK_SELECT_MAINFUNCTION",
		&select_main_function,
		NULL,
		TASK_SYSTEM_BROADCAST,
		TASK_SYSTEM_QUIT,					// 保留给 show 工作人员
		TASK_SYSTEM_QUIT 
	},
// ------------------------------- 3 
	{	TASK_SYSTEM_BROADCAST,				// 系统公告界面 
		"TASK_SYSTEM_BROADCAST",
		&system_broadcast,
		NULL,
		TASK_SERVER_SELECT,
		TASK_SYSTEM_QUIT,
		TASK_SYSTEM_QUIT 
	},
// ------------------------------- 4 
	{	TASK_SERVER_SELECT,					// 选择主机 
		"TASK_SERVER_SELECT",
		&server_select,
		NULL,
		TASK_ACCOUNT_INPUT,
		TASK_SELECT_MAINFUNCTION,			// ----- > cancel 回到主登入界面
		TASK_SYSTEM_QUIT 
	},
// ------------------------------- 5 
	{	TASK_ACCOUNT_INPUT,					// 帐号输入界面 
		"TASK_ACCOUNT_INPUT",
		&account_input,
		NULL,
		TASK_SELECT_CHARACTER_IN,
		TASK_SELECT_MAINFUNCTION,			// ----- > cancel 回到主登入界面
		TASK_SYSTEM_QUIT 
	},
// ------------------------------- 6 
	{	TASK_SELECT_CHARACTER_IN,			// 主角选择界面进入画面 
		"TASK_SELECT_CHARACTER_IN",
		&select_character_in,
		NULL,
		TASK_SELECT_CHARACTER,
		TASK_SELECT_MAINFUNCTION,			// ----- > cancel 回到主登入界面
		TASK_SYSTEM_QUIT 
	},
// ------------------------------- 7	
	{	TASK_SELECT_CHARACTER,				// 6 主角选择界面
		"TASK_SELECT_CHARACTER",
		&select_character,
		NULL,
		TASK_GAME_LOGIN,
		TASK_SELECT_MAINFUNCTION,			// ----- > cancel 回到主登入界面
		TASK_SYSTEM_QUIT 
	},
// ------------------------------- 8
	{	TASK_CREATE_CHARACTER,				// 创建人物
		"TASK_CREATE_CHARACTER",
		&create_character,
		NULL,
		TASK_GAME_LOGIN,
		TASK_SELECT_CHARACTER,
		TASK_SYSTEM_QUIT 
	},
// ------------------------------- 9 
	{	TASK_GAME_LOGIN,					//  游戏主登陆
		"TASK_GAME_LOGIN",
		&game_login,
		NULL,
		TASK_GAME_MAIN,
		TASK_SELECT_MAINFUNCTION,
		TASK_SYSTEM_QUIT 
	},


// ------------------------------- 10 
	{	TASK_GAME_MAIN,						//  游戏主回路
		"TASK_GAME_MAIN",
		&game_main_loop,
		NULL,
		TASK_SELECT_MAINFUNCTION,
		TASK_SELECT_MAINFUNCTION,
		TASK_SYSTEM_QUIT 
	},
// ------------------------------- 11 
	{	TASK_SYSTEM_QUIT,					//  
		"TASK_SYSTEM_QUIT",
		NULL,
		NULL,
		TASK_SYSTEM_QUIT,
		TASK_SYSTEM_QUIT,
		TASK_SYSTEM_QUIT 
	},
// ------------------------------- 12 
	{	TASK_SYSTEM_QUIT,					//  
		"TASK_SYSTEM_QUIT",
		NULL,
		NULL,
		TASK_SYSTEM_QUIT,
		TASK_SYSTEM_QUIT,
		TASK_SYSTEM_QUIT 
	},
// ------------------------------- 13
	{	TASK_SYSTEM_QUIT,					//  
		"TASK_SYSTEM_QUIT",
		NULL,
		NULL,
		TASK_SYSTEM_QUIT,
		TASK_SYSTEM_QUIT,
		TASK_SYSTEM_QUIT 
	},
// ------------------------------- 14 
	{	TASK_SYSTEM_QUIT,					//  
		"TASK_SYSTEM_QUIT",
		NULL,
		NULL,
		TASK_SYSTEM_QUIT,
		TASK_SYSTEM_QUIT,
		TASK_SYSTEM_QUIT 
	},
// ------------------------------- 15 
	{	TASK_SYSTEM_QUIT,					//  
		"TASK_SYSTEM_QUIT",
		NULL,
		NULL,
		TASK_SYSTEM_QUIT,
		TASK_SYSTEM_QUIT,
		TASK_SYSTEM_QUIT 
	},
// ------------------------------- 16 
	{	TASK_SYSTEM_QUIT,					//  
		"TASK_SYSTEM_QUIT",
		NULL,
		NULL,
		TASK_SYSTEM_QUIT,
		TASK_SYSTEM_QUIT,
		TASK_SYSTEM_QUIT 
	},
// ------------------------------- 17 
	{	TASK_SYSTEM_QUIT,					//  
		"TASK_SYSTEM_QUIT",
		NULL,
		NULL,
		TASK_SYSTEM_QUIT,
		TASK_SYSTEM_QUIT,
		TASK_SYSTEM_QUIT 
	},
// ------------------------------- 18 
	{	TASK_SYSTEM_QUIT,					//  
		"TASK_SYSTEM_QUIT",
		NULL,
		NULL,
		TASK_SYSTEM_QUIT,
		TASK_SYSTEM_QUIT,
		TASK_SYSTEM_QUIT 
	},
// ------------------------------- 19 
	{	TASK_SYSTEM_QUIT,					//  系统结束
		"TASK_SYSTEM_QUIT",
		NULL,
		NULL,
		TASK_SYSTEM_QUIT,
		TASK_SYSTEM_QUIT,
		TASK_SYSTEM_QUIT 
	}

	
};



// -------- Fight --------------
CAMERA_ZOOM camera_zoom[MAX_CAMERA_ZOOM];
SLONG now_camera_zoom=0;
SLONG now_camera_pan_x=0;
SLONG now_camera_pan_y=0;


IMG256_GROUP *fight_image_index[MAX_FIGHT_IMAGE_NO]={
		&fight_image.menu_body,				// 0	主界面底板
		&fight_image.hp_body,				// 1	血槽
		&fight_image.mp_body,				// 2	法槽
		&fight_image.speed_body,			// 3	充电槽
		&fight_image.action_body0,			// 4	行动指令框动画
		&fight_image.action_body1,			// 5	行动指令框动画
		&fight_image.action_body2,			// 6	行动指令框动画
		&fight_image.attack_select,			// 7	攻击(选中)
		&fight_image.attack_push,			// 8	攻击(点击)
		&fight_image.magic_select,			// 9	法术(选中)
		&fight_image.magic_push,			// 10   法术(点击)
		&fight_image.item_select,			// 11   道具(选中)
		&fight_image.item_push,				// 12   道具(点击)
		&fight_image.protect_select,		// 13   保护(选中)
		&fight_image.protect_push,			// 14   保护(点击)
		&fight_image.recover_select,		// 15   防御(选中)
		&fight_image.recover_push,			// 16   防御(点击)
		&fight_image.run_select,			// 17   逃跑(选中)
		&fight_image.run_push,				// 18   逃跑(点击)
		&fight_image.change_select,			// 19   更换(选中)
		&fight_image.change_push,			// 20   更换(点击)
		&fight_image.auto_hp_select,		// 21   自动回血(选中)
		&fight_image.auto_hp_push,			// 22   自动回血(点击)
		&fight_image.auto_mp_select,		// 23   自动回法(选中)
		&fight_image.auto_mp_push,			// 24   自动回法(点击)
		&fight_image.ai_select,				// 25   AI选单(选中)
		&fight_image.ai_push,				// 26   AI选单(点击)
		&fight_image.auto_attack_select,	// 27	自动攻击(选中)
		&fight_image.auto_attack_push,		// 28	自动攻击(点击)	
		&fight_image.map_menu_mask,			// 29   地图选单 mask
		&fight_image.fight_baobao_icon		// 30   宝宝肖像
		
};



/*******************************************************************************************************************}
{																													}
{	人 物 数 值 部 分																								}
{																													}
********************************************************************************************************************/
BASE_CHARACTER_DATA base_character_data;
CLIENT_CHARACTER_POINT_BACK character_point_back;
CLIENT_CHARACTER_POINT_BACK baobao_point_back[MAX_BAOBAO_STACK];
BASE_BAOBAO_DATA base_baobao_data;
PUBLIC_SKILL_DATA character_skill_data[MAX_CHARACTER_SKILL_NO];
SLONG total_baobao_stack=0;
SLONG top_baobao_stack=0;
SLONG baobao_stack_index=-1;
SERVER_CHARACTER_FASTNESS character_fastness_data;
SERVER_BAOBAO_FASTNESS baobao_fastness_data[MAX_BAOBAO_STACK];
SERVER_CHARACTER_POCKER character_pocket_data; 
SERVER_CHARACTER_EQUIP character_equip_data;
SERVER_CHARACTER_BANK character_bank_data;
SERVER_CHARACTER_POPSHOP character_popshop_data;
SERVER_STORE_DATA store_data;
CLIENT_ITEM_INST detail_item_inst;
FIGHT_BAOBAO_SKILL_DATA baobao_skill_data[MAX_BAOBAO_SKILL_NO];

// ---------- Fight ---------------
FIGHT_NPC_GROUP fight_npc_group[MAX_FIGHT_UNIT];


POSITION fight_position[MAX_FIGHT_UNIT]={
	{ 1100,700 },				// 0
	{ 1000,680 },				// 1
	{ 1050,790 },				// 2
	{  950,770 },				// 3
	{ 1150,610 },				// 4
	{ 1050,590 },				// 5
	{ 1000,880-15 },			// 6
	{  900,860 },				// 7
	{ 1200,520 },				// 8
	{ 1100,500 },				// 9
	// ---
	{ 500,700 },				// 10
	{ 600,680 },				// 15
	{ 550,610 },				// 12
	{ 650,590 },				// 17
	{ 450,790 },				// 11
	{ 550,770 },				// 16
	{ 600,520 },				// 14
	{ 700,500 },				// 19
	{ 400,880-15 },				// 13
	{ 500,860 }					// 18
};



MACRO_COMMAND_DEFINE macro_command_define[MAX_MACRO_COMMAND_DEFINE]={
	{	MC_SAY,				2,0,0,0,0,0		},		// SAY			0
	{	MC_ASK,				2,2,0,0,0,0		},		// ASK			1
	// ---
	{	MC_STORE_BUY,		1,0,0,0,0,0		},		// 商店编号		2
	{	MC_STORE_SELL,		0,0,0,0,0,0		},		// 卖			3
	{	MC_STORE_OPEN,		0,0,0,0,0,0		},		// 开店面		4
	{	MC_POPSHOP_PUSH,	0,0,0,0,0,0		},		// 当			5
	{	MC_POPSHOP_POP,		0,0,0,0,0,0		},		// 赎			6
	
	// ---
	{	MC_BANK_STORE,		0,0,0,0,0,0		},		// 存			7
	{	MC_BANK_WITHDRAW,	0,0,0,0,0,0		},		// 提			8
	// ---
	{	MC_SYSTEM_MESSAGE,	2,0,0,0,0,0		},		// 显示系统消息		9
	{	MC_CHAT_MESSAGE,	2,0,0,0,0,0		},		// 显示聊天版消息	10
	// ---
	{	MC_NONE,			0,0,0,0,0,0		},		//					11
	{	MC_NONE,			0,0,0,0,0,0		},		//					12
	{	MC_NONE,			0,0,0,0,0,0		},		//					13
	{	MC_NONE,			0,0,0,0,0,0		},		//					14
	{	MC_NONE,			0,0,0,0,0,0		},		//					15
	{	MC_NONE,			0,0,0,0,0,0		},		//					16
	{	MC_NONE,			0,0,0,0,0,0		},		//					17
	{	MC_NONE,			0,0,0,0,0,0		},		//					18
	{	MC_NONE,			0,0,0,0,0,0		}		//					19
	
};



CAMERA_DATA fight_default_pos;

FIGHT_COMMAND_DATA character_fight_command_data;
FIGHT_COMMAND_DATA baobao_fight_command_data;

FIGHT_COMMAND_DATA backup_character_fight_command_data;
FIGHT_COMMAND_DATA backup_baobao_fight_command_data;

FIGHT_CONTROL_DATA fight_control_data;
FIGHT_CHARGE_TIME_DATA character_charge_time_data;
FIGHT_CHARGE_TIME_DATA baobao_charge_time_data;
FIGHT_INST_DATA fight_inst_data;
FIGHT_SCREEN_TEXT_DATA fight_screen_text_data[MAX_FIGHT_SCREEN_TEXT];
PUBLIC_TITLE_DATA base_title_data;

// --------------------- Fight Action 所需要的资料结构
FIGHT_ACTION_SINGLE_PHYSICS_DATA fight_action_single_physics_data;
FIGHT_ACTION_SINGLE_PHYSICS_DOUBLE_DATA fight_action_single_physics_double_data;
FIGHT_ACTION_SINGLE_MAGIC_DATA fight_action_single_magic_data;
FIGHT_ACTION_SINGLE_MAGIC_DOUBLE_DATA fight_action_single_magic_double_data;
FIGHT_ACTION_CHANGE_BAOBAO_DATA fight_action_change_baobao_data;


FIGHT_ACTION_MULTI_MAGIC_DATA fight_action_multi_magic_data;
FIGHT_ACTION_MULTI_MAGIC_DOUBLE_DATA fight_action_multi_magic_double_data;
// ---------



// ---- Emote
//////////////////////////////////////////////////////////////////////////
//

EMOTE_DATA emote_data[TOTAL_EMOTE]={
	{ MSG_EMOTE_0 },
	{ MSG_EMOTE_1 },
	{ MSG_EMOTE_2 },
	{ MSG_EMOTE_3 },
	{ MSG_EMOTE_4 },
	{ MSG_EMOTE_5 },
	{ MSG_EMOTE_6 },
	{ MSG_EMOTE_7 },
	{ MSG_EMOTE_8 },
	{ MSG_EMOTE_9 },

	{ MSG_EMOTE_10 },
	{ MSG_EMOTE_11 },
	{ MSG_EMOTE_12 },
	{ MSG_EMOTE_13 },
	{ MSG_EMOTE_14 },
	{ MSG_EMOTE_15 },
	{ MSG_EMOTE_16 },
	{ MSG_EMOTE_17 },
	{ MSG_EMOTE_18 },
	{ MSG_EMOTE_19 },

	{ MSG_EMOTE_20 },
	{ MSG_EMOTE_21 },
	{ MSG_EMOTE_22 },
	{ MSG_EMOTE_23 },
	{ MSG_EMOTE_24 },
	{ MSG_EMOTE_25 },
	{ MSG_EMOTE_26 },
	{ MSG_EMOTE_27 },
	{ MSG_EMOTE_28 },
	{ MSG_EMOTE_29 },

	{ MSG_EMOTE_30 },
	{ MSG_EMOTE_31 },
	{ MSG_EMOTE_32 },
	{ MSG_EMOTE_33 },
	{ MSG_EMOTE_34 },
	{ MSG_EMOTE_35 },
	{ MSG_EMOTE_36 },
	{ MSG_EMOTE_37 },
	{ MSG_EMOTE_38 },
	{ MSG_EMOTE_39 },

	{ MSG_EMOTE_40 },
	{ MSG_EMOTE_41 },
	{ MSG_EMOTE_42 },
	{ MSG_EMOTE_43 },
	{ MSG_EMOTE_44 },
	{ MSG_EMOTE_45 },
	{ MSG_EMOTE_46 },
	{ MSG_EMOTE_47 },
	{ MSG_EMOTE_48 },
	{ MSG_EMOTE_49 },
	
	{ MSG_EMOTE_50 },
	{ MSG_EMOTE_51 },
	{ MSG_EMOTE_52 },
	{ MSG_EMOTE_53 },
	{ MSG_EMOTE_54 },
	{ MSG_EMOTE_55 },
	{ MSG_EMOTE_56 },
	{ MSG_EMOTE_57 },
	{ MSG_EMOTE_58 },
	{ MSG_EMOTE_59 },
	
	{ MSG_EMOTE_60 },
	{ MSG_EMOTE_61 },
	{ MSG_EMOTE_62 },
	{ MSG_EMOTE_63 },
	{ MSG_EMOTE_64 },
	{ MSG_EMOTE_65 },
	{ MSG_EMOTE_66 },
	{ MSG_EMOTE_67 },
	{ MSG_EMOTE_68 },
	{ MSG_EMOTE_69 },
	
	{ MSG_EMOTE_70 },
	{ MSG_EMOTE_71 },
	{ MSG_EMOTE_72 },
	{ MSG_EMOTE_73 },
	{ MSG_EMOTE_74 },
	{ MSG_EMOTE_75 },
	{ MSG_EMOTE_76 },
	{ MSG_EMOTE_77 },
	{ MSG_EMOTE_78 },
	{ MSG_EMOTE_79 },

	{ MSG_EMOTE_80 },
	{ MSG_EMOTE_81 },
	{ MSG_EMOTE_82 },
	{ MSG_EMOTE_83 },
	{ MSG_EMOTE_84 },
	{ MSG_EMOTE_85 },
	{ MSG_EMOTE_86 },
	{ MSG_EMOTE_87 },
	{ MSG_EMOTE_88 },
	{ MSG_EMOTE_89 },
	
	{ MSG_EMOTE_90 },
	{ MSG_EMOTE_91 },
	{ MSG_EMOTE_92 },
	{ MSG_EMOTE_93 },
	{ MSG_EMOTE_94 },
	{ MSG_EMOTE_95 },
	{ MSG_EMOTE_96 },
	{ MSG_EMOTE_97 },
	{ MSG_EMOTE_98 },
	{ MSG_EMOTE_99 },

	{ MSG_EMOTE_100 },
	{ MSG_EMOTE_101 },
	{ MSG_EMOTE_102 },
	{ MSG_EMOTE_103 },
	{ MSG_EMOTE_104 },
	{ MSG_EMOTE_105 },
	{ MSG_EMOTE_106 },
	{ MSG_EMOTE_107 },
	{ MSG_EMOTE_108 },
	{ MSG_EMOTE_109 },

	{ MSG_EMOTE_110 },
	{ MSG_EMOTE_111 },
	{ MSG_EMOTE_112 },
	{ MSG_EMOTE_113 },
	{ MSG_EMOTE_114 },
	{ MSG_EMOTE_115 },
	{ MSG_EMOTE_116 },
	{ MSG_EMOTE_117 },
	{ MSG_EMOTE_118 },
	{ MSG_EMOTE_119 },

	{ MSG_EMOTE_120 },
	{ MSG_EMOTE_121 },
	{ MSG_EMOTE_122 },
	{ MSG_EMOTE_123 },
	{ MSG_EMOTE_124 },
	{ MSG_EMOTE_125 },
	{ MSG_EMOTE_126 },
	{ MSG_EMOTE_127 },
	{ MSG_EMOTE_128 },
	{ MSG_EMOTE_129 },

	{ MSG_EMOTE_130 },
	{ MSG_EMOTE_131 },
	{ MSG_EMOTE_132 },
	{ MSG_EMOTE_133 },
	{ MSG_EMOTE_134 },
	{ MSG_EMOTE_135 },
	{ MSG_EMOTE_136 },
	{ MSG_EMOTE_137 },
	{ MSG_EMOTE_138 },
	{ MSG_EMOTE_139 },

	{ MSG_EMOTE_140 },
	{ MSG_EMOTE_141 },
	{ MSG_EMOTE_142 },
	{ MSG_EMOTE_143 },
	{ MSG_EMOTE_144 },
	{ MSG_EMOTE_145 },
	{ MSG_EMOTE_146 },
	{ MSG_EMOTE_147 },
	{ MSG_EMOTE_148 },
	{ MSG_EMOTE_149 },
	
	{ MSG_EMOTE_150 },
	{ MSG_EMOTE_151 },
	{ MSG_EMOTE_152 },
	{ MSG_EMOTE_153 },
	{ MSG_EMOTE_154 },
	{ MSG_EMOTE_155 },
	{ MSG_EMOTE_156 },
	{ MSG_EMOTE_157 },
	{ MSG_EMOTE_158 },
	{ MSG_EMOTE_159 },
	
	{ MSG_EMOTE_160 },
	{ MSG_EMOTE_161 },
	{ MSG_EMOTE_162 },
	{ MSG_EMOTE_163 },
	{ MSG_EMOTE_164 },
	{ MSG_EMOTE_165 },
	{ MSG_EMOTE_166 },
	{ MSG_EMOTE_167 },
	{ MSG_EMOTE_168 },
	{ MSG_EMOTE_169 },
	
	{ MSG_EMOTE_170 },
	{ MSG_EMOTE_171 },
	{ MSG_EMOTE_172 },
	{ MSG_EMOTE_173 },
	{ MSG_EMOTE_174 },
	{ MSG_EMOTE_175 },
	{ MSG_EMOTE_176 },
	{ MSG_EMOTE_177 },
	{ MSG_EMOTE_178 },
	{ MSG_EMOTE_179 },

	{ MSG_EMOTE_180 },
	{ MSG_EMOTE_181 },
	{ MSG_EMOTE_182 },
	{ MSG_EMOTE_183 },
	{ MSG_EMOTE_184 },
	{ MSG_EMOTE_185 },
	{ MSG_EMOTE_186 },
	{ MSG_EMOTE_187 },
	{ MSG_EMOTE_188 },
	{ MSG_EMOTE_189 },
	
	{ MSG_EMOTE_190 },
	{ MSG_EMOTE_191 },
	{ MSG_EMOTE_192 },
	{ MSG_EMOTE_193 },
	{ MSG_EMOTE_194 },
	{ MSG_EMOTE_195 },
	{ MSG_EMOTE_196 },
	{ MSG_EMOTE_197 },
	{ MSG_EMOTE_198 },
	{ MSG_EMOTE_199 }
	
};













#endif