/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : data_proc.cpp
  *owner  		: Stephen
  *description  : 
  *modified     : 2005/2/26
******************************************************************************/ 

#include "engine_global.h"
#include "data_proc.h"
#include "vari-ext.h"
#include "utility.h"
#include "fight.h"
#include "graph.h"


void display_character_quick_status(BMP *buffer)
{
	SLONG hp,max_hp;
	SLONG hp_percent;
	SLONG mp,max_mp;
	SLONG mp_percent;
	
	SLONG img_xl;

	if(!base_character_data.active)return;

	hp=base_character_data.data.final.now_hp;
	max_hp=base_character_data.data.final.max_hp;

	mp=base_character_data.data.final.now_mp;
	max_mp=base_character_data.data.final.max_mp;

	if(max_hp<1)max_hp=1;
	if(hp<0)hp=0;
	if(hp>max_hp)hp=max_hp;
	hp_percent=hp*100/max_hp;

	if(max_mp<1)max_mp=1;
	if(mp<0)mp=0;
	if(mp>max_mp)mp=max_mp;
	mp_percent=mp*100/max_mp;
	

	img_xl=system_image.char_hp.rect.right-system_image.char_hp.rect.left;
	img_xl=img_xl*hp_percent/100;
	if(img_xl>0)
	{
		display_part_img256_buffer(system_image.char_hp.rect.left,system_image.char_hp.rect.top,img_xl,	system_image.char_hp.image,buffer);
	}

	img_xl=system_image.char_mp.rect.right-system_image.char_mp.rect.left;
	img_xl=img_xl*mp_percent/100;
	if(img_xl>0)
	{
		display_part_img256_buffer(system_image.char_mp.rect.left,system_image.char_mp.rect.top,img_xl,	system_image.char_mp.image,buffer);
	}
	

}


void display_fight_character_quick_status(BMP *buffer)
{
	SLONG hp,max_hp;
	SLONG hp_percent;
	SLONG mp,max_mp;
	SLONG mp_percent;
	SLONG percent;
	
	SLONG img_xl;

	if(fight_control_data.character_idx<0)
		return;
	if(!fight_npc_group[fight_control_data.character_idx].base.active)
		return;
	
	hp=fight_npc_group[fight_control_data.character_idx].base.now_hp;
	max_hp=fight_npc_group[fight_control_data.character_idx].base.max_hp;


	mp=fight_npc_group[fight_control_data.character_idx].base.now_mp;
	max_mp=fight_npc_group[fight_control_data.character_idx].base.max_mp;

	
	if(max_hp<1)max_hp=1;
	if(hp<0)hp=0;
	if(hp>max_hp)hp=max_hp;
	hp_percent=hp*100/max_hp;
	
	if(max_mp<1)max_mp=1;
	if(mp<0)mp=0;
	if(mp>max_mp)mp=max_mp;
	mp_percent=mp*100/max_mp;
	
	
	img_xl=fight_image.hp_body.rect.right-fight_image.hp_body.rect.left;
	img_xl=img_xl*hp_percent/100;
	if(img_xl>0)
	{
		display_part_img256_buffer(fight_image.menu_body.rect.left+108,fight_image.menu_body.rect.top+36,img_xl,fight_image.hp_body.image,buffer);
	}
	
	img_xl=fight_image.mp_body.rect.right-fight_image.mp_body.rect.left;
	img_xl=img_xl*mp_percent/100;
	if(img_xl>0)
	{
		display_part_img256_buffer(fight_image.menu_body.rect.left+108,fight_image.menu_body.rect.top+64,img_xl,fight_image.mp_body.image,buffer);
	}

	
	sprintf((char *)print_rec,"%d/%d",hp,max_hp);
	print16(fight_image.menu_body.rect.left+104,fight_image.menu_body.rect.top+18,print_rec,PEST_PUT,buffer);

	sprintf((char *)print_rec,"%d/%d",mp,max_mp);
	print16(fight_image.menu_body.rect.left+104,fight_image.menu_body.rect.top+46,print_rec,PEST_PUT,buffer);


	percent=fight_control_data.character_speed_percent;
	if(percent>0)
	{
		if(percent>98)
		{
			display_img256_buffer(fight_image.menu_body.rect.left+110,fight_image.menu_body.rect.top+90,fight_image.speed_body.image,buffer);
		}
		else
		{
			img_xl=fight_image.speed_body.rect.right-fight_image.speed_body.rect.left;
			img_xl=img_xl*percent/100;
			if(img_xl>0)
			{
				display_part_img256_buffer(fight_image.menu_body.rect.left+110,fight_image.menu_body.rect.top+90,img_xl,fight_image.speed_body.image,buffer);
			}
		}
		
	}
	


	
}



void display_character_quick_exp(BMP *buffer)
{
	SLONG exp,max_exp;
	SLONG exp_percent;
	SLONG exp_offset;
	SLONG i;
	SLONG img_xl;
	POSITION pp[10]={
		{ 87+64*0,590 },	// 0
		{ 87+64*1,590 },	// 1
		{ 87+64*2,590 },	// 2
		{ 87+64*3,590 },	// 3
		{ 87+64*4,590 },	// 4
		{ 87+64*5,590 },	// 5
		{ 87+64*6,590 },	// 6
		{ 87+64*7,590 },	// 7
		{ 87+64*8,590 },	// 8
		{ 87+64*9,590 }		// 9
		
	};

	if(!base_character_data.active)return;
	
	exp=base_character_data.data.final.now_exp;
	max_exp=base_character_data.data.final.need_exp+exp;

	if(max_exp<1)max_exp=1;
	if(exp<0)
	{
		exp_percent=0;			// 负经验处理
	}
	else
	{
		if(exp>max_exp)exp=max_exp;
		exp_percent=exp*100/max_exp;
	}

	if(exp_percent<0)exp_percent=0;
	if(exp_percent>100)exp_percent=100;
	
	exp_offset=exp_percent/10;
	for(i=0;i<exp_offset;i++)
	{
		display_img256_buffer(pp[i].x,pp[i].y,system_image.char_exp.image,buffer);
	}


	exp_percent=exp_percent%10;
	if(exp_percent>0)
	{
		img_xl=system_image.char_exp.rect.right-system_image.char_exp.rect.left;
		img_xl=img_xl*exp_percent/10;
		if(img_xl>0)
		{
			display_part_img256_buffer(pp[exp_offset].x,pp[exp_offset].y,img_xl,system_image.char_exp.image,buffer);
		}
	}

	
	
}


void display_baobao_quick_status(BMP *buffer)
{
	SLONG idx;
	SLONG hp,max_hp;
	SLONG hp_percent;
	SLONG mp,max_mp;
	SLONG mp_percent;
	SLONG exp,max_exp;
	SLONG exp_percent;
	
	SLONG img_xl;
	

	if(!base_baobao_data.active)
		return;
	if(base_baobao_data.data.action_baobao_id==0)
		return;
	idx=get_baobao_data_idx(base_baobao_data.data.action_baobao_id);
	if(idx<0)return;

	hp=base_baobao_data.data.baobao_list[idx].data.now_hp;
	max_hp=base_baobao_data.data.baobao_list[idx].data.max_hp;
	
	mp=base_baobao_data.data.baobao_list[idx].data.now_mp;
	max_mp=base_baobao_data.data.baobao_list[idx].data.max_mp;
	
	exp=base_baobao_data.data.baobao_list[idx].data.now_exp;
	max_exp=base_baobao_data.data.baobao_list[idx].data.need_exp+exp;

	
	if(max_hp<1)max_hp=1;
	if(hp<0)hp=0;
	if(hp>max_hp)hp=max_hp;
	hp_percent=hp*100/max_hp;

	if(max_mp<1)max_mp=1;
	if(mp<0)mp=0;
	if(mp>max_mp)mp=max_mp;
	mp_percent=mp*100/max_mp;

	if(max_exp<1)max_exp=1;
	if(exp<0)
	{
		exp_percent=0;			// 负经验处理
	}
	else
	{
		if(exp>max_exp)exp=max_exp;
		exp_percent=exp*100/max_exp;
	}
	

	img_xl=system_image.baobao_hp.rect.right-system_image.baobao_hp.rect.left;
	img_xl=img_xl*hp_percent/100;
	if(img_xl>0)
	{
		display_part_img256_buffer(system_image.baobao_hp.rect.left,system_image.baobao_hp.rect.top,img_xl,	system_image.baobao_hp.image,buffer);
	}
	
	img_xl=system_image.baobao_mp.rect.right-system_image.baobao_mp.rect.left;
	img_xl=img_xl*mp_percent/100;
	if(img_xl>0)
	{
		display_part_img256_buffer(system_image.baobao_mp.rect.left,system_image.baobao_mp.rect.top,img_xl,	system_image.baobao_mp.image,buffer);
	}
	
	img_xl=system_image.baobao_exp.rect.right-system_image.baobao_exp.rect.left;
	img_xl=img_xl*exp_percent/100;
	if(img_xl>0)
	{
		display_part_img256_buffer(system_image.baobao_exp.rect.left,system_image.baobao_exp.rect.top,img_xl,	system_image.baobao_exp.image,buffer);
	}
	

}




void display_fight_baobao_quick_status(BMP *buffer)
{
	SLONG idx;
	SLONG hp,max_hp;
	SLONG hp_percent;
	SLONG mp,max_mp;
	SLONG mp_percent;
	SLONG percent;
	ULONG baobao_id;
	
	SLONG img_xl;
	

	if(fight_control_data.baobao_idx<0)return;
	if(!fight_npc_group[fight_control_data.baobao_idx].base.active)return;
	baobao_id=fight_npc_group[fight_control_data.baobao_idx].base.id;			// 宝宝 id

	idx=get_baobao_data_idx(baobao_id);
	if(idx<0)return;
	
	
	hp=fight_npc_group[fight_control_data.baobao_idx].base.now_hp;
	max_hp=fight_npc_group[fight_control_data.baobao_idx].base.max_hp;


	
	mp=fight_npc_group[fight_control_data.baobao_idx].base.now_mp;
	max_mp=fight_npc_group[fight_control_data.baobao_idx].base.max_mp;
	
	
	
	if(max_hp<1)max_hp=1;
	if(hp<0)hp=0;
	if(hp>max_hp)hp=max_hp;
	hp_percent=hp*100/max_hp;
	
	if(max_mp<1)max_mp=1;
	if(mp<0)mp=0;
	if(mp>max_mp)mp=max_mp;
	mp_percent=mp*100/max_mp;
	
	img_xl=fight_image.hp_body.rect.right-fight_image.hp_body.rect.left;
	img_xl=img_xl*hp_percent/100;
	if(img_xl>0)
	{
		display_part_img256_buffer(fight_image.menu_body.rect.left+503,fight_image.menu_body.rect.top+36,img_xl,fight_image.hp_body.image,buffer);
	}
	
	img_xl=fight_image.mp_body.rect.right-fight_image.mp_body.rect.left;
	img_xl=img_xl*mp_percent/100;
	if(img_xl>0)
	{
		display_part_img256_buffer(fight_image.menu_body.rect.left+503,fight_image.menu_body.rect.top+64,img_xl,fight_image.mp_body.image,buffer);
	}
	

	sprintf((char *)print_rec,"%d/%d",hp,max_hp);
	print16(fight_image.menu_body.rect.left+498,fight_image.menu_body.rect.top+18,print_rec,PEST_PUT,buffer);
	
	sprintf((char *)print_rec,"%d/%d",mp,max_mp);
	print16(fight_image.menu_body.rect.left+498,fight_image.menu_body.rect.top+46,print_rec,PEST_PUT,buffer);
	

	percent=fight_control_data.baobao_speed_percent;
	if(percent>0)
	{
		img_xl=fight_image.speed_body.rect.right-fight_image.speed_body.rect.left;
		img_xl=img_xl*percent/100;
		if(img_xl>0)
		{
			display_part_img256_buffer(fight_image.menu_body.rect.left+505,fight_image.menu_body.rect.top+90,img_xl,fight_image.speed_body.image,buffer);
		}
		
	}
	
	
	
}


