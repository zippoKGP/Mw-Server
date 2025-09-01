/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : graph.h
  *owner  		: Stephen
  *description  : 
  *modified     : 2004/12/6
******************************************************************************/ 

#ifndef _GRAPH_H_
#define _GRAPH_H_

#include "xmedia.h"


#define IMG_COPY					0xFF    // Copy + Length + Data... ( Total Length + 3 Byte )
#define IMG_SKIP					0xFE    // Skip + Length  ( Total 3 Byte )
#define IMG_LINE					0xFD    // One Line End


/***************************************************************************************}
{	Fade Point																			}
****************************************************************************************/
typedef struct FADE_POINT_STRUCT
{
	SLONG start_flag;
	SLONG speed;
	SLONG point_alpha;
	SLONG x;
} FADE_POINT;




/***************************************************************************************}
{	Protype define																		}
****************************************************************************************/
void put_scroll_box(SLONG sx,SLONG sy,SLONG ex,SLONG ey,UHINT color,UHINT color1,BMP *bit_map);
void change_screen_effect0(void);
void change_screen_effect1(void);
void change_screen_effect2(void);
void change_screen_effect3(SLONG speed);
void auto_change_screen_effect(void);


void rectangle_scale_put_bmp(int des_x,int des_y,int des_w,int des_h,BMP *des,
							 int src_x,int src_y,int src_w,int src_h,const BMP *src);
		
void scale_put_dire_bmp(int win_x,int win_y,int win_w,int win_h,
						int src_w,int src_h,const unsigned short *src,
						int des_w,int des_h,unsigned short *des);
						

void display_hightlight_zoom_img256_buffer(SLONG x,SLONG y,UCHR *data_buffer,BMP *bitmap,SLONG zoom_percent);
void rgb_hightlight_color(UCHR fr,UCHR fg,UCHR fb,UHINT *bk_color);
void oper_hightlight_color(UHINT *color,UHINT *bk_color);
void display_hightlight_img256_buffer(SLONG x,SLONG y,UCHR *data_buffer,BMP *bitmap);
void display_zoom_img256_buffer_alpha(SLONG x,SLONG y,UCHR *data_buffer,BMP *bitmap,SLONG zoom_percent,SLONG alpha_base);
void display_img256_buffer_alpha(SLONG x,SLONG y,UCHR *data_buffer,BMP *bitmap,SLONG alpha_base);
void display_zoom_img256_buffer(SLONG x,SLONG y,UCHR *data_buffer,BMP *bitmap,SLONG zoom_percent);
void display_img256_buffer(SLONG x,SLONG y,UCHR *data_buffer,BMP *bitmap);
void display_part_img256_buffer(SLONG x,SLONG y,SLONG cut_xl,UCHR *data_buffer,BMP *bitmap);
SLONG dire_play_avi(UCHR *filename,SLONG cancel_flag);
void change_screen_effect_flc(SLONG idx);




#endif





















