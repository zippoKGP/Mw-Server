/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : graph.cpp
  *owner  		: Stephen
  *description  : 
  *modified     : 2004/12/6
******************************************************************************/ 

#include "engine_global.h"
#include "graph.h"
#include "vari-ext.h"
#include "taskman.h"

void put_scroll_box(SLONG sx,SLONG sy,SLONG ex,SLONG ey,UHINT color,UHINT color1,BMP *bit_map)
{
	SLONG x,y;
	SLONG step;
	

	if((sx<0)||(sy<0)||(ex<0)||(ey<0))
		return;
	if((sx>=bit_map->w)||(sy>=bit_map->h)||
		(ex>=bit_map->w)||(ey>=bit_map->h))
		return;


	if(ex>=bit_map->w-1)ex=bit_map->w-1;
	if(ey>=bit_map->h-1)ey=bit_map->h-1;
	if(sx<0)sx=0;
	if(sy<0)sy=0;
	
	for(x=sx;x<ex;x++)
	{
		step=(x+timer_tick00)/8;
		if(step%2==0)
		{
			bit_map->line[sy][x]=color;
			bit_map->line[ey][x]=color;
		}
		else
		{
			bit_map->line[sy][x]=color1;
			bit_map->line[ey][x]=color1;
		}
	}
	for(y=sy;y<ey;y++)
	{
		step=(y+timer_tick00)/8;
		if(step%2==0)
		{
			bit_map->line[y][sx]=color;
			bit_map->line[y][ex]=color;
		}
		else
		{
			bit_map->line[y][sx]=color1;
			bit_map->line[y][ex]=color1;
		}
	}
	
}


void auto_change_screen_effect(void)
{
	SLONG type;

	type=rand()%(4+9);

	switch(type)
	{
	case 0:
		change_screen_effect0();
		break;
	case 1:
		change_screen_effect1();
		break;
	case 2:
		change_screen_effect2();
		break;
	case 3:
		change_screen_effect3(8);
		break;
	case 4:
		change_screen_effect_flc(0);
		break;
	case 5:
		change_screen_effect_flc(1);
		break;
	case 6:
		change_screen_effect_flc(2);
		break;
	case 7:
		change_screen_effect_flc(3);
		break;
	case 8:
		change_screen_effect_flc(4);
		break;
	case 9:
		change_screen_effect_flc(5);
		break;
	case 10:
		change_screen_effect_flc(6);
		break;
	case 11:
		change_screen_effect_flc(7);
		break;
	case 12:
		change_screen_effect_flc(8);
		break;
	default:
		change_screen_effect0();
		break;
	}


}


// change screen effect screen 0 -> screen_buffer
// ---- Use channel2 to backup
void change_screen_effect0(void)
{
	FADE_POINT fade_point[SCREEN_HEIGHT];
	SLONG i,j;
	SLONG main_pass;
	SLONG alpha_data;
	SLONG x_stop;
	
	
	for(i=0;i<SCREEN_HEIGHT;i++)
	{
		fade_point[i].start_flag=-rand()%4;
		fade_point[i].speed=rand()%2+1;
		fade_point[i].point_alpha=0;
		fade_point[i].x=0;
	}
	get_bitmap(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,screen_channel2,screen_buffer);
	
	main_pass=0;
	while(main_pass<SCREEN_HEIGHT)
	{
		system_idle_loop();
		get_bitmap(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,screen_buffer,screen_channel2);
		main_pass=0;
		for(i=0;i<SCREEN_HEIGHT;i++)
		{
			if(fade_point[i].start_flag<0)
			{
				fade_point[i].start_flag++;
				continue;
			}
			x_stop=fade_point[i].x;
			if(x_stop>SCREEN_WIDTH)
				x_stop=SCREEN_WIDTH;
			for(j=0;j<x_stop;j++)
			{
				alpha_data=fade_point[i].x-j;
				if(alpha_data>=255)
				{
					screen_buffer->line[i][j]=screen_channel0->line[i][j];
				}
				else
				{
					oper_alpha_color( &screen_channel0->line[i][j],&screen_buffer->line[i][j],alpha_data);
				}
			}
			if(fade_point[i].x<SCREEN_WIDTH+255)
			{
				fade_point[i].x+=32;
			}
			else
			{
				main_pass++;
			}
		}

		execute_window_base();						// »æÖÆ window & Ö´ÐÐ
		
		update_screen(screen_buffer);
	}
	
}


// change screen effect screen 0 -> screen_buffer
// ---- Use channel2 to backup
void change_screen_effect1(void)
{
	SLONG i;
	SLONG x,y,xl,yl;
	
	x=0;
	y=0;
	xl=800;
	yl=600;
	for(i=0;i<9;i++)
	{
		clear_time_delay();
		system_idle_loop();
		fill_bitmap(screen_channel1,color_control.black);
		rectangle_scale_put_bmp(x,y,xl,yl,screen_channel1, 
			0,
			0,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			screen_buffer);
		x+=40;
		xl-=80;
		y+=30;
		yl-=60;
		update_screen(screen_channel1);
		wait_time_delay(SYSTEM_MAIN_DELAY);
	}
	
	x=360;
	y=270;
	xl=80;
	yl=60;
	for(i=0;i<9;i++)
	{
		clear_time_delay();
		system_idle_loop();
		fill_bitmap(screen_channel1,color_control.black);
		rectangle_scale_put_bmp(x,y,xl,yl,screen_channel1, 
			0,
			0,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			screen_channel0);
		x-=40;
		xl+=80;
		y-=30;
		yl+=60;

		execute_window_base();						// »æÖÆ window & Ö´ÐÐ
		
		update_screen(screen_channel1);
		wait_time_delay(SYSTEM_MAIN_DELAY);
	}
	get_bitmap(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,screen_buffer,screen_channel0);
	

}

#define MAX_X_BOX		10
#define MAX_Y_BOX		10