void get_near_position(SLONG sx,SLONG sy,SLONG range,SLONG dir,SLONG *tx,SLONG *ty)
{
	SLONG ret_x,ret_y;	
	SLONG temp_x,temp_y;

	ret_x=sx;
	ret_y=sy;

	switch(dir)
	{
	case NPC_DIR_UP:					//	0
		ret_y=sy+range;
		break;
	case NPC_DIR_DOWN:					//  1
		ret_y=sy-range;
		break;
	case NPC_DIR_LEFT:					//	2
		ret_x=sx+range;
		break;
	case NPC_DIR_RIGHT:					//  3
		ret_x=sx-range;
		break;
	case NPC_DIR_UP_RIGHT:				//  4
		ret_y=sy+range;
		ret_x=sx-range;
		break;
	case NPC_DIR_DOWN_LEFT:				//  5
		ret_y=sy-range;
		ret_x=sx+range;
		break;
	case NPC_DIR_UP_LEFT:				// 	6
		ret_y=sy+range;
		ret_x=sx+range;
		break;
	case NPC_DIR_DOWN_RIGHT:			//	7
		ret_y=sy-range;
		ret_x=sx-range;
		break;
	default:
		break;
	}

	if(ret_x<0)ret_x=0;
	if(ret_y<0)ret_y=0;
	if(ret_x>=map_base_data.map_width-16)
		ret_x=map_base_data.map_width-16;
	if(ret_y>=map_base_data.map_height-8)
		ret_y=map_base_data.map_height-8;

	if(check_cell_stop(ret_x/map_base_data.cell_width,ret_y/map_base_data.cell_height)==TTN_OK) // 在阻挡点上面#3
	{
		if(get_random_near_offset_position(ret_x,ret_y,&temp_x,&temp_y)==TTN_OK)
		{
			*tx=temp_x;
			*ty=temp_y;
		}
		else
		{
			*tx=sx;
			*ty=sy;
		}
			
	}
	else
	{
		*tx=ret_x;
		*ty=ret_y;
	}

}



SLONG get_map_npc_follow_conut(ULONG follow_id)		// 计算目前有多少人跟随这个id
{
	SLONG i;
	SLONG ret_val;

	ret_val=0;
	for(i=0;i<total_map_npc;i++)
	{
		if(map_npc_group[i].follow_id==follow_id)
			ret_val++;
	}

	return(ret_val);
	
}




SLONG get_random_near_offset_position(SLONG sx,SLONG sy,SLONG *tx,SLONG *ty)
{
	SLONG i,j,temp;
	SLONG search_dir[MAX_NPC_DIR];
	SLONG new_x,new_y;
	SLONG search_loop;
	SLONG range=50;
	SLONG offset;
	SLONG x,y;
	POSITION new_pp[32+10];            // 8 dir * 4 + 10 loop
	
	
	for(search_loop=0;search_loop<4;search_loop++)
	{
		for(i=0;i<MAX_NPC_DIR;i++)
		{
			search_dir[i]=i;
		}
		for(i=0;i<MAX_NPC_DIR;i++)
		{
			j=rand()%MAX_NPC_DIR;
			temp=search_dir[i];               // change search position i & j
			search_dir[i]=search_dir[j];
			search_dir[j]=temp;
		}
		
		
		for(i=0;i<MAX_NPC_DIR;i++)
		{
			new_x=sx;
			new_y=sy;
			
			switch(i)
			{
			case NPC_DIR_UP:					//	0
				new_y=sy+range*(search_loop+1);
				break;
			case NPC_DIR_DOWN:					//  1
				new_y=sy-range*(search_loop+1);
				break;
			case NPC_DIR_LEFT:					//	2
				new_x=sx+range*(search_loop+1);
				break;
			case NPC_DIR_RIGHT:					//  3
				new_x=sx-range*(search_loop+1);
				break;
			case NPC_DIR_UP_RIGHT:				//  4
				new_y=sy+range*(search_loop+1);
				new_x=sx-range*(search_loop+1);
				break;
			case NPC_DIR_DOWN_LEFT:				//  5
				new_y=sy-range*(search_loop+1);
				new_x=sx+range*(search_loop+1);
				break;
			case NPC_DIR_UP_LEFT:				// 	6
				new_y=sy+range*(search_loop+1);
				new_x=sx+range*(search_loop+1);
				break;
			case NPC_DIR_DOWN_RIGHT:			//	7
				new_y=sy-range*(search_loop+1);
				new_x=sx-range*(search_loop+1);
				break;
			default:
				break;
			}
			offset=search_loop*8+search_dir[i];
			new_pp[offset].x=new_x;
			new_pp[offset].y=new_y;
			
		}
		
	}

	
	for(i=0;i<32;i++)
	{
		x=new_pp[i].x;
		y=new_pp[i].y;
		if((x<0)||(y<0)||
			(x>=map_base_data.map_width-16)||
			(y>=map_base_data.map_height-8))
			continue;
		if(check_cell_stop(x/map_base_data.cell_width,y/map_base_data.cell_height)==TTN_OK)
			continue;
		*tx=x;
		*ty=y;
		return(TTN_OK);              // OK get it !
	}
	*tx=sx;
	*ty=sy;
	return(TTN_NOT_OK);

}




void check_screen_zoom(void)
{
	UCHR ch;
	SLONG xl,yl;

	ch=read_mouse_key();
	switch(ch)
	{
	case MS_Forward:
		reset_mouse_key();
		if(game_control.map_zoom_index<=0)break;
		game_control.map_zoom_index--;
		break;
	case MS_Backward:
		reset_mouse_key();
		if(game_control.map_zoom_index>=14)break;
		game_control.map_zoom_index++;
		break;
	}

	if(game_control.map_zoom_index==0)return;


	xl=8*game_control.map_zoom_index;
	yl=6*game_control.map_zoom_index;

	rectangle_scale_put_bmp(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,screen_channel2,
							 xl,yl,800-xl*2,600-yl*2,screen_buffer);
	get_bitmap(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,screen_buffer,screen_channel2);

}



void auto_close_map_npc_handle(SLONG handle)
{
	SLONG i;

// --- ( 0 - 0 ) Check Map npc 有没有用到
	for(i=0;i<total_map_npc;i++)
	{
		if(map_npc_group[i].handle==handle)
			return;
	}

	mda_group_close(&handle);
}


void close_fight_npc_group(SLONG idx)
{
	SLONG i;
	SLONG now_handle;
	SLONG check_count;
	
	// --- ( 0 - 0 ) Check Map npc 有没有用到
	if(idx<0)return;
	check_count=0;
	now_handle=fight_npc_group[idx].handle;
	for(i=0;i<MAX_FIGHT_UNIT;i++)
	{
		if(i==idx)continue;					// 同一个人不用check
		if(fight_npc_group[i].handle==now_handle)
			check_count++;
	}
	
	fight_npc_group[idx].base.active=false;
	fight_npc_group[idx].display=false;
	fight_npc_group[idx].handle=-1;

	if(check_count>0)return;
	mda_group_close(&now_handle);

}


void get_money_string(SLONG value,UCHR *text)
{
	SLONG number0,number1,number2,number3;

	number0=value%1000;
	number1=(value%1000000)/1000;
	number2=(value%1000000000)/1000000;
	number3=(value%1000000000000)/1000000000;

	if(number3>0)
	{
		sprintf((char *)text,"%d,%03d,%03d,%03d",number3,number2,number1,number0);

	} else if(number2 >0)
	{
		sprintf((char *)text,"%d,%03d,%03d",number2,number1,number0);

	} else if(number1 > 0)
	{
		sprintf((char *)text,"%d,%03d",number1,number0);
		
	} else
	{
		sprintf((char *)text,"%d",number0);
	}


}



void set_give_item_index(void)
{
	SLONG idx;
	SLONG i;

// 寻找同样的内容

	if(character_pocket_data.idx<0)return;

	for(i=0;i<3;i++)
	{
		if(character_pocket_data.give_idx[i]==character_pocket_data.idx) 	// 一样的内容
		{

			character_pocket_data.give_number[i]++;
			if(character_pocket_data.give_number[i]>=character_pocket_data.item[character_pocket_data.idx].number)
				character_pocket_data.give_number[i]=character_pocket_data.item[character_pocket_data.idx].number;
			return;
		}
	}


	idx=-1;
	for(i=0;i<3;i++)
	{
		if(character_pocket_data.give_idx[i]<0)
		{
			idx=i;
			break;
		}
	}

	if(idx<0)return;			// 没有位置
		
	character_pocket_data.give_idx[idx]=character_pocket_data.idx;
	character_pocket_data.give_number[idx]=1;



}


void set_give_item_index_max(void)
{
	SLONG idx;
	SLONG i;
	
	// 寻找同样的内容
	
	if(character_pocket_data.idx<0)return;
	
	for(i=0;i<3;i++)
	{
		if(character_pocket_data.give_idx[i]==character_pocket_data.idx)	// 一样的内容
		{
			character_pocket_data.give_number[i]=character_pocket_data.item[character_pocket_data.idx].number;
			return;
		}
	}
	
	
	idx=-1;
	for(i=0;i<3;i++)
	{
		if(character_pocket_data.give_idx[i]<0)
		{
			idx=i;
			break;
		}
	}
	
	if(idx<0)return;			// 没有位置
	
	character_pocket_data.give_idx[idx]=character_pocket_data.idx;
	character_pocket_data.give_number[idx]=character_pocket_data.item[character_pocket_data.idx].number;
	
	
	
}




