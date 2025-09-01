/*
**  WINFONT.H
**  Windows font functions header.
**  
**  Dixon, 2000.3.13.
**      Created.
**  ZJian, 2001.1.12.
**      Added in.
**
*/
/*****************************************************************************}
{ TEXT Control format                                                         }
{                                                                             }
{ Leading Code : '~'                                                          }
{  '~'+'Z+' --> Clear Zoom                                                    }
{  '~'+'Z-' --> Enable Zoom                                                   }
{  '~'+'Z'+'NUM' --> Set specious font size level    0~5                      }
{  '~'+'S'+'NUM' --> Set specious font  SPECIAL TYPE                          }
{  '~'+'A'+'NUM' --> Set specious font alpah level   0~9                      }
{  '~'+'C'+'NUM' --> Set specious font color                                  }
{  '~'+'F'+'1'   --> Enable font out frame                                    }
{  '~'+'F'+'2'   --> Enable font shadow                                       }
{  '~'+'F'+'0'   --> Disable font out frame  and shadow                       }
******************************************************************************/
#ifndef WINFONT_H_INCLUDE   
#define WINFONT_H_INCLUDE   1
#include "xgrafx.h"


#define USE_WINFONT

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
//#define FONT_BIG52GB    1         


//--------------------------------------------------------------
// if want to change font buffer width (change thest define )
//#define PEST_PUT                0
//#define COPY_PUT                1

//#define FONT_WIDTH        16
//#define FONT_HEIGHT       16

#define FONT_DATA_SIZE      32   //ULONG type size

#define MAX_FONT_SIZE_TYPE  6

#define FONT_BUFFER_WIDTH   4
#define FONT_BUFFER_HEIGHT  32

#define MAX_FONT_NUM        100
#define FONT_BUFFER_SIZE    MAX_FONT_NUM*FONT_BUFFER_WIDTH*FONT_BUFFER_HEIGHT

#define FONT_NAME       ".\\FONT\\FONT1.TTF"

#define MAX_FONT_COLOR      16
/////////////////////////////////////////////////////////////////////////////////////////////
#define MAKE_ULONG(d0,d1,d2,d3)     (((d0) << 24) | ((d1)<<16) | ((d2)<<8) | (d3))
#define MAKE_UHINT(d0,d1)           (((d0) << 8 ) | d1)
/////////////////////////////////////////////////////////////////////////////////////////////
#define FONT_FRAME_DUMP             0x00
#define FONT_FRAME_OUTLINE          0x01
#define FONT_FRAME_SHADOW           0x02
/////////////////////////////////////////////////////////////////////////////////////////////

//font support system flags, can be conbined.
#define SUPPORT_FONT_SYSTEM_GB      0x01
#define SUPPORT_FONT_SYSTEM_BIG5    0x02


typedef struct FONT_CONFIG_STRUCT
{
    
    SLONG lFontWidth  ;
    SLONG lFontHeight ;
    UHINT iFontType   ;
    UHINT iFontColor  ;
}FONT_CONFIG;
typedef struct FONT_BUFFER_INFO_STRUCT
{
    SLONG   xl;
    SLONG   yl;
    UHINT   iChar;        
    
    SHINT   iNext;
    
}FONT_BUFFER_INFO;

typedef struct FONT_WIDTH_HEIGHT_STRUCT
{
    SLONG lWidth;
    SLONG lHeight;
}FONT_WIDTH_HEIGHT;


//字间距
static FONT_WIDTH_HEIGHT s_taFontWH[MAX_FONT_SIZE_TYPE] = 
{
    {12,12},
    {16,16},
    {20,20},
    {24,24},
    {28,28},
    {32,32}
};
//字体大小
static FONT_WIDTH_HEIGHT s_taFaceFontWH[MAX_FONT_SIZE_TYPE] =
{
    {12,12},
    {16,16},
    {20,20},
    {24,24},
    {28,28},
    {32,32}
};



