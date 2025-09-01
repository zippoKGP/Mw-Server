/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : fight.cpp
  *owner  		: Stephen
  *description  : 
  *modified     : 2005/3/2
******************************************************************************/ 


#include "engine_global.h"
#include "fight.h"
#include "graph.h"
#include "vari-ext.h"
#include "net.h"
#include "data.h"
#include "jpg.h"
#include "data_proc.h"

#define FIGHT_MAIN_DELAY			40



void change_to_fight(void)			// screen_channel0 -> screen_buffer 
{
	UCHR real_filename[80];
	

	if(read_fight_image()!=TTN_OK)
	{
		display_error_message((UCHR *)MSG_FIGHT_IMAGE_ERROR,1);
		game_control.main_loop_break=true;
		game_control.main_loop_ret_val=MAIN_TASK_RETURN_QUIT;
		disconnectServer();
		return;
	}

	if(!character_pocket_data.active)
		request_character_pocket_data();

	
	clear_fight_command();
	clear_fight_control();
	initializa_display_text();
	clear_all_fight_parameter();		

	sprintf((char *)real_filename,"MUSIC\\%s",change_map_info.fight_music_filename);
	play_music_file(real_filename,1,100);
	
	get_default_position();
	set_camera(fight_default_pos.zoom_index,fight_default_pos.pan_x,fight_default_pos.pan_y);
	
	game_control.main_loop_mode=MAIN_LOOP_FIGHT_MODE;
	fight_camera(screen_channel0);
	draw_main_map(screen_channel0);					// 绘画出地图主menu
	
	change_screen_effect_flc(rand()%9);

	close_all_main_map_window();

	clear_all_map_npc();							// 清除所有 MAP_NPC 的资料

	show_fight_welcome();

	wait_first_time_command();						// 等待第一次命令

	send_fight_ready();								// 传送 FIGHT_READY 给主机

}



void clear_all_fight_parameter(void)
{

// ----- ( 0 - 0 ) Clear fight inst
	fight_inst_data.active=false;
	fight_inst_data.inst[0]=NULL;
	fight_inst_data.status=0;
	fight_inst_data.still_timer=0;
	fight_inst_data.y_pos=-32;

}




void show_fight_welcome(void)
{
	SLONG show_idx;
	SLONG i;
	SLONG total_manchar;
	SLONG main_pass;
	SLONG extra_index;
	SLONG count;
	UCHR npc_filename[80];

	total_manchar=0;
	for(i=0;i<10;i=i+2)				// 只搜寻人
	{
		if(fight_npc_group[i].base.active)		// enable
		{
			total_manchar++;
		}
	}
	
	if(total_manchar<=0)return;

	show_idx=0;						// 默认队长
	for(i=0;i<10;i=i+2)				// 只搜寻人
	{
		if(fight_npc_group[i].base.active)		// enable
		{
			if(rand()%(total_manchar*20)<=20)					// 1/5 几率换人
				show_idx=i;
		}
	}

	
	if((fight_npc_group[show_idx].base.active==false)||
		(fight_npc_group[show_idx].base.unit_type!=FIGHT_UNIT_TYPE_CHARACTER))
		return;

	sprintf((char *)npc_filename,"NPC\\NPC%03dc.MDA",(char *)fight_npc_group[show_idx].base.filename_id);
	
	extra_index=add_extra_fight_mda(npc_filename,fight_npc_group[show_idx].x,fight_npc_group[show_idx].y,false,true,false,EXTRA_MIDDLE_LAYER,(UCHR *)"MainCommand0");
	
	if(extra_index<0)return;


	auto_camera_pan(100,fight_npc_group[show_idx].x,fight_npc_group[show_idx].y-50,12);


	fight_npc_group[show_idx].display=false;
	main_pass=0;
	count=0;
	while(main_pass==0)
	{
		if(game_control.main_loop_mode==MAIN_LOOP_MAP_MODE)
			main_pass=1;
		system_idle_loop();

		process_window_move();						// 处理是否 window 拖移
		fight_camera(screen_buffer);
		exec_screen_text(screen_buffer);			// 执行当前荧幕文字
		draw_main_map(screen_buffer);				// 绘画出地图主menu
	
		if(extra_fight_mda_data[extra_index].play_end)
			main_pass=1;
		check_fight_main_command();					// 检查玩家命令是否完全
		execute_window_base();						// 绘制 window & 执行
		exec_main_map();
		update_screen(screen_buffer);
		count++;
		if(count>250)main_pass=1;


	}

	fight_npc_group[show_idx].display=true;

	auto_camera_pan(fight_default_pos.zoom_index,fight_default_pos.center_x,fight_default_pos.center_y,12);

	fight_control_data.camera_follow_flag=true;
	fight_control_data.camera_follow_x=fight_default_pos.center_x;		// 镜头自动跟随中心点坐标
	fight_control_data.camera_follow_y=fight_default_pos.center_x;
	fight_control_data.camera_follow_zoom=fight_default_pos.zoom_index;	// 镜头自动跟随 zoom idx
	
	
}


void get_default_position(void)
{
	SLONG i;
	SLONG count;

// --- ( 0 - 0 ) Check Right team
	count=0;
	for(i=0;i<10;i++)
	{
		if(fight_npc_group[i].base.active)
		{
			count++;
		}
	}

	if(count>6)
	{
		fight_default_pos.zoom_index=46;
		fight_default_pos.pan_x=350;
		fight_default_pos.pan_y=365;
		fight_default_pos.center_x=fight_default_pos.pan_x+camera_zoom[fight_default_pos.zoom_index].width/2;
		fight_default_pos.center_y=fight_default_pos.pan_y+camera_zoom[fight_default_pos.zoom_index].height/2;
		
		return;
	}

	count=0;
	for(i=10;i<20;i++)
	{
		if(fight_npc_group[i].base.active)
		{
			count++;
		}
	}

	if(count>6)
	{
		fight_default_pos.zoom_index=46;
		fight_default_pos.pan_x=350;
		fight_default_pos.pan_y=365;
		fight_default_pos.center_x=fight_default_pos.pan_x+camera_zoom[fight_default_pos.zoom_index].width/2;
		fight_default_pos.center_y=fight_default_pos.pan_y+camera_zoom[fight_default_pos.zoom_index].height/2;
		return;
	}

	fight_default_pos.zoom_index=67;
	fight_default_pos.pan_x=410;
	fight_default_pos.pan_y=410;
	fight_default_pos.center_x=fight_default_pos.pan_x+camera_zoom[fight_default_pos.zoom_index].width/2;
	fight_default_pos.center_y=fight_default_pos.pan_y+camera_zoom[fight_default_pos.zoom_index].height/2;
	

}


void change_to_map(void)
{
	UCHR real_filename[80];
	
	refresh_all_map_npc();							// 重新要求地图所有 NPC

// stephen 
// 重新要求玩家跟宝宝的数值
//	之要求 now_hp,now_mp, 还有所有宝宝的 now_hp,now_mp
	
	free_fight_image();	

	close_all_main_map_window();
	initializa_display_text();
	
	auto_close_extra_fight_mda();
		
	system_idle_loop();
	while(system_control.control_npc_idx<0)
	{
		system_idle_loop();
		update_screen(screen_buffer);
	}

	game_control.main_loop_mode=MAIN_LOOP_MAP_MODE;
	sprintf((char *)real_filename,"MUSIC\\%s",change_map_info.music_filename);
	play_music_file(real_filename,1,100);

	get_bitmap(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,screen_channel1,screen_buffer);

	redraw_map(screen_buffer);
	draw_main_map(screen_buffer);			// 绘画出地图主menu

	get_bitmap(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,screen_channel0,screen_buffer);
	get_bitmap(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,screen_buffer,screen_channel1);
	

	change_screen_effect_flc(rand()%9);
	
}



SLONG read_fight_image(void)
{
	UCHR mda_filename[80];

	IMG256_GROUP *temp;

	SLONG i;
	UCHR *file_buffer=NULL;
	UCHR *bit24_screen_buffer=NULL;
	SLONG bit24_buffer_len=0;
	ULONG file_size;
	JPEGAREA jpeg_area;
	JPEGINFO info;
	


	// --- ( 0 - 0 ) Background BMP
	if(fight_background!=NULL)
	{
		destroy_bitmap(&fight_background);
		fight_background=NULL;
	}
	
	fight_background=create_bitmap(FIGHT_MAP_WIDTH,FIGHT_MAP_HEIGHT);
	if(fight_background==NULL)
	{
		display_error_message((UCHR *)"Memory error (FIGHT_BACKGROUND)",true);
		return(TTN_NOT_OK);
	}
	
	if(read_mda_database((UCHR *)"system\\fightback.mda",change_map_info.fight_map_no,&file_buffer,&file_size)!=TTN_OK)
	{
		display_error_message((UCHR *)"Read FightBack MDA error",true);
		return(TTN_NOT_OK);
	}
		
	jpeg_area.xPos=0;
	jpeg_area.yPos=0;
	jpeg_area.nWidth=FIGHT_MAP_WIDTH;	
	jpeg_area.nHeight=FIGHT_MAP_HEIGHT;
	
	// -------- Process JPG start	
	jpeg_get(file_buffer,file_size,info);    //得到JPEG的数据
	
	if((jpeg_area.xPos<0)||
		(jpeg_area.yPos<0)||
		(jpeg_area.nWidth>info.nWidth)||
		(jpeg_area.nHeight>info.nHeight)||
		(jpeg_area.xPos+jpeg_area.nWidth>info.nWidth)||
		(jpeg_area.yPos+jpeg_area.nHeight>info.nHeight))
	{
		sprintf((char *)print_rec,MSG_FILE_RANGE_ERROR,"fightback.mda -> ");
		display_error_message(print_rec,true);
		return(TTN_NOT_OK);
	}
	
	
	if(jpeg_read(file_buffer,file_size,jpeg_area,&bit24_screen_buffer,bit24_buffer_len)!=JPG_OK)
	{
		if(file_buffer!=NULL)
			free(file_buffer);
		if(bit24_screen_buffer!=NULL)
			free(bit24_screen_buffer);
		bit24_screen_buffer=NULL;
		sprintf((char *)print_rec,MSG_FILE_DECODE_ERROR,"fightback.mda ->");
		display_error_message(print_rec,true);
		return(TTN_NOT_OK);
	}
	
	bit24_to_BMP(bit24_screen_buffer,fight_background,FIGHT_MAP_WIDTH,FIGHT_MAP_HEIGHT);
	
	if(bit24_screen_buffer!=NULL)
		free(bit24_screen_buffer);
	bit24_screen_buffer=NULL;

	if(file_buffer!=NULL)
		free(file_buffer);
	file_buffer=NULL;
	


// --- ( 0 - 1 ) Menu 
	strcpy((char *)mda_filename,"system\\fightmenu.mda");
	for(i=0;i<MAX_FIGHT_IMAGE_NO;i++)
	{
		temp=fight_image_index[i];
		if(temp->image!=NULL)
			free(temp->image);
		temp->image=NULL;

		if(read_mda_frame((UCHR *)mda_filename,i,&temp->image)!=TTN_OK)		
			return(TTN_NOT_OK);
		get_img256_rect(temp->image,&temp->rect);


	}


// --- ( 0 - 2 ) MDA
	if(fight_image.fight_item_handle>=0)
		mda_group_close(&fight_image.fight_item_handle);
	fight_image.fight_item_handle=-1;				// fight item handle
	strcpy((char *)mda_filename,"system\\fightitem.mda");
	mda_group_open(mda_filename,&fight_image.fight_item_handle);
	if(fight_image.fight_item_handle<0)
		return(TTN_NOT_OK);

	if(fight_image.number0_handle>=0)
		mda_group_close(&fight_image.number0_handle);
	fight_image.number0_handle=-1;					// 绿色 大字
	strcpy((char *)mda_filename,"system\\number0.mda");
	mda_group_open(mda_filename,&fight_image.number0_handle);
	if(fight_image.number0_handle<0)
		return(TTN_NOT_OK);
	

	if(fight_image.number1_handle>=0)
		mda_group_close(&fight_image.number1_handle);
	fight_image.number1_handle=-1;					// 红色 小字
	strcpy((char *)mda_filename,"system\\number1.mda");
	mda_group_open(mda_filename,&fight_image.number1_handle);
	if(fight_image.number1_handle<0)
		return(TTN_NOT_OK);

	
	if(fight_image.number2_handle>=0)
		mda_group_close(&fight_image.number2_handle);
	fight_image.number2_handle=-1;					// 红色 小字
	strcpy((char *)mda_filename,"system\\number2.mda");
	mda_group_open(mda_filename,&fight_image.number2_handle);
	if(fight_image.number2_handle<0)
		return(TTN_NOT_OK);
	

	if(fight_image.double_handle>=0)
		mda_group_close(&fight_image.double_handle);
	fight_image.double_handle=-1;					// 连击 MDA
	strcpy((char *)mda_filename,"system\\double.mda");
	mda_group_open(mda_filename,&fight_image.double_handle);
	if(fight_image.double_handle<0)
		return(TTN_NOT_OK);
	

	if(fight_image.fullmiss_handle>=0)
		mda_group_close(&fight_image.fullmiss_handle);
	fight_image.fullmiss_handle=-1;					// status MDA
	strcpy((char *)mda_filename,"system\\fullmiss.mda");
	mda_group_open(mda_filename,&fight_image.fullmiss_handle);
	if(fight_image.fullmiss_handle<0)
		return(TTN_NOT_OK);

	
	if(fight_image.fight_hit_handle>=0)
		mda_group_close(&fight_image.fight_hit_handle);
	fight_image.fight_hit_handle=-1;				// fight hit handle
	strcpy((char *)mda_filename,"system\\hit.mda");
	mda_group_open(mda_filename,&fight_image.fight_hit_handle);
	if(fight_image.fight_hit_handle<0)
		return(TTN_NOT_OK);
	


	return(TTN_OK);
}



