/*
**      XFONT.H
**      chine font functions.
**      ZJian,2000/7/26.
*/
#ifndef _XFONT_H_INCLUDE_
#define _XFONT_H_INCLUDE_       1
#include "xgrafx.h"
#include "winfont.h"

//#define   USE_PIXFONT

/*
FONT COLOR:
0   SYSTEM_WHITE,
1   SYSTEM_RED,
2   SYSTEM_GREEN,
3   SYSTEM_BLUE,
4   SYSTEM_YELLOW,
5   SYSTEM_CYAN,
6   SYSTEM_PINK,
7   SYSTEM_WHITE,
8   SYSTEM_BLACK,
9   SYSTEM_DARK0,
10  SYSTEM_DARK1,
11  SYSTEM_DARK2,
12  SYSTEM_DARK3,
13  SYSTEM_DARK4,
14  SYSTEM_DARK5,
15  SYSTEM_DARK6
*/



#define FONT16_FILENAME     "font16.dat"
#define FONT24_FILENAME     "font24.dat"

#define MAX_FONT_DATA       4   // max font data
#define MAX_FONT_PATTERN    32  // max font pattern

#define FONT_TYPE_BIG5      1   // font type of BIG5
#define FONT_TYPE_GB        2   // font type of GB

#define FONT16              0   // 16X16 BIG5 font
#define FONT24              1   // 24X24 BIG5 font

#define COPY_PUT_COLOR(a)   ( ( ((a)<<16) & 0xffff0000 ) | (COPY_PUT & 0x0000ffff) )



EXPORT  SLONG   FNBACK  make_font_file(char *font_filename,
                                       SLONG type,
                                       SLONG ascii_width,SLONG ascii_height,
                                       SLONG chine_width,SLONG chine_height,
                                       char *ascii_filename,char *chine_filename);
EXPORT  SLONG   FNBACK  load_font_file(SLONG index,char *filename);
EXPORT  int     FNBACK  init_font(void);
EXPORT  void    FNBACK  free_font(void);
EXPORT  void    FNBACK  active_font(int active);
EXPORT  void    FNBACK  print_font(SLONG font_index,SLONG sx,SLONG sy,USTR *data,SLONG put_type,BMP *bitmap);


#ifdef  USE_PIXFONT

#define print16(x,y,str,type,bitmap)    print_font(FONT16,(x),(y),(str),(type),(bitmap))
#define print24(x,y,str,type,bitmap)    print_font(FONT24,(x),(y),(str),(type),(bitmap))

#endif//USE_PIXFONT


#endif//_XFONT_H_INCLUDE_
//=======================================================================