void change_screen_effect2(void)
{
	SLONG id[MAX_X_BOX*MAX_Y_BOX];
	SLONG i;
	SLONG xx;
	SLONG temp;
	SLONG x,y;
	SLONG w,h;
	
	w=SCREEN_WIDTH/MAX_X_BOX;
	h=SCREEN_HEIGHT/MAX_Y_BOX;
	for(i=0;i<MAX_X_BOX*MAX_Y_BOX;i++)
		id[i]=i;
	for(i=0;i<MAX_X_BOX*MAX_Y_BOX;i++)
	{
		xx=rand()%(MAX_X_BOX*MAX_Y_BOX);
		temp=id[i];
		id[i]=id[xx];
		id[xx]=temp;
	}
	for(i=0;i<MAX_X_BOX*MAX_Y_BOX;i++)
	{
		system_idle_loop();
		x=id[i]%MAX_X_BOX;
		y=id[i]/MAX_Y_BOX;
		bound_put_bitmap(x*w,y*h,x*w,y*h,w,h,screen_channel0,screen_buffer);

		execute_window_base();						// »æÖÆ window & Ö´ÐÐ
		
		update_screen(screen_buffer);
	}
	
	get_bitmap(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,screen_buffer,screen_channel0);
}


void change_screen_effect_flc(SLONG file_idx)
{
	SLONG total;
	SLONG i;
	SLONG idx;
	SLONG x,y;
	UCHR *buffer=NULL;
	
	buffer=(UCHR *)malloc(640*480);
	if(buffer==NULL)
	{
		return;
	}
	
	get_bitmap(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,screen_channel2,screen_buffer);
	
	sprintf((char *)print_rec,"system\\mask\\mask%d.msk",file_idx);
	total=FLIC_open_flic_file(print_rec,OPEN_FLIC_MODE_640X480,buffer);
	if(total<=0)
	{
		if(buffer!=NULL)
			free(buffer);
		buffer=NULL;
		return;
	}
	
	
	for(i=0;i<total;i++)
	{
		clear_time_delay();
		
		FLIC_read_flic_data();
		FLIC_play_flic_frame();
		
		idx=0;
		for(y=0;y<480;y++)
		{
			for(x=0;x<640;x++)
			{
				screen_channel1->line[y][x]=buffer[idx];
				idx++;
			}
		}
		
		rectangle_scale_put_bmp( 0, 0, 800, 600, map_control.screen_mask,
			0 , 0, 640, 480, screen_channel1);
		
		
		for(y=0;y<SCREEN_HEIGHT;y++)
		{
			for(x=0;x<SCREEN_WIDTH;x++)
			{
				if(map_control.screen_mask->line[y][x])
					screen_buffer->line[y][x]=screen_channel0->line[y][x];
				else
					screen_buffer->line[y][x]=screen_channel2->line[y][x];
				
			}
		}
		
		
		update_screen(screen_buffer);
		wait_time_delay(SYSTEM_MAIN_DELAY);
		
	}
	
	FLIC_close_flic_file();
	
	
	if(buffer!=NULL)
		free(buffer);
	buffer=NULL;
	get_bitmap(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,screen_buffer,screen_channel0);
	
}



void rectangle_scale_put_bmp(int des_x,int des_y,int des_w,int des_h,BMP *des,
							 int src_x,int src_y,int src_w,int src_h,const BMP *src)
{
/*
pixel_t *des_line;
pixel_t *src_line;
	*/
	int    *table_x;
	float  scalex;
	float  scaley;
	float  src_fx;
	float  src_fy;
	int    sx,sy;
	int    ex,ey;
	int    i;

	if(src==NULL)
		return;
	if(des==NULL)
		return;
	
	if(des_w>0&&des_h>0&&src_w>0&&src_h>0)
	{
		i=des->w;
		if(des_x<0)
			sx=0,src_x-=des_x;
		else if(des_x<i)
			sx=des_x;
		else return;
		if((ex=des_x+des_w)>=i)
			ex=i;
		if(ex<sx) return;
		
		i=des->h;
		if(des_y<0)
			sy=0,src_y-=des_y;
		else if(des_y<i)
			sy=des_y;
		else return;
		if((ey=des_y+des_h)>=i)
			ey=i;
		if(ey<sy) return;
		
		scalex=(float)src_w/(float)des_w;
		scaley=(float)src_h/(float)des_h;
		src_fy=(float)src_y;
		src_fx=(float)src_x;
		table_x=(int*)malloc((ex)*sizeof(int));
		for(i=sx;i<ex;i++)
			table_x[i]=(int)src_fx,src_fx+=scalex;
		while(sy<ey)
		{
		/*
		src_line=src->line[(int)src_fy];
		src_fy+=scaley;
		des_line=&des->line[sy][sx];
		for(i=sx;i<ex;i++)
		*des_line++=src_line[table_x[i]];
		sy++;
			*/
			i=(int)src_fy;
			src_fy+=scaley;
			__asm
			{
				mov   esi,src
					mov   edi,des
					mov   eax,i
					mov   ebx,sy
					mov   ecx,sx
					mov   edx,table_x
					mov   edi,[edi+ebx*4]BMP.line
					mov   esi,[esi+eax*4]BMP.line
					inc   ebx
					lea   edi,[edi+ecx*2]
					mov   sy,ebx
					mov   eax,ex
					ALIGN 4
_for_loop:
				mov   ebx,[edx+ecx*4]
					xor   eax,eax
					mov   ax,word ptr [esi+ebx*2]
					inc   ecx
					mov   [edi],ax
					mov   eax,ex
					add   edi,2
					cmp   ecx,eax
					jl   _for_loop
			}
		}
		free(table_x);
	}
}



