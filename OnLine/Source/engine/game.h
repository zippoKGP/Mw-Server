/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : game.h
  *owner  		: Stephen
  *description  : 
  *modified     : 2004/12/14
******************************************************************************/ 

#ifndef _GAME_H_
#define _GAME_H_


#include "map_info.h"
#include "player_info.h"
#include "utility.h"


#define MAP_MOVE_STAY_CURRENT			0
#define MAP_MOVE_TO_POSITION			1
#define MAP_MOVE_FOLLOW_NPC				2


#define MAX_SERVER_PAGE					20			// 一页最多显示




#define MAIN_TASK_MOVE					0xFFFF		// 移动
#define MAIN_TASK_MENU					0xFFFE		// 进入地图主选单
#define MAIN_TASK_NPC					0xFFFD		// 玩家 or NPC
#define MAIN_TASK_SMALL_MAP				0xFFFC		// 小地图
#define MAIN_TASK_CHAR_STATUS			0xFFFB		// 玩家头像
#define MAIN_TASK_BAOBAO_STATUS			0xFFFA		// 宝宝头像
#define MAIN_TASK_TEAM1					0xFFF9		// 队伍第一人
#define MAIN_TASK_TEAM2					0xFFF8		// 队伍第二人
#define MAIN_TASK_TEAM3					0xFFF7		// 队伍第三人
#define MAIN_TASK_TEAM4					0xFFF6		// 队伍第四人
#define MAIN_TASK_TEAM5					0xFFF5		// 队伍第五人
#define MAIN_TASK_FIGHT_MENU			0xFFF4		// 进入战斗选单

#define MAIN_TASK_END					MAIN_TASK_FIGHT_MENU-1	// 限定 TASK 最下面


// -------- map main menu button 
enum	MAP_MAIN_MENU_TASK_CODE
{
		MAP_MAIN_MENU_TASK_ITEM=					0,			// 物品
		MAP_MAIN_MENU_TASK_TEAM=					1,			// 组队
		MAP_MAIN_MENU_TASK_ATTACK=					2,			// 攻击
		MAP_MAIN_MENU_TASK_GIVE=					3,			// 给予
		MAP_MAIN_MENU_TASK_DEAL=					4,			// 交易
		MAP_MAIN_MENU_TASK_MISSION=					5,			// 任务
		MAP_MAIN_MENU_TASK_GROUP=					6,			// 公会
		MAP_MAIN_MENU_TASK_SYSTEM=					7,			// 系统
		MAP_MAIN_MENU_TASK_CHAT=					8,			// 聊天
// ---
		MAP_MAIN_MENU_TASK_CHATWIN=					9,			// 窗口切换
		MAP_MAIN_MENI_TASK_HISTORY=					10,			// 历史纪录
		MAP_MAIN_MENU_TASK_DISCARE=					11,			// 屏蔽选单
		MAP_MAIN_MENU_TASK_PHIZ=					12,			// 表情符号
		MAP_MAIN_MENU_TASK_SCROLL=					13,			// 滚屏开关
		MAP_MAIN_MENU_TASK_VIEW=					14,			// 窗口加大
		MAP_MAIN_MENU_TASK_UP=						15,			// 上翻
		MAP_MAIN_MENU_TASK_DOWN=					16,			// 下翻
// ---------		
		MAP_MAIN_MENU_TASK_SWITCH=					17,			// 窗口切换
		MAP_MAIN_MENU_TASK_WORLD=					18,			// 世界地图
		MAP_MAIN_MENU_TASK_SMALL=					19,			// 缩略地图
// --------
		MAP_MAIN_MENU_TASK_CHATCHANNEL=				20,			// 频道开关
// --------
		MAP_MAIN_MENU_TASK_CHARACTER_STATUS=		21,			// 人物状态
		MAP_MAIN_MENU_TASK_BAOBAO_STATUS=			22,			// 宝宝状态
		MAP_MAIN_MENU_TASK_CHARACTER_HP=			23,			// 人物自动+血
		MAP_MAIN_MENU_TASK_CHARACTER_MP=			24,			// 人物自动+法
		MAP_MAIN_MENU_TASK_BAOBAO_HP=				25,			// 宝宝自动+血
		MAP_MAIN_MENU_TASK_BAOBAO_MP=				26,			// 宝宝自动+法
		MAP_MAIN_MENI_TASK_BAOBAO_EXP=				27,			// display 宝宝经验

// -------- 选择NPC 的 task
		MAP_MAIN_MENU_TASK_ADD_FRIEND=				28,			// 加好友

// -------- 队伍状态
		MAP_MAIN_MENU_TASK_TEAM_STATUS=				29,			// 队伍状态
		

// -------- 战斗
		FIGHT_MENU_CHARACTER_STATUS=				100,		// 头像		0
		FIGHT_MENU_CHARACTER_ATTACK=				101,		// 攻击		1
		FIGHT_MENU_CHARACTER_MAGIC=					102,		// 法术		2
		FIGHT_MENU_CHARACTER_ITEM=					103,		// 道具		3
		FIGHT_MENU_CHARACTER_PROTECT=				104,		// 保护		4
		FIGHT_MENU_CHARACTER_RECOVER=				105,		// 防御		5
		FIGHT_MENU_CHARACTER_RUN=					106,		// 逃跑		6
		FIGHT_MENU_CHARACTER_AUTO_HP=				107,		// 红瓶		7
		FIGHT_MENU_CHARACTER_AUTO_MP=				108,		// 蓝瓶		8
		FIGHT_MENU_CHARACTER_ACTION=				109,		// 可点区域	9

