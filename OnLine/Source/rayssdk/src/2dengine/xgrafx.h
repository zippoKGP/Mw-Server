/*
**    XGRAFX.H
**    graphics functions header.
**    ZJian,2000/7/11.
*/
#ifndef _XGRAFX_H_INCLUDE_
#define _XGRAFX_H_INCLUDE_      1

#define U4_SYSTEM_RED       0x00ff0000
#define U4_SYSTEM_GREEN     0x0000ff00
#define U4_SYSTEM_BLUE      0x000000ff
#define U4_SYSTEM_YELLOW    0x00ffff00
#define U4_SYSTEM_CYAN      0x0000ffff
#define U4_SYSTEM_PINK      0x00ff00ff
#define U4_SYSTEM_WHITE     0x00ffffff
#define U4_SYSTEM_BLACK     0x00000000
#define U4_SYSTEM_DARK0     0x00202020
#define U4_SYSTEM_DARK1     0x00404040
#define U4_SYSTEM_DARK2     0x00606060
#define U4_SYSTEM_DARK3     0x00808080
#define U4_SYSTEM_DARK4     0x00a0a0a0
#define U4_SYSTEM_DARK5     0x00c0c0c0
#define U4_SYSTEM_DARK6     0x00e0e0e0
#define U4_SYSTEM_PEST      0x000000ff

#define ENCODE_LEFT         0x01
#define ENCODE_BOTTOM       0x02
#define ENCODE_RIGHT        0x04
#define ENCODE_TOP          0x08

//////////////////////////////////////////////////////////////////////
// PIXEL data type
#define SIZEOFPIXEL         2

typedef unsigned short int  PIXEL;

//////////////////////////////////////////////////////////////////////
// BMP struct
typedef struct tagBMP
{
    SLONG w,h,pitch;       //BMP width,height,pitch(bytes per line)
    PIXEL *line[1];      //BMP line address
} BMP,*LPBMP;

//////////////////////////////////////////////////////////////////////
// ABMP struct
typedef struct tagABMP
{
    SLONG w,h,pitch;
    USTR  *line[1];
} ABMP,*LPABMP;

//////////////////////////////////////////////////////////////////////
// ANI struct
typedef struct  tagBITMAP_ANI   BITMAP_ANI,*LPBITMAP_ANI;
struct  tagBITMAP_ANI
{
    BMP *bitmap;        // ani frame image 
    BITMAP_ANI *last;   // last node
    BITMAP_ANI *next;   // next node
};

//////////////////////////////////////////////////////////////////////
// PAL struct
typedef struct  tagPAL
{
    PIXEL data[256];    // palette indexed color
} PAL,*LPPAL;

//////////////////////////////////////////////////////////////////////
// ALF struct
typedef struct  tagALF
{
    SLONG   w;        // alfa pixel width
    SLONG   h;        // alfa pixel height
    USTR    *line[1]; // alfa information
} ALF,*LPALF;

//////////////////////////////////////////////////////////////////////
// IMG struct
typedef struct  tagIMG
{
    SLONG   w;          // img image width
    SLONG   h;          // img image height
    USTR    *line[1];   // img image line pointer
} IMG,*LPIMG;

//////////////////////////////////////////////////////////////////////
// typedef functions
typedef SLONG   (*PFNREDRAW) ( BMP *bmp);

//////////////////////////////////////////////////////////////////////
// global variables
extern  BMP *   screen_buffer;
extern  BMP *   screen_channel0;
extern  BMP *   screen_channel1;
extern  BMP *   screen_channel2;

