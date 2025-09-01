/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : system.cpp
  *owner  		: Stephen
  *description  : 
  *modified     : 2004/12/6
******************************************************************************/ 

#include "engine_global.h"
#include "vari-ext.h"
#include "system.h"
#include "utility.h"
#include "dm_music.h"
#include "net.h"
#include "taskman.h"
#include "data.h"
#include "player_info.h"
#include "chat_msg.h"
#include "chatwindow.h"


#define SYSTEM_CONFIG_FILENAME "myth.cfg"



/***************************************************************************************}
{	initialize_system																	}
{	ϵͳ������ʼ��																		}
****************************************************************************************/
SLONG initialize_system(void)
{
	static bool initialize_flag=false;

	if(initialize_flag==true)
		return(TTN_OK);
	initialize_flag=true;


// --- clear
	clear_message_stack();
	clear_chat_friend_list();
	
// --- initialize 
	if(initialize_frame_data()!=TTN_OK)
	{
		display_error_message((UCHR *)"Initialize frame data Error !",true);
		return(RETURN_ERROR);
	}
		
	
	if(initialize_color_control()!=TTN_OK)
	{
		display_error_message((UCHR *)"Initialize color control Error !",true);
		return(RETURN_ERROR);
	}

	if(initialize_extra_mda()!=TTN_OK)
	{
		display_error_message((UCHR *)"Initialize extra mda Error !",true);
		return(RETURN_ERROR);
	}

	if(initializa_display_text()!=TTN_OK)
	{
		display_error_message((UCHR *)"Initialize display text Error !",true);
		return(RETURN_ERROR);
	}

	if(initialize_game_control()!=TTN_OK)
	{
		display_error_message((UCHR *)"Initialize game control Error !",true);
		return(RETURN_ERROR);
	}

	if(initialize_music_voice()!=TTN_OK)
	{
		display_error_message((UCHR *)"Initialize music & voice Error !",true);
		return(RETURN_ERROR);
	}

	if(read_system_image()!=TTN_OK)
	{
		display_error_message((UCHR *)"Read system image error !",true);
		return(RETURN_ERROR);
	}

	if(initialize_map_base_data()!=TTN_OK)
	{
		display_error_message((UCHR *)"Initialize map_base_data Error !",true);
		return(RETURN_ERROR);
	}

	if(initialize_map_control()!=TTN_OK)
	{
		display_error_message((UCHR *)"Initialize map control Error !",true);
		return(RETURN_ERROR);
	}

	if(initialize_system_control()!=TTN_OK)
	{
		display_error_message((UCHR *)"Initialize system control Error !",true);
		return(RETURN_ERROR);
	}

	if(initialize_chat_data()!=TTN_OK)
	{
		display_error_message((UCHR *)"Initialize chat data Error !",true);
		return(RETURN_ERROR);
	}

	if(initialize_chat_data_buffer()!=TTN_OK)
	{
		display_error_message((UCHR *)"Initialize chat data buffer Error !",true);
		return(RETURN_ERROR);
		
	}


	if(initialize_start_up_data()!=TTN_OK)
	{
		display_error_message((UCHR *)"Initialize START_UP Error !",true);
		return(RETURN_ERROR);
	}

	init_ripple();
	system_idle_loop();
	
	return(RETURN_OK);
}


void clear_message_stack(void)
{
	SLONG i;
	for(i=0;i<MAX_MESSAGE_STACK;i++)
		message_stack[i].active=false;
}


SLONG initialize_frame_data(void)
{
	create_new_frame(1);
	return(TTN_OK);
}


void create_new_frame(SLONG no)
{
	SLONG i;
	
	if(frame_data!=NULL)
		free_frame_data();
	frame_data=(FRAME_DATA *)malloc(sizeof(struct FRAME_DATA_STRUCT)*no);
	if(frame_data==NULL)
	{
		display_message((UCHR *)"create Malloc error(create new frame)",true);
		return;
	}
	
//	now_camera_zoom=0;
//	now_camera_pan_x=0;
//	now_camera_pan_y=0;
	for(i=0;i<no;i++)
	{
		if(control_ani_head.control_type==MAP_CONTROL_TYPE)
		{
			frame_data[i].center_x=SCREEN_WIDTH/2;
			frame_data[i].center_y=SCREEN_HEIGHT/2;
		}
		else
		{
			frame_data[i].center_x=FIGHT_MAP_WIDTH/2;
			frame_data[i].center_y=FIGHT_MAP_HEIGHT/2;
		}
		frame_data[i].zoom=now_camera_zoom;
		frame_data[i].effect=0;
		frame_data[i].fade_alpha_value=0;
		frame_data[i].fade_alpha_color_r=0;
		frame_data[i].fade_alpha_color_g=0;
		frame_data[i].fade_alpha_color_b=0;
		
		frame_data[i].total_handle=0;
		frame_data[i].key_frame=0;
		frame_data[i].handle=NULL;
		frame_data[i].track_no=NULL;
		frame_data[i].character_frame_no=NULL;
		frame_data[i].char_x=NULL;
		frame_data[i].char_y=NULL;
		frame_data[i].zoom_value=NULL;
		frame_data[i].alpha_value=NULL;
	}
	ani_total_frame=no;
	ani_now_frame_no=0;
}



SLONG initialize_system_control(void)
{
	system_control.control_npc_idx=-1;
	system_control.control_user_id=0;
	system_control.point_npc_idx=-1;

	system_control.person_npc_id=0;
	system_control.macro_npc_id=0;

	system_control.system_timer=0;
	system_control.mouse_timer=0;
	system_control.data_key=NULL;
	system_control.key=NULL;
	system_control.mouse_key=NULL;
	system_control.mouse_x=400;
	system_control.mouse_y=300;
	return(TTN_OK);
}




void init_random(void)
{
	ULONG start;
	SLONG i,dd;
	
	start=GetTickCount()%1000;
	for(i=0;i<(SLONG)start;i++)
		dd=rand()%1000;
	
}


/***************************************************************************************}
{	initialize_system_config															}
{	��ȡϵͳ�趨����, ���û��ϵͳ�趨����,���Զ�����									}
****************************************************************************************/
void initialize_system_config(void)
{
FILE *fp;
SLONG struct_size;

	struct_size=sizeof(struct SYSTEM_CONFIG_STRUCT);
	if((check_file_exist((UCHR *)SYSTEM_CONFIG_FILENAME)!=TTN_OK)||
		(check_file_size((UCHR *)SYSTEM_CONFIG_FILENAME)!=struct_size))
	{					// Auto initialize 
		create_system_config();
	}
	else
	{
		fp=fopen(SYSTEM_CONFIG_FILENAME,"rb");
		if(fp==NULL)
		{
			create_system_config();
		}
		else
		{
			fread(&system_config,sizeof(struct SYSTEM_CONFIG_STRUCT),1,fp);
			fclose(fp);
		}
	}

	update_client_config();
}


void create_system_config(void)
{
	strcpy((char *)system_config.copyright,"UNIGIUM SYSTEM");
	system_config.text_end_code=0x1A;
	system_config.window_mode=0;	// �Ӵ�ģʽ
	system_config.music_flag=1;		// ���ֿ���
	system_config.voice_flag=1;		// ��Ч����
	system_config.volume=100;		// ������Ч���� ( 0 -> 100 )
	system_config.record_flag=0;	// ��Ϸ¼�񿪹� ( ���� )
	system_config.fight_flag=1;		// �д迪�� ( �ر� )
	system_config.mail_flag=1;		// �ż����� ( �ż����ܴ� )
	system_config.item_flag=1;		// ��Ʒ���� ( ��Ʒ���ܴ� )
	system_config.friend_flag=1;	// ���ѿ��� ( ������Ѵ� )
	system_config.team_flag=1;		// ��ӿ��� ( ������Ӽ��� )
//	system_config.map_effect=1;		// ��ͼ��Ч ( ��ͼ��Ч�� )
	system_config.head_talk=1;		// ͷ���Ի����� ( ����ͷ���Ի���ʾ )
	system_config.strange_flag=0;	// İ���˿��� ( �ر� )
	system_config.quick_menu=1;		// �Ҽ�ѡ������ ( �����Ҽ�ѡ�� )
	system_config.clothes_flag=1;	// װ����ʾ���� ( ��ʾװ����ֵ )
// ---
	system_config.default_server_id=-1;
// --- chat
	system_config.channel_screen_flag=1;	// ��ǰƵ������
	system_config.channel_team_flag=1;		// ����Ƶ������
	system_config.channel_person_flag=1;	// ˽��Ƶ������
	system_config.channel_group_flag=1;		// ����Ƶ������
	system_config.channel_sellbuy_flag=1;	// ����Ƶ������
	system_config.channel_world_flag=1;		// ����Ƶ������

	update_system_config();
}

void update_system_config(void)
{
FILE *fp;

	fp=fopen((const char *)SYSTEM_CONFIG_FILENAME,"wb");
	if(fp==NULL)
	{
		sprintf((char *)print_rec,MSG_FILE_CREAT_ERROR,SYSTEM_CONFIG_FILENAME);
		log_error(1,print_rec);
		return;
	}
	
	// --------- Save SAVE_HEAD
	fwrite(&system_config,sizeof(struct SYSTEM_CONFIG_STRUCT),1,fp);
	fclose(fp);
}