void set_give_item_index_del(SLONG idx,SLONG number)
{
	if((idx<0)||(idx>=3))return;
	if(character_pocket_data.give_idx[idx]<0)return;		// 空的

	character_pocket_data.give_number[idx]-=number;

	if(character_pocket_data.give_number[idx]<1)
	{
		character_pocket_data.give_idx[idx]=-1;
		character_pocket_data.give_number[idx]=0;
	}


}





void set_deal_item_index(void)
{
	SLONG idx;
	SLONG source_idx;
	SLONG i;

// 寻找同样的内容

	if(character_pocket_data.idx<0)return;

	source_idx=character_pocket_data.idx;
	if(game_control.deal_type==1)
		source_idx=source_idx+100;

	for(i=0;i<3;i++)
	{
		if(character_pocket_data.my_deal_idx[i]==source_idx) 	// 一样的内容
		{
			character_pocket_data.my_deal_number[i]++;
			if(character_pocket_data.my_deal_number[i]>=character_pocket_data.item[character_pocket_data.idx].number)
				character_pocket_data.my_deal_number[i]=character_pocket_data.item[character_pocket_data.idx].number;
			return;
		}
	}


	idx=-1;
	for(i=0;i<3;i++)
	{
		if(character_pocket_data.my_deal_idx[i]<0)
		{
			idx=i;
			break;
		}
	}

	if(idx<0)return;			// 没有位置
		
	character_pocket_data.my_deal_idx[idx]=source_idx;
	character_pocket_data.my_deal_number[idx]=1;



}


void set_deal_item_index_max(void)
{
	SLONG idx;
	SLONG i;
	
	// 寻找同样的内容
	
	if(character_pocket_data.idx<0)return;
	
	for(i=0;i<3;i++)
	{
		if(character_pocket_data.my_deal_idx[i]==character_pocket_data.idx)	// 一样的内容
		{
			character_pocket_data.my_deal_number[i]=character_pocket_data.item[character_pocket_data.idx].number;
			return;
		}
	}
	
	
	idx=-1;
	for(i=0;i<3;i++)
	{
		if(character_pocket_data.give_idx[i]<0)
		{
			idx=i;
			break;
		}
	}
	
	if(idx<0)return;			// 没有位置
	
	character_pocket_data.my_deal_idx[idx]=character_pocket_data.idx;
	character_pocket_data.my_deal_number[idx]=character_pocket_data.item[character_pocket_data.idx].number;
	
	
	
}




void set_deal_item_index_del(SLONG idx,SLONG number)
{
	if((idx<0)||(idx>=3))return;
	if(character_pocket_data.my_deal_idx[idx]<0)return;		// 空的

	character_pocket_data.my_deal_number[idx]-=number;

	if(character_pocket_data.my_deal_number[idx]<1)
	{
		character_pocket_data.my_deal_idx[idx]=-1;
		character_pocket_data.my_deal_number[idx]=0;
	}


}




void clear_detail_item_inst(void)
{
	detail_item_inst.active=false;
	detail_item_inst.idx=-1;
	detail_item_inst.old_idx=-1;
	detail_item_inst.timer=system_control.system_timer;
	
}


void make_detail_inst(void)
{
	SERVER_CHARACTER_BAOBAO data;
	SLONG skill_id;
	SLONG idx;


	switch(detail_item_inst.type)
	{
	case ITEM_INST_MY_BAOBAO:
		if((detail_item_inst.idx<0)||(detail_item_inst.idx>=MAX_BAOBAO_STACK))
			return;
		data=base_baobao_data.data.baobao_list[detail_item_inst.idx];
		if(data.baobao_id==0)return;
		
		sprintf((char *)print_rec,MSG_BAOBAO_DETAIL_INST,data.name,data.data.level,
			data.data.now_hp,data.data.max_hp,
			data.data.now_mp,data.data.max_mp,
			data.data.att,
			data.data.speed,
			data.base.life_base,
			data.base.grow);
		detail_item_inst.active=true;
		strcpy(detail_item_inst.inst,(char *)print_rec);
		break;
	case ITEM_INST_SKILL:
		if((detail_item_inst.idx<0)||(detail_item_inst.idx>=MAX_CHARACTER_SKILL_NO))
			return;
		skill_id=character_skill_data[detail_item_inst.idx].idx;
		if(skill_id<0)return;
		
		sprintf((char *)print_rec,MSG_SKILL_DETAIL_INST,skill_base_data[skill_id].name,skill_base_data[skill_id].inst);
		detail_item_inst.active=true;
		strcpy(detail_item_inst.inst,(char *)print_rec);
		break;
	case ITEM_INST_TITLE:
		if((detail_item_inst.idx<0)||(detail_item_inst.idx>=MAX_TITLE))
			return;
		if(base_title_data.data[detail_item_inst.idx].type==0)return;
		idx=get_title_index(base_title_data.data[detail_item_inst.idx].type);
		if(idx<0)return;
		detail_item_inst.active=true;
		strcpy(detail_item_inst.inst,(char *)base_title_data.data[idx].inst);
		break;
	}
	

	
}


void clear_fight_command(void)
{
	character_fight_command_data.command=-1;
	character_fight_command_data.source_id=0;
	character_fight_command_data.target_id=0;
	character_fight_command_data.type_no=-1;

	baobao_fight_command_data.command=-1;
	baobao_fight_command_data.source_id=0;
	baobao_fight_command_data.target_id=0;
	baobao_fight_command_data.type_no=-1;


	backup_character_fight_command_data.command=-1;
	backup_character_fight_command_data.source_id=0;
	backup_character_fight_command_data.target_id=0;
	backup_character_fight_command_data.type_no=-1;
	
	backup_baobao_fight_command_data.command=-1;
	backup_baobao_fight_command_data.source_id=0;
	backup_baobao_fight_command_data.target_id=0;
	backup_baobao_fight_command_data.type_no=-1;
	

}



void clear_fight_control(void)
{
	SLONG i;
	ULONG user_id;


	user_id=system_control.control_user_id;
	fight_control_data.character_idx=get_fight_npc_index(user_id);
	if(fight_control_data.character_idx>=0)
		fight_control_data.baobao_idx=fight_control_data.character_idx+1;
	else
		fight_control_data.baobao_idx=-1;
	
	fight_control_data.charge_flag=false;				// 是否充电
	
	fight_control_data.character_command_ready=false;	// 玩家命令 ready
	fight_control_data.baobao_command_ready=false;		// 宝宝命令 ready
	
	fight_control_data.character_speed_percent=0;		// 玩家充电数值 ( 0 到 100 )
	fight_control_data.baobao_speed_percent=0;			// 玩家充电数值 ( 0 到 100 )
	
	
	fight_control_data.fight_action_ready=false;				// 战斗动作 ready
	fight_control_data.fight_action_command=FIGHT_ACTION_SINGLE_PHYSICS;			// 战斗动作种类



	// --- Camera 参数
	fight_control_data.camera_follow_flag=false;				// 镜头自动跟随 flag
	fight_control_data.camera_follow_x=FIGHT_MAP_WIDTH/2;		// 镜头自动跟随中心点坐标
	fight_control_data.camera_follow_y=FIGHT_MAP_HEIGHT/2;
	fight_control_data.camera_follow_zoom=MAX_CAMERA_ZOOM/2;	// 镜头自动跟随 zoom idx
	

// --- fight display text
	for(i=0;i<MAX_FIGHT_UNIT;i++)
	{
		fight_screen_text_data[i].active=false;
		fight_screen_text_data[i].scroll=true;
		fight_screen_text_data[i].type=0;
		fight_screen_text_data[i].x=0;
		fight_screen_text_data[i].y=0;
		fight_screen_text_data[i].still_timer=0;
		fight_screen_text_data[i].text[0]=NULL;
	}


// --- Other 
	fight_control_data.effect0_extra_mda_idx=-1;		// 地裂 MDA 效果
	fight_control_data.see_fight_extra_mda_idx=-1;
	fight_control_data.danger_flag=false;				// team danger flag
	fight_control_data.danger_status=0;
	fight_control_data.long_war_flag=false;
	fight_control_data.long_war_status=0;
	fight_control_data.fight_loop=0;					// 战斗回合
	fight_control_data.player_select=true;
	
}




void charge_time_control(SLONG idx,SLONG code)
{
	ULONG time_offset;


	switch(code)
	{
	case CHARGE_TIME_INITIALIZE:				// 充电开始 test
		switch(idx)
		{
		case FIGHT_COMMAND_CHARACTER:
			character_charge_time_data.charge_start_time=system_control.system_timer;
			fight_control_data.character_pause_time=system_control.system_timer;
			fight_control_data.charge_flag=false;
//			fight_control_data.character_command_send=false;
			break;
		case FIGHT_COMMAND_BAOBAO:
			baobao_charge_time_data.charge_start_time=system_control.system_timer;
			fight_control_data.baobao_pause_time=system_control.system_timer;
			fight_control_data.charge_flag=false;
//			fight_control_data.baobao_command_send=false;
			break;
		}
		break;		
	case CHARGE_TIME_PAUSE:						// 暂停
		if(fight_control_data.charge_flag==false)	// 已经暂停了
			break;
		fight_control_data.charge_flag=false;
		fight_control_data.character_pause_time=system_control.system_timer;
		fight_control_data.baobao_pause_time=system_control.system_timer;
		break;		
	case CHARGE_TIME_CONTINUE:					// 继续
		if(fight_control_data.charge_flag==true)	
			break;
		time_offset=system_control.system_timer-fight_control_data.character_pause_time;
		character_charge_time_data.charge_start_time+=time_offset;

		time_offset=system_control.system_timer-fight_control_data.baobao_pause_time;
		baobao_charge_time_data.charge_start_time+=time_offset;
		fight_control_data.charge_flag=true;
		break;		
	case CHARGE_TIME_RESET:					// 充电重新开始
		switch(idx)
		{
		case FIGHT_COMMAND_CHARACTER:
			character_charge_time_data.charge_start_time=system_control.system_timer;
			fight_control_data.character_pause_time=system_control.system_timer;
//			fight_control_data.character_command_send=false;
			break;
		case FIGHT_COMMAND_BAOBAO:
			baobao_charge_time_data.charge_start_time=system_control.system_timer;
			fight_control_data.baobao_pause_time=system_control.system_timer;
//			fight_control_data.baobao_command_send=false;
			break;
		}
		break;
	}




}
			

