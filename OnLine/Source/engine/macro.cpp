/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : macro.cpp
  *owner  		: Stephen
  *description  : 
  *modified     : 2005/2/21
******************************************************************************/ 

#include "engine_global.h"
#include "macro.h"
#include "macrodef.h"
#include "graph.h"
#include "data.h"
#include "vari-ext.h"
#include "net.h"
#include "data_proc.h"



SLONG macro_number_var[MAX_MACRO_PARAMETER];
UCHR macro_string_var[MAX_MACRO_PARAMETER][1024];




void exec_macro(UCHR command,UCHR *data,SLONG data_length)
{
	SLONG command_idx;
	SLONG i;
	SLONG num_idx,str_idx;

	SLONG buffer_idx;
	SLONG number;
	SLONG j;
	UCHR str_buffer[1024];
	RECT rect;

	command_idx=-1;
	for(i=0;i<MAX_MACRO_COMMAND_DEFINE;i++)
	{
		if(command==macro_command_define[i].command_code)
		{
			command_idx=i;
			break;
		}
	}

	if(command_idx<0)
	{
		sprintf((char *)print_rec,"Error Macro code %d",command);
		display_error_message(print_rec,1);
		return;
	}

// --- ( 0 - 0 ) 装换参数
	clear_macro_var();
	if(data)
	{
		num_idx=0;
		str_idx=0;
		buffer_idx=0;
		for(i=0;i<MAX_MACRO_PARAMETER;i++)
		{
			switch(macro_command_define[command_idx].var_type[i])
			{
			case 0:				// None
				break;
			case 1:				// Int
				number=data[buffer_idx]+data[buffer_idx+1]*0x100;
				buffer_idx=buffer_idx+2;
				macro_number_var[num_idx]=number;
				num_idx++;
				break;
			case 2:				// string
				for(j=0;j<1023;j++)
				{
					str_buffer[j]=data[buffer_idx];
					if(data[buffer_idx]==NULL)
					{
						buffer_idx++;
						break;
					}
					buffer_idx++;
					
				}
				str_buffer[j]=NULL;
				strcpy((char *)macro_string_var[str_idx],(char *)str_buffer);
				str_idx++;
				break;
			default:
				break;
			}
			
			if(buffer_idx>data_length)
			{
				display_error_message((UCHR *)"Decode Macro Error!(Length)",1);
				return;
				
			} else if(buffer_idx==data_length)
			{
				break;
			}
			
		}
	}



// --- ( 1 - 0 )  Exec macro routinue 
	set_mouse_cursor(MOUSE_IMG_STAND);

	switch(command)
	{
	case MC_SAY:							// 广播 window		2,0,0,0,0,0
		main_loop_active_task=mc_exec_say;
		break;
	case MC_ASK:							// 询问 window		2,2,0,0,0,0
		main_loop_active_task=mc_exec_ask;
		break;
// ---
	case MC_STORE_BUY:						// 商店买
		if(!character_pocket_data.active)
			request_character_pocket_data();
		clear_store_data();
		main_loop_active_task=mc_exec_store_buy;
		break;
	case MC_STORE_SELL:						// 商店卖
		if(!character_pocket_data.active)
			request_character_pocket_data();
		main_loop_active_task=mc_exec_store_sell;
		break;
	case MC_STORE_OPEN:						// 开店面
		if(!character_pocket_data.active)
			request_character_pocket_data();
//		main_loop_active_task=mc_exec_store_sellmake;
		break;
	case MC_POPSHOP_PUSH:
		if(!character_pocket_data.active)
			request_character_pocket_data();
		character_pocket_data.sell_number = 0;
		main_loop_active_task=mc_exec_popshop_push;
		break;
	case MC_POPSHOP_POP:
		character_popshop_data.sell_number=1;
		if(!character_popshop_data.active)
			request_character_popshop_data();	// 要求当铺资料
		character_popshop_data.idx=-1;
		main_loop_active_task=mc_exec_popshop_pop;
		break;
	case MC_BANK_STORE:						// 存款
		SetRect(&rect, 70-2, 117-2, 70+102-2, 117+16-2);
		g_BankEdit.Init(rect, 8);
		g_BankEdit.setMaxNum(character_bank_data.now_money);
		main_loop_active_task=mc_exec_bank_store;
		break;
	case MC_BANK_WITHDRAW:					// 提款
		SetRect(&rect, 70-2, 117-2, 70-2+102, 117+16-2);
		g_BankEdit.Init(rect, 8);
		g_BankEdit.setMaxNum(character_bank_data.bank_money);
		main_loop_active_task=mc_exec_bank_withdraw;
		break;
	case MC_SYSTEM_MESSAGE:
		display_system_message(macro_string_var[0]);
		break;
	case MC_CHAT_MESSAGE:
		push_chat_data(CHAT_CHANNEL_SYSTEM,0,(UCHR *)"",macro_string_var[0]);
		break;
		

	default:
		sprintf((char *)print_rec,"Not define macro command:%d",command);
		display_error_message(print_rec,1);
		break;
	}
	

}



void clear_store_data(void)
{
	SLONG i;

	store_data.active=false;
	for(i=0;i<MAX_POCKET_ITEM;i++)
	{
		store_data.item[i].item_id=-1;
	}

	store_data.idx=-1;
	store_data.buy_number=0;
	store_data.total=0;


}



void clear_macro_var(void)
{
	SLONG i;

	for(i=0;i<MAX_MACRO_PARAMETER;i++)
	{
		macro_number_var[i]=-1;
		macro_string_var[i][0]=NULL;
	}

}



void mc_exec_say(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	SLONG npc_idx;
	SLONG text_x,text_y,text_xl,text_yl;
	SLONG mda_command;
	static ULONG timer=0;
	

	window_x=system_image.mc_window_body.rect.left;
	window_y=system_image.mc_window_body.rect.top;
	window_xl=system_image.mc_window_body.rect.right-system_image.mc_window_body.rect.left;
	window_yl=system_image.mc_window_body.rect.bottom-system_image.mc_window_body.rect.top;

	text_x=window_x+146;
	text_y=window_y+29;
	text_xl=472;
	text_yl=122;
	

// --- ( 0 - 0 ) Draw body 
	display_img256_buffer(window_x,window_y,system_image.mc_window_body.image,screen_buffer);	
	


// --- ( 0 - 1 ) display data 
// ---- display npc 
	if(system_control.macro_npc_id!=-1)
	{
		mda_command=MDA_COMMAND_LOOP;

		if(system_control.system_timer>timer)
		{
			mda_command=mda_command|MDA_COMMAND_NEXT_FRAME;
			timer=system_control.system_timer+SYSTEM_MAIN_DELAY;
		}
		npc_idx=get_map_npc_index(system_control.macro_npc_id);
		if((npc_idx>=0)&&(npc_idx<total_map_npc))
		{
			if(system_control.mouse_x<window_x+73)
				mda_group_exec(map_npc_group[npc_idx].handle,(UCHR *)"MainCommand1",window_x+73,window_y+143-10,
				screen_buffer,mda_command,0,0,0);
			else			
				mda_group_exec(map_npc_group[npc_idx].handle,(UCHR *)"MainCommand3",window_x+73,window_y+143-10,
				screen_buffer,mda_command,0,0,0);
			
			set_word_color(0,color_control.yellow);
			print16(window_x+28,window_y+165,map_npc_group[npc_idx].npc_info.name,PEST_PUT,screen_buffer);
			set_word_color(0,color_control.white);
		}

	}
	
// ---- display text
	text_out_data.g_pChat->updateString(text_x,text_y,text_xl,text_yl,
		(char *)macro_string_var[0]);
	text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,screen_buffer,false);