/***************************************************************************************}
{	initialize color control 															}
****************************************************************************************/
SLONG initialize_color_control(void)
{
	SLONG i;

	color_control.white=rgb2hi(255,255,255);
	color_control.black=rgb2hi(0,0,0);
	color_control.red=rgb2hi(255,0,0);
	color_control.green=rgb2hi(0,255,0);
	color_control.blue=rgb2hi(0,0,255);
	color_control.yellow=rgb2hi(255,255,0);
	color_control.pink=rgb2hi(255,0,255);
	color_control.cyan=rgb2hi(0,255,255);
	color_control.gray=rgb2hi(128,128,128);

	color_control.light_green=rgb2hi(128,255,128);
	color_control.light_blue=rgb2hi(128,128,255);
	color_control.oringe=rgb2hi(255,102,0);
	

	color_control.low_gray=rgb2hi(64,64,64);


	color_control.life_base0=rgb2hi(0,255,0);
	color_control.life_base1=rgb2hi(255,140,0);
	color_control.life_base2=rgb2hi(250,140,250);
	color_control.life_base3=rgb2hi(0,255,255);
		
	color_control.window_blue=rgb2hi(7,44,124);

	for(i=0;i<256;i++)
	{
		r_full_palette[i*3]=0xFF;
		r_full_palette[i*3+1]=0x00;
		r_full_palette[i*3+2]=0x00;
		
		g_full_palette[i*3]=0x00;
		g_full_palette[i*3+1]=0xFF;
		g_full_palette[i*3+2]=0x00;
		
		b_full_palette[i*3]=0x00;
		b_full_palette[i*3+1]=0x00;
		b_full_palette[i*3+2]=0xFF;
		
		yellow_full_palette[i*3]=0xFF;
		yellow_full_palette[i*3+1]=0xFF;
		yellow_full_palette[i*3+2]=0x00;
		
		black_full_palette[i*3]=0x00;
		black_full_palette[i*3+1]=0x00;
		black_full_palette[i*3+2]=0x00;
		
		white_full_palette[i*3]=0xFF;
		white_full_palette[i*3+1]=0xFF;
		white_full_palette[i*3+2]=0xFF;

		mask_full_palette[i*3]=60;
		mask_full_palette[i*3+1]=60;
		mask_full_palette[i*3+2]=60;

		point_palette[i*3]=0xFF;
		point_palette[i*3+1]=0;
		point_palette[i*3+2]=0;
		
	}
	
	init_random();
	set_word_color(4,color_control.yellow);
	set_back_color(4,color_control.black);
	set_word_color(5,color_control.cyan);
	set_back_color(5,color_control.black);

	return(TTN_OK);	
}

/***************************************************************************************}
{	initialize GAME control  															}
****************************************************************************************/
SLONG initialize_game_control(void)
{
	SLONG i;

	strcpy((char *)game_control.music_filename,"");
	strcpy((char *)game_control.server_name,"");
	game_control.music_loop_flag=0;
	game_control.music_playing=0;
	game_control.screen_text_timer=0;

// --- main loop
	game_control.main_loop_task=MAIN_TASK_MOVE;		// Ŀǰ����·״̬ 
	game_control.main_loop_break=false;				// �Ƿ���������·
	game_control.main_loop_ret_val=RETURN_OK;		// main_loop �ش�ֵ
	game_control.main_loop_select_npc_idx=-1;		// ����·ѡ��NPC idx
	game_control.main_loop_select_npc_task=-1;		// ����·ѡ��NPC ��task

	game_control.main_loop_mode=MAIN_LOOP_MAP_MODE;	// Ĭ�Ͻ����ͼģʽ

// --- ����ϵͳ
	game_control.weather_enable=false;				// Ŀǰ��ͼ�Ƿ�ִ������ϵͳ
	game_control.weather_timer=0;
	game_control.weather_add_timer=0;
	game_control.weather_change_timer=0;

// --- ����
	game_control.chat_channel=CHAT_CHANNEL_SCREEN;	// ��ǰ����Ƶ��
	game_control.chat_timer=0;
	game_control.chat_message_in=0;

// --- main_loop ������ window �������
	game_control.window_channel=-1;					// Ƶ���л� window �� handle
	game_control.old_window_task=-1;
	game_control.chat_stack_index=0;				// Ŀǰ stack index
	game_control.chat_stack_find_index=0;			// 
	for(i=0;i<MAX_CHAT_STACK;i++)
	{
		strcpy((char *)game_control.chat_stack_buffer[MAX_CHAT_STACK],"");
	}
	game_control.chat_window_type=0;				// ������� 0 -> window  1-> �ڲ�
	game_control.chat_pause_flag=0;
	
// --- system window
	game_control.window_system=-1;
	game_control.window_system_button_idx=-1;


// --- quit window
	game_control.window_quit=-1;
	game_control.window_quit_button_idx=-1;
	

// --- ������
	game_control.window_chat=-1;
	game_control.window_chat_type=0;

// --- �������
	game_control.window_phiz=-1;

// --- ����״̬
	game_control.window_character_status=-1;

// --- ����״̬
	game_control.window_baobao_status=-1;

// --- ��ʷ����
	game_control.window_history=-1;

// --- ������Ʒ
	game_control.window_item=-1;

// --- ����
	game_control.window_mission=-1;

// --- ����
	game_control.window_group=-1;

// --- send message
	game_control.window_send_message=-1;

// --- receive message
	game_control.window_receive_message=-1;

// --- chat attrib
	game_control.window_chat_attrib=-1;

// --- chat group send
	game_control.window_chat_group_send=-1;

// --- chat hirstory 
	game_control.window_chat_hirstory=-1;

// --- team
	game_control.window_team=-1;

// --- chat search
	game_control.window_chat_search=-1;

// --- team request
	game_control.window_team_request=-1;

// --- small map
	game_control.window_small_map=-1;

// --- world map
	game_control.window_world_map=-1;

// --- macro
	game_control.window_macro=-1;

// --- Give
	game_control.window_give=-1;

// --- Deal
	game_control.window_deal=-1;

// --- skill
	game_control.window_skill=-1;

// --- title 
	game_control.window_title=-1;

// --- baobao item
	game_control.window_baobao_item=-1;

// --- character fastness
	game_control.window_character_fastness=-1;

// --- baobao fastness
	game_control.window_baobao_fastness=-1;

// --- fight character skill
	game_control.window_fight_character_skill=-1;

// --- fight baobao skill
	game_control.window_fight_baobao_skill=-1;

// --- fight character item 
	game_control.window_fight_character_item=-1;

// --- fight baobao item
	game_control.window_baobao_item=-1;
	
// --- fight baobao change
	game_control.window_fight_baobao_change=-1;	// ս����������
	
// --- fight talk window 
	game_control.window_fight_talk=-1;			// ս����������
	

	
// -------- for debug
//	insert_task_list(auto_change_weather,3);
	insert_task_list(display_user_info,1);
	insert_task_list(display_map_info,2);
	

	return(TTN_OK);
}


/***************************************************************************************}
{	initialize music & voice  															}
****************************************************************************************/
SLONG initialize_music_voice(void)
{
SLONG i;

// -------------- clear voice control ------------------
	for(i=0;i<VOICE_CHANNEL_MAX;i++)
	{
		voice_control.busy[i]=0;
		voice_control.play_time[i]=0;
		voice_control.loop[i]=0;
		voice_control.voice_type[i]=0;
	}

	
	return(TTN_OK);
}