extern  UHINT   (*true2hi)(ULONG c);
extern  ULONG   (*hi2true)(UHINT c);
extern  UCHR    (*get_r)(UHINT c);
extern  UCHR    (*get_g)(UHINT c);
extern  UCHR    (*get_b)(UHINT c);
extern  void    (*set_r)(UHINT *c,UCHR r);
extern  void    (*set_g)(UHINT *c,UCHR g);
extern  void    (*set_b)(UHINT *c,UCHR b);
extern  UHINT   (*rgb2hi)(UCHR r,UCHR g,UCHR b);
extern  void    (*hi2rgb)(UHINT c,UCHR *r,UCHR *g,UCHR *b);
extern  UHINT   (*hi2fff)(UHINT c);
extern  UHINT   (*fff2hi)(UHINT c);
extern  void    (*oper_alpha_color)(UHINT *fore_color, UHINT *back_color, SLONG alpha);
extern  void    (*oper_additive_color)(UHINT *fore_color, UHINT *back_color);
extern  void    (*oper_gray_color) (UHINT *fore_color,UHINT *back_color);
extern  void    (*oper_subtractive_color) (UHINT *fore_color,UHINT *back_color);
extern  void    (*oper_minimum_color) (UHINT *fore_color,UHINT *back_color);
extern  void    (*oper_maximum_color) (UHINT *fore_color,UHINT *back_color);
extern  void    (*oper_half_color) (UHINT *fore_color,UHINT *back_color);
extern  void    (*oper_dark_color) (UHINT *fore_color, SLONG dark);
extern  void    (*oper_red_color) (UHINT *fore_color, UHINT *back_color);
extern  void    (*oper_green_color) (UHINT *fore_color, UHINT *back_color);
extern  void    (*oper_blue_color) (UHINT *fore_color, UHINT *back_color);
extern  void    (*oper_yellow_color) (UHINT *fore_color, UHINT *back_color);
extern  void    (*oper_merge_color) (UHINT *fore_color, UHINT *back_color, SLONG alpha);
extern  void    (*oper_partial_color) (UHINT *fore_color, UHINT *back_color, SLONG alpha);
extern  void    (*oper_complement_color) (UHINT *fore_color, UHINT *back_color, SLONG alpha);
extern	void	(*oper_adulterate_color) (UHINT *fore_color,  SLONG r, SLONG g, SLONG b);
extern	void	(*oper_eclipse_color) (UHINT *fore_color,  SLONG r, SLONG g, SLONG b);
extern  void    (*oper_blue_alpha_color) (UHINT *fore_color, UHINT *back_color, SLONG alpha);
//
extern  void    (*update_screen)(BMP *bitmap);
extern  void    (*get_screen)(BMP *bitmap);
//
extern	void    (*put_bitmap) (SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);
extern	void    (*alpha_put_bitmap) (SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp,SLONG alpha);
extern	void	(*half_put_bitmap) (SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);
extern	void	(*gray_put_bitmap) (SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);
extern	void	(*additive_put_bitmap) (SLONG sx,SLONG sy,BMP *src,BMP *des);
extern	void	(*bound_put_bitmap) (SLONG sx, SLONG sy, SLONG bound_sx, SLONG bound_sy, SLONG bound_xl, SLONG bound_yl, BMP *src_bmp, BMP *dest_bmp);
extern	void	(*adulterate_bitmap) (SLONG r,SLONG g,SLONG b,BMP *bmp);
extern	void	(*eclipse_bitmap) (SLONG r,SLONG g,SLONG b,BMP *bmp);
extern  void    (*scale_put_bitmap) (SLONG sx,SLONG sy,SLONG scale_xl,SLONG scale_yl,BMP *src_bmp,BMP *dest_bmp);
//
extern	void    (*alpha_put_bar)(SLONG sx,SLONG sy,SLONG xl,SLONG yl,PIXEL c,BMP *bmp,SLONG alpha);