		FIGHT_MENU_BAOBAO_STATUS=					110,		// 头像		10
		FIGHT_MENU_BAOBAO_ATTACK=					111,		// 攻击		11
		FIGHT_MENU_BAOBAO_MAGIC=					112,		// 法术		12
		FIGHT_MENU_BAOBAO_ITEM=						113,		// 道具		13
		FIGHT_MENU_BAOBAO_PROTECT=					114,		// 保护		14
		FIGHT_MENU_BAOBAO_RECOVER=					115,		// 防御		15
		FIGHT_MENU_BAOBAO_CHANGE=					116,		// 更换		16
		FIGHT_MENU_BAOBAO_AUTO_HP=					117,		// 红瓶		17
		FIGHT_MENU_BAOBAO_AUTO_MP=					118,		// 蓝瓶		18
		FIGHT_MENU_BAOBAO_ACTION=					119,		// 可点区域	19
		
		FIGHT_MENU_ATTACK_WINDOW=					120,		// 上拉选单 20
		FIGHT_MENU_AUTO_ATTACK=						121			// 自动		21
		
};



#define BUTTON_NONE						0
#define BUTTON_SELECT					1
#define BUTTON_PUSH						2


#define PHIZ_FONT_X			48
#define PHIZ_FONT_Y			48

#define PHIZ_WINDOW_X		400-(PHIZ_FONT_X*9)/2
#define PHIZ_WINDOW_Y		300-(PHIZ_FONT_Y*9)/2


#define CHARACTER_WINDOW_X		15
#define CHARACTER_WINDOW_Y		10
#define CHARACTER_WINDOW_XL		54
#define CHARACTER_WINDOW_YL		54

#define CHARACTER_HP_WINDOW_X	74
#define CHARACTER_HP_WINDOW_Y	20
#define CHARACTER_HP_WINDOW_XL	88
#define CHARACTER_HP_WINDOW_YL	11

#define CHARACTER_MP_WINDOW_X	74
#define CHARACTER_MP_WINDOW_Y	44
#define CHARACTER_MP_WINDOW_XL	88
#define CHARACTER_MP_WINDOW_YL	11

#define BAOBAO_WINDOW_X			62
#define BAOBAO_WINDOW_Y			63
#define BAOBAO_WINDOW_XL		40
#define BAOBAO_WINDOW_YL		40

#define BAOBAO_HP_WINDOW_X		104
#define BAOBAO_HP_WINDOW_Y		70
#define BAOBAO_HP_WINDOW_XL		58
#define BAOBAO_HP_WINDOW_YL		8

#define BAOBAO_MP_WINDOW_X		104
#define BAOBAO_MP_WINDOW_Y		88
#define BAOBAO_MP_WINDOW_XL		58
#define BAOBAO_MP_WINDOW_YL		8

#define BAOBAO_EXP_WINDOW_X		105
#define BAOBAO_EXP_WINDOW_Y		80
#define BAOBAO_EXP_WINDOW_XL	79
#define BAOBAO_EXP_WINDOW_YL	6




#define MAIN_TASK_RETURN_QUIT			0			// 结束主回路
#define MAIN_TASK_RETURN_READY_QUIT		1			// 进入询问结束 window
#define MAIN_TASK_RETURN_END			2			// 结束程式回到 WINDOW