SLONG free_fight_image(void)
{
	IMG256_GROUP *temp;
	SLONG handle;	
	SLONG check_idx;
	
	SLONG i;


// --- ( 0 - 0 ) Menu	
	for(i=0;i<MAX_FIGHT_IMAGE_NO;i++)
	{
		temp=fight_image_index[i];
		if(temp->image!=NULL)
		{
			free(temp->image);
			temp->image=NULL;

		}
		
	}
	

// --- ( 0 - 1 )  background
	if(fight_background!=NULL)
	{
		destroy_bitmap(&fight_background);
		fight_background=NULL;
	}
	

// --- ( 0 - 2 ) Fight npc
	for(i=0;i<MAX_FIGHT_UNIT;i++)
	{
		handle=fight_npc_group[i].handle;
		check_idx=get_mda_index(handle);
		if(check_idx>=0)
			mda_group_close(&handle);

		fight_npc_group[i].base.active=false;
		fight_npc_group[i].handle=-1;
	}

	
// --- ( 0 - 3 ) MDA handle

	// --- ( 0 - 2 ) MDA
	if(fight_image.fight_item_handle>=0)
		mda_group_close(&fight_image.fight_item_handle);
	fight_image.fight_item_handle=-1;				// fight item handle
	
	if(fight_image.number0_handle>=0)
		mda_group_close(&fight_image.number0_handle);
	fight_image.number0_handle=-1;					// 绿色 大字
	
	
	if(fight_image.number1_handle>=0)
		mda_group_close(&fight_image.number1_handle);
	fight_image.number1_handle=-1;					// 红色 小字
	
	if(fight_image.number2_handle>=0)
		mda_group_close(&fight_image.number2_handle);
	fight_image.number2_handle=-1;					// 绿色 大字
	
	if(fight_image.double_handle>=0)
		mda_group_close(&fight_image.double_handle);
	fight_image.double_handle=-1;					// 连击 MDA
	
	
	if(fight_image.fullmiss_handle>=0)
		mda_group_close(&fight_image.fullmiss_handle);
	fight_image.fullmiss_handle=-1;					// status MDA
	

	if(fight_image.fight_hit_handle>=0)
		mda_group_close(&fight_image.fight_hit_handle);
	fight_image.fight_hit_handle=-1;				// hit MDA
	
	
	game_control.main_loop_mode=MAIN_LOOP_FIGHT_MODE;
	return(TTN_OK);
}


void clear_fight_npc_group(void)
{
	SLONG i;

	for(i=0;i<MAX_FIGHT_UNIT;i++)
	{
		fight_npc_group[i].base.active=false;
		fight_npc_group[i].display=false;
		fight_npc_group[i].handle=-1;

	}

// clear baobao skill
	for(i=0;i<MAX_BAOBAO_SKILL_NO;i++)
	{
		baobao_skill_data[i].skill_id=-1;
	}

	if(!game_control.character_skill_ready)
		request_character_skill_data();
	
	if(!game_control.baobao_skill_ready)
		request_baobao_skill_data();

	game_control.skill_start_index=0;
	game_control.skill_select_index=-1;
	

}



void insert_fight_unit(FIGHT_BASE_DATA data,SLONG idx)
{
	SLONG mda_handle;
	SLONG mda_index;
	UCHR npc_filename[80];


	if(fight_npc_group[idx].base.active)
	{
		display_error_message((UCHR *)"Fight unit already have data , Error!",1);
		return;
	}

	memcpy(&fight_npc_group[idx].base,&data,sizeof(struct FIGHT_BASE_DATA_STRUCT));
	fight_npc_group[idx].base.active=true;
	fight_npc_group[idx].display=true;
	fight_npc_group[idx].npc_timer=0;
	if(fight_npc_group[idx].base.filename_id<100)
		fight_npc_group[idx].npc_type=NPC_CHARACTER_TYPE;
	else
		fight_npc_group[idx].npc_type=NPC_CPU_TYPE;
	

	sprintf((char *)npc_filename,"NPC\\NPC%03db.MDA",(char *)data.filename_id);
	mda_handle=get_mda_handle(npc_filename);
	if(mda_handle>=0)					// Already Open
	{
		fight_npc_group[idx].handle=mda_handle;
	}
	else
	{
		if(mda_group_open(npc_filename,&mda_handle)!=TTN_OK)
		{
			sprintf((char *)print_rec,MSG_FILE_CREAT_ERROR,npc_filename);
			display_error_message((UCHR *)print_rec,true);
			fight_npc_group[idx].base.active=false;
			return;
		}
		fight_npc_group[idx].handle=mda_handle;
		
		// --- 第一次执行
		mda_group_exec(mda_handle,(UCHR *)"MainCommand0",-9999,-9999,screen_channel1,MDA_COMMAND_FIRST,0,0,0);
		
		
	}
	
	mda_index=get_mda_index(mda_handle);
	if(mda_index<0)
	{
		sprintf((char *)print_rec,MSG_FILE_CREAT_ERROR,npc_filename);
		display_error_message((UCHR *)print_rec,true);
		fight_npc_group[idx].base.active=false;
		return;
	}
	
	adjust_fight_npc_frame_data(idx);
	fight_npc_group[idx].x=fight_position[idx].x;			// 默认位置
	fight_npc_group[idx].y=fight_position[idx].y;
	fight_npc_group[idx].motion=FIGHT_MOTION_STANDBY;	
	if(idx<10)
		fight_npc_group[idx].dir=FIGHT_NPC_DIR_LEFT;
	else
		fight_npc_group[idx].dir=FIGHT_NPC_DIR_DOWN;

	
	switch(idx)
	{
	case 0:					// 主角
		character_charge_time_data.charge_time=base_charge_time_table[data.charge_speed];
		character_charge_time_data.delay_time=0;
		character_charge_time_data.charge_start_time=system_control.system_timer;
		break;
	case 1:					// 主角宝宝
		baobao_charge_time_data.charge_time=base_charge_time_table[data.charge_speed];
		baobao_charge_time_data.delay_time=0;
		baobao_charge_time_data.charge_start_time=system_control.system_timer;
		break;
	}

// ----- Copy result to data
	fight_npc_group[idx].result.charge_speed=fight_npc_group[idx].base.charge_speed;
	fight_npc_group[idx].result.id=fight_npc_group[idx].base.id;
	fight_npc_group[idx].result.max_hp=fight_npc_group[idx].base.max_hp;
	fight_npc_group[idx].result.max_mp=fight_npc_group[idx].base.max_mp;
	fight_npc_group[idx].result.now_hp=fight_npc_group[idx].base.now_hp;
	fight_npc_group[idx].result.now_mp=fight_npc_group[idx].base.now_mp;
	fight_npc_group[idx].result.status=fight_npc_group[idx].base.status;


}



void fight_camera(BMP *buffer)
{

	if(fight_control_data.camera_follow_flag)
		auto_camera_follow();

// ----- ( 0 - 1 ) Draw background ---------
	rectangle_scale_put_bmp(0,0,800,450,buffer,
		now_camera_pan_x,now_camera_pan_y,
		camera_zoom[now_camera_zoom].width,
		camera_zoom[now_camera_zoom].height,
		fight_background);


// ----- ( 0 - 2 ) draw fight npc
	clear_screen_mask();
	draw_fight_npc_layer(buffer);
	

// ----- ( 0 - 3 ) Process MAP task
	process_fight_camera_task(buffer);


// ----- ( 9 - 0 )  debug fight info

}



void draw_fight_npc_layer(BMP *buffer)
{
	SLONG i;

// --- ( 0 - 0 )  Clear
	clear_npc_layer_sort();

// --- ( 0 - 1 ) Add Npc Data & MAP_EFFECT & EXTRA_MDA
	for(i=0;i<MAX_FIGHT_UNIT;i++)
	{
		if(!fight_npc_group[i].display)continue;
		add_npc_layer_sort(SORT_TYPE_NPC,i,fight_npc_group[i].y);
	}

	for(i=0;i<MAX_EXTRA_FIGHT_MDA;i++)
	{
		if(extra_fight_mda_data[i].level!=EXTRA_MIDDLE_LAYER)continue;
		add_npc_layer_sort(SORT_TYPE_EXTRA,i,extra_fight_mda_data[i].display_y);
	}
	
	

// --- ( 1 - 0 ) Begin sort
	sort_npc_layer();


	draw_button_extra_fight_mda(buffer);
	
// --- ( 2 - 0 ) Begin Draw NPC LAYER
	for(i=0;i<total_npc_layer;i++)
	{
		switch(npc_layer_sort[i].type)
		{
		case SORT_TYPE_NPC:
			map_control.draw_mask=true;
			map_control.mask_npc=npc_layer_sort[i].idx;
			draw_fight_single_npc(npc_layer_sort[i].idx,buffer);
			map_control.draw_mask=false;
			break;
		case SORT_TYPE_MDA:
			break;
		case SORT_TYPE_EXTRA:
			draw_single_extra_fight_mda(npc_layer_sort[i].idx,buffer);
			break;
		default:
			continue;
		}
	}

	draw_top_extra_fight_mda(buffer);
	

	if(system_control.point_npc_idx>=0)
		display_fight_single_name(system_control.point_npc_idx,buffer);

	if(system_image.icon_timer<system_control.system_timer)
	{
		system_image.icon_timer=system_control.system_timer+SYSTEM_MAIN_DELAY;
	}	


}