// --- ( 1 - 0 ) Exec & Control
	if((system_control.mouse_x>=window_x)&&(system_control.mouse_x<window_x+window_xl)&&
		(system_control.mouse_y>=window_y)&&(system_control.mouse_y<window_y+window_yl))
	{
		switch(system_control.mouse_key)
		{
		case MS_LUp:				// close
			main_loop_active_task=NULL;		
			reset_mouse_key();
			system_control.mouse_key=MS_Dummy;
			send_macro_return(MC_SAY,-1);
			break;
		}
	}

// --- check is 队员
	if((system_control.control_npc_idx>=0)&&(system_control.control_npc_idx<total_map_npc))
	{
		if(map_npc_group[system_control.control_npc_idx].npc_info.status & NPC_STATUS_FOLLOW)
		{
		main_loop_active_task=NULL;		
		reset_mouse_key();
		system_control.mouse_key=MS_Dummy;
		send_macro_return(MC_SAY,-1);
		}
	}


// ---- clear all input 
	system_control.mouse_key=MS_Dummy;
	system_control.key=NULL;
	system_control.data_key=NULL;
	
}


void mc_exec_ask(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	SLONG npc_idx;
	SLONG text_x,text_y,text_xl,text_yl;
	SLONG mda_command;

	SLONG total_menu_item;
	MENU_ITEM_DATA menu_data[MAX_MENU_ITEM_LIST];
	SLONG i;
	SLONG temp_x,temp_y;
	RECT test_rect;
	SLONG backup_y;
	SLONG task;


	static ULONG timer=0;
	
	
	window_x=system_image.mc_window_body.rect.left;
	window_y=system_image.mc_window_body.rect.top;
	window_xl=system_image.mc_window_body.rect.right-system_image.mc_window_body.rect.left;
	window_yl=system_image.mc_window_body.rect.bottom-system_image.mc_window_body.rect.top;
	
	text_x=window_x+146;
	text_y=window_y+29;
	text_xl=472;
	text_yl=122;
	
	
	// --- ( 0 - 0 ) Draw body 
	display_img256_buffer(window_x,window_y,system_image.mc_window_body.image,screen_buffer);	
	
	
	
	// --- ( 0 - 1 ) display data 
	// ---- display npc 
	if(system_control.macro_npc_id!=-1)
	{
		mda_command=MDA_COMMAND_LOOP;
		
		if(system_control.system_timer>timer)
		{
			mda_command=mda_command|MDA_COMMAND_NEXT_FRAME;
			timer=system_control.system_timer+SYSTEM_MAIN_DELAY;
		}
		npc_idx=get_map_npc_index(system_control.macro_npc_id);

		if((npc_idx>=0)&&(npc_idx<total_map_npc))
		{
			if(system_control.mouse_x<window_x+73)
				mda_group_exec(map_npc_group[npc_idx].handle,(UCHR *)"MainCommand1",window_x+73,window_y+143-10,
				screen_buffer,mda_command,0,0,0);
			else
				mda_group_exec(map_npc_group[npc_idx].handle,(UCHR *)"MainCommand3",window_x+73,window_y+143-10,
				screen_buffer,mda_command,0,0,0);
			
			
			set_word_color(0,color_control.yellow);
			print16(window_x+28,window_y+165,map_npc_group[npc_idx].npc_info.name,PEST_PUT,screen_buffer);
			set_word_color(0,color_control.white);
		}

	}
	
// ---- display text
// ---- display title
	text_out_data.g_pChat->updateString(text_x,text_y,text_xl,text_yl,
		(char *)macro_string_var[0]);

	test_rect.left=text_x;
	test_rect.top=text_y;
	test_rect.right=text_x+text_xl;
	test_rect.bottom=text_y+text_yl;
	text_out_data.g_pChat->insertString((char *)macro_string_var[0],test_rect,false);
	

	decode_menu_item(macro_string_var[1],&total_menu_item,&menu_data[0]);
	
	temp_x=text_x+16;
	temp_y=text_y+(test_rect.bottom-test_rect.top);
	backup_y=temp_y;

	for(i=0;i<total_menu_item;i++)
	{

		test_rect.left=0;
		test_rect.top=0;
		test_rect.right=SCREEN_WIDTH;
		test_rect.bottom=SCREEN_HEIGHT;

		text_out_data.g_pChat->insertString((char *)menu_data[i].text,test_rect,false);

		menu_data[i].pos.left=temp_x;
		menu_data[i].pos.top=temp_y;
		menu_data[i].pos.right=temp_x+test_rect.right;
		menu_data[i].pos.bottom=temp_y+test_rect.bottom;

		temp_y=temp_y+test_rect.bottom;
		if(temp_y>=window_y+window_yl-20)
		{
			temp_x=text_x+text_xl/2;
			temp_y=backup_y;
			menu_data[i].pos.left=temp_x;
			menu_data[i].pos.top=temp_y;
			menu_data[i].pos.right=temp_x+test_rect.right;
			menu_data[i].pos.bottom=temp_y+test_rect.bottom;

			temp_y=temp_y+test_rect.bottom;
			
		}

	}


	task=-1;
	for(i=0;i<total_menu_item;i++)
	{
		if((system_control.mouse_x>=menu_data[i].pos.left)&&(system_control.mouse_x<menu_data[i].pos.right)&&
			(system_control.mouse_y>=menu_data[i].pos.top)&&(system_control.mouse_y<menu_data[i].pos.bottom))
		{
			task=i;
		}
		
	}
	


	for(i=0;i<total_menu_item;i++)
	{
		if(task==i)
		{
			put_bar(menu_data[i].pos.left,menu_data[i].pos.top,
				menu_data[i].pos.right-menu_data[i].pos.left,
				menu_data[i].pos.bottom-menu_data[i].pos.top,color_control.red,screen_buffer);
		}
		
		text_out_data.g_pChat->updateString(menu_data[i].pos.left,menu_data[i].pos.top,
			menu_data[i].pos.right-menu_data[i].pos.left,
			menu_data[i].pos.bottom-menu_data[i].pos.top,
			(char *)menu_data[i].text);
		
	}
		

	text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,screen_buffer,false);
	
	
	// --- ( 1 - 0 ) Exec & Control
	if((system_control.mouse_x>=window_x)&&(system_control.mouse_x<window_x+window_xl)&&
		(system_control.mouse_y>=window_y)&&(system_control.mouse_y<window_y+window_yl))
	{

		switch(system_control.mouse_key)
		{
		case MS_LUp:				// close
			if(task<0)break;
			main_loop_active_task=NULL;		
			reset_mouse_key();
			system_control.mouse_key=MS_Dummy;
			send_macro_return(MC_ASK,task);
			break;
		}
	}
	

	// --- check is 队员
	if((system_control.control_npc_idx>=0)&&(system_control.control_npc_idx<total_map_npc))
	{
		if(map_npc_group[system_control.control_npc_idx].npc_info.status & NPC_STATUS_FOLLOW)
		{
			main_loop_active_task=NULL;		
			reset_mouse_key();
			system_control.mouse_key=MS_Dummy;
			send_macro_return(MC_ASK,-1);
		}
	}
	
	
	// ---- clear all input 
	system_control.mouse_key=MS_Dummy;
	system_control.key=NULL;
	system_control.data_key=NULL;
	reset_mouse_key();
	reset_key();
	
}




