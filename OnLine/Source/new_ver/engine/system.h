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

	MOUSE_IMG_STAND=					0,			// �����
	MOUSE_IMG_ERROR=					1,			// �������
	MOUSE_IMG_TEAM= 					2,			// ���
	MOUSE_IMG_ATTACK=					3,			// ����
	MOUSE_IMG_GIVE=						4,			// ����
	MOUSE_IMG_DEAL=						5,			// ����
	MOUSE_IMG_FRIEND=					6,			// �Ӻ���
	MOUSE_IMG_ITEM=						7,			// ʹ����Ʒ
	MOUSE_IMG_MACRO=					8			// �Ի�

};





#define MESSAGE_DELAY					1000*3			// 3 ��


#define MASK_ALPHA_LEVEL				255*75/100

#define MAX_CHAT_STACK					16

/***************************************************************************************}
{	SYSTEM struct define area															}
{	ϵͳ�����趨����																	}
****************************************************************************************/
typedef struct SYSTEM_CONFIG_STRUCT
{
	UCHR copyright[19];			// 'UNIGIUM SYSTEM' config file Head
	UCHR text_end_code;			// 0x1A 
// ----------- main system -----------
	SLONG window_mode;			// 0 -> �Ӵ�ģʽ , 1 -> ȫ��ģʽ
	SLONG music_flag;			// 0 -> ���ֹر� , 1 -> ���ֿ���
	SLONG voice_flag;			// 0 -> ��Ч�ر� , 1 -> ��Ч����
	SLONG volume;				// ������Ч���� ( 0 -> 100 )
	SLONG record_flag;			// ��Ϸ¼�񿪹� ( 0 -> ���� ,1 -> ��Ϸ¼���� )
	SLONG fight_flag;			// �д迪�� ( 0 -> �ر�, 1 -> �д�� )
	SLONG mail_flag;			// �ż����� ( 0 -> �ر�, 1 -> �ż����ܴ� )
	SLONG item_flag;			// ��Ʒ���� ( 0 -> �ر�, 1 -> ��Ʒ���ܴ� )
	SLONG friend_flag;			// ���ѿ��� ( 0 -> �ر�, 1 -> ������Ѵ� )
	SLONG team_flag;			// ��ӿ��� ( 0 -> �ر�, 1 -> ������Ӽ��� )
	SLONG head_talk;			// ͷ���Ի����� ( 0 -> �ر�, 1 -> ����ͷ���Ի���ʾ )
	SLONG strange_flag;			// İ���˿��� ( 0 -> �ر�, 1 -> �ܾ�İ������Ϣ )
	SLONG quick_menu;			// �Ҽ�ѡ������ ( 0 -> �ر�, 1 -> �����Ҽ�ѡ�� )
	SLONG clothes_flag;			// װ����ʾ���� ( 0 -> �ر�, 1 -> ��ʾװ����ֵ )
// -----------------------------------	
	SLONG default_server_id;	// Ĭ�Ϸ�����

// ---------------- chat channel data
	SLONG channel_screen_flag;	// ��ǰƵ������
	SLONG channel_team_flag;	// ����Ƶ������
	SLONG channel_person_flag;	// ˽��Ƶ������
	SLONG channel_group_flag;	// ����Ƶ������
	SLONG channel_sellbuy_flag;	// ����Ƶ������
	SLONG channel_world_flag;	// ����Ƶ������

} SYSTEM_CONFIG;


