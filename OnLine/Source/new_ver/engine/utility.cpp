/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : utility.cpp
  *owner  		: Stephen
  *description  : 
  *modified     : 2004/12/6
******************************************************************************/ 

#include "engine_global.h"
#include "utility.h"
#include "graph.h"
#include "vari-ext.h"
#include "jpg.h"
#include "shellapi.h"
#include <string.h>
#include <tchar.h>
#include "idoorserver.h"
#include "ugdef.h"
#include "map.h"
#include "findpath.h"
#include "tier0/dbg.h"
#include "dm_music.h"
#include "net.h"
#include "player_info.h"
#include "taskman.h"
#include "weather.h"
#include "data_proc.h"
#include "data.h"
#include "fight.h"

short *EnergyFrame1=NULL;
short *EnergyFrame2=NULL;


void get_version_string(char *text,int version_no)
{
	sprintf(text,"%d.%04d",version_no/10000,version_no%10000);
}


SLONG check_file_exist(UCHR *filename)
{
	FILE *fp;
	
	if(filename[0]==0x00)return(TTN_NOT_OK);
	fp=fopen((const char *)filename,"rb");
	if(fp==NULL)
		return(TTN_NOT_OK);
	
	fclose(fp);
	return(TTN_OK);
	
}

SLONG check_file_size(UCHR *filename)
{
	FILE *fp;
	int handle;
	SLONG size;
	
	if(filename[0]==0x00)return(-1);
	fp=fopen((const char *)filename,"rb");
	if(fp==NULL)
	{
		sprintf((char *)print_rec,MSG_FILE_OPEN_ERROR,filename);
		log_error(1,print_rec);
		return(-1);
	}
	
	handle = fileno(fp);
	size=filelength(handle);
	fclose(fp);
	return(size);
	
}


SLONG check_is_ani_file(UCHR *filename)
{
	FILE *fp;
	CONTROL_ANI temp_ani_head;
	
	fp=fopen((char *)filename,"rb");
	if(fp==NULL)
		return(TTN_NOT_OK);
	fread(&temp_ani_head,sizeof(struct CONTROL_ANI_HEAD_STRUCT),1,fp);
	fclose(fp);
	if(strcmpi((char *)temp_ani_head.copyright,"RAYS CONTROL FILE")!=0)
		return(TTN_NOT_OK);

	if(temp_ani_head.control_type!=FIGHT_CONTROL_TYPE)
	{
		display_error_message((UCHR *)"Ani file type error !",true);
		return(TTN_NOT_OK);
	}
				
	return(TTN_OK);
}


SLONG load_jpg_file(UCHR *filename,BMP *bmp_buffer,SLONG x,SLONG y,SLONG xlen,SLONG ylen)
{
	FILE *fp;
	SLONG file_size;
	UCHR *file_buffer=NULL;
	UCHR *bit24_screen_buffer=NULL;
	SLONG bit24_buffer_len=0;
	
	
	JPEGAREA jpeg_area;
	JPEGINFO info;
	
	
	
	file_size=check_file_size(filename);
	if(file_size<=0)
	{
		sprintf((char *)print_rec,MSG_FILE_CHECK_ERROR,filename);
		log_error(1,print_rec);
		return(TTN_NOT_OK);
	}
	fp=fopen((char *)filename,"rb");
	if(fp==NULL)
	{
		sprintf((char *)print_rec,MSG_FILE_OPEN_ERROR,filename);
		log_error(1,print_rec);
		return(TTN_NOT_OK);
		
	}
	file_buffer=(UCHR *)malloc(sizeof(UCHR)*file_size);
	if(file_buffer==NULL)
	{
		sprintf((char *)print_rec,MSG_MEMORY_ALLOC_ERROR,filename);
		log_error(1,print_rec);
		return(TTN_NOT_OK);
	}
	fread(file_buffer,file_size,1,fp);
	fclose(fp);
	
	jpeg_area.xPos=x;
	jpeg_area.yPos=y;
	jpeg_area.nWidth=xlen;	
	jpeg_area.nHeight=ylen;
	
	// -------- Process JPG start	
	jpeg_get(file_buffer,file_size,info);    //�õ�JPEG������
	
	if((jpeg_area.xPos<0)||
		(jpeg_area.yPos<0)||
		(jpeg_area.nWidth>info.nWidth)||
		(jpeg_area.nHeight>info.nHeight)||
		(jpeg_area.xPos+jpeg_area.nWidth>info.nWidth)||
		(jpeg_area.yPos+jpeg_area.nHeight>info.nHeight))
	{
		sprintf((char *)print_rec,MSG_FILE_RANGE_ERROR,filename);
		log_error(1,print_rec);
		return(TTN_NOT_OK);
	}
	
	
	if(jpeg_read(file_buffer,file_size,jpeg_area,&bit24_screen_buffer,bit24_buffer_len)!=JPG_OK)
	{
		if(file_buffer!=NULL)
			free(file_buffer);
		if(bit24_screen_buffer!=NULL)
			free(bit24_screen_buffer);
		bit24_screen_buffer=NULL;
		sprintf((char *)print_rec,MSG_FILE_DECODE_ERROR,filename);
		log_error(1,print_rec);
		return(TTN_NOT_OK);
	}
	
	bit24_to_BMP(bit24_screen_buffer,bmp_buffer,xlen,ylen);
	
	if(file_buffer!=NULL)
		free(file_buffer);
	if(bit24_screen_buffer!=NULL)
		free(bit24_screen_buffer);
	bit24_screen_buffer=NULL;
	
	return(TTN_OK);
}


void bit24_to_BMP(UCHR *bit24_screen_buffer,BMP *bmp_buffer,SLONG xlen,SLONG ylen)
{
	SLONG x,y;
	SLONG bit24_offset;
	UCHR r,g,b;
	UINT color;
	
	for(y=0;y<ylen;y++)
	{
		bit24_offset=y*xlen*3;
		for(x=0;x<xlen;x++)
		{
			b=bit24_screen_buffer[bit24_offset];
			g=bit24_screen_buffer[bit24_offset+1];
			r=bit24_screen_buffer[bit24_offset+2];
			color=rgb2hi(r,g,b);
			bmp_buffer->line[y][x]=color;
			bit24_offset+=3;
		}
	}
}


void bit24_to_partBMP(UCHR *bit24_screen_buffer,BMP *bmp_buffer,SLONG sx,SLONG sy,SLONG xlen,SLONG ylen,SLONG bit24_xlen,SLONG bit24_ylen)
{
	SLONG x,y;
	SLONG bit24_x;
	SLONG bit24_y;
	SLONG bit24_offset;
	UCHR r,g,b;
	UINT color;
	
	for(y=sy,bit24_y=0;y<sy+ylen;y++,bit24_y++)
	{
		bit24_offset=bit24_y*bit24_xlen*3;
		if(bit24_y>=bit24_ylen)return;
//		for(x=sx;x<sx+xlen;x++)
		for(bit24_x=0,x=sx;bit24_x<bit24_xlen;x++,bit24_x++)
		{
			b=bit24_screen_buffer[bit24_offset];
			g=bit24_screen_buffer[bit24_offset+1];
			r=bit24_screen_buffer[bit24_offset+2];
			color=rgb2hi(r,g,b);
			bmp_buffer->line[y][x]=color;
			bit24_offset+=3;
		}
	}
}



void display_system_message(UCHR *message)
{
	static SLONG x_off=0;
	static SLONG y_off=0;

	SLONG x,y,xl,yl;
	SLONG stack_idx;
	RECT rect;
	ULONG handle;
	SLONG command_len;

	
	if(is_emery_message_stack()==TTN_OK)
	{
		x_off=0;
		y_off=0;
	}
	
	xl=strlen((char *)message)*8+32;
	command_len=get_print_command_number(message);
	xl=xl-command_len*3*8;
	if(xl<16)xl=16;
		
	yl=16+32;
	x=SCREEN_WIDTH/2-xl/2;
	y=SCREEN_HEIGHT/2-yl/2;
	x=x+x_off;
	y=y+y_off;
	x_off+=16;
	y_off+=16;
	if((x_off>400)||(y_off>300))
	{
		x_off=0;
		y_off=0;
	}
	
	

	stack_idx=find_emery_message_idx();
	if(stack_idx<0)
	{
		display_error_message(message,1);
		return;
	}

	rect.left=x;
	rect.top=y;
	rect.right=rect.left+xl;
	rect.bottom=rect.top+yl;
	
	message_stack[stack_idx].x=x;
	message_stack[stack_idx].y=y;
	message_stack[stack_idx].xl=xl;
	message_stack[stack_idx].yl=yl;
	strcpy((char *)message_stack[stack_idx].message,(char *)message);
	handle=insert_window_base(rect,&draw_message_window,&exec_message_window,WINDOW_TYPE_NORMAL);
	if(handle>=0)
		{
			set_window_base_data(handle,NULL,stack_idx);
			message_stack[stack_idx].window_handle=handle;
			message_stack[stack_idx].active=true;
			message_stack[stack_idx].timer=system_control.system_timer;
		}
	
}


SLONG is_emery_message_stack(void)
{
	SLONG i;
	for(i=0;i<MAX_MESSAGE_STACK;i++)
	{
		if(message_stack[i].active)
			return(TTN_NOT_OK);
		
	}
	return(TTN_OK);
}


SLONG find_emery_message_idx(void)
{
	SLONG i;

	for(i=0;i<MAX_MESSAGE_STACK;i++)
	{
		if(!message_stack[i].active)
			return(i);
	}
	
	return(TTN_NOT_OK);
}


SLONG get_print_command_number(UCHR *text)
{
	SLONG i;
	SLONG ret_val;
	ret_val=0;
	for(i=0;i<(SLONG)strlen((char *)text);i++)
	{
		if(text[i]=='~')
			ret_val++;
	}
	return(ret_val);
}


void display_error_message(UCHR *message,bool wait_key_flag)
{
	SLONG x,y,xl,yl;
	SLONG main_pass;
	SLONG com_len;

	log_error(1,message);
	xl=strlen((char *)message)*8+32;
	com_len=get_print_command_number(message);
	xl=xl-com_len*3*8;
	if(xl<=16)xl=16;
	yl=16+32;
	x=SCREEN_WIDTH/2-xl/2;
	y=SCREEN_HEIGHT/2-yl/2;

	get_bitmap(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,screen_channel2,screen_buffer);
	
	alpha_put_bar(x,y,xl,yl,color_control.red,screen_buffer,128);
	put_scroll_box(x,y,x+xl,y+yl,color_control.red,color_control.yellow,screen_buffer);
	set_word_color(0,color_control.black);
	print16(x+16+1,y+16+1, message, PEST_PUT, screen_buffer);
	set_word_color(0,color_control.white);
	print16(x+16,y+16, message, PEST_PUT, screen_buffer);
	
	if(wait_key_flag)
	{
		system_idle_loop();
		
		system_control.mouse_key=MS_Dummy;
		system_control.key=NULL;
		system_control.data_key=NULL;
		reset_mouse_key();
		reset_key();
		main_pass=0;
		while(main_pass==0)
		{
			system_idle_loop();
			get_bitmap(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,screen_buffer,screen_channel2);

			
			alpha_put_bar(x,y,xl,yl,color_control.red,screen_buffer,128);
			put_scroll_box(x,y,x+xl,y+yl,color_control.red,color_control.yellow,screen_buffer);
			set_word_color(0,color_control.black);
			print16(x+16+1,y+16+1, message, PEST_PUT, screen_buffer);
			set_word_color(0,color_control.white);
			print16(x+16,y+16, message, PEST_PUT, screen_buffer);
			
			update_screen(screen_buffer);
			switch(system_control.mouse_key)
			{
			case MS_LUp:
			case MS_RUp:
				reset_mouse_key();
				system_control.mouse_key=MS_Dummy;
				main_pass=1;
				break;
			}
			switch(system_control.key)
			{
			case Esc: case Enter: case Blank:
				system_control.key=NULL;
				reset_key();
				main_pass=1;
				break;
			default:
				reset_key();
				break;
			}
		}
	}
	
	system_idle_loop();
}



void display_message(UCHR *message,bool wait_key_flag)
{
	SLONG x,y,xl,yl;
	SLONG main_pass;
	SLONG com_len;
	
	log_error(1,message);
	xl=strlen((char *)message)*8+32;
	com_len=get_print_command_number(message);
	xl=xl-com_len*3*8;
	if(xl<=16)xl=16;
	yl=16+32;
	x=SCREEN_WIDTH/2-xl/2;
	y=SCREEN_HEIGHT/2-yl/2;

	get_bitmap(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,screen_channel2,screen_buffer);
	
	alpha_put_bar(x,y,xl,yl,color_control.black,screen_buffer,128);
	put_box(x+2,y+2,xl-4,yl-4,color_control.white,screen_buffer);
	set_word_color(0,color_control.yellow);
	print16(x+16,y+16, message, PEST_PUT, screen_buffer);
	set_word_color(0,color_control.white);
	
	if(wait_key_flag)
	{
		reset_mouse_key();
		reset_key();
		main_pass=0;
		while(main_pass==0)
		{
			system_idle_loop();
			get_bitmap(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,screen_buffer,screen_channel2);

			alpha_put_bar(x,y,xl,yl,color_control.black,screen_buffer,128);
			put_box(x+2,y+2,xl-4,yl-4,color_control.white,screen_buffer);
			set_word_color(0,color_control.yellow);
			print16(x+16,y+16, message, PEST_PUT, screen_buffer);
			set_word_color(0,color_control.white);
			
			
			update_screen(screen_buffer);
			switch(system_control.mouse_key)
			{
			case MS_LUp:
			case MS_RUp:
				reset_mouse_key();
				system_control.mouse_key=MS_Dummy;
				main_pass=1;
				break;
			}
			switch(system_control.key)
			{
			case Esc: case Enter: case Blank:
				system_control.key=NULL;
				reset_key();
				main_pass=1;
				break;
			default:
				reset_key();
				break;
			}
		}
	}
	
	system_idle_loop();
	
}



/***************************************************************************************}
{ Ripple routine																		}
****************************************************************************************/
void init_ripple(void)
{
	
	EnergyFrame1=(short *)malloc(SCREEN_WIDTH*SCREEN_HEIGHT*2);
	if(EnergyFrame1==NULL)
	{
		log_error(1,MSG_MEMORY_ALLOC_ERROR);
		return;
	}
	EnergyFrame2=(short *)malloc(SCREEN_WIDTH*SCREEN_HEIGHT*2);
	if(EnergyFrame2==NULL)
	{
		log_error(1,MSG_MEMORY_ALLOC_ERROR);
		return;
	}
	memset(EnergyFrame1,0x00,SCREEN_WIDTH*SCREEN_HEIGHT*2);
	memset(EnergyFrame2,0x00,SCREEN_WIDTH*SCREEN_HEIGHT*2);
	
}

void reset_ripple(void)
{
	memset(EnergyFrame1,0x00,SCREEN_WIDTH*SCREEN_HEIGHT*2);
	memset(EnergyFrame2,0x00,SCREEN_WIDTH*SCREEN_HEIGHT*2);
	
}

void free_ripple(void)
{
	if(EnergyFrame1!=NULL)
		free(EnergyFrame1);
	if(EnergyFrame2!=NULL)
		free(EnergyFrame2);
	EnergyFrame1=NULL;
	EnergyFrame2=NULL;
}

void RippleSpread(void)
{
	int i;
	short *tmpFrame;
	short *tmpEnergy;
	
	
	tmpEnergy = EnergyFrame2+640;
	for(i=SCREEN_WIDTH; i<SCREEN_WIDTH*SCREEN_HEIGHT-SCREEN_WIDTH; i++)
	{
		// ------------------------------- Spread out
		EnergyFrame2[i] = (
            ((EnergyFrame1[i-1]+
			EnergyFrame1[i+1]+
            EnergyFrame1[i-SCREEN_WIDTH]+
            EnergyFrame1[i+SCREEN_WIDTH])>>1)) - EnergyFrame2[i];
		
		if(EnergyFrame2[i] < 0)
			EnergyFrame2[i] = 0;
		// ------------------------------- Energy damping
		//          EnergyFrame2[i] -= EnergyFrame2[i]>>5;
	}
	
	// --------------- Swap screen_channel1 and screen_channel2
	tmpFrame = EnergyFrame1;
	EnergyFrame1 = EnergyFrame2;
	EnergyFrame2 = tmpFrame;
}

UHINT *tmpScr;

void RippleRender(void)
{
	long xoff,yoff;
	long i,j,l;
	long k = SCREEN_WIDTH;
	
	
	tmpScr = screen_buffer->line[1];
	for(i=1;i<SCREEN_HEIGHT-1;i++)
	{
		for(j=0;j<SCREEN_WIDTH;j++)
		{
			xoff = (EnergyFrame1[k - 1] - EnergyFrame1[k + 1]);
			yoff = (EnergyFrame1[k-SCREEN_WIDTH] - EnergyFrame1[k+SCREEN_WIDTH]);
			
			l=j + xoff;
			if(l < 0 ||l >= SCREEN_WIDTH)
			{
				k++;
				tmpScr++;
				continue;
			}
			l=i+yoff;
			if(l<0||l>=SCREEN_HEIGHT)
			{
				k++;
				tmpScr++;
				continue;
			}
			k++;
			//			*tmpScr++=screen_channel3[j + xoff + ((i + yoff)<<7)+((i+yoff)<<9)];
			*tmpScr++=screen_buffer->line[i+yoff][j+xoff];
			
		}
	}
}


void push_ripple(SLONG x,SLONG y,SLONG value)
{
	if((x<2)||(y<2)||(x>=SCREEN_WIDTH-1)||(y>=SCREEN_HEIGHT-1))
		return;
	EnergyFrame1[y*SCREEN_WIDTH+x]=(short)value;
	EnergyFrame2[y*SCREEN_WIDTH+x]=(short)value;
	
}


/***************************************************************************************}
{ Music & Voice 																		}
****************************************************************************************/
SLONG auto_play_voice(SLONG file_type,UCHR *filename,UCHR *buffer,SLONG buffer_size,SLONG loop_flag,SLONG volume,SLONG pan,SLONG *channel)
{
	SLONG real_volume;
	SLONG temp_volume;
	SLONG find_channel;
	SLONG i;
	ULONG now_time;
	SLONG bs,bok,bsize;
	ULONG cc,ss,bps,size;
	ULONG tt;
	WAVEFORMATEX wfx;
	
	
	if(system_config.voice_flag==0)return(TTN_NOT_OK);
	temp_volume=volume+3000;
	real_volume=temp_volume*system_config.volume/100;
	real_volume=real_volume-3000;
	
	switch(file_type)
	{
	case 0:					// Play File
		tt=0;
		break;
	case 1:					// play buffer
		if((buffer==NULL)||(buffer_size<=0))return(TTN_NOT_OK);
		break;
	default:
		return(TTN_NOT_OK);
	}
	
	find_channel=-1;
	// ---------------- ��Ѱ�յ� channel -------------
	for(i=0;i<VOICE_CHANNEL_MAX;i++)
	{
		if(voice_control.busy[i]==0)
		{
			find_channel=i;
			i=VOICE_CHANNEL_MAX;
		}
	}
	if(find_channel==-1)	// no emtry channel
	{
		now_time=GetTickCount();
		for(i=0;i<VOICE_CHANNEL_MAX;i++)		// find playend channel 
		{
			if((now_time>=voice_control.play_time[i])&&
				(voice_control.loop[i]==0))
			{
				find_channel=i;
				voice_control.busy[i]=0;		// clear busy flag
				voice_control.loop[i]=0;
			}
		}
	}
	if(find_channel==-1)
	{
		for(i=0;i<VOICE_CHANNEL_MAX;i++)
		{
			if(voice_control.loop[i]==0)
			{
				find_channel=i;
				i=VOICE_CHANNEL_MAX;
			}
			
		}
		if(find_channel==-1)
			find_channel=rand()%VOICE_CHANNEL_MAX;
	}
	
	if(find_channel<0)return(TTN_NOT_OK);
	// ----------- find channel OK -----------
	*channel=find_channel;
	
	switch(file_type)
	{
	case 0:						// wave file
		play_voice( find_channel,loop_flag,filename);
		get_wavfile_information(filename,&cc,&ss,&bps,&size,&tt); 
		now_time=GetTickCount();
		voice_control.play_time[find_channel]=now_time+tt;
		break;
	case 1:						// wave buffer
		play_buffer_voice( find_channel,loop_flag,buffer,buffer_size);
		get_buffer_wav_format(buffer,&bs,&wfx,&bok,&bsize);	
		tt=bsize * 1000 / wfx.nAvgBytesPerSec;
		now_time=GetTickCount();
		voice_control.play_time[find_channel]=now_time+tt;
		break;
	default:
		break;
	}
	
	set_voice_volume(find_channel,real_volume);
	set_voice_pan(find_channel,pan);
	voice_control.busy[find_channel]=1;
	voice_control.loop[find_channel]=loop_flag;
	
	
	return(TTN_OK);
}

void auto_set_voice_control(SLONG channel,SLONG volume,SLONG pan)
{
	SLONG real_volume;
	SLONG temp_volume;
	
	if(system_config.voice_flag==0)return;
	temp_volume=volume+10000;
	real_volume=temp_volume*system_config.volume/100;
	real_volume=real_volume-10000;
	set_voice_volume(channel,real_volume);
	set_voice_pan(channel,pan);
	
}

SLONG play_voice_file(UCHR *filename,SLONG loop,SLONG volume,SLONG pan)
{
	SLONG play_channel;
	if(system_config.voice_flag==0)return(TTN_NOT_OK);
	play_channel=-1;
	auto_play_voice(0,filename,NULL,0,loop,volume,pan,&play_channel);
	return(play_channel);
}

void stop_all_voice(void)
{
	SLONG i;
	
	for(i=0;i<VOICE_CHANNEL_MAX;i++)
		auto_stop_voice(i);
	
}

void auto_stop_voice(SLONG channel)
{
	voice_control.busy[channel]=0;
	voice_control.loop[channel]=0;
	voice_control.play_time[channel]=0;
	stop_voice(channel);
}



SLONG play_music_file(UCHR *filename,SLONG loop,SLONG volume)
{
	SLONG real_volume;

	
	if(system_config.music_flag==0)
	{
		strcpy((char *)game_control.music_filename,(char *)filename);
		return(TTN_NOT_OK);
	}

	if(game_control.music_playing==1)		// ͬһ�׸�
	{
		if(strcmpi((char *)filename,(char *)game_control.music_filename)==0)
			return(TTN_OK);
	}

	mp3_loop_flag=0;
	game_control.music_playing=1;
	
	musicOpen((LPCTSTR)filename);
	musicPlay();
	
	game_control.music_loop_flag=loop;

	real_volume=volume*system_config.volume/100;
	auto_set_music_control(real_volume);
		
	strcpy((char *)game_control.music_filename,(char *)filename);
	return(TTN_OK);
	
}


void auto_set_music_control(SLONG volume)
{
	SLONG real_volume;
	
	if(system_config.music_flag==0)return;

	real_volume=volume*system_config.volume/100;
	setMusicVolume(real_volume);
	
}


void stop_music_file(void)
{
	musicStop();
	game_control.music_playing=0;
}


/***************************************************************************************}
{ MDA group	process																		}
****************************************************************************************/
SLONG mda_group_open(UCHR *filename,SLONG *handle)
{
	SLONG mda_group_index;
	FILE *fp;
	SLONG file_size;
	MDA_HEAD temp_mda_head;
	UCHR *data_buffer;
	
	
	*handle=-1;
	if(check_file_exist(filename)!=TTN_OK)
	{
		sprintf((char *)print_rec,MSG_FILE_OPEN_ERROR,filename);
		display_error_message(print_rec,true);
		return(TTN_NOT_OK);
	}
	if(check_is_mda_file(filename)!=TTN_OK)
	{
		sprintf((char *)print_rec,MSG_FILE_NOT_MDA_ERROR,filename);
		display_error_message(print_rec,true);
		return(TTN_NOT_OK);
	}
	
	// ---- alloc mda struct 
	mda_group_index=add_mda_group();
	if(mda_group_index<0)
	{
		sprintf((char *)print_rec,MSG_FILE_CREAT_ERROR,filename);
		display_error_message((UCHR *)print_rec,true);
		return(TTN_NOT_OK);
	}
	*handle=mda_group[mda_group_index].handle;
	strcpy((CHAR *)mda_group[mda_group_index].filename,(char *)filename);
	file_size=check_file_size(filename);
	fp=fopen((char *)filename,"rb");
	if(fp==NULL)
		return(TTN_NOT_OK);	
	mda_group[mda_group_index].body=(UCHR *)malloc(sizeof(UCHR)*file_size);
	if(mda_group[mda_group_index].body==NULL)
	{
		display_error_message((UCHR *)MSG_MEMORY_ALLOC_ERROR,true);
		fclose(fp);
		return(TTN_NOT_OK);
	}
	fread(mda_group[mda_group_index].body,1,file_size,fp);
	fclose(fp);
	memcpy(&temp_mda_head,mda_group[mda_group_index].body,sizeof(struct MDA_HEAD_STRUCT));
	mda_group[mda_group_index].total_database=temp_mda_head.total_database_frame;
	data_buffer=&mda_group[mda_group_index].body[temp_mda_head.ani_start_offset];
	mda_group[mda_group_index].total_ani_command=get_total_ani_command(data_buffer,mda_group_index);
	mda_group[mda_group_index].database_start_offset=temp_mda_head.data_start_offset;
	mda_group[mda_group_index].command_start_offset=temp_mda_head.ani_start_offset;
	
	// init frame parameter
	mda_group[mda_group_index].img256_head=NULL;
	
	// ---- Decode database index
	if(temp_mda_head.total_database_frame>0)
	{
		mda_group[mda_group_index].database_index=
			(struct INDEX_OFFSET_SIZE_STRUCT *)malloc(sizeof(struct INDEX_OFFSET_SIZE_STRUCT)*temp_mda_head.total_database_frame);
		if(mda_group[mda_group_index].database_index==NULL)
		{
			display_error_message((UCHR *)MSG_MEMORY_ALLOC_ERROR,true);
			return(TTN_NOT_OK);
		}
		memcpy(mda_group[mda_group_index].database_index,&mda_group[mda_group_index].body[sizeof(struct MDA_HEAD_STRUCT)],
			sizeof(struct INDEX_OFFSET_SIZE_STRUCT)*temp_mda_head.total_database_frame);
	}
	
	// ---- Decode ani command
	if(temp_mda_head.total_ani_frame>0)
	{
		data_buffer=&mda_group[mda_group_index].body[temp_mda_head.ani_start_offset];
		decode_ani_command(data_buffer,mda_group_index);
	}
	

	return(TTN_OK);
}


void mda_group_close(SLONG *handle)
{
	SLONG mda_group_index;
	SLONG new_total_mda_group;
	SLONG i,j;
	MDA_GROUP *temp_mda_group=NULL;
	
	
	if(total_mda_group==1)
	{
		free_mda_group();
		total_mda_group=0;
		*handle=-1;				
		return;
	}
	// ---- get mda_group_index
	mda_group_index=get_mda_index(*handle);
	if(mda_group_index<0)return;				// already close
	
	// ---- 
	new_total_mda_group=total_mda_group-1;
	temp_mda_group=(MDA_GROUP *)malloc(sizeof(struct MDA_GROUP_STRUCT)*new_total_mda_group);
	if(temp_mda_group==NULL)
	{
		display_error_message((UCHR *)MSG_MEMORY_ALLOC_ERROR,true);
		return;
	}
	
	// -------- Copy to temp
	for(i=0,j=0;i<total_mda_group;i++)
	{
		if(i==(SLONG)mda_group_index)
		{
			if(mda_group[i].body!=NULL)
				free(mda_group[i].body);
			mda_group[i].body=NULL;
			if(mda_group[i].now_frame!=NULL)
				free(mda_group[i].now_frame);
			mda_group[i].now_frame=NULL;
			if(mda_group[i].total_frame!=NULL)
				free(mda_group[i].total_frame);
			mda_group[i].total_frame=NULL;
			if(mda_group[i].database_index!=NULL)
				free(mda_group[i].database_index);
			mda_group[i].database_index=NULL;
// ---
			if(mda_group[i].main_command_text!=NULL)
				free(mda_group[i].main_command_text);
			mda_group[i].main_command_text=NULL;

			if(mda_group[i].database_command_text!=NULL)
				free(mda_group[i].database_command_text);
			mda_group[i].database_command_text=NULL;

			if(mda_group[i].effect_command_text!=NULL)
				free(mda_group[i].effect_command_text);
			mda_group[i].effect_command_text=NULL;

			continue;
		}
		memcpy(&temp_mda_group[j],&mda_group[i],sizeof(struct MDA_GROUP_STRUCT));
		j++;
	}
	
	// ---- Free old mda_group
	if(mda_group!=NULL)
		free(mda_group);
	
	// ---- Copy temp to mda_group
	total_mda_group=new_total_mda_group;
	mda_group=(MDA_GROUP *)malloc(sizeof(struct MDA_GROUP_STRUCT)*total_mda_group);
	if(mda_group==NULL)
	{
		display_error_message((UCHR *)MSG_MEMORY_ALLOC_ERROR,true);
		return;
	}
	for(i=0;i<total_mda_group;i++)
		memcpy(&mda_group[i],&temp_mda_group[i],sizeof(struct MDA_GROUP_STRUCT));
	
	free(temp_mda_group);
	*handle=-1;				
}


SLONG get_mda_index(ULONG mda_handle)
{
	SLONG i;
	
	for(i=0;i<total_mda_group;i++)
	{
		if(mda_group[i].handle==mda_handle)
			return(i);
	}
	return(TTN_NOT_OK);
}


SLONG get_mda_handle(UCHR *filename)
{
	SLONG i;
	for(i=0;i<total_mda_group;i++)
	{
		if(strcmpi((char *)mda_group[i].filename,(char *)filename)==NULL)
			return(mda_group[i].handle);
	}
	return(TTN_NOT_OK);
	
}


void free_mda_group(void)
{
	SLONG i;
	
	if(mda_group==NULL)return;
	for(i=0;i<total_mda_group;i++)
	{
		if(mda_group[i].body!=NULL)
		{
			free(mda_group[i].body);
			mda_group[i].body=NULL;
		}
		if(mda_group[i].database_index!=NULL)
		{
			free(mda_group[i].database_index);
			mda_group[i].database_index=NULL;
		}
		if(mda_group[i].now_frame!=NULL)
			free(mda_group[i].now_frame);
		mda_group[i].now_frame=NULL;
		if(mda_group[i].total_frame!=NULL)
			free(mda_group[i].total_frame);
		mda_group[i].total_frame=NULL;

		if(mda_group[i].main_command_text!=NULL)
			free(mda_group[i].main_command_text);
		mda_group[i].main_command_text=NULL;

		if(mda_group[i].database_command_text!=NULL)
			free(mda_group[i].database_command_text);
		mda_group[i].database_command_text=NULL;

		if(mda_group[i].effect_command_text!=NULL)
			free(mda_group[i].effect_command_text);
		mda_group[i].effect_command_text=NULL;

	}
	free(mda_group);
	total_mda_group=0;
	mda_group=NULL;
}

SLONG check_is_mda_file(UCHR *filename)
{
	FILE *fp;
	MDA_HEAD temp_mda_head;
	
	fp=fopen((char *)filename,"rb");
	if(fp==NULL)
		return(TTN_NOT_OK);
	fread(&temp_mda_head,sizeof(struct MDA_HEAD_STRUCT),1,fp);
	fclose(fp);
	if(strcmpi((char *)temp_mda_head.copyright,"RAYS MEDIA FILE")!=0)
		return(TTN_NOT_OK);
	return(TTN_OK);
}


SLONG add_mda_group(void)
{
	MDA_GROUP *temp_mda_group=NULL;
	SLONG new_total_mda_group;
	SLONG i;
	SLONG new_handle;
	
	// ---- alloc new buffer
	new_total_mda_group=total_mda_group+1;
	new_handle=total_mda_group;
	temp_mda_group=(MDA_GROUP *)malloc(sizeof(MDA_GROUP_STRUCT)*new_total_mda_group);
	if(temp_mda_group==NULL)
	{
		display_error_message((UCHR *)MSG_MEMORY_ALLOC_ERROR,true);
		return(TTN_NOT_OK);
	}
	
	// ---- Copy to temp
	for(i=0;i<total_mda_group;i++)
		memcpy(&temp_mda_group[i],&mda_group[i],sizeof(struct MDA_GROUP_STRUCT));
	
	// ---- Free old mda_group
	if(mda_group!=NULL)
		free(mda_group);
	
	// ---- Copy temp to mda_group
	total_mda_group=new_total_mda_group;
	mda_group=(MDA_GROUP *)malloc(sizeof(MDA_GROUP_STRUCT)*total_mda_group);
	if(mda_group==NULL)
	{
		display_error_message((UCHR *)MSG_MEMORY_ALLOC_ERROR,true);
		return(TTN_NOT_OK);
	}
	for(i=0;i<total_mda_group;i++)
		memcpy(&mda_group[i],&temp_mda_group[i],sizeof(struct MDA_GROUP_STRUCT));
	
	free(temp_mda_group);
	// ---- Initial new mda_group parameter
	mda_group[new_handle].body=NULL;
	mda_group[new_handle].handle=mda_handle_count;
	mda_group[new_handle].database_index=NULL;
	mda_group[new_handle].now_frame=NULL;
	mda_group[new_handle].total_frame=NULL;
	mda_group[new_handle].total_main_text=0;
	mda_group[new_handle].main_command_text=NULL;
	mda_group[new_handle].total_database_text=0;
	mda_group[new_handle].database_command_text=NULL;
	mda_group[new_handle].total_effect_text=0;
	mda_group[new_handle].effect_command_text=NULL;
	mda_handle_count=find_next_mda_handle();
	return(new_handle);
}


SLONG get_total_ani_command(UCHR *body,SLONG mda_group_index)
{
	SLONG body_size;
	UCHR *ani_buffer;
	SLONG decode_index;
	SLONG main_pass;
	SLONG command_level,command_length;
	SLONG total_ani_command;
	
	body_size=*(SLONG *)&body[0];
	ani_buffer=&body[4];
	
	total_ani_command=0;
	decode_index=0;			
	main_pass=0;
	while(main_pass==0)
	{
		command_level=(SLONG)ani_buffer[decode_index];
		command_length=(SLONG)ani_buffer[decode_index+1];
		switch(command_level)
		{
		case ANI_MAIN_COMMAND:					// Main Command
			total_ani_command++;
			break;
		}
		decode_index=decode_index+command_length;
		if(decode_index>=body_size)
			main_pass=1;
	}
	return(total_ani_command);
}