//////////////////////////////////////////////////////////////////////
// system colors
//====================================================================
extern  PIXEL   SYSTEM_RED;
extern  PIXEL   SYSTEM_GREEN;
extern  PIXEL   SYSTEM_BLUE;
extern  PIXEL   SYSTEM_YELLOW;
extern  PIXEL   SYSTEM_CYAN;
extern  PIXEL   SYSTEM_PINK;
extern  PIXEL   SYSTEM_WHITE;
extern  PIXEL   SYSTEM_BLACK;
extern  PIXEL   SYSTEM_DARK0;
extern  PIXEL   SYSTEM_DARK1;
extern  PIXEL   SYSTEM_DARK2;
extern  PIXEL   SYSTEM_DARK3;
extern  PIXEL   SYSTEM_DARK4;
extern  PIXEL   SYSTEM_DARK5;
extern  PIXEL   SYSTEM_DARK6;
extern  PIXEL   SYSTEM_PEST;

//////////////////////////////////////////////////////////////////////
// function prototypes
//====================================================================
EXPORT  BMP *   FNBACK  create_bitmap(SLONG w,SLONG h);
EXPORT  BMP *   FNBACK  create_mirror_bitmap(SLONG w,SLONG h);
EXPORT  BMP *   FNBACK  create_sub_bitmap(BMP *fbmp,SLONG x,SLONG y,SLONG w,SLONG h);
EXPORT  void    FNBACK  destroy_bitmap(BMP **bmp);
EXPORT  void    FNBACK  rotate_bitmap_left_right(BMP *bitmap);
EXPORT  void    FNBACK  rotate_bitmap_top_bottom(BMP *bitmap);
EXPORT  SLONG   FNBACK  compute_bitmap_save_size(BMP *bitmap);

EXPORT  ABMP*   FNBACK  create_abitmap(SLONG w,SLONG h);
EXPORT  void    FNBACK  destroy_abitmap(ABMP **abmp);
EXPORT  void    FNBACK  put_abitmap(SLONG sx,SLONG sy,ABMP *abmp,BMP *bmp);
EXPORT  void    FNBACK  rotate_abitmap_left_right(ABMP *abitmap);
EXPORT  void    FNBACK  rotate_abitmap_top_bottom(ABMP *abitmap);

EXPORT  ALF *   FNBACK  create_alf(SLONG w,SLONG h);
EXPORT  void    FNBACK  destroy_alf(ALF **alf);

EXPORT  IMG *   FNBACK  create_img(SLONG w,SLONG h);
EXPORT  void    FNBACK  destroy_img(IMG **img);
EXPORT  void    FNBACK  clear_img(IMG *img);
EXPORT  void    FNBACK  get_img(SLONG sx,SLONG sy,SLONG xl,SLONG yl,IMG *des_img,IMG *src_img);

EXPORT  UCHR    FNBACK  encode_point(SLONG x,SLONG y,SLONG left,SLONG top,SLONG right,SLONG bottom);
EXPORT  SLONG   FNBACK  clip_segment(SLONG *x1,SLONG *y1,SLONG *x2,SLONG *y2,SLONG left,SLONG top,SLONG right,SLONG bottom);

EXPORT  PIXEL   FNBACK  get_pixel(SLONG x,SLONG y,BMP *bmp);
EXPORT  void    FNBACK  put_pixel(SLONG x,SLONG y,PIXEL c,BMP *bmp);
EXPORT  void    FNBACK  range_put_pixel(SLONG x,SLONG y,PIXEL c,BMP *bmp);

EXPORT  void    FNBACK  put_line(SLONG x1,SLONG y1,SLONG x2,SLONG y2,PIXEL c,BMP *bmp);
EXPORT  void    FNBACK  put_dash_line(SLONG x1,SLONG y1,SLONG x2,SLONG y2,PIXEL c,SLONG skip,BMP *bmp);
EXPORT  void    FNBACK  put_arc(SLONG cx,SLONG cy,SLONG a,SLONG b,SLONG t1,SLONG t2,PIXEL color,BMP *bitmap);
EXPORT  void    FNBACK  put_circle(SLONG cx,SLONG cy,SLONG r,PIXEL color,BMP *bitmap);
EXPORT  void    FNBACK  put_ellipse(SLONG cx,SLONG cy,SLONG a,SLONG b,PIXEL color,BMP *bitmap);
EXPORT  void    FNBACK  put_cross(SLONG x,SLONG y,PIXEL c,BMP *bmp);
EXPORT  void    FNBACK  put_xcross(SLONG x,SLONG y,PIXEL c,BMP *bmp);
EXPORT  void    FNBACK  put_box(SLONG sx,SLONG sy,SLONG xl,SLONG yl,PIXEL c,BMP *bmp);
EXPORT  void    FNBACK  put_section(SLONG x,SLONG y,SLONG half_size,PIXEL outline_color,PIXEL inline_color,BMP *bmp);