/***************************************************************************************}
{	Read all system used image file														}
****************************************************************************************/
SLONG read_system_image(void)
{
	SLONG i;
	SLONG idx;

	MOUSE_DATA_TABLE mouse_data[9]={
		{  MOUSE_IMG_STAND	,  4,  3	},		// �����
		{  MOUSE_IMG_ERROR	, 19,  21	},		// �������
		{  MOUSE_IMG_TEAM	, 18,  33	},		// ���	
		{  MOUSE_IMG_ATTACK	,  3,  2	},		// ����
		{  MOUSE_IMG_GIVE	,  1,  2	},		// ����
		{  MOUSE_IMG_DEAL	,  1,  1	},		// ����
		{  MOUSE_IMG_FRIEND	,  6,  3	},		// �Ӻ���		
		{  MOUSE_IMG_ITEM	,  7,  23	},		// ʹ����Ʒ
		{  MOUSE_IMG_MACRO	,  1,  4	}		// MACRO

	};
	
// ---------- ( 0 - 0 ) Mouse image 
	for(i=0;i<9;i++)
	{
		sprintf((char *)print_rec,"SYSTEM\\MOUSE%d.ani",mouse_data[i].id);	
		load_mouse_image_cursor(mouse_data[i].id,print_rec);	
		set_mouse_spot(mouse_data[i].id,mouse_data[i].x_offset,mouse_data[i].y_offset);
	}


// ---------- ( 0 - 0 ) base data
	mda_group_open((UCHR *)"system\\head.mda",&system_image.character_head);
	if(system_image.character_head<0)
		return(TTN_NOT_OK);
	mda_group_open((UCHR *)"system\\weather.mda",&system_image.weather_handle);
	if(system_image.weather_handle<0)
		return(TTN_NOT_OK);
	mda_group_open((UCHR *)"system\\icon.mda",&system_image.status_handle);
	if(system_image.status_handle<0)
		return(TTN_NOT_OK);
	mda_group_open((UCHR *)"ani\\skl001.mda",&system_image.full_handle);
	if(system_image.full_handle<0)
		return(TTN_NOT_OK);
	
	system_image.icon_timer=0;

// ---------- ( 0 - 1 ) Message Window
	if(read_mda_frame((UCHR *)"system\\info.mda",0,&system_image.msg_TopLeft.image)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_frame((UCHR *)"system\\info.mda",1,&system_image.msg_Top.image)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_frame((UCHR *)"system\\info.mda",2,&system_image.msg_TopRight.image)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_frame((UCHR *)"system\\info.mda",3,&system_image.msg_Left.image)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_frame((UCHR *)"system\\info.mda",4,&system_image.msg_Right.image)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_frame((UCHR *)"system\\info.mda",5,&system_image.msg_BottomLeft.image)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_frame((UCHR *)"system\\info.mda",6,&system_image.msg_Bottom.image)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_frame((UCHR *)"system\\info.mda",7,&system_image.msg_BottomRight.image)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_frame((UCHR *)"system\\info.mda",8,&system_image.msg_Middle.image)!=TTN_OK)
		return(TTN_NOT_OK);
	
// --------- ( 0 - 2 ) ��ȡ 
	if(read_mda_frame((UCHR *)"system\\c1meu018a.mda",0,&system_image.status_body.image)!=TTN_OK)		// �����뱦������		0
		return(TTN_NOT_OK);
	get_img256_rect(system_image.status_body.image,&system_image.status_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\c1meu018a.mda",1,&system_image.map_body.image)!=TTN_OK)			// С��ͼ����			1
		return(TTN_NOT_OK);
	get_img256_rect(system_image.map_body.image,&system_image.map_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\c1meu018a.mda",2,&system_image.map_main_body.image)!=TTN_OK)		// ��ͼ����������		2	
		return(TTN_NOT_OK);
	get_img256_rect(system_image.map_main_body.image,&system_image.map_main_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\c1meu018a.mda",3,&system_image.channel_body.image)!=TTN_OK)		// Ƶ��ѡ������			3
		return(TTN_NOT_OK);
	get_img256_rect(system_image.channel_body.image,&system_image.channel_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\c1meu018a.mda",4,&system_image.face_font.image)!=TTN_OK)			// ͷ�����				4
		return(TTN_NOT_OK);
	get_img256_rect(system_image.face_font.image,&system_image.face_font.rect);
	
	if(read_mda_frame((UCHR *)"system\\c1meu018a.mda",5,&system_image.char_hp.image)!=TTN_OK)			// ���� HP body			5
		return(TTN_NOT_OK);
	get_img256_rect(system_image.char_hp.image,&system_image.char_hp.rect);
	
	if(read_mda_frame((UCHR *)"system\\c1meu018a.mda",6,&system_image.char_mp.image)!=TTN_OK)			// ���� MP body			6
		return(TTN_NOT_OK);
	get_img256_rect(system_image.char_mp.image,&system_image.char_mp.rect);
	
	if(read_mda_frame((UCHR *)"system\\c1meu018a.mda",7,&system_image.baobao_hp.image)!=TTN_OK)			// ���� HP				7
		return(TTN_NOT_OK);
	get_img256_rect(system_image.baobao_hp.image,&system_image.baobao_hp.rect);
	
	if(read_mda_frame((UCHR *)"system\\c1meu018a.mda",8,&system_image.baobao_exp.image)!=TTN_OK)		// ���� EXP				8
		return(TTN_NOT_OK);
	get_img256_rect(system_image.baobao_exp.image,&system_image.baobao_exp.rect);
	
	if(read_mda_frame((UCHR *)"system\\c1meu018a.mda",9,&system_image.baobao_mp.image)!=TTN_OK)			// ���� HP				9
		return(TTN_NOT_OK);
	get_img256_rect(system_image.baobao_mp.image,&system_image.baobao_mp.rect);
	
	if(read_mda_frame((UCHR *)"system\\c1meu018a.mda",10,&system_image.char_exp.image)!=TTN_OK)			// ���� exp				10
		return(TTN_NOT_OK);
	get_img256_rect(system_image.char_exp.image,&system_image.char_exp.rect);
	
	mda_group_open((UCHR *)"system\\c1meu018b.mda",&system_image.main_menu_handle);
	if(system_image.main_menu_handle<0)
		return(TTN_NOT_OK);

// ---- main menu	
	if(read_mda_button(system_image.main_menu_handle,0,&system_image.button_channel_active,(UCHR *)"",-1)!=TTN_OK)
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.main_menu_handle,29,&system_image.button_map_main_menu[0],(UCHR *)MSG_MAP_MAIN_MENU_INST_ITEM,MAP_MAIN_MENU_TASK_ITEM)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_button(system_image.main_menu_handle,31,&system_image.button_map_main_menu[1],(UCHR *)MSG_MAP_MAIN_MENU_INST_TEAM,MAP_MAIN_MENU_TASK_TEAM)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_button(system_image.main_menu_handle,33,&system_image.button_map_main_menu[2],(UCHR *)MSG_MAP_MAIN_MENU_INST_ATTACK,MAP_MAIN_MENU_TASK_ATTACK)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_button(system_image.main_menu_handle,35,&system_image.button_map_main_menu[3],(UCHR *)MSG_MAP_MAIN_MENU_INST_GIVE,MAP_MAIN_MENU_TASK_GIVE)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_button(system_image.main_menu_handle,37,&system_image.button_map_main_menu[4],(UCHR *)MSG_MAP_MAIN_MENU_INST_DEAL,MAP_MAIN_MENU_TASK_DEAL)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_button(system_image.main_menu_handle,39,&system_image.button_map_main_menu[5],(UCHR *)MSG_MAP_MAIN_MENU_INST_MISSION,MAP_MAIN_MENU_TASK_MISSION)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_button(system_image.main_menu_handle,41,&system_image.button_map_main_menu[6],(UCHR *)MSG_MAP_MAIN_MENU_INST_GROUP,MAP_MAIN_MENU_TASK_GROUP)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_button(system_image.main_menu_handle,43,&system_image.button_map_main_menu[7],(UCHR *)MSG_MAP_MAIN_MENU_INST_SYSTEM,MAP_MAIN_MENU_TASK_SYSTEM)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_button(system_image.main_menu_handle,45,&system_image.button_map_main_menu[8],(UCHR *)MSG_MAP_MAIN_MENU_INST_CHAT,MAP_MAIN_MENU_TASK_CHAT)!=TTN_OK)
		return(TTN_NOT_OK);

// --- chat button	
	if(read_mda_button(system_image.main_menu_handle,13,&system_image.button_map_main_menu[9],(UCHR *)MSG_MAP_MAIN_MENU_INST_CHATWIN,MAP_MAIN_MENU_TASK_CHATWIN)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_button(system_image.main_menu_handle,15,&system_image.button_map_main_menu[10],(UCHR *)MSG_MAP_MAIN_MENI_INST_HISTORY,MAP_MAIN_MENI_TASK_HISTORY)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_button(system_image.main_menu_handle,17,&system_image.button_map_main_menu[11],(UCHR *)MSG_MAP_MAIN_MENU_INST_DISCARE,MAP_MAIN_MENU_TASK_DISCARE)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_button(system_image.main_menu_handle,19,&system_image.button_map_main_menu[12],(UCHR *)MSG_MAP_MAIN_MENU_INST_PHIZ,MAP_MAIN_MENU_TASK_PHIZ)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_button(system_image.main_menu_handle,21,&system_image.button_map_main_menu[13],(UCHR *)MSG_MAP_MAIN_MENU_INST_SCROLL,MAP_MAIN_MENU_TASK_SCROLL)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_button(system_image.main_menu_handle,23,&system_image.button_map_main_menu[14],(UCHR *)MSG_MAP_MAIN_MENU_INST_VIEW,MAP_MAIN_MENU_TASK_VIEW)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_button(system_image.main_menu_handle,25,&system_image.button_map_main_menu[15],(UCHR *)MSG_MAP_MAIN_MENU_INST_UP,MAP_MAIN_MENU_TASK_UP)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_button(system_image.main_menu_handle,27,&system_image.button_map_main_menu[16],(UCHR *)MSG_MAP_MAIN_MENU_INST_DOWN,MAP_MAIN_MENU_TASK_DOWN)!=TTN_OK)
		return(TTN_NOT_OK);

// --- chat channel button
	if(read_mda_button(system_image.main_menu_handle,1,&system_image.button_chat_channel[0],(UCHR *)"",MAP_MAIN_MENU_TASK_CHATCHANNEL)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_button(system_image.main_menu_handle,3,&system_image.button_chat_channel[1],(UCHR *)"",MAP_MAIN_MENU_TASK_CHATCHANNEL)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_button(system_image.main_menu_handle,5,&system_image.button_chat_channel[2],(UCHR *)"",MAP_MAIN_MENU_TASK_CHATCHANNEL)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_button(system_image.main_menu_handle,7,&system_image.button_chat_channel[3],(UCHR *)"",MAP_MAIN_MENU_TASK_CHATCHANNEL)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_button(system_image.main_menu_handle,9,&system_image.button_chat_channel[4],(UCHR *)"",MAP_MAIN_MENU_TASK_CHATCHANNEL)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_button(system_image.main_menu_handle,11,&system_image.button_chat_channel[5],(UCHR *)"",MAP_MAIN_MENU_TASK_CHATCHANNEL)!=TTN_OK)
		return(TTN_NOT_OK);
	
// --- small map
	if(read_mda_button(system_image.main_menu_handle,47,&system_image.button_small_map[0],(UCHR *)MSG_MAP_MAIN_MENU_INST_SWITCH,MAP_MAIN_MENU_TASK_SWITCH)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_button(system_image.main_menu_handle,49,&system_image.button_small_map[1],(UCHR *)MSG_MAP_MAIN_MENU_INST_WORLD,MAP_MAIN_MENU_TASK_WORLD)!=TTN_OK)
		return(TTN_NOT_OK);
	if(read_mda_button(system_image.main_menu_handle,51,&system_image.button_small_map[2],(UCHR *)MSG_MAP_MAIN_MENU_INST_SMALL,MAP_MAIN_MENU_TASK_SMALL)!=TTN_OK)
		return(TTN_NOT_OK);
	

// --- system board
	mda_group_open((UCHR *)"system\\c1meu026.mda",&system_image.system_menu_handle);
	if(system_image.system_menu_handle<0)
		return(TTN_NOT_OK);
	if(read_mda_frame((UCHR *)"system\\c1meu026.mda",1,&system_image.volume.image )!=TTN_OK)			// ������С 1
		return(TTN_NOT_OK);
	get_img256_rect(system_image.volume.image,&system_image.volume.rect);
	
	if(read_mda_button(system_image.system_menu_handle,0,&system_image.system_background,(UCHR *)"",0)!=TTN_OK)	// ��ͼ 0
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.system_menu_handle,2,&system_image.system_close,(UCHR *)MSG_WINDOW_CLOSE,2)!=TTN_OK)	// �رհ�ť 2
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.system_menu_handle,4,&system_image.sub_volume,(UCHR *)MSG_VOLUME_SUB,4)!=TTN_OK)  // �������� 4
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.system_menu_handle,6,&system_image.add_volume,(UCHR *)MSG_VOLUME_ADD,6)!=TTN_OK)  // �����Ӵ� 6
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.system_menu_handle,8,&system_image.system_save,(UCHR *)"",8)!=TTN_OK)  // ���水ť 8
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.system_menu_handle,11,&system_image.system_help,(UCHR *)"",11)!=TTN_OK)  // ��Ϸ���� 11
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.system_menu_handle,14,&system_image.system_bbs,(UCHR *)"",14)!=TTN_OK)  // ��Ϸ��̳ 14
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.system_menu_handle,17,&system_image.system_quit,(UCHR *)"",17)!=TTN_OK)  // �˳���Ϸ 17
		return(TTN_NOT_OK);