void decode_ani_command(UCHR *body,SLONG mda_group_index)
{
	SLONG body_size;
	UCHR *ani_buffer;
	SLONG decode_index;
	SLONG main_pass;
	SLONG command_level,command_length;
	SLONG main_command_index;
//	UCHR no_text[255];
	UCHR command_text[255];
// 	UCHR main_command[MAX_COMMAND_LENGTH];
//	UCHR database_command[MAX_COMMAND_LENGTH];
//	UCHR effect_command[MAX_COMMAND_LENGTH];
	SLONG total_main_command_text=0;
	SLONG total_database_command_text=0;
	SLONG total_effect_command_text=0;


	SLONG total_ani_command;
	SLONG total_database_frame;
	SLONG i;
	
	total_ani_command=mda_group[mda_group_index].total_ani_command;
	if(total_ani_command<=0)return;
	
	// ---- Alloc memory 
	mda_group[mda_group_index].total_frame=(SLONG *)malloc(sizeof(SLONG)*total_ani_command);
	mda_group[mda_group_index].now_frame=(SLONG *)malloc(sizeof(SLONG)*total_ani_command);
	if((mda_group[mda_group_index].total_frame==NULL)||
		(mda_group[mda_group_index].now_frame==NULL))
	{
		display_error_message((UCHR *)MSG_MEMORY_ALLOC_ERROR,true);
		return;
	}
	// ----- Reset now frame
	for(i=0;i<total_ani_command;i++)
	{
		mda_group[mda_group_index].now_frame[i]=0;
		mda_group[mda_group_index].total_frame[i]=0;
	}
	
	body_size=*(SLONG *)&body[0];
	ani_buffer=&body[4];

// --- ( 0 - 0 )  ���� Total

	total_main_command_text=0;
	total_database_command_text=0;
	total_effect_command_text=0;
	
	total_database_frame=0;
	main_command_index=-1;
	decode_index=0;			
	main_pass=0;
	while(main_pass==0)
	{
		command_level=(SLONG)ani_buffer[decode_index];
		command_length=(SLONG)ani_buffer[decode_index+1];
		memcpy(command_text,&ani_buffer[decode_index+2],command_length-2);
		switch(command_level)
		{
		case ANI_MAIN_COMMAND:					// Main Command
			if(main_command_index>=0)
			{
				mda_group[mda_group_index].total_frame[main_command_index]=
					total_database_frame;
			}
			total_database_frame=0;
			main_command_index++;

			total_main_command_text++;			// Count ++

//			sprintf((char *)no_text,"(%d)",main_command_index);
//			strcat((char *)main_command,(char *)no_text);
//			strcat((char *)main_command,(char *)command_text);
//			strcat((char *)main_command,";");
			break;
		case ANI_DATABASE:                      // DataBase Command
//			sprintf((char *)no_text,"(%d)",main_command_index);
//			strcat((char *)database_command,(char *)no_text);
//			strcat((char *)database_command,(char *)command_text);
//			strcat((char *)database_command,";");
			total_database_frame++;

			total_database_command_text++;		// Count ++

			break;
		case ANI_EFFECT1:                       // Effect Command
		case ANI_EFFECT2:                       // Effect Command
//			sprintf((char *)no_text,"(%d)",main_command_index*10000+total_database_frame-1);
//			strcat((char *)effect_command,(char *)no_text);
//			strcat((char *)effect_command,(char *)command_text);
//			strcat((char *)effect_command,";");

			total_effect_command_text++;
			break;
		}
		decode_index=decode_index+command_length;
		if(decode_index>=body_size)
			main_pass=1;
		
	}
	
// --- ( 0 - 1 ) ���� memory 
	if(mda_group[mda_group_index].main_command_text!=NULL)
	{
		free(mda_group[mda_group_index].main_command_text);
		mda_group[mda_group_index].main_command_text=NULL;
	}

	if(mda_group[mda_group_index].database_command_text!=NULL)
	{
		free(mda_group[mda_group_index].database_command_text);
		mda_group[mda_group_index].database_command_text=NULL;
	}

	if(mda_group[mda_group_index].effect_command_text!=NULL)
	{
		free(mda_group[mda_group_index].effect_command_text);
		mda_group[mda_group_index].effect_command_text=NULL;
	}

	mda_group[mda_group_index].total_main_text=total_main_command_text;
	mda_group[mda_group_index].total_database_text=total_database_command_text;
	mda_group[mda_group_index].total_effect_text=total_effect_command_text;

	if (total_main_command_text)
	{
		mda_group[mda_group_index].main_command_text=(struct MDA_STRING_STRUCT *)malloc(sizeof (struct MDA_STRING_STRUCT)*total_main_command_text);
		
		if(mda_group[mda_group_index].main_command_text==NULL)
		{
			display_error_message((UCHR*)MSG_MEMORY_ALLOC_ERROR,1);
			return;
		}
	}

	if (total_database_command_text)
	{
		mda_group[mda_group_index].database_command_text=(struct MDA_STRING_STRUCT *)malloc(sizeof (struct MDA_STRING_STRUCT)*total_database_command_text);
		
		if(mda_group[mda_group_index].database_command_text==NULL)
		{
			display_error_message((UCHR *)MSG_MEMORY_ALLOC_ERROR,1);
			return;
		}
	}

	if (total_effect_command_text)
	{
		mda_group[mda_group_index].effect_command_text=(struct MDA_STRING_STRUCT *)malloc(sizeof (struct MDA_STRING_STRUCT)*total_effect_command_text);

		if(mda_group[mda_group_index].effect_command_text==NULL)
		{
			display_error_message((UCHR *)MSG_MEMORY_ALLOC_ERROR,1);
			return;
		}		
	}	

// --- ( 1 - 0 ) Get data
	total_main_command_text=0;
	total_database_command_text=0;
	total_effect_command_text=0;
	
	
	total_database_frame=0;
	main_command_index=-1;
	decode_index=0;			
	main_pass=0;
	while(main_pass==0)
	{
		command_level=(SLONG)ani_buffer[decode_index];
		command_length=(SLONG)ani_buffer[decode_index+1];
		memcpy(command_text,&ani_buffer[decode_index+2],command_length-2);
		switch(command_level)
		{
		case ANI_MAIN_COMMAND:					// Main Command
			if(main_command_index>=0)
			{
				mda_group[mda_group_index].total_frame[main_command_index]=
					total_database_frame;
			}
			total_database_frame=0;
			main_command_index++;
		
			mda_group[mda_group_index].main_command_text[total_main_command_text].idx=main_command_index;

			if(strlen((char *)command_text)>=MAX_MDA_STRING-1)
				display_error_message((UCHR *)MSG_MDA_ERROR,1);
			else
				strcpy((char *)mda_group[mda_group_index].main_command_text[total_main_command_text].text,(char *)command_text);

			total_main_command_text++;			// Count ++
			
			
//			sprintf((char *)no_text,"(%d)",main_command_index);
//			strcat((char *)main_command,(char *)no_text);
//			strcat((char *)main_command,(char *)command_text);
//			strcat((char *)main_command,";");
			break;
		case ANI_DATABASE:                      // DataBase Command

			mda_group[mda_group_index].database_command_text[total_database_command_text].idx=main_command_index;

			if(strlen((char *)command_text)>=MAX_MDA_STRING-1)
				display_error_message((UCHR *)MSG_MDA_ERROR,1);
			else	
				strcpy((char *)mda_group[mda_group_index].database_command_text[total_database_command_text].text,(char *)command_text);
			
//			sprintf((char *)no_text,"(%d)",main_command_index);
//			strcat((char *)database_command,(char *)no_text);
//			strcat((char *)database_command,(char *)command_text);
//			strcat((char *)database_command,";");

			total_database_command_text++;		// Count ++

			total_database_frame++;
			break;
		case ANI_EFFECT1:                       // Effect Command
		case ANI_EFFECT2:                       // Effect Command

			mda_group[mda_group_index].effect_command_text[total_effect_command_text].idx=main_command_index*10000+total_database_frame-1;
			if(strlen((char *)command_text)>=MAX_MDA_STRING-1)
				display_error_message((UCHR *)MSG_MDA_ERROR,1);
			else				
				strcpy((char *)mda_group[mda_group_index].effect_command_text[total_effect_command_text].text,(char *)command_text);
			
//			sprintf((char *)no_text,"(%d)",main_command_index*10000+total_database_frame-1);
//			strcat((char *)effect_command,(char *)no_text);
//			strcat((char *)effect_command,(char *)command_text);
//			strcat((char *)effect_command,";");

			total_effect_command_text++;
			
			break;
		}
		decode_index=decode_index+command_length;
		if(decode_index>=body_size)
			main_pass=1;

	}

//	strcpy((char *)mda_group[mda_group_index].main_command_text,(char *)main_command);
//	strcpy((char *)mda_group[mda_group_index].database_command_text,(char *)database_command);
//	strcpy((char *)mda_group[mda_group_index].effect_command_text,(char *)effect_command);

	mda_group[mda_group_index].total_frame[main_command_index]=
		total_database_frame;
	
}


ULONG find_next_mda_handle(void)
{
	SLONG i;
	ULONG find_handle;
	SLONG main_pass=0;
	bool find;
	
	find_handle=total_mda_group;
	while(main_pass==0)
	{
		find=false;
		for(i=0;i<total_mda_group;i++)
		{
			if(mda_group[i].handle==find_handle)
			{
				find=true;
				i=total_mda_group;
			}
		}
		if(find==false)
		{	
			return(find_handle);
		}
		else
		{
			find_handle++;
			if(find_handle>1000000)
				find_handle=0;
		}
	}
	return(TTN_NOT_OK);
}


void get_img256_rect(UCHR *buffer,RECT *rect)
{
	IMG256_HEAD temp_img256_head;
	
	memcpy(&temp_img256_head,buffer,sizeof(struct IMG256_HEAD_STRUCT));
	rect->left=temp_img256_head.orig_x;
	rect->top=temp_img256_head.orig_y;
	rect->right=temp_img256_head.orig_x+temp_img256_head.xlen;
	rect->bottom=temp_img256_head.orig_y+temp_img256_head.ylen;
}


SLONG read_mda_frame(UCHR *mda_filename,SLONG frame_no,UCHR **buffer)
{
	FILE *fp;
	MDA_HEAD temp_mda_head;
	INDEX_OFFSET_SIZE *index=NULL;
	SLONG offset,size;
	
	if(check_file_exist(mda_filename)!=TTN_OK)
	{
		sprintf((char *)print_rec,MSG_FILE_OPEN_ERROR,mda_filename);
		display_error_message(print_rec,true);
		return(TTN_NOT_OK);
	}
	if(check_is_mda_file(mda_filename)!=TTN_OK)
	{
		sprintf((char *)print_rec,MSG_FILE_CHECK_ERROR,mda_filename);
		display_error_message(print_rec,true);
		return(TTN_NOT_OK);
	}
	
	fp=fopen((char *)mda_filename,"rb");
	if(fp==NULL)
		return(TTN_NOT_OK);
	fread(&temp_mda_head,sizeof(struct MDA_HEAD_STRUCT),1,fp);
	if((temp_mda_head.total_database_frame<=0)||
		(frame_no>=temp_mda_head.total_database_frame))
	{
		sprintf((char *)print_rec,"Read mda frame no error (%d/%d)",frame_no,temp_mda_head.total_database_frame);
		display_error_message(print_rec,true);
		fclose(fp);
		return(TTN_NOT_OK);
	}
	fseek(fp,temp_mda_head.data_start_offset,SEEK_SET);
	index=(struct INDEX_OFFSET_SIZE_STRUCT *)malloc(sizeof(struct INDEX_OFFSET_SIZE_STRUCT)*temp_mda_head.total_database_frame);
	if(index==NULL)
	{
		display_error_message((UCHR *)MSG_MEMORY_ALLOC_ERROR,true);
		fclose(fp);
		return(TTN_NOT_OK);
	}
	fread(index,sizeof( struct INDEX_OFFSET_SIZE_STRUCT)*temp_mda_head.total_database_frame,1,fp);
	offset=index[frame_no].offset;
	size=index[frame_no].size;
	fseek(fp,offset,SEEK_SET);
	*buffer=(UCHR *)malloc(sizeof(UCHR)*size);
	if(*buffer==NULL)
	{
		display_error_message((UCHR *)MSG_MEMORY_ALLOC_ERROR,true);
		free(index);
		fclose(fp);
		return(TTN_NOT_OK);
	}
	fread(*buffer,sizeof(UCHR)*size,1,fp);
	fclose(fp);
	free(index);
	return(TTN_OK);
}



SLONG read_mda_database(UCHR *mda_filename,SLONG frame_no,UCHR **buffer,ULONG *ret_size)
{
	FILE *fp;
	MDA_HEAD temp_mda_head;
	INDEX_OFFSET_SIZE *index=NULL;
	SLONG offset,size;
	
	if(check_file_exist(mda_filename)!=TTN_OK)
	{
		sprintf((char *)print_rec,MSG_FILE_OPEN_ERROR,mda_filename);
		display_error_message(print_rec,true);
		return(TTN_NOT_OK);
	}
	if(check_is_mda_file(mda_filename)!=TTN_OK)
	{
		sprintf((char *)print_rec,MSG_FILE_CHECK_ERROR,mda_filename);
		display_error_message(print_rec,true);
		return(TTN_NOT_OK);
	}
	
	fp=fopen((char *)mda_filename,"rb");
	if(fp==NULL)
		return(TTN_NOT_OK);
	fread(&temp_mda_head,sizeof(struct MDA_HEAD_STRUCT),1,fp);
	if((temp_mda_head.total_database_frame<=0)||
		(frame_no>=temp_mda_head.total_database_frame))
	{
		sprintf((char *)print_rec,"Read mda frame no error (%d/%d)",frame_no,temp_mda_head.total_database_frame);
		display_error_message(print_rec,true);
		fclose(fp);
		return(TTN_NOT_OK);
	}
	fseek(fp,temp_mda_head.data_start_offset,SEEK_SET);
	index=(struct INDEX_OFFSET_SIZE_STRUCT *)malloc(sizeof(struct INDEX_OFFSET_SIZE_STRUCT)*temp_mda_head.total_database_frame);
	if(index==NULL)
	{
		display_error_message((UCHR *)MSG_MEMORY_ALLOC_ERROR,true);
		fclose(fp);
		return(TTN_NOT_OK);
	}
	fread(index,sizeof( struct INDEX_OFFSET_SIZE_STRUCT)*temp_mda_head.total_database_frame,1,fp);
	offset=index[frame_no].offset;
	size=index[frame_no].size;
	fseek(fp,offset,SEEK_SET);
	*buffer=(UCHR *)malloc(sizeof(UCHR)*size);
	if(*buffer==NULL)
	{
		display_error_message((UCHR *)MSG_MEMORY_ALLOC_ERROR,true);
		free(index);
		fclose(fp);
		return(TTN_NOT_OK);
	}
	fread(*buffer,sizeof(UCHR)*size,1,fp);
	*ret_size=(ULONG)size;
	fclose(fp);
	free(index);
	return(TTN_OK);
}


void get_mda_group_image(SLONG mda_index,SLONG index,UCHR **image)
{
	MDA_GROUP *data;
	SLONG ii;

	*image=NULL;

	if((mda_index<0)||(mda_index>=total_mda_group))return;
	data=&mda_group[mda_index];
	if((index<0)||(index>=data->total_database))
		return;

	ii=data->database_index[index].offset;

	*image=&data->body[ii];

}


/***************************************************************************************}
{ MDA group	Exec																		}
****************************************************************************************/
SLONG mda_group_exec(SLONG handle,UCHR *command,SLONG x,SLONG y,BMP *bitmap,SLONG mda_command,SLONG value,SLONG alpha_value,SLONG frame_value)
{
	SLONG mda_group_index;
	SLONG command_no;
	SLONG database_no;
	SLONG now_frame;
	UCHR database_text[255];
	UCHR effect_text[255];
	SLONG offset,size;
	SLONG media_type;
	UCHR *data_buffer=NULL;
	SLONG ret_val;
	SLONG i;
	SLONG voice_channel;
	UCHR backup_palette[768];
	bool zoom_flag=false;
	bool next_frame_flag=false;
	bool first_frame_flag=false;
	bool last_frame_flag=false;
	bool loop_frame_flag=false;
	bool alpha_flag=false;
	bool hight_light_flag=false;
	bool assign_flag=false;
	bool palette_flag=false;
	bool palette_or_flag=false;
	bool palette_and_flag=false;
	bool palette_sub_flag=false;
	bool palette_add_flag=false;
	bool palette_gray=false;
	
	if(handle<0)return(MDA_EXEC_ERROR);
	goto_frame=-1;
	
	// ---- decode mda command
	if(mda_command&MDA_COMMAND_ZOOM)
		zoom_flag=true;
	if(mda_command&MDA_COMMAND_NEXT_FRAME)
		next_frame_flag=true;
	if(mda_command&MDA_COMMAND_FIRST)
		first_frame_flag=true;
	if(mda_command&MDA_COMMAND_LAST)
		last_frame_flag=true;
	if(mda_command&MDA_COMMAND_LOOP)
		loop_frame_flag=true;
	if(mda_command&MDA_COMMAND_ALPHA)
		alpha_flag=true;
	if(mda_command&MDA_COMMAND_HIGHTLIGHT)
		hight_light_flag=true;
	if(mda_command&MDA_COMMAND_ASSIGN)
		assign_flag=true;
	if(mda_command&MDA_COMMAND_PALETTE)
		palette_flag=true;
	if(mda_command&MDA_COMMAND_PALETTE_OR)
		palette_or_flag=true;
	if(mda_command&MDA_COMMAND_PALETTE_AND)
		palette_and_flag=true;
	if(mda_command&MDA_COMMAND_PALETTE_SUB)
		palette_sub_flag=true;
	if(mda_command&MDA_COMMAND_PALETTE_ADD)
		palette_add_flag=true;
	if(mda_command&MDA_COMMAND_PALETTE_GRAY)
		palette_gray=true;
	
	// ---- Get mda_group index
	mda_group_index=get_mda_index(handle);
	if(mda_group_index<0)return(MDA_EXEC_ERROR);
	// ---- OK ! next get 
	command_no=decode_command_str(command,mda_group_index);
	if(command_no<0)
	{
		sprintf((char *)print_rec,MSG_MDA_COMMAND_ERROR,command);
		display_error_message(print_rec,true);
		return(MDA_EXEC_ERROR);
	}
	
	if(first_frame_flag)
		mda_group[mda_group_index].now_frame[command_no]=0;
	if(last_frame_flag)
		mda_group[mda_group_index].now_frame[command_no]=mda_group[mda_group_index].total_frame[command_no]-1;
	if(assign_flag)
	{
//		mda_group[mda_group_index].now_frame[command_no]=frame_value;
		now_frame=frame_value;
	}
	else
	{

		now_frame=mda_group[mda_group_index].now_frame[command_no];
	}
	
	
	if(decode_database_str(command_no,now_frame,mda_group_index,database_text)!=TTN_OK)
	{
        sprintf((char *)print_rec,MSG_MDA_DATABASE_FRAME_ERROR,now_frame);
		display_error_message(print_rec,true);
		return(MDA_EXEC_ERROR);
	}
	
	database_no=find_database(mda_group_index,database_text);
	if(database_no<0)			// It's a user command
	{
		exec_user_command(database_text,data_buffer);
	}
	else
	{
		offset=mda_group[mda_group_index].database_index[database_no].offset;
		size=mda_group[mda_group_index].database_index[database_no].size;
		
		data_buffer=&mda_group[mda_group_index].body[offset];
		media_type=check_media_type(data_buffer);
		switch(media_type)
		{
		case FILE_TYPE_UNKNOW:
			exec_user_command(database_text,data_buffer);
			break;
		case FILE_TYPE_WAV:
			auto_play_voice(1,(UCHR *)"",data_buffer,size,0,0,0,&voice_channel);
			break;
		case FILE_TYPE_IMG:
			mda_group[mda_group_index].img256_head=(struct IMG256_HEAD_STRUCT *)data_buffer;
			if((palette_or_flag)||(palette_flag)||(palette_and_flag)||(palette_sub_flag)||(palette_add_flag)||(palette_gray))
			{
				memcpy(backup_palette,mda_group[mda_group_index].img256_head->palette,768);
				for(i=0;i<768;i++)
				{
					if(palette_flag)
						mda_group[mda_group_index].img256_head->palette[i]=system_palette[i];
					if(palette_or_flag)
						mda_group[mda_group_index].img256_head->palette[i]=mda_group[mda_group_index].img256_head->palette[i]|system_palette[i];
					if(palette_and_flag)
						mda_group[mda_group_index].img256_head->palette[i]=mda_group[mda_group_index].img256_head->palette[i]&system_palette[i];
					if(palette_sub_flag)
						mda_group[mda_group_index].img256_head->palette[i]=mda_group[mda_group_index].img256_head->palette[i]*system_palette[i]/100;
					if(palette_add_flag)
					{
						if((mda_group[mda_group_index].img256_head->palette[i]+mda_group[mda_group_index].img256_head->palette[i]*system_palette[i]/100)>=255)
							mda_group[mda_group_index].img256_head->palette[i]=255;
						else
							mda_group[mda_group_index].img256_head->palette[i]=mda_group[mda_group_index].img256_head->palette[i]+mda_group[mda_group_index].img256_head->palette[i]*system_palette[i]/100;
					}
				}

				if(palette_gray)
				{
					for(i=0;i<256;i++)
					{
						mda_group[mda_group_index].img256_head->palette[i*3+1]=mda_group[mda_group_index].img256_head->palette[i*3];
						mda_group[mda_group_index].img256_head->palette[i*3+2]=mda_group[mda_group_index].img256_head->palette[i*3];
					}
				}
				
			}
			
			if(hight_light_flag)
			{
				if(zoom_flag)
					display_hightlight_zoom_img256_buffer(x,y,data_buffer,bitmap,value);
				else
					display_hightlight_img256_buffer(x,y,data_buffer,bitmap);
			}
			else
			{
				if(alpha_flag)
				{
					if(zoom_flag)
						display_zoom_img256_buffer_alpha(x,y,data_buffer,bitmap,value,alpha_value);
					else
						display_img256_buffer_alpha(x,y,data_buffer,bitmap,alpha_value);
				}
				else
				{
					if(zoom_flag)
						display_zoom_img256_buffer(x,y,data_buffer,bitmap,value);
					else
						display_img256_buffer(x,y,data_buffer,bitmap);
				}
			}
			break;
		}
	}
	
	
	if(decode_effect_str(command_no,now_frame,mda_group_index,effect_text)==TTN_OK)
	{
		database_no=find_database(mda_group_index,effect_text);
		if(database_no<0)			// It's a user command
		{
			exec_user_command(effect_text,data_buffer);
		}
		else
		{
			offset=mda_group[mda_group_index].database_index[database_no].offset;
			size=mda_group[mda_group_index].database_index[database_no].size;
			
			data_buffer=&mda_group[mda_group_index].body[offset];
			media_type=check_media_type(data_buffer);
			switch(media_type)
			{
			case FILE_TYPE_UNKNOW:
				exec_user_command(database_text,data_buffer);
				break;
			case FILE_TYPE_WAV:
				auto_play_voice(1,(UCHR *)"",data_buffer,size,0,0,0,&voice_channel);
				break;
			case FILE_TYPE_IMG:
				mda_group[mda_group_index].img256_head=(struct IMG256_HEAD_STRUCT *)data_buffer;
				if((palette_or_flag)||(palette_flag)||(palette_and_flag)||(palette_sub_flag)||(palette_add_flag)||(palette_gray))
				{
					memcpy(backup_palette,mda_group[mda_group_index].img256_head->palette,768);
					for(i=0;i<768;i++)
					{
						if(palette_flag)
							mda_group[mda_group_index].img256_head->palette[i]=system_palette[i];
						if(palette_or_flag)
							mda_group[mda_group_index].img256_head->palette[i]=mda_group[mda_group_index].img256_head->palette[i]|system_palette[i];
						if(palette_and_flag)
							mda_group[mda_group_index].img256_head->palette[i]=mda_group[mda_group_index].img256_head->palette[i]&system_palette[i];
						if(palette_sub_flag)
							mda_group[mda_group_index].img256_head->palette[i]=mda_group[mda_group_index].img256_head->palette[i]*system_palette[i]/100;
						if(palette_add_flag)
						{
							if((mda_group[mda_group_index].img256_head->palette[i]+mda_group[mda_group_index].img256_head->palette[i]*system_palette[i]/100)>=255)
								mda_group[mda_group_index].img256_head->palette[i]=255;
							else
								mda_group[mda_group_index].img256_head->palette[i]=mda_group[mda_group_index].img256_head->palette[i]+mda_group[mda_group_index].img256_head->palette[i]*system_palette[i]/100;
						}
						
					}

					if(palette_gray)
					{
						for(i=0;i<256;i++)
						{
							mda_group[mda_group_index].img256_head->palette[i*3+1]=mda_group[mda_group_index].img256_head->palette[i*3];
							mda_group[mda_group_index].img256_head->palette[i*3+2]=mda_group[mda_group_index].img256_head->palette[i*3];
						}
					}
					
				}
				if(alpha_flag)
				{
					if(zoom_flag)
						display_zoom_img256_buffer_alpha(x,y,data_buffer,bitmap,value,alpha_value);
					else
						display_img256_buffer_alpha(x,y,data_buffer,bitmap,alpha_value);
				}
				else
				{
					if(zoom_flag)
						display_zoom_img256_buffer(x,y,data_buffer,bitmap,value);
					else
						display_img256_buffer(x,y,data_buffer,bitmap);
				}
				break;
			}
		}
	}
	
	
	
	
	ret_val=MDA_EXEC_OK;
	if(now_frame==0)
		ret_val=MDA_EXEC_FIRST;
	// --- Loop control
	if(next_frame_flag)
	{
		now_frame++;
		mda_group[mda_group_index].now_frame[command_no]=now_frame;
		
		if(now_frame>=mda_group[mda_group_index].total_frame[command_no])
		{
			ret_val=MDA_EXEC_LAST;
			if(loop_frame_flag)
				mda_group[mda_group_index].now_frame[command_no]=0;
			else
				mda_group[mda_group_index].now_frame[command_no]=mda_group[mda_group_index].total_frame[command_no]-1;
		}
	}
	
	// -----User command
	if((goto_frame>=0)&&(goto_frame<=mda_group[mda_group_index].total_frame[command_no]-1))
		mda_group[mda_group_index].now_frame[command_no]=goto_frame;
	
	if((palette_or_flag)||(palette_flag)||(palette_and_flag)||(palette_sub_flag)||(palette_add_flag)||(palette_gray))
		memcpy(mda_group[mda_group_index].img256_head->palette,backup_palette,768);
	
	return(ret_val);
}



SLONG check_map_layer_touch(SLONG real_x,SLONG real_y)
{
	SLONG i;
	SLONG touch_offset;
	SLONG idx;
	SLONG bit_x;
	UCHR mask[8]={
		0x80,
		0x40,
		0x20,
		0x10,
		0x08,
		0x04,
		0x02,
		0x01
	};

	SLONG offset_x,offset_y;

	for(i=0;i<map_layer_touch.total;i++)
	{
		idx=map_layer_touch.idx[i];
		if((real_x<map_base_data.map_layer_data[idx].orig_x)||
			(real_x>=map_base_data.map_layer_data[idx].orig_x+map_base_data.map_layer_data[idx].xlen)||				// û���ཻ
			(real_y<map_base_data.map_layer_data[idx].orig_y)||
			(real_y>=map_base_data.map_layer_data[idx].orig_y+map_base_data.map_layer_data[idx].ylen)) 
		{
			continue;
		}
		offset_y=real_y-map_base_data.map_layer_data[idx].orig_y;
		offset_x=(real_x-map_base_data.map_layer_data[idx].orig_x)/8;
		bit_x=(real_x-map_base_data.map_layer_data[idx].orig_x)%8;
		touch_offset=offset_y*(map_base_data.map_layer_data[idx].xlen/8)+offset_x;
		if(touch_offset<0)continue;
		if(map_base_data.map_layer_data[idx].image[touch_offset]&mask[bit_x])
			return(TTN_OK);

	}
	return(TTN_NOT_OK);
}

SLONG check_media_type(UCHR *data_buffer)
{
	SLONG media_type;
	
	media_type=FILE_TYPE_UNKNOW;
	if(check_is_wav_type(data_buffer)==TTN_OK)
		media_type=FILE_TYPE_WAV;
	else if (check_is_img256_type(data_buffer)==TTN_OK)
		media_type=FILE_TYPE_IMG;
	else
		media_type=FILE_TYPE_UNKNOW;
	
	return(media_type);
	
}


SLONG check_is_wav_type(UCHR *data_buffer)
{
	UCHR temp[30];
	
	memcpy(temp,data_buffer,20);
	if((temp[8]!='W')||(temp[9]!='A')||
		(temp[10]!='V')||(temp[11]!='E'))
		return(TTN_NOT_OK);
	return(TTN_OK);
}


SLONG check_is_img256_type(UCHR *data_buffer)
{
	IMG256_HEAD temp_img256_head;
	
	memcpy(&temp_img256_head,data_buffer,sizeof(struct IMG256_HEAD_STRUCT));
	if(strcmpi((char *)temp_img256_head.copyright,"RAYS IMG256 FILE")!=0)
		return(TTN_NOT_OK);
	return(TTN_OK);
}


SLONG find_database(SLONG mda_group_index,UCHR *database_text)
{
	SLONG ret_val;
	SLONG i;
	
	ret_val=TTN_NOT_OK;
	for(i=0;i<mda_group[mda_group_index].total_database;i++)
	{
		if(strcmpi((char *)mda_group[mda_group_index].database_index[i].name,(char *)database_text)==0)
		{
			ret_val=i;
			return(ret_val);
		}
	}
	return(ret_val);
}



SLONG decode_database_str(SLONG command_no,SLONG now_frame,SLONG idx,UCHR *database_text)
{
	SLONG i;
	SLONG database_count;

	
	database_count=0;
	for(i=0;i<mda_group[idx].total_database_text;i++)
	{
		if(mda_group[idx].database_command_text[i].idx==command_no)		// find command no
		{
			if(database_count==now_frame)
			{
				strcpy((char *)database_text,(char *)mda_group[idx].database_command_text[i].text);
				return(TTN_OK);
			}
			database_count++;
			
		}
		
	}
	
	return(TTN_NOT_OK);

/*
	SLONG total_char;
	bool get_no;
	bool find_command;
	SLONG database_count;
	UCHR no_text[255];
	UCHR text[255];
	SLONG no_save_index;
	SLONG text_save_index;
	SLONG i;
	
	
	total_char=strlen((char *)src_text);
	no_save_index=0;
	text_save_index=0;
	strcpy((char *)no_text,"");
	strcpy((char *)text,"");
	get_no=false;
	find_command=false;
	database_count=0;
	
	for(i=0;i<total_char;i++)
	{
		switch(src_text[i])
		{
		case '(':
			no_save_index=0;
			get_no=true;
			break;
		case ')':
			get_no=false;
			no_text[no_save_index]=0x00;
			break;
		case ';':
			text[text_save_index]=0x00;
			text_save_index=0;
			if(command_no==atoi((char *)no_text))
				find_command=true;
			if(find_command==false)break;
			if(database_count==now_frame)
			{
				strcpy((char *)database_text,(char *)text);
				return(TTN_OK);
			}
			database_count++;
			break;
		default:
			if(get_no)
			{
				no_text[no_save_index]=src_text[i];
				no_save_index++;
			}
			else
			{
				text[text_save_index]=src_text[i];
				text_save_index++;
			}
			break;
		}
	}

	return(TTN_NOT_OK);
*/
}


SLONG decode_effect_str(SLONG command_no,SLONG now_frame,SLONG idx,UCHR *database_text)
{

	SLONG i;
	SLONG real_frame;
	SLONG check_id;

	
	real_frame=command_no*10000+now_frame;
	for(i=0;i<mda_group[idx].total_effect_text;i++)
	{
		check_id=mda_group[idx].effect_command_text[i].idx;
		if(mda_group[idx].effect_command_text[i].idx==real_frame)		// find command no
		{
			strcpy((char *)database_text,(char *)mda_group[idx].effect_command_text[i].text);
			return(TTN_OK);
		}
		
	}
	
	return(TTN_NOT_OK);
	

/*
	SLONG total_char;
	bool get_no;
	bool find_command;
	UCHR no_text[255];
	UCHR text[255];
	SLONG no_save_index;
	SLONG text_save_index;
	SLONG real_frame;
	SLONG i;
	
	
	real_frame=command_no*10000+now_frame;
	total_char=strlen((char *)src_text);
	no_save_index=0;
	text_save_index=0;
	strcpy((char *)no_text,"");
	strcpy((char *)text,"");
	get_no=false;
	find_command=false;
	
	for(i=0;i<total_char;i++)
	{
		switch(src_text[i])
		{
		case '(':
			no_save_index=0;
			get_no=true;
			break;
		case ')':
			get_no=false;
			no_text[no_save_index]=0x00;
			break;
		case ';':
			text[text_save_index]=0x00;
			text_save_index=0;
			if(real_frame==atoi((char *)no_text))
				find_command=true;
			if(find_command==false)break;
			strcpy((char *)database_text,(char *)text);
			return(TTN_OK);
		default:
			if(get_no)
			{
				no_text[no_save_index]=src_text[i];
				no_save_index++;
			}
			else
			{
				text[text_save_index]=src_text[i];
				text_save_index++;
			}
			break;
		}
	}
	return(TTN_NOT_OK);
*/
}



SLONG decode_command_str(UCHR *command,SLONG idx)
{
	SLONG i;

	for(i=0;i<mda_group[idx].total_main_text;i++)
	{
		if(strcmpi((char *)command,(char *)mda_group[idx].main_command_text[i].text)==NULL)
			return(mda_group[idx].main_command_text[i].idx);

	}

	return(TTN_NOT_OK);
/*
	SLONG total_char;
	SLONG command_index;
	bool get_no;
	UCHR no_text[255];
	UCHR text[255];
	SLONG no_save_index;
	SLONG text_save_index;
	SLONG i;
	
	
	total_char=strlen((char *)src_text);
	no_save_index=0;
	text_save_index=0;
	strcpy((char *)no_text,"");
	strcpy((char *)text,"");
	get_no=false;
	command_index=-1;
	
	for(i=0;i<total_char;i++)
	{
		switch(src_text[i])
		{
		case '(':
			no_save_index=0;
			get_no=true;
			break;
		case ')':
			get_no=false;
			no_text[no_save_index]=0x00;
			break;
		case ';':
			text[text_save_index]=0x00;
			if(strcmpi((char *)command,(char *)text)==0)
			{
				command_index=atoi((char *)no_text);
				return(command_index);
			}
			text_save_index=0;
			break;
		default:
			if(get_no)
			{
				no_text[no_save_index]=src_text[i];
				no_save_index++;
			}
			else
			{
				text[text_save_index]=src_text[i];
				text_save_index++;
			}
			break;
		}
	}
	return(command_index);
*/
}


void exec_user_command(UCHR *src_command,UCHR *data_buffer)
{
	SLONG idx;
	UCHR command[255];
	UCHR value[255];
	
	goto_frame=-1;
	take_space(src_command);
	idx=0;
	get_part_str(src_command,command,&idx,' ',200);
	if(strcmpi((char *)command,"attack")==0)					// attack
	{
		idx++;
		get_part_str(src_command,value,&idx,' ',200);
		attack_start=1;
		attack_value=atoi((char *)value);
	}
	else if(strcmpi((char *)command,"goto")==0)					// Goto Frame
	{
		idx++;
		get_part_str(src_command,value,&idx,' ',200);
		goto_frame=atoi((char *)value);
	}
	else if(strcmpi((char *)command,"front")==0)				// front
		front_start=1;
	else if(strcmpi((char *)command,"magic")==0)				//  magic
		magic_start=1;
	else
	{
		sprintf((char *)print_rec,MSG_MDA_UNKNOW_COMMAND,command);
		display_error_message(print_rec,true);
	}
}



void take_space(UCHR *data)
{
	SLONG i;
	SLONG j;
	SLONG pass;
	UCHR temp[255];
	
	j=0;
	pass=0;
	memset(temp,0x00,254);
	for(j=0,i=0;i<(SLONG)strlen((const char *)data);i++)
	{
		if(((data[i]==' ')||(data[i]==0x09))&&(pass==0))
			continue;
		else
		{
			pass=1;
			temp[j]=data[i];
			j=j+1;
		}
	}
	temp[j]=(UCHR)NULL;
	strcpy((char *)data,(char *)temp);
}


void get_part_str(UCHR *in,UCHR *out,SLONG *idx,UCHR key,SLONG max)
{
	SLONG i,j;
	UCHR skip_char;
	
	skip_char=' ';
	for(j=0,i=*idx;i<(SLONG)strlen((const char *)in);i++)
	{
		if(j>=max)
		{
			out[j]=(UCHR)NULL;
			*idx=i;
			skip_char=(UCHR)NULL;
			return;
		}
		else if(key!=(UCHR)NULL)
		{
			switch(in[i])
			{
			case '=': case ',':
				out[j]=(UCHR)NULL;
				*idx=i;
				skip_char=in[i];
				return;
			case ' ':
				continue;
			default:
				break;
			}
		}
		out[j]=in[i];
		j=j+1;
	}
	out[j]=(UCHR)NULL;
	skip_char=(UCHR)NULL;
	*idx=i;
}


void get_part_str1(UCHR *in,UCHR *out,SLONG *idx,UCHR key,SLONG max)
{
	SLONG i,j;
	UCHR skip_char;
	
	skip_char=' ';
	for(j=0,i=*idx;i<(SLONG)strlen((const char *)in);i++)
	{
		if(j>=max)
		{
			out[j]=(UCHR)NULL;
			*idx=i;
			skip_char=(UCHR)NULL;
			return;
		}
		else if(key!=(UCHR)NULL)
		{
			switch(in[i])
			{
			case ' ':
				out[j]=(UCHR)NULL;
				*idx=i;
				skip_char=in[i];
				return;
			default:
				break;
			}
		}
		out[j]=in[i];
		j=j+1;
	}
	out[j]=(UCHR)NULL;
	skip_char=(UCHR)NULL;
	*idx=i;
}


/******************************************************************************
*function name: GetRegKey
*description  : 
*return       : 
*parameter    : 
*modified     : 2004/6/10 by joe
******************************************************************************/
LONG getRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
    HKEY hkey;
	
    LONG retval = RegOpenKeyEx(key, subkey, 0, KEY_QUERY_VALUE, &hkey);
	
    if (retval == ERROR_SUCCESS) 
	{
        long datasize = MAX_PATH;
		
        TCHAR data[MAX_PATH];
		
        RegQueryValue(hkey, NULL, data, &datasize);
		
        lstrcpy(retdata, data);
		
        RegCloseKey(hkey);
    }
	
    return retval;
}