void charge_control(void )
{
	ULONG time_offset;
	SLONG percent;


	if(!fight_control_data.charge_flag)			// 暂停充电
		return;

	if(fight_control_data.character_idx<0)return;
	if(!fight_npc_group[fight_control_data.character_idx].base.active)
		return;

// ----- Process character
	time_offset=system_control.system_timer-character_charge_time_data.charge_start_time;

	if(fight_control_data.character_command_ready)		// 命令已经 ready !!!! GO!GO!GO!
	{
		send_character_fight_command();
		check_character_command_change();
	}
		

	if(time_offset>character_charge_time_data.charge_time)
	{
		fight_control_data.character_speed_percent=100;
/*
		if(fight_control_data.character_command_ready)		// 命令已经 ready !!!! GO!GO!GO!
		{
			send_character_fight_command();
		}
		else
		{
			character_charge_time_data.delay_time=time_offset-character_charge_time_data.charge_time; // 开始计算 delay 时间
			if(character_charge_time_data.delay_time>=CHARGE_DELAY_TIME)
			{
				send_character_fight_command();
			}
		}
*/
	}
	else
	{
		percent=time_offset*100/character_charge_time_data.charge_time;
		if(percent>=100)
			percent=100;
		character_charge_time_data.delay_time=0;		
		fight_control_data.character_speed_percent=percent;
	}
	


		
	if(fight_control_data.baobao_idx<0)return;
	if(!fight_npc_group[fight_control_data.baobao_idx].base.active)				// 玩家宝宝
		return;
	

	// ----- Process baobao
	time_offset=system_control.system_timer-baobao_charge_time_data.charge_start_time;

	if(fight_control_data.baobao_command_ready)		// 命令已经 ready !!!! GO!GO!GO!
	{
		send_baobao_fight_command();
		check_baobao_command_change();
	}
	

	if(time_offset>baobao_charge_time_data.charge_time)
	{
		fight_control_data.baobao_speed_percent=100;
/*
		if(fight_control_data.baobao_command_ready)		// 命令已经 ready !!!! GO!GO!GO!
		{
			send_baobao_fight_command();
		}
		else
		{
			baobao_charge_time_data.delay_time=time_offset-baobao_charge_time_data.charge_time;
			if(baobao_charge_time_data.delay_time>=CHARGE_DELAY_TIME)
			{
				send_baobao_fight_command();
			}
		}
*/
	}
	else
	{
		percent=time_offset*100/baobao_charge_time_data.charge_time;
		if(percent>=100)
			percent=100;
		baobao_charge_time_data.delay_time=0;
		fight_control_data.baobao_speed_percent=percent;
	}
	
	


}



void set_fight_main_command(SLONG idx,char command)
{

	if(fight_control_data.character_idx<0)return;
	
	if(character_fight_command_data.command==FIGHT_COMMAND_AUTO_ATTACK)		// 目前在自动攻击
	{

		fight_control_data.character_command_ready=false;

		character_fight_command_data.command=-1;			// 目前无命令
		character_fight_command_data.source_id=fight_npc_group[fight_control_data.character_idx].base.id;
		
		fight_control_data.baobao_command_ready=false;
		
		if(fight_control_data.baobao_idx>=0)
		{
			if(fight_npc_group[fight_control_data.baobao_idx].base.active)
			{
				baobao_fight_command_data.command=-1;
				baobao_fight_command_data.source_id=fight_npc_group[fight_control_data.baobao_idx].base.id;
			}
		}
		fight_control_data.player_select=true;		// 玩家命令

		character_fight_command_data.command=-1;
		baobao_fight_command_data.command=-1;
		if(command==FIGHT_COMMAND_AUTO_ATTACK)		
			return;
		
	}
	


	switch(idx)
	{
	case FIGHT_COMMAND_CHARACTER:
		if(fight_control_data.character_idx<0)break;
		if(!fight_npc_group[fight_control_data.character_idx].base.active)break;	// 目前player 退出战场

		character_fight_command_data.source_id=fight_npc_group[fight_control_data.character_idx].base.id;

		if(command==FIGHT_COMMAND_AUTO_ATTACK)					// 如果是自动攻击
		{
			character_fight_command_data.command=command;
			character_fight_command_data.source_id=fight_npc_group[fight_control_data.character_idx].base.id;
			baobao_fight_command_data.command=command;
			baobao_fight_command_data.source_id=fight_npc_group[fight_control_data.baobao_idx].base.id;

			fight_control_data.character_command_ready=true;
			fight_control_data.baobao_command_ready=true;
			break;
		}
		fight_control_data.character_command_ready=false;
		if(character_fight_command_data.command!=command)		// 不同命令
		{
			character_fight_command_data.command=command;
			character_fight_command_data.source_id=fight_npc_group[fight_control_data.character_idx].base.id;
			character_fight_command_data.target_id=0;
			character_fight_command_data.type_no=-1;
		}
		else													// 相同命令,检查参数是否有用
		{
			character_fight_command_data.command=command;
			character_fight_command_data.source_id=fight_npc_group[fight_control_data.character_idx].base.id;
		}

		close_character_fight_all_window();
		switch(command)					// 自动开启所需要的 window
		{
		case FIGHT_COMMAND_MAGIC:
			exec_fight_menu_task_skill();
			break;
		case FIGHT_COMMAND_ITEM:
			exec_fight_menu_task_character_item();
			break;
		case FIGHT_COMMAND_CHANGE:
			exec_fight_menu_task_baobao_change();
			break;
		}

		fight_control_data.player_select=true;		// 玩家命令
		break;
	case FIGHT_COMMAND_BAOBAO:
		if(fight_control_data.baobao_idx<0)break;
		fight_control_data.baobao_command_ready=false;
		if(!fight_npc_group[fight_control_data.baobao_idx].base.active)break;				// 目前没有宝宝参战
		baobao_fight_command_data.source_id=fight_npc_group[fight_control_data.baobao_idx].base.id;
		if(command==FIGHT_COMMAND_AUTO_ATTACK)					// 如果是自动攻击
		{
			character_fight_command_data.command=command;
			character_fight_command_data.source_id=fight_npc_group[fight_control_data.character_idx].base.id;
			baobao_fight_command_data.command=command;
			baobao_fight_command_data.source_id=fight_npc_group[fight_control_data.baobao_idx].base.id;
			fight_control_data.character_command_ready=true;
			fight_control_data.baobao_command_ready=true;
			break;
		}
		fight_control_data.baobao_command_ready=false;
		if(baobao_fight_command_data.command!=command)		// 不同命令
		{
			baobao_fight_command_data.command=command;
			baobao_fight_command_data.source_id=fight_npc_group[fight_control_data.baobao_idx].base.id;
			baobao_fight_command_data.target_id=0;
			baobao_fight_command_data.type_no=-1;
		}
		else													// 相同命令,检查参数是否有用
		{
			baobao_fight_command_data.command=command;
			baobao_fight_command_data.source_id=fight_npc_group[fight_control_data.baobao_idx].base.id;
		}

		close_baobao_fight_all_window();
		switch(command)					// 自动开启宝宝所需要的 window
		{
		case FIGHT_COMMAND_MAGIC:
			exec_fight_menu_task_baobao_skill();
			break;
		case FIGHT_COMMAND_ITEM:
			exec_fight_menu_task_baobao_item();
			break;
		case FIGHT_COMMAND_CHANGE:
			baobao_fight_command_data.source_id=fight_npc_group[fight_control_data.character_idx].base.id;
			exec_fight_menu_task_baobao_change();
			break;
		}
		
		fight_control_data.player_select=false;		// 宝宝命令
		break;
	}


}


void close_character_fight_all_window(void)
{
	if(game_control.window_fight_character_item>=0)			// close item window
	{			
		delete_window_base(game_control.window_fight_character_item);
	}
	game_control.window_fight_character_item=-1;
	

	if(game_control.window_fight_character_skill>=0)		// close skill window
	{			
		delete_window_base(game_control.window_fight_character_skill);
	}
	game_control.window_fight_character_skill=-1;


	if(game_control.window_fight_baobao_change>=0)		// close baobao change window
	{			
		delete_window_base(game_control.window_fight_baobao_change);
	}
	game_control.window_fight_baobao_change=-1;
	
	
}

void close_baobao_fight_all_window(void)
{
	if(game_control.window_fight_baobao_item>=0)			// close item window
	{			
		delete_window_base(game_control.window_fight_baobao_item);
	}
	game_control.window_fight_baobao_item=-1;
	
	
	if(game_control.window_fight_baobao_skill>=0)		// close skill window
	{			
		delete_window_base(game_control.window_fight_baobao_skill);
	}
	game_control.window_fight_baobao_skill=-1;

	
	if(game_control.window_fight_baobao_change>=0)		// close baobao change window
	{			
		delete_window_base(game_control.window_fight_baobao_change);
	}
	game_control.window_fight_baobao_change=-1;
	
}


ULONG get_avail_attack_id(bool type)
{
	SLONG i,start,end;
	ULONG ret_val;

	ret_val=0;
	if(type)
		start=10;
	else
		start=0;

	end=start+10;
	
	for(i=start;i<end;i++)
	{
		if(fight_npc_group[i].base.active)	
		{
			if(fight_npc_group[i].base.status&FIGHT_STATUS_STONE)
				continue;
			if(fight_npc_group[i].base.status&FIGHT_STATUS_DEAD)
				continue;
			ret_val=fight_npc_group[i].base.id;
			break;
		}
	}

	return(ret_val);
}


