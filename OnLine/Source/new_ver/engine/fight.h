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

#define CAMERA_ZOOM_STILL			0				// �s�Ų���
#define CAMERA_ZOOM_IN				1				// �R�^����(�Ŵ�)
#define CAMERA_ZOOM_OUT				2				// �R�^���h(�sС)
#define CAMERA_ZOOM_LOCK			3				// �R�^�i��(�̶�ֱ)(����څ��)

#define CAMERA_PAN_STILL			0				// �R�^�Ƅ�(��׃)
#define CAMERA_PAN_UP				1				// �R�^�Ƅ�(����)
#define CAMERA_PAN_DOWN				2				// �R�^�Ƅ�(����)
#define CAMERA_PAN_LEFT				3				// �R�^�Ƅ�(����)
#define CAMERA_PAN_RIGHT			4				// �R�^�Ƅ�(����)
#define CAMERA_PAN_LOCK_CENTER		5				// ��ס�����c(�؈D�����c)
#define CAMERA_PAN_LOCK_LEFT		6				// ��ס����߅(�؈D��߅)
#define CAMERA_PAN_LOCK_RIGHT		7				// ��ס����߅(�؈D��߅)
#define CAMERA_PAN_LOCK_POINT		8				// ��סĳһ�c(�����Ƅ�)
#define CAMERA_PAN_LOCK				9				// ��סĳһ�c(ֱ�Ӷ�λ)


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
	FIGHT_VALUE_RESULT result;			// һ�غ�ս�����
	
	bool display;						// �Ƿ�Ҫ display 
	SLONG x,y;							// ����
	SLONG handle;						// MDA GROUP handle
	ULONG npc_timer;					// ����ʱ�����
	SLONG npc_type;						// 0 -> ����, 1 -> ����
	SLONG motion;						// Ŀǰ����
	SLONG dir;
	FIGHT_NPC_MDA_CONTROL dir_frame[MAX_FIGHT_DIR] ;			// ս������
	FIGHT_NPC_MDA_CONTROL attack1_frame[MAX_FIGHT_DIR];		// ������1
	FIGHT_NPC_MDA_CONTROL attack2_frame[MAX_FIGHT_DIR];		// ������2
	FIGHT_NPC_MDA_CONTROL attack3_frame[MAX_FIGHT_DIR];		// ������3
	FIGHT_NPC_MDA_CONTROL magic_frame[MAX_FIGHT_DIR];			// �����ͷ�
	FIGHT_NPC_MDA_CONTROL special_frame[MAX_FIGHT_DIR];		// ���⼼��
	FIGHT_NPC_MDA_CONTROL dead_frame[MAX_FIGHT_DIR];			// ��������
	
// ---------- Auto Control 
	SLONG dead_status;					// ����״̬����
	
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
// --- ��������
	SLONG character_idx;				// �����������Ӧ�� fight_npc_group idx
	SLONG baobao_idx;					// ������ҵı�������Ӧ�� fight_npc_group idx
	bool charge_flag;					// �Ƿ���
	bool character_command_ready;		// ������� ready
	bool baobao_command_ready;			// �������� ready
	bool character_command_send;
	bool baobao_command_send;
	SLONG character_speed_percent;		// ��ҳ����ֵ ( 0 �� 100 )
	SLONG baobao_speed_percent;			// ��ҳ����ֵ ( 0 �� 100 )
	SLONG character_ai_type;			// ����Զ������� AI
	SLONG baobao_ai_type;				// �����Զ������� AI
	ULONG character_pause_time;			// �����ͣʱ��
	ULONG baobao_pause_time;			// ������ͣʱ��
	
// --- Fight Action ����
	bool fight_action_ready;			// ս������ ready
	char fight_action_command;			// ս����������

	ULONG action_source_id;				// ���������� id
	ULONG action_target_id;				// ������ id ( 0 -> ����ȫ�� )
	char action_idx;					// ���� idx
	

// --- Camera ����
	bool camera_follow_flag;			// ��ͷ�Զ����� flag
	SLONG camera_follow_x;				// ��ͷ�Զ��������ĵ�����
	SLONG camera_follow_y;
	SLONG camera_follow_zoom;			// ��ͷ�Զ����� zoom idx


// --- �������Ʋ���
	SLONG effect0_extra_mda_idx;		// ���� MDA Ч��
	bool danger_flag;					// team danger flag
	SLONG danger_status;
	bool long_war_flag;					// �־�ս
	SLONG long_war_status;
	SLONG fight_loop;					// ���ս���غ�
	bool fight_end;						// ս������
	
} FIGHT_CONTROL_DATA;



typedef struct FIGHT_INST_DATA_STRUCT
{
	bool active;						// �Ƿ���������Ҫ��ʾ�����Ѿ���ʾ���

	SLONG status;						// Ŀǰ��ʾ status	0 -> Scroll in
										//					1 -> display data
										//					2 -> Scroll out
	SLONG y_pos;						// display y position offset
	ULONG still_timer;					// ��¼��ʾʱ��
	ULONG display_time;					// ��ʾʱ��

	UCHR inst[256];						// inst data

} FIGHT_INST_DATA;



typedef struct FIGHT_SCREEN_TEXT_DATA_STRUCT
{
	bool active;						// �Ƿ�������
	SLONG type;							// ���ֻ���С�� 0 -> ���� 1 -> С��
	SLONG x;							// ����
	SLONG y;
	ULONG still_timer;					// ��ʾʱ��
	UCHR text[80];						// ��ʾ���� '0'��'9'  'M'-> Miss, 'F'-> Full
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
void add_fight_screen_text(SLONG x,SLONG y,SLONG type,ULONG timer,UCHR *text);
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



// ------����ս������
void exec_fight_action_single_physica(void);
void exec_fight_action_single_physica_double(void);
void exec_fight_action_single_magic(void);
void exec_fight_action_single_magic_double(void);
void exec_fight_action_item(void);
void exec_fight_action_run(void);
void exec_fight_action_change(void);
void exec_fight_action_single_status(void);
// --- ����
void exec_fight_action_multi_magic(void);
void exec_fight_action_multi_magic_double(void);
void exec_fight_action_multi_status(void);





// ---------------
void test_fight_camera(void);




#endif





