/******************************************************************************
*function name: GotoURL
*description  : 
*return       : 
*parameter    : 
*modified     : 2004/6/10 by joe
******************************************************************************/
HINSTANCE gotoURL(LPCTSTR url, int nShowCmd)
{
	TCHAR key[MAX_PATH + MAX_PATH];
	// First try ShellExecute()
	HINSTANCE result = ShellExecute(NULL, _T("open"), url, NULL, NULL, nShowCmd);
    // If it failed, get the .htm regkey and lookup the program
    if ((UINT) result <= HINSTANCE_ERROR)
	{
        if (getRegKey(HKEY_CLASSES_ROOT, _T(".htm"), key) == ERROR_SUCCESS)
		{
            lstrcat(key, _T("\\shell\\open\\command"));
            if (getRegKey(HKEY_CLASSES_ROOT, key, key) == ERROR_SUCCESS) 
			{
                TCHAR *pos;
                pos = _tcsstr(key, _T("\"%1\""));
                if (pos == NULL)                       // No quotes found
				{                     
                    pos = _tcsstr(key, _T("%1"));      // Check for %1, without quotes 
					if (pos == NULL)                   // No parameter at all...
                        pos = key + lstrlen(key) - 1;
                    else
                        *pos = '\0';                   // Remove the parameter
                }
                else
                    *pos = '\0';                       // Remove the parameter
                lstrcat(pos, _T(" "));
                lstrcat(pos, url);
				//                USES_CONVERSION;
                result = (HINSTANCE) WinExec((char*)key, nShowCmd);
            }
        }
    }
    return result;
}

/******************************************************************************
*function name: OpenHttp
*description  : 
*return       : 
*parameter    : 
*modified     : 2004/6/10 by joe
******************************************************************************/ 
BOOL openHttp(LPCTSTR url)
{
	int result = (int) gotoURL(url, SW_SHOW);
	BOOL bVisitable = (result > HINSTANCE_ERROR);
	if (!bVisitable)
	{
		MessageBeep(MB_ICONEXCLAMATION);     // Unable to follow link
	}
	return bVisitable;
}


/*
static SERVERINFO server_list[] = { 
	{ "��������", 1000, UGCOLOR_ARGB(211,161,197,119), 9009, 0, 0},
	{ "��������", 1001, UGCOLOR_ARGB(211,161,197,120), 9009, 80, 1},
	{ "��������", 1002, UGCOLOR_ARGB(211,161,197,121), 9009, 20, 1},
	{ "��������", 1003, UGCOLOR_ARGB(211,161,197,122), 9009, 40, 1},
	{ "��������", 2000, UGCOLOR_ARGB(211,161,197,123), 9009, 60, 1},
	{ "��������", 2001, UGCOLOR_ARGB(211,161,197,124), 9009, 100, 1},
	{ "��������", 3000, UGCOLOR_ARGB(211,161,197,125), 9009, 0, 1},
	{ "��������", 3001, UGCOLOR_ARGB(211,161,197,126), 9009, 0, 1},
	{ "�����ײ�", 4000, UGCOLOR_ARGB(211,161,197,127), 9009, 0, 1}
};
*/

SLONG getServerInfoList(void)
{	

/*
#if 0
	SLONG total;
	total=ARRAYSIZE(server_list);
	server_info=(SERVERINFO *)malloc(sizeof(SERVER_INFO_STRUCT)*total);
	if(server_info==NULL)
		return(TTN_NOT_OK);
	memcpy(server_info,server_list,sizeof(SERVER_INFO_STRUCT)*total);

	return total;	
	
#else
*/
	
	DOORINFO_HEAD doorinfo_head = {0};
	
	memset(&doorinfo_head,0,sizeof(DOORINFO_HEAD));
		
	FILE* fp = fopen("door.dat", "rb");
	
	if (!fp)
	{
		display_error_message((UCHR*)"can not load door.dat", true);

		return 0;
	}
		
	int len = 0;
	
	fseek(fp, 256, SEEK_SET);	
		
	fread(&doorinfo_head, sizeof(DOORINFO_HEAD), 1, fp);	
	
	len = ftell(fp);
	
	fseek(fp, doorinfo_head.offset_server, SEEK_SET);
	
	server_info=(SERVERINFO *)malloc(sizeof(SERVER_INFO_STRUCT)*doorinfo_head.total_server);

	if (!server_info)
	{
		display_error_message((UCHR*)MSG_MEMORY_ALLOC_ERROR, true);
	}
		
	if (server_info)
	{
		fread(server_info,sizeof(SERVERINFO)*doorinfo_head.total_server, 1, fp);
	}
	
	fclose(fp);

	return doorinfo_head.total_server;
	
}

SpewRetval_t MySpewFunc( SpewType_t type, char const *pMsg )
{		
	::MessageBox(NULL, pMsg, "Error", MB_ICONSTOP|MB_OK);
	
	if( type == SPEW_ASSERT )
		return SPEW_DEBUGGER;
	else if( type == SPEW_ERROR )
		return SPEW_ABORT;
	else
		return SPEW_CONTINUE;
}

//////////////////////////////////////////////////////////////////////////
//

void system_quit_process(void)
{
	SLONG i;

// --- ( 0 - 1 ) 
	if(server_info!=NULL)
		free(server_info);

//	text_out_data.g_pChat->shutdown();
//	delete text_out_data.g_pChat;

	if(map_control.screen_mask!=NULL)
	{
		destroy_bitmap(&map_control.screen_mask);
		map_control.screen_mask=NULL;
	}

	if(map_control.point_idx>=0)
	{
		extra_mda_data[map_control.point_idx].auto_close=true;
		auto_close_extra_mda();
	}

// --- ( 0 - 2 )  MAP
	free_map_base_data();
	for(i=0;i<MAP_MAX_BOTTOM_BUFFER;i++)		// Create Bottom buffer
	{
		if(map_base_data.map_bottom_image[i].image!=NULL)
		{
			destroy_bitmap(&map_base_data.map_bottom_image[i].image);
		}
	}
	
// --- ( 1 - 0 ) Free system image
// -------------------------- message window 	
	if(system_image.msg_TopLeft.image!=NULL)
		free(system_image.msg_TopLeft.image);
	system_image.msg_TopLeft.image=NULL;
	if(system_image.msg_Top.image!=NULL)
		free(system_image.msg_Top.image);
	system_image.msg_Top.image=NULL;
	if(system_image.msg_TopRight.image!=NULL)
		free(system_image.msg_TopRight.image);
	system_image.msg_TopRight.image=NULL;
	if(system_image.msg_Left.image!=NULL)
		free(system_image.msg_Left.image);
	system_image.msg_Left.image=NULL;
	if(system_image.msg_Middle.image!=NULL)
		free(system_image.msg_Middle.image);
	system_image.msg_Middle.image=NULL;
	if(system_image.msg_Right.image!=NULL)
		free(system_image.msg_Right.image);
	system_image.msg_Right.image=NULL;
	if(system_image.msg_BottomLeft.image!=NULL)
		free(system_image.msg_BottomLeft.image);
	system_image.msg_BottomLeft.image=NULL;
	if(system_image.msg_Bottom.image!=NULL)
		free(system_image.msg_Bottom.image);
	system_image.msg_Bottom.image=NULL;
	if(system_image.msg_BottomRight.image!=NULL)
		free(system_image.msg_BottomRight.image);
	system_image.msg_BottomRight.image=NULL;


	free_ripple();	



	for(i=0;i<MAX_ITEM_IMAGE;i++)
	{
		if(system_image.item_image[i]!=NULL)
			free(system_image.item_image[i]);
		system_image.item_image[i]=NULL;
	}

	free_fight_image();

// ---
	text_out_data.g_pChat->shutdown();
	delete text_out_data.g_pChat;
	text_out_data.g_pChat = NULL;
	
}


/*******************************************************************************************}
{																							}
{		M A P     R O U T I N E																}
{																							}
********************************************************************************************/ 
SLONG load_map_file(UCHR *filename,SLONG center_x,SLONG center_y)
{
	MAP_HEAD temp_map_head;
	MAP_IMG_HEAD temp_map_img_head;
	IMG_HEAD temp_img_head;
	EVENT_HEAD temp_event_head;
	CONTROL_HEAD temp_control_head;
	MAP_EFFECT temp_map_effect;
	MAP_LINK temp_map_link;
	SLONG img_idx;
	UCHR real_filename[1024];
	SLONG mda_handle;
	
	SLONG *img_offset=NULL;
	FILE *fp;
	SLONG i;
	SLONG map_start_x,map_start_y;
	
	fp=fopen((char *)filename,"rb");
	if(fp==NULL)
	{
		sprintf((char *)print_rec,MSG_FILE_OPEN_ERROR,filename);
		log_error(1,print_rec);
		return(TTN_NOT_OK);
	}
	
	// --- ( 1 - 0 ) SKIP FILE_HEAD_NULL
	fseek(fp, FILE_HEAD_NULL, SEEK_SET);
	// --- ( 1 - 1 ) Read MAP HEAD
	fread(&temp_map_head, sizeof(temp_map_head), 1, fp);
	if((strcmpi((char *)temp_map_head.copyright,"MAP_HEAD")!=NULL)||
		(temp_map_head.version_number!=MAP_VERSION))
	{
		sprintf((char *)print_rec,MSG_MAP_VERSION_ERROR,filename);
		fclose(fp);
		return(TTN_NOT_OK);
	}
	// --- ( 1 - 2 ) Reload map base data
	free_map_base_data();

	auto_close_extra_mda();				// close map extra 
	clear_all_screen_text();			// ���ĿǰӫĻ����
	stop_all_voice();
		
	
	strcpy((char *)map_base_data.map_filename,(char *)filename);
	strcpy((char *)map_base_data.map_info,(char *)temp_map_head.map_info);
	map_base_data.cell_width=temp_map_head.cell_width;
	map_base_data.cell_height=temp_map_head.cell_height;
	map_base_data.map_width=temp_map_head.map_width;
	map_base_data.map_height=temp_map_head.map_height;
	map_base_data.map_angel=temp_map_head.map_angel;
	
	
	// --- ( 2 - 1 ) fing map IMG Head
	fseek(fp, temp_map_head.offset_img, SEEK_SET);
	fread(&temp_map_img_head,sizeof(struct MAP_IMG_HEAD_STRUCT), 1, fp);		
	if(strcmpi((char *)temp_map_img_head.copyright,"MAP_IMG")!=NULL)
	{
		sprintf((char *)print_rec,MSG_MAP_VERSION_ERROR,filename);
		fclose(fp);
		return(TTN_NOT_OK);
	}
	
	map_base_data.total_map_layer=temp_map_img_head.total_img;		// Total Image 
	if(map_base_data.total_map_layer<=0)
	{
		sprintf((char *)print_rec,MSG_MAP_FORMAT_ERROR,filename);
		log_error(1,print_rec);
		return(TTN_NOT_OK);
	}
	
	
	// --- ( 2 - 2 ) ����Img start offset
	fseek(fp, temp_map_img_head.img_date_offset, SEEK_SET);
	img_offset=(SLONG*)malloc(temp_map_img_head.total_img*sizeof(SLONG));
	if(img_offset==NULL)
	{
		log_error(1,MSG_MEMORY_ALLOC_ERROR);
		fclose(fp);
		return(TTN_NOT_OK);
	}
	fread(img_offset, sizeof(SLONG)*temp_map_img_head.total_img,1, fp);
	
	
	// --- ( 2 -3 ) ���� Img data 
	map_base_data.map_layer_data=(MAP_LAYER_DATA *)malloc(temp_map_img_head.total_img*sizeof(struct MAP_LAYER_DATA_STRUCT));
	if(map_base_data.map_layer_data==NULL)
	{
		log_error(1,MSG_MEMORY_ALLOC_ERROR);
		fclose(fp);
		if(img_offset!=NULL)
			free(img_offset);
		return(TTN_NOT_OK);
	}
	
	
	for(i=0;i<temp_map_img_head.total_img;i++)
	{
		// ---- layer head 
		fseek(fp, img_offset[i], SEEK_SET);
		fread(&temp_img_head, sizeof(IMG_HEAD), 1, fp);
		map_base_data.map_layer_data[i].size=temp_img_head.size;	//ͼƬ�ߴ�
		map_base_data.map_layer_data[i].xlen=temp_img_head.xlen;	//��
		map_base_data.map_layer_data[i].ylen=temp_img_head.ylen;	//��
		map_base_data.map_layer_data[i].orig_x=temp_img_head.orig_x;	// ԭʼ����λ��
		map_base_data.map_layer_data[i].orig_y=temp_img_head.orig_y;
		map_base_data.map_layer_data[i].center_x=temp_img_head.center_x;	// �����c����
		map_base_data.map_layer_data[i].center_y=temp_img_head.center_y;
		map_base_data.map_layer_data[i].type=temp_img_head.type&0xC0;	//ͼƬ�����磺��ͼ���м�㣬�ײ㣬�ϲ㼰�ò�ڼ���ͼ
		// ---- layer image		
		fseek(fp, temp_img_head.offset_img, SEEK_SET);
		map_base_data.map_layer_data[i].image=(UCHR *)malloc(sizeof(UCHR)*map_base_data.map_layer_data[i].size);
		if(map_base_data.map_layer_data[i].image==NULL)
		{
			log_error(1,MSG_MEMORY_ALLOC_ERROR);
			fclose(fp);
			if(img_offset!=NULL)
				free(img_offset);
			return(TTN_NOT_OK);
		}
		fread(map_base_data.map_layer_data[i].image, 
			map_base_data.map_layer_data[i].size, 1, fp);
	}
	
	if(img_offset!=NULL)
		free(img_offset);
	
	
	// ---- ( 3 - 1 )  Load Event 
	map_base_data.map_event_data.max_cell_x=map_base_data.map_width/map_base_data.cell_width;
	map_base_data.map_event_data.max_cell_y=map_base_data.map_height/map_base_data.cell_height;
	fseek(fp, temp_map_head.offset_event, SEEK_SET);
	fread(&temp_event_head, sizeof(temp_event_head), 1, fp);	
	if(strcmpi((char *)temp_event_head.copyright,"EVENT")!=NULL)
	{
		sprintf((char *)print_rec,MSG_MAP_VERSION_ERROR,filename);
		fclose(fp);
		if(img_offset!=NULL)
			free(img_offset);
		return(TTN_NOT_OK);
	}
	
	map_base_data.map_event_data.total_cell=temp_event_head.total_cell;
	if(map_base_data.map_event_data.total_cell!=map_base_data.map_event_data.max_cell_x*map_base_data.map_event_data.max_cell_y)
	{
		sprintf((char *)print_rec,MSG_MAP_FORMAT_ERROR,filename);
		log_error(1,print_rec);
		fclose(fp);
		return(TTN_NOT_OK);
	}
	
	// --- ( 3 - 2 ) Event Data 
	fseek(fp, temp_event_head.event_offset, SEEK_SET);
	if(map_base_data.map_event_data.event_data!=NULL)
		free(map_base_data.map_event_data.event_data);
	map_base_data.map_event_data.event_data=(UHINT *)malloc(sizeof(UHINT)*temp_event_head.total_cell);
	if(map_base_data.map_event_data.event_data==NULL)
	{
		log_error(1,MSG_MEMORY_ALLOC_ERROR);
		fclose(fp);
		return(TTN_NOT_OK);
	}
	fread(map_base_data.map_event_data.event_data, sizeof(UHINT)*temp_event_head.total_cell,1,fp);
	
	
	// --- ( 4 - 1 ) Control Head
	fseek(fp, temp_map_head.offset_control,SEEK_SET);
	fread(&temp_control_head, sizeof(temp_control_head), 1,fp);
	if(strcmpi((char *)temp_control_head.copyright,"CONTROL")!=NULL)
	{
		sprintf((char *)print_rec,MSG_MAP_VERSION_ERROR,filename);
		fclose(fp);
		return(TTN_NOT_OK);
	}
	// --- ( 4 - 2 ) Read Effect Data Process IMG or WAV file 
	map_base_data.total_map_effect=temp_control_head.total_effect;
	if(map_base_data.total_map_effect>0)
	{
		map_base_data.map_effect_data=(MAP_EFFECT_DATA *)malloc(map_base_data.total_map_effect*
			sizeof (struct MAP_EFFECT_DATA_STRUCT));
		if(map_base_data.map_effect_data==NULL)
		{
			log_error(1,MSG_MEMORY_ALLOC_ERROR);
			fclose(fp);
			return(TTN_NOT_OK);
		}
		fseek(fp, temp_control_head.effect_offset,SEEK_SET);
		for(i=0;i<map_base_data.total_map_effect;i++)
		{
			fread(&temp_map_effect, sizeof(struct MAP_EFFECT_STRUCT), 1, fp);
			map_base_data.map_effect_data[i].delay=temp_map_effect.delay;
			map_base_data.map_effect_data[i].times=temp_map_effect.times;
			map_base_data.map_effect_data[i].x=temp_map_effect.x;
			map_base_data.map_effect_data[i].y=temp_map_effect.y;
			map_base_data.map_effect_data[i].idle_timer=0;
			strcpy((char *)map_base_data.map_effect_data[i].filename,
				(char *)temp_map_effect.filename);
			if(map_base_data.map_effect_data[i].times==MAP_EFFECT_IMG)
			{
				sprintf((char *)real_filename,"MDA\\%s",map_base_data.map_effect_data[i].filename);

				mda_handle=get_mda_handle(real_filename);
				if(mda_handle>=0)
				{
					map_base_data.map_effect_data[i].handle=mda_handle;
				}
				else
				{
					if(mda_group_open(real_filename,&map_base_data.map_effect_data[i].handle)!=TTN_OK)
					{
						sprintf((char *)print_rec,MSG_FILE_OPEN_ERROR,filename,map_base_data.map_effect_data[i].filename);
						log_error(1,print_rec);
						map_base_data.map_effect_data[i].handle=-1;
						continue;
						
					}
				}

				mda_group_exec(map_base_data.map_effect_data[i].handle,(UCHR *)"MainCommand0",
					0,0,screen_channel0,
					MDA_COMMAND_FIRST,0,0,0);
				img_idx=get_mda_index(map_base_data.map_effect_data[i].handle);
				if(img_idx<0)
				{
					display_error_message((UCHR *)"Map File error !",1);
					continue;
				}
				map_base_data.map_effect_data[i].img_xl=mda_group[img_idx].img256_head->xlen;
				map_base_data.map_effect_data[i].img_yl=mda_group[img_idx].img256_head->ylen;
				map_base_data.map_effect_data[i].now_frame=mda_group[img_idx].now_frame[0];
				map_base_data.map_effect_data[i].total_frame=mda_group[img_idx].total_frame[0];
				

			}
			else
			{
				map_base_data.map_effect_data[i].handle=-1;
				map_base_data.map_effect_data[i].now_frame=0;
				map_base_data.map_effect_data[i].total_frame=0;
				if(map_base_data.map_effect_data[i].times==MAP_EFFECT_WAV)	// Wav
				{
					map_base_data.map_effect_data[i].wav_active=0;
					map_base_data.map_effect_data[i].channel=-1;
					map_base_data.map_effect_data[i].wav_idle_timer=0;
				}
					
			}
	
		}
		
	}
	
	// --- ( 4 - 3 ) Read Link Data 
	map_base_data.total_map_link=temp_control_head.total_link;
	if(map_base_data.total_map_link>0)
	{
		map_base_data.map_link_data=(MAP_LINK_DATA *)malloc(map_base_data.total_map_link*
			sizeof (struct MAP_LINK_DATA_STRUCT));
		if(map_base_data.map_link_data==NULL)
		{
			log_error(1,MSG_MEMORY_ALLOC_ERROR);
			fclose(fp);
			return(TTN_NOT_OK);
		}
		fseek(fp, temp_control_head.link_offset,SEEK_SET);
		for(i=0;i<map_base_data.total_map_link;i++)
		{
			fread(&temp_map_link, sizeof(struct MAP_LINK_STRUCT), 1, fp);
			map_base_data.map_link_data[i].end_x=temp_map_link.end_x;
			map_base_data.map_link_data[i].end_y=temp_map_link.end_y;
			map_base_data.map_link_data[i].event_no=temp_map_link.event_no;
			strcpy((char *)map_base_data.map_link_data[i].intro,
				(char *)temp_map_link.intro);
			strcpy((char *)map_base_data.map_link_data[i].map_filename,
				(char *)temp_map_link.map_filename);
			map_base_data.map_link_data[i].src_x=temp_map_link.src_x;
			map_base_data.map_link_data[i].src_y=temp_map_link.src_y;
			map_base_data.map_link_data[i].start_x=temp_map_link.start_x;
			map_base_data.map_link_data[i].start_y=temp_map_link.start_y;
		}
		
	}
	
	fclose(fp);

	
	
// --- ( 6 - 0 ) Create Find Path Bar	 ���ŵ�ͼѰ��
	destroy_bar_layer(&map_bar_layer);
	create_bar_layer(&map_bar_layer,
		map_base_data.map_event_data.max_cell_x,
		map_base_data.map_event_data.max_cell_y,
		map_base_data.cell_width,
		map_base_data.cell_height,
		map_base_data.map_event_data.event_data);
	
// --- ( 9 - 0 ) Redraw Map  backgruong
	map_start_x=center_x-(SCREEN_WIDTH/2);
	map_start_y=center_y-(SCREEN_HEIGHT/2);
	if(map_start_x+SCREEN_WIDTH>=map_base_data.map_width)
		map_start_x=map_base_data.map_width-SCREEN_WIDTH;
	if(map_start_y+SCREEN_HEIGHT>=map_base_data.map_height)
		map_start_y=map_base_data.map_height-SCREEN_HEIGHT;
	if(map_start_x<0)map_start_x=0;
	if(map_start_y<0)map_start_y=0;
	map_control.start_x=map_start_x;
	map_control.start_y=map_start_y;
	map_control.target_x=map_start_x;
	map_control.target_y=map_start_y;
	if(load_map_background(map_start_x,map_start_y)!=TTN_OK)
		return(TTN_NOT_OK);

// --- ( 9 - 1 ) 
	create_small_map();

	game_control.map_zoom_index=0;

	return(TTN_OK);
}


SLONG load_map_background(SLONG start_x,SLONG start_y)
{
	SLONG i;
	SLONG oper_x,oper_y;
	SLONG old_oper_x;
	UCHR *bit24_screen_buffer=NULL;
	SLONG bit24_buffer_len=0;
	JPEGAREA jpeg_area;
	SLONG decode_xl,decode_yl;
	
	
// --- ( 0 - 1 )  Clear old map bottom image buffer
	for(i=0;i<MAP_MAX_BOTTOM_BUFFER;i++)
	{
		map_base_data.map_bottom_image[i].image_rect.left=-99999;
		map_base_data.map_bottom_image[i].image_rect.top=-99999;
		map_base_data.map_bottom_image[i].image_rect.right=-99999;
		map_base_data.map_bottom_image[i].image_rect.bottom=-99999;
	}

// --- ( 0 - 2 ) ��������
	oper_x=start_x/SCREEN_WIDTH*SCREEN_WIDTH;		// ���Ͻ�
	oper_y=start_y/SCREEN_HEIGHT*SCREEN_HEIGHT;

	if(!(start_x/(SCREEN_WIDTH/2)) ||  oper_x == map_base_data.map_width - SCREEN_WIDTH)
	{
		oper_x -= SCREEN_WIDTH;
	}
	
	if(!(start_y/(SCREEN_HEIGHT/2)) || oper_y == map_base_data.map_height - SCREEN_HEIGHT)
	{
		oper_y -= SCREEN_HEIGHT;
	}		
	

	if(oper_x<0)oper_x=0;
	if(oper_y<0)oper_y=0;


	old_oper_x=oper_x;

// --- ( 0 - 3 ) Load Buffer 
	for(i=0;i<MAP_MAX_BOTTOM_BUFFER;i++)
	{
		if(check_box_intersect(oper_x,oper_y,oper_x+SCREEN_WIDTH,oper_y+SCREEN_HEIGHT,
			0,0, map_base_data.map_width,map_base_data.map_height)==TTN_OK)	
		{			// �ཻ
			decode_xl=SCREEN_WIDTH;
			decode_yl=SCREEN_HEIGHT;
			if(oper_x+decode_xl>=map_base_data.map_width)
				decode_xl=map_base_data.map_width-oper_x;
			if(oper_y+decode_yl>=map_base_data.map_height)
				decode_yl=map_base_data.map_height-oper_y;
			if((decode_xl>0)&&(decode_yl>0))
			{
				map_base_data.map_bottom_image[i].image_rect.left=oper_x;
				map_base_data.map_bottom_image[i].image_rect.top=oper_y;
				map_base_data.map_bottom_image[i].image_rect.right=oper_x+decode_xl;
				map_base_data.map_bottom_image[i].image_rect.bottom=oper_y+decode_yl;
				jpeg_area.xPos=oper_x;
				jpeg_area.yPos=oper_y;
				jpeg_area.nWidth=decode_xl;	
				jpeg_area.nHeight=decode_yl;

				if(jpg_map_read(map_base_data.map_layer_data[0].image,
					map_base_data.map_layer_data[0].size,
					jpeg_area,&bit24_screen_buffer,bit24_buffer_len)!=JPG_OK)
				{
					if(bit24_screen_buffer!=NULL)
						free(bit24_screen_buffer);
					bit24_screen_buffer=NULL;
					sprintf((char *)print_rec,MSG_FILE_DECODE_ERROR,map_base_data.map_filename);
					log_error(1,print_rec);
					
				}
				
				fill_bitmap(map_base_data.map_bottom_image[i].image,color_control.black);
				bit24_to_partBMP(bit24_screen_buffer,
					map_base_data.map_bottom_image[i].image,
					0,0,
					SCREEN_WIDTH,SCREEN_HEIGHT,
					decode_xl,
					decode_yl);
				
				if(bit24_screen_buffer!=NULL)
					free(bit24_screen_buffer);
				bit24_screen_buffer=NULL;
				
			}
			
		}
		
		oper_x+=SCREEN_WIDTH;
		if((i+1)%3==0)
		{
			oper_x=old_oper_x;
			oper_y+=SCREEN_HEIGHT;
			if(oper_y>=map_base_data.map_height)
				break;
		}

	}
	return(TTN_OK);
	
}


void create_small_map(void)
{
	ULONG file_size;
	UCHR *buffer=NULL;
	
	JPEGAREA jpeg_area;
	UCHR *bit24_screen_buffer=NULL;
	SLONG bit24_buffer_len=0;
	
	
	if(game_control.window_small_map>=0)				// С��ͼ
	{
		delete_window_base(game_control.window_small_map);
	}
	game_control.window_small_map=-1;
	

	game_control.window_world_map=-1;					// �����ͼ
	

	small_map_data.x_percent=(float)SMALL_MAP_WIDTH/(float)map_base_data.map_width;
	small_map_data.y_percent=(float)SMALL_MAP_HEIGHT/(float)map_base_data.map_height;
	
	
	fill_bitmap(small_map_data.image_buffer,color_control.black);
	
	if(change_map_info.small_map_file_no<0)return;
	if(read_mda_database((UCHR *)"system\\smallmap.mda",change_map_info.small_map_file_no,&buffer,&file_size)!=TTN_OK)
		return;
	
	
	jpeg_area.xPos=0;
	jpeg_area.yPos=0;
	jpeg_area.nWidth=SMALL_MAP_WIDTH;	
	jpeg_area.nHeight=SMALL_MAP_HEIGHT;

	if(jpeg_read(buffer,file_size,jpeg_area,&bit24_screen_buffer,bit24_buffer_len)!=JPG_OK)
	{
		if(bit24_screen_buffer!=NULL)
			free(bit24_screen_buffer);
		bit24_screen_buffer=NULL;
		sprintf((char *)print_rec,MSG_FILE_DECODE_ERROR,map_base_data.map_filename);
		log_error(1,print_rec);
		return;		
	}
	
	
	bit24_to_partBMP(bit24_screen_buffer,
		small_map_data.image_buffer,
		0,0,
		SMALL_MAP_WIDTH,SMALL_MAP_HEIGHT,
		SMALL_MAP_WIDTH,SMALL_MAP_HEIGHT);
	
	if(buffer!=NULL)
		free(buffer);
	if(bit24_screen_buffer!=NULL)
		free(bit24_screen_buffer);
	bit24_screen_buffer=NULL;
				
}



SLONG check_box_intersect(SLONG dx,SLONG dy,SLONG dx1,SLONG dy1,SLONG sx,SLONG sy,SLONG sx1,SLONG sy1)
{
	if((dx>sx1)||(dy>sy1)||
		(dx1<sx)||(dy1<sy))    // �z���Пo�ཻ
		return(TTN_NOT_OK);
	return(TTN_OK);
}



void map_control_move(SLONG move_dir,SLONG move_step)
{
	switch(move_dir)
	{
	case MAP_MOVE_UP:
		map_control.target_y-=move_step;
		break;
	case MAP_MOVE_DOWN:
		map_control.target_y+=move_step;
		break;
	case MAP_MOVE_LEFT:
		map_control.target_x-=move_step;
		break;
	case MAP_MOVE_RIGHT:
		map_control.target_x+=move_step;
		break;
	}
	
	if(map_control.target_x+SCREEN_WIDTH>=map_base_data.map_width)
		map_control.target_x=map_base_data.map_width-SCREEN_WIDTH;
	if(map_control.target_y+SCREEN_HEIGHT>=map_base_data.map_height)
		map_control.target_y=map_base_data.map_height-SCREEN_HEIGHT;
	if(map_control.target_x<0)
		map_control.target_x=0;
	if(map_control.target_y<0)
		map_control.target_y=0;
	
}

//#define MOVE_X_BORDER	200
//#define MOVE_Y_BORDER   150

#define MOVE_X_BORDER	250
#define MOVE_Y_BORDER   180


void map_auto_move(void)
{
	SLONG x_offset,y_offset;
	SLONG x_move_offset,y_move_offset;
	SLONG npc_x,npc_y;
	SLONG left_border_x,up_border_y;
	SLONG right_border_x,down_border_y;


	static ULONG timer=0;

	if(system_control.system_timer<timer)
		return;

	timer=system_control.system_timer+MAP_MOVE_SPEED;
	


	map_control.target_x=map_npc_group[system_control.control_npc_idx].npc_info.x-SCREEN_WIDTH/2;
	map_control.target_y=map_npc_group[system_control.control_npc_idx].npc_info.y-SCREEN_HEIGHT/2;
	
// --- ( 0 ) Check Range
	if(map_control.target_x<0)map_control.target_x=0;
	if(map_control.target_y<0)map_control.target_y=0;
	if(map_control.target_x+SCREEN_WIDTH>=map_base_data.map_width)
		map_control.target_x=map_base_data.map_width-SCREEN_WIDTH;
	if(map_control.target_y+SCREEN_HEIGHT>=map_base_data.map_height)
		map_control.target_y=map_base_data.map_height-SCREEN_HEIGHT;
	
	
	if(map_control.start_x<0)map_control.start_x=0;
	if(map_control.start_y<0)map_control.start_y=0;
	if(map_control.start_x+SCREEN_WIDTH>=map_base_data.map_width)
		map_control.start_x=map_base_data.map_width-SCREEN_WIDTH;
	if(map_control.start_y+SCREEN_HEIGHT>=map_base_data.map_height)
		map_control.start_y=map_base_data.map_height-SCREEN_HEIGHT;
	
// --- ( 0 - 1 ) Check Need Move
	if((map_control.start_x==map_control.target_x)&&
		(map_control.start_y==map_control.target_y))
		return;

// --- ( 1 - 0 ) Cehck 	In Border 
	npc_x=map_npc_group[system_control.control_npc_idx].npc_info.x;
	npc_y=map_npc_group[system_control.control_npc_idx].npc_info.y;
	left_border_x=npc_x-map_control.start_x;
	up_border_y=npc_y-map_control.start_y;
	right_border_x=(map_control.start_x+SCREEN_WIDTH)-npc_x;
	down_border_y=(map_control.start_y+SCREEN_HEIGHT)-npc_y;


	if((left_border_x<=MOVE_X_BORDER)||(right_border_x<=MOVE_X_BORDER))
	{
		if(map_npc_group[system_control.control_npc_idx].npc_info.speed==0)	// ��·
			x_move_offset=4;
		else
			x_move_offset=STAND_MOVE_STEP*2;


	}
	else 
	{
		if((left_border_x<=MOVE_X_BORDER+120)||(right_border_x<=MOVE_X_BORDER+120))
			x_move_offset=3;
		else
			x_move_offset=1;
	}

	if((up_border_y<=MOVE_Y_BORDER+80)||(down_border_y<=MOVE_Y_BORDER))
	{
		if(map_npc_group[system_control.control_npc_idx].npc_info.speed==0)	// ��·
			y_move_offset=4;
		else
			y_move_offset=STAND_MOVE_STEP*2;

	
	}
	else
	{
		if((up_border_y<=MOVE_Y_BORDER+120)||(down_border_y<=MOVE_Y_BORDER+75))
			y_move_offset=3;
		else
			y_move_offset=1;
		
	}

	
// --- ( 2 - 0 ) Check Move X position 

	x_offset=map_control.target_x-map_control.start_x;
	y_offset=map_control.target_y-map_control.start_y;




	
	if(x_offset>0)
	{
		if(x_offset>=x_move_offset)
			move_map_background(MAP_MOVE_RIGHT,x_move_offset);
		else
			move_map_background(MAP_MOVE_RIGHT,1);
		
	}
	else if(x_offset<0)
	{
		if(abs(x_offset)>=x_move_offset)
			move_map_background(MAP_MOVE_LEFT,x_move_offset);
		else
			move_map_background(MAP_MOVE_LEFT,1);
		
	}
	
	
	
	if(y_offset>0)
	{
		if(y_offset>=y_move_offset)
			move_map_background(MAP_MOVE_DOWN,y_move_offset);
		else
			move_map_background(MAP_MOVE_DOWN,1);
		
		
	} else if(y_offset<0)
	{
		if(abs(y_offset)>=y_move_offset)
			move_map_background(MAP_MOVE_UP,y_move_offset);
		else
			move_map_background(MAP_MOVE_UP,1);
		
	}
	
	
	
}


void exec_map_control(void)
{

// --- ( 0 - 0 ) Map Move 
	map_auto_move();

	
}


void move_map_background(SLONG move_dir,SLONG step)
{

	SLONG check_x,check_y;
	SLONG change_buffer_idx;
	SLONG screen_x,screen_y;
	
	if(map_base_data.map_layer_data[0].image==NULL)		// �յ� IMAGE
		return;
	
	switch(move_dir)
	{
	case MAP_MOVE_LEFT:
		map_control.start_x-=step;
		break;
	case MAP_MOVE_RIGHT:
		map_control.start_x+=step;
		break;
	case MAP_MOVE_UP:
		map_control.start_y-=step;
		break;
	case MAP_MOVE_DOWN:
		map_control.start_y+=step;
		break;
	}
	
// -------- ����Ƿ���Ҫ�� Buffer
// --- ( 0 - 1 ) ��� ���ϵ�
	check_x=map_control.start_x;
	check_y=map_control.start_y;
	if(check_need_load_background_buffer(check_x,check_y)==TTN_OK)
	{
		change_buffer_idx=get_destory_background_buffer(map_control.start_x+SCREEN_WIDTH/2,map_control.start_y+SCREEN_HEIGHT/2);
		if(change_buffer_idx<0)
			change_buffer_idx=0;
		screen_x=(check_x/SCREEN_WIDTH)*SCREEN_WIDTH;
		screen_y=(check_y/SCREEN_HEIGHT)*SCREEN_HEIGHT;
		load_jpg_to_background_buffer(screen_x,screen_y,change_buffer_idx);
	}

// --- ( 0 - 2 ) ��� ���ϵ�
	check_x=map_control.start_x+SCREEN_WIDTH;
	check_y=map_control.start_y;
	if(check_need_load_background_buffer(check_x,check_y)==TTN_OK)
	{
		change_buffer_idx=get_destory_background_buffer(map_control.start_x+SCREEN_WIDTH/2,map_control.start_y+SCREEN_HEIGHT/2);
		if(change_buffer_idx<0)
			change_buffer_idx=0;
		screen_x=(check_x/SCREEN_WIDTH)*SCREEN_WIDTH;
		screen_y=(check_y/SCREEN_HEIGHT)*SCREEN_HEIGHT;
		load_jpg_to_background_buffer(screen_x,screen_y,change_buffer_idx);
	}

// --- ( 0 - 3 ) ��� ���µ�
	check_x=map_control.start_x;
	check_y=map_control.start_y+SCREEN_HEIGHT;
	if(check_need_load_background_buffer(check_x,check_y)==TTN_OK)
	{
		change_buffer_idx=get_destory_background_buffer(map_control.start_x+SCREEN_WIDTH/2,map_control.start_y+SCREEN_HEIGHT/2);
		if(change_buffer_idx<0)
			change_buffer_idx=0;
		screen_x=(check_x/SCREEN_WIDTH)*SCREEN_WIDTH;
		screen_y=(check_y/SCREEN_HEIGHT)*SCREEN_HEIGHT;
		load_jpg_to_background_buffer(screen_x,screen_y,change_buffer_idx);
	}
	
// --- ( 0 - 4 ) ��� ���µ�
	check_x=map_control.start_x+SCREEN_WIDTH;
	check_y=map_control.start_y+SCREEN_HEIGHT;
	if(check_need_load_background_buffer(check_x,check_y)==TTN_OK)
	{
		change_buffer_idx=get_destory_background_buffer(map_control.start_x+SCREEN_WIDTH/2,map_control.start_y+SCREEN_HEIGHT/2);
		if(change_buffer_idx<0)
			change_buffer_idx=0;
		screen_x=(check_x/SCREEN_WIDTH)*SCREEN_WIDTH;
		screen_y=(check_y/SCREEN_HEIGHT)*SCREEN_HEIGHT;
		load_jpg_to_background_buffer(screen_x,screen_y,change_buffer_idx);
	}
	
	

}