// --- quit board
	mda_group_open((UCHR *)"system\\c1meu074.mda",&system_image.quit_menu_handle);
	if(system_image.quit_menu_handle<0)
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.quit_menu_handle,0,&system_image.quit_body,(UCHR *)"",0)!=TTN_OK)	// ��ͼ 0
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.quit_menu_handle,1,&system_image.quit_restart,(UCHR *)"",1)!=TTN_OK)	// ���¿�ʼ
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.quit_menu_handle,4,&system_image.quit_quit,(UCHR *)"",4)!=TTN_OK)	// ������Ϸ
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.quit_menu_handle,7,&system_image.quit_cancel,(UCHR *)"",7)!=TTN_OK)	// ������Ϸ
		return(TTN_NOT_OK);

// ----- ���ƶ� window body
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",0,&system_image.chat_body.image)!=TTN_OK)			// ������
		return(TTN_NOT_OK);
	get_img256_rect(system_image.chat_body.image,&system_image.chat_body.rect);

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",1,&system_image.character_attrib_body.image)!=TTN_OK)	// ��������
		return(TTN_NOT_OK);
	get_img256_rect(system_image.character_attrib_body.image,&system_image.character_attrib_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",2,&system_image.baobao_attrib_body.image)!=TTN_OK)	// ��������
		return(TTN_NOT_OK);
	get_img256_rect(system_image.baobao_attrib_body.image,&system_image.baobao_attrib_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",3,&system_image.history_body.image)!=TTN_OK)		// ��ʷ����
		return(TTN_NOT_OK);
	get_img256_rect(system_image.history_body.image,&system_image.history_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",4,&system_image.item_body.image)!=TTN_OK)			// ��Ʒװ��
		return(TTN_NOT_OK);
	get_img256_rect(system_image.item_body.image,&system_image.item_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",5,&system_image.team_body.image)!=TTN_OK)			// ���
		return(TTN_NOT_OK);
	get_img256_rect(system_image.team_body.image,&system_image.team_body.rect);

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",6,&system_image.give_body.image)!=TTN_OK)			// ����
		return(TTN_NOT_OK);
	get_img256_rect(system_image.give_body.image,&system_image.give_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",7,&system_image.deal_body.image)!=TTN_OK)			// ����
		return(TTN_NOT_OK);
	get_img256_rect(system_image.deal_body.image,&system_image.deal_body.rect);

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",8,&system_image.mission_body.image)!=TTN_OK)		// ����
		return(TTN_NOT_OK);
	get_img256_rect(system_image.mission_body.image,&system_image.mission_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",9,&system_image.group_body.image)!=TTN_OK)			// ����
		return(TTN_NOT_OK);
	get_img256_rect(system_image.group_body.image,&system_image.group_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",10,&system_image.chat_receive_body.image)!=TTN_OK)	// ������ ����
		return(TTN_NOT_OK);
	get_img256_rect(system_image.chat_receive_body.image,&system_image.chat_receive_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",11,&system_image.chat_send_body.image)!=TTN_OK)		// ������ ����
		return(TTN_NOT_OK);
	get_img256_rect(system_image.chat_send_body.image,&system_image.chat_send_body.rect);

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",12,&system_image.chat_setup.image)!=TTN_OK)			// ������ setup
		return(TTN_NOT_OK);
	get_img256_rect(system_image.chat_setup.image,&system_image.chat_setup.rect);
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",13,&system_image.chat_search.image)!=TTN_OK)		// ������ search
		return(TTN_NOT_OK);
	get_img256_rect(system_image.chat_search.image,&system_image.chat_search.rect);
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",14,&system_image.chat_attrib_body.image)!=TTN_OK)		// ������ ����
		return(TTN_NOT_OK);
	get_img256_rect(system_image.chat_attrib_body.image,&system_image.chat_attrib_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",15,&system_image.chat_group_send_body.image)!=TTN_OK)		// ������ Ⱥ��
		return(TTN_NOT_OK);
	get_img256_rect(system_image.chat_group_send_body.image,&system_image.chat_group_send_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",16,&system_image.chat_hirstory_mask.image)!=TTN_OK)		// ��������ʷ���� mask
		return(TTN_NOT_OK);
	get_img256_rect(system_image.chat_hirstory_mask.image,&system_image.chat_hirstory_mask.rect);
	



// ------------- ���а�ť --------------
	mda_group_open((UCHR *)"system\\mapbutton.mda",&system_image.button_handle);
	if(system_image.button_handle<0)
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.button_handle,0,&system_image.button_chat_select,(UCHR *)"",0)!=TTN_OK)	// ����ѡ�п�
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.button_handle,1,&system_image.button_window_close,(UCHR *)"",1)!=TTN_OK)	// window �ر�
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.button_handle,3,&system_image.button_add,(UCHR *)"",3)!=TTN_OK)		// �Ӻ�
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.button_handle,5,&system_image.button_sub,(UCHR *)"",5)!=TTN_OK)		// ����
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.button_handle,7,&system_image.button_up,(UCHR *)"",7)!=TTN_OK)		// ��
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.button_handle,9,&system_image.button_down,(UCHR *)"",9)!=TTN_OK)	// ��
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.button_handle,11,&system_image.button_check,(UCHR *)"",11)!=TTN_OK)	// ��
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.button_handle,12,&system_image.button_chat_friend,(UCHR *)"",12)!=TTN_OK)	// ����
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.button_handle,15,&system_image.button_chat_temp,(UCHR *)"",15)!=TTN_OK)	// ��ʱ����
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.button_handle,18,&system_image.button_chat_mask,(UCHR *)"",18)!=TTN_OK)	// ���κ���
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.button_handle,21,&system_image.button_chat_auto_answer,(UCHR *)"",21)!=TTN_OK)	// �Զ��ظ�
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.button_handle,23,&system_image.button_chat_search,(UCHR *)"",23)!=TTN_OK)	// ��Ѱ����
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.button_handle,25,&system_image.button_chat_add_friend,(UCHR *)"",25)!=TTN_OK)	// ��Ϊ����
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.button_handle,27,&system_image.button_ok,(UCHR *)"",27)!=TTN_OK)	// ȷ��
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.button_handle,29,&system_image.button_cancel,(UCHR *)"",29)!=TTN_OK)	// ȡ��
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.button_handle,31,&system_image.button_chat_attr_friend,(UCHR *)"",31)!=TTN_OK)	// ��Ϊ����
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.button_handle,33,&system_image.button_chat_attr_temp,(UCHR *)"",33)!=TTN_OK)	// ��Ϊ��ʱ����
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.button_handle,35,&system_image.button_chat_person,(UCHR *)"",35)!=TTN_OK)	// ˽��
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.button_handle,37,&system_image.button_chat_cancel,(UCHR *)"",37)!=TTN_OK)	// �Ͻ�
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.button_handle,39,&system_image.button_chat_attr_mask,(UCHR *)"",39)!=TTN_OK)		// ����
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.button_handle,41,&system_image.button_chat_refresh,(UCHR *)"",41)!=TTN_OK)	// ����
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.button_handle,43,&system_image.button_chat_hirstory,(UCHR *)"",43)!=TTN_OK)	// ��ʷ�ż�
		return(TTN_NOT_OK);
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",17,&system_image.team_list_mark.image)!=TTN_OK)		// �������
		return(TTN_NOT_OK);
	get_img256_rect(system_image.team_list_mark.image,&system_image.team_list_mark.rect);


	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",18,&system_image.team_list_request_mark.image)!=TTN_OK)		// �����б����
		return(TTN_NOT_OK);
	get_img256_rect(system_image.team_list_request_mark.image,&system_image.team_list_request_mark.rect);

	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",19,&system_image.conform_body.image)!=TTN_OK)		// ȷ�� body
		return(TTN_NOT_OK);
	get_img256_rect(system_image.conform_body.image,&system_image.conform_body.rect);

	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",20,&system_image.baobao_item_action.image)!=TTN_OK)		// 20 ������Ʒ��������	
		return(TTN_NOT_OK);
	get_img256_rect(system_image.baobao_item_action.image,&system_image.baobao_item_action.rect);
	

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",21,&system_image.skill_body.image)!=TTN_OK)			// ���ܲ鿴
		return(TTN_NOT_OK);
	get_img256_rect(system_image.skill_body.image,&system_image.skill_body.rect);
	

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",22,&system_image.title_body.image)!=TTN_OK)			// ͷ�θ���
		return(TTN_NOT_OK);
	get_img256_rect(system_image.title_body.image,&system_image.title_body.rect);
	

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",23,&system_image.mc_window_body.image)!=TTN_OK)		// Macro window body
		return(TTN_NOT_OK);
	get_img256_rect(system_image.mc_window_body.image,&system_image.mc_window_body.rect);
	

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",24,&system_image.item_public_body.image)!=TTN_OK)		// ��Ʒ�����ò���
		return(TTN_NOT_OK);
	get_img256_rect(system_image.item_public_body.image,&system_image.item_public_body.rect);

	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",25,&system_image.item_action_body.image)!=TTN_OK)		// ���붯������
		return(TTN_NOT_OK);
	get_img256_rect(system_image.item_action_body.image,&system_image.item_action_body.rect);

	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",26,&system_image.deal_action_body.image)!=TTN_OK)		// ���׶�������
		return(TTN_NOT_OK);
	get_img256_rect(system_image.deal_action_body.image,&system_image.deal_action_body.rect);
	

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",27,&system_image.store_body.image)!=TTN_OK)		// �̵�body
		return(TTN_NOT_OK);
	get_img256_rect(system_image.store_body.image,&system_image.store_body.rect);
	

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",28,&system_image.store_sell_mark.image)!=TTN_OK)		// �̵���mask
		return(TTN_NOT_OK);
	get_img256_rect(system_image.store_sell_mark.image,&system_image.store_sell_mark.rect);
	

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",29,&system_image.store_buy_mark.image)!=TTN_OK)		// �̵���mask
		return(TTN_NOT_OK);
	get_img256_rect(system_image.store_buy_mark.image,&system_image.store_buy_mark.rect);
	

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",30,&system_image.store_sellmake.image)!=TTN_OK)		// �̵����mask
		return(TTN_NOT_OK);
	get_img256_rect(system_image.store_sellmake.image,&system_image.store_sellmake.rect);
	

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",31,&system_image.baobao_icon.image)!=TTN_OK)		// ���ױ��� icon
		return(TTN_NOT_OK);
	get_img256_rect(system_image.baobao_icon.image,&system_image.baobao_icon.rect);
	

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",32,&system_image.baobao_item_mask.image)!=TTN_OK)		// 32 BAOBAO ʹ����Ʒ mark
		return(TTN_NOT_OK);
	get_img256_rect(system_image.baobao_item_mask.image,&system_image.baobao_item_mask.rect);

	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",33,&system_image.bank_body.image)!=TTN_OK)		// 33 BANK BODY
		return(TTN_NOT_OK);
	get_img256_rect(system_image.bank_body.image,&system_image.bank_body.rect);
	
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",34,&system_image.popshop_push_mark.image)!=TTN_OK)		// 34 ���̴��� mark
		return(TTN_NOT_OK);
	get_img256_rect(system_image.popshop_push_mark.image,&system_image.popshop_push_mark.rect);


	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",35,&system_image.popshop_pop_mark.image)!=TTN_OK)		// 35 ����ȡ�� mark
		return(TTN_NOT_OK);
	get_img256_rect(system_image.popshop_pop_mark.image,&system_image.popshop_pop_mark.rect);
	


