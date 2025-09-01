/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : system.h
  *owner  		: Stephen
  *description  : 
  *modified     : 2004/12/6
******************************************************************************/ 

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "utility.h"
#include "data.h"


#define SYSTEM_MAIN_DELAY		50


enum{

	MOUSE_IMG_STAND=					0,			// 主鼠标
	MOUSE_IMG_ERROR=					1,			// 点击不能
	MOUSE_IMG_TEAM= 					2,			// 组队
	MOUSE_IMG_ATTACK=					3,			// 攻击
	MOUSE_IMG_GIVE=						4,			// 给予
	MOUSE_IMG_DEAL=						5,			// 交易
	MOUSE_IMG_FRIEND=					6,			// 加好友
	MOUSE_IMG_ITEM=						7,			// 使用物品
	MOUSE_IMG_MACRO=					8			// 对话

};





#define MESSAGE_DELAY					1000*3			// 3 秒


#define MASK_ALPHA_LEVEL				255*75/100

#define MAX_CHAT_STACK					16

/***************************************************************************************}
{	SYSTEM struct define area															}
{	系统参数设定档案																	}
****************************************************************************************/
typedef struct SYSTEM_CONFIG_STRUCT
{
	UCHR copyright[19];			// 'UNIGIUM SYSTEM' config file Head
	UCHR text_end_code;			// 0x1A 
// ----------- main system -----------
	SLONG window_mode;			// 0 -> 视窗模式 , 1 -> 全屏模式
	SLONG music_flag;			// 0 -> 音乐关闭 , 1 -> 音乐开启
	SLONG voice_flag;			// 0 -> 音效关闭 , 1 -> 音效开启
	SLONG volume;				// 音乐音效音量 ( 0 -> 100 )
	SLONG record_flag;			// 游戏录像开关 ( 0 -> 正常 ,1 -> 游戏录像中 )
	SLONG fight_flag;			// 切磋开关 ( 0 -> 关闭, 1 -> 切磋打开 )
	SLONG mail_flag;			// 信件接受 ( 0 -> 关闭, 1 -> 信件接受打开 )
	SLONG item_flag;			// 物品接受 ( 0 -> 关闭, 1 -> 物品接受打开 )
	SLONG friend_flag;			// 好友开关 ( 0 -> 关闭, 1 -> 加入好友打开 )
	SLONG team_flag;			// 组队开关 ( 0 -> 关闭, 1 -> 允许组队加入 )
	SLONG head_talk;			// 头顶对话开关 ( 0 -> 关闭, 1 -> 允许头顶对话显示 )
	SLONG strange_flag;			// 陌生人开关 ( 0 -> 关闭, 1 -> 拒绝陌生人消息 )
	SLONG quick_menu;			// 右键选单开关 ( 0 -> 关闭, 1 -> 开启右键选单 )
	SLONG clothes_flag;			// 装备显示开关 ( 0 -> 关闭, 1 -> 显示装备价值 )
// -----------------------------------	
	SLONG default_server_id;	// 默认服务器

// ---------------- chat channel data
	SLONG channel_screen_flag;	// 当前频道开关
	SLONG channel_team_flag;	// 队伍频道开关
	SLONG channel_person_flag;	// 私聊频道开关
	SLONG channel_group_flag;	// 工会频道开关
	SLONG channel_sellbuy_flag;	// 经济频道开关
	SLONG channel_world_flag;	// 世界频道开关

} SYSTEM_CONFIG;