void scale_put_dire_bmp(int win_x,int win_y,int win_w,int win_h,
						int src_w,int src_h,const unsigned short *src,
						int des_w,int des_h,unsigned short *des)
{
	int    *table_x;
	float  scalex;
	float  scaley;
	float  src_fx;
	float  src_fy;
	int    sx,sy;
	int    ex,ey;
	int    src_x;
	int    src_y;
	int    i;
	const unsigned short *src_p;
	unsigned short *des_p;
	
	if(win_w>0&&win_h>0)
	{
		if(win_x<0)
			sx=0,src_x=-win_x;
		else if(win_x<des_w)
			sx=win_x,src_x=0;
		else return;
		if((ex=win_x+win_w)>=des_w)
			ex=des_w;
		if(ex<sx) return;
		
		if(win_y<0)
			sy=0,src_y=-win_y;
		else if(win_y<des_h)
			sy=win_y,src_y=0;
		else return;
		if((ey=win_y+win_h)>=des_h)
			ey=des_h;
		if(ey<sy) return;
		
		scalex=(float)src_w/(float)win_w;
		scaley=(float)src_h/(float)win_h;
		src_fy=(float)src_y*scaley;
		src_fx=(float)src_x*scalex;
		table_x=(int*)malloc((ex)*sizeof(int));
		for(i=sx;i<ex;i++)
			table_x[i]=(int)src_fx,src_fx+=scalex;
		while(sy<ey)
		{
			i=(int)src_fy;
			src_fy+=scaley;
			src_p=src+i*src_w;
			des_p=des+sy*des_w+sx;
			__asm
			{
				mov   esi,src_p
					mov   edi,des_p
					mov   eax,i
					mov   ebx,sy
					mov   ecx,sx
					mov   edx,table_x
					inc   ebx
					mov   sy,ebx
					mov   eax,ex
					
_for_loop:
				ALIGN 4
					mov   ebx,[edx+ecx*4]
					xor   eax,eax
					mov   ax,word ptr [esi+ebx*2]
					inc   ecx
					mov   [edi],ax
					mov   eax,ex
					add   edi,2
					cmp   ecx,eax
					jl   _for_loop
			}
		}
		free(table_x);
	}
}




// change screen effect screen 0 -> screen_buffer
// ---- Use channel2 to backup
void change_screen_effect3(SLONG speed)
{
	SLONG alpha_data;
	SLONG x,y;

	get_bitmap(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,screen_channel2,screen_buffer);
	for(alpha_data=0;alpha_data<255;alpha_data+=speed)
	{
		for(y=0;y<SCREEN_HEIGHT;y++)
		{
			for(x=0;x<SCREEN_WIDTH;x++)
			{
				oper_alpha_color( &screen_channel0->line[y][x],&screen_buffer->line[y][x],alpha_data);
			}
		}
		system_idle_loop();

		execute_window_base();						// »æÖÆ window & Ö´ÐÐ
		
		update_screen(screen_buffer);
		get_bitmap(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,screen_buffer,screen_channel2);
	}
	get_bitmap(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,screen_buffer,screen_channel0);
	update_screen(screen_buffer);

}




void display_hightlight_zoom_img256_buffer(SLONG x,SLONG y,UCHR *data_buffer,BMP *bitmap,SLONG zoom_percent)
{
IMG256_HEAD temp_img256_head;
SLONG display_x,display_y;
UCHR *img256_buffer;
SLONG src_x,src_y;
SLONG yl,xl;
SLONG data_size;
SLONG end_y;
SLONG skip_length,copy_length;
SLONG decode_index;
SLONG main_pass;
SLONG xcount,ycount;
UCHR compress_type;
SLONG index;
SLONG i;
SLONG yy,ww,xx;
SLONG *xstep=NULL,*ystep=NULL;
float xscale;
float yscale;

// ------ Get Base Data
		memcpy(&temp_img256_head,data_buffer,sizeof(struct IMG256_HEAD_STRUCT));
		xl=temp_img256_head.xlen;
        yl=temp_img256_head.ylen;
		if((xl<=1)||(yl<=1))return;
        data_size=temp_img256_head.image_compress_size;
		img256_buffer=&data_buffer[temp_img256_head.img_start_offset];

// ------ Open Zoom table
		yscale=(float)zoom_percent/100;
		xscale=yscale;

//		xscale=(float)yscale*(float)0.75;

		xstep=(SLONG *)malloc(sizeof(SLONG)*xl);
		ystep=(SLONG *)malloc(sizeof(SLONG)*yl);
		for(i=0;i<xl;i++)
			{
			xstep[i]=(SLONG)((float)i*xscale);
			if(i>0)
				xstep[i-1]=xstep[i]-xstep[i-1];
			}
		xstep[xl-1]=(SLONG)((float)xl*xscale)-xstep[xl-1];

		for(i=0;i<yl;i++)
			{
			ystep[i]=(SLONG)((float)i*yscale);
			if(i>0)
				ystep[i-1]=ystep[i]-ystep[i-1];
			}
		ystep[yl-1]=(SLONG)((float)yl*yscale)-ystep[yl-1];

		src_x=x-(SLONG)((float)temp_img256_head.center_x*xscale);
		src_y=y-(SLONG)((float)temp_img256_head.center_y*yscale);
		display_x=src_x;
		display_y=src_y;


// ------ Display Loop
        end_y=display_y+yl;
        skip_length=0;
        copy_length=0;
        decode_index=0;
		ycount=0;
		xcount=0;
        main_pass=0;
        while(main_pass==0)
			{
			compress_type=img256_buffer[decode_index];
			decode_index++;
			switch(compress_type)
				{
				case IMG_SKIP:
					skip_length=img256_buffer[decode_index]+
						        (img256_buffer[decode_index+1]<<8);
					for(i=xcount;i<xcount+skip_length;i++)
						display_x=display_x+xstep[i];
					xcount=xcount+skip_length;
					decode_index=decode_index+2;
					break;
				case IMG_COPY:
					copy_length=img256_buffer[decode_index]+
								(img256_buffer[decode_index+1]<<8);
					decode_index=decode_index+2;
					xx=0;
					for(yy=0;yy<ystep[ycount];yy++)
						{
						i=0;			// ¸ê®Æ°¾²¾
						xx=0;			// ®y¼Ð°¾²¾
						while(i<copy_length)
							{
							for(ww=0;ww<xstep[xcount+i];ww++)
								{
								if(((display_x+xx)<bitmap->w)&&
									(display_y+yy<bitmap->h)&&
									(display_x+xx>=0)&&
									(display_y+yy>=0))
									{
									index=(SLONG)img256_buffer[decode_index+(i<<1)]*3;
//									color=rgb2hi(temp_img256_head.palette[index],
//										temp_img256_head.palette[index+1],temp_img256_head.palette[index+2]);

									rgb_hightlight_color( temp_img256_head.palette[index],
										temp_img256_head.palette[index+1],temp_img256_head.palette[index+2],
										&bitmap->line[display_y+yy][display_x+xx]);
									}
								xx++;		// ®y¼Ð°¾²¾
								}
							i++;			// ¸ê®Æ°¾²¾
							}
						}
					display_x=display_x+xx;
					xcount=xcount+copy_length;
					decode_index=decode_index+(copy_length<<1);
					break;
				case IMG_LINE:
					for(yy=0;yy<ystep[ycount];yy++)
						display_y++;
					display_x=src_x;
					ycount++;
					xcount=0;
					break;
				default:
					sprintf((char *)print_rec,MSG_IMG256_DECODE_ERROR,compress_type);
					display_error_message(print_rec,true);
					return;
				}
			if((decode_index>=data_size)||(ycount>=yl))
				main_pass=1;
			}

		free(xstep);
		free(ystep);

}


