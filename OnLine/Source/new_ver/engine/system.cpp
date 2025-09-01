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
{	系统启动初始化																		}
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
{	读取系统设定档案, 如果没有系统设定档案,则自动产生									}
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
	system_config.window_mode=0;	// 视窗模式
	system_config.music_flag=1;		// 音乐开启
	system_config.voice_flag=1;		// 音效开启
	system_config.volume=100;		// 音乐音效音量 ( 0 -> 100 )
	system_config.record_flag=0;	// 游戏录像开关 ( 正常 )
	system_config.fight_flag=1;		// 切磋开关 ( 关闭 )
	system_config.mail_flag=1;		// 信件接受 ( 信件接受打开 )
	system_config.item_flag=1;		// 物品接受 ( 物品接受打开 )
	system_config.friend_flag=1;	// 好友开关 ( 加入好友打开 )
	system_config.team_flag=1;		// 组队开关 ( 允许组队加入 )
//	system_config.map_effect=1;		// 地图特效 ( 地图特效打开 )
	system_config.head_talk=1;		// 头顶对话开关 ( 允许头顶对话显示 )
	system_config.strange_flag=0;	// 陌生人开关 ( 关闭 )
	system_config.quick_menu=1;		// 右键选单开关 ( 开启右键选单 )
	system_config.clothes_flag=1;	// 装备显示开关 ( 显示装备价值 )
// ---
	system_config.default_server_id=-1;
// --- chat
	system_config.channel_screen_flag=1;	// 当前频道开关
	system_config.channel_team_flag=1;		// 队伍频道开关
	system_config.channel_person_flag=1;	// 私聊频道开关
	system_config.channel_group_flag=1;		// 工会频道开关
	system_config.channel_sellbuy_flag=1;	// 经济频道开关
	system_config.channel_world_flag=1;		// 世界频道开关

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
	game_control.main_loop_task=MAIN_TASK_MOVE;		// 目前主回路状态 
	game_control.main_loop_break=false;				// 是否跳出主回路
	game_control.main_loop_ret_val=RETURN_OK;		// main_loop 回传值
	game_control.main_loop_select_npc_idx=-1;		// 主回路选择NPC idx
	game_control.main_loop_select_npc_task=-1;		// 主回路选择NPC 的task

	game_control.main_loop_mode=MAIN_LOOP_MAP_MODE;	// 默认进入地图模式

// --- 天气系统
	game_control.weather_enable=false;				// 目前地图是否执行天气系统
	game_control.weather_timer=0;
	game_control.weather_add_timer=0;
	game_control.weather_change_timer=0;

// --- 聊天
	game_control.chat_channel=CHAT_CHANNEL_SCREEN;	// 当前聊天频道
	game_control.chat_timer=0;
	game_control.chat_message_in=0;

// --- main_loop 开启的 window 程序控制
	game_control.window_channel=-1;					// 频道切换 window 的 handle
	game_control.old_window_task=-1;
	game_control.chat_stack_index=0;				// 目前 stack index
	game_control.chat_stack_find_index=0;			// 
	for(i=0;i<MAX_CHAT_STACK;i++)
	{
		strcpy((char *)game_control.chat_stack_buffer[MAX_CHAT_STACK],"");
	}
	game_control.chat_window_type=0;				// 聊天输出 0 -> window  1-> 内部
	game_control.chat_pause_flag=0;
	
// --- system window
	game_control.window_system=-1;
	game_control.window_system_button_idx=-1;


// --- quit window
	game_control.window_quit=-1;
	game_control.window_quit_button_idx=-1;
	

// --- 聊天器
	game_control.window_chat=-1;
	game_control.window_chat_type=0;

// --- 表情符号
	game_control.window_phiz=-1;

// --- 人物状态
	game_control.window_character_status=-1;

// --- 宝宝状态
	game_control.window_baobao_status=-1;

// --- 历史资料
	game_control.window_history=-1;

// --- 道具物品
	game_control.window_item=-1;

// --- 任务
	game_control.window_mission=-1;

// --- 公会
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
	game_control.window_fight_baobao_change=-1;	// 战斗宝宝更换
	