/***************************************************************************************}
{	GAME struct define area																}
{	游戏参数结构																		}
****************************************************************************************/
typedef struct GAME_CONTROL_STRUCT
{
// --- config
	UCHR music_filename[256];		// 音乐档案名称
	UCHR server_name[256];			// 服务器名称
	SLONG music_loop_flag;			// Loop flag
	SLONG music_playing;			// 是否正在播放音乐
	SLONG npc_display_flag;
	SLONG map_zoom_index;			// MAP 放大 idx
	SLONG deal_type;				// 交易物品，或者宝宝
// --- main loop
	SLONG main_loop_mode;			// 目前战斗还是地图模式
	SLONG main_loop_task;			// 目前主回路状态 
	bool main_loop_break;			// 是否跳出主回路
	SLONG main_loop_ret_val;		// main_loop 回传值
	SLONG main_loop_select_npc_idx;	// 主回路选择NPC idx
	SLONG main_loop_select_npc_task;// 主回路选择NPC 的task

// --- 玩法部分
	ULONG game_give_id;				// 玩家给与人的 ID
	ULONG game_attack_id;			// 玩家攻击的 ID
	ULONG game_deal_id;				// 与玩家交易的 ID
	bool character_skill_ready;		// 玩家 skill data ready 
	SLONG skill_start_index;		// skill data display start idx
	SLONG skill_select_index;		// skill data selected idx
	bool baobao_skill_ready;		// 宝宝 skill_ready 
	SLONG baobao_skill_index;		// 宝宝 skill data index
	
// --- screen text
	ULONG screen_text_timer;		// SCREEN_TEXT 用的 更换表情 timer


// --- 天气系统
	bool weather_enable;			// 目前地图是否执行天气系统
	ULONG weather_timer;			// 天气控制 timer
	ULONG weather_add_timer;		// 天气移动 timer
	ULONG weather_change_timer;		// 天气风向控制 timer;

// --- 聊天系统
	SLONG chat_channel;				// 目前聊天频道
	ULONG chat_timer;				// 聊天控制时间
	UCHR chat_stack_buffer[MAX_CHAT_STACK][MAX_CHAT_LENGTH];		// 玩家输入历史资料
	SLONG chat_stack_index;			// 目前 stack index
	SLONG chat_stack_find_index;	// 目前寻找的 idx
	SLONG chat_window_type;			// 目前聊天输出 window  0 -> window , 1 -> 内部
	SLONG chat_pause_flag;			// pause flag
	SLONG chat_message_in;			// 有讯息来了

// --- main_loop 开启的 window 程序控制
	SLONG window_task;		
	SLONG old_window_task;			// 

// --- channel window
	SLONG window_channel;			// 频道切换 window 的 handle
// --- system window
	SLONG window_system;			// 系统 window 的 handle
	SLONG window_system_button_idx; // system window 的 button idx	

// --- ask_quit 
	SLONG window_quit;				// Quit winqod
	SLONG window_quit_button_idx;

// --- 聊天器
	SLONG window_chat;				// 聊天器 window handle
	SLONG window_chat_type;			// 聊天器目前种类
	SLONG window_chat_select_idx;	// 选择的 idx
	
	SLONG window_chat_friend_start_idx;
	SLONG window_chat_friend_end_idx;
	SLONG window_chat_temp_start_idx;
	SLONG window_chat_temp_end_idx;
	SLONG window_chat_mask_start_idx;
	SLONG window_chat_mask_end_idx;
	
// --- 队伍
	SLONG team_message_in;			// 有人申请加入队员

// --- 表情符号
	SLONG window_phiz;				// 表情符号

// --- 人物状态
	SLONG window_character_status;	// 人物状态

// --- 宝宝状态
	SLONG window_baobao_status;		// 宝宝状态

// --- 历史资料
	SLONG window_history;			// 历史资料

// --- 道具物品
	SLONG window_item;				// 道具物品

// --- 任务
	SLONG window_mission;			// 任务

// --- 公会
	SLONG window_group;				// 公会

// --- Send message					
	SLONG window_send_message;		// 发送讯息

// --- Rceive message
	SLONG window_receive_message;	// 收讯息
	
// --- chat attrib	
	SLONG window_chat_attrib;		// 好友属性

// --- chat group send
	SLONG window_chat_group_send;	// 好友群发

// --- chat hirstory
	SLONG window_chat_hirstory;		// 好友历史资料

// --- team
	SLONG window_team;				// 队伍列表

// --- chat search
	SLONG window_chat_search;		// 好友查询

// --- team request
	SLONG window_team_request;		// 请求列表

// --- small map 
	SLONG window_small_map;			// 小地图显示

// --- world map
	SLONG window_world_map;			// 世界地图

// --- Macro
	SLONG window_macro;				// 剧本 window

// --- Give 
	SLONG window_give;				// 给与

// --- Deal
	SLONG window_deal;				// 交易

// --- Skill
	SLONG window_skill;				// 技能查看

// --- title
	SLONG window_title;				// 头衔更换

// --- baobao item
	SLONG window_baobao_item;		// 宠物物品使用

// --- character fastness	
	SLONG window_character_fastness;	// 人物抗性

// --- baobao fastness
	SLONG window_baobao_fastness;		// 宝宝抗性

// --- fight character skill
	SLONG window_fight_character_skill;	// 战斗任务技能

// --- fight baobao skill
	SLONG window_fight_baobao_skill;	// 宝宝战斗技能

// --- fight character item handle
	SLONG window_fight_character_item;	// 战斗人物物品

// --- fight baobao item handle
	SLONG window_fight_baobao_item;		// 战斗人物物品

// --- fight baobao change
	SLONG window_fight_baobao_change;	// 战斗宝宝更换

// --- fight talk window 
	SLONG window_fight_talk;			// 战斗快速留言
	
} GAME_CONTROL;