void rgb_hightlight_color(UCHR fr,UCHR fg,UCHR fb,UHINT *bk_color)
{
UCHR br,bg,bb;
SLONG r,g,b;

		hi2rgb(*bk_color,&br,&bg,&bb);

		r=(SLONG)fr+(SLONG)br;
		g=(SLONG)fg+(SLONG)bg;
		b=(SLONG)fb+(SLONG)bb;
		if(r>=255)r=255;
		if(g>=255)g=255;
		if(b>=255)b=255;

/*
	__asm
	{

		xor  eax,eax
		xor  ebx,ebx
		mov  al,fr
		mov  bl,br
		
		xor  ecx,ecx
		xor  edx,edx
		mov  cl,fg
		mov  dl,bg
		
		add  eax,ebx
		add  ecx,edx

		cmp  eax,0xff
		jle  _r_ok
		mov  eax,0xff
_r_ok:
		mov  r,eax

		cmp  ecx,0xff
		jle  _g_ok
		mov  ecx,0xff
_g_ok:
		mov  g,ecx

		xor  eax,eax
		xor  ebx,ebx
		mov  al,fb
		mov  bl,bb

		add  eax,ebx
		cmp  eax,0xff

		jle  _b_ok
		mov  eax,0xff
_b_ok:
		mov  b,eax
	}
*/
		*bk_color=rgb2hi((UCHR)r,(UCHR)g,(UCHR)b);

}



void oper_hightlight_color(UHINT *color,UHINT *bk_color)
{
UCHR fr,fg,fb;
UCHR br,bg,bb;
UCHR r,g,b;

		hi2rgb(*color,&fr,&fg,&fb);
		hi2rgb(*bk_color,&br,&bg,&bb);

		r=(UCHR)((SLONG)fr+(SLONG)br);
		g=(UCHR)((SLONG)fg+(SLONG)bg);
		b=(UCHR)((SLONG)fb+(SLONG)bb);
		*bk_color=rgb2hi(r,g,b);

}



void display_hightlight_img256_buffer(SLONG x,SLONG y,UCHR *data_buffer,BMP *bitmap)
{
	IMG256_HEAD temp_img256_head;
	SLONG display_x,display_y;
	UCHR *img256_buffer;
	SLONG yl;
	SLONG data_size;
	SLONG end_y;
	SLONG skip_length,copy_length;
	SLONG decode_index;
	SLONG main_pass;
	UCHR compress_type;
	SLONG index;
	SLONG i;
	//SLONG xstart;
	
	
	
	// ------ Get Base Data
	memcpy(&temp_img256_head,data_buffer,sizeof(struct IMG256_HEAD_STRUCT));
	yl=temp_img256_head.ylen;
	data_size=temp_img256_head.image_compress_size;
	display_x=x-temp_img256_head.center_x;
	display_y=y-temp_img256_head.center_y;
	img256_buffer=&data_buffer[temp_img256_head.img_start_offset];
	
	// ------ Display Loop
	end_y=display_y+yl;
	skip_length=0;
	copy_length=0;
	decode_index=0;
	main_pass=0;
	while(main_pass==0)
	{
		compress_type=img256_buffer[decode_index];
		decode_index++;
		switch(compress_type)
		{
		case IMG_SKIP:
			skip_length=img256_buffer[decode_index]+
				(img256_buffer[decode_index+1]<<8);
			display_x=display_x+skip_length;
			decode_index=decode_index+2;
			break;
		case IMG_COPY:
			copy_length=img256_buffer[decode_index]+
				(img256_buffer[decode_index+1]<<8);
			decode_index=decode_index+2;
			
			for(i=0;i<copy_length;i++)
			{
				if(((display_x+i)<bitmap->w)&&
					(display_y<bitmap->h)&&
					(display_x+i>=0)&&
					(display_y>=0))
				{
					index=(SLONG)img256_buffer[decode_index+(i<<1)]*3;
					rgb_hightlight_color( temp_img256_head.palette[index],
						temp_img256_head.palette[index+1],temp_img256_head.palette[index+2],
						&bitmap->line[display_y][display_x+i]);
					
				}
			}
			display_x=display_x+copy_length;
			decode_index=decode_index+(copy_length<<1);
			break;
		case IMG_LINE:
			display_y++;
			display_x=x-temp_img256_head.center_x;
			break;
		default:
			sprintf((char *)print_rec,MSG_IMG256_DECODE_ERROR,compress_type);
			display_error_message(print_rec,true);
			return;
		}
		if((decode_index>=data_size)||(display_y>=end_y))
			main_pass=1;
	}
	
	
}



