/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : findpath.cpp
  *owner  		: Stephen
  *description  : 
  *modified     : 2005/1/4
******************************************************************************/ 

#include "engine_global.h"
#include "vari-ext.h"
#include "findpath.h"
#include "map.h"


//******************************************************************************}
//  bar_layer bit 7,8 用于 mask 寻找											}
// ******************************************************************************


#define WALK_FLAG_MAKE  0xc0  // 11xxxxxx
#define WALK_BAR_FLAG   0x80  // 10xxxxxx 阻挡
#define WALK_WALK_FLAG  0xc0  // 11xxxxxx 已走
#define WALK_NULL_FLAG  0x00  // 00xxxxxx 可走
#define WALK_READY_FLAG 0x40  // 01xxxxxx 待走


#define UP           0
#define DOWN         1
#define LEFT         2
#define RIGHT        3
#define LEFT_UP      4
#define LEFT_DOWN    5
#define RIGHT_UP     6
#define RIGHT_DOWN   7




typedef struct
{
	int  x;
	int  y;
	int  way;
	int  father;
} PAHT_STACK;

#define UP           0
#define DOWN         1
#define LEFT         2
#define RIGHT        3
#define LEFT_UP      4
#define LEFT_DOWN    5
#define RIGHT_UP     6
#define RIGHT_DOWN   7

typedef struct
{
	struct
	{
		int           x;
		int           y;
		unsigned char flag;
	}way[8];
} ZONE;

PAHT_STACK         path_buffer[FIND_PATH_MAX_RANGE];
ZONE               zone;
int                meet;



static void init_map_bar_layer(MAP_BAR_LAYER *bar_layer)
{
	register int x,y;
	int bar_w,bar_h;
	unsigned char  *l1,*l2;

	bar_w=bar_layer->width;
	bar_h=bar_layer->height;
	l1=&bar_layer->state[0];
	l2=&bar_layer->state[(bar_h-1)*bar_w];
	for(x=0;x<bar_w;x++)
	{
		*l1=(*l1|WALK_FLAG_MAKE);
		*l2=(*l2|WALK_FLAG_MAKE);
		l1++;
		l2++;
	}
	for(y=1;y<bar_h-1;y++)
	{
		l1=&bar_layer->state[y*bar_w];
		*l1=(*l1|WALK_FLAG_MAKE);
		l1++;
		for(x=1;x<bar_w-1;x++)
		{
			//---------------------
			// clear 原来的阻挡
			*l1=(*l1&~0x40);
			l1++;
		}
		*l1=(*l1|WALK_FLAG_MAKE);
	}
}


int create_bar_layer(MAP_BAR_LAYER *bar_layer,int width,int height,int cell_width,int cell_height,unsigned short *map_flag)
{
	int  i,j;
	unsigned char *p;

	bar_layer->width=width;
	bar_layer->height=height;
	bar_layer->cell_width=cell_width;
	bar_layer->cell_height=cell_height;
	i=width*height;
	p=(unsigned char*)malloc(i);
	bar_layer->state=p;
	memset(p,0,i);
	for(j=0;j<i;j++)
	{
		if(BITBAR&map_flag[j])
			p[j]=WALK_BAR_FLAG;
	}
	init_map_bar_layer(bar_layer);
	return 0;
}


void destroy_bar_layer(MAP_BAR_LAYER *bar_layer)
{
	if( NULL != bar_layer->state )
	{
		free( bar_layer->state );
		bar_layer->state   = NULL;
	}
	bar_layer->width       = 0;
	bar_layer->height      = 0;
	bar_layer->cell_width  = 0;
	bar_layer->cell_height = 0;
}


static int optimize_path( int sx, int sy, int ex, int ey, const MAP_BAR_LAYER *bar_layer )
{
	double   d0,d1;
	int      bar_w,bar_h;
	int      temp;
	register int dx, dy;

	bar_w = bar_layer->width;
	bar_h = bar_layer->height;
	dx = abs(ex-sx);
	dy = abs(ey-sy);
	if( dx>dy )
	{
		if( sx>ex )
		{
			temp = sx, sx = ex, ex = temp;
			temp = sy, sy = ey, ey = temp;
		}
		d0   = (double)dy/(double)dx;
		d1   = 0.5f;
		temp = 0;
		if( sy<ey )
		{
			if( sx<0||sy<0 ) goto not_ok;
			for( ; sx<=ex&&sx<bar_w&&sy<=ey&&sy<bar_h; sx++ )
			{
				if( BARMASK_MAP_BAR&bar_layer->state[sy*bar_w+sx] )
					goto not_ok;
				d1 += d0;
				if( (int)d1>temp )
					temp++, sy++;
			}
		}
		else
		{
			if( sx<0||sy>=bar_h ) goto not_ok;
			for( ; sx<=ex&&sx<bar_w&&sy>=ey&&sy>=0; sx++ )
			{
				if( BARMASK_MAP_BAR&bar_layer->state[sy*bar_w+sx] )
					goto not_ok;
				d1 += d0;
				if( (int)d1>temp )
					temp++, sy--;
			}
		}
	}
	else
	{
		if( sy > ey )
		{
			temp = sx, sx = ex, ex = temp;
			temp = sy, sy = ey, ey = temp;
		}
		d0   = (double)dx/(double)dy;
		d1   = 0.5f;
		temp = 0;
		if( sx<ex )
		{
			if( sy<0||sx<0 ) goto not_ok;
			for( ; sy<=ey&&sy<bar_h&&sx<=ex&&sx<bar_w; sy++ )
			{
				if( BARMASK_MAP_BAR&bar_layer->state[sy*bar_w+sx] )
					goto not_ok;
				d1 += d0;
				if( (int)d1>temp )
					temp++, sx++;
			}
		}
		else
		{
			if( sy<0||sx>=bar_w ) goto not_ok;
			for( ; sy<=ey&&sy<bar_h&&sx>=ex&&sx>=0; sy++ )
			{
				if( BARMASK_MAP_BAR&bar_layer->state[sy*bar_w+sx] )
					goto not_ok;
				d1 += d0;
				if( (int)d1>temp )
					temp++, sx--;
			}
		}
	}	
	return 0;

not_ok:
	return -1;
}