// --- Item image 
	for(i=0;i<MAX_ITEM_IMAGE;i++)
	{
		system_image.item_image[i]=NULL;
		if(read_mda_frame((UCHR *)"system\\item.mda",i,&system_image.item_image[i])!=TTN_OK)		//  ��Ʒ image
			return(TTN_NOT_OK);
	}



// ---
	mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand7",-9999,-9999,screen_buffer,MDA_COMMAND_FIRST,0,0,0);
	idx=get_mda_index(system_image.button_handle);
	if(idx>=0)
	{
		for(i=0;i<256;i++)
		{
			gray_palette[i*3]=mda_group[idx].img256_head->palette[i*3];
			gray_palette[i*3+1]=gray_palette[i*3];
			gray_palette[i*3+2]=gray_palette[i*3];
		}
	}
		


// --- �趨��Ҫ�����(��ͼ����)
	s_editChat.Init();
	
	return(TTN_OK);
}


/***************************************************************************************}
{	Initialize MAP_BASE_DATA    														}
****************************************************************************************/
SLONG initialize_map_base_data(void)
{
	SLONG i;

	strcpy((char *)map_base_data.map_filename,"");
	strcpy((char *)map_base_data.map_info,"");
	map_base_data.cell_width=16;		// CELL�� 
	map_base_data.cell_height=8;		// CELL�� 
	map_base_data.map_width=800;		// �؈D�� Pixel
	map_base_data.map_width=600;		// �؈D�� Pixel
	map_base_data.map_angel=0;			// �؈D�Ƕ�
	map_base_data.total_map_layer=0;	// �ܹ����� layer
	map_base_data.map_layer_data=NULL;	// Layer ����	
// ---
	map_base_data.map_event_data.total_cell=0;	// �ܹ����� cell
	map_base_data.map_event_data.max_cell_x=0;
	map_base_data.map_event_data.max_cell_y=0;
	map_base_data.map_event_data.event_data=NULL;  // Event ����
// ---	
	map_base_data.total_map_link=0;
	map_base_data.map_link_data=NULL;			// ��������
// ---
	map_base_data.total_map_effect=0;
	map_base_data.map_effect_data=NULL;			// ��Ч����

// --- Redraw Map Data
	for(i=0;i<MAP_MAX_BOTTOM_BUFFER;i++)		// Create Bottom buffer
	{
		map_base_data.map_bottom_image[i].image=create_bitmap(SCREEN_WIDTH+1,SCREEN_HEIGHT+1);
		if(map_base_data.map_bottom_image[i].image==NULL)
		{
			log_error(1,MSG_MEMORY_ALLOC_ERROR);
			return(TTN_NOT_OK);
		}
		map_base_data.map_bottom_image[i].image_rect.left=-1;
		map_base_data.map_bottom_image[i].image_rect.top=-1;
		map_base_data.map_bottom_image[i].image_rect.right=-1;
		map_base_data.map_bottom_image[i].image_rect.bottom=-1;
	}


	return(TTN_OK);
}


void free_map_base_data(void)
{
SLONG i;

// --- Head
	map_base_data.cell_width=16;		// CELL�� 
	map_base_data.cell_height=8;		// CELL�� 
	map_base_data.map_width=800;		// �؈D�� Pixel
	map_base_data.map_width=600;		// �؈D�� Pixel
	map_base_data.map_angel=0;			// �؈D�Ƕ�
// --- image 	
	for(i=0;i<map_base_data.total_map_layer;i++)
	{
		if(map_base_data.map_layer_data[i].image!=NULL)
			free(map_base_data.map_layer_data[i].image);
		map_base_data.map_layer_data[i].image=NULL;
	}
	if(map_base_data.map_layer_data!=NULL)
	{
		free(map_base_data.map_layer_data);
		map_base_data.map_layer_data=NULL;
	}
// --- event 
	map_base_data.map_event_data.total_cell=0;	// �ܹ����� cell
	map_base_data.map_event_data.max_cell_x=0;
	map_base_data.map_event_data.max_cell_y=0;
	if(map_base_data.map_event_data.event_data!=NULL)
		free(map_base_data.map_event_data.event_data);
	map_base_data.map_event_data.event_data=NULL;  // Event ����
// --- Link
	map_base_data.total_map_link=0;
	if(map_base_data.map_link_data!=NULL)		// ��������
		free(map_base_data.map_link_data);
	map_base_data.map_link_data=NULL;

// --- Effect
	if(map_base_data.map_effect_data!=NULL)		// ��Ч����
	{
		for(i=0;i<map_base_data.total_map_effect;i++)
		{
			if(map_base_data.map_effect_data[i].times==MAP_EFFECT_IMG)
			{
				if(map_base_data.map_effect_data[i].handle>=0)
				{
					mda_group_close(&map_base_data.map_effect_data[i].handle);
					map_base_data.map_effect_data[i].handle=-1;
				}
			}
		}
	}

	map_base_data.total_map_effect=0;
	free(map_base_data.map_effect_data);
	map_base_data.map_effect_data=NULL;

	
}

/***************************************************************************************}
{	initialize MAP  control 															}
****************************************************************************************/
SLONG initialize_map_control(void)
{
	map_control.start_x=0;
	map_control.start_y=0;
	map_control.target_x=0;
	map_control.target_y=0;


	map_layer_touch.flag=false;
	map_layer_touch.character=false;
	map_layer_touch.total=0;


	map_control.screen_mask=create_bitmap(SCREEN_WIDTH,SCREEN_HEIGHT);
	if(map_control.screen_mask==NULL)
	{
		display_error_message((UCHR *)MSG_MEMORY_ALLOC_ERROR,1);
		return(TTN_NOT_OK);
	}
	map_control.draw_mask=false;		
	map_control.mask_npc=0xFFFF;
	map_control.draw_point_color=false;	


	map_control.character_title_color=color_control.cyan;	// ��ҳ�ν��ɫ
	map_control.npc_name_color=color_control.yellow;		// NPC ������ɫ
	map_control.point_color=color_control.red;				// ָ���˵���ɫ

										
	if(map_control.point_idx<0)
	{
		sprintf((char *)print_rec,MSG_FILE_OPEN_ERROR,CURSO_MDA_FILE);
		display_error_message(print_rec,1);
	}

// --- Add cursor MDA
	map_control.point_idx=add_extra_mda((UCHR *)CURSO_MDA_FILE,-999,-999,false,false,false,2,(UCHR *)"MainCommand0");
	
	return(TTN_OK);
}



SLONG initialize_chat_data(void)
{
	text_out_data.g_pChat=NULL;			
	text_out_data.pt.x=0;										// mouse position 
	text_out_data.pt.y=0;

	text_out_data.chatRect.left=0;
	text_out_data.chatRect.top=0;
	text_out_data.chatRect.right=SCREEN_WIDTH;
	text_out_data.chatRect.bottom=SCREEN_HEIGHT;
	
	memset(&text_out_data.chatnet,0,sizeof(CHATNET));
	

	text_out_data.g_pChat=new CChat;
	
	if (!SUCCEEDED(text_out_data.g_pChat->init("system\\Phiz.MDA")))
		return(TTN_NOT_OK);

	return(TTN_OK);	
	
}