// --- fight talk window 
	game_control.window_fight_talk=-1;			// 战斗快速留言
	

	
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
		{  MOUSE_IMG_STAND	,  4,  3	},		// 主鼠标
		{  MOUSE_IMG_ERROR	, 19,  21	},		// 点击不能
		{  MOUSE_IMG_TEAM	, 18,  33	},		// 组队	
		{  MOUSE_IMG_ATTACK	,  3,  2	},		// 攻击
		{  MOUSE_IMG_GIVE	,  1,  2	},		// 给予
		{  MOUSE_IMG_DEAL	,  1,  1	},		// 交易
		{  MOUSE_IMG_FRIEND	,  6,  3	},		// 加好友		
		{  MOUSE_IMG_ITEM	,  7,  23	},		// 使用物品
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
	
// --------- ( 0 - 2 ) 读取 
	if(read_mda_frame((UCHR *)"system\\c1meu018a.mda",0,&system_image.status_body.image)!=TTN_OK)		// 人物与宝宝主体		0
		return(TTN_NOT_OK);
	get_img256_rect(system_image.status_body.image,&system_image.status_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\c1meu018a.mda",1,&system_image.map_body.image)!=TTN_OK)			// 小地图主体			1
		return(TTN_NOT_OK);
	get_img256_rect(system_image.map_body.image,&system_image.map_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\c1meu018a.mda",2,&system_image.map_main_body.image)!=TTN_OK)		// 地图主界面主体		2	
		return(TTN_NOT_OK);
	get_img256_rect(system_image.map_main_body.image,&system_image.map_main_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\c1meu018a.mda",3,&system_image.channel_body.image)!=TTN_OK)		// 频道选择主体			3
		return(TTN_NOT_OK);
	get_img256_rect(system_image.channel_body.image,&system_image.channel_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\c1meu018a.mda",4,&system_image.face_font.image)!=TTN_OK)			// 头像外框				4
		return(TTN_NOT_OK);
	get_img256_rect(system_image.face_font.image,&system_image.face_font.rect);
	
	if(read_mda_frame((UCHR *)"system\\c1meu018a.mda",5,&system_image.char_hp.image)!=TTN_OK)			// 人物 HP body			5
		return(TTN_NOT_OK);
	get_img256_rect(system_image.char_hp.image,&system_image.char_hp.rect);
	
	if(read_mda_frame((UCHR *)"system\\c1meu018a.mda",6,&system_image.char_mp.image)!=TTN_OK)			// 人物 MP body			6
		return(TTN_NOT_OK);
	get_img256_rect(system_image.char_mp.image,&system_image.char_mp.rect);
	
	if(read_mda_frame((UCHR *)"system\\c1meu018a.mda",7,&system_image.baobao_hp.image)!=TTN_OK)			// 宝宝 HP				7
		return(TTN_NOT_OK);
	get_img256_rect(system_image.baobao_hp.image,&system_image.baobao_hp.rect);
	
	if(read_mda_frame((UCHR *)"system\\c1meu018a.mda",8,&system_image.baobao_exp.image)!=TTN_OK)		// 宝宝 EXP				8
		return(TTN_NOT_OK);
	get_img256_rect(system_image.baobao_exp.image,&system_image.baobao_exp.rect);
	
	if(read_mda_frame((UCHR *)"system\\c1meu018a.mda",9,&system_image.baobao_mp.image)!=TTN_OK)			// 宝宝 HP				9
		return(TTN_NOT_OK);
	get_img256_rect(system_image.baobao_mp.image,&system_image.baobao_mp.rect);
	
	if(read_mda_frame((UCHR *)"system\\c1meu018a.mda",10,&system_image.char_exp.image)!=TTN_OK)			// 人物 exp				10
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
	if(read_mda_frame((UCHR *)"system\\c1meu026.mda",1,&system_image.volume.image )!=TTN_OK)			// 音量大小 1
		return(TTN_NOT_OK);
	get_img256_rect(system_image.volume.image,&system_image.volume.rect);
	
	if(read_mda_button(system_image.system_menu_handle,0,&system_image.system_background,(UCHR *)"",0)!=TTN_OK)	// 底图 0
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.system_menu_handle,2,&system_image.system_close,(UCHR *)MSG_WINDOW_CLOSE,2)!=TTN_OK)	// 关闭按钮 2
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.system_menu_handle,4,&system_image.sub_volume,(UCHR *)MSG_VOLUME_SUB,4)!=TTN_OK)  // 音量降低 4
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.system_menu_handle,6,&system_image.add_volume,(UCHR *)MSG_VOLUME_ADD,6)!=TTN_OK)  // 音量加大 6
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.system_menu_handle,8,&system_image.system_save,(UCHR *)"",8)!=TTN_OK)  // 保存按钮 8
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.system_menu_handle,11,&system_image.system_help,(UCHR *)"",11)!=TTN_OK)  // 游戏帮助 11
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.system_menu_handle,14,&system_image.system_bbs,(UCHR *)"",14)!=TTN_OK)  // 游戏论坛 14
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.system_menu_handle,17,&system_image.system_quit,(UCHR *)"",17)!=TTN_OK)  // 退出游戏 17
		return(TTN_NOT_OK);