#define push_path_buffer_a(tbx,tby,tway) \
{\
	if(way==tway)\
	{\
		path_buffer[pi].x=tbx;\
		path_buffer[pi].y=tby;\
	}\
	else\
	{\
		pi++;\
		path_buffer[pi].x=tbx;\
		path_buffer[pi].y=tby;\
		way=tway;\
	}\
}

int find_path_a(int sx,int sy,int &ex,int &ey,int &line_ex,int &line_ey,int len,const MAP_BAR_LAYER *bar_layer)
{
	int  pi,lx;
	int  bar_w,bar_h,cell_w,cell_h,uw,uh;
	int  way,i,r;
	int  bsx,bsy,bex,bey,tbx,tby,nx,ny;
	double dx,dy,dl;

	if(NULL==bar_layer) return FIND_FAIL_ERROR;
	bar_w=bar_layer->width;
	bar_h=bar_layer->height;
	cell_w=bar_layer->cell_width;
	cell_h=bar_layer->cell_height;
	uw=bar_w-2;
	uh=bar_h-2;
	// 把地圖位置轉成阻擋層位置
	bsx=sx/cell_w;
	bsy=sy/cell_h;
	bex=ex/cell_w;
	bey=ey/cell_h;
	if(	bsx<1||bsx>uw||bsy<1||bsy>uh|| // 起點是否有越界		
		(BARMASK_MAP_BAR&bar_layer->state[bsy*bar_w+bsx])) // 檢查起始點是否是阻擋		
		return FIND_FAIL_ERROR;
	// 調整bex,bey
	if(bex<=0) bex=1; else if(bex>uw) bex=uw;
	if(bey<=0) bey=1; else if(bey>uh) bey=uh;
	// 起點和終點是否在一起
	if(bsx==bex&&bsy==bey) return FIND_OK_REACH;
	// 如果起始點和總點是一條直線,就...
	if(0==optimize_path(bsx,bsy,bex,bey,bar_layer))
	{
		// 糾正 bex, bey 到格子中間
		nx=bex*cell_w+cell_w/2;
		ny=bey*cell_h+cell_h/2;
		r=FIND_OK_CONTINUE;
		goto next_position;
	}
	// 初始反回值
	r=FIND_OK_TOWARDS;
	path_buffer[0].x=tbx=bsx;
	path_buffer[0].y=tby=bsy;
	pi=0;
	way=-1;
	while(	tbx>=1&&tbx<=uw&&
			tby>=1&&tby<=uh&&
			pi<FIND_PATH_MAX_RANGE-1)
	{
		if(tby>bey)
		{
			lx=(tby-1)*bar_w;
			if(tbx>bex&&!(BARMASK_MAP_BAR&bar_layer->state[lx+tbx-1]))
			{
				// 左上				
				tbx--,tby--;
				push_path_buffer_a(tbx,tby,LEFT_UP);
			}
			else if(tbx<bex&&!(BARMASK_MAP_BAR&bar_layer->state[lx+tbx+1]))
			{
				// 右上
				tbx++,tby--;
				push_path_buffer_a(tbx,tby,RIGHT_UP);
			}
			else if(!(BARMASK_MAP_BAR&bar_layer->state[lx+tbx]))
			{
				// 上				
				tby--;
				push_path_buffer_a(tbx,tby,UP);
			}
			else if(tbx>bex&&!(BARMASK_MAP_BAR&bar_layer->state[tby*bar_w+tbx-1]))
			{
				// 左
				tbx--;
				push_path_buffer_a(tbx,tby,LEFT);
			}
			else if(tbx<bex&&!(BARMASK_MAP_BAR&bar_layer->state[tby*bar_w+tbx+1]))
			{
				// 右
				tbx++;
				push_path_buffer_a(tbx,tby,RIGHT);
			}
			else
			{
				// 遇到阻擋格
				break;
			}
		}
		else if(tby<bey)
		{
			lx=(tby+1)*bar_w;
			if(tbx>bex&&!(BARMASK_MAP_BAR&bar_layer->state[lx+tbx-1]))
			{
				// 左下
				tbx--,tby++;
				push_path_buffer_a(tbx,tby,LEFT_DOWN);
			}
			else if(tbx<bex&&!(BARMASK_MAP_BAR&bar_layer->state[lx+tbx+1]))
			{
				// 右下
				tbx++,tby++;
				push_path_buffer_a(tbx,tby,RIGHT_DOWN);
			}
			else if(!(BARMASK_MAP_BAR&bar_layer->state[lx+tbx]))
			{
				// 下
				tby++;
				push_path_buffer_a(tbx,tby,DOWN);
			}
			else if(tbx>bex&&!(BARMASK_MAP_BAR&bar_layer->state[tby*bar_w+tbx-1]))
			{
				// 左
				tbx--;
				push_path_buffer_a(tbx,tby,LEFT);
			}
			else if(tbx<bex&&!(BARMASK_MAP_BAR&bar_layer->state[tby*bar_w+tbx+1]))
			{
				// 右
				tbx++;
				push_path_buffer_a(tbx,tby,RIGHT);
			}
			else
			{
				// 遇到阻擋格
				break;
			}
		}
		else if(tbx>bex&&!(BARMASK_MAP_BAR&bar_layer->state[tby*bar_w+tbx-1]))
		{
			// 左
			tbx--;
			push_path_buffer_a(tbx,tby,LEFT);
		}
		else if(tbx<bex&&!(BARMASK_MAP_BAR&bar_layer->state[tby*bar_w+tbx+1]))
		{
			// 右
			tbx++;
			push_path_buffer_a(tbx,tby,RIGHT);
		}
		else
		{
			break;
		}
		if(tbx==bex&&tby==bey) r=FIND_OK_CONTINUE;
	}
// 不能找到目標點,
// 這種情況出現在npc面前就是阻擋格,
// 但是為了不讓npc處在行走狀態中,
// 所以反回 FIND_OK_REACH

	if(pi==0) return FIND_OK_REACH;
// 求出從起始點出發的第一條直線	
	tbx=path_buffer[1].x;
	tby=path_buffer[1].y;
	for(i=2;i<=pi;i++)
	{
		if(0!=optimize_path(bsx,bsy,path_buffer[i].x,path_buffer[i].y,bar_layer))
			break;
		tbx=path_buffer[i].x;
		tby=path_buffer[i].y;
	}
	// 糾正 x, y 到格子中間
	nx=tbx*cell_w+cell_w/2;
	ny=tby*cell_h+cell_h/2;

// 計算下一步的位置
next_position:
	line_ex=nx;
	line_ey=ny;

	dx=abs(nx-sx);
	dy=abs(ny-sy);
	dl=sqrt(dx*dx+dy*dy);

	// 3D行走, 如果不需要將它刪除即可
	// 調整步長

//	stephen
//	double d_t;
//	d_t=asin(dx/dl)/(3.415926/2);
//	len=(int)((d_t*d_t+1)*((double)len/2));
//	if(len<2) len=2;

	
	if(dl>len)
	{
		i = -1;
		while(++i<len)
		{
			dx=dl/(double)(len+i);
			if(nx>sx)
				ex=sx+(int)((double)((nx-sx))/(double)dx+1.0f);
			else if(nx<sx)
				ex=sx-(int)((double)((sx-nx))/(double)dx+1.0f);
			else
				ex=nx;

			if(ny>sy)
				ey=sy+(int)((double)((ny-sy))/(double)dx+1.0f);
			else if(ny<sy)
				ey=sy-(int)((double)((sy-ny))/(double)dx+1.0f);
			else
				ey=ny;

			if(!(BARMASK_MAP_BAR&bar_layer->state[ey/cell_h*bar_w+ex/cell_w]))
				break;
		}
		// 防止異常
		if(i>=len)
		{
			if(len<cell_h)
			{
				if(sy<ny&&!(BARMASK_MAP_BAR&bar_layer->state[(sy+len)/cell_h*bar_w+sx/cell_w]))
					ex=sx,ey=sy+len;
				else if( sy>ny&&!(BARMASK_MAP_BAR&bar_layer->state[(sy-len)/cell_h*bar_w+sx/cell_w]))
					ex=sx,ey=sy-len;
				else if( sx<nx&&!(BARMASK_MAP_BAR&bar_layer->state[sy/cell_h*bar_w+(sx+len)/cell_w]))
					ex=sx+len,ey=sy;
				else if( sx>nx&&!(BARMASK_MAP_BAR&bar_layer->state[sy/cell_h*bar_w+(sx-len)/cell_w]))
					ex=sx-len,ey=sy;
				else
					return FIND_FAIL_NO_PATH;
			}
			else
			{
				if(sy<ny&&!(BARMASK_MAP_BAR&bar_layer->state[(sy+(cell_h-1))/cell_h*bar_w+sx/cell_w]))
					ex=sx, ey = sy+(cell_h-1);
				else if(sy>ny&&!(BARMASK_MAP_BAR&bar_layer->state[(sy-(cell_h-1))/cell_h*bar_w+sx/cell_w]))
					ex=sx, ey = sy-(cell_h-1);
				else if(sx<nx&&!(BARMASK_MAP_BAR&bar_layer->state[sy/cell_h*bar_w+(sx+(cell_h-1))/cell_w]))
					ex=sx+(cell_h-1),ey=sy;
				else if(sx>nx&&!(BARMASK_MAP_BAR&bar_layer->state[sy/cell_h*bar_w+(sx-(cell_h-1))/cell_w]))
					ex=sx-(cell_h-1),ey=sy;
				else
					return FIND_FAIL_NO_PATH;
			}
		}
	}
	else
	{
		ex = nx;
		ey = ny;
	}
	if(BARMASK_MAP_BAR&bar_layer->state[bey*bar_w+bex]) // 檢查終點是否是阻擋
		return FIND_OK_CONTINUE;
	return r;
}












