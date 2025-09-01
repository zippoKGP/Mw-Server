/*
**      XVGA.H
**      DirectDraw VGA functions header.
**      ZJian,20000901
*/
#ifndef XVGA_H_INCLUDE
#define XVGA_H_INCLUDE   1


#define R_MASK_555      0x7c00
#define G_MASK_555      0x03e0
#define B_MASK_555      0x001f
#define RB_MASK_555     0xfc1f      //0x7c1f
#define GRB32_MASK_555  0x03e0fc1f  //0x03e07c1f
#define RGB_HALF_555    0x7bde      //0RRRRrGGGGgBBBBb => 0RRRR0GGGG0BBBB0


#define R_MASK_655      0xfc00
#define G_MASK_655      0x03e0
#define B_MASK_655      0x001f
#define RB_MASK_655     0xfc1f
#define GRB32_MASK_655  0x03e0fc1f
#define RGB_HALF_655    0xfbde


#define R_MASK_565      0xf800
#define G_MASK_565      0x07e0
#define B_MASK_565      0x001f
#define RB_MASK_565     0xf81f
#define GRB32_MASK_565  0x07e0f81f
#define RGB_HALF_565    0xf7de


#define R_MASK_556      0xf800
#define G_MASK_556      0x07c0
#define B_MASK_556      0x003f
#define RB_MASK_556     0xf83f
#define GRB32_MASK_556  0x07c0f83f
#define RGB_HALF_556    0xf7be



typedef struct  tagVGA_INFO
{
    ULONG   r_mask;
    SLONG   r_bit_high;
    SLONG   r_bit_low;
    SLONG   r_bit_num;
    SLONG   r_shift;

    ULONG   g_mask;
    SLONG   g_bit_high;
    SLONG   g_bit_low;
    SLONG   g_bit_num;
    SLONG   g_shift;

    ULONG   b_mask;
    SLONG   b_bit_high;
    SLONG   b_bit_low;
    SLONG   b_bit_num;
    SLONG   b_shift;
} VGA_INFO,*LPVGA_INFO;;

extern  VGA_INFO    vga_info;