/***************************************************************************************}
{	GAME struct define area																}
{	��Ϸ�����ṹ																		}
****************************************************************************************/
typedef struct GAME_CONTROL_STRUCT
{
// --- config
	UCHR music_filename[256];		// ���ֵ�������
	UCHR server_name[256];			// ����������
	SLONG music_loop_flag;			// Loop flag
	SLONG music_playing;			// �Ƿ����ڲ�������
	SLONG npc_display_flag;
	SLONG map_zoom_index;			// MAP �Ŵ� idx
	SLONG deal_type;				// ������Ʒ�����߱���
// --- main loop
	SLONG main_loop_mode;			// Ŀǰս�����ǵ�ͼģʽ
	SLONG main_loop_task;			// Ŀǰ����·״̬ 
	bool main_loop_break;			// �Ƿ���������·
	SLONG main_loop_ret_val;		// main_loop �ش�ֵ
	SLONG main_loop_select_npc_idx;	// ����·ѡ��NPC idx
	SLONG main_loop_select_npc_task;// ����·ѡ��NPC ��task

// --- �淨����
	ULONG game_give_id;				// ��Ҹ����˵� ID
	ULONG game_attack_id;			// ��ҹ����� ID
	ULONG game_deal_id;				// ����ҽ��׵� ID
	bool character_skill_ready;		// ��� skill data ready 
	SLONG skill_start_index;		// skill data display start idx
	SLONG skill_select_index;		// skill data selected idx
	bool baobao_skill_ready;		// ���� skill_ready 
	SLONG baobao_skill_index;		// ���� skill data index
	
// --- screen text
	ULONG screen_text_timer;		// SCREEN_TEXT �õ� �������� timer


// --- ����ϵͳ
	bool weather_enable;			// Ŀǰ��ͼ�Ƿ�ִ������ϵͳ
	ULONG weather_timer;			// �������� timer
	ULONG weather_add_timer;		// �����ƶ� timer
	ULONG weather_change_timer;		// ����������� timer;

// --- ����ϵͳ
	SLONG chat_channel;				// Ŀǰ����Ƶ��
	ULONG chat_timer;				// �������ʱ��
	UCHR chat_stack_buffer[MAX_CHAT_STACK][MAX_CHAT_LENGTH];		// ���������ʷ����
	SLONG chat_stack_index;			// Ŀǰ stack index
	SLONG chat_stack_find_index;	// ĿǰѰ�ҵ� idx
	SLONG chat_window_type;			// Ŀǰ������� window  0 -> window , 1 -> �ڲ�
	SLONG chat_pause_flag;			// pause flag
	SLONG chat_message_in;			// ��ѶϢ����

// --- main_loop ������ window �������
	SLONG window_task;		
	SLONG old_window_task;			// 

// --- channel window
	SLONG window_channel;			// Ƶ���л� window �� handle
// --- system window
	SLONG window_system;			// ϵͳ window �� handle
	SLONG window_system_button_idx; // system window �� button idx	

// --- ask_quit 
	SLONG window_quit;				// Quit winqod
	SLONG window_quit_button_idx;

// --- ������
	SLONG window_chat;				// ������ window handle
	SLONG window_chat_type;			// ������Ŀǰ����
	SLONG window_chat_select_idx;	// ѡ��� idx
	
	SLONG window_chat_friend_start_idx;
	SLONG window_chat_friend_end_idx;
	SLONG window_chat_temp_start_idx;
	SLONG window_chat_temp_end_idx;
	SLONG window_chat_mask_start_idx;
	SLONG window_chat_mask_end_idx;
	
// --- ����
	SLONG team_message_in;			// ������������Ա

// --- �������
	SLONG window_phiz;				// �������

// --- ����״̬
	SLONG window_character_status;	// ����״̬

// --- ����״̬
	SLONG window_baobao_status;		// ����״̬

// --- ��ʷ����
	SLONG window_history;			// ��ʷ����

// --- ������Ʒ
	SLONG window_item;				// ������Ʒ

// --- ����
	SLONG window_mission;			// ����

// --- ����
	SLONG window_group;				// ����

// --- Send message					
	SLONG window_send_message;		// ����ѶϢ

// --- Rceive message
	SLONG window_receive_message;	// ��ѶϢ
	
// --- chat attrib	
	SLONG window_chat_attrib;		// ��������

// --- chat group send
	SLONG window_chat_group_send;	// ����Ⱥ��

// --- chat hirstory
	SLONG window_chat_hirstory;		// ������ʷ����

// --- team
	SLONG window_team;				// �����б�

// --- chat search
	SLONG window_chat_search;		// ���Ѳ�ѯ

// --- team request
	SLONG window_team_request;		// �����б�

// --- small map 
	SLONG window_small_map;			// С��ͼ��ʾ

// --- world map
	SLONG window_world_map;			// �����ͼ

// --- Macro
	SLONG window_macro;				// �籾 window

// --- Give 
	SLONG window_give;				// ����

// --- Deal
	SLONG window_deal;				// ����

// --- Skill
	SLONG window_skill;				// ���ܲ鿴

// --- title
	SLONG window_title;				// ͷ�θ���

// --- baobao item
	SLONG window_baobao_item;		// ������Ʒʹ��

// --- character fastness	
	SLONG window_character_fastness;	// ���￹��

// --- baobao fastness
	SLONG window_baobao_fastness;		// ��������

// --- fight character skill
	SLONG window_fight_character_skill;	// ս��������

// --- fight baobao skill
	SLONG window_fight_baobao_skill;	// ����ս������

// --- fight character item handle
	SLONG window_fight_character_item;	// ս��������Ʒ

// --- fight baobao item handle
	SLONG window_fight_baobao_item;		// ս��������Ʒ

// --- fight baobao change
	SLONG window_fight_baobao_change;	// ս����������

// --- fight talk window 
	SLONG window_fight_talk;			// ս����������
	
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
	ULONG system_timer;				// ϵͳ����ͬ��ʱ��
	ULONG control_user_id;			// ��ҵ� ID
	SLONG control_npc_idx;			// ��ҵ� map_npc_group(IDX)
	SLONG point_npc_idx;			// ���Ŀǰ��ָ���� NPC IDX 
	ULONG person_npc_id;			// ���˽�� ID
	UCHR person_name[MAX_NAME_LENGTH+1];	// ˽���������
	ULONG macro_npc_id;				// Ŀǰִ�� macro npc id
// --------- mouse data 
	ULONG mouse_timer;
	SLONG mouse_x;					// ��������
	SLONG mouse_y;
	SLONG zoom_mouse_x;				// Zoom ��������
	SLONG zoom_mouse_y;
	UCHR mouse_key;					// ���󰴼�
	UCHR key;						// keyboard ����
	UCHR data_key;					// ���� key
	bool alt_key;					// �Ƿ��� alt key
	
} SYSTEM_CONTROL;