void draw_fight_single_npc(SLONG idx,BMP *buffer)
{
	SLONG screen_x,screen_y;
	SLONG npc_mda_handle,npc_mda_index;
	SLONG mda_command;
	UCHR command_str[80];
	SLONG motion_index;
	SLONG frame_index;
	SLONG now_frame;
	SLONG now_dir;
	SLONG x,y;
	SLONG hp_percent;
	SLONG img_xl;
	

	SLONG hp,max_hp;
	

// --- Get Base Data 
	

	screen_x=(fight_npc_group[idx].x-now_camera_pan_x)*camera_zoom[now_camera_zoom].x_pos_zoom/1000;
	screen_y=(fight_npc_group[idx].y-now_camera_pan_y)*camera_zoom[now_camera_zoom].y_pos_zoom/1000;
	npc_mda_handle=fight_npc_group[idx].handle;
	npc_mda_index=get_mda_index(npc_mda_handle);
	if(npc_mda_index<0)return;



	now_dir=fight_npc_group[idx].dir;
	motion_index=0;
	switch(fight_npc_group[idx].motion)
	{
	case FIGHT_MOTION_STANDBY:
		motion_index=fight_npc_group[idx].dir_frame[now_dir].motion;
		now_frame=fight_npc_group[idx].dir_frame[now_dir].now_frame;
		break;
	case FIGHT_MOTION_RUN:
		motion_index=4;
		now_frame=0;
		break;
	case FIGHT_MOTION_ATTACK1:
		motion_index=fight_npc_group[idx].attack1_frame[now_dir].motion;
		now_frame=fight_npc_group[idx].attack1_frame[now_dir].now_frame;
		break;
	case FIGHT_MOTION_ATTACK2:
		motion_index=fight_npc_group[idx].attack2_frame[now_dir].motion;
		now_frame=fight_npc_group[idx].attack2_frame[now_dir].now_frame;
		break;
	case FIGHT_MOTION_ATTACK3:
		motion_index=fight_npc_group[idx].attack3_frame[now_dir].motion;
		now_frame=fight_npc_group[idx].attack3_frame[now_dir].now_frame;
		break;
	case FIGHT_MOTION_MAGIC:
		motion_index=fight_npc_group[idx].magic_frame[now_dir].motion;
		now_frame=fight_npc_group[idx].magic_frame[now_dir].now_frame;
		break;
	case FIGHT_MOTION_SPECIAL:
		motion_index=fight_npc_group[idx].special_frame[now_dir].motion;
		now_frame=fight_npc_group[idx].special_frame[now_dir].now_frame;
		break;
	case FIGHT_MOTION_HURT:
		motion_index=4;
		now_frame=0;
		
		break;
	case FIGHT_MOTION_DEAD:
		motion_index=fight_npc_group[idx].dead_frame[now_dir].motion;
		now_frame=fight_npc_group[idx].dead_frame[now_dir].now_frame;
		break;
	}
	
	

	frame_index=motion_index+now_dir;
	sprintf((char *)command_str,"MainCommand%d",frame_index);
	

	if(frame_index>=mda_group[npc_mda_index].total_ani_command)
		return;	// MDA out fo range

// -- set effect
	mda_command=MDA_COMMAND_ASSIGN|MDA_COMMAND_ZOOM;

	if(system_control.point_npc_idx==idx)
	{
		map_control.draw_point_color=true;
		system_palette=point_palette;
	}
	


	map_layer_touch.flag=false;
	map_layer_touch.character=false;

// --- Draw

	mda_group_exec(npc_mda_handle,command_str,screen_x,screen_y,buffer,mda_command,camera_zoom[now_camera_zoom].size_zoom,0,now_frame);


	map_control.draw_point_color=false;



	switch(fight_npc_group[idx].motion)
	{
	case FIGHT_MOTION_STANDBY:
		if(system_control.system_timer>=fight_npc_group[idx].npc_timer)
		{
			fight_npc_group[idx].npc_timer=system_control.system_timer+SYSTEM_MAIN_DELAY;
			change_fight_npc_next_frame(idx,now_dir,fight_npc_group[idx].motion);
		}
		break;
	case FIGHT_MOTION_RUN:
		break;
	case FIGHT_MOTION_ATTACK1:
	case FIGHT_MOTION_ATTACK2:
	case FIGHT_MOTION_ATTACK3:
	//	if(system_control.system_timer>=fight_npc_group[idx].npc_timer)
	//	{
	//		fight_npc_group[idx].npc_timer=system_control.system_timer+40;
	//		change_fight_npc_next_frame(idx,now_dir,fight_npc_group[idx].motion);
	//	}
		break;
	case FIGHT_MOTION_MAGIC:
	//	if(system_control.system_timer>=fight_npc_group[idx].npc_timer)
	//	{
	//		fight_npc_group[idx].npc_timer=system_control.system_timer+SYSTEM_MAIN_DELAY;
	//		change_fight_npc_next_frame(idx,now_dir,fight_npc_group[idx].motion);
	//	}
		break;
	case FIGHT_MOTION_SPECIAL:
	//	if(system_control.system_timer>=fight_npc_group[idx].npc_timer)
	//	{
	//		fight_npc_group[idx].npc_timer=system_control.system_timer+40;
	//		change_fight_npc_next_frame(idx,now_dir,fight_npc_group[idx].motion);
	//	}
		break;
	case FIGHT_MOTION_HURT:
	//	if(system_control.system_timer>=fight_npc_group[idx].npc_timer)
	//	{
	//		fight_npc_group[idx].npc_timer=system_control.system_timer+SYSTEM_MAIN_DELAY;
	//		change_fight_npc_next_frame(idx,now_dir,fight_npc_group[idx].motion);
	//	}
		break;
	case FIGHT_MOTION_DEAD:
		if(system_control.system_timer>=fight_npc_group[idx].npc_timer)
		{
			fight_npc_group[idx].npc_timer=system_control.system_timer+SYSTEM_MAIN_DELAY;
			change_fight_npc_next_frame(idx,now_dir,fight_npc_group[idx].motion);
		}
		break;
	}

	


	y=screen_y+10;
	x=screen_x-strlen(fight_npc_group[idx].base.name)*8/2;


	if(idx < 10 )		// 我方人员
	{
		hp=fight_npc_group[idx].base.now_hp;
		max_hp=fight_npc_group[idx].base.max_hp;
		if(max_hp<1)max_hp=1;
		if(hp<0)hp=0;
		if(hp>max_hp)hp=max_hp;
		hp_percent=hp*100/max_hp;

		put_box(x+1,y-4+1,81,4,color_control.black,buffer);
		put_box(x,y-4,81,4,color_control.gray,buffer);
		img_xl=80*hp_percent/100;
		if(img_xl>0)
			put_bar(x+1,y-4+1,img_xl,3,color_control.red,buffer);
		
	}
	

	set_word_color(0,color_control.black);
	print12(x+1,y+1,fight_npc_group[idx].base.name,PEST_PUT,buffer);
	set_word_color(0,color_control.green);
	print12(x,y,fight_npc_group[idx].base.name,PEST_PUT,buffer);
	
	set_word_color(0,color_control.white);


// ---- debug
//	sprintf((char *)print_rec,"             %u",fight_npc_group[idx].base.id);
//	print12(x,y+16,print_rec,COPY_PUT,buffer);
	


}



void display_fight_single_name(SLONG idx,BMP *buffer)
{
	SLONG screen_x,screen_y;
	SLONG npc_mda_handle,npc_mda_index;
	SLONG x,y;
	
	// --- Get Base Data 
	
	
	screen_x=(fight_npc_group[idx].x-now_camera_pan_x)*camera_zoom[now_camera_zoom].x_pos_zoom/1000;
	screen_y=(fight_npc_group[idx].y-now_camera_pan_y)*camera_zoom[now_camera_zoom].y_pos_zoom/1000;
	npc_mda_handle=fight_npc_group[idx].handle;
	npc_mda_index=get_mda_index(npc_mda_handle);
	if(npc_mda_index<0)return;
	
	y=screen_y+10;
	x=screen_x-strlen(fight_npc_group[idx].base.name)*8/2;
	
	set_word_color(0,color_control.black);
	print12(x+1,y+1,fight_npc_group[idx].base.name,PEST_PUT,buffer);
	set_word_color(0,map_control.point_color);
	print12(x,y,fight_npc_group[idx].base.name,PEST_PUT,buffer);
	
	set_word_color(0,color_control.white);
	
	
}




void change_fight_npc_next_frame(SLONG idx,SLONG now_dir,SLONG motion)
{
	
	switch(motion)
	{
	case FIGHT_MOTION_STANDBY:
		fight_npc_group[idx].dir_frame[now_dir].now_frame++;
		if(fight_npc_group[idx].dir_frame[now_dir].now_frame>=fight_npc_group[idx].dir_frame[now_dir].total_frame)
			fight_npc_group[idx].dir_frame[now_dir].now_frame=0;
		break;
	case FIGHT_MOTION_RUN:
		break;
	case FIGHT_MOTION_ATTACK1:
		fight_npc_group[idx].attack1_frame[now_dir].now_frame++;
		if(fight_npc_group[idx].attack1_frame[now_dir].now_frame>=fight_npc_group[idx].attack1_frame[now_dir].total_frame)
			fight_npc_group[idx].attack1_frame[now_dir].now_frame=fight_npc_group[idx].attack1_frame[now_dir].total_frame-1;
		break;
	case FIGHT_MOTION_ATTACK2:
		fight_npc_group[idx].attack2_frame[now_dir].now_frame++;
		if(fight_npc_group[idx].attack2_frame[now_dir].now_frame>=fight_npc_group[idx].attack2_frame[now_dir].total_frame)
			fight_npc_group[idx].attack2_frame[now_dir].now_frame=fight_npc_group[idx].attack2_frame[now_dir].total_frame-1;
		break;
	case FIGHT_MOTION_ATTACK3:
		fight_npc_group[idx].attack3_frame[now_dir].now_frame++;
		if(fight_npc_group[idx].attack3_frame[now_dir].now_frame>=fight_npc_group[idx].attack3_frame[now_dir].total_frame)
			fight_npc_group[idx].attack3_frame[now_dir].now_frame=fight_npc_group[idx].attack3_frame[now_dir].total_frame-1;
		break;
	case FIGHT_MOTION_MAGIC:
		fight_npc_group[idx].magic_frame[now_dir].now_frame++;
		if(fight_npc_group[idx].magic_frame[now_dir].now_frame>=fight_npc_group[idx].magic_frame[now_dir].total_frame)
			fight_npc_group[idx].magic_frame[now_dir].now_frame=fight_npc_group[idx].magic_frame[now_dir].total_frame-1;
		
		break;
	case FIGHT_MOTION_SPECIAL:
		fight_npc_group[idx].special_frame[now_dir].now_frame++;
		if(fight_npc_group[idx].special_frame[now_dir].now_frame>=fight_npc_group[idx].special_frame[now_dir].total_frame)
			fight_npc_group[idx].special_frame[now_dir].now_frame=fight_npc_group[idx].special_frame[now_dir].total_frame-1;
		
		break;
	case FIGHT_MOTION_HURT:
		break;
	case FIGHT_MOTION_DEAD:
		fight_npc_group[idx].dead_frame[now_dir].now_frame++;
		if(fight_npc_group[idx].dead_frame[now_dir].now_frame>=fight_npc_group[idx].dead_frame[now_dir].total_frame)
			fight_npc_group[idx].dead_frame[now_dir].now_frame=fight_npc_group[idx].dead_frame[now_dir].total_frame-1;
		break;
	default:
		return;
	}

	
}



void set_camera(SLONG zoom,SLONG x,SLONG y)
{
	now_camera_zoom=zoom;
	now_camera_pan_x=x;
	now_camera_pan_y=y;
	if(now_camera_pan_x<0)
		now_camera_pan_x=0;
	if(now_camera_pan_y<0)
		now_camera_pan_y=0;
	if(now_camera_pan_x+camera_zoom[now_camera_zoom].width>=FIGHT_MAP_WIDTH)
		now_camera_pan_x=FIGHT_MAP_WIDTH-camera_zoom[now_camera_zoom].width;
	if(now_camera_pan_y+camera_zoom[now_camera_zoom].height>=FIGHT_MAP_HEIGHT)
		now_camera_pan_y=FIGHT_MAP_HEIGHT-camera_zoom[now_camera_zoom].height;
	
}



void test_fight_camera(void)
{
	UCHR ch;
		
	ch=system_control.key;
	switch(ch)
	{
	case Home:
		camera_auto_control(CAMERA_ZOOM_IN,CAMERA_PAN_LOCK_CENTER,10,1,0);
		break;
	case End:
		camera_auto_control(CAMERA_ZOOM_OUT,CAMERA_PAN_LOCK_CENTER,10,1,0);
		break;
	case PgUp:
		camera_auto_control(CAMERA_ZOOM_IN,CAMERA_PAN_LOCK_CENTER,1,1,0);
		break;
	case PgDn:
		camera_auto_control(CAMERA_ZOOM_OUT,CAMERA_PAN_LOCK_CENTER,1,1,0);
		break;
	case Up:
		camera_auto_control(CAMERA_ZOOM_STILL,CAMERA_PAN_UP,1,1,0);
		break;
	case Dn:
		camera_auto_control(CAMERA_ZOOM_STILL,CAMERA_PAN_DOWN,1,1,0);
		break;
	case Left:
		camera_auto_control(CAMERA_ZOOM_STILL,CAMERA_PAN_LEFT,1,1,0);
		break;
	case Right:
		camera_auto_control(CAMERA_ZOOM_STILL,CAMERA_PAN_RIGHT,1,1,0);
		break;
		

	}


	sprintf((char *)print_rec,"%d,%d (%d)-(%d)",now_camera_pan_x,now_camera_pan_y,now_camera_zoom,camera_zoom[now_camera_zoom].size_zoom);
	print16(0,0,print_rec,PEST_PUT,screen_buffer);
	
}	