void load_jpg_to_background_buffer(SLONG screen_x,SLONG screen_y,SLONG idx)
{
	UCHR *bit24_screen_buffer=NULL;
	SLONG bit24_buffer_len=0;
	JPEGAREA jpeg_area;
	SLONG decode_xl,decode_yl;
	
	
	decode_xl=SCREEN_WIDTH;
	decode_yl=SCREEN_HEIGHT;
	if(screen_x+decode_xl>map_base_data.map_width)
		decode_xl=map_base_data.map_width-screen_x;
	if(screen_y+decode_yl>map_base_data.map_height)
		decode_yl=map_base_data.map_height-screen_y;
	if((decode_xl>0)&&(decode_yl>0))
	{
		map_base_data.map_bottom_image[idx].image_rect.left=screen_x;
		map_base_data.map_bottom_image[idx].image_rect.top=screen_y;
		map_base_data.map_bottom_image[idx].image_rect.right=screen_x+decode_xl;
		map_base_data.map_bottom_image[idx].image_rect.bottom=screen_y+decode_yl;
		jpeg_area.xPos=screen_x;
		jpeg_area.yPos=screen_y;
		jpeg_area.nWidth=decode_xl;	
		jpeg_area.nHeight=decode_yl;
		
		if(jpg_map_read(map_base_data.map_layer_data[0].image,
			map_base_data.map_layer_data[0].size,
			jpeg_area,&bit24_screen_buffer,bit24_buffer_len)!=JPG_OK)
		{
			if(bit24_screen_buffer!=NULL)
				free(bit24_screen_buffer);
			bit24_screen_buffer=NULL;
			sprintf((char *)print_rec,MSG_FILE_DECODE_ERROR,map_base_data.map_filename);
			log_error(1,print_rec);
			
		}
		
		fill_bitmap(map_base_data.map_bottom_image[idx].image,color_control.black);
		bit24_to_partBMP(bit24_screen_buffer,
			map_base_data.map_bottom_image[idx].image,
			0,0,
			SCREEN_WIDTH,SCREEN_HEIGHT,
			decode_xl,
			decode_yl);
		
		if(bit24_screen_buffer!=NULL)
			free(bit24_screen_buffer);
		bit24_screen_buffer=NULL;
		
	}

}
	

SLONG get_destory_background_buffer(SLONG x,SLONG y)
{
	SLONG i;
	SLONG bx,by;
	double x_offset,y_offset;
	double offset,old_offset;
	SLONG find_idx;

	SLONG check_screen_x,check_screen_y,check_screen_x1,check_screen_y1;

	check_screen_x=map_control.start_x-1;
	check_screen_y=map_control.start_y-1;
	check_screen_x1=map_control.start_x+SCREEN_WIDTH+1;
	check_screen_y1=map_control.start_y+SCREEN_HEIGHT+1;

	if(check_screen_x<0)check_screen_x=0;
	if(check_screen_y<0)check_screen_y=0;
	if(check_screen_x1>=map_base_data.map_width)check_screen_x1=map_base_data.map_width;
	if(check_screen_y1>=map_base_data.map_height)check_screen_y1=map_base_data.map_height;
	
	find_idx=-1;
	offset=0;
	old_offset=0;

	for(i=0;i<MAP_MAX_BOTTOM_BUFFER;i++)
	{

		if(check_box_intersect(check_screen_x,
			check_screen_y,
			check_screen_x1,
			check_screen_y1,
			map_base_data.map_bottom_image[i].image_rect.left,
			map_base_data.map_bottom_image[i].image_rect.top,
			map_base_data.map_bottom_image[i].image_rect.right,
			map_base_data.map_bottom_image[i].image_rect.bottom)==TTN_OK)
			continue;

		bx=map_base_data.map_bottom_image[i].image_rect.left+SCREEN_WIDTH/2;
		by=map_base_data.map_bottom_image[i].image_rect.top+SCREEN_HEIGHT/2;
		x_offset=abs(x-bx);
		y_offset=abs(y-by);
		offset=sqrt(x_offset*x_offset+y_offset*y_offset) ;

		if(offset>old_offset)
		{
			find_idx=i;
			old_offset=offset;
		}

	}

	if(find_idx<0)
		log_error(1,(UCHR *)"Inter error( select destory BMP not fing !");


	return(find_idx);

}

SLONG check_need_load_background_buffer(SLONG x,SLONG y)
{
	SLONG i;

	if((x<0)||(y<0)||
		(x>=map_base_data.map_width)||(y>=map_base_data.map_height))
		return(TTN_NOT_OK);

	for(i=0;i<MAP_MAX_BOTTOM_BUFFER;i++)
	{
		if((x>=map_base_data.map_bottom_image[i].image_rect.left)&&
			(x<map_base_data.map_bottom_image[i].image_rect.right)&&
			(y>=map_base_data.map_bottom_image[i].image_rect.top)&&
			(y<map_base_data.map_bottom_image[i].image_rect.bottom))
			return(TTN_NOT_OK);		// ����Load

	}

	return(TTN_OK);					// ��Ҫ load 
}


void redraw_map_background(BMP *buffer,SLONG sx,SLONG sy)
{
	SLONG i;
	RECT screen_rect;
	RECT draw_rect;
	SLONG move_x,move_y;
	SLONG move_xlen,move_ylen;
	SLONG src_x,src_y;
	SLONG y;


	screen_rect.left=sx;
	screen_rect.top=sy;
	screen_rect.right=sx+SCREEN_WIDTH;
	screen_rect.bottom=sy+SCREEN_HEIGHT;

	
	for(i=0;i<MAP_MAX_BOTTOM_BUFFER;i++)
	{
		if(check_box_intersect(screen_rect.left,screen_rect.top,screen_rect.right,screen_rect.bottom,
			map_base_data.map_bottom_image[i].image_rect.left,
			map_base_data.map_bottom_image[i].image_rect.top,
			map_base_data.map_bottom_image[i].image_rect.right,
			map_base_data.map_bottom_image[i].image_rect.bottom)==TTN_OK)	// �ཻ
		{

// --- �����������
			draw_rect.left=MAX(screen_rect.left,map_base_data.map_bottom_image[i].image_rect.left);
			draw_rect.right=MIN(screen_rect.right,map_base_data.map_bottom_image[i].image_rect.right);
			draw_rect.top=MAX(screen_rect.top,map_base_data.map_bottom_image[i].image_rect.top);
			draw_rect.bottom=MIN(screen_rect.bottom,map_base_data.map_bottom_image[i].image_rect.bottom);


			move_x=draw_rect.left-map_control.start_x;
			move_y=draw_rect.top-map_control.start_y;
			move_xlen=draw_rect.right-draw_rect.left;
			move_ylen=draw_rect.bottom-draw_rect.top;

			src_x=draw_rect.left-map_base_data.map_bottom_image[i].image_rect.left;
			src_y=draw_rect.top-map_base_data.map_bottom_image[i].image_rect.top;



			for(y=0;y<move_ylen;y++)
			{
				if((move_y+y>buffer->h)||
					(move_x+move_xlen>buffer->w)||
					(src_y+y>map_base_data.map_bottom_image[i].image->h )||
					(src_x+move_xlen>map_base_data.map_bottom_image[i].image->w))
					continue;
/*			
SLONG xx;
for(xx=0;xx<move_xlen;xx++)
    buffer->line[move_y+y][move_x+xx]=map_base_data.map_bottom_image[i].image->line[src_y+y][src_x+xx];
*/
				wmemcpy(&buffer->line[move_y+y][move_x],&map_base_data.map_bottom_image[i].image->line[src_y+y][src_x],move_xlen);
			
			}
			
		}
		
	}

}


void redraw_map(BMP *buffer)
{
	
// ----------- Process MAP Task --------------
// ---- Stephen need add 
// ---- ( 0 - 1 ) Check is need change MAP_ANI or MAP Task
	process_map_task();
	
// ---- ( 0 - 2 ) Check Map Move
	exec_map_control();

	
// ---- ( 0 - 3 ) Draw Map bottom image
	redraw_map_background(buffer,map_control.start_x,map_control.start_y);

// ---- ( 0 - 4 ) Draw NPC & Auto Layer
	clear_screen_mask();
	draw_npc_layer(buffer);

// ---- ( 1 - 0 ) Process Camera effect
// ---- ( 1 - 1 ) Draw window & text


// ---- ( 2 - 1 ) Capture screen
	
	

#ifdef MYTH_DEBUG	
// ---- ( 9 - 9 ) Display Debug Info 
	if(debug_info.map_debug)		
	{
		display_map_event(buffer);
		
SLONG i;

		for(i=1;i<map_base_data.total_map_layer;i++)
		{
			if(map_base_data.map_layer_data[i].type==IMG_LAYER_MIDDLE)
			{
				put_box( map_base_data.map_layer_data[i].orig_x-map_control.start_x,
					map_base_data.map_layer_data[i].orig_y-map_control.start_y,
					map_base_data.map_layer_data[i].xlen,
					map_base_data.map_layer_data[i].ylen,
					color_control.green,buffer);
				
				put_bar( map_base_data.map_layer_data[i].orig_x+map_base_data.map_layer_data[i].center_x-map_control.start_x-2,
					map_base_data.map_layer_data[i].orig_y+map_base_data.map_layer_data[i].center_y-map_control.start_y-2,
					4,
					4,
					color_control.red,buffer);
				
			}

			if(map_base_data.map_layer_data[i].type==IMG_LAYER_TOP)
			{
				put_box( map_base_data.map_layer_data[i].orig_x-map_control.start_x,
					map_base_data.map_layer_data[i].orig_y-map_control.start_y,
					map_base_data.map_layer_data[i].xlen,
					map_base_data.map_layer_data[i].ylen,
					color_control.white,buffer);
				
				put_bar( map_base_data.map_layer_data[i].orig_x+map_base_data.map_layer_data[i].center_x-map_control.start_x-2,
					map_base_data.map_layer_data[i].orig_y+map_base_data.map_layer_data[i].center_y-map_control.start_y-2,
					4,
					4,
					color_control.white,buffer);
				
			}
			

		}
	}	
#endif
	
}



void display_map_event(BMP *buffer)
{
	SLONG start_cell_x,start_cell_y;
	SLONG cell_xlen,cell_ylen;
	SLONG x,y;
	SLONG bar_x,bar_y;
	UHINT event;
	SLONG cell_index;
	SLONG i;
	SLONG display_x,display_y;
	SLONG event_no;
	SLONG event_show[255];
	POSITION event_pp[255];
	SLONG j;

	for(i=0;i<255;i++)
		event_show[i]=-1;

	start_cell_x=0;
	start_cell_y=0;
	cell_xlen=map_base_data.map_width/map_base_data.cell_width;
	cell_ylen=map_base_data.map_width/map_base_data.cell_height;
	

	for(y=0;y<cell_ylen;y++)
	{
		cell_index=(start_cell_y+y)*map_base_data.map_event_data.max_cell_x+start_cell_x;
		for(x=0;x<cell_xlen;x++)
		{
			bar_x=x*map_base_data.cell_width-map_control.start_x;
			bar_y=y*map_base_data.cell_height-map_control.start_y;
			
			if((bar_x<0)||(bar_x>=SCREEN_WIDTH)||
				(bar_y<0)||(bar_y>=SCREEN_HEIGHT))
				continue;

			event=map_base_data.map_event_data.event_data[cell_index+x];
			if(event==0)continue;

			if((event&BITBAR)&&(debug_info.map_stop))
				put_box(bar_x,bar_y,map_base_data.cell_width,map_base_data.cell_height,color_control.blue,buffer);

			
			if((event&BITPATH)&&(debug_info.map_path))
				put_box(bar_x+2,bar_y+2,map_base_data.cell_width-2,map_base_data.cell_height-2,color_control.yellow,buffer);

			if((event&BITSHADE)&&(debug_info.map_shadow))
				put_box(bar_x+3,bar_y+3,map_base_data.cell_width-3,map_base_data.cell_height-3,color_control.gray,buffer);


			if((event&BITEVENT)&&(debug_info.map_event))
			{
				put_box(bar_x+1,bar_y+1,map_base_data.cell_width-1,map_base_data.cell_height-1,color_control.red,buffer);
				if((x%5==0)&&(y%5==0))
				{
					event_no=event&0x00FF;
					sprintf((char *)print_rec,"%d",event_no);
					print16(bar_x,bar_y,print_rec,PEST_PUT,buffer);	
					if(event_show[event_no]<0)
					{
						for(j=0;j<map_base_data.total_map_link;j++)
						{
							if(map_base_data.map_link_data[j].event_no==event_no)
							{
								event_show[event_no]=j;
								event_pp[event_no].x=bar_x;
								event_pp[event_no].y=bar_y;
							}
						}
					}
				}
			}
		}
	}

	if(debug_info.map_sound)
	{
		for(i=0;i<map_base_data.total_map_effect;i++)
		{
		display_x=map_base_data.map_effect_data[i].x-map_control.start_x;		
		display_y=map_base_data.map_effect_data[i].y-map_control.start_y;
		bar_x=display_x-strlen((char *)map_base_data.map_effect_data[i].filename)*4;
		bar_y=display_y-16;
		if((display_x<0)||(display_y<0))continue;
		if((display_x>SCREEN_WIDTH)||(display_y>SCREEN_HEIGHT))continue;
		if(bar_x<0)bar_x=0;
		if(bar_y<0)bar_y=0;
		print16(bar_x,bar_y,(UCHR *)map_base_data.map_effect_data[i].filename,PEST_PUT,buffer);		
		}
	}

	for(i=0;i<255;i++)
	{
		if(event_show[i]>=0)
		{
			print16(event_pp[i].x,event_pp[i].y,(UCHR *)map_base_data.map_link_data[event_show[i]].map_filename,COPY_PUT,buffer);		
		}
	}

	
}



void process_map_task(void)
{
	SLONG i;
	SLONG ret_val;
	SLONG now_dir;
	ULONG rand_times; 
	SLONG volume,pan;
	SLONG follow_id;
	SLONG idx;
	UCHR real_filename[1024];
	
	// --- ( 0 - 0 )  Check NPC MOVE
	for(i=0;i<total_map_npc;i++)
	{
		switch(map_npc_group[i].npc_info.move_type)
		{
		case NPC_MOVE_PATH:					// ѭ���ƶ�
			ret_val=npc_auto_path_move(i);
			break;
		case NPC_MOVE_AUTO:					// �ڸ���(�༭���趨)�����ƶ�
		case NPC_MOVE_HOLD:					// ���ƶ�
			ret_val=npc_auto_cell_move(i);
			break;
		case NPC_MOVE_FOLLOW:
			ret_val=npc_auto_follow_move(i);
			break;
		default:
			continue;
			break;		
		}
		
// -------------- ret_val  ����� TTN_OK ���� Idle control 
		if((ret_val==TTN_OK)||
			(ret_val==FIND_FAIL_NO_PATH)||
			(ret_val==FIND_FAIL_ERROR))
		{
			switch(map_npc_group[i].npc_info.motility)
			{
			case NPC_MOTION_STANDBY1:
				if(map_npc_group[i].npc_idle_timer<system_control.system_timer)
				{
					change_npc_motion(i,NPC_MOTION_STANDBY2);
				}
				break;
			case NPC_MOTION_STANDBY2:
				now_dir=map_npc_group[i].npc_info.dir;
				if(map_npc_group[i].npc_mda_control[now_dir].now_frame>=map_npc_group[i].npc_mda_control[now_dir].total_frame-1)
				{
					rand_times=(rand()%10)*1000+NPC_IDLE_BASE;			// 5 ��
					map_npc_group[i].npc_idle_timer=system_control.system_timer+rand_times;
					change_npc_motion(i,NPC_MOTION_STANDBY1);
				}
				
				break;
			}
			
		}
		
	}
	
// --------- Exec Map Wav Effect 
	for(i=0;i<map_base_data.total_map_effect;i++)
	{
		if(map_base_data.map_effect_data[i].wav_idle_timer>=system_control.system_timer)
			continue;
		map_base_data.map_effect_data[i].wav_idle_timer=system_control.system_timer+SYSTEM_MAIN_DELAY;
		
		if(map_base_data.map_effect_data[i].times==MAP_EFFECT_WAV)
		{
			
			if(check_map_wave_active(map_base_data.map_effect_data[i].x,map_base_data.map_effect_data[i].y)==TTN_OK)	
			{					// ���� ���õ��ľ���
				if(map_base_data.map_effect_data[i].wav_active==0)	// ��һ�ν���
				{
					volume=get_map_wave_volume(map_base_data.map_effect_data[i].x,map_base_data.map_effect_data[i].y);
					pan=get_map_wave_pan(map_base_data.map_effect_data[i].x,map_base_data.map_effect_data[i].y);
					sprintf((char *)real_filename,"EFC\\%s",map_base_data.map_effect_data[i].filename);
					map_base_data.map_effect_data[i].channel=play_voice_file(real_filename,1,volume,pan);
					map_base_data.map_effect_data[i].wav_active=1;
				}
				else											// �Ѿ� ���Ź�
				{
					volume=get_map_wave_volume(map_base_data.map_effect_data[i].x,map_base_data.map_effect_data[i].y);
					pan=get_map_wave_pan(map_base_data.map_effect_data[i].x,map_base_data.map_effect_data[i].y);
					if(map_base_data.map_effect_data[i].channel>=0)	
					{
						auto_set_voice_control(map_base_data.map_effect_data[i].channel,volume,pan);
					}

				}

			}
			else		// �����õ��ķ�Χ��
			{
				if(map_base_data.map_effect_data[i].wav_active==1)	//  ��ǰ���Ź�
				{
					if(map_base_data.map_effect_data[i].channel>=0)	
						auto_stop_voice(map_base_data.map_effect_data[i].channel);
					map_base_data.map_effect_data[i].wav_active=0;
				}

			}
		}
	}



// --------- ��� û�����˵ı���  һ�μ��һ��#83
	for(i=0;i<total_map_npc;i++)
	{
		if(map_npc_group[i].npc_info.move_type==NPC_MOVE_FOLLOW)
		{
			follow_id=map_npc_group[i].follow_id;
			idx=get_map_npc_index(follow_id);
			if(idx<0)									// �������뵱ǰ
			{
				delete_map_npc_group(map_npc_group[i].npc_info.id); 
				i=total_map_npc+1;
				break;
			}
			
		}
		
	}
	

}



void reactive_map_effect(void)
{
	SLONG i;

	for(i=0;i<map_base_data.total_map_effect;i++)
	{
		if(map_base_data.map_effect_data[i].times==MAP_EFFECT_WAV)
		{
			if(check_map_wave_active(map_base_data.map_effect_data[i].x,map_base_data.map_effect_data[i].y)==TTN_OK)	
			{					// ���� ���õ��ľ���
				map_base_data.map_effect_data[i].wav_active=0;
			}
		}
	}
	
}



SLONG get_map_wave_pan(SLONG x,SLONG y)
{
	SLONG npc_x;
	SLONG x_range;
	SLONG pan_per;
	SLONG pan;

	npc_x=map_npc_group[system_control.control_npc_idx].npc_info.x;
	x_range=abs(npc_x-x);
	if(x_range<200)return(VOICE_PAN_CENTER);

	x_range=x_range-200;
	
	pan_per=x_range*100/MAP_EFFECT_X_RANGE;

	if(pan_per<=0)
	{
		if(x>npc_x)return(VOICE_PAN_RIGHT);
		else return(VOICE_PAN_LEFT);
	}
	if(pan_per>=100)
	{
		if(x>npc_x)return(VOICE_PAN_RIGHT);
		else return(VOICE_PAN_LEFT);
	}


	pan=10000 * pan_per /100; 
		
	if(x>npc_x)return(pan);
	pan=pan*-1;

	return(pan);
	
}


SLONG get_map_wave_volume(SLONG x,SLONG y)
{
	SLONG npc_x,npc_y;
	double offset;
	double x_offset,y_offset;
	SLONG volume_per;
	SLONG volume;

	npc_x=map_npc_group[system_control.control_npc_idx].npc_info.x;
	npc_y=map_npc_group[system_control.control_npc_idx].npc_info.y;

	x_offset=(double) abs(npc_x-x);
	y_offset=(double) abs(npc_y-y);

	offset=sqrt((x_offset*x_offset)+(y_offset*y_offset));

	volume_per=(SLONG)(offset*100/MAP_EFFECT_OFFSET);
	volume_per=100-volume_per;

	if(volume_per<=0)return(MAP_VOICE_VOLUME_MIN);
	if(volume_per>=100)return(MAP_VOICE_VOLUME_MAX);


	volume= (MAP_VOICE_VOLUME_MAX-MAP_VOICE_VOLUME_MIN) * volume_per/100;
// --- ��Ϊ 0 ���, -3000 ��С  ����
	volume= MAP_VOICE_VOLUME_MIN+volume;
	if(volume>=MAP_VOICE_VOLUME_MAX)
		volume=MAP_VOICE_VOLUME_MAX;
	return(volume);
}



SLONG check_map_wave_active(SLONG x,SLONG y)
{
	SLONG npc_x,npc_y;
	double x_offset,y_offset;
	double offset;
	
	if(system_control.control_npc_idx<0)return(TTN_NOT_OK);
	npc_x=map_npc_group[system_control.control_npc_idx].npc_info.x;
	npc_y=map_npc_group[system_control.control_npc_idx].npc_info.y;

	x_offset=(double) abs(npc_x-x);
	y_offset=(double) abs(npc_y-y);
	
	offset=sqrt((x_offset*x_offset)+(y_offset*y_offset));
	
	if(offset<=MAP_EFFECT_OFFSET)
		return(TTN_OK);
	return(TTN_NOT_OK);

}


/*******************************************************************************************}
{																							}
{		N P C     R O U T I N E																}
{																							}
********************************************************************************************/ 
SLONG add_map_npc( BASE_NPC_INFO& add_npc)
{
	SLONG map_npc_index;
	SLONG mda_handle;
	SLONG mda_index;
	UCHR npc_filename[256];


	if(check_same_npc_id(add_npc.id)==TTN_OK)
		return(TTN_NOT_OK);

	sprintf((char *)npc_filename,"NPC\\NPC%03da.MDA",(char *)add_npc.filename_id);
	
// --- ( 0 - 0 ) Check File
	if(check_file_exist(npc_filename)!=TTN_OK)
	{
		sprintf((char *)print_rec,MSG_FILE_OPEN_ERROR,npc_filename);
		display_error_message(print_rec,true);
		return(TTN_NOT_OK);
	}
	if(check_is_mda_file(npc_filename)!=TTN_OK)
	{
		sprintf((char *)print_rec,MSG_FILE_NOT_MDA_ERROR,npc_filename);
		display_error_message(print_rec,true);
		return(TTN_NOT_OK);
	}
	
// --- ( 0 - 1 ) alloc map_npc struct 
	map_npc_index=add_map_npc_group();
	if(map_npc_index<0)
	{
		sprintf((char *)print_rec,MSG_FILE_CREAT_ERROR,npc_filename);
		display_error_message((UCHR *)print_rec,true);
		return(TTN_NOT_OK);
	}

// --- ( 1 - 0 ) Fill data

	memcpy(&map_npc_group[map_npc_index].npc_info,&add_npc,sizeof (struct BASE_NPC_INFO_STRUCT));

	if(add_npc.id&NPC_ID_MASK)
	{
		if(add_npc.id&BAOBAO_ID_CHECK_MASK)
		{
			map_npc_group[map_npc_index].npc_type=NPC_BAOBAO_TYPE;
			map_npc_group[map_npc_index].npc_info.dir=map_npc_group[map_npc_index].npc_info.dir%4;	
		}
		else
		{
			map_npc_group[map_npc_index].npc_type=NPC_CPU_TYPE;
			map_npc_group[map_npc_index].npc_info.dir=map_npc_group[map_npc_index].npc_info.dir%4;	
		}
	}
	else
	{
		map_npc_group[map_npc_index].npc_type=NPC_CHARACTER_TYPE;
	}

// --- ( 1 - 1 ) Find MDA handle 
	mda_handle=get_mda_handle(npc_filename);
	if(mda_handle>=0)					// Already Open
	{
		map_npc_group[map_npc_index].handle=mda_handle;
	}
	else
	{
		if(mda_group_open(npc_filename,&mda_handle)!=TTN_OK)
		{
			sprintf((char *)print_rec,MSG_FILE_CREAT_ERROR,npc_filename);
			display_error_message((UCHR *)print_rec,true);
			return(TTN_NOT_OK);
		}
		map_npc_group[map_npc_index].handle=mda_handle;

// --- ��һ��ִ��
		mda_group_exec(mda_handle,(UCHR *)"MainCommand0",-9999,-9999,screen_channel1,MDA_COMMAND_FIRST,0,0,0);
		

	}

	mda_index=get_mda_index(mda_handle);
	if(mda_index<0)
	{
		sprintf((char *)print_rec,MSG_FILE_CREAT_ERROR,npc_filename);
		display_error_message((UCHR *)print_rec,true);
		return(TTN_NOT_OK);
	}


	adjust_npc_frame_data(map_npc_index);

	map_npc_group[map_npc_index].npc_timer=system_control.system_timer;				// Clear play speed timer
	map_npc_group[map_npc_index].npc_idle_timer=system_control.system_timer+NPC_IDLE_BASE;	// Clear play speed timer
	map_npc_group[map_npc_index].npc_move_timer=system_control.system_timer;		// Clear play speed timer
	map_npc_group[map_npc_index].npc_stage=NPC_STAGE_NONE;
	map_npc_group[map_npc_index].npc_status=add_npc.status;
	map_npc_group[map_npc_index].follow_id=-1;										// û�и���Ŀ��
	map_npc_group[map_npc_index].follow_range=0;		
	map_npc_group[map_npc_index].follow_timer=0;
	return(TTN_OK);

}



SLONG check_same_npc_id(ULONG id)
{
	SLONG i;

	for(i=0;i<total_map_npc;i++)
	{
		if(map_npc_group[i].npc_info.id==id)
			return(TTN_OK);
	}
	return(TTN_NOT_OK);
}



void adjust_npc_frame_data(SLONG map_npc_index)
{
	SLONG i;
	SLONG motion_index;
	SLONG total_frame;
	SLONG mda_index;

	mda_index=get_mda_index(map_npc_group[map_npc_index].handle);
	if(mda_index<0)return;
	switch(map_npc_group[map_npc_index].npc_type)
	{
	case NPC_CPU_TYPE:				// 4  ������
	case NPC_BAOBAO_TYPE:
		motion_index=map_npc_group[map_npc_index].npc_info.motility*4;
		for(i=0;i<4;i++)
		{
			map_npc_group[map_npc_index].npc_mda_control[i].now_frame=0;
			total_frame=mda_group[mda_index].total_frame[motion_index+i];
			map_npc_group[map_npc_index].npc_mda_control[i].total_frame=total_frame;
			// -- Copy
			map_npc_group[map_npc_index].npc_mda_control[i+4].now_frame=0;
			total_frame=mda_group[mda_index].total_frame[motion_index+i];
			map_npc_group[map_npc_index].npc_mda_control[i+4].total_frame=total_frame;
			
		}
		break;
	case NPC_CHARACTER_TYPE:		// 8 ������
		motion_index=map_npc_group[map_npc_index].npc_info.motility*8;	
		for(i=0;i<8;i++)
		{
			map_npc_group[map_npc_index].npc_mda_control[i].now_frame=0;
			total_frame=mda_group[mda_index].total_frame[motion_index+i];
			map_npc_group[map_npc_index].npc_mda_control[i].total_frame=total_frame;
		}
		break;
	default:
		sprintf((char *)print_rec,MSG_FILE_CREAT_ERROR,"UNKONOW");
		display_error_message((UCHR *)print_rec,true);
		break;
	}
	
}


void adjust_fight_npc_frame_data(SLONG index)
{
	SLONG i;
	SLONG motion_index;
	SLONG mda_index;
	
	mda_index=get_mda_index(fight_npc_group[index].handle);
	if(mda_index<0)return;
	for(i=0;i<MAX_FIGHT_DIR;i++)
	{
		switch(fight_npc_group[index].npc_type)
		{
		case NPC_CHARACTER_TYPE:
			motion_index=0;						// 0 ~ 4
			fight_npc_group[index].dir_frame[i].now_frame=0;
			fight_npc_group[index].dir_frame[i].total_frame=mda_group[mda_index].total_frame[motion_index+i];
			fight_npc_group[index].dir_frame[i].motion=motion_index;

			motion_index=8;						// 8 ~ 11
			fight_npc_group[index].attack1_frame[i].now_frame=0;
			fight_npc_group[index].attack1_frame[i].total_frame=mda_group[mda_index].total_frame[motion_index+i];
			fight_npc_group[index].attack1_frame[i].motion=motion_index;
			
			motion_index=12;					// 12 ~ 15
			fight_npc_group[index].attack2_frame[i].now_frame=0;
			fight_npc_group[index].attack2_frame[i].total_frame=mda_group[mda_index].total_frame[motion_index+i];
			fight_npc_group[index].attack2_frame[i].motion=motion_index;
			
			motion_index=16;					// 16 ~ 19
			fight_npc_group[index].attack3_frame[i].now_frame=0;
			fight_npc_group[index].attack3_frame[i].total_frame=mda_group[mda_index].total_frame[motion_index+i];
			fight_npc_group[index].attack3_frame[i].motion=motion_index;
			
			motion_index=20;					// 20 ~ 23
			fight_npc_group[index].magic_frame[i].now_frame=0;
			fight_npc_group[index].magic_frame[i].total_frame=mda_group[mda_index].total_frame[motion_index+i];
			fight_npc_group[index].magic_frame[i].motion=motion_index;
			
			motion_index=24;					// 24 ~ 27
			fight_npc_group[index].special_frame[i].now_frame=0;
			fight_npc_group[index].special_frame[i].total_frame=mda_group[mda_index].total_frame[motion_index+i];
			fight_npc_group[index].special_frame[i].motion=motion_index;
			
			motion_index=32;					// 32 ~ 35
			fight_npc_group[index].dead_frame[i].now_frame=0;
			fight_npc_group[index].dead_frame[i].total_frame=mda_group[mda_index].total_frame[motion_index+i];
			fight_npc_group[index].dead_frame[i].motion=motion_index;
			
			break;
		case NPC_CPU_TYPE:

			motion_index=0;						// 0 ~ 4
			fight_npc_group[index].dir_frame[i].now_frame=0;
			fight_npc_group[index].dir_frame[i].total_frame=mda_group[mda_index].total_frame[motion_index+i];
			fight_npc_group[index].dir_frame[i].motion=motion_index;
			
			motion_index=8;						// 8 ~ 11
			fight_npc_group[index].attack1_frame[i].now_frame=0;
			fight_npc_group[index].attack1_frame[i].total_frame=mda_group[mda_index].total_frame[motion_index+i];
			fight_npc_group[index].attack1_frame[i].motion=motion_index;
			
			motion_index=8;						// 8 ~ 11
			fight_npc_group[index].attack2_frame[i].now_frame=0;
			fight_npc_group[index].attack2_frame[i].total_frame=mda_group[mda_index].total_frame[motion_index+i];
			fight_npc_group[index].attack2_frame[i].motion=motion_index;
			
			motion_index=8;						// 8 ~ 11
			fight_npc_group[index].attack3_frame[i].now_frame=0;
			fight_npc_group[index].attack3_frame[i].total_frame=mda_group[mda_index].total_frame[motion_index+i];
			fight_npc_group[index].attack3_frame[i].motion=motion_index;
			
			motion_index=12;					// 12 ~ 15
			fight_npc_group[index].magic_frame[i].now_frame=0;
			fight_npc_group[index].magic_frame[i].total_frame=mda_group[mda_index].total_frame[motion_index+i];
			fight_npc_group[index].magic_frame[i].motion=motion_index;
			
			motion_index=12;					// 12 ~ 15
			fight_npc_group[index].special_frame[i].now_frame=0;
			fight_npc_group[index].special_frame[i].total_frame=mda_group[mda_index].total_frame[motion_index+i];
			fight_npc_group[index].special_frame[i].motion=motion_index;
			
			motion_index=20;					// 20 ~ 23
			fight_npc_group[index].dead_frame[i].now_frame=0;
			fight_npc_group[index].dead_frame[i].total_frame=mda_group[mda_index].total_frame[motion_index+i];
			fight_npc_group[index].dead_frame[i].motion=motion_index;
			
			break;
		}
	}

	
}


SLONG add_map_npc_group(void)
{
	MAP_NPC_GROUP *temp_map_npc_group=NULL;
	SLONG new_total_map_npc_group;
	SLONG i;
	SLONG new_index;
	
// --- ( 0 - 0 ) alloc new buffer
	new_total_map_npc_group=total_map_npc+1;
	new_index=total_map_npc;
	

	temp_map_npc_group=(MAP_NPC_GROUP *)malloc(sizeof(MAP_NPC_GROUP_STRUCT)*new_total_map_npc_group);
	if(temp_map_npc_group==NULL)
	{
		display_error_message((UCHR *)MSG_MEMORY_ALLOC_ERROR,true);
		return(TTN_NOT_OK);
	}
	
// --- ( 0 - 1 ) Copy to temp
	for(i=0;i<total_map_npc;i++)
		memcpy(&temp_map_npc_group[i],&map_npc_group[i],sizeof(struct MAP_NPC_GROUP_STRUCT));
	
// --- ( 0 - 2 ) Free old mda_group
	if(map_npc_group!=NULL)
		free(map_npc_group);
	map_npc_group=NULL;
	
// --- ( 0 - 3 ) Copy temp to mda_group
	total_map_npc=new_total_map_npc_group;
	map_npc_group=(MAP_NPC_GROUP *)malloc(sizeof(MAP_NPC_GROUP_STRUCT)*total_map_npc);
	if(map_npc_group==NULL)
	{
		display_error_message((UCHR *)MSG_MEMORY_ALLOC_ERROR,true);
		return(TTN_NOT_OK);
	}
	for(i=0;i<total_map_npc;i++)
		memcpy(&map_npc_group[i],&temp_map_npc_group[i],sizeof(struct MAP_NPC_GROUP_STRUCT));
	
	free(temp_map_npc_group);

	return(new_index);
}


void free_map_npc_group(void)
{

	if(map_npc_group==NULL)return;
	free(map_npc_group);
	total_map_npc=0;
	map_npc_group=NULL;
	
}



void clear_all_map_npc(void)
{
	SLONG i;
	SLONG handle;
	SLONG check_idx;

	for(i=0;i<total_map_npc;i++)
	{
		handle=map_npc_group[i].handle;
		check_idx=get_mda_index(handle);
		if(check_idx>=0)
			mda_group_close(&handle);
	}
		

	free_map_npc_group();
}


SLONG get_map_npc_index(ULONG user_id)
{
	SLONG i;
	
	for(i=0;i<total_map_npc;i++)
	{
		if(map_npc_group[i].npc_info.id==user_id)
			return(i);
	}
	return(TTN_NOT_OK);
}


void check_need_del_mda_handle(SLONG map_npc_group_index)
{
SLONG i;
SLONG check_handle;
SLONG check_count;

	check_handle=map_npc_group[map_npc_group_index].handle;
	check_count=0;
	for(i=0;i<total_map_npc;i++)
	{
		if(i==map_npc_group_index)continue;
		if(map_npc_group[i].handle==check_handle)
			check_count++;
	}

	if(check_count>0)return;
	mda_group_close(&check_handle);

}


void delete_map_npc_group(ULONG user_id)
{
	SLONG map_npc_group_index;
	SLONG new_total_map_npc;
	SLONG i,j;
	MAP_NPC_GROUP *temp_map_npc_group=NULL;
	

// --- ( 0 - 1 ) get map_npc_group_index
	map_npc_group_index=get_map_npc_index(user_id);
	if(map_npc_group_index<0)return;

	if(total_map_npc<0)return;

	check_need_del_mda_handle(map_npc_group_index);		// Delete MDA GROUOP HANDLE

// --- ( 0 - 0 ) Check ֻ��1 ��
	if(total_map_npc==1)
	{
		free_map_npc_group();
		total_map_npc=0;
		return;
	}
	
// ---- 
	new_total_map_npc=total_map_npc-1;
	temp_map_npc_group=(MAP_NPC_GROUP *)malloc(sizeof(struct MAP_NPC_GROUP_STRUCT)*new_total_map_npc);
	if(temp_map_npc_group==NULL)
	{
		display_error_message((UCHR *)MSG_MEMORY_ALLOC_ERROR,true);
		return;
	}
	
// --- ( 0 - 2 ) Copy to temp
	for(i=0,j=0;i<total_map_npc;i++)
	{
		if(i==map_npc_group_index)
		{
			continue;
		}
		memcpy(&temp_map_npc_group[j],&map_npc_group[i],sizeof(struct MAP_NPC_GROUP_STRUCT));
		j++;
	}
	
// --- ( 0 - 3 ) Free old mda_group
	if(map_npc_group!=NULL)
		free(map_npc_group);
	map_npc_group=NULL;
	
// --- ( 2 - 1 ) Copy temp to mda_group
	total_map_npc=new_total_map_npc;
	map_npc_group=(MAP_NPC_GROUP *)malloc(sizeof(struct MAP_NPC_GROUP_STRUCT)*total_map_npc);
	if(map_npc_group==NULL)
	{
		display_error_message((UCHR *)MSG_MEMORY_ALLOC_ERROR,true);
		return;
	}
	for(i=0;i<total_map_npc;i++)
		memcpy(&map_npc_group[i],&temp_map_npc_group[i],sizeof(struct MAP_NPC_GROUP_STRUCT));
	
	free(temp_map_npc_group);
	
}