EXPORT  void    FNBACK  put_bar(SLONG sx,SLONG sy,SLONG xl,SLONG yl,PIXEL c,BMP *bmp);
EXPORT  void    FNBACK  put_menu_hibar(SLONG sx,SLONG sy,SLONG xl,SLONG yl,BMP *bmp);
EXPORT  void    FNBACK  put_menu_lobar(SLONG sx,SLONG sy,SLONG xl,SLONG yl,BMP *bmp);
EXPORT  void    FNBACK  put_menu_box(SLONG sx,SLONG sy,SLONG xl,SLONG yl,BMP *bmp);

EXPORT  void    FNBACK  put_base_button(SLONG sx,SLONG sy,SLONG active,BMP *bmp);
EXPORT  void    FNBACK  put_minimize_button(SLONG sx,SLONG sy,SLONG active,BMP *bmp);
EXPORT  void    FNBACK  put_maximize_button(SLONG sx,SLONG sy,SLONG active,BMP *bmp);
EXPORT  void    FNBACK  put_closing_button(SLONG sx,SLONG sy,SLONG active,BMP *bmp);
EXPORT  void    FNBACK  put_up_scroll_button(SLONG sx,SLONG sy,SLONG active,BMP *bmp);
EXPORT  void    FNBACK  put_down_scroll_button(SLONG sx,SLONG sy,SLONG active,BMP *bmp);
EXPORT  void    FNBACK  put_left_scroll_button(SLONG sx,SLONG sy,SLONG active,BMP *bmp);
EXPORT  void    FNBACK  put_right_scroll_button(SLONG sx,SLONG sy,SLONG active,BMP *bmp);
EXPORT  void    FNBACK  put_vertical_scroll_bar(SLONG sx,SLONG sy,SLONG yl,BMP *bmp);
EXPORT  void    FNBACK  put_horizontal_scroll_bar(SLONG sx,SLONG sy,SLONG xl,BMP *bmp);
EXPORT  void    FNBACK  put_triangle_bar(SLONG sx, SLONG sy, PIXEL color, BMP *bmp);

EXPORT  void    FNBACK  clear_bitmap(BMP *bmp);
EXPORT  void    FNBACK  fill_bitmap(BMP *bmp,PIXEL c);
EXPORT  BMP *   FNBACK  copy_bitmap(SLONG sx,SLONG sy,SLONG xl,SLONG yl,BMP *src_bmp);
EXPORT  void    FNBACK  get_bitmap(SLONG sx,SLONG sy,SLONG xl,SLONG yl,BMP *des_bmp,BMP *src_bmp);
EXPORT  void    FNBACK  mirror_bitmap(SLONG sx,SLONG sy,SLONG xl,SLONG yl,BMP *des_bmp,BMP *src_bmp);

EXPORT  void    FNBACK  clear_abitmap(ABMP *abmp);
EXPORT  ABMP *  FNBACK  copy_abitmap(SLONG sx,SLONG sy,SLONG xl,SLONG yl,ABMP *src_abmp);
EXPORT  void    FNBACK  clear_abitmap_image(ABMP *abmp);
EXPORT  void    FNBACK  fill_abitmap_image(ABMP *abmp, PIXEL c);
EXPORT  void    FNBACK  clear_abitmap_alpha(ABMP *abmp);


EXPORT  void    FNBACK  pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);

