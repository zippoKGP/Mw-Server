/*
**      RIPPLE.CPP
**      ripple effect functions.
**
**      ZJian,2000.09.06.
*/
#include "rays.h"
#include "winmain.h"
#include "xsystem.h"
#include "xgrafx.h"

#define RIPPLE_INIT         0x01

short * ripple_buffer1 = NULL;
short * ripple_buffer2 = NULL;
SLONG   ripple_flag = 0;
SLONG   ripple_buffer_width;
SLONG   ripple_buffer_height;
SLONG   ripple_buffer_size;

EXPORT  int     FNBACK  init_ripple(void)
{
    ripple_buffer_width = g_nScreenWidth;
    ripple_buffer_height = g_nScreenHeight;
    ripple_buffer_size = ripple_buffer_width * ripple_buffer_height;
    if(ripple_flag & RIPPLE_INIT) return 0;
    ripple_buffer1=(short *)GlobalAlloc(GPTR, ripple_buffer_size*sizeof(short));
    ripple_buffer2=(short *)GlobalAlloc(GPTR, ripple_buffer_size*sizeof(short));
    memset(ripple_buffer1,0,sizeof(short)*ripple_buffer_size);
    memset(ripple_buffer2,0,sizeof(short)*ripple_buffer_size);
    ripple_flag = RIPPLE_INIT;
    return 0;
}


EXPORT  void    FNBACK  free_ripple(void)
{
    if(ripple_buffer1)
    {
        GlobalFree(ripple_buffer1);
        ripple_buffer1=NULL;
    }
    if(ripple_buffer2)
    {
        GlobalFree(ripple_buffer2);
        ripple_buffer2=NULL;
    }
    ripple_flag = 0;
}


EXPORT  void    FNBACK  active_ripple(int active)
{
    if(active)
    {
    }
}


EXPORT  void    FNBACK  clear_ripple(void)
{
    if(!(ripple_flag & RIPPLE_INIT))
        return;
    memset(ripple_buffer1,0,sizeof(short)*ripple_buffer_size);
    memset(ripple_buffer2,0,sizeof(short)*ripple_buffer_size);
}


EXPORT  void    FNBACK  spread_ripple(void)
{
    short temp;
    if(!(ripple_flag & RIPPLE_INIT))
        return;
    for(SLONG i=ripple_buffer_width;i<ripple_buffer_size-ripple_buffer_width;i++)
    {
        // ripple energe spread
        ripple_buffer2[i] = (short)( ((ripple_buffer1[i-1]+ripple_buffer1[i+1]+ripple_buffer1[i-ripple_buffer_width]
            +ripple_buffer1[i+ripple_buffer_width])>>1)
            -ripple_buffer2[i] );
        // ripple energe attenuation
        temp=(short)(ripple_buffer2[i]>>5);
        ripple_buffer2[i] = (short)(ripple_buffer2[i]-temp);
    }
    short *p=ripple_buffer1;
    ripple_buffer1=ripple_buffer2;
    ripple_buffer2=p;
}


EXPORT  void    FNBACK  render_ripple(BMP *bitmap)
{
    int xoff,yoff,k,x,y;

    if(!(ripple_flag & RIPPLE_INIT))
        return;
    k=ripple_buffer_width;
    for(y=1;y<ripple_buffer_height-1;y++)
    {
        for(x=0;x<ripple_buffer_width;x++)
        {
            // oper offset
            xoff=ripple_buffer1[k-1]-ripple_buffer1[k+1];
            yoff=ripple_buffer1[k-ripple_buffer_width]-ripple_buffer1[k+ripple_buffer_width];

            // check the position is in the window
            if((y+yoff)<0) {k++;continue;}
            if((y+yoff)>=ripple_buffer_height) {k++;continue;}
            if((x+xoff)<0) {k++;continue;}
            if((x+xoff)>=ripple_buffer_width) {k++;continue;}

            bitmap->line[y][x]=bitmap->line[y+yoff][x+xoff];
            k++;
        }
    }
}


EXPORT  void    FNBACK  drop_stone(SLONG x,SLONG y,SLONG stone_size,SLONG stone_weight)
{
    // PS: stone_weight = 32~128
    if(!(ripple_flag & RIPPLE_INIT))
        return;
    if( (x+stone_size)>ripple_buffer_width ||
        (y+stone_size)>ripple_buffer_height ||
        (x-stone_size)<0 ||
        (y-stone_size)<0)
        return;
    for(int posx=x-stone_size;posx<x+stone_size;posx++)
    {
        for(int posy=y-stone_size;posy<y+stone_size;posy++)
        {
            if((posx-x)*(posx-x)+(posy-y)*(posy-y) < stone_size*stone_size)
            {
                ripple_buffer1[ripple_buffer_width*posy+posx] = (short)(-stone_weight);
            }
        }
    }
}