void clear_npc_layer_sort(void)
{
	if(npc_layer_sort!=NULL)
		free(npc_layer_sort);
	npc_layer_sort=NULL;
	total_npc_layer=0;
}


void draw_npc_layer(BMP *buffer)
{
	SLONG i;
	SLONG img_x,img_y;
	SLONG mda_handle;
	SLONG mda_index;
	SLONG mda_width,mda_height;

// --- ( 0 - 0 )  Clear
	clear_npc_layer_sort();

// --- ( 0 - 1 ) Add Npc Data & MAP_EFFECT & EXTRA_MDA
	for(i=0;i<total_map_npc;i++)
		add_npc_layer_sort(SORT_TYPE_NPC,i,map_npc_group[i].npc_info.y);

	for(i=0;i<map_base_data.total_map_effect;i++ )
	{
		if(map_base_data.map_effect_data[i].times==MAP_EFFECT_IMG)
		{
			mda_handle=map_base_data.map_effect_data[i].handle;
			mda_index=get_mda_index(mda_handle);
			if(mda_index<0)
				continue;
			mda_width=map_base_data.map_effect_data[i].img_xl;
			mda_height=map_base_data.map_effect_data[i].img_yl;
			
			img_x=map_base_data.map_effect_data[i].x-mda_group[mda_index].img256_head->center_x;
			img_y=map_base_data.map_effect_data[i].y-mda_group[mda_index].img256_head->center_y;
			
			if(check_box_intersect(map_control.start_x-400,
				map_control.start_y-300,
				map_control.start_x+SCREEN_WIDTH+400,
				map_control.start_y+SCREEN_HEIGHT+300,
				img_x,
				img_y,
				img_x+map_base_data.map_effect_data[i].img_xl,
				img_y+map_base_data.map_effect_data[i].img_yl)==TTN_OK)	
			{
				add_npc_layer_sort(SORT_TYPE_MDA,i,map_base_data.map_effect_data[i].y);
			}
		}
	}

	for(i=0;i<MAX_EXTRA_MDA;i++)
	{
		if(extra_mda_data[i].level!=EXTRA_MIDDLE_LAYER)continue;

		if(check_box_intersect(map_control.start_x,
			map_control.start_y,
			map_control.start_x+SCREEN_WIDTH,
			map_control.start_y+SCREEN_HEIGHT,
			
			extra_mda_data[i].display_x-extra_mda_data[i].center_x,
			extra_mda_data[i].display_y-extra_mda_data[i].center_y,

			extra_mda_data[i].display_x-extra_mda_data[i].center_x+extra_mda_data[i].xl,
			extra_mda_data[i].display_y-extra_mda_data[i].center_y+extra_mda_data[i].yl)==TTN_OK)
		{
			add_npc_layer_sort(SORT_TYPE_EXTRA,i,extra_mda_data[i].display_y);
		}
			
	}



// --- ( 1 - 0 ) Begin sort
	sort_npc_layer();


	draw_button_extra_mda(buffer);
// --- ( 2 - 0 ) Begin Draw NPC LAYER
	for(i=0;i<total_npc_layer;i++)
	{
		switch(npc_layer_sort[i].type)
		{
		case SORT_TYPE_NPC:
			map_control.draw_mask=true;
			map_control.mask_npc=npc_layer_sort[i].idx;
			draw_single_npc(npc_layer_sort[i].idx,buffer);
			map_control.draw_mask=false;
			break;
		case SORT_TYPE_MDA:
			draw_single_mda(npc_layer_sort[i].idx,buffer);
			break;
		case SORT_TYPE_EXTRA:
			draw_single_extra_mda(npc_layer_sort[i].idx,buffer);
			break;
		default:
			continue;
		}
	}

	draw_top_extra_mda(buffer);

	if(system_control.point_npc_idx>=0)
		display_single_name(system_control.point_npc_idx,buffer);


	if(system_image.icon_timer<system_control.system_timer)
	{
		system_image.icon_timer=system_control.system_timer+SYSTEM_MAIN_DELAY;
	}	


}


void sort_npc_layer(void)
{
	SLONG i,j;
	NPC_LAYER_SORT temp_npc_layer_sort;
	
	if(total_npc_layer<1)return;
	
	for(i=0;i<total_npc_layer-1;i++)
	{
		for(j=0;j<total_npc_layer-1;j++)
		{
			if(npc_layer_sort[j].y>npc_layer_sort[j+1].y)
			{
				temp_npc_layer_sort.type=npc_layer_sort[j].type;
				temp_npc_layer_sort.idx=npc_layer_sort[j].idx;
				temp_npc_layer_sort.y=npc_layer_sort[j].y;
				
				npc_layer_sort[j].type=npc_layer_sort[j+1].type;
				npc_layer_sort[j].idx=npc_layer_sort[j+1].idx;
				npc_layer_sort[j].y=npc_layer_sort[j+1].y;
				
				npc_layer_sort[j+1].type=temp_npc_layer_sort.type;
				npc_layer_sort[j+1].idx=temp_npc_layer_sort.idx;
				npc_layer_sort[j+1].y=temp_npc_layer_sort.y;
			}
		}
	}
	
	
	
}


void add_npc_layer_sort(SLONG type,SLONG idx,SLONG y)
{
	NPC_LAYER_SORT *temp_npc_layer_sort=NULL;
	SLONG new_max_npc_layer_sort;
	SLONG new_index;
	SLONG i;
	
// ---- alloc new buffer
	new_max_npc_layer_sort=total_npc_layer+1;
	new_index=total_npc_layer;
	
	temp_npc_layer_sort=(NPC_LAYER_SORT *)malloc(sizeof(NPC_LAYER_SORT_STRUCT)*new_max_npc_layer_sort);
	if(temp_npc_layer_sort==NULL)
	{
		display_error_message((UCHR *)"Memory Error (FIGHT_NPC_SORT)",true);
		return;
	}
	
// ---- Copy to temp
	for(i=0;i<total_npc_layer;i++)
		memcpy(&temp_npc_layer_sort[i],&npc_layer_sort[i],sizeof(struct NPC_LAYER_SORT_STRUCT));
	
// ---- Free old mda_group
	if(npc_layer_sort!=NULL)
		free(npc_layer_sort);
	npc_layer_sort=NULL;
	
// ---- Copy temp to new
	total_npc_layer=new_max_npc_layer_sort;
	npc_layer_sort=(NPC_LAYER_SORT *)malloc(sizeof(NPC_LAYER_SORT_STRUCT)*total_npc_layer);
	if(npc_layer_sort==NULL)
	{
		display_error_message((UCHR *)"Memory Error (NPC_SORT)",true);
		return;
	}
	for(i=0;i<total_npc_layer;i++)
		memcpy(&npc_layer_sort[i],&temp_npc_layer_sort[i],sizeof(struct NPC_LAYER_SORT_STRUCT));
	
	free(temp_npc_layer_sort);
	// ---- Initial new mda_group parameter
	npc_layer_sort[new_index].type=type;
	npc_layer_sort[new_index].idx=idx;
	npc_layer_sort[new_index].y=y;
	
}


void draw_single_mda(SLONG idx,BMP *buffer)
{
	SLONG real_x,real_y;
	SLONG check_x,check_y;
	SLONG screen_x,screen_y;
	SLONG mda_handle,mda_index;
	SLONG mda_width,mda_height;
	SLONG mda_command;
	UCHR command_str[80];
	SLONG now_frame;
	

	
// --- Get Base Data 
	mda_handle=map_base_data.map_effect_data[idx].handle;
	mda_index=get_mda_index(mda_handle);
	if(mda_index<0)return;
	mda_width=map_base_data.map_effect_data[idx].img_xl;
	mda_height=map_base_data.map_effect_data[idx].img_yl;

	check_x=map_base_data.map_effect_data[idx].x-mda_group[mda_index].img256_head->center_x;
	check_y=map_base_data.map_effect_data[idx].y-mda_group[mda_index].img256_head->center_y;
	real_x=map_base_data.map_effect_data[idx].x;
	real_y=map_base_data.map_effect_data[idx].y;
	screen_x=real_x-map_control.start_x;
	screen_y=real_y-map_control.start_y;

	

	mda_command=MDA_COMMAND_ASSIGN;
	if(map_base_data.map_effect_data[idx].delay==1)
		mda_command=mda_command|MDA_COMMAND_HIGHTLIGHT;

	now_frame=map_base_data.map_effect_data[idx].now_frame;
	

	map_layer_touch.flag=false;
	if(check_need_npc_mask(check_x,check_y,mda_width,mda_height,mda_group[mda_index].img256_head->center_y)==TTN_OK)
		map_layer_touch.flag=true;
	map_layer_touch.character=false;
	
	strcpy((char *)command_str,"MainCommand0");
	mda_group_exec(mda_handle,command_str,screen_x,screen_y,buffer,mda_command,0,0,now_frame);
	map_layer_touch.flag=false;

	if(system_control.system_timer>=map_base_data.map_effect_data[idx].idle_timer)
	{
		map_base_data.map_effect_data[idx].now_frame++;
		if(map_base_data.map_effect_data[idx].now_frame>=map_base_data.map_effect_data[idx].total_frame)
			map_base_data.map_effect_data[idx].now_frame=0;
		map_base_data.map_effect_data[idx].idle_timer=system_control.system_timer+SYSTEM_MAIN_DELAY;
		
	}

// ---
//	sprintf((char *)print_rec,"%d-%d",map_base_data.map_effect_data[idx].now_frame,map_base_data.map_effect_data[idx].total_frame);
//	print16(screen_x,screen_y+32,print_rec,COPY_PUT,buffer);

}


void display_single_name(SLONG idx,BMP *buffer)
{
	SLONG screen_x,screen_y;
	SLONG npc_real_x,npc_real_y;
	SLONG npc_mda_handle,npc_mda_index;
	SLONG npc_width,npc_height;
	SLONG x,y;

// --- Get Base Data 
	if(idx<0)return;
	npc_real_x=map_npc_group[idx].npc_info.x;
	npc_real_y=map_npc_group[idx].npc_info.y;
	screen_x=npc_real_x-map_control.start_x;
	screen_y=npc_real_y-map_control.start_y;
	npc_mda_handle=map_npc_group[idx].handle;
	npc_mda_index=get_mda_index(npc_mda_handle);
	if(npc_mda_index<0)return;
	npc_width=mda_group[npc_mda_index].img256_head->xlen;
	npc_height=mda_group[npc_mda_index].img256_head->ylen;


	y=screen_y+20;
	if((map_npc_group[idx].npc_type==NPC_CHARACTER_TYPE)||		// Player
		(map_npc_group[idx].npc_type==NPC_BAOBAO_TYPE))
	{
		if(strlen(map_npc_group[idx].npc_info.title)>0)
		{
			x=screen_x-strlen(map_npc_group[idx].npc_info.title)*8/2;
			set_word_color(0,color_control.black);
			print16(x+1,y+1,map_npc_group[idx].npc_info.title,PEST_PUT,buffer);
			
			if(system_control.point_npc_idx==idx)
			{
				set_word_color(0,map_control.point_color);
				print16(x,y,map_npc_group[idx].npc_info.title,PEST_PUT,buffer);
			}
			else
			{
				set_word_color(0,map_control.character_title_color);
				print16(x,y,map_npc_group[idx].npc_info.title,PEST_PUT,buffer);
			}
			y+=16;
		}
	}
	x=screen_x-strlen(map_npc_group[idx].npc_info.name)*8/2;

	set_word_color(0,color_control.black);
	print16(x+1,y+1,map_npc_group[idx].npc_info.name,PEST_PUT,buffer);
	if(system_control.point_npc_idx==idx)
	{
		set_word_color(0,map_control.point_color);
		print16(x,y,map_npc_group[idx].npc_info.name,PEST_PUT,buffer);
	}
	else
	{
		if((map_npc_group[idx].npc_type==NPC_CHARACTER_TYPE)||			// Player
		   (map_npc_group[idx].npc_type==NPC_BAOBAO_TYPE))
		{
			switch(map_npc_group[idx].npc_info.life_base)
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
		}
		else
			set_word_color(0,map_control.npc_name_color);
		print16(x,y,map_npc_group[idx].npc_info.name,PEST_PUT,buffer);
	}
		

	set_word_color(0,color_control.white);
	

}



void draw_single_npc(SLONG idx,BMP *buffer)
{
	SLONG screen_x,screen_y;
	SLONG npc_real_x,npc_real_y;
	SLONG npc_mda_handle,npc_mda_index;
	SLONG npc_width,npc_height;
	SLONG mda_command;
	UCHR command_str[80];
	SLONG motion_index;
	SLONG frame_index;
	SLONG now_frame;
	SLONG now_dir;
	SLONG x,y;
	SLONG icon_x,icon_y;
	bool change_frame;
	int status;

// --- Get Base Data 
	npc_real_x=map_npc_group[idx].npc_info.x;
	npc_real_y=map_npc_group[idx].npc_info.y;
	screen_x=npc_real_x-map_control.start_x;
	screen_y=npc_real_y-map_control.start_y;
	npc_mda_handle=map_npc_group[idx].handle;
	npc_mda_index=get_mda_index(npc_mda_handle);
	if(npc_mda_index<0)return;
	npc_width=mda_group[npc_mda_index].img256_head->xlen;
	npc_height=mda_group[npc_mda_index].img256_head->ylen;
	
// --- Process frame & motion
	switch(map_npc_group[idx].npc_type)
	{
	case NPC_CHARACTER_TYPE:
		motion_index=map_npc_group[idx].npc_info.motility*8;
		now_dir=map_npc_group[idx].npc_info.dir;
		frame_index=motion_index+map_npc_group[idx].npc_info.dir;
		sprintf((char *)command_str,"MainCommand%d",frame_index);
		
		break;
	case NPC_CPU_TYPE:
	case NPC_BAOBAO_TYPE:
		motion_index=map_npc_group[idx].npc_info.motility*4;
		if(map_npc_group[idx].npc_info.dir>=4)
			now_dir=map_npc_group[idx].npc_info.dir%4;
		else
			now_dir=map_npc_group[idx].npc_info.dir;
		frame_index=motion_index+map_npc_group[idx].npc_info.dir%4;
		sprintf((char *)command_str,"MainCommand%d",frame_index);
		break;
	default:
		return;
	}
	
	
	if(frame_index>=mda_group[npc_mda_index].total_ani_command)
		return;	// MDA out fo range

// -- set effect
	mda_command=MDA_COMMAND_ASSIGN;

	if((system_control.point_npc_idx==idx)&&(map_npc_group[idx].npc_type!=NPC_BAOBAO_TYPE))
	{
		map_control.draw_point_color=true;
		system_palette=point_palette;
	}
	else if(check_cell_shadow(npc_real_x/map_base_data.cell_width,npc_real_y/map_base_data.cell_height)==TTN_OK)
	{
		system_palette=mask_full_palette;
		mda_command=mda_command|MDA_COMMAND_PALETTE_SUB;
	}
		
	now_frame=map_npc_group[idx].npc_mda_control[now_dir].now_frame;
	if(check_need_npc_mask(map_npc_group[idx].npc_info.x-mda_group[npc_mda_index].img256_head->center_x,
		map_npc_group[idx].npc_info.y-mda_group[npc_mda_index].img256_head->center_y,npc_width,npc_height,mda_group[npc_mda_index].img256_head->center_y)==TTN_OK)
	map_layer_touch.flag=true;
	if(idx==system_control.control_npc_idx)
		map_layer_touch.character=true;
	else
		map_layer_touch.character=false;

// --- Draw


	if(map_npc_group[idx].npc_type==NPC_CPU_TYPE)		// CPU
	{
		mda_group_exec(npc_mda_handle,command_str,screen_x,screen_y,buffer,mda_command,0,0,now_frame);
	}
	else if(system_control.control_npc_idx==idx)
	{
		if(base_character_data.data.final.now_hp<base_character_data.data.final.max_hp/5)
			mda_command=mda_command|MDA_COMMAND_PALETTE_GRAY;
		mda_group_exec(npc_mda_handle,command_str,screen_x,screen_y,buffer,mda_command,0,0,now_frame);
	}
	else
	{
		switch(game_control.npc_display_flag)
		{
		case 0: case 2:
			mda_group_exec(npc_mda_handle,command_str,screen_x,screen_y,buffer,mda_command,0,0,now_frame);
			break;
		}
	}

	map_layer_touch.flag=false;
	map_control.draw_point_color=false;
	


	
	switch(map_npc_group[idx].npc_info.motility)
	{
	case NPC_MOTION_STANDBY1:
	case NPC_MOTION_STANDBY2:
		if(system_control.system_timer>=map_npc_group[idx].npc_timer)
		{
			map_npc_group[idx].npc_timer=system_control.system_timer+SYSTEM_MAIN_DELAY;
			change_npc_next_frame(idx,now_dir);
		}
		break;
	case NPC_MOTION_WALK:
	case NPC_MOTION_RUN:
		break;
	}


// --- display status icon
	if(map_npc_group[idx].npc_type==NPC_CHARACTER_TYPE)		// Player
	{

		change_frame=false;
		if(system_image.icon_timer<system_control.system_timer)
			change_frame=true;


		status=map_npc_group[idx].npc_info.status;
		if(status&NPC_STATUS_LEADER)
		{
			icon_x=screen_x;
			icon_y=screen_y-mda_group[npc_mda_index].img256_head->center_y-8;
			if(change_frame)
				mda_command=MDA_COMMAND_LOOP|MDA_COMMAND_NEXT_FRAME;
			else
				mda_command=MDA_COMMAND_LOOP;
			
			mda_group_exec(system_image.status_handle,(UCHR *)"MainCommand1",icon_x,icon_y,screen_buffer,mda_command,0,0,0);			

		}
		if(status&NPC_STATUS_FIGHT)
		{
			icon_x=screen_x;
			icon_y=screen_y-mda_group[npc_mda_index].img256_head->center_y;
			if(change_frame)
				mda_command=MDA_COMMAND_LOOP|MDA_COMMAND_NEXT_FRAME;
			else
				mda_command=MDA_COMMAND_LOOP;
			
			mda_group_exec(system_image.status_handle,(UCHR *)"MainCommand2",icon_x,icon_y,screen_buffer,mda_command,0,0,0);			
		}
		if(status&NPC_STATUS_BORN)
		{
			icon_x=screen_x;
			icon_y=screen_y;
			if(change_frame)
				mda_command=MDA_COMMAND_LOOP|MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_HIGHTLIGHT;
			else
				mda_command=MDA_COMMAND_LOOP|MDA_COMMAND_HIGHTLIGHT;
			
			if(mda_group_exec(system_image.status_handle,(UCHR *)"MainCommand0",icon_x,icon_y,screen_buffer,mda_command,0,0,0)==MDA_EXEC_LAST)
				map_npc_group[idx].npc_info.status=map_npc_group[idx].npc_info.status&NPC_STATUS_BORN_MASK;
		}
		if(status&NPC_STATUS_FULL)
		{
			icon_x=screen_x;
			icon_y=screen_y;
			if(change_frame)
				mda_command=MDA_COMMAND_LOOP|MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_HIGHTLIGHT;
			else
				mda_command=MDA_COMMAND_LOOP|MDA_COMMAND_HIGHTLIGHT;
			
			if(mda_group_exec(system_image.full_handle,(UCHR *)"MainCommand0",icon_x,icon_y,screen_buffer,mda_command,0,0,0)==MDA_EXEC_LAST)
				map_npc_group[idx].npc_info.status=map_npc_group[idx].npc_info.status&NPC_STATUS_FULL_MASK;
		}
		
	}


// ---- Display name 
	if(debug_info.map_debug)
		put_bar(screen_x,screen_y,2,2,color_control.red,buffer);


	switch(game_control.npc_display_flag)
	{
	case 2: case 3:
		if((map_npc_group[idx].npc_type==NPC_CHARACTER_TYPE)&&		// Player
			(system_control.control_npc_idx!=idx))					// control
			return;
		if(map_npc_group[idx].npc_type==NPC_BAOBAO_TYPE)		// baobao
			return;
		break;
	}
	


	y=screen_y+20;
	if((map_npc_group[idx].npc_type==NPC_CHARACTER_TYPE)||		// Player or baobao
		(map_npc_group[idx].npc_type==NPC_BAOBAO_TYPE))
	{
		if(strlen(map_npc_group[idx].npc_info.title)>0)
		{
			x=screen_x-strlen(map_npc_group[idx].npc_info.title)*8/2;
			set_word_color(0,color_control.black);
			print16(x+1,y+1,map_npc_group[idx].npc_info.title,PEST_PUT,buffer);
			
			if(system_control.point_npc_idx==idx)
			{
				set_word_color(0,map_control.point_color);
				print16(x,y,map_npc_group[idx].npc_info.title,PEST_PUT,buffer);
			}
			else
			{
				if(map_npc_group[idx].npc_type==NPC_CHARACTER_TYPE)
					set_word_color(0,map_control.character_title_color);
				else
					set_word_color(0,color_control.pink);
				print16(x,y,map_npc_group[idx].npc_info.title,PEST_PUT,buffer);
			}
			y+=16;
		}
	}
	x=screen_x-strlen(map_npc_group[idx].npc_info.name)*8/2;

	set_word_color(0,color_control.black);
	print16(x+1,y+1,map_npc_group[idx].npc_info.name,PEST_PUT,buffer);
	if(system_control.point_npc_idx==idx)
	{
		set_word_color(0,map_control.point_color);
		print16(x,y,map_npc_group[idx].npc_info.name,PEST_PUT,buffer);
	}
	else
	{
		if((map_npc_group[idx].npc_type==NPC_CHARACTER_TYPE)||			// Player
			(map_npc_group[idx].npc_type==NPC_BAOBAO_TYPE))
		{
			switch(map_npc_group[idx].npc_info.life_base)
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
		}
		else
			set_word_color(0,map_control.npc_name_color);
		print16(x,y,map_npc_group[idx].npc_info.name,PEST_PUT,buffer);
	}
		

	set_word_color(0,color_control.white);
	
	
/*
SLONG i;
for(i=0;i<8;i++)
{
	sprintf((char *)print_rec,"%d-%d %d",map_npc_group[idx].npc_mda_control[i].now_frame,map_npc_group[idx].npc_mda_control[i].total_frame,now_dir);
	print16(screen_x,screen_y+32+i*16,print_rec,COPY_PUT,buffer);
}
*/	


}




void draw_single_extra_mda(SLONG idx,BMP *buffer)
{
	SLONG real_x,real_y;
	SLONG check_x,check_y;
	SLONG screen_x,screen_y;
	SLONG mda_handle,mda_index;
	SLONG mda_width,mda_height;
	SLONG mda_command;
	UCHR command_str[80];
	SLONG ret_val;
	
	

	if(!extra_mda_data[idx].display)return;
	if(extra_mda_data[idx].handle<0)return;
// --- Get Base Data 
	mda_handle=extra_mda_data[idx].handle;

	mda_index=get_mda_index(mda_handle);
	if(mda_index<0)return;
	mda_width=extra_mda_data[idx].xl;
	mda_height=extra_mda_data[idx].yl;
	
	check_x=extra_mda_data[idx].display_x-extra_mda_data[idx].center_x;
	check_y=extra_mda_data[idx].display_y-extra_mda_data[idx].center_y;

	real_x=extra_mda_data[idx].display_x;
	real_y=extra_mda_data[idx].display_y;

	screen_x=real_x-map_control.start_x;
	screen_y=real_y-map_control.start_y;
	
	mda_command=NULL;
	if(extra_mda_data[idx].hight_light_flag)
		mda_command=MDA_COMMAND_HIGHTLIGHT;
	if(extra_mda_data[idx].loop)
		mda_command=mda_command|MDA_COMMAND_LOOP;

	if(system_control.system_timer>=extra_mda_data[idx].timer)
	{
		mda_command=mda_command|MDA_COMMAND_NEXT_FRAME;
		extra_mda_data[idx].timer=system_control.system_timer+SYSTEM_MAIN_DELAY;
		
	}
	
	
	map_layer_touch.flag=false;
	if(check_need_npc_mask(check_x,check_y,mda_width,mda_height,mda_group[mda_index].img256_head->center_y)==TTN_OK)
		map_layer_touch.flag=true;
	map_layer_touch.character=false;
	strcpy((char *)command_str,(char *)extra_mda_data[idx].command);
	ret_val=mda_group_exec(mda_handle,command_str,screen_x,screen_y,buffer,mda_command,0,0,0);
	map_layer_touch.flag=false;
	
	if(ret_val==MDA_EXEC_LAST)
	{
		extra_mda_data[idx].play_end=true;			
		if(extra_mda_data[idx].auto_close)			// �Զ��ر�
		{
			if(!extra_mda_data[idx].loop)			// ������� loop
			{
				mda_group_close(&extra_mda_data[idx].handle);		// �ر�
				extra_mda_data[idx].handle=-1;
			}
		}
		else
		{
			if(!extra_mda_data[idx].loop)			// ������� loop
			{
				extra_mda_data[idx].display=false;
			}
				
		}

	}


	
}






SLONG check_need_npc_mask(SLONG npc_x,SLONG npc_y,SLONG npc_width,SLONG npc_height,SLONG cc_y)
{
	RECT npc_rect;
	RECT image_rect;
	SLONG center_y;
	SLONG i;
	
/*	npc_rect.left=npc_x-npc_width/2;
	npc_rect.top=npc_y-npc_height;
	npc_rect.right=npc_x+npc_width/2;
	npc_rect.bottom=npc_y;
*/	

	npc_rect.left=npc_x;
	npc_rect.top=npc_y;
	npc_rect.right=npc_x+npc_width;
	npc_rect.bottom=npc_y+npc_height;



	map_layer_touch.total=0;
	for(i=1;i<map_base_data.total_map_layer;i++)
	{
		if((map_base_data.map_layer_data[i].type==IMG_LAYER_MIDDLE)||
			(map_base_data.map_layer_data[i].type==IMG_LAYER_TOP))
		{
			image_rect.left=map_base_data.map_layer_data[i].orig_x;
			image_rect.top=map_base_data.map_layer_data[i].orig_y;
			image_rect.right=image_rect.left+map_base_data.map_layer_data[i].xlen;
			image_rect.bottom=image_rect.top+map_base_data.map_layer_data[i].ylen;
			center_y=map_base_data.map_layer_data[i].orig_y+map_base_data.map_layer_data[i].center_y;
			
			if(map_base_data.map_layer_data[i].type==IMG_LAYER_MIDDLE)
			{
				if((check_box_intersect(npc_rect.left,
					npc_rect.top,
					npc_rect.right,
					npc_rect.bottom,
					image_rect.left,
					image_rect.top,
					image_rect.right,
					image_rect.bottom)==TTN_OK)&&
					(npc_y+npc_height<center_y))
				{
					map_layer_touch.idx[map_layer_touch.total]=i;
					map_layer_touch.total++;

				}
			}
			else if(map_base_data.map_layer_data[i].type==IMG_LAYER_TOP)
			{
				if(check_box_intersect(npc_rect.left,
					npc_rect.top,
					npc_rect.right,
					npc_rect.bottom,
					image_rect.left,
					image_rect.top,
					image_rect.right,
					image_rect.bottom)==TTN_OK)
				{
					map_layer_touch.idx[map_layer_touch.total]=i;
					map_layer_touch.total++;
				}
				
			}
			
		}
	}
	
	if(map_layer_touch.total>0)return(TTN_OK);
	
	return(TTN_NOT_OK);		
	
	
}



void change_npc_next_frame(SLONG idx,SLONG now_dir)
{

	if((map_npc_group[idx].npc_type==NPC_CPU_TYPE)||(map_npc_group[idx].npc_type==NPC_BAOBAO_TYPE))
		now_dir=now_dir%4;
	map_npc_group[idx].npc_mda_control[now_dir].now_frame++;
	if(map_npc_group[idx].npc_mda_control[now_dir].now_frame>=map_npc_group[idx].npc_mda_control[now_dir].total_frame)
		map_npc_group[idx].npc_mda_control[now_dir].now_frame=0;
	
}


SLONG npc_auto_follow_move(SLONG idx)
{
	SLONG follow_count;
	ULONG follow_id;
	SLONG map_npc_idx;
	SLONG sx,sy,tx,ty;
	SLONG dir;
	SLONG range;
	SLONG ret_val;
	double offset,x_offset,y_offset;


	if(system_control.system_timer<map_npc_group[idx].npc_move_timer)
		return(TTN_OK);
	
	follow_id=map_npc_group[idx].follow_id;
	map_npc_idx=get_map_npc_index(follow_id);
	if(map_npc_idx<0)									// �������뵱ǰ
	{
		return(TTN_NOT_OK);
	}


// -------- Check is out of range 

	

	sx=map_npc_group[idx].npc_info.x;				// ����λ��
	sy=map_npc_group[idx].npc_info.y;
	tx=map_npc_group[map_npc_idx].npc_info.x;		// ����Ŀ��λ��
	ty=map_npc_group[map_npc_idx].npc_info.y;

	x_offset=abs(sx-tx);
	y_offset=abs(sy-ty);
	offset=sqrt(x_offset*x_offset+y_offset*y_offset) ;

	if(offset<FOLLOW_JUMP_RANGE)					// �ڸ����������
	{

		if(system_control.system_timer<map_npc_group[idx].follow_timer)
		{
			ret_val=npc_auto_path_move(idx);
			return(ret_val);
			
		}
		map_npc_group[idx].follow_timer=system_control.system_timer+PLAYER_MOVE_CHECK_TIMER;
		

		follow_count=map_npc_group[idx].follow_range;
		sx=map_npc_group[map_npc_idx].npc_info.x;
		sy=map_npc_group[map_npc_idx].npc_info.y;
		dir=map_npc_group[map_npc_idx].npc_info.dir;
		range=FOLLOW_NEAR_RANGE+(FOLLOW_NEAR_RANGE/2)*follow_count;
		get_near_position(sx,sy,range,dir,&tx,&ty);	// ȡ�ø���Ŀ��
		map_npc_group[idx].npc_info.tx=tx;
		map_npc_group[idx].npc_info.ty=ty;


		if((abs(map_npc_group[idx].npc_info.x-map_npc_group[idx].npc_info.tx)<=32) &&
			(abs(map_npc_group[idx].npc_info.y-map_npc_group[idx].npc_info.ty)<=16))
		{

			if((map_npc_group[idx].npc_info.motility==NPC_MOTION_WALK)||
				(map_npc_group[idx].npc_info.motility==NPC_MOTION_RUN))
				change_npc_motion(idx,NPC_MOTION_STANDBY1);

			return(TTN_OK);
		}
		
		ret_val=npc_auto_path_move(idx);

	}
	else											// �뿪�������
	{
		
		follow_count=map_npc_group[idx].follow_range;
		sx=map_npc_group[map_npc_idx].npc_info.x;
		sy=map_npc_group[map_npc_idx].npc_info.y;
		dir=map_npc_group[map_npc_idx].npc_info.dir;
		range=FOLLOW_NEAR_RANGE+(FOLLOW_NEAR_RANGE/2)*follow_count;
		get_near_position(sx,sy,range,dir,&tx,&ty);	// ȡ�ø���Ŀ��
		
		map_npc_group[idx].npc_info.tx=tx;			// ֱ����ҫ
		map_npc_group[idx].npc_info.ty=ty;
		map_npc_group[idx].npc_info.x=tx;
		map_npc_group[idx].npc_info.y=ty;

		ret_val=TTN_OK;
	}
	
	return(ret_val);
}




SLONG npc_auto_path_move(SLONG map_npc_group_index)
{
	SLONG ret_val;
	SLONG move_step;
	SLONG move_dir;


	int target_x,target_y;
	int dir_x,dir_y;
	
	// --- ( 0 - 0 ) Check is Move End or timer 
	if(system_control.system_timer<map_npc_group[map_npc_group_index].npc_move_timer)
		return(TTN_OK);

	map_npc_group[map_npc_group_index].npc_move_timer=system_control.system_timer+NPC_PATH_MOVE_SPEED;

	
	if((map_npc_group[map_npc_group_index].npc_info.x==map_npc_group[map_npc_group_index].npc_info.tx)&&
		(map_npc_group[map_npc_group_index].npc_info.y==map_npc_group[map_npc_group_index].npc_info.ty))
	{
		if((map_npc_group[map_npc_group_index].npc_info.motility==NPC_MOTION_WALK)||
			(map_npc_group[map_npc_group_index].npc_info.motility==NPC_MOTION_RUN))
			change_npc_motion(map_npc_group_index,NPC_MOTION_STANDBY1);
		return(TTN_OK);
	}
	
	// --- ( 0 - 1 ) Find
	if(map_npc_group[map_npc_group_index].npc_info.speed==0)	// ��·
		move_step=STAND_MOVE_STEP;
	else
		move_step=STAND_MOVE_STEP*2;
	
	target_x=(int)map_npc_group[map_npc_group_index].npc_info.tx;
	target_y=(int)map_npc_group[map_npc_group_index].npc_info.ty;
	
	adjust_pos(target_x,target_y,&map_bar_layer);
	map_npc_group[map_npc_group_index].npc_info.tx=target_x;
	map_npc_group[map_npc_group_index].npc_info.ty=target_y;


//ULONG start_timer;
//ULONG end_timer;
//
//start_timer=GetTickCount();

	ret_val=find_path_b(map_npc_group[map_npc_group_index].npc_info.x,
		map_npc_group[map_npc_group_index].npc_info.y,
		target_x,
		target_y,
		dir_x,
		dir_y,
		move_step,
		&map_bar_layer);


//end_timer=GetTickCount();
//if(end_timer-start_timer>0)
//{
//	sprintf((char *)print_rec,"%d",end_timer-start_timer);
//	display_system_message(print_rec);
//	log_error(1,print_rec);
//}
	
	
// --- ( 0 - 2 )
	switch(ret_val)
	{
	case FIND_OK_REACH:			//  �ѵ��_Ŀ�ĵ�
		change_npc_motion(map_npc_group_index,NPC_MOTION_STANDBY1);
		map_npc_group[map_npc_group_index].npc_idle_timer=system_control.system_timer+NPC_IDLE_BASE;
		map_npc_group[map_npc_group_index].npc_info.tx=map_npc_group[map_npc_group_index].npc_info.x;
		map_npc_group[map_npc_group_index].npc_info.ty=map_npc_group[map_npc_group_index].npc_info.y;
		{
//			postNetMessage(NET_MSG_PLAYER_MOVE_END, 0);			
		}
		break;
	case FIND_OK_CONTINUE:		//  �ҵ�Ŀ�ĵ�,��߀δ���_
	case FIND_OK_TOWARDS:		//  δ�ҵ�Ŀ�ĵ�,ֻ��Ŀ�ĵط����Ƅ�
		if((abs(map_npc_group[map_npc_group_index].npc_info.x-map_npc_group[map_npc_group_index].npc_info.tx)<=map_base_data.cell_width)&&
			(abs(map_npc_group[map_npc_group_index].npc_info.y-map_npc_group[map_npc_group_index].npc_info.ty)<=map_base_data.cell_height))
//			move_dir=map_npc_group[map_npc_group_index].npc_info.dir;
			move_dir=-1;
		else					
			move_dir=get_dir_data(map_npc_group[map_npc_group_index].npc_info.x,map_npc_group[map_npc_group_index].npc_info.y,
			dir_x,dir_y,move_step);

		map_npc_group[map_npc_group_index].npc_info.x=target_x;
		map_npc_group[map_npc_group_index].npc_info.y=target_y;
		
		if(move_dir>=0)
		{
			map_npc_group[map_npc_group_index].npc_info.dir=move_dir;
		}

		if(map_npc_group[map_npc_group_index].npc_info.speed==0)	// ��·
		{
			change_npc_motion(map_npc_group_index,NPC_MOTION_WALK);
		}
		else
		{
			change_npc_motion(map_npc_group_index,NPC_MOTION_RUN);
		}

		change_npc_next_frame(map_npc_group_index,move_dir);
		
		break;
	case FIND_FAIL_NO_PATH:		//  �]�������,��ʼ�c��K�c��������Ӄ�,��ʼ�c�������
	case FIND_FAIL_ERROR:		//  ʧ��
		change_npc_motion(map_npc_group_index,NPC_MOTION_STANDBY1);
		map_npc_group[map_npc_group_index].npc_idle_timer=system_control.system_timer+NPC_IDLE_BASE;
		map_npc_group[map_npc_group_index].npc_info.tx=map_npc_group[map_npc_group_index].npc_info.x;
		map_npc_group[map_npc_group_index].npc_info.ty=map_npc_group[map_npc_group_index].npc_info.y;

		{
//			postNetMessage(NET_MSG_PLAYER_MOVE_END, 0);
		}
		break;
	}


	return(ret_val);
		
}