#define FASTNESS_WINDOW_WIDTH			16*12
#define FASTNESS_WINDOW_HEIGHT			16*26


#define MAX_FIGHT_BUTTON			22



typedef struct MAP_MOVE_STRUCT
{
	SLONG type;
	SLONG dest_x;
	SLONG dest_y;
	SLONG move_speed;
} MAP_MOVE;


#define MAX_SCREEN_TEXT							50

typedef struct SCREEN_TEXT_DATA_STRUCT
{
	bool active;								// 目前有没有用
	RECT rect;									// display 坐标与大小
	ULONG timer;								// 自动关闭 timer
	UCHR text[MAX_CHAT_LENGTH];					// 资料
} SCREEN_TEXT_DATA;



typedef struct BASE_CHARACTER_DATA_STRUCT
{
	bool active;
// ---------- from player_info 取得
	char name[MAX_NAME_LENGTH+1];					// 昵称
	char title[MAX_TITLE_LENGTH+1];					// 称谓
	int base_type;									// 基本形态 0 到 8 

// ---------- 从主机取得
	CLIENT_CHARACTER_MAP_WINDOW data;

} BASE_CHARACTER_DATA;



typedef struct BASE_BAOBAO_DATA_STRUCT
{
	bool active;									// 资料 ready flag
	SERVER_CHARACTER_BAOBAO_STACK data;				// 宝宝 stack 资料
	SLONG display_handle;							// 显示的 handle
} BASE_BAOBAO_DATA;


/*******************************************************************************************************************}
{																													}
{  Game Use struct define																							}
{																													}
********************************************************************************************************************/
typedef struct CHANGE_MAP_INFO_STRUCT
{
// ---- MAP part
	UCHR map_name[MAX_MAP_NAME_LENGTH+1];					// 地图名称
	UCHR map_filename[MAX_FILENAME_LENGTH+1];				// 登入地图名称
	UCHR music_filename[MAX_FILENAME_LENGTH+1];				// 音乐档案名称
	SLONG small_map_file_no;								// 小地图名称
// ---
	UCHR fight_music_filename[MAX_FILENAME_LENGTH+1];		// 小战斗音乐
	SLONG fight_map_no;										// 一般战斗,战斗背景编号
// ---
	UCHR boss_fight_music_filename[MAX_FILENAME_LENGTH+1];	// Boss 战斗音乐
	SLONG boss_fight_map_no;								// Boss 战斗背景编号
// ---
	MAP_WEATHER_BASE map_weather_base;						// 目前天气状况
// ---
	MAP_ATTRIB_INFO map_attrib_info;						// 地图控制属性
// ---- NPC part
	BASE_NPC_INFO main_character;							// 主角资料 
	
} CHANGE_MAP_INFO;



typedef struct PHIZ_DATA_STAUCT
{
	POSITION pp;
	UCHR text[4];
} PHIZ_DATA;



/***************************************************************************************}
{	Protype define																		}
****************************************************************************************/
SLONG show_title(void);
SLONG select_main_function(void);
SLONG system_broadcast(void);
SLONG server_select(void);
SLONG account_input(void);
SLONG create_character(void);
SLONG ccl_test(void);
SLONG change_scene(void);

void draw_message_window(void);
void exec_message_window(void);
SLONG select_character(void);
SLONG select_character_in(void);
void convert_change_map_info(MAP_BASE_INFO map_base_info,MAP_ATTRIB_INFO map_attrib_info,
							 MAP_WEATHER_BASE map_weather_base,BASE_NPC_INFO player);


SLONG game_login(void);
SLONG game_main_loop(void);
void exec_main_map(void);
void draw_main_map(BMP *buffer);
void npc_talk(ULONG npc_id,UCHR *text);