/***************************************************************************************}
{	color control																		}
****************************************************************************************/
typedef struct COLOR_CONTROL_STRUCT
{
	UHINT white;
	UHINT black;
	UHINT red;
	UHINT green;
	UHINT blue;
	UHINT yellow;
	UHINT pink;
	UHINT cyan;
	UHINT gray;
	UHINT low_gray;
	UHINT light_green;
	UHINT light_blue;
	UHINT oringe;

	UHINT life_base0;
	UHINT life_base1;
	UHINT life_base2;
	UHINT life_base3;

	UHINT window_blue;
	
} COLOR_CONTROL;

/***************************************************************************************}
{	System control																		}
****************************************************************************************/
typedef struct SYSTEM_CONTROL_STRUCT
{
	ULONG system_timer;				// 系统控制同步时间
	ULONG control_user_id;			// 玩家的 ID
	SLONG control_npc_idx;			// 玩家的 map_npc_group(IDX)
	SLONG point_npc_idx;			// 鼠标目前所指到的 NPC IDX 
	ULONG person_npc_id;			// 玩家私聊 ID
	UCHR person_name[MAX_NAME_LENGTH+1];	// 私聊玩家姓名
	ULONG macro_npc_id;				// 目前执行 macro npc id
// --------- mouse data 
	ULONG mouse_timer;
	SLONG mouse_x;					// 滑鼠坐标
	SLONG mouse_y;
	SLONG zoom_mouse_x;				// Zoom 滑鼠坐标
	SLONG zoom_mouse_y;
	UCHR mouse_key;					// 滑鼠按键
	UCHR key;						// keyboard 按键
	UCHR data_key;					// 资料 key
	bool alt_key;					// 是否按下 alt key
	
} SYSTEM_CONTROL;