void change_npc_motion(SLONG idx,SLONG motion)
{

	if((map_npc_group[idx].npc_type==NPC_CPU_TYPE)&&(motion==NPC_MOTION_RUN))	// ǿ�� CPU_NPC ��·
		motion=NPC_MOTION_WALK;

	if((map_npc_group[idx].npc_type==NPC_BAOBAO_TYPE)&&(motion==NPC_MOTION_RUN))	// ǿ�� CPU_NPC ��·
		motion=NPC_MOTION_WALK;
	
	if(map_npc_group[idx].npc_info.motility!=motion)
	{
		map_npc_group[idx].npc_info.motility=motion;
		adjust_npc_frame_data(idx);
	}
}


SLONG get_dir_data(SLONG x,SLONG y,SLONG x1,SLONG y1,SLONG move_step)
{
	SLONG x_offset,y_offset;
	SLONG check_off;
	
	
	check_off=move_step;
	if(check_off<1)check_off=1;
	x_offset=x1-x;
	y_offset=y1-y;
	if((x==x1)&&(y==y1))return(TTN_NOT_OK);
	
	//  =================== check is UP or Down ===================
	if(abs(x_offset)<=check_off)
	{
		if(y>y1)
			return(NPC_DIR_UP);
		else
			return(NPC_DIR_DOWN);
	}
	// ==================== Check is Left or Right ================
	if(abs(y_offset)<=check_off)
	{
		if(x>x1)
			return(NPC_DIR_LEFT);
		else
			return(NPC_DIR_RIGHT);
	}
	
	// ==================== Check is UP_LEFT ================
	if((x1<x)&&(y1<y))
		return(NPC_DIR_UP_LEFT);
	
	// ==================== Check is UP_RIGHT ================
	if((x1>x)&&(y1<y))
		return(NPC_DIR_UP_RIGHT);
	
	// ==================== Check is DOWN_LEFT ================
	if((x1<x)&&(y1>y))
		return(NPC_DIR_DOWN_LEFT);
	
	// ==================== Check is DOWN_RIGHT ================
	return(NPC_DIR_DOWN_RIGHT);
	
}


SLONG check_cell_path(SLONG cell_x,SLONG cell_y)
{
	SLONG offset;
	SLONG flag;
	
	if(map_base_data.map_event_data.event_data==NULL)return(TTN_NOT_OK);
	if(cell_x>=map_base_data.map_event_data.max_cell_x)return(TTN_NOT_OK);
	if(cell_y>=map_base_data.map_event_data.max_cell_y)return(TTN_NOT_OK);
	offset=cell_y*map_base_data.map_event_data.max_cell_x+cell_x;
	flag=map_base_data.map_event_data.event_data[offset]&BITPATH;
	if(flag)return(TTN_OK);
	return(TTN_NOT_OK);
	
}

SLONG check_cell_shadow(SLONG cell_x,SLONG cell_y)
{
	SLONG offset;
	SLONG flag;
	
	if(map_base_data.map_event_data.event_data==NULL)return(TTN_NOT_OK);
	if(cell_x>=map_base_data.map_event_data.max_cell_x)return(TTN_NOT_OK);
	if(cell_y>=map_base_data.map_event_data.max_cell_y)return(TTN_NOT_OK);
	offset=cell_y*map_base_data.map_event_data.max_cell_x+cell_x;
	flag=map_base_data.map_event_data.event_data[offset]&BITSHADE;
	if(flag)return(TTN_OK);
	return(TTN_NOT_OK);
	
}


SLONG check_cell_stop(SLONG cell_x,SLONG cell_y)
{
	SLONG offset;
	SLONG flag;
	
	if(map_base_data.map_event_data.event_data==NULL)return(TTN_NOT_OK);
	if(cell_x>=map_base_data.map_event_data.max_cell_x)return(TTN_NOT_OK);
	if(cell_y>=map_base_data.map_event_data.max_cell_y)return(TTN_NOT_OK);
	offset=cell_y*map_base_data.map_event_data.max_cell_x+cell_x;
	flag=map_base_data.map_event_data.event_data[offset]&BITBAR;
	if(flag)return(TTN_OK);
	return(TTN_NOT_OK);
	
}



SLONG npc_auto_cell_move(SLONG idx)
{
	SLONG cell_x,cell_y;
	SLONG now_dir;
	SLONG old_npc_x,old_npc_y,old_npc_dir;
	ULONG rand_times;
	
	
	if(map_npc_group[idx].npc_type==NPC_CHARACTER_TYPE)return(TTN_OK);
	if(system_control.system_timer<map_npc_group[idx].npc_move_timer)
		return(TTN_OK);
	map_npc_group[idx].npc_move_timer=system_control.system_timer+NPC_CELL_MOVE_SPEED;


	cell_x=map_npc_group[idx].npc_info.x/map_base_data.cell_width;
	cell_y=map_npc_group[idx].npc_info.y/map_base_data.cell_height;
	if(check_cell_path(cell_x,cell_y)!=TTN_OK)return(TTN_OK);
	
	
	switch(map_npc_group[idx].npc_stage)
	{
	case NPC_STAGE_NONE:
		if(system_control.system_timer<map_npc_group[idx].npc_idle_timer)
			return(TTN_NOT_OK);
		rand_times=(rand()%5)*1000+1000;			// 5 ��
		map_npc_group[idx].npc_idle_timer=system_control.system_timer+rand_times;
		switch(rand()%2)
		{
		case 0:										// ������·
			map_npc_group[idx].npc_stage=NPC_STAGE_WALK;
			change_npc_motion(idx,NPC_MOTION_WALK);
			rand_times=(rand()%10)*1000+1000;			// 5 ��
			map_npc_group[idx].npc_idle_timer=system_control.system_timer+rand_times;
			return(TTN_NOT_OK);
		case 1:
			map_npc_group[idx].npc_stage=NPC_STAGE_IDLE;
			change_npc_motion(idx,NPC_MOTION_STANDBY2);
			rand_times=(rand()%5)*1000+1000;			// 5 ��
			map_npc_group[idx].npc_idle_timer=system_control.system_timer+rand_times;
			return(TTN_NOT_OK);
		}
		break;
	case NPC_STAGE_WALK:
		if(system_control.system_timer>map_npc_group[idx].npc_idle_timer)
		{
			map_npc_group[idx].npc_stage=NPC_STAGE_NONE;
			change_npc_motion(idx,NPC_MOTION_STANDBY1);
			rand_times=(rand()%5)*1000+1000;			// 5 ��
			map_npc_group[idx].npc_idle_timer=system_control.system_timer+rand_times;
			return(TTN_NOT_OK);
		}
		break;
	case NPC_STAGE_IDLE:
		if(system_control.system_timer<map_npc_group[idx].npc_idle_timer)
			return(TTN_NOT_OK);
		rand_times=(rand()%5)*1000+1000;			// 5 ��
		map_npc_group[idx].npc_idle_timer=system_control.system_timer+rand_times;
		switch(rand()%2)
		{
		case 0:										// ������·
			map_npc_group[idx].npc_stage=NPC_STAGE_WALK;
			change_npc_motion(idx,NPC_MOTION_WALK);
			rand_times=(rand()%10)*1000+1000;			// 5 ��
			map_npc_group[idx].npc_idle_timer=system_control.system_timer+rand_times;
			return(TTN_NOT_OK);
		case 1:
			map_npc_group[idx].npc_stage=NPC_STAGE_NONE;
			change_npc_motion(idx,NPC_MOTION_STANDBY1);
			rand_times=(rand()%5)*1000+1000;			// 5 ��
			map_npc_group[idx].npc_idle_timer=system_control.system_timer+rand_times;
			return(TTN_NOT_OK);
		}
		break;
	default:
		return(TTN_NOT_OK);
	}

	now_dir=map_npc_group[idx].npc_info.dir;
	change_npc_next_frame(idx,now_dir);
	
	cell_x=map_npc_group[idx].npc_info.x/map_base_data.cell_width;
	cell_y=map_npc_group[idx].npc_info.y/map_base_data.cell_height;
	if(check_cell_path(cell_x,cell_y)!=TTN_OK)return(TTN_NOT_OK);
	
	old_npc_x=map_npc_group[idx].npc_info.x;
	old_npc_y=map_npc_group[idx].npc_info.y;
	old_npc_dir=map_npc_group[idx].npc_info.dir;
	
	now_dir=map_npc_group[idx].npc_info.dir;

	if(npc_move(idx,now_dir)==TTN_OK)
	{
		cell_x=map_npc_group[idx].npc_info.x/map_base_data.cell_width;
		cell_y=map_npc_group[idx].npc_info.y/map_base_data.cell_height;
		if(check_cell_path(cell_x,cell_y)!=TTN_OK)			// recover old data
		{
			map_npc_group[idx].npc_info.x=old_npc_x;
			map_npc_group[idx].npc_info.y=old_npc_y;
			map_npc_group[idx].npc_info.dir=old_npc_dir;
			
		}
		else
		{
			return(TTN_NOT_OK);
		}
	}
	
	// --------- change dir
	now_dir=rand()%20;
	if(now_dir>7)			// Into idle loop
	{
		return(TTN_NOT_OK);
	}
	else
	{
		switch(now_dir)
		{
		case NPC_DIR_UP:
			map_npc_group[idx].npc_info.dir=NPC_DIR_UP_RIGHT;
			break;
		case NPC_DIR_DOWN:
			map_npc_group[idx].npc_info.dir=NPC_DIR_DOWN_LEFT;
			break;
		case NPC_DIR_LEFT:
			map_npc_group[idx].npc_info.dir=NPC_DIR_UP_LEFT;
			break;
		case NPC_DIR_RIGHT:
			map_npc_group[idx].npc_info.dir=NPC_DIR_DOWN_RIGHT;
			break;
		case NPC_DIR_UP_RIGHT:
			map_npc_group[idx].npc_info.dir=NPC_DIR_RIGHT;
			break;
		case NPC_DIR_DOWN_LEFT:
			map_npc_group[idx].npc_info.dir=NPC_DIR_LEFT;
			break;
		case NPC_DIR_UP_LEFT:
			map_npc_group[idx].npc_info.dir=NPC_DIR_UP;
			break;
		case NPC_DIR_DOWN_RIGHT:
			map_npc_group[idx].npc_info.dir=NPC_DIR_DOWN;
			break;
		}
	}

	return(TTN_NOT_OK);	

}




SLONG npc_move(SLONG npc_index,SLONG npc_dir)
{
SLONG x_move_step,y_move_step;
SLONG cell_x,cell_y;
SLONG x_flag,y_flag;


// ---------- Get Info			
		x_move_step=0;
		y_move_step=0;
		switch(npc_dir)		
			{
			case NPC_DIR_UP:
				x_move_step=0;
				y_move_step-=STAND_MOVE_STEP;
				break;
			case NPC_DIR_DOWN:
				x_move_step=0;
				y_move_step+=STAND_MOVE_STEP;
				break;
			case NPC_DIR_LEFT:
				x_move_step-=STAND_MOVE_STEP;
				y_move_step=0;
				break;
			case NPC_DIR_RIGHT:
				x_move_step+=STAND_MOVE_STEP;
				y_move_step=0;
				break;
			case NPC_DIR_UP_RIGHT:
				x_move_step+=STAND_MOVE_STEP;
				y_move_step-=STAND_MOVE_STEP;
				break;
			case NPC_DIR_DOWN_LEFT:
				x_move_step-=STAND_MOVE_STEP;
				y_move_step+=STAND_MOVE_STEP;
				break;
			case NPC_DIR_UP_LEFT:
				x_move_step-=STAND_MOVE_STEP;
				y_move_step-=STAND_MOVE_STEP;
				break;
			case NPC_DIR_DOWN_RIGHT:
				x_move_step+=STAND_MOVE_STEP;
				y_move_step+=STAND_MOVE_STEP;
				break;
			}

		if(map_npc_group[npc_index].npc_info.speed==1)
			{
			x_move_step=x_move_step*2;
			y_move_step=y_move_step*2;
			}

		if(x_move_step!=0)
			{
			if(x_move_step>0)
				x_flag=1;
			else 
				x_flag=-1;
			}
		
		if(y_move_step!=0)
			{
			if(y_move_step>0)
				y_flag=1;
			else 
				y_flag=-1;
			}

		cell_x=(map_npc_group[npc_index].npc_info.x+x_move_step)/map_base_data.cell_width;
		cell_y=(map_npc_group[npc_index].npc_info.y+y_move_step)/map_base_data.cell_height;
		if(check_cell_stop(cell_x,cell_y)==TTN_OK)
			return(TTN_NOT_OK);

		map_npc_group[npc_index].npc_info.dir=npc_dir;

		map_npc_group[npc_index].npc_info.x+=x_move_step;
		map_npc_group[npc_index].npc_info.y+=y_move_step;

		return(TTN_OK);
}


SLONG read_file_to_buffer(UCHR *filename,FILE_BUFFER *file_buffer)
{
	FILE *fp;
	int handle;

	if(file_buffer->buffer!=NULL)
		free(file_buffer->buffer);
	file_buffer->buffer=NULL;
	file_buffer->file_size=0;

	if(check_file_exist(filename)!=TTN_OK)
		return(TTN_NOT_OK);

	fp=fopen((char *)filename,"rb");
	if(fp==NULL)
	{
		return(TTN_NOT_OK);
	}

	handle = fileno(fp);
	file_buffer->file_size=filelength(handle);

	file_buffer->buffer=(UCHR *)malloc(sizeof(UCHR)*file_buffer->file_size);
	if(file_buffer->buffer==NULL)
	{
		display_error_message((UCHR *)MSG_MEMORY_ALLOC_ERROR,1);
		return(TTN_NOT_OK);
		fclose(fp);
	}

	fread(file_buffer->buffer,1,file_buffer->file_size,fp);

	fclose(fp);

	return(TTN_OK);
}



SLONG get_textbuffer_total_line(FILE_BUFFER *file_buffer)
{
	SLONG ret_val;
	ULONG i;

	ret_val=0;
	for(i=0;i<file_buffer->file_size-1;i++)
	{
		if((file_buffer->buffer[i]=='#')&&(file_buffer->buffer[i+1]=='L'))		// #L
		{
			i++;
			ret_val++;
		}
	}

	return(ret_val);
	
}



void get_textbuffer_data(SLONG idx,FILE_BUFFER *file_buffer,UCHR *text)
{
	ULONG i,j;
	SLONG line_no;

	line_no=0;
	j=0;
	for(i=0;i<file_buffer->file_size;i++)
	{
		if(line_no==idx)
		{
			text[j]=file_buffer->buffer[i];
			text[j+1]=NULL;
			j++;
			if(j>1023)
				return;
		}
		if((file_buffer->buffer[i]=='#')&&(file_buffer->buffer[i+1]=='L'))		// #L
		{
			line_no++;
			i++;
			if(line_no>idx)
			{
				if(j>0)
					text[j-1]=0x00;
				else
					text[j]=0x00;
				return;
			}
		}

	}

}


void set_text_out_window(RECT rect)
{
	memcpy(&text_out_data.chatRect,&rect,sizeof (RECT));
}


void update_player_move(PLAYER_MOVE& pp)
{
	ULONG id;
	SLONG idx;

	id=pp.char_id;
	idx=get_map_npc_index(id);
	if(idx<0)return;			// not find

	map_npc_group[idx].npc_info.tx=pp.tx;
	map_npc_group[idx].npc_info.ty=pp.ty;
	map_npc_group[idx].npc_info.speed=pp.speed;
//
// �������ܷ���
//	map_npc_group[idx].npc_info.dir=pp.dir;

}



void clear_screen_mask(void)
{
	fill_bitmap(map_control.screen_mask,MAIN_TASK_MOVE);
}



SLONG add_extra_mda(UCHR *filename,SLONG x,SLONG y,bool loop,bool auto_close,bool hight_light,SLONG level,UCHR *command)
{
	SLONG index;
	SLONG i;
	SLONG handle;

	index=-1;
	for(i=0;i<MAX_EXTRA_MDA;i++)
	{
		if(extra_mda_data[i].handle<0)		// û����
		{
			index=i;
			break;
		}
	}

	if(index<0)return(TTN_NOT_OK);			// full 
	
	mda_group_open(filename,&handle);
	if(handle<0)return(TTN_NOT_OK);			// file open error


	mda_group_exec(handle,(UCHR *)"MainCommand0",0,0,screen_channel0,MDA_COMMAND_FIRST,0,0,0);
	extra_mda_data[index].mda_index=get_mda_index(handle);
	
	extra_mda_data[index].handle=handle;
	extra_mda_data[index].display_x=x;
	extra_mda_data[index].display_y=y;
	extra_mda_data[index].xl=mda_group[extra_mda_data[index].mda_index].img256_head->xlen;
	extra_mda_data[index].yl=mda_group[extra_mda_data[index].mda_index].img256_head->ylen;
	extra_mda_data[index].center_x=mda_group[extra_mda_data[index].mda_index].img256_head->center_x;
	extra_mda_data[index].center_y=mda_group[extra_mda_data[index].mda_index].img256_head->center_y;
	extra_mda_data[index].loop=loop;
	extra_mda_data[index].auto_close=auto_close;
	extra_mda_data[index].hight_light_flag=hight_light;
	strcpy((char *)extra_mda_data[index].command,(char *)command);
	extra_mda_data[index].level=level;
	extra_mda_data[index].play_end=false;
	extra_mda_data[index].timer=system_control.system_timer;
	extra_mda_data[index].display=true;

	return(index);

}



void auto_close_extra_mda(void)
{
	SLONG i;
	SLONG handle;
	
	for(i=0;i<MAX_EXTRA_MDA;i++)
	{
		if(extra_mda_data[i].handle>=0)		// û����
		{
			if(extra_mda_data[i].auto_close==true)		// ��Ҫ�Զ��ر�
			{
				handle=extra_mda_data[i].handle;
				mda_group_close(&handle);
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
		}
	}
	
}


void reset_extra_mda_frame(SLONG idx)
{
	SLONG handle;
	SLONG mda_index;

	handle=extra_mda_data[idx].handle;
	mda_index=get_mda_index(handle);
	if(mda_index<0)
		return;
	mda_group[mda_index].now_frame[0]=0;
}



void draw_button_extra_mda(BMP *buffer)
{
	SLONG idx;
	SLONG screen_x,screen_y;
	SLONG mda_handle,mda_index;
	SLONG mda_width,mda_height;
	SLONG mda_command;
	UCHR command_str[80];
	SLONG ret_val;
	
	for(idx=0;idx<MAX_EXTRA_MDA;idx++)
	{
		if(extra_mda_data[idx].handle<0)continue;
		if(!extra_mda_data[idx].display)continue;
		if(extra_mda_data[idx].level!=EXTRA_BOTTOM_LAYER)continue;		// Button

		// --- Get Base Data 
		mda_handle=extra_mda_data[idx].handle;
		mda_index=get_mda_index(mda_handle);
		if(mda_index<0)continue;
		mda_width=extra_mda_data[idx].xl;
		mda_height=extra_mda_data[idx].yl;
		
		
		mda_command=NULL;
		if(extra_mda_data[idx].hight_light_flag)
			mda_command=MDA_COMMAND_HIGHTLIGHT;
		if(extra_mda_data[idx].loop)
			mda_command=mda_command|MDA_COMMAND_LOOP;
		
		if(system_control.system_timer>=extra_mda_data[idx].timer)
		{
			mda_command=mda_command|MDA_COMMAND_NEXT_FRAME;
			extra_mda_data[idx].timer=system_control.system_timer+SYSTEM_MAIN_DELAY;
			
		}
		
		screen_x=extra_mda_data[idx].display_x-map_control.start_x;
		screen_y=extra_mda_data[idx].display_y-map_control.start_y;
		
		strcpy((char *)command_str,(char *)extra_mda_data[idx].command);
		ret_val=mda_group_exec(mda_handle,command_str,screen_x,screen_y,buffer,mda_command,0,0,0);
		map_layer_touch.flag=false;
		
		if(ret_val==MDA_EXEC_LAST)
		{
			extra_mda_data[idx].play_end=true;			
			if(extra_mda_data[idx].auto_close)			// �Զ��ر�
			{
				if(!extra_mda_data[idx].loop)			// ������� loop
				{
					mda_group_close(&extra_mda_data[idx].handle);		// �ر�
					extra_mda_data[idx].handle=-1;
				}
			}
			else
			{
				if(!extra_mda_data[idx].loop)			// ������� loop
				{
					extra_mda_data[idx].display=false;
				}
				
			}
			
		}
		
		
	}
	
}



void draw_top_extra_mda(BMP *buffer)
{
	SLONG idx;
	SLONG screen_x,screen_y;
	SLONG mda_handle,mda_index;
	SLONG mda_width,mda_height;
	SLONG mda_command;
	UCHR command_str[80];
	SLONG ret_val;
	
	for(idx=0;idx<MAX_EXTRA_MDA;idx++)
	{
		if(extra_mda_data[idx].handle<0)continue;
		if(!extra_mda_data[idx].display)continue;
		if(extra_mda_data[idx].level!=EXTRA_TOP_LAYER)continue;		// Bottom
		
		// --- Get Base Data 
		mda_handle=extra_mda_data[idx].handle;
		mda_index=get_mda_index(mda_handle);
		if(mda_index<0)continue;
		mda_width=extra_mda_data[idx].xl;
		mda_height=extra_mda_data[idx].yl;
		
		
		mda_command=NULL;
		if(extra_mda_data[idx].hight_light_flag)
			mda_command=MDA_COMMAND_HIGHTLIGHT;
		if(extra_mda_data[idx].loop)
			mda_command=mda_command|MDA_COMMAND_LOOP;
		
		if(system_control.system_timer>=extra_mda_data[idx].timer)
		{
			mda_command=mda_command|MDA_COMMAND_NEXT_FRAME;
			extra_mda_data[idx].timer=system_control.system_timer+SYSTEM_MAIN_DELAY;
			
		}
		
		screen_x=extra_mda_data[idx].display_x-map_control.start_x;
		screen_y=extra_mda_data[idx].display_y-map_control.start_y;
		
		strcpy((char *)command_str,(char *)extra_mda_data[idx].command);
		ret_val=mda_group_exec(mda_handle,command_str,screen_x,screen_y,buffer,mda_command,0,0,0);
		map_layer_touch.flag=false;
		
		if(ret_val==MDA_EXEC_LAST)
		{
			extra_mda_data[idx].play_end=true;			
			if(extra_mda_data[idx].auto_close)			// �Զ��ر�
			{
				if(!extra_mda_data[idx].loop)			// ������� loop
				{
					mda_group_close(&extra_mda_data[idx].handle);		// �ر�
					extra_mda_data[idx].handle=-1;
				}
			}
			else
			{
				if(!extra_mda_data[idx].loop)			// ������� loop
				{
					extra_mda_data[idx].display=false;
				}
				
			}
			
		}
		
		
	}
	
}


/***************************************************************************************}
{ SCREEN TEXT DISPLAY 																	}
****************************************************************************************/
void clear_all_screen_text(void)
{
	SLONG i;

	for(i=0;i<MAX_SCREEN_TEXT;i++)
	{
		screen_text_data[i].active=false;
		screen_text_stack[i]=-1;
	}
}


SLONG get_screen_text_idx(void)
{
	SLONG i;

	for(i=0;i<MAX_SCREEN_TEXT;i++)
	{
		if(!screen_text_data[i].active)
		{
			add_screen_stack(i);
			return(i);
		}
	}
	add_screen_stack(0);
	return(0);					// ���� ��0 ��
}



void add_screen_stack(SLONG idx)
{
	SLONG i,j;
	SLONG temp_idx;

// ---- ( 0 - 0 ) Stack sort 
	for(i=0;i<MAX_SCREEN_TEXT-1;i++)
	{
		for(j=0;j<MAX_SCREEN_TEXT-1;j++)
		{
			if(screen_text_stack[j]<0)
			{
			temp_idx=screen_text_stack[j];
			screen_text_stack[j]=screen_text_stack[j+1];
			screen_text_stack[j+1]=temp_idx;
			}
		}
	}

// --- ( 0 -1 ) Add new
	for(i=0;i<MAX_SCREEN_TEXT;i++)
	{
		if(screen_text_stack[i]<0)
		{
			screen_text_stack[i]=idx;
			i=MAX_SCREEN_TEXT;
			break;
		}
	}

}



void delete_screen_stack(SLONG idx)
{
	SLONG i,j;
	SLONG temp_idx;
	


	// --- ( 0 -1 ) delete idx
	for(i=0;i<MAX_SCREEN_TEXT;i++)
	{
		if(screen_text_stack[i]==idx)
		{
			screen_text_stack[i]=-1;
			i=MAX_SCREEN_TEXT;
			break;
		}
	}
	
	// ---- ( 0 - 0 ) Stack sort 
	for(i=0;i<MAX_SCREEN_TEXT-1;i++)
	{
		for(j=0;j<MAX_SCREEN_TEXT-1;j++)
		{
			if(screen_text_stack[j]<0)
			{
				temp_idx=screen_text_stack[j];
				screen_text_stack[j]=screen_text_stack[j+1];
				screen_text_stack[j+1]=temp_idx;
			}
		}
	}
	
	
}





void draw_message_font(SLONG x,SLONG y,SLONG xl,SLONG yl,BMP *buffer)
{
	SLONG cell_xl,cell_yl;	
	SLONG xx,yy;
	SLONG real_xl,real_yl;

// draw_bar 
	cell_xl=(xl-16)/8;
	if(cell_xl<0)cell_xl=0;
	cell_yl=(yl-16)/8;
	if(cell_yl<0)cell_yl=0;

	real_xl=cell_xl*8+16;
	real_yl=cell_yl*8+16;
	
	
	display_img256_buffer(x,y,system_image.msg_TopLeft.image,buffer);
	for(xx=0;xx<cell_xl;xx++)
		display_img256_buffer(x+8+xx*8,y,system_image.msg_Top.image,buffer);
	display_img256_buffer(x+real_xl-8,y,system_image.msg_TopRight.image,buffer);
	
	for(yy=0;yy<cell_yl;yy++)
	{
		display_img256_buffer(x,y+8+yy*8,system_image.msg_Left.image,buffer);
		display_img256_buffer(x+real_xl-8,y+8+yy*8,system_image.msg_Right.image,buffer);
		for(xx=0;xx<cell_xl;xx++)
			display_img256_buffer(x+8+xx*8,y+8+yy*8,system_image.msg_Middle.image,buffer);
		
	}
	
	display_img256_buffer(x,y+real_yl-8,system_image.msg_BottomLeft.image,buffer);
	for(xx=0;xx<cell_xl;xx++)
		display_img256_buffer(x+8+xx*8,y+real_yl-8,system_image.msg_Bottom.image,buffer);
	display_img256_buffer(x+real_xl-8,y+real_yl-8,system_image.msg_BottomRight.image,buffer);
	
}



void draw_message_box(SLONG x,SLONG y,SLONG xl,SLONG yl,BMP *buffer)
{
	SLONG cell_xl,cell_yl;	
	SLONG xx,yy;
	SLONG real_xl,real_yl;
	
	// draw_bar 
	cell_xl=(xl-16)/8;
	if(cell_xl<0)cell_xl=0;
	cell_yl=(yl-16)/8;
	if(cell_yl<0)cell_yl=0;
	
	real_xl=cell_xl*8+16;
	real_yl=cell_yl*8+16;
	
	
	display_img256_buffer(x,y,system_image.msg_TopLeft.image,buffer);
	for(xx=0;xx<cell_xl;xx++)
		display_img256_buffer(x+8+xx*8,y,system_image.msg_Top.image,buffer);
	display_img256_buffer(x+real_xl-8,y,system_image.msg_TopRight.image,buffer);
	
	for(yy=0;yy<cell_yl;yy++)
	{
		display_img256_buffer(x,y+8+yy*8,system_image.msg_Left.image,buffer);
		display_img256_buffer(x+real_xl-8,y+8+yy*8,system_image.msg_Right.image,buffer);
		
	}
	
	display_img256_buffer(x,y+real_yl-8,system_image.msg_BottomLeft.image,buffer);
	for(xx=0;xx<cell_xl;xx++)
		display_img256_buffer(x+8+xx*8,y+real_yl-8,system_image.msg_Bottom.image,buffer);
	display_img256_buffer(x+real_xl-8,y+real_yl-8,system_image.msg_BottomRight.image,buffer);
	
}



void exec_screen_text(BMP *buffer)
{
	SLONG i;
	SLONG display_x,display_y;
	SLONG display_xl,display_yl;
	SLONG total_check;
	SLONG idx,idx1;
	bool change_frame;

	change_frame=false;
	if(system_control.system_timer>game_control.screen_text_timer)
	{
		game_control.screen_text_timer=system_control.system_timer+SYSTEM_MAIN_DELAY;
		change_frame=true;
	}

	for(i=0,total_check=0;i<MAX_SCREEN_TEXT;i++,total_check++)
	{
		if(screen_text_stack[i]<0)
			break;
	}

	if(total_check<=0)return;		

//	idx1=screen_text_stack[total_check-1];		// new id
	for(i=0;i<total_check-1;i++)
	{
		idx=screen_text_stack[i];
		idx1=screen_text_stack[i+1];
		if((idx<0)||(idx1<0))continue;
		if(check_box_intersect(screen_text_data[idx].rect.left,
			screen_text_data[idx].rect.top,
			screen_text_data[idx].rect.right,
			screen_text_data[idx].rect.bottom,
			screen_text_data[idx1].rect.left,
			screen_text_data[idx1].rect.top,
			screen_text_data[idx1].rect.right,
			screen_text_data[idx1].rect.bottom)==TTN_OK)	
		{
			screen_text_data[idx].rect.top-=1;
			screen_text_data[idx].rect.bottom-=1;
		}
	}
	

	if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)
	{
		for(i=0;i<total_check;i++)
		{
			idx=screen_text_stack[i];
			if(idx<0)continue;
			if(screen_text_data[idx].active)
			{

				display_x=(screen_text_data[idx].rect.left-now_camera_pan_x)*camera_zoom[now_camera_zoom].x_pos_zoom/1000;
				display_y=(screen_text_data[idx].rect.top-now_camera_pan_y)*camera_zoom[now_camera_zoom].y_pos_zoom/1000;
				display_xl=screen_text_data[idx].rect.right-screen_text_data[idx].rect.left;
				display_yl=screen_text_data[idx].rect.bottom-screen_text_data[idx].rect.top;
				draw_message_font(display_x,display_y,display_xl,display_yl,buffer);
				
				text_out_data.g_pChat->updateString(display_x+4,display_y+4,display_xl-8,display_yl-8,
					(char *)screen_text_data[idx].text);
				
				text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,buffer,change_frame);
				
				if(system_control.system_timer>screen_text_data[idx].timer)
				{
					screen_text_data[idx].active=false;
					delete_screen_stack(idx);
				}
			}
		}
	}
	else
	{
		for(i=0;i<total_check;i++)
		{
			idx=screen_text_stack[i];
			if(idx<0)continue;
			if(screen_text_data[idx].active)
			{
				display_x=screen_text_data[idx].rect.left-map_control.start_x;
				display_y=screen_text_data[idx].rect.top-map_control.start_y;
				display_xl=screen_text_data[idx].rect.right-screen_text_data[idx].rect.left;
				display_yl=screen_text_data[idx].rect.bottom-screen_text_data[idx].rect.top;
				draw_message_font(display_x,display_y,display_xl,display_yl,buffer);
				
				text_out_data.g_pChat->updateString(display_x+4,display_y+4,display_xl-8,display_yl-8,
					(char *)screen_text_data[idx].text);
				
				text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,buffer,change_frame);
				
				if(system_control.system_timer>screen_text_data[idx].timer)
				{
					screen_text_data[idx].active=false;
					delete_screen_stack(idx);
				}
			}
		}
	}
		
	
	

}



SLONG read_mda_button(SLONG handle,SLONG command_no,BUTTON_STATUS *button,UCHR *inst,SLONG code)
{
	SLONG mda_index;
	UCHR command_text[80];

	mda_index=get_mda_index(handle);
	if(mda_index<0)return(TTN_NOT_OK);

	button->command_no=command_no;
	strcpy((char *)button->inst,(char *)inst);
	button->ret_code=code;

	sprintf((char *)command_text,"MainCommand%d",command_no);
	if(mda_group_exec(handle,command_text,-9999,-9999,screen_channel1,MDA_COMMAND_FIRST,0,0,0)==MDA_EXEC_ERROR)
		return(TTN_NOT_OK);
	button->rect.left=mda_group[mda_index].img256_head->orig_x;
	button->rect.top=mda_group[mda_index].img256_head->orig_y;
	button->rect.right=mda_group[mda_index].img256_head->orig_x+mda_group[mda_index].img256_head->xlen;
	button->rect.bottom=mda_group[mda_index].img256_head->orig_y+mda_group[mda_index].img256_head->ylen;
	return(TTN_OK);	
}



void push_chat_stack(UCHR *text)
{
SLONG i;

	for(i=0;i<MAX_CHAT_STACK;i++)
	{
		if(strcmpi((char *)text,(char *)game_control.chat_stack_buffer[i])==0)
			return;
	}

	strcpy((char *)game_control.chat_stack_buffer[game_control.chat_stack_index],(char *)text);
	
	game_control.chat_stack_index++;
	if(game_control.chat_stack_index>=MAX_CHAT_STACK)	
		game_control.chat_stack_index=0;				// loop ����
}


void get_chat_stack(UCHR *text,UCHR key)
{
	SLONG idx;

	switch(key)
	{
	case Up:
		idx=game_control.chat_stack_find_index-1;
		if(idx<0)idx=MAX_CHAT_STACK-1;
		break;
	case Dn:
		if(game_control.chat_stack_find_index==game_control.chat_stack_index)
		{
			strcpy((char *)text,"");
			return;
		}
		idx=game_control.chat_stack_find_index+1;
		if(idx>=MAX_CHAT_STACK)
			idx=0;
		break;
	default:
		return;
	}


	if(strlen((char *)game_control.chat_stack_buffer[idx])<1)		// not fond 
	{
		strcpy((char *)text,"");
		return;
	}

	if(idx==game_control.chat_stack_index)
	{
		strcpy((char *)text,"");
		return;
	}
	
	strcpy((char *)text,(char *)game_control.chat_stack_buffer[idx]);
	game_control.chat_stack_find_index=idx;

}