void camera_auto_control(SLONG zoom_command,SLONG pan_command,SLONG zoom_value,SLONG pan_value,SLONG pan_value1)
{
SLONG move_x,move_y;
SLONG x_offset,y_offset,zoom_offset;
SLONG zoom_delta,x_delta,y_delta;
SLONG old_camera_zoom;


		old_camera_zoom=now_camera_zoom;
		switch(zoom_command)
			{
			case CAMERA_ZOOM_STILL:
				break;
			case CAMERA_ZOOM_IN:
				now_camera_zoom+=zoom_value;
				break;
			case CAMERA_ZOOM_OUT:
				now_camera_zoom-=zoom_value;
				break;
			case CAMERA_ZOOM_LOCK:
				zoom_offset=abs(now_camera_zoom-zoom_value);
				if(zoom_offset>8)
					zoom_delta=4;
				else if(zoom_offset>4)
					zoom_delta=2;
				else
					zoom_delta=1;

				if(now_camera_zoom>zoom_value)
					{
					now_camera_zoom-=zoom_delta;
					if(now_camera_zoom<zoom_value)
						now_camera_zoom=zoom_value;
					}
				else if(now_camera_zoom<zoom_value)
					{
					now_camera_zoom+=zoom_delta;
					if(now_camera_zoom>zoom_value)
						now_camera_zoom=zoom_value;
					}

				break;
			default:
				return;
			}

		if(now_camera_zoom<0)
			now_camera_zoom=0;
		if(now_camera_zoom>MAX_CAMERA_ZOOM-1)
			now_camera_zoom=MAX_CAMERA_ZOOM-1;
		

		switch(pan_command)
			{
			case CAMERA_PAN_STILL:
				break;
			case CAMERA_PAN_UP:
				now_camera_pan_y-=pan_value;
				break;
			case CAMERA_PAN_DOWN:
				now_camera_pan_y+=pan_value;
				break;
			case CAMERA_PAN_LEFT:
				now_camera_pan_x-=pan_value;
				break;
			case CAMERA_PAN_RIGHT:
				now_camera_pan_x+=pan_value;
				break;
			case CAMERA_PAN_LOCK_CENTER:
				now_camera_pan_x=(now_camera_pan_x+camera_zoom[old_camera_zoom].width/2)-camera_zoom[now_camera_zoom].width/2;
				now_camera_pan_y=(now_camera_pan_y+camera_zoom[old_camera_zoom].height/2)-camera_zoom[now_camera_zoom].height/2;
				break;
			case CAMERA_PAN_LOCK_LEFT:
				now_camera_pan_x=0;
				now_camera_pan_y=FIGHT_MAP_HEIGHT-camera_zoom[now_camera_zoom].height;
				break;
			case CAMERA_PAN_LOCK_RIGHT:
				now_camera_pan_x=FIGHT_MAP_WIDTH-camera_zoom[now_camera_zoom].width;
				now_camera_pan_y=FIGHT_MAP_HEIGHT-camera_zoom[now_camera_zoom].height;
				break;
			case CAMERA_PAN_LOCK_POINT:
				move_x=pan_value-camera_zoom[now_camera_zoom].width/2;
				move_y=pan_value1-camera_zoom[now_camera_zoom].height/2;

				x_offset=abs(now_camera_pan_x-move_x);
				y_offset=abs(now_camera_pan_y-move_y);

				if(x_offset>64)
					x_delta=32;
				else if(x_offset>32)
					x_delta=16;
				else if(x_offset>16)
					x_delta=8;
				else if(x_offset>8)
					x_delta=4;
				else
					x_delta=1;


				if(y_offset>40)
					y_delta=20;
				else if(y_offset>20)
					y_delta=10;
				else if(y_offset>10)
					y_delta=5;
				else if(y_offset>5)
					y_delta=3;
				else
					y_delta=1;
			
				if(now_camera_pan_x>move_x)
					{
					now_camera_pan_x-=x_delta;
					if(now_camera_pan_x<move_x)
						now_camera_pan_x=move_x;
					}
				else if(now_camera_pan_x<move_x)
					{
					now_camera_pan_x+=x_delta;
					if(now_camera_pan_x>move_x)
						now_camera_pan_x=move_x;
					}

				if(now_camera_pan_y>move_y)
					{
					now_camera_pan_y-=y_delta;
					if(now_camera_pan_y<move_y)
						now_camera_pan_y=move_y;
					}

				else if(now_camera_pan_y<move_y)
					{
					now_camera_pan_y+=y_delta;
					if(now_camera_pan_y>move_y)
						now_camera_pan_y=move_y;

					}
				break;
			case CAMERA_PAN_LOCK:
				now_camera_pan_x=pan_value-camera_zoom[now_camera_zoom].width/2;
				now_camera_pan_y=pan_value1-camera_zoom[now_camera_zoom].height/2;
				break;
			default:
				break;
			}


		if(now_camera_pan_x<0)
			now_camera_pan_x=0;
		if(now_camera_pan_y<0)
			now_camera_pan_y=0;
		if(now_camera_pan_x+camera_zoom[now_camera_zoom].width>=FIGHT_MAP_WIDTH)
			now_camera_pan_x=FIGHT_MAP_WIDTH-camera_zoom[now_camera_zoom].width;
		if(now_camera_pan_y+camera_zoom[now_camera_zoom].height>=FIGHT_MAP_HEIGHT)
			now_camera_pan_y=FIGHT_MAP_HEIGHT-camera_zoom[now_camera_zoom].height;


}







SLONG add_extra_fight_mda(UCHR *filename,SLONG x,SLONG y,bool loop,bool auto_close,bool hight_light,SLONG level,UCHR *command)
{
	SLONG index;
	SLONG i;
	SLONG handle;

	index=-1;
	for(i=0;i<MAX_EXTRA_FIGHT_MDA;i++)
	{
		if(extra_fight_mda_data[i].handle<0)		// 没人用
		{
			index=i;
			break;
		}
	}

	if(index<0)return(TTN_NOT_OK);			// full 
	
	mda_group_open(filename,&handle);
	if(handle<0)return(TTN_NOT_OK);			// file open error


	mda_group_exec(handle,(UCHR *)"MainCommand0",-9999,-9999,screen_channel0,MDA_COMMAND_FIRST,0,0,0);
	extra_fight_mda_data[index].mda_index=get_mda_index(handle);
	
	extra_fight_mda_data[index].handle=handle;
	extra_fight_mda_data[index].display_x=x;
	extra_fight_mda_data[index].display_y=y;
	extra_fight_mda_data[index].xl=mda_group[extra_fight_mda_data[index].mda_index].img256_head->xlen;
	extra_fight_mda_data[index].yl=mda_group[extra_fight_mda_data[index].mda_index].img256_head->ylen;
	extra_fight_mda_data[index].center_x=mda_group[extra_fight_mda_data[index].mda_index].img256_head->center_x;
	extra_fight_mda_data[index].center_y=mda_group[extra_fight_mda_data[index].mda_index].img256_head->center_y;
	extra_fight_mda_data[index].loop=loop;
	extra_fight_mda_data[index].auto_close=auto_close;
	extra_fight_mda_data[index].hight_light_flag=hight_light;
	strcpy((char *)extra_fight_mda_data[index].command,(char *)command);
	extra_fight_mda_data[index].level=level;
	extra_fight_mda_data[index].play_end=false;
	extra_fight_mda_data[index].timer=system_control.system_timer;
	extra_fight_mda_data[index].display=true;

	return(index);

}



void auto_close_extra_fight_mda(void)
{
	SLONG i;
	SLONG handle;
	
	for(i=0;i<MAX_EXTRA_FIGHT_MDA;i++)
	{
		if(extra_fight_mda_data[i].handle>=0)		// 没人用
		{
			if(extra_fight_mda_data[i].auto_close==true)		// 需要自动关闭
			{
				handle=extra_fight_mda_data[i].handle;
				mda_group_close(&handle);
				extra_fight_mda_data[i].handle=-1;								// MDA handle
				extra_fight_mda_data[i].mda_index=-1;							// mda index
				extra_fight_mda_data[i].display_x=0;							// 实际坐标 
				extra_fight_mda_data[i].display_y=0;
				extra_fight_mda_data[i].loop=false;								// 是否 Loop 播放
				extra_fight_mda_data[i].play_end=false;							// 是否已经到最后1祯
				extra_fight_mda_data[i].auto_close=true;						// 是否到最后1祯自动关闭(loop 为 false) 或者更换地图关闭
				extra_fight_mda_data[i].hight_light_flag=false;					// 是否需要 HightLight
				extra_fight_mda_data[i].level=0;								// 等级 0 -> 底层  1-> 中间层  2-> 最上层
				strcpy((char *)extra_fight_mda_data[i].command,"");				// 播放命令
				extra_fight_mda_data[i].timer=0;
				extra_fight_mda_data[i].xl=0;
				extra_fight_mda_data[i].yl=0;
				extra_fight_mda_data[i].center_x=0;
				extra_fight_mda_data[i].center_y=0;
			}
		}
	}
	
}


void reset_extra_fight_mda_frame(SLONG idx)
{
	SLONG handle;
	SLONG mda_index;

	handle=extra_fight_mda_data[idx].handle;
	mda_index=get_mda_index(handle);
	if(mda_index<0)
		return;
	mda_group[mda_index].now_frame[0]=0;
}



void draw_button_extra_fight_mda(BMP *buffer)
{
	SLONG idx;
	SLONG screen_x,screen_y;
	SLONG mda_handle,mda_index;
	SLONG mda_width,mda_height;
	SLONG mda_command;
	UCHR command_str[80];
	SLONG ret_val;
	
	for(idx=0;idx<MAX_EXTRA_FIGHT_MDA;idx++)
	{
		if(extra_fight_mda_data[idx].handle<0)continue;
		if(!extra_fight_mda_data[idx].display)continue;
		if(extra_fight_mda_data[idx].level!=EXTRA_BOTTOM_LAYER)continue;		// Button

		// --- Get Base Data 
		mda_handle=extra_fight_mda_data[idx].handle;
		mda_index=get_mda_index(mda_handle);
		if(mda_index<0)continue;
		mda_width=extra_fight_mda_data[idx].xl;
		mda_height=extra_fight_mda_data[idx].yl;
		
		
		mda_command=MDA_COMMAND_ZOOM;
		if(extra_fight_mda_data[idx].hight_light_flag)
			mda_command=MDA_COMMAND_HIGHTLIGHT;
		if(extra_fight_mda_data[idx].loop)
			mda_command=mda_command|MDA_COMMAND_LOOP;
		
		if(system_control.system_timer>=extra_fight_mda_data[idx].timer)
		{
			mda_command=mda_command|MDA_COMMAND_NEXT_FRAME;
			extra_fight_mda_data[idx].timer=system_control.system_timer+45;
			
		}

		screen_x=(extra_fight_mda_data[idx].display_x-now_camera_pan_x)*camera_zoom[now_camera_zoom].x_pos_zoom/1000;
		screen_y=(extra_fight_mda_data[idx].display_y-now_camera_pan_y)*camera_zoom[now_camera_zoom].y_pos_zoom/1000;
		
		
		strcpy((char *)command_str,(char *)extra_fight_mda_data[idx].command);
		ret_val=mda_group_exec(mda_handle,command_str,screen_x,screen_y,buffer,mda_command,camera_zoom[now_camera_zoom].size_zoom,0,0);
		map_layer_touch.flag=false;
		
		if(ret_val==MDA_EXEC_LAST)
		{
			extra_fight_mda_data[idx].play_end=true;			
			if(extra_fight_mda_data[idx].auto_close)	// 自动关闭
			{
				if(!extra_fight_mda_data[idx].loop)			// 如果不是 loop
				{
					mda_group_close(&extra_fight_mda_data[idx].handle);		// 关闭
					extra_fight_mda_data[idx].handle=-1;
				}
			}
			else
			{
				if(!extra_fight_mda_data[idx].loop)			// 如果不是 loop
				{
					extra_fight_mda_data[idx].display=false;
				}
				
			}
			
		}
		
		
	}
	
}



void draw_top_extra_fight_mda(BMP *buffer)
{
	SLONG idx;
	SLONG screen_x,screen_y;
	SLONG mda_handle,mda_index;
	SLONG mda_width,mda_height;
	SLONG mda_command;
	UCHR command_str[80];
	SLONG ret_val;
	
	for(idx=0;idx<MAX_EXTRA_FIGHT_MDA;idx++)
	{
		if(extra_fight_mda_data[idx].handle<0)continue;
		if(!extra_fight_mda_data[idx].display)continue;
		if(extra_fight_mda_data[idx].level!=EXTRA_TOP_LAYER)continue;		// Bottom
		
		// --- Get Base Data 
		mda_handle=extra_fight_mda_data[idx].handle;
		mda_index=get_mda_index(mda_handle);
		if(mda_index<0)continue;
		mda_width=extra_fight_mda_data[idx].xl;
		mda_height=extra_fight_mda_data[idx].yl;
		
		
		mda_command=MDA_COMMAND_ZOOM;
		if(extra_fight_mda_data[idx].hight_light_flag)
			mda_command=MDA_COMMAND_HIGHTLIGHT;
		if(extra_fight_mda_data[idx].loop)
			mda_command=mda_command|MDA_COMMAND_LOOP;
		
		if(system_control.system_timer>=extra_fight_mda_data[idx].timer)
		{
			mda_command=mda_command|MDA_COMMAND_NEXT_FRAME;
			extra_fight_mda_data[idx].timer=system_control.system_timer+45;
			
		}

		screen_x=(extra_fight_mda_data[idx].display_x-now_camera_pan_x)*camera_zoom[now_camera_zoom].x_pos_zoom/1000;
		screen_y=(extra_fight_mda_data[idx].display_y-now_camera_pan_y)*camera_zoom[now_camera_zoom].y_pos_zoom/1000;
		
		strcpy((char *)command_str,(char *)extra_fight_mda_data[idx].command);
		ret_val=mda_group_exec(mda_handle,command_str,screen_x,screen_y,buffer,mda_command,camera_zoom[now_camera_zoom].size_zoom,0,0);
		map_layer_touch.flag=false;
		
		if(ret_val==MDA_EXEC_LAST)
		{
			extra_fight_mda_data[idx].play_end=true;			
			if(extra_fight_mda_data[idx].auto_close)			// 自动关闭
			{
				if(!extra_fight_mda_data[idx].loop)			// 如果不是 loop
				{
					mda_group_close(&extra_fight_mda_data[idx].handle);		// 关闭
					extra_fight_mda_data[idx].handle=-1;
				}
			}
			else
			{
				if(!extra_fight_mda_data[idx].loop)			// 如果不是 loop
				{
					extra_fight_mda_data[idx].display=false;
				}
				
			}
			
		}
		
		
	}
	
}