// --- quit board
	mda_group_open((UCHR *)"system\\c1meu074.mda",&system_image.quit_menu_handle);
	if(system_image.quit_menu_handle<0)
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.quit_menu_handle,0,&system_image.quit_body,(UCHR *)"",0)!=TTN_OK)	// 底图 0
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.quit_menu_handle,1,&system_image.quit_restart,(UCHR *)"",1)!=TTN_OK)	// 重新开始
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.quit_menu_handle,4,&system_image.quit_quit,(UCHR *)"",4)!=TTN_OK)	// 结束游戏
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.quit_menu_handle,7,&system_image.quit_cancel,(UCHR *)"",7)!=TTN_OK)	// 结束游戏
		return(TTN_NOT_OK);

// ----- 可移动 window body
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",0,&system_image.chat_body.image)!=TTN_OK)			// 聊天器
		return(TTN_NOT_OK);
	get_img256_rect(system_image.chat_body.image,&system_image.chat_body.rect);

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",1,&system_image.character_attrib_body.image)!=TTN_OK)	// 人物属性
		return(TTN_NOT_OK);
	get_img256_rect(system_image.character_attrib_body.image,&system_image.character_attrib_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",2,&system_image.baobao_attrib_body.image)!=TTN_OK)	// 宝宝属性
		return(TTN_NOT_OK);
	get_img256_rect(system_image.baobao_attrib_body.image,&system_image.baobao_attrib_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",3,&system_image.history_body.image)!=TTN_OK)		// 历史资料
		return(TTN_NOT_OK);
	get_img256_rect(system_image.history_body.image,&system_image.history_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",4,&system_image.item_body.image)!=TTN_OK)			// 物品装备
		return(TTN_NOT_OK);
	get_img256_rect(system_image.item_body.image,&system_image.item_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",5,&system_image.team_body.image)!=TTN_OK)			// 组队
		return(TTN_NOT_OK);
	get_img256_rect(system_image.team_body.image,&system_image.team_body.rect);

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",6,&system_image.give_body.image)!=TTN_OK)			// 给与
		return(TTN_NOT_OK);
	get_img256_rect(system_image.give_body.image,&system_image.give_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",7,&system_image.deal_body.image)!=TTN_OK)			// 交易
		return(TTN_NOT_OK);
	get_img256_rect(system_image.deal_body.image,&system_image.deal_body.rect);

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",8,&system_image.mission_body.image)!=TTN_OK)		// 任务
		return(TTN_NOT_OK);
	get_img256_rect(system_image.mission_body.image,&system_image.mission_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",9,&system_image.group_body.image)!=TTN_OK)			// 公会
		return(TTN_NOT_OK);
	get_img256_rect(system_image.group_body.image,&system_image.group_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",10,&system_image.chat_receive_body.image)!=TTN_OK)	// 聊天器 接受
		return(TTN_NOT_OK);
	get_img256_rect(system_image.chat_receive_body.image,&system_image.chat_receive_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",11,&system_image.chat_send_body.image)!=TTN_OK)		// 聊天器 发送
		return(TTN_NOT_OK);
	get_img256_rect(system_image.chat_send_body.image,&system_image.chat_send_body.rect);

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",12,&system_image.chat_setup.image)!=TTN_OK)			// 聊天器 setup
		return(TTN_NOT_OK);
	get_img256_rect(system_image.chat_setup.image,&system_image.chat_setup.rect);
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",13,&system_image.chat_search.image)!=TTN_OK)		// 聊天器 search
		return(TTN_NOT_OK);
	get_img256_rect(system_image.chat_search.image,&system_image.chat_search.rect);
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",14,&system_image.chat_attrib_body.image)!=TTN_OK)		// 聊天器 属性
		return(TTN_NOT_OK);
	get_img256_rect(system_image.chat_attrib_body.image,&system_image.chat_attrib_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",15,&system_image.chat_group_send_body.image)!=TTN_OK)		// 聊天器 群发
		return(TTN_NOT_OK);
	get_img256_rect(system_image.chat_group_send_body.image,&system_image.chat_group_send_body.rect);
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",16,&system_image.chat_hirstory_mask.image)!=TTN_OK)		// 聊天器历史资料 mask
		return(TTN_NOT_OK);
	get_img256_rect(system_image.chat_hirstory_mask.image,&system_image.chat_hirstory_mask.rect);
	



// ------------- 所有按钮 --------------
	mda_group_open((UCHR *)"system\\mapbutton.mda",&system_image.button_handle);
	if(system_image.button_handle<0)
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.button_handle,0,&system_image.button_chat_select,(UCHR *)"",0)!=TTN_OK)	// 人名选中框
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.button_handle,1,&system_image.button_window_close,(UCHR *)"",1)!=TTN_OK)	// window 关闭
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.button_handle,3,&system_image.button_add,(UCHR *)"",3)!=TTN_OK)		// 加号
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.button_handle,5,&system_image.button_sub,(UCHR *)"",5)!=TTN_OK)		// 减号
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.button_handle,7,&system_image.button_up,(UCHR *)"",7)!=TTN_OK)		// 上
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.button_handle,9,&system_image.button_down,(UCHR *)"",9)!=TTN_OK)	// 下
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.button_handle,11,&system_image.button_check,(UCHR *)"",11)!=TTN_OK)	// 打勾
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.button_handle,12,&system_image.button_chat_friend,(UCHR *)"",12)!=TTN_OK)	// 好友
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.button_handle,15,&system_image.button_chat_temp,(UCHR *)"",15)!=TTN_OK)	// 临时好友
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.button_handle,18,&system_image.button_chat_mask,(UCHR *)"",18)!=TTN_OK)	// 屏蔽好友
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.button_handle,21,&system_image.button_chat_auto_answer,(UCHR *)"",21)!=TTN_OK)	// 自动回复
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.button_handle,23,&system_image.button_chat_search,(UCHR *)"",23)!=TTN_OK)	// 搜寻好友
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.button_handle,25,&system_image.button_chat_add_friend,(UCHR *)"",25)!=TTN_OK)	// 加为好友
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.button_handle,27,&system_image.button_ok,(UCHR *)"",27)!=TTN_OK)	// 确定
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.button_handle,29,&system_image.button_cancel,(UCHR *)"",29)!=TTN_OK)	// 取消
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.button_handle,31,&system_image.button_chat_attr_friend,(UCHR *)"",31)!=TTN_OK)	// 加为好友
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.button_handle,33,&system_image.button_chat_attr_temp,(UCHR *)"",33)!=TTN_OK)	// 加为临时好友
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.button_handle,35,&system_image.button_chat_person,(UCHR *)"",35)!=TTN_OK)	// 私聊
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.button_handle,37,&system_image.button_chat_cancel,(UCHR *)"",37)!=TTN_OK)	// 断交
		return(TTN_NOT_OK);

	if(read_mda_button(system_image.button_handle,39,&system_image.button_chat_attr_mask,(UCHR *)"",39)!=TTN_OK)		// 屏蔽
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.button_handle,41,&system_image.button_chat_refresh,(UCHR *)"",41)!=TTN_OK)	// 更新
		return(TTN_NOT_OK);
	
	if(read_mda_button(system_image.button_handle,43,&system_image.button_chat_hirstory,(UCHR *)"",43)!=TTN_OK)	// 历史信件
		return(TTN_NOT_OK);
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",17,&system_image.team_list_mark.image)!=TTN_OK)		// 队伍标题
		return(TTN_NOT_OK);
	get_img256_rect(system_image.team_list_mark.image,&system_image.team_list_mark.rect);


	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",18,&system_image.team_list_request_mark.image)!=TTN_OK)		// 请求列表标题
		return(TTN_NOT_OK);
	get_img256_rect(system_image.team_list_request_mark.image,&system_image.team_list_request_mark.rect);

	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",19,&system_image.conform_body.image)!=TTN_OK)		// 确认 body
		return(TTN_NOT_OK);
	get_img256_rect(system_image.conform_body.image,&system_image.conform_body.rect);

	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",20,&system_image.baobao_item_action.image)!=TTN_OK)		// 20 宝宝物品动作部分	
		return(TTN_NOT_OK);
	get_img256_rect(system_image.baobao_item_action.image,&system_image.baobao_item_action.rect);
	

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",21,&system_image.skill_body.image)!=TTN_OK)			// 技能查看
		return(TTN_NOT_OK);
	get_img256_rect(system_image.skill_body.image,&system_image.skill_body.rect);
	

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",22,&system_image.title_body.image)!=TTN_OK)			// 头衔更换
		return(TTN_NOT_OK);
	get_img256_rect(system_image.title_body.image,&system_image.title_body.rect);
	

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",23,&system_image.mc_window_body.image)!=TTN_OK)		// Macro window body
		return(TTN_NOT_OK);
	get_img256_rect(system_image.mc_window_body.image,&system_image.mc_window_body.rect);
	

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",24,&system_image.item_public_body.image)!=TTN_OK)		// 物品栏公用部分
		return(TTN_NOT_OK);
	get_img256_rect(system_image.item_public_body.image,&system_image.item_public_body.rect);

	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",25,&system_image.item_action_body.image)!=TTN_OK)		// 给与动作部分
		return(TTN_NOT_OK);
	get_img256_rect(system_image.item_action_body.image,&system_image.item_action_body.rect);

	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",26,&system_image.deal_action_body.image)!=TTN_OK)		// 交易动作部分
		return(TTN_NOT_OK);
	get_img256_rect(system_image.deal_action_body.image,&system_image.deal_action_body.rect);
	

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",27,&system_image.store_body.image)!=TTN_OK)		// 商店body
		return(TTN_NOT_OK);
	get_img256_rect(system_image.store_body.image,&system_image.store_body.rect);
	

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",28,&system_image.store_sell_mark.image)!=TTN_OK)		// 商店卖mask
		return(TTN_NOT_OK);
	get_img256_rect(system_image.store_sell_mark.image,&system_image.store_sell_mark.rect);
	

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",29,&system_image.store_buy_mark.image)!=TTN_OK)		// 商店买mask
		return(TTN_NOT_OK);
	get_img256_rect(system_image.store_buy_mark.image,&system_image.store_buy_mark.rect);
	

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",30,&system_image.store_sellmake.image)!=TTN_OK)		// 商店寄卖mask
		return(TTN_NOT_OK);
	get_img256_rect(system_image.store_sellmake.image,&system_image.store_sellmake.rect);
	

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",31,&system_image.baobao_icon.image)!=TTN_OK)		// 交易宝宝 icon
		return(TTN_NOT_OK);
	get_img256_rect(system_image.baobao_icon.image,&system_image.baobao_icon.rect);
	

	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",32,&system_image.baobao_item_mask.image)!=TTN_OK)		// 32 BAOBAO 使用物品 mark
		return(TTN_NOT_OK);
	get_img256_rect(system_image.baobao_item_mask.image,&system_image.baobao_item_mask.rect);

	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",33,&system_image.bank_body.image)!=TTN_OK)		// 33 BANK BODY
		return(TTN_NOT_OK);
	get_img256_rect(system_image.bank_body.image,&system_image.bank_body.rect);
	
	
	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",34,&system_image.popshop_push_mark.image)!=TTN_OK)		// 34 当铺储存 mark
		return(TTN_NOT_OK);
	get_img256_rect(system_image.popshop_push_mark.image,&system_image.popshop_push_mark.rect);


	if(read_mda_frame((UCHR *)"system\\mapmenu.mda",35,&system_image.popshop_pop_mark.image)!=TTN_OK)		// 35 当铺取出 mark
		return(TTN_NOT_OK);
	get_img256_rect(system_image.popshop_pop_mark.image,&system_image.popshop_pop_mark.rect);
	