void display_chat_inter_window(BMP *buffer)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	
	SLONG word_color;
	SLONG word_count;
	SLONG main_pass;
	SLONG idx;
	SLONG text_yl;
	SLONG i;
	SLONG title_offset;
	SLONG font_height;
	UCHR text[1024];
	UCHR title[1024];
	UCHR inst[1024];
	RECT window_rect;
	SLONG display_x,display_y;

	bool show_title;
	bool show_name;
	
	
	
	window_xl=CHAT_WINDOW_WIDTH;
	window_yl=chat_data_buffer.colume*20+25; // 567
	window_x=0;
	window_y=567-window_yl;
	
	if(game_control.chat_pause_flag==1)
		alpha_put_bar(window_x,window_y,window_xl,window_yl,color_control.red,buffer,60);
	else
		alpha_put_bar(window_x,window_y,window_xl,window_yl,color_control.black,buffer,128);
	
	window_yl=window_yl-25;
	window_x=40;
	window_xl=window_xl-38;
	
	window_rect.left=window_x;
	window_rect.top=window_y;
	window_rect.right=window_x+window_xl;
	window_rect.bottom=window_y+window_yl;
	
	
	word_count=0;
	main_pass=0;
	text_yl=0;
	idx=chat_data_buffer.display_index-1;

	if(chat_data_buffer.bottom_index==chat_data_buffer.display_index)
		main_pass=1;				// first time enter

	while(main_pass==0)
	{
		if(idx<0)
			idx=MAX_CHAT_RECORD-1;
		if(idx==chat_data_buffer.bottom_index)
			main_pass=1;
		
		window_rect.left=window_x;
		window_rect.top=window_y;
		window_rect.right=window_x+window_xl;
		window_rect.bottom=window_y+window_yl;
		

		strcpy((char *)inst,"");
		switch(chat_data_buffer.record[idx].channel)
		{
		case CHAT_CHANNEL_PERSON:			// ˽��
			if(chat_data_buffer.record[idx].user_id==system_control.control_user_id)
				sprintf((char *)inst,MSG_PERSON_SEND,chat_data_buffer.record[idx].nickname);
			else
				sprintf((char *)inst,MSG_PERSON_RECEIVE,chat_data_buffer.record[idx].nickname);
			show_name=false;
			break;
		case CHAT_CHANNEL_SCREEN:			// ��ǰƵ��
		case CHAT_CHANNEL_TEAM:				// ����
		case CHAT_CHANNEL_GROUP:			// ����
		case CHAT_CHANNEL_SELLBUY:			// ����
		case CHAT_CHANNEL_WORLD:			// ����
			show_name=true;
			break;
		case CHAT_CHANNEL_SYSTEM:			// ϵͳ˵��
			show_name=false;
			break;
		case CHAT_CHANNEL_MESSGAE:			// ���ѶϢ
			show_name=false;
		default:
			show_name=false;
			break;
			
		}
		
		if(show_name)
			sprintf((char *)text,"#C00FFFF%s��#N%s",chat_data_buffer.record[idx].nickname,chat_data_buffer.record[idx].text);
		else
			sprintf((char *)text,"%s%s",inst,chat_data_buffer.record[idx].text);
		text_out_data.g_pChat->insertString((char *)text,window_rect,false);
		text_yl+=window_rect.bottom-window_rect.top;		

		if(text_yl>=window_yl)
		{
			main_pass=1;
		}
		else
		{
			word_count++;
			idx--;
		}
		
	}
	
	display_x=window_x;
	display_y=window_y+window_yl;
	idx=chat_data_buffer.display_index-1;
	for(i=0;i<word_count;i++)
	{
		if(idx<0)
			idx=MAX_CHAT_RECORD-1;
		
		window_rect.left=window_x;
		window_rect.top=window_y;
		window_rect.right=window_x+window_xl;
		window_rect.bottom=window_y+window_yl;
		
		strcpy((char *)inst,"");
		show_title=true;
		show_name=true;
		switch(chat_data_buffer.record[idx].channel)
		{
		case CHAT_CHANNEL_SCREEN:			// ��ǰƵ��
			strcpy((char *)title,(char *)MSG_CHANNEL_NAME_SCREEN);
			word_color=color_control.light_green;
			show_name=true;
			break;
		case CHAT_CHANNEL_TEAM:				// ����
			strcpy((char *)title,(char *)MSG_CHANNEL_NAME_TEAM);
			word_color=color_control.oringe;
			show_name=true;
			break;
		case CHAT_CHANNEL_PERSON:			// ˽��
			strcpy((char *)title,(char *)MSG_CHANNEL_NAME_PERSON);
			word_color=color_control.white;
			if(chat_data_buffer.record[idx].user_id==system_control.control_user_id)
				sprintf((char *)inst,MSG_PERSON_SEND,chat_data_buffer.record[idx].nickname);
			else
				sprintf((char *)inst,MSG_PERSON_RECEIVE,chat_data_buffer.record[idx].nickname);
			show_name=false;
			break;
		case CHAT_CHANNEL_GROUP:			// ����
			strcpy((char *)title,(char *)MSG_CHANNEL_NAME_GROUP);
			word_color=color_control.light_blue;
			show_name=true;
			break;
		case CHAT_CHANNEL_SELLBUY:			// ����
			strcpy((char *)title,(char *)MSG_CHANNEL_NAME_SELLBUY);
			word_color=color_control.yellow;
			show_name=true;
			break;
		case CHAT_CHANNEL_WORLD:			// ����
			strcpy((char *)title,(char *)MSG_CHANNEL_NAME_WORLD);
			word_color=color_control.pink;
			show_name=true;
			break;
		case CHAT_CHANNEL_SYSTEM:			// ϵͳ˵��
			strcpy((char *)title,(char *)MSG_CHANNEL_NAME_SYSTEM);
			word_color=color_control.red;
			show_name=false;
			break;
		case CHAT_CHANNEL_MESSGAE:			// ���ѶϢ
			strcpy((char *)title,(char *)MSG_CHANNEL_NAME_MESSAGE);
			word_color=color_control.red;
			show_name=false;
			break;
		default:
			strcpy((char *)title,"");
			show_title=false;
			show_name=false;
			break;
		}
		
		
		if(show_name)
			sprintf((char *)text,"#C00FFFF%s��#N%s",chat_data_buffer.record[idx].nickname,chat_data_buffer.record[idx].text);
		else
			sprintf((char *)text,"%s%s",inst,chat_data_buffer.record[idx].text);
		
		title_offset=text_out_data.g_pChat->insertString((char *)text,window_rect,false);
		
		font_height=window_rect.bottom-window_rect.top;
		display_y=display_y-font_height;
		
		if(idx%2==0)
			alpha_put_bar(display_x,display_y,window_xl-2,font_height,color_control.white,buffer,30);
		
		
		if(show_title)
		{
			
			put_bar(2,display_y+2+title_offset,36,16,color_control.black,buffer);
			put_bar(2,display_y+2+title_offset,34,14,color_control.low_gray,buffer);
			put_box(0,display_y+title_offset,36,16,color_control.white,buffer);
			set_word_color(color_control.black);
			print12(5+1,display_y+3+1+title_offset,title,PEST_PUT,buffer);
			set_word_color(word_color);
			print12(5,display_y+3+title_offset,title,PEST_PUT,buffer);
		}
		
		text_out_data.g_pChat->updateString(display_x,display_y,window_xl,window_yl,
			(char *)text);
//		text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,screen_buffer,false);
		
		idx--;
		
	}
	

	text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,buffer,false);
	
	set_word_color(color_control.white);
	
	
}


void scroll_chat_data(UCHR key)
{

	switch(key)
	{
	case Up:
		if(chat_data_buffer.bottom_index==chat_data_buffer.display_index)
			break;				// first time enter
		chat_data_buffer.display_index--;
		if(chat_data_buffer.display_index<0)
			chat_data_buffer.display_index=MAX_CHAT_RECORD-1;
		if(chat_data_buffer.display_index==chat_data_buffer.bottom_index)
		{
			chat_data_buffer.display_index=chat_data_buffer.bottom_index+1;
			if(chat_data_buffer.display_index>=MAX_CHAT_RECORD)
				chat_data_buffer.display_index=0;
		}
		
		break;
	case Dn:
		if(chat_data_buffer.display_index==chat_data_buffer.top_index)
			break;
		if(chat_data_buffer.bottom_index==chat_data_buffer.display_index)
			break;				// first time enter
		chat_data_buffer.display_index++;
		if(chat_data_buffer.display_index>=MAX_CHAT_RECORD)
			chat_data_buffer.display_index=0;

		if(chat_data_buffer.display_index==chat_data_buffer.bottom_index)
		{
			chat_data_buffer.display_index=chat_data_buffer.bottom_index+1;
			if(chat_data_buffer.display_index>=MAX_CHAT_RECORD)
				chat_data_buffer.display_index=0;
		}
		break;
	}
}


void push_chat_data(SLONG channel,ULONG id,UCHR *nickname,UCHR *text)
{
	SLONG idx;


	switch(channel)
	{
	case CHAT_CHANNEL_SCREEN:			// ��ǰƵ��
		if(system_config.channel_screen_flag==0)
			return;
		break;	
	case CHAT_CHANNEL_TEAM:				// ����
		if(system_config.channel_team_flag==0)
			return;
		break;	
	case CHAT_CHANNEL_PERSON:			// ˽��
		if(system_config.channel_person_flag==0)
			return;
		break;	
	case CHAT_CHANNEL_GROUP:			// ����
		if(system_config.channel_group_flag==0)
			break;
		break;	
	case CHAT_CHANNEL_SELLBUY:			// ����
		if(system_config.channel_sellbuy_flag==0)
			break;
		break;	
	case CHAT_CHANNEL_WORLD:			// ����
		if(system_config.channel_world_flag==0)
			return;
		break;	
	}


	
	
	idx=chat_data_buffer.top_index;
	chat_data_buffer.record[idx].channel=channel;
	strcpy((char *)chat_data_buffer.record[idx].nickname,(char *)nickname);

	chat_data_buffer.record[idx].user_id=id;
	strcpy((char *)chat_data_buffer.record[idx].text,(char *)text);

	
	chat_data_buffer.top_index++;
	if(chat_data_buffer.top_index>=MAX_CHAT_RECORD)
		chat_data_buffer.top_index=0;

	if(chat_data_buffer.top_index==chat_data_buffer.bottom_index)
		chat_data_buffer.bottom_index++;

	if(chat_data_buffer.bottom_index>=MAX_CHAT_RECORD)
		chat_data_buffer.bottom_index=0;

	if(game_control.chat_pause_flag==0)
	{
		chat_data_buffer.display_index=chat_data_buffer.top_index;
	}

}


void change_map(void)
{
	UCHR real_filename[1024];
	char version_text[256];
	char window_name[256];
	


// --- ( 0 - 0 ) Load data
	sprintf((char *)real_filename,(char *)"MAP\\%s",change_map_info.map_filename);
	if(load_map_file(real_filename,change_map_info.main_character.x,change_map_info.main_character.y)!=TTN_OK)
	{
		sprintf((char *)print_rec,MSG_MAP_FILE_LOAD_ERROR,real_filename);
		display_error_message(print_rec,1);
		return;
	}

// --- music 
	sprintf((char *)real_filename,"MUSIC\\%s",change_map_info.music_filename);
	play_music_file(real_filename,1,100);
	
	get_version_string(version_text,GAME_VERSION);
	sprintf(window_name,"%s( version %s ) [ %s - %s - %d ]",GAME_NAME,version_text,
		game_control.server_name,
		change_map_info.main_character.name,
		change_map_info.main_character.id);
	SetWindowText(g_hDDWnd,  (char *)window_name);
	

	set_weather_type(change_map_info.map_weather_base.weather_type);		
}



void show_friend_list(SLONG x,SLONG y,SLONG function)
{
	SLONG i;
	SLONG idx;
	UCHR name[80];
	UHINT color;
	SLONG start_idx;
	bool show_flag;
	bool draw_back_select;


	draw_back_select=false;
	for(i=0;i<14;i++)
	{
		color=color_control.gray;
		show_flag=false;
		switch(function)
		{
		case FRIEND_NORMAL:					// һ�����
			idx=game_control.window_chat_friend_start_idx+i;
			if(display_friend_list[idx].level<0)continue;
			show_flag=true;
			strcpy((char *)name,(char *)display_friend_list[idx].nickname);
			if(display_friend_list[idx].online==1)
				color=color_control.green;
			start_idx=game_control.window_chat_friend_start_idx;
			break;
		case FRIEND_TEMP:					// ��ʱ����
			idx=game_control.window_chat_temp_start_idx+i;
			if(display_temp_list[idx].level<0)continue;
			strcpy((char *)name,(char *)display_temp_list[idx].nickname);
			if(display_temp_list[idx].online==1)
				color=color_control.green;
			show_flag=true;
			start_idx=game_control.window_chat_temp_start_idx;
			break;
		case FRIEND_MASK:					// ������
			idx=game_control.window_chat_mask_start_idx+i;
			if(display_mask_list[idx].level<0)continue;
			strcpy((char *)name,(char *)display_mask_list[idx].nickname);
			if(display_mask_list[idx].online==1)
				color=color_control.green;
			show_flag=true;
			start_idx=game_control.window_chat_mask_start_idx;
			break;			
		default:
			continue;
			break;
		}
		

		if(show_flag)
		{

			if((system_control.mouse_x>=x)&&(system_control.mouse_x<x+139)&&
				(system_control.mouse_y>=y+i*21)&&(system_control.mouse_y<y+i*21+18))
			{
				mda_group_exec(system_image.button_handle,(UCHR *)"MainCommand0",
					x,
					y+i*21,
					screen_buffer,MDA_COMMAND_ASSIGN,0,0,0);

			}


			if(i==game_control.window_chat_select_idx)
				alpha_put_bar(x,y+i*21,139,18,color_control.pink,screen_buffer,128);

			set_word_color(0,color);
			print16(x+8,y+1+i*21,name,PEST_PUT,screen_buffer);
			set_word_color(0,color_control.white);
		}
		
		
		
	}
	
	
}



void scroll_friend_list(UCHR dir)
{

	switch(dir)
	{
	case Up:
		switch(game_control.window_chat_type)
		{
		case FRIEND_NORMAL:					// һ�����
			if(game_control.window_chat_friend_start_idx>0)
			{
				game_control.window_chat_friend_start_idx--;
			}
			break;				
		case FRIEND_TEMP:					// ��ʱ����
			if(game_control.window_chat_temp_start_idx>0)
			{
				game_control.window_chat_temp_start_idx--;
			}
			break;				
		case FRIEND_MASK:					// ������
			if(game_control.window_chat_mask_start_idx>0)
			{
				game_control.window_chat_mask_start_idx--;
			}
			break;				
		}



		break;
	case Dn:
		switch(game_control.window_chat_type)
		{
		case FRIEND_NORMAL:					// һ�����
			if(game_control.window_chat_friend_start_idx<game_control.window_chat_friend_end_idx-14)
			{
				game_control.window_chat_friend_start_idx++;
			}
			break;				
		case FRIEND_TEMP:					// ��ʱ����
			if(game_control.window_chat_temp_start_idx<game_control.window_chat_temp_end_idx-14)
			{
				game_control.window_chat_temp_start_idx++;
			}
			break;				
		case FRIEND_MASK:					// ������
			if(game_control.window_chat_mask_start_idx<game_control.window_chat_mask_end_idx-14)
			{
				game_control.window_chat_mask_start_idx++;
			}
			break;				
		}
		

		break;

	}



}



void add_friend_list(ULONG friend_id,SLONG type)
{
	SLONG i;
	SLONG idx;


	
	idx=-1;
	for(i=0;i<MAX_CHAT_FRIEND_LIST;i++)
	{
		if(chat_friend_list[i].level<0)
		{
		idx=i;
		i=MAX_CHAT_FRIEND_LIST;
		break;
		}
	}

	if(idx<0)
	{
		display_system_message((UCHR *)MSG_MAX_CHAT_FRIEND);
		return;
	}


	for(i=0;i<MAX_CHAT_FRIEND_LIST;i++)
	{
		if(chat_friend_list[i].level>=0)
		{
			if(chat_friend_list[i].id==friend_id)
			{
				if(chat_friend_list[i].friend_type!=type)
				{
					change_friend_list_friend_type(friend_id,type);
					return;		
				}
				else
				{
					sprintf((char *)print_rec,MSG_ADD_SAME_FRIEND,(char *)chat_friend_list[i].nickname);
					push_chat_data(CHAT_CHANNEL_MESSGAE,0,(UCHR *)"",print_rec);
					sprintf((char *)print_rec,MSG_SYSTEM_ADD_SAME_FRIEND,(char *)chat_friend_list[i].nickname);
					display_system_message(print_rec);
					return;
				}
			}
		}
		
	}
	

// send command 
	

	postNetMessage(NET_MSG_PLAYER_FRIEND_ADD, friend_id, (char*)&type, sizeof(type));

}





void update_friend_list(CHAT_FRIEND_LIST data)
{
	SLONG i;
	SLONG idx;
	
	idx=-1;
	for(i=0;i<MAX_CHAT_FRIEND_LIST;i++)
	{
		if(chat_friend_list[i].level<0)
		{
			idx=i;
			i=MAX_CHAT_FRIEND_LIST;
			break;
		}
	}
	if(idx<0)
		return;
	
	
	for(i=0;i<MAX_CHAT_FRIEND_LIST;i++)
	{
		if(chat_friend_list[i].level>=0)
		{
			if(chat_friend_list[i].id==data.id)
			{
				return;
			}
		}
		
	}
	
	

	memcpy(&chat_friend_list[idx],&data,sizeof(struct CHAT_FRIEND_LIST_STRUCT));

	sprintf((char *)print_rec,MSG_ADD_FRIEND,data.nickname);
	push_chat_data(CHAT_CHANNEL_MESSGAE,0,(UCHR *)"",print_rec);

	sprintf((char *)print_rec,MSG_SYSTEM_ADD_FRIEND,data.nickname);
	display_system_message(print_rec);
	
	reload_chat_friend_list();	
	
	
}



void update_friend_list_online(ULONG friend_id,char online)
{
	SLONG i;
	SLONG idx;

	idx=-1;
	for(i=0;i<MAX_CHAT_FRIEND_LIST;i++)
	{
		if(chat_friend_list[i].level<0)
		{
			continue;
		}
		if(chat_friend_list[i].id==friend_id)
		{
			chat_friend_list[i].online=online;
			switch(online)
			{
			case 0:
				sprintf((char *)print_rec,MSG_FRIEND_OFFLINE,(char *)chat_friend_list[i].nickname);
				push_chat_data(CHAT_CHANNEL_MESSGAE,0,(UCHR *)"",print_rec);
				break;
			case 1:
				sprintf((char *)print_rec,MSG_FRIEND_ONLINE,(char *)chat_friend_list[i].nickname);
				push_chat_data(CHAT_CHANNEL_MESSGAE,0,(UCHR *)"",print_rec);
				break;
			}
		reload_chat_friend_list();
		return;
		}
		
	}

	
}





const char* format_time(long long_time)
{
	static char time_string[128];

	struct tm *newtime;
	
	newtime = localtime( &long_time ); /* Convert to local time. */

	Q_snprintf(time_string, sizeof(time_string), "%.4d-%.2d-%.2d %.2d:%.2d", 
		newtime->tm_year+1900, newtime->tm_mon+1, newtime->tm_mday, 
		newtime->tm_hour, newtime->tm_min);
	
	return time_string;
}


UCHR *get_nexus_string(SLONG type,SLONG num)
{
	static UCHR text[128];
	
	text[0]=NULL;
	switch(type)
	{
	case NEXUS_FRIEND:				// һ�����ѹ�ϵ
		if(num<1000)
			strcpy((char *)text,MSG_NEXUS_FRIEND0);
		else if(num < 2000)
			strcpy((char *)text,MSG_NEXUS_FRIEND1);
		else if(num < 3000)
			strcpy((char *)text,MSG_NEXUS_FRIEND2);
		else if(num < 4000)
			strcpy((char *)text,MSG_NEXUS_FRIEND3);
		else 
			strcpy((char *)text,MSG_NEXUS_FRIEND4);

		break;
	case NEXUS_MARRIAGE:			// ����
		if(num<1000)
			strcpy((char *)text,MSG_NEXUS_MARRIAGE0);
		else if(num < 2000)
			strcpy((char *)text,MSG_NEXUS_MARRIAGE1);
		else if(num < 3000)
			strcpy((char *)text,MSG_NEXUS_MARRIAGE2);
		else if(num < 4000)
			strcpy((char *)text,MSG_NEXUS_MARRIAGE3);
		else 
			strcpy((char *)text,MSG_NEXUS_MARRIAGE4);
			

		break;
	case NEXUS_BROTHER:				// ���
		if(num<1000)
			strcpy((char *)text,MSG_NEXUS_BROTHER0);
		else if(num < 2000)
			strcpy((char *)text,MSG_NEXUS_BROTHER1);
		else if(num < 3000)
			strcpy((char *)text,MSG_NEXUS_BROTHER2);
		else if(num < 4000)
			strcpy((char *)text,MSG_NEXUS_BROTHER3);
		else 
			strcpy((char *)text,MSG_NEXUS_BROTHER4);

		break;
	case NEXUS_MASTER:				// ʦͽ
		if(num<1000)
			strcpy((char *)text,MSG_NEXUS_MASTER0);
		else if(num < 2000)
			strcpy((char *)text,MSG_NEXUS_MASTER1);
		else if(num < 3000)
			strcpy((char *)text,MSG_NEXUS_MASTER2);
		else if(num < 4000)
			strcpy((char *)text,MSG_NEXUS_MASTER3);
		else 
			strcpy((char *)text,MSG_NEXUS_MASTER4);
			

		break;
	default:
		return text;
	}


	return text;
}




void change_friend_list_friend_type(ULONG id,SLONG type)
{

	postNetMessage(NET_MSG_PLAYER_FRIEND_TYPE, id, (char*)&type, sizeof(type));

}


void update_friend_list_friend_type(ULONG id,SLONG type)
{
	SLONG i;
	
	for(i=0;i<MAX_CHAT_FRIEND_LIST;i++)
	{
		if(chat_friend_list[i].level<0)
		{
			continue;
		}
		if(chat_friend_list[i].id==id)
		{
			chat_friend_list[i].friend_type=type;
			reload_chat_friend_list();
			return;
		}
	}


}




void delete_friend_list(ULONG friend_id)
{
	SLONG i,j;
	
	

	for(i=0;i<MAX_CHAT_FRIEND_LIST;i++)		// clear temp
	{
		display_friend_list[i].online=0;
		display_friend_list[i].id=0;
		display_friend_list[i].friend_type=0;					// һ�����
		display_friend_list[i].nickname[0]=NULL;				// nicknane
		display_friend_list[i].title[0]=NULL;					// ��ν
		display_friend_list[i].level=-1;						// �ȼ�
		display_friend_list[i].base_type=0;					// ������̬
		display_friend_list[i].group[0]=0;						// ����
		display_friend_list[i].nexus=0;						// ��ϵ
		display_friend_list[i].amity=0;						// �Ѻö�

	}


	for(i=0,j=0;i<MAX_CHAT_FRIEND_LIST;i++)		// delete data 
	{
		if(chat_friend_list[i].id==friend_id)continue;
		memcpy(&display_friend_list[j],&chat_friend_list[i],sizeof(struct CHAT_FRIEND_LIST_STRUCT));
		j++;
				
	}


	for(i=0;i<MAX_CHAT_FRIEND_LIST;i++)		// restore  data 
	{
		memcpy(&chat_friend_list[i],&display_friend_list[i],sizeof(struct CHAT_FRIEND_LIST_STRUCT));
	}
	
	reload_chat_friend_list();
	
}


void refresh_friend_list(CHAT_FRIEND_LIST data)
{
	SLONG i;
	SLONG idx;
	
	idx=-1;
	for(i=0;i<MAX_CHAT_FRIEND_LIST;i++)
	{
		if(chat_friend_list[i].level<0)
			continue;
		if(chat_friend_list[i].id==data.id)
		{
			idx=i;
			i=MAX_CHAT_FRIEND_LIST;
			break;
		}
	}

	if(idx<0)
		return;
	
	memcpy(&chat_friend_list[idx],&data,sizeof(struct CHAT_FRIEND_LIST_STRUCT));
	
	reload_chat_friend_list();	
	
	memcpy(&send_message_data,&data,sizeof(struct CHAT_FRIEND_LIST_STRUCT));
	
}




void display_hirstory_chat_inter_window(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	
	SLONG word_color;
	SLONG word_count;
	SLONG main_pass;
	SLONG idx;
	SLONG text_yl;
	SLONG i;
	SLONG title_offset;
	SLONG font_height;
	UCHR text[1024];
	UCHR title[1024];
	UCHR inst[1024];
	RECT window_rect;
	SLONG display_x,display_y;

	bool show_title;
	bool show_name;
	
	
	window_x=system_image.history_body.rect.left+22+40;
	window_y=system_image.history_body.rect.top+76;
	window_xl=CHAT_WINDOW_WIDTH-40;
	window_yl=200; 
	
	
	window_rect.left=window_x;
	window_rect.top=window_y;
	window_rect.right=window_x+window_xl;
	window_rect.bottom=window_y+window_yl;
	
	
	word_count=0;
	main_pass=0;
	text_yl=0;
	idx=hirstory_chat_data_buffer.display_index-1;

	if(hirstory_chat_data_buffer.bottom_index==hirstory_chat_data_buffer.display_index)
		main_pass=1;				// first time enter

	while(main_pass==0)
	{
		if(idx<0)
			idx=MAX_CHAT_RECORD-1;
		if(idx==hirstory_chat_data_buffer.bottom_index)
			main_pass=1;
		
		window_rect.left=window_x;
		window_rect.top=window_y;
		window_rect.right=window_x+window_xl;
		window_rect.bottom=window_y+window_yl;
		

		strcpy((char *)inst,"");
		switch(hirstory_chat_data_buffer.record[idx].channel)
		{
		case CHAT_CHANNEL_PERSON:			// ˽��
			if(hirstory_chat_data_buffer.record[idx].user_id==system_control.control_user_id)
				sprintf((char *)inst,MSG_PERSON_SEND,hirstory_chat_data_buffer.record[idx].nickname);
			else
				sprintf((char *)inst,MSG_PERSON_RECEIVE,hirstory_chat_data_buffer.record[idx].nickname);
			show_name=false;
			break;
		case CHAT_CHANNEL_SCREEN:			// ��ǰƵ��
		case CHAT_CHANNEL_TEAM:				// ����
		case CHAT_CHANNEL_GROUP:			// ����
		case CHAT_CHANNEL_SELLBUY:			// ����
		case CHAT_CHANNEL_WORLD:			// ����
			show_name=true;
			break;
		case CHAT_CHANNEL_SYSTEM:			// ϵͳ˵��
			show_name=false;
			break;
		case CHAT_CHANNEL_MESSGAE:			// ���ѶϢ
			show_name=false;
		default:
			show_name=false;
			break;
			
		}
		
		if(show_name)
			sprintf((char *)text,"#C00FFFF%s��#N%s",hirstory_chat_data_buffer.record[idx].nickname,hirstory_chat_data_buffer.record[idx].text);
		else
			sprintf((char *)text,"%s%s",inst,hirstory_chat_data_buffer.record[idx].text);
		text_out_data.g_pChat->insertString((char *)text,window_rect,false);
		text_yl+=window_rect.bottom-window_rect.top;		

		if(text_yl>=window_yl)
		{
			main_pass=1;
		}
		else
		{
			word_count++;
			idx--;
		}
		
	}
	
	display_x=window_x;
	display_y=window_y+window_yl;
	idx=hirstory_chat_data_buffer.display_index-1;
	for(i=0;i<word_count;i++)
	{
		if(idx<0)
			idx=MAX_CHAT_RECORD-1;
		
		window_rect.left=window_x;
		window_rect.top=window_y;
		window_rect.right=window_x+window_xl;
		window_rect.bottom=window_y+window_yl;
		
		strcpy((char *)inst,"");
		show_title=true;
		show_name=true;
		switch(hirstory_chat_data_buffer.record[idx].channel)
		{
		case CHAT_CHANNEL_SCREEN:			// ��ǰƵ��
			strcpy((char *)title,(char *)MSG_CHANNEL_NAME_SCREEN);
			word_color=color_control.light_green;
			show_name=true;
			break;
		case CHAT_CHANNEL_TEAM:				// ����
			strcpy((char *)title,(char *)MSG_CHANNEL_NAME_TEAM);
			word_color=color_control.oringe;
			show_name=true;
			break;
		case CHAT_CHANNEL_PERSON:			// ˽��
			strcpy((char *)title,(char *)MSG_CHANNEL_NAME_PERSON);
			word_color=color_control.white;
			if(hirstory_chat_data_buffer.record[idx].user_id==system_control.control_user_id)
				sprintf((char *)inst,MSG_PERSON_SEND,hirstory_chat_data_buffer.record[idx].nickname);
			else
				sprintf((char *)inst,MSG_PERSON_RECEIVE,hirstory_chat_data_buffer.record[idx].nickname);
			show_name=false;
			break;
		case CHAT_CHANNEL_GROUP:			// ����
			strcpy((char *)title,(char *)MSG_CHANNEL_NAME_GROUP);
			word_color=color_control.light_blue;
			show_name=true;
			break;
		case CHAT_CHANNEL_SELLBUY:			// ����
			strcpy((char *)title,(char *)MSG_CHANNEL_NAME_SELLBUY);
			word_color=color_control.yellow;
			show_name=true;
			break;
		case CHAT_CHANNEL_WORLD:			// ����
			strcpy((char *)title,(char *)MSG_CHANNEL_NAME_WORLD);
			word_color=color_control.pink;
			show_name=true;
			break;
		case CHAT_CHANNEL_SYSTEM:			// ϵͳ˵��
			strcpy((char *)title,(char *)MSG_CHANNEL_NAME_SYSTEM);
			word_color=color_control.red;
			show_name=false;
			break;
		case CHAT_CHANNEL_MESSGAE:			// ���ѶϢ
			strcpy((char *)title,(char *)MSG_CHANNEL_NAME_MESSAGE);
			word_color=color_control.red;
			show_name=false;
			break;
		default:
			strcpy((char *)title,"");
			show_title=false;
			show_name=false;
			break;
		}
		
		
		if(show_name)
			sprintf((char *)text,"#C00FFFF%s��#N%s",hirstory_chat_data_buffer.record[idx].nickname,hirstory_chat_data_buffer.record[idx].text);
		else
			sprintf((char *)text,"%s%s",inst,hirstory_chat_data_buffer.record[idx].text);
		
		title_offset=text_out_data.g_pChat->insertString((char *)text,window_rect,false);
		
		font_height=window_rect.bottom-window_rect.top;
		display_y=display_y-font_height;
		
		if(idx%2==0)
			alpha_put_bar(display_x,display_y,window_xl-2,font_height,color_control.white,screen_buffer,30);
		
		
		if(show_title)
		{
			
			put_bar(window_x-40+2,display_y+2+title_offset,36,16,color_control.black,screen_buffer);
			put_bar(window_x-40+2,display_y+2+title_offset,34,14,color_control.low_gray,screen_buffer);
			put_box(window_x-40+0,display_y+title_offset,36,16,color_control.white,screen_buffer);
			set_word_color(color_control.black);
			print12(window_x-40+5+1,display_y+3+1+title_offset,title,PEST_PUT,screen_buffer);
			set_word_color(word_color);
			print12(window_x-40+5,display_y+3+title_offset,title,PEST_PUT,screen_buffer);
		}
		
		text_out_data.g_pChat->updateString(display_x,display_y,window_xl,window_yl,
			(char *)text);
		
		idx--;
		
	}
	

	text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,screen_buffer,false);
	
	set_word_color(color_control.white);
	
	
}


void scroll_hirstory_chat_data(UCHR key)
{

	switch(key)
	{
	case Up:
		if(hirstory_chat_data_buffer.bottom_index==hirstory_chat_data_buffer.display_index)
			break;				// first time enter
		hirstory_chat_data_buffer.display_index--;
		if(hirstory_chat_data_buffer.display_index<0)
			hirstory_chat_data_buffer.display_index=MAX_CHAT_RECORD-1;
		if(hirstory_chat_data_buffer.display_index==hirstory_chat_data_buffer.bottom_index)
		{
			hirstory_chat_data_buffer.display_index=hirstory_chat_data_buffer.bottom_index+1;
			if(hirstory_chat_data_buffer.display_index>=MAX_CHAT_RECORD)
				hirstory_chat_data_buffer.display_index=0;
		}
		
		break;
	case Dn:
		if(hirstory_chat_data_buffer.display_index==hirstory_chat_data_buffer.top_index)
			break;
		if(hirstory_chat_data_buffer.bottom_index==hirstory_chat_data_buffer.display_index)
			break;				// first time enter
		hirstory_chat_data_buffer.display_index++;
		if(hirstory_chat_data_buffer.display_index>=MAX_CHAT_RECORD)
			hirstory_chat_data_buffer.display_index=0;

		if(hirstory_chat_data_buffer.display_index==hirstory_chat_data_buffer.bottom_index)
		{
			hirstory_chat_data_buffer.display_index=hirstory_chat_data_buffer.bottom_index+1;
			if(hirstory_chat_data_buffer.display_index>=MAX_CHAT_RECORD)
				hirstory_chat_data_buffer.display_index=0;
		}
		break;
	}
}



void push_friend_data(ULONG send_id,ULONG receive_id,long time,UCHR *name,UCHR *text)
{

	friend_data_record[friend_data_record_top_index].send_id=send_id;
	friend_data_record[friend_data_record_top_index].receive_id=receive_id;
	friend_data_record[friend_data_record_top_index].time=time;
	strcpy((char *)friend_data_record[friend_data_record_top_index].nickname,(char *)name);
	strcpy((char *)friend_data_record[friend_data_record_top_index].text,(char *)text);

	friend_data_record_top_index++;
	if(friend_data_record_top_index>=MAX_FRIEND_DATA_RECORD)
		friend_data_record_top_index=0;

	if(friend_data_record_top_index==friend_data_record_bottom_index)
	{
		friend_data_record_bottom_index++;
		if(friend_data_record_bottom_index>=MAX_FRIEND_DATA_RECORD)
			friend_data_record_bottom_index=0;
	}


}



void reload_friend_data_record(ULONG id)
{
	SLONG idx;
	SLONG main_pass;

	display_friend_data_record_top_index=0;
	display_friend_data_record_bottom_index=0;
	display_friend_data_record_display_index=0;
	
	if(friend_data_record_top_index==friend_data_record_bottom_index)	// emtry
		return;

	idx=friend_data_record_bottom_index;
	main_pass=0;
	while(main_pass==0)
	{
		if((friend_data_record[idx].send_id==id)||(friend_data_record[idx].receive_id==id))
		{
			memcpy(&display_friend_data_record[display_friend_data_record_top_index],&friend_data_record[idx],
				sizeof( struct FRIEND_DATA_RECORD_STRUCT));
			display_friend_data_record_top_index++;
		}

		idx++;
		if(idx>=MAX_FRIEND_DATA_RECORD)
			idx=0;
		if(idx==friend_data_record_top_index)
			main_pass=1;
	}
	

	display_friend_data_record_display_index=display_friend_data_record_top_index;

}




void display_friend_data_record_window(void)
{
	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	
	SLONG word_count;
	SLONG main_pass;
	SLONG idx;
	SLONG text_yl;
	SLONG i;
	SLONG title_offset;
	SLONG font_height;
	UCHR text[1024];
	RECT window_rect;
	SLONG display_x,display_y;

	
	
	window_x=system_image.history_body.rect.left+22;
	window_y=system_image.history_body.rect.top+76;
	window_xl=CHAT_WINDOW_WIDTH;
	window_yl=200; 
	
	
	window_rect.left=window_x;
	window_rect.top=window_y;
	window_rect.right=window_x+window_xl;
	window_rect.bottom=window_y+window_yl;
	
	
	word_count=0;
	main_pass=0;
	text_yl=0;
	idx=display_friend_data_record_display_index-1;

	if(display_friend_data_record_bottom_index==display_friend_data_record_display_index)
		main_pass=1;				// first time enter

	while(main_pass==0)
	{
		if(idx<0)
			idx=MAX_FRIEND_DATA_RECORD-1;
		if(idx==display_friend_data_record_bottom_index)
			main_pass=1;
		
		window_rect.left=window_x;
		window_rect.top=window_y;
		window_rect.right=window_x+window_xl;
		window_rect.bottom=window_y+window_yl;

		sprintf((char *)text,"#R%s #Y%s",format_time(display_friend_data_record[idx].time),(char *)display_friend_data_record[idx].nickname);
		text_out_data.g_pChat->insertString((char *)text,window_rect,false);
		text_yl+=window_rect.bottom-window_rect.top;		

		window_rect.left=window_x;
		window_rect.top=window_y;
		window_rect.right=window_x+window_xl;
		window_rect.bottom=window_y+window_yl;
		
		strcpy((char *)text,(char *)display_friend_data_record[idx].text);
		text_out_data.g_pChat->insertString((char *)text,window_rect,false);
		text_yl+=window_rect.bottom-window_rect.top;		
		

		if(text_yl>=window_yl)
		{
			main_pass=1;
		}
		else
		{
			word_count++;
			idx--;
		}
		
	}
	


	display_x=window_x;
	display_y=window_y+window_yl;
	idx=display_friend_data_record_display_index-1;
	for(i=0;i<word_count;i++)
	{
		if(idx<0)
			idx=MAX_CHAT_RECORD-1;
		
		window_rect.left=window_x;
		window_rect.top=window_y;
		window_rect.right=window_x+window_xl;
		window_rect.bottom=window_y+window_yl;


		strcpy((char *)text,(char *)display_friend_data_record[idx].text);
		title_offset=text_out_data.g_pChat->insertString((char *)text,window_rect,false);
		font_height=window_rect.bottom-window_rect.top;
		display_y=display_y-font_height;
		
		text_out_data.g_pChat->updateString(display_x,display_y,window_xl,window_yl,
			(char *)text);
		


		window_rect.left=window_x;
		window_rect.top=window_y;
		window_rect.right=window_x+window_xl;
		window_rect.bottom=window_y+window_yl;
		
		sprintf((char *)text,"#R%s #Y%s",format_time(display_friend_data_record[idx].time),(char *)display_friend_data_record[idx].nickname);

		title_offset=text_out_data.g_pChat->insertString((char *)text,window_rect,false);
		font_height=window_rect.bottom-window_rect.top;
		display_y=display_y-font_height;
		alpha_put_bar(display_x,display_y,window_xl-2,font_height,color_control.white,screen_buffer,30);
		
		text_out_data.g_pChat->updateString(display_x,display_y,window_xl,window_yl,
			(char *)text);
		
		
		idx--;
		
	}
	

	text_out_data.g_pChat->updateframe(text_out_data.pt,0.07,screen_buffer,false);
	
	set_word_color(color_control.white);
	
	
}