void draw_single_extra_fight_mda(SLONG idx,BMP *buffer)
{
	SLONG screen_x,screen_y;
	SLONG mda_handle,mda_index;
	SLONG mda_width,mda_height;
	SLONG mda_command;
	UCHR command_str[80];
	SLONG ret_val;
	
	
	
	if(!extra_fight_mda_data[idx].display)return;
	if(extra_fight_mda_data[idx].handle<0)return;
	// --- Get Base Data 
	mda_handle=extra_fight_mda_data[idx].handle;
	
	mda_index=get_mda_index(mda_handle);
	if(mda_index<0)return;
	mda_width=extra_fight_mda_data[idx].xl;
	mda_height=extra_fight_mda_data[idx].yl;
	
	
	
	mda_command=MDA_COMMAND_ZOOM;
	if(extra_fight_mda_data[idx].hight_light_flag)
		mda_command=MDA_COMMAND_HIGHTLIGHT;
	if(extra_fight_mda_data[idx].loop)
		mda_command=mda_command|MDA_COMMAND_LOOP;
	
	if(system_control.system_timer>=extra_fight_mda_data[idx].timer)
	{
		mda_command=mda_command|MDA_COMMAND_NEXT_FRAME;
		extra_fight_mda_data[idx].timer=system_control.system_timer+45;
		
	}

	screen_x=(extra_fight_mda_data[idx].display_x-now_camera_pan_x)*camera_zoom[now_camera_zoom].x_pos_zoom/1000;
	screen_y=(extra_fight_mda_data[idx].display_y-now_camera_pan_y)*camera_zoom[now_camera_zoom].y_pos_zoom/1000;
	
	
	map_layer_touch.flag=false;
	map_layer_touch.character=false;

	strcpy((char *)command_str,(char *)extra_fight_mda_data[idx].command);
	ret_val=mda_group_exec(mda_handle,command_str,screen_x,screen_y,buffer,mda_command,camera_zoom[now_camera_zoom].size_zoom,0,0);
	
	if(ret_val==MDA_EXEC_LAST)
	{
		extra_fight_mda_data[idx].play_end=true;			
		if(extra_fight_mda_data[idx].auto_close)			// 自动关闭
		{
			if(!extra_fight_mda_data[idx].loop)			// 如果不是 loop
			{
				mda_group_close(&extra_fight_mda_data[idx].handle);		// 关闭
				extra_fight_mda_data[idx].handle=-1;
			}
		}
		else
		{
			if(!extra_fight_mda_data[idx].loop)			// 如果不是 loop
			{
				extra_fight_mda_data[idx].display=false;
			}
			
		}
		
	}
	
	
	
}





void auto_camera_pan(SLONG target_zoom,SLONG target_center_x,SLONG target_center_y,SLONG step)
{
	float zoom_offset;
	float x_offset,y_offset;
	SLONG now_center_x;
	SLONG now_center_y;
	SLONG i;
	SLONG old_zoom,old_center_x,old_center_y;
	
	// --------- Initialize 
	now_center_x=now_camera_pan_x+camera_zoom[now_camera_zoom].width/2;
	now_center_y=now_camera_pan_y+camera_zoom[now_camera_zoom].height/2;
	
	zoom_offset=(float)(target_zoom-now_camera_zoom)/(float)step;
	x_offset=(float)(target_center_x-now_center_x)/(float)step;
	y_offset=(float)(target_center_y-now_center_y)/(float)step;
	
	old_zoom=now_camera_zoom;
	old_center_x=now_center_x;
	old_center_y=now_center_y;
	
	fight_control_data.camera_follow_flag=false;

	for(i=1;i<=step;i++)		
	{
		clear_time_delay();
		system_idle_loop();
		now_camera_zoom=old_zoom+(SLONG)(i*zoom_offset);
		now_center_x=old_center_x+(SLONG)(i*x_offset);
		now_center_y=old_center_y+(SLONG)(i*y_offset);
		now_camera_pan_x=now_center_x-camera_zoom[now_camera_zoom].width/2;
		now_camera_pan_y=now_center_y-camera_zoom[now_camera_zoom].height/2;
		
		if(now_camera_zoom<0)
			now_camera_zoom=0;
		if(now_camera_zoom>MAX_CAMERA_ZOOM-1)
			now_camera_zoom=MAX_CAMERA_ZOOM-1;
		if(now_camera_pan_x<0)
			now_camera_pan_x=0;
		if(now_camera_pan_y<0)
			now_camera_pan_y=0;
		if(now_camera_pan_x+camera_zoom[now_camera_zoom].width>=FIGHT_MAP_WIDTH)
			now_camera_pan_x=FIGHT_MAP_WIDTH-camera_zoom[now_camera_zoom].width;
		if(now_camera_pan_y+camera_zoom[now_camera_zoom].height>=FIGHT_MAP_HEIGHT)
			now_camera_pan_y=FIGHT_MAP_HEIGHT-camera_zoom[now_camera_zoom].height;

		process_window_move();						// 处理是否 window 拖移
		fight_camera(screen_buffer);
		exec_screen_text(screen_buffer);			// 执行当前荧幕文字
		draw_main_map(screen_buffer);				// 绘画出地图主menu
		check_fight_main_command();					// 检查玩家命令是否完全
		execute_window_base();						// 绘制 window & 执行
		exec_main_map();
		update_screen(screen_buffer);
		wait_time_delay(FIGHT_MAIN_DELAY);

		fight_control_data.camera_follow_x=now_center_x;
		fight_control_data.camera_follow_y=now_center_y;
		fight_control_data.camera_follow_zoom=now_camera_zoom;
			

	}

	fight_control_data.camera_follow_flag=true;
	
}





void auto_step_camera_pan(SLONG command,SLONG target_zoom,SLONG target_center_x,SLONG target_center_y,SLONG step)
{
	static float zoom_offset;
	static float x_offset,y_offset;
	static SLONG now_center_x;
	static SLONG now_center_y;
	static SLONG old_zoom,old_center_x,old_center_y;
	
	// --------- Initialize 
	switch(command)			
	{
	case AUTO_CAMERA_SET:
		now_center_x=now_camera_pan_x+camera_zoom[now_camera_zoom].width/2;
		now_center_y=now_camera_pan_y+camera_zoom[now_camera_zoom].height/2;
		zoom_offset=(float)(target_zoom-now_camera_zoom)/(float)step;
		x_offset=(float)(target_center_x-now_center_x)/(float)step;
		y_offset=(float)(target_center_y-now_center_y)/(float)step;
		old_zoom=now_camera_zoom;
		old_center_x=now_center_x;
		old_center_y=now_center_y;
		break;
	case AUTO_CAMERA_RUN:
		now_camera_zoom=old_zoom+(SLONG)(step*zoom_offset);
		now_center_x=old_center_x+(SLONG)(step*x_offset);
		now_center_y=old_center_y+(SLONG)(step*y_offset);
		now_camera_pan_x=now_center_x-camera_zoom[now_camera_zoom].width/2;
		now_camera_pan_y=now_center_y-camera_zoom[now_camera_zoom].height/2;
		if(now_camera_zoom<0)
			now_camera_zoom=0;
		if(now_camera_zoom>MAX_CAMERA_ZOOM-1)
			now_camera_zoom=MAX_CAMERA_ZOOM-1;
		if(now_camera_pan_x<0)
			now_camera_pan_x=0;
		if(now_camera_pan_y<0)
			now_camera_pan_y=0;
		if(now_camera_pan_x+camera_zoom[now_camera_zoom].width>=FIGHT_MAP_WIDTH)
			now_camera_pan_x=FIGHT_MAP_WIDTH-camera_zoom[now_camera_zoom].width;
		if(now_camera_pan_y+camera_zoom[now_camera_zoom].height>=FIGHT_MAP_HEIGHT)
			now_camera_pan_y=FIGHT_MAP_HEIGHT-camera_zoom[now_camera_zoom].height;
		break;
	}
}


void push_fight_inst(UCHR *text,ULONG timer)
{

	if(strlen((char *)text)>=256)return;
	
	strcpy((char *)fight_inst_data.inst,(char *)text);

	switch(fight_inst_data.status)
	{
	case 0:									// Scroll in
		break;
	case 1:									// display data
		break;								
	case 2:									// scroll out
		fight_inst_data.status=0;			// 强迫 scroll in
		break;
	default:
		fight_inst_data.status=1;			// 强迫显示
		break;
	}

	fight_inst_data.display_time=timer;
	fight_inst_data.still_timer=system_control.system_timer+fight_inst_data.display_time;	// reset timer

	fight_inst_data.active=true;			// ok

}




void process_fight_camera_task(BMP *buffer)
{


// --- ( 0 - 0 ) display fight screen text
	display_fight_screen_text(buffer);
		

// --- ( 0 - 1 ) display fight inst	
	process_fight_inst(buffer);

}




void process_fight_inst(BMP *buffer)
{
	SLONG ll;

	if(!fight_inst_data.active)return;

	switch(fight_inst_data.status)
	{
	case 0:									// Scroll in
		fight_inst_data.y_pos+=2;
		if(fight_inst_data.y_pos>=8)
		{
			fight_inst_data.y_pos=8;
			fight_inst_data.status=1;
			fight_inst_data.still_timer=system_control.system_timer+fight_inst_data.display_time;	// reset timer
		}
		break;
	case 1:									// display data
		if(system_control.system_timer>=fight_inst_data.still_timer)
		{
			fight_inst_data.status=2;		// change scroll out
		}
		break;								
	case 2:									// scroll out
		fight_inst_data.y_pos-=2;
		if(fight_inst_data.y_pos<-32)		// close
		{
			fight_inst_data.active=false;
			fight_inst_data.y_pos=-32;
			fight_inst_data.status=0;
			fight_inst_data.still_timer=0;
		}
		break;
	default:
		break;
	}


	ll=gium_strlen((char *)fight_inst_data.inst)*8;
	alpha_put_bar(0,fight_inst_data.y_pos,SCREEN_WIDTH,32,color_control.blue,buffer,128);
	print16((SCREEN_WIDTH/2)-(ll/2),fight_inst_data.y_pos+8,fight_inst_data.inst,PEST_PUT,buffer);

}



void wait_first_time_command(void)						// 等待第一次命令
{
	ULONG timer;
	SLONG time_offset;
	SLONG main_pass;

	sprintf((char *)print_rec,MSG_FIGHT_FIRST_INST,FIRST_FIGHT_DELAY_TIME/1000);
	push_fight_inst(print_rec,FIRST_FIGHT_DELAY_TIME-2000);
	
	timer=system_control.system_timer+FIRST_FIGHT_DELAY_TIME;

	
	main_pass=0;
	while(main_pass==0)
	{
		system_idle_loop();

		time_offset=(SLONG)(timer/1000)-(SLONG)(system_control.system_timer/1000);
		if(time_offset<0)
			main_pass=1;

		process_window_move();						// 处理是否 window 拖移

// --- draw 
		fight_camera(screen_buffer);
		exec_screen_text(screen_buffer);			// 执行当前荧幕文字

		if(time_offset>=0)
		{
			sprintf((char *)print_rec,"%d",time_offset);
			display_timer_text(print_rec,screen_buffer);
		}

		draw_main_map(screen_buffer);				// 绘画出地图主menu
		check_fight_main_command();					// 检查玩家命令是否完全
		execute_window_base();						// 绘制 window & 执行
		exec_main_map();
		update_screen(screen_buffer);
	
		if(system_control.system_timer>=timer)		// time out
			main_pass=1;
		
		if(fight_npc_group[1].base.active)		// 有宝宝参战
		{
			if((fight_control_data.character_command_ready)&&(fight_control_data.baobao_command_ready))
				main_pass=1;
		}
		else									// 没有宝宝参战
		{
			if(fight_control_data.character_command_ready)
				main_pass=1;
		}


	}

	fight_inst_data.status=2;

}



void display_timer_text(UCHR *text,BMP *buffer)
{
	ULONG i;
	ULONG ll;
	SLONG mda_index;
	UCHR *image;
	SLONG index;
	SLONG display_x,display_y;

	mda_index=get_mda_index(fight_image.fight_item_handle);
	if(mda_index<0)return;

	ll=strlen((char *)text);
	display_y=100;
	display_x=SCREEN_WIDTH/2-(ll*60)/2+20;
	for(i=0;i<ll;i++)
	{
		index=text[i]-'0';
		get_mda_group_image(mda_index,index,&image);
		if(image!=NULL)			
		{
			display_img256_buffer(display_x,display_y,image,buffer);
			display_x+=60;
		}

	}


}