void check_fight_command_parameter(SLONG npc,char command,ULONG source_id,ULONG *target_id,char *type_no)
{
	ULONG id;
	bool left;
	SLONG idx;


	switch(command)
	{
	case FIGHT_COMMAND_ATTACK:		// 物理攻击	-> 检查 target_id
		if(source_id==*target_id)
		{
			*target_id=0;
			return;
		}
		idx=get_fight_npc_index(*target_id);
		if(idx<0)				// 没有这个 id,或者已经倒地
		{
			if(fight_control_data.character_idx<10)		
				left=true;
			else
				left=false;
			id=get_avail_attack_id(left);
			if(id!=0)
				*target_id=id;
			else
				*target_id=0;
		}
		
		break;
	case FIGHT_COMMAND_MAGIC:		// 法术攻击
		if(*type_no<0)				// 没有技能编号
			return;
		switch(npc)
		{
		case FIGHT_COMMAND_CHARACTER:
			idx=get_character_skill_index(*type_no);
			if(idx<0)				// 没有这个法术
			{
				*type_no=-1;
				*target_id=0;
				return;				
			}
			idx=get_fight_npc_index(*target_id);
			if(idx<0)				// 没有这个 id,或者已经倒地
			{
				*target_id=0;
				return;
			}
			
			if(skill_base_data[*type_no].type==0)		// 我方
			{
				if(idx>=10)		// 指到敌方 error
				{
					*target_id=0;
					return;					
				}
			}
			else							// 敌方
			{
				if(idx<10)		// 指到我方 error
				{
					*target_id=0;
					return;					
				}
				
			}

			break;
		case FIGHT_COMMAND_BAOBAO:				// 检查宝宝技能
			idx=get_baobao_skill_index(*type_no);
			if(idx<0)				// 没有这个法术
			{
				*type_no=-1;
				*target_id=0;
				return;				
			}
			idx=get_fight_npc_index(*target_id);
			if(idx<0)				// 没有这个 id,或者已经倒地
			{
				*target_id=0;
				return;
			}
			
			if(skill_base_data[*type_no].type==0)		// 我方
			{
				if(idx>=10)		// 指到敌方 error
				{
					*target_id=0;
					return;					
				}
			}
			else							// 敌方
			{
				if(idx<10)		// 指到我方 error
				{
					*target_id=0;
					return;					
				}
				
			}

			break;
		default:
			*type_no=-1;
			*target_id=0;
			return;
		}

		break;
	case FIGHT_COMMAND_ITEM:		// 使用道具
		if(*type_no<0)				// 没有口袋 idx
			return;
		idx=*type_no;
		if((idx<0)||(idx>=MAX_POCKET_ITEM))
		{
			*type_no=-1;
			return;
		}
		if((character_pocket_data.item[idx].item_id<0)||(character_pocket_data.item[idx].number<1))
		{
			*type_no=-1;
			return;
		}
		idx=get_fight_npc_index(*target_id);
		if(idx<0)				// 没有这个 id,或者已经倒地
			*target_id=0;
		
		break;
	case FIGHT_COMMAND_PROTECT:		// 保护
		idx=get_fight_npc_index(*target_id);
		if(idx<0)				// 没有这个 id,或者已经倒地
			*target_id=0;
		if(*target_id==source_id)
			*target_id=0;
		break;
	case FIGHT_COMMAND_RECOVER:		// 防御
		break;
	case FIGHT_COMMAND_RUN:			// 逃跑
		break;
	case FIGHT_COMMAND_CHANGE:		// 更换宝宝
		if(*type_no<0)
			return;
		if(base_baobao_data.data.baobao_list[*type_no].baobao_id==0)
			*type_no=-1;
		break;
	case FIGHT_COMMAND_AUTO_HP:		// 自动补血
		break;
	case FIGHT_COMMAND_AUTO_MP:		// 自动补法
		break;
	case FIGHT_COMMAND_AUTO_ATTACK:		// 自动攻击
		break;
	}
}



SLONG get_character_skill_index(SLONG skill_id)
{
	SLONG i;
	SLONG ret_val;

	ret_val=-1;
	for(i=0;i<MAX_CHARACTER_SKILL_NO;i++)
	{
		if(character_skill_data[i].idx<0)continue;
		if(character_skill_data[i].idx==skill_id)
			return(i);
	}

	return(ret_val);
}


SLONG get_baobao_skill_index(SLONG skill_id)
{
	SLONG i;
	SLONG ret_val;
	
	ret_val=-1;
	for(i=0;i<MAX_BAOBAO_SKILL_NO;i++)
	{
		if(baobao_skill_data[i].skill_id<0)continue;
		if(baobao_skill_data[i].skill_id==skill_id)
			return(i);
	}
	
	return(ret_val);
}



SLONG get_fight_npc_index(ULONG id)
{
	SLONG i;
	SLONG ret_val;

	ret_val=-1;
	for(i=0;i<MAX_FIGHT_UNIT;i++)
	{
		if(!fight_npc_group[i].base.active)continue;
		if(fight_npc_group[i].base.id==id)
		{
			return(i);
		}
	}


	return(ret_val);
}