#define init_walk(ix,iy,way,meet) \
{\
	f=0;\
	for(ti=0;ti<8;ti++)\
	{\
		if(	ix==zone.way[ti].x&&\
			iy==zone.way[ti].y)\
		{\
			switch(zone.way[ti].flag)\
			{\
			case WALK_NULL_FLAG:\
				zone.way[ti].flag=WALK_READY_FLAG;\
				break;\
			case WALK_WALK_FLAG:\
				if(way==ti)\
				{\
					f=1,meet=1;\
					break;\
				}\
			}\
			if(f) break;\
		}\
	}\
}

#define push_path_buffer_b(p,px,py,w) \
{\
	if((bar_layer->state[p]&WALK_FLAG_MAKE)==WALK_NULL_FLAG)\
	{\
		ei++;\
		path_buffer[ei].x=px;\
		path_buffer[ei].y=py;\
		path_buffer[ei].father=si;\
		path_buffer[ei].way=w;\
		if(	px==bex&&py==bey) goto _ok;\
		if(ei>=FIND_PATH_MAX_RANGE-1) goto _not_ok;\
		bar_layer->state[p]|=WALK_WALK_FLAG;\
	}\
}

int find_path_b(int sx,int sy,int &ex,int &ey,int &line_ex,int &line_ey,int len,const MAP_BAR_LAYER *bar_layer)
{
	int meet;
	int si,ei;
	int x,y;
	int bar_w,bar_h,bcw,bch;
	int bsx,bsy,bex,bey;
	int ai12x,ai12y,ai12r;
	int i,flag;
	int lx,mx,rx;
	int ty,my,by;
	int way;
	int tl,ml,bl;

	// 先用find_path_a找路徑
	ai12x=ex,ai12y=ey;
	ai12r=find_path_a(sx,sy,ai12x,ai12y,line_ex,line_ey,len,bar_layer);
	if(ai12r==FIND_FAIL_ERROR)return ai12r;
	if(	ai12r==FIND_OK_CONTINUE)
	{
		ex=ai12x,ey=ai12y;
		return FIND_OK_CONTINUE;
	}


	//在 find_path_a 已檢查過
	//if(NULL==bar_layer) return FIND_FAIL_ERROR;
	//阻擋層的數據
	bar_w=bar_layer->width;
	bar_h=bar_layer->height;
	bcw=bar_layer->cell_width;
	bch=bar_layer->cell_height;
	// 把地圖位置轉成阻擋層位置
	bsx=sx/bcw;
	bsy=sy/bch;
	bex=ex/bcw;
	bey=ey/bch;
	// 在 find_path_a 已經有檢擦,這些語句已是死句
	//if(	bsx<=0||bsx>=bar_w-1||bsy<=0||bsy>=bar_h-1|| // 起點是否有越界
	//	(BARMASK_MAP_BAR&bar_layer->state[bsy*bar_w+bsx])) // 檢查起始點是否是阻擋
	//	return FIND_FAIL_ERROR;
	// 調整bex,bey
	
	if(bex<1) bex=1; else if(bex>bar_w-2) bex=bar_w-2;
	if(bey<1) bey=1; else if(bey>bar_h-2) bey=bar_h-2;
	// 起點和終點是否在一起
	if(bsx==bex&&bsy==bey) return FIND_OK_REACH;
	// 檢查終點是否是阻擋
	if(BARMASK_MAP_BAR&bar_layer->state[bey*bar_w+bex])
	{
		ex=ai12x,ey=ai12y;
		return ai12r;
	}
	// 出始化 path_buffer 指針
	si=0,ei=0;
	// 把阻擋層的起始位置壓入 path_buffer
	path_buffer[0].x=bsx;
	path_buffer[0].y=bsy;
	path_buffer[0].way=-1;
	path_buffer[0].father=-1;
	// 開始尋找
	while(si<=ei&&ei<FIND_PATH_MAX_RANGE-1)
	{
		// 取出出發點		
		// 把出發點周圍八個點存入zone
		mx=path_buffer[si].x;
		my=path_buffer[si].y;
		way=path_buffer[si].way;
		lx=mx-1;
		rx=mx+1;
		ty=my-1;
		by=my+1;
		ml=my*bar_w;
		tl=ml-bar_w;
		bl=ml+bar_w;
		zone.way[LEFT].x=lx;
		zone.way[LEFT].y=my;
		zone.way[LEFT].flag=bar_layer->state[ml+lx]&WALK_FLAG_MAKE;
		zone.way[RIGHT].x=rx;
		zone.way[RIGHT].y=my;
		zone.way[RIGHT].flag=bar_layer->state[ml+rx]&WALK_FLAG_MAKE;
		zone.way[UP].x=mx;
		zone.way[UP].y=ty;
		zone.way[UP].flag=bar_layer->state[tl+mx]&WALK_FLAG_MAKE;
		zone.way[LEFT_UP].x=lx;
		zone.way[LEFT_UP].y=ty;
		zone.way[LEFT_UP].flag=bar_layer->state[tl+lx]&WALK_FLAG_MAKE;
		zone.way[RIGHT_UP].x=rx;
		zone.way[RIGHT_UP].y=ty;
		zone.way[RIGHT_UP].flag=bar_layer->state[tl+rx]&WALK_FLAG_MAKE;
		zone.way[DOWN].x=mx;
		zone.way[DOWN].y=by;
		zone.way[DOWN].flag=bar_layer->state[bl+mx]&WALK_FLAG_MAKE;
		zone.way[LEFT_DOWN].x=lx;
		zone.way[LEFT_DOWN].y=by;
		zone.way[LEFT_DOWN].flag=bar_layer->state[bl+lx]&WALK_FLAG_MAKE;
		zone.way[RIGHT_DOWN].x=rx;
		zone.way[RIGHT_DOWN].y=by;
		zone.way[RIGHT_DOWN].flag=bar_layer->state[bl+rx]&WALK_FLAG_MAKE;
		meet=0,
		flag=0;
		for(i=0;i<8;i++)
		{
			if(zone.way[i].flag==WALK_BAR_FLAG)
			{
				int  ti,f;

				flag=1;
				mx=zone.way[i].x;
				my=zone.way[i].y;
				lx=mx-1;
				rx=mx+1;
				ty=my-1;
				by=my+1;
				init_walk(mx,ty,way,meet);
				if(meet) break;
				init_walk(mx,by,way,meet);
				if(meet) break;
				init_walk(lx,my,way,meet);
				if(meet) break;
				init_walk(rx,my,way,meet);
				if(meet) break;
				init_walk(lx,ty,way,meet);
				if(meet) break;
				init_walk(lx,by,way,meet);
				if(meet) break;
				init_walk(rx,ty,way,meet);
				if(meet) break;
				init_walk(rx,by,way,meet);
				if(meet) break;
			}
		}
		if(flag)
		{
			if(meet==0)
			{
				for(i=0;i<8;i++)
				{
					if(zone.way[i].flag==WALK_READY_FLAG)
					{
						ei++;
						path_buffer[ei].x=zone.way[i].x;
						path_buffer[ei].y=zone.way[i].y;
						path_buffer[ei].father=si;
						path_buffer[ei].way=i;
						bar_layer->state[zone.way[i].y*bar_w+zone.way[i].x]|=WALK_WALK_FLAG;
						// 是否到達終點
						if(zone.way[i].x==bex&&zone.way[i].y==bey) goto _ok;
						if(ei>=FIND_PATH_MAX_RANGE-1) goto _not_ok;
					}
				}
			}
			// 向原來的方向移動一格
			i=path_buffer[si].way;
			if(si>0&&zone.way[i].flag==WALK_NULL_FLAG)
			{
				ei++;
				x=zone.way[i].x;
				y=zone.way[i].y;
				path_buffer[ei].x=x;
				path_buffer[ei].y=y;
				path_buffer[ei].father=si;
				path_buffer[ei].way=path_buffer[si].way;
				bar_layer->state[y*bar_w+x]|=WALK_WALK_FLAG;
				if(x==bex&&y==bey) goto _ok;
				if(ei>=FIND_PATH_MAX_RANGE-1) goto _not_ok;
			}
		}
		else
		{
		// 向終點移動
			if(lx>=bex)
			{
				// 左上
				if(	ty>=bey&&
					(	(bar_layer->state[tl+lx]&WALK_FLAG_MAKE)==WALK_NULL_FLAG||
						(bar_layer->state[tl+lx]&WALK_FLAG_MAKE)==WALK_WALK_FLAG))
				{
					push_path_buffer_b(tl+lx,lx,ty,LEFT_UP);
				}
				// 左下
				else if(by<=bey&&
						(	(bar_layer->state[bl+lx]&WALK_FLAG_MAKE)==WALK_NULL_FLAG||
							(bar_layer->state[bl+lx]&WALK_FLAG_MAKE)==WALK_WALK_FLAG))
				{
					push_path_buffer_b(bl+lx,lx,by,LEFT_DOWN);
				}
				// 左				
				else if(	(bar_layer->state[ml+lx]&WALK_FLAG_MAKE)==WALK_NULL_FLAG||
							(bar_layer->state[ml+lx]&WALK_FLAG_MAKE)==WALK_WALK_FLAG)
				{
					push_path_buffer_b(ml+lx,lx,my,LEFT);
				}
			}
			else if(rx<=bex)
			{
				// 右上
				if(	ty>=bey&&
					(	(bar_layer->state[tl+rx]&WALK_FLAG_MAKE)==WALK_NULL_FLAG||
						(bar_layer->state[tl+rx]&WALK_FLAG_MAKE)==WALK_WALK_FLAG))
				{
					push_path_buffer_b(tl+rx,rx,ty,RIGHT_UP);
				}
				// 右下
				else if(by<=bey&&
						(	(bar_layer->state[bl+rx]&WALK_FLAG_MAKE)==WALK_NULL_FLAG||
							(bar_layer->state[bl+rx]&WALK_FLAG_MAKE)==WALK_WALK_FLAG))
				{
					push_path_buffer_b(bl+rx,rx,by,RIGHT_DOWN);
				}
				// 右
				else if(	(bar_layer->state[ml+rx]&WALK_FLAG_MAKE)==WALK_NULL_FLAG||
							(bar_layer->state[ml+rx]&WALK_FLAG_MAKE)==WALK_WALK_FLAG)
				{
					push_path_buffer_b(ml+rx,rx,my,RIGHT);
				}
			}
			// 上
			else if(ty>=bey&&
					(	(bar_layer->state[tl+mx]&WALK_FLAG_MAKE)==WALK_NULL_FLAG||
						(bar_layer->state[tl+mx]&WALK_FLAG_MAKE)==WALK_WALK_FLAG))
			{
				push_path_buffer_b(tl+mx,mx,ty,UP);
			}
			// 下
			else if(by<=bey&&
					(	(bar_layer->state[bl+mx]&WALK_FLAG_MAKE)==WALK_NULL_FLAG||
						(bar_layer->state[bl+mx]&WALK_FLAG_MAKE)==WALK_WALK_FLAG))
			{
				push_path_buffer_b(bl+mx,mx,by,DOWN);
			}
		}
		si++;
	}
_not_ok:
	// 恢復在尋找過程中修改過的阻擋信息
	for(si=0;si<=ei;si++)
		bar_layer->state[path_buffer[si].y*bar_w+path_buffer[si].x]&=~WALK_FLAG_MAKE;
	// 反回ai12的結果
	ex=ai12x,ey=ai12y;
	return ai12r;
_ok:
	// 恢復在尋找過程中修改過的阻擋信息	
	for(si=0;si<=ei;si++)
		bar_layer->state[path_buffer[si].y*bar_w+path_buffer[si].x]&=~WALK_FLAG_MAKE;
	// 找到終點,往回找來的路
	way=path_buffer[ei].way;
	i=path_buffer[ei].father;
	si=ei;
	while(i>=0)
	{
		if(way!=path_buffer[i].way)
		{
			si--;
			path_buffer[si].x=path_buffer[i].x;
			path_buffer[si].y=path_buffer[i].y;
			way=path_buffer[i].way;
		}
		i=path_buffer[i].father;
	}
	// 求出從起點出發的第一條直線
	bsx=path_buffer[si].x;
	bsy=path_buffer[si].y;
	i=si+1;
	bex=path_buffer[i].x;
	bey=path_buffer[i].y;
	while(++i<=ei)
	{
		if(0!=optimize_path(bsx,bsy,path_buffer[i].x, path_buffer[i].y,bar_layer))
			break;
		bex=path_buffer[i].x;
		bey=path_buffer[i].y;
	}
	// 糾正 x, y 到格子中間
	x=bex*bcw+bcw/2;
	y=bey*bch+bch/2;

	line_ex=x;
	line_ey=y;

	// 計算下一步的位置
	double dx,dy,dl;
	dx=abs(x-sx);
	dy=abs(y-sy);
	dl=sqrt(dx*dx+dy*dy);

	// 3D行走, 如果不需要將它刪除即可
	// 調整步長

//
//	double d_t;
//	d_t=asin((double)dx/(double)dl)/(3.415926/2);
//	len=(int)((d_t*d_t+1)*((double)len/2));
//	if(len<2) len=2;
//
	
	if(dl>len)
	{
		i=-1;
		while(++i<len)
		{
			dx=dl/(double)(len+i);
			if(x>sx)
				ex=sx+(int)((double)((x-sx))/(double)dx+1.0f);
			else if(x<sx)
				ex=sx-(int)((double)((sx-x))/(double)dx+1.0f);
			else
				ex=x;
			if( y>sy)
				ey=sy+(int)((double)((y-sy))/(double)dx+1.0f);
			else if(y<sy)
				ey=sy-(int)((double)((sy-y))/(double)dx+1.0f);
			else
				ey=y;
			if(!(BARMASK_MAP_BAR&bar_layer->state[ey/bch*bar_w+ex/bcw]))
				break;
		}
		// 防止異常
		if(i>=len)
		{
			if(len<bch)
			{
				if(sy<y&&!(BARMASK_MAP_BAR&bar_layer->state[(sy+len)/bch*bar_w+sx/bcw]))
					ex=sx,ey=sy+len;
				else if(sy>y&&!(BARMASK_MAP_BAR&bar_layer->state[(sy-len)/bch*bar_w+sx/bcw]))
					ex=sx,ey=sy-len;
				else if(sx<x&&!(BARMASK_MAP_BAR&bar_layer->state[sy/bch*bar_w+(sx+len)/bcw]))
					ex=sx+len,ey=sy;
				else if(sx>x&&!(BARMASK_MAP_BAR&bar_layer->state[sy/bch*bar_w+(sx-len)/bcw]))
					ex=sx-len,ey=sy;
				else
					return FIND_FAIL_NO_PATH;
			}
			else
			{
				if(sy<y&&!(BARMASK_MAP_BAR&bar_layer->state[(sy+(bch-1))/bch*bar_w+sx/bcw]))
					ex=sx,ey=sy+(bch-1);
				else if(sy>y&&!(BARMASK_MAP_BAR&bar_layer->state[(sy-(bch-1))/bch*bar_w+sx/bcw]))
					ex=sx,ey=sy-(bch-1);
				else if( sx<x&&!(BARMASK_MAP_BAR&bar_layer->state[sy/bch*bar_w+(sx+(bch-1))/bcw]))
					ex=sx+(bch-1),ey=sy;
				else if( sx>x&&!(BARMASK_MAP_BAR&bar_layer->state[sy/bch*bar_w+(sx-(bch-1))/bcw]))
					ex=sx-(bch-1),ey=sy;
				else
					return FIND_FAIL_NO_PATH;
			}
		}
	}
	else
	{
		ex=x;
		ey=y;
	}
	return FIND_OK_CONTINUE;
}