void exec_fight_action(BMP *buffer)
{
	if(!fight_control_data.fight_action_ready)
		return;
	if(get_fight_npc_index(fight_control_data.action_source_id)<0)	// 没有这个人
	{
		fight_control_data.fight_action_ready=false;
		return;
	}

	if(fight_control_data.charge_flag==true)			// 还在充电#24 error
	{
		return;
	}
	
	switch(fight_control_data.fight_action_command)		// 动作命令 see enum ( FIGHT_ACTION_CODE )
	{
// --- 基本攻击部分
	case FIGHT_ACTION_SINGLE_PHYSICS:			// 物理单攻击 ( 含 feedback,致命, 狂暴 )
												// FIGHT_ACTION_SINGLE_PHYSICS_DATA	
		exec_fight_action_single_physica();
		break;
	case FIGHT_ACTION_SINGLE_PHYSICS_DOUBLE:	// 物理连击 ( 含 feedback, 致命，狂暴 )
												// FIGHT_ACTION_SINGLE_PHYSICS_DOUBLE_DATA
		exec_fight_action_single_physica_double();
		break;
	case FIGHT_ACTION_SINGLE_MAGIC:				// 法术单攻击 ( 含 feedback, 狂暴 )
												// FIGHT_ACTION_SINGLE_MAGIC_DATA
		exec_fight_action_single_magic();
		break;
	case FIGHT_ACTION_SINGLE_MAGIC_DOUBLE:		// 法术连击单攻击 ( 含 feedback, 狂暴 )
												// FIGHT_ACTION_SINGLE_MAGIC_DOUBLE_DATA
		break;
	case FIGHT_ACTION_ITEM:						// 道具单人
		break;
	case FIGHT_ACTION_RUN:						// 逃跑单人
		break;
	case FIGHT_ACTION_CHANGE:					// 更换宝宝单人
		break;
	case FIGHT_ACTION_SINGLE_SUCK:				// 吸取单人
												// FIGHT_ACTION_SINGLE_SUCK_DATA
		break;
	case FIGHT_ACTION_SINGLE_VIRUS:				// 中毒单人
												// FIGHT_ACTION_SINGLE_VIRUS_DATA
		break;
	case FIGHT_ACTION_SINGLE_STATUS:			// 状态单人(石化,混,睡,死亡宣告.... )
		break;			
		
		
		
// ---- 法术技能部分
	case FIGHT_ACTION_MULTI_PHYSICS:			// 物理多攻击 ( 含 feedback,致命, 狂暴 )
												// FIGHT_ACTION_MULTI_PHYSICS_DATA
		break;
	case FIGHT_ACTION_MULTI_PHYSICS_DOUBLE:		// 物理多连击 ( 含 feedback, 致命，狂暴 )
												// FIGHT_ACTION_MULTI_PHYSICS_DOUBLE_DATA
		break;
	case FIGHT_ACTION_MULTI_MAGIC:				// 法术多攻击 ( 含 feedback, 狂暴 )
												// FIGHT_ACTION_MULTI_MAGIC_DATA
		break;
	case FIGHT_ACTION_MULTI_MAGIC_DOUBLE:		// 法术连击多攻击 ( 含 feedback, 狂暴 )
												// FIGHT_ACTION_MULTI_MAGIC_DOUBLE_DATA
		break;
	case FIGHT_ACTION_MULIT_SUCK:				// 吸取多人
												// FIGHT_ACTION_MULIT_SUCK_DATA
		break;
	case FIGHT_ACTION_MULTI_VIRUS:				// 中毒多人
												// FIGHT_ACTION_MULTI_VIRUS_DATA
		break;
	case FIGHT_ACTION_MULTI_STATUS:				// 状态多人(石化,混,睡,死亡宣告.... )
												// FIGHT_ACTION_MULTI_STATUS_DATA
		break;
	default:
		display_error_message((UCHR *)"Fight Command Error!",1);
		break;
		
	}
	






	fight_control_data.fight_action_ready=false;
	
}






void auto_camera_follow(void)
{
	SLONG target_x;
	SLONG target_y;
	SLONG target_zoom;
	SLONG now_center_x,now_center_y;


	target_x=fight_control_data.camera_follow_x;
	target_y=fight_control_data.camera_follow_y;
	target_zoom=fight_control_data.camera_follow_zoom;


	now_center_x=now_camera_pan_x+camera_zoom[now_camera_zoom].width/2;
	now_center_y=now_camera_pan_y+camera_zoom[now_camera_zoom].height/2;
	

	if(target_zoom<now_camera_zoom)
		now_camera_zoom--;
	else if(target_zoom>now_camera_zoom)
		now_camera_zoom++;

	if(target_x<now_center_x)
		now_camera_pan_x--;		
	else if(target_x>now_center_x)
		now_camera_pan_x++;


	if(target_y<now_center_y)
		now_camera_pan_y--;		
	else if(target_y>now_center_y)
		now_camera_pan_y++;
	

	if(now_camera_pan_x<0)
		now_camera_pan_x=0;
	if(now_camera_pan_y<0)
		now_camera_pan_y=0;
	if(now_camera_pan_x+camera_zoom[now_camera_zoom].width>=FIGHT_MAP_WIDTH)
		now_camera_pan_x=FIGHT_MAP_WIDTH-camera_zoom[now_camera_zoom].width;
	if(now_camera_pan_y+camera_zoom[now_camera_zoom].height>=FIGHT_MAP_HEIGHT)
		now_camera_pan_y=FIGHT_MAP_HEIGHT-camera_zoom[now_camera_zoom].height;
	
}


void add_fight_screen_text(SLONG x,SLONG y,SLONG type,ULONG timer,UCHR *text)
{
	SLONG idx;
	SLONG i;

	for(i=0;i<MAX_FIGHT_SCREEN_TEXT;i++)
	{
		if(fight_screen_text_data[i].active)continue;
		idx=i;
		i=MAX_FIGHT_SCREEN_TEXT;
		break;
	}

	fight_screen_text_data[idx].still_timer=system_control.system_timer+timer;
	fight_screen_text_data[idx].x=x;
	fight_screen_text_data[idx].y=y;
	fight_screen_text_data[idx].type=type;
	if(strlen((char *)text)>=79)
		text[79]=NULL;
	strcpy((char *)fight_screen_text_data[idx].text,(char *)text);
	fight_screen_text_data[idx].active=true;

}


void display_fight_screen_text(BMP *buffer)
{
	SLONG i;
	SLONG display_x,display_y;
	
	for(i=0;i<MAX_FIGHT_SCREEN_TEXT;i++)
	{
		if(!fight_screen_text_data[i].active)continue;
		display_x=(fight_screen_text_data[i].x-now_camera_pan_x)*camera_zoom[now_camera_zoom].x_pos_zoom/1000;
		display_y=(fight_screen_text_data[i].y-now_camera_pan_y)*camera_zoom[now_camera_zoom].y_pos_zoom/1000;

		if((fight_screen_text_data[i].text[0]=='H')||
			(fight_screen_text_data[i].text[0]=='h'))
			fight_screen_text_data[i].y-=0;
		else
			fight_screen_text_data[i].y-=3;			

		switch(fight_screen_text_data[i].type)
		{
		case 0:
			display_number_image0(display_x,display_y,fight_screen_text_data[i].text,buffer);
			break;
		case 1:
			display_number_image1(display_x,display_y,fight_screen_text_data[i].text,buffer);
			break;
		default:
			display_number_image0(display_x,display_y,fight_screen_text_data[i].text,buffer);
			break;
		}

		if(system_control.system_timer>=fight_screen_text_data[i].still_timer)
			fight_screen_text_data[i].active=0;
	}
}




void display_number_image0(SLONG display_x,SLONG display_y,UCHR *text,BMP *buffer)
{
	ULONG i;
	ULONG ll;
	SLONG number_mda_index;
	SLONG text_mda_index;
	UCHR *image;
	SLONG index;
	IMG256_HEAD *temp_head;
	SLONG hit_mda_index;
	bool hight_light;
	
	number_mda_index=get_mda_index(fight_image.number0_handle);
	if(number_mda_index<0)return;

	text_mda_index=get_mda_index(fight_image.fullmiss_handle);
	if(text_mda_index<0)return;

	hit_mda_index=get_mda_index(fight_image.fight_hit_handle);
	if(hit_mda_index<0)return;
	
	
	ll=strlen((char *)text);
	for(i=0;i<ll;i++)
	{
		hight_light=false;
		switch(text[i])
		{
		case '0':case '1':case '2':case '3':case '4':
		case '5':case '6':case '7':case '8':case '9':
			index=text[i]-'0';
			get_mda_group_image(number_mda_index,index,&image);
			break;
		case ' ':
			image=NULL;
			break;
		case 'm':case 'M':
			get_mda_group_image(text_mda_index,1,&image);
			break;
		case 'f':case 'F':
			get_mda_group_image(text_mda_index,0,&image);
			break;
		case 'h': case 'H':
			hight_light=true;
			get_mda_group_image(hit_mda_index,0,&image);
			break;
		default:
			image=NULL;
			continue;
			break;
		}

		if(image!=NULL)			
		{
			if(hight_light)
				display_hightlight_img256_buffer(display_x,display_y,image,buffer);
			else
				display_img256_buffer(display_x,display_y,image,buffer);
			temp_head=(IMG256_HEAD *)image;
			display_x+=temp_head->xlen;
		}
		
	}
	
	
}


void display_number_image1(SLONG display_x,SLONG display_y,UCHR *text,BMP *buffer)
{
	ULONG i;
	ULONG ll;
	SLONG number_mda_index;
	SLONG text_mda_index;
	SLONG hit_mda_index;
	UCHR *image;
	SLONG index;
	IMG256_HEAD *temp_head;
	bool hight_light;
	
	number_mda_index=get_mda_index(fight_image.number1_handle);
	if(number_mda_index<0)return;
	
	text_mda_index=get_mda_index(fight_image.fullmiss_handle);
	if(text_mda_index<0)return;
	
	hit_mda_index=get_mda_index(fight_image.fight_hit_handle);
	if(hit_mda_index<0)return;
	
	ll=strlen((char *)text);
	for(i=0;i<ll;i++)
	{
		hight_light=false;
		switch(text[i])
		{
		case '0':case '1':case '2':case '3':case '4':
		case '5':case '6':case '7':case '8':case '9':
			index=text[i]-'0';
			get_mda_group_image(number_mda_index,index,&image);
			break;
		case ' ':
			image=NULL;
			break;
		case 'm':case 'M':
			get_mda_group_image(text_mda_index,1,&image);
			break;
		case 'f':case 'F':
			get_mda_group_image(text_mda_index,0,&image);
			break;
		case 'h': case 'H':
			hight_light=true;
			get_mda_group_image(hit_mda_index,0,&image);
			break;
		default:
			image=NULL;
			continue;
			break;
		}
		
		if(image!=NULL)			
		{
			if(hight_light)
				display_hightlight_img256_buffer(display_x,display_y,image,buffer);
			else
				display_img256_buffer(display_x,display_y,image,buffer);
			temp_head=(IMG256_HEAD *)image;
			display_x+=temp_head->xlen;
		}
		
	}
	
	
}



void update_fight_result(SLONG idx)
{
	if(!fight_npc_group[idx].base.active)
		return;

	fight_npc_group[idx].base.charge_speed=fight_npc_group[idx].result.charge_speed;
	fight_npc_group[idx].base.now_hp=fight_npc_group[idx].result.now_hp;
	fight_npc_group[idx].base.max_hp=fight_npc_group[idx].result.max_hp;
	fight_npc_group[idx].base.now_mp=fight_npc_group[idx].result.now_mp;
	fight_npc_group[idx].base.max_mp=fight_npc_group[idx].result.max_mp;
	fight_npc_group[idx].base.status=fight_npc_group[idx].result.status;

}