typedef struct SYSTEM_IMAGE_STRUCT
{
// -------------------------- ��������------------------------
	SLONG character_head;				// ����ͷ��
	SLONG weather_handle;				// ����ͼ��
	SLONG status_handle;				// ����icon
	SLONG full_handle;
	ULONG icon_timer;			

// -------------------------- ѶϢ�� -------------------------
	IMG256_GROUP msg_TopLeft;
	IMG256_GROUP msg_Top;
	IMG256_GROUP msg_TopRight;
	IMG256_GROUP msg_Left;
	IMG256_GROUP msg_Middle;
	IMG256_GROUP msg_Right;
	IMG256_GROUP msg_BottomLeft;
	IMG256_GROUP msg_Bottom;
	IMG256_GROUP msg_BottomRight;
// -------------------------- ��ͼ������ -------------------
	IMG256_GROUP status_body;			// �����뱦������		0
	IMG256_GROUP map_body;				// С��ͼ����			1
	IMG256_GROUP map_main_body;			// ��ͼ����������		2	
	IMG256_GROUP channel_body;			// Ƶ��ѡ������			3
	IMG256_GROUP face_font;				// ͷ�����				4
	IMG256_GROUP char_hp;				// ���� HP body			5
	IMG256_GROUP char_mp;				// ���� MP body			6
	IMG256_GROUP baobao_hp;				// ���� HP				7
	IMG256_GROUP baobao_exp;			// ���� EXP				8
	IMG256_GROUP baobao_mp;				// ���� HP				9
	IMG256_GROUP char_exp;				// ���� exp				10
	SLONG main_menu_handle;
	BUTTON_STATUS button_channel_active;		// ��
	BUTTON_STATUS button_map_main_menu[17];		// ��menu ��ť
	BUTTON_STATUS button_small_map[3];			// ��С��ͼ
	BUTTON_STATUS button_chat_channel[6];		// Ƶ��
// --------------------------- ϵͳ������ -------------------------------	
	SLONG system_menu_handle;
	BUTTON_STATUS system_background;	// ��ͼ 0
	IMG256_GROUP volume;				// ������С 1
	BUTTON_STATUS system_close;			// �رհ�ť 2
	BUTTON_STATUS sub_volume;			// �������� 4
	BUTTON_STATUS add_volume;			// �����Ӵ� 6
	BUTTON_STATUS system_save;			// ���水ť 8
	BUTTON_STATUS system_help;			// ��Ϸ���� 11
	BUTTON_STATUS system_bbs;			// ��Ϸ��̳ 14
	BUTTON_STATUS system_quit;			// �˳���Ϸ 17
// ---------------------------- ������Ϸ���� -----------------------------------
	SLONG quit_menu_handle;
	BUTTON_STATUS quit_body;			// ��ͼ
	BUTTON_STATUS quit_restart;			// ���½�����Ϸ
	BUTTON_STATUS quit_quit;			// ������Ϸ
	BUTTON_STATUS quit_cancel;			// �ص���Ϸ
// --------------------------- ���ƶ� window �װ� ------------------------------
	IMG256_GROUP chat_body;				// ������
	IMG256_GROUP character_attrib_body;	// ��������
	IMG256_GROUP baobao_attrib_body;	// ��������
	IMG256_GROUP history_body;			// ��ʷ����
	IMG256_GROUP item_body;				// ��Ʒװ��
	IMG256_GROUP team_body;				// ���
	IMG256_GROUP give_body;				// ����
	IMG256_GROUP deal_body;				// ����
	IMG256_GROUP mission_body;			// ����
	IMG256_GROUP group_body;			// ����
	IMG256_GROUP chat_receive_body;		// ������ ����
	IMG256_GROUP chat_send_body;		// ������ ����
	IMG256_GROUP chat_setup;			// ������ setup
	IMG256_GROUP chat_search;			// ������ ��Ѱ
//--
	IMG256_GROUP chat_attrib_body;		// ���������Բ쿴
	IMG256_GROUP chat_group_send_body;	// ������Ⱥ��
	IMG256_GROUP chat_hirstory_mask;	// �������mask

	IMG256_GROUP item_public_body;		// 24 ��Ʒ�����ò���
	IMG256_GROUP item_action_body;		// 25 ���붯������
	IMG256_GROUP deal_action_body;		// 26 deal

	IMG256_GROUP skill_body;			// 21 ���ܲ쿴
	IMG256_GROUP title_body;			// 22 ͷ��

	IMG256_GROUP conform_body;			// 19 ȷ��  
		
	IMG256_GROUP baobao_item_action;	// 20 ������Ʒ��������	

	IMG256_GROUP baobao_item_mask;		// 32 ������Ʒ mask

	IMG256_GROUP store_body;			// 27 �̵� body
	IMG256_GROUP store_sell_mark;		// 28 sell mark
	IMG256_GROUP store_buy_mark; 		// 29 buy mark
	IMG256_GROUP store_sellmake;		// 30 ����
		
	IMG256_GROUP baobao_icon;			// 31 BAOBAO ͼ��

	IMG256_GROUP baobao_item_mark;		// 32 BAOBAO ʹ����Ʒ mark
	IMG256_GROUP bank_body;				// 33 BANK BODY
	IMG256_GROUP popshop_push_mark;		// 34 ���̴��� mark
	IMG256_GROUP popshop_pop_mark;		// 35 ����ȡ�� mark
		

// ----------------------------- ���еİ�ť --------------------
	SLONG button_handle;				// handle
	BUTTON_STATUS button_chat_select;	// ����ѡ�п�
	BUTTON_STATUS button_window_close;	// window �ر�
	BUTTON_STATUS button_add;			// �Ӻ�
	BUTTON_STATUS button_sub;			// ����
	BUTTON_STATUS button_up;			// ��
	BUTTON_STATUS button_down;			// ��
	BUTTON_STATUS button_check;			// ��
	
	BUTTON_STATUS button_chat_friend;	// ����
	BUTTON_STATUS button_chat_temp;		// ��ʱ����
	BUTTON_STATUS button_chat_mask;		// ���κ���

	BUTTON_STATUS button_chat_auto_answer;	// �Զ��ظ�
	BUTTON_STATUS button_chat_search;		// ��Ѱ����
	BUTTON_STATUS button_chat_add_friend;	// ��Ϊ����
// ---
	BUTTON_STATUS button_ok;				// ȷ��
	BUTTON_STATUS button_cancel;			// ȡ��

	BUTTON_STATUS button_chat_attr_friend;	// ��Ϊ����		����������(���š����������Բ鿴)
	BUTTON_STATUS button_chat_attr_temp;	// ��ʱ����		����������(���š����������Բ鿴)
	BUTTON_STATUS button_chat_person;		// ˽��			����������(���š����������Բ鿴)
	BUTTON_STATUS button_chat_cancel;		// �Ͻ�			����������(���Բ鿴)
	BUTTON_STATUS button_chat_attr_mask;	// ����			����������(���Բ鿴)
	BUTTON_STATUS button_chat_refresh;		// ����			����������(���Բ鿴)
	BUTTON_STATUS button_chat_hirstory;		// ��ʷ�ż�		����������(���Բ鿴)

// -------- ���
	IMG256_GROUP team_list_mark;			// �������
	IMG256_GROUP team_list_request_mark;	// �����б�	


// -------- Macro 
	IMG256_GROUP mc_window_body;			// �Ի�����ͼ 23






// -------- Item image 
	UCHR *item_image[MAX_ITEM_IMAGE];		// Item �� image

} SYSTEM_IMAGE;


