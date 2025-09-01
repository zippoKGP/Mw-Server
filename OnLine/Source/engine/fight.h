/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : fight.h
  *owner  		: Stephen
  *description  : 
  *modified     : 2005/3/2
******************************************************************************/ 

#ifndef _FIGHT_H_
#define _FIGHT_H_

#include "data.h"
#include "utility.h"

#define MAX_FIGHT_DIR				4

#define FIGHT_NPC_DIR_RIGHT			0
#define FIGHT_NPC_DIR_LEFT			1
#define FIGHT_NPC_DIR_UP			2
#define FIGHT_NPC_DIR_DOWN			3


#define FIGHT_MOTION_STANDBY		0
#define FIGHT_MOTION_RUN			1
#define FIGHT_MOTION_ATTACK1		2
#define FIGHT_MOTION_ATTACK2		3
#define FIGHT_MOTION_ATTACK3		4
#define FIGHT_MOTION_MAGIC			5
#define FIGHT_MOTION_SPECIAL		6
#define FIGHT_MOTION_HURT			7
#define FIGHT_MOTION_DEAD			8

#define MAX_FIGHT_SCREEN_TEXT		30

// -------------- FIGHT AREA
#define FIGHT_MAP_WIDTH				1600
#define FIGHT_MAP_HEIGHT			900		
#define MAX_CAMERA_ZOOM				200

#define CAMERA_ZOOM_STILL			0				// 縮放不变
#define CAMERA_ZOOM_IN				1				// 鏡頭拉近(放大)
#define CAMERA_ZOOM_OUT				2				// 鏡頭拉遠(縮小)
#define CAMERA_ZOOM_LOCK			3				// 鏡頭鎖定(固定直)(慢慢趨近)

#define CAMERA_PAN_STILL			0				// 鏡頭移動(不變)
#define CAMERA_PAN_UP				1				// 鏡頭移動(向上)
#define CAMERA_PAN_DOWN				2				// 鏡頭移動(向下)
#define CAMERA_PAN_LEFT				3				// 鏡頭移動(向左)
#define CAMERA_PAN_RIGHT			4				// 鏡頭移動(向右)
#define CAMERA_PAN_LOCK_CENTER		5				// 定住中心點(地圖中心點)
#define CAMERA_PAN_LOCK_LEFT		6				// 定住最左邊(地圖左邊)
#define CAMERA_PAN_LOCK_RIGHT		7				// 定住最右邊(地圖右邊)
#define CAMERA_PAN_LOCK_POINT		8				// 定住某一點(慢慢移動)
#define CAMERA_PAN_LOCK				9				// 定住某一點(直接定位)


#define FIRST_FIGHT_DELAY_TIME		20000


#define AUTO_CAMERA_SET				0
#define	AUTO_CAMERA_RUN				1


typedef struct CAMERA_ZOOM_STRUCT
{
	SLONG width;
	SLONG height;
	SLONG size_zoom;
	SLONG x_pos_zoom;
	SLONG y_pos_zoom;
} CAMERA_ZOOM;



typedef struct FIGHT_NPC_GROUP_STRUCT
{
	FIGHT_BASE_DATA base;
	FIGHT_VALUE_RESULT result;			// 一回合战斗结果
	
	bool display;						// 是否要 display 
	SLONG x,y;							// 坐标
	SLONG handle;						// MDA GROUP handle
	ULONG npc_timer;					// 播放时间控制
	SLONG npc_type;						// 0 -> 主角, 1 -> 变身
	SLONG motion;						// 目前动作
	SLONG dir;
	FIGHT_NPC_MDA_CONTROL dir_frame[MAX_FIGHT_DIR] ;			// 战斗待机
	FIGHT_NPC_MDA_CONTROL attack1_frame[MAX_FIGHT_DIR];		// 物理攻击1
	FIGHT_NPC_MDA_CONTROL attack2_frame[MAX_FIGHT_DIR];		// 物理攻击2
	FIGHT_NPC_MDA_CONTROL attack3_frame[MAX_FIGHT_DIR];		// 物理攻击3
	FIGHT_NPC_MDA_CONTROL magic_frame[MAX_FIGHT_DIR];			// 法术释放
	FIGHT_NPC_MDA_CONTROL special_frame[MAX_FIGHT_DIR];		// 特殊技能
	FIGHT_NPC_MDA_CONTROL dead_frame[MAX_FIGHT_DIR];			// 死亡动作
	
// ---------- Auto Control 
	SLONG dead_status;					// 死亡状态控制
	
} FIGHT_NPC_GROUP;



typedef struct CAMERA_DATA_STRUCT
{
	SLONG pan_x;
	SLONG pan_y;
	SLONG zoom_index;
	SLONG center_x;
	SLONG center_y;
} CAMERA_DATA;