SLONG initialize_extra_mda(void)
{
	SLONG i;

	for(i=0;i<MAX_EXTRA_MDA;i++)
	{
		extra_mda_data[i].handle=-1;								// MDA handle
		extra_mda_data[i].mda_index=-1;								// mda index
		extra_mda_data[i].display_x=0;								// ʵ������ 
		extra_mda_data[i].display_y=0;
		extra_mda_data[i].loop=false;								// �Ƿ� Loop ����
		extra_mda_data[i].play_end=false;							// �Ƿ��Ѿ������1��
		extra_mda_data[i].auto_close=true;							// �Ƿ����1���Զ��ر�(loop Ϊ false) ���߸�����ͼ�ر�
		extra_mda_data[i].hight_light_flag=false;					// �Ƿ���Ҫ HightLight
		extra_mda_data[i].level=0;									// �ȼ� 0 -> �ײ�  1-> �м��  2-> ���ϲ�
		strcpy((char *)extra_mda_data[i].command,"");				// ��������
		extra_mda_data[i].timer=0;
		extra_mda_data[i].xl=0;
		extra_mda_data[i].yl=0;
		extra_mda_data[i].center_x=0;
		extra_mda_data[i].center_y=0;
	}


	for(i=0;i<MAX_EXTRA_FIGHT_MDA;i++)
	{
		extra_fight_mda_data[i].handle=-1;								// MDA handle
		extra_fight_mda_data[i].mda_index=-1;								// mda index
		extra_fight_mda_data[i].display_x=0;								// ʵ������ 
		extra_fight_mda_data[i].display_y=0;
		extra_fight_mda_data[i].loop=false;								// �Ƿ� Loop ����
		extra_fight_mda_data[i].play_end=false;							// �Ƿ��Ѿ������1��
		extra_fight_mda_data[i].auto_close=true;							// �Ƿ����1���Զ��ر�(loop Ϊ false) ���߸�����ͼ�ر�
		extra_fight_mda_data[i].hight_light_flag=false;					// �Ƿ���Ҫ HightLight
		extra_fight_mda_data[i].level=0;									// �ȼ� 0 -> �ײ�  1-> �м��  2-> ���ϲ�
		strcpy((char *)extra_fight_mda_data[i].command,"");				// ��������
		extra_fight_mda_data[i].timer=0;
		extra_fight_mda_data[i].xl=0;
		extra_fight_mda_data[i].yl=0;
		extra_fight_mda_data[i].center_x=0;
		extra_fight_mda_data[i].center_y=0;
	}

	return(TTN_OK);

}


SLONG initializa_display_text(void)
{
	SLONG i;

	for(i=0;i<MAX_SCREEN_TEXT;i++)
	{
		screen_text_data[i].active=false;
		screen_text_data[i].timer=0;
		screen_text_data[i].rect.left=0;
		screen_text_data[i].rect.top=0;
		screen_text_data[i].rect.right=0;
		screen_text_data[i].rect.bottom=0;
		strcpy((char *)screen_text_data[i].text,"");
	
		screen_text_stack[i]=-1;		// clear
	}

	return(TTN_OK);
}


/***************************************************************************************}
{	ϵͳ idle loop																		}
{																						}
{	1. ���� idle_loop();																}
{	2. ȡ�� system_control.system_timer ��Ϊͬ������									}
{	3. ���� Task Manger																	}
{	4. ���� sleep ( CPU ��Դ )															}
{	5. ���� polling list 																}
{	6. ���� ����ѶϢ																	}
{																						}
{																						}
{																						}
****************************************************************************************/
void system_idle_loop(void)
{
	static ULONG check_time=0;
	static bool check_flag=false;
	ULONG idle_time;
	SLONG xl,yl;
	

// --- ( 0 - 0 )  ���� Idle_lood
	idle_loop();

// --- ( 0 - 1 ) ȡ�� system_control.system_timer ��Ϊͬ������
	system_control.system_timer=GetTickCount();

// --- ( 0 - 2 ) ȡ����� ID & IDX 
	system_control.control_npc_idx=get_map_npc_index(system_control.control_user_id);
	
// --- ( 0 - 3 ) Check MP3 Loop

	if((game_control.music_playing==1)&&(system_config.music_flag==1))		// ��Ҫ��� MP3 Loop
	{
		if(IsIconic(g_hDDWnd))		// �Ƿ���С��
		{
			musicPause(); 
		}
		else
		{
			musicPlay();
		}
			
		if((mp3_loop_flag==1)&&(game_control.music_loop_flag==1))
		{
			handleMusicEvent();
			mp3_loop_flag=0;
		}

	}

// --- ( 0 - 4 ) ��ȡ ������
	get_mouse_position(&system_control.mouse_x,&system_control.mouse_y);

	if(game_control.map_zoom_index>0)
	{
		xl=800-game_control.map_zoom_index*16;
		yl=600-game_control.map_zoom_index*12;
		system_control.zoom_mouse_x=system_control.mouse_x*xl/800+game_control.map_zoom_index*8;
		system_control.zoom_mouse_y=system_control.mouse_y*yl/600+game_control.map_zoom_index*6;
	}
	else
	{
		system_control.zoom_mouse_x=system_control.mouse_x;
		system_control.zoom_mouse_y=system_control.mouse_y;
	}
	



	system_control.mouse_key=get_mouse_key();
	system_control.key=read_system_key();
	system_control.data_key=read_data_key();
	if(fnCheckAltKey())
		system_control.alt_key=true;
	else
		system_control.alt_key=false;



// --- ( 0 - 3 ) ����������	
	updateNetData();



// --- ( 9 - 9 ) ����idle time

	if(check_flag)
	{
		idle_time=system_control.system_timer-check_time;
		if(idle_time<20)
		{
			idle_time=20-idle_time;
			Sleep(idle_time);
			check_time=system_control.system_timer;
		}
		
	}
	else
	{
		check_time=system_control.system_timer;
	}
	check_flag=!check_flag;
	



}



void dummy_draw(void)
{
}


void dummy_exec(void)
{
}









SLONG initialize_chat_data_buffer(void)
{
	SLONG i;

	for(i=0;i<MAX_CHAT_RECORD;i++)
	{
		chat_data_buffer.bottom_index=0;
		chat_data_buffer.display_index=0;
		chat_data_buffer.colume=7;
		chat_data_buffer.top_index=0;
		chat_data_buffer.record[i].user_id=0x80000000;
		chat_data_buffer.record[i].channel=CHAT_CHANNEL_MESSGAE;
		strcpy((char *)chat_data_buffer.record[i].text,"");


		hirstory_chat_data_buffer.bottom_index=0;
		hirstory_chat_data_buffer.display_index=0;
		hirstory_chat_data_buffer.colume=7;
		hirstory_chat_data_buffer.top_index=0;
		hirstory_chat_data_buffer.record[i].user_id=0x80000000;
		hirstory_chat_data_buffer.record[i].channel=CHAT_CHANNEL_MESSGAE;
		strcpy((char *)hirstory_chat_data_buffer.record[i].text,"");
		

	}

	hirstory_channel=CHAT_CHANNEL_SCREEN;

	return(TTN_OK);
}




void clear_chat_friend_list(void)
{
	SLONG i;

	for(i=0;i<MAX_CHAT_FRIEND_LIST;i++)
	{
		chat_friend_list[i].online=0;
		chat_friend_list[i].id=0;
		chat_friend_list[i].friend_type=0;					// һ�����
		chat_friend_list[i].nickname[0]=NULL;				// nicknane
		chat_friend_list[i].title[0]=NULL;					// ��ν
		chat_friend_list[i].level=-1;						// �ȼ�
		chat_friend_list[i].base_type=0;					// ������̬
		chat_friend_list[i].group[0]=0;						// ����
		chat_friend_list[i].nexus=0;						// ��ϵ
		chat_friend_list[i].amity=0;						// �Ѻö�

	}

	

// --- add dummy
/*
	for(i=0;i<MAX_CHAT_FRIEND_LIST-10;i++)
	{
		chat_friend_list[i].online=rand()%2;
		chat_friend_list[i].id=0;
		chat_friend_list[i].friend_type=rand()%3;					// һ�����
		sprintf((char *)chat_friend_list[i].nickname,"NAME%d",i);
		chat_friend_list[i].title[0]=NULL;					// ��ν
		chat_friend_list[i].level=i;						// �ȼ�
		chat_friend_list[i].base_type=0;					// ������̬
		chat_friend_list[i].group[0]=0;						// ����
		chat_friend_list[i].nexus=0;						// ��ϵ
		chat_friend_list[i].amity=0;						// �Ѻö�
		
	}
*/	

}



void reload_chat_friend_list(void)
{
	SLONG i;	

	for(i=0;i<MAX_CHAT_FRIEND_LIST;i++)
	{
		display_friend_list[i].level=-1;
		display_temp_list[i].level=-1;
		display_mask_list[i].level=-1;
		chat_group_send_flag[i]=false;
	}

	game_control.window_chat_friend_start_idx=0;
	game_control.window_chat_friend_end_idx=0;
	game_control.window_chat_temp_start_idx=0;
	game_control.window_chat_temp_end_idx=0;
	game_control.window_chat_mask_start_idx=0;
	game_control.window_chat_mask_end_idx=0;
	chat_group_send_start_idx=0;

	for(i=0;i<MAX_CHAT_FRIEND_LIST;i++)
	{
		if(chat_friend_list[i].level<0)continue;
		if(chat_friend_list[i].online==0)continue;
		switch(chat_friend_list[i].friend_type)
		{
		case FRIEND_NORMAL:					// һ�����
			memcpy(&display_friend_list[game_control.window_chat_friend_end_idx],
				&chat_friend_list[i],sizeof( struct CHAT_FRIEND_LIST_STRUCT));
			game_control.window_chat_friend_end_idx++;

			break;
		case FRIEND_TEMP:					// ��ʱ����
			memcpy(&display_temp_list[game_control.window_chat_temp_end_idx],
				&chat_friend_list[i],sizeof( struct CHAT_FRIEND_LIST_STRUCT));
			game_control.window_chat_temp_end_idx++;
			break;
		case FRIEND_MASK:					// ������
			memcpy(&display_mask_list[game_control.window_chat_mask_end_idx],
				&chat_friend_list[i],sizeof( struct CHAT_FRIEND_LIST_STRUCT));
			game_control.window_chat_mask_end_idx++;
			break;
		}

	}


	for(i=0;i<MAX_CHAT_FRIEND_LIST;i++)
	{
		if(chat_friend_list[i].level<0)continue;
		if(chat_friend_list[i].online==1)continue;
		switch(chat_friend_list[i].friend_type)
		{
		case FRIEND_NORMAL:					// һ�����
			memcpy(&display_friend_list[game_control.window_chat_friend_end_idx],
				&chat_friend_list[i],sizeof( struct CHAT_FRIEND_LIST_STRUCT));
			game_control.window_chat_friend_end_idx++;
			break;
		case FRIEND_TEMP:					// ��ʱ����
			memcpy(&display_temp_list[game_control.window_chat_temp_end_idx],
				&chat_friend_list[i],sizeof( struct CHAT_FRIEND_LIST_STRUCT));
			game_control.window_chat_temp_end_idx++;
			break;
		case FRIEND_MASK:					// ������
			memcpy(&display_mask_list[game_control.window_chat_mask_end_idx],
				&chat_friend_list[i],sizeof( struct CHAT_FRIEND_LIST_STRUCT));
			game_control.window_chat_mask_end_idx++;
			break;
		}
		
	}
	


}