/*******************************************************************************************************************}
{																													}
{	战斗动画播放																									}
{																													}
********************************************************************************************************************/
/*******************************************************************************************************************}
{																													}
{	物理单攻击 ( 含 feedback,致命, 狂暴 )																			}
{																													}
********************************************************************************************************************/
void exec_fight_action_single_physica(void)
{
	SLONG source_idx,target_idx,protect_idx;
	SLONG blood,feed_back_blood;
	FIGHT_NPC_GROUP *source_data;
	FIGHT_NPC_GROUP *target_data;
	FIGHT_NPC_GROUP *protect_data;
	SLONG source_dir;
	SLONG target_dir;
	SLONG attack_no;
	SLONG total_frame;

	SLONG target_backup_x,target_backup_y;
	SLONG target_hurt_offset_x,target_hurt_offset_y;
	SLONG real_blood;

	SLONG i;
	POSITION offset_pp[5];

	bool protect;
	SLONG protect_x,protect_y,protect_dir;
	

// --- 准备资料
	source_idx=get_fight_npc_index(fight_control_data.action_source_id);
	target_idx=get_fight_npc_index(fight_control_data.action_target_id);
	if((source_idx<0)||(target_idx<0))
		return;
	source_data=&fight_npc_group[source_idx];
	target_data=&fight_npc_group[target_idx];

	source_dir=source_data->dir;
	target_dir=target_data->dir;

	blood=fight_action_single_physics_data.hp;
	feed_back_blood=fight_action_single_physics_data.feed_back_hp;

	protect=false;
	protect_idx=-1;
	if(fight_action_single_physics_data.protect_id!=fight_control_data.action_source_id)
	{
		if(fight_action_single_physics_data.protect_id!=0)	
		{
			protect_idx=get_fight_npc_index(fight_action_single_physics_data.protect_id);
			if(protect_idx>=0)
			{
				protect_data=&fight_npc_group[protect_idx];
				protect_x=protect_data->x;
				protect_y=protect_data->y;
				protect_dir=protect_data->dir;
				protect=true;
			}
		}
	}
	
	

	offset_pp[0].x=source_data->x;
	offset_pp[0].y=source_data->y;
	offset_pp[4].x=target_data->x;
	offset_pp[4].y=target_data->y;
	target_backup_x=target_data->x;
	target_backup_y=target_data->y;
	
	if(target_idx>=10)			// 对面 在左边
	{
		offset_pp[4].x+=80;	// 
		target_hurt_offset_x=-50;
		target_hurt_offset_y=50;
		
	}
	else						// 我方
	{
		offset_pp[4].x-=80;	// 
		target_hurt_offset_x=50;
		target_hurt_offset_y=50;
	}


	offset_pp[2].x=(offset_pp[0].x+offset_pp[4].x)/2;
	offset_pp[2].y=(offset_pp[0].y+offset_pp[4].y)/2;
	offset_pp[1].x=(offset_pp[0].x+offset_pp[2].x)/2;
	offset_pp[1].y=(offset_pp[0].y+offset_pp[2].y)/2;
	offset_pp[3].x=(offset_pp[2].x+offset_pp[4].x)/2;
	offset_pp[3].y=(offset_pp[2].y+offset_pp[4].y)/2;


	fight_control_data.camera_follow_x=offset_pp[4].x;
	fight_control_data.camera_follow_y=offset_pp[4].y;
	fight_control_data.camera_follow_flag=true;
	

// --- ( 0 - 0 )  step 1 移动到位置
	source_data->motion=FIGHT_MOTION_RUN;
	for(i=0;i<5;i++)
	{
		clear_time_delay();
		system_idle_loop();

		source_data->x=offset_pp[i].x;
		source_data->y=offset_pp[i].y;
		
		process_window_move();						// 处理是否 window 拖移


		fight_camera(screen_buffer);
		exec_screen_text(screen_buffer);			// 执行当前荧幕文字
		draw_main_map(screen_buffer);				// 绘画出地图主menu
		
		check_fight_main_command();					// 检查玩家命令是否完全
		execute_window_base();						// 绘制 window & 执行
		exec_main_map();
		update_screen(screen_buffer);
		wait_time_delay(FIGHT_MAIN_DELAY);

	}

// ------ change dir
	if(source_data->x>target_data->x)
	{
		source_data->dir=FIGHT_NPC_DIR_LEFT;
	}
	else
	{
		source_data->dir=FIGHT_NPC_DIR_RIGHT;
	}


// --- Attack 
	total_frame=0;
	attack_no=rand()%3;
	switch(attack_no)
	{
	case 0:
		total_frame=source_data->attack1_frame[source_data->dir].total_frame;
		source_data->motion=FIGHT_MOTION_ATTACK1;
		break;
	case 1:
		total_frame=source_data->attack2_frame[source_data->dir].total_frame;
		source_data->motion=FIGHT_MOTION_ATTACK2;
		break;
	case 2:
		total_frame=source_data->attack3_frame[source_data->dir].total_frame;
		source_data->motion=FIGHT_MOTION_ATTACK3;
		break;
	}


	attack_start=0;
	attack_value=100;
	
	for(i=0;i<total_frame;i++)
	{
		clear_time_delay();
		system_idle_loop();
		
		switch(attack_no)
		{
		case 0:
			source_data->attack1_frame[source_data->dir].now_frame=i;
			break;
		case 1:
			source_data->attack2_frame[source_data->dir].now_frame=i;
			break;
		case 2:
			source_data->attack3_frame[source_data->dir].now_frame=i;
			break;
		}


		if(attack_start==1)
		{
			if(blood>0)		// 扣血
			{
				add_fight_screen_text(target_data->x,target_data->y,0,100,(UCHR *)"H");
				real_blood=blood*attack_value/100;
				sprintf((char *)print_rec,"%d",real_blood);
				add_fight_screen_text(target_data->x,target_data->y-80,1,2000,print_rec);
				target_data->motion=FIGHT_MOTION_HURT;
				target_data->x+=target_hurt_offset_x;
					
			}
			else			// miss
			{
				add_fight_screen_text(target_data->x,target_data->y-50,1,2000,(UCHR *)"M");
				target_data->x+=target_hurt_offset_x;
				target_data->y+=target_hurt_offset_y;
			}

			attack_start=0;							// clear attack flag
		}

		if(protect)
		{
			protect_data->x=(source_data->x+target_data->x)/2;
			protect_data->y=(source_data->y+target_data->y)/2;
			protect_data->motion=FIGHT_MOTION_HURT;
			protect_data->dir=target_data->dir;
		}
		
		process_window_move();						// 处理是否 window 拖移
		fight_camera(screen_buffer);
		exec_screen_text(screen_buffer);			// 执行当前荧幕文字
		draw_main_map(screen_buffer);				// 绘画出地图主menu
		
		check_fight_main_command();					// 检查玩家命令是否完全
		execute_window_base();						// 绘制 window & 执行
		exec_main_map();
		update_screen(screen_buffer);
		wait_time_delay(FIGHT_MAIN_DELAY);
		
	}

	

// ---- source back
	target_data->x=target_backup_x;
	target_data->y=target_backup_y;
	target_data->motion=FIGHT_MOTION_STANDBY;

	
// ------ 检查是否反击
	if(fight_action_single_physics_data.back_hp>0)
	{
		blood=fight_action_single_physics_data.back_hp;
		total_frame=0;
		attack_no=rand()%3;
		switch(attack_no)
		{
		case 0:
			total_frame=target_data->attack1_frame[target_data->dir].total_frame;
			target_data->motion=FIGHT_MOTION_ATTACK1;
			break;
		case 1:
			total_frame=target_data->attack2_frame[target_data->dir].total_frame;
			target_data->motion=FIGHT_MOTION_ATTACK2;
			break;
		case 2:
			total_frame=target_data->attack3_frame[target_data->dir].total_frame;
			target_data->motion=FIGHT_MOTION_ATTACK3;
			break;
		}
		

		attack_start=0;
		attack_value=100;
		
		for(i=0;i<total_frame;i++)
		{
			clear_time_delay();
			system_idle_loop();
			
			switch(attack_no)
			{
			case 0:
				target_data->attack1_frame[target_data->dir].now_frame=i;
				break;
			case 1:
				target_data->attack2_frame[target_data->dir].now_frame=i;
				break;
			case 2:
				target_data->attack3_frame[target_data->dir].now_frame=i;
				break;
			}
			
			
			if(attack_start==1)
			{
				add_fight_screen_text(source_data->x,source_data->y,0,100,(UCHR *)"H");
				real_blood=blood*attack_value/100;
				sprintf((char *)print_rec,"%d",real_blood);
				add_fight_screen_text(source_data->x,source_data->y-80,1,2000,print_rec);
				source_data->motion=FIGHT_MOTION_HURT;
				source_data->x+=target_hurt_offset_x;
				
				attack_start=0;							// clear attack flag
			}
			
			
			process_window_move();						// 处理是否 window 拖移
			fight_camera(screen_buffer);
			exec_screen_text(screen_buffer);			// 执行当前荧幕文字
			draw_main_map(screen_buffer);				// 绘画出地图主menu
			
			check_fight_main_command();					// 检查玩家命令是否完全
			execute_window_base();						// 绘制 window & 执行
			exec_main_map();
			update_screen(screen_buffer);
			wait_time_delay(FIGHT_MAIN_DELAY);
			
		}
		

	}
	


	target_data->x=target_backup_x;
	target_data->y=target_backup_y;
	target_data->motion=FIGHT_MOTION_STANDBY;
	

	source_data->motion=FIGHT_MOTION_RUN;
	if(offset_pp[4].x>offset_pp[0].x)
		source_data->dir=FIGHT_NPC_DIR_LEFT;
	else
		source_data->dir=FIGHT_NPC_DIR_RIGHT;


	if(protect)
	{
		protect_data->x=protect_x;
		protect_data->y=protect_y;
		protect_data->dir=protect_dir;
		protect_data->motion=FIGHT_MOTION_STANDBY;
	}
				

	for(i=4;i>=0;i--)
	{
		clear_time_delay();
		system_idle_loop();
		
		source_data->x=offset_pp[i].x;
		source_data->y=offset_pp[i].y;
		
		process_window_move();						// 处理是否 window 拖移
		fight_camera(screen_buffer);

		
		exec_screen_text(screen_buffer);			// 执行当前荧幕文字
		draw_main_map(screen_buffer);				// 绘画出地图主menu
		
		check_fight_main_command();					// 检查玩家命令是否完全
		execute_window_base();						// 绘制 window & 执行
		exec_main_map();
		update_screen(screen_buffer);
		wait_time_delay(FIGHT_MAIN_DELAY);
		
	}
	

	if(fight_action_single_physics_data.feed_back_hp>0)		// feed back
	{
		sprintf((char *)print_rec,"%d",fight_action_single_physics_data.feed_back_hp);
		add_fight_screen_text(source_data->x,source_data->y-50,1,1000,print_rec);
	}
	


	auto_camera_pan(fight_default_pos.zoom_index,fight_default_pos.center_x,fight_default_pos.center_y,12);
	
	fight_control_data.camera_follow_zoom=fight_default_pos.zoom_index;
	fight_control_data.camera_follow_x=fight_default_pos.center_x;
	fight_control_data.camera_follow_y=fight_default_pos.center_y;
	fight_control_data.camera_follow_flag=true;
	
	
// --- restore 
	source_data->dir=source_dir;
	target_data->dir=target_dir;
	source_data->x=offset_pp[0].x;
	source_data->y=offset_pp[0].y;
	source_data->motion=FIGHT_MOTION_STANDBY;


	update_fight_result(source_idx);
	update_fight_result(target_idx);
	
}