#define MAX_FIGHT_IMAGE_NO					30

typedef struct FIGHT_IMAGE_STRUCT
{
	IMG256_GROUP menu_body;					// 0	������װ�
	IMG256_GROUP hp_body;					// 1	Ѫ��
	IMG256_GROUP mp_body;					// 2	����
	IMG256_GROUP speed_body;				// 3	����
	IMG256_GROUP action_body0;				// 4	�ж�ָ��򶯻�
	IMG256_GROUP action_body1;				// 5	�ж�ָ��򶯻�
	IMG256_GROUP action_body2;				// 6	�ж�ָ��򶯻�
	IMG256_GROUP attack_select;				// 7	����(ѡ��)
	IMG256_GROUP attack_push;				// 8	����(���)
	IMG256_GROUP magic_select;				// 9	����(ѡ��)
	IMG256_GROUP magic_push;				// 10   ����(���)
	IMG256_GROUP item_select;				// 11   ����(ѡ��)
	IMG256_GROUP item_push;					// 12   ����(���)
	IMG256_GROUP protect_select;			// 13   ����(ѡ��)
	IMG256_GROUP protect_push;				// 14   ����(���)
	IMG256_GROUP recover_select;			// 15   ����(ѡ��)
	IMG256_GROUP recover_push;				// 16   ����(���)
	IMG256_GROUP run_select;				// 17   ����(ѡ��)
	IMG256_GROUP run_push;					// 18   ����(���)
	IMG256_GROUP change_select;				// 19   ����(ѡ��)
	IMG256_GROUP change_push;				// 20   ����(���)
	IMG256_GROUP auto_hp_select;			// 21   �Զ���Ѫ(ѡ��)
	IMG256_GROUP auto_hp_push;				// 22   �Զ���Ѫ(���)
	IMG256_GROUP auto_mp_select;			// 23   �Զ��ط�(ѡ��)
	IMG256_GROUP auto_mp_push;				// 24   �Զ��ط�(���)
	IMG256_GROUP ai_select;					// 25   AIѡ��(ѡ��)
	IMG256_GROUP ai_push;					// 26   AIѡ��(���)
	IMG256_GROUP auto_attack_select;		// 27	�Զ�����(ѡ��)
	IMG256_GROUP auto_attack_push;			// 28	�Զ�����(���)	
	IMG256_GROUP map_menu_mask;				// 29	��ͼѡ�� mask

// ------ Other handle
	SLONG fight_item_handle;				// fight item handle
	SLONG number0_handle;					// ��ɫ С��
	SLONG number1_handle;					// ��ɫ С��
	SLONG number2_handle;					// ��ɫ ����
	SLONG double_handle;					// ���� MDA
	SLONG fullmiss_handle;					// status MDA
	SLONG fight_hit_handle;					// hit handle

	SLONG status_power_handle;				// ���� MDA
	SLONG status_chaos_handle;				// ���� MDA
	SLONG status_sleep_handle;				// ��˯ MDA
	SLONG status_plague_handle;				// ���� MDA
	SLONG status_dark_handle;				// �ڰ� MDA
	SLONG status_delay_handle;				// �ٻ� MDA
	SLONG status_stone_handle;				// ʯ�� MDA
	SLONG status_fastness_handle;			// �Ό MDA
	SLONG status_speed_handle;				// ���� MDA

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