void main_task_character_move(SLONG mouse_x,SLONG mouse_y);
void main_task_fast_key(void);
void display_map_main_menu_status(SLONG status);
void display_fight_menu_status(SLONG status);
void SendPlayerMove(int idx, int x, int y, int speed);
void reset_map_main_menu_button_frame(void);
void reset_map_small_map_button_frame(void);
void display_map_small_map_status(SLONG status);
void exec_map_main_menu(void);
void exec_fight_main_menu(void);
void exec_main_menu_task_channel(void);
void close_all_main_map_window(void);
void exec_main_menu_task_system(void);
void exec_system_config(void);
void exec_main_menu_task_quit(void);
void exec_map_small_map(void);
void update_weather_data(MAP_WEATHER_BASE data);
void exec_map_npc_active(void);
void exec_fight_npc_active(void);
void exec_main_menu_task_chat(void);
void exec_main_menu_task_phiz(void);
void display_map_character_status(SLONG status);
void display_map_team_status(SLONG task_id,SLONG status);
void exec_main_menu_task_character_status(void);
void exec_map_character_status(void);
void process_window_move(void);
void exec_main_menu_task_baobao_status(void);
void exec_main_menu_task_history(void);
void exec_main_menu_task_item(void);
void exec_main_menu_task_mission(void);
void exec_main_menu_task_group(void);
void display_map_npc_data(SLONG idx);
void exec_main_loop_select_npc(void);
void exec_send_message(void);
void check_message_in(void);
void exec_chat_attrib(void);
void exec_chat_group_send(void);
void chat_group_send_message(void);
void exec_main_menu_task_team(void);
void exec_main_menu_task_team_action(ULONG id);
void exec_main_menu_task_small_map(void);
void exec_main_menu_task_world_map(void);
SLONG check_system_npc_in_range(SLONG check_offset);
void exec_main_menu_task_give(void);
void exec_main_menu_task_deal(void);
void exec_main_menu_task_skill(void);
void exec_main_menu_task_title(void);
void ask_yes_no(UCHR *text, void (*yes_exec)(void),void (*no_exec)(void));
void exec_main_menu_task_baobao_item(void);
void exec_main_menu_task_character_fastness(void);
void exec_main_menu_task_baobao_fastness(void);
void exec_fight_main_menu(void);
void exec_fight_menu_task_skill(void);
void exec_fight_menu_task_baobao_skill(void);
void exec_fight_menu_task_character_item(void);
void exec_fight_menu_task_baobao_item(void);
void exec_fight_menu_task_baobao_change(void);
void exec_fight_talk_window(void);



// --- window routinue
void window_draw_select_channel(void);
void window_exec_select_channel(void);
void window_draw_system(void);
void window_exec_system(void);
void window_draw_quit(void);
void window_exec_quit(void);
void window_draw_phiz(void);
void window_exec_phiz(void);
void window_draw_chat(void);
void window_exec_chat(void);
void window_draw_character_status(void);
void window_exec_character_status(void);
void window_draw_baobao_status(void);
void window_exec_baobao_status(void);
void window_draw_history(void);
void window_exec_history(void);
void window_draw_item(void);
void window_exec_item(void);
void window_draw_mission(void);
void window_exec_mission(void);
void window_draw_group(void);
void window_exec_group(void);
void window_draw_send_message(void);
void window_exec_send_message(void);
void window_draw_chat_receive(void);
void window_exec_chat_receive(void);
void window_draw_chat_attrib(void);
void window_exec_chat_attrib(void);
void window_draw_chat_group_send(void);
void window_exec_chat_group_send(void);
void window_draw_chat_hirstory(void);
void window_exec_chat_hirstory(void);
void window_draw_team(void);
void window_exec_team(void);
void window_draw_chat_search(void);
void window_exec_chat_search(void);
void window_draw_team_request(void);
void window_exec_team_request(void);
void window_draw_small_map(void);
void window_exec_small_map(void);
void window_draw_world_map(void);
void window_exec_world_map(void);
void window_draw_give(void);
void window_exec_give(void);
void window_draw_deal(void);
void window_exec_deal(void);
void window_draw_skill(void);
void window_exec_skill(void);
void window_draw_title(void);
void window_exec_title(void);
void window_draw_baobao_item(void);
void window_exec_baobao_item(void);
void window_draw_character_fastness(void);
void window_exec_character_fastness(void);
void window_draw_baobao_fastness(void);
void window_exec_baobao_fastness(void);
void window_draw_fight_character_skill(void);
void window_exec_fight_character_skill(void);
void window_draw_fight_baobao_skill(void);
void window_exec_fight_baobao_skill(void);
void window_draw_fight_character_item(void);
void window_exec_fight_character_item(void);
void window_draw_fight_baobao_item(void);
void window_exec_fight_baobao_item(void);
void window_draw_fight_baobao_change(void);
void window_exec_fight_baobao_change(void);
void window_draw_fight_talk(void);
void window_exec_fight_talk(void);



// --- test use
void display_map_info(void);
void display_user_info(void);
void auto_change_weather(void);



#endif