void check_fight_main_command(void)
{
	UCHR main_command_str[80];
	UCHR sub_command_str[80];
	SLONG command_offset;
	IMG256_GROUP *image;
	SLONG x,y;
	SLONG idx;
	bool main_ready;
	bool sub_ready;
	
	
	RECT menu_pp[MAX_FIGHT_BUTTON]={
		{  22, 31, 22+56, 31+56 },			// 头像		0
		{ 238, 18,238+24, 18+55 },			// 攻击		1
		{ 264, 18,264+24, 18+55 },			// 法术		2
		{ 290, 18,290+24, 18+55 },			// 道具		3
		{ 316, 18,316+24, 18+55 },			// 保护		4
		{ 342, 18,342+24, 18+55 },			// 防御		5
		{ 368, 18,368+24, 18+55 },			// 逃跑		6
		{ 210, 19,210+26, 19+25 },			// 红瓶		7
		{ 210, 48,210+26, 48+25 },			// 蓝瓶		8
		{ 244, 81,244+143,81+38 },			// 可点区域	9
		
		{ 429, 26,429+44, 26+44 },			// 头像		10
		{ 633, 18,633+24, 18+55 },			// 攻击		11
		{ 659, 18,659+24, 18+55 },			// 法术		12
		{ 685, 18,685+24, 18+55 },			// 道具		13
		{ 711, 18,711+24, 18+55 },			// 保护		14
		{ 737, 18,737+24, 18+55 },			// 防御		15
		{ 763, 18,763+24, 18+55 },			// 更换		16
		{ 605, 19,605+26, 19+25 },			// 红瓶		17
		{ 605, 48,605+26, 48+25 },			// 蓝瓶		18
		{ 639, 81,639+143,81+38 },			// 可点区域	19
		
		{ 403, 10, 403+15, 10+26 },			// 上拉选单 20
		{ 400, 49, 400+21, 49+72 }			// 自动		21
		
	};
	
	SLONG button_index[MAX_FIGHT_BUTTON]={
		-1,			// 头像		0
		7,			// 攻击		1
		9,			// 法术		2
		11,			// 道具		3
		13,			// 保护		4
		15,			// 防御		5
		17,			// 逃跑		6
		21,			// 红瓶		7
		23,			// 蓝瓶		8
		-1,			// 可点区域	9
			
		-1,			// 头像		10
		7,			// 攻击		11
		9,			// 法术		12
		11,			// 道具		13
		13,			// 保护		14
		15,			// 防御		15
		19,			// 更换		16
		21,			// 红瓶		17
		23,			// 蓝瓶		18
		-1,			// 可点区域	19
			
		25,			// 上拉选单 20
		27			// 自动		21
			
	};
	
	
	x=fight_image.menu_body.rect.left;
	y=fight_image.menu_body.rect.top;
	
	// ----- display command status
	main_command_str[0]=NULL;
	sub_command_str[0]=NULL;
	command_offset=1;

	main_ready=false;
	sub_ready=false;
	
	check_fight_command_parameter(FIGHT_COMMAND_CHARACTER,character_fight_command_data.command,character_fight_command_data.source_id,&character_fight_command_data.target_id,&character_fight_command_data.type_no);

	if(fight_control_data.character_idx<0)
	{
		strcpy((char *)main_command_str,MSG_FIGHT_SPECTATOR);
		strcpy((char *)sub_command_str,MSG_FIGHT_SPECTATOR_COMMAND);
		print16(x+255,y+82,main_command_str,PEST_PUT,screen_buffer);
		print16(x+255,y+102,sub_command_str,PEST_PUT,screen_buffer);
		return;
	}

	if(fight_npc_group[fight_control_data.character_idx].base.active)
	{
		switch(character_fight_command_data.command)
		{
		case FIGHT_COMMAND_ATTACK:		// 物理攻击
			fight_control_data.character_command_ready=false;
			image=fight_image_index[button_index[1]+command_offset];
			display_img256_buffer(menu_pp[1].left+x,menu_pp[1].top+y,image->image,screen_buffer);
			strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_ATTACK);
			if(character_fight_command_data.target_id!=0)
			{
				idx=get_fight_npc_index(character_fight_command_data.target_id);
				if(idx<0)
				{
					character_fight_command_data.target_id=0;
					strcpy((char *)sub_command_str,MSG_FIGHT_LEFT_TARGET_NONE);
				}
				else
				{
					sprintf((char *)print_rec,MSG_FIGHT_TARGET,(char *)fight_npc_group[idx].base.name);
					strcpy((char *)sub_command_str,(char *)print_rec);
					fight_control_data.character_command_ready=true;
				}
			}
			else
			{
				character_fight_command_data.target_id=0;
				strcpy((char *)sub_command_str,MSG_FIGHT_LEFT_TARGET_NONE);
			}
			
			break;
		case FIGHT_COMMAND_MAGIC:		// 法术攻击
			fight_control_data.character_command_ready=false;
			image=fight_image_index[button_index[2]+command_offset];
			display_img256_buffer(menu_pp[2].left+x,menu_pp[2].top+y,image->image,screen_buffer);
			idx=character_fight_command_data.type_no;
			if(idx<0)
				strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_MAGIC_SEL);
			else
			{
				sprintf((char *)print_rec,MSG_FIGHT_COMMAND_MAGIC,skill_base_data[idx].name);
				strcpy((char *)main_command_str,(char *)print_rec);
				main_ready=true;
			}

			if(main_ready)
			{
				if(character_fight_command_data.target_id!=0)		// 有人
				{
					idx=get_fight_npc_index(character_fight_command_data.target_id);
					if(idx<0)
					{
						character_fight_command_data.target_id=0;
						strcpy((char *)sub_command_str,MSG_FIGHT_LEFT_TARGET_NONE);
					}
					else
					{
						sprintf((char *)print_rec,MSG_FIGHT_TARGET,(char *)fight_npc_group[idx].base.name);
						strcpy((char *)sub_command_str,(char *)print_rec);
						fight_control_data.character_command_ready=true;
					}
					
				}
				else 
				{
					character_fight_command_data.target_id=0;
					strcpy((char *)sub_command_str,MSG_FIGHT_LEFT_TARGET_NONE);
				}
			}

			break;
		case FIGHT_COMMAND_ITEM:		// 使用道具
			fight_control_data.character_command_ready=false;
			image=fight_image_index[button_index[3]+command_offset];
			display_img256_buffer(menu_pp[3].left+x,menu_pp[3].top+y,image->image,screen_buffer);
			idx=character_fight_command_data.type_no;
			if(idx<0)
				strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_ITEM_SEL);
			else
			{
				strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_ITEM);
				display_img256_buffer(x+351,y+84,system_image.item_image[character_pocket_data.item[idx].filename_id],screen_buffer);
				main_ready=true;
			}
			
			if(main_ready)
			{
				if(character_fight_command_data.target_id!=0)		// 有人
				{
					idx=get_fight_npc_index(character_fight_command_data.target_id);
					if(idx<0)
					{
						character_fight_command_data.target_id=0;
						strcpy((char *)sub_command_str,MSG_FIGHT_LEFT_TARGET_NONE);
					}
					else
					{
						sprintf((char *)print_rec,MSG_FIGHT_TARGET,(char *)fight_npc_group[idx].base.name);
						strcpy((char *)sub_command_str,(char *)print_rec);
						fight_control_data.character_command_ready=true;
					}
					
				}
				else 
				{
					character_fight_command_data.target_id=0;
					strcpy((char *)sub_command_str,MSG_FIGHT_LEFT_TARGET_NONE);
				}
			}
			
			break;
		case FIGHT_COMMAND_PROTECT:		// 保护
			fight_control_data.character_command_ready=false;
			image=fight_image_index[button_index[4]+command_offset];
			display_img256_buffer(menu_pp[4].left+x,menu_pp[4].top+y,image->image,screen_buffer);
			strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_PROTECT);
			if(character_fight_command_data.target_id!=0)
			{
				idx=get_fight_npc_index(character_fight_command_data.target_id);
				if(idx<0)
				{
					character_fight_command_data.target_id=0;
					strcpy((char *)sub_command_str,MSG_FIGHT_LEFT_TARGET_NONE);
				}
				else
				{
					sprintf((char *)print_rec,MSG_FIGHT_TARGET,(char *)fight_npc_group[idx].base.name);
					strcpy((char *)sub_command_str,(char *)print_rec);
					fight_control_data.character_command_ready=true;
				}
			}
			else
			{
				character_fight_command_data.target_id=0;
				strcpy((char *)sub_command_str,MSG_FIGHT_LEFT_TARGET_NONE);
			}
			
			break;
		case FIGHT_COMMAND_RECOVER:		// 防御
			image=fight_image_index[button_index[5]+command_offset];
			display_img256_buffer(menu_pp[5].left+x,menu_pp[5].top+y,image->image,screen_buffer);
			strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_RECOVER);
			fight_control_data.character_command_ready=true;
			break;
		case FIGHT_COMMAND_RUN:			// 逃跑
			fight_control_data.character_command_ready=true;
			image=fight_image_index[button_index[6]+command_offset];
			display_img256_buffer(menu_pp[6].left+x,menu_pp[6].top+y,image->image,screen_buffer);
			strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_RUN);
			break;
		case FIGHT_COMMAND_AUTO_HP:		// 自动补血
			fight_control_data.character_command_ready=true;
			image=fight_image_index[button_index[7]+command_offset];
			display_img256_buffer(menu_pp[7].left+x,menu_pp[7].top+y,image->image,screen_buffer);
			strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_AUTO_HP);
			break;
		case FIGHT_COMMAND_AUTO_MP:		// 自动补法
			fight_control_data.character_command_ready=true;
			image=fight_image_index[button_index[8]+command_offset];
			display_img256_buffer(menu_pp[8].left+x,menu_pp[8].top+y,image->image,screen_buffer);
			strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_AUTO_MP);
			break;
		case FIGHT_COMMAND_AUTO_ATTACK:	// 自动攻击
			fight_control_data.character_command_ready=true;
			image=fight_image_index[button_index[21]+command_offset];
			display_img256_buffer(menu_pp[21].left+x,menu_pp[21].top+y,image->image,screen_buffer);
			strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_AUTO_ATTACK);
			break;
		case FIGHT_COMMAND_CHANGE:		// 更换
			fight_control_data.character_command_ready=false;
			image=fight_image_index[button_index[16]+command_offset];
			display_img256_buffer(menu_pp[16].left+x,menu_pp[16].top+y,image->image,screen_buffer);
			idx=character_fight_command_data.type_no;
			if(idx<0)
				strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_CHANGE_SEL);
			else
			{
				if(base_baobao_data.data.baobao_list[idx].baobao_id!=0)
				{
					strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_CHANGE);
					sprintf((char *)print_rec,MSG_FIGHT_TARGET,(char *)base_baobao_data.data.baobao_list[idx].name);
					strcpy((char *)sub_command_str,(char *)print_rec);
					main_ready=true;
					fight_control_data.character_command_ready=true;
				}
				else
				{
					strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_CHANGE_SEL);
				}
			}
			break;
		default:						// 没有命令
			fight_control_data.character_command_ready=false;
			strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_NONE);
			strcpy((char *)sub_command_str,MSG_FIGHT_LEFT_TARGET_NONE);
			break;
		}
		
		
		print16(x+255,y+82,main_command_str,PEST_PUT,screen_buffer);
		print16(x+255,y+102,sub_command_str,PEST_PUT,screen_buffer);


		if(fight_control_data.character_command_ready)		// display fight command status
		{
			image=fight_image_index[4];
			display_img256_buffer(fight_image.menu_body.rect.left+101,fight_image.menu_body.rect.top+70,image->image,screen_buffer);
		}
		else
		{
			if(character_charge_time_data.delay_time>0)		// 超过充电时间
			{
				switch((system_control.system_timer/100)%3)
				{
				case 0:
					image=fight_image_index[4];
					break;
				case 1:
					image=fight_image_index[5];
					break;
				case 2:
					image=fight_image_index[6];
					break;
				default:
					break;
				}
				display_img256_buffer(fight_image.menu_body.rect.left+101,fight_image.menu_body.rect.top+70,image->image,screen_buffer);
			}
		}
		
		
		
	}
	



	if(fight_control_data.baobao_idx<0)
	{
		fight_control_data.baobao_command_ready=false;
		return;
	}
	if(!fight_npc_group[fight_control_data.baobao_idx].base.active)					// 目前没有宝宝参战
	{
		fight_control_data.baobao_command_ready=false;
		return;
	}



	main_command_str[0]=NULL;
	sub_command_str[0]=NULL;
	command_offset=1;

	main_ready=false;
	sub_ready=false;
	

	check_fight_command_parameter(FIGHT_COMMAND_BAOBAO,baobao_fight_command_data.command,baobao_fight_command_data.source_id,&baobao_fight_command_data.target_id,&baobao_fight_command_data.type_no);
	
	switch(baobao_fight_command_data.command)
	{
	case FIGHT_COMMAND_ATTACK:		// 物理攻击
		fight_control_data.baobao_command_ready=false;
		image=fight_image_index[button_index[11]+command_offset];
		display_img256_buffer(menu_pp[11].left+x,menu_pp[11].top+y,image->image,screen_buffer);
		strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_ATTACK);
		if(baobao_fight_command_data.target_id!=0)
		{
			idx=get_fight_npc_index(baobao_fight_command_data.target_id);
			if(idx<0)
			{
				baobao_fight_command_data.target_id=0;
				strcpy((char *)sub_command_str,MSG_FIGHT_RIGHT_TARGET_NONE);
			}
			else
			{
				sprintf((char *)print_rec,MSG_FIGHT_TARGET,(char *)fight_npc_group[idx].base.name);
				strcpy((char *)sub_command_str,(char *)print_rec);
				fight_control_data.baobao_command_ready=true;
			}
		}
		else
		{
			baobao_fight_command_data.target_id=0;
			strcpy((char *)sub_command_str,MSG_FIGHT_RIGHT_TARGET_NONE);
		}
		
		break;
	case FIGHT_COMMAND_MAGIC:		// 法术攻击
		fight_control_data.baobao_command_ready=false;
		image=fight_image_index[button_index[12]+command_offset];
		display_img256_buffer(menu_pp[12].left+x,menu_pp[12].top+y,image->image,screen_buffer);
		idx=baobao_fight_command_data.type_no;
		if(idx<0)
			strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_MAGIC_SEL);
		else
		{
			sprintf((char *)print_rec,MSG_FIGHT_COMMAND_MAGIC,skill_base_data[idx].name);
			strcpy((char *)main_command_str,(char *)print_rec);
			main_ready=true;
		}
		
		if(main_ready)
		{
			if(baobao_fight_command_data.target_id!=0)		// 有人
			{
				idx=get_fight_npc_index(baobao_fight_command_data.target_id);
				if(idx<0)
				{
					baobao_fight_command_data.target_id=0;
					strcpy((char *)sub_command_str,MSG_FIGHT_RIGHT_TARGET_NONE);
				}
				else
				{
					sprintf((char *)print_rec,MSG_FIGHT_TARGET,(char *)fight_npc_group[idx].base.name);
					strcpy((char *)sub_command_str,(char *)print_rec);
					fight_control_data.baobao_command_ready=true;
				}
				
			}
			else 
			{
				baobao_fight_command_data.target_id=0;
				strcpy((char *)sub_command_str,MSG_FIGHT_RIGHT_TARGET_NONE);
			}
		}
		

		break;
	case FIGHT_COMMAND_ITEM:		// 使用道具
		fight_control_data.baobao_command_ready=false;
		image=fight_image_index[button_index[13]+command_offset];
		display_img256_buffer(menu_pp[13].left+x,menu_pp[13].top+y,image->image,screen_buffer);
		idx=baobao_fight_command_data.type_no;
		if(idx<0)
			strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_ITEM_SEL);
		else
		{
			strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_ITEM);
			display_img256_buffer(x+746,y+84,system_image.item_image[character_pocket_data.item[idx].filename_id],screen_buffer);
			main_ready=true;
		}
		
		if(main_ready)
		{
			if(baobao_fight_command_data.target_id!=0)		// 有人
			{
				idx=get_fight_npc_index(baobao_fight_command_data.target_id);
				if(idx<0)
				{
					baobao_fight_command_data.target_id=0;
					strcpy((char *)sub_command_str,MSG_FIGHT_RIGHT_TARGET_NONE);
				}
				else
				{
					sprintf((char *)print_rec,MSG_FIGHT_TARGET,(char *)fight_npc_group[idx].base.name);
					strcpy((char *)sub_command_str,(char *)print_rec);
					fight_control_data.baobao_command_ready=true;
				}
				
			}
			else 
			{
				baobao_fight_command_data.target_id=0;
				strcpy((char *)sub_command_str,MSG_FIGHT_RIGHT_TARGET_NONE);
			}
		}
		
		
		break;
	case FIGHT_COMMAND_PROTECT:		// 保护
		fight_control_data.baobao_command_ready=false;
		image=fight_image_index[button_index[14]+command_offset];
		display_img256_buffer(menu_pp[14].left+x,menu_pp[14].top+y,image->image,screen_buffer);
		strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_PROTECT);
		if(baobao_fight_command_data.target_id!=0)
		{
			idx=get_fight_npc_index(baobao_fight_command_data.target_id);
			if(idx<0)
			{
				baobao_fight_command_data.target_id=0;
				strcpy((char *)sub_command_str,MSG_FIGHT_RIGHT_TARGET_NONE);
			}
			else
			{
				sprintf((char *)print_rec,MSG_FIGHT_TARGET,(char *)fight_npc_group[idx].base.name);
				strcpy((char *)sub_command_str,(char *)print_rec);
				fight_control_data.baobao_command_ready=true;
			}
		}
		else
		{
			baobao_fight_command_data.target_id=0;
			strcpy((char *)sub_command_str,MSG_FIGHT_RIGHT_TARGET_NONE);
		}
		
		break;
	case FIGHT_COMMAND_RECOVER:		// 防御
		image=fight_image_index[button_index[15]+command_offset];
		display_img256_buffer(menu_pp[15].left+x,menu_pp[15].top+y,image->image,screen_buffer);
		strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_RECOVER);
		fight_control_data.baobao_command_ready=true;
		break;
	case FIGHT_COMMAND_CHANGE:		// 更换
		fight_control_data.baobao_command_ready=false;
		image=fight_image_index[button_index[16]+command_offset];
		display_img256_buffer(menu_pp[16].left+x,menu_pp[16].top+y,image->image,screen_buffer);
		idx=baobao_fight_command_data.type_no;
		if(idx<0)
			strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_CHANGE_SEL);
		else
		{
			if(base_baobao_data.data.baobao_list[idx].baobao_id!=0)
			{
				strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_CHANGE);
				sprintf((char *)print_rec,MSG_FIGHT_TARGET,(char *)base_baobao_data.data.baobao_list[idx].name);
				strcpy((char *)sub_command_str,(char *)print_rec);
				main_ready=true;
				fight_control_data.baobao_command_ready=true;
			}
			else
			{
				strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_CHANGE_SEL);
			}
		}

		break;
	case FIGHT_COMMAND_AUTO_HP:		// 自动补血
		fight_control_data.baobao_command_ready=true;
		image=fight_image_index[button_index[17]+command_offset];
		display_img256_buffer(menu_pp[17].left+x,menu_pp[17].top+y,image->image,screen_buffer);
		strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_AUTO_HP);
		break;
	case FIGHT_COMMAND_AUTO_MP:		// 自动补法
		fight_control_data.baobao_command_ready=true;
		image=fight_image_index[button_index[18]+command_offset];
		display_img256_buffer(menu_pp[18].left+x,menu_pp[18].top+y,image->image,screen_buffer);
		strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_AUTO_MP);
		break;		
	case FIGHT_COMMAND_AUTO_ATTACK:
		fight_control_data.baobao_command_ready=true;
		strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_AUTO_ATTACK);
		break;
	default:						// 没有命令
		strcpy((char *)main_command_str,MSG_FIGHT_COMMAND_NONE);
		strcpy((char *)sub_command_str,MSG_FIGHT_RIGHT_TARGET_NONE);
		break;
	}
		

	print16(x+651,y+82,main_command_str,PEST_PUT,screen_buffer);
	print16(x+651,y+102,sub_command_str,PEST_PUT,screen_buffer);
	

	if(fight_control_data.baobao_command_ready)		// display fight command status
	{
		image=fight_image_index[4];
		display_img256_buffer(fight_image.menu_body.rect.left+496,fight_image.menu_body.rect.top+70,image->image,screen_buffer);
	}
	else
	{
		if(baobao_charge_time_data.delay_time>0)		// 超过充电时间
		{
			switch((system_control.system_timer/100)%3)
			{
			case 0:
				image=fight_image_index[4];
				break;
			case 1:
				image=fight_image_index[5];
				break;
			case 2:
				image=fight_image_index[6];
				break;
			default:
				break;
			}
			display_img256_buffer(fight_image.menu_body.rect.left+496,fight_image.menu_body.rect.top+70,image->image,screen_buffer);
		}
	}
	
}