void scroll_friend_data_record(UCHR key)
{

	switch(key)
	{
	case Up:
		if(display_friend_data_record_bottom_index==display_friend_data_record_display_index)
			break;				// first time enter
		display_friend_data_record_display_index--;
		if(display_friend_data_record_display_index<0)
			display_friend_data_record_display_index=MAX_FRIEND_DATA_RECORD-1;
		if(display_friend_data_record_display_index==display_friend_data_record_bottom_index)
		{
			display_friend_data_record_display_index=display_friend_data_record_bottom_index+1;
			if(display_friend_data_record_display_index>=MAX_FRIEND_DATA_RECORD)
				display_friend_data_record_display_index=0;
		}
		
		break;
	case Dn:
		if(display_friend_data_record_display_index==display_friend_data_record_top_index)
			break;
		if(display_friend_data_record_bottom_index==display_friend_data_record_display_index)
			break;				// first time enter
		display_friend_data_record_display_index++;
		if(display_friend_data_record_display_index>=MAX_FRIEND_DATA_RECORD)
			display_friend_data_record_display_index=0;

		if(display_friend_data_record_display_index==display_friend_data_record_bottom_index)
		{
			display_friend_data_record_display_index=display_friend_data_record_bottom_index+1;
			if(display_friend_data_record_display_index>=MAX_FRIEND_DATA_RECORD)
				display_friend_data_record_display_index=0;
		}
		break;
	}
}



SLONG jpg_map_read(BYTE* pJpegData, SLONG nLenJData, JPEGAREA& area,
				   BYTE** pBmpData, SLONG& nLenBData )
{

	MAP_JPG_HEAD *pMapj = (MAP_JPG_HEAD*) pJpegData;
	
	SLONG x, y;
	SLONG i, j;
	SLONG size = sizeof(MAP_JPG_HEAD);
	
	JPEGAREA arj = {0,0, MAP_SRCEEN_WIDTH,MAP_SRCEEN_HEIGHT};
	
	if(area.xPos%MAP_SRCEEN_WIDTH || area.yPos%MAP_SRCEEN_HEIGHT
		|| area.nWidth != MAP_SRCEEN_WIDTH 
		|| area.nHeight !=  MAP_SRCEEN_HEIGHT)
	{
		display_message((UCHR*)"map jpg erro set", true);	
		
		return E_FAIL;
	}
	
	i = j = 0;
	x = area.xPos/MAP_SRCEEN_WIDTH;
	y = area.yPos/MAP_SRCEEN_HEIGHT;
	
	//����õ�����jpg����ʼƫ��
	for(j = 0; j < y+1; j++)
	{
		if(y == j)
		{
			for	(i = 0; i < x; i++)
			{
				size += pMapj->array_len[j][i];
			}
		}
		else
		{
			for	(i = 0; i < pMapj->columns; i++)
			{
				size += pMapj->array_len[j][i];
			}			
		}
	}
	
	if(jpeg_read(pJpegData+size,pMapj->array_len[y][x],arj,pBmpData,nLenBData))
	{
		display_error_message((UCHR*)"map jpg erro read", true);	
		
		return E_FAIL;
	}
	
	return 0;
}




SLONG read_control_ani_file(UCHR *filename)
{
FILE *fp;
SLONG i;
SLONG new_handle[MAX_MDA_HANDLE];                 // MAX_MDA_HANDLE
SLONG handle;
SLONG ccl;
UCHR text[255];


		for(i=0;i<MAX_MDA_HANDLE;i++)
			new_handle[i]=-1;

        fp=fopen((char *)filename,"rb");
        if(fp==NULL)
			{
			sprintf((char *)print_rec,"Can Read file %s",filename);
			display_message(filename,true);
			return(TTN_NOT_OK);
			}	

// --- (0) Close File First
// ------ (0-1) close character view & mda handle
// ------ (0-2) close layer view
// ------ (0-3)
        free_frame_data();


// -- Ok ! ready to read file
// --- (1) Read Control_ani_head
        fread(&control_ani_head,sizeof(struct CONTROL_ANI_HEAD_STRUCT),1,fp);

// ---(2) Read Character view struct
        if(control_ani_head.total_character>0)
			{
			char_view_data=(CHAR_VIEW_DATA *)malloc(sizeof(struct CHAR_VIEW_DATA_STRUCT)*control_ani_head.total_character);
			if(char_view_data==NULL)
				{
				display_message((UCHR *)"Memory error (CHAR_VIEW_DATA)",true);
				fclose(fp);
				return(TTN_NOT_OK);
				}
			fread(char_view_data,sizeof(struct CHAR_VIEW_DATA_STRUCT)*control_ani_head.total_character,1,fp);
			for(i=0;i<control_ani_head.total_character;i++)
				{
				sprintf((char *)text,"ANI\\%s",char_view_data[i].filename);
				if(mda_group_open(text,&handle)!=TTN_OK)
					{
					sprintf((char *)print_rec,"File %s open error",text);
					display_message(print_rec,true);
					free(char_view_data);
					fclose(fp);
					return(TTN_NOT_OK);
					}
				if(handle<0)
					{
					sprintf((char *)print_rec,"File %s open error",text);
					display_message(print_rec,true);
					free(char_view_data);
					fclose(fp);
					return(TTN_NOT_OK);
					}
// change handle
				if((char_view_data[i].handle<0)||
					(char_view_data[i].handle>=MAX_MDA_HANDLE))
					{
					display_message((UCHR *)"Handle error",true);
					free(char_view_data);
					fclose(fp);
					return(TTN_NOT_OK);
					}
				new_handle[char_view_data[i].handle]=handle;
				char_view_data[i].handle=handle;
	
// stephen cancel for not proview
//				mda_group_exec(handle,(UCHR *)"MainCommand0",400,500,&proview_screen_buffer,
//					MDA_COMMAND_FIRST,100,0,0);

				}
			}

// ---(3) Read Layer view struct
        if(control_ani_head.total_layer>0)
			{
			layer_view_data=(LAYER_VIEW_DATA *)malloc(sizeof(struct LAYER_VIEW_DATA_STRUCT)*control_ani_head.total_layer);
			if(layer_view_data==NULL)
				{
				display_message((UCHR *)"Memory error (LAYER_VIEW_DATA)",true);
				fclose(fp);
				return(TTN_NOT_OK);
				}
			fread(layer_view_data,sizeof(struct LAYER_VIEW_DATA_STRUCT)*control_ani_head.total_layer,1,fp);
			for(i=0;i<control_ani_head.total_layer;i++)
				{
// ----- change new handle
				handle=new_handle[layer_view_data[i].handle];
				if((handle<0)||(handle>=MAX_MDA_HANDLE))
					{
					display_message((UCHR *)"Handle error",true);
					free(layer_view_data);
					fclose(fp);
					return(TTN_NOT_OK);
					}
// save new handle 
				layer_view_data[i].handle=handle;
				}
			}
// ---(4) Read Frame_data
        create_new_frame(control_ani_head.total_frame);
        ani_total_frame=control_ani_head.total_frame;
        for(i=0;i<control_ani_head.total_frame;i++)
			{
			fread(&frame_data[i].center_x,sizeof(SLONG),1,fp);
			fread(&frame_data[i].center_y,sizeof(SLONG),1,fp);
			fread(&frame_data[i].zoom,sizeof(SLONG),1,fp);

			fread(&frame_data[i].effect,sizeof(SLONG),1,fp);
			fread(&frame_data[i].fade_alpha_value,sizeof(SLONG),1,fp);
			fread(&frame_data[i].fade_alpha_color_r,sizeof(SLONG),1,fp);
			fread(&frame_data[i].fade_alpha_color_g,sizeof(SLONG),1,fp);
			fread(&frame_data[i].fade_alpha_color_b,sizeof(SLONG),1,fp);

			fread(&frame_data[i].key_frame,sizeof(SLONG),1,fp);
			fread(&frame_data[i].total_handle,sizeof(SLONG),1,fp);

			if(frame_data[i].total_handle>0)
				{
				frame_data[i].handle=(SLONG *)malloc(sizeof(SLONG)*frame_data[i].total_handle);
				frame_data[i].character_frame_no=(SLONG *)malloc(sizeof(SLONG)*frame_data[i].total_handle);
				frame_data[i].track_no=(SLONG *)malloc(sizeof(SLONG)*frame_data[i].total_handle);
				frame_data[i].char_x=(SLONG *)malloc(sizeof(SLONG)*frame_data[i].total_handle);
				frame_data[i].char_y=(SLONG *)malloc(sizeof(SLONG)*frame_data[i].total_handle);

				
				frame_data[i].zoom_value=(SLONG *)malloc(sizeof(SLONG)*frame_data[i].total_handle);
				frame_data[i].alpha_value=(SLONG *)malloc(sizeof(SLONG)*frame_data[i].total_handle);

				if((frame_data[i].handle==NULL)||
					(frame_data[i].character_frame_no==NULL)||
					(frame_data[i].track_no==NULL)||
					(frame_data[i].char_x==NULL)||
					(frame_data[i].char_y==NULL)||
					(frame_data[i].zoom_value==NULL)||
					(frame_data[i].alpha_value==NULL))
					{
					display_message((UCHR *)"Memory error",true);
					fclose(fp);
					return(TTN_NOT_OK);
					}
				fread(frame_data[i].handle,sizeof(SLONG)*frame_data[i].total_handle,1,fp);

// ----- change new handle
				for(ccl=0;ccl<frame_data[i].total_handle;ccl++)
					{
					handle=new_handle[frame_data[i].handle[ccl]];
					if((handle<0)||(handle>=MAX_MDA_HANDLE))
						{
						display_message((UCHR *)"Handle error",true);
						fclose(fp);
						return(TTN_NOT_OK);
						}
					frame_data[i].handle[ccl]=handle;                   
					}

				fread(frame_data[i].character_frame_no,sizeof(SLONG)*frame_data[i].total_handle,1,fp);
				fread(frame_data[i].track_no,sizeof(SLONG)*frame_data[i].total_handle,1,fp);
				fread(frame_data[i].char_x,sizeof(SLONG)*frame_data[i].total_handle,1,fp);
				fread(frame_data[i].char_y,sizeof(SLONG)*frame_data[i].total_handle,1,fp);
				fread(frame_data[i].zoom_value,sizeof(SLONG)*frame_data[i].total_handle,1,fp);
				fread(frame_data[i].alpha_value,sizeof(SLONG)*frame_data[i].total_handle,1,fp);
				}
			}
        fclose(fp);
		return(TTN_OK);
}



void free_control_ani_file(void)
{
	SLONG i;
	
	// ----------- Part(0) Free MDA Handle
	if(char_view_data!=NULL)
	{
		for(i=0;i<control_ani_head.total_character;i++)
		{
			if(char_view_data[i].handle<0)continue;
			mda_group_close(&char_view_data[i].handle);
		}
	}
	// ----------- 
	free_frame_data();
	
}


void free_frame_data(void)
{
	SLONG i;
	
	
	if(char_view_data!=NULL)
		free(char_view_data);
	char_view_data=NULL;
	if(layer_view_data!=NULL)
		free(layer_view_data);
	layer_view_data=NULL;
	
	if(frame_data!=NULL)
	{
		for(i=0;i<ani_total_frame;i++)
		{
			if(frame_data[i].handle!=NULL)
				free(frame_data[i].handle);
			frame_data[i].handle=NULL;
			
			if(frame_data[i].track_no!=NULL)
				free(frame_data[i].track_no);
			frame_data[i].track_no=NULL;
			
			if(frame_data[i].character_frame_no!=NULL)
				free(frame_data[i].character_frame_no);
			frame_data[i].character_frame_no=NULL;
			
			if(frame_data[i].char_x!=NULL)
				free(frame_data[i].char_x);
			frame_data[i].char_x=NULL;
			
			if(frame_data[i].char_y!=NULL)
				free(frame_data[i].char_y);
			frame_data[i].char_y=NULL;
			
			if(frame_data[i].zoom_value!=NULL)
				free(frame_data[i].zoom_value);
			frame_data[i].zoom_value=NULL;
			
			if(frame_data[i].alpha_value!=NULL)
				free(frame_data[i].alpha_value);
			frame_data[i].alpha_value=NULL;
		}
		free(frame_data);
	}
	frame_data=NULL;
	ani_now_frame_no=0;
	ani_total_frame=0;
}




/*******************************************************************************************************************}
{																													}
{	NET_MESSAGE control routinue																					}
{																													}
********************************************************************************************************************/
void player_leave_team(ULONG id)
{
// --- ���� �뿪��������
	postNetMessage(NET_MSG_PLAYER_EXIT_GROUP, id);
}


void request_team_list_data(void)
{
// --- ���� �����б�����
	postNetMessage(NET_MSG_PLAYER_GET_GROUP_LIST, 0);
}


void team_request_add_list(ULONG id)
{
	postNetMessage(NET_MSG_PLAYER_ADD_TO_GROUP, id);
}


void team_request_delete_list(ULONG id)
{
	postNetMessage(NET_MSG_PLAYER_SUB_GROUP_LIST, id);	
}


void touch_system_npc(SLONG player_id,SLONG npc_id)
{

	postNetMessage(NET_MSG_PLAYER_MACRO_NPC, npc_id);
	system_control.macro_npc_id=npc_id;

}


void request_mission_data(void)
{
// -------- Send Request mission 

	postNetMessage(NET_MSG_PLAYER_TASK_LIST, 0);
}



void request_base_character_data(void)
{
	
	base_character_data.active=false;
	base_character_data.base_type=map_npc_group[system_control.control_npc_idx].npc_info.base_type;
	strcpy(base_character_data.title,map_npc_group[system_control.control_npc_idx].npc_info.title);
	strcpy(base_character_data.name,map_npc_group[system_control.control_npc_idx].npc_info.name);

// -------- Send request base character data
	postNetMessage(NET_MSG_PLAYER_DATA_CHAR_INFO, 0);

// -------- ��Ǯ����
	postNetMessage(NET_MSG_PLAYER_ITEM_MONEY, 0);
	
}


void request_base_baobao_data(void)
{
	base_baobao_data.active=false;
	base_baobao_data.data.active_idx=-1;
	top_baobao_stack=0;
// -------- Send request baobao
	postNetMessage(NET_MSG_PLAYER_BAOBAO_LIST, 0);
}


void disable_baobao_action(void)
{
	SLONG id;

	id=base_baobao_data.data.action_baobao_id;

// -------- send disable baobao action  & wait feed back clear action_baobao_id= 0;
//	base_baobao_data.data.active_idx=-1;
//	base_baobao_data.data.action_baobao_id=0;

	postNetMessage(NET_MSG_PLAYER_BAOBAO_ACTION_OFF, id);
}


void enable_baobao_action(SLONG id)
{

// -------- send enable baobao action  & wait feed back clear action_baobao_id=id;

//	base_baobao_data.data.action_baobao_id=id;

	postNetMessage(NET_MSG_PLAYER_BAOBAO_ACTION_ON, id);
}


SLONG get_baobao_data_idx(ULONG id)
{
	SLONG i;
	SLONG idx;

	idx=-1;
	for(i=0;i<MAX_BAOBAO_STACK;i++)
	{
		if(base_baobao_data.data.baobao_list[i].baobao_id==0)continue;
		if(base_baobao_data.data.baobao_list[i].baobao_id==id)
		{
			idx=i;
			break;
		}
	}

	return(idx);
}


void disable_baobao_show(ULONG id)
{
// -------- disable baobao show & wait feed back clear action_idx=idx;

	postNetMessage(NET_MSG_PLAYER_BAOBAO_CLOSE, id);		
}

void enable_baobao_show(ULONG id)
{
	
// -------- �ͳ� show baobao id, ����ʱ�� update show flag, ������ add_baobao_show(new_baobao);

	postNetMessage(NET_MSG_PLAYER_BAOBAO_FOLLOW, id);


}


void add_baobao_show(CLIENT_BAOBAO_SHOW baobao_data)
{
	BASE_NPC_INFO npc_data;
	SLONG sx,sy;
	SLONG map_npc_idx;
	SLONG follow_id;
	SLONG follow_count;
	SLONG dir;
	UCHR title[80];

	
	follow_id=baobao_data.follow_id;
	map_npc_idx=get_map_npc_index(follow_id);
	if(map_npc_idx<0)			// �����Ѿ����ڵ�ǰ��Ļ
		return;

	npc_data.id=baobao_data.id;							// ID
	strcpy((char *)npc_data.name,baobao_data.name);		// �ǳ�

	sprintf((char *)title,MSG_BAOBAO_TITLE,map_npc_group[map_npc_idx].npc_info.name);		// ��ν
	if(strlen((char *)title)>MAX_TITLE_LENGTH)
		npc_data.title[0]=NULL;
	else
		strcpy((char *)npc_data.title,(char *)title);
	npc_data.filename_id=baobao_data.filename_id;		// MDA �������� id
	npc_data.base_type=0;								// ������̬ 0 �� 8 
	npc_data.speed=0;									// �ܲ�������· 0 -> ��· 1 -> �ܲ�
	npc_data.motility=0;								// �ƶ�����
	npc_data.dir=NPC_DIR_DOWN;							// ����
	npc_data.move_type=NPC_MOVE_FOLLOW;					// �ƶ���ʽ
	npc_data.status=NULL;								// NPC Ŀǰ״̬(��ʾ)
	npc_data.life_base=baobao_data.life_base;			// Ŀǰ��ת


	follow_count=get_map_npc_follow_conut(follow_id);	// ����Ŀǰ�ж����˸������id
	sx=map_npc_group[map_npc_idx].npc_info.x;
	sy=map_npc_group[map_npc_idx].npc_info.y;
	dir=map_npc_group[map_npc_idx].npc_info.dir;

	npc_data.x=sx;										// Ŀǰ����
	npc_data.y=sy;
	npc_data.tx=sx;										// Ŀ�ĵ�����
	npc_data.ty=sy;


	add_map_npc(npc_data);								// add map npc
	
	map_npc_idx=get_map_npc_index(baobao_data.id);
	map_npc_group[map_npc_idx].follow_id=baobao_data.follow_id;
	map_npc_group[map_npc_idx].follow_range=follow_count;
	
}



void check_baobao_stack(void)
{
	SLONG i;

	total_baobao_stack=0;
	for(i=0;i<MAX_BAOBAO_STACK;i++)
	{
		if(base_baobao_data.data.baobao_list[i].baobao_id>0)
			total_baobao_stack++;
	}

	base_baobao_data.data.active_idx=-1;		// clear
	for(i=0;i<total_baobao_stack;i++)
	{
		if(base_baobao_data.data.baobao_list[i].baobao_id==0)continue;
		if(base_baobao_data.data.action_baobao_id==base_baobao_data.data.baobao_list[i].baobao_id)
		{
			base_baobao_data.data.active_idx=i;
			break;
		}
	}

}


void insert_baobao_data(SERVER_CHARACTER_BAOBAO *new_baobao)
{
	SLONG i,idx;

	idx=-1;
	for(i=0;i<MAX_BAOBAO_STACK;i++)
	{
		if(base_baobao_data.data.baobao_list[i].baobao_id==0)
		{
			idx=i;
			break;
		}
	}

	if(idx<0)
	{
		display_error_message((UCHR *)"BAOBAO STACK OVERFLOW !!!",1);
		return;
	}

	memcpy(&base_baobao_data.data.baobao_list[idx],new_baobao,sizeof(struct SERVER_CHARACTER_BAOBAO_STRUCT));
	base_baobao_data.data.show[idx]=false;
	check_baobao_stack();
		
}



void delete_baobao_data(ULONG baobao_id)
{
	SLONG i;
	bool temp_show;											// �����Ƿ���ʾ
	SERVER_CHARACTER_BAOBAO temp_baobao_list;				// �����ı�����ǰ����


	for(i=0;i<MAX_BAOBAO_STACK;i++)
	{
		if(base_baobao_data.data.baobao_list[i].baobao_id==baobao_id)
		{
			base_baobao_data.data.baobao_list[i].baobao_id=0;		// clear
			break;
		}
	}

// --- sort

	for(i=0;i<MAX_BAOBAO_STACK-1;i++)
	{
		if(base_baobao_data.data.baobao_list[i].baobao_id==0)
		{
			int j = i;

			temp_show=base_baobao_data.data.show[j];
			memcpy(&temp_baobao_list,&base_baobao_data.data.baobao_list[j],sizeof(struct SERVER_CHARACTER_BAOBAO_STRUCT));

			base_baobao_data.data.show[j]=base_baobao_data.data.show[j+1];
			memcpy(&base_baobao_data.data.baobao_list[j],&base_baobao_data.data.baobao_list[j+1],sizeof(struct SERVER_CHARACTER_BAOBAO_STRUCT));
			
			base_baobao_data.data.show[j+1]=temp_show;
			memcpy(&base_baobao_data.data.baobao_list[j+1],&temp_baobao_list,sizeof(struct SERVER_CHARACTER_BAOBAO_STRUCT));			
		}
	}	

	
	auto_close_map_npc_handle(base_baobao_data.display_handle);
	baobao_stack_index=-1;
	

}



void request_login_data(void)
{

// --- ( 0 - 0 ) ����
	request_base_character_data();
	if(!game_control.character_skill_ready)
		request_character_skill_data();
	

// --- ( 0 - 1 ) ����
	request_base_baobao_data();
	if(!game_control.baobao_skill_ready)
		request_baobao_skill_data();
	
}



void send_character_point_adjust(void)
{
// --- Send adjust point data
//	�� character_point_back ���ϸ� server,����ʱ���� update base_character_data, ������� character_point_back ����

	base_character_data.active=false;

	postNetMessage(NET_MSG_PLAYER_DATA_POINT_BACK, 0, (char*)&character_point_back, sizeof(character_point_back));
}



void send_baobao_point_adjust(ULONG baobao_id)
{
	// --- Send adjust point data
	//	�� baobao_point_back ���ϸ� server,����ʱ���� update baobao_character_data[], ������� baobao_point_back[] ����
	
	int idx = get_baobao_data_idx(baobao_id);

	postNetMessage(NET_MSG_PLAYER_BAOBAO_POINT_BACK, baobao_id, (char*)&baobao_point_back[idx], sizeof(baobao_point_back[idx]));
}



void request_character_fastness_data(void)
{
// --- ����ʱ���� update character_fastness_data.active=true;
	postNetMessage(NET_MSG_PLAYER_FASTNESS, 0);
}

void request_baobao_fastness_data(void)
{
// --- ����ʱ���� update baobao_fastness_data.active=true;
	
	postNetMessage(NET_MSG_PLAYER_BAOBAO_FASTNESS, 0);
}


void release_baobao(void)
{
	ULONG id;

	if(baobao_stack_index<0)return;
	if(base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id==0)return;

	id=base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id;
// --- Send  ID

	postNetMessage(NET_MSG_PLAYER_BAOBAO_REMOVE, id);
}


void character_item_drop(void)
{
	if(character_pocket_data.idx<0)return;
	if(character_pocket_data.item[character_pocket_data.idx].number<=0)return;
	if(character_pocket_data.item[character_pocket_data.idx].item_id<0)return;
	send_character_item_drop(character_pocket_data.idx);
}

void change_map_npc_name(ULONG id,char *name)
{
	SLONG i;

	for(i=0;i<total_map_npc;i++)
	{
		if(map_npc_group[i].npc_info.id==id)
		{
			strcpy(map_npc_group[i].npc_info.name,name);
			return;
		}
	}
}



void request_character_pocket_data(void)
{
	// ------ Send Request pocket data 
	// --- ����ʱ���� update character_pocket_data.active = true 
	
	postNetMessage(NET_MSG_PLAYER_ITEM_LIST, 0);

}


void send_character_item_use(SLONG item_idx)
{
	if(!character_pocket_data.active)return;
	
	// ------ Send item use (idx)
	// --- ����ʱ���� ���� delete_character_item_pocket

//	delete_character_item_pocket(item_idx,1);

	postNetMessage(NET_MSG_PLAYER_ITEM_USE, item_idx);
	
}


void send_character_item_drop(SLONG item_idx)
{
	if(!character_pocket_data.active)return;
	
	// ------ Send item drop (idx)
	// --- ����ʱ���� ���� delete_character_item_pocket
	
//	delete_character_item_pocket(item_idx,999);
	postNetMessage(NET_MSG_PLAYER_ITEM_REMOVE, item_idx);	
}


void send_character_item_change(SLONG src_idx,SLONG tar_idx)
{
	if(!character_pocket_data.active)return;
	
	// ------ Send item change (src_idx,tar_idx)
	
	DWORD dwParam = ((src_idx & 0xffff) << 16) | (tar_idx & 0xffff);

	postNetMessage(NET_MSG_PLAYER_ITEM_CHANGE, dwParam);	
}


void delete_character_item_pocket(SLONG item_idx,SLONG number)
{
	if(!character_pocket_data.active)return;

	if(character_pocket_data.item[item_idx].item_id<0)
	{
		character_pocket_data.item[item_idx].number=0;
		return;
	}

	if(number>=character_pocket_data.item[item_idx].number)
	{
		character_pocket_data.item[item_idx].number=0;
		character_pocket_data.item[item_idx].item_id = -1;
	}
	else	
		character_pocket_data.item[item_idx].number-=number;

}


/*******************************************************************************************************************}
{																													}
{  ���� Rutinue																										}
{																													}
********************************************************************************************************************/
void request_deal(void)			// Ҫ����Է�����
{
	ULONG id;
	// --- Send request deal Ҫ����Է�����
	// �ɹ� ���� open_window_deal, ������� open_window_deal==TTN_NOT_OK ,Ŀǰ���ڽ���
	// ���ɹ� display_system_message("�޷���Է�����,���ܶԷ����ڽ���,ս��,������ӵ���!");
	
	id=game_control.game_deal_id;
	if(id==0)return;

	postNetMessage(NET_MSG_PLAYER_DEAL_SEND_REQUEST_RETURN, id);	

}


void send_cancel_deal(void)
{
	ULONG id;
	
	// --- Send cancel deal Ҫ��ȡ������
	// ���ܳɹ���� ���� close_window_deal
	id=game_control.game_deal_id;
	if(id==0)return;
	
	postNetMessage(NET_MSG_PLAYER_DEAL_CANCLE, 0);	
}


SLONG open_window_deal(void)
{
	
	RECT rect; 
	SLONG xl,yl;
	
	
	// --- clear deal data
	// --- My
	character_pocket_data.my_deal_idx[0]=-1;
	character_pocket_data.my_deal_idx[1]=-1;
	character_pocket_data.my_deal_idx[2]=-1;
	character_pocket_data.my_deal_number[0]=0;
	character_pocket_data.my_deal_number[1]=0;
	character_pocket_data.my_deal_number[2]=0;
	character_pocket_data.my_deal_money=0;
	// --- Other
	character_pocket_data.other_deal_filename_id[0]=-1;
	character_pocket_data.other_deal_filename_id[1]=-1;
	character_pocket_data.other_deal_filename_id[2]=-1;
	character_pocket_data.other_deal_number[0]=0;
	character_pocket_data.other_deal_number[1]=0;
	character_pocket_data.other_deal_number[2]=0;
	character_pocket_data.other_deal_money=0;

	character_pocket_data.my_deal_ready=false;
	character_pocket_data.other_deal_ready=false;

	
	if(!character_pocket_data.active)
		request_character_pocket_data();
	
	clear_detail_item_inst();
	
	
	if(game_control.window_deal>=0)			// ���ڽ���
		return(TTN_NOT_OK);
	
	SetRect(&rect, 181, 249, 181+85, 249+16);
	g_DealEdit.Init(rect, 8);
	g_DealEdit.setMaxNum(character_bank_data.now_money);
	
	game_control.deal_type=0;						// Ĭ�Ͻ�����Ʒ
	
	xl=system_image.item_public_body.rect.right-system_image.item_public_body.rect.left;
	yl=(system_image.item_public_body.rect.bottom-system_image.item_public_body.rect.top)+
		(system_image.deal_action_body.rect.bottom-system_image.deal_action_body.rect.top);
	
	rect.left=system_image.item_public_body.rect.left;
	rect.top=system_image.item_public_body.rect.top;
	rect.right=rect.left+xl;
	rect.bottom=rect.top+yl;
	game_control.window_deal=insert_window_base(rect ,&window_draw_deal,&window_exec_deal,WINDOW_TYPE_ON_TOP);
	
	return(TTN_OK);
}






void close_window_deal(void)
{
	if(game_control.window_deal<0)return;
	delete_window_base(game_control.window_deal);
	game_control.window_deal=-1;
	game_control.game_deal_id=0;

	g_DealEdit.Shutdown();			
}




void send_item_give(void)
{
// -------- Send Give command
	
// 
//
//
/*
	character_pocket_data.give_idx[3];
	character_pocket_data.give_number[3];
	character_pocket_data.give_money;
*/	
	if (game_control.game_give_id != 0)
	{
		SLONG obj_list[7];

		Q_memcpy(obj_list+0, character_pocket_data.give_idx, sizeof(character_pocket_data.give_idx));
		Q_memcpy(obj_list+3, character_pocket_data.give_number, sizeof(character_pocket_data.give_number));
		
		char num_text[128] = {0};

		g_GiveEdit.getText(num_text);

		obj_list[6] = Q_atoi(num_text);
		
		postNetMessage(NET_MSG_PLAYER_ITEM_GIVE, game_control.game_give_id, (char*)obj_list, sizeof(obj_list));
	}
}


void send_my_deal_ready(void)
{

	if(character_pocket_data.my_deal_ready)
	{
		// --- Send �ҵ�����׼�����
// character_pocket_data.my_deal_idx[3];
// character_pocket_data.my_deal_number[3];
// character_pocket_data.my_deal_money;
		
		deal_item_t deal_item = {0};
		memcpy(deal_item.item_idx, character_pocket_data.my_deal_idx, sizeof(deal_item.item_idx));
		memcpy(deal_item.item_num, character_pocket_data.my_deal_number, sizeof(deal_item.item_num));

//		deal_item.money = character_pocket_data.my_deal_money;

		deal_item.money = g_DealEdit.getNumber();
				
		postNetMessage(NET_MSG_PLAYER_DEAL_ITEM_LIST, 0, (char*)&deal_item, sizeof(deal_item));
	}
	else
	{
		// --- Send �ҵ�������������

		postNetMessage(NET_MSG_PLAYER_DEAL_RESET_ITEM_RETURN, 0);
	}
}


void send_item_deal(void)
{

// --- send item_deal

// --- ˫��������ready, �ҷ����� ȷ��
//  ����ʱ������� close_window_deal();

	postNetMessage(NET_MSG_PLAYER_DEAL_OK, 0);
}



void refresh_all_map_npc(void)							// ����Ҫ���ͼ���� NPC
{
// --- Send refresh all map npc

	postNetMessage(NET_MSG_PLAYER_REFRESH_MAP, 0);
}



void send_character_equip_take_off(SLONG idx)
{
// --- Send take off eruip  ȡ��װ��

	postNetMessage(NET_MSG_PLAYER_ITEM_DISCHARGE, idx);
}


void set_buy_item_index(SLONG idx)
{
// send �����̵���Ʒ idx


//	store_data.idx;			// ���� idx
//	store_data.buy_number;	// ����

	DWORD param = (store_data.idx << 16) | (store_data.buy_number & 0xffff);

	postNetMessage(NET_MSG_PLAYER_STORE_BUY, param);
}


void set_sell_item_index(SLONG idx,SLONG number)
{
// send ���Ŀڴ� idx,���� number
	
//	character_pocket_data.idx;
//	character_pocket_data.sell_number;

	DWORD param = (idx << 16) | (number & 0xffff);
	
	postNetMessage(NET_MSG_PLAYER_STORE_SELL, param);	
}


void send_store_money(SLONG money)
{
// ---- Send ��Ǯ
	postNetMessage(NET_MSG_PLAYER_BANK_SAVE, money);
}

void send_withdraw_money(SLONG money)
{
// ---- Send ���
	postNetMessage(NET_MSG_PLAYER_BANK_GIVE, money);
}


void set_push_item_index(SLONG idx,SLONG number)
{
// send ���ڵ��̿ڴ� idx,���� number
	
//	character_popshop_data.idx;
//	character_popshop_data.sell_number;

	DWORD param = (idx << 16) | (number & 0xffff);
	
	postNetMessage(NET_MSG_PLAYER_POPSHOP_ITEM_PUSH, param);
}


void set_pop_item_index(SLONG idx,SLONG number)
{
// send ȡ�����̿ڴ� idx,���� number
	
//	character_popshop_data.idx;
//	character_popshop_data.sell_number;

	DWORD param = (idx << 16) | (number & 0xffff);
	
	postNetMessage(NET_MSG_PLAYER_POPSHOP_ITEM_POP, param);	
}



void request_character_popshop_data(void)
{
// ---- Send Ҫ��������  
// ����ʱ���� update  character_popshop_data.active=true;
// ���϶��� character_popshop_data 

	postNetMessage(NET_MSG_PLAYER_POPSHOP_ITEM_LIST, 0);
}



void request_detail_inst(void)
{
// --- Send ��Ʒ����˵��Ҫ��
	
//	detail_item_inst.type;		����
//	detail_item_inst.idx;		idx
//	sprintf((char *)print_rec,"����%d->%d,%d#E˵������#Eװ������#Ecakscjascjasc#Eakdcvjsdlvk#Ecsdvsdvjksd#Ecsdvsdv#Evsdvsdv#E#E#Ecdscvsd",detail_item_inst.type,detail_item_inst.idx,detail_item_inst.old_idx);
//	display_system_message(print_rec);

//	detail_item_inst.active=true;
//	strcpy(detail_item_inst.inst,(char *)print_rec);

	int param = (detail_item_inst.type << 16) | (detail_item_inst.idx & 0xffff);

	postNetMessage(NET_MSG_PLAYER_ITEM_INFO, param);
}



void send_fight_ready(void)
{
// ---- ���� FIGHT_READY ������

	postNetMessage(NET_MSG_PLAYER_FIGHT_CLIENT_READY, 0);
}


void request_character_skill_data(void)
{
// ---- ���� request skill data
// --- ����ʱ���� update  game_control.character_skill_data_ready=true;

	postNetMessage(NET_MSG_PLAYER_SKILL_LIST, 0);
}

void request_baobao_skill_data(void)
{
// ---- ���� request baobao skill data
// --- ����ʱ���� update  game_control.baobao_skill_data_ready=true;

//	postNetMessage(NET_MSG_PLAYER_PET_SKILL_LIST, )
}


void send_baobao_item_use(SLONG baobao_id,SLONG item_idx)
{
	if(!character_pocket_data.active)return;
	
// ------ Send baobao item use (idx)
// --- ����ʱ���� ���� delete_character_item_pocket
	
//	delete_character_item_pocket(item_idx,1);
	
}


void exec_character_auto_full_hp(void)
{
// ------ Send character auto full hp

	postNetMessage(NET_MSG_PLAYER_NOW_HP_CHANGE, 0);
	
}

void exec_character_auto_full_mp(void)
{
// ------ Send character auto full mp
	postNetMessage(NET_MSG_PLAYER_NOW_MP_CHANGE, 0);
}


void exec_baobao_auto_full_hp(ULONG baobao_id)
{
// ------ Send baobao auto full hp
	postNetMessage(NET_MSG_PLAYER_PET_NOW_HP_CHANGE, 0);
}


void exec_baobao_auto_full_mp(ULONG baobao_id)
{
// ------ Send baobao auto full mp
	postNetMessage(NET_MSG_PLAYER_PET_NOW_MP_CHANGE, 0);
}


void fight_charge_start(void)				// ս����ʼ���	,����ս����һ�κ���
{
	charge_time_control(FIGHT_COMMAND_CHARACTER,CHARGE_TIME_INITIALIZE);	// ���
	charge_time_control(FIGHT_COMMAND_BAOBAO,CHARGE_TIME_INITIALIZE);		// ���
	charge_time_control(FIGHT_COMMAND_CHARACTER,CHARGE_TIME_CONTINUE);		// ��ʼ
	charge_time_control(FIGHT_COMMAND_BAOBAO,CHARGE_TIME_CONTINUE);			// ��ʼ
}

void fight_charge_pause(void)				// ս����ͣ���
{
	charge_time_control(FIGHT_COMMAND_CHARACTER,CHARGE_TIME_PAUSE);	// ��ͣ
}

void fight_charge_continue(void)			// ս���������
{
	charge_time_control(FIGHT_COMMAND_CHARACTER,CHARGE_TIME_CONTINUE);	// ��ͣ
}



void send_character_fight_command(void)
{
// --- ������������ս������
//	character_fight_command_data

	if(fight_control_data.character_command_send)return;
				
	if(!fight_control_data.character_command_ready)return;
	postNetMessage(NET_MSG_PLAYER_FIGHT_COMMAND, 0, (char*)&character_fight_command_data, 
		sizeof(character_fight_command_data));

	fight_control_data.character_command_send=true;
}


void send_baobao_fight_command(void)
{
// --- ������������ս������
//	baobao_fight_command_data	

	if(fight_control_data.baobao_command_send)return;
	
	if(!fight_control_data.baobao_command_ready)return;
	postNetMessage(NET_MSG_PLAYER_PET_FIGHT_COMMAND, 0, (char*)&baobao_fight_command_data, 
		sizeof(baobao_fight_command_data));

	fight_control_data.baobao_command_send=true;
	
}


void send_fight_action_finish(void)
{
// --- ���Ͷ����������
	postNetMessage(NET_MSG_PLAYER_FIGHT_PLAY_FINISH, 0);
}