typedef struct SYSTEM_IMAGE_STRUCT
{
// -------------------------- 基本资料------------------------
	SLONG character_head;				// 人物头像
	SLONG weather_handle;				// 天气图标
	SLONG status_handle;				// 人物icon
	SLONG full_handle;
	ULONG icon_timer;			

// -------------------------- 讯息框 -------------------------
	IMG256_GROUP msg_TopLeft;
	IMG256_GROUP msg_Top;
	IMG256_GROUP msg_TopRight;
	IMG256_GROUP msg_Left;
	IMG256_GROUP msg_Middle;
	IMG256_GROUP msg_Right;
	IMG256_GROUP msg_BottomLeft;
	IMG256_GROUP msg_Bottom;
	IMG256_GROUP msg_BottomRight;
// -------------------------- 地图主界面 -------------------
	IMG256_GROUP status_body;			// 人物与宝宝主体		0
	IMG256_GROUP map_body;				// 小地图主体			1
	IMG256_GROUP map_main_body;			// 地图主界面主体		2	
	IMG256_GROUP channel_body;			// 频道选择主体			3
	IMG256_GROUP face_font;				// 头像外框				4
	IMG256_GROUP char_hp;				// 人物 HP body			5
	IMG256_GROUP char_mp;				// 人物 MP body			6
	IMG256_GROUP baobao_hp;				// 宝宝 HP				7
	IMG256_GROUP baobao_exp;			// 宝宝 EXP				8
	IMG256_GROUP baobao_mp;				// 宝宝 HP				9
	IMG256_GROUP char_exp;				// 人物 exp				10
	SLONG main_menu_handle;
	BUTTON_STATUS button_channel_active;		// 大勾
	BUTTON_STATUS button_map_main_menu[17];		// 主menu 按钮
	BUTTON_STATUS button_small_map[3];			// 缩小地图
	BUTTON_STATUS button_chat_channel[6];		// 频道
// --------------------------- 系统主界面 -------------------------------	
	SLONG system_menu_handle;
	BUTTON_STATUS system_background;	// 底图 0
	IMG256_GROUP volume;				// 音量大小 1
	BUTTON_STATUS system_close;			// 关闭按钮 2
	BUTTON_STATUS sub_volume;			// 音量降低 4
	BUTTON_STATUS add_volume;			// 音量加大 6
	BUTTON_STATUS system_save;			// 保存按钮 8
	BUTTON_STATUS system_help;			// 游戏帮助 11
	BUTTON_STATUS system_bbs;			// 游戏论坛 14
	BUTTON_STATUS system_quit;			// 退出游戏 17
// ---------------------------- 结束游戏界面 -----------------------------------
	SLONG quit_menu_handle;
	BUTTON_STATUS quit_body;			// 底图
	BUTTON_STATUS quit_restart;			// 重新进入游戏
	BUTTON_STATUS quit_quit;			// 结束游戏
	BUTTON_STATUS quit_cancel;			// 回到游戏
// --------------------------- 可移动 window 底板 ------------------------------
	IMG256_GROUP chat_body;				// 聊天器
	IMG256_GROUP character_attrib_body;	// 人物属性
	IMG256_GROUP baobao_attrib_body;	// 宝宝属性
	IMG256_GROUP history_body;			// 历史资料
	IMG256_GROUP item_body;				// 物品装备
	IMG256_GROUP team_body;				// 组队
	IMG256_GROUP give_body;				// 给与
	IMG256_GROUP deal_body;				// 交易
	IMG256_GROUP mission_body;			// 任务
	IMG256_GROUP group_body;			// 公会
	IMG256_GROUP chat_receive_body;		// 聊天器 接受
	IMG256_GROUP chat_send_body;		// 聊天器 发送
	IMG256_GROUP chat_setup;			// 聊天器 setup
	IMG256_GROUP chat_search;			// 聊天器 搜寻
//--
	IMG256_GROUP chat_attrib_body;		// 聊天器属性察看
	IMG256_GROUP chat_group_send_body;	// 聊天器群发
	IMG256_GROUP chat_hirstory_mask;	// 公用面板mask

	IMG256_GROUP item_public_body;		// 24 物品栏公用部分
	IMG256_GROUP item_action_body;		// 25 给与动作部分
	IMG256_GROUP deal_action_body;		// 26 deal

	IMG256_GROUP skill_body;			// 21 技能察看
	IMG256_GROUP title_body;			// 22 头衔

	IMG256_GROUP conform_body;			// 19 确认  
		
	IMG256_GROUP baobao_item_action;	// 20 宝宝物品动作部分	

	IMG256_GROUP baobao_item_mask;		// 32 宝宝物品 mask

	IMG256_GROUP store_body;			// 27 商店 body
	IMG256_GROUP store_sell_mark;		// 28 sell mark
	IMG256_GROUP store_buy_mark; 		// 29 buy mark
	IMG256_GROUP store_sellmake;		// 30 寄卖
		
	IMG256_GROUP baobao_icon;			// 31 BAOBAO 图标

	IMG256_GROUP baobao_item_mark;		// 32 BAOBAO 使用物品 mark
	IMG256_GROUP bank_body;				// 33 BANK BODY
	IMG256_GROUP popshop_push_mark;		// 34 当铺储存 mark
	IMG256_GROUP popshop_pop_mark;		// 35 当铺取出 mark
		

// ----------------------------- 所有的按钮 --------------------
	SLONG button_handle;				// handle
	BUTTON_STATUS button_chat_select;	// 人名选中框
	BUTTON_STATUS button_window_close;	// window 关闭
	BUTTON_STATUS button_add;			// 加号
	BUTTON_STATUS button_sub;			// 减号
	BUTTON_STATUS button_up;			// 上
	BUTTON_STATUS button_down;			// 下
	BUTTON_STATUS button_check;			// 打勾
	
	BUTTON_STATUS button_chat_friend;	// 好友
	BUTTON_STATUS button_chat_temp;		// 临时好友
	BUTTON_STATUS button_chat_mask;		// 屏蔽好友

	BUTTON_STATUS button_chat_auto_answer;	// 自动回复
	BUTTON_STATUS button_chat_search;		// 搜寻好友
	BUTTON_STATUS button_chat_add_friend;	// 加为好友
// ---
	BUTTON_STATUS button_ok;				// 确定
	BUTTON_STATUS button_cancel;			// 取消

	BUTTON_STATUS button_chat_attr_friend;	// 加为好友		聊天器界面(收信、搜索、属性查看)
	BUTTON_STATUS button_chat_attr_temp;	// 临时好友		聊天器界面(收信、搜索、属性查看)
	BUTTON_STATUS button_chat_person;		// 私聊			聊天器界面(收信、搜索、属性查看)
	BUTTON_STATUS button_chat_cancel;		// 断交			聊天器界面(属性查看)
	BUTTON_STATUS button_chat_attr_mask;	// 屏蔽			聊天器界面(属性查看)
	BUTTON_STATUS button_chat_refresh;		// 更新			聊天器界面(属性查看)
	BUTTON_STATUS button_chat_hirstory;		// 历史信件		聊天器界面(属性查看)

// -------- 组队
	IMG256_GROUP team_list_mark;			// 队伍标题
	IMG256_GROUP team_list_request_mark;	// 请求列表	


// -------- Macro 
	IMG256_GROUP mc_window_body;			// 对话窗地图 23






// -------- Item image 
	UCHR *item_image[MAX_ITEM_IMAGE];		// Item 的 image

} SYSTEM_IMAGE;