void display_zoom_img256_buffer_alpha(SLONG x,SLONG y,UCHR *data_buffer,BMP *bitmap,SLONG zoom_percent,SLONG alpha_base)
{
	IMG256_HEAD temp_img256_head;
	SLONG display_x,display_y;
	UCHR *img256_buffer;
	SLONG src_x,src_y;
	SLONG yl,xl;
	SLONG data_size;
	SLONG end_y;
	SLONG skip_length,copy_length;
	SLONG decode_index;
	SLONG main_pass;
	SLONG xcount,ycount;
	UHINT color;
	UCHR alpha;
	UCHR compress_type;
	SLONG index;
	SLONG i;
	SLONG yy,ww,xx;
	SLONG *xstep=NULL,*ystep=NULL;
	float xscale;
	float yscale;
	SLONG alpha1;
	
	
	// ------ Get Base Data
	memcpy(&temp_img256_head,data_buffer,sizeof(struct IMG256_HEAD_STRUCT));
	xl=temp_img256_head.xlen;
	yl=temp_img256_head.ylen;
	if((xl<=1)||(yl<=1))return;
	data_size=temp_img256_head.image_compress_size;
	img256_buffer=&data_buffer[temp_img256_head.img_start_offset];
	
	// ------ Open Zoom table
	yscale=(float)zoom_percent/100;
	xscale=yscale;
	
	//		xscale=(float)yscale*(float)0.75;
	
	xstep=(SLONG *)malloc(sizeof(SLONG)*xl);
	ystep=(SLONG *)malloc(sizeof(SLONG)*yl);
	for(i=0;i<xl;i++)
	{
		xstep[i]=(SLONG)((float)i*xscale);
		if(i>0)
			xstep[i-1]=xstep[i]-xstep[i-1];
	}
	xstep[xl-1]=(SLONG)((float)xl*xscale)-xstep[xl-1];
	
	for(i=0;i<yl;i++)
	{
		ystep[i]=(SLONG)((float)i*yscale);
		if(i>0)
			ystep[i-1]=ystep[i]-ystep[i-1];
	}
	ystep[yl-1]=(SLONG)((float)yl*yscale)-ystep[yl-1];
	
	src_x=x-(SLONG)((float)temp_img256_head.center_x*xscale);
	src_y=y-(SLONG)((float)temp_img256_head.center_y*yscale);
	display_x=src_x;
	display_y=src_y;
	
	
	// ------ Display Loop
	end_y=display_y+yl;
	skip_length=0;
	copy_length=0;
	decode_index=0;
	ycount=0;
	xcount=0;
	main_pass=0;
	while(main_pass==0)
	{
		compress_type=img256_buffer[decode_index];
		decode_index++;
		switch(compress_type)
		{
		case IMG_SKIP:
			skip_length=img256_buffer[decode_index]+
				(img256_buffer[decode_index+1]<<8);
			for(i=xcount;i<xcount+skip_length;i++)
				display_x=display_x+xstep[i];
			xcount=xcount+skip_length;
			decode_index=decode_index+2;
			break;
		case IMG_COPY:
			copy_length=img256_buffer[decode_index]+
				(img256_buffer[decode_index+1]<<8);
			decode_index=decode_index+2;
			xx=0;
			for(yy=0;yy<ystep[ycount];yy++)
			{
				i=0;			// ¸ê®Æ°¾²¾
				xx=0;			// ®y¼Ð°¾²¾
				while(i<copy_length)
				{
					for(ww=0;ww<xstep[xcount+i];ww++)
					{
						if(((display_x+xx)<bitmap->w)&&
							(display_y+yy<bitmap->h)&&
							(display_x+xx>=0)&&
							(display_y+yy>=0))
						{
							index=(SLONG)img256_buffer[decode_index+(i<<1)]*3;
							alpha=img256_buffer[decode_index+(i<<1)+1];
							alpha1=(SLONG)alpha-alpha_base;
							if(alpha1<0)alpha1=0;
							if(alpha1>=255)alpha1=255;
							//									alpha=(UCHR)alpha1;
							color=rgb2hi(temp_img256_head.palette[index],
								temp_img256_head.palette[index+1],temp_img256_head.palette[index+2]);
							
							if((map_control.draw_mask)&&(alpha>=MASK_ALPHA_LEVEL))
								map_control.screen_mask->line[display_y+yy][display_x+xx]=map_control.mask_npc;
							

							if(map_layer_touch.flag)		// ÐèÒª¸úµØÍ¼×ö mask check 
							{
								if(check_map_layer_touch(map_control.start_x+display_x+xx,map_control.start_y+display_y+yy)==TTN_OK)
								{
									if(map_layer_touch.character)
										alpha1=NPC_MASK_ALPHA;				// °ëÍ¸Ã÷
									else
										continue;
								}
							}
						

							if((map_control.draw_point_color)&&(alpha>=MASK_ALPHA_LEVEL))
							{
								color=rgb2hi(temp_img256_head.palette[index]|0xFF,
									temp_img256_head.palette[index+1],temp_img256_head.palette[index+2]);
							}
							
							oper_alpha_color( &color, &bitmap->line[display_y+yy][display_x+xx], alpha1);
						}
						xx++;		// ®y¼Ð°¾²¾
					}
					i++;			// ¸ê®Æ°¾²¾
				}
			}
			display_x=display_x+xx;
			xcount=xcount+copy_length;
			decode_index=decode_index+(copy_length<<1);
			break;
		case IMG_LINE:
			for(yy=0;yy<ystep[ycount];yy++)
				display_y++;
			display_x=src_x;
			ycount++;
			xcount=0;
			break;
		default:
			sprintf((char *)print_rec,MSG_IMG256_DECODE_ERROR,compress_type);
			display_error_message(print_rec,true);
			return;
		}
		if((decode_index>=data_size)||(ycount>=yl))
			main_pass=1;
	}
	
	free(xstep);
	free(ystep);
	
}