void decode_menu_item(UCHR *data,SLONG *total,MENU_ITEM_DATA *item)
{
	ULONG len;
	ULONG i;
	ULONG word_idx;
	SLONG item_idx;

	*total=0;
	for(i=0;i<MAX_MENU_ITEM_LIST;i++)
	{
		item[i].text[0]=NULL;

	}


	len=strlen((char *)data);
	if(len==0)return;
	word_idx=0;
	item_idx=0;
	for(i=0;i<len;i++)
	{
		switch(data[i])
		{
		case '@':
			item[item_idx].text[word_idx]=NULL;				// end of line
			item_idx++;
			if(item_idx>=MAX_MENU_ITEM_LIST)
			{
				*total=item_idx;
				return;
			}
			word_idx=0;
			break;
		default:
			item[item_idx].text[word_idx]=data[i];			// save 
			word_idx++;
			break;
		}
	}

	if((word_idx>0)&&(item_idx<MAX_MENU_ITEM_LIST))
	{
		item[item_idx].text[word_idx]=NULL;				// end of line
		item_idx++;
		word_idx=0;
	}

	*total=item_idx;
				
}





// ------ Send Macro return
void send_macro_return(UCHR command,SLONG ret_val)
{
	postNetMessage(NET_MSG_PLAYER_MACRO_SELECT, ret_val);
}





void exec_conform(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	SLONG text_x,text_y,text_xl,text_yl;
	SLONG offset_x,offset_y;
	SLONG channel;

	SLONG i;
	RECT rect[2]={
		{ 53, 88, 53+63, 88+21 },
		{ 174,88,174+63, 88+21 }
	};
	SLONG task;

	
	window_x=system_image.conform_body.rect.left;
	window_y=system_image.conform_body.rect.top;
	window_xl=system_image.conform_body.rect.right-system_image.conform_body.rect.left;
	window_yl=system_image.conform_body.rect.bottom-system_image.conform_body.rect.top;
	
	text_x=window_x+27;
	text_y=window_y+27;
	text_xl=242;
	text_yl=64;
	
	offset_x=system_control.mouse_x-window_x;
	offset_y=system_control.mouse_y-window_y;

	task=-1;
	for(i=0;i<2;i++)
	{
		if((offset_x>rect[i].left)&&(offset_x<=rect[i].right)&&
			(offset_y>rect[i].top)&&(offset_y<=rect[i].bottom))
			task=i;
	}
	
	// --- ( 0 - 0 ) Draw body 
	display_img256_buffer(window_x,window_y,system_image.conform_body.image,screen_buffer);	
	
	
	
	// --- ( 0 - 1 ) display data 
	
// ---- display title
	text_out_data.g_pChat->updateString(text_x,text_y,text_xl,text_yl,
		(char *)conform_text);


	text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,screen_buffer,false);
	
	
	// --- ( 1 - 0 ) Exec & Control
	if((system_control.mouse_x>=window_x)&&(system_control.mouse_x<window_x+window_xl)&&
		(system_control.mouse_y>=window_y)&&(system_control.mouse_y<window_y+window_yl))
	{

		switch(system_control.mouse_key)
		{
		case MS_RUp:				// close
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			main_loop_active_task=NULL;		
			break;
		case MS_Move:
		case MS_Dummy:
			if(task<0)break;
			switch(task)
			{
			case 0:		// OK
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand27",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
			case 1:		// Cancel
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand29",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
			}			
			break;

		case MS_LUp:
			if(task<0)break;
			switch(task)
			{
			case 0:		// OK
				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
				if(ask_yes_exec_task)
					ask_yes_exec_task();
				main_loop_active_task=NULL;		
				break;
			case 1:		// Cancel
				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
				if(ask_no_exec_task)
					ask_yes_exec_task();
				main_loop_active_task=NULL;		
				break;
			}			
			break;
		case MS_LDrag:
		case MS_LDn:
			if(task<0)break;
			switch(task)
			{
			case 0:		// OK
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand28",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
			case 1:		// Cancel
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand30",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
			}			
			break;

		}
	}
	
	
	// ---- clear all input 
	system_control.mouse_key=MS_Dummy;
	system_control.key=NULL;
	system_control.data_key=NULL;
	reset_mouse_key();
	reset_key();
	
}