EXPORT  void    FNBACK  mesh_put_bar(SLONG sx,SLONG sy,SLONG xl,SLONG yl,PIXEL c,BMP *bmp);
EXPORT  void    FNBACK  mesh_put_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);
EXPORT  void    FNBACK  mesh_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);

EXPORT  void    FNBACK  mask_pest_bitmap(SLONG sx,SLONG sy,PIXEL mask_color,BMP *src_bmp,BMP *dest_bmp);
EXPORT  void    FNBACK  mask_edge_pest_bitmap(SLONG sx,SLONG sy,PIXEL mask_color,BMP *src_bmp,BMP *dest_bmp);

EXPORT  void    FNBACK  gray_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);
EXPORT  void    FNBACK  gray_edge_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);

EXPORT  void    FNBACK  alpha_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp,SLONG alpha);
EXPORT  void    FNBACK  alpha_edge_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp,SLONG alpha);

EXPORT  void    FNBACK  additive_put_bar(SLONG sx,SLONG sy,SLONG xl,SLONG yl,PIXEL c,BMP *bmp);
EXPORT  void    FNBACK  additive_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);
EXPORT  void    FNBACK  additive_edge_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);

EXPORT  void    FNBACK  subtractive_put_bar(SLONG sx,SLONG sy,SLONG xl,SLONG yl,PIXEL c,BMP *bmp);
EXPORT  void    FNBACK  subtractive_put_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);
EXPORT  void    FNBACK  subtractive_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);
EXPORT  void    FNBACK  subtractive_edge_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);

EXPORT  void    FNBACK  minimum_put_bar(SLONG sx,SLONG sy,SLONG xl,SLONG yl,PIXEL c,BMP *bmp);
EXPORT  void    FNBACK  minimum_put_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);
EXPORT  void    FNBACK  minimum_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);
EXPORT  void    FNBACK  minimum_edge_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);

EXPORT  void    FNBACK  maximum_put_bar(SLONG sx,SLONG sy,SLONG xl,SLONG yl,PIXEL c,BMP *bmp);
EXPORT  void    FNBACK  maximum_put_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);
EXPORT  void    FNBACK  maximum_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);
EXPORT  void    FNBACK  maximum_edge_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);

EXPORT  void    FNBACK  half_put_bar(SLONG sx,SLONG sy,SLONG xl,SLONG yl,PIXEL c,BMP *bmp);
EXPORT  void    FNBACK  half_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);
EXPORT  void    FNBACK  half_edge_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);

EXPORT  void    FNBACK  dark_put_bar(SLONG sx,SLONG sy,SLONG xl,SLONG yl,BMP *bmp,SLONG dark);

EXPORT  void    FNBACK  scale_pest_bitmap(SLONG sx,SLONG sy,SLONG scalex,SLONG scaley,BMP *src_bmp,BMP *dest_bmp);

EXPORT  void    FNBACK  set_system_color(void);
EXPORT  void    FNBACK  convert_buffer_hi2fff(PIXEL *buffer,SLONG size);
EXPORT  void    FNBACK  convert_buffer_fff2hi(PIXEL *buffer,SLONG size);
EXPORT  void    FNBACK  convert_bitmap_hi2fff(BMP *bitmap);
EXPORT  void    FNBACK  convert_bitmap_fff2hi(BMP *bitmap);

EXPORT  void    FNBACK  destroy_bitmap_ani(BITMAP_ANI **bitmap_ani);
EXPORT  void    FNBACK  fade_effect(SLONG tick);
EXPORT  void    FNBACK  oper_image_size(SLONG *sx,SLONG *sy,SLONG *ex,SLONG *ey,BMP *bitmap);
EXPORT  BMP *   FNBACK  copy_bitmap_by_image(BMP *bmp,SLONG *sx,SLONG *sy);
EXPORT  void    FNBACK  convert_buffer_to_bitmap(PIXEL *buffer,SLONG w,SLONG h,BMP *dest_bmp);
EXPORT  void    FNBACK  oper_abitmap_image_size(SLONG *sx,SLONG *sy,SLONG *ex,SLONG *ey,ABMP *abitmap);
EXPORT  ABMP *  FNBACK  copy_abitmap_by_image(ABMP *abmp,SLONG *sx,SLONG *sy);