void display_img256_buffer_alpha(SLONG x,SLONG y,UCHR *data_buffer,BMP *bitmap,SLONG alpha_base)
{
	IMG256_HEAD temp_img256_head;
	SLONG display_x,display_y;
	UCHR *img256_buffer;
	SLONG yl;
	SLONG data_size;
	SLONG end_y;
	SLONG skip_length,copy_length;
	SLONG decode_index;
	SLONG main_pass;
	UHINT color;
	UCHR alpha;
	SLONG alpha1;
	UCHR compress_type;
	SLONG index;
	SLONG i;
	//SLONG xstart;
	
	
	
	// ------ Get Base Data
	memcpy(&temp_img256_head,data_buffer,sizeof(struct IMG256_HEAD_STRUCT));
	yl=temp_img256_head.ylen;
	data_size=temp_img256_head.image_compress_size;
	display_x=x-temp_img256_head.center_x;
	display_y=y-temp_img256_head.center_y;
	img256_buffer=&data_buffer[temp_img256_head.img_start_offset];
	
	// ------ Display Loop
	end_y=display_y+yl;
	skip_length=0;
	copy_length=0;
	decode_index=0;
	main_pass=0;
	while(main_pass==0)
	{
		compress_type=img256_buffer[decode_index];
		decode_index++;
		switch(compress_type)
		{
		case IMG_SKIP:
			skip_length=img256_buffer[decode_index]+
				(img256_buffer[decode_index+1]<<8);
			display_x=display_x+skip_length;
			decode_index=decode_index+2;
			break;
		case IMG_COPY:
			copy_length=img256_buffer[decode_index]+
				(img256_buffer[decode_index+1]<<8);
			decode_index=decode_index+2;
			
			for(i=0;i<copy_length;i++)
			{
				if(((display_x+i)<bitmap->w)&&
					(display_y<bitmap->h)&&
					(display_x+i>=0)&&
					(display_y>=0))
				{
					index=(SLONG)img256_buffer[decode_index+(i<<1)]*3;
					alpha=img256_buffer[decode_index+(i<<1)+1];
					alpha1=(SLONG)alpha-alpha_base;
					if(alpha1<0)alpha1=0;
					if(alpha1>=255)alpha1=255;
					//							alpha=(UCHR)alpha1;
					color=rgb2hi(temp_img256_head.palette[index],
						temp_img256_head.palette[index+1],temp_img256_head.palette[index+2]);

					if((map_control.draw_mask)&&(alpha>=MASK_ALPHA_LEVEL))
						map_control.screen_mask->line[display_y][display_x+i]=map_control.mask_npc;
					
					if(map_layer_touch.flag)		// ÐèÒª¸úµØÍ¼×ö mask check 
					{
						if(check_map_layer_touch(map_control.start_x+display_x+i,map_control.start_y+display_y)==TTN_OK)
						{
							if(map_layer_touch.character)
								alpha1=NPC_MASK_ALPHA;				// °ëÍ¸Ã÷
							else
								continue;
						}
					}


					if((map_control.draw_point_color)&&(alpha>=MASK_ALPHA_LEVEL))
					{
						color=rgb2hi(temp_img256_head.palette[index]|0xFF,
							temp_img256_head.palette[index+1],temp_img256_head.palette[index+2]);
					}
					

					oper_alpha_color( &color, &bitmap->line[display_y][display_x+i],alpha1);
					
				}
			}
			display_x=display_x+copy_length;
			decode_index=decode_index+(copy_length<<1);
			break;
		case IMG_LINE:
			display_y++;
			display_x=x-temp_img256_head.center_x;
			break;
		default:
			sprintf((char *)print_rec,MSG_IMG256_DECODE_ERROR,compress_type);
			display_error_message(print_rec,true);
			return;
		}
		if((decode_index>=data_size)||(display_y>=end_y))
			main_pass=1;
	}
	
	
}