// VGA 555 FUNCTIONS
extern  UHINT   _true2hi_555(ULONG c);
extern  ULONG   _hi2true_555(UHINT c);
extern  UCHR    _get_r_555(UHINT c);
extern  UCHR    _get_g_555(UHINT c);
extern  UCHR    _get_b_555(UHINT c);
extern  void    _set_r_555(UHINT *c,UCHR r);
extern  void    _set_g_555(UHINT *c,UCHR g);
extern  void    _set_b_555(UHINT *c,UCHR b);
extern  UHINT   _rgb2hi_555(UCHR r,UCHR g,UCHR b);
extern  void    _hi2rgb_555(UHINT c,UCHR *r,UCHR *g,UCHR *b);
extern  UHINT   _fff2hi_555(UHINT c);
extern  UHINT   _hi2fff_555(UHINT c);
extern  void    _oper_alpha_color_555(UHINT *fore_color,UHINT *back_color,SLONG alpha);
extern  void    _oper_additive_color_555(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_gray_color_555(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_subtractive_color_555(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_minimum_color_555(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_maximum_color_555(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_half_color_555(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_dark_color_555(UHINT *fore_color, SLONG dark);
extern  void    _oper_red_color_555(UHINT *fore_color, UHINT *back_color);
extern  void    _oper_green_color_555(UHINT *fore_color, UHINT *back_color);
extern  void    _oper_blue_color_555(UHINT *fore_color, UHINT *back_color);
extern  void    _oper_yellow_color_555(UHINT *fore_color, UHINT *back_color);
extern  void    _oper_merge_color_555(UHINT *fore_color, UHINT *back_color, SLONG alpha);
extern  void    _oper_partial_color_555(UHINT *fore_color, UHINT *back_color, SLONG alpha);
extern  void    _oper_complement_color_555(UHINT *fore_color, UHINT *back_color, SLONG alpha);
extern	void	_oper_adulterate_color_555(UHINT *fore_color, SLONG r, SLONG g, SLONG b);
extern	void	_oper_eclipse_color_555(UHINT *fore_color, SLONG r, SLONG g, SLONG b);
extern  void    _oper_blue_alpha_color_555(UHINT *fore_color, UHINT *back_color, SLONG alpha);


// VGA 655 FUNCTIONS
extern  UHINT   _true2hi_655(ULONG c);
extern  ULONG   _hi2true_655(UHINT c);
extern  UCHR    _get_r_655(UHINT c);
extern  UCHR    _get_g_655(UHINT c);
extern  UCHR    _get_b_655(UHINT c);
extern  void    _set_r_655(UHINT *c,UCHR r);
extern  void    _set_g_655(UHINT *c,UCHR g);
extern  void    _set_b_655(UHINT *c,UCHR b);
extern  UHINT   _rgb2hi_655(UCHR r,UCHR g,UCHR b);
extern  void    _hi2rgb_655(UHINT c,UCHR *r,UCHR *g,UCHR *b);
extern  UHINT   _fff2hi_655(UHINT c);
extern  UHINT   _hi2fff_655(UHINT c);
extern  void    _oper_alpha_color_655(UHINT *fore_color,UHINT *back_color,SLONG alpha);
extern  void    _oper_additive_color_655(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_gray_color_655(UHINT *fore_color,UHINT*back_color);
extern  void    _oper_subtractive_color_655(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_minimum_color_655(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_maximum_color_655(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_half_color_655(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_dark_color_655(UHINT *fore_color, SLONG dark);
extern  void    _oper_red_color_655(UHINT *fore_color, UHINT *back_color);
extern  void    _oper_green_color_655(UHINT *fore_color, UHINT *back_color);
extern  void    _oper_blue_color_655(UHINT *fore_color, UHINT *back_color);
extern  void    _oper_yellow_color_655(UHINT *fore_color, UHINT *back_color);
extern  void    _oper_merge_color_655(UHINT *fore_color, UHINT *back_color, SLONG alpha);
extern  void    _oper_partial_color_655(UHINT *fore_color, UHINT *back_color, SLONG alpha);
extern  void    _oper_complement_color_655(UHINT *fore_color, UHINT *back_color, SLONG alpha);
extern	void	_oper_adulterate_color_655(UHINT *fore_color, SLONG r, SLONG g, SLONG b);
extern	void	_oper_eclipse_color_655(UHINT *fore_color, SLONG r, SLONG g, SLONG b);
extern  void    _oper_blue_alpha_color_655(UHINT *fore_color, UHINT *back_color, SLONG alpha);



// VGA 565 FUNCTIONS
extern  UHINT   _true2hi_565(ULONG c);
extern  ULONG   _hi2true_565(UHINT c);
extern  UCHR    _get_r_565(UHINT c);
extern  UCHR    _get_g_565(UHINT c);
extern  UCHR    _get_b_565(UHINT c);
extern  void    _set_r_565(UHINT *c,UCHR r);
extern  void    _set_g_565(UHINT *c,UCHR g);
extern  void    _set_b_565(UHINT *c,UCHR b);
extern  UHINT   _rgb2hi_565(UCHR r,UCHR g,UCHR b);
extern  void    _hi2rgb_565(UHINT c,UCHR *r,UCHR *g,UCHR *b);
extern  UHINT   _fff2hi_565(UHINT c);
extern  UHINT   _hi2fff_565(UHINT c);
extern  void    _oper_alpha_color_565(UHINT *fore_color,UHINT *back_color,SLONG alpha);
extern  void    _oper_additive_color_565(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_gray_color_565(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_subtractive_color_565(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_minimum_color_565(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_maximum_color_565(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_half_color_565(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_dark_color_565(UHINT *fore_color, SLONG dark);
extern  void    _oper_red_color_565(UHINT *fore_color, UHINT *back_color);
extern  void    _oper_green_color_565(UHINT *fore_color, UHINT *back_color);
extern  void    _oper_blue_color_565(UHINT *fore_color, UHINT *back_color);
extern  void    _oper_yellow_color_565(UHINT *fore_color, UHINT *back_color);
extern  void    _oper_merge_color_565(UHINT *fore_color, UHINT *back_color, SLONG alpha);
extern  void    _oper_partial_color_565(UHINT *fore_color, UHINT *back_color, SLONG alpha);
extern  void    _oper_complement_color_565(UHINT *fore_color, UHINT *back_color, SLONG alpha);
extern	void	_oper_adulterate_color_565(UHINT *fore_color, SLONG r, SLONG g, SLONG b);
extern	void	_oper_eclipse_color_565(UHINT *fore_color, SLONG r, SLONG g, SLONG b);
extern  void    _oper_blue_alpha_color_565(UHINT *fore_color, UHINT *back_color, SLONG alpha);



// VGA 556 FUNCTIONS
extern  UHINT   _true2hi_556(ULONG c);
extern  ULONG   _hi2true_556(UHINT c);
extern  UCHR    _get_r_556(UHINT c);
extern  UCHR    _get_g_556(UHINT c);
extern  UCHR    _get_b_556(UHINT c);
extern  void    _set_r_556(UHINT *c,UCHR r);
extern  void    _set_g_556(UHINT *c,UCHR g);
extern  void    _set_b_556(UHINT *c,UCHR b);
extern  UHINT   _rgb2hi_556(UCHR r,UCHR g,UCHR b);
extern  void    _hi2rgb_556(UHINT c,UCHR *r,UCHR *g,UCHR *b);
extern  UHINT   _fff2hi_556(UHINT c);
extern  UHINT   _hi2fff_556(UHINT c);
extern  void    _oper_alpha_color_556(UHINT *fore_color,UHINT *back_color,SLONG alpha);
extern  void    _oper_additive_color_556(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_gray_color_556(UHINT *fore_color, UHINT *back_color);
extern  void    _oper_subtractive_color_556(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_minimum_color_556(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_maximum_color_556(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_half_color_556(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_dark_color_556(UHINT *fore_color, SLONG dark);
extern  void    _oper_red_color_556(UHINT *fore_color, UHINT *back_color);
extern  void    _oper_green_color_556(UHINT *fore_color, UHINT *back_color);
extern  void    _oper_blue_color_556(UHINT *fore_color, UHINT *back_color);
extern  void    _oper_yellow_color_556(UHINT *fore_color, UHINT *back_color);
extern  void    _oper_merge_color_556(UHINT *fore_color, UHINT *back_color, SLONG alpha);
extern  void    _oper_partial_color_556(UHINT *fore_color, UHINT *back_color, SLONG alpha);
extern  void    _oper_complement_color_556(UHINT *fore_color, UHINT *back_color, SLONG alpha);
extern	void	_oper_adulterate_color_556(UHINT *fore_color, SLONG r, SLONG g, SLONG b);
extern	void	_oper_eclipse_color_556(UHINT *fore_color, SLONG r, SLONG g, SLONG b);
extern  void    _oper_blue_alpha_color_556(UHINT *fore_color, UHINT *back_color, SLONG alpha);



// VGA ANY FUNCTIONS
extern  UHINT   _true2hi_any(ULONG c);
extern  ULONG   _hi2true_any(UHINT c);
extern  UCHR    _get_r_any(UHINT c);
extern  UCHR    _get_g_any(UHINT c);
extern  UCHR    _get_b_any(UHINT c);
extern  void    _set_r_any(UHINT *c,UCHR r);
extern  void    _set_g_any(UHINT *c,UCHR g);
extern  void    _set_b_any(UHINT *c,UCHR b);
extern  UHINT   _rgb2hi_any(UCHR r,UCHR g,UCHR b);
extern  void    _hi2rgb_any(UHINT c,UCHR *r,UCHR *g,UCHR *b);
extern  UHINT   _fff2hi_any(UHINT c);
extern  UHINT   _hi2fff_any(UHINT c);
extern  void    _oper_alpha_color_any(UHINT *fore_color,UHINT *back_color,SLONG alpha);
extern  void    _oper_additive_color_any(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_gray_color_any(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_subtractive_color_any(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_minimum_color_any(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_maximum_color_any(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_half_color_any(UHINT *fore_color,UHINT *back_color);
extern  void    _oper_dark_color_any(UHINT *fore_color, SLONG dark);
extern  void    _oper_red_color_any(UHINT *fore_color, UHINT *back_color);
extern  void    _oper_green_color_any(UHINT *fore_color, UHINT *back_color);
extern  void    _oper_blue_color_any(UHINT *fore_color, UHINT *back_color);
extern  void    _oper_yellow_color_any(UHINT *fore_color, UHINT *back_color);
extern  void    _oper_merge_color_any(UHINT *fore_color, UHINT *back_color, SLONG alpha);
extern  void    _oper_partial_color_any(UHINT *fore_color, UHINT *back_color, SLONG alpha);
extern  void    _oper_complement_color_any(UHINT *fore_color, UHINT *back_color, SLONG alpha);
extern	void	_oper_adulterate_color_any(UHINT *fore_color, SLONG r, SLONG g, SLONG b);
extern	void	_oper_eclipse_color_any(UHINT *fore_color, SLONG r, SLONG g, SLONG b);
extern  void    _oper_blue_alpha_color_any(UHINT *fore_color, UHINT *back_color, SLONG alpha);





#endif//XVGA_H_INCLUDE