EXPORT  double  FNBACK  analyse_bitmap(BMP *src_bmp);
EXPORT  void    FNBACK  find_bitmap_barycenter(SLONG *bx, SLONG *by, BMP *src_bmp);
EXPORT  ABMP *  FNBACK  make_abitmap_from_buffer(SLONG w,SLONG h,PIXEL *pix_buffer,USTR *alpha_buffer);
EXPORT  void    FNBACK  put_abitmap_to_abitmap(SLONG sx,SLONG sy,ABMP *src_abmp,ABMP *dest_abmp);
EXPORT  ABMP *  FNBACK  make_abitmap_from_bitmap(BMP *src_bmp);
EXPORT  BMP *   FNBACK  make_shadow_bitmap(BMP *src_bitmap, SLONG *stagger_rx, SLONG *stagger_ry);
EXPORT  ABMP *  FNBACK  make_shadow_abitmap(ABMP *src_abitmap, SLONG *stagger_rx, SLONG *stagger_ry);

EXPORT  void    FNBACK  change_color(SLONG sx,SLONG sy,SLONG xl,SLONG yl,PIXEL src_color,PIXEL des_color,BMP *bitmap);

EXPORT  int     FNBACK  init_grafx(void);
EXPORT  void    FNBACK  free_grafx(void);

EXPORT  void    FNBACK  make_indexed_color_table(USTR *palette, PIXEL *color_table);

/****************************************************************************************************************
** 转换 RGB(红绿蓝) 到 HLS(Hue, Lightness, Saturation)(色度,亮度,饱和度)                                        *
** known:    r,g,b all in[0,1]                                                                                  *
** find:     h in [0,360], l and s in [0,1],                                                                    *
**           except if s=0, then h=undefined                                                                    *
*****************************************************************************************************************/
EXPORT  void    FNBACK  float_rgb2hls(float r, float g, float b, float *h, float *l, float *s);


/****************************************************************************************************************
** 转换 RGB(红绿蓝) 到 HSV(Hue, Saturation, Value)(色度,亮度,饱和度)                                            *
** known:    r,g,b all in[0,1]                                                                                  *
** find:     h in [0,360], s and v in [0,1]                                                                     *
**                                                                                                              *
*****************************************************************************************************************/
EXPORT  void    FNBACK  float_rgb2hsv(float r, float g, float b, float *h, float *s, float *v);


//LOCAL CPP FUNCTIONS -------------------------------------------------------------------------------------------
extern	void    _put_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);
extern	void    _alpha_put_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp,SLONG alpha);
extern	void	_half_put_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);
extern	void	_gray_put_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);
extern	void	_additive_put_bitmap(SLONG sx,SLONG sy,BMP *src,BMP *des);
extern	void	_bound_put_bitmap (SLONG sx, SLONG sy, SLONG bound_sx, SLONG bound_sy, SLONG bound_xl, SLONG bound_yl, BMP *src_bmp, BMP *dest_bmp);
extern	void	_adulterate_bitmap(SLONG r,SLONG g,SLONG b,BMP *bmp);
extern	void	_eclipse_bitmap(SLONG r,SLONG g,SLONG b,BMP *bmp);
extern  void    _scale_put_bitmap(SLONG sx,SLONG sy,SLONG scalex,SLONG scaley,BMP *src_bmp,BMP *dest_bmp);
//
extern	void    _alpha_put_bar(SLONG sx,SLONG sy,SLONG xl,SLONG yl,PIXEL c,BMP *bmp,SLONG alpha);



#endif//_XGRAFX_H_INCLUDE_