void update_client_config(void)
{

	client_config.fight_flag=system_config.fight_flag;			// �д迪�� ( 0 -> �ر�, 1 -> �д�� )
	client_config.mail_flag=system_config.mail_flag;			// �ż����� ( 0 -> �ر�, 1 -> �ż����ܴ� )
	client_config.item_flag=system_config.item_flag;			// ��Ʒ���� ( 0 -> �ر�, 1 -> ��Ʒ���ܴ� )
	client_config.friend_flag=system_config.friend_flag;		// ���ѿ��� ( 0 -> �ر�, 1 -> ������Ѵ� )
	client_config.team_flag=system_config.team_flag;			// ��ӿ��� ( 0 -> �ر�, 1 -> ������Ӽ��� )
	client_config.strange_flag=system_config.strange_flag;		// İ���˿��� ( 0 -> �ر�, 1 -> �ܾ�İ������Ϣ )


	client_config.channel_screen_flag=system_config.channel_screen_flag;	// ��ǰƵ������
	client_config.channel_team_flag=system_config.channel_team_flag;		// ����Ƶ������
	client_config.channel_person_flag=system_config.channel_person_flag;	// ˽��Ƶ������
	client_config.channel_group_flag=system_config.channel_group_flag;		// ����Ƶ������
	client_config.channel_sellbuy_flag=system_config.channel_sellbuy_flag;	// ����Ƶ������
	client_config.channel_world_flag=system_config.channel_world_flag;		// ����Ƶ������
	

	
	postNetMessage(NET_MSG_PLAYER_CLIENT_CONFIG, 0, (char*)&client_config, sizeof(client_config));							

	PostChatNetMessage(NET_MSG_PLAYER_CLIENT_CONFIG, 0, (char*)&client_config, sizeof(client_config));	
}


 

SLONG initialize_chat_window(void)
{
	if(system_config.window_mode==0)
	{
		game_control.chat_window_type=0;
		getChatWindow()->setChatWindowState(true);

	}
	else
	{
		game_control.chat_window_type=1;
		getChatWindow()->setChatWindowState(false);
	}

	return(TTN_OK);
}



void update_chat_window(void)
{
	if(game_control.chat_window_type==0)
	{
		getChatWindow()->setChatWindowState(true);
	}
	else
	{
		getChatWindow()->setChatWindowState(false);
	}
}


void clear_chat_receive_data(void)
{
	chat_receive_data.channel=CHAT_CHANNEL_FRIEND;
	chat_receive_data.nickname[0]=NULL;
	chat_receive_data.text[0]=NULL;
	chat_receive_data.user_id=0;

}


SLONG check_chat_group_send_max(void)
{
	SLONG i;
	SLONG count;		

	count=0;
	for(i=0;i<MAX_CHAT_FRIEND_LIST;i++)
	{
		if(chat_group_send_flag[i])
			count++;
	}
	if(count>8)
	{
		display_system_message((UCHR *)MSG_MAX_FRIEND_GROUP_SEND);
		return(TTN_NOT_OK);
	}
	return(TTN_OK);
}




void copy_chat_data_to_hirstory(void)
{
	SLONG i,j;


	hirstory_chat_data_buffer.bottom_index=0;
	hirstory_chat_data_buffer.top_index=0;
	hirstory_chat_data_buffer.display_index=0;

	for(i=0,j=0;i<MAX_CHAT_RECORD;i++)
	{
		if(chat_data_buffer.record[i].channel==(ULONG)hirstory_channel)
		{
			memcpy(&hirstory_chat_data_buffer.record[j],&chat_data_buffer.record[i],sizeof(struct CHAT_RECORD_STRUCT));
			j++;
			hirstory_chat_data_buffer.top_index=j;
			hirstory_chat_data_buffer.display_index=hirstory_chat_data_buffer.top_index;
		}
	}


}


void clear_friend_data_record(void)

{
	SLONG i;

	friend_data_record_top_index=0;
	friend_data_record_bottom_index=0;
	
	for(i=0;i<MAX_FRIEND_DATA_RECORD;i++)
	{
		friend_data_record[i].send_id=0;
		friend_data_record[i].receive_id=0;
		friend_data_record[i].nickname[0]=NULL;
		friend_data_record[i].time=0;
		friend_data_record[i].text[0]=NULL;
	}

	
}



SLONG initialize_start_up_data(void)
{
	SLONG i;
	SLONG real_zoom;
	float size_zoom_step;
	
// --- ( 0 - 0 ) chat search data 
	chat_search_data.char_id=0|NPC_ID_MASK;
	chat_search_data.level=0;
	chat_search_data.nickname[0]=NULL;
	chat_search_data.phyle=0;
	chat_search_data.sex=0;

// --- ( 0 - 1 ) initialize small map data
	small_map_data.image_buffer=create_bitmap(SMALL_MAP_WIDTH,SMALL_MAP_HEIGHT);
	small_map_data.x_percent=100;
	small_map_data.y_percent=100;


// --- ( 0 - 2 ) Initialize baobao data
	base_baobao_data.display_handle=-1;
	detail_item_inst.handle=-1;


// --- ( 0 - 3 ) Initialize Fight Image
	memset(&fight_image,0,sizeof(struct FIGHT_IMAGE_STRUCT));

	fight_image.fight_item_handle=-1;				// fight item handle
	fight_image.number0_handle=-1;					// ��ɫ ����
	fight_image.number1_handle=-1;					// ��ɫ С��
	fight_image.number2_handle=-1;					// 
	fight_image.double_handle=-1;					// ���� MDA
	fight_image.fullmiss_handle=-1;					// status MDA
	fight_image.fight_hit_handle=-1;
	

	fight_image.status_power_handle=-1;				// ���� MDA
	fight_image.status_chaos_handle=-1;				// ���� MDA
	fight_image.status_sleep_handle=-1;				// ��˯ MDA
	fight_image.status_plague_handle=-1;				// ���� MDA
	fight_image.status_dark_handle=-1;				// �ڰ� MDA
	fight_image.status_delay_handle=-1;				// �ٻ� MDA
	fight_image.status_stone_handle=-1;				// ʯ�� MDA
	fight_image.status_fastness_handle=-1;			// �Ό MDA
	fight_image.status_speed_handle=-1;				// ���� MDA
	

	
// --- ( 0 - 4 ) Initialize camera zoom data
	size_zoom_step=(float)150/MAX_CAMERA_ZOOM;
	for(i=0;i<MAX_CAMERA_ZOOM;i++)
	{
		real_zoom=50+(SLONG)(i*size_zoom_step);
		camera_zoom[i].size_zoom=real_zoom;
		camera_zoom[i].width=80000/real_zoom;
		camera_zoom[i].height=camera_zoom[i].width*9/16;
		camera_zoom[i].x_pos_zoom=(SLONG)(((float)800/(float)camera_zoom[i].width)*1000);
		camera_zoom[i].y_pos_zoom=(SLONG)(((float)450/(float)camera_zoom[i].height)*1000);
	}
	

// --- ( 0 - 5 ) fight npc
	clear_fight_npc_group();
	

// --- ( 9 - 0 ) Initialize routinue task
	main_loop_active_task=NULL;
	ask_yes_exec_task=NULL;
	ask_no_exec_task=NULL;
	

	return(TTN_OK);

}



void clear_team_list_data(void)
{
	SLONG i;

	for(i=0;i<MAX_TEAM_PLAYER;i++)
	{
		team_list_data[i].char_id=0|NPC_ID_MASK;		
	}

	team_select_index=-1;
	
}


void insert_team_list_data(ULONG id, BASE_CHARACTER_INFO *pInfo)
{
	SLONG i;
	SLONG src_idx,target_idx;
	
	src_idx=-1;
	target_idx=-1;
	for(i=0;i<MAX_TEAM_PLAYER;i++)
	{
		if ((ULONG)team_list_data[i].char_id & NPC_ID_MASK)
		{
			src_idx=i;
			break;
		}
	}

	if(src_idx<0)return;

	memcpy(&team_list_data[src_idx], pInfo, sizeof(BASE_CHARACTER_INFO));	

/*	for(i=0;i<MAX_TEAM_REQUEST;i++)
	{
		if ((ULONG)team_request_list_data[i].char_id == id)
		{
			target_idx=i;
			memcpy(&team_list_data[src_idx],&team_request_list_data[target_idx],sizeof(BASE_CHARACTER_INFO));
			break;
		}
	}
*/	

}


void clear_team_request_list_data(void)
{
	SLONG i;
	
	for(i=0;i<MAX_TEAM_REQUEST;i++)
	{
		team_request_list_data[i].char_id=0|NPC_ID_MASK;		
	}

	team_request_select_index=-1;
	team_request_select_start_index=0;
	
}



void insert_team_request_list_data(BASE_CHARACTER_INFO *pInfo)
{
	SLONG i;
	
	for(i=0;i<MAX_TEAM_REQUEST;i++)
	{
		if (team_request_list_data[i].char_id & NPC_ID_MASK)
		{
			Q_memcpy(team_request_list_data+i, pInfo, sizeof(*pInfo));

			break;
		}
	}
}