#define MAX_FIGHT_IMAGE_NO					30

typedef struct FIGHT_IMAGE_STRUCT
{
	IMG256_GROUP menu_body;					// 0	主界面底板
	IMG256_GROUP hp_body;					// 1	血槽
	IMG256_GROUP mp_body;					// 2	法槽
	IMG256_GROUP speed_body;				// 3	充电槽
	IMG256_GROUP action_body0;				// 4	行动指令框动画
	IMG256_GROUP action_body1;				// 5	行动指令框动画
	IMG256_GROUP action_body2;				// 6	行动指令框动画
	IMG256_GROUP attack_select;				// 7	攻击(选中)
	IMG256_GROUP attack_push;				// 8	攻击(点击)
	IMG256_GROUP magic_select;				// 9	法术(选中)
	IMG256_GROUP magic_push;				// 10   法术(点击)
	IMG256_GROUP item_select;				// 11   道具(选中)
	IMG256_GROUP item_push;					// 12   道具(点击)
	IMG256_GROUP protect_select;			// 13   保护(选中)
	IMG256_GROUP protect_push;				// 14   保护(点击)
	IMG256_GROUP recover_select;			// 15   防御(选中)
	IMG256_GROUP recover_push;				// 16   防御(点击)
	IMG256_GROUP run_select;				// 17   逃跑(选中)
	IMG256_GROUP run_push;					// 18   逃跑(点击)
	IMG256_GROUP change_select;				// 19   更换(选中)
	IMG256_GROUP change_push;				// 20   更换(点击)
	IMG256_GROUP auto_hp_select;			// 21   自动回血(选中)
	IMG256_GROUP auto_hp_push;				// 22   自动回血(点击)
	IMG256_GROUP auto_mp_select;			// 23   自动回法(选中)
	IMG256_GROUP auto_mp_push;				// 24   自动回法(点击)
	IMG256_GROUP ai_select;					// 25   AI选单(选中)
	IMG256_GROUP ai_push;					// 26   AI选单(点击)
	IMG256_GROUP auto_attack_select;		// 27	自动攻击(选中)
	IMG256_GROUP auto_attack_push;			// 28	自动攻击(点击)	
	IMG256_GROUP map_menu_mask;				// 29	地图选单 mask

// ------ Other handle
	SLONG fight_item_handle;				// fight item handle
	SLONG number0_handle;					// 绿色 小字
	SLONG number1_handle;					// 红色 小字
	SLONG number2_handle;					// 绿色 大字
	SLONG double_handle;					// 连击 MDA
	SLONG fullmiss_handle;					// status MDA
	SLONG fight_hit_handle;					// hit handle

	SLONG status_power_handle;				// 威力 MDA
	SLONG status_chaos_handle;				// 混乱 MDA
	SLONG status_sleep_handle;				// 昏睡 MDA
	SLONG status_plague_handle;				// 瘟疫 MDA
	SLONG status_dark_handle;				// 黑暗 MDA
	SLONG status_delay_handle;				// 迟缓 MDA
	SLONG status_stone_handle;				// 石化 MDA
	SLONG status_fastness_handle;			// 提抗 MDA
	SLONG status_speed_handle;				// 加速 MDA

	ULONG status_timer;						
	bool change_frame;


} FIGHT_IMAGE;