SLONG gium_strlen(char *title)
{
	SLONG idx;
	SLONG main_pass;
	SLONG len;
	
	idx=0;
	main_pass=0;
	len=0;
	while(main_pass==0)
	{
		switch(title[idx])
		{
		case 0x00:
			main_pass=1;
			break;
		case '~':
			if((title[idx+1]==0x00)||(title[idx+2]==0x00))
				main_pass=1;
			idx=idx+2;
			break;
		default:
			len++;
			break;
		}
		idx++;
	}
	return(len);
}






/****************************************************************
{  更换人物状态
*****************************************************************/
void update_fight_status_result(FIGHT_STATUS_RESULT data)
{
	SLONG idx;
	ULONG id;
	SLONG npc_height;
	SLONG handle;
	SLONG mda_index;
	
	id=data.target_id;
	idx=get_fight_npc_index(id);
	if(idx<0)return;

	fight_npc_group[idx].base.status=data.status;
	fight_npc_group[idx].result.status=data.status;

	if(data.hp>0)
	{
		if(fight_npc_group[idx].base.active)
		{
			sprintf((char *)print_rec,"%d",data.hp);
			if(data.status&FIGHT_STATUS_DEAD_CALL)
			{
				handle=fight_npc_group[idx].handle;
				npc_height=0;
				mda_index=get_mda_index(handle);
				if((mda_index>=0)&&(mda_index<total_mda_group))
				{
					if(mda_group[mda_index].img256_head!=NULL)
					{
						npc_height=mda_group[mda_index].img256_head->ylen;
					}
				}
				add_fight_screen_text(fight_npc_group[idx].x,fight_npc_group[idx].y-npc_height,0,false,5000,print_rec);
			}
			else
			{
				add_fight_screen_text(fight_npc_group[idx].x,fight_npc_group[idx].y,0,true,2000,print_rec);
			}
		}
		
	}

}

/****************************************************************
{  更新人物数值
*****************************************************************/
void update_fight_value_result(FIGHT_VALUE_RESULT data)
{
	SLONG idx;
	ULONG id;

	id=data.id;
	idx=get_fight_npc_index(id);
	if(idx<0)return;
	memcpy(&fight_npc_group[idx].result,&data,sizeof(struct FIGHT_VALUE_RESULT_STRUCT));
	
	fight_npc_group[idx].base.now_hp = fight_npc_group[idx].result.now_hp;
	fight_npc_group[idx].base.max_hp = fight_npc_group[idx].result.max_hp;

	fight_npc_group[idx].base.now_mp = fight_npc_group[idx].result.now_mp;
	fight_npc_group[idx].base.max_mp = fight_npc_group[idx].result.max_mp;
		
	if((fight_npc_group[idx].result.status&FIGHT_STATUS_DEAD)&&
		(fight_npc_group[idx].result.status&FIGHT_STATUS_DEAD_CALL))
	{
		fight_npc_group[idx].result.status=fight_npc_group[idx].result.status&(~FIGHT_STATUS_DEAD_CALL);
		update_fight_result(idx);
	}

}


/****************************************************************
{  下达动作命令
*****************************************************************/
void update_fight_action_result(FIGHT_ACTION_RESULT data)
{
	if(get_fight_npc_index(data.source_id)<0)
	{
		display_error_message((UCHR *)"Fight source id error !",1);
		return;
	}
	
	fight_control_data.action_source_id=data.source_id;					// 发动人 id
	fight_control_data.action_target_id=data.target_id;					// 受动作人 id ( 0 -> 代表多人)
	fight_control_data.action_idx=data.idx;

	fight_control_data.fight_action_ready=true;				// 战斗动作 ready
	fight_control_data.fight_action_command=data.command;	// 战斗动作种类
//	fight_charge_pause();									// 战斗暂停充电
		
}




void check_emote(UCHR *data,UCHR *name)
{
	UCHR main_command[80];
	UCHR sub_command[80];
	UCHR my_id,other_id;
	UCHR my_name[80];
	UCHR other_name[80];
	SLONG command_type;
	UCHR text[1024];
	SLONG i,j,k;
	SLONG idx;
	bool find;

	take_space(data);
	if(data[0]!='/')return;



	command_type=0;	
	idx=1;
	get_part_str1(data,main_command,&idx,' ',200);

	my_id=0;
	other_id=0;
//	sprintf((char *)my_name,"#Y%s#N",(char *)base_character_data.name);
	sprintf((char *)my_name,"#Y%s#N",(char *)name);
	strcpy((char *)other_name,"");
	idx++;
	get_part_str1(data,sub_command,&idx,' ',200);
	switch(sub_command[0])
	{
	case '1':
		command_type=1;
		break;
	case '2':
		command_type=2;
		if(system_control.person_npc_id<=0)
		{
			return;
		}
		sprintf((char *)other_name,"#cfa8cfa%s#N",(char *)system_control.person_name);
		break;
	}

	find=false;
	strcpy((char *)text,"");
	for(i=0;i<TOTAL_EMOTE;i++)
	{
		if(strcmpi((char *)main_command,(char *)emote_data[i].command)==0)
		{
			find=true;
			switch(command_type)
			{
			case 0:
				strcpy((char *)text,(char *)emote_data[i].say0);
				break;
			case 1:
				strcpy((char *)text,(char *)emote_data[i].say1);
				break;
			case 2:
				strcpy((char *)text,(char *)emote_data[i].say2);
				break;
			}
		}
		else if(strcmpi((char *)main_command,(char *)emote_data[i].command1)==0)
		{
			find=true;
			switch(command_type)
			{
			case 0:
				strcpy((char *)text,(char *)emote_data[i].say0);
				break;
			case 1:
				strcpy((char *)text,(char *)emote_data[i].say1);
				break;
			case 2:
				strcpy((char *)text,(char *)emote_data[i].say2);
				break;
			}
		}
			
		if(find)break;
	}

	if(find==false)return;

// ----- convert data 
	for(j=0,i=0;i<(SLONG)strlen((char *)text);i++)
	{
		if(text[i]=='$')
		{
			i++;
			switch(text[i])
			{
			case '1':
				for(k=0;k<(SLONG)strlen((char *)my_name);k++)
				{
					data[j]=my_name[k];
					j++;
				}
				break;
			case '2':
				for(k=0;k<(SLONG)strlen((char *)other_name);k++)
				{
					data[j]=other_name[k];
					j++;
				}
				break;
			}
		}
		else
		{
			data[j]=text[i];
			j++;
		}

	}


	data[j]=NULL;

}




SLONG check_chat_command(UCHR *data)
{
	SLONG ret_val;
	bool change_channel;
	UCHR temp[1024];
	SLONG i,j;

	take_space(data);

	if(data[0]!=':')return(TTN_NOT_OK);

	change_channel=false;
	ret_val=TTN_NOT_OK;
	switch(data[1])
	{
	case 's': case 'S':			// 当前频道
		game_control.chat_channel=CHAT_CHANNEL_SCREEN;
		change_channel=true;
		break;
	case 't': case 'T':			// 队伍
		game_control.chat_channel=CHAT_CHANNEL_TEAM;
		change_channel=true;
		break;
	case 'p': case 'P':			// 私聊
		game_control.chat_channel=CHAT_CHANNEL_PERSON;
		change_channel=true;
		break;
	case 'g': case 'G':			// 公会
		game_control.chat_channel=CHAT_CHANNEL_GROUP;
		change_channel=true;
		break;
	case 'e': case 'E':			// 经济
		game_control.chat_channel=CHAT_CHANNEL_SELLBUY;
		change_channel=true;
		break;
	case 'w': case 'W':			// 世界
		game_control.chat_channel=CHAT_CHANNEL_WORLD;
		change_channel=true;
		break;
	}

	temp[0]=NULL;
	if(change_channel)
	{
		for(j=0,i=2;i<(SLONG)strlen((char *)data);i++,j++)
		{
			temp[j]=data[i];
		}
		temp[j]=NULL;
		strcpy((char *)data,(char *)temp);
		ret_val=TTN_OK;
	}

	return(TTN_OK);

}