void delete_team_request_list_data(ULONG id)
{
	SLONG i,j;
	BASE_CHARACTER_INFO temp_data;
	
	for(i=0;i<MAX_TEAM_REQUEST;i++)
	{
		if (team_request_list_data[i].char_id == id)
		{
			team_request_list_data[i].char_id=0|NPC_ID_MASK;
			
			break;
		}
	}

// --- sort  list
	
	for(i=0;i<MAX_TEAM_REQUEST-1;i++)
	{
		for(j=0;j<MAX_TEAM_REQUEST-1;j++)
		{
			if(team_request_list_data[j].char_id&NPC_ID_MASK)
			{
			memcpy(&temp_data,&team_request_list_data[j],sizeof(BASE_CHARACTER_INFO));
			memcpy(&team_request_list_data[j],&team_request_list_data[j+1],sizeof(BASE_CHARACTER_INFO));
			memcpy(&team_request_list_data[j+1],&temp_data,sizeof(BASE_CHARACTER_INFO));
			}
		}
	}
	
	
	



}


void delete_team_list_data(ULONG id)
{
	SLONG i,j;
	BASE_CHARACTER_INFO temp_data;
	
	for(i=0;i<MAX_TEAM_PLAYER;i++)
	{
		if (team_list_data[i].char_id == id)
		{
			team_list_data[i].char_id=0|NPC_ID_MASK;

			sprintf((char *)print_rec,MSG_LEAVE_TEAM,team_list_data[i].nickname);
			push_chat_data(CHAT_CHANNEL_MESSGAE,0,(UCHR *)"",print_rec);

			break;
		}
	}

	for(i=0;i<MAX_TEAM_PLAYER-1;i++)
	{
		for(j=0;j<MAX_TEAM_PLAYER-1;j++)
		{
			if(team_list_data[j].char_id&NPC_ID_MASK)
			{
				memcpy(&temp_data,&team_list_data[j],sizeof(BASE_CHARACTER_INFO));
				memcpy(&team_list_data[j],&team_list_data[j+1],sizeof(BASE_CHARACTER_INFO));
				memcpy(&team_list_data[j+1],&temp_data,sizeof(BASE_CHARACTER_INFO));
			}
		}
	}
	

}



void clear_mission_data(void)
{
	SLONG i;

	for(i=0;i<MAX_MISSION;i++)
	{
		mission_data[i].mission_id=-1;
	}

	total_mission_data=0;
	mission_data_start_index=0;
}



void insert_mission_data(MISSION_DATA data)
{
	SLONG i,idx;

	idx=-1;
	for(i=0;i<MAX_MISSION;i++)
	{
		if(mission_data[i].mission_id<0)
		{
			idx=i;
			break;
		}
	}
	if(idx<0)
	{
		log_error(0,(UCHR *)"Mission data overflow !");
		return;			// Error Mission too much
	}

	total_mission_data=idx+1;
	memcpy(&mission_data[idx],&data,sizeof(struct MISSION_DATA_STRUCT));

}



/*******************************************************************************************************************}
{																													}
{	�� �� �� ֵ �� ��																								}
{																													}
********************************************************************************************************************/
void clear_all_main_loop_data(void)
{
	SLONG i;
	SLONG j;

	game_control.npc_display_flag=0;
	game_control.map_zoom_index=0;

// --- ( 0 - 0 ) Clear character base data
	base_character_data.active=false;
	base_character_data.data.final.level=0;					// �ȼ�
	base_character_data.data.group[0];						// ����
	base_character_data.data.other.credit=0;				// ����
	base_character_data.data.other.dead_count=0;			// ��ѫ
	base_character_data.data.other.kind=0;					// �ƶ�
	base_character_data.data.other.military=0;				// ս��
	base_character_data.data.final.now_exp=0;				// ��ǰ����
	base_character_data.data.final.need_exp=0;				// ��������
	base_character_data.data.base.point=0;					// �ɷ������
	base_character_data.data.final.max_hp=10;				// Ѫ
	base_character_data.data.final.now_hp=10;				// Ѫ
	base_character_data.data.base.hp_point=0;				// Ѫ �������

	base_character_data.data.final.max_mp=10;				// ����
	base_character_data.data.final.now_mp=10;
	base_character_data.data.base.mp_point=0;				// �����������
	base_character_data.data.final.att=0;					// ������
	base_character_data.data.base.att_point=0;				// �������������
	base_character_data.data.final.speed=0;					// �ٶ�
	base_character_data.data.base.speed_point=0;			// �ٶȷ������

	character_point_back.total_point=0;
	character_point_back.hp_point=0;
	character_point_back.mp_point=0;
	character_point_back.att_point=0;
	character_point_back.speed_point=0;


	for(i=0;i<MAX_CHARACTER_SKILL_NO;i++)
	{
		character_skill_data[i].idx=-1;
		character_skill_data[i].level=0;
		character_skill_data[i].lithhand=0;
		character_skill_data[i].action=1;	
		character_skill_data[i].need_mp=0;
	}
	game_control.character_skill_ready=false;
	game_control.baobao_skill_ready=false;	// ���� skill data ready
	game_control.baobao_skill_index=-1;
	game_control.skill_select_index=-1;
	game_control.skill_start_index=0;

// --- ( 0 - 1 ) Clear baobao base data
	base_baobao_data.active=false;
	base_baobao_data.data.active_idx=-1;

	total_baobao_stack=0;
	top_baobao_stack=0;
	baobao_stack_index=-1;

	
	for(i=0;i<MAX_BAOBAO_STACK;i++)
	{
		base_baobao_data.data.show[i]=false;
		base_baobao_data.data.baobao_list[i].baobao_id=0;
		base_baobao_data.data.baobao_list[i].closed=0;
		base_baobao_data.data.baobao_list[i].loyalty=0;
		base_baobao_data.data.baobao_list[i].name[0]=NULL;
		base_baobao_data.data.baobao_list[i].data.att=0;
		base_baobao_data.data.baobao_list[i].data.level=0;
		base_baobao_data.data.baobao_list[i].data.max_hp=10;
		base_baobao_data.data.baobao_list[i].data.max_mp=10;
		base_baobao_data.data.baobao_list[i].data.need_exp=0;
		base_baobao_data.data.baobao_list[i].data.now_exp=0;
		base_baobao_data.data.baobao_list[i].data.now_hp=0;
		base_baobao_data.data.baobao_list[i].data.now_mp=0;
		base_baobao_data.data.baobao_list[i].data.speed=0;


		baobao_point_back[i].total_point=0;
		baobao_point_back[i].hp_point=0;
		baobao_point_back[i].mp_point=0;
		baobao_point_back[i].att_point=0;
		baobao_point_back[i].speed_point=0;
		
	}

	for(j=0;j<MAX_BAOBAO_SKILL_NO;j++)
		baobao_skill_data[j].skill_id=-1;


// --- ( 0 - 2 ) Clear character fastness data
	memset(&character_fastness_data,0,sizeof( struct SERVER_CHARACTER_FASTNESS_STRUCT ));
	character_fastness_data.active=false;


// --- ( 0 - 3 ) Clear character fastness data
	for(i=0;i<MAX_BAOBAO_STACK;i++)
	{
		memset(&baobao_fastness_data[i].fastness,0,sizeof( struct SERVER_BAOBAO_FASTNESS_STRUCT ));
		baobao_fastness_data[i].active=false;
	}
	

// --- ( 0 - 4 ) Clear character pocket 
	character_pocket_data.active=false;
	character_pocket_data.idx=-1;
	for(i=0;i<MAX_POCKET_ITEM;i++)
	{
		character_pocket_data.item[i].filename_id=-1;
		character_pocket_data.item[i].item_id=-1;
		character_pocket_data.item[i].number=0;
		character_pocket_data.item[i].single_price=0;
	}

// --- ( 0 - 5 ) Clear EQUIP data
	character_equip_data.active=false;
	character_equip_data.idx=-1;
	for(i=0;i<MAX_EQUIP_ITEM;i++)
	{
		character_equip_data.equip[i].filename_id=-1;
		character_equip_data.equip[i].item_id=-1;
		character_equip_data.equip[i].number=0;
	}


// --- ( 0 - 6 ) Clear bank data
	character_bank_data.active=false;
	character_bank_data.bank_money=0;
	character_bank_data.max_money=0;
	character_bank_data.now_money=0;


// --- ( 0 - 7 ) Clear popshop data
	character_popshop_data.active=false;
	character_popshop_data.idx=-1;
	for(i=0;i<MAX_POPSHOP_ITEM;i++)
	{
		character_popshop_data.item[i].filename_id=-1;
		character_popshop_data.item[i].item_id=-1;
		character_popshop_data.item[i].number=0;
	}

	
// --- ( 0 - 8 ) 
	clear_all_screen_text();			// ���ĿǰӫĻ����
	clear_message_stack();
	clear_friend_data_record();
		
	game_control.chat_message_in=0;					// clear message in 
	game_control.team_message_in=0;					// clear team message in
	game_control.deal_type=0;						// Ĭ�Ͻ�����Ʒ
	clear_chat_receive_data();
	clear_friend_data_record();
	initialize_chat_data_buffer();
		


// --- ( 0 - 9 ) �����Ϸ�淨����
	game_control.game_give_id=0;			// ��Ҹ����˵� ID
	game_control.game_attack_id=0;			// ��ҹ����� ID
	game_control.game_deal_id=0;			// ����ҽ��׵� ID
	game_control.character_skill_ready=false;	// ��� shill data ready 
	game_control.baobao_skill_ready=false;	// ���� skill data ready
	game_control.baobao_skill_index=-1;

	detail_item_inst.active=false;			
	detail_item_inst.idx=-1;
	detail_item_inst.old_idx=-1;
	detail_item_inst.type=0;
	detail_item_inst.inst[0]=NULL;
	
// --- ( 1 - 0 ) Fight
	clear_fight_npc_group();
		

	fight_control_data.character_ai_type=-1;			// ����Զ������� AI
	fight_control_data.baobao_ai_type=-1;				// �����Զ������� AI
	


}