int is_bar( int mx, int my, const MAP_BAR_LAYER *bar_layer )
{
	register int bx, by;
	int          temp;

	bx = mx / bar_layer->cell_width;
	by = my / bar_layer->cell_height;

	temp = 0;
	if( bx < 0 )
	{
		bx = 0;
		temp = 1;
	}
	else if( bx >= bar_layer->width )
	{
		bx = bar_layer->width - 1;
		temp = 1;
	}

	if( by < 0 )
	{
		by = 0;
		temp = 1;
	}
	else if( by >= bar_layer->height )
	{
		by = bar_layer->height - 1;
		temp = 1;
	}

	if( temp == 0 && ( BARMASK_MAP_BAR & bar_layer->state[ by * bar_layer->width + bx ] ) )
		goto ok;

	return FALSE;

ok:
	return TRUE;
}



int set_bar( int mx, int my, const MAP_BAR_LAYER *bar_layer )
{
	register int bx, by;
	int          temp;

	bx = mx / bar_layer->cell_width;
	by = my / bar_layer->cell_height;

	temp = 0;
	if( bx < 0 )
	{
		bx = 0;
		temp = 1;
	}
	else if( bx >= bar_layer->width )
	{
		bx = bar_layer->width - 1;
		temp = 1;
	}

	if( by < 0 )
	{
		by = 0;
		temp = 1;
	}
	else if( by >= bar_layer->height )
	{
		by = bar_layer->height - 1;
		temp = 1;
	}

	if( temp == 0 )
		bar_layer->state[ by * bar_layer->width + bx ] |= WALK_BAR_FLAG;
		goto ok;

	return FALSE;

ok:
	return TRUE;
}