typedef struct MOUSE_DATA_TABLE_STRUCT
{
	SLONG id;
	SLONG x_offset;
	SLONG y_offset;

} MOUSE_DATA_TABLE;


#define SMALL_MAP_WIDTH				400
#define SMALL_MAP_HEIGHT			300

typedef struct SMALL_MAP_DATA_STRUCT
{
	BMP* image_buffer;
	float x_percent;
	float y_percent;

} SMALL_MAP_DATA;


/***************************************************************************************}
{																						}
{	SYSTEM.CPP  Protype Define															}
{																						}
****************************************************************************************/

SLONG initialize_system(void);
void initialize_system_config(void);
void create_system_config(void);
void update_system_config(void);
SLONG initialize_color_control(void);
SLONG initialize_game_control(void);
SLONG initialize_music_voice(void);
SLONG read_system_image(void);
SLONG initialize_map_base_data(void);
SLONG initialize_chat_data(void);
SLONG initialize_extra_mda(void);
SLONG initializa_display_text(void);
SLONG initialize_chat_data_buffer(void);
SLONG initialize_chat_window(void);
void update_chat_window(void);
void clear_chat_friend_list(void);
void reload_chat_friend_list(void);
void update_client_config(void);
void clear_chat_receive_data(void);
SLONG check_chat_group_send_max(void);
void copy_chat_data_to_hirstory(void);
void clear_friend_data_record(void);
SLONG initialize_start_up_data(void);
void clear_team_list_data(void);
void clear_team_request_list_data(void);
void insert_team_request_list_data(BASE_CHARACTER_INFO *pInfo);
void insert_team_list_data(ULONG id, BASE_CHARACTER_INFO *pInfo);
void delete_team_request_list_data(ULONG id);
void delete_team_list_data(ULONG id);
void create_new_frame(SLONG no);
SLONG initialize_frame_data(void);



void free_map_base_data(void);
SLONG initialize_map_control(void);
void system_idle_loop(void);
void init_random(void);
SLONG initialize_system_control(void);
void clear_message_stack(void);

void clear_mission_data(void);
void insert_mission_data(MISSION_DATA data);

void clear_all_main_loop_data(void);




void dummy_draw(void);
void dummy_exec(void);

#endif	// _SYSTEM_H_