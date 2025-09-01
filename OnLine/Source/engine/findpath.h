/*
 *
 *
 */


#ifndef __FINDPATH_H__
#define __FINDPATH_H__


//#define BARMASK_MAP_BAR				BITBAR
#define BARMASK_MAP_BAR				0x80

#define FIND_OK_REACH				0  // 已到達目的地
#define FIND_OK_CONTINUE			1  // 找到目的地,但還未到達
#define FIND_OK_TOWARDS				2  // 未找到目的地,只向目的地方向移動
#define FIND_FAIL_NO_PATH			-1 // 沒有阻擋層,起始點或終點不在阻擋層內,起始點是阻擋格
#define FIND_FAIL_ERROR				-2 // 失敗


#define FIND_PATH_MAX_RANGE			400


typedef struct MAP_BAR_LAYER_STRUCT
{
	int           width;             
	int           height;            
	int           cell_width;        
	int           cell_height;       
	unsigned char *state;            
} MAP_BAR_LAYER;




extern int  create_bar_layer(MAP_BAR_LAYER *bar_layer,int width,int height,int cell_width,int cell_height,unsigned short *map_flag);
extern void free_bar_layer(MAP_BAR_LAYER *bar_layer);
int find_path_a(int sx,int sy,int &ex,int &ey,int &line_ex,int &line_ey,int len,const MAP_BAR_LAYER *bar_layer);
int find_path_b(int sx,int sy,int &ex,int &ey,int &line_ex,int &line_ey,int len,const MAP_BAR_LAYER *bar_layer);
void destroy_bar_layer(MAP_BAR_LAYER *bar_layer);
void adjust_pos(int &x,int &y,const MAP_BAR_LAYER *bar_layer);


#endif /*__FINDPATH_H__*/