void mc_exec_store_buy(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	SLONG offset_x,offset_y;
	SLONG channel;
	
	SLONG inst_x,inst_y,inst_xl,inst_yl;
	RECT test_rect;
	
	
	SLONG i;
	RECT rect[2]={
		{ 247,  9,247+28, 9+28 },		// close 
		{ 192,261,192+63, 261+21 }		// 确定
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
	
	SLONG task;
	static SLONG old_idx=-1;;
	
	
	window_x=system_image.item_public_body.rect.left;
	window_y=system_image.item_public_body.rect.top;
	window_xl=system_image.item_public_body.rect.right-system_image.item_public_body.rect.left;
	window_yl=(system_image.item_public_body.rect.bottom-system_image.item_public_body.rect.top)+
		(system_image.store_body.rect.bottom-system_image.store_body.rect.top);
	
	
	
	offset_x=system_control.mouse_x-window_x;
	offset_y=system_control.mouse_y-window_y;
	
	task=-1;
	for(i=0;i<2;i++)
	{
		if((offset_x>rect[i].left)&&(offset_x<=rect[i].right)&&
			(offset_y>rect[i].top)&&(offset_y<=rect[i].bottom))
			task=i;
	}
	
	
	if(store_data.active)
	{
		if(task<0)
		{
			for(i=0;i<MAX_POCKET_ITEM;i++)
			{
				if((offset_x>=item_pp[i].left)&&(offset_x<item_pp[i].right)&&
					(offset_y>=item_pp[i].top)&&(offset_y<item_pp[i].bottom))
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
	
	
	if(store_data.active)
	{
		if((task>=100)&&(task<200))
		{
			detail_item_inst.idx=task-100;
			if(( store_data.item[detail_item_inst.idx].number>0)&&(store_data.item[detail_item_inst.idx].filename_id>=0))
			{
				if(detail_item_inst.old_idx!=detail_item_inst.idx)
				{
					if(system_control.system_timer>detail_item_inst.timer+DETAIL_DELAY_TIME)
					{
						detail_item_inst.old_idx=detail_item_inst.idx;
						detail_item_inst.active=false;
						detail_item_inst.type=ITEM_INST_SELL_STORE;
						request_detail_inst();
						detail_item_inst.timer=system_control.system_timer;
					}
				}
			}
		}
	}
	
	
	
	// --- ( 0 - 0 ) Draw body 
	display_img256_buffer(window_x,window_y,system_image.item_public_body.image,screen_buffer);	
	display_img256_buffer(window_x+2,window_y+(system_image.item_public_body.rect.bottom-system_image.item_public_body.rect.top),system_image.store_body.image,screen_buffer);	
	display_img256_buffer(window_x+105,window_y+14,system_image.store_buy_mark.image,screen_buffer);	
	
	
	// --- ( 0 - 1 ) display data 
	if(store_data.active)
	{
		for(i=0;i<MAX_POCKET_ITEM;i++)
		{
			if((store_data.item[i].item_id>=0)&&(store_data.item[i].filename_id>=0))
			{
				display_img256_buffer(window_x+item_pp[i].left,window_y+item_pp[i].top,
					system_image.item_image[store_data.item[i].filename_id],screen_buffer);
			}
			
			if(i==store_data.idx)
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
	

	// ---------
	if(store_data.idx>=0)
	{
		if((store_data.item[store_data.idx].item_id>=0)&&(store_data.item[store_data.idx].filename_id>=0))		// 单价
		{
			sprintf((char *)print_rec,"%d",store_data.item[store_data.idx].number);
			print16(window_x+71,window_y+228-2,print_rec,PEST_PUT,screen_buffer);

			store_data.total=store_data.item[store_data.idx].number*store_data.buy_number;
			sprintf((char *)print_rec,"%d",store_data.total);								// 总价
			print16(window_x+71,window_y+282-2,print_rec,PEST_PUT,screen_buffer);

		}
	}


// --- 改成 edit
	sprintf((char *)print_rec,"%d",store_data.buy_number);							// 数量
	print16(window_x+71,window_y+255-2,print_rec,PEST_PUT,screen_buffer);
	

	sprintf((char *)print_rec,"%d",character_bank_data.now_money);					// 现金
	print16(window_x+71,window_y+309-2,print_rec,PEST_PUT,screen_buffer);
	
	
	// --- ( 1 - 0 ) Exec & Control
	if((system_control.mouse_x>=window_x)&&(system_control.mouse_x<window_x+window_xl)&&
		(system_control.mouse_y>=window_y)&&(system_control.mouse_y<window_y+window_yl))
	{

		// ------ display inst
		if(store_data.active)
		{

			if((detail_item_inst.active)&&(detail_item_inst.idx>=0))
			{
				if((store_data.item[detail_item_inst.idx].number>0)&&(store_data.item[detail_item_inst.idx].filename_id>=0))
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
						system_image.item_image[store_data.item[detail_item_inst.idx].filename_id],screen_buffer,200);
					
					text_out_data.g_pChat->updateString(inst_x+16+64,inst_y+16,test_rect.right,inst_yl,detail_item_inst.inst);
					text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,screen_buffer,false);
				}
				
				
			}
			
		}
		
		
		
		
		
		switch(system_control.mouse_key)
		{
		case MS_RUp:				// close
			if(task<0)
			{
				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
				main_loop_active_task=NULL;		
				break;
			}
			switch(task)
			{
			case 100: case 101: case 102: case 103: case 104: case 105:
			case 106: case 107: case 108: case 109: case 110: case 111:
			case 112: case 113: case 114: case 115: case 116: case 117:
			case 118: case 119: case 120: case 121: case 122: case 123:
				store_data.idx=task-100;
				if((store_data.idx<0)||(store_data.idx>=24))break;
				if(store_data.item[store_data.idx].item_id<0)
				{
					store_data.buy_number=0;
					break;
				}
				if(old_idx!=store_data.idx)
				{
					store_data.buy_number=1;
					old_idx=store_data.idx;
				}
				else
				{
					if(store_data.buy_number>1)
						store_data.buy_number--;
				}
				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
				break;				
				
			}

			break;
		case MS_Move:
		case MS_Dummy:
			if(task<0)break;
			switch(task)
			{
			case 0:		// Close
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
			case 1:		// ok
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand27",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
			}			
			break;
			
		case MS_LUp:
			if(task<0)break;
			switch(task)
			{
			case 0:		// cancel
				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
				main_loop_active_task=NULL;		
				break;
			case 1:		// Ok
				if((store_data.idx<0)||(store_data.idx>=24))break;
				if(store_data.item[store_data.idx].item_id<0)break;
				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
				set_buy_item_index(store_data.idx);
				store_data.buy_number=1;
				store_data.idx=-1;
				old_idx=-1;
				break;
			case 100: case 101: case 102: case 103: case 104: case 105:
			case 106: case 107: case 108: case 109: case 110: case 111:
			case 112: case 113: case 114: case 115: case 116: case 117:
			case 118: case 119: case 120: case 121: case 122: case 123:
				store_data.idx=task-100;
				if((store_data.idx<0)||(store_data.idx>=24))break;
				if(store_data.item[store_data.idx].item_id<0)
				{
					store_data.buy_number=0;
					break;
				}
				if(old_idx!=store_data.idx)
				{
					store_data.buy_number=1;
					old_idx=store_data.idx;
				}
				else
				{
					if(character_bank_data.now_money>=store_data.item[store_data.idx].number*(store_data.buy_number+1))
						store_data.buy_number++;
				}

				auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
				break;
			}
			break;
		case MS_LDrag:
		case MS_LDn:
			if(task<0)break;
			switch(task)
			{
			case 0:		// close
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
			case 1:		// OK
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand28",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
			}			
			break;
					
		}
	}
	

	// --- check is 队员
	if((system_control.control_npc_idx>=0)&&(system_control.control_npc_idx<total_map_npc))
	{
		if(map_npc_group[system_control.control_npc_idx].npc_info.status & NPC_STATUS_FOLLOW)
		{
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			main_loop_active_task=NULL;		
		}
	}
	
	
	// ---- clear all input 
	system_control.mouse_key=MS_Dummy;
	system_control.key=NULL;
	system_control.data_key=NULL;
	reset_mouse_key();
	reset_key();
	
}

	



void mc_exec_store_sell(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	SLONG offset_x,offset_y;
	SLONG channel;
	SLONG total_money;

	SLONG inst_x,inst_y,inst_xl,inst_yl;
	RECT test_rect;
	

	SLONG i;
	RECT rect[2]={
		{ 247,  9,247+28, 9+28 },		// close 
		{ 192,261,192+63, 261+21 }		// 确定
	};
	SLONG task;

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
	
	static int old_idx=-1;	

	
	window_x=system_image.item_public_body.rect.left;
	window_y=system_image.item_public_body.rect.top;
	window_xl=system_image.item_public_body.rect.right-system_image.item_public_body.rect.left;
	window_yl=(system_image.item_public_body.rect.bottom-system_image.item_public_body.rect.top)+
		(system_image.store_body.rect.bottom-system_image.store_body.rect.top);

	

	offset_x=system_control.mouse_x-window_x;
	offset_y=system_control.mouse_y-window_y;

	task=-1;
	for(i=0;i<2;i++)
	{
		if((offset_x>rect[i].left)&&(offset_x<=rect[i].right)&&
			(offset_y>rect[i].top)&&(offset_y<=rect[i].bottom))
			task=i;
	}

	if(character_pocket_data.active)
	{
		if(task<0)
		{
			for(i=0;i<MAX_POCKET_ITEM;i++)
			{
				if((offset_x>=item_pp[i].left)&&(offset_x<item_pp[i].right)&&
					(offset_y>=item_pp[i].top)&&(offset_y<item_pp[i].bottom))
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
	

	if(character_pocket_data.active)
	{
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
	}
		
	

	
	// --- ( 0 - 0 ) Draw body 
	display_img256_buffer(window_x,window_y,system_image.item_public_body.image,screen_buffer);	
	display_img256_buffer(window_x+2,window_y+(system_image.item_public_body.rect.bottom-system_image.item_public_body.rect.top),system_image.store_body.image,screen_buffer);	
	display_img256_buffer(window_x+105,window_y+14,system_image.store_sell_mark.image,screen_buffer);	
	
	
	// --- ( 0 - 1 ) display data 
	if(character_pocket_data.active)
	{
		for(i=0;i<MAX_POCKET_ITEM;i++)
		{
			if((character_pocket_data.item[i].item_id>=0)&&(character_pocket_data.item[i].filename_id>=0))
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
	
	
	total_money=0;
	// ---------
	if(character_pocket_data.idx>=0)
	{
		if((character_pocket_data.item[character_pocket_data.idx].item_id>=0)&&(character_pocket_data.item[character_pocket_data.idx].filename_id>=0))		// 单价
		{
			sprintf((char *)print_rec,"%d",character_pocket_data.item[character_pocket_data.idx].single_price);
			print16(window_x+71,window_y+228-2,print_rec,PEST_PUT,screen_buffer);			// 单价
			
			total_money=character_pocket_data.sell_number*character_pocket_data.item[character_pocket_data.idx].single_price;
			sprintf((char *)print_rec,"%d",total_money);								// 总价
			print16(window_x+71,window_y+282-2,print_rec,PEST_PUT,screen_buffer);
			
		}
	}
	
	
	// --- 改成 edit
	sprintf((char *)print_rec,"%d",character_pocket_data.sell_number);				// 数量
	print16(window_x+71,window_y+255-2,print_rec,PEST_PUT,screen_buffer);
	
	
	sprintf((char *)print_rec,"%d",character_bank_data.now_money);					// 现金
	print16(window_x+71,window_y+309-2,print_rec,PEST_PUT,screen_buffer);
	
	

	



	// --- ( 1 - 0 ) Exec & Control
	if((system_control.mouse_x>=window_x)&&(system_control.mouse_x<window_x+window_xl)&&
		(system_control.mouse_y>=window_y)&&(system_control.mouse_y<window_y+window_yl))
	{

		// ------ display inst
		if(character_pocket_data.active)
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
			

		switch(system_control.mouse_key)
		{
		case MS_RUp:				// close
			if(task<0)
			{
				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
				main_loop_active_task=NULL;		
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
				if(character_pocket_data.item[character_pocket_data.idx].item_id<0)
				{
					character_pocket_data.sell_number=0;
					break;
				}
				if(old_idx!=character_pocket_data.idx)
				{
					character_pocket_data.sell_number=1;
					old_idx=character_pocket_data.idx;
				}
				else
				{
					character_pocket_data.sell_number=character_pocket_data.item[character_pocket_data.idx].number;
				}
				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
				break;				
				
			}
			break;
		case MS_Move:
		case MS_Dummy:
			if(task<0)break;
			switch(task)
			{
			case 0:		// Close
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
			case 1:		// ok
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand27",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
			}			
			break;

		case MS_LUp:
			if(task<0)break;
			switch(task)
			{
			case 0:		// cancel
				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
				main_loop_active_task=NULL;		
				break;
			case 1:		// Ok
				if((character_pocket_data.idx<0)||(character_pocket_data.idx>=24))break;
				if(character_pocket_data.item[character_pocket_data.idx].item_id<0)break;
				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
				set_sell_item_index(character_pocket_data.idx,character_pocket_data.sell_number);
				character_pocket_data.sell_number=1;
				character_pocket_data.idx=-1;
				old_idx=-1;
				break;
			case 100: case 101: case 102: case 103: case 104: case 105:
			case 106: case 107: case 108: case 109: case 110: case 111:
			case 112: case 113: case 114: case 115: case 116: case 117:
			case 118: case 119: case 120: case 121: case 122: case 123:
				character_pocket_data.idx=task-100;
				if((character_pocket_data.idx<0)||(character_pocket_data.idx>=24))break;
				if(character_pocket_data.item[character_pocket_data.idx].item_id<0)
				{
					character_pocket_data.sell_number=0;
					break;
				}
				if(old_idx!=character_pocket_data.idx)
				{
					character_pocket_data.sell_number=1;
					old_idx=character_pocket_data.idx;
				}
				else
				{
					if(character_pocket_data.sell_number<character_pocket_data.item[character_pocket_data.idx].number)
						character_pocket_data.sell_number++;
				}
				
				auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
				break;
			}			
			break;
		case MS_LDrag:
		case MS_LDn:
			if(task<0)break;
			switch(task)
			{
			case 0:		// close
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
			case 1:		// OK
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand28",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
			}			
			break;

		}
	}
	

	// --- check is 队员
	if((system_control.control_npc_idx>=0)&&(system_control.control_npc_idx<total_map_npc))
	{
		if(map_npc_group[system_control.control_npc_idx].npc_info.status & NPC_STATUS_FOLLOW)
		{
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			main_loop_active_task=NULL;		
		}
	}
				
	
	// ---- clear all input 
	system_control.mouse_key=MS_Dummy;
	system_control.key=NULL;
	system_control.data_key=NULL;
	reset_mouse_key();
	reset_key();
	
}






void mc_exec_bank_store(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	SLONG offset_x,offset_y;
	SLONG channel;

	SLONG i;
	RECT rect[3]={
		{ 165, 10,165+28, 10+28 },		// close 
		{  28,146, 28+63, 146+21 },		// 存钱
		{ 107,146,107+63, 146+21 }		// 取钱
	};
	SLONG task;
	static SLONG store_money=0;

	
	window_x=system_image.bank_body.rect.left;
	window_y=system_image.bank_body.rect.top;
	window_xl=system_image.bank_body.rect.right-system_image.bank_body.rect.left;
	window_yl=system_image.bank_body.rect.bottom-system_image.bank_body.rect.top;


	offset_x=system_control.mouse_x-window_x;
	offset_y=system_control.mouse_y-window_y;

	task=-1;
	for(i=0;i<3;i++)
	{
		if((offset_x>rect[i].left)&&(offset_x<=rect[i].right)&&
			(offset_y>rect[i].top)&&(offset_y<=rect[i].bottom))
			task=i;
	}
	
	// --- ( 0 - 0 ) Draw body 
	display_img256_buffer(window_x,window_y,system_image.bank_body.image,screen_buffer);	
	
	mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand100",window_x+rect[2].left,window_y+rect[2].top,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_PALETTE_GRAY,0,0,0);
				

	// --- ( 0 - 1 ) display data 
	sprintf((char *)print_rec,"%d",character_bank_data.now_money);		// 现金
	print16(window_x+70,window_y+57-2,print_rec,PEST_PUT,screen_buffer);

	sprintf((char *)print_rec,"%d",character_bank_data.bank_money);		// 存款
	print16(window_x+70,window_y+87-2,print_rec,PEST_PUT,screen_buffer);

//	sprintf((char *)print_rec,"%d",store_money);		// 金额
//	print16(window_x+70,window_y+117-2,print_rec,PEST_PUT,screen_buffer);
	
	UpdateEditKey();

	g_BankEdit.Paint(window_x, window_y, screen_buffer);
	
	
	
	// --- ( 1 - 0 ) Exec & Control
	if((system_control.mouse_x>=window_x)&&(system_control.mouse_x<window_x+window_xl)&&
		(system_control.mouse_y>=window_y)&&(system_control.mouse_y<window_y+window_yl))
	{

		switch(system_control.mouse_key)
		{
		case MS_RUp:				// close
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			main_loop_active_task=NULL;		
			store_money=0;
			g_BankEdit.Shutdown();
			break;
		case MS_Move:
		case MS_Dummy:
			if(task<0)break;
			switch(task)
			{
			case 0:		// Close
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
			case 1:		// 存钱
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand98",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
//			case 2:		// 提钱
//				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand100",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
//				break;
			}			
			break;

		case MS_LUp:
			if(task<0)break;
			switch(task)
			{
			case 0:		// cancel
				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
				main_loop_active_task=NULL;		
				store_money=0;
				break;
			case 1:		// 存钱
				store_money = g_BankEdit.getNumber();
				if(store_money<=0)break;
				if(store_money>character_bank_data.now_money)break;
				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
				send_store_money(store_money);
				main_loop_active_task=NULL;		
				store_money=0;
				g_BankEdit.Shutdown();
				break;
//			case 2:		// 取钱
//				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
//				break;
			}			
			break;
		case MS_LDrag:
		case MS_LDn:
			g_BankEdit.LBtnDown(system_control.mouse_x, system_control.mouse_y);
			if(task<0)break;
			switch(task)
			{
			case 0:		// Close
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
			case 1:		// 存钱
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand99",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
//			case 2:		// 提钱
//				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand101",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
//				break;
			}			
			break;

		}
	}

	
	// --- check is 队员
	if((system_control.control_npc_idx>=0)&&(system_control.control_npc_idx<total_map_npc))
	{
		if(map_npc_group[system_control.control_npc_idx].npc_info.status & NPC_STATUS_FOLLOW)
		{
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			main_loop_active_task=NULL;		
			store_money=0;
			g_BankEdit.Shutdown();
		}
	}
	
	
	// ---- clear all input 
	system_control.mouse_key=MS_Dummy;
	system_control.key=NULL;
	system_control.data_key=NULL;
	reset_mouse_key();
	reset_key();
	
}





void mc_exec_bank_withdraw(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	SLONG offset_x,offset_y;
	SLONG channel;

	SLONG i;
	RECT rect[3]={
		{ 165, 10,165+28, 10+28 },		// close 
		{  28,146, 28+63, 146+21 },		// 存钱
		{ 107,146,107+63, 146+21 }		// 取钱
	};
	SLONG task;
	static SLONG store_money=0;

	
	window_x=system_image.bank_body.rect.left;
	window_y=system_image.bank_body.rect.top;
	window_xl=system_image.bank_body.rect.right-system_image.bank_body.rect.left;
	window_yl=system_image.bank_body.rect.bottom-system_image.bank_body.rect.top;


	offset_x=system_control.mouse_x-window_x;
	offset_y=system_control.mouse_y-window_y;

	task=-1;
	for(i=0;i<3;i++)
	{
		if((offset_x>rect[i].left)&&(offset_x<=rect[i].right)&&
			(offset_y>rect[i].top)&&(offset_y<=rect[i].bottom))
			task=i;
	}
	
	// --- ( 0 - 0 ) Draw body 
	display_img256_buffer(window_x,window_y,system_image.bank_body.image,screen_buffer);	
	
	mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand98",window_x+rect[1].left,window_y+rect[1].top,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_PALETTE_GRAY,0,0,0);
				

	// --- ( 0 - 1 ) display data 
	sprintf((char *)print_rec,"%d",character_bank_data.now_money);		// 现金
	print16(window_x+70,window_y+57-2,print_rec,PEST_PUT,screen_buffer);

	sprintf((char *)print_rec,"%d",character_bank_data.bank_money);		// 存款
	print16(window_x+70,window_y+87-2,print_rec,PEST_PUT,screen_buffer);

//	sprintf((char *)print_rec,"%d",store_money);		// 金额
//	print16(window_x+70,window_y+117-2,print_rec,PEST_PUT,screen_buffer);
	
	UpdateEditKey();

	g_BankEdit.Paint(window_x, window_y, screen_buffer);;
	
		
	// --- ( 1 - 0 ) Exec & Control
	if((system_control.mouse_x>=window_x)&&(system_control.mouse_x<window_x+window_xl)&&
		(system_control.mouse_y>=window_y)&&(system_control.mouse_y<window_y+window_yl))
	{

		switch(system_control.mouse_key)
		{
		case MS_RUp:				// close
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			main_loop_active_task=NULL;		
			store_money=0;
			g_BankEdit.Shutdown();
			break;
		case MS_Move:
		case MS_Dummy:
			if(task<0)break;
			switch(task)
			{
			case 0:		// Close
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
//			case 1:		// 存钱
//				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand98",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
//				break;
			case 2:		// 提钱
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand100",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
			}			
			break;

		case MS_LUp:
			if(task<0)break;
			switch(task)
			{
			case 0:		// cancel
				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
				main_loop_active_task=NULL;		
				store_money=0;
				g_BankEdit.Shutdown();
				break;
//			case 1:		// 存钱
//				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
//				break;
			case 2:		// 取钱
				store_money = g_BankEdit.getNumber();
				if(store_money<=0)break;
				if(store_money>character_bank_data.bank_money)break;
				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
				send_withdraw_money(store_money);
				main_loop_active_task=NULL;		
				store_money=0;
				g_BankEdit.Shutdown();
				break;
			}			
			break;
		case MS_LDrag:
		case MS_LDn:
			if(task<0)break;
			switch(task)
			{
			case 0:		// Close
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
//			case 1:		// 存钱
//				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand99",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
//				break;
			case 2:		// 提钱
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand101",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
			}			
			break;

		}
	}
	

	// --- check is 队员
	if((system_control.control_npc_idx>=0)&&(system_control.control_npc_idx<total_map_npc))
	{
		if(map_npc_group[system_control.control_npc_idx].npc_info.status & NPC_STATUS_FOLLOW)
		{
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			main_loop_active_task=NULL;		
			store_money=0;
			g_BankEdit.Shutdown();
		}
	}
	
	
	// ---- clear all input 
	system_control.mouse_key=MS_Dummy;
	system_control.key=NULL;
	system_control.data_key=NULL;
	reset_mouse_key();
	reset_key();
	
}





void mc_exec_popshop_push(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	SLONG offset_x,offset_y;
	SLONG channel;
	SLONG total_money;

	SLONG inst_x,inst_y,inst_xl,inst_yl;
	RECT test_rect;
	
	SLONG i;
	RECT rect[2]={
		{ 247,  9,247+28, 9+28 },		// close 
		{ 192,261,192+63, 261+21 }		// 确定
	};
	SLONG task;

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
	
	static int old_idx=-1;	

	
	window_x=system_image.item_public_body.rect.left;
	window_y=system_image.item_public_body.rect.top;
	window_xl=system_image.item_public_body.rect.right-system_image.item_public_body.rect.left;
	window_yl=(system_image.item_public_body.rect.bottom-system_image.item_public_body.rect.top)+
		(system_image.store_body.rect.bottom-system_image.store_body.rect.top);

	
	
	offset_x=system_control.mouse_x-window_x;
	offset_y=system_control.mouse_y-window_y;

	task=-1;
	for(i=0;i<2;i++)
	{
		if((offset_x>rect[i].left)&&(offset_x<=rect[i].right)&&
			(offset_y>rect[i].top)&&(offset_y<=rect[i].bottom))
			task=i;
	}

	if(character_pocket_data.active)
	{
		if(task<0)
		{
			for(i=0;i<MAX_POCKET_ITEM;i++)
			{
				if((offset_x>=item_pp[i].left)&&(offset_x<item_pp[i].right)&&
					(offset_y>=item_pp[i].top)&&(offset_y<item_pp[i].bottom))
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
	

	if(character_pocket_data.active)
	{
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
	}
	
	

	// --- ( 0 - 0 ) Draw body 
	display_img256_buffer(window_x,window_y,system_image.item_public_body.image,screen_buffer);	
	display_img256_buffer(window_x+2,window_y+(system_image.item_public_body.rect.bottom-system_image.item_public_body.rect.top),system_image.store_body.image,screen_buffer);	
	display_img256_buffer(window_x+105,window_y+14,system_image.popshop_push_mark.image,screen_buffer);	
	
	
	// --- ( 0 - 1 ) display data 
	if(character_pocket_data.active)
	{
		for(i=0;i<MAX_POCKET_ITEM;i++)
		{
			if((character_pocket_data.item[i].item_id>=0)&&(character_pocket_data.item[i].filename_id>=0))
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
	
	
	total_money=0;
	// ---------
	if(character_pocket_data.idx>=0)
	{
		if((character_pocket_data.item[character_pocket_data.idx].item_id>=0)&&(character_pocket_data.item[character_pocket_data.idx].filename_id>=0))		// 单价
		{
// 当铺 存取 不用钱			
//			sprintf((char *)print_rec,"%d",character_pocket_data.item[character_pocket_data.idx].single_price);
//			sprintf((char *)print_rec,"%d",0);
			strcpy((char *)print_rec,"0");
			print16(window_x+71,window_y+228-2,print_rec,PEST_PUT,screen_buffer);			// 单价

// 当铺 存取 不用钱			
//			total_money=character_pocket_data.sell_number*character_pocket_data.item[character_pocket_data.idx].single_price;
//			sprintf((char *)print_rec,"%d",0);												// 总价
			strcpy((char *)print_rec,"0");
			print16(window_x+71,window_y+282-2,print_rec,PEST_PUT,screen_buffer);
			
		}
	}
	
	
	// --- 改成 edit
	sprintf((char *)print_rec,"%d",character_pocket_data.sell_number);				// 数量
	print16(window_x+71,window_y+255-2,print_rec,PEST_PUT,screen_buffer);
	
	
	sprintf((char *)print_rec,"%d",character_bank_data.now_money);					// 现金
	print16(window_x+71,window_y+309-2,print_rec,PEST_PUT,screen_buffer);
	
	
	// --- ( 1 - 0 ) Exec & Control
	if((system_control.mouse_x>=window_x)&&(system_control.mouse_x<window_x+window_xl)&&
		(system_control.mouse_y>=window_y)&&(system_control.mouse_y<window_y+window_yl))
	{

		// ------ display inst
		if(character_pocket_data.active)
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
		
		


		switch(system_control.mouse_key)
		{
		case MS_RUp:				// close
			if(task<0)
			{
				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
				main_loop_active_task=NULL;		
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
				if(character_pocket_data.item[character_pocket_data.idx].item_id<0)
				{
					character_pocket_data.sell_number=0;
					break;
				}
				if(old_idx!=character_pocket_data.idx)
				{
					character_pocket_data.sell_number=1;
					old_idx=character_pocket_data.idx;
				}
				else
				{
					character_pocket_data.sell_number=character_pocket_data.item[character_pocket_data.idx].number;
				}
				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
				break;				
				
			}
			break;
		case MS_Move:
		case MS_Dummy:
			if(task<0)break;
			switch(task)
			{
			case 0:		// Close
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
			case 1:		// ok
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand27",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
			}			
			break;

		case MS_LUp:
			if(task<0)break;
			switch(task)
			{
			case 0:		// cancel
				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
				main_loop_active_task=NULL;		
				break;
			case 1:		// Ok
				if((character_pocket_data.idx<0)||(character_pocket_data.idx>=24))break;
				if(character_pocket_data.item[character_pocket_data.idx].item_id<0)break;
				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
				set_push_item_index(character_pocket_data.idx,character_pocket_data.sell_number);
				character_pocket_data.sell_number=1;
				character_pocket_data.idx=-1;
				old_idx=-1;
				break;
			case 100: case 101: case 102: case 103: case 104: case 105:
			case 106: case 107: case 108: case 109: case 110: case 111:
			case 112: case 113: case 114: case 115: case 116: case 117:
			case 118: case 119: case 120: case 121: case 122: case 123:
				character_pocket_data.idx=task-100;
				if((character_pocket_data.idx<0)||(character_pocket_data.idx>=24))break;
				if(character_pocket_data.item[character_pocket_data.idx].item_id<0)
				{
					character_pocket_data.sell_number=0;
					break;
				}
				if(old_idx!=character_pocket_data.idx)
				{
					character_pocket_data.sell_number=1;
					old_idx=character_pocket_data.idx;
				}
				else
				{
					if(character_pocket_data.sell_number<character_pocket_data.item[character_pocket_data.idx].number)
						character_pocket_data.sell_number++;
				}
				
				auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
				break;
			}			
			break;
		case MS_LDrag:
		case MS_LDn:
			if(task<0)break;
			switch(task)
			{
			case 0:		// close
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
			case 1:		// OK
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand28",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
			}			
			break;

		}
	}
	

	// --- check is 队员
	if((system_control.control_npc_idx>=0)&&(system_control.control_npc_idx<total_map_npc))
	{
		if(map_npc_group[system_control.control_npc_idx].npc_info.status & NPC_STATUS_FOLLOW)
		{
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			main_loop_active_task=NULL;		
		}
	}
	
	
	// ---- clear all input 
	system_control.mouse_key=MS_Dummy;
	system_control.key=NULL;
	system_control.data_key=NULL;
	reset_mouse_key();
	reset_key();

}





void mc_exec_popshop_pop(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	SLONG offset_x,offset_y;
	SLONG channel;
	SLONG total_money;

	SLONG inst_x,inst_y,inst_xl,inst_yl;
	RECT test_rect;
	
	SLONG i;
	RECT rect[2]={
		{ 247,  9,247+28, 9+28 },		// close 
		{ 192,261,192+63, 261+21 }		// 确定
	};
	SLONG task;

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
	
	static int old_idx=-1;	

	
	window_x=system_image.item_public_body.rect.left;
	window_y=system_image.item_public_body.rect.top;
	window_xl=system_image.item_public_body.rect.right-system_image.item_public_body.rect.left;
	window_yl=(system_image.item_public_body.rect.bottom-system_image.item_public_body.rect.top)+
		(system_image.store_body.rect.bottom-system_image.store_body.rect.top);

	
	
	offset_x=system_control.mouse_x-window_x;
	offset_y=system_control.mouse_y-window_y;

	task=-1;
	for(i=0;i<2;i++)
	{
		if((offset_x>rect[i].left)&&(offset_x<=rect[i].right)&&
			(offset_y>rect[i].top)&&(offset_y<=rect[i].bottom))
			task=i;
	}

	if(character_popshop_data.active)
	{
		if(task<0)
		{
			for(i=0;i<MAX_POCKET_ITEM;i++)
			{
				if((offset_x>=item_pp[i].left)&&(offset_x<item_pp[i].right)&&
					(offset_y>=item_pp[i].top)&&(offset_y<item_pp[i].bottom))
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
	
	
	if(character_popshop_data.active)
	{
		if((task>=100)&&(task<200))
		{
			detail_item_inst.idx=task-100;
			if(( character_popshop_data.item[detail_item_inst.idx].number>0)&&(character_popshop_data.item[detail_item_inst.idx].filename_id>=0))
			{
				if(detail_item_inst.old_idx!=detail_item_inst.idx)
				{
					if(system_control.system_timer>detail_item_inst.timer+DETAIL_DELAY_TIME)
					{
						detail_item_inst.old_idx=detail_item_inst.idx;
						detail_item_inst.active=false;
						detail_item_inst.type=ITEM_INST_POPSHOP;
						request_detail_inst();
						detail_item_inst.timer=system_control.system_timer;
					}
				}
			}
		}
	}
	
	
	
	// --- ( 0 - 0 ) Draw body 
	display_img256_buffer(window_x,window_y,system_image.item_public_body.image,screen_buffer);	
	display_img256_buffer(window_x+2,window_y+(system_image.item_public_body.rect.bottom-system_image.item_public_body.rect.top),system_image.store_body.image,screen_buffer);	
	display_img256_buffer(window_x+105,window_y+14,system_image.popshop_pop_mark.image,screen_buffer);	
	
	
	// --- ( 0 - 1 ) display data 
	if(character_popshop_data.active)
	{
		for(i=0;i<MAX_POCKET_ITEM;i++)
		{
			if((character_popshop_data.item[i].item_id>=0)&&(character_popshop_data.item[i].filename_id>=0))
			{
				display_img256_buffer(window_x+item_pp[i].left,window_y+item_pp[i].top,
					system_image.item_image[character_popshop_data.item[i].filename_id],screen_buffer);

				if(character_popshop_data.item[i].number>1)
				{
					sprintf((char *)print_rec,"%d",character_popshop_data.item[i].number);
					print12(window_x+item_pp[i].right-(strlen((char *)print_rec)*6)-6-1,window_y+item_pp[i].bottom-10-1,print_rec,COPY_PUT,screen_buffer);
					
				}
				
			}
			
			if(i==character_popshop_data.idx)
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
	
	
	total_money=0;
	// ---------
	if(character_popshop_data.idx>=0)
	{
		if((character_popshop_data.item[character_pocket_data.idx].item_id>=0)&&(character_popshop_data.item[character_pocket_data.idx].filename_id>=0))		// 单价
		{
// 当铺不用钱
//			sprintf((char *)print_rec,"%d",character_popshop_data.item[character_popshop_data.idx].single_price);
//			sprintf((char *)print_rec,"%d",0);
			strcpy((char *)print_rec,"0");
			print16(window_x+71,window_y+228-2,print_rec,PEST_PUT,screen_buffer);			// 单价

// 当铺 不用钱			
//			total_money=character_pocket_data.sell_number*character_pocket_data.item[character_pocket_data.idx].single_price;
//			sprintf((char *)print_rec,"%d",total_money);								// 总价
			strcpy((char *)print_rec,"0");
			print16(window_x+71,window_y+282-2,print_rec,PEST_PUT,screen_buffer);
			
		}
	}
	
	
	// --- 改成 edit
	sprintf((char *)print_rec,"%d",character_popshop_data.sell_number);							// 数量
	print16(window_x+71,window_y+255-2,print_rec,PEST_PUT,screen_buffer);
	
	
	sprintf((char *)print_rec,"%d",character_bank_data.now_money);					// 现金
	print16(window_x+71,window_y+309-2,print_rec,PEST_PUT,screen_buffer);
	
	
	// --- ( 1 - 0 ) Exec & Control
	if((system_control.mouse_x>=window_x)&&(system_control.mouse_x<window_x+window_xl)&&
		(system_control.mouse_y>=window_y)&&(system_control.mouse_y<window_y+window_yl))
	{

		// ------ display inst
		if(character_popshop_data.active)
		{
			
			if((detail_item_inst.active)&&(detail_item_inst.idx>=0))
			{
				if((character_popshop_data.item[detail_item_inst.idx].number>0)&&(character_popshop_data.item[detail_item_inst.idx].filename_id>=0))
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
						system_image.item_image[character_popshop_data.item[detail_item_inst.idx].filename_id],screen_buffer,200);
					
					text_out_data.g_pChat->updateString(inst_x+16+64,inst_y+16,test_rect.right,inst_yl,detail_item_inst.inst);
					text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,screen_buffer,false);
				}
				
				
			}
			
		}
		
		
		


		switch(system_control.mouse_key)
		{
		case MS_RUp:				// close
			if(task<0)
			{
				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
				main_loop_active_task=NULL;		
				break;
			}
			switch(task)
			{
			case 100: case 101: case 102: case 103: case 104: case 105:
			case 106: case 107: case 108: case 109: case 110: case 111:
			case 112: case 113: case 114: case 115: case 116: case 117:
			case 118: case 119: case 120: case 121: case 122: case 123:
				character_popshop_data.idx=task-100;
				if((character_popshop_data.idx<0)||(character_popshop_data.idx>=24))break;
				if(character_popshop_data.item[character_popshop_data.idx].item_id<0)
				{
					character_popshop_data.sell_number=0;
					break;
				}
				if(old_idx!=character_popshop_data.idx)
				{
					character_popshop_data.sell_number=1;
					old_idx=character_popshop_data.idx;
				}
				else
				{
					character_popshop_data.sell_number=character_popshop_data.item[character_popshop_data.idx].number;
				}
				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
				break;				
				
			}
			break;
		case MS_Move:
		case MS_Dummy:
			if(task<0)break;
			switch(task)
			{
			case 0:		// Close
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand1",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
			case 1:		// ok
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand27",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
			}			
			break;

		case MS_LUp:
			if(task<0)break;
			switch(task)
			{
			case 0:		// cancel
				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
				main_loop_active_task=NULL;		
				break;
			case 1:		// Ok
				if((character_popshop_data.idx<0)||(character_popshop_data.idx>=24))break;
				if(character_popshop_data.item[character_popshop_data.idx].item_id<0)break;
				auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
				set_pop_item_index(character_popshop_data.idx,character_popshop_data.sell_number);
				character_popshop_data.sell_number=1;
				character_popshop_data.idx=-1;
				old_idx=-1;
				break;
			case 100: case 101: case 102: case 103: case 104: case 105:
			case 106: case 107: case 108: case 109: case 110: case 111:
			case 112: case 113: case 114: case 115: case 116: case 117:
			case 118: case 119: case 120: case 121: case 122: case 123:
				character_popshop_data.idx=task-100;
				if((character_popshop_data.idx<0)||(character_popshop_data.idx>=24))break;
				if(character_popshop_data.item[character_popshop_data.idx].item_id<0)
				{
					character_popshop_data.sell_number=0;
					break;
				}
				if(old_idx!=character_popshop_data.idx)
				{
					character_popshop_data.sell_number=1;
					old_idx=character_popshop_data.idx;
				}
				else
				{
					if(character_popshop_data.sell_number<character_popshop_data.item[character_popshop_data.idx].number)
						character_popshop_data.sell_number++;
				}
				
				auto_play_voice(0,(UCHR *)"EFC\\button01.wav",NULL,0,0,0,0,&channel);
				break;
			}			
			break;
		case MS_LDrag:
		case MS_LDn:
			if(task<0)break;
			switch(task)
			{
			case 0:		// close
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand2",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
			case 1:		// OK
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand28",window_x+rect[task].left,window_y+rect[task].top,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
				break;
			}			
			break;

		}
	}
	

	// --- check is 队员
	if((system_control.control_npc_idx>=0)&&(system_control.control_npc_idx<total_map_npc))
	{
		if(map_npc_group[system_control.control_npc_idx].npc_info.status & NPC_STATUS_FOLLOW)
		{
			auto_play_voice(0,(UCHR *)"EFC\\button03.wav",NULL,0,0,0,0,&channel);
			main_loop_active_task=NULL;		
		}
	}
	
	
	// ---- clear all input 
	system_control.mouse_key=MS_Dummy;
	system_control.key=NULL;
	system_control.data_key=NULL;
	reset_mouse_key();
	reset_key();
	
}