/*******************************************************************************************************************}
{																													}
{	物理连击 ( 含 feedback, 致命，狂暴 )																			}
{																													}
********************************************************************************************************************/
void exec_fight_action_single_physica_double(void)
{
	SLONG source_idx,target_idx;
	SLONG blood,feed_back_blood;
	FIGHT_NPC_GROUP *source_data;
	FIGHT_NPC_GROUP *target_data;
	SLONG source_dir;
	SLONG target_dir;
	SLONG attack_no;
	SLONG total_frame;
	SLONG double_count,ccl;
	SLONG text_mda_index;

	SLONG target_backup_x,target_backup_y;
	SLONG target_hurt_offset_x,target_hurt_offset_y;
	SLONG real_blood;

	SLONG backup_x;

	SLONG i;
	UCHR *image;
	POSITION offset_pp[5];


// --- 准备资料
	source_idx=get_fight_npc_index(fight_control_data.action_source_id);
	target_idx=get_fight_npc_index(fight_control_data.action_target_id);
	if((source_idx<0)||(target_idx<0))
		return;

	source_data=&fight_npc_group[source_idx];
	target_data=&fight_npc_group[target_idx];

	source_dir=source_data->dir;
	target_dir=target_data->dir;



	offset_pp[0].x=source_data->x;
	offset_pp[0].y=source_data->y;
	offset_pp[4].x=target_data->x;
	offset_pp[4].y=target_data->y;
	target_backup_x=target_data->x;
	target_backup_y=target_data->y;
	
	if(target_idx>=10)			// 对面 在左边
	{
		offset_pp[4].x+=80;	// 
		target_hurt_offset_x=-50;
		target_hurt_offset_y=50;
		
	}
	else						// 我方
	{
		offset_pp[4].x-=80;	// 
		target_hurt_offset_x=50;
		target_hurt_offset_y=50;
	}


	offset_pp[2].x=(offset_pp[0].x+offset_pp[4].x)/2;
	offset_pp[2].y=(offset_pp[0].y+offset_pp[4].y)/2;
	offset_pp[1].x=(offset_pp[0].x+offset_pp[2].x)/2;
	offset_pp[1].y=(offset_pp[0].y+offset_pp[2].y)/2;
	offset_pp[3].x=(offset_pp[2].x+offset_pp[4].x)/2;
	offset_pp[3].y=(offset_pp[2].y+offset_pp[4].y)/2;


	fight_control_data.camera_follow_x=offset_pp[4].x;
	fight_control_data.camera_follow_y=offset_pp[4].y;
	fight_control_data.camera_follow_flag=true;
	

	text_mda_index=get_mda_index(fight_image.number2_handle);
	
// --- ( 0 - 0 )  step 1 移动到位置
	source_data->motion=FIGHT_MOTION_RUN;
	for(i=0;i<5;i++)
	{
		clear_time_delay();
		system_idle_loop();

		source_data->x=offset_pp[i].x;
		source_data->y=offset_pp[i].y;
		
		process_window_move();						// 处理是否 window 拖移


		fight_camera(screen_buffer);
		exec_screen_text(screen_buffer);			// 执行当前荧幕文字
		draw_main_map(screen_buffer);				// 绘画出地图主menu
		
		mda_group_exec(fight_image.double_handle,(UCHR *)"MainCOmmand0",i*(400/5),430,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
		
		check_fight_main_command();					// 检查玩家命令是否完全
		execute_window_base();						// 绘制 window & 执行
		exec_main_map();
		update_screen(screen_buffer);
		wait_time_delay(FIGHT_MAIN_DELAY);

	}

// ------ change dir
	if(source_data->x>target_data->x)
	{
		source_data->dir=FIGHT_NPC_DIR_LEFT;
	}
	else
	{
		source_data->dir=FIGHT_NPC_DIR_RIGHT;
	}


// --- Attack 
	double_count=fight_action_single_physics_double_data.double_count;
	for(ccl=0;ccl<double_count;ccl++)
	{

		blood=fight_action_single_physics_double_data.hp[ccl];
		

		total_frame=0;
		attack_no=rand()%3;
		switch(attack_no)
		{
		case 0:
			total_frame=source_data->attack1_frame[source_data->dir].total_frame;
			source_data->motion=FIGHT_MOTION_ATTACK1;
			break;
		case 1:
			total_frame=source_data->attack2_frame[source_data->dir].total_frame;
			source_data->motion=FIGHT_MOTION_ATTACK2;
			break;
		case 2:
			total_frame=source_data->attack3_frame[source_data->dir].total_frame;
			source_data->motion=FIGHT_MOTION_ATTACK3;
			break;
		}
		
		
		attack_start=0;
		attack_value=100;
		
		for(i=0;i<total_frame;i++)
		{
			clear_time_delay();
			system_idle_loop();
			
			switch(attack_no)
			{
			case 0:
				source_data->attack1_frame[source_data->dir].now_frame=i;
				break;
			case 1:
				source_data->attack2_frame[source_data->dir].now_frame=i;
				break;
			case 2:
				source_data->attack3_frame[source_data->dir].now_frame=i;
				break;
			}
			
			
			if(attack_start==1)
			{
				if(blood>0)		// 扣血
				{
					add_fight_screen_text(target_data->x,target_data->y,0,100,(UCHR *)"H");
					real_blood=blood*attack_value/100;
					sprintf((char *)print_rec,"%d",real_blood);
					add_fight_screen_text(target_data->x,target_data->y-80,1,2000,print_rec);
					target_data->motion=FIGHT_MOTION_HURT;
					target_data->x+=target_hurt_offset_x;
					
				}
				else			// miss
				{
					add_fight_screen_text(target_data->x,target_data->y-50,1,2000,(UCHR *)"M");
					target_data->x+=target_hurt_offset_x;
					target_data->y+=target_hurt_offset_y;
				}
				
				attack_start=0;							// clear attack flag
			}
			
			
			process_window_move();						// 处理是否 window 拖移
			fight_camera(screen_buffer);
			exec_screen_text(screen_buffer);			// 执行当前荧幕文字
			draw_main_map(screen_buffer);				// 绘画出地图主menu

			mda_group_exec(fight_image.double_handle,(UCHR *)"MainCOmmand0",400,430,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
			get_mda_group_image(text_mda_index,ccl+1,&image);
			if(image!=NULL)
				display_img256_buffer(400-20,430-30,image,screen_buffer);
			

			check_fight_main_command();					// 检查玩家命令是否完全
			execute_window_base();						// 绘制 window & 执行
			exec_main_map();
			update_screen(screen_buffer);
			wait_time_delay(FIGHT_MAIN_DELAY);
			
			
		}
		
		source_data->motion=FIGHT_MOTION_STANDBY;
		
		// ---- source back
		target_data->x=target_backup_x;
		target_data->y=target_backup_y;
		target_data->motion=FIGHT_MOTION_STANDBY;


		// ------ 检查是否反击
		if(fight_action_single_physics_double_data.back_hp[ccl]>0)
		{
			blood=fight_action_single_physics_double_data.back_hp[ccl];
			total_frame=0;
			attack_no=rand()%3;
			switch(attack_no)
			{
			case 0:
				total_frame=target_data->attack1_frame[target_data->dir].total_frame;
				target_data->motion=FIGHT_MOTION_ATTACK1;
				break;
			case 1:
				total_frame=target_data->attack2_frame[target_data->dir].total_frame;
				target_data->motion=FIGHT_MOTION_ATTACK2;
				break;
			case 2:
				total_frame=target_data->attack3_frame[target_data->dir].total_frame;
				target_data->motion=FIGHT_MOTION_ATTACK3;
				break;
			}
			
			
			attack_start=0;
			attack_value=100;
			
			for(i=0;i<total_frame;i++)
			{
				clear_time_delay();
				system_idle_loop();
				
				switch(attack_no)
				{
				case 0:
					target_data->attack1_frame[target_data->dir].now_frame=i;
					break;
				case 1:
					target_data->attack2_frame[target_data->dir].now_frame=i;
					break;
				case 2:
					target_data->attack3_frame[target_data->dir].now_frame=i;
					break;
				}
				

				backup_x=source_data->x;
				if(attack_start==1)
				{

					add_fight_screen_text(source_data->x,source_data->y,0,100,(UCHR *)"H");
					real_blood=blood*attack_value/100;
					sprintf((char *)print_rec,"%d",real_blood);
					add_fight_screen_text(source_data->x,source_data->y-80,1,2000,print_rec);
					source_data->motion=FIGHT_MOTION_HURT;
					source_data->x+=target_hurt_offset_x;
					
					attack_start=0;							// clear attack flag
				}
				
				
				process_window_move();						// 处理是否 window 拖移
				fight_camera(screen_buffer);
				exec_screen_text(screen_buffer);			// 执行当前荧幕文字
				draw_main_map(screen_buffer);				// 绘画出地图主menu

				mda_group_exec(fight_image.double_handle,(UCHR *)"MainCOmmand0",400,430,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
				get_mda_group_image(text_mda_index,ccl+1,&image);
				if(image!=NULL)
					display_img256_buffer(400-20,430-30,image,screen_buffer);
				
				check_fight_main_command();					// 检查玩家命令是否完全
				execute_window_base();						// 绘制 window & 执行
				exec_main_map();
				update_screen(screen_buffer);
				wait_time_delay(FIGHT_MAIN_DELAY);
				

				source_data->x=backup_x;
				
			}
			
		}
		

		
	}



	// ---- source back
	target_data->x=target_backup_x;
	target_data->y=target_backup_y;
	target_data->motion=FIGHT_MOTION_STANDBY;
	
	
	source_data->motion=FIGHT_MOTION_RUN;
	if(offset_pp[4].x>offset_pp[0].x)
		source_data->dir=FIGHT_NPC_DIR_LEFT;
	else
		source_data->dir=FIGHT_NPC_DIR_RIGHT;

	

	for(i=4;i>=0;i--)
	{
		clear_time_delay();
		system_idle_loop();
		
		source_data->x=offset_pp[i].x;
		source_data->y=offset_pp[i].y;
		
		process_window_move();						// 处理是否 window 拖移
		fight_camera(screen_buffer);

		
		exec_screen_text(screen_buffer);			// 执行当前荧幕文字
		draw_main_map(screen_buffer);				// 绘画出地图主menu

		
		check_fight_main_command();					// 检查玩家命令是否完全
		execute_window_base();						// 绘制 window & 执行
		exec_main_map();
		update_screen(screen_buffer);
		wait_time_delay(FIGHT_MAIN_DELAY);
		
	}
	

	feed_back_blood=0;
	for(ccl=0;ccl<double_count;ccl++)
	{
		feed_back_blood+=fight_action_single_physics_double_data.feedback[ccl];
	}
		
	if(feed_back_blood>0)		// feed back
	{
		sprintf((char *)print_rec,"%d",feed_back_blood);
		add_fight_screen_text(source_data->x,source_data->y-50,1,1000,print_rec);
	}
	


	auto_camera_pan(fight_default_pos.zoom_index,fight_default_pos.center_x,fight_default_pos.center_y,12);
	
	fight_control_data.camera_follow_zoom=fight_default_pos.zoom_index;
	fight_control_data.camera_follow_x=fight_default_pos.center_x;
	fight_control_data.camera_follow_y=fight_default_pos.center_y;
	fight_control_data.camera_follow_flag=true;
	
	
// --- restore 
	source_data->dir=source_dir;
	target_data->dir=target_dir;
	source_data->x=offset_pp[0].x;
	source_data->y=offset_pp[0].y;
	source_data->motion=FIGHT_MOTION_STANDBY;


	update_fight_result(source_idx);
	update_fight_result(target_idx);
	
}





/*******************************************************************************************************************}
{																													}
{	法术单攻击 ( 含 feedback,致命, 狂暴 )																			}
{																													}
********************************************************************************************************************/
void exec_fight_action_single_magic(void)
{
	SLONG source_idx,target_idx;
	SLONG blood,feed_back_blood;
	FIGHT_NPC_GROUP *source_data;
	FIGHT_NPC_GROUP *target_data;

	SLONG source_dir;
	SLONG target_dir;

	SLONG total_frame;
	SLONG real_blood;

	SLONG i;

	UCHR magic_filename[80];
	SLONG extra_index;
	SLONG count;
	SLONG main_pass;

// --- 准备资料
	source_idx=get_fight_npc_index(fight_control_data.action_source_id);
	target_idx=get_fight_npc_index(fight_control_data.action_target_id);
	if((source_idx<0)||(target_idx<0))
		return;
	source_data=&fight_npc_group[source_idx];
	target_data=&fight_npc_group[target_idx];

	source_dir=source_data->dir;
	target_dir=target_data->dir;

	blood=fight_action_single_magic_data.hp;
	feed_back_blood=fight_action_single_magic_data.feed_back_hp;

	sprintf((char *)magic_filename,"ani\\skl%03d.mda",skill_base_data[fight_control_data.action_idx].mda_filename_id);

// --- magic
	total_frame=0;
	total_frame=source_data->magic_frame[source_data->dir].total_frame;
	source_data->motion=FIGHT_MOTION_MAGIC;

	attack_start=0;
	attack_value=100;
	for(i=0;i<total_frame;i++)
	{
		clear_time_delay();
		system_idle_loop();
		
		source_data->magic_frame[source_data->dir].now_frame=i;


		process_window_move();						// 处理是否 window 拖移
		fight_camera(screen_buffer);
		exec_screen_text(screen_buffer);			// 执行当前荧幕文字
		draw_main_map(screen_buffer);				// 绘画出地图主menu
		
		check_fight_main_command();					// 检查玩家命令是否完全
		execute_window_base();						// 绘制 window & 执行
		exec_main_map();
		update_screen(screen_buffer);
		wait_time_delay(FIGHT_MAIN_DELAY);
		
	}

	
// --- restore 
	source_data->dir=source_dir;
	target_data->dir=target_dir;
	source_data->motion=FIGHT_MOTION_STANDBY;
	target_data->motion=FIGHT_MOTION_STANDBY;

	extra_index=add_extra_fight_mda(magic_filename,target_data->x,target_data->y,false,true,true,EXTRA_MIDDLE_LAYER,(UCHR *)"MainCommand0");
	magic_start=0;							// clear attack flag
	
		
	main_pass=0;
	count=0;
	while(main_pass==0)
	{
		clear_time_delay();
		system_idle_loop();

		if(attack_start==1)
		{
			add_fight_screen_text(target_data->x,target_data->y,0,100,(UCHR *)"H");
			real_blood=blood*attack_value/100;
			sprintf((char *)print_rec,"%d",real_blood);
			add_fight_screen_text(target_data->x,target_data->y-80,1,2000,print_rec);
			target_data->motion=FIGHT_MOTION_HURT;
			attack_start=0;							// clear attack flag
		}
		
		process_window_move();						// 处理是否 window 拖移
		fight_camera(screen_buffer);
		exec_screen_text(screen_buffer);			// 执行当前荧幕文字
		draw_main_map(screen_buffer);				// 绘画出地图主menu
		
		check_fight_main_command();					// 检查玩家命令是否完全
		execute_window_base();						// 绘制 window & 执行
		exec_main_map();
		update_screen(screen_buffer);
		wait_time_delay(FIGHT_MAIN_DELAY);

		if((extra_fight_mda_data[extra_index].handle<0)||
			(extra_fight_mda_data[extra_index].play_end))
		{
			main_pass=1;
		}

		if(extra_index<0)main_pass=1;
		
		count++;
		if(count>100)
			main_pass=1;
		
	}

// ---- source back
	source_data->dir=source_dir;
	target_data->dir=target_dir;
	source_data->motion=FIGHT_MOTION_STANDBY;
	target_data->motion=FIGHT_MOTION_STANDBY;
	
	if(feed_back_blood>0)		// feed back
	{
		sprintf((char *)print_rec,"%d",feed_back_blood);
		add_fight_screen_text(source_data->x,source_data->y-50,1,1000,print_rec);
	}
	

	auto_camera_pan(fight_default_pos.zoom_index,fight_default_pos.center_x,fight_default_pos.center_y,12);
	
	fight_control_data.camera_follow_zoom=fight_default_pos.zoom_index;
	fight_control_data.camera_follow_x=fight_default_pos.center_x;
	fight_control_data.camera_follow_y=fight_default_pos.center_y;
	fight_control_data.camera_follow_flag=true;
	
	


	update_fight_result(source_idx);
	update_fight_result(target_idx);
	
}