typedef class GDI_FONT_CLASS
{
   public:
       GDI_FONT_CLASS();
       ~GDI_FONT_CLASS();
       
       SLONG    MyCreateFont(USTR * pbFile);
       SLONG    Initialize(void);
       SLONG    SearchFont(UHINT iChar);
       
       void     Buf_print(SLONG sx,SLONG sy,USTR * bStr,BMP *bitmap);
       void     Buf_range_print(SLONG sx,SLONG sy,USTR * bStr,BMP *bitmap);
       
       void     Buf_pest_print(SLONG sx,SLONG sy,USTR * bStr,BMP *bitmap);
       void     Buf_range_pest_print(SLONG sx,SLONG sy,USTR * bStr,BMP *bitmap);
       
       void     SetWordColor(UHINT iColor){m_iaColor[0]=iColor;m_now_front_color=0;}
       UHINT    GetWordColor(void){return(m_iaColor[m_now_front_color]);}
       SLONG    GetWordWidth(void);
       
       void     SetBackColor(UHINT iColor){m_iaBackColor[0]=iColor;m_now_back_color=0;}
       UHINT    GetBackColor(void){return(m_iaBackColor[m_now_back_color]);}
       
       void     SetWordColor(SLONG lab,UHINT iColor){m_iaColor[lab]=iColor;}
       void     SetBackColor(SLONG lab,UHINT iColor){m_iaBackColor[lab]=iColor;}
       
       //Jack, 2002.4.10. added.
       void     SetSupportFontSystem(SLONG flags) {m_support_system = flags; }
       void     SetConvertBig5ToGB(SLONG flag) {m_convert_big5togb = flag; }
       void     SetFontWeight(SLONG weight) {m_weight = weight;}

       
   protected:
       ULONG*   GetFont( UHINT iChar );
       SLONG    GdiWriteFont(ULONG *plDes,UHINT ichar);
       void     RsetFontSizeLevel(void);
       
   private:
       UHINT            m_iaColor[MAX_FONT_COLOR];
       UHINT            m_iaBackColor[MAX_FONT_COLOR];
       SLONG            m_now_front_color;
       SLONG            m_now_back_color;
       
       ULONG            *font_cache;
       
       FONT_BUFFER_INFO *font_bufinfo;
       FONT_BUFFER_INFO *m_pstFontInfoCache;

       
       USTR             *m_pabtBitMap[MAX_FONT_SIZE_TYPE];
       
       HDC              m_ahDc[MAX_FONT_SIZE_TYPE];
       HFONT            m_ahFont[MAX_FONT_SIZE_TYPE];
       HBITMAP          m_ahBmp[MAX_FONT_SIZE_TYPE];
       HPALETTE         m_hPal;
       
       LONG             m_lPitchBM;
       
       HBITMAP          m_ahBmpSave[MAX_FONT_SIZE_TYPE];
       HFONT            m_ahFontSave[MAX_FONT_SIZE_TYPE];
       HPALETTE         m_ahPalSave[MAX_FONT_SIZE_TYPE];
       
       SLONG            m_lSizeLevel;

       SLONG            m_special;
       SLONG            m_alpha_level;
       SLONG            m_frame_type;

       SLONG            m_gb_flag;
       SLONG            m_support_system;
       SLONG            m_convert_big5togb;
       SLONG            m_weight;
} GDI_FONT;
//////////////////////////////////////////////////////////////////////////////////////////////

int     init_winfont(void);
void    active_winfont(int active);
void    free_winfont(void);

void    print_640x480x16M(SLONG x,SLONG y,USTR *data,SLONG put_type,BMP *bit_map);
void    print_range_640x480x16M(SLONG x,SLONG y,USTR *data,SLONG put_type,BMP *bit_map);
void    set_word_color(UHINT color);
void    set_back_color(UHINT color);
void    set_word_color(SLONG lab,UHINT color);
void    set_back_color(SLONG lab,UHINT color);
SLONG   get_word_width(void);
void    set_winfont_support_system(SLONG flags);
void    set_winfont_name(USTR *name);
void    set_winfont_convert_big5_to_gb(SLONG flag);

//FW_BOLD, FW_THIN, FW_NORMAL,  FW_BOLD, FW_NORMAL
void    set_winfont_weight(SLONG weight);

//
UHINT   convert_word_gb2big(UHINT gb_code);
UHINT   convert_word_big2gb(UHINT big_code);




#ifdef  USE_WINFONT
#define print12(x,y,str,type,bitmap)    {\
    print_range_640x480x16M(0,0,(USTR *)"~Z0",PEST_PUT,bitmap);\
print_range_640x480x16M(x,y,(USTR *)str,type,bitmap);}

#define print16(x,y,str,type,bitmap)    {\
    print_range_640x480x16M(0,0,(USTR *)"~Z1",PEST_PUT,bitmap);\
print_range_640x480x16M(x,y,(USTR *)str,type,bitmap);}

#define print20(x,y,str,type,bitmap)    {\
    print_range_640x480x16M(0,0,(USTR *)"~Z2",PEST_PUT,bitmap);\
print_range_640x480x16M(x,y,(USTR *)str,type,bitmap);}

#define print24(x,y,str,type,bitmap)    {\
    print_range_640x480x16M(0,0,(USTR *)"~Z3",PEST_PUT,bitmap);\
print_range_640x480x16M(x,y,(USTR *)str,type,bitmap);}

#define print28(x,y,str,type,bitmap)    {\
    print_range_640x480x16M(0,0,(USTR *)"~Z4",PEST_PUT,bitmap);\
print_range_640x480x16M(x,y,(USTR *)str,type,bitmap);}

#define print32(x,y,str,type,bitmap)    {\
    print_range_640x480x16M(0,0,(USTR *)"~Z5",PEST_PUT,bitmap);\
print_range_640x480x16M(x,y,(USTR *)str,type,bitmap);}

#endif//USE_WINFONT


#endif//WINFONT_H_INCLUDE