int clear_bar( int mx, int my, const MAP_BAR_LAYER *bar_layer )
{
	register int bx, by;
	int          temp;

	bx = mx / bar_layer->cell_width;
	by = my / bar_layer->cell_height;

	temp = 0;
	if( bx < 0 )
	{
		bx = 0;
		temp = 1;
	}
	else if( bx >= bar_layer->width )
	{
		bx = bar_layer->width - 1;
		temp = 1;
	}

	if( by < 0 )
	{
		by = 0;
		temp = 1;
	}
	else if( by >= bar_layer->height )
	{
		by = bar_layer->height - 1;
		temp = 1;
	}

	if( temp == 0 )
		bar_layer->state[ by * bar_layer->width + bx ] &= (~WALK_BAR_FLAG);
		goto ok;

	return FALSE;

ok:
	return TRUE;
}




void adjust_pos(int &x,int &y,const MAP_BAR_LAYER *bar_layer)
{
	int   px,py;
	int   si,ei;
	int   tx,ty;
	int   width,height;
	int   t_width,t_height;
	int   cell_width,cell_height;
	UCHR  *state;


	width=bar_layer->width;
	height=bar_layer->height;
	cell_width=bar_layer->cell_width;
	cell_height=bar_layer->cell_height;
	state=bar_layer->state;

	px=x/cell_width;
	py=y/cell_height;

	if(px<=0) px=1;
	else if(px>=width-1) px=width-2;
	if(py<=0) py=1;
	else if(py>=height-1) py=height-2;
	if(	width>2&&
		height>2)
	{
		if(!(BARMASK_MAP_BAR&state[py*width+px]) )
		{
			x=px*cell_width+cell_width/2;
			y=py*cell_height+cell_height/2;
			return;
		}

		t_width=width-1;
		t_height=height-1;
		si=0;
		ei=1;
		path_buffer[0].x=px;
		path_buffer[0].y=py;
		state[py*width+px]|=WALK_READY_FLAG;
		while(ei<=FIND_PATH_MAX_RANGE)
		{
			px=path_buffer[si].x;
			py=path_buffer[si].y;

			//right
			tx=px+1;
			if(tx<t_width)
			{
				ty=py;
				if(!(WALK_READY_FLAG&state[ty*width+tx]))
				{
					if(!(BARMASK_MAP_BAR&state[ty*width+tx]))  goto _ok;
					if(ei>=FIND_PATH_MAX_RANGE) break;
					path_buffer[ei].x=tx;
					path_buffer[ei].y=ty;
					ei++;
					state[ty*width+tx]|=WALK_READY_FLAG;
				}
			}

			//up
			ty=py-1;
			if(ty>0)
			{
				tx=px;
				if(!(WALK_READY_FLAG&state[ty*width+tx]))
				{
					if(!(BARMASK_MAP_BAR&state[ty*width+tx]))  goto _ok;
					if(ei>=FIND_PATH_MAX_RANGE) break;
					path_buffer[ei].x=tx;
					path_buffer[ei].y=ty;
					ei++;
					state[ty*width+tx]|=WALK_READY_FLAG;
				}
			}

			//left
			tx=px-1;
			if(tx>0)
			{
				ty=py;
				if(!(WALK_READY_FLAG&state[ty*width+tx]))
				{
					if(!(BARMASK_MAP_BAR&state[ty*width+tx]))  goto _ok;
					if(ei>=FIND_PATH_MAX_RANGE) break;
					path_buffer[ei].x=tx;
					path_buffer[ei].y=ty;
					ei++;
					state[ty*width+tx]|=WALK_READY_FLAG;
				}
			}

			//down
			ty=py+1;
			if(ty<t_height)
			{
				tx=px;
				if(!(WALK_READY_FLAG&state[ty*width+tx]))
				{
					if(!(BARMASK_MAP_BAR&state[ty*width+tx]))  goto _ok;
					if(ei>=FIND_PATH_MAX_RANGE) break;
					path_buffer[ei].x=tx;
					path_buffer[ei].y=ty;
					ei++;
					state[ty*width+tx]|=WALK_READY_FLAG;
				}
			}

			//right up
			tx=px+1;
			ty=py-1;
			if(	tx<t_width&&
				ty>0)
			{
				if(!(WALK_READY_FLAG&state[ty*width+tx]))
				{
					if(!(BARMASK_MAP_BAR&state[ty*width+tx]))  goto _ok;
					if(ei>=FIND_PATH_MAX_RANGE) break;
					path_buffer[ei].x=tx;
					path_buffer[ei].y=ty;
					ei++;
					state[ty*width+tx]|=WALK_READY_FLAG;
				}
			}

			//left up
			tx=px-1;
			ty=py-1;
			if(	tx>0&&
				ty>0)
			{
				if(!(WALK_READY_FLAG&state[ty*width+tx]))
				{
					if(!(BARMASK_MAP_BAR&state[ty*width+tx]))  goto _ok;
					if(ei>=FIND_PATH_MAX_RANGE) break;
					path_buffer[ei].x=tx;
					path_buffer[ei].y=ty;
					ei++;
					state[ty*width+tx]|=WALK_READY_FLAG;
				}
			}

			//left down
			tx=px-1;
			ty=py+1;
			if(	tx>0&&
				ty<t_height)
			{
				if(!(WALK_READY_FLAG&state[ty*width+tx]))
				{
					if(!(BARMASK_MAP_BAR&state[ty*width+tx]))  goto _ok;
					if(ei>=FIND_PATH_MAX_RANGE) break;
					path_buffer[ei].x=tx;
					path_buffer[ei].y=ty;
					ei++;
					state[ty*width+tx]|=WALK_READY_FLAG;
				}
			}

			//right down
			tx=px+1;
			ty=py+1;
			if(	tx<t_width&&
				ty<t_height)
			{
				if(!(WALK_READY_FLAG&state[ty*width+tx]))
				{
					if(!(BARMASK_MAP_BAR&state[ty*width+tx]))  goto _ok;
					if(ei>=FIND_PATH_MAX_RANGE) break;
					path_buffer[ei].x=tx;
					path_buffer[ei].y=ty;
					ei++;
					state[ty*width+tx]|=WALK_READY_FLAG;
				}
			}

			si++;
		}
		while(ei--)
			state[path_buffer[ei].y*width+path_buffer[ei].x]&=~WALK_READY_FLAG;
	}
	return;
_ok:;
	while(ei--)
		state[path_buffer[ei].y*width+path_buffer[ei].x]&=~WALK_READY_FLAG;
	x=tx*cell_width+cell_width/2;
	y=ty*cell_height+cell_height/2;
}