SLONG check_is_system_command(UCHR *data)
{
	insert_task_list(display_map_info,0);
	UCHR main_command[80];
	UCHR sub_command[80];
	UCHR value_command[80];
	UCHR value1_command[80];
	SLONG idx;
	SLONG ret_val;
	
	take_space(data);
	if(data[0]!='*')return(TTN_NOT_OK);
	
	idx=1;
	get_part_str1(data,main_command,&idx,' ',200);
	idx++;
	get_part_str1(data,sub_command,&idx,' ',200);
	idx++;
	get_part_str1(data,value_command,&idx,' ',200);
	idx++;
	get_part_str1(data,value1_command,&idx,' ',200);
	
	ret_val=TTN_NOT_OK;
	if(strcmpi((char *)main_command,"stephenandy")==0)		// 启动系统命令
	{
		ret_val=TTN_OK;

#ifdef MYTH_DEBUG
		
		if(strcmpi((char *)sub_command,"map")==0)			// 地图 debug 
		{
			if(strcmpi((char *)value_command,"debug")==0)
			{
				debug_info.map_debug=!debug_info.map_debug;
			}
			else if(strcmpi((char *)value_command,"stop")==0)
			{
				debug_info.map_stop=!debug_info.map_stop;
			}
			else if(strcmpi((char *)value_command,"event")==0)
			{
				debug_info.map_event=!debug_info.map_event;
			}
			else if(strcmpi((char *)value_command,"path")==0)
			{
				debug_info.map_path=!debug_info.map_path;
			}
			else if(strcmpi((char *)value_command,"shadow")==0)
			{
				debug_info.map_shadow=!debug_info.map_shadow;
			}
			else if(strcmpi((char *)value_command,"sound")==0)
			{
				debug_info.map_sound=!debug_info.map_sound;
			}
			else if(strcmpi((char *)value_command,"?")==0)
			{
				push_chat_data(CHAT_CHANNEL_MESSAGE,0,NULL,(UCHR *)"debug");
				push_chat_data(CHAT_CHANNEL_MESSAGE,0,NULL,(UCHR *)"stop");
				push_chat_data(CHAT_CHANNEL_MESSAGE,0,NULL,(UCHR *)"event");
				push_chat_data(CHAT_CHANNEL_MESSAGE,0,NULL,(UCHR *)"path");
				push_chat_data(CHAT_CHANNEL_MESSAGE,0,NULL,(UCHR *)"shadow");
				push_chat_data(CHAT_CHANNEL_MESSAGE,0,NULL,(UCHR *)"sound");

			}
		}
#endif

		if(strcmpi((char *)sub_command,"administrator")==0)			// 地图 debug 
		{
			if(strcmpi((char *)value_command,"frame")==0)
			{
				insert_task_list(display_map_info,0);
			}
			else if(strcmpi((char *)value_command,"npc")==0)
			{
				insert_task_list(display_user_info,1);
			}
			else if(strcmpi((char *)value_command,"?")==0)
			{
				push_chat_data(CHAT_CHANNEL_MESSAGE,0,NULL,(UCHR *)"frame");
				push_chat_data(CHAT_CHANNEL_MESSAGE,0,NULL,(UCHR *)"npc");
			}
			
		}
			
		
		if(strcmpi((char *)sub_command,"?")==0)		
		{
			push_chat_data(CHAT_CHANNEL_MESSAGE,0,NULL,(UCHR *)"administrator");
			push_chat_data(CHAT_CHANNEL_MESSAGE,0,NULL,(UCHR *)"map");
		}
			
	}

		
	return(ret_val);


	
}




void check_character_command_change(void)
{

	if(!fight_control_data.character_command_send)		// 目前命令还没送出去
		return;

	if((character_fight_command_data.command!=backup_character_fight_command_data.command)||
		(character_fight_command_data.source_id!=backup_character_fight_command_data.source_id)||
		(character_fight_command_data.target_id!=backup_character_fight_command_data.target_id)||
		(character_fight_command_data.type_no!=backup_character_fight_command_data.type_no))
	{			// 命令变了
		fight_control_data.character_command_send=false;
	}

}


void check_baobao_command_change(void)
{

	if(!fight_control_data.baobao_command_send)		// 目前命令还没送出去
		return;

	if((baobao_fight_command_data.command!=backup_baobao_fight_command_data.command)||
		(baobao_fight_command_data.source_id!=backup_baobao_fight_command_data.source_id)||
		(baobao_fight_command_data.target_id!=backup_baobao_fight_command_data.target_id)||
		(baobao_fight_command_data.type_no!=backup_baobao_fight_command_data.type_no))
	{			// 命令变了
		fight_control_data.baobao_command_send=false;
	}
	
}



void convert_text_chat(UCHR *text,UCHR *chat_text)
{
	ULONG i,j;

	i=0;
	j=0;
	for(j=0,i=0;i<strlen((char *)text);i++)
	{
		switch(text[i])
		{
		case '~':
			if(i>=strlen((char *)text)-2)break;
			if((text[i+1]=='c')||(text[i+1]=='C'))
			{
				switch(text[i+2])
				{
				case '0':
					chat_text[j]='#';
					j++;
					chat_text[j]='N';
					j++;
					break;
				case '1':
					chat_text[j]='#';
					j++;
					chat_text[j]='R';
					j++;
					break;
				case '2':
					chat_text[j]='#';
					j++;
					chat_text[j]='G';
					j++;
					break;
				case '3':
					chat_text[j]='#';
					j++;
					chat_text[j]='B';
					j++;
					break;
				case '4':
					chat_text[j]='#';
					j++;
					chat_text[j]='Y';
					j++;
					break;
				default:
					chat_text[j]='#';
					j++;
					chat_text[j]='N';
					j++;
					break;
				}
			}
			i+=2;
			break;
		default:
			chat_text[j]=text[i];
			j++;
			break;
		}

	}
	chat_text[j]=NULL;
	
}



void auto_set_fight_mouse_image(void)
{
	RECT mouse_rect;
	ULONG baobao_id;
	SLONG idx;


	if(fight_control_data.character_idx<0)
	{
		set_mouse_cursor(MOUSE_IMG_STAND);
		return;
	}
	
	mouse_rect.left=system_control.mouse_x;
	mouse_rect.top=system_control.mouse_y;
	mouse_rect.right=system_control.mouse_x;
	mouse_rect.bottom=system_control.mouse_y;
	if(check_have_touch_window(mouse_rect)>=0)
	{
		set_mouse_cursor(MOUSE_IMG_STAND);
		return;
	}


	if(fight_control_data.player_select)		// 目前主角命令
	{
		switch(character_fight_command_data.command)
		{
		case FIGHT_COMMAND_ATTACK:		// 物理攻击
			set_mouse_cursor(MOUSE_IMG_ATTACK);
			break;
		case FIGHT_COMMAND_MAGIC:		// 法术攻击
			if(character_fight_command_data.type_no>=0)
				set_mouse_cursor(MOUSE_IMG_MAGIC);
			else
				set_mouse_cursor(MOUSE_IMG_STAND);
			break;
		case FIGHT_COMMAND_ITEM:		// 使用道具
			if(character_fight_command_data.type_no>=0)
				set_mouse_cursor(MOUSE_IMG_ITEM);
			else
				set_mouse_cursor(MOUSE_IMG_STAND);
			break;
		case FIGHT_COMMAND_PROTECT:		// 保护
		case FIGHT_COMMAND_RECOVER:		// 防御
		case FIGHT_COMMAND_RUN:			// 逃跑
		case FIGHT_COMMAND_AUTO_HP:		// 自动补血
		case FIGHT_COMMAND_AUTO_MP:		// 自动补法
		case FIGHT_COMMAND_AUTO_ATTACK:	// 自动攻击
		case FIGHT_COMMAND_CHANGE:		// 更换
		default:						// 没有命令
			set_mouse_cursor(MOUSE_IMG_STAND);
			break;
		}
	}
	else
	{

		if(fight_control_data.baobao_idx<0)
		{
			set_mouse_cursor(MOUSE_IMG_STAND);
			return;
		}

		if(!fight_npc_group[fight_control_data.baobao_idx].base.active)
		{
			set_mouse_cursor(MOUSE_IMG_STAND);
			return;
		}

		baobao_id=fight_npc_group[fight_control_data.baobao_idx].base.id;			// 宝宝 id
		idx=get_baobao_data_idx(baobao_id);
		if(idx<0)
		{
			set_mouse_cursor(MOUSE_IMG_STAND);
			return;
		}
		



		switch(baobao_fight_command_data.command)
		{
		case FIGHT_COMMAND_ATTACK:		// 物理攻击
			set_mouse_cursor(MOUSE_IMG_ATTACK);
			break;
		case FIGHT_COMMAND_MAGIC:		// 法术攻击
			if(baobao_fight_command_data.type_no>=0)
				set_mouse_cursor(MOUSE_IMG_MAGIC);
			else
				set_mouse_cursor(MOUSE_IMG_STAND);
			break;
		case FIGHT_COMMAND_ITEM:		// 使用道具
			if(baobao_fight_command_data.type_no>=0)
				set_mouse_cursor(MOUSE_IMG_ITEM);
			else
				set_mouse_cursor(MOUSE_IMG_STAND);
			break;
		case FIGHT_COMMAND_PROTECT:		// 保护
		case FIGHT_COMMAND_RECOVER:		// 防御
		case FIGHT_COMMAND_CHANGE:		// 更换
		case FIGHT_COMMAND_AUTO_HP:		// 自动补血
		case FIGHT_COMMAND_AUTO_MP:		// 自动补法
		case FIGHT_COMMAND_AUTO_ATTACK:
		default:
			set_mouse_cursor(MOUSE_IMG_STAND);
			break;
		}
			

	}


}



SLONG check_character_magic(SLONG skill_id)
{
	SLONG i;

	for(i=0;i<MAX_CHARACTER_SKILL_NO;i++)
	{
		if(character_skill_data[i].idx<0)continue;
		if(character_skill_data[i].idx==skill_id)			
			return(TTN_OK);
	}

	return(TTN_NOT_OK);
}


SLONG check_baobao_magic(SLONG skill_id)
{
	SLONG i;

	for(i=0;i<MAX_BAOBAO_SKILL_NO;i++)
	{
		if(baobao_skill_data[i].skill_id<0)continue;
		if(baobao_skill_data[i].skill_id==skill_id)
			return(TTN_OK);
	}

	return(TTN_NOT_OK);
}






