/*
**      XMEDIA.H
**      direct media functions header.
**
**      PS: AVI(exclude MPEG method avi)
**          MPEG(need some downloaded dll driver...)
**
**      created by dixon,2000/9/12.
**      modified by zjian,2000/10/14.
*/
#ifndef XMEDIA_H_INCLUDE
#define XMEDIA_H_INCLUDE    1
#pragma warning(disable:4201)   /* nonstandard extension used : nameless struct/union   */
//#include <amstream.h>
#pragma warning(default:4201)   /* nonstandard extension used : nameless struct/union   */
//#include <ddstream.h>
//#include <vfw.h>                /* video for windows    */


enum    XMEDIA_ENUMS
{  BREAK_PLAY_ENABLE    = 1,
BREAK_PLAY_DISABLE      = 0
};

typedef LONGLONG STREAM_TIME;

extern  RECT            rcXMedia;
extern  STREAM_TIME     stStartTime;
extern  STREAM_TIME     stEndTime;
extern  STREAM_TIME     stCurrentTime;
extern  STREAM_TIME     stDuration;

void    set_xmedia_over_draw(PFNREDRAW my_redraw);
void    play_xmedia_movie(SLONG sx,SLONG sy,SLONG ex,SLONG ey,USTR *filename,SLONG wait_flag);

SLONG   open_xmedia_file(USTR *filename,RECT *rect);
SLONG   play_xmedia_frame(BMP *bitmap,SLONG left_top_flag);
SLONG   close_xmedia_file(void);
void    seek_xmedia_file(STREAM_TIME st);


#endif/* XMEDIA_H_INCLUDE   */