typedef struct FIGHT_CONTROL_DATA_STRUCT
{
// --- 基本控制
	SLONG character_idx;				// 控制玩家所对应的 fight_npc_group idx
	SLONG baobao_idx;					// 控制玩家的宝宝所对应的 fight_npc_group idx
	bool charge_flag;					// 是否充电
	bool character_command_ready;		// 玩家命令 ready
	bool baobao_command_ready;			// 宝宝命令 ready
	bool character_command_send;
	bool baobao_command_send;
	SLONG character_speed_percent;		// 玩家充电数值 ( 0 到 100 )
	SLONG baobao_speed_percent;			// 玩家充电数值 ( 0 到 100 )
	SLONG character_ai_type;			// 玩家自动攻击的 AI
	SLONG baobao_ai_type;				// 宝宝自动攻击的 AI
	ULONG character_pause_time;			// 玩家暂停时间
	ULONG baobao_pause_time;			// 寶寶暂停时间
	
// --- Fight Action 参数
	bool fight_action_ready;			// 战斗动作 ready
	char fight_action_command;			// 战斗动作种类
	ULONG action_source_id;				// 动作接受人 id
	ULONG action_target_id;				// 动作人 id ( 0 -> 代表全体 )
	char action_idx;					// 法术 idx

// --- Camera 参数
	bool camera_follow_flag;			// 镜头自动跟随 flag
	SLONG camera_follow_x;				// 镜头自动跟随中心点坐标
	SLONG camera_follow_y;
	SLONG camera_follow_zoom;			// 镜头自动跟随 zoom idx


// --- 其他控制参数
	SLONG effect0_extra_mda_idx;		// 地裂 MDA 效果
	SLONG see_fight_extra_mda_idx;		// 观战 MDA 效果
	bool danger_flag;					// team danger flag
	SLONG danger_status;
	bool long_war_flag;					// 持久战
	SLONG long_war_status;
	SLONG fight_loop;					// 玩家战斗回合
	bool fight_end;						// 战斗结束
	bool player_select;					// 是玩家还是宝宝命令
	
} FIGHT_CONTROL_DATA;



typedef struct FIGHT_INST_DATA_STRUCT
{
	bool active;						// 是否有资料需要显示或者已经显示完毕

	SLONG status;						// 目前显示 status	0 -> Scroll in
										//					1 -> display data
										//					2 -> Scroll out
	SLONG y_pos;						// display y position offset
	ULONG still_timer;					// 记录显示时间
	ULONG display_time;					// 显示时间

	UCHR inst[256];						// inst data

} FIGHT_INST_DATA;



typedef struct FIGHT_SCREEN_TEXT_DATA_STRUCT
{
	bool active;						// 是否有资料
	bool scroll;
	SLONG type;							// 大字还是小字 0 -> 大字 1 -> 小字
	SLONG x;							// 坐标
	SLONG y;
	ULONG still_timer;					// 显示时间
	UCHR text[80];						// 显示内容 '0'到'9'  'M'-> Miss, 'F'-> Full
} FIGHT_SCREEN_TEXT_DATA;


/*******************************************************************************************************************}
{																													}
{	Routinue Define 																								}
{																													}
********************************************************************************************************************/
void change_to_fight(void);
void change_to_map(void);
SLONG read_fight_image(void);
SLONG free_fight_image(void);
void fight_camera(BMP *buffer);
void clear_fight_npc_group(void);
void insert_fight_unit(FIGHT_BASE_DATA data,SLONG idx);
void draw_fight_npc_layer(BMP *buffer);
void draw_fight_single_npc(SLONG idx,BMP *buffer);
void change_fight_npc_next_frame(SLONG idx,SLONG now_dir,SLONG motion);
void set_camera(SLONG zoom,SLONG x,SLONG y);
void camera_auto_control(SLONG zoom_command,SLONG pan_command,SLONG zoom_value,SLONG pan_value,SLONG pan_value1);
void get_default_position(void);
void display_fight_single_name(SLONG idx,BMP *buffer);
void show_fight_welcome(void);
SLONG add_extra_fight_mda(UCHR *filename,SLONG x,SLONG y,bool loop,bool auto_close,bool hight_light,SLONG level,UCHR *command);
void auto_close_extra_fight_mda(void);
void reset_extra_fight_mda_frame(SLONG idx);
void draw_button_extra_fight_mda(BMP *buffer);
void draw_top_extra_fight_mda(BMP *buffer);
void draw_single_extra_fight_mda(SLONG idx,BMP *buffer);
void auto_camera_pan(SLONG target_zoom,SLONG target_center_x,SLONG target_center_y,SLONG step);
void process_fight_camera_task(BMP *buffer);
void clear_all_fight_parameter(void);
void push_fight_inst(UCHR *text,ULONG timer);
void process_fight_inst(BMP *buffer);
void wait_first_time_command(void);
void display_timer_text(UCHR *text,BMP *buffer);
void exec_fight_action(BMP *buffer);
void auto_camera_follow(void);
void add_fight_screen_text(SLONG x,SLONG y,SLONG type,bool scroll,ULONG timer,UCHR *text);
void display_fight_screen_text(BMP *buffer);
void display_number_image0(SLONG x,SLONG y,UCHR *text,BMP *buffer);
void display_number_image1(SLONG display_x,SLONG display_y,UCHR *text,BMP *buffer);
void update_fight_result(SLONG idx);
void clear_fight_mda_status(void);
void get_pan(SLONG zoom,SLONG center_x,SLONG center_y,SLONG *pan_x,SLONG *pan_y);
SLONG draw_single_frame_mda(SLONG idx,BMP *buffer);
void exec_fight_auto_control(void);
void show_fight_result(void);
void exec_fight_auto_npc_talk(void);
void change_status_handle_next_frame(SLONG handle);


// ------播放战斗动画
void exec_fight_action_single_physica(void);
void exec_fight_action_single_physica_double(void);
void exec_fight_action_single_magic(void);
void exec_fight_action_single_magic_double(void);
void exec_fight_action_item(void);
void exec_fight_action_run(void);
void exec_fight_action_change(void);
void exec_fight_action_single_status(void);
// --- 多人
void exec_fight_action_multi_magic(void);
void exec_fight_action_multi_magic_double(void);
void exec_fight_action_multi_status(void);

void make_fight_help(void);




// ---------------
void test_fight_camera(void);




#endif





