void display_zoom_img256_buffer(SLONG x,SLONG y,UCHR *data_buffer,BMP *bitmap,SLONG zoom_percent)
{
	IMG256_HEAD temp_img256_head;
	SLONG display_x,display_y;
	UCHR *img256_buffer;
	SLONG src_x,src_y;
	SLONG yl,xl;
	SLONG data_size;
	SLONG end_y;
	SLONG skip_length,copy_length;
	SLONG decode_index;
	SLONG main_pass;
	SLONG xcount,ycount;
	UHINT color;
	UCHR alpha;
	UCHR compress_type;
	SLONG index;
	SLONG i;
	SLONG yy,ww,xx;
	SLONG *xstep=NULL,*ystep=NULL;
	float xscale;
	float yscale;
	
	
	// ------ Get Base Data
	memcpy(&temp_img256_head,data_buffer,sizeof(struct IMG256_HEAD_STRUCT));
	xl=temp_img256_head.xlen;
	yl=temp_img256_head.ylen;
	if((xl<=1)||(yl<=1))return;
	data_size=temp_img256_head.image_compress_size;
	img256_buffer=&data_buffer[temp_img256_head.img_start_offset];
	
	// ------ Open Zoom table
	yscale=(float)zoom_percent/100;
	xscale=yscale;
	
	//		xscale=(float)yscale*(float)0.75;
	
	xstep=(SLONG *)malloc(sizeof(SLONG)*xl);
	ystep=(SLONG *)malloc(sizeof(SLONG)*yl);
	for(i=0;i<xl;i++)
	{
		xstep[i]=(SLONG)((float)i*xscale);
		if(i>0)
			xstep[i-1]=xstep[i]-xstep[i-1];
	}
	xstep[xl-1]=(SLONG)((float)xl*xscale)-xstep[xl-1];
	
	for(i=0;i<yl;i++)
	{
		ystep[i]=(SLONG)((float)i*yscale);
		if(i>0)
			ystep[i-1]=ystep[i]-ystep[i-1];
	}
	ystep[yl-1]=(SLONG)((float)yl*yscale)-ystep[yl-1];
	
	src_x=x-(SLONG)((float)temp_img256_head.center_x*xscale);
	src_y=y-(SLONG)((float)temp_img256_head.center_y*yscale);
	
	
	display_x=src_x;
	display_y=src_y;
	
	
	// ------ Display Loop
	end_y=display_y+yl;
	skip_length=0;
	copy_length=0;
	decode_index=0;
	ycount=0;
	xcount=0;
	main_pass=0;
	while(main_pass==0)
	{
		compress_type=img256_buffer[decode_index];
		decode_index++;
		switch(compress_type)
		{
		case IMG_SKIP:
			skip_length=img256_buffer[decode_index]+
				(img256_buffer[decode_index+1]<<8);
			for(i=xcount;i<xcount+skip_length;i++)
				display_x=display_x+xstep[i];
			xcount=xcount+skip_length;
			decode_index=decode_index+2;
			break;
		case IMG_COPY:
			copy_length=img256_buffer[decode_index]+
				(img256_buffer[decode_index+1]<<8);
			decode_index=decode_index+2;
			xx=0;
			for(yy=0;yy<ystep[ycount];yy++)
			{
				i=0;			// ¸ê®Æ°¾²¾
				xx=0;			// ®y¼Ð°¾²¾
				while(i<copy_length)
				{
					for(ww=0;ww<xstep[xcount+i];ww++)
					{
						if(((display_x+xx)<bitmap->w)&&
							(display_y+yy<bitmap->h)&&
							(display_x+xx>=0)&&
							(display_y+yy>=0))
						{
							index=(SLONG)img256_buffer[decode_index+(i<<1)]*3;
							alpha=img256_buffer[decode_index+(i<<1)+1];
							color=rgb2hi(temp_img256_head.palette[index],
								temp_img256_head.palette[index+1],temp_img256_head.palette[index+2]);
							
							if((map_control.draw_mask)&&(alpha>=MASK_ALPHA_LEVEL))
								map_control.screen_mask->line[display_y+yy][display_x+xx]=map_control.mask_npc;
							
							if(map_layer_touch.flag)		// ÐèÒª¸úµØÍ¼×ö mask check 
							{
								if(check_map_layer_touch(map_control.start_x+display_x+xx,map_control.start_y+display_y+yy)==TTN_OK)
								{
									if(map_layer_touch.character)
										alpha=NPC_MASK_ALPHA;				// °ëÍ¸Ã÷
									else
										continue;
								}
							}

							if((map_control.draw_point_color)&&(alpha>=MASK_ALPHA_LEVEL))
							{
								color=rgb2hi(temp_img256_head.palette[index]|0xFF,
									temp_img256_head.palette[index+1],temp_img256_head.palette[index+2]);
							}
							
							
							oper_alpha_color( &color, &bitmap->line[display_y+yy][display_x+xx],(SLONG) alpha);
						}
						xx++;		// ®y¼Ð°¾²¾
					}
					i++;			// ¸ê®Æ°¾²¾
				}
			}
			display_x=display_x+xx;
			xcount=xcount+copy_length;
			decode_index=decode_index+(copy_length<<1);
			break;
		case IMG_LINE:
			for(yy=0;yy<ystep[ycount];yy++)
				display_y++;
			display_x=src_x;
			ycount++;
			xcount=0;
			break;
		default:
			sprintf((char *)print_rec,MSG_IMG256_DECODE_ERROR,compress_type);
			display_error_message(print_rec,true);
			return;
		}
		if((decode_index>=data_size)||(ycount>=yl))
			main_pass=1;
	}
	
	free(xstep);
	free(ystep);
	
}


void display_img256_buffer(SLONG x,SLONG y,UCHR *data_buffer,BMP *bitmap)
{
	IMG256_HEAD temp_img256_head;
	SLONG display_x,display_y;
	UCHR *img256_buffer;
	SLONG yl;
	SLONG data_size;
	SLONG end_y;
	SLONG skip_length,copy_length;
	SLONG decode_index;
	SLONG main_pass;
	UHINT color;
	UCHR alpha;
	UCHR compress_type;
	SLONG index;
	SLONG i;
	//SLONG xstart;
	
	
	
	// ------ Get Base Data
	memcpy(&temp_img256_head,data_buffer,sizeof(struct IMG256_HEAD_STRUCT));
	yl=temp_img256_head.ylen;
	data_size=temp_img256_head.image_compress_size;
	// stephen
	display_x=x-temp_img256_head.center_x;
	display_y=y-temp_img256_head.center_y;
	img256_buffer=&data_buffer[temp_img256_head.img_start_offset];
	
	// ------ Display Loop
	end_y=display_y+yl;
	skip_length=0;
	copy_length=0;
	decode_index=0;
	main_pass=0;
	while(main_pass==0)
	{
		compress_type=img256_buffer[decode_index];
		decode_index++;
		switch(compress_type)
		{
		case IMG_SKIP:
			skip_length=img256_buffer[decode_index]+
				(img256_buffer[decode_index+1]<<8);
			display_x=display_x+skip_length;
			decode_index=decode_index+2;
			break;
		case IMG_COPY:
			copy_length=img256_buffer[decode_index]+
				(img256_buffer[decode_index+1]<<8);
			decode_index=decode_index+2;
			
			for(i=0;i<copy_length;i++)
			{
				if(((display_x+i)<bitmap->w)&&
					(display_y<bitmap->h)&&
					(display_x+i>=0)&&
					(display_y>=0))
				{
					index=(SLONG)img256_buffer[decode_index+(i<<1)]*3;
					alpha=img256_buffer[decode_index+(i<<1)+1];

					
					color=rgb2hi(temp_img256_head.palette[index],
						temp_img256_head.palette[index+1],temp_img256_head.palette[index+2]);

					if((map_control.draw_mask)&&(alpha>=MASK_ALPHA_LEVEL))
						map_control.screen_mask->line[display_y][display_x+i]=map_control.mask_npc;

					if(map_layer_touch.flag)		// ÐèÒª¸úµØÍ¼×ö mask check 
					{
						if(check_map_layer_touch(map_control.start_x+display_x+i,map_control.start_y+display_y)==TTN_OK)
						{
							if(map_layer_touch.character)
								alpha=NPC_MASK_ALPHA;				// °ëÍ¸Ã÷
							else
								continue;
						}
					}

					if((map_control.draw_point_color)&&(alpha>=MASK_ALPHA_LEVEL))
					{
						color=rgb2hi(temp_img256_head.palette[index]|0xFF,
							temp_img256_head.palette[index+1],temp_img256_head.palette[index+2]);
					}
					
					oper_alpha_color( &color, &bitmap->line[display_y][display_x+i],(SLONG) alpha);
				}
			}

			display_x=display_x+copy_length;
			decode_index=decode_index+(copy_length<<1);
			break;
		case IMG_LINE:
			display_y++;
			display_x=x-temp_img256_head.center_x;
			break;
		default:
			sprintf((char *)print_rec,MSG_IMG256_DECODE_ERROR,compress_type);
			display_error_message(print_rec,true);
			return;
		}
		if((decode_index>=data_size)||(display_y>=end_y))
			main_pass=1;
	}
	
	
}




