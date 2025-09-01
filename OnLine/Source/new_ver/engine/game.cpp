/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : game.cpp
  *owner  		: Stephen
  *description  : 
  *modified     : 2004/12/14
******************************************************************************/ 

#include "engine_global.h"
#include "game.h"
#include "graph.h"
#include "utility.h"
#include "vari-ext.h"
#include "utlvector.h"
#include "ichat.h"
#include "taskman.h"
#include "account.h"
#include "data.h"
#include "net.h"
#include "net_msg.h"
#include "chat_msg.h"
#include "weather.h"
#include "chatwindow.h"
#include "edit.h"
#include "chat_info.h"
#include "macro.h"
#include "data_proc.h"
#include "fight.h"

#include "dm_music.h"
#include "macrodef.h"



#define GAME_HELP_URL "www.unigium.com"			// ��Ϸ����
#define GAME_CARD_URL "www.unigium.com"			// �㿨
#define GAME_REGISTER_URL "www.unigium.com"		// ע��
#define GAME_UNIGIUM_URL "www.unigium.com"		// ��˾��ҳ
#define GAME_BBS_URL "www.unigium.com"			// ��Ϸ��̳
#define GAME_INFO_URL "www.unigium.com"			// ��Ϸ�ֲ�
#define GAME_QA_URL "www.unigium.com"			// ������



SLONG select_character_return_ok = 0;
SLONG create_character_return_ok = 0;

//////////////////////////////////////////////////////////////////////////
//
SLONG show_title(void)
{
	SLONG i;
	IMG256_GROUP button_img[7];
	RECT source_rect[7];
	SLONG button_handle;
	SLONG idx;
	POSITION back_center;
	
	if(mda_group_open((UCHR *)"system\\Button.mda",&button_handle)!=TTN_OK)
		return(RETURN_ERROR);
	
	idx=get_mda_index(button_handle);
	if(idx<0)return(RETURN_ERROR);
	mda_group_exec(button_handle,(UCHR *)"MainCommand8",0,0,screen_channel2,MDA_COMMAND_FIRST,0,0,0);
	back_center.x=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->center_x;
	back_center.y=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->center_y;
	
	
	show_mouse(SHOW_WINDOW_CURSOR);
	dire_play_avi((UCHR *)"system\\title.avi",2);
	show_mouse(SHOW_IMAGE_CURSOR);
	
	get_bitmap(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,screen_buffer,screen_channel0);
	update_screen(screen_buffer);	
	if(load_jpg_file((UCHR *)"system\\c1meu005b.JPG",screen_channel0,0,0,SCREEN_WIDTH,SCREEN_HEIGHT)!=TTN_OK)
		return(RETURN_ERROR);
	
	mda_group_exec(button_handle,(UCHR *)"MainCommand8",
		back_center.x,back_center.y,screen_channel0,MDA_COMMAND_FIRST,0,0,0);
	
	
	for(i=0;i<7;i++)
	{
		if(read_mda_frame((UCHR *)"SYSTEM\\c1meu005b.MDA",i,&button_img[i].image)!=TTN_OK)
			return(RETURN_ERROR);
		get_img256_rect(button_img[i].image,&button_img[i].rect);
		source_rect[i].left=button_img[i].rect.left;		// Get X
		source_rect[i].top=button_img[i].rect.top;			// Get Y
		
		display_img256_buffer(source_rect[i].left,source_rect[i].top,button_img[i].image,screen_channel0);
	}
	
	
	
	change_screen_effect3(16);
	get_bitmap(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,screen_buffer,screen_channel0);
	update_screen(screen_buffer);	
	
	
	for(i=0;i<7;i++)
	{
		if(button_img[i].image!=NULL)
			free(button_img[i].image);
	}
	
	mda_group_close(&button_handle);
	
	return(RETURN_OK);
}



SLONG select_main_function(void)
{
	SLONG main_function;	
	SLONG button_handle;
	SLONG menu_handle;
	SLONG main_pass;
	UCHR command_text[1024];
	RECT menu_rect[7];
	POSITION menu_center[7];
	POSITION back_center;
	SLONG button_status[7];
	SLONG idx;
	SLONG mouse_x,mouse_y,old_mouse_x,old_mouse_y;
	UCHR mouse_key;
	SLONG old_function;
	SLONG i;
	UCHR big_mask[80];
	UCHR small_mask[80];
	UCHR push_big_mask[80];
	UCHR push_small_mask[80];
	ULONG mda_timer=0;
	ULONG mda_command;
	SLONG ret_val;
	
	
	POSITION crystal_dark_pos[6]=
	{
		{ 752,263 },
		{ 752,498 },
		{ 758,447 },
		{ 758,405 },
		{ 758,363 },
		{ 758,321 }
		
	};
	
	POSITION crystal_light_pos[6]=
	{
		{ 741,252 },
		{ 741,487 },
		{ 749,439 },
		{ 749,397 },
		{ 749,355 },
		{ 749,313 }
		
	};
	
	POSITION mask_pos[6]={
		{ 468,271 },
		{ 609,513 },
		{ 620,456 },
		{ 470,408 },
		{ 468,364 },
		{ 470,324 }
	};
	
	
	SLONG crystal_size[6]={						// ˮ����С 0 -> С, 1 -> ��
		1,
			1,
			0,
			0,
			0,
			0
	};
	
	SLONG mask_size[6]={						// ���δ�С 0 -> С, 1 -> ��
		1,
			0,
			0,
			1,
			1,
			1
	};	

	getChatWindow()->setChatWindowState(false);
	disconnectServer();

	game_control.chat_message_in=0;					// clear message in 
	game_control.team_message_in=0;					// clear team message in
	clear_chat_receive_data();
	clear_friend_data_record();
		
	
	strcpy((char *)big_mask,"MainCommand0");
	strcpy((char *)small_mask,"MainCommand2");
	strcpy((char *)push_big_mask,"MainCommand1");
	strcpy((char *)push_small_mask,"MainCommand3");
	main_function=-1;
	
	play_music_file((UCHR *)"music\\menu01.MP3",1,100);
	
	if(mda_group_open((UCHR *)"system\\Button.mda",&button_handle)!=TTN_OK)
		return(RETURN_ERROR);
	if(mda_group_open((UCHR *)"system\\c1meu005b.MDA",&menu_handle)!=TTN_OK)
		return(RETURN_ERROR);
	if(load_jpg_file((UCHR *)"system\\c1meu005b.JPG",screen_channel0,0,0,SCREEN_WIDTH,SCREEN_HEIGHT)!=TTN_OK)
		return(RETURN_ERROR);
	get_bitmap(0,0,800,600,screen_channel1,screen_channel0);
	
	for(i=0;i<7;i++)
	{
		sprintf((char *)command_text,"MainCommand%d",i);
		mda_group_exec(menu_handle,command_text,0,0,screen_channel2,MDA_COMMAND_FIRST,0,0,0);
		idx=get_mda_index(menu_handle);
		if(idx<0)return(RETURN_ERROR);
		menu_rect[i].left=mda_group[idx].img256_head->orig_x;
		menu_rect[i].top=mda_group[idx].img256_head->orig_y;
		menu_rect[i].right=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->xlen;
		menu_rect[i].bottom=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->ylen;
		menu_center[i].x=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->center_x;
		menu_center[i].y=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->center_y;
		button_status[i]=0;
		
		sprintf((char *)command_text,"MainCommand%d",i);
		mda_group_exec(menu_handle,command_text,menu_center[i].x,menu_center[i].y,screen_channel0,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
		
	}
	
	idx=get_mda_index(button_handle);
	if(idx<0)return(RETURN_ERROR);
	mda_group_exec(button_handle,(UCHR *)"MainCommand8",0,0,screen_channel2,MDA_COMMAND_FIRST,0,0,0);
	back_center.x=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->center_x;
	back_center.y=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->center_y;
	
	mda_group_exec(button_handle,(UCHR *)"MainCommand8",
		back_center.x,back_center.y,screen_channel0,MDA_COMMAND_FIRST,0,0,0);
	
	
	change_screen_effect3(16);
	
	get_bitmap(0,0,800,600,screen_channel0,screen_channel1);
	
	reset_ripple();
	old_mouse_x=0;
	old_mouse_y=0;
	old_function=-1;
	main_pass=0;
	ret_val=RETURN_ERROR;
	while(main_pass==0)
	{
		mda_command=MDA_COMMAND_LOOP;
		if(system_control.system_timer>=mda_timer)
		{
			mda_timer=system_control.system_timer+SYSTEM_MAIN_DELAY;
			mda_command=MDA_COMMAND_LOOP|MDA_COMMAND_NEXT_FRAME;
		}
		system_idle_loop();
		
		if(!IsIconic(g_hDDWnd))
		{
			get_mouse_position(&mouse_x,&mouse_y);
			mouse_key=get_mouse_key();
			
			main_function=-1;
			for(i=0;i<7;i++)
			{
				button_status[i]=0;
				if((mouse_x>=menu_rect[i].left)&&(mouse_x<menu_rect[i].right)&&
					(mouse_y>=menu_rect[i].top)&&(mouse_y<menu_rect[i].bottom))
				{
					if(mouse_key==MS_LDn)				
						button_status[i]=2;
					else
						button_status[i]=1;
					main_function=i;
					if(old_function!=main_function)
					{
						idx=get_mda_index(button_handle);
						if(idx>=0)
						{
							mda_group[idx].now_frame[0]=0;
							mda_group[idx].now_frame[2]=0;
						}
						old_function=main_function;
					}
				}
			}
			
			if(main_function==-1)
			{
				idx=get_mda_index(button_handle);
				if(idx>=0)
				{
					mda_group[idx].now_frame[1]=0;
					mda_group[idx].now_frame[3]=0;
				}
			}
			
			// --- Read backgroung
			get_bitmap(0,0,800,600,screen_buffer,screen_channel0);
			
			mda_group_exec(button_handle,(UCHR *)"MainCommand8",
				back_center.x,back_center.y,screen_buffer,mda_command,0,0,0);
			
			
			RippleSpread();
			RippleRender();
			
			switch(mouse_key)
			{
			case MS_LUp:
				reset_mouse_key();
				push_ripple(mouse_x,mouse_y,1000);
				switch(main_function)
				{
				case 0:					// ��ʼ��Ϸ
					ret_val=RETURN_OK;
					main_pass=1;
					break;
				case 1:					// �˳���Ϸ
					ret_val=RETURN_ERROR;
					main_pass=1;
					break;
				case 2:					// ��Ϸ����
					openHttp(GAME_HELP_URL);
					break;
				case 3:					// �����Ŷ�
					ret_val=RETURN_CHANGE_TASK+TASK_WORK_TEAM;
					main_pass=1;
					break;
				case 4:					// �㿨
					openHttp(GAME_CARD_URL);
					break;
				case 5:					// ע��
					openHttp(GAME_REGISTER_URL);
					break;
				case 6:					// ��˾��ҳ
					openHttp(GAME_UNIGIUM_URL);
					break;
				}
				
				break;
				case MS_LDrag:
					if((old_mouse_x!=mouse_x)||
						(old_mouse_y!=mouse_y))
					{
						push_ripple(mouse_x,mouse_y,2000);
						old_mouse_x=mouse_x;
						old_mouse_y=mouse_y;
					}
					break;
				default:
					if((old_mouse_x!=mouse_x)||
						(old_mouse_y!=mouse_y))
					{
						push_ripple(mouse_x,mouse_y,100);
						old_mouse_x=mouse_x;
						old_mouse_y=mouse_y;
					}
					break;
			}
			
			
			
			
			// --- Draw button
			for(i=0;i<6;i++)
			{
				switch(button_status[i])
				{
				case 0:				// None 
					sprintf((char *)command_text,"MainCommand%d",i);
					mda_group_exec(menu_handle,command_text,menu_center[i].x,menu_center[i].y,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
					if(crystal_size[i]==1)
						mda_group_exec(button_handle,(UCHR *)"MainCommand4",crystal_dark_pos[i].x,
						crystal_dark_pos[i].y,screen_buffer,mda_command,0,0,0);
					else
						mda_group_exec(button_handle,(UCHR *)"MainCommand6",crystal_dark_pos[i].x,
						crystal_dark_pos[i].y,screen_buffer,mda_command,0,0,0);
					break;
				case 1:			// Select
					if(crystal_size[i]==1)
					{
						if(mask_size[i]==1)
							strcpy((char *)command_text,(char *)big_mask);
						else
							strcpy((char *)command_text,(char *)small_mask);
						mda_group_exec(button_handle,(UCHR *)command_text,mask_pos[i].x,
							mask_pos[i].y,screen_buffer,mda_command,0,0,0);
						mda_group_exec(button_handle,(UCHR *)"MainCommand5",crystal_light_pos[i].x,
							crystal_light_pos[i].y,screen_buffer,mda_command,0,0,0);
					}
					else
					{
						if(mask_size[i]==1)
							strcpy((char *)command_text,(char *)big_mask);
						else
							strcpy((char *)command_text,(char *)small_mask);
						mda_group_exec(button_handle,command_text,mask_pos[i].x,
							mask_pos[i].y,screen_buffer,mda_command,0,0,0);
						
						mda_group_exec(button_handle,(UCHR *)"MainCommand7",crystal_light_pos[i].x,
							crystal_light_pos[i].y,screen_buffer,mda_command,0,0,0);
					}
					sprintf((char *)command_text,"MainCommand%d",i);
					mda_group_exec(menu_handle,command_text,menu_center[i].x,menu_center[i].y,screen_buffer,mda_command,0,0,0);
					break;
				case 2:			// Push
					if(crystal_size[i]==1)
					{
						if(mask_size[i]==1)
							strcpy((char *)command_text,(char *)push_big_mask);
						else
							strcpy((char *)command_text,(char *)push_small_mask);
						
						mda_group_exec(button_handle,command_text,mask_pos[i].x,
							mask_pos[i].y,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
						mda_group_exec(button_handle,(UCHR *)"MainCommand5",crystal_light_pos[i].x,
							crystal_light_pos[i].y,screen_buffer,mda_command,0,0,0);
					}
					else
					{
						if(mask_size[i]==1)
							strcpy((char *)command_text,(char *)push_big_mask);
						else
							strcpy((char *)command_text,(char *)push_small_mask);
						
						mda_group_exec(button_handle,command_text,mask_pos[i].x,
							mask_pos[i].y,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
						mda_group_exec(button_handle,(UCHR *)"MainCommand7",crystal_light_pos[i].x,
							crystal_light_pos[i].y,screen_buffer,mda_command,0,0,0);
					}
					sprintf((char *)command_text,"MainCommand%d",i);
					mda_group_exec(menu_handle,command_text,menu_center[i].x,menu_center[i].y,screen_buffer,mda_command,0,0,0);
					break;
				}
			}
			// --- Draw unigium
			switch(button_status[6])
			{
			case 0:
				strcpy((char *)command_text,"MainCommand6");
				break;
			case 1:
				strcpy((char *)command_text,"MainCommand7");
				break;
			case 2:
				strcpy((char *)command_text,"MainCommand8");
				break;
			}
			
			mda_group_exec(menu_handle,command_text,menu_center[6].x,menu_center[6].y,screen_buffer,mda_command,0,0,0);
			
			
			process_window_move();						// �����Ƿ� window ����

			execute_window_base();
			update_screen(screen_buffer);
		}
		
	}
	
	change_screen_effect3(16);
	
	mda_group_close(&menu_handle);
	mda_group_close(&button_handle);
	
	return(ret_val);
}




SLONG system_broadcast(void)
{
	SLONG button_handle;
	SLONG menu_handle;
	RECT menu_rect[10];
	POSITION updown_center[2];
	POSITION menu_center[8];
	POSITION back_center;
	SLONG button_status[10];
	UCHR command_text[1024];
	SLONG idx;
	SLONG i;
	SLONG main_pass;
	SLONG mouse_x,mouse_y,old_mouse_x,old_mouse_y;
	UCHR mouse_key;
	UCHR big_mask[80];
	UCHR small_mask[80];
	UCHR push_big_mask[80];
	UCHR push_small_mask[80];
	ULONG mda_timer=0;
	ULONG mda_command;
	SLONG main_function,old_function;
	bool change_frame=false;
	FILE_BUFFER file_buffer={ NULL,NULL };
	
	RECT text_window={ 140,120,420,440};// Text out window 
	
	
	SLONG total_text_line;
	SLONG text_start_idx,text_end_idx;
	UCHR text[1024];
	SLONG file_change;
	SLONG file_no;
	
	
	POSITION crystal_dark_pos[8]=
	{
		{ 758,148 },
		{ 758,189 },
		{ 758,235 },
		{ 758,278 },
		{ 758,321 },
		{ 758,363 },
		{ 758,405 },
		{ 752,498 }
		
	};
	
	POSITION crystal_light_pos[8]=
	{
		{ 749,140 },
		{ 749,181 },
		{ 749,227 },
		{ 749,270 },
		{ 749,313 },
		{ 749,355 },
		{ 749,397 },
		{ 741,487 }
		
	};
	
	POSITION mask_pos[8]={
		{ 609,158 },
		{ 498,188 },
		{ 489,240 },
		{ 466,280 },
		{ 498,323 },
		{ 491,369 },
		{ 612,413 },
		{ 609,513 }
		
	};
	
	
	SLONG crystal_size[8]={						// ˮ����С 0 -> С, 1 -> ��
		0,
			0,
			0,
			0,
			0,
			0,
			0,
			1
	};
	
	SLONG mask_size[8]={						// ���δ�С 0 -> С, 1 -> ��
		0,
			1,
			1,
			1,
			1,
			1,
			0,
			0
	};
	
	
	disconnectServer();
	
	


	strcpy((char *)big_mask,"MainCommand0");
	strcpy((char *)small_mask,"MainCommand2");
	strcpy((char *)push_big_mask,"MainCommand1");
	strcpy((char *)push_small_mask,"MainCommand3");
	
	
	if(mda_group_open((UCHR *)"system\\Button.mda",&button_handle)!=TTN_OK)
		return(RETURN_ERROR);
	if(mda_group_open((UCHR *)"system\\c1meu005c.MDA",&menu_handle)!=TTN_OK)
		return(RETURN_ERROR);
	if(load_jpg_file((UCHR *)"system\\c1meu005c.JPG",screen_channel0,0,0,SCREEN_WIDTH,SCREEN_HEIGHT)!=TTN_OK)
		return(RETURN_ERROR);
	
	
	
	get_bitmap(0,0,800,600,screen_channel1,screen_channel0);
	
	
	mda_group_exec(menu_handle,(UCHR *)"MainCommand0",0,0,screen_channel2,MDA_COMMAND_FIRST,0,0,0);
	idx=get_mda_index(menu_handle);
	if(idx<0)return(RETURN_ERROR);
	back_center.x=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->center_x;
	back_center.y=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->center_y;
	
	
	for(i=0;i<8;i++)
	{
		sprintf((char *)command_text,"MainCommand%d",i+1);
		mda_group_exec(menu_handle,command_text,0,0,screen_channel2,MDA_COMMAND_FIRST,0,0,0);
		idx=get_mda_index(menu_handle);
		if(idx<0)return(RETURN_ERROR);
		menu_rect[i].left=mda_group[idx].img256_head->orig_x;
		menu_rect[i].top=mda_group[idx].img256_head->orig_y;
		menu_rect[i].right=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->xlen;
		menu_rect[i].bottom=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->ylen;
		menu_center[i].x=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->center_x;
		menu_center[i].y=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->center_y;
		button_status[i]=0;
		sprintf((char *)command_text,"MainCommand%d",i+1);
		mda_group_exec(menu_handle,command_text,menu_center[i].x,menu_center[i].y,screen_channel0,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
		
	}
	
	
	mda_group_exec(menu_handle,(UCHR *)"MainCommand9",0,0,screen_channel2,MDA_COMMAND_FIRST,0,0,0);
	idx=get_mda_index(menu_handle);
	if(idx<0)return(RETURN_ERROR);
	menu_rect[8].left=mda_group[idx].img256_head->orig_x;
	menu_rect[8].top=mda_group[idx].img256_head->orig_y;
	menu_rect[8].right=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->xlen;
	menu_rect[8].bottom=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->ylen;
	
	updown_center[0].x=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->center_x;
	updown_center[0].y=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->center_y;
	button_status[8]=0;
	mda_group_exec(menu_handle,(UCHR *)"MainCommand9",updown_center[0].x,updown_center[0].y,screen_channel0,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
	
	mda_group_exec(menu_handle,(UCHR *)"MainCommand12",0,0,screen_channel2,MDA_COMMAND_FIRST,0,0,0);
	idx=get_mda_index(menu_handle);
	if(idx<0)return(RETURN_ERROR);
	menu_rect[9].left=mda_group[idx].img256_head->orig_x;
	menu_rect[9].top=mda_group[idx].img256_head->orig_y;
	menu_rect[9].right=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->xlen;
	menu_rect[9].bottom=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->ylen;
	updown_center[1].x=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->center_x;
	updown_center[1].y=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->center_y;
	button_status[9]=0;
	mda_group_exec(menu_handle,(UCHR *)"MainCommand12",updown_center[1].x,updown_center[1].y,screen_channel0,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
	
	mda_group_exec(menu_handle,(UCHR *)"MainCommand0",back_center.x,back_center.y,screen_channel0,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
	
	
	change_screen_effect3(16);
	
	get_bitmap(0,0,800,600,screen_channel0,screen_channel1);
	
	
	
	if(read_file_to_buffer((UCHR *)"download\\Affiche01.txt",&file_buffer)!=TTN_OK)
	{
		mda_group_close(&menu_handle);
		mda_group_close(&button_handle);
		return(RETURN_ERROR);
	}
	
	total_text_line=get_textbuffer_total_line(&file_buffer);
	text_start_idx=0;
	text_end_idx=0;
	
	
	set_text_out_window(text_window);
	text_out_data.g_pChat->clearString();
	
	if(total_text_line>0)
	{
		// --- First times insert 
		while(text_out_data.chatRect.top<text_window.bottom)
		{
			get_textbuffer_data(text_end_idx,&file_buffer,text);
			
			text_out_data.g_pChat->insertString((char *)text,text_out_data.chatRect);
			
			text_out_data.chatRect.top=text_out_data.chatRect.bottom;
			text_out_data.chatRect.bottom=text_window.bottom;
			text_out_data.chatRect.left=text_window.left;
			text_out_data.chatRect.right=text_window.right;
			text_end_idx++;		
		}
		
	}
	
	
	reset_ripple();
	mda_timer=0;
	old_mouse_x=0;
	old_mouse_y=0;
	old_function=-1;
	file_change=0;
	file_no=0;
	main_pass=0;
	while(main_pass==0)
	{
		system_idle_loop();
		
		if(!IsIconic(g_hDDWnd))
		{
			
			get_mouse_position(&mouse_x,&mouse_y);
			text_out_data.pt.x=mouse_x;
			text_out_data.pt.y=mouse_y;
			mouse_key=get_mouse_key();
			
			change_frame=false;
			mda_command=MDA_COMMAND_LOOP;
			if(system_control.system_timer>=mda_timer)
			{
				mda_timer=system_control.system_timer+SYSTEM_MAIN_DELAY;
				mda_command=MDA_COMMAND_LOOP|MDA_COMMAND_NEXT_FRAME;
				change_frame=true;
			}
			
			
			main_function=-1;
			for(i=0;i<10;i++)
			{
				button_status[i]=0;
				if((mouse_x>=menu_rect[i].left)&&(mouse_x<menu_rect[i].right)&&
					(mouse_y>=menu_rect[i].top)&&(mouse_y<menu_rect[i].bottom))
				{
					if(mouse_key==MS_LDn)				
						button_status[i]=2;
					else
						button_status[i]=1;
					main_function=i;
					if(old_function!=main_function)
					{
						idx=get_mda_index(button_handle);
						if(idx>=0)
						{
							mda_group[idx].now_frame[0]=0;
							mda_group[idx].now_frame[2]=0;
						}
						
						idx=get_mda_index(menu_handle);
						if(idx>=0)
						{
							mda_group[idx].now_frame[11]=0;
							mda_group[idx].now_frame[14]=0;
						}
						
						old_function=main_function;
					}
				}
			}
			
			
			if(main_function==-1)
			{
				idx=get_mda_index(button_handle);
				if(idx>=0)
				{
					mda_group[idx].now_frame[1]=0;
					mda_group[idx].now_frame[3]=0;
				}
				
				idx=get_mda_index(menu_handle);
				if(idx>=0)
				{
					mda_group[idx].now_frame[11]=0;
					mda_group[idx].now_frame[14]=0;
				}
				
				
			}
			
			
			switch(mouse_key)
			{
				
			case MS_LUp:
				reset_mouse_key();
				push_ripple(mouse_x,mouse_y,1000);
				idx=get_mda_index(menu_handle);
				if(idx>=0)
				{
					mda_group[idx].now_frame[11]=0;
					mda_group[idx].now_frame[14]=0;
				}
				
				switch(main_function)
				{
				case 0:					// ���¹���
					if(file_no==0)break;
					file_no=0;
					if(read_file_to_buffer((UCHR *)"download\\Affiche01.txt",&file_buffer)!=TTN_OK)
						break;
					file_change=1;				
					text_start_idx=0;
					break;
				case 1:					// �û�Э��
					if(file_no==1)break;
					file_no=1;
					if(read_file_to_buffer((UCHR *)"download\\Affiche02.txt",&file_buffer)!=TTN_OK)
						break;
					file_change=1;				
					text_start_idx=0;
					break;
				case 2:					// ���ֽ���
					if(file_no==2)break;
					file_no=2;
					if(read_file_to_buffer((UCHR *)"download\\Affiche03.txt",&file_buffer)!=TTN_OK)
						break;
					file_change=1;				
					text_start_idx=0;
					break;
				case 3:					// ��������
					if(file_no==3)break;
					file_no=3;
					if(read_file_to_buffer((UCHR *)"download\\Affiche04.txt",&file_buffer)!=TTN_OK)
						break;
					file_change=1;				
					text_start_idx=0;
					break;
				case 4:					// �����ֲ�
					openHttp(GAME_INFO_URL);
					break;
				case 5:					// ��̳
					openHttp(GAME_BBS_URL);
					break;
				case 6:					// ������
					openHttp(GAME_QA_URL);
					break;
				case 7:					// ��һ��
					main_pass=1;
					break;
				case 8:					// Text Down
					if(text_start_idx>0)
						text_start_idx--;
					file_change=1;
					break;
				case 9:					// Text Up
					if(text_start_idx<total_text_line-1)
						text_start_idx++;
					file_change=1;
					break;
				}
				
				break;
				case MS_LDrag:
					if((old_mouse_x!=mouse_x)||
						(old_mouse_y!=mouse_y))
					{
						push_ripple(mouse_x,mouse_y,2000);
						old_mouse_x=mouse_x;
						old_mouse_y=mouse_y;
					}
					break;
				default:
					if((old_mouse_x!=mouse_x)||
						(old_mouse_y!=mouse_y))
					{
						push_ripple(mouse_x,mouse_y,100);
						old_mouse_x=mouse_x;
						old_mouse_y=mouse_y;
					}
					break;
			}
			
			
			if(file_change==1)
			{
				total_text_line=get_textbuffer_total_line(&file_buffer);
				text_out_data.g_pChat->clearString();
				text_end_idx=0;
				memcpy(&text_out_data.chatRect,&text_window,sizeof(RECT));
				for(i=text_start_idx;i<total_text_line;i++)
				{
					get_textbuffer_data(i,&file_buffer,text);
					text_out_data.g_pChat->insertString((char *)text,text_out_data.chatRect);
					text_out_data.chatRect.top=text_out_data.chatRect.bottom;
					text_out_data.chatRect.bottom=text_window.bottom;
					text_out_data.chatRect.left=text_window.left;
					text_out_data.chatRect.right=text_window.right;
					text_end_idx++;		
					if(text_out_data.chatRect.top>=text_window.bottom)
						break;
				}
				
				file_change=0;			
			}
			
			
			// --- Read backgroung
			get_bitmap(0,0,800,600,screen_buffer,screen_channel0);
			RippleSpread();
			RippleRender();
			
			mda_group_exec(menu_handle,(UCHR *)"MainCommand0",back_center.x,back_center.y,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
			
			// --- Draw button
			for(i=0;i<8;i++)
			{
				switch(button_status[i])
				{
				case 0:				// None 
					sprintf((char *)command_text,"MainCommand%d",i+1);
					mda_group_exec(menu_handle,command_text,menu_center[i].x,menu_center[i].y,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
					if(crystal_size[i]==1)
						mda_group_exec(button_handle,(UCHR *)"MainCommand4",crystal_dark_pos[i].x,
						crystal_dark_pos[i].y,screen_buffer,mda_command,0,0,0);
					else
						mda_group_exec(button_handle,(UCHR *)"MainCommand6",crystal_dark_pos[i].x,
						crystal_dark_pos[i].y,screen_buffer,mda_command,0,0,0);
					break;
				case 1:			// Select
					if(crystal_size[i]==1)
					{
						if(mask_size[i]==1)
							strcpy((char *)command_text,(char *)big_mask);
						else
							strcpy((char *)command_text,(char *)small_mask);
						mda_group_exec(button_handle,(UCHR *)command_text,mask_pos[i].x,
							mask_pos[i].y,screen_buffer,mda_command,0,0,0);
						mda_group_exec(button_handle,(UCHR *)"MainCommand5",crystal_light_pos[i].x,
							crystal_light_pos[i].y,screen_buffer,mda_command,0,0,0);
					}
					else
					{
						if(mask_size[i]==1)
							strcpy((char *)command_text,(char *)big_mask);
						else
							strcpy((char *)command_text,(char *)small_mask);
						mda_group_exec(button_handle,command_text,mask_pos[i].x,
							mask_pos[i].y,screen_buffer,mda_command,0,0,0);
						
						mda_group_exec(button_handle,(UCHR *)"MainCommand7",crystal_light_pos[i].x,
							crystal_light_pos[i].y,screen_buffer,mda_command,0,0,0);
					}
					sprintf((char *)command_text,"MainCommand%d",i+1);
					mda_group_exec(menu_handle,command_text,menu_center[i].x,menu_center[i].y,screen_buffer,mda_command,0,0,0);
					break;
				case 2:			// Push
					if(crystal_size[i]==1)
					{
						if(mask_size[i]==1)
							strcpy((char *)command_text,(char *)push_big_mask);
						else
							strcpy((char *)command_text,(char *)push_small_mask);
						
						mda_group_exec(button_handle,command_text,mask_pos[i].x,
							mask_pos[i].y,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
						mda_group_exec(button_handle,(UCHR *)"MainCommand5",crystal_light_pos[i].x,
							crystal_light_pos[i].y,screen_buffer,mda_command,0,0,0);
					}
					else
					{
						if(mask_size[i]==1)
							strcpy((char *)command_text,(char *)push_big_mask);
						else
							strcpy((char *)command_text,(char *)push_small_mask);
						
						mda_group_exec(button_handle,command_text,mask_pos[i].x,
							mask_pos[i].y,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
						mda_group_exec(button_handle,(UCHR *)"MainCommand7",crystal_light_pos[i].x,
							crystal_light_pos[i].y,screen_buffer,mda_command,0,0,0);
					}
					sprintf((char *)command_text,"MainCommand%d",i+1);
					mda_group_exec(menu_handle,command_text,menu_center[i].x,menu_center[i].y,screen_buffer,mda_command,0,0,0);
					break;
				}
			}
			
			for(i=0;i<2;i++)		// Draw Up,Down
			{
				sprintf((char *)command_text,"MainCommand%d",i*3+9+button_status[i+8]);
				mda_group_exec(menu_handle,command_text,updown_center[i].x,updown_center[i].y,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			}
			
			text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,screen_buffer,change_frame);
			

			process_window_move();						// �����Ƿ� window ����
			
			execute_window_base();
			
			update_screen(screen_buffer);
		}
		
	}
	

	text_out_data.g_pChat->clearString();
	
	change_screen_effect3(16);
	
	mda_group_close(&menu_handle);
	mda_group_close(&button_handle);
	
	
	if(file_buffer.buffer!=NULL)
		free(file_buffer.buffer);
	
	return(RETURN_OK);
	
}



SLONG server_select(void)
{
	SLONG menu_handle;
	SLONG button_handle;
	SLONG select_handle;
	SLONG ret_val;
	SLONG main_pass;
	POSITION back_center;
	POSITION menu_center[6];
	RECT menu_rect[6];
	SLONG i,j;
	SLONG button_status[6];
	SLONG mouse_x,mouse_y,old_mouse_x,old_mouse_y;
	UCHR mouse_key;
	ULONG mda_timer=0;
	ULONG mda_command;
	SLONG main_function,old_function;
	SLONG idx;
	UCHR command_text[1024];
	UCHR big_mask[80];
	UCHR small_mask[80];
	UCHR push_big_mask[80];
	UCHR push_small_mask[80];
	SLONG default_page;
	SLONG total_all_server;
	SLONG now_page_server_idx[20];
	SLONG server_id;
	UCHR server_text[80];
	SLONG change_page;
	SLONG select_id;
	
	POSITION crystal_dark_pos[6]=
	{
		{ -999,-999 },
		{ -999,-999 },
		{ -999,-999 },
		{ -999,-999 },
		{ 752,449 },
		{ 752,498 }
		
	};
	
	POSITION crystal_light_pos[6]=
	{
		{ -999,-999 },
		{ -999,-999 },
		{ -999,-999 },
		{ -999,-999 },
		{ 741,438 },
		{ 741,487 }
		
	};
	
	POSITION mask_pos[6]={
		{ 144,98 },
		{ 235,98 },
		{ 323,98 },
		{ 414,98 },
		{ 609,464 },
		{ 609,513 }
		
	};
	
	
	SLONG crystal_size[6]={						// ˮ����С 0 -> С, 1 -> ��
		0,
			0,
			0,
			0,
			1,
			1
	};
	
	SLONG mask_size[6]={						// ���δ�С 0 -> С, 1 -> ��
		0,
			0,
			0,
			0,
			0,
			0
	};
	
	
	POSITION server_pp[MAX_SERVER_PAGE]={
		{ 207,146 },
		{ 297,146 },
		{ 388,146 },
		{ 477,146 },
		
		{ 207,203 },
		{ 297,203 },
		{ 388,203 },
		{ 477,203 },
		
		{ 207,259 },
		{ 297,259 },
		{ 388,259 },
		{ 477,259 },
		
		{ 207,316 },
		{ 297,316 },
		{ 388,316 },
		{ 477,316 },
		
		{ 207,372 },
		{ 297,372 },
		{ 388,372 },
		{ 477,372 }
		
	};
	
	disconnectServer();

	strcpy((char *)big_mask,"MainCommand0");
	strcpy((char *)small_mask,"MainCommand2");
	strcpy((char *)push_big_mask,"MainCommand1");
	strcpy((char *)push_small_mask,"MainCommand3");
	
	
	total_all_server=getServerInfoList();			// Get Server data 
	if(total_all_server<=0)
	{
		display_system_message((UCHR *)MSG_SERVER_NOT_FIND);
		return(RETURN_ERROR);
	}
	
	
	if(mda_group_open((UCHR *)"system\\Button.mda",&button_handle)!=TTN_OK)
		return(RETURN_ERROR);
	if(mda_group_open((UCHR *)"system\\Button.mda",&select_handle)!=TTN_OK)
		return(RETURN_ERROR);
	if(mda_group_open((UCHR *)"system\\c1meu010.MDA",&menu_handle)!=TTN_OK)
		return(RETURN_ERROR);
	if(load_jpg_file((UCHR *)"system\\c1meu005c.JPG",screen_channel0,0,0,SCREEN_WIDTH,SCREEN_HEIGHT)!=TTN_OK)
		return(RETURN_ERROR);
	
	get_bitmap(0,0,800,600,screen_channel1,screen_channel0);
	
	
	mda_group_exec(menu_handle,(UCHR *)"MainCommand0",0,0,screen_channel2,MDA_COMMAND_FIRST,0,0,0);
	idx=get_mda_index(menu_handle);
	if(idx<0)return(RETURN_ERROR);
	back_center.x=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->center_x;
	back_center.y=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->center_y;
	
	for(i=0;i<6;i++)
	{
		sprintf((char *)command_text,"MainCommand%d",i+1);
		mda_group_exec(menu_handle,command_text,0,0,screen_channel2,MDA_COMMAND_FIRST,0,0,0);
		idx=get_mda_index(menu_handle);
		if(idx<0)return(RETURN_ERROR);
		menu_rect[i].left=mda_group[idx].img256_head->orig_x;
		menu_rect[i].top=mda_group[idx].img256_head->orig_y;
		menu_rect[i].right=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->xlen;
		menu_rect[i].bottom=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->ylen;
		menu_center[i].x=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->center_x;
		menu_center[i].y=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->center_y;
		button_status[i]=0;
		sprintf((char *)command_text,"MainCommand%d",i+1);
		mda_group_exec(menu_handle,command_text,menu_center[i].x,menu_center[i].y,screen_channel0,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
		
	}
	
	
	mda_group_exec(menu_handle,(UCHR *)"MainCommand0",back_center.x,back_center.y,screen_channel0,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
	
	change_screen_effect3(16);
	
	get_bitmap(0,0,800,600,screen_channel0,screen_channel1);
	
	
	// --- Clear server idx
	default_page=0;
	for(i=0,j=0;i<MAX_SERVER_PAGE;i++)
	{
		now_page_server_idx[i]=-1;
		if(i<total_all_server)
		{
			server_id=	server_info[i].nServerID/1000 -1;
			if( server_id==default_page)
			{
				now_page_server_idx[j]=i;			// Store
				j++;
			}
		}
	}
	
	reset_ripple();
	ret_val=-1;
	mda_timer=0;
	old_mouse_x=0;
	old_mouse_y=0;
	main_pass=0;
	old_function=-1;
	system_palette=r_full_palette;
	change_page=0;
	server_id = 0;
	
	while(main_pass==0)
	{
		system_idle_loop();
		
		if(!IsIconic(g_hDDWnd))
		{
			
			get_mouse_position(&mouse_x,&mouse_y);
			mouse_key=get_mouse_key();
			
			mda_command=MDA_COMMAND_LOOP;
			if(system_control.system_timer>=mda_timer)
			{
				mda_timer=system_control.system_timer+SYSTEM_MAIN_DELAY;
				mda_command=MDA_COMMAND_LOOP|MDA_COMMAND_NEXT_FRAME;
			}
			
			
			
			main_function=-1;
			for(i=0;i<6;i++)
			{
				button_status[i]=0;
				if((mouse_x>=menu_rect[i].left)&&(mouse_x<menu_rect[i].right)&&
					(mouse_y>=menu_rect[i].top)&&(mouse_y<menu_rect[i].bottom))
				{
					if(mouse_key==MS_LDn)				
						button_status[i]=2;
					else
						button_status[i]=1;
					main_function=i;
					if(old_function!=main_function)
					{
						idx=get_mda_index(button_handle);
						if(idx>=0)
						{
							mda_group[idx].now_frame[0]=0;
							mda_group[idx].now_frame[2]=0;
						}
						old_function=main_function;
					}
					
				}
			}
			
			if(main_function==-1)
			{
				idx=get_mda_index(button_handle);
				if(idx>=0)
				{
					mda_group[idx].now_frame[1]=0;
					mda_group[idx].now_frame[3]=0;
				}
			}
			
			// ----- Check select 
			select_id=-1;
			for(i=0;i<MAX_SERVER_PAGE;i++)
			{
				if((mouse_x>=server_pp[i].x)&&(mouse_x<server_pp[i].x+70)
					&&(mouse_y>=server_pp[i].y)&&(mouse_y<server_pp[i].y+20))
				{
					if(now_page_server_idx[i]>=0)
					{
						select_id=i;
						break;
					}
				}
			}
			
			if((mouse_x>=351)&&(mouse_x<351+70)
				&&(mouse_y>=428)&&(mouse_y<428+20))
			{
				if(system_config.default_server_id>=0)
					select_id=9999;
			}
			
			
			change_page=0;
			switch(mouse_key)
			{
				
			case MS_LUp:
				reset_mouse_key();
				push_ripple(mouse_x,mouse_y,1000);
				
				switch(main_function)
				{
				case 0:					// �Ƽ�
					if(default_page==0)break;
					default_page=0;
					change_page=1;
					break;
				case 1:					// �漣ʱ��
					if(default_page==1)break;
					default_page=1;
					change_page=1;
					break;
				case 2:					// ����ʱ��
					if(default_page==2)break;
					default_page=2;
					change_page=1;
					break;
				case 3:					// ս��ʱ��
					if(default_page==3)break;
					default_page=3;
					change_page=1;
					break;
				case 4:					// Back
					ret_val=RETURN_CANCEL;
					main_pass=1;
					change_page=1;
					break;
				case 5:					// Next
					if(system_config.default_server_id<=0)break;
					ret_val=RETURN_OK;
					main_pass=1;
					break;
				}
				
				if((select_id>=0)&&(select_id<9999))
				{
					server_id=now_page_server_idx[select_id];
					if (server_info[server_id].bLine)
					{
						system_config.default_server_id=server_info[server_id].nServerID;
						main_pass=1;				
						ret_val=RETURN_OK;
					}
					else
					{
						display_system_message((UCHR*)MSG_SERVER_SHUTDOWN);
					}
				}
				if(select_id==9999)
				{
					if (server_info[server_id].bLine)
					{
						main_pass=1;				
						ret_val=RETURN_OK;
					}
					else
					{
						display_system_message((UCHR*)MSG_SERVER_SHUTDOWN);
					}
				}
				
				break;
				case MS_LDrag:
					if((old_mouse_x!=mouse_x)||
						(old_mouse_y!=mouse_y))
					{
						push_ripple(mouse_x,mouse_y,2000);
						old_mouse_x=mouse_x;
						old_mouse_y=mouse_y;
					}
					break;
				default:
					if((old_mouse_x!=mouse_x)||
						(old_mouse_y!=mouse_y))
					{
						push_ripple(mouse_x,mouse_y,100);
						old_mouse_x=mouse_x;
						old_mouse_y=mouse_y;
					}
					break;
			}
			
			// --- Read backgroung
			get_bitmap(0,0,800,600,screen_buffer,screen_channel0);
			RippleSpread();
			RippleRender();
			
			mda_group_exec(menu_handle,(UCHR *)"MainCommand0",back_center.x,back_center.y,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
			
			// --- Show Data
			
			mda_group_exec(select_handle,small_mask,mask_pos[default_page].x,
				mask_pos[default_page].y,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP|MDA_COMMAND_PALETTE_OR,0,0,0);
			
			if(change_page==1)
			{
				for(i=0,j=0;i<MAX_SERVER_PAGE;i++)
				{
					now_page_server_idx[i]=-1;
					if(i<total_all_server)
					{
						server_id=server_info[i].nServerID/1000 -1;
						if( server_id==default_page)
						{
							now_page_server_idx[j]=i;			// Store
							j++;
						}
					}
				}
				
			}
			
			for(i=0;i<MAX_SERVER_PAGE;i++)
			{
				if(now_page_server_idx[i]>=0)
				{
					server_id=now_page_server_idx[i];
					print16(server_pp[i].x+3,server_pp[i].y,server_info[server_id].szServerName,PEST_PUT,screen_buffer);
					if(server_info[server_id].bLine==0)			
					{
						strcpy((char *)server_text,"MainCommand12");
					}
					else
					{
						if(server_info[server_id].nLineNum<=20)
						{
							strcpy((char *)server_text,"MainCommand8");
						}
						else if (server_info[server_id].nLineNum<=50)
						{
							strcpy((char *)server_text,"MainCommand9");
						}
						else if (server_info[server_id].nLineNum<=80)
						{
							strcpy((char *)server_text,"MainCommand10");
						}
						else
						{
							strcpy((char *)server_text,"MainCommand11");
						}
					}
					mda_group_exec(menu_handle,server_text,server_pp[i].x+9,server_pp[i].y+15,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
					
				}
			}
			
			// ---- default server
			server_id=-1;
			
			if(system_config.default_server_id>=0)
			{
				for(i=0;i<MAX_SERVER_PAGE;i++)
				{
					if(server_info[i].nServerID==(ULONG)system_config.default_server_id)
					{
						server_id=i;
						break;
					}
				}
			}
			if(server_id>=0)
			{
				print16(351+3,428,server_info[server_id].szServerName,PEST_PUT,screen_buffer);
				strcpy((char *)game_control.server_name,server_info[server_id].szServerName);
				
				if(server_info[server_id].bLine==0)			
				{
					strcpy((char *)server_text,"MainCommand12");
				}
				else
				{
					if(server_info[server_id].nLineNum<=20)
					{
						strcpy((char *)server_text,"MainCommand8");
					}
					else if (server_info[server_id].nLineNum<=50)
					{
						strcpy((char *)server_text,"MainCommand9");
					}
					else if (server_info[server_id].nLineNum<=80)
					{
						strcpy((char *)server_text,"MainCommand10");
					}
					else
					{
						strcpy((char *)server_text,"MainCommand11");
					}
				}
				mda_group_exec(menu_handle,server_text,351+9,428+15,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
				sprintf((char *)command_text,"MainCommand%d",system_config.default_server_id/1000);
				mda_group_exec(menu_handle,command_text,270,425,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
				
			}
			
			
			// --- Draw button
			for(i=0;i<6;i++)
			{
				switch(button_status[i])
				{
				case 0:				// None 
					sprintf((char *)command_text,"MainCommand%d",i+1);
					mda_group_exec(menu_handle,command_text,menu_center[i].x,menu_center[i].y,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
					if(crystal_size[i]==1)
						mda_group_exec(button_handle,(UCHR *)"MainCommand4",crystal_dark_pos[i].x,
						crystal_dark_pos[i].y,screen_buffer,mda_command,0,0,0);
					else
						mda_group_exec(button_handle,(UCHR *)"MainCommand6",crystal_dark_pos[i].x,
						crystal_dark_pos[i].y,screen_buffer,mda_command,0,0,0);
					break;
				case 1:			// Select
					if(crystal_size[i]==1)
					{
						if(mask_size[i]==1)
							strcpy((char *)command_text,(char *)big_mask);
						else
							strcpy((char *)command_text,(char *)small_mask);
						mda_group_exec(button_handle,(UCHR *)command_text,mask_pos[i].x,
							mask_pos[i].y,screen_buffer,mda_command,0,0,0);
						mda_group_exec(button_handle,(UCHR *)"MainCommand5",crystal_light_pos[i].x,
							crystal_light_pos[i].y,screen_buffer,mda_command,0,0,0);
					}
					else
					{
						if(mask_size[i]==1)
							strcpy((char *)command_text,(char *)big_mask);
						else
							strcpy((char *)command_text,(char *)small_mask);
						mda_group_exec(button_handle,command_text,mask_pos[i].x,
							mask_pos[i].y,screen_buffer,mda_command,0,0,0);
						
						mda_group_exec(button_handle,(UCHR *)"MainCommand7",crystal_light_pos[i].x,
							crystal_light_pos[i].y,screen_buffer,mda_command,0,0,0);
					}
					sprintf((char *)command_text,"MainCommand%d",i+1);
					mda_group_exec(menu_handle,command_text,menu_center[i].x,menu_center[i].y,screen_buffer,mda_command,0,0,0);
					break;
				case 2:			// Push
					if(crystal_size[i]==1)
					{
						if(mask_size[i]==1)
							strcpy((char *)command_text,(char *)push_big_mask);
						else
							strcpy((char *)command_text,(char *)push_small_mask);
						
						mda_group_exec(button_handle,command_text,mask_pos[i].x,
							mask_pos[i].y,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
						mda_group_exec(button_handle,(UCHR *)"MainCommand5",crystal_light_pos[i].x,
							crystal_light_pos[i].y,screen_buffer,mda_command,0,0,0);
					}
					else
					{
						if(mask_size[i]==1)
							strcpy((char *)command_text,(char *)push_big_mask);
						else
							strcpy((char *)command_text,(char *)push_small_mask);
						
						mda_group_exec(button_handle,command_text,mask_pos[i].x,
							mask_pos[i].y,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
						mda_group_exec(button_handle,(UCHR *)"MainCommand7",crystal_light_pos[i].x,
							crystal_light_pos[i].y,screen_buffer,mda_command,0,0,0);
					}
					sprintf((char *)command_text,"MainCommand%d",i+1);
					mda_group_exec(menu_handle,command_text,menu_center[i].x,menu_center[i].y,screen_buffer,mda_command,0,0,0);
					break;
				}
			}
			
			
			if((main_pass==1)&&(ret_val==RETURN_OK))
			{
				main_pass=0;
				// --- connect server
				for (SLONG i = 0; i < total_all_server; i ++)
				{
					if (server_info[i].nServerID == (ULONG)system_config.default_server_id)
					{
						break;
					}
				}
				
				if (i < total_all_server)
				{
					DWORD ip = server_info[i].nIP;
					
					int port = server_info[i].nPort;
					
					
					display_message((UCHR*)MSG_CONNECT_SERVER, false );
					update_screen(screen_buffer);
					
					if (connectServerIdle(ip, port, ClientNetHandle) == -1)
					{
						ret_val=RETURN_ERROR;
						
						display_error_message((UCHR*)MSG_CONNECT_SERVER_ERROR, true );
					}
					else
					{
						main_pass=1;
					}
				}			
			}
			
			process_window_move();						// �����Ƿ� window ����
			
			execute_window_base();
			update_screen(screen_buffer);
		}
	}
	
	change_screen_effect3(16);
	
	mda_group_close(&button_handle);
	mda_group_close(&select_handle);
	mda_group_close(&menu_handle);
	
	if(system_config.default_server_id>=0)
		update_system_config();
	
	return(ret_val);
	
}



SLONG account_input(void)
{
	SLONG ret_val;

	ret_val=account_function();

	return(ret_val);
}




void exec_message_window(void)
{
	SLONG stack_idx;
	RECT temp_rect;
	
	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&temp_rect,&stack_idx)!=TTN_OK)
		return;
	if(now_exec_window_handle!=message_stack[stack_idx].window_handle)
		return;

	switch(system_control.mouse_key)
	{
	case MS_RUp:				// �ر�
		reset_mouse_key();
		message_stack[stack_idx].active=false;
		now_delete_window_handle=now_exec_window_handle;
		break;
	case MS_LDrag:				// �϶�
		now_window_move_xoffset=system_control.mouse_x-temp_rect.left;
		now_window_move_yoffset=system_control.mouse_y-temp_rect.top;
		now_window_move_handle=now_exec_window_handle;
		break;
	}
	
	system_control.mouse_key=MS_Dummy;
	

}



void draw_message_window(void)
{
	SLONG stack_idx;
	RECT window_rect;
	SLONG x,y,xl,yl;
	SLONG cell_xl,cell_yl;
	SLONG xx,yy;

	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&stack_idx)!=TTN_OK)
		return;
	if(now_exec_window_handle!=message_stack[stack_idx].window_handle)
		return;


	x=window_rect.left;
	y=window_rect.top;
	xl=window_rect.right-window_rect.left;
	yl=window_rect.bottom-window_rect.top;
	

// draw_bar 
	cell_xl=(xl-16)/8;
	if(cell_xl<0)cell_xl=0;
	cell_yl=(yl-16)/8;
	if(cell_yl<0)cell_yl=0;


	display_img256_buffer(x,y,system_image.msg_TopLeft.image,screen_buffer);
	for(xx=0;xx<cell_xl;xx++)
		display_img256_buffer(x+8+xx*8,y,system_image.msg_Top.image,screen_buffer);
	display_img256_buffer(x+xl-8,y,system_image.msg_TopRight.image,screen_buffer);
	
	for(yy=0;yy<cell_yl;yy++)
	{
		display_img256_buffer(x,y+8+yy*8,system_image.msg_Left.image,screen_buffer);
		display_img256_buffer(x+xl-8,y+8+yy*8,system_image.msg_Right.image,screen_buffer);
		for(xx=0;xx<cell_xl;xx++)
			display_img256_buffer(x+8+xx*8,y+8+yy*8,system_image.msg_Middle.image,screen_buffer);
		
	}

	display_img256_buffer(x,y+yl-8,system_image.msg_BottomLeft.image,screen_buffer);
	for(xx=0;xx<cell_xl;xx++)
		display_img256_buffer(x+8+xx*8,y+yl-8,system_image.msg_Bottom.image,screen_buffer);
	display_img256_buffer(x+xl-8,y+yl-8,system_image.msg_BottomRight.image,screen_buffer);

	set_word_color(0,color_control.cyan);
	print16(x+16,y+16,message_stack[stack_idx].message,PEST_PUT,screen_buffer);
	set_word_color(0,color_control.white);
	
	if(system_control.system_timer>message_stack[stack_idx].timer+MESSAGE_DELAY)
	{
		message_stack[stack_idx].active=false;
		now_delete_window_handle=now_exec_window_handle;
	}
		
}




SLONG select_character_in(void)
{
	SLONG change_handle;
	SLONG idx;
	SLONG i;
	SLONG total_frame;
	POSITION back_center;


// --- ( 0 - 0 )show title
	if(mda_group_open((UCHR *)"system\\button.mda",&change_handle)!=TTN_OK)
		return(RETURN_ERROR);
	
	if(load_jpg_file((UCHR *)"system\\c1meu008.JPG",screen_buffer,0,0,SCREEN_WIDTH,SCREEN_HEIGHT)!=TTN_OK)
		return(RETURN_ERROR);
	

	get_bitmap(0,0,800,600,screen_channel2,screen_buffer);
	mda_group_exec(change_handle,(UCHR *)"MainCommand9",0,0,screen_channel1,MDA_COMMAND_FIRST,0,0,0);
	idx=get_mda_index(change_handle);
	if(idx<0)return(RETURN_ERROR);
	back_center.x=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->center_x;
	back_center.y=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->center_y;
	
	if(load_jpg_file((UCHR *)"system\\c1meu011.JPG",screen_channel0,0,0,SCREEN_WIDTH,SCREEN_HEIGHT)!=TTN_OK)
		return(RETURN_ERROR);
	
	total_frame=mda_group[idx].total_frame[9];
	for(i=0;i<total_frame;i++)
	{
		clear_time_delay();
		system_idle_loop();
		if(i<5)
			get_bitmap(0,0,800,600,screen_buffer,screen_channel2);
		else
			get_bitmap(0,0,800,600,screen_buffer,screen_channel0);


		mda_group_exec(change_handle,(UCHR *)"MainCommand9",back_center.x,back_center.y,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
		
		update_screen(screen_buffer);
		wait_time_delay(SYSTEM_MAIN_DELAY);
		
	}

	get_bitmap(0,0,800,600,screen_buffer,screen_channel0);
	update_screen(screen_buffer);
	mda_group_close(&change_handle);

	
	return(RETURN_OK);

}





SLONG select_character(void)
{
	SLONG idx;
	SLONG i;
	POSITION back_center;
	// ---
	SLONG leader_handle;
	SLONG head_handle;
	SLONG menu_handle;
	SLONG button_handle;
	ULONG mda_timer;
	SLONG mda_command;
	// 
	RECT menu_rect[8];
	POSITION menu_center[8];
	SLONG button_status[8];
	UCHR command_text[1024];
	
	SLONG old_mouse_x,old_mouse_y,mouse_x,mouse_y;
	SLONG main_function,old_function;
	SLONG main_pass;	
	
	UCHR big_mask[80];
	UCHR small_mask[80];
	UCHR push_big_mask[80];
	UCHR push_small_mask[80];
	BASE_CHARACTER_INFO base_character;
	
	POSITION leader_center;
	POSITION face_center;
	
	SLONG icon_id;
	SLONG file_select;
	SLONG ret_val;
	
	UINT nick_color=rgb2hi(255,255,255);
	UINT level_color=rgb2hi(247,76,30);
	UINT phyle_color=rgb2hi(245,185,86);
	
	POSITION crystal_dark_pos[3]=
	{
		{ 14 ,498 },
		{ 752,449 },
		{ 752,498 }
		
	};
	
	POSITION crystal_light_pos[3]=
	{
		{  3 ,487 },
		{ 741,438 },
		{ 741,487 }
		
	};
	
	POSITION mask_pos[3]={
		{ 15 ,508 },
		{ 609,464 },
		{ 609,513 }
		
	};
	
	
	SLONG crystal_size[3]={						// ˮ����С 0 -> С, 1 -> ��
		1,
			1,
			1
	};
	
	SLONG mask_size[3]={						// ���δ�С 0 -> С, 1 -> ��
		1,
			0,
			0
	};
	
	POSITION nane_pp[5]={
		{ 641, 109  },
		{ 641, 177 	},
		{ 641, 245 	},
		{ 641, 313 	},
		{ 641, 381 	}
	};
	
	POSITION level_pp[5]={
		{ 643, 126 	},
		{ 643, 194 	},
		{ 643, 262 	},
		{ 643, 330 	},
		{ 643, 398 	}
	};
	
	POSITION phyle_pp[5]={
		{ 691, 126 	},
		{ 691, 194 	},
		{ 691, 262 	},
		{ 691, 330 	},
		{ 691, 398 	}
	};
	
	
	
	play_music_file((UCHR *)"music\\menu02.MP3",1,100);
	
	strcpy((char *)big_mask,"MainCommand0");
	strcpy((char *)small_mask,"MainCommand2");
	strcpy((char *)push_big_mask,"MainCommand1");
	strcpy((char *)push_small_mask,"MainCommand3");
	
	if(load_jpg_file((UCHR *)"system\\c1meu011.JPG",screen_channel0,0,0,SCREEN_WIDTH,SCREEN_HEIGHT)!=TTN_OK)
		return(RETURN_ERROR);
	
	
	change_screen_effect3(16);	
	
	// --- ( 0 - 1 ) 
	if(mda_group_open((UCHR *)"system\\Leader.mda",&leader_handle)!=TTN_OK)
		return(RETURN_ERROR);
	if(mda_group_open((UCHR *)"system\\Head.MDA",&head_handle)!=TTN_OK)
		return(RETURN_ERROR);
	if(mda_group_open((UCHR *)"system\\c1meu011.MDA",&menu_handle)!=TTN_OK)
		return(RETURN_ERROR);
	if(mda_group_open((UCHR *)"system\\button.mda",&button_handle)!=TTN_OK)
		return(RETURN_ERROR);
	
	get_bitmap(0,0,800,600,screen_channel2,screen_channel0);		// backup background
	
	
	mda_group_exec(menu_handle,(UCHR *)"MainCommand0",0,0,screen_channel1,MDA_COMMAND_FIRST,0,0,0);
	idx=get_mda_index(menu_handle);
	if(idx<0)return(RETURN_ERROR);
	back_center.x=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->center_x;
	back_center.y=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->center_y;
	mda_group_exec(menu_handle,(UCHR *)"MainCommand0",back_center.x,back_center.y,screen_channel0,MDA_COMMAND_FIRST,0,0,0);
	
	mda_group_exec(leader_handle,(UCHR *)"MainCommand0",0,0,screen_channel1,MDA_COMMAND_FIRST,0,0,0);
	idx=get_mda_index(leader_handle);
	if(idx<0)return(RETURN_ERROR);
	leader_center.x=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->center_x;
	leader_center.y=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->center_y;
	
	mda_group_exec(leader_handle,(UCHR *)"MainCommand2",0,0,screen_channel1,MDA_COMMAND_FIRST,0,0,0);
	idx=get_mda_index(leader_handle);
	if(idx<0)return(RETURN_ERROR);
	face_center.x=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->center_x;
	face_center.y=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->center_y;
	
	for(i=0;i<5;i++)
	{
		sprintf((char *)command_text,"MainCommand%d",i*3+1);
		mda_group_exec(menu_handle,command_text,0,0,screen_channel1,MDA_COMMAND_FIRST,0,0,0);
		idx=get_mda_index(menu_handle);
		if(idx<0)return(RETURN_ERROR);
		menu_rect[i].left=mda_group[idx].img256_head->orig_x;
		menu_rect[i].top=mda_group[idx].img256_head->orig_y;
		menu_rect[i].right=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->xlen;
		menu_rect[i].bottom=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->ylen;
		menu_center[i].x=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->center_x;
		menu_center[i].y=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->center_y;
		button_status[i]=0;
		sprintf((char *)command_text,"MainCommand%d",i*3+1);
		mda_group_exec(menu_handle,command_text,menu_center[i].x,menu_center[i].y,screen_channel0,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
	}
	
	
	
	for(i=0;i<3;i++)
	{
		sprintf((char *)command_text,"MainCommand%d",i+16);
		mda_group_exec(menu_handle,command_text,0,0,screen_channel1,MDA_COMMAND_FIRST,0,0,0);
		idx=get_mda_index(menu_handle);
		if(idx<0)return(RETURN_ERROR);
		menu_rect[i+5].left=mda_group[idx].img256_head->orig_x;
		menu_rect[i+5].top=mda_group[idx].img256_head->orig_y;
		menu_rect[i+5].right=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->xlen;
		menu_rect[i+5].bottom=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->ylen;
		menu_center[i+5].x=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->center_x;
		menu_center[i+5].y=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->center_y;
		button_status[i+5]=0;
		sprintf((char *)command_text,"MainCommand%d",i+16);
		mda_group_exec(menu_handle,command_text,menu_center[i+5].x,menu_center[i+5].y,screen_channel0,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
	}
	
	
	// --- fade in
	change_screen_effect3(16);
	
	
	get_bitmap(0,0,800,600,screen_channel0,screen_channel2);		// backup background
	
	mda_timer=0;
	old_mouse_x=0;
	old_mouse_y=0;
	old_function=-1;
	main_pass=0;
	ret_val=-1;
	if(total_base_character<=0)
		file_select=-1;
	else
		file_select=0;
	
	// --- process 2 background
	get_bitmap(0,0,800,600,screen_channel1,screen_channel0);		// restore 
	mda_group_exec(leader_handle,(UCHR *)"MainCommand0",leader_center.x,leader_center.y,screen_channel1,MDA_COMMAND_FIRST,0,0,0);
	
	text_out_data.g_pChat->clearString();
	
	reset_ripple();
	while(main_pass==0 && select_character_return_ok == 0)
	{
		system_idle_loop();
		
		if(!IsIconic(g_hDDWnd))
		{
			
			mouse_x=system_control.mouse_x;
			mouse_y=system_control.mouse_y;
			
			mda_command=MDA_COMMAND_LOOP;
			if(system_control.system_timer>=mda_timer)
			{
				mda_timer=system_control.system_timer+SYSTEM_MAIN_DELAY;
				mda_command=MDA_COMMAND_LOOP|MDA_COMMAND_NEXT_FRAME;
			}
			
			
			main_function=-1;
			for(i=0;i<8;i++)
			{
				button_status[i]=0;
				if(i<5)
				{
					if(total_base_character<=i)
					{
						continue;
					}
				}
				if((mouse_x>=menu_rect[i].left)&&(mouse_x<menu_rect[i].right)&&
					(mouse_y>=menu_rect[i].top)&&(mouse_y<menu_rect[i].bottom))
				{
					if(system_control.mouse_key==MS_LDn)				
						button_status[i]=2;
					else
						button_status[i]=1;
					main_function=i;
					if(old_function!=main_function)
					{
						idx=get_mda_index(button_handle);
						if(idx>=0)
						{
							mda_group[idx].now_frame[0]=0;
							mda_group[idx].now_frame[2]=0;
						}
						old_function=main_function;
						idx=get_mda_index(menu_handle);
						if(idx>=0)
						{
							for(i=0;i<5;i++)
								mda_group[idx].now_frame[i*3+3]=0;
						}
						
					}
				}
			}
			
			
			if(main_function==-1)
			{
				idx=get_mda_index(button_handle);
				if(idx>=0)
				{
					mda_group[idx].now_frame[1]=0;
					mda_group[idx].now_frame[3]=0;
				}
				
				idx=get_mda_index(menu_handle);
				if(idx>=0)
				{
					for(i=0;i<5;i++)
						mda_group[idx].now_frame[i*3+3]=0;
				}
				
			}
			
			
			switch(system_control.mouse_key)
			{
			case MS_LDn:
				reset_mouse_key();
				push_ripple(mouse_x,mouse_y,1000);
				
				switch(main_function)
				{
				case 0:					// 
					if(total_base_character<=0)
						break;
					file_select=0;
					break;
				case 1:					// 
					if(total_base_character<=1)
						break;
					file_select=1;
					break;
				case 2:					// 
					if(total_base_character<=2)
						break;
					file_select=2;
					break;
				case 3:					// 
					if(total_base_character<=3)
						break;
					file_select=3;
					break;
				case 4:					// 
					if(total_base_character<=4)
						break;
					file_select=4;
					break;
				case 5:					// 
					if(total_base_character>=5)
					{
						display_system_message((UCHR *)MSG_CHARACTER_CREATE_FULL);
						break;
					}
					ret_val=RETURN_CHANGE_TASK+TASK_CREATE_CHARACTER;
					main_pass=1;
					break;
				case 6:					// Back
					ret_val=RETURN_CANCEL;
					main_pass=1;
					break;
				case 7:					// Next
					if(file_select<0)break;
					ret_val=RETURN_OK;
					postNetMessage(NET_MSG_PLAYER_SELECT_CHARACTER, base_character_info[file_select].char_id);
					display_message((UCHR*)MSG_CONNECT_WAITTING,false);
					update_screen(screen_buffer);
					
					//				main_pass=1;
					break;
				}
				
				break;
				case MS_LDrag:
					if((old_mouse_x!=mouse_x)||
						(old_mouse_y!=mouse_y))
					{
						push_ripple(mouse_x,mouse_y,2000);
						old_mouse_x=mouse_x;
						old_mouse_y=mouse_y;
					}
					break;
				default:
					if((old_mouse_x!=mouse_x)||
						(old_mouse_y!=mouse_y))
					{
						push_ripple(mouse_x,mouse_y,100);
						old_mouse_x=mouse_x;
						old_mouse_y=mouse_y;
					}
					break;
			}
			
			if(file_select>=0)
				get_bitmap(0,0,800,600,screen_buffer,screen_channel1);		// restore 
			else
				get_bitmap(0,0,800,600,screen_buffer,screen_channel0);		// restore 
			
//			RippleSpread();
//			RippleRender();
			
			
			mda_group_exec(menu_handle,(UCHR *)"MainCommand0",back_center.x,back_center.y,screen_buffer,MDA_COMMAND_FIRST,0,0,0);
			for(i=0;i<5;i++)
			{
				switch(button_status[i])
				{
				case 0:				// None 
					sprintf((char *)command_text,"MainCommand%d",i*3+1);
					mda_group_exec(menu_handle,command_text,menu_center[i].x,menu_center[i].y,screen_buffer,mda_command,0,0,0);
					break;
				case 1:			// Select
					sprintf((char *)command_text,"MainCommand%d",i*3+2);
					mda_group_exec(menu_handle,command_text,menu_center[i].x,menu_center[i].y,screen_buffer,mda_command,0,0,0);
					break;
				case 2:			// Push
					sprintf((char *)command_text,"MainCommand%d",i*3+3);
					mda_group_exec(menu_handle,command_text,menu_center[i].x,menu_center[i].y,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
					break;
				}
				
				if(file_select==i)
				{
					sprintf((char *)command_text,"MainCommand%d",i*3+3);
					mda_group_exec(menu_handle,command_text,menu_center[i].x,menu_center[i].y,screen_buffer,MDA_COMMAND_LAST,0,0,0);
				}
				
				
			}
			
			
			for(i=0;i<3;i++)
			{
				switch(button_status[i+5])
				{
				case 0:				// None 
					sprintf((char *)command_text,"MainCommand%d",i+16);
					mda_group_exec(menu_handle,command_text,menu_center[i+5].x,menu_center[i+5].y,screen_buffer,mda_command,0,0,0);
					if(crystal_size[i]==1)
						mda_group_exec(button_handle,(UCHR *)"MainCommand4",crystal_dark_pos[i].x,
						crystal_dark_pos[i].y,screen_buffer,mda_command,0,0,0);
					else
						mda_group_exec(button_handle,(UCHR *)"MainCommand6",crystal_dark_pos[i].x,
						crystal_dark_pos[i].y,screen_buffer,mda_command,0,0,0);
					break;
				case 1:			// Select
					if(crystal_size[i]==1)
					{
						if(mask_size[i]==1)
							strcpy((char *)command_text,(char *)big_mask);
						else
							strcpy((char *)command_text,(char *)small_mask);
						mda_group_exec(button_handle,(UCHR *)command_text,mask_pos[i].x,
							mask_pos[i].y,screen_buffer,mda_command,0,0,0);
						mda_group_exec(button_handle,(UCHR *)"MainCommand5",crystal_light_pos[i].x,
							crystal_light_pos[i].y,screen_buffer,mda_command,0,0,0);
					}
					else
					{
						if(mask_size[i]==1)
							strcpy((char *)command_text,(char *)big_mask);
						else
							strcpy((char *)command_text,(char *)small_mask);
						mda_group_exec(button_handle,command_text,mask_pos[i].x,
							mask_pos[i].y,screen_buffer,mda_command,0,0,0);
						
						mda_group_exec(button_handle,(UCHR *)"MainCommand7",crystal_light_pos[i].x,
							crystal_light_pos[i].y,screen_buffer,mda_command,0,0,0);
					}
					sprintf((char *)command_text,"MainCommand%d",i+16);
					mda_group_exec(menu_handle,command_text,menu_center[i+5].x,menu_center[i+5].y,screen_buffer,mda_command,0,0,0);
					break;
				case 2:			// Push
					if(crystal_size[i]==1)
					{
						if(mask_size[i]==1)
							strcpy((char *)command_text,(char *)push_big_mask);
						else
							strcpy((char *)command_text,(char *)push_small_mask);
						
						mda_group_exec(button_handle,command_text,mask_pos[i].x,
							mask_pos[i].y,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
						mda_group_exec(button_handle,(UCHR *)"MainCommand5",crystal_light_pos[i].x,
							crystal_light_pos[i].y,screen_buffer,mda_command,0,0,0);
					}
					else
					{
						if(mask_size[i]==1)
							strcpy((char *)command_text,(char *)push_big_mask);
						else
							strcpy((char *)command_text,(char *)push_small_mask);
						
						mda_group_exec(button_handle,command_text,mask_pos[i].x,
							mask_pos[i].y,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
						mda_group_exec(button_handle,(UCHR *)"MainCommand7",crystal_light_pos[i].x,
							crystal_light_pos[i].y,screen_buffer,mda_command,0,0,0);
					}
					sprintf((char *)command_text,"MainCommand%d",i+16);
					mda_group_exec(menu_handle,command_text,menu_center[i+5].x,menu_center[i+5].y,screen_buffer,mda_command,0,0,0);
					break;
				}
			}
			
			
			
			// ----- display character 
			if(total_base_character>=0)
			{
				for(i=0;i<total_base_character;i++)
				{
					memcpy(&base_character,&base_character_info[i],sizeof(struct _BASE_CHARACTER_INFO));		// Move data
					set_word_color(0,nick_color);
					print12(nane_pp[i].x,nane_pp[i].y,base_character.nickname,PEST_PUT,screen_buffer);
					
					set_word_color(0,phyle_color);
					print12(phyle_pp[i].x,phyle_pp[i].y,character_phyle_text[base_character.phyle],PEST_PUT,screen_buffer);
					sprintf((char *)command_text,MSG_CHAR_LEVEL,base_character.level);
					set_word_color(0,level_color);
					print12(level_pp[i].x,level_pp[i].y,command_text,PEST_PUT,screen_buffer);
					
					icon_id=base_character.phyle*2+base_character.sex;
					sprintf((char *)command_text,"MainCommand%d",icon_id);
					mda_group_exec(head_handle,command_text,menu_center[i].x+6,menu_center[i].y+2,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
				}
			}
			
			set_word_color(0,color_control.white);
			
			
			if(file_select>=0)
			{
				memcpy(&base_character,&base_character_info[file_select],sizeof(struct _BASE_CHARACTER_INFO));		// Move data
				//			mda_group_exec(leader_handle,(UCHR *)"MainCommand0",leader_center.x,leader_center.y,screen_buffer,MDA_COMMAND_FIRST,0,0,0);
				icon_id=base_character.phyle*2+base_character.sex;
				
				sprintf((char *)command_text,"MainCommand%d",icon_id+2);
				mda_group_exec(leader_handle,command_text,face_center.x,face_center.y,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
				mda_group_exec(leader_handle,(UCHR *)"MainCommand1",leader_center.x,leader_center.y,screen_buffer,MDA_COMMAND_LOOP|MDA_COMMAND_NEXT_FRAME,0,0,0);
				
			}
			
			
			process_window_move();						// �����Ƿ� window ����
			
			execute_window_base();
			update_screen(screen_buffer);
		}
	}
	
	change_screen_effect3(16);
	
	mda_group_close(&leader_handle);
	mda_group_close(&head_handle);
	mda_group_close(&menu_handle);
	mda_group_close(&button_handle);
	
	if (select_character_return_ok == 1)
	{	
		select_character_return_ok = 0;
		
		ret_val = RETURN_OK;
	}

	text_out_data.g_pChat->clearString();
	
	
	return(ret_val);
	
}

SLONG create_character(void)
{
	SLONG leader_handle;
	SLONG menu_handle;
	SLONG button_handle;
	
	
	ULONG mda_timer;
	ULONG change_timer;
	SLONG mda_command;
	
	RECT menu_rect[8];					// 0 ~ 3 ����  4,5 ��Ů 6,7 ok ,cancel
	POSITION menu_center[8];
	SLONG button_status[8];
	POSITION leader_center;
	POSITION light_center;
	POSITION front_center;
	POSITION change_center;
	UCHR command_text[1024];
	
	SLONG sex=0;						// Ĭ�� ����
	SLONG i;
	SLONG idx;
	SLONG main_pass;
	SLONG mouse_x,mouse_y,old_mouse_x,old_mouse_y;
	SLONG main_function,old_function;
	SLONG ret_val;
	
	bool change_frame;
	
	SLONG character_type;
	SLONG character_group;
	
	UCHR big_mask[80];				
	UCHR small_mask[80];
	UCHR push_big_mask[80];
	UCHR push_small_mask[80];
	
	
	SLONG change_status;
	SLONG change_status_frame;
	SLONG change_status_total_frame;
	
	CNickNameBox editNickname;
	
	
	RECT text_window={ 520,80,778,124 };	// Text out window 
	UCHR text[8][1024]={
		CHARACTER_INST_HUMAN_MAN,
			CHARACTER_INST_HUMAN_WOMAN,
			CHARACTER_INST_GENUS_MAN,
			CHARACTER_INST_GENUS_WOMAN,
			CHARACTER_INST_MAGIC_MAN,
			CHARACTER_INST_MAGIC_WOMAN,
			CHARACTER_INST_ROBET_MAN,
			CHARACTER_INST_ROBET_WOMAN
	};
	
	
	POSITION crystal_dark_pos[4]=
	{
		{ 752,449 },
		{ 752,498 },
		{ 14,498 },
		{ 14,449 }
		
	};
	
	POSITION crystal_light_pos[4]=
	{
		{ 741,438 },
		{ 741,487 },
		{ 3,487  },
		{ 3,438  }
		
	};
	
	POSITION mask_pos[4]={
		{ 609,464 },
		{ 609,513 },
		{ -1, -1 },
		{ -1, -1 }
		
	};
	
	
	SLONG crystal_size[4]={						// ˮ����С 0 -> С, 1 -> ��
		1,
			1,
			1,
			1
	};
	
	SLONG mask_size[4]={						// ���δ�С 0 -> С, 1 -> ��
		-1,
			-1,
			0,
			0
	};
		
	editNickname.Init();

	if(load_jpg_file((UCHR *)"system\\c1meu012.JPG",screen_channel0,0,0,SCREEN_WIDTH,SCREEN_HEIGHT)!=TTN_OK)
		return(RETURN_ERROR);
	
	
	get_bitmap(0,0,800,600,screen_channel2,screen_channel0);
	
	strcpy((char *)big_mask,"MainCommand0");
	strcpy((char *)small_mask,"MainCommand2");
	strcpy((char *)push_big_mask,"MainCommand1");
	strcpy((char *)push_small_mask,"MainCommand3");
	
	
	// --- ( 0 - 1 ) 
	if(mda_group_open((UCHR *)"system\\Leader.mda",&leader_handle)!=TTN_OK)
		return(RETURN_ERROR);
	if(mda_group_open((UCHR *)"system\\c1meu012.MDA",&menu_handle)!=TTN_OK)
		return(RETURN_ERROR);
	if(mda_group_open((UCHR *)"system\\button.mda",&button_handle)!=TTN_OK)
		return(RETURN_ERROR);
	
	
	for(i=0;i<4;i++)			// ����
	{
		sprintf((char *)command_text,"MainCommand%d",i+9);
		mda_group_exec(menu_handle,command_text,0,0,screen_channel1,MDA_COMMAND_FIRST,0,0,0);
		idx=get_mda_index(menu_handle);
		if(idx<0)return(RETURN_ERROR);
		menu_rect[i].left=mda_group[idx].img256_head->orig_x;
		menu_rect[i].top=mda_group[idx].img256_head->orig_y;
		menu_rect[i].right=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->xlen;
		menu_rect[i].bottom=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->ylen;
		menu_center[i].x=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->center_x;
		menu_center[i].y=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->center_y;
		button_status[i]=0;
		sprintf((char *)command_text,"MainCommand%d",i+9);
		mda_group_exec(menu_handle,command_text,menu_center[i].x,menu_center[i].y,screen_channel0,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
	}
	
	
	for(i=0;i<2;i++)			// ��Ů
	{
		sprintf((char *)command_text,"MainCommand%d",i*3+3);
		mda_group_exec(menu_handle,command_text,0,0,screen_channel1,MDA_COMMAND_FIRST,0,0,0);
		idx=get_mda_index(menu_handle);
		if(idx<0)return(RETURN_ERROR);
		menu_rect[i+4].left=mda_group[idx].img256_head->orig_x;
		menu_rect[i+4].top=mda_group[idx].img256_head->orig_y;
		menu_rect[i+4].right=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->xlen;
		menu_rect[i+4].bottom=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->ylen;
		menu_center[i+4].x=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->center_x;
		menu_center[i+4].y=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->center_y;
		button_status[i+4]=0;
		sprintf((char *)command_text,"MainCommand%d",i*3+3);
		mda_group_exec(menu_handle,command_text,menu_center[i+4].x,menu_center[i+4].y,screen_channel0,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
	}
	
	
	for(i=0;i<2;i++)			// OK,cancel
	{
		sprintf((char *)command_text,"MainCommand%d",i+1);
		mda_group_exec(menu_handle,command_text,0,0,screen_channel1,MDA_COMMAND_FIRST,0,0,0);
		idx=get_mda_index(menu_handle);
		if(idx<0)return(RETURN_ERROR);
		menu_rect[i+6].left=mda_group[idx].img256_head->orig_x;
		menu_rect[i+6].top=mda_group[idx].img256_head->orig_y;
		menu_rect[i+6].right=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->xlen;
		menu_rect[i+6].bottom=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->ylen;
		menu_center[i+6].x=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->center_x;
		menu_center[i+6].y=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->center_y;
		button_status[i+6]=0;
		sprintf((char *)command_text,"MainCommand%d",i+1);
		mda_group_exec(menu_handle,command_text,menu_center[i+6].x,menu_center[i+6].y,screen_channel0,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
	}
	
	
	mda_group_exec(leader_handle,(UCHR *)"MainCommand2",0,0,screen_channel1,MDA_COMMAND_FIRST,0,0,0);
	idx=get_mda_index(leader_handle);
	if(idx<0)return(RETURN_ERROR);
	leader_center.x=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->center_x;
	leader_center.y=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->center_y;
	
	mda_group_exec(leader_handle,(UCHR *)"MainCommand1",0,0,screen_channel1,MDA_COMMAND_FIRST,0,0,0);
	idx=get_mda_index(leader_handle);
	if(idx<0)return(RETURN_ERROR);
	light_center.x=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->center_x;
	light_center.y=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->center_y;
	
	mda_group_exec(menu_handle,(UCHR *)"MainCommand0",0,0,screen_channel1,MDA_COMMAND_FIRST,0,0,0);
	idx=get_mda_index(menu_handle);
	if(idx<0)return(RETURN_ERROR);
	front_center.x=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->center_x;
	front_center.y=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->center_y;
	
	mda_group_exec(menu_handle,(UCHR *)"MainCommand30",0,0,screen_channel1,MDA_COMMAND_FIRST,0,0,0);
	idx=get_mda_index(menu_handle);
	if(idx<0)return(RETURN_ERROR);
	change_center.x=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->center_x;
	change_center.y=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->center_y;
	
	change_screen_effect3(16);
	
	if(load_jpg_file((UCHR *)"system\\c1meu012.JPG",screen_channel0,0,0,SCREEN_WIDTH,SCREEN_HEIGHT)!=TTN_OK)
		return(RETURN_ERROR);
	
	
	set_text_out_window(text_window);
	text_out_data.g_pChat->clearString();
	
	
	ret_val=RETURN_ERROR;
	character_type=-1;
	character_group=-1;
	mda_timer=0;
	change_timer=0;
	old_mouse_x=0;
	old_mouse_y=0;
	old_function=-1;
	main_pass=0;
	change_status_frame=0;
	change_status_total_frame=0;
	change_status=-1;
	sex=0;
	reset_ripple();
	
	while(main_pass==0 && create_character_return_ok ==0)
	{
		system_idle_loop();
		
		if(!IsIconic(g_hDDWnd))
		{
			
			mouse_x=system_control.mouse_x;
			mouse_y=system_control.mouse_y;
			text_out_data.pt.x=mouse_x;
			text_out_data.pt.y=mouse_y;
			
			change_frame=false;
			mda_command=MDA_COMMAND_LOOP;
			if(system_control.system_timer>=mda_timer)
			{
				mda_timer=system_control.system_timer+SYSTEM_MAIN_DELAY;
				mda_command=MDA_COMMAND_LOOP|MDA_COMMAND_NEXT_FRAME;
				change_frame=true;
			}
			
			
			main_function=-1;
			for(i=0;i<8;i++)
			{
				button_status[i]=0;
				if((mouse_x>=menu_rect[i].left)&&(mouse_x<menu_rect[i].right)&&
					(mouse_y>=menu_rect[i].top)&&(mouse_y<menu_rect[i].bottom))
				{
					if(system_control.mouse_key==MS_LDn)				
						button_status[i]=2;
					else
						button_status[i]=1;
					main_function=i;
					if(old_function!=main_function)
					{
						idx=get_mda_index(button_handle);
						if(idx>=0)
						{
							mda_group[idx].now_frame[0]=0;
							mda_group[idx].now_frame[2]=0;
						}
						old_function=main_function;
						
					}
				}
			}
			
			
			if(main_function==-1)
			{
				idx=get_mda_index(button_handle);
				if(idx>=0)
				{
					mda_group[idx].now_frame[1]=0;
					mda_group[idx].now_frame[3]=0;
				}
			}
			
			
			//////////////////////////////////////////////////////////////////////////
			//
			/*		UCHR key_num = get_char_key_num();
			
			  bool bkey_press = false;
			  
				for (int key_count = 0; key_count < key_num; key_count ++)
				{
				UCHR key_char = get_char_key(key_count);
				
				  switch (key_char) 
				  {
				  case S_Dn:
				  case S_Up:
				  case S_Left:
				  case S_Right:
				  case Up:
				  case Dn:
				  case Left:
				  case Right:
				  case S_Backspace:
				  case S_Del:
				  case S_Home:
				  case S_End:				
				  break;
				  
					default:
					{
					editNickname.OnChar(key_char, 0, 0);
					
					  bkey_press = true;
					  }
					  break;
					  }
					  }
					  
						reset_char_key();
						
						  if (!bkey_press)
						  {
						  UCHR key_system = system_control.key;
						  
							if (key_system == 0)
							{
							key_system = system_control.data_key;
							}
							
							  if (key_system != 0)
							  {
							  editNickname.OnKeyDown(key_system, 0, 0);
							  }
							  }
							  
								reset_data_key();
			*/
			editNickname.updateKey();
			
			//////////////////////////////////////////////////////////////////////////
			//
			
			switch(system_control.mouse_key)
			{
			case MS_LDn:
				reset_mouse_key();
				push_ripple(mouse_x,mouse_y,1000);
				
				{
					POINT ptCursor = { mouse_x, mouse_y};
					
					if (editNickname.LBtnDown(ptCursor))
					{
						break;
					}				
				}
				
				switch(main_function)
				{
				case 0:					// ��
					character_group=0;
					character_type=character_group+sex;
					break;
				case 1:					// ����
					character_group=2;
					character_type=character_group+sex;
					break;
				case 2:					// ��ʦ
					character_group=4;
					character_type=character_group+sex;
					break;
				case 3:					// ����
					character_group=6;
					character_type=character_group+sex;
					break;
				case 4:					//	����
					if(sex==0)break;
					change_status=1;
					change_status_frame=0;
					idx=get_mda_index(menu_handle);
					change_status_total_frame=mda_group[idx].total_frame[31];
					sex=0;
					break;
				case 5:					//  Ů��
					if(sex==1)break;
					change_status=0;
					change_status_frame=0;
					idx=get_mda_index(menu_handle);
					change_status_total_frame=mda_group[idx].total_frame[30];
					sex=1;
					break;
				case 6:					// Back
					ret_val=RETURN_CANCEL;
					main_pass=1;
					break;
				case 7:					// Next
					{
						bool bbreak = false;
						
						BASE_CHARACTER_INFO info = {0xffffffff, "", 0, sex, 0};
						
						switch (character_type) 
						{
						case 0:
						case 1:
							info.phyle = CHARACTER_PHYLE_HUMAN;
							break;
							
						case 2:
						case 3:
							info.phyle = CHARACTER_PHYLE_GENUS;
							break;
							
						case 4:
						case 5:
							info.phyle = CHARACTER_PHYLE_MAGIC;
							break;
							
						case 6:
						case 7:
							info.phyle = CHARACTER_PHYLE_ROBOT;
							break;
							
						default:
							{
								display_error_message((UCHR*)MSG_NOT_SELECT_PHYLE, true);
								
								bbreak = true;
							}
							break;
						}
						
						if (bbreak)
						{
							break;
						}
						
						char text[128] = {0};
						
						Q_strncpy(info.nickname, editNickname.getText(text), sizeof(info.nickname));
						
						if (*info.nickname == NULL)
						{
							display_error_message((UCHR*)MSG_NOT_NICK_NAME, true);
							
							break;
						}
						
						postNetMessage(NET_MSG_PLAYER_CREATE_CHARACTER, 1, (char*)&info, sizeof(info));
						
						main_pass=0;
					}
					break;
			}			
			break;
		case MS_LDrag:
			if((old_mouse_x!=mouse_x)||
				(old_mouse_y!=mouse_y))
			{
				push_ripple(mouse_x,mouse_y,2000);
				old_mouse_x=mouse_x;
				old_mouse_y=mouse_y;
			}
			break;
		default:
			if((old_mouse_x!=mouse_x)||
				(old_mouse_y!=mouse_y))
			{
				push_ripple(mouse_x,mouse_y,100);
				old_mouse_x=mouse_x;
				old_mouse_y=mouse_y;
			}
			
			break;
		}
		
		get_bitmap(0,0,800,600,screen_buffer,screen_channel0);		// restore 
		

		character_type=character_group+sex;
		
//		RippleSpread();
//		RippleRender();
		
		
		if(change_status>=0)
		{
			sprintf((char *)command_text,"MainCommand%d",change_status+30);
			mda_group_exec(menu_handle,command_text,change_center.x,change_center.y,screen_buffer,
				MDA_COMMAND_ASSIGN,0,0,change_status_frame);
			
			if(system_control.system_timer>=change_timer)
			{
				change_timer=system_control.system_timer+SYSTEM_MAIN_DELAY;
				change_status_frame++;
			}
			if(change_status_frame>=change_status_total_frame)
			{
				change_status_frame=0;
				change_status=-1;
			}
			
		}
		
		
		if(change_status<0)
		{
			for(i=0;i<4;i++)
			{
				switch(button_status[i])
				{
				case 0:			// None 
					sprintf((char *)command_text,"MainCommand%d",i+9+sex*4);
					mda_group_exec(menu_handle,command_text,menu_center[i].x,menu_center[i].y,screen_buffer,mda_command,0,0,0);
					break;
				case 1:			// Select
				case 2:			// Push
					sprintf((char *)command_text,"MainCommand%d",i+9+sex*4+8);
					mda_group_exec(menu_handle,command_text,menu_center[i].x,menu_center[i].y,screen_buffer,mda_command,0,0,0);
					break;
				}
				
			}
		}
		
		
		mda_group_exec(button_handle,(UCHR *)"MainCommand5",3,487,screen_buffer,mda_command,0,0,0);
		
		if((button_status[4]>0)||(button_status[5]>0))
			mda_group_exec(button_handle,(UCHR *)"MainCommand5",3,438,screen_buffer,mda_command,0,0,0);
		else
			mda_group_exec(button_handle,(UCHR *)"MainCommand4",14,449,screen_buffer,mda_command,0,0,0);
		
		
		
		for(i=0;i<2;i++)			// ��Ů
		{
			
			if(sex==i)
			{
				sprintf((char *)command_text,"MainCommand%d",i*3+3+1);
				mda_group_exec(menu_handle,command_text,menu_center[i+4].x,menu_center[i+4].y,screen_buffer,mda_command,0,0,0);
				continue;
			}
			
			switch(button_status[i+4])
			{
			case 0:				// None 
				sprintf((char *)command_text,"MainCommand%d",i*3+3);
				mda_group_exec(menu_handle,command_text,menu_center[i+4].x,menu_center[i+4].y,screen_buffer,mda_command,0,0,0);
				break;
			case 1:			// Select
				sprintf((char *)command_text,"MainCommand%d",i*3+3+1);
				mda_group_exec(menu_handle,command_text,menu_center[i+4].x,menu_center[i+4].y,screen_buffer,mda_command,0,0,0);
				break;
			case 2:			// Push
				sprintf((char *)command_text,"MainCommand%d",i*3+3+2);
				mda_group_exec(menu_handle,command_text,menu_center[i+4].x,menu_center[i+4].y,screen_buffer,mda_command,0,0,0);
				break;
			}
		}
		
		
		for(i=0;i<2;i++)			// ok cancel
		{
			switch(button_status[i+6])
			{
			case 0:				// None 
				sprintf((char *)command_text,"MainCommand%d",i+1);
				mda_group_exec(menu_handle,command_text,menu_center[i+6].x,menu_center[i+6].y,screen_buffer,mda_command,0,0,0);
				
				if(crystal_size[i]==1)
					mda_group_exec(button_handle,(UCHR *)"MainCommand4",crystal_dark_pos[i].x,
					crystal_dark_pos[i].y,screen_buffer,mda_command,0,0,0);
				else
					mda_group_exec(button_handle,(UCHR *)"MainCommand6",crystal_dark_pos[i].x,
					crystal_dark_pos[i].y,screen_buffer,mda_command,0,0,0);
				break;
			case 1:			// Select
				if(crystal_size[i]==1)
				{
					if(mask_size[i]==1)
						strcpy((char *)command_text,(char *)big_mask);
					else
						strcpy((char *)command_text,(char *)small_mask);
					mda_group_exec(button_handle,(UCHR *)command_text,mask_pos[i].x,
						mask_pos[i].y,screen_buffer,mda_command,0,0,0);
					mda_group_exec(button_handle,(UCHR *)"MainCommand5",crystal_light_pos[i].x,
						crystal_light_pos[i].y,screen_buffer,mda_command,0,0,0);
				}
				else
				{
					if(mask_size[i]==1)
						strcpy((char *)command_text,(char *)big_mask);
					else
						strcpy((char *)command_text,(char *)small_mask);
					mda_group_exec(button_handle,command_text,mask_pos[i].x,
						mask_pos[i].y,screen_buffer,mda_command,0,0,0);
					
					mda_group_exec(button_handle,(UCHR *)"MainCommand7",crystal_light_pos[i].x,
						crystal_light_pos[i].y,screen_buffer,mda_command,0,0,0);
				}
				sprintf((char *)command_text,"MainCommand%d",i+1);
				mda_group_exec(menu_handle,command_text,menu_center[i+6].x,menu_center[i+6].y,screen_buffer,mda_command,0,0,0);
				break;
			case 2:			// Push
				if(crystal_size[i]==1)
				{
					if(mask_size[i]==1)
						strcpy((char *)command_text,(char *)push_big_mask);
					else
						strcpy((char *)command_text,(char *)push_small_mask);
					
					mda_group_exec(button_handle,command_text,mask_pos[i].x,
						mask_pos[i].y,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
					mda_group_exec(button_handle,(UCHR *)"MainCommand5",crystal_light_pos[i].x,
						crystal_light_pos[i].y,screen_buffer,mda_command,0,0,0);
				}
				else
				{
					if(mask_size[i]==1)
						strcpy((char *)command_text,(char *)push_big_mask);
					else
						strcpy((char *)command_text,(char *)push_small_mask);
					
					mda_group_exec(button_handle,command_text,mask_pos[i].x,
						mask_pos[i].y,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
					mda_group_exec(button_handle,(UCHR *)"MainCommand7",crystal_light_pos[i].x,
						crystal_light_pos[i].y,screen_buffer,mda_command,0,0,0);
				}
				sprintf((char *)command_text,"MainCommand%d",i+1);
				mda_group_exec(menu_handle,command_text,menu_center[i+6].x,menu_center[i+6].y,screen_buffer,mda_command,0,0,0);
				break;
			}
		}
		
		
		
		if((character_type>=0)&&(character_group>=0))
		{
			sprintf((char *)command_text,"MainCommand%d",character_type+2);
			mda_group_exec(leader_handle,command_text,leader_center.x,leader_center.y,screen_buffer,mda_command,0,0,0);
			mda_group_exec(leader_handle,(UCHR *)"MainCommand1",light_center.x,light_center.y,screen_buffer,mda_command,0,0,0);
			mda_group_exec(menu_handle,(UCHR *)"MainCommand0",front_center.x,front_center.y,screen_buffer,mda_command,0,0,0);

			text_out_data.g_pChat->clearString();
			memcpy(&text_out_data.chatRect,&text_window,sizeof( RECT));
			text_out_data.g_pChat->insertString((char *)text[character_type],text_out_data.chatRect);
			
			
		}
		
		
		POINT ptCursor = { mouse_x, mouse_y };
		
		editNickname.Update(ptCursor);
		editNickname.Paint(screen_buffer);
		
		process_window_move();						// �����Ƿ� window ����
		
		execute_window_base();

		text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,screen_buffer,change_frame);
		update_screen(screen_buffer);
		}		
		
	}
	
	
	mda_group_close(&leader_handle);
	mda_group_close(&menu_handle);
	mda_group_close(&button_handle);
	
	if (create_character_return_ok == 1)
	{
		create_character_return_ok = 0;
		
		ret_val=RETURN_OK;
	}

	text_out_data.g_pChat->clearString();
	
	return(ret_val);
}


/*******************************************************************************************************************}
{																													}
{		��Ϸ��һ�ε���																								}
{																													}
{	�����ͼ�Լ�NPC data																												}
********************************************************************************************************************/
SLONG change_scene_return_ok = 0;

SLONG game_login(void)
{
	UCHR real_filename[256];
	char version_text[256];
	char window_name[256];

	
// --- ( 0 - 0 ) �����׼������ ---------

	SLONG main_pass = 0;


	while (main_pass == 0)
	{
		system_idle_loop();
		
		if (change_scene_return_ok == 1)
		{
			main_pass = 1;
		}
		else if (change_scene_return_ok == -1)
		{
			break;
		}
		
		process_window_move();						// �����Ƿ� window ����
		
		execute_window_base();
		
		update_screen(screen_buffer);
	}
	

// --- ( 2 - 0 ) Load data
	sprintf((char *)real_filename,(char *)"MAP\\%s",change_map_info.map_filename);
	if(load_map_file(real_filename,change_map_info.main_character.x,change_map_info.main_character.y)!=TTN_OK)
	{
		sprintf((char *)print_rec,MSG_MAP_FILE_LOAD_ERROR,real_filename);
		display_error_message(print_rec,1);
		return(RETURN_ERROR);
	}
// --- music 
	sprintf((char *)real_filename,"MUSIC\\%s",change_map_info.music_filename);
	play_music_file(real_filename,1,100);
		
	get_version_string(version_text,GAME_VERSION);
	sprintf(window_name,"%s( version %s ) [ %s - %s - %d ] ( %s - %s )",GAME_NAME,version_text,
		game_control.server_name,
		change_map_info.main_character.name,
		change_map_info.main_character.id,__DATE__,__TIME__);
	SetWindowText(g_hDDWnd,  (char *)window_name);


	initialize_chat_window();
	
	return(RETURN_OK);
}



void convert_change_map_info(MAP_BASE_INFO map_base_info,MAP_ATTRIB_INFO map_attrib_info,
							 MAP_WEATHER_BASE map_weather_base,BASE_NPC_INFO player)
{

	strcpy((char *)change_map_info.map_name,(char *)map_base_info.map_name);						// ��ͼ����
	sprintf((char *)change_map_info.map_filename,"MAP%03d.MAP",map_base_info.map_filename_id);		// �����ͼ��������
	sprintf((char *)change_map_info.music_filename,"MUS%03d.MP3",map_base_info.music_filename_id);	// ���ֵ�������
	change_map_info.small_map_file_no=map_base_info.small_map_filename_id;							// С��ͼ���
	if(change_map_info.small_map_file_no<0)
		change_map_info.map_attrib_info.small_map_flag=0;
// ---
	sprintf((char *)change_map_info.fight_music_filename,"MUS%03d.mp3",map_base_info.fight_music_id);		// Сս������
	change_map_info.fight_map_no=map_base_info.fight_map_filename_id;										// һ��ս��,ս���������
	if(change_map_info.fight_map_no<0)
		change_map_info.fight_map_no=0;
// ---
	sprintf((char *)change_map_info.boss_fight_music_filename,"MUS%03d.mp3",map_base_info.boss_fight_music_id);	// Boss ս������

	change_map_info.boss_fight_map_no=map_base_info.fight_map_filename_id;									// Boss ս���������
	if(change_map_info.boss_fight_map_no<0)
		change_map_info.boss_fight_map_no=0;


//----- ��ͼ������������
	memcpy(&change_map_info.map_weather_base,&map_weather_base,sizeof(struct MAP_WEATHER_BASE_STRUCT));

//----- ��ͼ��������
	memcpy(&change_map_info.map_attrib_info,&map_attrib_info,sizeof(struct MAP_ATTRIB_INFO_STRUCT));

// ---- ���� ����
	memcpy(&change_map_info.main_character,&player,sizeof(struct BASE_NPC_INFO_STRUCT));


	
}

/*******************************************************************************************************************}
{																													}
{		G A M E    M A I N    L O O P																				}
{																													}
********************************************************************************************************************/
SLONG game_main_loop(void)
{
	SLONG main_pass;
	SLONG ret_val;
	RECT main_window={ 0,0, SCREEN_WIDTH,SCREEN_HEIGHT 	};

// ---- ( 0 - 0 ) ����ʱ������׼��
/*******************************************************************************************************************}
{																													}
{	�� �� �� ֵ �� ��																								}
{																													}
********************************************************************************************************************/
	clear_all_main_loop_data();					// ���������������,��������,ս������
												// ��½�Ժ�����Ҫ������

	request_login_data();						// ������Ҫ���½������

// -----------
	game_control.weather_enable=true;			// �Ժ���� MAP_BASE_CONFIG
	set_weather_type(change_map_info.map_weather_base.weather_type);
	

	
	redraw_map(screen_channel0);
	auto_change_screen_effect();

	set_text_out_window(main_window);
	text_out_data.g_pChat->clearString();

	s_editChat.setFocus();
	system_control.data_key=NULL;
	system_control.key=NULL;
	
		
	game_control.main_loop_select_npc_idx=-1;		// ����·ѡ��NPC idx
	game_control.main_loop_select_npc_task=-1;		// ����·ѡ��NPC ��task


// ------------------------------------------------------------------------------------
// ����Ƿ�Ϊ MAIN_LOOP_MAP_MODE ���� MAIN_LOOP_FIGHT_MODE 
// ����� MAP_MODE ����, ����� Fight ,����� change_to_fight();
// ------------------------------------------------------------------------------------	
	if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)
		change_to_fight();
				

	ret_val=RETURN_ERROR;
	main_pass=0;
	while(main_pass==0)
	{
// --- ( 0 - 1 ) Process ǰ��׼�� -----------
		game_control.main_loop_task=MAIN_TASK_MOVE;
		game_control.main_loop_break=false;
		game_control.main_loop_ret_val=RETURN_ERROR;
		
		system_idle_loop();

		if(!IsIconic(g_hDDWnd))
		{

// ----- ��ͼģʽ routinue ---- 			
			if(game_control.main_loop_mode==MAIN_LOOP_MAP_MODE)
		{
				process_window_move();						// �����Ƿ� window ����
				redraw_map(screen_buffer);					// ���Ƶ�ͼ
				refresh_weather();							// ��������
				exec_weather(screen_buffer);				// ִ������
				exec_screen_text(screen_buffer);			// ִ�е�ǰӫĻ����
				check_screen_zoom();
				if(game_control.main_loop_select_npc_task<0)
				{
					draw_main_map(screen_buffer);			// �滭����ͼ��menu
					check_message_in();						// ����Ƿ���ѶϢ
					execute_window_base();					// ���� window & ִ��
					execute_task_manger();					// ��̨��ҵ
					if(!main_loop_active_task)
					{
						exec_main_map();
					}
					else
					{
						if(main_loop_active_task)
							main_loop_active_task();
					}
				}
				else
				{
					exec_main_loop_select_npc();
				}
				
			}
			else		// --------- ����ս��ģʽ
			{

// --- Draw �Ĳ���
				
				process_window_move();						// �����Ƿ� window ����
				fight_camera(screen_buffer);				// ���Ƶ�ͼ �� NPC
				exec_screen_text(screen_buffer);			// ִ�е�ǰӫĻ����
				draw_main_map(screen_buffer);				// �滭����ͼ��menu

// -- test
//	test_fight_camera();
				

// --- Exec �Ĳ���
				check_fight_main_command();					// �����������Ƿ���ȫ
				charge_control();							// ������
				execute_window_base();						// ���� window & ִ��
				execute_task_manger();						// ��̨��ҵ
				exec_main_map();
// ---- 
				exec_fight_action(screen_buffer);			// ִ��ս������

				if (fight_control_data.fight_end)
				{
					change_to_map();
				}
			}



		}	// --- end of IsIconic(g_hDDWnd)
		

		if(game_control.main_loop_break)
		{
			switch(game_control.main_loop_ret_val)
			{
			case MAIN_TASK_RETURN_QUIT:
				ret_val=RETURN_OK;
				main_pass=1;
				break;
			case MAIN_TASK_RETURN_READY_QUIT:
				exec_main_menu_task_quit();
				break;
			case MAIN_TASK_RETURN_END:
				ret_val=RETURN_ERROR;
				main_pass=1;
				break;
			}
		}

// ---- ( 9 - 0 ) ��ͼ 
		
		update_screen(screen_buffer);

// --- check mask
//		update_screen(map_control.screen_mask);
		
	}


	game_control.weather_enable=false;
	set_weather_type(WEATHER_TYPE_NONE);
	stop_all_voice();
	stop_music_file();
	
	close_all_main_map_window();

	game_control.main_loop_mode=MAIN_LOOP_MAP_MODE;

	game_control.main_loop_select_npc_idx=-1;		// ����·ѡ��NPC idx
	game_control.main_loop_select_npc_task=-1;		// ����·ѡ��NPC ��task
	

	return(ret_val);

}


void draw_main_map(BMP *buffer)
{
	SLONG backup_id;
	SLONG chat_channel;
	UCHR command_text[80];
	SLONG mouse_x,mouse_y;
	UHINT screen_mask;
	RECT mouse_rect;
	SLONG weather_no;
	SLONG i;
	int status;

	POSITION head_pp[MAX_TEAM_PLAYER]={
		{ 177, 5 },
		{ 177+63, 5 },
		{ 177+63*2, 5 },
		{ 177+63*3, 5 },
		{ 177+63*4, 5 }
	};
	
	
	POINT ptCursor = {0,0};
	
	backup_id=map_control.mask_npc;
	
	map_control.draw_mask=true;
// --- ( 0 - 0 ) Draw �����뱦������		0
	if(game_control.main_loop_mode==MAIN_LOOP_MAP_MODE)
	{
		map_control.mask_npc=MAIN_TASK_CHAR_STATUS;
		display_img256_buffer(system_image.status_body.rect.left,system_image.status_body.rect.top,
			system_image.status_body.image,buffer);
		//      ��ʾ����ͷ��
		sprintf((char *)command_text,"MainCommand%d",base_character_data.base_type);
		mda_group_exec(system_image.character_head,command_text,15,10,buffer,MDA_COMMAND_FIRST,0,0,0);
		
		display_character_quick_status(buffer);
		display_baobao_quick_status(buffer);
	}
	
// --- ( 0 - 1 ) Draw С��ͼ����			1
	if(game_control.main_loop_mode==MAIN_LOOP_MAP_MODE)
	{
		map_control.mask_npc=MAIN_TASK_SMALL_MAP;
		display_img256_buffer(system_image.map_body.rect.left,system_image.map_body.rect.top,
			system_image.map_body.image,buffer);
		weather_no=get_weather_icon_no();
		sprintf((char *)command_text,"MainCommand%d",weather_no);
		mda_group_exec(system_image.weather_handle,command_text,741,4,screen_buffer,MDA_COMMAND_FIRST,0,0,0);
		
		
		set_word_color(0,rgb2hi(230,230,230));
		print12(648,5,change_map_info.map_name,PEST_PUT,buffer);
		sprintf((char *)print_rec,"%3d,%3d",map_npc_group[system_control.control_npc_idx].npc_info.x/map_base_data.cell_width,
			map_npc_group[system_control.control_npc_idx].npc_info.y/map_base_data.cell_height);
		print12(648,24,print_rec,PEST_PUT,buffer);
		
	}

// --- ( 0 - 1 - 1 ) ս��ѡ��
	if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)
	{
		map_control.mask_npc=MAIN_TASK_FIGHT_MENU;
		display_img256_buffer( fight_image.menu_body.rect.left,fight_image.menu_body.rect.top,
			fight_image.menu_body.image,buffer);
		
		sprintf((char *)command_text,"MainCommand%d",base_character_data.base_type);
		mda_group_exec(system_image.character_head,command_text,fight_image.menu_body.rect.left+22,fight_image.menu_body.rect.top+31,buffer,MDA_COMMAND_FIRST,0,0,0);

		display_fight_character_quick_status(buffer);
		display_fight_baobao_quick_status(buffer);
		
	}

// --- ( 0 - 2 ) ������������
	if(game_control.chat_window_type==1)
	{
		display_chat_inter_window(buffer);
	}
	else
	{
		getChatWindow()->drawChatWindow();
	}

// --- ( 0 - 3 ) ��ͼ����������		2	
	map_control.mask_npc=MAIN_TASK_MENU;
	display_img256_buffer(system_image.map_main_body.rect.left,system_image.map_main_body.rect.top,
		system_image.map_main_body.image,buffer);
	chat_channel=game_control.chat_channel;
	sprintf((char *)command_text,"MainCommand%d",system_image.button_chat_channel[chat_channel].command_no);
	mda_group_exec(system_image.main_menu_handle,command_text,
		6,
		569,
		buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);

	if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)
	{
		display_img256_buffer( fight_image.map_menu_mask.rect.left,fight_image.map_menu_mask.rect.top,
		fight_image.map_menu_mask.image,buffer);
	}
	display_character_quick_exp(buffer);
	
// --- ( 0 - 4 ) ��������
	if(game_control.main_loop_mode==MAIN_LOOP_MAP_MODE)
	{
		status=map_npc_group[system_control.control_npc_idx].npc_info.status;
		if((status&NPC_STATUS_LEADER)||					// ����Ƕӳ�
			(status&NPC_STATUS_FOLLOW))					// ����Ƕ�Ա   
		{												
			for(i=0;i<MAX_TEAM_PLAYER;i++)
			{
				if(!(team_list_data[i].char_id&NPC_ID_MASK))
				{
					map_control.mask_npc=MAIN_TASK_TEAM1-i;
					display_img256_buffer(head_pp[i].x,head_pp[i].y,system_image.face_font.image,screen_buffer);
					sprintf((char *)command_text,"MainCommand%d",team_list_data[i].phyle*2+team_list_data[i].sex);
					mda_group_exec(system_image.character_head,command_text,head_pp[i].x+4,head_pp[i].y+7,screen_buffer,MDA_COMMAND_FIRST,0,0,0);
				}
			}
			
		}
	}
		
	
// --- ( 0 - 6 ) ���� ���������
	s_editChat.Update(ptCursor);
	s_editChat.Paint(buffer);
	
	map_control.draw_mask=false;
	map_control.mask_npc=backup_id;
	set_word_color(0,color_control.white);

// ---- ( 1 - 0 ) ��ȡ screen_mask & ��� mouse �����Ǹ� TASK ����
	mouse_x=system_control.mouse_x;
	mouse_y=system_control.mouse_y;


	mouse_rect.left=mouse_x;
	mouse_rect.top=mouse_y;
	mouse_rect.right=mouse_x;
	mouse_rect.bottom=mouse_y;
	if(check_have_touch_window(mouse_rect)>=0)
	{
		mouse_x=-1;
		mouse_y=-1;
		system_control.point_npc_idx=-1;
		game_control.main_loop_task=MAIN_TASK_END;		// �趨 TASK ID Ϊ NPC �� CHAR
		return;
	}
	if((mouse_x>=0)&&(mouse_y>=0)&&(mouse_x<SCREEN_WIDTH)&&(mouse_y<SCREEN_HEIGHT))
	{
		
		game_control.window_task=-1;

		screen_mask=map_control.screen_mask->line[mouse_y][mouse_x];
		if(screen_mask>=MAIN_TASK_END)					
		{
			system_control.point_npc_idx=-1;
			game_control.main_loop_task=screen_mask;		// ��ȡ TASK ID
		}

		screen_mask=map_control.screen_mask->line[system_control.zoom_mouse_y][system_control.zoom_mouse_x];
		if(screen_mask<MAIN_TASK_END)	
		{
			screen_mask=map_control.screen_mask->line[system_control.zoom_mouse_y][system_control.zoom_mouse_x];
			system_control.point_npc_idx=screen_mask;
			game_control.main_loop_task=MAIN_TASK_NPC;		// �趨 TASK ID Ϊ NPC �� CHAR
		}
		switch(game_control.main_loop_task)
		{
		case MAIN_TASK_MENU:						// �����ͼ��ѡ�� show Key Help
			reset_map_main_menu_button_frame();
			display_map_main_menu_status(BUTTON_SELECT);
			
			break;
		case MAIN_TASK_SMALL_MAP:					// С��ͼ
			if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)break;
			reset_map_small_map_button_frame();
			display_map_small_map_status(BUTTON_SELECT);
			break;
		case MAIN_TASK_CHAR_STATUS:					// ���ͷ�� show HP or MP
		case MAIN_TASK_BAOBAO_STATUS:				// ����ͷ��	show HP or MP
			if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)break;
			display_map_character_status(BUTTON_SELECT); // ��ʾ����򱦱�״̬
			break;
		case MAIN_TASK_TEAM1:						// �����һ�� Show base data
		case MAIN_TASK_TEAM2:						// ����ڶ���
		case MAIN_TASK_TEAM3:						// ���������
		case MAIN_TASK_TEAM4:						// ���������
		case MAIN_TASK_TEAM5:						// ���������
			if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)break;
			display_map_team_status(game_control.main_loop_task,BUTTON_SELECT);
			break;		
		case MAIN_TASK_FIGHT_MENU:					// ս��ѡ��
			if(game_control.main_loop_mode==MAIN_LOOP_MAP_MODE)break;
			display_fight_menu_status(BUTTON_SELECT);

			
			break;
		}
		
	}	

	
}



void exec_main_map(void)
{
	SLONG mouse_x,mouse_y;
	

	mouse_x=system_control.mouse_x;
	mouse_y=system_control.mouse_y;

	if((mouse_x>=0)&&(mouse_y>=0)&&(mouse_x<SCREEN_WIDTH)&&(mouse_y<SCREEN_HEIGHT))
	{
		
// --- ( 1 - 0 ) ���� û����갴���µ� TASK
		if((system_control.mouse_key==MS_LUp)||
			(system_control.mouse_key==MS_RUp)||
			(system_control.mouse_key==MS_Move))
			game_control.old_window_task=-1;
		
		switch(game_control.main_loop_task)
		{
		case MAIN_TASK_NPC:							// ��� or NPC
			if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)
				break;
			if(check_system_npc_in_range(NPC_TALK_RANGE)!=TTN_OK)
				break;
			set_mouse_cursor(MOUSE_IMG_MACRO);
			break;	
		default:
			set_mouse_cursor(MOUSE_IMG_STAND);
			break;
		}
		
		
// --- ( 2 - 0 ) ���� ��갴���� TASK
		if(system_control.mouse_key!=MS_Dummy)
		{
			switch(game_control.main_loop_task)
			{
			case MAIN_TASK_MOVE:						// �ƶ�
				if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)break;
				if(map_npc_group[system_control.control_npc_idx].npc_info.status&NPC_STATUS_FOLLOW)	// �Ѿ��Ƕ�Ա����ֹ�ƶ�
					break;			
				main_task_character_move(system_control.zoom_mouse_x,system_control.zoom_mouse_y);
				break;
			case MAIN_TASK_MENU:						// �����ͼ��ѡ��
				if((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag))
				{
					display_map_main_menu_status(BUTTON_PUSH);
					exec_map_main_menu();
					system_control.data_key=NULL;
					system_control.key=NULL;
					s_editChat.setFocus();
				}
				break;
			case MAIN_TASK_SMALL_MAP:					// С��ͼ
				if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)break;
				if((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag))
				{
					display_map_small_map_status(BUTTON_PUSH);
					exec_map_small_map();
				}
				
				break;
			case MAIN_TASK_NPC:							// ��� or NPC
				if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)
				{
					exec_fight_npc_active();
				}
					else
				{
					exec_map_npc_active();
				}
				break;
			case MAIN_TASK_CHAR_STATUS:					// ���ͷ��
			case MAIN_TASK_BAOBAO_STATUS:				// ����ͷ��
				if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)break;
				if((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag)||
					(system_control.mouse_key==MS_RDn)||(system_control.mouse_key==MS_RDrag))
				{
					display_map_character_status(BUTTON_PUSH); // ��ʾ����򱦱�״̬
					exec_map_character_status();
				}
				break;
			case MAIN_TASK_TEAM1:						// �����һ��
			case MAIN_TASK_TEAM2:						// ����ڶ���
			case MAIN_TASK_TEAM3:						// ���������
			case MAIN_TASK_TEAM4:						// ���������
			case MAIN_TASK_TEAM5:						// ���������
				if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)break;
				if((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag))
				{
					display_map_team_status(game_control.main_loop_task,BUTTON_PUSH);
				}
				break;		
			case MAIN_TASK_FIGHT_MENU:					// ս��ѡ��
				if(game_control.main_loop_mode==MAIN_LOOP_MAP_MODE)break;
				if((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag))
				{
					display_fight_menu_status(BUTTON_PUSH);
					exec_fight_main_menu();
				}
				break;
				
			}
			
		}
		
	}
	
	if (!is_window_on()) // û�д��ڵ�ʱ��ִ��
	{
		UpdateEditKey();
	}
	
// --- ( 3 - 0 ) ���� ��ݼ��� TASK
	if((system_control.key!=NULL)||(system_control.data_key!=NULL))
	{
		main_task_fast_key();
	}

	
}



void display_map_info(void)
{
	extern int sync_count;

	sprintf((char *)print_rec,"��(%d)��ͼ(%d,%d),NPC(%d),MDA(%d) sync_count:%d (%d,%d),(%d,%d)",get_fps(),
		map_control.start_x,
		map_control.start_y,
		total_map_npc,
		total_mda_group,
		sync_count,
		system_control.mouse_x,system_control.mouse_y,
		system_control.zoom_mouse_x,system_control.zoom_mouse_y);
	print12(240,0, print_rec, PEST_PUT, screen_buffer);
	
}


void display_user_info(void)
{

	if(total_map_npc<=0)return;
	sprintf((char *)print_rec,"����(%d,%d)-(%d,%d)(%d) %d Task(%d,%d)",
		map_npc_group[system_control.control_npc_idx].npc_info.x,
		map_npc_group[system_control.control_npc_idx].npc_info.y,
		map_npc_group[system_control.control_npc_idx].npc_info.tx,
		map_npc_group[system_control.control_npc_idx].npc_info.ty,
		map_npc_group[system_control.control_npc_idx].npc_info.motility,
		system_config.channel_screen_flag,
		game_control.window_task,game_control.old_window_task);
	print12(240,13, print_rec, PEST_PUT, screen_buffer);

	
}


void auto_change_weather(void)
{
	static ULONG timer=0;
	SLONG type;

	SLONG i;

	if(timer==0)		// first times
	{
		timer=system_control.system_timer+1000*(rand()%10+15);		// 10 ��
		return;	
	}
	if(system_control.system_timer<timer)
		return;
	timer=system_control.system_timer+1000*60*10;					// 10 ��
	type=WEATHER_TYPE_NONE;
	for(i=0;i<4;i++)
	{
		switch(rand()%10)
		{
		case 0:
			type=type|WEATHER_TYPE_RAIN;
			break;
		case 1:
			type=type|WEATHER_TYPE_SNOW;
			break;
		case 2:
			type=type|WEATHER_TYPE_CLOUD;
			break;
		case 3:
			type=type|WEATHER_TYPE_THUNDER;
			break;
		default:
			break;
		}
		
	}
	

	switch(rand()%3)
	{
	case 0:
		type=type|WEATHER_RANGE_SMALL;
		break;
	case 1:
		type=type|WEATHER_RANGE_MIDDLE;
		break;
	case 2:
		type=type|WEATHER_RANGE_LARGE;
		break;
	default:
		break;
	}

	set_weather_type(type);
		

}



void SendPlayerMove(int idx, int x, int y, int speed)
{
	PLAYER_MOVE move = { map_npc_group[idx].npc_info.id, x, y, speed, 0 };

	postNetMessage(NET_MSG_PLAYER_MOVE_START, map_npc_group[idx].npc_info.id, 
		(char*)&move, sizeof(move));
	
}

void main_task_character_move(SLONG mouse_x,SLONG mouse_y)
{
	SLONG tx,ty;
	SLONG orig_tx,orig_ty;
	SLONG new_tx,new_ty;


// --- ( 0 - 0 ) 
	tx=mouse_x+map_control.start_x;
	ty=mouse_y+map_control.start_y;
	
	if((tx<0)||(ty<0)||(tx>=map_base_data.map_width)||
		(ty>=map_base_data.map_height))
		return;								// out of range
	


// --- ( 0 - 1 ) ����Ƿ��赲��

	switch(system_control.mouse_key)
	{
	case MS_LUp:
		reset_mouse_key();
		if(system_control.system_timer<system_control.mouse_timer)
			return;
		system_control.mouse_timer=system_control.system_timer+PLAYER_MOVE_CHECK_TIMER;

		orig_tx=tx;
		orig_ty=ty;
		if(check_cell_stop(tx/map_base_data.cell_width,ty/map_base_data.cell_height)==TTN_OK) // ���赲������#3
		{
			if(get_random_near_offset_position(tx,ty,&new_tx,&new_ty)==TTN_OK)
			{
				tx=new_tx;
				ty=new_ty;
			}
			
		}
		

		if((abs(map_npc_group[system_control.control_npc_idx].npc_info.x-(tx))<=map_base_data.cell_width*2)&&
			(abs(map_npc_group[system_control.control_npc_idx].npc_info.y-(ty))<=map_base_data.cell_height*2))
			break;				

		map_npc_group[system_control.control_npc_idx].npc_info.tx=tx;
		map_npc_group[system_control.control_npc_idx].npc_info.ty=ty;
		map_npc_group[system_control.control_npc_idx].npc_info.speed=0;
		
		SendPlayerMove(system_control.control_npc_idx, tx, ty, 0);
		
		extra_mda_data[map_control.point_idx].display_x=orig_tx;
		extra_mda_data[map_control.point_idx].display_y=orig_ty;
		extra_mda_data[map_control.point_idx].display=true;

		extra_mda_data[map_control.point_idx].loop=false;
		

		reset_extra_mda_frame(map_control.point_idx);
		break;
		
	case MS_LDrag:

		if(system_control.system_timer<system_control.mouse_timer)
			return;
		system_control.mouse_timer=system_control.system_timer+PLAYER_MOVE_CHECK_TIMER;

		orig_tx=tx;
		orig_ty=ty;
		if(check_cell_stop(tx/map_base_data.cell_width,ty/map_base_data.cell_height)==TTN_OK) // ���赲������#3
		{
			if(get_random_near_offset_position(tx,ty,&new_tx,&new_ty)==TTN_OK)
			{
				tx=new_tx;
				ty=new_ty;
			}
			
		}
		
		if((abs(map_npc_group[system_control.control_npc_idx].npc_info.x-(tx))<=map_base_data.cell_width*2)&&
			(abs(map_npc_group[system_control.control_npc_idx].npc_info.y-(ty))<=map_base_data.cell_height*2))
			break;				
		map_npc_group[system_control.control_npc_idx].npc_info.tx=tx;
		map_npc_group[system_control.control_npc_idx].npc_info.ty=ty;
		map_npc_group[system_control.control_npc_idx].npc_info.speed=0;

		extra_mda_data[map_control.point_idx].display_x=orig_tx;
		extra_mda_data[map_control.point_idx].display_y=orig_ty;
		extra_mda_data[map_control.point_idx].display=true;
		
		extra_mda_data[map_control.point_idx].loop=true;
		
		
		SendPlayerMove(system_control.control_npc_idx, tx, ty, 0);
		break;
	case MS_RUp:
		reset_mouse_key();

		if(system_control.system_timer<system_control.mouse_timer)
			return;
		system_control.mouse_timer=system_control.system_timer+PLAYER_MOVE_CHECK_TIMER;

		orig_tx=tx;
		orig_ty=ty;
		if(check_cell_stop(tx/map_base_data.cell_width,ty/map_base_data.cell_height)==TTN_OK) // ���赲������#3
		{
			if(get_random_near_offset_position(tx,ty,&new_tx,&new_ty)==TTN_OK)
			{
				tx=new_tx;
				ty=new_ty;
			}
			
		}
		
		
		if((abs(map_npc_group[system_control.control_npc_idx].npc_info.x-(tx))<=map_base_data.cell_width*2)&&
			(abs(map_npc_group[system_control.control_npc_idx].npc_info.y-(ty))<=map_base_data.cell_height*2))
			break;				
		map_npc_group[system_control.control_npc_idx].npc_info.tx=tx;
		map_npc_group[system_control.control_npc_idx].npc_info.ty=ty;
		map_npc_group[system_control.control_npc_idx].npc_info.speed=1;
		
		SendPlayerMove(system_control.control_npc_idx, tx, ty, 1);
		
		extra_mda_data[map_control.point_idx].display_x=orig_tx;
		extra_mda_data[map_control.point_idx].display_y=orig_ty;
		extra_mda_data[map_control.point_idx].display=true;
		reset_extra_mda_frame(map_control.point_idx);

		extra_mda_data[map_control.point_idx].loop=false;
		
		break;

	case MS_RDrag:

		if(system_control.system_timer<system_control.mouse_timer)
			return;
		system_control.mouse_timer=system_control.system_timer+PLAYER_MOVE_CHECK_TIMER;

		orig_tx=tx;
		orig_ty=ty;
		if(check_cell_stop(tx/map_base_data.cell_width,ty/map_base_data.cell_height)==TTN_OK) // ���赲������#3
		{
			if(get_random_near_offset_position(tx,ty,&new_tx,&new_ty)==TTN_OK)
			{
				tx=new_tx;
				ty=new_ty;
			}
			
		}
		
		if((abs(map_npc_group[system_control.control_npc_idx].npc_info.x-(tx))<=map_base_data.cell_width*2)&&
			(abs(map_npc_group[system_control.control_npc_idx].npc_info.y-(ty))<=map_base_data.cell_height*2))
			break;				
		map_npc_group[system_control.control_npc_idx].npc_info.tx=tx;
		map_npc_group[system_control.control_npc_idx].npc_info.ty=ty;
		map_npc_group[system_control.control_npc_idx].npc_info.speed=1;

		extra_mda_data[map_control.point_idx].display_x=orig_tx;
		extra_mda_data[map_control.point_idx].display_y=orig_ty;
		extra_mda_data[map_control.point_idx].display=true;

		extra_mda_data[map_control.point_idx].loop=true;
		
		SendPlayerMove(system_control.control_npc_idx, tx, ty, 1);
		
		break;

	case MS_Move:
	case MS_Dummy:
		extra_mda_data[map_control.point_idx].loop=false;
		break;
		
	}



}


void main_task_fast_key(void)
{
	bool key_use;



// --- test ues
	UCHR text[1024];
	SLONG i=0,j;
	FIGHT_ACTION_RESULT aaa;
	FIGHT_VALUE_RESULT bbb;
	static bool tt=true;
	

	key_use=false;

	if(system_control.key!=NULL)
	{
		switch(system_control.key)
		{
		case Up:						// ��������������ʷ��¼
			reset_key();
			key_use=true;
			strcpy((char *)text,"");
			get_chat_stack(text,Up);
			if(strlen((char *)text)<1)break;
			s_editChat.Erase();
			s_editChat.setText((char *)text);
			break;
		case Dn:						// ��������������ʷ��¼
			key_use=true;
			reset_key();
			strcpy((char *)text,"");
			get_chat_stack(text,Dn);
			if(strlen((char *)text)<1)break;
			s_editChat.Erase();
			s_editChat.setText((char *)text);
			break;
			
		case Tab:						// С��ͼ�л�
			if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)break;
			key_use=true;
			reset_key();
			if((change_map_info.map_attrib_info.small_map_flag==0)||
				(change_map_info.small_map_file_no<0))break;
			exec_main_menu_task_small_map();
			break;
			
		case KB_F2:						// ����NPC �����л�
			if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)break;
			key_use=true;
			reset_key();
			game_control.npc_display_flag++;
			if(game_control.npc_display_flag>3)
				game_control.npc_display_flag=0;
			break;

			



		case Home:				// ������
			// --- һ��������
			if(fight_control_data.character_idx<0)break;
			aaa.command=FIGHT_ACTION_SINGLE_PHYSICS;
			aaa.source_id=fight_npc_group[fight_control_data.character_idx].base.id;
			aaa.target_id=character_fight_command_data.target_id;
			
			if(rand()%100>50)
				fight_action_single_physics_data.hp=30000;
			else
				fight_action_single_physics_data.hp=0;
			
			fight_action_single_physics_data.feed_back_hp=100;
			fight_action_single_physics_data.protect_id=0;
			fight_action_single_physics_data.back_hp=2000;
			
			update_fight_action_result(aaa);
			
			bbb.charge_speed=fight_npc_group[fight_control_data.character_idx].base.charge_speed;
			bbb.now_hp=50;
			bbb.id=fight_npc_group[fight_control_data.character_idx].base.id;
			bbb.max_hp=fight_npc_group[fight_control_data.character_idx].base.max_hp;
			bbb.now_mp=fight_npc_group[fight_control_data.character_idx].base.now_mp;
			bbb.max_mp=fight_npc_group[fight_control_data.character_idx].base.max_mp;
			bbb.status=0;
			update_fight_value_result(bbb);

			bbb.charge_speed=fight_npc_group[fight_control_data.character_idx].base.charge_speed;
			bbb.now_hp=0;
			bbb.id=aaa.target_id;
			bbb.max_hp=fight_npc_group[fight_control_data.character_idx].base.max_hp;
			bbb.now_mp=fight_npc_group[fight_control_data.character_idx].base.now_mp;
			bbb.max_mp=fight_npc_group[fight_control_data.character_idx].base.max_mp;
			bbb.status=FIGHT_STATUS_DEAD;
			update_fight_value_result(bbb);
			
			break;
		case PgUp:				// ��������
			
			if(fight_control_data.character_idx<0)break;
			aaa.command=FIGHT_ACTION_SINGLE_PHYSICS_DOUBLE;
			aaa.source_id=fight_npc_group[rand()%20].base.id;
			//			aaa.source_id=fight_npc_group[fight_control_data.character_idx0].base.id;
			aaa.target_id=character_fight_command_data.target_id;
			
			
			fight_action_single_physics_double_data.double_count=7;
			for(i=0;i<7;i++)
			{
				fight_action_single_physics_double_data.feedback[i]=100*i;
				fight_action_single_physics_double_data.hp[i]=rand()%10000;
				if(rand()%100>50)
					fight_action_single_physics_double_data.hp[i]=0;
				
				fight_action_single_physics_double_data.back_hp[i]=100*i/2+50;
				
			}
			
			
			update_fight_action_result(aaa);
			
			bbb.charge_speed=fight_npc_group[fight_control_data.character_idx].base.charge_speed;
			bbb.now_hp=50;
			bbb.id=aaa.source_id;
			bbb.max_hp=fight_npc_group[fight_control_data.character_idx].base.max_hp;
			bbb.now_mp=fight_npc_group[fight_control_data.character_idx].base.now_mp;
			bbb.max_mp=fight_npc_group[fight_control_data.character_idx].base.max_mp;
			bbb.status=0x00;
			update_fight_value_result(bbb);
			
			break;
		case KB_F9:		// ����������
			if(fight_control_data.character_idx<0)break;
			aaa.command=FIGHT_ACTION_SINGLE_MAGIC;
			aaa.source_id=fight_npc_group[fight_control_data.character_idx].base.id;
			aaa.target_id=character_fight_command_data.target_id;
			aaa.idx=0;			// ���� idx 
			
			update_fight_action_result(aaa);
			
			fight_action_single_magic_data.hp=1000;
			fight_action_single_magic_data.feed_back_hp=100;
			
			
			bbb.charge_speed=fight_npc_group[fight_control_data.character_idx].base.charge_speed;
			bbb.now_hp=50;
			bbb.id=fight_npc_group[fight_control_data.character_idx].base.id;
			bbb.max_hp=fight_npc_group[fight_control_data.character_idx].base.max_hp;
			bbb.now_mp=fight_npc_group[fight_control_data.character_idx].base.now_mp;
			bbb.max_mp=fight_npc_group[fight_control_data.character_idx].base.max_mp;
			bbb.status=0x00;
			update_fight_value_result(bbb);
			
			break;
		case KB_F10:	// ��������������
			if(fight_control_data.character_idx<0)break;
			aaa.command=FIGHT_ACTION_SINGLE_MAGIC_DOUBLE;
			aaa.source_id=fight_npc_group[fight_control_data.character_idx].base.id;
			aaa.target_id=character_fight_command_data.target_id;
			aaa.idx=0;			// ���� idx 
			
			update_fight_action_result(aaa);
			
			fight_action_single_magic_double_data.double_count=7;
			
			for(i=0;i<7;i++)
			{
				fight_action_single_magic_double_data.hp[i]=1000*i+1000;
				fight_action_single_magic_double_data.feedback[i]=100;
			}
			
			
			bbb.charge_speed=fight_npc_group[fight_control_data.character_idx].base.charge_speed;
			bbb.now_hp=50;
			bbb.id=fight_npc_group[fight_control_data.character_idx].base.id;
			bbb.max_hp=fight_npc_group[fight_control_data.character_idx].base.max_hp;
			bbb.now_mp=fight_npc_group[fight_control_data.character_idx].base.now_mp;
			bbb.max_mp=fight_npc_group[fight_control_data.character_idx].base.max_mp;
			bbb.status=0x00;
			update_fight_value_result(bbb);
			
			break;
		case KB_F11:		// ����
			if(fight_control_data.character_idx<0)break;
			aaa.command=FIGHT_ACTION_ITEM;
			aaa.source_id=fight_npc_group[fight_control_data.character_idx].base.id;
			aaa.target_id=character_fight_command_data.target_id;
			
			update_fight_action_result(aaa);
			
			
			bbb.charge_speed=fight_npc_group[fight_control_data.character_idx].base.charge_speed;
			bbb.now_hp=50;
			bbb.id=fight_npc_group[fight_control_data.character_idx].base.id;
			bbb.max_hp=fight_npc_group[fight_control_data.character_idx].base.max_hp;
			bbb.now_mp=fight_npc_group[fight_control_data.character_idx].base.now_mp;
			bbb.max_mp=fight_npc_group[fight_control_data.character_idx].base.max_mp;
			bbb.status=0x00;
			update_fight_value_result(bbb);
			
			break;
		case KB_F12:	// ����
			if(fight_control_data.character_idx<0)break;
			aaa.command=FIGHT_ACTION_RUN;
			aaa.source_id=fight_npc_group[fight_control_data.character_idx].base.id;
			aaa.idx=0;
			
			update_fight_action_result(aaa);
			
			bbb.charge_speed=fight_npc_group[fight_control_data.character_idx].base.charge_speed;
			bbb.now_hp=50;
			bbb.id=fight_npc_group[fight_control_data.character_idx].base.id;
			bbb.max_hp=fight_npc_group[fight_control_data.character_idx].base.max_hp;
			bbb.now_mp=fight_npc_group[fight_control_data.character_idx].base.now_mp;
			bbb.max_mp=fight_npc_group[fight_control_data.character_idx].base.max_mp;
			bbb.status=0x00;
			update_fight_value_result(bbb);
			break;

		case PgDn:			// ��������
			if(fight_control_data.character_idx<0)break;
			aaa.command=FIGHT_ACTION_CHANGE;		// reset charge timer
			if(fight_control_data.baobao_idx>=0)
				aaa.source_id=fight_npc_group[fight_control_data.character_idx].base.id;
			else
				aaa.source_id=0;
			aaa.target_id=base_baobao_data.data.baobao_list[0].baobao_id;
//			aaa.target_id=0;		// �����������ɹ�
			aaa.idx=0;
			

			update_fight_action_result(aaa);
			
			
			// ��������������	**** ��ע��!!!!
			// ������ update_fight_value_result(), �ĳ� update  fight_action_change_baobao_data;
			
			
			fight_action_change_baobao_data.filename_id=base_baobao_data.data.baobao_list[1].base.filename_id;
			fight_action_change_baobao_data.life_base=1;
			strcpy((char *)fight_action_change_baobao_data.name,(char *)"������");
			fight_action_change_baobao_data.unit_type=FIGHT_UNIT_TYPE_BAOBAO;
			fight_action_change_baobao_data.data.charge_speed=0;
			fight_action_change_baobao_data.data.id=aaa.target_id;
			fight_action_change_baobao_data.data.status=0x00;
			fight_action_change_baobao_data.data.now_hp=100;
			fight_action_change_baobao_data.data.now_mp=200;
			fight_action_change_baobao_data.data.max_hp=100;
			fight_action_change_baobao_data.data.max_hp=200;
			
			break;
		case End:
//			charge_time_control(FIGHT_COMMAND_BAOBAO,CHARGE_TIME_INITIALIZE);		// ���
//			charge_time_control(FIGHT_COMMAND_BAOBAO,CHARGE_TIME_CONTINUE);			// ��ʼ


			for(i=0;i<7;i++)
			{
//				if(tt)
//				{
//					fight_npc_group[i].base.now_hp=0;
//				}
//				else
//				{
//					fight_npc_group[i].base.max_hp=100;
//					fight_npc_group[i].base.now_hp=100;
//					
//				}

//				fight_npc_group[i].base.status=fight_npc_group[i].base.status|FIGHT_STATUS_SPEED;
				fight_npc_group[i].base.status=0xFFFF;
			}

			if(tt)
				tt=false;
			else
				tt=true;
			break;
			
			
		case Ins:

				






			if(fight_control_data.character_idx<0)break;

//			aaa.command=FIGHT_ACTION_MULTI_MAGIC_DOUBLE;
			aaa.command=FIGHT_ACTION_MULTI_MAGIC;
			aaa.command=FIGHT_ACTION_MULTI_STATUS;
			aaa.source_id=fight_npc_group[fight_control_data.character_idx].base.id;
			aaa.target_id=0;
			aaa.idx=0;			// ���� idx 

			fight_action_multi_magic_double_data.double_count=6;
			update_fight_action_result(aaa);


				
/*
			fight_action_multi_magic_double_data.double_count=7;
			fight_action_multi_magic_double_data.number=5;
			for(j=0;j<7;j++)
			{
				for(i=0;i<5;i++)
				{
					fight_action_multi_magic_double_data.hp[j][i]=10000;
					fight_action_multi_magic_double_data.feed_back_hp[j][i]=100;
					fight_action_multi_magic_double_data.id[i]=10+i;
				}
			}
*/

			bbb.charge_speed=fight_npc_group[fight_control_data.character_idx].base.charge_speed;
			bbb.now_hp=50;
			bbb.id=fight_npc_group[fight_control_data.character_idx].base.id;
			bbb.max_hp=fight_npc_group[fight_control_data.character_idx].base.max_hp;
			bbb.now_mp=fight_npc_group[fight_control_data.character_idx].base.now_mp;
			bbb.max_mp=fight_npc_group[fight_control_data.character_idx].base.max_mp;
			bbb.status=0x00;
			update_fight_value_result(bbb);
			
			break;	
			
/*
					exec_macro(MC_POPSHOP_POP,NULL,0);
					character_popshop_data.active=true;
					store_data.active=true;

					for(i=0;i<10;i++)
					{
						store_data.item[i].filename_id=0;
						store_data.item[i].item_id=i;
						store_data.item[i].number=100*i+10;
					}
					

					for(i=0;i<10;i++)
					{
						character_popshop_data.item[i].filename_id=0;
						character_popshop_data.item[i].item_id=i;
						character_popshop_data.item[i].number=100*i+10;
					}
*/					
/*
					store_data.active=true;
					for(i=0;i<10;i++)
					{
						store_data.item[i].filename_id=0;
						store_data.item[i].item_id=i;
						store_data.item[i].number=100*i+10;
					}
*/
			
			//		sprintf((char *)test.text,"%d%d%d%d#%d",count,count,count,count,count);
			//		count++;
			//		insert_mission_data(test);
			//		set_weather_type(WEATHER_TYPE_CLOUD|WEATHER_SMALL);
			

			break;
			
		case Del:

			if(game_control.main_loop_mode==MAIN_LOOP_MAP_MODE)
			{
				clear_fight_npc_group();		// step 1. ��� fight_npc_group
												// step 2. add fight_base_data

FIGHT_BASE_DATA data;


				data.active=true;
				data.filename_id=map_npc_group[system_control.control_npc_idx].npc_info.filename_id;
				data.id=map_npc_group[system_control.control_npc_idx].npc_info.id;
				data.life_base=2;
				data.max_hp=base_character_data.data.final.max_hp;
				data.now_hp=base_character_data.data.final.now_hp;
				data.max_mp=base_character_data.data.final.max_mp;
				data.now_mp=base_character_data.data.final.now_mp;
				data.unit_type=FIGHT_UNIT_TYPE_CHARACTER;
				data.status=FIGHT_STATUS_NORMAL;
				strcpy((char *)data.name,(char *)map_npc_group[system_control.control_npc_idx].npc_info.name);



				for(i=0;i<10;i+=2)
//				for(i=0;i<1;i+=2)
					{

					data.max_hp=i*100+100;
					data.now_hp=i*100+100;
					data.max_mp=i*200+200;
					data.now_mp=i*200+200;

					if(i>0)
					{
						sprintf((char *)data.name,"�ҷ���Ա%d",i);
						data.id=i;
					}
					data.charge_speed=i;
					insert_fight_unit(data,i);
				}

				data.unit_type=FIGHT_UNIT_TYPE_BAOBAO;

				for(i=1;i<10;i+=2)
				{
					data.filename_id=201+i;
					data.charge_speed=i;
					data.id=i;
					sprintf((char *)data.name,"�ҷ�����%d",i);
					
					SLONG idx;
					// -------------
					if(i==1)
					{
						idx=get_baobao_data_idx(base_baobao_data.data.action_baobao_id);
						if(idx>=0)
						{
							data.id=base_baobao_data.data.action_baobao_id;
							data.now_hp=base_baobao_data.data.baobao_list[idx].data.now_hp;
							data.now_mp=base_baobao_data.data.baobao_list[idx].data.now_mp;
							data.max_hp=base_baobao_data.data.baobao_list[idx].data.max_hp;
							data.max_mp=base_baobao_data.data.baobao_list[idx].data.max_mp;
							data.filename_id=base_baobao_data.data.baobao_list[idx].base.filename_id;
							strcpy((char *)data.name,(char *)base_baobao_data.data.baobao_list[idx].name);
						}
						else
						{
							continue;
						}
					}
					
					insert_fight_unit(data,i);
					
				}
				
				data.unit_type=FIGHT_UNIT_TYPE_MONSTER;
				for(i=10;i<20;i++)
//				for(i=10;i<11;i++)
				{
					data.filename_id=225+i;
					sprintf((char *)data.name,"����%d",i-10);
					data.id=i;
					if(i==10)
						data.unit_type=FIGHT_UNIT_TYPE_BOSS;
					else
						data.unit_type=FIGHT_UNIT_TYPE_MONSTER;
					
					insert_fight_unit(data,i);
				}


				change_to_fight();
			}
			else
			{
				change_to_map();
			}
			
			break;

		case KB_F1:
			fight_charge_start();				// ս����ʼ���	,����ս����һ�κ���
			break;

		case KB_F3:
			fight_charge_pause();				// ս����ͣ���
			break;
		case KB_F4:
			fight_charge_continue();			// ս���������
			break;
		case KB_F5:
			charge_time_control(FIGHT_COMMAND_CHARACTER,CHARGE_TIME_RESET);
			charge_time_control(FIGHT_COMMAND_BAOBAO,CHARGE_TIME_RESET);
			break;
			
		}
		
		
	}
	
	if(system_control.alt_key)
	{
		switch(system_control.data_key)
		{
		case 'i': case 'I':				// ����
			if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)break;
			key_use=true;
			reset_key();
			exec_main_menu_task_item();
			break;
		case 'a': case 'A':				// ����
			if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)
			{
				key_use=true;
				reset_key();
				set_fight_main_command(FIGHT_COMMAND_CHARACTER,FIGHT_COMMAND_AUTO_ATTACK);
			}
				else
			{
				key_use=true;
				reset_key();
				game_control.main_loop_select_npc_task=MAP_MAIN_MENU_TASK_ATTACK;	// ����
			}
			break;
		case 't': case 'T':				// ���
			if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)break;
			key_use=true;
			reset_key();
			exec_main_menu_task_team();
			break;
		case 'g': case 'G':				// ����
			if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)break;
			key_use=true;
			reset_key();
			game_control.main_loop_select_npc_task=MAP_MAIN_MENU_TASK_GIVE;	// ����
			break;
		case 'x': case 'X':				// ����
			if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)break;
			key_use=true;
			reset_key();
			game_control.main_loop_select_npc_task=MAP_MAIN_MENU_TASK_DEAL;	// ����
			break;
		case 'q': case 'Q':				// ����
			if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)break;
			key_use=true;
			reset_key();
			exec_main_menu_task_mission();
			break;
		case 'b': case 'B':				// ����
			if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)break;
			key_use=true;
			reset_key();
			exec_main_menu_task_group();
			break;
		case 's': case 'S':				// ϵͳ
			key_use=true;
			reset_key();
			exec_main_menu_task_system();
			break;
		case 'f': case 'F':				// ������
			key_use=true;
			reset_key();
			exec_main_menu_task_chat();
			break;
		case 'w': case 'W':				// ��������
			if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)break;
			key_use=true;
			reset_key();
			exec_main_menu_task_character_status();
			break;
		case 'p': case 'P':				// ��������
			if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)break;
			key_use=true;
			reset_key();
			exec_main_menu_task_baobao_status();
			break;
		case 'd': case 'D':				// �����ͼ
			if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)break;
			key_use=true;
			reset_key();
			exec_main_menu_task_world_map();
			break;
		case 'v': case 'V':				// ���촰+��
			key_use=true;
			reset_key();
			chat_data_buffer.colume++;
			if(chat_data_buffer.colume>=23)
				chat_data_buffer.colume=3;
			break;
		case 'r': case 'R':
			if(game_control.main_loop_mode==MAIN_LOOP_MAP_MODE)break;
			key_use=true;
			reset_key();
			set_fight_main_command(FIGHT_COMMAND_CHARACTER,FIGHT_COMMAND_RUN);
			set_fight_main_command(FIGHT_COMMAND_BAOBAO,FIGHT_COMMAND_RECOVER);
			break;
		case '0':						// �������
			key_use=true;
			reset_key();
			exec_main_menu_task_phiz();
			break;
		
			

		}



	}



	if(key_use)
	{
		system_control.key=NULL;
		system_control.data_key=NULL;
		system_control.alt_key=false;
	}


}



void display_map_main_menu_status(SLONG status)
{
	SLONG command_offset;
	SLONG i;
	SLONG mouse_x,mouse_y;
	SLONG xl,yl;
	SLONG x,y;
	SLONG channel;
	UCHR command_text[80];
	RECT tt;
	static ULONG timer=0;
	static SLONG old_mouse_x=0,old_mouse_y=0;


	mouse_x=system_control.mouse_x;
	mouse_y=system_control.mouse_y;
	if((old_mouse_x!=mouse_x)||(old_mouse_y!=mouse_y))
	{
		old_mouse_x=mouse_x;
		old_mouse_y=mouse_y;
		timer=system_control.system_timer;
	}

	command_offset=0;
	switch(status)
	{
	case BUTTON_NONE:
		return;
	case BUTTON_SELECT:
		command_offset=0;
		break;
	case BUTTON_PUSH:	
		command_offset=1;
		break;
	}

	for(i=0;i<17;i++)			// MAP_MAIN_MENU ����
	{

		if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)
		{
			if(i<7)continue;
		}
		
		if((mouse_x>=system_image.button_map_main_menu[i].rect.left)&&
			(mouse_x<system_image.button_map_main_menu[i].rect.right)&&
			(mouse_y>=system_image.button_map_main_menu[i].rect.top)&&
			(mouse_y<system_image.button_map_main_menu[i].rect.bottom))
		{

			if(status==BUTTON_PUSH)
			{
				if(check_have_touch_window(system_image.button_map_main_menu[i].rect)>=0)
					continue;
			}


			sprintf((char *)command_text,"MainCommand%d",system_image.button_map_main_menu[i].command_no+command_offset);
			mda_group_exec(system_image.main_menu_handle,command_text,
				system_image.button_map_main_menu[i].rect.left,
				system_image.button_map_main_menu[i].rect.top,
				screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);

			game_control.window_task=system_image.button_map_main_menu[i].ret_code;

			if(system_control.system_timer>=timer+1000)
			{
				xl=strlen((char *)system_image.button_map_main_menu[i].inst)*8+8;
				yl=16+8;
				x=system_image.button_map_main_menu[i].rect.left+(system_image.button_map_main_menu[i].rect.right-system_image.button_map_main_menu[i].rect.left)/2-xl/2;
				y=system_image.button_map_main_menu[i].rect.top-yl;
				if(x<0)x=0;
				if(y<0)y=0;
				if(x+xl>SCREEN_WIDTH)x=SCREEN_WIDTH-xl;
				if(y+yl>SCREEN_HEIGHT)y=SCREEN_HEIGHT-yl;

				put_bar(x,y,xl,yl,color_control.white,screen_buffer);
				put_box(x,y,xl,yl,color_control.black,screen_buffer);
				set_word_color(0,color_control.black);
				print16(x+4,y+4,system_image.button_map_main_menu[i].inst,PEST_PUT,screen_buffer);
				set_word_color(0,color_control.white);
				
			}
			
			
		}
	}


	if((mouse_x>=6)&&
		(mouse_x<6+52)&&
		(mouse_y>=569)&&
		(mouse_y<569+18))
	{

		channel=game_control.chat_channel;
		tt.left=6;
		tt.top=569;
		tt.right=6+52;
		tt.bottom=569+18;
		if(check_have_touch_window(tt)==TTN_NOT_OK)
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.button_chat_channel[channel].command_no+command_offset);
			mda_group_exec(system_image.main_menu_handle,command_text,
				6,
				569,
				screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);

			game_control.window_task=system_image.button_chat_channel[channel].ret_code;
		}
		
	}
	


}




void display_fight_menu_status(SLONG status)
{
	SLONG command_offset;
	SLONG i;
	SLONG mouse_x,mouse_y;
	SLONG x,y;
	SLONG channel;
	IMG256_GROUP *image;

	
	RECT menu_pp[MAX_FIGHT_BUTTON]={
		{  22, 31, 22+56, 31+56 },			// ͷ��		0
		{ 238, 18,238+24, 18+55 },			// ����		1
		{ 264, 18,264+24, 18+55 },			// ����		2
		{ 290, 18,290+24, 18+55 },			// ����		3
		{ 316, 18,316+24, 18+55 },			// ����		4
		{ 342, 18,342+24, 18+55 },			// ����		5
		{ 368, 18,368+24, 18+55 },			// ����		6
		{ 210, 19,210+26, 19+25 },			// ��ƿ		7
		{ 210, 48,210+26, 48+25 },			// ��ƿ		8
		{ 244, 81,244+143,81+38 },			// �ɵ�����	9
		
		{ 429, 26,429+44, 26+44 },			// ͷ��		10
		{ 633, 18,633+24, 18+55 },			// ����		11
		{ 659, 18,659+24, 18+55 },			// ����		12
		{ 685, 18,685+24, 18+55 },			// ����		13
		{ 711, 18,711+24, 18+55 },			// ����		14
		{ 737, 18,737+24, 18+55 },			// ����		15
		{ 763, 18,763+24, 18+55 },			// ����		16
		{ 605, 19,605+26, 19+25 },			// ��ƿ		17
		{ 605, 48,605+26, 48+25 },			// ��ƿ		18
		{ 639, 81,639+143,81+38 },			// �ɵ�����	19
		
		{ 403, 10, 403+15, 10+26 },			// ����ѡ�� 20
		{ 400, 49, 400+21, 49+72 }			// �Զ�		21
			
	};
		
	SLONG button_index[MAX_FIGHT_BUTTON]={
		-1,			// ͷ��		0
		7,			// ����		1
		9,			// ����		2
		11,			// ����		3
		13,			// ����		4
		15,			// ����		5
		17,			// ����		6
		21,			// ��ƿ		7
		23,			// ��ƿ		8
		-1,			// �ɵ�����	9
		
		-1,			// ͷ��		10
		7,			// ����		11
		9,			// ����		12
		11,			// ����		13
		13,			// ����		14
		15,			// ����		15
		19,			// ����		16
		21,			// ��ƿ		17
		23,			// ��ƿ		18
		-1,			// �ɵ�����	19
		
		25,			// ����ѡ�� 20
		27			// �Զ�		21
		
	};
	

	mouse_x=system_control.mouse_x;
	mouse_y=system_control.mouse_y;

	x=fight_image.menu_body.rect.left;
	y=fight_image.menu_body.rect.top;
	mouse_x=mouse_x-x;
	mouse_y=mouse_y-y;

	command_offset=0;
	switch(status)
	{
	case BUTTON_NONE:
		return;
	case BUTTON_SELECT:
		command_offset=0;
		break;
	case BUTTON_PUSH:	
		command_offset=1;
		break;
	}

	for(i=0;i<MAX_FIGHT_BUTTON;i++)			// MAP_MAIN_MENU ����
	{

		if((mouse_x>=menu_pp[i].left)&&
			(mouse_x<menu_pp[i].right)&&
			(mouse_y>=menu_pp[i].top)&&
			(mouse_y<menu_pp[i].bottom))
		{

			if(status==BUTTON_PUSH)
			{
				if(check_have_touch_window(menu_pp[i])>=0)
					continue;
			}


			if(button_index[i]>=0)
			{
				image=fight_image_index[button_index[i]+command_offset];
				
				display_img256_buffer(menu_pp[i].left+x,menu_pp[i].top+y,image->image,screen_buffer);
			}

			if((i==9)||(i==19))			// ȡ�� �ɵ�������
				continue;

			game_control.window_task=i+FIGHT_MENU_CHARACTER_STATUS;

			if((status==BUTTON_PUSH)&&(game_control.window_task!=game_control.old_window_task))
				auto_play_voice(0,(UCHR *)"EFC\\button02.wav",NULL,0,0,0,0,&channel);
			

			i=MAX_FIGHT_BUTTON;
			break;
			
		}
	}


	

}




void npc_talk(ULONG npc_id,UCHR *text)
{
	SLONG idx;
	SLONG display_idx;
	RECT test_rect={ 0,0, SCREEN_TEXT_WINDOW_WIDTH,SCREEN_HEIGHT };
	SLONG mda_handle,mda_index;
	SLONG npc_x,npc_y;
	SLONG window_xl,window_yl;

	if(system_config.head_talk==0)
		return;
	if(system_config.channel_screen_flag==0)
		return;

	if(strlen((char *)text)>=MAX_CHAT_LENGTH)
	{
		text[MAX_CHAT_LENGTH]=NULL;
	}


	if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)
	{
		display_idx=get_screen_text_idx();
		text_out_data.g_pChat->insertString((char *)text,test_rect,false);
		window_xl=test_rect.right-test_rect.left;
		window_yl=test_rect.bottom-test_rect.top;

		idx=get_fight_npc_index(npc_id);
		if(idx<0)return;
		if(npc_id!=fight_npc_group[idx].base.id)return;
	
		mda_handle=fight_npc_group[idx].handle;
		mda_index=get_mda_index(mda_handle);
		if(mda_index<0)return;
		
		npc_x=fight_npc_group[idx].x;
		npc_y=fight_npc_group[idx].y-mda_group[mda_index].img256_head->center_y-20;
		
		strcpy((char *)screen_text_data[display_idx].text,(char *)text);
		screen_text_data[display_idx].timer=system_control.system_timer+SCREEN_TEXT_TIMER;
		screen_text_data[display_idx].rect.left=npc_x-window_xl/2-4;
		screen_text_data[display_idx].rect.top=npc_y-window_yl-4;
		screen_text_data[display_idx].rect.right=screen_text_data[display_idx].rect.left+window_xl+8;
		screen_text_data[display_idx].rect.bottom=screen_text_data[display_idx].rect.top+window_yl+12;
		screen_text_data[display_idx].active=true;
		
	}
	else
	{
		idx=get_map_npc_index(npc_id);
		if(idx<0)return;
		if(npc_id!=map_npc_group[idx].npc_info.id)return;
		
		mda_handle=map_npc_group[idx].handle;
		mda_index=get_mda_index(mda_handle);
		if(mda_index<0)return;

		display_idx=get_screen_text_idx();
		text_out_data.g_pChat->insertString((char *)text,test_rect,false);
		window_xl=test_rect.right-test_rect.left;
		window_yl=test_rect.bottom-test_rect.top;
		
		npc_x=map_npc_group[idx].npc_info.x;
		npc_y=map_npc_group[idx].npc_info.y-mda_group[mda_index].img256_head->center_y-20;
		
		strcpy((char *)screen_text_data[display_idx].text,(char *)text);
		screen_text_data[display_idx].timer=system_control.system_timer+SCREEN_TEXT_TIMER;
		screen_text_data[display_idx].rect.left=npc_x-window_xl/2-4;
		screen_text_data[display_idx].rect.top=npc_y-window_yl-4;
		screen_text_data[display_idx].rect.right=screen_text_data[display_idx].rect.left+window_xl+8;
		screen_text_data[display_idx].rect.bottom=screen_text_data[display_idx].rect.top+window_yl+12;
		screen_text_data[display_idx].active=true;
		
	}
		
}


void reset_map_main_menu_button_frame(void)
{
	SLONG i;
	SLONG mda_index;
	SLONG command_no;

	if((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag))
		return;

	mda_index=get_mda_index(system_image.main_menu_handle);
	if(mda_index<0)return;
	for(i=0;i<17;i++)
	{
		command_no=system_image.button_map_main_menu[i].command_no+1;
		mda_group[mda_index].now_frame[command_no]=0;
	}

	for(i=0;i<6;i++)
	{
		command_no=system_image.button_chat_channel[i].command_no+1;
		mda_group[mda_index].now_frame[command_no]=0;
	}
	
}

void reset_map_small_map_button_frame(void)
{
	SLONG i;
	SLONG mda_index;
	SLONG command_no;

	if((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag))
		return;
	
	mda_index=get_mda_index(system_image.main_menu_handle);
	if(mda_index<0)return;
	for(i=0;i<3;i++)
	{
		command_no=system_image.button_small_map[i].command_no+1;
		mda_group[mda_index].now_frame[command_no]=0;
	}
	
}


void display_map_small_map_status(SLONG status)
{
	SLONG command_offset;
	SLONG i;
	SLONG mouse_x,mouse_y;
	SLONG x,y,xl,yl;
	UCHR command_text[80];
	UCHR weather_text[1024];
	
	static ULONG timer=0;
	static SLONG old_mouse_x=0,old_mouse_y=0;
	
	mouse_x=system_control.mouse_x;
	mouse_y=system_control.mouse_y;
	if((old_mouse_x!=mouse_x)||(old_mouse_y!=mouse_y))
	{
		old_mouse_x=mouse_x;
		old_mouse_y=mouse_y;
		timer=system_control.system_timer;
	}
	
	command_offset=0;
	switch(status)
	{
	case BUTTON_NONE:
		return;
		break;
	case BUTTON_SELECT:
		command_offset=0;
		break;
	case BUTTON_PUSH:		
		command_offset=1;
		break;
	}
	
	for(i=0;i<3;i++)			// MAP_MAIN_MENU ����
	{
		if((mouse_x>=system_image.button_small_map[i].rect.left)&&
			(mouse_x<system_image.button_small_map[i].rect.right)&&
			(mouse_y>=system_image.button_small_map[i].rect.top)&&
			(mouse_y<system_image.button_small_map[i].rect.bottom))
		{


			if(status==BUTTON_PUSH)
			{
				if(check_have_touch_window(system_image.button_small_map[i].rect)>=0)
					continue;
			}
			
			sprintf((char *)command_text,"MainCommand%d",system_image.button_small_map[i].command_no+command_offset);
			mda_group_exec(system_image.main_menu_handle,command_text,
				system_image.button_small_map[i].rect.left,
				system_image.button_small_map[i].rect.top,
				screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);

			game_control.window_task=system_image.button_small_map[i].ret_code;
			
			if(system_control.system_timer>=timer+1000)
			{
				xl=strlen((char *)system_image.button_small_map[i].inst)*8+8;
				yl=16+8;
				x=system_image.button_small_map[i].rect.left-xl;
				y=system_image.button_small_map[i].rect.top;
				if(x<0)x=0;
				if(y<0)y=0;
				if(x+xl>SCREEN_WIDTH)x=SCREEN_WIDTH-xl;
				if(y+yl>SCREEN_HEIGHT)y=SCREEN_HEIGHT-yl;
				
				put_bar(x,y,xl,yl,color_control.white,screen_buffer);
				put_box(x,y,xl,yl,color_control.black,screen_buffer);
				set_word_color(0,color_control.black);
				print16(x+4,y+4,system_image.button_small_map[i].inst,PEST_PUT,screen_buffer);
				set_word_color(0,color_control.white);
				
			}
			
		}
	}
	

	if(status==BUTTON_SELECT)
	{
		if(system_control.system_timer>=timer+1000)
		{
			if(game_control.window_task==-1)
			{
				get_weather_sample_report(weather_text);
				xl=strlen((char *)weather_text)*8+8;
				yl=16+8;
				x=mouse_x;
				y=mouse_y-yl;
				if(x<0)x=0;
				if(y<0)y=0;
				if(x+xl>SCREEN_WIDTH)x=SCREEN_WIDTH-xl;
				if(y+yl>SCREEN_HEIGHT)y=SCREEN_HEIGHT-yl;
				
				put_bar(x,y,xl,yl,color_control.white,screen_buffer);
				put_box(x,y,xl,yl,color_control.black,screen_buffer);
				set_word_color(0,color_control.black);
				print16(x+4,y+4,weather_text,PEST_PUT,screen_buffer);
				set_word_color(0,color_control.white);
				
			}
			
		}
	}
	


}



void exec_fight_main_menu(void)
{

	if(!((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag)))
		return;

	if((game_control.window_task>=0)&&(game_control.window_task!=game_control.old_window_task))
	{
		game_control.old_window_task=game_control.window_task;



		switch(game_control.window_task)
		{
		case FIGHT_MENU_CHARACTER_STATUS:		// ͷ��		0
			exec_main_menu_task_character_fastness();
			break;	
		case FIGHT_MENU_CHARACTER_ATTACK:		// ����		4
			set_fight_main_command(FIGHT_COMMAND_CHARACTER,FIGHT_COMMAND_ATTACK);
			break;	
		case FIGHT_MENU_CHARACTER_MAGIC:		// ����		5
			set_fight_main_command(FIGHT_COMMAND_CHARACTER,FIGHT_COMMAND_MAGIC);
			break;	
		case FIGHT_MENU_CHARACTER_ITEM:			// ����		6
			set_fight_main_command(FIGHT_COMMAND_CHARACTER,FIGHT_COMMAND_ITEM);
			break;	
		case FIGHT_MENU_CHARACTER_PROTECT:		// ����		7
			set_fight_main_command(FIGHT_COMMAND_CHARACTER,FIGHT_COMMAND_PROTECT);
			break;	
		case FIGHT_MENU_CHARACTER_RECOVER:		// ����		8
			set_fight_main_command(FIGHT_COMMAND_CHARACTER,FIGHT_COMMAND_RECOVER);
			break;	
		case FIGHT_MENU_CHARACTER_RUN:			// ����		9
			set_fight_main_command(FIGHT_COMMAND_CHARACTER,FIGHT_COMMAND_RUN);
			break;	
		case FIGHT_MENU_CHARACTER_AUTO_HP:		// ��ƿ		10
			set_fight_main_command(FIGHT_COMMAND_CHARACTER,FIGHT_COMMAND_AUTO_HP);
			break;	
		case FIGHT_MENU_CHARACTER_AUTO_MP:		// ��ƿ		11
			set_fight_main_command(FIGHT_COMMAND_CHARACTER,FIGHT_COMMAND_AUTO_MP);
			break;	
		case FIGHT_MENU_CHARACTER_ACTION:		// �ɵ�����	12
			break;
		case FIGHT_MENU_BAOBAO_STATUS:			// ͷ��		13
			baobao_stack_index=base_baobao_data.data.active_idx;
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			exec_main_menu_task_baobao_fastness();
			break;	
		case FIGHT_MENU_BAOBAO_ATTACK:			// ����		17
			set_fight_main_command(FIGHT_COMMAND_BAOBAO,FIGHT_COMMAND_ATTACK);
			break;	
		case FIGHT_MENU_BAOBAO_MAGIC:			// ����		18
			set_fight_main_command(FIGHT_COMMAND_BAOBAO,FIGHT_COMMAND_MAGIC);
			break;	
		case FIGHT_MENU_BAOBAO_ITEM:			// ����		19
			set_fight_main_command(FIGHT_COMMAND_BAOBAO,FIGHT_COMMAND_ITEM);
			break;	
		case FIGHT_MENU_BAOBAO_PROTECT:			// ����		20
			set_fight_main_command(FIGHT_COMMAND_BAOBAO,FIGHT_COMMAND_PROTECT);
			break;	
		case FIGHT_MENU_BAOBAO_RECOVER:			// ����		21
			set_fight_main_command(FIGHT_COMMAND_BAOBAO,FIGHT_COMMAND_RECOVER);
			break;	
		case FIGHT_MENU_BAOBAO_CHANGE:			// ����		22
			if(fight_control_data.baobao_idx<0)break;
			if(!fight_npc_group[fight_control_data.baobao_idx].base.active)
			{
				set_fight_main_command(FIGHT_COMMAND_CHARACTER,FIGHT_COMMAND_CHANGE);
			}
			else
			{
				set_fight_main_command(FIGHT_COMMAND_BAOBAO,FIGHT_COMMAND_CHANGE);
			}
			break;	
		case FIGHT_MENU_BAOBAO_AUTO_HP:			// ��ƿ		23
			set_fight_main_command(FIGHT_COMMAND_BAOBAO,FIGHT_COMMAND_AUTO_HP);
			break;	
		case FIGHT_MENU_BAOBAO_AUTO_MP:			// ��ƿ		24
			set_fight_main_command(FIGHT_COMMAND_BAOBAO,FIGHT_COMMAND_AUTO_MP);
			break;	
		case FIGHT_MENU_BAOBAO_ACTION:			// �ɵ�����	25
			break;	
		case FIGHT_MENU_ATTACK_WINDOW:			// ����ѡ�� 26
			exec_fight_talk_window();
			break;	
		case FIGHT_MENU_AUTO_ATTACK:			// �Զ�		27
			set_fight_main_command(FIGHT_COMMAND_CHARACTER,FIGHT_COMMAND_AUTO_ATTACK);
			break;	

		}

// -----

		
	}
		

	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;

}




void exec_map_main_menu(void)
{
	
	if(!((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag)))
		return;
	
	if((game_control.window_task>=0)&&(game_control.window_task!=game_control.old_window_task))
	{
		game_control.old_window_task=game_control.window_task;
		
		switch(game_control.window_task)
		{
			// ----- main icon
		case MAP_MAIN_MENU_TASK_CHATCHANNEL:			// Ƶ���л�
			exec_main_menu_task_channel();
			break;
		case MAP_MAIN_MENU_TASK_SYSTEM:					// ϵͳ window
			exec_main_menu_task_system();
			break;
		case MAP_MAIN_MENU_TASK_CHAT:					// ������
			exec_main_menu_task_chat();
			break;
		case MAP_MAIN_MENU_TASK_ITEM:					// ������Ʒ
			exec_main_menu_task_item();
			break;
		case MAP_MAIN_MENU_TASK_MISSION:				// ����
			exec_main_menu_task_mission();
			break;
		case MAP_MAIN_MENU_TASK_GROUP:					// ����
			exec_main_menu_task_group();
			break;
		case MAP_MAIN_MENU_TASK_TEAM:					// ���
			exec_main_menu_task_team();
			break;
		case MAP_MAIN_MENU_TASK_GIVE:					// ����
			game_control.main_loop_select_npc_task=MAP_MAIN_MENU_TASK_GIVE;	// ����
			set_mouse_cursor(MOUSE_IMG_GIVE);
			break;
		case MAP_MAIN_MENU_TASK_DEAL:
			game_control.main_loop_select_npc_task=MAP_MAIN_MENU_TASK_DEAL;	// ����
			set_mouse_cursor(MOUSE_IMG_DEAL);
			break;
		case MAP_MAIN_MENU_TASK_ATTACK:
			game_control.main_loop_select_npc_task=MAP_MAIN_MENU_TASK_ATTACK;	// ����
			set_mouse_cursor(MOUSE_IMG_ATTACK);
			break;
			
			// ---- small icon
		case MAP_MAIN_MENU_TASK_CHATWIN:				// �л����� window 
			if(game_control.chat_window_type==0)
				game_control.chat_window_type=1;
			else
				game_control.chat_window_type=0;
			update_chat_window();
			break;
		case MAP_MAIN_MENU_TASK_VIEW:					// ���� + ��
			chat_data_buffer.colume++;
			if(chat_data_buffer.colume>=23)
				chat_data_buffer.colume=3;
			break;
		case MAP_MAIN_MENU_TASK_SCROLL:					// ���� pause
			if(game_control.chat_pause_flag==0)
				game_control.chat_pause_flag=1;
			else
				game_control.chat_pause_flag=0;
			break;
		case MAP_MAIN_MENU_TASK_UP:						// �Ͼ�
			scroll_chat_data(Up);
			break;
		case MAP_MAIN_MENU_TASK_DOWN:					// �¾�
			scroll_chat_data(Dn);
			break;
		case MAP_MAIN_MENU_TASK_PHIZ:					// �������
			exec_main_menu_task_phiz();
			break;
		case MAP_MAIN_MENI_TASK_HISTORY:
			exec_main_menu_task_history();				// ��ʷ����
			break;
		case MAP_MAIN_MENU_TASK_DISCARE:				// ������
			if(system_control.person_npc_id<0)break;
			if(system_control.person_npc_id==system_control.control_user_id)break;
			add_friend_list(system_control.person_npc_id,FRIEND_MASK);
			break;
		}
		// -----
		
		
	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}


void exec_map_character_status(void)
{
	SLONG channel;
	SLONG idx;	
	
	switch(system_control.mouse_key)
	{
	case MS_LDn:
	case MS_LDrag:
		if((game_control.window_task>=0)&&(game_control.window_task!=game_control.old_window_task))
		{
			game_control.old_window_task=game_control.window_task;
			switch(game_control.window_task)
			{
			case MAP_MAIN_MENU_TASK_CHARACTER_STATUS:		// ����״̬
				auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
				exec_main_menu_task_character_status();
				break;
			case MAP_MAIN_MENU_TASK_BAOBAO_STATUS:			// ����״̬  
				auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
				exec_main_menu_task_baobao_status();
				break;
			}
		}
		break;
	case MS_RDn:
	case MS_RDrag:
		if((game_control.window_task>=0)&&(game_control.window_task!=game_control.old_window_task))
		{
			game_control.old_window_task=game_control.window_task;
			switch(game_control.window_task)
			{
			case MAP_MAIN_MENU_TASK_CHARACTER_STATUS:		// ����״̬
				auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
				exec_main_menu_task_item();
				break;
			case MAP_MAIN_MENU_TASK_BAOBAO_STATUS:			// ����״̬  
				auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
				check_baobao_stack();
				if(base_baobao_data.data.action_baobao_id==0)break;
				idx=get_baobao_data_idx(base_baobao_data.data.action_baobao_id);
				baobao_stack_index=idx;
				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
				exec_main_menu_task_baobao_item();
				break;
			case MAP_MAIN_MENU_TASK_CHARACTER_HP:			// �����Զ�+Ѫ
				exec_character_auto_full_hp();
				break;
			case MAP_MAIN_MENU_TASK_CHARACTER_MP:			// �����Զ�+��
				exec_character_auto_full_mp();
				break;
			case MAP_MAIN_MENU_TASK_BAOBAO_HP:				// �����Զ�+Ѫ
				if(base_baobao_data.data.action_baobao_id==0)break;
				exec_baobao_auto_full_hp(base_baobao_data.data.action_baobao_id);
				break;
			case MAP_MAIN_MENU_TASK_BAOBAO_MP:				// �����Զ�+��
				if(base_baobao_data.data.action_baobao_id==0)break;
				exec_baobao_auto_full_mp(base_baobao_data.data.action_baobao_id);
				break;
				
			}
		}
		break;		
		
	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}



void exec_map_small_map(void)
{
	
	if(!((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag)))
		return;
	
	if((game_control.window_task>=0)&&(game_control.window_task!=game_control.old_window_task))
	{
		game_control.old_window_task=game_control.window_task;
		
		switch(game_control.window_task)
		{
		case MAP_MAIN_MENU_TASK_SWITCH:
			switch_screen_mode();
			game_control.old_window_task=-1;		// ���⴦�� �л� Fullscreen ���״̬��ʧ,����task ����
			break;
		case MAP_MAIN_MENU_TASK_SMALL:				// С��ͼ
			if((change_map_info.map_attrib_info.small_map_flag==0)||
				(change_map_info.small_map_file_no<0))break;
			exec_main_menu_task_small_map();
			break;
		case MAP_MAIN_MENU_TASK_WORLD:				// �����ͼ
			exec_main_menu_task_world_map();
			break;
		}
		
		
		
	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}



/*******************************************************************************************************************}
{	channel																											}
********************************************************************************************************************/
void exec_main_menu_task_channel(void)
{
	RECT rect; 

	if(game_control.window_channel<0)			// Create window
	{
		rect.left=system_image.channel_body.rect.left;
		rect.top=system_image.channel_body.rect.top;
		rect.right=system_image.channel_body.rect.right;
		rect.bottom=system_image.channel_body.rect.bottom;
		game_control.window_channel=insert_window_base(rect ,&window_draw_select_channel,&window_exec_select_channel,WINDOW_TYPE_NORMAL);

	}
	else										// close window
	{
		delete_window_base(game_control.window_channel);
		game_control.window_channel=-1;
	}	
}

/*******************************************************************************************************************}
{	system																											}
********************************************************************************************************************/
void exec_main_menu_task_system(void)
{
	RECT rect; 
	
	if(game_control.window_system<0)			// Create window
	{
// ------ backup system_config
		memcpy(&backup_system_config,&system_config,sizeof ( struct SYSTEM_CONFIG_STRUCT));
		rect.left=system_image.system_background.rect.left;
		rect.top=system_image.system_background.rect.top;
		rect.right=system_image.system_background.rect.right;
		rect.bottom=system_image.system_background.rect.bottom;
		game_control.window_system=insert_window_base(rect ,&window_draw_system,&window_exec_system,WINDOW_TYPE_NORMAL);
		game_control.window_system_button_idx=-1;
	}
	else										// close window
	{
// ---- restore 
		delete_window_base(game_control.window_system);
		game_control.window_system=-1;
	}	
}

/*******************************************************************************************************************}
{	quit																											}
********************************************************************************************************************/
void exec_main_menu_task_quit(void)
{
	RECT rect; 

	if(game_control.window_quit>=0)
		delete_window_base(game_control.window_quit);
	game_control.window_quit=-1;
	

// ------ 242 186
	rect.left=242;
	rect.top=186;
	rect.right=242+(system_image.quit_body.rect.right-system_image.quit_body.rect.left);
	rect.bottom=186+(system_image.quit_body.rect.bottom-system_image.quit_body.rect.top);
	game_control.window_quit=insert_window_base(rect ,&window_draw_quit,&window_exec_quit,WINDOW_TYPE_NORMAL);
	game_control.window_quit_button_idx=-1;
	
}

/*******************************************************************************************************************}
{	chat																											}
********************************************************************************************************************/
void exec_main_menu_task_chat(void)
{
	RECT rect; 
	

	if(game_control.chat_message_in>0)
	{
		if(game_control.window_receive_message<0)			// Create window
		{
			rect.left=system_image.chat_receive_body.rect.left;
			rect.top=system_image.chat_receive_body.rect.top;
			rect.right=system_image.chat_receive_body.rect.right;
			rect.bottom=system_image.chat_receive_body.rect.bottom;
		
//			chat_receive_data.channel=CHAT_CHANNEL_NONE;

			PostChatNetMessage(NET_MSG_CHAT_GET_MAIL, 0);
			
			game_control.window_receive_message=insert_window_base(rect ,&window_draw_chat_receive,&window_exec_chat_receive,WINDOW_TYPE_NORMAL);
		}
		return;			
	}


	if(game_control.window_chat<0)			// Create window
	{
		rect.left=system_image.chat_body.rect.left;
		rect.top=system_image.chat_body.rect.top;
		rect.right=system_image.chat_body.rect.right;
		rect.bottom=system_image.chat_body.rect.bottom;
		game_control.window_chat_select_idx=-1;
		reload_chat_friend_list();
		game_control.window_chat=insert_window_base(rect ,&window_draw_chat,&window_exec_chat,WINDOW_TYPE_NORMAL);
	}
	else										// close window
	{
		delete_window_base(game_control.window_chat);
		game_control.window_chat=-1;
	}	
}

/*******************************************************************************************************************}
{	phiz																											}
********************************************************************************************************************/
void exec_main_menu_task_phiz(void)
{
	RECT rect; 
	
	if(game_control.window_phiz<0)			// Create window
	{
		rect.left=PHIZ_WINDOW_X;
		rect.top=PHIZ_WINDOW_Y;
		rect.right=PHIZ_WINDOW_X+PHIZ_FONT_X*9;
		rect.bottom=PHIZ_WINDOW_Y+PHIZ_FONT_Y*9;
		game_control.window_phiz=insert_window_base(rect ,&window_draw_phiz,&window_exec_phiz,WINDOW_TYPE_NORMAL);
		
	}
	else										// close window
	{
		delete_window_base(game_control.window_phiz);
		game_control.window_phiz=-1;
	}	
}

/*******************************************************************************************************************}
{	character status																								}
********************************************************************************************************************/
void exec_main_menu_task_character_status(void)
{
	RECT rect; 
	
	if(game_control.window_character_status<0)			// Create window
	{
		rect.left=system_image.character_attrib_body.rect.left;
		rect.top=system_image.character_attrib_body.rect.top;
		rect.right=system_image.character_attrib_body.rect.right;
		rect.bottom=system_image.character_attrib_body.rect.bottom;
		game_control.window_character_status=insert_window_base(rect ,&window_draw_character_status,&window_exec_character_status,WINDOW_TYPE_NORMAL);

	}
	else										// close window
	{
		delete_window_base(game_control.window_character_status);
		game_control.window_character_status=-1;
	}	
}


/*******************************************************************************************************************}
{	baobao status																									}
********************************************************************************************************************/
void exec_main_menu_task_baobao_status(void)
{
	RECT rect; 
	
	if(game_control.window_baobao_status<0)			// Create window
	{
		rect.left=system_image.baobao_attrib_body.rect.left+200;
		rect.top=system_image.baobao_attrib_body.rect.top-64;
		rect.right=system_image.baobao_attrib_body.rect.right+200;
		rect.bottom=system_image.baobao_attrib_body.rect.bottom-64;

		g_PetNameEdit.Init();

		if((baobao_stack_index>=0)&&(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id!=0))
		{
			g_PetNameEdit.setText(base_baobao_data.data.baobao_list[baobao_stack_index].name);
		}
		game_control.window_baobao_status=insert_window_base(rect ,&window_draw_baobao_status,&window_exec_baobao_status,WINDOW_TYPE_NORMAL);
	}
	else										// close window
	{
		delete_window_base(game_control.window_baobao_status);
		game_control.window_baobao_status=-1;
		g_PetNameEdit.Shutdown();		
	}	
}


/*******************************************************************************************************************}
{	history																											}
********************************************************************************************************************/
void exec_main_menu_task_history(void)
{
	RECT rect; 
	
	if(game_control.window_history<0)			// Create window
	{
		rect.left=system_image.history_body.rect.left;
		rect.top=system_image.history_body.rect.top;
		rect.right=system_image.history_body.rect.right;
		rect.bottom=system_image.history_body.rect.bottom;
		copy_chat_data_to_hirstory();
		game_control.window_history=insert_window_base(rect ,&window_draw_history,&window_exec_history,WINDOW_TYPE_NORMAL);
	}
	else										// close window
	{
		delete_window_base(game_control.window_history);
		game_control.window_history=-1;
	}	
}


/*******************************************************************************************************************}
{	item																											}
********************************************************************************************************************/
void exec_main_menu_task_item(void)
{
	RECT rect; 
	

	if(!character_pocket_data.active)
		request_character_pocket_data();

	clear_detail_item_inst();
	

	if(game_control.window_item<0)			// Create window
	{
		rect.left=system_image.item_body.rect.left-200;
		rect.top=system_image.item_body.rect.top;
		rect.right=system_image.item_body.rect.right-200;
		rect.bottom=system_image.item_body.rect.bottom;
		game_control.window_item=insert_window_base(rect ,&window_draw_item,&window_exec_item,WINDOW_TYPE_NORMAL);
	}
	else										// close window
	{
		delete_window_base(game_control.window_item);
		game_control.window_item=-1;
	}	
}


/*******************************************************************************************************************}
{	mission																											}
********************************************************************************************************************/
void exec_main_menu_task_mission(void)
{
	RECT rect; 
	
	if(game_control.window_mission<0)			// Create window
	{
		rect.left=system_image.mission_body.rect.left;
		rect.top=system_image.mission_body.rect.top;
		rect.right=system_image.mission_body.rect.right;
		rect.bottom=system_image.mission_body.rect.bottom;
		game_control.window_mission=insert_window_base(rect ,&window_draw_mission,&window_exec_mission,WINDOW_TYPE_NORMAL);
// ------ Request mission data 
		clear_mission_data();
		request_mission_data();

	}
	else										// close window
	{
		delete_window_base(game_control.window_mission);
		game_control.window_mission=-1;
	}	
}


/*******************************************************************************************************************}
{	group																											}
********************************************************************************************************************/
void exec_main_menu_task_group(void)
{
	RECT rect; 
	
	if(game_control.window_group<0)			// Create window
	{
		rect.left=system_image.group_body.rect.left;
		rect.top=system_image.group_body.rect.top;
		rect.right=system_image.group_body.rect.right;
		rect.bottom=system_image.group_body.rect.bottom;
		game_control.window_group=insert_window_base(rect ,&window_draw_group,&window_exec_group,WINDOW_TYPE_NORMAL);
	}
	else										// close window
	{
		delete_window_base(game_control.window_group);
		game_control.window_group=-1;
	}	
}



/*******************************************************************************************************************}
{	send message																									}
********************************************************************************************************************/
void exec_send_message(void)
{
	RECT rect; 
	
	if(game_control.window_send_message>=0)			// already open
		return;

	rect.left=system_image.chat_send_body.rect.left;
	rect.top=system_image.chat_send_body.rect.top;
	rect.right=system_image.chat_send_body.rect.right;
	rect.bottom=system_image.chat_send_body.rect.bottom;

	s_FriendEdit.Init();
	system_control.data_key=NULL;
	system_control.key=NULL;
	

	game_control.window_send_message=insert_window_base(rect ,&window_draw_send_message,&window_exec_send_message,WINDOW_TYPE_NORMAL);	
	
}


/*******************************************************************************************************************}
{	chat attrib																										}
********************************************************************************************************************/
void exec_chat_attrib(void)
{
	RECT rect; 
	
	if(game_control.window_chat_attrib>=0)			// already open
		return;

	rect.left=system_image.chat_attrib_body.rect.left;
	rect.top=system_image.chat_attrib_body.rect.top;
	rect.right=system_image.chat_attrib_body.rect.right;
	rect.bottom=system_image.chat_attrib_body.rect.bottom;

	game_control.window_chat_attrib=insert_window_base(rect ,&window_draw_chat_attrib,&window_exec_chat_attrib,WINDOW_TYPE_NORMAL);	
}


/*******************************************************************************************************************}
{	group send message																								}
********************************************************************************************************************/
void exec_chat_group_send(void)
{
	RECT rect; 
	
	if(game_control.window_chat_group_send>=0)			// already open
		return;

	rect.left=system_image.chat_group_send_body.rect.left;
	rect.top=system_image.chat_group_send_body.rect.top;
	rect.right=system_image.chat_group_send_body.rect.right;
	rect.bottom=system_image.chat_group_send_body.rect.bottom;

	game_control.window_chat_group_send=insert_window_base(rect ,&window_draw_chat_group_send,&window_exec_chat_group_send,WINDOW_TYPE_NORMAL);	
}



/*******************************************************************************************************************}
{	friend firstory																									}
********************************************************************************************************************/
void exec_friend_hirstory(void)
{
	RECT rect; 
	
	if(game_control.window_chat_hirstory>=0)			// already open
		return;

	rect.left=system_image.history_body.rect.left;
	rect.top=system_image.history_body.rect.top;
	rect.right=system_image.history_body.rect.right;
	rect.bottom=system_image.history_body.rect.bottom;

	reload_friend_data_record(send_message_data.id);

	game_control.window_chat_hirstory=insert_window_base(rect ,&window_draw_chat_hirstory,&window_exec_chat_hirstory,WINDOW_TYPE_NORMAL);	
}



/*******************************************************************************************************************}
{	Team																											}
********************************************************************************************************************/
void exec_main_menu_task_team(void)
{
	int status;
	RECT rect; 

	team_request_select_start_index=0;
	
	if(game_control.team_message_in>0)
	{
		if(game_control.window_team_request>=0)					// already open
			return;
		rect.left=system_image.team_body.rect.left+40;
		rect.top=system_image.team_body.rect.top+40;
		rect.right=system_image.team_body.rect.right+40;
		rect.bottom=system_image.team_body.rect.bottom+40;
		
		game_control.window_team_request=insert_window_base(rect ,&window_draw_team_request,&window_exec_team_request,WINDOW_TYPE_NORMAL);	

		game_control.team_message_in=0;

		return;
	}


	status=map_npc_group[system_control.control_npc_idx].npc_info.status;
	if((status&NPC_STATUS_LEADER)||					// ����Ƕӳ�
		(status&NPC_STATUS_FOLLOW))					// ����Ƕ�Ա   
	{												// �Ѿ��ڶ������� �򿪶������ window
		if(game_control.window_team>=0)					// already open
			return;
		rect.left=system_image.team_body.rect.left;
		rect.top=system_image.team_body.rect.top;
		rect.right=system_image.team_body.rect.right;
		rect.bottom=system_image.team_body.rect.bottom;

//		clear_team_list_data();
//		request_team_list_data();			
		
		game_control.window_team=insert_window_base(rect ,&window_draw_team,&window_exec_team,WINDOW_TYPE_NORMAL);	
		
	}
	else											// ѡ��ӳ�
	{
		clear_team_request_list_data();
		clear_team_list_data();
		
		game_control.main_loop_select_npc_task=MAP_MAIN_MENU_TASK_TEAM;
		set_mouse_cursor(MOUSE_IMG_TEAM);
	}
		

	
}


void exec_main_menu_task_team_action(ULONG id)
{
	if(id==system_control.control_user_id)
	{
// ----- send �������ӳ�

		postNetMessage(NET_MSG_PLAYER_CREATE_GROUP, id);
	}
	else
	{
// ----- send ��������Ա
		postNetMessage(NET_MSG_PLAYER_ADD_GROUP_LIST, id);
	}
}



/*******************************************************************************************************************}
{	friend search																									}
********************************************************************************************************************/
void exec_friend_search(void)
{
	RECT rect; 
	
	if(game_control.window_chat_search>=0)			// already open
		return;

	rect.left=system_image.chat_search.rect.left;
	rect.top=system_image.chat_search.rect.top;
	rect.right=system_image.chat_search.rect.right;
	rect.bottom=system_image.chat_search.rect.bottom;

	g_FindNickNameEdit.Init();
	g_FindIDEdit.Init();
	
	g_FindNickNameEdit.setFocus();

	game_control.window_chat_search=insert_window_base(rect ,&window_draw_chat_search,&window_exec_chat_search,WINDOW_TYPE_NORMAL);	
}



void exec_main_menu_task_small_map(void)
{
	RECT rect; 
	

	if(game_control.window_small_map<0)			// Create window
	{
		rect.left=SCREEN_WIDTH/2-SMALL_MAP_WIDTH/2-4;
		rect.top=SCREEN_HEIGHT/2-(SMALL_MAP_HEIGHT/8*8)/2-4;
		rect.right=rect.left+SMALL_MAP_WIDTH+8;
		rect.bottom=rect.top+(SMALL_MAP_HEIGHT/8*8)+8;
		game_control.window_small_map=insert_window_base(rect ,&window_draw_small_map,&window_exec_small_map,WINDOW_TYPE_NORMAL);
	}
	else										// close window
	{
		delete_window_base(game_control.window_small_map);
		game_control.window_small_map=-1;
	}	
	

}


void exec_main_menu_task_world_map(void)
{
	RECT rect; 
	

	if(game_control.window_world_map<0)			// Create window
	{

		if(load_jpg_file((UCHR *)"system\\world.JPG",screen_channel0,0,0,SCREEN_WIDTH,SCREEN_HEIGHT)!=TTN_OK)
		{
			return;
		}

		rect.left=0;
		rect.top=0;
		rect.right=SCREEN_WIDTH;
		rect.bottom=SCREEN_HEIGHT;
		game_control.window_world_map=insert_window_base(rect ,&window_draw_world_map,&window_exec_world_map,WINDOW_TYPE_NORMAL);

		game_control.old_window_task=-1;
		
	}
	else										// close window
	{
		delete_window_base(game_control.window_world_map);
		game_control.window_world_map=-1;
	}	
	
	
}




/*******************************************************************************************************************}
{	Give																											}
********************************************************************************************************************/
void exec_main_menu_task_give(void)
{
	RECT rect; 
	SLONG xl,yl;


// --- clear give data
	character_pocket_data.give_idx[0]=-1;
	character_pocket_data.give_idx[1]=-1;
	character_pocket_data.give_idx[2]=-1;

	character_pocket_data.give_number[0]=0;
	character_pocket_data.give_number[1]=0;
	character_pocket_data.give_number[2]=0;

	character_pocket_data.give_money=0;

	if(!character_pocket_data.active)
		request_character_pocket_data();
	
	clear_detail_item_inst();
	

	if(game_control.window_give>=0)			// already open
		return;

	SetRect(&rect, 161, 281, 161+99, 281+16);
	g_GiveEdit.Init(rect, 8);
	g_GiveEdit.setMaxNum(character_bank_data.now_money);

	xl=system_image.item_public_body.rect.right-system_image.item_public_body.rect.left;
	yl=(system_image.item_public_body.rect.bottom-system_image.item_public_body.rect.top)+
		(system_image.item_action_body.rect.bottom-system_image.item_action_body.rect.top);

	rect.left=system_image.item_public_body.rect.left;
	rect.top=system_image.item_public_body.rect.top;
	rect.right=rect.left+xl;
	rect.bottom=rect.top+yl;
	game_control.window_give=insert_window_base(rect ,&window_draw_give,&window_exec_give,WINDOW_TYPE_NORMAL);
	

}



/*******************************************************************************************************************}
{	Deal																											}
********************************************************************************************************************/
void exec_main_menu_task_deal(void)
{

	if(!character_pocket_data.active)
		request_character_pocket_data();
	
	if(game_control.window_deal>=0)			// already open
		return;

	request_deal();

}



/*******************************************************************************************************************}
{	Skill																											}
********************************************************************************************************************/
void exec_main_menu_task_skill(void)
{
	RECT rect; 


	if(!game_control.character_skill_ready)
		request_character_skill_data();


	game_control.skill_start_index=0;

	if(game_control.window_skill<0)			// Create window
	{
		rect.left=system_image.skill_body.rect.left;
		rect.top=system_image.skill_body.rect.top;
		rect.right=system_image.skill_body.rect.right;
		rect.bottom=system_image.skill_body.rect.bottom;
		game_control.window_skill=insert_window_base(rect ,&window_draw_skill,&window_exec_skill,WINDOW_TYPE_NORMAL);
	}
	else										// close window
	{
		delete_window_base(game_control.window_skill);
		game_control.window_skill=-1;
	}	
	
}



/*******************************************************************************************************************}
{	Title																											}
********************************************************************************************************************/
void exec_main_menu_task_title(void)
{
	RECT rect; 

	if(game_control.window_title<0)			// Create window
	{
		rect.left=system_image.title_body.rect.left;
		rect.top=system_image.title_body.rect.top;
		rect.right=system_image.title_body.rect.right;
		rect.bottom=system_image.title_body.rect.bottom;
		game_control.window_title=insert_window_base(rect ,&window_draw_title,&window_exec_title,WINDOW_TYPE_NORMAL);
	}
	else										// close window
	{
		delete_window_base(game_control.window_title);
		game_control.window_title=-1;
	}	
	
}



/*******************************************************************************************************************}
{	Baobao item																										}
********************************************************************************************************************/
void exec_main_menu_task_baobao_item(void)
{
	RECT rect; 
	SLONG xl,yl;


	if(!character_pocket_data.active)
		request_character_pocket_data();
	
	clear_detail_item_inst();


	if(game_control.window_baobao_item<0)			// Create window
	{
		rect.left=system_image.item_public_body.rect.left;
		rect.top=system_image.item_public_body.rect.top;

		xl=system_image.item_public_body.rect.right-system_image.item_public_body.rect.left;
		yl=(system_image.item_public_body.rect.bottom-system_image.item_public_body.rect.top)+
			(system_image.baobao_item_action.rect.bottom-system_image.baobao_item_action.rect.top);
		

		rect.right=rect.left+xl;
		rect.bottom=rect.top+yl;
		game_control.window_baobao_item=insert_window_base(rect ,&window_draw_baobao_item,&window_exec_baobao_item,WINDOW_TYPE_NORMAL);
	}
	else										// close window
	{
		delete_window_base(game_control.window_baobao_item);
		game_control.window_baobao_item=-1;
	}	
	
}



/*******************************************************************************************************************}
{	character fastness																								}
********************************************************************************************************************/
void exec_main_menu_task_character_fastness(void)
{
	RECT rect; 
	

	if(!character_fastness_data.active)
	{
		request_character_fastness_data();
	}
	
	if(game_control.window_character_fastness<0)			// Create window
	{
		if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)
		{
			rect.left=40;
			rect.top=25;
		}
		else
		{
			rect.left=364;
			rect.top=129;
		}
		rect.right=rect.left+FASTNESS_WINDOW_WIDTH;
		rect.bottom=rect.top+FASTNESS_WINDOW_HEIGHT;
		game_control.window_character_fastness=insert_window_base(rect ,&window_draw_character_fastness,&window_exec_character_fastness,WINDOW_TYPE_NORMAL);

	}
	else										// close window
	{
		delete_window_base(game_control.window_character_fastness);
		game_control.window_character_fastness=-1;
	}	
}





/*******************************************************************************************************************}
{	baobao fastness																									}
********************************************************************************************************************/
void exec_main_menu_task_baobao_fastness(void)
{
	RECT rect; 


	if(baobao_stack_index<0)return;
	if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)return;
	if(!baobao_fastness_data[baobao_stack_index].active)
	{
		request_baobao_fastness_data();
	}
	

	if(game_control.window_baobao_fastness<0)			// Create window
	{
		if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)
		{
			rect.left=300;
			rect.top=25;
		}
		else
		{
			rect.left=562;
			rect.top=64;
		}
		rect.right=rect.left+FASTNESS_WINDOW_WIDTH;
		rect.bottom=rect.top+FASTNESS_WINDOW_HEIGHT;
		game_control.window_baobao_fastness=insert_window_base(rect ,&window_draw_baobao_fastness,&window_exec_baobao_fastness,WINDOW_TYPE_NORMAL);

	}
	else										// close window
	{
		delete_window_base(game_control.window_baobao_fastness);
		game_control.window_baobao_fastness=-1;
	}	
}




/*******************************************************************************************************************}
{	Fight character skill																							}
********************************************************************************************************************/
void exec_fight_menu_task_skill(void)
{
	SLONG window_xl,window_yl;
	RECT rect; 
	
	if(game_control.window_fight_character_skill>=0)		// Already open
		return;

	window_xl=(MAX_SKILL_NAME_LENGTH*8)+16+16;		// 1 ��
	window_yl=16+16+18*20+20-4;						// 20 ����ʾ + ������ʾ

	rect.left=SCREEN_WIDTH/2-window_xl/2;
	rect.top=SCREEN_HEIGHT/2-window_yl/2;
	rect.right=rect.left+window_xl;
	rect.bottom=rect.top+window_yl;
	game_control.window_fight_character_skill=insert_window_base(rect ,&window_draw_fight_character_skill,&window_exec_fight_character_skill,WINDOW_TYPE_NORMAL);
	
}


/*******************************************************************************************************************}
{	Fight baobao skill																							}
********************************************************************************************************************/
void exec_fight_menu_task_baobao_skill(void)
{
	SLONG window_xl,window_yl;
	RECT rect; 
	
	if(game_control.window_fight_baobao_skill>=0)		// Already open
		return;

	window_xl=(MAX_SKILL_NAME_LENGTH*8)+16+16;		// 1 ��
	window_yl=16+16+18*8+20-4;						// 8 ����ʾ + ������ʾ

	rect.left=SCREEN_WIDTH/2-window_xl/2+100;		// ƫ��
	rect.top=SCREEN_HEIGHT/2-window_yl/2;
	rect.right=rect.left+window_xl;
	rect.bottom=rect.top+window_yl;
	game_control.window_fight_baobao_skill=insert_window_base(rect ,&window_draw_fight_baobao_skill,&window_exec_fight_baobao_skill,WINDOW_TYPE_NORMAL);
	
}


/*******************************************************************************************************************}
{	Fight character item																							}
********************************************************************************************************************/
void exec_fight_menu_task_character_item(void)
{
	RECT rect; 
	SLONG xl,yl;
	

	if(!character_pocket_data.active)
		request_character_pocket_data();
	
	clear_detail_item_inst();
	
	if(game_control.window_fight_character_item>=0)		// Already open
		return;

	xl=system_image.item_public_body.rect.right-system_image.item_public_body.rect.left;
	yl=(system_image.item_public_body.rect.bottom-system_image.item_public_body.rect.top)+
		(system_image.baobao_item_action.rect.bottom-system_image.baobao_item_action.rect.top);
	
	rect.left=SCREEN_WIDTH/2-xl/2;
	rect.top=SCREEN_HEIGHT/2-yl/2;
	rect.right=rect.left+xl;
	rect.bottom=rect.top+yl;
	game_control.window_fight_character_item=insert_window_base(rect ,&window_draw_fight_character_item,&window_exec_fight_character_item,WINDOW_TYPE_NORMAL);
	
}

/*******************************************************************************************************************}
{	Fight baobao item																								}
********************************************************************************************************************/
void exec_fight_menu_task_baobao_item(void)
{
	SLONG xl,yl;
	RECT rect; 

	
	if(!character_pocket_data.active)
		request_character_pocket_data();
	
	clear_detail_item_inst();
	
	if(game_control.window_fight_baobao_item>=0)		// Already open
		return;

	
	xl=system_image.item_public_body.rect.right-system_image.item_public_body.rect.left;
	yl=(system_image.item_public_body.rect.bottom-system_image.item_public_body.rect.top)+
		(system_image.baobao_item_action.rect.bottom-system_image.baobao_item_action.rect.top);
	
	rect.left=SCREEN_WIDTH/2-xl/2+200;
	rect.top=SCREEN_HEIGHT/2-yl/2;
	rect.right=rect.left+xl;
	rect.bottom=rect.top+yl;
	game_control.window_fight_baobao_item=insert_window_base(rect ,&window_draw_fight_baobao_item,&window_exec_fight_baobao_item,WINDOW_TYPE_NORMAL);
	
}



/*******************************************************************************************************************}
{	Fight baobao change																								}
********************************************************************************************************************/
void exec_fight_menu_task_baobao_change(void)
{
	SLONG xl,yl;
	RECT rect; 

	
	if(!base_baobao_data.active)
		request_base_baobao_data();
	
	if(game_control.window_fight_baobao_change>=0)		// Already open
		return;

	
	xl=system_image.baobao_attrib_body.rect.right-system_image.baobao_attrib_body.rect.left;
	yl=system_image.baobao_attrib_body.rect.bottom-system_image.baobao_attrib_body.rect.top;
	
	rect.left=SCREEN_WIDTH/2-xl/2+200;
	rect.top=SCREEN_HEIGHT/2-yl/2;
	rect.right=rect.left+xl;
	rect.bottom=rect.top+yl;
	game_control.window_fight_baobao_change=insert_window_base(rect ,&window_draw_fight_baobao_change,&window_exec_fight_baobao_change,WINDOW_TYPE_NORMAL);

}



/*******************************************************************************************************************}
{	Fight talk window																								}
********************************************************************************************************************/
void exec_fight_talk_window(void)
{
	SLONG xl,yl;
	RECT rect; 

	
	if(game_control.window_fight_talk<0)	
	{
		xl=200;
		yl=16+18*5-2;
		rect.left=310;
		rect.top=340;
		rect.right=rect.left+xl;
		rect.bottom=rect.top+yl;
		game_control.window_fight_talk=insert_window_base(rect ,&window_draw_fight_talk,&window_exec_fight_talk,WINDOW_TYPE_NORMAL);
	}
	else
	{
		delete_window_base(game_control.window_fight_talk);
		game_control.window_fight_talk=-1;
	}
	

	

}

/*******************************************************************************************************************}
{																													}
{				Window Routinue																						}
{																													}
********************************************************************************************************************/
/*******************************************************************************************************************}
{	channel																											}
********************************************************************************************************************/
void window_draw_select_channel(void)
{
	SLONG i;
	UCHR command_text[80];
	SLONG mda_command;
	SLONG mouse_x,mouse_y;
	SLONG idx;


	display_img256_buffer(system_image.channel_body.rect.left,
		system_image.channel_body.rect.top,
		system_image.channel_body.image,screen_buffer);
		

	if(system_config.channel_world_flag==1)		// ����Ƶ������
	{
		mda_group_exec(system_image.main_menu_handle,(UCHR *)"MainCOmmand0",
			60,433,screen_buffer,MDA_COMMAND_FIRST,0,0,0);
	}
	if(system_config.channel_sellbuy_flag==1)	// ����Ƶ������
	{
		mda_group_exec(system_image.main_menu_handle,(UCHR *)"MainCOmmand0",
			60,433+22,screen_buffer,MDA_COMMAND_FIRST,0,0,0);
	}
	if(system_config.channel_group_flag==1)		// ����Ƶ������
	{
		mda_group_exec(system_image.main_menu_handle,(UCHR *)"MainCOmmand0",
			60,433+22*2,screen_buffer,MDA_COMMAND_FIRST,0,0,0);
	}
	if(system_config.channel_person_flag==1)	// ˽��Ƶ������
	{
		mda_group_exec(system_image.main_menu_handle,(UCHR *)"MainCOmmand0",
			60,433+22*3,screen_buffer,MDA_COMMAND_FIRST,0,0,0);
	}
	if(system_config.channel_team_flag==1)		// ����Ƶ������
	{
		mda_group_exec(system_image.main_menu_handle,(UCHR *)"MainCOmmand0",
			60,433+22*4,screen_buffer,MDA_COMMAND_FIRST,0,0,0);
	}
	if(system_config.channel_screen_flag==1) 	// ��ǰƵ������
	{
		mda_group_exec(system_image.main_menu_handle,(UCHR *)"MainCOmmand0",
			60,433+22*5,screen_buffer,MDA_COMMAND_FIRST,0,0,0);
	}
	

	mouse_x=system_control.mouse_x;
	mouse_y=system_control.mouse_y;
	
	for(i=0;i<6;i++)			// draw status 
	{
		if((mouse_x>=system_image.button_chat_channel[i].rect.left)&&
			(mouse_x<system_image.button_chat_channel[i].rect.right)&&
			(mouse_y>=system_image.button_chat_channel[i].rect.top)&&
			(mouse_y<system_image.button_chat_channel[i].rect.bottom))
		{

			if((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag))
			{
				sprintf((char *)command_text,"MainCommand%d",system_image.button_chat_channel[i].command_no+1);
				mda_command=MDA_COMMAND_NEXT_FRAME;
			}
			else				
			{
				idx=get_mda_index(system_image.main_menu_handle);
				if(idx>=0)
				{
					mda_group[idx].now_frame[system_image.button_chat_channel[i].command_no+1]=0;
				}
				mda_command=MDA_COMMAND_FIRST;
				sprintf((char *)command_text,"MainCommand%d",system_image.button_chat_channel[i].command_no);
			}

			mda_group_exec(system_image.main_menu_handle,command_text,
				system_image.button_chat_channel[i].rect.left,
				system_image.button_chat_channel[i].rect.top,
				screen_buffer,mda_command,0,0,0);
		}
		
		
	}


}


void window_exec_select_channel(void)
{
	SLONG mouse_x,mouse_y;	
	SLONG i;
	SLONG no;
	
	if(now_exec_window_handle<0)return;
	
	mouse_x=system_control.mouse_x;
	mouse_y=system_control.mouse_y;
	
	if((mouse_x>=60)&&(mouse_x<60+52)&&
		(mouse_y>433)&&(mouse_y<433+22*6))
	{
		if(system_control.mouse_key==MS_LUp)
		{			
			mda_group_exec(system_image.main_menu_handle,(UCHR *)"MainCOmmand0",
				-9999,-9999,screen_buffer,MDA_COMMAND_LAST,0,0,0);
			no=(mouse_y-433)/22;
			no=5-no;
			switch(no)
			{
			case CHAT_CHANNEL_SCREEN:				// ��ǰƵ��
				if(system_config.channel_screen_flag==0)
					system_config.channel_screen_flag=1;
				else
					system_config.channel_screen_flag=0;
				break;
			case CHAT_CHANNEL_TEAM:					// ����
				if(system_config.channel_team_flag==0)
					system_config.channel_team_flag=1;
				else
					system_config.channel_team_flag=0;
				break;
			case CHAT_CHANNEL_PERSON:				// ˽��
				if(system_config.channel_person_flag==0)
					system_config.channel_person_flag=1;
				else
					system_config.channel_person_flag=0;
				break;
			case CHAT_CHANNEL_GROUP:				// ����
				if(system_config.channel_group_flag==0)
					system_config.channel_group_flag=1;
				else
					system_config.channel_group_flag=0;
				break;
			case CHAT_CHANNEL_SELLBUY:				// ����
				if(system_config.channel_sellbuy_flag==0)
					system_config.channel_sellbuy_flag=1;
				else
					system_config.channel_sellbuy_flag=0;
				break;
			case CHAT_CHANNEL_WORLD:				// ����
				if(system_config.channel_world_flag==0)
					system_config.channel_world_flag=1;
				else
					system_config.channel_world_flag=0;
				break;
			}
		}
	}
	
	
	
	
	for(i=0;i<6;i++)			// draw status 
	{
		if((mouse_x>=system_image.button_chat_channel[i].rect.left)&&
			(mouse_x<system_image.button_chat_channel[i].rect.right)&&
			(mouse_y>=system_image.button_chat_channel[i].rect.top)&&
			(mouse_y<system_image.button_chat_channel[i].rect.bottom))
		{
			
			if(system_control.mouse_key==MS_LUp)
			{
				game_control.chat_channel=i;
				now_delete_window_handle=now_exec_window_handle;
				game_control.window_channel=-1;
				break;
			}
		}
		
		
	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}




/*******************************************************************************************************************}
{																													}
{	game_control.window_system_button_idx ��¼���ѡ���������														}
{																													}
{	0 �� 16 --> switch ����																							}
{	17,18 volume +-																									}
{	19,20,21,22 ��ť																								}
{																													}
********************************************************************************************************************/
/*******************************************************************************************************************}
{	system																											}
********************************************************************************************************************/
void window_draw_system(void)
{
	SLONG i;
	UCHR command_text[80];
	UCHR on_text[80];
	SLONG window_x,window_y;
	SLONG mda_command;
	SLONG idx;
	POSITION switch_pp[16]={
		{  17, 81 }, { 187, 81 },
		{  17, 81+30 }, { 187, 81+30 },
		{  17, 81+90 }, { 187, 81+90 },
		{  17, 81+90+30 }, { 187, 81+90+30 },
		{  17, 81+90+30*2 }, { 187, 81+90+30*2 },
		{  17, 81+90+30*3 }, { 187, 81+90+30*3 },
		{  17, 81+90+30*4 }, { 187, 81+90+30*4 },
		{  17, 81+90+30*5 }, { 187, 81+90+30*5 }
	};

	SLONG volume_xl,real_xl;

	
	volume_xl=system_image.volume.rect.right-system_image.volume.rect.left;
	real_xl=volume_xl*system_config.volume/100;

	window_x=system_image.system_background.rect.left;
	window_y=system_image.system_background.rect.top;
	

	for(i=0;i<16;i++)
	{
		switch_pp[i].x=switch_pp[i].x+window_x;
		switch_pp[i].y=switch_pp[i].y+window_y;
	}

	sprintf((char *)on_text,"MainCommand%d",system_image.button_channel_active.command_no);
	
	
// --- background 
	sprintf((char *)command_text,"MainCommand%d",system_image.system_background.command_no);

	mda_group_exec(system_image.system_menu_handle,command_text,
		system_image.system_background.rect.left,
		system_image.system_background.rect.top,
		screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
	
	
	idx=get_mda_index(system_image.system_menu_handle);
	if(idx<0)return;

// -- volume 
	display_part_img256_buffer(172+window_x,134+window_y,
		real_xl,system_image.volume.image,screen_buffer);

	if(system_config.window_mode==0)
		mda_group_exec(system_image.main_menu_handle,on_text,switch_pp[0].x,switch_pp[0].y,
			screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
	else
		mda_group_exec(system_image.main_menu_handle,on_text,switch_pp[1].x,switch_pp[1].y,
		screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);


	if(system_config.music_flag==1)
		mda_group_exec(system_image.main_menu_handle,on_text,switch_pp[2].x,switch_pp[2].y,
		screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
	
	if(system_config.voice_flag==1)
		mda_group_exec(system_image.main_menu_handle,on_text,switch_pp[3].x,switch_pp[3].y,
		screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
	
	if(system_config.record_flag==1)
		mda_group_exec(system_image.main_menu_handle,on_text,switch_pp[4].x,switch_pp[4].y,
		screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);

	if(system_config.fight_flag==1)
		mda_group_exec(system_image.main_menu_handle,on_text,switch_pp[6].x,switch_pp[6].y,
		screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
	
	if(system_config.mail_flag==1)
		mda_group_exec(system_image.main_menu_handle,on_text,switch_pp[8].x,switch_pp[8].y,
		screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
	
	if(system_config.head_talk==1)
		mda_group_exec(system_image.main_menu_handle,on_text,switch_pp[9].x,switch_pp[9].y,
		screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
	
	if(system_config.item_flag==1)
		mda_group_exec(system_image.main_menu_handle,on_text,switch_pp[10].x,switch_pp[10].y,
		screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
	
	if(system_config.strange_flag==1)
		mda_group_exec(system_image.main_menu_handle,on_text,switch_pp[11].x,switch_pp[11].y,
		screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);

	if(system_config.friend_flag==1)
		mda_group_exec(system_image.main_menu_handle,on_text,switch_pp[12].x,switch_pp[12].y,
		screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);

	if(system_config.quick_menu==1)
		mda_group_exec(system_image.main_menu_handle,on_text,switch_pp[13].x,switch_pp[13].y,
		screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
	
	if(system_config.team_flag==1)
		mda_group_exec(system_image.main_menu_handle,on_text,switch_pp[14].x,switch_pp[14].y,
		screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);

	if(system_config.clothes_flag==1)
		mda_group_exec(system_image.main_menu_handle,on_text,switch_pp[15].x,switch_pp[15].y,
		screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
	
	mda_command=MDA_COMMAND_NEXT_FRAME;

	if(game_control.window_system_button_idx==17)
	{
		if((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag))
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.sub_volume.command_no+1);
		}
		else
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.sub_volume.command_no);
			mda_group[idx].now_frame[system_image.sub_volume.command_no+1]=0;
		}

		mda_group_exec(system_image.system_menu_handle,command_text,
			window_x+142,window_y+141,
			screen_buffer,mda_command,0,0,0);		// ��ť 17
	}



	if(game_control.window_system_button_idx==18)
	{
		if((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag))
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.add_volume.command_no+1);
		}
		else
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.add_volume.command_no);
			mda_group[idx].now_frame[system_image.add_volume.command_no+1]=0;
		}
		
		mda_group_exec(system_image.system_menu_handle,command_text,
			window_x+288,window_y+141,
			screen_buffer,mda_command,0,0,0);		// ��ť 18
	}
	


	if(game_control.window_system_button_idx==19)
	{
		if((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag))
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.system_save.command_no+2);
		}
		else
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.system_save.command_no+1);
			mda_group[idx].now_frame[system_image.system_save.command_no+2]=0;
		}
		
	}
	else
	{
		sprintf((char *)command_text,"MainCommand%d",system_image.system_save.command_no);
	}
	mda_group_exec(system_image.system_menu_handle,command_text,
		system_image.system_save.rect.left,
		system_image.system_save.rect.top,
		screen_buffer,mda_command,0,0,0);		// ��ť 19
	

	if(game_control.window_system_button_idx==20)
	{
		if((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag))
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.system_help.command_no+2);
		}
		else
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.system_help.command_no+1);
			mda_group[idx].now_frame[system_image.system_help.command_no+2]=0;
		}
		
	}
	else
	{
		sprintf((char *)command_text,"MainCommand%d",system_image.system_help.command_no);
	}
		
	mda_group_exec(system_image.system_menu_handle,command_text,
		system_image.system_help.rect.left,
		system_image.system_help.rect.top,
		screen_buffer,mda_command,0,0,0);		// ��Ϸ���� 20


	if(game_control.window_system_button_idx==21)
	{
		if((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag))
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.system_bbs.command_no+2);
		}
		else
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.system_bbs.command_no+1);
			mda_group[idx].now_frame[system_image.system_bbs.command_no+2]=0;
		}
	}
	else
	{
		sprintf((char *)command_text,"MainCommand%d",system_image.system_bbs.command_no);
	}
		
	mda_group_exec(system_image.system_menu_handle,command_text,
		system_image.system_bbs.rect.left,
		system_image.system_bbs.rect.top,
		screen_buffer,mda_command,0,0,0);		// ��Ϸ��̳ 21


	if(game_control.window_system_button_idx==22)
	{
		if((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag))
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.system_quit.command_no+2);
		}
		else
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.system_quit.command_no+1);
			mda_group[idx].now_frame[system_image.system_quit.command_no+2]=0;
		}
	}
	else
	{
		sprintf((char *)command_text,"MainCommand%d",system_image.system_quit.command_no);
	}
		
	mda_group_exec(system_image.system_menu_handle,command_text,
		system_image.system_quit.rect.left,
		system_image.system_quit.rect.top,
		screen_buffer,mda_command,0,0,0);		// ��Ϸ���� 22
	

	if(game_control.window_system_button_idx==23)
	{
		if((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag))
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.system_close.command_no+1);
		}
		else
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.system_close.command_no);
			mda_group[idx].now_frame[system_image.system_close.command_no+1]=0;
		}
		
		mda_group_exec(system_image.system_menu_handle,command_text,
			window_x+333,window_y+34,
			screen_buffer,mda_command,0,0,0);		// ��ť 18
	}
	


}


void window_exec_system(void)
{
	SLONG mouse_x,mouse_y;	
	SLONG i;
	SLONG window_x,window_y;
	POSITION switch_pp[16]={
		{  17, 81 }, { 187, 81 },
		{  17, 81+30 }, { 187, 81+30 },
		{  17, 81+90 }, { 187, 81+90 },
		{  17, 81+90+30 }, { 187, 81+90+30 },
		{  17, 81+90+30*2 }, { 187, 81+90+30*2 },
		{  17, 81+90+30*3 }, { 187, 81+90+30*3 },
		{  17, 81+90+30*4 }, { 187, 81+90+30*4 },
		{  17, 81+90+30*5 }, { 187, 81+90+30*5 }
	};
	

	if(now_exec_window_handle<0)return;

	window_x=system_image.system_background.rect.left;
	window_y=system_image.system_background.rect.top;

	for(i=0;i<16;i++)
	{
		switch_pp[i].x=switch_pp[i].x+window_x;
		switch_pp[i].y=switch_pp[i].y+window_y;
	}
	
	mouse_x=system_control.mouse_x;
	mouse_y=system_control.mouse_y;
	
	game_control.window_system_button_idx=-1;

// --- check is in switch
	for(i=0;i<16;i++)
	{
		if((mouse_x>=switch_pp[i].x)&&(mouse_x<switch_pp[i].x+21)&&
			(mouse_y>=switch_pp[i].y)&&(mouse_y<switch_pp[i].y+21))
		{
			game_control.window_system_button_idx=i;
			goto Exit_loop;
		}
	}

// ---check is volume
	if((mouse_x>=window_x+142)&&(mouse_x<window_x+142+21)&&
		(mouse_y>window_y+141)&&(mouse_y<window_y+141+21))
	{
		game_control.window_system_button_idx=17;
		goto Exit_loop;
		
	}
	
	if((mouse_x>=window_x+288)&&(mouse_x<window_x+288+21)&&
		(mouse_y>window_y+141)&&(mouse_y<window_y+141+21))
	{
		game_control.window_system_button_idx=18;
		goto Exit_loop;
		
	}

// --- check is other switch
	if((mouse_x>=system_image.system_save.rect.left)&&(mouse_x<system_image.system_save.rect.right)&&
		(mouse_y>=system_image.system_save.rect.top)&&(mouse_y<system_image.system_save.rect.bottom))
	{
		game_control.window_system_button_idx=19;
		goto Exit_loop;
	}


	if((mouse_x>=system_image.system_help.rect.left)&&(mouse_x<system_image.system_help.rect.right)&&
		(mouse_y>=system_image.system_help.rect.top)&&(mouse_y<system_image.system_help.rect.bottom))
	{
		game_control.window_system_button_idx=20;
		goto Exit_loop;
	}
	

	if((mouse_x>=system_image.system_bbs.rect.left)&&(mouse_x<system_image.system_bbs.rect.right)&&
		(mouse_y>=system_image.system_bbs.rect.top)&&(mouse_y<system_image.system_bbs.rect.bottom))
	{
		game_control.window_system_button_idx=21;
		goto Exit_loop;
	}

	if((mouse_x>=system_image.system_quit.rect.left)&&(mouse_x<system_image.system_quit.rect.right)&&
		(mouse_y>=system_image.system_quit.rect.top)&&(mouse_y<system_image.system_quit.rect.bottom))
	{
		game_control.window_system_button_idx=22;
		goto Exit_loop;
	}

	if((mouse_x>=window_x+333)&&(mouse_x<window_x+333+28)&&
		(mouse_y>window_y+34)&&(mouse_y<window_y+34+28))
	{
		game_control.window_system_button_idx=23;
		goto Exit_loop;
		
	}
	
	
	
Exit_loop:

	switch(system_control.mouse_key)
	{
	case MS_RUp:
		if(game_control.window_system_button_idx==-1)
		{
			memcpy(&system_config,&backup_system_config,sizeof ( struct SYSTEM_CONFIG_STRUCT));
			exec_system_config();
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_system=-1;
		}
		break;
	case MS_LDn: case MS_LDrag:
		switch(game_control.window_system_button_idx)
		{
		case 17:							// volume --
			system_config.volume-=1;
			if(system_config.volume<0)
				system_config.volume=0;
			
			auto_set_music_control(system_config.volume);
			break;
		case 18:							// volume ++
			system_config.volume+=1;
			if(system_config.volume>=100)
				system_config.volume=100;
			auto_set_music_control(system_config.volume);
			break;
		}
		break;
	case MS_LUp:
		switch(game_control.window_system_button_idx)
		{
		case 0:								// switch to window mode
			system_config.window_mode=0;
			break;
		case 1:								// switch to full screen mode
			system_config.window_mode=1;
			break;
		case 2:								// music ON/OFF
			if(system_config.music_flag==0)
			{
				system_config.music_flag=1;
				play_music_file(game_control.music_filename,1,100);
			}
			else
			{
				stop_music_file();
				system_config.music_flag=0;
			}
			break;
		case 3:								// VOICE ON/OFF
			if(system_config.voice_flag==0)
			{
				system_config.voice_flag=1;
				reactive_map_effect();
			}
			else
			{
				stop_all_voice();
				system_config.voice_flag=0;
			}
			break;
		case 4:								// game record
			if(system_config.record_flag==0)
				system_config.record_flag=1;
			else
				system_config.record_flag=0;
			break;
		case 6:								// fight �д�
			if(system_config.fight_flag==0)
				system_config.fight_flag=1;
			else
				system_config.fight_flag=0;
			break;
		case 8:								// mail
			if(system_config.mail_flag==0)
				system_config.mail_flag=1;
			else
				system_config.mail_flag=0;
			break;
		case 9:								// npc talk
			if(system_config.head_talk==0)
				system_config.head_talk=1;
			else
				system_config.head_talk=0;
			break;
		case 10:							// item
			if(system_config.item_flag==0)
				system_config.item_flag=1;
			else
				system_config.item_flag=0;
			break;
		case 11:							// strange
			if(system_config.strange_flag==0)
				system_config.strange_flag=1;
			else
				system_config.strange_flag=0;
			break;
		case 12:							// friend
			if(system_config.friend_flag==0)
				system_config.friend_flag=1;
			else
				system_config.friend_flag=0;
			break;
		case 13:							// quick menu
			if(system_config.quick_menu==0)
				system_config.quick_menu=1;
			else
				system_config.quick_menu=0;
			break;
		case 14:							// team
			if(system_config.team_flag==0)
				system_config.team_flag=1;
			else
				system_config.team_flag=0;
			break;
		case 15:							// clothes
			if(system_config.clothes_flag==0)
				system_config.clothes_flag=1;
			else
				system_config.clothes_flag=0;
			break;

		case 19:							// save
			if(system_config.window_mode==0)
			{
				if(g_dwScreenMode==FULL_SCREEN_MODE)
					switch_screen_mode();
				
			}
			else
			{
				if(g_dwScreenMode==WINDOWS_SCREEN_MODE )
					switch_screen_mode();
			}
			exec_system_config();
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_system=-1;
			update_system_config();
			break;
		case 20:							// help
			openHttp(GAME_HELP_URL);
			break;
		case 21:							// bbs
			openHttp(GAME_BBS_URL);
			break;
		case 22:							// quit game
			memcpy(&system_config,&backup_system_config,sizeof ( struct SYSTEM_CONFIG_STRUCT));
			exec_system_config();
			game_control.main_loop_break=true;
			game_control.main_loop_ret_val=MAIN_TASK_RETURN_READY_QUIT;
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_system=-1;
			break;

		case 23:							// close window
			memcpy(&system_config,&backup_system_config,sizeof ( struct SYSTEM_CONFIG_STRUCT));
			exec_system_config();
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_system=-1;
			break;
		}
		break;
			
	}

		
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}



/*******************************************************************************************************************}
{	config																											}
********************************************************************************************************************/
void exec_system_config(void)
{


	if(system_config.music_flag==0)
	{
		stop_music_file();
	}
	else
	{
		play_music_file(game_control.music_filename,1,100);
	}

	
	if(system_config.voice_flag==0)
	{
		stop_all_voice();
	}
	else
	{
		reactive_map_effect();
	}
	

	update_client_config();
	update_chat_window();

}


/*******************************************************************************************************************}
{	quit																											}
********************************************************************************************************************/
void window_draw_quit(void)
{
	UCHR command_text[80];
	SLONG window_x,window_y;
	SLONG mda_command;
	SLONG idx;

// --- background 
	window_x=242;
	window_y=186;
	
	sprintf((char *)command_text,"MainCommand%d",system_image.quit_body.command_no);

	mda_group_exec(system_image.quit_menu_handle,command_text,
		window_x,
		window_y,
		screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
	

	mda_command=MDA_COMMAND_NEXT_FRAME;
	idx=get_mda_index(system_image.quit_menu_handle);
	if(idx<0)return;

	if(game_control.window_quit_button_idx==0)			// ���¿�ʼ
	{
		if((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag))
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.quit_restart.command_no+2);
		}
		else
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.quit_restart.command_no+1);
			mda_group[idx].now_frame[system_image.quit_restart.command_no+2]=0;
		}
	}
	else
	{
		sprintf((char *)command_text,"MainCommand%d",system_image.quit_restart.command_no);
	}
	
	mda_group_exec(system_image.quit_menu_handle,command_text,
		window_x+38,window_y+76,screen_buffer,mda_command,0,0,0);		


	if(game_control.window_quit_button_idx==1)			// ������Ϸ
	{
		if((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag))
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.quit_quit.command_no+2);
		}
		else
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.quit_quit.command_no+1);
			mda_group[idx].now_frame[system_image.quit_quit.command_no+2]=0;
		}
	}
	else
	{
		sprintf((char *)command_text,"MainCommand%d",system_image.quit_quit.command_no);
	}
	
	mda_group_exec(system_image.quit_menu_handle,command_text,
		window_x+38,window_y+118,screen_buffer,mda_command,0,0,0);		
	


	if(game_control.window_quit_button_idx==2)			// ������Ϸ
	{
		if((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag))
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.quit_cancel.command_no+2);
		}
		else
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.quit_cancel.command_no+1);
			mda_group[idx].now_frame[system_image.quit_cancel.command_no+2]=0;
		}
	}
	else
	{
		sprintf((char *)command_text,"MainCommand%d",system_image.quit_cancel.command_no);
	}
	
	mda_group_exec(system_image.quit_menu_handle,command_text,
		window_x+38,window_y+178,screen_buffer,mda_command,0,0,0);		
	

	

}

void window_exec_quit(void)
{
	SLONG window_x,window_y;
	SLONG mouse_x,mouse_y;

	if(now_exec_window_handle<0)return;
	
	window_x=242;
	window_y=186;
	mouse_x=system_control.mouse_x-window_x;
	mouse_y=system_control.mouse_y-window_y;
	

	game_control.window_quit_button_idx=-1;	

	if((mouse_x>=38)&&(mouse_x<38+system_image.quit_restart.rect.right-system_image.quit_restart.rect.left)&&
		(mouse_y>=76)&&(mouse_y<76+system_image.quit_restart.rect.bottom-system_image.quit_restart.rect.top))
		game_control.window_quit_button_idx=0;

	if((mouse_x>=38)&&(mouse_x<38+system_image.quit_quit.rect.right-system_image.quit_quit.rect.left)&&
		(mouse_y>=118)&&(mouse_y<118+system_image.quit_quit.rect.bottom-system_image.quit_quit.rect.top))
		game_control.window_quit_button_idx=1;
	
	if((mouse_x>=38)&&(mouse_x<38+system_image.quit_cancel.rect.right-system_image.quit_cancel.rect.left)&&
		(mouse_y>=178)&&(mouse_y<178+system_image.quit_cancel.rect.bottom-system_image.quit_cancel.rect.top))
		game_control.window_quit_button_idx=2;
	

	switch(system_control.mouse_key)
	{
	case MS_RUp:
		if(game_control.window_quit_button_idx==-1)
		{
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_system=-1;
		}
		break;
	case MS_LUp:
		switch(game_control.window_quit_button_idx)
		{
		case 0:							// restart
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_quit=-1;
			game_control.main_loop_break=true;
			game_control.main_loop_ret_val=MAIN_TASK_RETURN_QUIT;
			disconnectServer();
			break;
		case 1:							// quit
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_quit=-1;
			game_control.main_loop_break=true;
			game_control.main_loop_ret_val=MAIN_TASK_RETURN_END;
			disconnectServer();
			break;
		case 2:							// cancel
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_quit=-1;
			break;
		}
		break;
	}



	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}




void close_all_main_map_window(void)
{

	// --- ( 0 - 0 ) �����Ϸ�淨����
	game_control.game_give_id=0;			// ��Ҹ����˵� ID
	game_control.game_attack_id=0;			// ��ҹ����� ID
	game_control.game_deal_id=0;			// ����ҽ��׵� ID
	

	if(game_control.window_channel>=0)			// Ƶ��ѡ��
	{
		delete_window_base(game_control.window_channel);
	}	
	game_control.window_channel=-1;
	
	if(game_control.window_system>=0)			// ϵͳ
	{
		delete_window_base(game_control.window_system);
	}	
	game_control.window_system=-1;
	

	if(game_control.window_quit>=0)				// ����ȷ��
	{
		delete_window_base(game_control.window_quit);
	}	
	game_control.window_quit=-1;
	

	if(game_control.window_chat>=0)				// --- ������
	{
		delete_window_base(game_control.window_chat);
	}	
	game_control.window_chat=-1;
	

	if(game_control.window_phiz>=0)				// --- �������
	{
		delete_window_base(game_control.window_phiz);
	}	
	game_control.window_phiz=-1;
	

	if(game_control.window_character_status>=0)		// --- ����״̬
	{
		delete_window_base(game_control.window_character_status);
	}	
	game_control.window_character_status=-1;
	

	if(game_control.window_baobao_status>=0)		// --- ����״̬
	{
		delete_window_base(game_control.window_baobao_status);
	}	
	game_control.window_baobao_status=-1;
	

	if(game_control.window_history>=0)			// --- ��ʷ����
	{
		delete_window_base(game_control.window_history);
	}	
	game_control.window_history=-1;
	
	

	if(game_control.window_item>=0)				// --- ������Ʒ
	{
		delete_window_base(game_control.window_item);
	}	
	game_control.window_item=-1;
	
	
	if(game_control.window_mission>=0)			// --- ����
	{
		delete_window_base(game_control.window_mission);
	}	
	game_control.window_mission=-1;
	
	
	if(game_control.window_group>=0)			// --- ����
	{
		delete_window_base(game_control.window_group);
	}	
	game_control.window_group=-1;
	

	if(game_control.window_send_message>=0)			// --- ����ѶϢ
	{
		delete_window_base(game_control.window_send_message);
	}	
	game_control.window_send_message=-1;
	

	if(game_control.window_receive_message>=0)		// --- ����ѶϢ
	{
		delete_window_base(game_control.window_receive_message);
	}	
	game_control.window_receive_message=-1;


	if(game_control.window_chat_attrib>=0)		// --- ��������
	{
		delete_window_base(game_control.window_chat_attrib);
	}	
	game_control.window_chat_attrib=-1;
	

	if(game_control.window_chat_group_send>=0)		// --- ����Ⱥ��
	{
		delete_window_base(game_control.window_chat_group_send);
	}	
	game_control.window_chat_group_send=-1;
	

	if(game_control.window_chat_hirstory>=0)		// --- ������ʷ����
	{
		delete_window_base(game_control.window_chat_hirstory);
	}	
	game_control.window_chat_hirstory=-1;

	
	if(game_control.window_team>=0)				// �����б�
	{
		delete_window_base(game_control.window_team);
	}	
	game_control.window_team=-1;


	if(game_control.window_chat_search>=0)				// ������Ѱ
	{
		delete_window_base(game_control.window_chat_search);
	}	
	game_control.window_chat_search=-1;

	
	if(game_control.window_team_request>=0)				// �����б�
	{
		delete_window_base(game_control.window_team_request);
	}	
	game_control.window_team_request=-1;
	
	
	if(game_control.window_small_map>=0)				// С��ͼ
	{
		delete_window_base(game_control.window_small_map);
	}	
	game_control.window_small_map=-1;
	

	if(game_control.window_world_map>=0)				// �����ͼ
	{
		delete_window_base(game_control.window_world_map);
	}	
	game_control.window_world_map=-1;
	

	if(game_control.window_macro>=0)					// �籾
	{
		delete_window_base(game_control.window_macro);
	}	
	game_control.window_macro=-1;
	
	
	if(game_control.window_give>=0)						// ����
	{
		delete_window_base(game_control.window_give);
	}
	game_control.window_give=-1;

	
		
	if(game_control.window_deal>=0)						// ����
	{
		send_cancel_deal();
	}
	game_control.window_deal=-1;
	


	if(game_control.window_skill>=0)						// ���ܲ鿴
	{
		delete_window_base(game_control.window_skill);
	}
	game_control.window_skill=-1;

	
	if(game_control.window_title>=0)						// ͷ�θ���
	{
		delete_window_base(game_control.window_title);
	}
	game_control.window_title=-1;
	


	if(game_control.window_baobao_item>=0)					// ������Ʒ
	{
		delete_window_base(game_control.window_baobao_item);
	}
	game_control.window_baobao_item=-1;
	

	if(game_control.window_character_fastness>=0)			// ���￹��
	{
		delete_window_base(game_control.window_character_fastness);
	}
	game_control.window_character_fastness=-1;
	

	if(game_control.window_baobao_fastness>=0)			// ��������
	{
		delete_window_base(game_control.window_baobao_fastness);
	}
	game_control.window_baobao_fastness=-1;
	


	if(game_control.window_fight_character_skill>=0)	// ս�����＼��
	{
		delete_window_base(game_control.window_fight_character_skill);
	}
	game_control.window_fight_character_skill=-1;
	

	if(game_control.window_fight_baobao_skill>=0)	// ս����������
	{
		delete_window_base(game_control.window_fight_baobao_skill);
	}
	game_control.window_fight_baobao_skill=-1;
	

	if(game_control.window_fight_character_item>=0)	// ս��������Ʒ����
	{
		delete_window_base(game_control.window_fight_character_item);
	}
	game_control.window_fight_character_item=-1;
	

	if(game_control.window_fight_baobao_item>=0)	// ս��������Ʒ����
	{
		delete_window_base(game_control.window_fight_baobao_item);
	}
	game_control.window_fight_baobao_item=-1;
	

	if(game_control.window_fight_baobao_change>=0)	// ս������������Ʒ����
	{
		delete_window_base(game_control.window_fight_baobao_change);
	}
	game_control.window_fight_baobao_change=-1;
	

	if(game_control.window_fight_talk>=0)	// ս������
	{
		delete_window_base(game_control.window_fight_talk);
	}
	game_control.window_fight_talk=-1;
	


	close_character_fight_all_window();
	close_baobao_fight_all_window();
		
// --- final
	main_loop_active_task=NULL;
	ask_yes_exec_task=NULL;
	ask_no_exec_task=NULL;
	
}


SLONG check_system_npc_in_range(SLONG check_offset)
{
	SLONG point_idx;
	SLONG target_x,target_y;
	SLONG src_x,src_y;
	double x_offset,y_offset,offset;
	
	if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)return(TTN_NOT_OK);
	point_idx=system_control.point_npc_idx;
	if(point_idx<0)return(TTN_NOT_OK);				
	if(map_npc_group[point_idx].npc_type!=NPC_CPU_TYPE)
		return(TTN_NOT_OK);

	if(map_npc_group[system_control.control_npc_idx].npc_info.status&NPC_STATUS_FOLLOW)
		return(TTN_NOT_OK);

	target_x=map_npc_group[point_idx].npc_info.x;
	target_y=map_npc_group[point_idx].npc_info.y;
	src_x=map_npc_group[system_control.control_npc_idx].npc_info.x;
	src_y=map_npc_group[system_control.control_npc_idx].npc_info.y;
	
	x_offset=abs(src_x-target_x);
	y_offset=abs(src_y-target_y);
	offset=sqrt(x_offset*x_offset+y_offset*y_offset) ;
	
	if(offset>=NPC_TALK_RANGE)
		return(TTN_NOT_OK);

	return(TTN_OK);
	
}


void exec_map_npc_active(void)
{
	ULONG point_id;
	SLONG point_idx;
	SLONG target_x,target_y;
	SLONG src_x,src_y;
	double x_offset,y_offset,offset;
	

	point_idx=system_control.point_npc_idx;
	if(point_idx<0)return;				// nothing

	point_id=map_npc_group[point_idx].npc_info.id;
	target_x=map_npc_group[point_idx].npc_info.x;
	target_y=map_npc_group[point_idx].npc_info.y;
	src_x=map_npc_group[system_control.control_npc_idx].npc_info.x;
	src_y=map_npc_group[system_control.control_npc_idx].npc_info.y;
	
	x_offset=abs(src_x-target_x);
	y_offset=abs(src_y-target_y);
	offset=sqrt(x_offset*x_offset+y_offset*y_offset) ;

	switch(system_control.mouse_key)
	{
	case MS_RUp:				// ˽��
		if(point_id==system_control.control_user_id)break;
		if(map_npc_group[point_idx].npc_type!=NPC_CHARACTER_TYPE)break;
		system_control.person_npc_id=point_id;
		sprintf((char *)print_rec,MSG_SET_PERSON_ID,(char *)map_npc_group[point_idx].npc_info.name,point_id);
		push_chat_data(CHAT_CHANNEL_MESSGAE,0,(UCHR *)"",print_rec);
		strcpy((char *)system_control.person_name,(char *)map_npc_group[point_idx].npc_info.name);
		break;
	case MS_LUp:				// Exec Macro ���� �鿴NPC ����
		switch(map_npc_group[point_idx].npc_type)
		{
		case NPC_CPU_TYPE:
			if(offset>=NPC_TALK_RANGE)break;
			if(map_npc_group[system_control.control_npc_idx].npc_info.status&NPC_STATUS_FOLLOW)
				break;	
			touch_system_npc(system_control.control_user_id,point_id);
			break;
		case NPC_CHARACTER_TYPE:
			display_map_npc_data(point_idx);
			break;
		case NPC_BAOBAO_TYPE:
			break;
		}

		break;
	}


	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	

}



void exec_fight_npc_active(void)
{
	ULONG point_id;
	SLONG point_idx;
	
	
	point_idx=system_control.point_npc_idx;
	if(point_idx<0)return;				// nothing
	
	point_id=fight_npc_group[point_idx].base.id;
	
	switch(system_control.mouse_key)
	{
	case MS_RUp:				// ������������
	case MS_RDn:
		if(fight_control_data.baobao_idx<0)break;
		if(!fight_npc_group[fight_control_data.baobao_idx].base.active)break;
		if(baobao_fight_command_data.command<0)		// û������,Ĭ��������
		{
			if(point_id==fight_npc_group[fight_control_data.baobao_idx].base.id)break;
			set_fight_main_command(FIGHT_COMMAND_BAOBAO,FIGHT_COMMAND_ATTACK);
		}
		baobao_fight_command_data.target_id=point_id;
		break;
	case MS_LUp:				// ��ҹ�������
	case MS_LDn:
		if(fight_control_data.character_idx<0)break;
		if(character_fight_command_data.command<0)		// û������,Ĭ��������
		{
			if(point_id==fight_npc_group[fight_control_data.character_idx].base.id)break;
			set_fight_main_command(FIGHT_COMMAND_CHARACTER,FIGHT_COMMAND_ATTACK);
		}
		character_fight_command_data.target_id=point_id;
		
		break;
	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}






void display_map_npc_data(SLONG idx)
{
	if(idx<0)return;

	sprintf((char *)print_rec,MSG_PLAYER_INFO_ID,map_npc_group[idx].npc_info.id);
	push_chat_data(CHAT_CHANNEL_MESSGAE,map_npc_group[idx].npc_info.id,(UCHR *)map_npc_group[idx].npc_info.name,print_rec);
	
	if(map_npc_group[idx].npc_info.base_type%2==0)
		sprintf((char *)print_rec,MSG_PLAYER_INFO_SEX,CHARACTER_SEX_MAN_TEXT);
	else
		sprintf((char *)print_rec,MSG_PLAYER_INFO_SEX,CHARACTER_SEX_WOMAN_TEXT);
	push_chat_data(CHAT_CHANNEL_NONE,map_npc_group[idx].npc_info.id,(UCHR *)map_npc_group[idx].npc_info.name,print_rec);


	strcpy((char *)print_rec,"");
	switch(map_npc_group[idx].npc_info.base_type/2)
	{
	case 0:
		sprintf((char *)print_rec,MSG_PLAYER_INFO_PHYLE,CHARACTER_PHYLE_HUMAN_TEXT);
		break;
	case 1:
		sprintf((char *)print_rec,MSG_PLAYER_INFO_PHYLE,CHARACTER_PHYLE_GENUS_TEXT);
		break;
	case 2:
		sprintf((char *)print_rec,MSG_PLAYER_INFO_PHYLE,CHARACTER_PHYLE_MAGIC_TEXT);
		break;
	case 3:
		sprintf((char *)print_rec,MSG_PLAYER_INFO_PHYLE,CHARACTER_PHYLE_ROBET_TEXT);
		break;
	}
	push_chat_data(CHAT_CHANNEL_NONE,map_npc_group[idx].npc_info.id,(UCHR *)map_npc_group[idx].npc_info.name,print_rec);
	
	
	sprintf((char *)print_rec,MSG_PLAYER_INFO_TITLE,map_npc_group[idx].npc_info.title);
	push_chat_data(CHAT_CHANNEL_NONE,map_npc_group[idx].npc_info.id,(UCHR *)map_npc_group[idx].npc_info.name,print_rec);
	
	sprintf((char *)print_rec,MSG_PLAYER_INFO_NAME,map_npc_group[idx].npc_info.name);
	push_chat_data(CHAT_CHANNEL_NONE,map_npc_group[idx].npc_info.id,(UCHR *)map_npc_group[idx].npc_info.name,print_rec);
	

}


// �Ժ�� ��ֵ����
void update_weather_data(MAP_WEATHER_BASE data )
{
	
	set_weather_type(data.weather_type);
}



/*******************************************************************************************************************}
{	phiz																											}
********************************************************************************************************************/
void window_draw_phiz(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	SLONG i;
	SLONG font_xl,font_yl;
	RECT test_rect;
	SHINT back_color=rgb2hi(75,0,61);
	

// --- background 
	window_x=PHIZ_WINDOW_X;
	window_y=PHIZ_WINDOW_Y;
	window_xl=PHIZ_FONT_X*9;
	window_yl=PHIZ_FONT_Y*9;

	
//
//	draw_message_font(window_x,window_y,window_xl,window_yl,screen_buffer);
	draw_message_box(window_x,window_y,window_xl,window_yl,screen_buffer);
	put_bar(window_x+2,window_y+2,window_xl-4,window_yl-4,back_color,screen_buffer);
	

	for(i=0;i<81;i++)
	{
		test_rect.left=0;
		test_rect.top=0;
		test_rect.right=SCREEN_WIDTH;
		test_rect.bottom=SCREEN_HEIGHT;
		text_out_data.g_pChat->insertString((char *)phiz_data[i].text,test_rect,false);
		font_xl=PHIZ_FONT_X/2-(test_rect.right-test_rect.left)/2;
		font_yl=PHIZ_FONT_Y/2-(test_rect.bottom-test_rect.top)/2;

		if((system_control.mouse_x>=window_x+phiz_data[i].pp.x)&&
			(system_control.mouse_y>=window_y+phiz_data[i].pp.y)&&
			(system_control.mouse_x<window_x+phiz_data[i].pp.x+PHIZ_FONT_X)&&
			(system_control.mouse_y<window_y+phiz_data[i].pp.y+PHIZ_FONT_Y))
		{
			alpha_put_bar(window_x+phiz_data[i].pp.x+1,window_y+phiz_data[i].pp.y+1,PHIZ_FONT_X-3,PHIZ_FONT_Y-3,color_control.yellow,screen_buffer,120);
			put_scroll_box(window_x+phiz_data[i].pp.x+1,window_y+phiz_data[i].pp.y+1,window_x+phiz_data[i].pp.x+1+PHIZ_FONT_X-3,window_y+phiz_data[i].pp.y+1+PHIZ_FONT_Y-3,
				color_control.green,color_control.blue,screen_buffer);
			
		}

		
		text_out_data.g_pChat->updateString(window_x+phiz_data[i].pp.x+font_xl,window_y+phiz_data[i].pp.y+font_yl,window_xl,window_yl,
			(char *)phiz_data[i].text);
		

	}

	text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,screen_buffer,false);
	
	
}

void window_exec_phiz(void)
{
	SLONG window_x,window_y;
	SLONG mouse_x,mouse_y;
	SLONG select_index;
	SLONG i;
	CUGEditBox *pEdit;

	if(now_exec_window_handle<0)return;
	
	window_x=PHIZ_WINDOW_X;
	window_y=PHIZ_WINDOW_Y;
	mouse_x=system_control.mouse_x-window_x;
	mouse_y=system_control.mouse_y-window_y;

	select_index=-1;
	for(i=0;i<81;i++)
	{
		
		if((system_control.mouse_x>=window_x+phiz_data[i].pp.x)&&
			(system_control.mouse_y>=window_y+phiz_data[i].pp.y)&&
			(system_control.mouse_x<window_x+phiz_data[i].pp.x+PHIZ_FONT_X)&&
			(system_control.mouse_y<window_y+phiz_data[i].pp.y+PHIZ_FONT_Y))
		{
			select_index=i;
			break;
		}
		
		
	}
	
		

	switch(system_control.mouse_key)
	{
	case MS_RUp:
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_phiz=-1;
		break;
	case MS_LUp:
		if(select_index<0)break;
		pEdit = getActiveEdit();

		if (pEdit)
		{
			pEdit->setText((char *)phiz_data[select_index].text);
		}
		break;
	}


	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}



/*******************************************************************************************************************}
{	chat																											}
********************************************************************************************************************/
void window_draw_chat(void)
{
	SLONG window_x,window_y;
	UCHR command_text[80];
	SLONG friend_off;
	SLONG temp_off;
	SLONG mask_off;
	SLONG function;
	SLONG mouse_x,mouse_y;
	SLONG offset_x,offset_y;
	
	window_x=system_image.chat_body.rect.left;
	window_y=system_image.chat_body.rect.top;
	
	mouse_x=system_control.mouse_x;
	mouse_y=system_control.mouse_y;
	offset_x=mouse_x-system_image.chat_body.rect.left;
	offset_y=mouse_y-system_image.chat_body.rect.top;
	
// --- ( 0 - 0 )  Draw body
	display_img256_buffer(window_x,window_y,system_image.chat_body.image,screen_buffer);	
	
	// --- ( 0 - 1 )  Draw button 
	friend_off=0;
	temp_off=0;
	mask_off=0;
	
	
	offset_x=mouse_x-system_image.chat_body.rect.left;
	offset_y=mouse_y-system_image.chat_body.rect.top;
	
	if((mouse_x>=system_image.button_chat_friend.rect.left)&&(mouse_x<system_image.button_chat_friend.rect.right)&&
		(mouse_y>=system_image.button_chat_friend.rect.top)&&(mouse_y<system_image.button_chat_friend.rect.bottom))
		friend_off=1;
	
	if((mouse_x>=system_image.button_chat_temp.rect.left)&&(mouse_x<system_image.button_chat_temp.rect.right)&&
		(mouse_y>=system_image.button_chat_temp.rect.top)&&(mouse_y<system_image.button_chat_temp.rect.bottom))
		temp_off=1;
	
	if((mouse_x>=system_image.button_chat_mask.rect.left)&&(mouse_x<system_image.button_chat_mask.rect.right)&&
		(mouse_y>=system_image.button_chat_mask.rect.top)&&(mouse_y<system_image.button_chat_mask.rect.bottom))
		mask_off=1;
	
	function=0;
	switch(game_control.window_chat_type)
	{
	case 0:						// ����
		friend_off=2;
		function=0;
		break;
	case 1:						// ��ʱ
		temp_off=2;
		function=1;
		break;
	case 2:						// ����
		function=2;
		mask_off=2;
		break;
	default:					// UNknow
		break;
	}
	
	
	
	sprintf((char *)command_text,"MainCommand%d",system_image.button_chat_friend.command_no+friend_off);
	mda_group_exec(system_image.button_handle,command_text,
		system_image.button_chat_friend.rect.left,
		system_image.button_chat_friend.rect.top,
		screen_buffer,
		MDA_COMMAND_NEXT_FRAME,0,0,0);
	sprintf((char *)command_text,"MainCommand%d",system_image.button_chat_temp.command_no+temp_off);
	mda_group_exec(system_image.button_handle,command_text,
		system_image.button_chat_temp.rect.left,
		system_image.button_chat_temp.rect.top,
		screen_buffer,
		MDA_COMMAND_NEXT_FRAME,0,0,0);
	sprintf((char *)command_text,"MainCommand%d",system_image.button_chat_mask.command_no+mask_off);
	mda_group_exec(system_image.button_handle,command_text,
		system_image.button_chat_mask.rect.left,
		system_image.button_chat_mask.rect.top,
		screen_buffer,
		MDA_COMMAND_NEXT_FRAME,0,0,0);
	
	
	if((offset_x>=136)&&(offset_x<136+28)&&				// close button
		(offset_y>=10)&&(offset_y<10+28))
	{
		if((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag))
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.button_window_close.command_no+1);
			mda_group_exec(system_image.button_handle,command_text,
				136+system_image.chat_body.rect.left,
				10+system_image.chat_body.rect.top,
				screen_buffer,
				MDA_COMMAND_NEXT_FRAME,0,0,0);
		}
		else
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.button_window_close.command_no);
			mda_group_exec(system_image.button_handle,command_text,
				136+system_image.chat_body.rect.left,
				10+system_image.chat_body.rect.top,
				screen_buffer,
				MDA_COMMAND_NEXT_FRAME,0,0,0);
			
		}
		
	} else 	if((offset_x>=102)&&(offset_x<102+21)&&				// up button
		(offset_y>=383)&&(offset_y<383+21))
	{
		if((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag))
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.button_up.command_no+1);
			mda_group_exec(system_image.button_handle,command_text,
				102+system_image.chat_body.rect.left,
				383+system_image.chat_body.rect.top,
				screen_buffer,
				MDA_COMMAND_NEXT_FRAME,0,0,0);
		}
		else
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.button_up.command_no);
			mda_group_exec(system_image.button_handle,command_text,
				102+system_image.chat_body.rect.left,
				383+system_image.chat_body.rect.top,
				screen_buffer,
				MDA_COMMAND_NEXT_FRAME,0,0,0);
		}
		
		
	} else 	if((offset_x>=128)&&(offset_x<128+21)&&				// down button
		(offset_y>=383)&&(offset_y<383+21))
	{
		if((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag))
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.button_down.command_no+1);
			mda_group_exec(system_image.button_handle,command_text,
				128+system_image.chat_body.rect.left,
				383+system_image.chat_body.rect.top,
				screen_buffer,
				MDA_COMMAND_NEXT_FRAME,0,0,0);
		}
		else
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.button_down.command_no);
			mda_group_exec(system_image.button_handle,command_text,
				128+system_image.chat_body.rect.left,
				383+system_image.chat_body.rect.top,
				screen_buffer,
				MDA_COMMAND_NEXT_FRAME,0,0,0);
		}
		
		
	} else 	if((offset_x>=76)&&(offset_x<76+21)&&				// add friend button
		(offset_y>=383)&&(offset_y<383+21))
	{
		if((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag))
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.button_chat_add_friend.command_no+1);
			mda_group_exec(system_image.button_handle,command_text,
				76+system_image.chat_body.rect.left,
				383+system_image.chat_body.rect.top,
				screen_buffer,
				MDA_COMMAND_NEXT_FRAME,0,0,0);
		}
		else
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.button_chat_add_friend.command_no);
			mda_group_exec(system_image.button_handle,command_text,
				76+system_image.chat_body.rect.left,
				383+system_image.chat_body.rect.top,
				screen_buffer,
				MDA_COMMAND_NEXT_FRAME,0,0,0);
		}
		
	} else 	if((offset_x>=50)&&(offset_x<50+21)&&				// search button
		(offset_y>=383)&&(offset_y<383+21))
	{
		if((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag))
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.button_chat_search.command_no+1);
			mda_group_exec(system_image.button_handle,command_text,
				50+system_image.chat_body.rect.left,
				383+system_image.chat_body.rect.top,
				screen_buffer,
				MDA_COMMAND_NEXT_FRAME,0,0,0);
		}
		else
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.button_chat_search.command_no);
			mda_group_exec(system_image.button_handle,command_text,
				50+system_image.chat_body.rect.left,
				383+system_image.chat_body.rect.top,
				screen_buffer,
				MDA_COMMAND_NEXT_FRAME,0,0,0);
		}
		
	} else 	if((offset_x>=24)&&(offset_x<24+21)&&				// setup button
		(offset_y>=383)&&(offset_y<383+21))
	{
		if((system_control.mouse_key==MS_LDn)||(system_control.mouse_key==MS_LDrag))
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.button_chat_auto_answer.command_no+1);
			mda_group_exec(system_image.button_handle,command_text,
				24+system_image.chat_body.rect.left,
				383+system_image.chat_body.rect.top,
				screen_buffer,
				MDA_COMMAND_NEXT_FRAME,0,0,0);
		}
		else
		{
			sprintf((char *)command_text,"MainCommand%d",system_image.button_chat_auto_answer.command_no);
			mda_group_exec(system_image.button_handle,command_text,
				24+system_image.chat_body.rect.left,
				383+system_image.chat_body.rect.top,
				screen_buffer,
				MDA_COMMAND_NEXT_FRAME,0,0,0);
		}
		
	}
	
	
	
	
	// --- display data
	show_friend_list(system_image.chat_body.rect.left+17,
		system_image.chat_body.rect.top+77,
		function);
	
}


void window_exec_chat(void)
{
	SLONG function;
	SLONG mouse_x,mouse_y;
	SLONG offset_x,offset_y;
	SLONG channel;
	SLONG x,y;
	SLONG i;
	SLONG idx;
	SLONG task;
	SLONG select_index;
	
	
	if(now_exec_window_handle<0)return;
	
	x=system_image.chat_body.rect.left+17;
	y=system_image.chat_body.rect.top+77;
	
	function=-1;
	mouse_x=system_control.mouse_x;
	mouse_y=system_control.mouse_y;
	offset_x=mouse_x-system_image.chat_body.rect.left;
	offset_y=mouse_y-system_image.chat_body.rect.top;
	
	task=-1;
	
	
	if((mouse_x>=system_image.button_chat_friend.rect.left)&&(mouse_x<system_image.button_chat_friend.rect.right)&&
		(mouse_y>=system_image.button_chat_friend.rect.top)&&(mouse_y<system_image.button_chat_friend.rect.bottom))
	{
		task=0;
	}
	
	if((mouse_x>=system_image.button_chat_temp.rect.left)&&(mouse_x<system_image.button_chat_temp.rect.right)&&
		(mouse_y>=system_image.button_chat_temp.rect.top)&&(mouse_y<system_image.button_chat_temp.rect.bottom))
	{
		task=1;
	}
	
	if((mouse_x>=system_image.button_chat_mask.rect.left)&&(mouse_x<system_image.button_chat_mask.rect.right)&&
		(mouse_y>=system_image.button_chat_mask.rect.top)&&(mouse_y<system_image.button_chat_mask.rect.bottom))
	{
		task=2;
	}
	
	
	
	if((offset_x>=135)&&(offset_x<135+28)&&				// close button
		(offset_y>=10)&&(offset_y<10+28))
	{
		task=9999;
		
	} else	if((offset_x>=102)&&(offset_x<102+21)&&				// up button
		(offset_y>=383)&&(offset_y<383+21))
	{
		task=10;
		
	} else 	if((offset_x>=128)&&(offset_x<128+21)&&				// down button
		(offset_y>=383)&&(offset_y<383+21))
	{
		task=11;
		
	} else 	if((offset_x>=76)&&(offset_x<76+21)&&				// add friend button
		(offset_y>=383)&&(offset_y<383+21))
	{
		task=12;
		
	} else 	if((offset_x>=50)&&(offset_x<50+21)&&				// search button
		(offset_y>=383)&&(offset_y<383+21))
	{
		task=13;
		
	} else 	if((offset_x>=24)&&(offset_x<24+21)&&				// setup button
		(offset_y>=383)&&(offset_y<383+21))
	{
		task=14;
	
	}
	
	
	
	select_index=-1;
	for(i=0;i<14;i++)
	{
		
		if((system_control.mouse_x>=x)&&(system_control.mouse_x<x+139)&&
			(system_control.mouse_y>=y+i*21)&&(system_control.mouse_y<y+i*21+18))
		{
			switch(game_control.window_chat_type)
			{
			case FRIEND_NORMAL:					// һ�����
				idx=game_control.window_chat_friend_start_idx+i;
				if(display_friend_list[idx].level<0)continue;
				break;
			case FRIEND_TEMP:					// ��ʱ����
				idx=game_control.window_chat_temp_start_idx+i;
				if(display_temp_list[idx].level<0)continue;
				break;
			case FRIEND_MASK:					// ������
				idx=game_control.window_chat_mask_start_idx+i;
				if(display_mask_list[idx].level<0)continue;
				break;			
			}
			
			task=100;
			select_index=idx;
			i=14;
			break;
			
		}
		
	}
	
	
	
	
	
	
	
	switch(system_control.mouse_key)
	{
	case MS_RUp:
		if(task<0)
		{
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_chat=-1;
		}
		else		// �����쿴���� window 
		{
			if(task!=100)break;
			if(select_index<0)break;
			switch(game_control.window_chat_type)
			{
			case FRIEND_NORMAL:
				memcpy(&send_message_data,&display_friend_list[select_index],sizeof(struct CHAT_FRIEND_LIST_STRUCT));
				game_control.window_chat_select_idx=select_index-game_control.window_chat_friend_start_idx;
				break;
			case FRIEND_TEMP:
				memcpy(&send_message_data,&display_temp_list[select_index],sizeof(struct CHAT_FRIEND_LIST_STRUCT));
				game_control.window_chat_select_idx=select_index-game_control.window_chat_temp_start_idx;
				break;
			case FRIEND_MASK:
				memcpy(&send_message_data,&display_mask_list[select_index],sizeof(struct CHAT_FRIEND_LIST_STRUCT));
				game_control.window_chat_select_idx=select_index-game_control.window_chat_temp_start_idx;
				break;
			}
			exec_chat_attrib();

		}
		
		break;
	case MS_LUp:
		switch(task)
		{
		case 0:						// change 
			function=FRIEND_NORMAL;
			break;
		case 1:
			function=FRIEND_TEMP;
			break;
		case 2:
			function=FRIEND_MASK;
			break;				
		case 9999:					// close window 
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_chat=-1;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			break;
		case 10:					// up
			auto_play_voice(0,(UCHR *)"EFC\\button02.wav",NULL,0,0,0,0,&channel);
			scroll_friend_list(Up);
			break;
		case 11:					// down
			auto_play_voice(0,(UCHR *)"EFC\\button02.wav",NULL,0,0,0,0,&channel);
			scroll_friend_list(Dn);
			break;
		case 12:					// add friend
			auto_play_voice(0,(UCHR *)"EFC\\button02.wav",NULL,0,0,0,0,&channel);
			game_control.main_loop_select_npc_task=MAP_MAIN_MENU_TASK_ADD_FRIEND;
			set_mouse_cursor(MOUSE_IMG_FRIEND);
			break;
		case 13:					// search 
			auto_play_voice(0,(UCHR *)"EFC\\button02.wav",NULL,0,0,0,0,&channel);
			exec_friend_search();
			break;
		case 14:					// setup Ŀǰ�Ȳ���
			auto_play_voice(0,(UCHR *)"EFC\\button02.wav",NULL,0,0,0,0,&channel);
			break;
		case 100:
			if(select_index<0)break;
			
			switch(game_control.window_chat_type)
			{
			case FRIEND_NORMAL:
				memcpy(&send_message_data,&display_friend_list[select_index],sizeof(struct CHAT_FRIEND_LIST_STRUCT));
				game_control.window_chat_select_idx=select_index-game_control.window_chat_friend_start_idx;
				break;
			case FRIEND_TEMP:
				memcpy(&send_message_data,&display_temp_list[select_index],sizeof(struct CHAT_FRIEND_LIST_STRUCT));
				game_control.window_chat_select_idx=select_index-game_control.window_chat_temp_start_idx;
				break;
			case FRIEND_MASK:
				memcpy(&send_message_data,&display_mask_list[select_index],sizeof(struct CHAT_FRIEND_LIST_STRUCT));
				game_control.window_chat_select_idx=select_index-game_control.window_chat_temp_start_idx;
				break;
			}
			exec_send_message();
			break;
		}


		if(function>=0)
		{
			if(game_control.window_chat_type!=function)
				game_control.window_chat_select_idx=-1;
			game_control.window_chat_type=function;
			
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			
		}
		
		break;
	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}


/*******************************************************************************************************************}
{	character status																								}
********************************************************************************************************************/
void display_map_character_status(SLONG status)
{
	SLONG mouse_x,mouse_y;
	SLONG x,y,xl,yl;
	SLONG idx;
	bool show;
	
	static SLONG old_mouse_x=0,old_mouse_y=0;
	
	
	if(status==BUTTON_PUSH)
	{
		if(check_have_touch_window(system_image.status_body.rect)>=0)
			return;
	}
	
	mouse_x=system_control.mouse_x;
	mouse_y=system_control.mouse_y;
	if((old_mouse_x!=mouse_x)||(old_mouse_y!=mouse_y))
	{
		old_mouse_x=mouse_x;
		old_mouse_y=mouse_y;
	}
	
	
	
	if(game_control.window_task<0)
	{
		if((mouse_x>=CHARACTER_HP_WINDOW_X)&&(mouse_x<CHARACTER_HP_WINDOW_X+CHARACTER_HP_WINDOW_XL)&&
			(mouse_y>=CHARACTER_HP_WINDOW_Y)&&(mouse_y<CHARACTER_HP_WINDOW_Y+CHARACTER_HP_WINDOW_YL))
		{
			game_control.window_task=MAP_MAIN_MENU_TASK_CHARACTER_HP;
		} 
		else if((mouse_x>=CHARACTER_MP_WINDOW_X)&&(mouse_x<CHARACTER_MP_WINDOW_X+CHARACTER_MP_WINDOW_XL)&&
			(mouse_y>=CHARACTER_MP_WINDOW_Y)&&(mouse_y<CHARACTER_MP_WINDOW_Y+CHARACTER_MP_WINDOW_YL))
		{
			game_control.window_task=MAP_MAIN_MENU_TASK_CHARACTER_MP;
		}
		else if((mouse_x>=BAOBAO_HP_WINDOW_X)&&(mouse_x<BAOBAO_HP_WINDOW_X+BAOBAO_HP_WINDOW_XL)&&
			(mouse_y>=BAOBAO_HP_WINDOW_Y)&&(mouse_y<BAOBAO_HP_WINDOW_Y+BAOBAO_HP_WINDOW_YL))
		{
			game_control.window_task=MAP_MAIN_MENU_TASK_BAOBAO_HP;
		}
		else if((mouse_x>=BAOBAO_MP_WINDOW_X)&&(mouse_x<BAOBAO_MP_WINDOW_X+BAOBAO_MP_WINDOW_XL)&&
			(mouse_y>=BAOBAO_MP_WINDOW_Y)&&(mouse_y<BAOBAO_MP_WINDOW_Y+BAOBAO_MP_WINDOW_YL))
		{
			game_control.window_task=MAP_MAIN_MENU_TASK_BAOBAO_MP;
		}
		else if((mouse_x>=BAOBAO_EXP_WINDOW_X)&&(mouse_x<BAOBAO_EXP_WINDOW_X+BAOBAO_EXP_WINDOW_XL)&&
			(mouse_y>=BAOBAO_EXP_WINDOW_Y)&&(mouse_y<BAOBAO_EXP_WINDOW_Y+BAOBAO_EXP_WINDOW_YL))
		{
			game_control.window_task=MAP_MAIN_MENI_TASK_BAOBAO_EXP;
		}
		else if((mouse_x>=CHARACTER_WINDOW_X)&&(mouse_x<CHARACTER_WINDOW_X+CHARACTER_WINDOW_XL)&&
			(mouse_y>=CHARACTER_WINDOW_Y)&&(mouse_y<CHARACTER_WINDOW_Y+CHARACTER_WINDOW_YL))
		{
			game_control.window_task=MAP_MAIN_MENU_TASK_CHARACTER_STATUS;
		}
		else if((mouse_x>=BAOBAO_WINDOW_X)&&(mouse_x<BAOBAO_WINDOW_X+BAOBAO_WINDOW_XL)&&
			(mouse_y>=BAOBAO_WINDOW_Y)&&(mouse_y<BAOBAO_WINDOW_Y+BAOBAO_WINDOW_YL))
		{
			game_control.window_task=MAP_MAIN_MENU_TASK_BAOBAO_STATUS;
		}
		
		
	}
	
	
	if(game_control.window_task<0)
		return;
	
	
	if(status==BUTTON_SELECT)
	{
		
		show=false;
		switch(game_control.window_task)
		{
		case MAP_MAIN_MENU_TASK_CHARACTER_HP:
			if(!base_character_data.active)break;
			sprintf((char *)print_rec,"%s%d/%d",MSG_HP_TEXT,base_character_data.data.final.now_hp,base_character_data.data.final.max_hp);
			show=true;
			break;
		case MAP_MAIN_MENU_TASK_CHARACTER_MP:
			if(!base_character_data.active)break;
			sprintf((char *)print_rec,"%s%d/%d",MSG_MP_TEXT,base_character_data.data.final.now_mp,base_character_data.data.final.max_mp);
			show=true;
			break;
		case MAP_MAIN_MENU_TASK_BAOBAO_HP:
			if(!base_baobao_data.active)
				break;
			if(base_baobao_data.data.action_baobao_id==0)
				break;
			idx=get_baobao_data_idx(base_baobao_data.data.action_baobao_id);
			if(idx<0)break;
			sprintf((char *)print_rec,"%s%d/%d",MSG_HP_TEXT,base_baobao_data.data.baobao_list[idx].data.now_hp,base_baobao_data.data.baobao_list[idx].data.max_hp);
			show=true;
			break;
		case MAP_MAIN_MENU_TASK_BAOBAO_MP:
			if(!base_baobao_data.active)
				break;
			if(base_baobao_data.data.action_baobao_id==0)
				break;
			idx=get_baobao_data_idx(base_baobao_data.data.action_baobao_id);
			if(idx<0)break;
			sprintf((char *)print_rec,"%s%d/%d",MSG_MP_TEXT,base_baobao_data.data.baobao_list[idx].data.now_mp,base_baobao_data.data.baobao_list[idx].data.max_mp);
			show=true;
			break;
		case MAP_MAIN_MENI_TASK_BAOBAO_EXP:
			if(!base_baobao_data.active)
				break;
			if(base_baobao_data.data.action_baobao_id==0)
				break;
			idx=get_baobao_data_idx(base_baobao_data.data.action_baobao_id);
			if(idx<0)break;
			sprintf((char *)print_rec,"%s%d/%d",MSG_EXP_TEXT,base_baobao_data.data.baobao_list[idx].data.now_exp,base_baobao_data.data.baobao_list[idx].data.need_exp);
			show=true;
			break;
		case MAP_MAIN_MENU_TASK_CHARACTER_STATUS:
			break;
		case MAP_MAIN_MENU_TASK_BAOBAO_STATUS:
			break;
			
		}
		
		if(show)
		{
			xl=strlen((char *)print_rec)*8+8;
			yl=16+8;
			x=mouse_x;
			y=mouse_y-yl;
			if(x<0)x=0;
			if(y<0)y=0;
			if(x+xl>SCREEN_WIDTH)x=SCREEN_WIDTH-xl;
			if(y+yl>SCREEN_HEIGHT)y=SCREEN_HEIGHT-yl;
			
			put_bar(x,y,xl,yl,color_control.white,screen_buffer);
			put_box(x,y,xl,yl,color_control.black,screen_buffer);
			set_word_color(0,color_control.black);
			print16(x+4,y+4,print_rec,PEST_PUT,screen_buffer);
			set_word_color(0,color_control.white);
		}
		
	}
	
	
}


/*******************************************************************************************************************}
{	team status																										}
********************************************************************************************************************/
void display_map_team_status(SLONG team_task,SLONG status)
{
	SLONG mouse_x,mouse_y;
	SLONG x,y,xl,yl;
	SLONG team_no;
	SLONG idx;

	static ULONG timer=0;
	static SLONG old_mouse_x=0,old_mouse_y=0;

	
	if(status==BUTTON_PUSH)
	{
		if(check_have_touch_window(system_image.status_body.rect)>=0)
			return;
	}
	
	mouse_x=system_control.mouse_x;
	mouse_y=system_control.mouse_y;
	if((old_mouse_x!=mouse_x)||(old_mouse_y!=mouse_y))
	{
		old_mouse_x=mouse_x;
		old_mouse_y=mouse_y;
		timer=system_control.system_timer;
	}

	game_control.window_task=MAP_MAIN_MENU_TASK_TEAM_STATUS;
	team_no=MAIN_TASK_TEAM1-team_task;
	if((team_no<0)&&(team_no>=5))return;

	if(status==BUTTON_SELECT)
	{
		if(system_control.system_timer>=timer+1000)
		{
			strcpy((char *)print_rec,team_list_data[team_no].nickname);
			xl=strlen((char *)print_rec)*8+8;
			yl=16+8;
			x=mouse_x;
			y=mouse_y-yl;
			if(x<0)x=0;
			if(y<0)y=0;
			if(x+xl>SCREEN_WIDTH)x=SCREEN_WIDTH-xl;
			if(y+yl>SCREEN_HEIGHT)y=SCREEN_HEIGHT-yl;
			
			put_bar(x,y,xl,yl,color_control.white,screen_buffer);
			put_box(x,y,xl,yl,color_control.black,screen_buffer);
			set_word_color(0,color_control.black);
			print16(x+4,y+4,print_rec,PEST_PUT,screen_buffer);
			set_word_color(0,color_control.white);
			
		}
	}
	else if(status==BUTTON_PUSH)
	{
		if((game_control.window_task>=0)&&(game_control.window_task!=game_control.old_window_task))
		{
			game_control.old_window_task=game_control.window_task;
			idx=get_map_npc_index(team_list_data[team_no].char_id);
			display_map_npc_data(idx);
		}
	}

	
}



/*******************************************************************************************************************}
{	character status																								}
********************************************************************************************************************/
void window_draw_character_status(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	

	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;

	
// --- ( 0 - 0 )  Draw body
	display_img256_buffer(window_x,window_y,system_image.character_attrib_body.image,screen_buffer);	


// --- ( 0 - 0 ) display data
	
	print16(window_x+73,window_y+87,(UCHR *)base_character_data.title,PEST_PUT,screen_buffer);

	print16(window_x+73,window_y+113,(UCHR *)base_character_data.name,PEST_PUT,screen_buffer);

	switch(base_character_data.base_type/2)
	{
	case 0:
		strcpy((char *)print_rec,CHARACTER_PHYLE_HUMAN_TEXT);
		break;
	case 1:
		strcpy((char *)print_rec,CHARACTER_PHYLE_GENUS_TEXT);
		break;
	case 2:
		strcpy((char *)print_rec,CHARACTER_PHYLE_MAGIC_TEXT);
		break;
	case 3:
		strcpy((char *)print_rec,CHARACTER_PHYLE_ROBET_TEXT);
		break;
	}
	
	print16(window_x+73,window_y+165,print_rec,PEST_PUT,screen_buffer);


	if(!base_character_data.active)		// data not available
		return;
	

	sprintf((char *)print_rec,"%d",base_character_data.data.other.credit);	// ����
	print16(window_x+73,window_y+191,print_rec,PEST_PUT,screen_buffer);
	
	sprintf((char *)print_rec,"%d",base_character_data.data.other.kind);		// �ƶ�
	print16(window_x+73,window_y+217,print_rec,PEST_PUT,screen_buffer);
	
	sprintf((char *)print_rec,"%d",base_character_data.data.final.level);	// �ȼ�
	print16(window_x+73,window_y+139,print_rec,PEST_PUT,screen_buffer);
	

	print16(window_x+238,window_y+139,(UCHR *)base_character_data.data.group,PEST_PUT,screen_buffer);	// ����


	sprintf((char *)print_rec,"%d",base_character_data.data.other.military);	// ս��
	print16(window_x+238,window_y+165,print_rec,PEST_PUT,screen_buffer);
	

	sprintf((char *)print_rec,"%d",base_character_data.data.other.dead_count);	// ��������
	print16(window_x+238,window_y+191,print_rec,PEST_PUT,screen_buffer);


	sprintf((char *)print_rec,"%u",base_character_data.data.final.now_exp);		// ��ǰ����
	print16(window_x+238,window_y+217,print_rec,PEST_PUT,screen_buffer);


	sprintf((char *)print_rec,"%u",base_character_data.data.final.need_exp);		// ��������
	print16(window_x+238,window_y+243,print_rec,PEST_PUT,screen_buffer);

	
	sprintf((char *)print_rec,"%d",base_character_data.data.base.point+character_point_back.total_point );	// �ɷ������
	print16(window_x+238,window_y+270,print_rec,PEST_PUT,screen_buffer);
	
	// --- 
	sprintf((char *)print_rec,"%d/%d",base_character_data.data.final.now_hp,base_character_data.data.final.max_hp);			// Ѫ
	print16(window_x+81,window_y+307,print_rec,PEST_PUT,screen_buffer);
	

	sprintf((char *)print_rec,"%d/%d",base_character_data.data.final.now_mp,base_character_data.data.final.max_mp);			// ����
	print16(window_x+81,window_y+333,print_rec,PEST_PUT,screen_buffer);


	sprintf((char *)print_rec,"%d",base_character_data.data.final.att);			// ������
	print16(window_x+81,window_y+359,print_rec,PEST_PUT,screen_buffer);

	
	sprintf((char *)print_rec,"%d",base_character_data.data.final.speed);		// �ٶ�
	print16(window_x+81,window_y+385,print_rec,PEST_PUT,screen_buffer);
	
	
	sprintf((char *)print_rec,"%d",base_character_data.data.base.hp_point+character_point_back.hp_point);		// Ѫ �������
	print16(window_x+244+7,window_y+307,print_rec,PEST_PUT,screen_buffer);
	
	sprintf((char *)print_rec,"%d",base_character_data.data.base.mp_point+character_point_back.mp_point);		// �����������
	print16(window_x+244+7,window_y+333,print_rec,PEST_PUT,screen_buffer);

	sprintf((char *)print_rec,"%d",base_character_data.data.base.att_point+character_point_back.att_point);		// �������������
	print16(window_x+244+7,window_y+359,print_rec,PEST_PUT,screen_buffer);
	
	sprintf((char *)print_rec,"%d",base_character_data.data.base.speed_point+character_point_back.speed_point);	// �ٶȷ������
	print16(window_x+244+7,window_y+385,print_rec,PEST_PUT,screen_buffer);
	



}


void window_exec_character_status(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;

	SLONG task;
	SLONG x_offset,y_offset;
	SLONG i;
	SLONG channel;
	
	
	RECT rect[13]={
		{ 333,41 ,333+28,41+28	},				// �ر�			0
		{ 299,306,299+21,306+21 },				// +	HP		1
		{ 322,306,322+21,306+21 },				// -			2
		{ 299,332,299+21,332+21 },				// +	MP		3
		{ 322,332,322+21,332+21 },				// -			4
		{ 299,358,299+21,358+21 },				// +	ATT		5
		{ 322,358,322+21,358+21 },				// -			6
		{ 299,384,299+21,384+21 },				// +	SPP		7
		{ 322,384,322+21,384+21 },				// -			8
		{ 238, 80,238+122, 80+29 },				// ����ͷ��		9	
		{ 272,268,272+96,268+22 },				// ��������		10
		{ 47 ,416, 47+96,416+22 },				// ���Բ鿴		11
		{ 204,416,204+96,416+22 }				// ���ܲ鿴		12
	};

	

	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	

	task=-1;
	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;
	
	for(i=0;i<13;i++)
	{
		if((x_offset>=rect[i].left)&&(x_offset<rect[i].right)&&
			(y_offset>=rect[i].top)&&(y_offset<rect[i].bottom))
		{
			task=i;
			break;
		}
	}
	
	
	
	switch(system_control.mouse_key)
	{
	case MS_RUp:						//	close 
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_character_status=-1;
		break;

	case MS_Move:
	case MS_Dummy:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// + HP
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand3",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// - HP
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand5",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 3:		// + MP
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand3",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 4:		// - MP
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand5",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 5:		// + ATT
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand3",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 6:		// - ATT
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand5",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 7:		// + SPEED
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand3",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 8:		// - SPEED
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand5",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 9:		// ����ͷ��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand65",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 10:	// ��������
			if(character_point_back.total_point==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand67",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 11:	// ���Բ鿴
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand69",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 12:	// ���ܲ鿴
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand71",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		}
		break;

	case MS_LUp:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_character_status=-1;
			break;
		case 1:		// + HP
			if(abs(character_point_back.total_point)>=base_character_data.data.base.point)break;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			character_point_back.total_point--;
			character_point_back.hp_point++;
			break;
		case 2:		// - HP
			if(character_point_back.hp_point<=0)break;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			character_point_back.total_point++;
			character_point_back.hp_point--;
			break;
		case 3:		// + MP
			if(abs(character_point_back.total_point)>=base_character_data.data.base.point)break;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			character_point_back.total_point--;
			character_point_back.mp_point++;
			break;
		case 4:		// - MP
			if(character_point_back.mp_point<=0)break;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			character_point_back.total_point++;
			character_point_back.mp_point--;
			break;
		case 5:		// + ATT
			if(abs(character_point_back.total_point)>=base_character_data.data.base.point)break;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			character_point_back.total_point--;
			character_point_back.att_point++;
			break;
		case 6:		// - ATT
			if(character_point_back.att_point<=0)break;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			character_point_back.total_point++;
			character_point_back.att_point--;
			break;
		case 7:		// + SPEED
			if(abs(character_point_back.total_point)>=base_character_data.data.base.point)break;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			character_point_back.total_point--;
			character_point_back.speed_point++;
			break;
		case 8:		// - SPEED
			if(character_point_back.speed_point<=0)break;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			character_point_back.total_point++;
			character_point_back.speed_point--;
			break;
		case 9:		// ����ͷ��
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			exec_main_menu_task_title();
			break;
		case 10:	// ��������
			if(character_point_back.total_point>=0)break;
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			send_character_point_adjust();
			break;
		case 11:	// ���Բ鿴
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			exec_main_menu_task_character_fastness();
			break;
		case 12:	// ���ܲ鿴
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			exec_main_menu_task_skill();
			break;
		}
		break;	
	case MS_LDrag:				// �϶�
			
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// + HP
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand4",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// - HP
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand6",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 3:		// + MP
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand4",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 4:		// - MP
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand6",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 5:		// + ATT
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand4",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 6:		// - ATT
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand6",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 7:		// + SPEED
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand4",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 8:		// - SPEED
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand6",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 9:		// ����ͷ��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand66",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 10:	// ��������
			if(character_point_back.total_point==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand68",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 11:	// ���Բ鿴
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand70",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 12:	// ���ܲ鿴
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand72",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		default:
			now_window_move_xoffset=system_control.mouse_x-window_x;
			now_window_move_yoffset=system_control.mouse_y-window_y;
			now_window_move_handle=now_exec_window_handle;
			break;
		}
		break;
		
	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}


void process_window_move(void)
{
	RECT window_rect;
	RECT temp_rect;
	SLONG window_x,window_y,window_xl,window_yl;
	SLONG dummy;

	if(now_window_move_handle<0)
		return;

	if(read_window_base_data(now_window_move_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;

	
	temp_rect.left=system_control.mouse_x-now_window_move_xoffset;
	temp_rect.top=system_control.mouse_y-now_window_move_yoffset;
	temp_rect.right=temp_rect.left+window_xl;
	temp_rect.bottom=temp_rect.top+window_yl;
	set_window_base_data(now_window_move_handle,&temp_rect,dummy);

	switch(system_control.mouse_key)
	{
	default:
		reset_mouse_key();
		system_control.mouse_key=MS_Dummy;
		break;
	case MS_LUp:
	case MS_Move:
		now_window_move_handle=-1;			// clear
		break;
	}

}




/*******************************************************************************************************************}
{	cbaobao status																									}
********************************************************************************************************************/
void window_draw_baobao_status(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	SLONG i,j;
	UCHR npc_filename[256];
	SLONG mda_handle;

	static ULONG timer=0;
	SLONG mda_command;


	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	

	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	
// --- ( 0 - 0 )  Draw body


	display_img256_buffer(window_x,window_y,system_image.baobao_attrib_body.image,screen_buffer);	

// --- ( 0 - 1 ) display data	


	if(!base_baobao_data.active)return;

// --- ( 1 - 0 ) Display baobao stack
	check_baobao_stack();


	for(j=0,i=top_baobao_stack;i<total_baobao_stack;i++,j++)
	{
		if((i-top_baobao_stack)>=6)
			break;
		if(base_baobao_data.data.baobao_list[i].baobao_id==0)
			continue;

		if(i==baobao_stack_index)
		{
			alpha_put_bar(window_x+25,window_y+83+j*20,139,18,color_control.pink,screen_buffer,128);
		} 

		if(i==base_baobao_data.data.active_idx)
		{
			put_bar(window_x+25,window_y+83+j*20,139,18,color_control.green,screen_buffer);
		}
		
		if((system_control.mouse_x>=window_x+25)&&(system_control.mouse_x<window_x+25+139)&&
			(system_control.mouse_y>=window_y+83+j*20)&&(system_control.mouse_y<window_y+83+j*20+18))
		{
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand0",window_x+25,window_y+83+j*20,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
		}
		
		if(base_baobao_data.data.show[i])
		{
			set_word_color(0,color_control.yellow);
			print16(window_x+25+8,window_y+83+1+(i-top_baobao_stack)*20,base_baobao_data.data.baobao_list[i].name,PEST_PUT,screen_buffer);
			set_word_color(0,color_control.white);
		}
		else
		{
			print16(window_x+25+8,window_y+83+1+(i-top_baobao_stack)*20,base_baobao_data.data.baobao_list[i].name,PEST_PUT,screen_buffer);
		}
		
	}
	

// --- ( 2 - 0 ) display select 
	if(baobao_stack_index<0)return;



// --- display baobao image

	sprintf((char *)npc_filename,"NPC\\NPC%da.MDA",base_baobao_data.data.baobao_list[baobao_stack_index].base.filename_id);
	mda_handle=get_mda_handle(npc_filename);

	if(mda_handle<0)
	{
		if(base_baobao_data.display_handle>=0)
		auto_close_map_npc_handle(base_baobao_data.display_handle);
		mda_group_open((UCHR *)npc_filename,&mda_handle);
	}
		
	base_baobao_data.display_handle=mda_handle;


	mda_command=MDA_COMMAND_LOOP;
	if(base_baobao_data.display_handle>=0)
	{
		if(system_control.system_timer>timer)
		{
			mda_command=mda_command|MDA_COMMAND_NEXT_FRAME;
			timer=system_control.system_timer+SYSTEM_MAIN_DELAY;
		}

		mda_group_exec(base_baobao_data.display_handle,(UCHR *)"MainCommand3",window_x+275,window_y+208,
			screen_buffer,mda_command,0,0,0);
	}
	



// ϵ��	
	switch(base_baobao_data.data.baobao_list[baobao_stack_index].base.phyle)
	{
	case BAOBAO_PHYLE_HUMAN	:
		print16(window_x+76,window_y+235,(UCHR *)BAOBAO_PHYLE_HUMAN_TEXT,PEST_PUT,screen_buffer);
		break;
	case BAOBAO_PHYLE_FLY:
		print16(window_x+76,window_y+235,(UCHR *)BAOBAO_PHYLE_FLY_TEXT,PEST_PUT,screen_buffer);
		break;
	case BAOBAO_PHYLE_DEVIL:
		print16(window_x+76,window_y+235,(UCHR *)BAOBAO_PHYLE_DEVIL_TEXT,PEST_PUT,screen_buffer);
		break;
	case BAOBAO_PHYLE_DEAD:
		print16(window_x+76,window_y+235,(UCHR *)BAOBAO_PHYLE_DEAD_TEXT,PEST_PUT,screen_buffer);
		break;
	case BAOBAO_PHYLE_DRAGON:
		print16(window_x+76,window_y+235,(UCHR *)BAOBAO_PHYLE_DRAGON_TEXT,PEST_PUT,screen_buffer);
		break;	
	case BAOBAO_PHYLE_SPECIAL:
		print16(window_x+76,window_y+235,(UCHR *)BAOBAO_PHYLE_SPECIAL_TEXT,PEST_PUT,screen_buffer);
		break;
	}

	
// level
	
	sprintf((char *)print_rec,"%d",base_baobao_data.data.baobao_list[baobao_stack_index].data.level);
	print16(window_x+76,window_y+260,print_rec,PEST_PUT,screen_buffer);


	sprintf((char *)print_rec,"%d",base_baobao_data.data.baobao_list[baobao_stack_index].loyalty);
	print16(window_x+76,window_y+285,print_rec,PEST_PUT,screen_buffer);

	sprintf((char *)print_rec,"%d/%d",base_baobao_data.data.baobao_list[baobao_stack_index].data.now_hp,
		base_baobao_data.data.baobao_list[baobao_stack_index].data.max_hp);
	print16(window_x+75,window_y+311-3,print_rec,PEST_PUT,screen_buffer);

	sprintf((char *)print_rec,"%d/%d",base_baobao_data.data.baobao_list[baobao_stack_index].data.now_mp,
		base_baobao_data.data.baobao_list[baobao_stack_index].data.max_mp);
	print16(window_x+75,window_y+332-3,print_rec,PEST_PUT,screen_buffer);

	sprintf((char *)print_rec,"%d",base_baobao_data.data.baobao_list[baobao_stack_index].data.att);
	print16(window_x+75,window_y+353-3,print_rec,PEST_PUT,screen_buffer);
	
	sprintf((char *)print_rec,"%d",base_baobao_data.data.baobao_list[baobao_stack_index].data.speed);
	print16(window_x+75,window_y+374-3,print_rec,PEST_PUT,screen_buffer);
	
	sprintf((char *)print_rec,"%d/%d",base_baobao_data.data.baobao_list[baobao_stack_index].data.now_exp,
		base_baobao_data.data.baobao_list[baobao_stack_index].data.need_exp);
	print16(window_x+75,window_y+395-3,print_rec,PEST_PUT,screen_buffer);

// ---
	sprintf((char *)print_rec,"%d",base_baobao_data.data.baobao_list[baobao_stack_index].base.hp_point+baobao_point_back[baobao_stack_index].hp_point);
	print16(window_x+250,window_y+311-3,print_rec,PEST_PUT,screen_buffer);
	
	sprintf((char *)print_rec,"%d",base_baobao_data.data.baobao_list[baobao_stack_index].base.mp_point+baobao_point_back[baobao_stack_index].mp_point);
	print16(window_x+250,window_y+332-3,print_rec,PEST_PUT,screen_buffer);
	
	sprintf((char *)print_rec,"%d",base_baobao_data.data.baobao_list[baobao_stack_index].base.att_point+baobao_point_back[baobao_stack_index].att_point);
	print16(window_x+250,window_y+353-3,print_rec,PEST_PUT,screen_buffer);

	sprintf((char *)print_rec,"%d",base_baobao_data.data.baobao_list[baobao_stack_index].base.speed_point+baobao_point_back[baobao_stack_index].speed_point);
	print16(window_x+250,window_y+374-3,print_rec,PEST_PUT,screen_buffer);

	sprintf((char *)print_rec,"%d",base_baobao_data.data.baobao_list[baobao_stack_index].base.point+baobao_point_back[baobao_stack_index].total_point);
	print16(window_x+312,window_y+287-3,print_rec,PEST_PUT,screen_buffer);

	g_PetNameEdit.Paint(window_x, window_y, screen_buffer);
}


void window_exec_baobao_status(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;

	SLONG task;
	SLONG x_offset,y_offset;
	SLONG i;
	SLONG channel;
	UCHR title[80];

	RECT rect[17]={
		{ 333,41 ,333+28,41+28	},				// �ر�			0
		{ 171,150,171+21,150+21	},				// �Ϸ�			1
		{ 171,177,171+21,177+21 },				// �·�			2
		{ 306,305,306+21,305+21 },				// +	HP		3
		{ 306,327,306+21,327+21 },				// +	MP		4
		{ 306,349,306+21,349+21 },				// +	ATT		5
		{ 306,371,306+21,371+21 },				// +	SPP		6
		{ 329,305,322+21,305+21 },				// -			7
		{ 329,327,322+21,327+21 },				// -			8
		{ 329,349,322+21,349+21 },				// -			9
		{ 329,371,322+21,371+21 },				// -			10
		{  30,417, 30+63,417+21 },				// ����			11	
		{ 125,416,125+96,416+22 },				// ���Բ鿴		12
		{ 246,416,246+96,416+22 },				// ��������		13
		{ 198,235,198+71,235+19 },				// ��ս			14
		{ 285,235,285+71,235+19 },				// �ۿ�			15
		{ 204,261,204+63,261+21 }				// ��Ʒ			16
	};
	
	
	
	
	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	

	task=-1;
	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;
	
	for(i=0;i<17;i++)
	{
		if((x_offset>=rect[i].left)&&(x_offset<rect[i].right)&&
			(y_offset>=rect[i].top)&&(y_offset<rect[i].bottom))
		{
			task=i;
			break;
		}
	}
	

	for(i=0;i<6;i++)
	{	
		if((system_control.mouse_x>=window_x+25)&&(system_control.mouse_x<window_x+25+139)&&
			(system_control.mouse_y>=window_y+83+i*20)&&(system_control.mouse_y<window_y+83+i*20+18))
		{
			task=100+i;
		}
	}
	
	
	
	switch(system_control.mouse_key)
	{
	case MS_LDn:
		g_PetNameEdit.LBtnDown(system_control.mouse_x, system_control.mouse_y);
		break;
		
	case MS_RUp:						//	close 
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_baobao_status=-1;
		g_PetNameEdit.Shutdown();
		break;
	case MS_Move:
	case MS_Dummy:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// �Ϸ�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand7",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// �·�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand9",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 3:		// + HP
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand3",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 4:		// + MP
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand3",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 5:		// + ATT
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand3",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 6:		// + SPEED
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand3",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 7:		// - HP
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand5",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 8:		// - MP
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand5",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 9:		// - ATT
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand5",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 10:		// - SPEED
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand5",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 11:	// ����
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand83",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 12:	// ���Բ鿴
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand69",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 13:	// ��������
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			if(baobao_point_back[baobao_stack_index].total_point==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand67",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 14:	// ��ս
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand73",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 15:	// �ۿ�
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand75",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 16:	// ��Ʒ
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand81",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		}
		break;

	case MS_LUp:
		g_PetNameEdit.LBtnDown(system_control.mouse_x, system_control.mouse_y);
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_baobao_status=-1;
			g_PetNameEdit.Shutdown();
			break;
		case 1:		// �Ϸ�
			if(top_baobao_stack<=0)break;
			top_baobao_stack--;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			break;
		case 2:		// �·�
			if(top_baobao_stack>=total_baobao_stack-6)break;
			top_baobao_stack++;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			break;
		case 3:		// + HP
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;

			if(abs(baobao_point_back[baobao_stack_index].total_point)>=base_baobao_data.data.baobao_list[baobao_stack_index].base.point)break;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);

			baobao_point_back[baobao_stack_index].total_point--;
			baobao_point_back[baobao_stack_index].hp_point++;
			
			break;
		case 4:		// + MP
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			
			if(abs(baobao_point_back[baobao_stack_index].total_point)>=base_baobao_data.data.baobao_list[baobao_stack_index].base.point)break;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);

			baobao_point_back[baobao_stack_index].total_point--;
			baobao_point_back[baobao_stack_index].mp_point++;
			
			break;
		case 5:		// + ATT
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			
			if(abs(baobao_point_back[baobao_stack_index].total_point)>=base_baobao_data.data.baobao_list[baobao_stack_index].base.point)break;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);

			baobao_point_back[baobao_stack_index].total_point--;
			baobao_point_back[baobao_stack_index].att_point++;
			
			break;
		case 6:		// + SPEED
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			
			if(abs(baobao_point_back[baobao_stack_index].total_point)>=base_baobao_data.data.baobao_list[baobao_stack_index].base.point)break;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);

			baobao_point_back[baobao_stack_index].total_point--;
			baobao_point_back[baobao_stack_index].speed_point++;
			
			break;
		case 7:		// - HP
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
		
			if(baobao_point_back[baobao_stack_index].hp_point<=0)break;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);

			baobao_point_back[baobao_stack_index].total_point++;
			baobao_point_back[baobao_stack_index].hp_point--;
			
			break;
		case 8:		// - MP
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			
			if(baobao_point_back[baobao_stack_index].mp_point<=0)break;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);

			baobao_point_back[baobao_stack_index].total_point++;
			baobao_point_back[baobao_stack_index].mp_point--;
			
			break;
		case 9:		// - ATT
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			
			if(baobao_point_back[baobao_stack_index].att_point<=0)break;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);

			baobao_point_back[baobao_stack_index].total_point++;
			baobao_point_back[baobao_stack_index].att_point--;
			
			break;
		case 10:		// - SPEED
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			
			if(baobao_point_back[baobao_stack_index].speed_point<=0)break;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);

			baobao_point_back[baobao_stack_index].total_point++;
			baobao_point_back[baobao_stack_index].speed_point--;
			
			break;
		case 11:	// ����
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			if(base_baobao_data.data.show[baobao_stack_index])
			{
				display_system_message((UCHR *)MSG_BAOBAO_RELEASE_ERROR);
				break;
			}
			if(base_baobao_data.data.action_baobao_id==base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id)
			{
				display_system_message((UCHR *)MSG_BAOBAO_RELEASE_ERROR1);
				break;
			}
			sprintf((char *)title,MSG_RELEASE_BAOBAO,base_baobao_data.data.baobao_list[baobao_stack_index].name);
			ask_yes_no((UCHR *)title,&release_baobao,NULL);
			
			break;
		case 12:	// ���Բ鿴
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			exec_main_menu_task_baobao_fastness();
			break;
		case 13:	// ��������
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			if(baobao_point_back[baobao_stack_index].total_point>=0)break;
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			send_baobao_point_adjust(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id);
			break;
		case 14:	// ��ս
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			if(base_baobao_data.data.active_idx==baobao_stack_index)	// ���ñ�������ս
			{
				disable_baobao_action();
			}
			else														// ���ñ�����ս
			{
				enable_baobao_action(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id);
			}

			break;
		case 15:	// �ۿ�
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			if(base_baobao_data.data.show[baobao_stack_index])
				disable_baobao_show(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id);
			else
				enable_baobao_show(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id);

			break;
		case 16:	// ��Ʒ
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			exec_main_menu_task_baobao_item();
			break;

		case 100: case 101:case 102:case 103:case 104:case 105:
			if(base_baobao_data.data.baobao_list[task-100+top_baobao_stack].baobao_id>0)
			{
				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
				baobao_stack_index=task-100+top_baobao_stack;

				g_PetNameEdit.Erase();
				if (*base_baobao_data.data.baobao_list[baobao_stack_index].name)
				{
					g_PetNameEdit.setText(base_baobao_data.data.baobao_list[baobao_stack_index].name);
				}
			}
			break;
		}
		break;
		
	case MS_LDrag:				// �϶�
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
//			g_PetNameEdit.Shutdown();
			break;
		case 1:		// �Ϸ�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand8",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// �·�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand10",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 3:		// + HP
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand4",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 4:		// + MP
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand4",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 5:		// + ATT
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand4",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 6:		// + SPEED
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand4",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 7:		// - HP
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand6",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 8:		// - MP
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand6",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 9:		// - ATT
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand6",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 10:		// - SPEED
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand6",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 11:	// ����
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand84",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 12:	// ���Բ鿴
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand70",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 13:	// ��������
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			if(baobao_point_back[baobao_stack_index].total_point==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand68",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 14:	// ��ս
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand74",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 15:	// �ۿ�
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand76",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 16:	// ��Ʒ
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand82",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		default:
			now_window_move_xoffset=system_control.mouse_x-window_x;
			now_window_move_yoffset=system_control.mouse_y-window_y;
			now_window_move_handle=now_exec_window_handle;
			break;
		}
		break;
		
	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}



/*******************************************************************************************************************}
{	history																											}
********************************************************************************************************************/
void window_draw_history(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	
	
	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	
// --- ( 0 - 0 )  Draw body
	display_img256_buffer(window_x,window_y,system_image.history_body.image,screen_buffer);	
	
// --- ( 1 - 0 ) show data 
	display_hirstory_chat_inter_window();
	
}



void window_exec_history(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	SLONG task;
	SLONG x_offset,y_offset;
	SLONG i;
	SLONG channel;
	RECT rect[10]={
		{ 406,10 ,406+28,10+28 	},				// �ر�
		{ 375,286,375+21,286+21 },				// �Ϸ�
		{ 400,286,400+21,286+21 },				// �·�
		{  23, 46, 23+53, 46+20 },				// ��ǰ
		{  81, 46, 81+53, 46+20 },				// ����
		{ 139, 46,139+53, 46+20 },				// ˽��
		{ 197, 46,197+53, 46+20 },				// ����
		{ 255, 46,255+53, 46+20 },				// ����
		{ 313, 46,313+53, 46+20 },				// ����
		{ 371, 46,371+53, 46+20 }				// ϵͳ
		
	};
	

		

	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	task=-1;
	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;
	
	for(i=0;i<10;i++)
	{
		if((x_offset>=rect[i].left)&&(x_offset<rect[i].right)&&
			(y_offset>=rect[i].top)&&(y_offset<rect[i].bottom))
		{
			task=i;
			break;
		}
	}
	
	
	
	
	switch(system_control.mouse_key)
	{
	case MS_RUp:						//	close 
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_history=-1;
		break;
	case MS_Move:
	case MS_Dummy:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// �Ϸ�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand7",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// �·�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand9",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 3:		// ��ǰ
			mda_group_exec(system_image.main_menu_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 4:		// ����
			mda_group_exec(system_image.main_menu_handle,(UCHR *)"MainCommand3",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 5:		// ˽��
			mda_group_exec(system_image.main_menu_handle,(UCHR *)"MainCommand5",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 6:		// ����
			mda_group_exec(system_image.main_menu_handle,(UCHR *)"MainCommand7",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 7:		// ����
			mda_group_exec(system_image.main_menu_handle,(UCHR *)"MainCommand9",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 8:		// ����
			mda_group_exec(system_image.main_menu_handle,(UCHR *)"MainCommand11",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 9:		// ϵͳ
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand45",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
			
		}
		break;
	case MS_LDn:
	case MS_LDrag:				// �϶�
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// �Ϸ�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand8",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// �·�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand10",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 3:		// ��ǰ
			mda_group_exec(system_image.main_menu_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 4:		// ����
			mda_group_exec(system_image.main_menu_handle,(UCHR *)"MainCommand4",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 5:		// ˽��
			mda_group_exec(system_image.main_menu_handle,(UCHR *)"MainCommand6",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 6:		// ����
			mda_group_exec(system_image.main_menu_handle,(UCHR *)"MainCommand8",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 7:		// ����
			mda_group_exec(system_image.main_menu_handle,(UCHR *)"MainCommand10",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 8:		// ����
			mda_group_exec(system_image.main_menu_handle,(UCHR *)"MainCommand12",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 9:		// ϵͳ
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand46",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
			
		}

		break;			

	case MS_LUp:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_history=-1;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			break;
		case 1:		// �Ϸ�
			auto_play_voice(0,(UCHR *)"EFC\\button02.wav",NULL,0,0,0,0,&channel);
			scroll_hirstory_chat_data(Up);
			break;
		case 2:		// �·�
			auto_play_voice(0,(UCHR *)"EFC\\button02.wav",NULL,0,0,0,0,&channel);
			scroll_hirstory_chat_data(Dn);
			break;
		case 3:		// ��ǰ
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			hirstory_channel=CHAT_CHANNEL_SCREEN;
			copy_chat_data_to_hirstory();
			break;
		case 4:		// ����
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			hirstory_channel=CHAT_CHANNEL_TEAM;
			copy_chat_data_to_hirstory();
			break;
		case 5:		// ˽��
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			hirstory_channel=CHAT_CHANNEL_PERSON;
			copy_chat_data_to_hirstory();
			break;
		case 6:		// ����
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			hirstory_channel=CHAT_CHANNEL_GROUP;
			copy_chat_data_to_hirstory();
			break;
		case 7:		// ����
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			hirstory_channel=CHAT_CHANNEL_SELLBUY;
			copy_chat_data_to_hirstory();
			break;
		case 8:		// ����
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			hirstory_channel=CHAT_CHANNEL_WORLD;
			copy_chat_data_to_hirstory();
			break;
		case 9:		// ϵͳ
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			hirstory_channel=CHAT_CHANNEL_SYSTEM;
			copy_chat_data_to_hirstory();
			break;
		}
		
		break;
			

		
	}
	

	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}





/*******************************************************************************************************************}
{	item																											}
********************************************************************************************************************/
void window_draw_item(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	SLONG i;
	SLONG inst_x,inst_y,inst_xl,inst_yl;
	RECT test_rect;

	
	RECT item_pp[MAX_POCKET_ITEM]={
		{ 44+40*0, 268+40*0, 44+40*0+32, 268+40*0+32 },
		{ 44+40*1, 268+40*0, 44+40*1+32, 268+40*0+32 },
		{ 44+40*2, 268+40*0, 44+40*2+32, 268+40*0+32 },
		{ 44+40*3, 268+40*0, 44+40*3+32, 268+40*0+32 },
		{ 44+40*4, 268+40*0, 44+40*4+32, 268+40*0+32 },
		{ 44+40*5, 268+40*0, 44+40*5+32, 268+40*0+32 },

		{ 44+40*0, 268+40*1, 44+40*0+32, 268+40*1+32 },
		{ 44+40*1, 268+40*1, 44+40*1+32, 268+40*1+32 },
		{ 44+40*2, 268+40*1, 44+40*2+32, 268+40*1+32 },
		{ 44+40*3, 268+40*1, 44+40*3+32, 268+40*1+32 },
		{ 44+40*4, 268+40*1, 44+40*4+32, 268+40*1+32 },
		{ 44+40*5, 268+40*1, 44+40*5+32, 268+40*1+32 },

		{ 44+40*0, 268+40*2, 44+40*0+32, 268+40*2+32 },
		{ 44+40*1, 268+40*2, 44+40*1+32, 268+40*2+32 },
		{ 44+40*2, 268+40*2, 44+40*2+32, 268+40*2+32 },
		{ 44+40*3, 268+40*2, 44+40*3+32, 268+40*2+32 },
		{ 44+40*4, 268+40*2, 44+40*4+32, 268+40*2+32 },
		{ 44+40*5, 268+40*2, 44+40*5+32, 268+40*2+32 },

		{ 44+40*0, 268+40*3, 44+40*0+32, 268+40*3+32 },
		{ 44+40*1, 268+40*3, 44+40*1+32, 268+40*3+32 },
		{ 44+40*2, 268+40*3, 44+40*2+32, 268+40*3+32 },
		{ 44+40*3, 268+40*3, 44+40*3+32, 268+40*3+32 },
		{ 44+40*4, 268+40*3, 44+40*4+32, 268+40*3+32 },
		{ 44+40*5, 268+40*3, 44+40*5+32, 268+40*3+32 }
		
	};

	RECT equip_pp[MAX_EQUIP_ITEM]={
		{ 90, 73, 90+32, 73+32  },			// ͷ��
		{ 90,117, 90+32,117+32  },			// ͷ��
		{ 34,155, 34+32,155+32  },			// ͷ��
		{ 34,208, 34+32,208+32  },			// ͷ��
		{150, 92,150+32, 92+32  },			// ͷ��
		{150,134,150+32,134+32  },			// ͷ��
		{150,176,150+32,176+32  }			// ͷ��
	};
	


	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	
	// --- ( 0 - 0 )  Draw body
	display_img256_buffer(window_x,window_y,system_image.item_body.image,screen_buffer);	

	// --- ( 1 - 0 ) display data
	if(character_pocket_data.active)
	{
		for(i=0;i<MAX_POCKET_ITEM;i++)
		{

			if((character_pocket_data.item[i].number>0)&&(character_pocket_data.item[i].filename_id>=0))
			{
				
				display_img256_buffer(window_x+item_pp[i].left,window_y+item_pp[i].top,
					system_image.item_image[character_pocket_data.item[i].filename_id],screen_buffer);

				if(character_pocket_data.item[i].number>1)
				{
					sprintf((char *)print_rec,"%d",character_pocket_data.item[i].number);
					print12(window_x+item_pp[i].right-(strlen((char *)print_rec)*6)-6-1,window_y+item_pp[i].bottom-10-1,print_rec,COPY_PUT,screen_buffer);
					
				}
				
			}

			if(i==character_pocket_data.idx)
			{
				put_scroll_box(window_x+item_pp[i].left-1,
					window_y+item_pp[i].top-1,
					window_x+item_pp[i].right+1,
					window_y+item_pp[i].bottom+1,color_control.red,color_control.yellow,screen_buffer);
				put_scroll_box(window_x+item_pp[i].left-2,
					window_y+item_pp[i].top-2,
					window_x+item_pp[i].right+2,
					window_y+item_pp[i].bottom+2,color_control.red,color_control.yellow,screen_buffer);
			}
			
		}

	}

	if(character_equip_data.active)
	{
		for(i=0;i<MAX_EQUIP_ITEM;i++)
		{
			if((character_equip_data.equip[i].number>0)&&(character_equip_data.equip[i].filename_id>=0))
			{
				display_img256_buffer(window_x+equip_pp[i].left,window_y+equip_pp[i].top,
					system_image.item_image[character_equip_data.equip[i].filename_id],screen_buffer);
			}


			if(i==character_equip_data.idx)
			{
				put_scroll_box(window_x+equip_pp[i].left-1,
					window_y+equip_pp[i].top-1,
					window_x+equip_pp[i].right+1,
					window_y+equip_pp[i].bottom+1,color_control.red,color_control.yellow,screen_buffer);
				put_scroll_box(window_x+equip_pp[i].left-2,
					window_y+equip_pp[i].top-2,
					window_x+equip_pp[i].right+2,
					window_y+equip_pp[i].bottom+2,color_control.red,color_control.yellow,screen_buffer);
			}
			
		}
		
	}


	if(character_bank_data.active)
	{
		get_money_string(character_bank_data.now_money,print_rec);
		print16(window_x+214,window_y+125-2,print_rec,PEST_PUT,screen_buffer);
		get_money_string(character_bank_data.bank_money,print_rec);
		print16(window_x+214,window_y+125+89-2,print_rec,PEST_PUT,screen_buffer);
	}


// ------ display inst
	if((system_control.mouse_x>window_x)&&(system_control.mouse_x<=window_x+window_xl)&&
		(system_control.mouse_y>window_y)&&(system_control.mouse_y<=window_y+window_yl))
	{
		if((detail_item_inst.active)&&(detail_item_inst.idx>=0))
		{
			switch(detail_item_inst.type)
			{
			case ITEM_INST_POCKET:
				if((character_pocket_data.item[detail_item_inst.idx].number>0)&&(character_pocket_data.item[detail_item_inst.idx].filename_id>=0))
				{
					test_rect.left=0;
					test_rect.top=0;
					test_rect.right=160;
					test_rect.bottom=SCREEN_WIDTH;
					inst_xl=32+32*2+160;
					
					text_out_data.g_pChat->insertString((char *)detail_item_inst.inst,test_rect,false);
					inst_yl=test_rect.bottom+32;
					if(inst_yl<80)
						inst_yl=80;
					
					inst_x=system_control.mouse_x-inst_xl/2;
					inst_y=system_control.mouse_y-inst_yl-16;
					if(inst_x<=0)
						inst_x=system_control.mouse_x;
					
					if(inst_y<0)
						inst_y=system_control.mouse_y+32;
					if(inst_x+inst_xl>=SCREEN_WIDTH)inst_x=SCREEN_WIDTH-inst_xl;
					if(inst_y+inst_yl>=SCREEN_HEIGHT)inst_y=SCREEN_HEIGHT-inst_yl;
					draw_message_box(inst_x,inst_y,inst_xl,inst_yl,screen_buffer);
					put_bar(inst_x+2,inst_y+2,inst_xl-4,inst_yl-6,color_control.black,screen_buffer);
					
					display_zoom_img256_buffer(inst_x+16,inst_y+16,
						system_image.item_image[character_pocket_data.item[detail_item_inst.idx].filename_id],screen_buffer,200);
					
					text_out_data.g_pChat->updateString(inst_x+16+64,inst_y+16,test_rect.right,inst_yl,detail_item_inst.inst);
					text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,screen_buffer,false);
				}
				
				break;
			case ITEM_INST_EQUIP:
				if((character_equip_data.equip[detail_item_inst.idx].number>0)&&(character_equip_data.equip[detail_item_inst.idx].filename_id>=0))
				{
					test_rect.left=0;
					test_rect.top=0;
					test_rect.right=160;
					test_rect.bottom=SCREEN_WIDTH;
					inst_xl=32+32*2+160;
					
					text_out_data.g_pChat->insertString((char *)detail_item_inst.inst,test_rect,false);
					inst_yl=test_rect.bottom+32;
					if(inst_yl<80)
						inst_yl=80;
					
					inst_x=system_control.mouse_x-inst_xl/2;
					inst_y=system_control.mouse_y-inst_yl-16;
					if(inst_x<=0)
						inst_x=system_control.mouse_x;
					
					if(inst_y<0)
						inst_y=system_control.mouse_y+32;
					if(inst_x+inst_xl>=SCREEN_WIDTH)inst_x=SCREEN_WIDTH-inst_xl;
					if(inst_y+inst_yl>=SCREEN_HEIGHT)inst_y=SCREEN_HEIGHT-inst_yl;
					draw_message_box(inst_x,inst_y,inst_xl,inst_yl,screen_buffer);
					put_bar(inst_x+2,inst_y+2,inst_xl-4,inst_yl-6,color_control.black,screen_buffer);
					
					display_zoom_img256_buffer(inst_x+16,inst_y+16,
						system_image.item_image[character_equip_data.equip[detail_item_inst.idx].filename_id],screen_buffer,200);
					
					text_out_data.g_pChat->updateString(inst_x+16+64,inst_y+16,test_rect.right,inst_yl,detail_item_inst.inst);
					text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,screen_buffer,false);
				}
				
				break;
			}
			
		}
	}

}



void window_exec_item(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;

	SLONG task;
	SLONG x_offset,y_offset;
	SLONG i;
	SLONG channel;
	SLONG change_idx;
	
	
	RECT rect[4]={
		{ 333,34 ,333+28,34+28	},				// �ر�			0
		{ 293,290,293+63,290+21 },				// ʹ�� 		1
		{ 293,329,293+63,329+21 },				// ����			2
		{ 292,370,292+63,370+21 }				// ����			3
	};
	

	RECT item_pp[MAX_POCKET_ITEM]={
		{ 44+40*0, 268+40*0, 44+40*0+32, 268+40*0+32 },
		{ 44+40*1, 268+40*0, 44+40*1+32, 268+40*0+32 },
		{ 44+40*2, 268+40*0, 44+40*2+32, 268+40*0+32 },
		{ 44+40*3, 268+40*0, 44+40*3+32, 268+40*0+32 },
		{ 44+40*4, 268+40*0, 44+40*4+32, 268+40*0+32 },
		{ 44+40*5, 268+40*0, 44+40*5+32, 268+40*0+32 },
		
		{ 44+40*0, 268+40*1, 44+40*0+32, 268+40*1+32 },
		{ 44+40*1, 268+40*1, 44+40*1+32, 268+40*1+32 },
		{ 44+40*2, 268+40*1, 44+40*2+32, 268+40*1+32 },
		{ 44+40*3, 268+40*1, 44+40*3+32, 268+40*1+32 },
		{ 44+40*4, 268+40*1, 44+40*4+32, 268+40*1+32 },
		{ 44+40*5, 268+40*1, 44+40*5+32, 268+40*1+32 },
		
		{ 44+40*0, 268+40*2, 44+40*0+32, 268+40*2+32 },
		{ 44+40*1, 268+40*2, 44+40*1+32, 268+40*2+32 },
		{ 44+40*2, 268+40*2, 44+40*2+32, 268+40*2+32 },
		{ 44+40*3, 268+40*2, 44+40*3+32, 268+40*2+32 },
		{ 44+40*4, 268+40*2, 44+40*4+32, 268+40*2+32 },
		{ 44+40*5, 268+40*2, 44+40*5+32, 268+40*2+32 },
		
		{ 44+40*0, 268+40*3, 44+40*0+32, 268+40*3+32 },
		{ 44+40*1, 268+40*3, 44+40*1+32, 268+40*3+32 },
		{ 44+40*2, 268+40*3, 44+40*2+32, 268+40*3+32 },
		{ 44+40*3, 268+40*3, 44+40*3+32, 268+40*3+32 },
		{ 44+40*4, 268+40*3, 44+40*4+32, 268+40*3+32 },
		{ 44+40*5, 268+40*3, 44+40*5+32, 268+40*3+32 }
		
	};
	
	RECT equip_pp[MAX_EQUIP_ITEM]={
		{ 90, 73, 90+32, 73+32  },			// ͷ��
		{ 90,117, 90+32,117+32  },			// ͷ��
		{ 34,155, 34+32,155+32  },			// ͷ��
		{ 34,208, 34+32,208+32  },			// ͷ��
		{150, 92,150+32, 92+32  },			// ͷ��
		{150,134,150+32,134+32  },			// ͷ��
		{150,176,150+32,176+32  }			// ͷ��
	};
	
	
	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	
	task=-1;
	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;
	
	for(i=0;i<4;i++)
	{
		if((x_offset>=rect[i].left)&&(x_offset<rect[i].right)&&
			(y_offset>=rect[i].top)&&(y_offset<rect[i].bottom))
		{
			task=i;
			break;
		}
	}

	if(task<0)
	{
		for(i=0;i<MAX_POCKET_ITEM;i++)
		{
			if((x_offset>=item_pp[i].left)&&(x_offset<item_pp[i].right)&&
				(y_offset>=item_pp[i].top)&&(y_offset<item_pp[i].bottom))
			{
				task=100+i;
				break;
			}
			
		}
	}
	
	if(task<0)
	{
		for(i=0;i<MAX_EQUIP_ITEM;i++)
		{
			if((x_offset>=equip_pp[i].left)&&(x_offset<equip_pp[i].right)&&
				(y_offset>=equip_pp[i].top)&&(y_offset<equip_pp[i].bottom))
			{
				task=200+i;
				break;
			}
			
		}
	}
	

	if(task<0)
	{
		clear_detail_item_inst();
	}

	if((task>=100)&&(task<200))
	{
		detail_item_inst.idx=task-100;
		if((character_pocket_data.item[detail_item_inst.idx].number>0)&&(character_pocket_data.item[detail_item_inst.idx].filename_id>=0))
		{
			if(detail_item_inst.old_idx!=detail_item_inst.idx)
			{
				if(system_control.system_timer>detail_item_inst.timer+DETAIL_DELAY_TIME)
				{
					detail_item_inst.old_idx=detail_item_inst.idx;
					detail_item_inst.active=false;
					detail_item_inst.type=ITEM_INST_POCKET;
					request_detail_inst();
					detail_item_inst.timer=system_control.system_timer;
				}
			}
		}
	}

	if((task>=200)&&(task<300))
	{
		detail_item_inst.idx=task-200;
		if((character_equip_data.equip[detail_item_inst.idx].number>0)&&(character_equip_data.equip[detail_item_inst.idx].filename_id>=0))
		{
			if(detail_item_inst.old_idx!=detail_item_inst.idx)
			{
				if(system_control.system_timer>detail_item_inst.timer+DETAIL_DELAY_TIME)
				{
					detail_item_inst.old_idx=detail_item_inst.idx;
					detail_item_inst.active=false;
					detail_item_inst.type=ITEM_INST_EQUIP;
					request_detail_inst();
					detail_item_inst.timer=system_control.system_timer;
				}
			}
			else
			{
				detail_item_inst.timer=system_control.system_timer;
			}
			
		}
	}
				

	
	switch(system_control.mouse_key)
	{
	case MS_RUp:						//	close 
		clear_detail_item_inst();
										// ����
		if(task<0)				// close
		{
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_item=-1;
		}
		else if(task<200)		// ����
		{
			change_idx=task-100;
			if(character_pocket_data.idx<0)break;
			if((change_idx<0)||(change_idx>=MAX_POCKET_ITEM))break;
			send_character_item_change(character_pocket_data.idx,change_idx);
				
		} else					// ж��װ��
		{
			change_idx=task-200;
			send_character_equip_take_off(change_idx);
		}
		
		break;

	case MS_Move:
	case MS_Dummy:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// ʹ��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand90",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand92",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 3:		// ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand94",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		}
		break;


	case MS_LUp:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_item=-1;
			break;
		case 1:		// ʹ��
			if(character_pocket_data.idx<0)break;
			if(character_pocket_data.item[character_pocket_data.idx].number<=0)break;
			if(character_pocket_data.item[character_pocket_data.idx].item_id<0)break;
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			send_character_item_use(character_pocket_data.idx);
			break;
		case 2:		// ����
			if(character_pocket_data.idx<0)break;
			if(character_pocket_data.item[character_pocket_data.idx].number<=0)break;
			if(character_pocket_data.item[character_pocket_data.idx].item_id<0)break;
			ask_yes_no((UCHR *)MSG_ITEM_DROP,&character_item_drop,NULL);
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			break;
		case 3:		// ����
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			break;

		case 100: case 101: case 102: case 103: case 104: case 105:
		case 106: case 107: case 108: case 109: case 110: case 111:
		case 112: case 113: case 114: case 115: case 116: case 117:
		case 118: case 119: case 120: case 121: case 122: case 123:
			character_pocket_data.idx=task-100;
			break;

		case 200: case 201: case 202: case 203: case 204: case 205: case 206:
			character_equip_data.idx=task-200;
			break;

		}
		break;

			
	case MS_LDrag:				// �϶�

		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// ʹ��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand91",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand93",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 3:		// ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand95",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		default:
			now_window_move_xoffset=system_control.mouse_x-window_x;
			now_window_move_yoffset=system_control.mouse_y-window_y;
			now_window_move_handle=now_exec_window_handle;
			break;
		}
		
		break;
		
	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}






/*******************************************************************************************************************}
{	mission																											}
********************************************************************************************************************/
void window_draw_mission(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	SLONG i;
	SLONG display_x,display_y;

	SLONG text_x,text_y,text_xl,text_yl;
	RECT test_rect;
	
	
	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	text_x=window_x+35;
	text_y=window_y+68;
	text_xl=256;
	text_yl=162;
	display_x=text_x;
	display_y=text_y;
	
// --- ( 0 - 0 )  Draw body
	display_img256_buffer(window_x,window_y,system_image.mission_body.image,screen_buffer);	

	for(i=mission_data_start_index;i<total_mission_data;i++)
	{

		test_rect.left=0;
		test_rect.top=0;
		test_rect.right=text_xl;
		test_rect.bottom=SCREEN_HEIGHT;
		text_out_data.g_pChat->insertString((char *)mission_data[i].text,test_rect,false);
		
		if(display_y+test_rect.bottom>text_y+text_yl)		// over font
		{
			i=total_mission_data;
			break;
		}


		text_out_data.g_pChat->updateString(display_x,display_y,text_xl,text_yl,
			(char *)mission_data[i].text);

		display_y=display_y+test_rect.bottom;

	}

	text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,screen_buffer,false);
	

	
}



void window_exec_mission(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;

	SLONG task;
	SLONG x_offset,y_offset;
	SLONG i;
	SLONG channel;
	

	RECT rect[3]={
		{ 311,10 ,311+28,10+28	},				// �ر�
		{ 309,163,309+21,163+21 },				// �Ϸ�
		{ 309,187,309+21,187+21 }				// �·�
	};
	
	
	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	
	task=-1;
	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;
	
	for(i=0;i<3;i++)
	{
		if((x_offset>=rect[i].left)&&(x_offset<rect[i].right)&&
			(y_offset>=rect[i].top)&&(y_offset<rect[i].bottom))
		{
			task=i;
			break;
		}
	}
	
	
	switch(system_control.mouse_key)
	{
	case MS_RUp:						//	close 
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_mission=-1;
		break;

	case MS_Move:
	case MS_Dummy:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// �Ϸ�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand7",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// �·�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand9",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		
		}
		break;

	case MS_LUp:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_mission=-1;
			break;
		case 1:		// �Ϸ�
			if(mission_data_start_index>0)
				mission_data_start_index--;
			auto_play_voice(0,(UCHR *)"EFC\\button02.wav",NULL,0,0,0,0,&channel);
			break;
		case 2:		// �·�
			if(mission_data_start_index<total_mission_data-1)
				mission_data_start_index++;
			auto_play_voice(0,(UCHR *)"EFC\\button02.wav",NULL,0,0,0,0,&channel);
			break;
		}
		break;	

	case MS_LDrag:				// �϶�

		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// �Ϸ�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand8",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// �·�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand10",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		default:
			now_window_move_xoffset=system_control.mouse_x-window_x;
			now_window_move_yoffset=system_control.mouse_y-window_y;
			now_window_move_handle=now_exec_window_handle;
			break;
		}
			

		break;
		
	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}







/*******************************************************************************************************************}
{	group																											}
********************************************************************************************************************/
void window_draw_group(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	
	
	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	
	// --- ( 0 - 0 )  Draw body
	display_img256_buffer(window_x,window_y,system_image.group_body.image,screen_buffer);	
	
}



void window_exec_group(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	
	
	SLONG task;
	SLONG x_offset,y_offset;
	SLONG i;
	SLONG channel;
	
	
	RECT rect[4]={
		{ 333,34 ,333+28,34+28	},				// �ر�			0
		{ 321,346,321+21,346+21 },				// ����			1
		{ 321,373,321+21,373+21 },				// ����			2
		{  16,403, 16+100,403+25 }				// ��Ա�б�		3
	};
	

	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	task=-1;
	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;
	
	for(i=0;i<4;i++)
	{
		if((x_offset>=rect[i].left)&&(x_offset<rect[i].right)&&
			(y_offset>=rect[i].top)&&(y_offset<rect[i].bottom))
		{
			task=i;
			break;
		}
	}
	
	
	
	
	switch(system_control.mouse_key)
	{
	case MS_RUp:						//	close 
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_group=-1;
		break;

	case MS_Move:
	case MS_Dummy:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand7",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand9",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 3:		// ��Ա�б�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand96",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		}
		break;

	case MS_LUp:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_group=-1;
			break;
		case 1:		// ����
			auto_play_voice(0,(UCHR *)"EFC\\button02.wav",NULL,0,0,0,0,&channel);
			break;
		case 2:		// ����
			auto_play_voice(0,(UCHR *)"EFC\\button02.wav",NULL,0,0,0,0,&channel);
			break;
		case 3:		// ��Ա�б�
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			break;
		}
		break;	

	case MS_LDrag:				// �϶�
			
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand8",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand10",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 3:		// ��Ա�б�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand97",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		default:
			now_window_move_xoffset=system_control.mouse_x-window_x;
			now_window_move_yoffset=system_control.mouse_y-window_y;
			now_window_move_handle=now_exec_window_handle;
			break;
		}
		break;
		
	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}


void exec_main_loop_select_npc(void)
{
	UHINT screen_mask;
	SLONG mouse_x,mouse_y;

	
// ---- ( 1 - 0 ) ��ȡ screen_mask & ��� mouse �����Ǹ� TASK ����
	mouse_x=system_control.zoom_mouse_x;
	mouse_y=system_control.zoom_mouse_y;

	if((mouse_x>=0)&&(mouse_y>=0)&&(mouse_x<SCREEN_WIDTH)&&(mouse_y<SCREEN_HEIGHT))
	{
		screen_mask=map_control.screen_mask->line[mouse_y][mouse_x];
		if(screen_mask>=MAIN_TASK_END)
		{
			system_control.point_npc_idx=-1;
			game_control.main_loop_select_npc_idx=-1;
		}
		else
		{
			system_control.point_npc_idx=screen_mask;
			game_control.main_loop_select_npc_idx=screen_mask;
		}


	switch(game_control.main_loop_select_npc_task)
	{
	case MAP_MAIN_MENU_TASK_ADD_FRIEND:
		set_mouse_cursor(MOUSE_IMG_FRIEND);
		break;
	case MAP_MAIN_MENU_TASK_TEAM:
		set_mouse_cursor(MOUSE_IMG_TEAM);
		break;
	case MAP_MAIN_MENU_TASK_GIVE:
		set_mouse_cursor(MOUSE_IMG_GIVE);
		break;
	case MAP_MAIN_MENU_TASK_DEAL:
		set_mouse_cursor(MOUSE_IMG_DEAL);
		break;
	case MAP_MAIN_MENU_TASK_ATTACK:
		set_mouse_cursor(MOUSE_IMG_ATTACK);
		break;
	}
	
	switch(system_control.mouse_key)
	{
	case MS_RUp:
		game_control.main_loop_select_npc_task=-1;
		set_mouse_cursor(MOUSE_IMG_STAND);
		break;
	case MS_LDn:
		switch(game_control.main_loop_select_npc_task)
		{
		case MAP_MAIN_MENU_TASK_ADD_FRIEND:
			if(game_control.main_loop_select_npc_idx<0)break;
			if(game_control.main_loop_select_npc_idx==system_control.control_npc_idx)break;
			if(map_npc_group[game_control.main_loop_select_npc_idx].npc_type!=NPC_CHARACTER_TYPE)break;
			add_friend_list(map_npc_group[game_control.main_loop_select_npc_idx].npc_info.id,FRIEND_NORMAL);
			game_control.main_loop_select_npc_task=-1;
			set_mouse_cursor(MOUSE_IMG_STAND);
			break;
		case MAP_MAIN_MENU_TASK_TEAM:
			if(game_control.main_loop_select_npc_idx<0)break;
			if(map_npc_group[game_control.main_loop_select_npc_idx].npc_type!=NPC_CHARACTER_TYPE)break;
			exec_main_menu_task_team_action(map_npc_group[game_control.main_loop_select_npc_idx].npc_info.id);
			game_control.main_loop_select_npc_task=-1;
			set_mouse_cursor(MOUSE_IMG_STAND);
			break;
		case MAP_MAIN_MENU_TASK_GIVE:
			if(game_control.main_loop_select_npc_idx<0)break;
			if(game_control.main_loop_select_npc_idx==system_control.control_npc_idx)break;
			if(map_npc_group[game_control.main_loop_select_npc_idx].npc_type==NPC_BAOBAO_TYPE)break;
			game_control.game_give_id=map_npc_group[game_control.main_loop_select_npc_idx].npc_info.id;
			exec_main_menu_task_give();			
			game_control.main_loop_select_npc_task=-1;
			set_mouse_cursor(MOUSE_IMG_STAND);
			break;
		case MAP_MAIN_MENU_TASK_DEAL:
			if(game_control.main_loop_select_npc_idx<0)break;
			if(map_npc_group[game_control.main_loop_select_npc_idx].npc_type!=NPC_CHARACTER_TYPE)break;
			if(game_control.main_loop_select_npc_idx==system_control.control_npc_idx)break;
			game_control.game_deal_id=map_npc_group[game_control.main_loop_select_npc_idx].npc_info.id;
			exec_main_menu_task_deal();
			game_control.main_loop_select_npc_task=-1;
			set_mouse_cursor(MOUSE_IMG_STAND);
			break;
		case MAP_MAIN_MENU_TASK_ATTACK:
			if(game_control.main_loop_select_npc_idx<0)break;
			if(game_control.main_loop_select_npc_idx==system_control.control_npc_idx)break;
			if(map_npc_group[game_control.main_loop_select_npc_idx].npc_type==NPC_BAOBAO_TYPE)break;
			game_control.game_attack_id=map_npc_group[game_control.main_loop_select_npc_idx].npc_info.id;

// --- ���� attack 
			
			game_control.game_attack_id=0;		// ��� attack ID
			game_control.main_loop_select_npc_task=-1;
			set_mouse_cursor(MOUSE_IMG_STAND);
			change_to_fight();
			
			break;
		}


		break;
	}

		
	}	

	system_control.mouse_key=MS_Dummy;
	reset_mouse_key();
		

}




/*******************************************************************************************************************}
{	send message																									}
********************************************************************************************************************/
void window_draw_send_message(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	
	
	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
// --- ( 0 - 0 )  Draw body
	display_img256_buffer(window_x,window_y,system_image.chat_send_body.image,screen_buffer);
	

// --- ( 1 - 0 ) display data 
	print16(window_x+116+16,window_y+21+5,send_message_data.nickname,PEST_PUT,screen_buffer);


	s_FriendEdit.Paint(window_x, window_y, screen_buffer);	

}



void window_exec_send_message(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	SLONG x_offset,y_offset;
	RECT window_rect;
	SLONG dummy;
	SLONG task;
	SLONG channel;
	
	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;


	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;

	task=-1;
	if((x_offset>=251)&&(x_offset<251+63)&&
		(y_offset>=83)&&(y_offset<83+21))
	{
		task=0;					// ok
	} else if( (x_offset>=337)&&(x_offset<337+63)&&
		(y_offset>=83)&&(y_offset<83+21))
	{
		task=1;					// cancel
	} else if( (x_offset>=380)&&(x_offset<380+21)&&
			(y_offset>=26)&&(y_offset<26+21))
	{
		task=2;					// add
	}


	
//	s_FriendEdit.updateKey();
	
	switch(system_control.mouse_key)
	{
	case MS_RUp:						//	close 
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_send_message=-1;
		s_FriendEdit.Shutdown();
		break;
	case MS_Move:
	case MS_Dummy:
		if(task<0)break;
		switch(task)
		{
		case 0:
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand27",window_x+251,window_y+83,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand29",window_x+337,window_y+83,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand3",window_x+380,window_y+26,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		}
		break;
	case MS_LDn:
		s_FriendEdit.LBtnDown(system_control.mouse_x, system_control.mouse_y);		
		if(task<0)break;
		switch(task)
		{
		case 0:
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand28",window_x+251,window_y+83,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand30",window_x+337,window_y+83,screen_buffer,MDA_COMMAND_LAST,0,0,0);
			break;
		case 2:
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand4",window_x+380,window_y+26,screen_buffer,MDA_COMMAND_LAST,0,0,0);
			break;
		}
		break;
	case MS_LUp:
		switch(task)
		{
		case 0:
			s_FriendEdit.Send();
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			break;
		case 1:
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_send_message=-1;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			s_FriendEdit.Shutdown();			
			break;
		case 2:					// Ⱥ��
			exec_chat_group_send();
			break;
		}
		break;

	case MS_LDrag:				// �϶�
		switch(task)
		{
		case 0:
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand28",window_x+251,window_y+83,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand30",window_x+337,window_y+83,screen_buffer,MDA_COMMAND_LAST,0,0,0);
			break;
		case 2:
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand4",window_x+380,window_y+26,screen_buffer,MDA_COMMAND_LAST,0,0,0);
			break;
		default:
			now_window_move_xoffset=system_control.mouse_x-window_x;
			now_window_move_yoffset=system_control.mouse_y-window_y;
			now_window_move_handle=now_exec_window_handle;
			break;
		}
		break;
		
		
	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}



void check_message_in(void)
{
	if(game_control.chat_message_in>0)
	{
		if((system_control.system_timer/500)%2==0)
		{
			mda_group_exec(system_image.main_menu_handle,(UCHR *)"MainCommand45",
				system_image.button_map_main_menu[8].rect.left,
				system_image.button_map_main_menu[8].rect.top,
				screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			
		}
	}

	if(game_control.team_message_in>0)
	{
		if((system_control.system_timer/500)%2==0)
		{
			mda_group_exec(system_image.main_menu_handle,(UCHR *)"MainCommand31",
				system_image.button_map_main_menu[1].rect.left,
				system_image.button_map_main_menu[1].rect.top,
				screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			
		}
	}
	
}





/*******************************************************************************************************************}
{	receive message																									}
********************************************************************************************************************/
void window_draw_chat_receive(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	SLONG text_x,text_y;
	RECT window_rect;
	SLONG dummy;
	
	
	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	
// --- ( 0 - 0 )  Draw body
	display_img256_buffer(window_x,window_y,system_image.chat_receive_body.image,screen_buffer);	
	


// --- ( 1 - 0 ) display data
	print16(window_x+71,window_y+55,chat_receive_data.nickname,PEST_PUT,screen_buffer);

	sprintf((char *)print_rec,"%d",chat_receive_data.user_id);
	print16(window_x+239,window_y+55,print_rec,PEST_PUT,screen_buffer);

	print16(window_x+71,window_y+88,chat_receive_data.time,PEST_PUT,screen_buffer);
	
	text_x=window_x+30;
	text_y=window_y+122;
	text_out_data.g_pChat->updateString(text_x+4,text_y+4,272-8,136-8,
		(char *)chat_receive_data.text);
	text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,screen_buffer,false);
	
	
}



void window_exec_chat_receive(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	SLONG x_offset,y_offset;
	RECT window_rect;
	SLONG dummy;
	SLONG task;
	SLONG channel;
	
	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	
	task=-1;
	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;
	


	if((x_offset>=254)&&(x_offset<254+63)&&
		(y_offset>=274)&&(y_offset<274+21))
	{
		task=0;					// ok
	} else if( (x_offset>=295)&&(x_offset<295+28)&&
		(y_offset>=11)&&(y_offset<11+28))
	{
		task=1;					// close
	} else if( (x_offset>=223)&&(x_offset<223+96)&&
		(y_offset>=85)&&(y_offset<85+22))
	{
		task=2;					// next
	} else if( (x_offset>=132)&&(x_offset<132+96)&&
		  (y_offset>=273)&&(y_offset<273+22))
	{
		task=3;					// add friend
	} else if( (x_offset>=13)&&(x_offset<13+96)&&
		  (y_offset>=273)&&(y_offset<273+21))
	{
		task=4;					// temp
	}



	
	switch(system_control.mouse_key)
	{
	case MS_RUp:						//	close 
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_receive_message=-1;
		break;
	case MS_Move:
	case MS_Dummy:
		if(task<0)break;
		switch(task)
		{
		case 0:
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand49",window_x+254,window_y+274,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+295,window_y+11,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:
			if(game_control.chat_message_in>0)
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand47",window_x+223,window_y+85,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 3:
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand31",window_x+132,window_y+273,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 4:
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand33",window_x+13,window_y+273,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		}
		break;
	case MS_LDn:
		s_FriendEdit.LBtnDown(system_control.mouse_x, system_control.mouse_y);		
		if(task<0)break;
		switch(task)
		{
		case 0:
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand50",window_x+254,window_y+274,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+295,window_y+11,screen_buffer,MDA_COMMAND_LAST,0,0,0);
			break;
		case 2:
			if(game_control.chat_message_in>0)
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand48",window_x+223,window_y+85,screen_buffer,MDA_COMMAND_LAST,0,0,0);
			break;
		case 3:
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand32",window_x+132,window_y+273,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 4:
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand34",window_x+13,window_y+273,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		}
		break;

	case MS_LUp:
		switch(task)
		{
		case 0:					// OK �ظ�
			send_message_data.id=chat_receive_data.user_id;
			strcpy((char *)send_message_data.nickname,(char *)chat_receive_data.nickname);
			exec_send_message();
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_receive_message=-1;
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			break;
		case 1:					// close
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_receive_message=-1;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			break;
		case 2:					// next
			if(game_control.chat_message_in<=0)break;
			PostChatNetMessage(NET_MSG_CHAT_GET_MAIL, 0);
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			break;
		case 3:					// add friend
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			add_friend_list(chat_receive_data.user_id,FRIEND_NORMAL);
			break;
		case 4:					// add temp
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			add_friend_list(chat_receive_data.user_id,FRIEND_TEMP);
			break;
		}
		break;
			
	case MS_LDrag:				// �϶�
		switch(task)
		{
		case 0:
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand50",window_x+254,window_y+274,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+295,window_y+11,screen_buffer,MDA_COMMAND_LAST,0,0,0);
			break;
		case 2:
			if(game_control.chat_message_in>0)
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand48",window_x+223,window_y+85,screen_buffer,MDA_COMMAND_LAST,0,0,0);
			break;
		case 3:
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand32",window_x+132,window_y+273,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 4:
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand34",window_x+13,window_y+273,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		default:
			now_window_move_xoffset=system_control.mouse_x-window_x;
			now_window_move_yoffset=system_control.mouse_y-window_y;
			now_window_move_handle=now_exec_window_handle;
			break;
		}
		break;
		
	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}




/*******************************************************************************************************************}
{	Chat attrib																										}
********************************************************************************************************************/
void window_draw_chat_attrib(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	UCHR command_text[80];
	
	
	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	
// --- ( 0 - 0 )  Draw body
	display_img256_buffer(window_x,window_y,system_image.chat_attrib_body.image,screen_buffer);	
	

// --- ( 1 - 0 ) display data
	sprintf((char *)command_text,"MainCommand%d",send_message_data.base_type);
	mda_group_exec(system_image.character_head,command_text,window_x+264,window_y+54,screen_buffer,MDA_COMMAND_FIRST,0,0,0);
	
	sprintf((char *)print_rec,"%d",send_message_data.id);
	print16(window_x+73,window_y+64,print_rec,PEST_PUT,screen_buffer);

	sprintf((char *)print_rec,"%d",send_message_data.level);
	print16(window_x+210,window_y+64,print_rec,PEST_PUT,screen_buffer);

	print16(window_x+73,window_y+94,send_message_data.nickname,PEST_PUT,screen_buffer);

	print16(window_x+73,window_y+124,send_message_data.title,PEST_PUT,screen_buffer);

	print16(window_x+73,window_y+154,send_message_data.group,PEST_PUT,screen_buffer);

	print16(window_x+73,window_y+184,character_phyle_text[(send_message_data.base_type/2)%4],PEST_PUT,screen_buffer);

	sprintf((char *)print_rec,"%d",send_message_data.amity);
	print16(window_x+193,window_y+184,print_rec,PEST_PUT,screen_buffer);
	

	strcpy((char *)print_rec,(char *)get_nexus_string(send_message_data.nexus,send_message_data.amity));
	print16(window_x+73,window_y+214,print_rec,PEST_PUT,screen_buffer);



	
}



void window_exec_chat_attrib(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	SLONG task;
	SLONG x_offset,y_offset;
	SLONG i;
	SLONG channel;
	RECT rect[8]={
		{ 313,11, 313+28,11+28 	},				// �ر�
		{ 263,131,263+63,131+21	},				// �Ͻ�
		{ 263,161,263+63,161+21 },				// ����
		{ 263,191,263+63,191+21 },				// ˽��
		{ 263,221,263+63,221+21 },				// ����
		{ 244,250,244+96,250+22	},				// ��ʷ
		{ 128,250,128+96,250+22	},				// ����
		{  13,250, 13+96,250+22 }				// ��ʱ

	};
	

	
	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	
	task=-1;
	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;
	
	for(i=0;i<8;i++)
	{
		if((x_offset>=rect[i].left)&&(x_offset<rect[i].right)&&
			(y_offset>=rect[i].top)&&(y_offset<rect[i].bottom))
		{
			task=i;
			break;
		}
	}


	
	switch(system_control.mouse_key)
	{
	case MS_RUp:						//	close 
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_chat_attrib=-1;
		break;

	case MS_Move:
	case MS_Dummy:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// �Ͻ�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand37",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand39",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 3:		// ˽��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand35",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 4:		// ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand41",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 5:		// ��ʷ
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand43",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 6:		// ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand31",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 7:		// ��ʱ
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand33",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		}
		break;

	case MS_LDn:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// �Ͻ�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand38",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand40",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 3:		// ˽��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand36",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 4:		// ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand42",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 5:		// ��ʷ
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand44",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 6:		// ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand32",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 7:		// ��ʱ
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand34",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		}
		
		break;
			
	case MS_LUp:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_chat_attrib=-1;
			break;
		case 1:		// �Ͻ�
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			postNetMessage(NET_MSG_PLAYER_FRIEND_SUB, send_message_data.id);
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_chat_attrib=-1;
			break;
		case 2:		// ����
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			if(send_message_data.friend_type==FRIEND_MASK)break;
			change_friend_list_friend_type(send_message_data.id,FRIEND_MASK);
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_chat_attrib=-1;
			break;
		case 3:		// ˽��
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			if(send_message_data.id==system_control.control_user_id)break;
			system_control.person_npc_id=send_message_data.id;
			sprintf((char *)print_rec,MSG_SET_PERSON_ID,send_message_data.nickname,send_message_data.id);
			push_chat_data(CHAT_CHANNEL_MESSGAE,0,(UCHR *)"",print_rec);
			strcpy((char *)system_control.person_name,(char *)send_message_data.nickname);
			game_control.chat_channel=CHAT_CHANNEL_PERSON;
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_chat_attrib=-1;
			break;
		case 4:		// ����
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			postNetMessage(NET_MSG_PLAYER_FRIEND_UPDATE, send_message_data.id);
			break;
		case 5:		// ��ʷ
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			exec_friend_hirstory();
			break;
		case 6:		// ����
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			if(send_message_data.friend_type==FRIEND_NORMAL)break;
			change_friend_list_friend_type(send_message_data.id,FRIEND_NORMAL);
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_chat_attrib=-1;
			break;
		case 7:		// ��ʱ
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			if(send_message_data.friend_type==FRIEND_TEMP)break;
			change_friend_list_friend_type(send_message_data.id,FRIEND_TEMP);
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_chat_attrib=-1;
			break;
		}
		
		break;
			

	case MS_LDrag:				// �϶�
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// �Ͻ�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand38",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand40",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 3:		// ˽��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand36",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 4:		// ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand42",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 5:		// ��ʷ
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand44",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 6:		// ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand32",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 7:		// ��ʱ
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand34",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		default:
			now_window_move_xoffset=system_control.mouse_x-window_x;
			now_window_move_yoffset=system_control.mouse_y-window_y;
			now_window_move_handle=now_exec_window_handle;
			break;
		}
		
		break;
		
	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}



/*******************************************************************************************************************}
{	group send																										}
********************************************************************************************************************/
void window_draw_chat_group_send(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	SLONG i;
	SLONG idx;
	SLONG x_offset,y_offset;
	
	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	
// --- ( 0 - 0 )  Draw body
	display_img256_buffer(window_x,window_y,system_image.chat_group_send_body.image,screen_buffer);
	
// --- ( 1 - 0 ) display data 

	for(i=0;i<8;i++)
	{
		idx=chat_group_send_start_idx+i;
		if(display_friend_list[idx].level<0)continue;
		x_offset=window_x+19;
		y_offset=window_y+51+i*22;

		if((system_control.mouse_x>=x_offset)&&(system_control.mouse_x<x_offset+139)&&
			(system_control.mouse_y>=y_offset)&&(system_control.mouse_y<y_offset+18))
		{
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand0",
				x_offset,
				y_offset,
				screen_buffer,MDA_COMMAND_ASSIGN,0,0,0);
			
		}
		
		if(chat_group_send_flag[idx])
			alpha_put_bar(x_offset,y_offset,139,18,color_control.pink,screen_buffer,128);
		
		if(display_friend_list[idx].online==1)
			set_word_color(0,color_control.green);
		else
			set_word_color(0,color_control.gray);

		print16(x_offset+8,y_offset+1,display_friend_list[idx].nickname,PEST_PUT,screen_buffer);
		set_word_color(0,color_control.white);

	}


	if(game_control.window_send_message<0)
	{
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_chat_group_send=-1;
	}
	
}



void window_exec_chat_group_send(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	SLONG task;
	SLONG x_offset,y_offset;
	SLONG i;
	SLONG idx;
	SLONG channel;
	SLONG function;
	RECT rect[4]={
		{ 137,10 ,137+28,10+28 	},				// �ر�
		{  83,233, 83+63,233+21	},				// ȷ��
		{  19,232, 19+21,232+21 },				// �Ϸ�
		{  43,232, 43+21,232+21 }				// �·�
		
	};
	
	
	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	

	task=-1;
	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;
	
	for(i=0;i<4;i++)
	{
		if((x_offset>=rect[i].left)&&(x_offset<rect[i].right)&&
			(y_offset>=rect[i].top)&&(y_offset<rect[i].bottom))
		{
			task=i;
			break;
		}
	}
	

	function=-1;
	for(i=0;i<8;i++)
	{
		idx=chat_group_send_start_idx+i;
		if(display_friend_list[idx].level<0)continue;
		x_offset=window_x+19;
		y_offset=window_y+51+i*22;
		
		if((system_control.mouse_x>=x_offset)&&(system_control.mouse_x<x_offset+139)&&
			(system_control.mouse_y>=y_offset)&&(system_control.mouse_y<y_offset+18))
		{
			task=100;
			function=idx;
			i=8;
		}
		
	}
	

	
	switch(system_control.mouse_key)
	{
	case MS_RUp:						//	close 
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_chat_group_send=-1;
		break;
	case MS_Move:
	case MS_Dummy:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// ȷ��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand27",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// �Ϸ�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand7",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 3:		// �·�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand9",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		}
		break;
		
	case MS_LDn:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// ȷ��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand28",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// �Ϸ�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand8",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 3:		// �·�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand10",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		}
		break;
	case MS_LUp:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_chat_group_send=-1;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			break;
		case 1:		// ȷ��
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			if(s_FriendEdit.getCharNum()<=0)
			{
				display_system_message((UCHR *)MSG_NO_MESSAGE);
				break;
			}
			chat_group_send_message();
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_chat_group_send=-1;
			break;
		case 2:		// �Ϸ�
			auto_play_voice(0,(UCHR *)"EFC\\button02.wav",NULL,0,0,0,0,&channel);
			if(chat_group_send_start_idx<=0)
				break;
			chat_group_send_start_idx--;
			break;
		case 3:		// �·�
			auto_play_voice(0,(UCHR *)"EFC\\button02.wav",NULL,0,0,0,0,&channel);
			if(chat_group_send_start_idx<game_control.window_chat_friend_end_idx-8)
				chat_group_send_start_idx++;	
			break;
		case 100:
			if(function<0)break;
			chat_group_send_flag[function]=!chat_group_send_flag[function];
			if(check_chat_group_send_max()!=TTN_OK)
				chat_group_send_flag[function]=false;
			break;
		}
		
		break;
			

	case MS_LDrag:				// �϶�
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// ȷ��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand28",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// �Ϸ�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand8",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 3:		// �·�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand10",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		default:
			now_window_move_xoffset=system_control.mouse_x-window_x;
			now_window_move_yoffset=system_control.mouse_y-window_y;
			now_window_move_handle=now_exec_window_handle;
			break;
		}
		break;
		
	}

	
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}




void chat_group_send_message(void)
{
	SLONG i;
	UCHR text[1024];

	
	s_FriendEdit.getText((char *)text);

	if (!*text)
	{
		return;
	}

	for(i=0;i<MAX_CHAT_FRIEND_LIST;i++)
	{
		if((chat_group_send_flag[i])&&(display_friend_list[i].level>=0))
		{
			FriendChat((char *)text, system_control.control_user_id,display_friend_list[i].id);
		}
	}

}



/*******************************************************************************************************************}
{	friend history																									}
********************************************************************************************************************/
void window_draw_chat_hirstory(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	
	
	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	
// --- ( 0 - 0 )  Draw body
	display_img256_buffer(window_x,window_y,system_image.history_body.image,screen_buffer);	
	display_img256_buffer(window_x+22,window_y+46,system_image.chat_hirstory_mask.image,screen_buffer);	
	
	
// --- ( 1 - 0 ) show data 
	display_friend_data_record_window();
}



void window_exec_chat_hirstory(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	SLONG task;
	SLONG x_offset,y_offset;
	SLONG i;
	SLONG channel;
	RECT rect[3]={
		{ 406,10 ,406+28,10+28 	},				// �ر�
		{ 375,286,375+21,286+21 },				// �Ϸ�
		{ 400,286,400+21,286+21 }				// �·�
		
	};
	

		

	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	task=-1;
	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;
	
	for(i=0;i<3;i++)
	{
		if((x_offset>=rect[i].left)&&(x_offset<rect[i].right)&&
			(y_offset>=rect[i].top)&&(y_offset<rect[i].bottom))
		{
			task=i;
			break;
		}
	}
	
	
	
	
	switch(system_control.mouse_key)
	{
	case MS_RUp:						//	close 
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_chat_hirstory=-1;
		break;
	case MS_Move:
	case MS_Dummy:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// �Ϸ�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand7",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// �·�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand9",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
			
		}
		break;
	case MS_LDn:
	case MS_LDrag:				// �϶�
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// �Ϸ�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand8",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// �·�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand10",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
			
		}

		break;			

	case MS_LUp:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_chat_hirstory=-1;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			break;
		case 1:		// �Ϸ�
			auto_play_voice(0,(UCHR *)"EFC\\button02.wav",NULL,0,0,0,0,&channel);
			scroll_friend_data_record(Up);
			break;
		case 2:		// �·�
			auto_play_voice(0,(UCHR *)"EFC\\button02.wav",NULL,0,0,0,0,&channel);
			scroll_friend_data_record(Dn);
			break;
		}
		
		break;
			

		
	}
	

	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}



/*******************************************************************************************************************}
{	TEAM																											}
********************************************************************************************************************/
void window_draw_team(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	SLONG x_offset,y_offset;
	RECT window_rect;
	SLONG dummy;
	SLONG i;
	
	
	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;


	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;
	
	
	// --- ( 0 - 0 )  Draw body
	display_img256_buffer(window_x,window_y,system_image.team_body.image,screen_buffer);	
	display_img256_buffer(window_x+120,window_y+12,system_image.team_list_mark.image,screen_buffer);
	

	mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand51",window_x+24,window_y+188,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
	mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand54",window_x+125,window_y+188,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
	mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand57",window_x+223,window_y+188,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);

	system_palette=gray_palette;
	mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand7",window_x+285,window_y+126,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_PALETTE,0,0,0);
	mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand9",window_x+285,window_y+153,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_PALETTE,0,0,0);
	
	

	// --- ( 0 - 1 ) display data 
	for(i=0;i<MAX_TEAM_PLAYER;i++)
	{
		if(!(team_list_data[i].char_id&NPC_ID_MASK))
		{


			if((x_offset>=17)&&(x_offset<17+252)&&
				(y_offset>=71+i*18)&&(y_offset<71+i*18+18))
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand0",window_x+17,window_y+71+i*18,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);

			if(i==team_select_index)
				alpha_put_bar(window_x+17,window_y+71+i*18,139,18,color_control.pink,screen_buffer,128);
			
			
			print16(window_x+17+8,window_y+71+1+i*18,team_list_data[i].nickname,PEST_PUT,screen_buffer);

			if(team_list_data[i].sex%2==0)
				strcpy((char *)print_rec,CHARACTER_SEX_MAN_TEXT);
			else
				strcpy((char *)print_rec,CHARACTER_SEX_WOMAN_TEXT);
			
			switch(team_list_data[i].phyle)
			{
			case 0:
				strcat((char *)print_rec,CHARACTER_PHYLE_HUMAN_TEXT);
				break;
			case 1:
				strcat((char *)print_rec,CHARACTER_PHYLE_GENUS_TEXT);
				break;
			case 2:
				strcat((char *)print_rec,CHARACTER_PHYLE_MAGIC_TEXT);
				break;
			case 3:
				strcat((char *)print_rec,CHARACTER_PHYLE_ROBET_TEXT);
				break;
			}
		
			print16(window_x+168,window_y+71+i*18,print_rec,PEST_PUT,screen_buffer);
			
			sprintf((char *)print_rec,"%d",team_list_data[i].level);
			print16(window_x+239,window_y+71+i*18,print_rec,PEST_PUT,screen_buffer);

		}
	}

}



void window_exec_team(void)
{
	int status;
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;

	SLONG task;
	SLONG x_offset,y_offset;
	SLONG i;
	SLONG channel;
	SLONG idx;
	bool data_ok;
	RECT add_rect;
	
	RECT rect[6]={
		{ 291,7  ,291+28, 7+28	},				// �ر�
		{ 285,126,285+21,126+21 },				// �Ϸ�
		{ 285,153,285+21,153+21 },				// �·�
		{  24,188, 24+74,188+23 },				// �����б�
		{ 125,188,125+74,188+23 },				// �߳�����
		{ 223,188,223+74,188+23 }				// �뿪����
		
	};
	
	
	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	

		
	task=-1;
	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;
	
	for(i=0;i<6;i++)
	{
		if((x_offset>=rect[i].left)&&(x_offset<rect[i].right)&&
			(y_offset>=rect[i].top)&&(y_offset<rect[i].bottom))
		{
			task=i;
			break;
		}
	}

	for(i=0;i<5;i++)
	{
		if((x_offset>=17)&&(x_offset<17+252)&&
			(y_offset>=71+i*18)&&(y_offset<71+i*18+18))
		{
			task=10+i;
		}
	}
	
	if(team_select_index>=0)
		data_ok=true;
	else
		data_ok=false;
				

	status=map_npc_group[system_control.control_npc_idx].npc_info.status;

	
	switch(system_control.mouse_key)
	{
	case MS_RUp:						//	close 
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_team=-1;
		break;

	case MS_Move:
	case MS_Dummy:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
//		case 1:		// �Ϸ�
//			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand7",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
//			break;
//		case 2:		// �·�
//			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand9",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
//			break;
		case 3:		// �����б�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand52",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 4:		// �߳�����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand55",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 5:		// �뿪����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand58",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
			
		}
		break;

	case MS_LUp:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_team=-1;
			break;
//		case 1:		// �Ϸ�
//			break;
//		case 2:		// �·�
//			break;
		case 3:		// �����б�
			if(status&NPC_STATUS_FOLLOW)					// ����Ƕ�Աû���б�
				break;
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			if(game_control.window_team_request>=0)					// already open
				break;
			add_rect.left=system_image.team_body.rect.left+40;
			add_rect.top=system_image.team_body.rect.top+40;
			add_rect.right=system_image.team_body.rect.right+40;
			add_rect.bottom=system_image.team_body.rect.bottom+40;
			game_control.window_team_request=insert_window_base(add_rect ,&window_draw_team_request,&window_exec_team_request,WINDOW_TYPE_NORMAL);	
			game_control.team_message_in=0;
			break;
		case 4:		// �߳�����
			if(!data_ok)break;
			if(status&NPC_STATUS_FOLLOW)					// ����Ƕ�Ա����T��
				break;
			if((ULONG)team_list_data[team_select_index].char_id==system_control.control_user_id)break;
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			player_leave_team(team_list_data[team_select_index].char_id);
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_team=-1;
			break;
		case 5:		// �뿪����
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			player_leave_team(system_control.control_user_id);
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_team=-1;
			break;

		case 10: case 11: case 12:case 13:case 14:			// select
			idx=task-10;
			if(!(team_list_data[idx].char_id&NPC_ID_MASK))	// ������
				team_select_index=idx;
			break;

		}
		
		break;	

	case MS_LDrag:				// �϶�
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
//		case 1:		// �Ϸ�
//			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand8",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
//			break;
//		case 2:		// �·�
//			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand10",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
//			break;
		case 3:		// �����б�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand53",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 4:		// �߳�����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand56",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 5:		// �뿪����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand59",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		default:
			now_window_move_xoffset=system_control.mouse_x-window_x;
			now_window_move_yoffset=system_control.mouse_y-window_y;
			now_window_move_handle=now_exec_window_handle;
			break;
			
		}
		
		break;
		
	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}




/*******************************************************************************************************************}
{	chat search																										}
********************************************************************************************************************/
void window_draw_chat_search(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	
	
	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	
	// --- ( 0 - 0 )  Draw body
	display_img256_buffer(window_x,window_y,system_image.chat_search.image,screen_buffer);	
	
	g_FindIDEdit.Paint(window_x, window_y, screen_buffer);
	g_FindNickNameEdit.Paint(window_x, window_y, screen_buffer);

	// --- ( 1 - 0 ) show data 
	if(!(chat_search_data.char_id&NPC_ID_MASK))		// ������
	{
		sprintf((char *)print_rec,MSG_PLAYER_INFO_ID,chat_search_data.char_id);
		print16(window_x+30,window_y+124,print_rec,PEST_PUT,screen_buffer);
		sprintf((char *)print_rec,MSG_PLAYER_INFO_NAME,chat_search_data.nickname);
		print16(window_x+30,window_y+124+16,print_rec,PEST_PUT,screen_buffer);
		sprintf((char *)print_rec,MSG_PLAYER_INFO_LEVEL,chat_search_data.level);
		print16(window_x+30,window_y+124+16*2,print_rec,PEST_PUT,screen_buffer);

	
		strcpy((char *)print_rec,"");
		switch(chat_search_data.phyle)
		{
		case 0:
			sprintf((char *)print_rec,MSG_PLAYER_INFO_PHYLE,CHARACTER_PHYLE_HUMAN_TEXT);
			break;
		case 1:
			sprintf((char *)print_rec,MSG_PLAYER_INFO_PHYLE,CHARACTER_PHYLE_GENUS_TEXT);
			break;
		case 2:
			sprintf((char *)print_rec,MSG_PLAYER_INFO_PHYLE,CHARACTER_PHYLE_MAGIC_TEXT);
			break;
		case 3:
			sprintf((char *)print_rec,MSG_PLAYER_INFO_PHYLE,CHARACTER_PHYLE_ROBET_TEXT);
			break;
		}

		print16(window_x+30,window_y+124+16*3,print_rec,PEST_PUT,screen_buffer);

		
		if(chat_search_data.sex%2==0)
			sprintf((char *)print_rec,MSG_PLAYER_INFO_SEX,CHARACTER_SEX_MAN_TEXT);
		else
			sprintf((char *)print_rec,MSG_PLAYER_INFO_SEX,CHARACTER_SEX_WOMAN_TEXT);
		print16(window_x+30,window_y+124+16*4,print_rec,PEST_PUT,screen_buffer);

			
	}
	
}

void window_exec_chat_search(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	SLONG task;
	SLONG x_offset,y_offset;
	SLONG i;
	SLONG channel;
	bool data_ok;

	RECT rect[5]={
		{ 295,11 ,295+28, 11+28	},				// �ر�
		{ 239,70 ,239+63, 70+21 },				// ȷ��
		{ 255,239,255+63,239+21 },				// ˽��
		{ 130,238,130+96,238+22 },				// ��Ϊ����
		{  14,238, 14+96,238+22 }				// ��ʱ����
		
	};


	
	
	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	

	task=-1;

	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;

	for(i=0;i<5;i++)
	{
		if((x_offset>=rect[i].left)&&(x_offset<rect[i].right)&&
			(y_offset>=rect[i].top)&&(y_offset<rect[i].bottom))
		{
			task=i;
			break;
		}
	}
	
	if(!(chat_search_data.char_id&NPC_ID_MASK))		// ������
		data_ok=true;
	else
		data_ok=false;
		
	
	switch(system_control.mouse_key)
	{
	case MS_LDn:
		if (g_FindIDEdit.LBtnDown(system_control.mouse_x, system_control.mouse_y) != 1)
		{
			if (g_FindNickNameEdit.LBtnDown(system_control.mouse_x, system_control.mouse_y) != -1)
			{
				g_FindNickNameEdit.setFocus();
			}
		}
		break;

	case MS_RUp:						//	close 
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_chat_search=-1;
		g_FindIDEdit.Shutdown();
		g_FindNickNameEdit.Shutdown();
		break;

	case MS_Move:
	case MS_Dummy:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// ȷ��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand27",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// ˽��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand35",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 3:		// ��Ϊ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand31",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 4:		// ��ʱ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand33",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
			
		}
		break;

	case MS_LUp:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_chat_search=-1;
			g_FindIDEdit.Shutdown();
			g_FindNickNameEdit.Shutdown();
			break;
		case 1:		// ȷ��
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			if (g_FindIDEdit.getFocus())
			{
				g_FindIDEdit.OnEnter();
			}
			else if (g_FindNickNameEdit.getFocus())
			{
				g_FindNickNameEdit.OnEnter();
			}
			break;
		case 2:		// ˽��
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			if(!data_ok)break;
			if((ULONG)chat_search_data.char_id==system_control.control_user_id)break;
			system_control.person_npc_id=chat_search_data.char_id;
			sprintf((char *)print_rec,MSG_SET_PERSON_ID,chat_search_data.nickname,chat_search_data.char_id);
			push_chat_data(CHAT_CHANNEL_MESSGAE,0,(UCHR *)"",print_rec);
			strcpy((char *)system_control.person_name,(char *)chat_search_data.nickname);
			break;
		case 3:		// ��Ϊ����
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			if(!data_ok)break;
			add_friend_list(chat_search_data.char_id,FRIEND_NORMAL);
			
			break;
		case 4:		// ��ʱ����
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			if(!data_ok)break;
			add_friend_list(chat_search_data.char_id,FRIEND_NORMAL);
			change_friend_list_friend_type(chat_search_data.char_id,FRIEND_TEMP);
			break;
			
		}
		break;


	case MS_LDrag:						// �϶�
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// ȷ��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand28",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// ˽��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand36",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 3:		// ��Ϊ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand32",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 4:		// ��ʱ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand34",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		default:
			now_window_move_xoffset=system_control.mouse_x-window_x;
			now_window_move_yoffset=system_control.mouse_y-window_y;
			now_window_move_handle=now_exec_window_handle;
			break;
			
		}
		
		break;
		
	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}





/*******************************************************************************************************************}
{	TEAM REQUEST																									}
********************************************************************************************************************/
void window_draw_team_request(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	SLONG x_offset,y_offset;
	RECT window_rect;
	SLONG dummy;
	SLONG i,idx;
	
	
	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;


	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;
	
	
	// --- ( 0 - 0 )  Draw body
	display_img256_buffer(window_x,window_y,system_image.team_body.image,screen_buffer);	
	display_img256_buffer(window_x+120,window_y+12,system_image.team_list_request_mark.image,screen_buffer);
	

	mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand60",window_x+34,window_y+192,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
	mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand62",window_x+127,window_y+192,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
	mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand87",window_x+219,window_y+192,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
	

// --- ( 0 - 1 ) display data 

	for(i=0;i<6;i++)
	{
		idx=team_request_select_start_index+i;
		if(idx>=MAX_TEAM_REQUEST)continue;

		if(!(team_request_list_data[idx].char_id&NPC_ID_MASK))
		{

			if((x_offset>=17)&&(x_offset<17+252)&&
				(y_offset>=71+i*18)&&(y_offset<71+i*18+18))
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand0",window_x+17,window_y+71+i*18,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);

			if(i==team_request_select_index)
				alpha_put_bar(window_x+17,window_y+71+i*18,139,18,color_control.pink,screen_buffer,128);
			
			
			print16(window_x+17+8,window_y+71+1+i*18,team_request_list_data[idx].nickname,PEST_PUT,screen_buffer);

			if(team_request_list_data[idx].sex%2==0)
				strcpy((char *)print_rec,CHARACTER_SEX_MAN_TEXT);
			else
				strcpy((char *)print_rec,CHARACTER_SEX_WOMAN_TEXT);
			
			switch(team_request_list_data[idx].phyle)
			{
			case 0:
				strcat((char *)print_rec,CHARACTER_PHYLE_HUMAN_TEXT);
				break;
			case 1:
				strcat((char *)print_rec,CHARACTER_PHYLE_GENUS_TEXT);
				break;
			case 2:
				strcat((char *)print_rec,CHARACTER_PHYLE_MAGIC_TEXT);
				break;
			case 3:
				strcat((char *)print_rec,CHARACTER_PHYLE_ROBET_TEXT);
				break;
			}
		
			print16(window_x+168,window_y+71+i*18,print_rec,PEST_PUT,screen_buffer);
			
			sprintf((char *)print_rec,"%d",team_request_list_data[idx].level);
			print16(window_x+239,window_y+71+i*18,print_rec,PEST_PUT,screen_buffer);

		}
	}

}



void window_exec_team_request(void)
{
	int status;
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;

	SLONG task;
	SLONG x_offset,y_offset;
	SLONG i;
	SLONG channel;
	SLONG idx;
	bool data_ok;
	
	RECT rect[6]={
		{ 291,7  ,291+28, 7+28	},				// �ر�
		{ 285,126,285+21,126+21 },				// �Ϸ�
		{ 285,153,285+21,153+21 },				// �·�
		{  34,192, 34+57,192+15 },				// ȷ��
		{ 127,192,127+57,192+15 },				// ɾ��
		{ 219,192,219+74,192+23 }				// ���
		
	};
	
	
	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	

		
	task=-1;
	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;
	
	for(i=0;i<6;i++)
	{
		if((x_offset>=rect[i].left)&&(x_offset<rect[i].right)&&
			(y_offset>=rect[i].top)&&(y_offset<rect[i].bottom))
		{
			task=i;
			break;
		}
	}

	for(i=0;i<6;i++)
	{
		if((x_offset>=17)&&(x_offset<17+252)&&
			(y_offset>=71+i*18)&&(y_offset<71+i*18+18))
		{
			task=10+i;
		}
	}

	data_ok=false;
	if(team_request_select_index>=0)
	{
		if(!(team_request_list_data[team_request_select_index+team_request_select_start_index].char_id&NPC_ID_MASK))	// ������
			data_ok=true;
		else
			team_request_select_index=-1;
	}
				

	status=map_npc_group[system_control.control_npc_idx].npc_info.status;

	
	switch(system_control.mouse_key)
	{
	case MS_RUp:						//	close 
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_team_request=-1;
		break;

	case MS_Move:
	case MS_Dummy:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// �Ϸ�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand7",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// �·�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand9",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 3:		// ȷ��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand27",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 4:		// ɾ��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand63",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 5:		// ���
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand88",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
			
		}
		break;

	case MS_LUp:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_team_request=-1;
			break;
		case 1:		// �Ϸ�
			if(team_request_select_start_index>0)
				team_request_select_start_index--;
			auto_play_voice(0,(UCHR *)"EFC\\button02.wav",NULL,0,0,0,0,&channel);
			break;
		case 2:		// �·�
			if(team_request_select_start_index<MAX_TEAM_REQUEST-6)
				team_request_select_start_index++;
			auto_play_voice(0,(UCHR *)"EFC\\button02.wav",NULL,0,0,0,0,&channel);
			break;
		case 3:		// ȷ��
			if(!data_ok)break;
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			if(!(team_request_list_data[team_request_select_index+team_request_select_start_index].char_id&NPC_ID_MASK))	// ������
			{
				team_request_add_list(team_request_list_data[team_request_select_index+team_request_select_start_index].char_id);
			}
				
			break;
		case 4:		// ɾ��
			if(!data_ok)break;
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			if(!(team_request_list_data[team_request_select_index+team_request_select_start_index].char_id&NPC_ID_MASK))	// ������
			{
				team_request_delete_list(team_request_list_data[team_request_select_index+team_request_select_start_index].char_id);
			}
			break;
		case 5:		// ���
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			postNetMessage(NET_MSG_PLAYER_CLEAR_REQUEST_LIST, 0);
			break;

		case 10: case 11: case 12:case 13:case 14: case 15:			// select
			idx=task-10;
			if(!(team_request_list_data[idx+team_request_select_start_index].char_id&NPC_ID_MASK))	// ������
				team_request_select_index=idx;
			
			break;

		}
		
		break;	

	case MS_LDrag:				// �϶�
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// �Ϸ�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand8",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// �·�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand10",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 3:		// ȷ��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand28",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 4:		// ɾ��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand64",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 5:		// ���
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand89",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		default:
			now_window_move_xoffset=system_control.mouse_x-window_x;
			now_window_move_yoffset=system_control.mouse_y-window_y;
			now_window_move_handle=now_exec_window_handle;
			break;
			
		}
		
		break;
		
	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}




/*******************************************************************************************************************}
{	SMALL MAP																										}
********************************************************************************************************************/
void window_draw_small_map(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	SLONG x_offset,y_offset;
	RECT window_rect;
	SLONG dummy;
	SLONG yoff;
	static UCHR r=250;
	static bool dir=false;
	UHINT color;
	float npc_x,npc_y;
	float npc_xoff,npc_yoff;
	
	
	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	color=rgb2hi(r,0,255);
	if(!dir)
	{
		r-=8;
		if(r<10)
			dir=!dir;
	}
	else
	{
		r+=8;
		if(r>240)
			dir=!dir;
	}

	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;

	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;
	
	yoff=(window_yl-SMALL_MAP_HEIGHT)/2;
	
// --- ( 0 - 0 )  Draw body
	draw_message_box(window_x,window_y,window_xl,window_yl,screen_buffer);
	put_bitmap(window_x+4,window_y+yoff,small_map_data.image_buffer,screen_buffer);
	
// --- ( 0 - 1 ) display data 
	npc_x=(float)map_npc_group[system_control.control_npc_idx].npc_info.x;
	npc_y=(float)map_npc_group[system_control.control_npc_idx].npc_info.y;

	npc_xoff=npc_x*small_map_data.x_percent;
	npc_yoff=npc_y*small_map_data.y_percent;

	put_bar(window_x+npc_xoff-3,window_y+npc_yoff-3,7,7,color,screen_buffer);
	put_scroll_box(window_x+npc_xoff-4,window_y+npc_yoff-4,window_x+npc_xoff+4,window_y+npc_yoff+4,color_control.red,color_control.yellow,screen_buffer);
	put_xcross(window_x+npc_xoff,window_y+npc_yoff,color_control.green,screen_buffer);
	
}



void window_exec_small_map(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	
	
	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	

		
	
	switch(system_control.mouse_key)
	{
	case MS_RUp:						//	close 
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_small_map=-1;
		break;

	case MS_LDrag:				// �϶�
		now_window_move_xoffset=system_control.mouse_x-window_x;
		now_window_move_yoffset=system_control.mouse_y-window_y;
		now_window_move_handle=now_exec_window_handle;
		break;
		
	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}



/*******************************************************************************************************************}
{	WORLD MAP																										}
********************************************************************************************************************/
void window_draw_world_map(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;


	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;


// --- ( 0 - 0 )  Draw body
	get_bitmap(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,screen_buffer,screen_channel0);
	
	


}



void window_exec_world_map(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;

	SLONG task;
	SLONG x_offset,y_offset;
	SLONG i;
	SLONG channel;
	

	RECT rect[1]={
		{ 499,0  ,499+28, 0+28	}				// �ر�
	};
	
	
	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	

	task=-1;
	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;
	
	for(i=0;i<1;i++)
	{
		if((x_offset>=rect[i].left)&&(x_offset<rect[i].right)&&
			(y_offset>=rect[i].top)&&(y_offset<rect[i].bottom))
		{
			task=i;
			break;
		}
	}
	
	
	
	switch(system_control.mouse_key)
	{
	case MS_RUp:						//	close 
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_world_map=-1;
		break;

	case MS_Move:
	case MS_Dummy:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		}
		break;

	case MS_LUp:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_world_map=-1;
			break;
		}
		break;	
	case MS_LDrag:				// �϶�
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		}

	
	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}







/*******************************************************************************************************************}
{	give																											}
********************************************************************************************************************/
void window_draw_give(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	SLONG i;
	SLONG idx;
	SLONG inst_x,inst_y,inst_xl,inst_yl;
	RECT test_rect;
	
	RECT item_pp[MAX_POCKET_ITEM]={
		{ 26+40*0, 56+40*0, 26+40*0+32, 56+40*0+32 },
		{ 26+40*1, 56+40*0, 26+40*1+32, 56+40*0+32 },
		{ 26+40*2, 56+40*0, 26+40*2+32, 56+40*0+32 },
		{ 26+40*3, 56+40*0, 26+40*3+32, 56+40*0+32 },
		{ 26+40*4, 56+40*0, 26+40*4+32, 56+40*0+32 },
		{ 26+40*5, 56+40*0, 26+40*5+32, 56+40*0+32 },
		
		{ 26+40*0, 56+40*1, 26+40*0+32, 56+40*1+32 },
		{ 26+40*1, 56+40*1, 26+40*1+32, 56+40*1+32 },
		{ 26+40*2, 56+40*1, 26+40*2+32, 56+40*1+32 },
		{ 26+40*3, 56+40*1, 26+40*3+32, 56+40*1+32 },
		{ 26+40*4, 56+40*1, 26+40*4+32, 56+40*1+32 },
		{ 26+40*5, 56+40*1, 26+40*5+32, 56+40*1+32 },
		
		{ 26+40*0, 56+40*2, 26+40*0+32, 56+40*2+32 },
		{ 26+40*1, 56+40*2, 26+40*1+32, 56+40*2+32 },
		{ 26+40*2, 56+40*2, 26+40*2+32, 56+40*2+32 },
		{ 26+40*3, 56+40*2, 26+40*3+32, 56+40*2+32 },
		{ 26+40*4, 56+40*2, 26+40*4+32, 56+40*2+32 },
		{ 26+40*5, 56+40*2, 26+40*5+32, 56+40*2+32 },
		
		{ 26+40*0, 56+40*3, 26+40*0+32, 56+40*3+32 },
		{ 26+40*1, 56+40*3, 26+40*1+32, 56+40*3+32 },
		{ 26+40*2, 56+40*3, 26+40*2+32, 56+40*3+32 },
		{ 26+40*3, 56+40*3, 26+40*3+32, 56+40*3+32 },
		{ 26+40*4, 56+40*3, 26+40*4+32, 56+40*3+32 },
		{ 26+40*5, 56+40*3, 26+40*5+32, 56+40*3+32 }
		
	};
	
	
	
	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	
// --- ( 0 - 0 )  Draw body
	display_img256_buffer(window_x,window_y,system_image.item_public_body.image,screen_buffer);	
	display_img256_buffer(window_x+2,window_y+(system_image.item_public_body.rect.bottom-system_image.item_public_body.rect.top),system_image.item_action_body.image,screen_buffer);	
	display_img256_buffer(window_x+107,window_y+14,system_image.give_body.image,screen_buffer);	


// --- ( 0 - 1 ) �ڴ� item
	if(character_pocket_data.active)
	{
		for(i=0;i<MAX_POCKET_ITEM;i++)
		{
			
			if((character_pocket_data.item[i].number>0)&&(character_pocket_data.item[i].filename_id>=0))
			{
				
				display_img256_buffer(window_x+item_pp[i].left,window_y+item_pp[i].top,
					system_image.item_image[character_pocket_data.item[i].filename_id],screen_buffer);
				
				if(character_pocket_data.item[i].number>1)
				{
					sprintf((char *)print_rec,"%d",character_pocket_data.item[i].number);
					print12(window_x+item_pp[i].right-(strlen((char *)print_rec)*6)-6-1,window_y+item_pp[i].bottom-10-1,print_rec,COPY_PUT,screen_buffer);
					
				}
				
			}
			
			if(i==character_pocket_data.idx)
			{
				put_scroll_box(window_x+item_pp[i].left-1,
					window_y+item_pp[i].top-1,
					window_x+item_pp[i].right+1,
					window_y+item_pp[i].bottom+1,color_control.red,color_control.yellow,screen_buffer);
				put_scroll_box(window_x+item_pp[i].left-2,
					window_y+item_pp[i].top-2,
					window_x+item_pp[i].right+2,
					window_y+item_pp[i].bottom+2,color_control.red,color_control.yellow,screen_buffer);
			}
			
		}
	}

// --- ( 0 - 2 ) ��Ǯ
	if(character_bank_data.active)
	{
		get_money_string(character_bank_data.now_money,print_rec);
		print16(window_x+161,window_y+238-2,print_rec,PEST_PUT,screen_buffer);
		
	}
	

// --- ( 0 - 3 ) ����������
	if(game_control.game_give_id!=0)
	{
		idx=get_map_npc_index(game_control.game_give_id);
		if(idx>=0)	
		{
			switch(map_npc_group[idx].npc_type)
			{
			case NPC_CHARACTER_TYPE:
				set_word_color(0,color_control.green);
				break;
			case NPC_CPU_TYPE:
				set_word_color(0,color_control.yellow);
				break;
			case NPC_BAOBAO_TYPE:
				break;
			}
			print16(window_x+24,window_y+327,map_npc_group[idx].npc_info.name,PEST_PUT,screen_buffer );
			set_word_color(0,color_control.white);
		}

	}

// --- ( 0 - 4 ) ��������

	if((character_pocket_data.give_idx[0]>=0)&&(character_pocket_data.give_number[0]>0))
	{
		idx=character_pocket_data.give_idx[0];
		if(character_pocket_data.item[idx].item_id>=0)
		{
			display_img256_buffer(window_x+24,window_y+242,
				system_image.item_image[character_pocket_data.item[idx].filename_id],screen_buffer);
			
			sprintf((char *)print_rec,"%d",character_pocket_data.give_number[0]);
			print16(window_x+27,window_y+281-2,print_rec,PEST_PUT,screen_buffer);
		}
		
	}

	if((character_pocket_data.give_idx[1]>=0)&&(character_pocket_data.give_number[1]>0))
	{
		idx=character_pocket_data.give_idx[1];

		if(character_pocket_data.item[idx].item_id>=0)
		{
			display_img256_buffer(window_x+64,window_y+242,
				system_image.item_image[character_pocket_data.item[idx].filename_id],screen_buffer);
			
			sprintf((char *)print_rec,"%d",character_pocket_data.give_number[1]);
			print16(window_x+67,window_y+281-2,print_rec,PEST_PUT,screen_buffer);
		}
		
	}

	if((character_pocket_data.give_idx[2]>=0)&&(character_pocket_data.give_number[2]>0))
	{
		idx=character_pocket_data.give_idx[2];

		if(character_pocket_data.item[idx].item_id>=0)
		{
			display_img256_buffer(window_x+104,window_y+242,
				system_image.item_image[character_pocket_data.item[idx].filename_id],screen_buffer);
			
			sprintf((char *)print_rec,"%d",character_pocket_data.give_number[2]);
			print16(window_x+107,window_y+281-2,print_rec,PEST_PUT,screen_buffer);
		}
			
		
	}
	

	g_GiveEdit.Paint(window_x, window_y, screen_buffer);

	
	// ------ display inst
	if((system_control.mouse_x>window_x)&&(system_control.mouse_x<=window_x+window_xl)&&
		(system_control.mouse_y>window_y)&&(system_control.mouse_y<=window_y+window_yl))
	{
		if((detail_item_inst.active)&&(detail_item_inst.idx>=0))
		{
			if((character_pocket_data.item[detail_item_inst.idx].number>0)&&(character_pocket_data.item[detail_item_inst.idx].filename_id>=0))
			{
				test_rect.left=0;
				test_rect.top=0;
				test_rect.right=160;
				test_rect.bottom=SCREEN_WIDTH;
				inst_xl=32+32*2+160;
				
				text_out_data.g_pChat->insertString((char *)detail_item_inst.inst,test_rect,false);
				inst_yl=test_rect.bottom+32;
				if(inst_yl<80)
					inst_yl=80;
				
				inst_x=system_control.mouse_x-inst_xl/2;
				inst_y=system_control.mouse_y-inst_yl-16;
				if(inst_x<=0)
					inst_x=system_control.mouse_x;
				
				if(inst_y<0)
					inst_y=system_control.mouse_y+32;
				if(inst_x+inst_xl>=SCREEN_WIDTH)inst_x=SCREEN_WIDTH-inst_xl;
				if(inst_y+inst_yl>=SCREEN_HEIGHT)inst_y=SCREEN_HEIGHT-inst_yl;
				draw_message_box(inst_x,inst_y,inst_xl,inst_yl,screen_buffer);
				put_bar(inst_x+2,inst_y+2,inst_xl-4,inst_yl-6,color_control.black,screen_buffer);
				
				display_zoom_img256_buffer(inst_x+16,inst_y+16,
					system_image.item_image[character_pocket_data.item[detail_item_inst.idx].filename_id],screen_buffer,200);
				
				text_out_data.g_pChat->updateString(inst_x+16+64,inst_y+16,test_rect.right,inst_yl,detail_item_inst.inst);
				text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,screen_buffer,false);
			}
			
		}
	}

	


}



void window_exec_give(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;

	SLONG task;
	SLONG x_offset,y_offset;
	SLONG i;
	SLONG channel;
	SLONG idx;
	
	RECT rect[2]={
		{ 247, 9 ,247+28, 9+28	},				// �ر�			0
		{ 180,318,180+63,318+21 }				// ȷ��			1
	};


	RECT item_pp[MAX_POCKET_ITEM]={
		{ 26+40*0, 56+40*0, 26+40*0+32, 56+40*0+32 },
		{ 26+40*1, 56+40*0, 26+40*1+32, 56+40*0+32 },
		{ 26+40*2, 56+40*0, 26+40*2+32, 56+40*0+32 },
		{ 26+40*3, 56+40*0, 26+40*3+32, 56+40*0+32 },
		{ 26+40*4, 56+40*0, 26+40*4+32, 56+40*0+32 },
		{ 26+40*5, 56+40*0, 26+40*5+32, 56+40*0+32 },
		
		{ 26+40*0, 56+40*1, 26+40*0+32, 56+40*1+32 },
		{ 26+40*1, 56+40*1, 26+40*1+32, 56+40*1+32 },
		{ 26+40*2, 56+40*1, 26+40*2+32, 56+40*1+32 },
		{ 26+40*3, 56+40*1, 26+40*3+32, 56+40*1+32 },
		{ 26+40*4, 56+40*1, 26+40*4+32, 56+40*1+32 },
		{ 26+40*5, 56+40*1, 26+40*5+32, 56+40*1+32 },
		
		{ 26+40*0, 56+40*2, 26+40*0+32, 56+40*2+32 },
		{ 26+40*1, 56+40*2, 26+40*1+32, 56+40*2+32 },
		{ 26+40*2, 56+40*2, 26+40*2+32, 56+40*2+32 },
		{ 26+40*3, 56+40*2, 26+40*3+32, 56+40*2+32 },
		{ 26+40*4, 56+40*2, 26+40*4+32, 56+40*2+32 },
		{ 26+40*5, 56+40*2, 26+40*5+32, 56+40*2+32 },
		
		{ 26+40*0, 56+40*3, 26+40*0+32, 56+40*3+32 },
		{ 26+40*1, 56+40*3, 26+40*1+32, 56+40*3+32 },
		{ 26+40*2, 56+40*3, 26+40*2+32, 56+40*3+32 },
		{ 26+40*3, 56+40*3, 26+40*3+32, 56+40*3+32 },
		{ 26+40*4, 56+40*3, 26+40*4+32, 56+40*3+32 },
		{ 26+40*5, 56+40*3, 26+40*5+32, 56+40*3+32 }
		
	};
	

	RECT give_pp[3]={
		{  24 ,242, 24+32, 242+32 },
		{  64 ,242, 64+32, 242+32 },
		{ 104 ,242,104+32, 242+32 }
	};
	

	
	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	
	task=-1;
	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;
	
	for(i=0;i<2;i++)
	{
		if((x_offset>=rect[i].left)&&(x_offset<rect[i].right)&&
			(y_offset>=rect[i].top)&&(y_offset<rect[i].bottom))
		{
			task=i;
			break;
		}
	}
	
	
	if(task<0)
	{
		for(i=0;i<MAX_POCKET_ITEM;i++)
		{
			if((x_offset>=item_pp[i].left)&&(x_offset<item_pp[i].right)&&
				(y_offset>=item_pp[i].top)&&(y_offset<item_pp[i].bottom))
			{
				task=100+i;
				break;
			}
			
		}
	}
	
	if(task<0)
	{
		for(i=0;i<3;i++)
		{
			if((x_offset>=give_pp[i].left)&&(x_offset<give_pp[i].right)&&
				(y_offset>=give_pp[i].top)&&(y_offset<give_pp[i].bottom))
			{
				task=200+i;
				break;
			}
			
		}
	}

	
	if(game_control.game_give_id!=0)
	{
		idx=get_map_npc_index(game_control.game_give_id);
		if(idx<0)		// ���Ѿ����ٵ�ǰ	close
		{
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_give=-1;
			game_control.game_give_id=0;
		}

	}		

	
	for(i=0;i<3;i++)
	{
		idx=character_pocket_data.give_idx[i];
		if(idx<0)continue;
		if(character_pocket_data.item[idx].item_id>=0)
		{
			if(character_pocket_data.give_number[i]>=character_pocket_data.item[idx].number)
			{
				character_pocket_data.give_number[i]=character_pocket_data.item[idx].number;
				
			}
		}
	}
	

	if(task<0)
	{
		clear_detail_item_inst();
	}
	

	if((task>=100)&&(task<200))
	{
		detail_item_inst.idx=task-100;
		if((character_pocket_data.item[detail_item_inst.idx].number>0)&&(character_pocket_data.item[detail_item_inst.idx].filename_id>=0))
		{
			if(detail_item_inst.old_idx!=detail_item_inst.idx)
			{
				if(system_control.system_timer>detail_item_inst.timer+DETAIL_DELAY_TIME)
				{
					detail_item_inst.old_idx=detail_item_inst.idx;
					detail_item_inst.active=false;
					detail_item_inst.type=ITEM_INST_POCKET;
					request_detail_inst();
					detail_item_inst.timer=system_control.system_timer;
				}
			}
		}
	}


	if((task>=200)&&(task<300))
	{
		idx=character_pocket_data.give_idx[task-200];
		detail_item_inst.idx=idx;
		if(idx>=0)
		{
			if((character_pocket_data.item[idx].number>0)&&(character_pocket_data.item[idx].filename_id>=0))
			{
				if(detail_item_inst.old_idx!=detail_item_inst.idx)
				{
					if(system_control.system_timer>detail_item_inst.timer+DETAIL_DELAY_TIME)
					{
						detail_item_inst.old_idx=detail_item_inst.idx;
						detail_item_inst.active=false;
						detail_item_inst.type=ITEM_INST_POCKET;
						request_detail_inst();
						detail_item_inst.timer=system_control.system_timer;
					}
				}
				else
				{
					detail_item_inst.timer=system_control.system_timer;
				}
				
			}
		}
		
	}
				
	
	
	
	switch(system_control.mouse_key)
	{
	case MS_LDn:
		g_GiveEdit.LBtnDown(system_control.mouse_x, system_control.mouse_y);
		break;
		
	case MS_RUp:						//	close 
		if(task<0)
		{
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_give=-1;
			game_control.game_give_id=0;
			g_GiveEdit.Shutdown();
			break;
		}
		switch(task)
		{
		case 100: case 101: case 102: case 103: case 104: case 105:
		case 106: case 107: case 108: case 109: case 110: case 111:
		case 112: case 113: case 114: case 115: case 116: case 117:
		case 118: case 119: case 120: case 121: case 122: case 123:
			character_pocket_data.idx=task-100;
			if((character_pocket_data.idx<0)||(character_pocket_data.idx>=24))break;
			if(character_pocket_data.item[character_pocket_data.idx].item_id<0)break;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			set_give_item_index_max();
			break;
		case 200: case 201: case 202:
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			set_give_item_index_del(task-200,999);
			break;
		}
		
		break;
	case MS_Move:
	case MS_Dummy:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// ȷ��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand27",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		}
		break;

	case MS_LUp:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_give=-1;
			game_control.game_give_id=0;
			g_GiveEdit.Shutdown();
			break;
		case 1:		// ȷ��
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			character_pocket_data.give_money = g_GiveEdit.getNumber();
			if((character_pocket_data.give_idx[0]<0)&&
				(character_pocket_data.give_idx[1]<0)&&
				(character_pocket_data.give_idx[2]<0)&&
				(character_pocket_data.give_money<=0))
			{
				display_system_message((UCHR *)MSG_NO_GIVE_DATA);
				break;
			}
			send_item_give();
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_give=-1;
			game_control.game_give_id=0;
			g_GiveEdit.Shutdown();
			break;

		case 100: case 101: case 102: case 103: case 104: case 105:
		case 106: case 107: case 108: case 109: case 110: case 111:
		case 112: case 113: case 114: case 115: case 116: case 117:
		case 118: case 119: case 120: case 121: case 122: case 123:
			character_pocket_data.idx=task-100;
			if((character_pocket_data.idx<0)||(character_pocket_data.idx>=24))break;
			if(character_pocket_data.item[character_pocket_data.idx].item_id<0)break;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			set_give_item_index();
			break;

		case 200: case 201: case 202:
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			set_give_item_index_del(task-200,1);
			break;
			
		}
		break;

	case MS_LDrag:				// �϶�

		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// ȷ��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand28",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		default:
			now_window_move_xoffset=system_control.mouse_x-window_x;
			now_window_move_yoffset=system_control.mouse_y-window_y;
			now_window_move_handle=now_exec_window_handle;
			break;
		}
		
		break;
		
	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}








/*******************************************************************************************************************}
{	deal																											}
********************************************************************************************************************/
void window_draw_deal(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;

	UCHR npc_filename[80];
	SLONG mda_handle;
	SLONG mda_command;
	SLONG i;
	SLONG idx;

	static ULONG timer=0;
	SLONG inst_x,inst_y,inst_xl,inst_yl;
	RECT test_rect;
	
	
	RECT item_pp[MAX_POCKET_ITEM]={
		{ 26+40*0, 56+40*0, 26+40*0+32, 56+40*0+32 },
		{ 26+40*1, 56+40*0, 26+40*1+32, 56+40*0+32 },
		{ 26+40*2, 56+40*0, 26+40*2+32, 56+40*0+32 },
		{ 26+40*3, 56+40*0, 26+40*3+32, 56+40*0+32 },
		{ 26+40*4, 56+40*0, 26+40*4+32, 56+40*0+32 },
		{ 26+40*5, 56+40*0, 26+40*5+32, 56+40*0+32 },
		
		{ 26+40*0, 56+40*1, 26+40*0+32, 56+40*1+32 },
		{ 26+40*1, 56+40*1, 26+40*1+32, 56+40*1+32 },
		{ 26+40*2, 56+40*1, 26+40*2+32, 56+40*1+32 },
		{ 26+40*3, 56+40*1, 26+40*3+32, 56+40*1+32 },
		{ 26+40*4, 56+40*1, 26+40*4+32, 56+40*1+32 },
		{ 26+40*5, 56+40*1, 26+40*5+32, 56+40*1+32 },
		
		{ 26+40*0, 56+40*2, 26+40*0+32, 56+40*2+32 },
		{ 26+40*1, 56+40*2, 26+40*1+32, 56+40*2+32 },
		{ 26+40*2, 56+40*2, 26+40*2+32, 56+40*2+32 },
		{ 26+40*3, 56+40*2, 26+40*3+32, 56+40*2+32 },
		{ 26+40*4, 56+40*2, 26+40*4+32, 56+40*2+32 },
		{ 26+40*5, 56+40*2, 26+40*5+32, 56+40*2+32 },
		
		{ 26+40*0, 56+40*3, 26+40*0+32, 56+40*3+32 },
		{ 26+40*1, 56+40*3, 26+40*1+32, 56+40*3+32 },
		{ 26+40*2, 56+40*3, 26+40*2+32, 56+40*3+32 },
		{ 26+40*3, 56+40*3, 26+40*3+32, 56+40*3+32 },
		{ 26+40*4, 56+40*3, 26+40*4+32, 56+40*3+32 },
		{ 26+40*5, 56+40*3, 26+40*5+32, 56+40*3+32 }
		
	};
	
	
	
	
	
	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	
	// --- ( 0 - 0 )  Draw body
	display_img256_buffer(window_x,window_y,system_image.item_public_body.image,screen_buffer);	
	display_img256_buffer(window_x+2,window_y+(system_image.item_public_body.rect.bottom-system_image.item_public_body.rect.top),system_image.deal_action_body.image,screen_buffer);	
	display_img256_buffer(window_x+107,window_y+14,system_image.deal_body.image,screen_buffer);	

	
	// --- ( 0 - 1 ) �ڴ� item
	if(character_pocket_data.active)
	{
		for(i=0;i<MAX_POCKET_ITEM;i++)
		{
			
			if(game_control.deal_type==0)						
			{
				if((character_pocket_data.item[i].number>0)&&(character_pocket_data.item[i].filename_id>=0))
				{
					
					display_img256_buffer(window_x+item_pp[i].left,window_y+item_pp[i].top,
						system_image.item_image[character_pocket_data.item[i].filename_id],screen_buffer);
					
					if(character_pocket_data.item[i].number>1)
					{
						sprintf((char *)print_rec,"%d",character_pocket_data.item[i].number);
						print12(window_x+item_pp[i].right-(strlen((char *)print_rec)*6)-6-1,window_y+item_pp[i].bottom-10-1,print_rec,COPY_PUT,screen_buffer);
						
					}
					
				}
			}
			else						// ���ױ���
			{
				if(i<MAX_BAOBAO_STACK)
				{
					if(base_baobao_data.data.baobao_list[i].baobao_id>0)
					{
						display_img256_buffer(window_x+item_pp[i].left,window_y+item_pp[i].top,
							system_image.baobao_icon.image,screen_buffer);
					}
				}
			}
			
			
			if(i==character_pocket_data.idx)
			{
				put_scroll_box(window_x+item_pp[i].left-1,
					window_y+item_pp[i].top-1,
					window_x+item_pp[i].right+1,
					window_y+item_pp[i].bottom+1,color_control.red,color_control.yellow,screen_buffer);
				put_scroll_box(window_x+item_pp[i].left-2,
					window_y+item_pp[i].top-2,
					window_x+item_pp[i].right+2,
					window_y+item_pp[i].bottom+2,color_control.red,color_control.yellow,screen_buffer);
			}
			
		}
	}
	

	// --- ( 0 - 2 ) �ҷ�����
	set_word_color(0,color_control.green);
	print16(window_x+149,window_y+223,map_npc_group[system_control.control_npc_idx].npc_info.name,PEST_PUT,screen_buffer);
	set_word_color(0,color_control.white);
	
	
	// --- ( 0 - 3 ) ����������
	if(game_control.game_deal_id!=0)
	{
		idx=get_map_npc_index(game_control.game_deal_id);
		if(idx>=0)	
		{
			switch(map_npc_group[idx].npc_type)
			{
			case NPC_CHARACTER_TYPE:
				set_word_color(0,color_control.green);
				break;
			case NPC_CPU_TYPE:
				set_word_color(0,color_control.yellow);
				break;
			case NPC_BAOBAO_TYPE:
				break;
			}
			print16(window_x+149,window_y+302,map_npc_group[idx].npc_info.name,PEST_PUT,screen_buffer );
			set_word_color(0,color_control.white);
		}
		
	}
	

	// --- ( 0 - 4 ) �ҷ���������
	
	if((character_pocket_data.my_deal_idx[0]>=0)&&(character_pocket_data.my_deal_number[0]>0))
	{
		idx=character_pocket_data.my_deal_idx[0];
		if(idx<100)
		{
			if(character_pocket_data.item[idx].item_id>=0)
			{
				display_img256_buffer(window_x+23,window_y+227,
					system_image.item_image[character_pocket_data.item[idx].filename_id],screen_buffer);
				
				sprintf((char *)print_rec,"%d",character_pocket_data.my_deal_number[0]);
				print16(window_x+25,window_y+266-2,print_rec,PEST_PUT,screen_buffer);
				
				if(character_pocket_data.my_deal_ready)
				{
					put_scroll_box(window_x+23-2,window_y+227-2,window_x+23+32+1,window_y+227+32+1,color_control.white,color_control.black,screen_buffer);
				}
				
			}
		}
		else
		{
			idx=idx-100;
			if((idx>=0)&&(idx<MAX_BAOBAO_STACK))
			{
				if( base_baobao_data.data.baobao_list[idx].baobao_id>0)
				{
					display_img256_buffer(window_x+23,window_y+227,
						system_image.baobao_icon.image,screen_buffer);

					print16(window_x+25,window_y+266-2,(UCHR *)"1",PEST_PUT,screen_buffer);
					
					if(character_pocket_data.my_deal_ready)
					{
						put_scroll_box(window_x+23-2,window_y+227-2,window_x+23+32+1,window_y+227+32+1,color_control.white,color_control.black,screen_buffer);
					}
					
				}
			}
			
		}
		
	}
	
	if((character_pocket_data.my_deal_idx[1]>=0)&&(character_pocket_data.my_deal_number[1]>0))
	{
		idx=character_pocket_data.my_deal_idx[1];
		if(idx<100)
		{
			if(character_pocket_data.item[idx].item_id>=0)
			{
				display_img256_buffer(window_x+63,window_y+227,
					system_image.item_image[character_pocket_data.item[idx].filename_id],screen_buffer);
				
				sprintf((char *)print_rec,"%d",character_pocket_data.my_deal_number[1]);
				print16(window_x+65,window_y+266-2,print_rec,PEST_PUT,screen_buffer);
				
				if(character_pocket_data.my_deal_ready)
				{
					put_scroll_box(window_x+63-2,window_y+227-2,window_x+63+32+1,window_y+227+32+1,color_control.white,color_control.black,screen_buffer);
				}
				
			}
		}
		else
		{
			idx=idx-100;
			if((idx>=0)&&(idx<MAX_BAOBAO_STACK))
			{
				display_img256_buffer(window_x+63,window_y+227,
					system_image.baobao_icon.image,screen_buffer);

				print16(window_x+65,window_y+266-2,(UCHR *)"1",PEST_PUT,screen_buffer);
				
				if(character_pocket_data.my_deal_ready)
				{
					put_scroll_box(window_x+63-2,window_y+227-2,window_x+63+32+1,window_y+227+32+1,color_control.white,color_control.black,screen_buffer);
				}
			}
		}
		
	}

	
	if((character_pocket_data.my_deal_idx[2]>=0)&&(character_pocket_data.my_deal_number[2]>0))
	{
		idx=character_pocket_data.my_deal_idx[2];
		if(idx<100)
		{
			if(character_pocket_data.item[idx].item_id>=0)
			{
				display_img256_buffer(window_x+103,window_y+227,
					system_image.item_image[character_pocket_data.item[idx].filename_id],screen_buffer);
				
				sprintf((char *)print_rec,"%d",character_pocket_data.my_deal_number[2]);
				print16(window_x+105,window_y+266-2,print_rec,PEST_PUT,screen_buffer);
				
				if(character_pocket_data.my_deal_ready)
				{
					put_scroll_box(window_x+103-2,window_y+227-2,window_x+103+32+1,window_y+227+32+1,color_control.white,color_control.black,screen_buffer);
				}
				
			}
		}
		else
		{
			idx=idx-100;
			if((idx>=0)&&(idx<MAX_BAOBAO_STACK))
			{
				display_img256_buffer(window_x+103,window_y+227,
					system_image.baobao_icon.image,screen_buffer);

				print16(window_x+105,window_y+266-2,(UCHR *)"1",PEST_PUT,screen_buffer);
				
				if(character_pocket_data.my_deal_ready)
				{
					put_scroll_box(window_x+103-2,window_y+227-2,window_x+103+32+1,window_y+227+32+1,color_control.white,color_control.black,screen_buffer);
				}
				
			}
		}
		
	}
	
	


	// --- ( 0 - 4 ) �Է���������
	
	if((character_pocket_data.other_deal_filename_id[0]>=0)&&(character_pocket_data.other_deal_number[0]>0))
	{
		if(character_pocket_data.other_deal_filename_id[0]>=10000)
			display_img256_buffer(window_x+23,window_y+304,
			system_image.baobao_icon.image,screen_buffer);
		else
			display_img256_buffer(window_x+23,window_y+304,
				system_image.item_image[character_pocket_data.other_deal_filename_id[0]],screen_buffer);
			
		sprintf((char *)print_rec,"%d",character_pocket_data.other_deal_number[0]);
		print16(window_x+25,window_y+343-2,print_rec,PEST_PUT,screen_buffer);
	}


	
	if((character_pocket_data.other_deal_filename_id[1]>=0)&&(character_pocket_data.other_deal_number[1]>0))
	{
		if(character_pocket_data.other_deal_filename_id[1]>=10000)
			display_img256_buffer(window_x+63,window_y+304,
			system_image.baobao_icon.image,screen_buffer);
		else
			display_img256_buffer(window_x+63,window_y+304,
			system_image.item_image[character_pocket_data.other_deal_filename_id[1]],screen_buffer);
		
		sprintf((char *)print_rec,"%d",character_pocket_data.other_deal_number[1]);
		print16(window_x+65,window_y+343-2,print_rec,PEST_PUT,screen_buffer);
	}
	

	if((character_pocket_data.other_deal_filename_id[2]>=0)&&(character_pocket_data.other_deal_number[2]>0))
	{
		if(character_pocket_data.other_deal_filename_id[2]>=10000)
			display_img256_buffer(window_x+103,window_y+304,
			system_image.baobao_icon.image,screen_buffer);
		else
			display_img256_buffer(window_x+103,window_y+304,
			system_image.item_image[character_pocket_data.other_deal_filename_id[2]],screen_buffer);
		
		sprintf((char *)print_rec,"%d",character_pocket_data.other_deal_number[2]);
		print16(window_x+105,window_y+343-2,print_rec,PEST_PUT,screen_buffer);
	}
	

	if (character_pocket_data.other_deal_money > 0)
	{
		sprintf((char *)print_rec,"%d",character_pocket_data.other_deal_money);	
		print16(window_x+181,window_y+332,print_rec,PEST_PUT,screen_buffer);		
	}

	// --- ( 0 - 4 ) ���� ready
	if(character_pocket_data.my_deal_ready)
	{
		mda_group_exec(system_image.main_menu_handle,(UCHR *)"MainCOmmand0",
			window_x+148,window_y+249,screen_buffer,MDA_COMMAND_FIRST,0,0,0);
	}
	
	if(character_pocket_data.other_deal_ready)
	{
		mda_group_exec(system_image.main_menu_handle,(UCHR *)"MainCOmmand0",
			window_x+148,window_y+332,screen_buffer,MDA_COMMAND_FIRST,0,0,0);
	}
	
	g_DealEdit.Paint(window_x, window_y, screen_buffer);


	// ------ display inst
	if((system_control.mouse_x>window_x)&&(system_control.mouse_x<=window_x+window_xl)&&
		(system_control.mouse_y>window_y)&&(system_control.mouse_y<=window_y+window_yl))
	{
	if((detail_item_inst.active)&&(detail_item_inst.idx>=0))
	{
		switch(detail_item_inst.type)
		{
		case ITEM_INST_POCKET:
			if((character_pocket_data.item[detail_item_inst.idx].number>0)&&(character_pocket_data.item[detail_item_inst.idx].filename_id>=0))
			{
				test_rect.left=0;
				test_rect.top=0;
				test_rect.right=160;
				test_rect.bottom=SCREEN_WIDTH;
				inst_xl=32+32*2+160;
				
				text_out_data.g_pChat->insertString((char *)detail_item_inst.inst,test_rect,false);
				inst_yl=test_rect.bottom+32;
				if(inst_yl<80)
					inst_yl=80;
				
				inst_x=system_control.mouse_x-inst_xl/2;
				inst_y=system_control.mouse_y-inst_yl-16;
				if(inst_x<=0)
					inst_x=system_control.mouse_x;
				
				if(inst_y<0)
					inst_y=system_control.mouse_y+32;
				if(inst_x+inst_xl>=SCREEN_WIDTH)inst_x=SCREEN_WIDTH-inst_xl;
				if(inst_y+inst_yl>=SCREEN_HEIGHT)inst_y=SCREEN_HEIGHT-inst_yl;
				draw_message_box(inst_x,inst_y,inst_xl,inst_yl,screen_buffer);
				put_bar(inst_x+2,inst_y+2,inst_xl-4,inst_yl-6,color_control.black,screen_buffer);
				
				display_zoom_img256_buffer(inst_x+16,inst_y+16,
					system_image.item_image[character_pocket_data.item[detail_item_inst.idx].filename_id],screen_buffer,200);
				
				text_out_data.g_pChat->updateString(inst_x+16+64,inst_y+16,test_rect.right,inst_yl,detail_item_inst.inst);
				text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,screen_buffer,false);
			}
			
			break;
		case ITEM_INST_DEAL_OTHER_ITEM:
			if((character_pocket_data.other_deal_number[detail_item_inst.idx]>0)&&(character_pocket_data.other_deal_filename_id[detail_item_inst.idx]>=0))
			{
				test_rect.left=0;
				test_rect.top=0;
				test_rect.right=160;
				test_rect.bottom=SCREEN_WIDTH;
				inst_xl=32+32*2+160;
				
				text_out_data.g_pChat->insertString((char *)detail_item_inst.inst,test_rect,false);
				inst_yl=test_rect.bottom+32;
				if(inst_yl<80)
					inst_yl=80;
				
				inst_x=system_control.mouse_x-inst_xl/2;
				inst_y=system_control.mouse_y-inst_yl-16;
				if(inst_x<=0)
					inst_x=system_control.mouse_x;
				
				if(inst_y<0)
					inst_y=system_control.mouse_y+32;
				if(inst_x+inst_xl>=SCREEN_WIDTH)inst_x=SCREEN_WIDTH-inst_xl;
				if(inst_y+inst_yl>=SCREEN_HEIGHT)inst_y=SCREEN_HEIGHT-inst_yl;
				draw_message_box(inst_x,inst_y,inst_xl,inst_yl,screen_buffer);
				put_bar(inst_x+2,inst_y+2,inst_xl-4,inst_yl-6,color_control.black,screen_buffer);
				
				display_zoom_img256_buffer(inst_x+16,inst_y+16,
					system_image.item_image[character_pocket_data.other_deal_filename_id[detail_item_inst.idx]],screen_buffer,200);
				
				text_out_data.g_pChat->updateString(inst_x+16+64,inst_y+16,test_rect.right,inst_yl,detail_item_inst.inst);
				text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,screen_buffer,false);
			}
			
			break;
		case ITEM_INST_DEAL_OTHER_BAOBAO:
			if( character_pocket_data.other_deal_filename_id[detail_item_inst.idx]>=10000)
			{
				test_rect.left=0;
				test_rect.top=0;
				test_rect.right=160;
				test_rect.bottom=SCREEN_WIDTH;
				inst_xl=32+104+160;
				
				text_out_data.g_pChat->insertString((char *)detail_item_inst.inst,test_rect,false);
				inst_yl=test_rect.bottom+32;
				if(inst_yl<80)
					inst_yl=80;
				
				inst_x=system_control.mouse_x-inst_xl/2;
				inst_y=system_control.mouse_y-inst_yl-16;
				if(inst_x<=0)
					inst_x=system_control.mouse_x;
				
				if(inst_y<0)
					inst_y=system_control.mouse_y+32;
				if(inst_x+inst_xl>=SCREEN_WIDTH)inst_x=SCREEN_WIDTH-inst_xl;
				if(inst_y+inst_yl>=SCREEN_HEIGHT)inst_y=SCREEN_HEIGHT-inst_yl;
				draw_message_box(inst_x,inst_y,inst_xl,inst_yl,screen_buffer);
				put_bar(inst_x+2,inst_y+2,inst_xl-4,inst_yl-6,color_control.black,screen_buffer);
				
				
				text_out_data.g_pChat->updateString(inst_x+16+104,inst_y+16,test_rect.right,inst_yl,detail_item_inst.inst);
				text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,screen_buffer,false);
				
				sprintf((char *)npc_filename,"NPC\\NPC%da.MDA",character_pocket_data.other_deal_filename_id[detail_item_inst.idx]-10000);
				mda_handle=get_mda_handle(npc_filename);
				
				if(mda_handle<0)
				{
					if(detail_item_inst.handle>=0)
						auto_close_map_npc_handle(detail_item_inst.handle);
					mda_group_open((UCHR *)npc_filename,&mda_handle);
				}
				
				detail_item_inst.handle=mda_handle;
				
				
				mda_command=MDA_COMMAND_LOOP;
				if(detail_item_inst.handle>=0)
				{
					if(system_control.system_timer>timer)
					{
						mda_command=mda_command|MDA_COMMAND_NEXT_FRAME;
						timer=system_control.system_timer+SYSTEM_MAIN_DELAY;
					}
					
					mda_group_exec(detail_item_inst.handle,(UCHR *)"MainCommand3",inst_x+16+52,inst_y+80+16,
						screen_buffer,mda_command,0,0,0);
				}
				
			}
			break;
		case ITEM_INST_MY_BAOBAO:
			if( base_baobao_data.data.baobao_list[detail_item_inst.idx].baobao_id>0)
			{
				test_rect.left=0;
				test_rect.top=0;
				test_rect.right=160;
				test_rect.bottom=SCREEN_WIDTH;
				inst_xl=32+104+160;
				
				text_out_data.g_pChat->insertString((char *)detail_item_inst.inst,test_rect,false);
				inst_yl=test_rect.bottom+32;
				if(inst_yl<80)
					inst_yl=80;
				
				inst_x=system_control.mouse_x-inst_xl/2;
				inst_y=system_control.mouse_y-inst_yl-16;
				if(inst_x<=0)
					inst_x=system_control.mouse_x;
				
				if(inst_y<0)
					inst_y=system_control.mouse_y+32;
				if(inst_x+inst_xl>=SCREEN_WIDTH)inst_x=SCREEN_WIDTH-inst_xl;
				if(inst_y+inst_yl>=SCREEN_HEIGHT)inst_y=SCREEN_HEIGHT-inst_yl;
				draw_message_box(inst_x,inst_y,inst_xl,inst_yl,screen_buffer);
				put_bar(inst_x+2,inst_y+2,inst_xl-4,inst_yl-6,color_control.black,screen_buffer);


				text_out_data.g_pChat->updateString(inst_x+16+104,inst_y+16,test_rect.right,inst_yl,detail_item_inst.inst);
				text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,screen_buffer,false);
				
				sprintf((char *)npc_filename,"NPC\\NPC%da.MDA",base_baobao_data.data.baobao_list[detail_item_inst.idx].base.filename_id);
				mda_handle=get_mda_handle(npc_filename);
				
				if(mda_handle<0)
				{
					if(detail_item_inst.handle>=0)
						auto_close_map_npc_handle(detail_item_inst.handle);
					mda_group_open((UCHR *)npc_filename,&mda_handle);
				}
				
				detail_item_inst.handle=mda_handle;
				
				
				mda_command=MDA_COMMAND_LOOP;
				if(detail_item_inst.handle>=0)
				{
					if(system_control.system_timer>timer)
					{
						mda_command=mda_command|MDA_COMMAND_NEXT_FRAME;
						timer=system_control.system_timer+SYSTEM_MAIN_DELAY;
					}
					
					mda_group_exec(detail_item_inst.handle,(UCHR *)"MainCommand3",inst_x+16+52,inst_y+80+16,
						screen_buffer,mda_command,0,0,0);
				}
				
			}
			
			break;
		}
		
	}
	
	}


}



void window_exec_deal(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;

	SLONG task;
	SLONG x_offset,y_offset;
	SLONG i;
	SLONG channel;
	SLONG idx;
	
	RECT rect[4]={
		{ 247,  9,247+28, 9+28	},				// �ر�			0
		{ 204,271,204+63,271+21 },				// ȷ��			1
		{  27,368, 27+88,368+17 },				// ������Ʒ		2
		{ 152,368,152+88,368+17 }				// ���׳���		3
	};

	RECT item_pp[MAX_POCKET_ITEM]={
		{ 26+40*0, 56+40*0, 26+40*0+32, 56+40*0+32 },
		{ 26+40*1, 56+40*0, 26+40*1+32, 56+40*0+32 },
		{ 26+40*2, 56+40*0, 26+40*2+32, 56+40*0+32 },
		{ 26+40*3, 56+40*0, 26+40*3+32, 56+40*0+32 },
		{ 26+40*4, 56+40*0, 26+40*4+32, 56+40*0+32 },
		{ 26+40*5, 56+40*0, 26+40*5+32, 56+40*0+32 },
		
		{ 26+40*0, 56+40*1, 26+40*0+32, 56+40*1+32 },
		{ 26+40*1, 56+40*1, 26+40*1+32, 56+40*1+32 },
		{ 26+40*2, 56+40*1, 26+40*2+32, 56+40*1+32 },
		{ 26+40*3, 56+40*1, 26+40*3+32, 56+40*1+32 },
		{ 26+40*4, 56+40*1, 26+40*4+32, 56+40*1+32 },
		{ 26+40*5, 56+40*1, 26+40*5+32, 56+40*1+32 },
		
		{ 26+40*0, 56+40*2, 26+40*0+32, 56+40*2+32 },
		{ 26+40*1, 56+40*2, 26+40*1+32, 56+40*2+32 },
		{ 26+40*2, 56+40*2, 26+40*2+32, 56+40*2+32 },
		{ 26+40*3, 56+40*2, 26+40*3+32, 56+40*2+32 },
		{ 26+40*4, 56+40*2, 26+40*4+32, 56+40*2+32 },
		{ 26+40*5, 56+40*2, 26+40*5+32, 56+40*2+32 },
		
		{ 26+40*0, 56+40*3, 26+40*0+32, 56+40*3+32 },
		{ 26+40*1, 56+40*3, 26+40*1+32, 56+40*3+32 },
		{ 26+40*2, 56+40*3, 26+40*2+32, 56+40*3+32 },
		{ 26+40*3, 56+40*3, 26+40*3+32, 56+40*3+32 },
		{ 26+40*4, 56+40*3, 26+40*4+32, 56+40*3+32 },
		{ 26+40*5, 56+40*3, 26+40*5+32, 56+40*3+32 }
		
	};
	
	
	RECT deal_pp[3]={
		{  23 ,227, 23+32, 227+32 },
		{  63 ,227, 63+32, 227+32 },
		{ 103 ,227,103+32, 227+32 }
	};
	
	RECT other_pp[3]={
		{  23 ,304, 23+32, 304+32 },
		{  63 ,304, 63+32, 304+32 },
		{ 103 ,304,103+32, 304+32 }
	};
	

	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	
	task=-1;
	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;
	
	for(i=0;i<4;i++)
	{
		if((x_offset>=rect[i].left)&&(x_offset<rect[i].right)&&
			(y_offset>=rect[i].top)&&(y_offset<rect[i].bottom))
		{
			task=i;
			break;
		}
	}


	if(task<0)
	{
		for(i=0;i<MAX_POCKET_ITEM;i++)
		{
			if((x_offset>=item_pp[i].left)&&(x_offset<item_pp[i].right)&&
				(y_offset>=item_pp[i].top)&&(y_offset<item_pp[i].bottom))
			{
				task=100+i;
				break;
			}
			
		}
	}
	
	
	if(task<0)
	{
		for(i=0;i<3;i++)
		{
			if((x_offset>=deal_pp[i].left)&&(x_offset<deal_pp[i].right)&&
				(y_offset>=deal_pp[i].top)&&(y_offset<deal_pp[i].bottom))
			{
				task=200+i;
				break;
			}
			
		}
	}

	if(task<0)
	{
		for(i=0;i<3;i++)
		{
			if((x_offset>=other_pp[i].left)&&(x_offset<other_pp[i].right)&&
				(y_offset>=other_pp[i].top)&&(y_offset<other_pp[i].bottom))
			{
				task=500+i;
				break;
			}
			
		}
	}
	

	if(task<0)
	{
		if((x_offset>=148)&&(x_offset<148+18)&&
			(y_offset>=249)&&(y_offset<249+18))
		{
			task=300;
		}
	}


	
	if(game_control.game_deal_id!=0)
	{
		idx=get_map_npc_index(game_control.game_deal_id);
		if(idx<0)		// ���Ѿ����ٵ�ǰ	close
		{
			
			send_cancel_deal();
			game_control.game_deal_id=0;
		}
		
	}		
	

	for(i=0;i<3;i++)
	{
		idx=character_pocket_data.my_deal_idx[i];
		if(idx<0)continue;
		if(idx<100)
		{
			if(character_pocket_data.item[idx].item_id>=0)
			{
				if(character_pocket_data.my_deal_number[i]>=character_pocket_data.item[idx].number)
				{
					character_pocket_data.my_deal_number[i]=character_pocket_data.item[idx].number;
					
				}
			}
		}
	}
	

	
	if((task>=100)&&(task<200))
	{
		detail_item_inst.idx=task-100;
		if(game_control.deal_type==0)						
		{
			if((character_pocket_data.item[detail_item_inst.idx].number>0)&&(character_pocket_data.item[detail_item_inst.idx].filename_id>=0))
			{
				if(detail_item_inst.old_idx!=detail_item_inst.idx)
				{
					if(system_control.system_timer>detail_item_inst.timer+DETAIL_DELAY_TIME)
					{
						detail_item_inst.old_idx=detail_item_inst.idx;
						detail_item_inst.active=false;
						detail_item_inst.type=ITEM_INST_POCKET;
						request_detail_inst();
						detail_item_inst.timer=system_control.system_timer;
					}
				}
			}
		}
		else			// ��������
		{
			idx=detail_item_inst.idx;
			if(base_baobao_data.data.baobao_list[idx].baobao_id>0)
			{
				if(detail_item_inst.old_idx!=detail_item_inst.idx)
				{
					if(system_control.system_timer>detail_item_inst.timer+DETAIL_DELAY_TIME)
					{
						detail_item_inst.old_idx=detail_item_inst.idx;
						detail_item_inst.active=false;
						detail_item_inst.type=ITEM_INST_MY_BAOBAO;
						make_detail_inst();
						detail_item_inst.timer=system_control.system_timer;
					}
				}
				else
				{
					detail_item_inst.timer=system_control.system_timer;
				}
				
			}
			
		}
	}
	

	if(task<0)
	{
		clear_detail_item_inst();
	}
	
	
	if((task>=200)&&(task<300))
	{
		idx=character_pocket_data.my_deal_idx[task-200];
		if(idx<100)					// ��Ʒ
		{
			detail_item_inst.idx=idx;
			if(idx>=0)
			{
				if((character_pocket_data.item[idx].number>0)&&(character_pocket_data.item[idx].filename_id>=0))
				{
					if(detail_item_inst.old_idx!=detail_item_inst.idx)
					{
						if(system_control.system_timer>detail_item_inst.timer+DETAIL_DELAY_TIME)
						{
							detail_item_inst.old_idx=detail_item_inst.idx;
							detail_item_inst.active=false;
							detail_item_inst.type=ITEM_INST_POCKET;
							request_detail_inst();
							detail_item_inst.timer=system_control.system_timer;
						}
					}
					else
					{
						detail_item_inst.timer=system_control.system_timer;
					}
					
				}
			}
		}
		else						// ����
		{
			idx=idx-100;
			detail_item_inst.idx=idx;
			if(idx>=0)
			{
				if(base_baobao_data.data.baobao_list[idx].baobao_id>0)
				{
					if(detail_item_inst.old_idx!=detail_item_inst.idx)
					{
						if(system_control.system_timer>detail_item_inst.timer+DETAIL_DELAY_TIME)
						{
							detail_item_inst.old_idx=detail_item_inst.idx;
							detail_item_inst.active=false;
							detail_item_inst.type=ITEM_INST_MY_BAOBAO;
							make_detail_inst();
							detail_item_inst.timer=system_control.system_timer;
						}
					}
					else
					{
						detail_item_inst.timer=system_control.system_timer;
					}
					
				}
			}
		}
		
	}
	

	if((task>=500)&&(task<600))
	{
		idx=task-500;
		if(character_pocket_data.other_deal_number[idx]>0)
		{
			detail_item_inst.idx=idx;
			if((character_pocket_data.other_deal_filename_id[idx]>=0)&&(character_pocket_data.other_deal_filename_id[idx]<10000)) // ��Ʒ
			{
				if(detail_item_inst.old_idx!=detail_item_inst.idx)
				{
					if(system_control.system_timer>detail_item_inst.timer+DETAIL_DELAY_TIME)
					{
						detail_item_inst.old_idx=detail_item_inst.idx;
						detail_item_inst.active=false;
						detail_item_inst.type=ITEM_INST_DEAL_OTHER_ITEM;
						request_detail_inst();
						detail_item_inst.timer=system_control.system_timer;
					}
				}
				else
				{
					detail_item_inst.timer=system_control.system_timer;
				}
				
			}
			else				// ����
			{
				if(detail_item_inst.old_idx!=detail_item_inst.idx)
				{
					if(system_control.system_timer>detail_item_inst.timer+DETAIL_DELAY_TIME)
					{
						detail_item_inst.old_idx=detail_item_inst.idx;
						detail_item_inst.active=false;
						detail_item_inst.type=ITEM_INST_DEAL_OTHER_BAOBAO;
						request_detail_inst();
						detail_item_inst.timer=system_control.system_timer;
					}
				}
				else
				{
					detail_item_inst.timer=system_control.system_timer;
				}
				
			}

		}
		
	}	
	
	switch(system_control.mouse_key)
	{
	case MS_LDn:
		g_DealEdit.LBtnDown(system_control.mouse_x, system_control.mouse_y);
		break;

	case MS_RUp:						//	close 
		if(task<0)
		{
			send_cancel_deal();
			break;
		}
		switch(task)
		{
		case 100: case 101: case 102: case 103: case 104: case 105:
		case 106: case 107: case 108: case 109: case 110: case 111:
		case 112: case 113: case 114: case 115: case 116: case 117:
		case 118: case 119: case 120: case 121: case 122: case 123:
			character_pocket_data.idx=task-100;
			if(game_control.deal_type==1)break;
			if(character_pocket_data.my_deal_ready)break;
			if((character_pocket_data.idx<0)||(character_pocket_data.idx>=24))break;
			if(character_pocket_data.item[character_pocket_data.idx].item_id<0)break;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			set_deal_item_index_max();
			break;
		case 200: case 201: case 202:
			if(character_pocket_data.my_deal_ready)break;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			set_deal_item_index_del(task-200,999);
			break;
		}

		break;

	case MS_Move:
	case MS_Dummy:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// ȷ��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand27",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// ������Ʒ
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand77",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 3:		// ���׳���
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand79",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		}
		break;

	case MS_LUp:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			send_cancel_deal();
			break;
		case 1:		// ȷ��
			if(!character_pocket_data.my_deal_ready)break;
			if(!character_pocket_data.other_deal_ready)break;
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
//			if(game_control.deal_type==0)
			{
				send_item_deal();
			}
			break;
		case 2:		// ������Ʒ
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			game_control.deal_type=0;
			break;
		case 3:		// ���׳���
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			game_control.deal_type=1;
			break;

		case 100: case 101: case 102: case 103: case 104: case 105:
		case 106: case 107: case 108: case 109: case 110: case 111:
		case 112: case 113: case 114: case 115: case 116: case 117:
		case 118: case 119: case 120: case 121: case 122: case 123:
			character_pocket_data.idx=task-100;
			if(character_pocket_data.my_deal_ready)break;
			if(game_control.deal_type==0)
			{
				if((character_pocket_data.idx<0)||(character_pocket_data.idx>=24))break;
				if(character_pocket_data.item[character_pocket_data.idx].item_id<0)break;
			}
			else
			{
				if((character_pocket_data.idx<0)||(character_pocket_data.idx>=MAX_BAOBAO_STACK))break;
				if(base_baobao_data.data.baobao_list[character_pocket_data.idx].baobao_id==0)break;
			}
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			set_deal_item_index();
			break;
			
		case 200: case 201: case 202:
			if(character_pocket_data.my_deal_ready)break;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			set_deal_item_index_del(task-200,1);
			break;

		case 300:
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			if(character_pocket_data.my_deal_ready)
			{
				character_pocket_data.my_deal_ready=false;
				character_pocket_data.my_deal_idx[0]=-1;
				character_pocket_data.my_deal_idx[1]=-1;
				character_pocket_data.my_deal_idx[2]=-1;
				character_pocket_data.my_deal_number[0]=0;
				character_pocket_data.my_deal_number[1]=0;
				character_pocket_data.my_deal_number[2]=0;
				character_pocket_data.my_deal_money=0;
				
			}
			else
			{
				character_pocket_data.my_deal_ready=true;
			}
			send_my_deal_ready();
				
			break;

		}
		break;	
	case MS_LDrag:				// �϶�
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// ȷ��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand28",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// ������Ʒ
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand78",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 3:		// ���׳���
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand80",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		default:
			now_window_move_xoffset=system_control.mouse_x-window_x;
			now_window_move_yoffset=system_control.mouse_y-window_y;
			now_window_move_handle=now_exec_window_handle;
			break;
		}
		break;
		
	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}







/*******************************************************************************************************************}
{	Skill																											}
********************************************************************************************************************/
void window_draw_skill(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	SLONG i;
	SLONG idx;
	SLONG x_offset,y_offset;

	SLONG inst_x,inst_y,inst_xl,inst_yl;
	RECT test_rect;
	
	
	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	

	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;
	
	// --- ( 0 - 0 )  Draw body
	display_img256_buffer(window_x,window_y,system_image.skill_body.image,screen_buffer);	
	

	// --- ( 1 - 0 ) display data
	if(!game_control.character_skill_ready)
		return;

	for(i=0;i<9;i++)
	{
		idx=game_control.skill_start_index+i;
		if(idx>=MAX_CHARACTER_SKILL_NO)continue;

		if(character_skill_data[idx].idx>=0)
		{
			if(now_exec_window_handle==now_active_window_handle)
			{
				if((x_offset>=18)&&(x_offset<18+257)&&
					(y_offset>=71+i*18)&&(y_offset<71+i*18+18))
					mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand0",window_x+18,window_y+71+i*18,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			}

			print16(window_x+17+8,window_y+71+1+i*18,skill_base_data[idx].name,PEST_PUT,screen_buffer);
	
			sprintf((char *)print_rec,"%d",character_skill_data[idx].level);
			print16(window_x+169,window_y+71+1+i*18,print_rec,PEST_PUT,screen_buffer);
			
			sprintf((char *)print_rec,"%d",character_skill_data[idx].lithhand);
			print16(window_x+215,window_y+71+1+i*18,print_rec,PEST_PUT,screen_buffer);
		

		}
		
	}
		

	// ------ display inst
	if((system_control.mouse_x>window_x)&&(system_control.mouse_x<=window_x+window_xl)&&
		(system_control.mouse_y>window_y)&&(system_control.mouse_y<=window_y+window_yl))
	{
		if((detail_item_inst.active)&&(detail_item_inst.idx>=0))
		{
			switch(detail_item_inst.type)
			{
			case ITEM_INST_SKILL:
				if(character_skill_data[detail_item_inst.idx].idx>=0)
				{
					test_rect.left=0;
					test_rect.top=0;
					test_rect.right=160;
					test_rect.bottom=SCREEN_WIDTH;
					inst_xl=32+160;
					
					text_out_data.g_pChat->insertString((char *)detail_item_inst.inst,test_rect,false);
					inst_yl=test_rect.bottom+32;
					if(inst_yl<80)
						inst_yl=80;
					
					inst_x=system_control.mouse_x-inst_xl/2;
					inst_y=system_control.mouse_y-inst_yl-16;
					if(inst_x<=0)
						inst_x=system_control.mouse_x;
					
					if(inst_y<0)
						inst_y=system_control.mouse_y+32;
					if(inst_x+inst_xl>=SCREEN_WIDTH)inst_x=SCREEN_WIDTH-inst_xl;
					if(inst_y+inst_yl>=SCREEN_HEIGHT)inst_y=SCREEN_HEIGHT-inst_yl;
					draw_message_box(inst_x,inst_y,inst_xl,inst_yl,screen_buffer);
					put_bar(inst_x+2,inst_y+2,inst_xl-4,inst_yl-6,color_control.black,screen_buffer);
					
					
					text_out_data.g_pChat->updateString(inst_x+16,inst_y+16,test_rect.right,inst_yl,detail_item_inst.inst);
					text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,screen_buffer,false);
				}
				break;
			}
			
		}
		
		
	}


	
}



void window_exec_skill(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	

	SLONG task;
	SLONG x_offset,y_offset;
	SLONG i;
	SLONG channel;
	
	
	RECT rect[3]={
		{ 291, 9 ,291+28, 9+28	},				// �ر�			0
		{ 285,195,285+21,195+21 },				// Up			1
		{ 285,222,285+21,222+21 },				// Dn			2
	};
	

	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	

	task=-1;
	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;

	

	for(i=0;i<3;i++)
	{
		if((x_offset>=rect[i].left)&&(x_offset<rect[i].right)&&
			(y_offset>=rect[i].top)&&(y_offset<rect[i].bottom))
		{
			task=i;
			break;
		}
	}
	

	if(now_exec_window_handle==now_active_window_handle)
	{
		if(task<0)
		{
			for(i=0;i<9;i++)
			{
				if((x_offset>=18)&&(x_offset<18+257)&&
					(y_offset>=71+i*18)&&(y_offset<71+i*18+18))
				{
					task=100+i;
					break;
				}
			}
			
		}
	}

	
	if(task<0)
	{
		clear_detail_item_inst();
	}
	

	if((task>=100)&&(task<200))
	{
		detail_item_inst.idx=task-100+game_control.skill_start_index;
		if(character_skill_data[detail_item_inst.idx].idx>=0)
		{
			if(detail_item_inst.old_idx!=detail_item_inst.idx)
			{
				if(system_control.system_timer>detail_item_inst.timer+DETAIL_DELAY_TIME)
				{
					detail_item_inst.old_idx=detail_item_inst.idx;
					detail_item_inst.active=false;
					detail_item_inst.type=ITEM_INST_SKILL;
					make_detail_inst();
					detail_item_inst.timer=system_control.system_timer;
				}
			}
		}
	}
	
	
	
	switch(system_control.mouse_key)
	{
	case MS_RUp:						//	close 
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_skill=-1;
		break;

	case MS_Move:
	case MS_Dummy:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// Up
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand7",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// Dn
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand9",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		}
		break;

	case MS_LUp:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_skill=-1;
			break;
		case 1:		// Up
			if(game_control.skill_start_index>0)
				game_control.skill_start_index--;
			else
				break;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			break;
		case 2:		// Dn
			if(game_control.skill_start_index<MAX_CHARACTER_SKILL_NO-9)
				game_control.skill_start_index++;
			else
				break;
			
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			break;
		}
		break;	
	case MS_LDrag:				// �϶�
			
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// Up
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand8",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// Dn
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand10",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		default:
			now_window_move_xoffset=system_control.mouse_x-window_x;
			now_window_move_yoffset=system_control.mouse_y-window_y;
			now_window_move_handle=now_exec_window_handle;
			break;
		}
		break;


		
	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}







/*******************************************************************************************************************}
{	Title																											}
********************************************************************************************************************/
void window_draw_title(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	
	
	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	
	// --- ( 0 - 0 )  Draw body
	display_img256_buffer(window_x,window_y,system_image.title_body.image,screen_buffer);	
	
	
}



void window_exec_title(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;

	SLONG task;
	SLONG x_offset,y_offset;
	SLONG i;
	SLONG channel;
	
	
	RECT rect[5]={
		{ 224, 9 ,224+28, 9+28	},				// �ر�			0
		{ 187,196,187+21,196+21 },				// ����			1
		{ 187,223,187+21,223+21 },				// ����			2
		{ 188,104,188+63,104+21 },				// ȷ��			3
		{ 188,132,188+63,132+21 }				// ����			4
	};
	
	
	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	task=-1;
	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;
	
	for(i=0;i<5;i++)
	{
		if((x_offset>=rect[i].left)&&(x_offset<rect[i].right)&&
			(y_offset>=rect[i].top)&&(y_offset<rect[i].bottom))
		{
			task=i;
			break;
		}
	}
	
	
	
	switch(system_control.mouse_key)
	{
	case MS_RUp:						//	close 
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_title=-1;
		break;
	case MS_Move:
	case MS_Dummy:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand7",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand9",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 3:		// ȷ��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand27",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 4:		// ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand85",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		}
		break;

	case MS_LUp:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_title=-1;
			break;
		case 1:		// ����
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			break;
		case 2:		// ����
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			break;
		case 3:		// ȷ��
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			break;
		case 4:		// ����
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			break;
		}
		break;	
	case MS_LDrag:				// �϶�
			
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand8",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand10",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 3:		// ȷ��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand28",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 4:		// ����
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand86",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		default:
			now_window_move_xoffset=system_control.mouse_x-window_x;
			now_window_move_yoffset=system_control.mouse_y-window_y;
			now_window_move_handle=now_exec_window_handle;
			break;
		}
		break;

	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}



void ask_yes_no(UCHR *text, void (*yes_exec)(void),void (*no_exec)(void))
{
	if(strlen((char *)text)>79)
		text[80]=NULL;

	strcpy((char *)conform_text,(char *)text);
	main_loop_active_task=exec_conform;


	ask_yes_exec_task=yes_exec;
	ask_no_exec_task=no_exec;

}





/*******************************************************************************************************************}
{	Baobao item																										}
********************************************************************************************************************/
void window_draw_baobao_item(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	SLONG i;

	SLONG inst_x,inst_y,inst_xl,inst_yl;
	RECT test_rect;
	

	RECT item_pp[MAX_POCKET_ITEM]={
		{ 26+40*0, 56+40*0, 26+40*0+32, 56+40*0+32 },
		{ 26+40*1, 56+40*0, 26+40*1+32, 56+40*0+32 },
		{ 26+40*2, 56+40*0, 26+40*2+32, 56+40*0+32 },
		{ 26+40*3, 56+40*0, 26+40*3+32, 56+40*0+32 },
		{ 26+40*4, 56+40*0, 26+40*4+32, 56+40*0+32 },
		{ 26+40*5, 56+40*0, 26+40*5+32, 56+40*0+32 },
		
		{ 26+40*0, 56+40*1, 26+40*0+32, 56+40*1+32 },
		{ 26+40*1, 56+40*1, 26+40*1+32, 56+40*1+32 },
		{ 26+40*2, 56+40*1, 26+40*2+32, 56+40*1+32 },
		{ 26+40*3, 56+40*1, 26+40*3+32, 56+40*1+32 },
		{ 26+40*4, 56+40*1, 26+40*4+32, 56+40*1+32 },
		{ 26+40*5, 56+40*1, 26+40*5+32, 56+40*1+32 },
		
		{ 26+40*0, 56+40*2, 26+40*0+32, 56+40*2+32 },
		{ 26+40*1, 56+40*2, 26+40*1+32, 56+40*2+32 },
		{ 26+40*2, 56+40*2, 26+40*2+32, 56+40*2+32 },
		{ 26+40*3, 56+40*2, 26+40*3+32, 56+40*2+32 },
		{ 26+40*4, 56+40*2, 26+40*4+32, 56+40*2+32 },
		{ 26+40*5, 56+40*2, 26+40*5+32, 56+40*2+32 },
		
		{ 26+40*0, 56+40*3, 26+40*0+32, 56+40*3+32 },
		{ 26+40*1, 56+40*3, 26+40*1+32, 56+40*3+32 },
		{ 26+40*2, 56+40*3, 26+40*2+32, 56+40*3+32 },
		{ 26+40*3, 56+40*3, 26+40*3+32, 56+40*3+32 },
		{ 26+40*4, 56+40*3, 26+40*4+32, 56+40*3+32 },
		{ 26+40*5, 56+40*3, 26+40*5+32, 56+40*3+32 }
		
	};
	
	
	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	
	// --- ( 0 - 0 )  Draw body
	display_img256_buffer(window_x,window_y,system_image.item_public_body.image,screen_buffer);	
	display_img256_buffer(window_x+2,window_y+(system_image.item_public_body.rect.bottom-system_image.item_public_body.rect.top),system_image.baobao_item_action.image,screen_buffer);	
	display_img256_buffer(window_x+107,window_y+14,system_image.baobao_item_mask.image,screen_buffer);	

	// --- ( 1 - 0 ) display data
	if(character_pocket_data.active)
	{
		for(i=0;i<MAX_POCKET_ITEM;i++)
		{
			
			if((character_pocket_data.item[i].number>0)&&(character_pocket_data.item[i].filename_id>=0))
			{
				
				display_img256_buffer(window_x+item_pp[i].left,window_y+item_pp[i].top,
					system_image.item_image[character_pocket_data.item[i].filename_id],screen_buffer);
				
				if(character_pocket_data.item[i].number>1)
				{
					sprintf((char *)print_rec,"%d",character_pocket_data.item[i].number);
					print12(window_x+item_pp[i].right-(strlen((char *)print_rec)*6)-6-1,window_y+item_pp[i].bottom-10-1,print_rec,COPY_PUT,screen_buffer);
					
				}
				
			}
			
			if(i==character_pocket_data.idx)
			{
				put_scroll_box(window_x+item_pp[i].left-1,
					window_y+item_pp[i].top-1,
					window_x+item_pp[i].right+1,
					window_y+item_pp[i].bottom+1,color_control.red,color_control.yellow,screen_buffer);
				put_scroll_box(window_x+item_pp[i].left-2,
					window_y+item_pp[i].top-2,
					window_x+item_pp[i].right+2,
					window_y+item_pp[i].bottom+2,color_control.red,color_control.yellow,screen_buffer);
			}
			
		}
		
	}
	

	
	// ------ display inst
	if((system_control.mouse_x>window_x)&&(system_control.mouse_x<=window_x+window_xl)&&
		(system_control.mouse_y>window_y)&&(system_control.mouse_y<=window_y+window_yl))
	{
		if((detail_item_inst.active)&&(detail_item_inst.idx>=0))
		{
			switch(detail_item_inst.type)
			{
			case ITEM_INST_POCKET:
				if((character_pocket_data.item[detail_item_inst.idx].number>0)&&(character_pocket_data.item[detail_item_inst.idx].filename_id>=0))
				{
					test_rect.left=0;
					test_rect.top=0;
					test_rect.right=160;
					test_rect.bottom=SCREEN_WIDTH;
					inst_xl=32+32*2+160;
					
					text_out_data.g_pChat->insertString((char *)detail_item_inst.inst,test_rect,false);
					inst_yl=test_rect.bottom+32;
					if(inst_yl<80)
						inst_yl=80;
					
					inst_x=system_control.mouse_x-inst_xl/2;
					inst_y=system_control.mouse_y-inst_yl-16;
					if(inst_x<=0)
						inst_x=system_control.mouse_x;
					
					if(inst_y<0)
						inst_y=system_control.mouse_y+32;
					if(inst_x+inst_xl>=SCREEN_WIDTH)inst_x=SCREEN_WIDTH-inst_xl;
					if(inst_y+inst_yl>=SCREEN_HEIGHT)inst_y=SCREEN_HEIGHT-inst_yl;
					draw_message_box(inst_x,inst_y,inst_xl,inst_yl,screen_buffer);
					put_bar(inst_x+2,inst_y+2,inst_xl-4,inst_yl-6,color_control.black,screen_buffer);
					
					display_zoom_img256_buffer(inst_x+16,inst_y+16,
						system_image.item_image[character_pocket_data.item[detail_item_inst.idx].filename_id],screen_buffer,200);
					
					text_out_data.g_pChat->updateString(inst_x+16+64,inst_y+16,test_rect.right,inst_yl,detail_item_inst.inst);
					text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,screen_buffer,false);
				}
				
				break;
			}
			
		}
	}
	

}



void window_exec_baobao_item(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;

	SLONG task;
	SLONG x_offset,y_offset;
	SLONG i;
	SLONG channel;
	SLONG baobao_id;

	RECT item_pp[MAX_POCKET_ITEM]={
		{ 26+40*0, 56+40*0, 26+40*0+32, 56+40*0+32 },
		{ 26+40*1, 56+40*0, 26+40*1+32, 56+40*0+32 },
		{ 26+40*2, 56+40*0, 26+40*2+32, 56+40*0+32 },
		{ 26+40*3, 56+40*0, 26+40*3+32, 56+40*0+32 },
		{ 26+40*4, 56+40*0, 26+40*4+32, 56+40*0+32 },
		{ 26+40*5, 56+40*0, 26+40*5+32, 56+40*0+32 },
		
		{ 26+40*0, 56+40*1, 26+40*0+32, 56+40*1+32 },
		{ 26+40*1, 56+40*1, 26+40*1+32, 56+40*1+32 },
		{ 26+40*2, 56+40*1, 26+40*2+32, 56+40*1+32 },
		{ 26+40*3, 56+40*1, 26+40*3+32, 56+40*1+32 },
		{ 26+40*4, 56+40*1, 26+40*4+32, 56+40*1+32 },
		{ 26+40*5, 56+40*1, 26+40*5+32, 56+40*1+32 },
		
		{ 26+40*0, 56+40*2, 26+40*0+32, 56+40*2+32 },
		{ 26+40*1, 56+40*2, 26+40*1+32, 56+40*2+32 },
		{ 26+40*2, 56+40*2, 26+40*2+32, 56+40*2+32 },
		{ 26+40*3, 56+40*2, 26+40*3+32, 56+40*2+32 },
		{ 26+40*4, 56+40*2, 26+40*4+32, 56+40*2+32 },
		{ 26+40*5, 56+40*2, 26+40*5+32, 56+40*2+32 },
		
		{ 26+40*0, 56+40*3, 26+40*0+32, 56+40*3+32 },
		{ 26+40*1, 56+40*3, 26+40*1+32, 56+40*3+32 },
		{ 26+40*2, 56+40*3, 26+40*2+32, 56+40*3+32 },
		{ 26+40*3, 56+40*3, 26+40*3+32, 56+40*3+32 },
		{ 26+40*4, 56+40*3, 26+40*4+32, 56+40*3+32 },
		{ 26+40*5, 56+40*3, 26+40*5+32, 56+40*3+32 }
		
	};
	
	
	RECT rect[2]={
		{ 247, 9 ,247+28, 9+28	},				// �ر�			0
		{ 114,226,114+63,226+21 },				// ȷ��			1
	};
	
	
	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	task=-1;
	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;
	
	for(i=0;i<2;i++)
	{
		if((x_offset>=rect[i].left)&&(x_offset<rect[i].right)&&
			(y_offset>=rect[i].top)&&(y_offset<rect[i].bottom))
		{
			task=i;
			break;
		}
	}
	

	if(task<0)
	{
		for(i=0;i<MAX_POCKET_ITEM;i++)
		{
			if((x_offset>=item_pp[i].left)&&(x_offset<item_pp[i].right)&&
				(y_offset>=item_pp[i].top)&&(y_offset<item_pp[i].bottom))
			{
				task=100+i;
				break;
			}
			
		}
	}
	

	
	if(task<0)
	{
		clear_detail_item_inst();
	}
	
	if((task>=100)&&(task<200))
	{
		detail_item_inst.idx=task-100;
		if((character_pocket_data.item[detail_item_inst.idx].number>0)&&(character_pocket_data.item[detail_item_inst.idx].filename_id>=0))
		{
			if(detail_item_inst.old_idx!=detail_item_inst.idx)
			{
				if(system_control.system_timer>detail_item_inst.timer+DETAIL_DELAY_TIME)
				{
					detail_item_inst.old_idx=detail_item_inst.idx;
					detail_item_inst.active=false;
					detail_item_inst.type=ITEM_INST_POCKET;
					request_detail_inst();
					detail_item_inst.timer=system_control.system_timer;
				}
			}
		}
	}
	
	

	switch(system_control.mouse_key)
	{
	case MS_RUp:						//	close 
		clear_detail_item_inst();
		if(task<0)				// close
		{
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_baobao_item=-1;
		}
		break;
	case MS_Move:
	case MS_Dummy:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// ȷ��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand90",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		}
		break;

	case MS_LUp:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_baobao_item=-1;
			break;
		case 1:		// ȷ��
			check_baobao_stack();
			if(character_pocket_data.idx<0)break;
			if(character_pocket_data.item[character_pocket_data.idx].number<=0)break;
			if(character_pocket_data.item[character_pocket_data.idx].item_id<0)break;
			if(baobao_stack_index<0)return;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)return;
			baobao_id=base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id;
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			send_baobao_item_use(baobao_id,character_pocket_data.idx);
			break;

		case 100: case 101: case 102: case 103: case 104: case 105:
		case 106: case 107: case 108: case 109: case 110: case 111:
		case 112: case 113: case 114: case 115: case 116: case 117:
		case 118: case 119: case 120: case 121: case 122: case 123:
			character_pocket_data.idx=task-100;
			break;
			
		}
		break;	
	case MS_LDrag:				// �϶�
			
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// ȷ��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand91",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		default:
			now_window_move_xoffset=system_control.mouse_x-window_x;
			now_window_move_yoffset=system_control.mouse_y-window_y;
			now_window_move_handle=now_exec_window_handle;
			break;
		}
		break;

	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}






/*******************************************************************************************************************}
{	character fastness																								}
********************************************************************************************************************/
void window_draw_character_fastness(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	SHINT back_color=rgb2hi(75,0,61);
	SLONG ll;
	
	
	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	

	// --- ( 0 - 0 )  Draw body
	draw_message_box(window_x,window_y,window_xl,window_yl,screen_buffer);
	put_bar(window_x+2,window_y+2,window_xl-4,window_yl-4,back_color,screen_buffer);
	if(!character_fastness_data.active)return;
	
		
	// --- ( 1 - 0 ) display data
	sprintf((char *)print_rec,MSG_FASTNESS_PROTECT,character_fastness_data.fastness.protect);
	print16(window_x+16,window_y+16+16*0,print_rec,PEST_PUT,screen_buffer);		// ������

	sprintf((char *)print_rec,MSG_FASTNESS_SHOOT,character_fastness_data.fastness.shoot);
	print16(window_x+16,window_y+16+16*1,print_rec,PEST_PUT,screen_buffer);		// ������
	
	sprintf((char *)print_rec,MSG_FASTNESS_DODGE,character_fastness_data.fastness.dodge);
	print16(window_x+16,window_y+16+16*2,print_rec,PEST_PUT,screen_buffer);		// �����
	
	sprintf((char *)print_rec,MSG_FASTNESS_SUCK,character_fastness_data.fastness.suck);
	print16(window_x+16,window_y+16+16*3,print_rec,PEST_PUT,screen_buffer);		// ����

	sprintf((char *)print_rec,MSG_FASTNESS_CRAZY,character_fastness_data.fastness.crazy_percent,character_fastness_data.fastness.crazy_value);
	print16(window_x+16,window_y+16+16*4,print_rec,PEST_PUT,screen_buffer);		// �񱩼���,��Ч��

	sprintf((char *)print_rec,MSG_FASTNESS_DEADLINESS,character_fastness_data.fastness.deadliness_percent,character_fastness_data.fastness.deadliness_value);
	print16(window_x+16,window_y+16+16*5,print_rec,PEST_PUT,screen_buffer);		// ��������,����Ч��
	
	sprintf((char *)print_rec,MSG_FASTNESS_REDEADLINESS,character_fastness_data.fastness.redeadliness_percent);
	print16(window_x+16,window_y+16+16*6,print_rec,PEST_PUT,screen_buffer);		// ����������
	
	sprintf((char *)print_rec,MSG_FASTNESS_DOUBLEATT,character_fastness_data.fastness.doubleatt_percent,character_fastness_data.fastness.doubleatt_times);
	print16(window_x+16,window_y+16+16*7,print_rec,PEST_PUT,screen_buffer);		// ��������,��������

	sprintf((char *)print_rec,MSG_FASTNESS_BACKATT,character_fastness_data.fastness.backatt_percent,character_fastness_data.fastness.backatt_times);
	print16(window_x+16,window_y+16+16*8,print_rec,PEST_PUT,screen_buffer);		// ��������,��������
	
	sprintf((char *)print_rec,MSG_FASTNESS_BREAK_PHYSICS,character_fastness_data.fastness.break_physics_percent,character_fastness_data.fastness.break_physics_value);
	print16(window_x+16,window_y+16+16*9,print_rec,PEST_PUT,screen_buffer);		// ��������,������̶�
	
	sprintf((char *)print_rec,MSG_FASTNESS_MAGIC_FEEDBACK,character_fastness_data.fastness.magic_feedback_percent,character_fastness_data.fastness.magic_feedback_value);
	print16(window_x+16,window_y+16+16*10,print_rec,PEST_PUT,screen_buffer);		// ������������,���������̶�

	sprintf((char *)print_rec,MSG_FASTNESS_PHYSICS_FEEDBACK,character_fastness_data.fastness.physics_feedback_percent,character_fastness_data.fastness.physics_feedback_value);
	print16(window_x+16,window_y+16+16*11,print_rec,PEST_PUT,screen_buffer);	// ����������,�������̶�

	sprintf((char *)print_rec,MSG_FASTNESS_DEAD,character_fastness_data.fastness.dead_percent);
	print16(window_x+16,window_y+16+16*12,print_rec,PEST_PUT,screen_buffer);	// ���������漸��
	
	sprintf((char *)print_rec,MSG_FASTNESS_MAGIC_DARK,character_fastness_data.fastness.magic_dark);
	print16(window_x+16,window_y+16+16*13,print_rec,PEST_PUT,screen_buffer);	// ���������̶�

	sprintf((char *)print_rec,MSG_FASTNESS_MAGIC_LIGHT,character_fastness_data.fastness.magic_light);
	print16(window_x+16,window_y+16+16*14,print_rec,PEST_PUT,screen_buffer);	// ���ⷨ���̶�

	sprintf((char *)print_rec,MSG_FASTNESS_MAGIC_WATER,character_fastness_data.fastness.magic_water);
	print16(window_x+16,window_y+16+16*15,print_rec,PEST_PUT,screen_buffer);	// ��ˮ�����̶�

	sprintf((char *)print_rec,MSG_FASTNESS_MAGIC_FIRE,character_fastness_data.fastness.magic_fire);
	print16(window_x+16,window_y+16+16*16,print_rec,PEST_PUT,screen_buffer);	// �������̶�

	sprintf((char *)print_rec,MSG_FASTNESS_PHYSICS,character_fastness_data.fastness.physics);
	print16(window_x+16,window_y+16+16*17,print_rec,PEST_PUT,screen_buffer);	// ������̶�
	
	sprintf((char *)print_rec,MSG_FASTNESS_VIRUS,character_fastness_data.fastness.virus);
	print16(window_x+16,window_y+16+16*18,print_rec,PEST_PUT,screen_buffer);	// ���ж��̶�
	
	sprintf((char *)print_rec,MSG_FASTNESS_CHAOS,character_fastness_data.fastness.chaos);
	print16(window_x+16,window_y+16+16*19,print_rec,PEST_PUT,screen_buffer);	// �����ҳ̶�
	
	sprintf((char *)print_rec,MSG_FASTNESS_STONE,character_fastness_data.fastness.stone);
	print16(window_x+16,window_y+16+16*20,print_rec,PEST_PUT,screen_buffer);	// ��ʯ���̶�
	
	sprintf((char *)print_rec,MSG_FASTNESS_SLEEP,character_fastness_data.fastness.sleep);
	print16(window_x+16,window_y+16+16*21,print_rec,PEST_PUT,screen_buffer);	// ����˯�̶�

	sprintf((char *)print_rec,MSG_FASTNESS_WEEK,character_fastness_data.fastness.week);
	print16(window_x+16,window_y+16+16*22,print_rec,PEST_PUT,screen_buffer);	// ��˥�ϳ̶�
	

	ll=(SLONG)strlen(base_character_data.name)*8;
	switch(base_character_data.data.base.life_base)
	{
	case 0:
		set_word_color(0,color_control.life_base0);
	case 1:
		set_word_color(0,color_control.life_base1);
	case 2:
		set_word_color(0,color_control.life_base2);
	case 3:
		set_word_color(0,color_control.life_base3);
	default:
		set_word_color(0,color_control.life_base0);
		break;
	}
	print16(window_x+(window_xl/2)-(ll/2),window_y+16+16*23+8,base_character_data.name,PEST_PUT,screen_buffer);	// name
	set_word_color(0,color_control.white);
	

}



void window_exec_character_fastness(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	
	
	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	
	
	switch(system_control.mouse_key)
	{
	case MS_RUp:						//	close 
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_character_fastness=-1;
		break;


	case MS_LDrag:				// �϶�
		now_window_move_xoffset=system_control.mouse_x-window_x;
		now_window_move_yoffset=system_control.mouse_y-window_y;
		now_window_move_handle=now_exec_window_handle;
		break;
		
	}
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}




/*******************************************************************************************************************}
{	baobao fastness																									}
********************************************************************************************************************/
void window_draw_baobao_fastness(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	SHINT back_color=rgb2hi(75,0,61);
	SLONG ll;
	
	
	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;



	if(baobao_stack_index<0)return;
	if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)return;
	
	// --- ( 0 - 0 )  Draw body
	draw_message_box(window_x,window_y,window_xl,window_yl,screen_buffer);
	put_bar(window_x+2,window_y+2,window_xl-4,window_yl-4,back_color,screen_buffer);
	if(!baobao_fastness_data[baobao_stack_index].active)return;
	

	
	// --- ( 1 - 0 ) display data
	sprintf((char *)print_rec,MSG_FASTNESS_PROTECT,baobao_fastness_data[baobao_stack_index].fastness.protect);
	print16(window_x+16,window_y+16+16*0,print_rec,PEST_PUT,screen_buffer);		// ������

	sprintf((char *)print_rec,MSG_FASTNESS_SHOOT,baobao_fastness_data[baobao_stack_index].fastness.shoot);
	print16(window_x+16,window_y+16+16*1,print_rec,PEST_PUT,screen_buffer);		// ������
	
	sprintf((char *)print_rec,MSG_FASTNESS_DODGE,baobao_fastness_data[baobao_stack_index].fastness.dodge);
	print16(window_x+16,window_y+16+16*2,print_rec,PEST_PUT,screen_buffer);		// �����
	
	sprintf((char *)print_rec,MSG_FASTNESS_SUCK,baobao_fastness_data[baobao_stack_index].fastness.suck);
	print16(window_x+16,window_y+16+16*3,print_rec,PEST_PUT,screen_buffer);		// ����

	sprintf((char *)print_rec,MSG_FASTNESS_CRAZY,baobao_fastness_data[baobao_stack_index].fastness.crazy_percent,baobao_fastness_data[baobao_stack_index].fastness.crazy_value);
	print16(window_x+16,window_y+16+16*4,print_rec,PEST_PUT,screen_buffer);		// �񱩼���,��Ч��

	sprintf((char *)print_rec,MSG_FASTNESS_DEADLINESS,baobao_fastness_data[baobao_stack_index].fastness.deadliness_percent,baobao_fastness_data[baobao_stack_index].fastness.deadliness_value);
	print16(window_x+16,window_y+16+16*5,print_rec,PEST_PUT,screen_buffer);		// ��������,����Ч��
	
	sprintf((char *)print_rec,MSG_FASTNESS_REDEADLINESS,baobao_fastness_data[baobao_stack_index].fastness.redeadliness_percent);
	print16(window_x+16,window_y+16+16*6,print_rec,PEST_PUT,screen_buffer);		// ����������
	
	sprintf((char *)print_rec,MSG_FASTNESS_DOUBLEATT,baobao_fastness_data[baobao_stack_index].fastness.doubleatt_percent,baobao_fastness_data[baobao_stack_index].fastness.doubleatt_times);
	print16(window_x+16,window_y+16+16*7,print_rec,PEST_PUT,screen_buffer);		// ��������,��������

	sprintf((char *)print_rec,MSG_FASTNESS_BACKATT,baobao_fastness_data[baobao_stack_index].fastness.backatt_percent,baobao_fastness_data[baobao_stack_index].fastness.backatt_times);
	print16(window_x+16,window_y+16+16*8,print_rec,PEST_PUT,screen_buffer);		// ��������,��������
	
	sprintf((char *)print_rec,MSG_FASTNESS_BREAK_PHYSICS,baobao_fastness_data[baobao_stack_index].fastness.break_physics_percent,baobao_fastness_data[baobao_stack_index].fastness.break_physics_value);
	print16(window_x+16,window_y+16+16*9,print_rec,PEST_PUT,screen_buffer);		// ��������,������̶�
	
	sprintf((char *)print_rec,MSG_FASTNESS_MAGIC_FEEDBACK,baobao_fastness_data[baobao_stack_index].fastness.magic_feedback_percent,baobao_fastness_data[baobao_stack_index].fastness.magic_feedback_value);
	print16(window_x+16,window_y+16+16*10,print_rec,PEST_PUT,screen_buffer);	// ������������,���������̶�

	sprintf((char *)print_rec,MSG_FASTNESS_PHYSICS_FEEDBACK,baobao_fastness_data[baobao_stack_index].fastness.physics_feedback_percent,baobao_fastness_data[baobao_stack_index].fastness.physics_feedback_value);
	print16(window_x+16,window_y+16+16*11,print_rec,PEST_PUT,screen_buffer);	// ����������,�������̶�

	sprintf((char *)print_rec,MSG_FASTNESS_DEAD,baobao_fastness_data[baobao_stack_index].fastness.dead_percent);
	print16(window_x+16,window_y+16+16*12,print_rec,PEST_PUT,screen_buffer);	// ���������漸��
	
	sprintf((char *)print_rec,MSG_FASTNESS_MAGIC_DARK,baobao_fastness_data[baobao_stack_index].fastness.magic_dark);
	print16(window_x+16,window_y+16+16*13,print_rec,PEST_PUT,screen_buffer);	// ���������̶�

	sprintf((char *)print_rec,MSG_FASTNESS_MAGIC_LIGHT,baobao_fastness_data[baobao_stack_index].fastness.magic_light);
	print16(window_x+16,window_y+16+16*14,print_rec,PEST_PUT,screen_buffer);	// ���ⷨ���̶�

	sprintf((char *)print_rec,MSG_FASTNESS_MAGIC_WATER,baobao_fastness_data[baobao_stack_index].fastness.magic_water);
	print16(window_x+16,window_y+16+16*15,print_rec,PEST_PUT,screen_buffer);	// ��ˮ�����̶�

	sprintf((char *)print_rec,MSG_FASTNESS_MAGIC_FIRE,baobao_fastness_data[baobao_stack_index].fastness.magic_fire);
	print16(window_x+16,window_y+16+16*16,print_rec,PEST_PUT,screen_buffer);	// �������̶�

	sprintf((char *)print_rec,MSG_FASTNESS_PHYSICS,baobao_fastness_data[baobao_stack_index].fastness.physics);
	print16(window_x+16,window_y+16+16*17,print_rec,PEST_PUT,screen_buffer);	// ������̶�
	
	sprintf((char *)print_rec,MSG_FASTNESS_VIRUS,baobao_fastness_data[baobao_stack_index].fastness.virus);
	print16(window_x+16,window_y+16+16*18,print_rec,PEST_PUT,screen_buffer);	// ���ж��̶�
	
	sprintf((char *)print_rec,MSG_FASTNESS_CHAOS,baobao_fastness_data[baobao_stack_index].fastness.chaos);
	print16(window_x+16,window_y+16+16*19,print_rec,PEST_PUT,screen_buffer);	// �����ҳ̶�
	
	sprintf((char *)print_rec,MSG_FASTNESS_STONE,baobao_fastness_data[baobao_stack_index].fastness.stone);
	print16(window_x+16,window_y+16+16*20,print_rec,PEST_PUT,screen_buffer);	// ��ʯ���̶�
	
	sprintf((char *)print_rec,MSG_FASTNESS_SLEEP,baobao_fastness_data[baobao_stack_index].fastness.sleep);
	print16(window_x+16,window_y+16+16*21,print_rec,PEST_PUT,screen_buffer);	// ����˯�̶�

	sprintf((char *)print_rec,MSG_FASTNESS_WEEK,baobao_fastness_data[baobao_stack_index].fastness.week);
	print16(window_x+16,window_y+16+16*22,print_rec,PEST_PUT,screen_buffer);	// ��˥�ϳ̶�
	

	ll=(SLONG)strlen(base_baobao_data.data.baobao_list[baobao_stack_index].name)*8;

	switch(base_baobao_data.data.baobao_list[baobao_stack_index].base.life_base)
	{
	case 0:
		set_word_color(0,color_control.life_base0);
	case 1:
		set_word_color(0,color_control.life_base1);
	case 2:
		set_word_color(0,color_control.life_base2);
	case 3:
		set_word_color(0,color_control.life_base3);
	default:
		set_word_color(0,color_control.life_base0);
		break;
	}

	print16(window_x+(window_xl/2)-(ll/2),window_y+16+16*23+8,base_baobao_data.data.baobao_list[baobao_stack_index].name,PEST_PUT,screen_buffer);	// name
	set_word_color(0,color_control.white);
	
}



void window_exec_baobao_fastness(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	
	
	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	
	
	switch(system_control.mouse_key)
	{
	case MS_RUp:						//	close 
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_baobao_fastness=-1;
		break;


	case MS_LDrag:				// �϶�
		now_window_move_xoffset=system_control.mouse_x-window_x;
		now_window_move_yoffset=system_control.mouse_y-window_y;
		now_window_move_handle=now_exec_window_handle;
		break;
		
	}
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}





/*******************************************************************************************************************}
{	Fight character skill 																							}
********************************************************************************************************************/
void window_draw_fight_character_skill(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	SLONG i;
	SLONG id;
	RECT rect[MAX_CHARACTER_SKILL_NO];
	SLONG task;
	SLONG x_offset,y_offset;
	SLONG name_xl;
	
	if(fight_control_data.character_idx<0)return;
	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;

	for(i=0;i<MAX_CHARACTER_SKILL_NO;i++)
	{
		rect[i].left=16;
		rect[i].top=16+i*18;
		rect[i].right=rect[i].left+(MAX_SKILL_NAME_LENGTH*8);
		rect[i].bottom=rect[i].top+16;
	}

	task=-1;
	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;
	
	for(i=0;i<MAX_CHARACTER_SKILL_NO;i++)
	{
		if((x_offset>=rect[i].left)&&(x_offset<rect[i].right)&&
			(y_offset>=rect[i].top)&&(y_offset<rect[i].bottom))
		{
			task=i;
			break;
		}
	}
	
	
	
	// --- ( 0 - 0 )  Draw body
	draw_message_box(window_x,window_y,window_xl,window_yl,screen_buffer);
	put_bar(window_x+2,window_y+2,window_xl-4,window_yl-4,color_control.window_blue,screen_buffer);

	if(fight_npc_group[fight_control_data.character_idx].base.active)
	{
		name_xl=strlen((char *)fight_npc_group[fight_control_data.character_idx].base.name)*8;
		set_word_color(0,color_control.black);
		print16(window_x+window_xl/2-name_xl/2-1, window_y+16+MAX_CHARACTER_SKILL_NO*18+2-1,fight_npc_group[fight_control_data.character_idx].base.name,PEST_PUT,screen_buffer);
		switch(fight_npc_group[fight_control_data.character_idx].base.life_base)
		{
		case 0:
			set_word_color(0,color_control.life_base0);
			break;
		case 1:
			set_word_color(0,color_control.life_base1);
			break;
		case 2:
			set_word_color(0,color_control.life_base2);
			break;
		case 3:
			set_word_color(0,color_control.life_base3);
			break;
		default:
			set_word_color(0,color_control.life_base0);
			break;
		}
		print16(window_x+window_xl/2-name_xl/2, window_y+16+MAX_CHARACTER_SKILL_NO*18+2,fight_npc_group[fight_control_data.character_idx].base.name,PEST_PUT,screen_buffer);
		set_word_color(0,color_control.white);
		
	}

	// --- ( 1 - 0 ) display data
	if(game_control.character_skill_ready==false)return;

	for(i=0;i<MAX_CHARACTER_SKILL_NO;i++)
	{
		if(now_exec_window_handle==now_active_window_handle)
		{
			if(task==i)
			{
				if(character_skill_data[i].idx>=0)
					put_bar(window_x+rect[i].left,window_y+rect[i].top,rect[i].right-rect[i].left,rect[i].bottom-rect[i].top,color_control.pink,screen_buffer);
			}
		}
		id=character_skill_data[i].idx;
		if(id<0)continue;

		if(character_skill_data[i].need_mp<=fight_npc_group[fight_control_data.character_idx].base.now_mp)
		{
			sprintf((char *)print_rec,"%s",(char *)skill_base_data[id].name);
			print16(window_x+16,window_y+16+i*18,print_rec,PEST_PUT,screen_buffer);
		}
		else
		{
			set_word_color(0,color_control.gray);
			sprintf((char *)print_rec,"%s",(char *)skill_base_data[id].name);
			print16(window_x+16,window_y+16+i*18,print_rec,PEST_PUT,screen_buffer);
			set_word_color(0,color_control.white);
		}
			

	}


	draw_message_box(window_x,window_y-32-16,window_xl,32+16,screen_buffer);

	
	if(now_exec_window_handle==now_active_window_handle)
	{
		put_bar(window_x+2,window_y+2-32-16,window_xl-4,32+16-4,color_control.blue,screen_buffer);
		
		if(task>=0)
		{
			if(character_skill_data[task].idx>=0)
			{
				sprintf((char *)print_rec,"����MP:%d",character_skill_data[task].need_mp);
				print16(window_x+2+8,window_y+2-32-16+8,print_rec,PEST_PUT,screen_buffer);
			}
		}
	}
	else
		put_bar(window_x+2,window_y+2-32-16,window_xl-4,32+16-4,color_control.window_blue,screen_buffer);
	
	sprintf((char *)print_rec,"~C4��ǰMP:%d~C0",fight_npc_group[fight_control_data.character_idx].base.now_mp);
	print16(window_x+2+8,window_y+2-32+8,print_rec,PEST_PUT,screen_buffer);
	
}



void window_exec_fight_character_skill(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	
	
	SLONG task;
	SLONG x_offset,y_offset;
	SLONG i;
	SLONG channel;
	SLONG id;
	
	RECT rect[MAX_CHARACTER_SKILL_NO];
	

	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;

	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	for(i=0;i<MAX_CHARACTER_SKILL_NO;i++)
	{
		rect[i].left=16;
		rect[i].top=16+i*18;
		rect[i].right=rect[i].left+(MAX_SKILL_NAME_LENGTH*8);
		rect[i].bottom=rect[i].top+16;
	}
	
	
	task=-1;
	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;

	
	for(i=0;i<MAX_CHARACTER_SKILL_NO;i++)
	{
		if((x_offset>=rect[i].left)&&(x_offset<rect[i].right)&&
			(y_offset>=rect[i].top)&&(y_offset<rect[i].bottom))
		{
			id=character_skill_data[i].idx;
			if(id>=0)
				task=i;
			break;
		}
	}
	
	
	switch(system_control.mouse_key)
	{
	case MS_RUp:						//	close 
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_fight_character_skill=-1;
		auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
		break;

	case MS_Move:
	case MS_Dummy:
		if((task<0)||(task>=MAX_CHARACTER_SKILL_NO))break;
		break;

	case MS_LUp:
		if((task<0)||(task>=MAX_CHARACTER_SKILL_NO))break;
		id=character_skill_data[task].idx;
		if(id<0)break;
		character_fight_command_data.type_no=id;			// �趨����id
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_fight_character_skill=-1;
		auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
		break;	

	case MS_LDrag:				// �϶�
		if(task<0)
		{
			now_window_move_xoffset=system_control.mouse_x-window_x;
			now_window_move_yoffset=system_control.mouse_y-window_y;
			now_window_move_handle=now_exec_window_handle;
		}
		break;
		
	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}




/*******************************************************************************************************************}
{	Fight baobao skill																								}
********************************************************************************************************************/
void window_draw_fight_baobao_skill(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	SLONG i;
	SLONG id;
	RECT rect[MAX_BAOBAO_SKILL_NO];
	SLONG task;
	SLONG x_offset,y_offset;
	SLONG name_xl;
	
	if(fight_control_data.baobao_idx<0)return;
	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;

	for(i=0;i<MAX_BAOBAO_SKILL_NO;i++)
	{
		rect[i].left=16;
		rect[i].top=16+i*18;
		rect[i].right=rect[i].left+(MAX_SKILL_NAME_LENGTH*8);
		rect[i].bottom=rect[i].top+16;
	}

	task=-1;
	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;
	
	for(i=0;i<MAX_BAOBAO_SKILL_NO;i++)
	{
		if((x_offset>=rect[i].left)&&(x_offset<rect[i].right)&&
			(y_offset>=rect[i].top)&&(y_offset<rect[i].bottom))
		{
			task=i;
			break;
		}
	}
	
	
	
	// --- ( 0 - 0 )  Draw body
	draw_message_box(window_x,window_y,window_xl,window_yl,screen_buffer);
	put_bar(window_x+2,window_y+2,window_xl-4,window_yl-4,color_control.window_blue,screen_buffer);

	if(fight_npc_group[fight_control_data.baobao_idx].base.active)
	{
		name_xl=strlen((char *)fight_npc_group[fight_control_data.baobao_idx].base.name)*8;
		set_word_color(0,color_control.black);
		print16(window_x+window_xl/2-name_xl/2-1, window_y+16+MAX_BAOBAO_SKILL_NO*18+2-1,fight_npc_group[fight_control_data.baobao_idx].base.name,PEST_PUT,screen_buffer);
		switch(fight_npc_group[fight_control_data.baobao_idx].base.life_base)
		{
		case 0:
			set_word_color(0,color_control.life_base0);
			break;
		case 1:
			set_word_color(0,color_control.life_base1);
			break;
		case 2:
			set_word_color(0,color_control.life_base2);
			break;
		case 3:
			set_word_color(0,color_control.life_base3);
			break;
		default:
			set_word_color(0,color_control.life_base0);
			break;
		}
		print16(window_x+window_xl/2-name_xl/2, window_y+16+MAX_BAOBAO_SKILL_NO*18+2,fight_npc_group[fight_control_data.baobao_idx].base.name,PEST_PUT,screen_buffer);
		set_word_color(0,color_control.white);
		
	}

	// --- ( 1 - 0 ) display data
	if(game_control.baobao_skill_ready==false)return;

	for(i=0;i<MAX_BAOBAO_SKILL_NO;i++)
	{
		if(now_exec_window_handle==now_active_window_handle)
		{
			if(task==i)
			{
				if(baobao_skill_data[i].skill_id>=0)
					put_bar(window_x+rect[i].left,window_y+rect[i].top,rect[i].right-rect[i].left,rect[i].bottom-rect[i].top,color_control.pink,screen_buffer);
			}
		}
		id=baobao_skill_data[i].skill_id;
		if(id<0)continue;

		if(skill_base_data[baobao_skill_data[i].skill_id].mp<=fight_npc_group[fight_control_data.baobao_idx].base.now_mp)
		{
			sprintf((char *)print_rec,"%s",(char *)skill_base_data[id].name);
			print16(window_x+16,window_y+16+i*18,print_rec,PEST_PUT,screen_buffer);
		}
		else
		{
			set_word_color(0,color_control.gray);
			sprintf((char *)print_rec,"%s",(char *)skill_base_data[id].name);
			print16(window_x+16,window_y+16+i*18,print_rec,PEST_PUT,screen_buffer);
			set_word_color(0,color_control.white);
		}

	}


	draw_message_box(window_x,window_y-32-16,window_xl,32+16,screen_buffer);
	put_bar(window_x+2,window_y+2-32-16,window_xl-4,32+16-4,color_control.blue,screen_buffer);


	if(now_exec_window_handle==now_active_window_handle)
	{
		put_bar(window_x+2,window_y+2-32-16,window_xl-4,32+16-4,color_control.blue,screen_buffer);
		
		if(task>=0)
		{
			if(baobao_skill_data[task].skill_id>=0)
			{
				sprintf((char *)print_rec,"����MP:%d",skill_base_data[baobao_skill_data[task].skill_id].mp);
				print16(window_x+2+8,window_y+2-32-16+8,print_rec,PEST_PUT,screen_buffer);
			}
		}
	}
	else
		put_bar(window_x+2,window_y+2-32-16,window_xl-4,32+16-4,color_control.window_blue,screen_buffer);
	
	
	sprintf((char *)print_rec,"~C4��ǰMP:%d~C0",fight_npc_group[fight_control_data.baobao_idx].base.now_mp);
	print16(window_x+2+8,window_y+2-32+8,print_rec,PEST_PUT,screen_buffer);
	

}



void window_exec_fight_baobao_skill(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	
	
	SLONG task;
	SLONG x_offset,y_offset;
	SLONG i;
	SLONG channel;
	SLONG id;
	
	RECT rect[MAX_BAOBAO_SKILL_NO];
	

	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;

	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	for(i=0;i<MAX_BAOBAO_SKILL_NO;i++)
	{
		rect[i].left=16;
		rect[i].top=16+i*18;
		rect[i].right=rect[i].left+(MAX_SKILL_NAME_LENGTH*8);
		rect[i].bottom=rect[i].top+16;
	}
	
	
	task=-1;
	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;

	
	for(i=0;i<MAX_BAOBAO_SKILL_NO;i++)
	{
		if((x_offset>=rect[i].left)&&(x_offset<rect[i].right)&&
			(y_offset>=rect[i].top)&&(y_offset<rect[i].bottom))
		{
			id=baobao_skill_data[i].skill_id;
			if(id>=0)
				task=i;
			break;
		}
	}
	
	
	
	
	switch(system_control.mouse_key)
	{
	case MS_RUp:						//	close 
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_fight_baobao_skill=-1;
		auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
		break;

	case MS_Move:
	case MS_Dummy:
		if((task<0)||(task>=MAX_BAOBAO_SKILL_NO))break;
		break;

	case MS_LUp:
		if((task<0)||(task>=MAX_BAOBAO_SKILL_NO))break;
		id=baobao_skill_data[task].skill_id;
		if(id<0)break;
		baobao_fight_command_data.type_no=id;			// �趨����id
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_fight_baobao_skill=-1;
		auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
		break;	

	case MS_LDrag:				// �϶�
		if(task<0)
		{
			now_window_move_xoffset=system_control.mouse_x-window_x;
			now_window_move_yoffset=system_control.mouse_y-window_y;
			now_window_move_handle=now_exec_window_handle;
		}
		break;
		
	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}



/*******************************************************************************************************************}
{	Fight character item																							}
********************************************************************************************************************/
void window_draw_fight_character_item(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	SLONG i;
	SLONG name_xl;

	SLONG inst_x,inst_y,inst_xl,inst_yl;
	RECT test_rect;
	
	
	RECT item_pp[MAX_POCKET_ITEM]={
		{ 26+40*0, 56+40*0, 26+40*0+32, 56+40*0+32 },
		{ 26+40*1, 56+40*0, 26+40*1+32, 56+40*0+32 },
		{ 26+40*2, 56+40*0, 26+40*2+32, 56+40*0+32 },
		{ 26+40*3, 56+40*0, 26+40*3+32, 56+40*0+32 },
		{ 26+40*4, 56+40*0, 26+40*4+32, 56+40*0+32 },
		{ 26+40*5, 56+40*0, 26+40*5+32, 56+40*0+32 },
		
		{ 26+40*0, 56+40*1, 26+40*0+32, 56+40*1+32 },
		{ 26+40*1, 56+40*1, 26+40*1+32, 56+40*1+32 },
		{ 26+40*2, 56+40*1, 26+40*2+32, 56+40*1+32 },
		{ 26+40*3, 56+40*1, 26+40*3+32, 56+40*1+32 },
		{ 26+40*4, 56+40*1, 26+40*4+32, 56+40*1+32 },
		{ 26+40*5, 56+40*1, 26+40*5+32, 56+40*1+32 },
		
		{ 26+40*0, 56+40*2, 26+40*0+32, 56+40*2+32 },
		{ 26+40*1, 56+40*2, 26+40*1+32, 56+40*2+32 },
		{ 26+40*2, 56+40*2, 26+40*2+32, 56+40*2+32 },
		{ 26+40*3, 56+40*2, 26+40*3+32, 56+40*2+32 },
		{ 26+40*4, 56+40*2, 26+40*4+32, 56+40*2+32 },
		{ 26+40*5, 56+40*2, 26+40*5+32, 56+40*2+32 },
		
		{ 26+40*0, 56+40*3, 26+40*0+32, 56+40*3+32 },
		{ 26+40*1, 56+40*3, 26+40*1+32, 56+40*3+32 },
		{ 26+40*2, 56+40*3, 26+40*2+32, 56+40*3+32 },
		{ 26+40*3, 56+40*3, 26+40*3+32, 56+40*3+32 },
		{ 26+40*4, 56+40*3, 26+40*4+32, 56+40*3+32 },
		{ 26+40*5, 56+40*3, 26+40*5+32, 56+40*3+32 }
		
	};
	
	
	if(fight_control_data.character_idx<0)return;
	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;

	
	
	// --- ( 0 - 0 )  Draw body
	display_img256_buffer(window_x,window_y,system_image.item_public_body.image,screen_buffer);	
	display_img256_buffer(window_x+2,window_y+(system_image.item_public_body.rect.bottom-system_image.item_public_body.rect.top),system_image.baobao_item_action.image,screen_buffer);	
	display_img256_buffer(window_x+107,window_y+14,system_image.baobao_item_mask.image,screen_buffer);	
	


	if(fight_npc_group[fight_control_data.character_idx].base.active)
	{
		name_xl=strlen((char *)fight_npc_group[fight_control_data.character_idx].base.name)*8;
		set_word_color(0,color_control.black);
		print16(window_x+19-1, window_y+248-1,fight_npc_group[fight_control_data.character_idx].base.name,PEST_PUT,screen_buffer);
		switch(fight_npc_group[fight_control_data.character_idx].base.life_base)
		{
		case 0:
			set_word_color(0,color_control.life_base0);
			break;
		case 1:
			set_word_color(0,color_control.life_base1);
			break;
		case 2:
			set_word_color(0,color_control.life_base2);
			break;
		case 3:
			set_word_color(0,color_control.life_base3);
			break;
		default:
			set_word_color(0,color_control.life_base0);
			break;
		}
		print16(window_x+19, window_y+248,fight_npc_group[fight_control_data.character_idx].base.name,PEST_PUT,screen_buffer);
		set_word_color(0,color_control.white);
		
	}
	
	
	// --- ( 1 - 0 ) display data
	if(character_pocket_data.active)
	{
		for(i=0;i<MAX_POCKET_ITEM;i++)
		{
			
			if((character_pocket_data.item[i].number>0)&&(character_pocket_data.item[i].filename_id>=0))
			{
				
				display_img256_buffer(window_x+item_pp[i].left,window_y+item_pp[i].top,
					system_image.item_image[character_pocket_data.item[i].filename_id],screen_buffer);
				
				if(character_pocket_data.item[i].number>1)
				{
					sprintf((char *)print_rec,"%d",character_pocket_data.item[i].number);
					print12(window_x+item_pp[i].right-(strlen((char *)print_rec)*6)-6-1,window_y+item_pp[i].bottom-10-1,print_rec,COPY_PUT,screen_buffer);
					
				}
				
			}
			
			if(i==character_pocket_data.idx)
			{
				put_scroll_box(window_x+item_pp[i].left-1,
					window_y+item_pp[i].top-1,
					window_x+item_pp[i].right+1,
					window_y+item_pp[i].bottom+1,color_control.red,color_control.yellow,screen_buffer);
				put_scroll_box(window_x+item_pp[i].left-2,
					window_y+item_pp[i].top-2,
					window_x+item_pp[i].right+2,
					window_y+item_pp[i].bottom+2,color_control.red,color_control.yellow,screen_buffer);
			}
			
		}
		
	}
	
	
	
	// ------ display inst
	if((system_control.mouse_x>window_x)&&(system_control.mouse_x<=window_x+window_xl)&&
		(system_control.mouse_y>window_y)&&(system_control.mouse_y<=window_y+window_yl))
	{
		if((detail_item_inst.active)&&(detail_item_inst.idx>=0))
		{
			switch(detail_item_inst.type)
			{
			case ITEM_INST_POCKET:
				if((character_pocket_data.item[detail_item_inst.idx].number>0)&&(character_pocket_data.item[detail_item_inst.idx].filename_id>=0))
				{
					test_rect.left=0;
					test_rect.top=0;
					test_rect.right=160;
					test_rect.bottom=SCREEN_WIDTH;
					inst_xl=32+32*2+160;
					
					text_out_data.g_pChat->insertString((char *)detail_item_inst.inst,test_rect,false);
					inst_yl=test_rect.bottom+32;
					if(inst_yl<80)
						inst_yl=80;
					
					inst_x=system_control.mouse_x-inst_xl/2;
					inst_y=system_control.mouse_y-inst_yl-16;
					if(inst_x<=0)
						inst_x=system_control.mouse_x;
					
					if(inst_y<0)
						inst_y=system_control.mouse_y+32;
					if(inst_x+inst_xl>=SCREEN_WIDTH)inst_x=SCREEN_WIDTH-inst_xl;
					if(inst_y+inst_yl>=SCREEN_HEIGHT)inst_y=SCREEN_HEIGHT-inst_yl;
					draw_message_box(inst_x,inst_y,inst_xl,inst_yl,screen_buffer);
					put_bar(inst_x+2,inst_y+2,inst_xl-4,inst_yl-6,color_control.black,screen_buffer);
					
					display_zoom_img256_buffer(inst_x+16,inst_y+16,
						system_image.item_image[character_pocket_data.item[detail_item_inst.idx].filename_id],screen_buffer,200);
					
					text_out_data.g_pChat->updateString(inst_x+16+64,inst_y+16,test_rect.right,inst_yl,detail_item_inst.inst);
					text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,screen_buffer,false);
				}
				
				break;
			}
			
		}
		
	}
		
	

}



void window_exec_fight_character_item(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;

	SLONG task;
	SLONG x_offset,y_offset;
	SLONG i;
	SLONG channel;

	RECT item_pp[MAX_POCKET_ITEM]={
		{ 26+40*0, 56+40*0, 26+40*0+32, 56+40*0+32 },
		{ 26+40*1, 56+40*0, 26+40*1+32, 56+40*0+32 },
		{ 26+40*2, 56+40*0, 26+40*2+32, 56+40*0+32 },
		{ 26+40*3, 56+40*0, 26+40*3+32, 56+40*0+32 },
		{ 26+40*4, 56+40*0, 26+40*4+32, 56+40*0+32 },
		{ 26+40*5, 56+40*0, 26+40*5+32, 56+40*0+32 },
		
		{ 26+40*0, 56+40*1, 26+40*0+32, 56+40*1+32 },
		{ 26+40*1, 56+40*1, 26+40*1+32, 56+40*1+32 },
		{ 26+40*2, 56+40*1, 26+40*2+32, 56+40*1+32 },
		{ 26+40*3, 56+40*1, 26+40*3+32, 56+40*1+32 },
		{ 26+40*4, 56+40*1, 26+40*4+32, 56+40*1+32 },
		{ 26+40*5, 56+40*1, 26+40*5+32, 56+40*1+32 },
		
		{ 26+40*0, 56+40*2, 26+40*0+32, 56+40*2+32 },
		{ 26+40*1, 56+40*2, 26+40*1+32, 56+40*2+32 },
		{ 26+40*2, 56+40*2, 26+40*2+32, 56+40*2+32 },
		{ 26+40*3, 56+40*2, 26+40*3+32, 56+40*2+32 },
		{ 26+40*4, 56+40*2, 26+40*4+32, 56+40*2+32 },
		{ 26+40*5, 56+40*2, 26+40*5+32, 56+40*2+32 },
		
		{ 26+40*0, 56+40*3, 26+40*0+32, 56+40*3+32 },
		{ 26+40*1, 56+40*3, 26+40*1+32, 56+40*3+32 },
		{ 26+40*2, 56+40*3, 26+40*2+32, 56+40*3+32 },
		{ 26+40*3, 56+40*3, 26+40*3+32, 56+40*3+32 },
		{ 26+40*4, 56+40*3, 26+40*4+32, 56+40*3+32 },
		{ 26+40*5, 56+40*3, 26+40*5+32, 56+40*3+32 }
		
	};
	
	
	RECT rect[2]={
		{ 247, 9 ,247+28, 9+28	},				// �ر�			0
		{ 114,226,114+63,226+21 },				// ȷ��			1
	};
	
	
	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	task=-1;
	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;
	
	for(i=0;i<2;i++)
	{
		if((x_offset>=rect[i].left)&&(x_offset<rect[i].right)&&
			(y_offset>=rect[i].top)&&(y_offset<rect[i].bottom))
		{
			task=i;
			break;
		}
	}
	

	if(task<0)
	{
		for(i=0;i<MAX_POCKET_ITEM;i++)
		{
			if((x_offset>=item_pp[i].left)&&(x_offset<item_pp[i].right)&&
				(y_offset>=item_pp[i].top)&&(y_offset<item_pp[i].bottom))
			{
				task=100+i;
				break;
			}
			
		}
	}
	

	
	if(task<0)
	{
		clear_detail_item_inst();
	}
	
	if((task>=100)&&(task<200))
	{
		detail_item_inst.idx=task-100;
		if((character_pocket_data.item[detail_item_inst.idx].number>0)&&(character_pocket_data.item[detail_item_inst.idx].filename_id>=0))
		{
			if(detail_item_inst.old_idx!=detail_item_inst.idx)
			{
				if(system_control.system_timer>detail_item_inst.timer+DETAIL_DELAY_TIME)
				{
					detail_item_inst.old_idx=detail_item_inst.idx;
					detail_item_inst.active=false;
					detail_item_inst.type=ITEM_INST_POCKET;
					request_detail_inst();
					detail_item_inst.timer=system_control.system_timer;
				}
			}
		}
	}
	
	

	switch(system_control.mouse_key)
	{
	case MS_RUp:						//	close 
		clear_detail_item_inst();
		if(task<0)				// close
		{
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_fight_character_item=-1;
		}
		break;
	case MS_Move:
	case MS_Dummy:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// ȷ��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand90",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		}
		break;

	case MS_LUp:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_fight_character_item=-1;
			break;
		case 1:		// ȷ��
			check_baobao_stack();
			if(character_pocket_data.idx<0)break;
			if(character_pocket_data.item[character_pocket_data.idx].number<=0)break;
			if(character_pocket_data.item[character_pocket_data.idx].item_id<0)break;
			character_fight_command_data.type_no=character_pocket_data.idx;	// �趨����id
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_fight_character_item=-1;
			break;
		case 100: case 101: case 102: case 103: case 104: case 105:
		case 106: case 107: case 108: case 109: case 110: case 111:
		case 112: case 113: case 114: case 115: case 116: case 117:
		case 118: case 119: case 120: case 121: case 122: case 123:
			character_pocket_data.idx=task-100;
			break;
			
		}
		break;	
	case MS_LDrag:				// �϶�
			
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// ȷ��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand91",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		default:
			now_window_move_xoffset=system_control.mouse_x-window_x;
			now_window_move_yoffset=system_control.mouse_y-window_y;
			now_window_move_handle=now_exec_window_handle;
			break;
		}
		break;

	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}




/*******************************************************************************************************************}
{	Fight baobao item																								}
********************************************************************************************************************/
void window_draw_fight_baobao_item(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	SLONG i;

	SLONG inst_x,inst_y,inst_xl,inst_yl;
	RECT test_rect;
	SLONG name_xl;	
	
	RECT item_pp[MAX_POCKET_ITEM]={
		{ 26+40*0, 56+40*0, 26+40*0+32, 56+40*0+32 },
		{ 26+40*1, 56+40*0, 26+40*1+32, 56+40*0+32 },
		{ 26+40*2, 56+40*0, 26+40*2+32, 56+40*0+32 },
		{ 26+40*3, 56+40*0, 26+40*3+32, 56+40*0+32 },
		{ 26+40*4, 56+40*0, 26+40*4+32, 56+40*0+32 },
		{ 26+40*5, 56+40*0, 26+40*5+32, 56+40*0+32 },
		
		{ 26+40*0, 56+40*1, 26+40*0+32, 56+40*1+32 },
		{ 26+40*1, 56+40*1, 26+40*1+32, 56+40*1+32 },
		{ 26+40*2, 56+40*1, 26+40*2+32, 56+40*1+32 },
		{ 26+40*3, 56+40*1, 26+40*3+32, 56+40*1+32 },
		{ 26+40*4, 56+40*1, 26+40*4+32, 56+40*1+32 },
		{ 26+40*5, 56+40*1, 26+40*5+32, 56+40*1+32 },
		
		{ 26+40*0, 56+40*2, 26+40*0+32, 56+40*2+32 },
		{ 26+40*1, 56+40*2, 26+40*1+32, 56+40*2+32 },
		{ 26+40*2, 56+40*2, 26+40*2+32, 56+40*2+32 },
		{ 26+40*3, 56+40*2, 26+40*3+32, 56+40*2+32 },
		{ 26+40*4, 56+40*2, 26+40*4+32, 56+40*2+32 },
		{ 26+40*5, 56+40*2, 26+40*5+32, 56+40*2+32 },
		
		{ 26+40*0, 56+40*3, 26+40*0+32, 56+40*3+32 },
		{ 26+40*1, 56+40*3, 26+40*1+32, 56+40*3+32 },
		{ 26+40*2, 56+40*3, 26+40*2+32, 56+40*3+32 },
		{ 26+40*3, 56+40*3, 26+40*3+32, 56+40*3+32 },
		{ 26+40*4, 56+40*3, 26+40*4+32, 56+40*3+32 },
		{ 26+40*5, 56+40*3, 26+40*5+32, 56+40*3+32 }
		
	};
	
	if(fight_control_data.baobao_idx<0)return;
	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;

	
	
	// --- ( 0 - 0 )  Draw body
	display_img256_buffer(window_x,window_y,system_image.item_public_body.image,screen_buffer);	
	display_img256_buffer(window_x+2,window_y+(system_image.item_public_body.rect.bottom-system_image.item_public_body.rect.top),system_image.baobao_item_action.image,screen_buffer);	
	display_img256_buffer(window_x+107,window_y+14,system_image.baobao_item_mask.image,screen_buffer);	
	

	
	
	if(fight_npc_group[fight_control_data.baobao_idx].base.active)
	{
		name_xl=strlen((char *)fight_npc_group[fight_control_data.baobao_idx].base.name)*8;
		set_word_color(0,color_control.black);
		print16(window_x+19-1, window_y+248-1,fight_npc_group[fight_control_data.baobao_idx].base.name,PEST_PUT,screen_buffer);
		switch(fight_npc_group[fight_control_data.baobao_idx].base.life_base)
		{
		case 0:
			set_word_color(0,color_control.life_base0);
			break;
		case 1:
			set_word_color(0,color_control.life_base1);
			break;
		case 2:
			set_word_color(0,color_control.life_base2);
			break;
		case 3:
			set_word_color(0,color_control.life_base3);
			break;
		default:
			set_word_color(0,color_control.life_base0);
			break;
		}
		print16(window_x+19, window_y+248,fight_npc_group[fight_control_data.baobao_idx].base.name,PEST_PUT,screen_buffer);
		set_word_color(0,color_control.white);
		
	}
	

	// --- ( 1 - 0 ) display data
	if(character_pocket_data.active)
	{
		for(i=0;i<MAX_POCKET_ITEM;i++)
		{
			
			if((character_pocket_data.item[i].number>0)&&(character_pocket_data.item[i].filename_id>=0))
			{
				
				display_img256_buffer(window_x+item_pp[i].left,window_y+item_pp[i].top,
					system_image.item_image[character_pocket_data.item[i].filename_id],screen_buffer);
				
				if(character_pocket_data.item[i].number>1)
				{
					sprintf((char *)print_rec,"%d",character_pocket_data.item[i].number);
					print12(window_x+item_pp[i].right-(strlen((char *)print_rec)*6)-6-1,window_y+item_pp[i].bottom-10-1,print_rec,COPY_PUT,screen_buffer);
					
				}
				
			}
			
			if(i==character_pocket_data.idx)
			{
				put_scroll_box(window_x+item_pp[i].left-1,
					window_y+item_pp[i].top-1,
					window_x+item_pp[i].right+1,
					window_y+item_pp[i].bottom+1,color_control.red,color_control.yellow,screen_buffer);
				put_scroll_box(window_x+item_pp[i].left-2,
					window_y+item_pp[i].top-2,
					window_x+item_pp[i].right+2,
					window_y+item_pp[i].bottom+2,color_control.red,color_control.yellow,screen_buffer);
			}
			
		}
		
	}
	
	
	
	// ------ display inst
	if((system_control.mouse_x>window_x)&&(system_control.mouse_x<=window_x+window_xl)&&
		(system_control.mouse_y>window_y)&&(system_control.mouse_y<=window_y+window_yl))
	{
		if((detail_item_inst.active)&&(detail_item_inst.idx>=0))
		{
			switch(detail_item_inst.type)
			{
			case ITEM_INST_POCKET:
				if((character_pocket_data.item[detail_item_inst.idx].number>0)&&(character_pocket_data.item[detail_item_inst.idx].filename_id>=0))
				{
					test_rect.left=0;
					test_rect.top=0;
					test_rect.right=160;
					test_rect.bottom=SCREEN_WIDTH;
					inst_xl=32+32*2+160;
					
					text_out_data.g_pChat->insertString((char *)detail_item_inst.inst,test_rect,false);
					inst_yl=test_rect.bottom+32;
					if(inst_yl<80)
						inst_yl=80;
					
					inst_x=system_control.mouse_x-inst_xl/2;
					inst_y=system_control.mouse_y-inst_yl-16;
					if(inst_x<=0)
						inst_x=system_control.mouse_x;
					
					if(inst_y<0)
						inst_y=system_control.mouse_y+32;
					if(inst_x+inst_xl>=SCREEN_WIDTH)inst_x=SCREEN_WIDTH-inst_xl;
					if(inst_y+inst_yl>=SCREEN_HEIGHT)inst_y=SCREEN_HEIGHT-inst_yl;
					draw_message_box(inst_x,inst_y,inst_xl,inst_yl,screen_buffer);
					put_bar(inst_x+2,inst_y+2,inst_xl-4,inst_yl-6,color_control.black,screen_buffer);
					
					display_zoom_img256_buffer(inst_x+16,inst_y+16,
						system_image.item_image[character_pocket_data.item[detail_item_inst.idx].filename_id],screen_buffer,200);
					
					text_out_data.g_pChat->updateString(inst_x+16+64,inst_y+16,test_rect.right,inst_yl,detail_item_inst.inst);
					text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,screen_buffer,false);
				}
				
				break;
			}
			
		}
		
	}

	

}



void window_exec_fight_baobao_item(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;

	SLONG task;
	SLONG x_offset,y_offset;
	SLONG i;
	SLONG channel;

	RECT item_pp[MAX_POCKET_ITEM]={
		{ 26+40*0, 56+40*0, 26+40*0+32, 56+40*0+32 },
		{ 26+40*1, 56+40*0, 26+40*1+32, 56+40*0+32 },
		{ 26+40*2, 56+40*0, 26+40*2+32, 56+40*0+32 },
		{ 26+40*3, 56+40*0, 26+40*3+32, 56+40*0+32 },
		{ 26+40*4, 56+40*0, 26+40*4+32, 56+40*0+32 },
		{ 26+40*5, 56+40*0, 26+40*5+32, 56+40*0+32 },
		
		{ 26+40*0, 56+40*1, 26+40*0+32, 56+40*1+32 },
		{ 26+40*1, 56+40*1, 26+40*1+32, 56+40*1+32 },
		{ 26+40*2, 56+40*1, 26+40*2+32, 56+40*1+32 },
		{ 26+40*3, 56+40*1, 26+40*3+32, 56+40*1+32 },
		{ 26+40*4, 56+40*1, 26+40*4+32, 56+40*1+32 },
		{ 26+40*5, 56+40*1, 26+40*5+32, 56+40*1+32 },
		
		{ 26+40*0, 56+40*2, 26+40*0+32, 56+40*2+32 },
		{ 26+40*1, 56+40*2, 26+40*1+32, 56+40*2+32 },
		{ 26+40*2, 56+40*2, 26+40*2+32, 56+40*2+32 },
		{ 26+40*3, 56+40*2, 26+40*3+32, 56+40*2+32 },
		{ 26+40*4, 56+40*2, 26+40*4+32, 56+40*2+32 },
		{ 26+40*5, 56+40*2, 26+40*5+32, 56+40*2+32 },
		
		{ 26+40*0, 56+40*3, 26+40*0+32, 56+40*3+32 },
		{ 26+40*1, 56+40*3, 26+40*1+32, 56+40*3+32 },
		{ 26+40*2, 56+40*3, 26+40*2+32, 56+40*3+32 },
		{ 26+40*3, 56+40*3, 26+40*3+32, 56+40*3+32 },
		{ 26+40*4, 56+40*3, 26+40*4+32, 56+40*3+32 },
		{ 26+40*5, 56+40*3, 26+40*5+32, 56+40*3+32 }
		
	};
	
	
	RECT rect[2]={
		{ 247, 9 ,247+28, 9+28	},				// �ر�			0
		{ 114,226,114+63,226+21 },				// ȷ��			1
	};
	
	
	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	task=-1;
	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;
	
	for(i=0;i<2;i++)
	{
		if((x_offset>=rect[i].left)&&(x_offset<rect[i].right)&&
			(y_offset>=rect[i].top)&&(y_offset<rect[i].bottom))
		{
			task=i;
			break;
		}
	}
	

	if(task<0)
	{
		for(i=0;i<MAX_POCKET_ITEM;i++)
		{
			if((x_offset>=item_pp[i].left)&&(x_offset<item_pp[i].right)&&
				(y_offset>=item_pp[i].top)&&(y_offset<item_pp[i].bottom))
			{
				task=100+i;
				break;
			}
			
		}
	}
	

	
	if(task<0)
	{
		clear_detail_item_inst();
	}
	
	if((task>=100)&&(task<200))
	{
		detail_item_inst.idx=task-100;
		if((character_pocket_data.item[detail_item_inst.idx].number>0)&&(character_pocket_data.item[detail_item_inst.idx].filename_id>=0))
		{
			if(detail_item_inst.old_idx!=detail_item_inst.idx)
			{
				if(system_control.system_timer>detail_item_inst.timer+DETAIL_DELAY_TIME)
				{
					detail_item_inst.old_idx=detail_item_inst.idx;
					detail_item_inst.active=false;
					detail_item_inst.type=ITEM_INST_POCKET;
					request_detail_inst();
					detail_item_inst.timer=system_control.system_timer;
				}
			}
		}
	}
	
	

	switch(system_control.mouse_key)
	{
	case MS_RUp:						//	close 
		clear_detail_item_inst();
		if(task<0)				// close
		{
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_fight_baobao_item=-1;
		}
		break;
	case MS_Move:
	case MS_Dummy:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// ȷ��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand90",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		}
		break;

	case MS_LUp:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_fight_baobao_item=-1;
			break;
		case 1:		// ȷ��
			check_baobao_stack();
			if(character_pocket_data.idx<0)break;
			if(character_pocket_data.item[character_pocket_data.idx].number<=0)break;
			if(character_pocket_data.item[character_pocket_data.idx].item_id<0)break;
			baobao_fight_command_data.type_no=character_pocket_data.idx;	
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_fight_baobao_item=-1;
			break;
		case 100: case 101: case 102: case 103: case 104: case 105:
		case 106: case 107: case 108: case 109: case 110: case 111:
		case 112: case 113: case 114: case 115: case 116: case 117:
		case 118: case 119: case 120: case 121: case 122: case 123:
			character_pocket_data.idx=task-100;
			break;
			
		}
		break;	
	case MS_LDrag:				// �϶�
			
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// ȷ��
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand91",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		default:
			now_window_move_xoffset=system_control.mouse_x-window_x;
			now_window_move_yoffset=system_control.mouse_y-window_y;
			now_window_move_handle=now_exec_window_handle;
			break;
		}
		break;

	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}



/*******************************************************************************************************************}
{	cbaobao change																									}
********************************************************************************************************************/
void window_draw_fight_baobao_change(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	SLONG i,j;
	UCHR npc_filename[256];
	SLONG mda_handle;

	RECT rect[17]={
		{ 333,41 ,333+28,41+28	},				// �ر�			0
		{ 171,150,171+21,150+21	},				// �Ϸ�			1
		{ 171,177,171+21,177+21 },				// �·�			2
		{ 306,305,306+21,305+21 },				// +	HP		3
		{ 306,327,306+21,327+21 },				// +	MP		4
		{ 306,349,306+21,349+21 },				// +	ATT		5
		{ 306,371,306+21,371+21 },				// +	SPP		6
		{ 329,305,322+21,305+21 },				// -			7
		{ 329,327,322+21,327+21 },				// -			8
		{ 329,349,322+21,349+21 },				// -			9
		{ 329,371,322+21,371+21 },				// -			10
		{  30,417, 30+63,417+21 },				// ����			11	
		{ 125,416,125+96,416+22 },				// ���Բ鿴		12
		{ 246,416,246+96,416+22 },				// ��������		13
		{ 198,235,198+71,235+19 },				// ��ս			14
		{ 285,235,285+71,235+19 },				// �ۿ�			15
		{ 204,261,204+63,261+21 }				// ��Ʒ			16
	};
	
	

	static ULONG timer=0;
	SLONG mda_command;


	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	

	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	
// --- ( 0 - 0 )  Draw body

	display_img256_buffer(window_x,window_y,system_image.baobao_attrib_body.image,screen_buffer);	



	mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand3",window_x+rect[3].left,window_y+rect[3].top,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_PALETTE_GRAY,0,0,0);
	mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand3",window_x+rect[4].left,window_y+rect[4].top,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_PALETTE_GRAY,0,0,0);
	mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand3",window_x+rect[5].left,window_y+rect[5].top,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_PALETTE_GRAY,0,0,0);
	mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand3",window_x+rect[6].left,window_y+rect[6].top,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_PALETTE_GRAY,0,0,0);
	mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand5",window_x+rect[7].left,window_y+rect[7].top,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_PALETTE_GRAY,0,0,0);
	mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand5",window_x+rect[8].left,window_y+rect[8].top,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_PALETTE_GRAY,0,0,0);
	mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand5",window_x+rect[9].left,window_y+rect[9].top,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_PALETTE_GRAY,0,0,0);
	mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand5",window_x+rect[10].left,window_y+rect[10].top,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_PALETTE_GRAY,0,0,0);
	mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand83",window_x+rect[11].left,window_y+rect[11].top,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_PALETTE_GRAY,0,0,0);
	mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand69",window_x+rect[12].left,window_y+rect[12].top,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_PALETTE_GRAY,0,0,0);
	mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand67",window_x+rect[13].left,window_y+rect[13].top,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_PALETTE_GRAY,0,0,0);
	mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand75",window_x+rect[15].left,window_y+rect[15].top,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_PALETTE_GRAY,0,0,0);
	mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand81",window_x+rect[16].left,window_y+rect[16].top,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_PALETTE_GRAY,0,0,0);
			

// --- ( 0 - 1 ) display data	

	if(!base_baobao_data.active)return;

// --- ( 1 - 0 ) Display baobao stack
	check_baobao_stack();


	for(j=0,i=top_baobao_stack;i<total_baobao_stack;i++,j++)
	{
		if((i-top_baobao_stack)>=6)
			break;
		if(base_baobao_data.data.baobao_list[i].baobao_id==0)
			continue;

		if(i==baobao_stack_index)
		{
			alpha_put_bar(window_x+25,window_y+83+j*20,139,18,color_control.pink,screen_buffer,128);
		} 

		if(i==base_baobao_data.data.active_idx)
		{
			put_bar(window_x+25,window_y+83+j*20,139,18,color_control.green,screen_buffer);
		}
		
		if((system_control.mouse_x>=window_x+25)&&(system_control.mouse_x<window_x+25+139)&&
			(system_control.mouse_y>=window_y+83+j*20)&&(system_control.mouse_y<window_y+83+j*20+18))
		{
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand0",window_x+25,window_y+83+j*20,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
		}
		
		if(base_baobao_data.data.show[i])
		{
			set_word_color(0,color_control.yellow);
			print16(window_x+25+8,window_y+83+1+(i-top_baobao_stack)*20,base_baobao_data.data.baobao_list[i].name,PEST_PUT,screen_buffer);
			set_word_color(0,color_control.white);
		}
		else
		{
			print16(window_x+25+8,window_y+83+1+(i-top_baobao_stack)*20,base_baobao_data.data.baobao_list[i].name,PEST_PUT,screen_buffer);
		}
		
	}
	

// --- ( 2 - 0 ) display select 
	if(baobao_stack_index<0)return;



// --- display baobao image

	sprintf((char *)npc_filename,"NPC\\NPC%da.MDA",base_baobao_data.data.baobao_list[baobao_stack_index].base.filename_id);
	mda_handle=get_mda_handle(npc_filename);

	if(mda_handle<0)
	{
		if(base_baobao_data.display_handle>=0)
		auto_close_map_npc_handle(base_baobao_data.display_handle);
		mda_group_open((UCHR *)npc_filename,&mda_handle);
	}
		
	base_baobao_data.display_handle=mda_handle;


	mda_command=MDA_COMMAND_LOOP;
	if(base_baobao_data.display_handle>=0)
	{
		if(system_control.system_timer>timer)
		{
			mda_command=mda_command|MDA_COMMAND_NEXT_FRAME;
			timer=system_control.system_timer+SYSTEM_MAIN_DELAY;
		}

		mda_group_exec(base_baobao_data.display_handle,(UCHR *)"MainCommand3",window_x+275,window_y+208,
			screen_buffer,mda_command,0,0,0);
	}
	



// ϵ��	
	switch(base_baobao_data.data.baobao_list[baobao_stack_index].base.phyle)
	{
	case BAOBAO_PHYLE_HUMAN	:
		print16(window_x+76,window_y+235,(UCHR *)BAOBAO_PHYLE_HUMAN_TEXT,PEST_PUT,screen_buffer);
		break;
	case BAOBAO_PHYLE_FLY:
		print16(window_x+76,window_y+235,(UCHR *)BAOBAO_PHYLE_FLY_TEXT,PEST_PUT,screen_buffer);
		break;
	case BAOBAO_PHYLE_DEVIL:
		print16(window_x+76,window_y+235,(UCHR *)BAOBAO_PHYLE_DEVIL_TEXT,PEST_PUT,screen_buffer);
		break;
	case BAOBAO_PHYLE_DEAD:
		print16(window_x+76,window_y+235,(UCHR *)BAOBAO_PHYLE_DEAD_TEXT,PEST_PUT,screen_buffer);
		break;
	case BAOBAO_PHYLE_DRAGON:
		print16(window_x+76,window_y+235,(UCHR *)BAOBAO_PHYLE_DRAGON_TEXT,PEST_PUT,screen_buffer);
		break;	
	case BAOBAO_PHYLE_SPECIAL:
		print16(window_x+76,window_y+235,(UCHR *)BAOBAO_PHYLE_SPECIAL_TEXT,PEST_PUT,screen_buffer);
		break;
	}

	
// level
	
	sprintf((char *)print_rec,"%d",base_baobao_data.data.baobao_list[baobao_stack_index].data.level);
	print16(window_x+76,window_y+260,print_rec,PEST_PUT,screen_buffer);


	sprintf((char *)print_rec,"%d",base_baobao_data.data.baobao_list[baobao_stack_index].loyalty);
	print16(window_x+76,window_y+285,print_rec,PEST_PUT,screen_buffer);

	sprintf((char *)print_rec,"%d/%d",base_baobao_data.data.baobao_list[baobao_stack_index].data.now_hp,
		base_baobao_data.data.baobao_list[baobao_stack_index].data.max_hp);
	print16(window_x+75,window_y+311-3,print_rec,PEST_PUT,screen_buffer);

	sprintf((char *)print_rec,"%d/%d",base_baobao_data.data.baobao_list[baobao_stack_index].data.now_mp,
		base_baobao_data.data.baobao_list[baobao_stack_index].data.max_mp);
	print16(window_x+75,window_y+332-3,print_rec,PEST_PUT,screen_buffer);

	sprintf((char *)print_rec,"%d",base_baobao_data.data.baobao_list[baobao_stack_index].data.att);
	print16(window_x+75,window_y+353-3,print_rec,PEST_PUT,screen_buffer);
	
	sprintf((char *)print_rec,"%d",base_baobao_data.data.baobao_list[baobao_stack_index].data.speed);
	print16(window_x+75,window_y+374-3,print_rec,PEST_PUT,screen_buffer);
	
	sprintf((char *)print_rec,"%d/%d",base_baobao_data.data.baobao_list[baobao_stack_index].data.now_exp,
		base_baobao_data.data.baobao_list[baobao_stack_index].data.need_exp);
	print16(window_x+75,window_y+395-3,print_rec,PEST_PUT,screen_buffer);

// ---
	sprintf((char *)print_rec,"%d",base_baobao_data.data.baobao_list[baobao_stack_index].base.hp_point+baobao_point_back[baobao_stack_index].hp_point);
	print16(window_x+250,window_y+311-3,print_rec,PEST_PUT,screen_buffer);
	
	sprintf((char *)print_rec,"%d",base_baobao_data.data.baobao_list[baobao_stack_index].base.mp_point+baobao_point_back[baobao_stack_index].mp_point);
	print16(window_x+250,window_y+332-3,print_rec,PEST_PUT,screen_buffer);
	
	sprintf((char *)print_rec,"%d",base_baobao_data.data.baobao_list[baobao_stack_index].base.att_point+baobao_point_back[baobao_stack_index].att_point);
	print16(window_x+250,window_y+353-3,print_rec,PEST_PUT,screen_buffer);

	sprintf((char *)print_rec,"%d",base_baobao_data.data.baobao_list[baobao_stack_index].base.speed_point+baobao_point_back[baobao_stack_index].speed_point);
	print16(window_x+250,window_y+374-3,print_rec,PEST_PUT,screen_buffer);

	sprintf((char *)print_rec,"%d",base_baobao_data.data.baobao_list[baobao_stack_index].base.point+baobao_point_back[baobao_stack_index].total_point);
	print16(window_x+312,window_y+287-3,print_rec,PEST_PUT,screen_buffer);

	print16(window_x+76,window_y+210,base_baobao_data.data.baobao_list[baobao_stack_index].name ,PEST_PUT,screen_buffer);
	
}


void window_exec_fight_baobao_change(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;

	SLONG task;
	SLONG x_offset,y_offset;
	SLONG i;
	SLONG channel;

	RECT rect[17]={
		{ 333,41 ,333+28,41+28	},				// �ر�			0
		{ 171,150,171+21,150+21	},				// �Ϸ�			1
		{ 171,177,171+21,177+21 },				// �·�			2
		{ 306,305,306+21,305+21 },				// +	HP		3
		{ 306,327,306+21,327+21 },				// +	MP		4
		{ 306,349,306+21,349+21 },				// +	ATT		5
		{ 306,371,306+21,371+21 },				// +	SPP		6
		{ 329,305,322+21,305+21 },				// -			7
		{ 329,327,322+21,327+21 },				// -			8
		{ 329,349,322+21,349+21 },				// -			9
		{ 329,371,322+21,371+21 },				// -			10
		{  30,417, 30+63,417+21 },				// ����			11	
		{ 125,416,125+96,416+22 },				// ���Բ鿴		12
		{ 246,416,246+96,416+22 },				// ��������		13
		{ 198,235,198+71,235+19 },				// ��ս			14
		{ 285,235,285+71,235+19 },				// �ۿ�			15
		{ 204,261,204+63,261+21 }				// ��Ʒ			16
	};
	
	
	
	
	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	

	task=-1;
	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;
	
	for(i=0;i<17;i++)
	{
		if((x_offset>=rect[i].left)&&(x_offset<rect[i].right)&&
			(y_offset>=rect[i].top)&&(y_offset<rect[i].bottom))
		{
			task=i;
			break;
		}
	}
	

	for(i=0;i<6;i++)
	{	
		if((system_control.mouse_x>=window_x+25)&&(system_control.mouse_x<window_x+25+139)&&
			(system_control.mouse_y>=window_y+83+i*20)&&(system_control.mouse_y<window_y+83+i*20+18))
		{
			task=100+i;
		}
	}
	
	
	
	switch(system_control.mouse_key)
	{
	case MS_RUp:						//	close 
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_fight_baobao_change=-1;
		break;
	case MS_Move:
	case MS_Dummy:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// �Ϸ�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand7",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// �·�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand9",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 14:	// ��ս
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand73",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		}
		break;

	case MS_LUp:
		if(task<0)break;
		switch(task)
		{
		case 0:		// close
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_fight_baobao_change=-1;
			break;
		case 1:		// �Ϸ�
			if(top_baobao_stack<=0)break;
			top_baobao_stack--;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			break;
		case 2:		// �·�
			if(top_baobao_stack>=total_baobao_stack-6)break;
			top_baobao_stack++;
			auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
			break;
		case 14:	// ��ս
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			if(!fight_npc_group[fight_control_data.baobao_idx].base.active)
				character_fight_command_data.type_no=baobao_stack_index;
			else
				baobao_fight_command_data.type_no=baobao_stack_index;
			now_delete_window_handle=now_exec_window_handle;
			game_control.window_fight_baobao_change=-1;
			break;

		case 100: case 101:case 102:case 103:case 104:case 105:
			if(base_baobao_data.data.baobao_list[task-100+top_baobao_stack].baobao_id>0)
			{
				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
				baobao_stack_index=task-100+top_baobao_stack;

				g_PetNameEdit.Erase();
				if (*base_baobao_data.data.baobao_list[baobao_stack_index].name)
				{
					g_PetNameEdit.setText(base_baobao_data.data.baobao_list[baobao_stack_index].name);
				}
			}
			break;
		}
		break;
		
	case MS_LDrag:				// �϶�
		switch(task)
		{
		case 0:		// close
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 1:		// �Ϸ�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand8",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 2:		// �·�
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand10",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		case 14:	// ��ս
			if(baobao_stack_index<0)break;
			if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)break;
			mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand74",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
			break;
		default:
			now_window_move_xoffset=system_control.mouse_x-window_x;
			now_window_move_yoffset=system_control.mouse_y-window_y;
			now_window_move_handle=now_exec_window_handle;
			break;
		}
		break;
		
	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}



/*******************************************************************************************************************}
{	Fight talk																											}
********************************************************************************************************************/
void window_draw_fight_talk(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	SLONG offset_x,offset_y;
	RECT window_rect;
	SLONG dummy;
	SLONG task,i;
	UCHR text[5][20]=
	{
		MSG_FIGHT_TALK_0,
		MSG_FIGHT_TALK_1,
		MSG_FIGHT_TALK_2,
		MSG_FIGHT_TALK_3,
		MSG_FIGHT_TALK_4
	};
	
	
	if(now_exec_window_handle<0)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	offset_x=system_control.mouse_x-window_x;
	offset_y=system_control.mouse_y-window_y;
	
	// --- ( 0 - 0 )  Draw body
	draw_message_box(window_x,window_y,window_xl,window_yl,screen_buffer);
	put_bar(window_x+2,window_y+2,window_xl-4,window_yl-4,color_control.black,screen_buffer);

	task=-1;
	for(i=0;i<5;i++)
	{
		if((offset_x>=16)&&(offset_x<16+window_xl-32)&&
			(offset_y>=8+i*18)&&(offset_y<8+i*18+18))
			task=i;
	}

	for(i=0;i<5;i++)
	{
		if(i==task)
			put_bar(window_x+16,window_y+8+i*18,window_xl-32,16,color_control.pink,screen_buffer);
		print16(window_x+16,window_y+8+i*18,text[i],PEST_PUT,screen_buffer);
	}
	
}



void window_exec_fight_talk(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	RECT window_rect;
	SLONG dummy;
	
	SLONG task;
	SLONG x_offset,y_offset;
	SLONG channel;
	SLONG i;
	UCHR text[5][20]=
	{
		MSG_FIGHT_TALK_0,
			MSG_FIGHT_TALK_1,
			MSG_FIGHT_TALK_2,
			MSG_FIGHT_TALK_3,
			MSG_FIGHT_TALK_4
	};
	
	

	if(now_exec_window_handle<0)return;
	if(now_window_move_handle==now_exec_window_handle)return;
	if(read_window_base_data(now_exec_window_handle,&window_rect,&dummy)!=TTN_OK)
		return;
	
	window_x=window_rect.left;
	window_y=window_rect.top;
	window_xl=window_rect.right-window_rect.left;
	window_yl=window_rect.bottom-window_rect.top;
	
	task=-1;
	x_offset=system_control.mouse_x-window_x;
	y_offset=system_control.mouse_y-window_y;

	
	task=-1;
	for(i=0;i<5;i++)
	{
		if((x_offset>=16)&&(x_offset<16+window_xl-32)&&
			(y_offset>=8+i*18)&&(y_offset<8+i*18+18))
			task=i;
	}
	
	
	switch(system_control.mouse_key)
	{
	case MS_RUp:						//	close 
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_fight_talk=-1;
		break;

	case MS_Move:
	case MS_Dummy:
		if(task<0)break;
		break;

	case MS_LUp:
		if(task<0)break;
		s_editChat.Erase();
		s_editChat.setText((char *)text[task]);
		now_delete_window_handle=now_exec_window_handle;
		game_control.window_fight_talk=-1;
		auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
		break;	

	case MS_LDrag:				// �϶�
		if(task>=0)break;			
		now_window_move_xoffset=system_control.mouse_x-window_x;
		now_window_move_yoffset=system_control.mouse_y-window_y;
		now_window_move_handle=now_exec_window_handle;
		break;
		
	}
	
	
	reset_mouse_key();
	system_control.mouse_key=MS_Dummy;
	
}