// --- Item image 
	for(i=0;i<MAX_ITEM_IMAGE;i++)
	{
		system_image.item_image[i]=NULL;
		if(read_mda_frame((UCHR *)"system\\item.mda",i,&system_image.item_image[i])!=TTN_OK)		//  物品 image
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
		


// --- 设定主要输入框(地图聊天)
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
	map_base_data.cell_width=16;		// CELL寬 
	map_base_data.cell_height=8;		// CELL高 
	map_base_data.map_width=800;		// 地圖寬 Pixel
	map_base_data.map_width=600;		// 地圖高 Pixel
	map_base_data.map_angel=0;			// 地圖角度
	map_base_data.total_map_layer=0;	// 总共多少 layer
	map_base_data.map_layer_data=NULL;	// Layer 资料	
// ---
	map_base_data.map_event_data.total_cell=0;	// 总共多少 cell
	map_base_data.map_event_data.max_cell_x=0;
	map_base_data.map_event_data.max_cell_y=0;
	map_base_data.map_event_data.event_data=NULL;  // Event 资料
// ---	
	map_base_data.total_map_link=0;
	map_base_data.map_link_data=NULL;			// 连接资料
// ---
	map_base_data.total_map_effect=0;
	map_base_data.map_effect_data=NULL;			// 音效资料

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
	map_base_data.cell_width=16;		// CELL寬 
	map_base_data.cell_height=8;		// CELL高 
	map_base_data.map_width=800;		// 地圖寬 Pixel
	map_base_data.map_width=600;		// 地圖高 Pixel
	map_base_data.map_angel=0;			// 地圖角度
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
	map_base_data.map_event_data.total_cell=0;	// 总共多少 cell
	map_base_data.map_event_data.max_cell_x=0;
	map_base_data.map_event_data.max_cell_y=0;
	if(map_base_data.map_event_data.event_data!=NULL)
		free(map_base_data.map_event_data.event_data);
	map_base_data.map_event_data.event_data=NULL;  // Event 资料
// --- Link
	map_base_data.total_map_link=0;
	if(map_base_data.map_link_data!=NULL)		// 连接资料
		free(map_base_data.map_link_data);
	map_base_data.map_link_data=NULL;

// --- Effect
	if(map_base_data.map_effect_data!=NULL)		// 音效资料
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


	map_control.character_title_color=color_control.cyan;	// 玩家称谓颜色
	map_control.npc_name_color=color_control.yellow;		// NPC 姓名颜色
	map_control.point_color=color_control.red;				// 指定了的颜色

										
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
		extra_mda_data[i].display_x=0;								// 实际坐标 
		extra_mda_data[i].display_y=0;
		extra_mda_data[i].loop=false;								// 是否 Loop 播放
		extra_mda_data[i].play_end=false;							// 是否已经到最后1祯
		extra_mda_data[i].auto_close=true;							// 是否到最后1祯自动关闭(loop 为 false) 或者更换地图关闭
		extra_mda_data[i].hight_light_flag=false;					// 是否需要 HightLight
		extra_mda_data[i].level=0;									// 等级 0 -> 底层  1-> 中间层  2-> 最上层
		strcpy((char *)extra_mda_data[i].command,"");				// 播放命令
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
		extra_fight_mda_data[i].display_x=0;								// 实际坐标 
		extra_fight_mda_data[i].display_y=0;
		extra_fight_mda_data[i].loop=false;								// 是否 Loop 播放
		extra_fight_mda_data[i].play_end=false;							// 是否已经到最后1祯
		extra_fight_mda_data[i].auto_close=true;							// 是否到最后1祯自动关闭(loop 为 false) 或者更换地图关闭
		extra_fight_mda_data[i].hight_light_flag=false;					// 是否需要 HightLight
		extra_fight_mda_data[i].level=0;									// 等级 0 -> 底层  1-> 中间层  2-> 最上层
		strcpy((char *)extra_fight_mda_data[i].command,"");				// 播放命令
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
{	系统 idle loop																		}
{																						}
{	1. 呼叫 idle_loop();																}
{	2. 取得 system_control.system_timer 做为同步控制									}
{	3. 处理 Task Manger																	}
{	4. 处理 sleep ( CPU 资源 )															}
{	5. 处理 polling list 																}
{	6. 处理 主机讯息																	}
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
	

// --- ( 0 - 0 )  呼叫 Idle_lood
	idle_loop();

// --- ( 0 - 1 ) 取得 system_control.system_timer 做为同步控制
	system_control.system_timer=GetTickCount();

// --- ( 0 - 2 ) 取得玩家 ID & IDX 
	system_control.control_npc_idx=get_map_npc_index(system_control.control_user_id);
	
// --- ( 0 - 3 ) Check MP3 Loop

	if((game_control.music_playing==1)&&(system_config.music_flag==1))		// 需要检查 MP3 Loop
	{
		if(IsIconic(g_hDDWnd))		// 是否最小化
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

// --- ( 0 - 4 ) 读取 鼠标键盘
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



// --- ( 0 - 3 ) 读网络数据	
	updateNetData();



// --- ( 9 - 9 ) 计算idle time

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
		chat_friend_list[i].friend_type=0;					// 一般好友
		chat_friend_list[i].nickname[0]=NULL;				// nicknane
		chat_friend_list[i].title[0]=NULL;					// 称谓
		chat_friend_list[i].level=-1;						// 等级
		chat_friend_list[i].base_type=0;					// 基本形态
		chat_friend_list[i].group[0]=0;						// 公会
		chat_friend_list[i].nexus=0;						// 关系
		chat_friend_list[i].amity=0;						// 友好度

	}

	

// --- add dummy
/*
	for(i=0;i<MAX_CHAT_FRIEND_LIST-10;i++)
	{
		chat_friend_list[i].online=rand()%2;
		chat_friend_list[i].id=0;
		chat_friend_list[i].friend_type=rand()%3;					// 一般好友
		sprintf((char *)chat_friend_list[i].nickname,"NAME%d",i);
		chat_friend_list[i].title[0]=NULL;					// 称谓
		chat_friend_list[i].level=i;						// 等级
		chat_friend_list[i].base_type=0;					// 基本形态
		chat_friend_list[i].group[0]=0;						// 公会
		chat_friend_list[i].nexus=0;						// 关系
		chat_friend_list[i].amity=0;						// 友好度
		
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
		case FRIEND_NORMAL:					// 一般好友
			memcpy(&display_friend_list[game_control.window_chat_friend_end_idx],
				&chat_friend_list[i],sizeof( struct CHAT_FRIEND_LIST_STRUCT));
			game_control.window_chat_friend_end_idx++;

			break;
		case FRIEND_TEMP:					// 临时好友
			memcpy(&display_temp_list[game_control.window_chat_temp_end_idx],
				&chat_friend_list[i],sizeof( struct CHAT_FRIEND_LIST_STRUCT));
			game_control.window_chat_temp_end_idx++;
			break;
		case FRIEND_MASK:					// 黑名单
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
		case FRIEND_NORMAL:					// 一般好友
			memcpy(&display_friend_list[game_control.window_chat_friend_end_idx],
				&chat_friend_list[i],sizeof( struct CHAT_FRIEND_LIST_STRUCT));
			game_control.window_chat_friend_end_idx++;
			break;
		case FRIEND_TEMP:					// 临时好友
			memcpy(&display_temp_list[game_control.window_chat_temp_end_idx],
				&chat_friend_list[i],sizeof( struct CHAT_FRIEND_LIST_STRUCT));
			game_control.window_chat_temp_end_idx++;
			break;
		case FRIEND_MASK:					// 黑名单
			memcpy(&display_mask_list[game_control.window_chat_mask_end_idx],
				&chat_friend_list[i],sizeof( struct CHAT_FRIEND_LIST_STRUCT));
			game_control.window_chat_mask_end_idx++;
			break;
		}
		
	}
	


}


void update_client_config(void)
{

	client_config.fight_flag=system_config.fight_flag;			// 切磋开关 ( 0 -> 关闭, 1 -> 切磋打开 )
	client_config.mail_flag=system_config.mail_flag;			// 信件接受 ( 0 -> 关闭, 1 -> 信件接受打开 )
	client_config.item_flag=system_config.item_flag;			// 物品接受 ( 0 -> 关闭, 1 -> 物品接受打开 )
	client_config.friend_flag=system_config.friend_flag;		// 好友开关 ( 0 -> 关闭, 1 -> 加入好友打开 )
	client_config.team_flag=system_config.team_flag;			// 组队开关 ( 0 -> 关闭, 1 -> 允许组队加入 )
	client_config.strange_flag=system_config.strange_flag;		// 陌生人开关 ( 0 -> 关闭, 1 -> 拒绝陌生人消息 )


	client_config.channel_screen_flag=system_config.channel_screen_flag;	// 当前频道开关
	client_config.channel_team_flag=system_config.channel_team_flag;		// 队伍频道开关
	client_config.channel_person_flag=system_config.channel_person_flag;	// 私聊频道开关
	client_config.channel_group_flag=system_config.channel_group_flag;		// 工会频道开关
	client_config.channel_sellbuy_flag=system_config.channel_sellbuy_flag;	// 经济频道开关
	client_config.channel_world_flag=system_config.channel_world_flag;		// 世界频道开关
	

	
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
	fight_image.number0_handle=-1;					// 绿色 大字
	fight_image.number1_handle=-1;					// 红色 小字
	fight_image.number2_handle=-1;					// 
	fight_image.double_handle=-1;					// 连击 MDA
	fight_image.fullmiss_handle=-1;					// status MDA
	fight_image.fight_hit_handle=-1;
	

	fight_image.status_power_handle=-1;				// 威力 MDA
	fight_image.status_chaos_handle=-1;				// 混乱 MDA
	fight_image.status_sleep_handle=-1;				// 昏睡 MDA
	fight_image.status_plague_handle=-1;				// 瘟疫 MDA
	fight_image.status_dark_handle=-1;				// 黑暗 MDA
	fight_image.status_delay_handle=-1;				// 迟缓 MDA
	fight_image.status_stone_handle=-1;				// 石化 MDA
	fight_image.status_fastness_handle=-1;			// 提抗 MDA
	fight_image.status_speed_handle=-1;				// 加速 MDA
	

	
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
{	人 物 数 值 部 分																								}
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
	base_character_data.data.final.level=0;					// 等级
	base_character_data.data.group[0];						// 工会
	base_character_data.data.other.credit=0;				// 声望
	base_character_data.data.other.dead_count=0;			// 功勋
	base_character_data.data.other.kind=0;					// 善恶
	base_character_data.data.other.military=0;				// 战绩
	base_character_data.data.final.now_exp=0;				// 当前经验
	base_character_data.data.final.need_exp=0;				// 升级经验
	base_character_data.data.base.point=0;					// 可分配点数
	base_character_data.data.final.max_hp=10;				// 血
	base_character_data.data.final.now_hp=10;				// 血
	base_character_data.data.base.hp_point=0;				// 血 分配点数

	base_character_data.data.final.max_mp=10;				// 法力
	base_character_data.data.final.now_mp=10;
	base_character_data.data.base.mp_point=0;				// 法力分配点数
	base_character_data.data.final.att=0;					// 攻击力
	base_character_data.data.base.att_point=0;				// 攻击力分配点数
	base_character_data.data.final.speed=0;					// 速度
	base_character_data.data.base.speed_point=0;			// 速度分配点数

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
	game_control.baobao_skill_ready=false;	// 宝宝 skill data ready
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
	clear_all_screen_text();			// 清除目前荧幕文字
	clear_message_stack();
	clear_friend_data_record();
		
	game_control.chat_message_in=0;					// clear message in 
	game_control.team_message_in=0;					// clear team message in
	game_control.deal_type=0;						// 默认交易物品
	clear_chat_receive_data();
	clear_friend_data_record();
	initialize_chat_data_buffer();
		


// --- ( 0 - 9 ) 清楚游戏玩法部分
	game_control.game_give_id=0;			// 玩家给与人的 ID
	game_control.game_attack_id=0;			// 玩家攻击的 ID
	game_control.game_deal_id=0;			// 与玩家交易的 ID
	game_control.character_skill_ready=false;	// 玩家 shill data ready 
	game_control.baobao_skill_ready=false;	// 宝宝 skill data ready
	game_control.baobao_skill_index=-1;

	detail_item_inst.active=false;			
	detail_item_inst.idx=-1;
	detail_item_inst.old_idx=-1;
	detail_item_inst.type=0;
	detail_item_inst.inst[0]=NULL;
	
// --- ( 1 - 0 ) Fight
	clear_fight_npc_group();
		

	fight_control_data.character_ai_type=-1;			// 玩家自动攻击的 AI
	fight_control_data.baobao_ai_type=-1;				// 宝宝自动攻击的 AI
	


}