void display_part_img256_buffer(SLONG x,SLONG y,SLONG cut_xl,UCHR *data_buffer,BMP *bitmap)
{
	IMG256_HEAD temp_img256_head;
	SLONG display_x,display_y;
	UCHR *img256_buffer;
	SLONG yl;
	SLONG data_size;
	SLONG end_y;
	SLONG skip_length,copy_length;
	SLONG decode_index;
	SLONG main_pass;
	UHINT color;
	UCHR alpha;
	UCHR compress_type;
	SLONG index;
	SLONG i;
	//SLONG xstart;
	
	
	
	// ------ Get Base Data
	memcpy(&temp_img256_head,data_buffer,sizeof(struct IMG256_HEAD_STRUCT));
	yl=temp_img256_head.ylen;
	data_size=temp_img256_head.image_compress_size;
	// stephen
	display_x=x-temp_img256_head.center_x;
	display_y=y-temp_img256_head.center_y;
	img256_buffer=&data_buffer[temp_img256_head.img_start_offset];
	
	// ------ Display Loop
	end_y=display_y+yl;
	skip_length=0;
	copy_length=0;
	decode_index=0;
	main_pass=0;
	while(main_pass==0)
	{
		compress_type=img256_buffer[decode_index];
		decode_index++;
		switch(compress_type)
		{
		case IMG_SKIP:
			skip_length=img256_buffer[decode_index]+
				(img256_buffer[decode_index+1]<<8);
			display_x=display_x+skip_length;
			decode_index=decode_index+2;
			break;
		case IMG_COPY:
			copy_length=img256_buffer[decode_index]+
				(img256_buffer[decode_index+1]<<8);
			decode_index=decode_index+2;
			
			for(i=0;i<copy_length;i++)
			{
				if(((display_x+i)<bitmap->w)&&
					(display_y<bitmap->h)&&
					(display_x+i>=0)&&
					(display_y>=0))
				{
					if(display_x+i>x+cut_xl)continue;

					index=(SLONG)img256_buffer[decode_index+(i<<1)]*3;
					alpha=img256_buffer[decode_index+(i<<1)+1];

					color=rgb2hi(temp_img256_head.palette[index],
						temp_img256_head.palette[index+1],temp_img256_head.palette[index+2]);

					if((map_control.draw_mask)&&(alpha>=MASK_ALPHA_LEVEL))
						map_control.screen_mask->line[display_y][display_x+i]=map_control.mask_npc;

					if(map_layer_touch.flag)		// ÐèÒª¸úµØÍ¼×ö mask check 
					{
						if(check_map_layer_touch(map_control.start_x+display_x+i,map_control.start_y+display_y)==TTN_OK)
						{
							if(map_layer_touch.character)
								alpha=NPC_MASK_ALPHA;				// °ëÍ¸Ã÷
							else
								continue;
						}
					}

					if((map_control.draw_point_color)&&(alpha>=MASK_ALPHA_LEVEL))
					{
						color=rgb2hi(temp_img256_head.palette[index]|0xFF,
							temp_img256_head.palette[index+1],temp_img256_head.palette[index+2]);
					}
					
					oper_alpha_color( &color, &bitmap->line[display_y][display_x+i],(SLONG) alpha);
				}
			}

			display_x=display_x+copy_length;
			decode_index=decode_index+(copy_length<<1);
			break;
		case IMG_LINE:
			display_y++;
			display_x=x-temp_img256_head.center_x;
			break;
		default:
			sprintf((char *)print_rec,MSG_IMG256_DECODE_ERROR,compress_type);
			display_error_message(print_rec,true);
			return;
		}
		if((decode_index>=data_size)||(display_y>=end_y))
			main_pass=1;
	}
	
	
}





SLONG dire_play_avi(UCHR *filename,SLONG cancel_flag)
{
	RECT rect;
	SLONG main_pass;
	SLONG ret;
	UCHR ch;
	
	
	if(check_file_exist(filename)!=TTN_OK)
	{
		sprintf((char *)print_rec,"File %s open error",filename);
		display_error_message(print_rec,true);
		return(TTN_NOT_OK);
	}


	ret=open_xmedia_file((USTR *)filename,&rect);
	clear_bitmap(screen_buffer);
	if(ret==TTN_OK)
	{
		main_pass=0;
		reset_key();
		while(main_pass==0)
		{
			system_idle_loop();
			ret=play_xmedia_frame(screen_buffer,0);
			rectangle_scale_put_bmp(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,screen_channel0,
				rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,screen_buffer);
			update_screen(screen_channel0);

			
			if(cancel_flag==1)
			{
				ch=read_system_key();
				switch(ch)
				{
				case Esc:
					reset_key();
					main_pass=1;
					break;
				}
			}
			if(cancel_flag==2)
			{
				ch=read_system_key();
				switch(ch)
				{
				case Esc:
					reset_key();
					main_pass=1;
					break;
				}
				ch=get_mouse_key();
				switch(ch)
				{
				case MS_RUp:				// Mouse Right Press
					reset_mouse_key();
					main_pass=1;
					break;
				}
			}
			
			if(ret != TTN_OK)
				main_pass=1;	
		}
	}
	close_xmedia_file();
	
	return(TTN_OK);
}














