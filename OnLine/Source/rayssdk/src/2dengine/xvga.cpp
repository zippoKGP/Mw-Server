/*
**      XVGA.CPP
**      XVGA functions for 64K(16Bits) color.
**
**      PS:
**      gray = ( red * 30 + green * 59 + blue * 11 ) / 100 ;
**
**      ZJian,2000.9.1.
**          Created.
**      ZJian,2001.1.10.
**          Optimized some functions for asm.
**      ZJian,2001.4.24.
**          Optimized oper_half_color().
**
*/
#include "rays.h"
#include "xvga.h"


VGA_INFO    vga_info;



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VGA 555 FUNCTIONS 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
UHINT   _true2hi_555(ULONG c)
{
    return (UHINT)(((c>>9)&R_MASK_555) | ((c>>6)&G_MASK_555) | ((c>>3)&B_MASK_555));
}

ULONG   _hi2true_555(UHINT c)
{
    return (ULONG)((((ULONG)c<<9)&0xf80000) | (((ULONG)c<<6)&0xf800) | (((ULONG)c<<3)&0xf8));
}

UCHR    _get_r_555(UHINT c)
{
    return (UCHR)((c&R_MASK_555)>>7);
}

UCHR    _get_g_555(UHINT c)
{
    return (UCHR)((c&G_MASK_555)>>2);
}

UCHR    _get_b_555(UHINT c)
{
    return (UCHR)((c&B_MASK_555)<<3);
}

void    _set_r_555(UHINT *c,UCHR r)
{
    (*c) &= ~R_MASK_555;
    (*c) |= ((UHINT)r << 7) & R_MASK_555;
}

void    _set_g_555(UHINT *c,UCHR g)
{
    (*c) &= ~G_MASK_555;
    (*c) |= ((UHINT)g << 2) & G_MASK_555;
}

void    _set_b_555(UHINT *c,UCHR b)
{
    (*c) &= ~B_MASK_555;
    (*c) |= ((UHINT)b >> 3) & B_MASK_555;
}


UHINT   _rgb2hi_555(UCHR r,UCHR g,UCHR b)
{
    return (UHINT)( (((UHINT)r<<7)&R_MASK_555) | (((UHINT)g<<2)&G_MASK_555) | (((UHINT)b>>3)&B_MASK_555) );
}

void    _hi2rgb_555(UHINT c,UCHR *r,UCHR *g,UCHR *b)
{
    *r=(UCHR)((c&R_MASK_555)>>7);
    *g=(UCHR)((c&G_MASK_555)>>2);
    *b=(UCHR)((c&B_MASK_555)<<3);
}

UHINT   _fff2hi_555(UHINT c)
{
    return c;
}


UHINT   _hi2fff_555(UHINT c)
{
    return c;
}


void    _oper_alpha_color_555(UHINT *fore_color,UHINT *back_color,SLONG alpha)
{
    /* C Version
    UCHR fr,fg,fb;
    UCHR br,bg,bb;
    UCHR r,g,b;

    _hi2rgb_555(*fore_color,&fr,&fg,&fb);
    _hi2rgb_555(*back_color,&br,&bg,&bb);
    r = (UCHR)( br + (fr-br)*alpha/255 );
    g = (UCHR)( bg + (fg-bg)*alpha/255 );
    b = (UCHR)( bb + (fb-bb)*alpha/255 );
    *back_color = _rgb2hi_555(r,g,b);
    */
    alpha >>= 3;
    _asm
    {
        //把 fore_color 中的内容移入 ax
        //把 back_color 中的内容移入 bx
        mov edx, fore_color
        mov ecx, back_color
        xor eax, eax
        xor ebx, ebx
        mov ax, WORD PTR [edx]
        mov bx, WORD PTR [ecx]

        //准备 eax 成 _fg_fr_fb 格式
        //准备 ebx 成 _bg_br_bb 格式
        mov ecx, eax
        mov edx, ebx

        and eax, G_MASK_555
        and ebx, G_MASK_555
        shl eax, 16
        shl ebx, 16
        and ecx, RB_MASK_555
        and edx, RB_MASK_555
        or  eax, ecx
        or  ebx, edx

        //计算 (( f - b ) * alpha)/32 + b
        sub eax, ebx
        imul eax, alpha
        shr eax, 5
        add eax, ebx

        //把 ax 转化成 16 位 rgb
        and eax, GRB32_MASK_555
        mov cx , ax
        shr eax, 16
        or  ax , cx

        //给 back_color 赋值
        mov edx, back_color
        mov WORD PTR [edx], ax
    }
}

void    _oper_additive_color_555( UHINT *fore_color, UHINT *back_color)
{
    ULONG t,dest=0;

    t = (*fore_color & R_MASK_555) + (*back_color & R_MASK_555);
    if(t > R_MASK_555)
        t = R_MASK_555;
    dest |= t;

    t = (*fore_color & G_MASK_555) + (*back_color & G_MASK_555);
    if(t > G_MASK_555)
        t = G_MASK_555;
    dest |= t;

    t = (*fore_color & B_MASK_555) + (*back_color & B_MASK_555);
    if( t > B_MASK_555)
        t = B_MASK_555;
    dest |= t;
    *back_color = (UHINT)dest;
    /*
    UCHR    fr, fg, fb;
    UCHR    br, bg, bb;
    UHINT   r, g, b;

    _hi2rgb_555(*fore_color, &fr, &fg, &fb);
    _hi2rgb_555(*back_color, &br, &bg, &bb);
    r = fr + br;
    if(r > 255) r = 255;
    g = fg + bg;
    if(g > 255) g = 255;
    b = fb + bb;
    if(b > 255) b = 255;
    *back_color = _rgb2hi_555((UCHR)r, (UCHR)g, (UCHR)b);
    */
}


void    _oper_gray_color_555(UHINT *fore_color, UHINT *back_color)
{
    UCHR r,g,b,gray;

    _hi2rgb_555(*fore_color,&r,&g,&b);
    gray = (unsigned char)((r*30+g*59+b*11)/100);
    *back_color = _rgb2hi_555(gray,gray,gray);
}

void    _oper_subtractive_color_555(UHINT *fore_color,UHINT *back_color)
{
    SLONG t;
    ULONG dest=0;

    t=(SLONG)(*fore_color & R_MASK_555) - (SLONG)(*back_color & R_MASK_555);
    if(t<0) t=0;
    dest |= ((ULONG)t & R_MASK_555);

    t=(SLONG)(*fore_color & G_MASK_555) - (SLONG)(*back_color & G_MASK_555);
    if(t<0) t=0;
    dest |= ((ULONG)t & G_MASK_555);

    t=(SLONG)(*fore_color & B_MASK_555) - (SLONG)(*back_color & B_MASK_555);
    if(t<0) t=0;
    dest |= ((ULONG)t & B_MASK_555);

    *back_color = (UHINT)dest;
}

void    _oper_minimum_color_555(UHINT *fore_color,UHINT *back_color)
{
    ULONG dest=0;

    dest |= MIN((*fore_color & R_MASK_555),(*back_color & R_MASK_555));
    dest |= MIN((*fore_color & G_MASK_555),(*back_color & G_MASK_555));
    dest |= MIN((*fore_color & B_MASK_555),(*back_color & B_MASK_555));
    *back_color = (UHINT)dest;
}

void    _oper_maximum_color_555(UHINT *fore_color,UHINT *back_color)
{
    ULONG dest=0;

    dest |= MAX((*fore_color & R_MASK_555),(*back_color & R_MASK_555));
    dest |= MAX((*fore_color & G_MASK_555),(*back_color & G_MASK_555));
    dest |= MAX((*fore_color & B_MASK_555),(*back_color & B_MASK_555));
    *back_color = (UHINT)dest;
}

void    _oper_half_color_555(UHINT *fore_color,UHINT *back_color)
{
    /* old version
    ULONG t,dest=0;

    t=((*fore_color & R_MASK_555)+(*back_color & R_MASK_555))>>1;
    dest |= t&R_MASK_555;
    t=((*fore_color & G_MASK_555)+(*back_color & G_MASK_555))>>1;
    dest |= t&G_MASK_555;
    t=((*fore_color & B_MASK_555)+(*back_color & B_MASK_555))>>1;
    dest |= t&B_MASK_555;

    *back_color = (UHINT)dest;
    */
    (*back_color) = (UHINT) ( ( ( *fore_color & RGB_HALF_555 ) >> 1 ) + ( ( *back_color & RGB_HALF_555) >> 1 ) );
}


void    _oper_dark_color_555(UHINT *fore_color, SLONG dark)
{
    /* C version
    UCHR r,g,b;

    _hi2rgb_555( *fore_color, &r, &g, &b );
    r = (UCHR)((ULONG)(r*dark)>>5);
    g = (UCHR)((ULONG)(g*dark)>>5);
    b = (UCHR)((ULONG)(b*dark)>>5);
    *fore_color = _rgb2hi_555( r, g, b );
    */
    _asm
    {
        //把 fore_color 中的内容移入 ax
        mov edx, fore_color
        xor eax, eax
        mov ax, WORD PTR [edx]

        //准备 eax 成 _fg_fr_fb 格式
        mov ecx, eax

        and eax, G_MASK_555
        shl eax, 16
        and ecx, RB_MASK_555
        or  eax, ecx

        //计算 ( f * dark )/32
        imul eax, dark
        shr eax, 5

        //把 ax 转化成 16 位 rgb
        and eax, GRB32_MASK_555
        mov cx , ax
        shr eax, 16
        or  ax , cx

        //给 fore_color 赋值
        //mov edx, fore_color
        mov WORD PTR [edx], ax
    }
}


void    _oper_red_color_555(UHINT *fore_color, UHINT *back_color)
{
    UCHR r,g,b;

    _hi2rgb_555(*fore_color, &r, &g, &b);
    if(r + r > 0xff) r = 0xff;
    else r = r + r;
    g = (UCHR)( g >> 1 );
    b = (UCHR)( b >> 1 );
    *back_color = _rgb2hi_555( r, g, b );
}


void    _oper_green_color_555(UHINT *fore_color, UHINT *back_color)
{
    UCHR r,g,b;

    _hi2rgb_555(*fore_color, &r, &g, &b);
    if( g + g > 0xff) g = 0xff;
    else g = g + g;
    r = (UCHR)( r >> 1 );
    b = (UCHR)( b >> 1 );
    *back_color = _rgb2hi_555( r, g, b );
}


void    _oper_blue_color_555(UHINT *fore_color, UHINT *back_color)
{
    UCHR r,g,b;

    _hi2rgb_555(*fore_color, &r, &g, &b);
    if( b + b > 0xff) b = 0xff;
    else b = b + b;
    r = (UCHR)( r >> 1 );
    g = (UCHR)( g >> 1 );
    *back_color = _rgb2hi_555( r, g, b );
}


void    _oper_yellow_color_555(UHINT *fore_color, UHINT *back_color)
{
    UCHR r,g,b;

    _hi2rgb_555(*fore_color, &r, &g, &b);
    if(r + r > 0xff) r = 0xff;
    else r = r + r;
    if( g + g > 0xff) g = 0xff;
    else g = g + g;
    b = (UCHR)( b >> 1 );
    *back_color = _rgb2hi_555( r, g, b );
}


void    _oper_merge_color_555(UHINT *fore_color, UHINT *back_color, SLONG alpha)
{
    UCHR fr,fg,fb;
    UCHR br,bg,bb;
    UCHR r,g,b;
    SLONG lr, lg, lb;

    _hi2rgb_555(*fore_color, &fr, &fg, &fb);
    _hi2rgb_555(*back_color, &br, &bg, &bb);
    lr = ( br + fr * alpha / 255 );
    lg = ( bg + fg * alpha / 255 );
    lb = ( bb + fb * alpha / 255 );
    if(lr > 0xff) r = 0xff;
    else r = (UCHR) lr;

    if(lg > 0xff) g = 0xff;
    else g = (UCHR) lg;

    if(lb > 0xff) b = 0xff;
    else b = (UCHR) lb;

    *back_color = _rgb2hi_555(r,g,b);
}


void    _oper_partial_color_555(UHINT *fore_color, UHINT *back_color, SLONG alpha)
{
    UCHR fr,fg,fb;
    UCHR r,g,b;

    _hi2rgb_555(*fore_color,&fr,&fg,&fb);
    r = (UCHR)( fr * alpha / 255 );
    g = (UCHR)( fg * alpha / 255 );
    b = (UCHR)( fb * alpha / 255 );
    *back_color = _rgb2hi_555(r,g,b);
}


void    _oper_complement_color_555(UHINT *fore_color, UHINT *back_color, SLONG alpha)
{
    UCHR fr,fg,fb;
    UCHR r,g,b;

    _hi2rgb_555(*fore_color,&fr,&fg,&fb);
    r = (UCHR)( fr - fr * alpha / 255 );
    g = (UCHR)( fg - fg * alpha / 255 );
    b = (UCHR)( fb - fb * alpha / 255 );
    *back_color = _rgb2hi_555(r,g,b);
}


void	_oper_adulterate_color_555(UHINT *fore_color, SLONG r, SLONG g, SLONG b)
{
	UCHR	fr, fg, fb;
	SLONG	nr, ng, nb;

	_hi2rgb_555(*fore_color, &fr, &fg, &fb);
	nr = fr + r;
	fr = (nr > 0xff) ? 0xff : (UCHR)nr;
	ng = fg + g;
	fg = (ng > 0xff) ? 0xff : (UCHR)ng;
	nb = fb + b;
	fb = (nb > 0xff) ? 0xff : (UCHR)nb;
	*fore_color = _rgb2hi_555(fr, fg, fb);
}


void	_oper_eclipse_color_555(UHINT *fore_color, SLONG r, SLONG g, SLONG b)
{
	UCHR	fr, fg, fb;
	SLONG	nr, ng, nb;

	_hi2rgb_555(*fore_color, &fr, &fg, &fb);
	nr = fr - r;
	fr = (nr < 0x00) ? 0x00 : (UCHR)nr;
	ng = fg - g;
	fg = (ng < 0x00) ? 0x00 : (UCHR)ng;
	nb = fb - b;
	fb = (nb < 0x00) ? 0x00 : (UCHR)nb;
	*fore_color = _rgb2hi_555(fr, fg, fb);
}


void    _oper_blue_alpha_color_555(UHINT *fore_color, UHINT *back_color, SLONG alpha)
{
    UCHR    fr,fg,fb;
    UCHR    br,bg,bb;
    SLONG   nr,ng,nb;
    UCHR    r,g,b;

    _hi2rgb_555(*fore_color,&fr,&fg,&fb);
    _hi2rgb_555(*back_color,&br,&bg,&bb);
    nr = fr >> 1;
    ng = fg >> 1;
    nb = min((SLONG)fb << 1, 255);
    r = (UCHR)( br + (nr-br)*alpha/255 );
    g = (UCHR)( bg + (ng-bg)*alpha/255 );
    b = (UCHR)( bb + (nb-bb)*alpha/255 );
    *back_color = _rgb2hi_555(r,g,b);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VGA 655 FUNCTIONS 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
UHINT   _true2hi_655(ULONG c)
{
    return (UHINT)(((c>>8)&R_MASK_655) | ((c>>6)&G_MASK_655) | ((c>>3)&B_MASK_655));
}

ULONG   _hi2true_655(UHINT c)
{
    return (ULONG)((((ULONG)c<<8)&0xfc0000) | (((ULONG)c<<6)&0xf800) | (((ULONG)c<<3)&0xf8));
}

UCHR    _get_r_655(UHINT c)
{
    return (UCHR)((c&R_MASK_655)>>8);
}

UCHR    _get_g_655(UHINT c)
{
    return (UCHR)((c&G_MASK_555)>>2);
}

UCHR    _get_b_655(UHINT c)
{
    return (UCHR)((c&B_MASK_655)<<3);
}

void    _set_r_655(UHINT *c,UCHR r)
{
    (*c) &= ~R_MASK_655;
    (*c) |= ((UHINT)r << 8) & R_MASK_655;
}

void    _set_g_655(UHINT *c,UCHR g)
{
    (*c) &= ~G_MASK_655;
    (*c) |= ((UHINT)g << 2) & G_MASK_655;
}

void    _set_b_655(UHINT *c,UCHR b)
{
    (*c) &= ~B_MASK_655;
    (*c) |= ((UHINT)b << 3) & B_MASK_655;
}


UHINT   _rgb2hi_655(UCHR r,UCHR g,UCHR b)
{
    return (UHINT)( (((UHINT)r<<8)&R_MASK_655) | (((UHINT)g<<2)&G_MASK_655) | (((UHINT)b>>3)&B_MASK_655) );
}

void    _hi2rgb_655(UHINT c,UCHR *r,UCHR *g,UCHR *b)
{
    *r=(UCHR)((c&R_MASK_655)>>8);
    *g=(UCHR)((c&G_MASK_655)>>2);
    *b=(UCHR)((c&B_MASK_655)<<3);
}

UHINT   _fff2hi_655(UHINT c)
{
    UCHR r,g,b;
    _hi2rgb_555(c,&r,&g,&b);
    return _rgb2hi_655(r,g,b);
}

UHINT   _hi2fff_655(UHINT c)
{
    UCHR r,g,b;
    _hi2rgb_655(c,&r,&g,&b);
    return _rgb2hi_555(r,g,b);
}


void    _oper_alpha_color_655(UHINT *fore_color,UHINT *back_color,SLONG alpha)
{
    /* C Version
    UCHR fr,fg,fb;
    UCHR br,bg,bb;
    UCHR r,g,b;

    _hi2rgb_655(*fore_color,&fr,&fg,&fb);
    _hi2rgb_655(*back_color,&br,&bg,&bb);
    r = (UCHR)( br + (fr-br)*alpha/255 );
    g = (UCHR)( bg + (fg-bg)*alpha/255 );
    b = (UCHR)( bb + (fb-bb)*alpha/255 );
    *back_color = _rgb2hi_655(r,g,b);
    */
    alpha >>= 3;
    _asm
    {
        //把 fore_color 中的内容移入 ax
        //把 back_color 中的内容移入 bx
        mov edx, fore_color
        mov ecx, back_color
        xor eax, eax
        xor ebx, ebx
        mov ax, WORD PTR [edx]
        mov bx, WORD PTR [ecx]

        //准备 eax 成 _fg_fr_fb 格式
        //准备 ebx 成 _bg_br_bb 格式
        mov ecx, eax
        mov edx, ebx

        and eax, G_MASK_655
        and ebx, G_MASK_655
        shl eax, 16
        shl ebx, 16
        and ecx, RB_MASK_655
        and edx, RB_MASK_655
        or  eax, ecx
        or  ebx, edx

        //计算 (( f - b ) * alpha)/32 + b
        sub eax, ebx
        imul eax, alpha
        shr eax, 5
        add eax, ebx

        //把 ax 转化成 16 位 rgb
        and eax, GRB32_MASK_655
        mov cx , ax
        shr eax, 16
        or  ax , cx

        //给 back_color 赋值
        mov edx, back_color
        mov WORD PTR [edx], ax
    }
}

void    _oper_additive_color_655( UHINT *fore_color, UHINT *back_color)
{
    ULONG t,dest=0;

    t = (*fore_color & R_MASK_655) + (*back_color & R_MASK_655);
    if(t > R_MASK_655)
        t = R_MASK_655;
    dest |= t;

    t = (*fore_color & G_MASK_655) + (*back_color & G_MASK_655);
    if(t > G_MASK_655)
        t = G_MASK_655;
    dest |= t;

    t = (*fore_color & B_MASK_655) + (*back_color & B_MASK_655);
    if( t > B_MASK_655)
        t = B_MASK_655;
    dest |= t;
    *back_color = (UHINT)dest;
    /*
    UCHR    fr, fg, fb;
    UCHR    br, bg, bb;
    UHINT   r, g, b;

    _hi2rgb_655(*fore_color, &fr, &fg, &fb);
    _hi2rgb_655(*back_color, &br, &bg, &bb);
    r = fr + br;
    if(r > 255) r = 255;
    g = fg + bg;
    if(g > 255) g = 255;
    b = fb + bb;
    if(b > 255) b = 255;
    *back_color = _rgb2hi_655((UCHR)r, (UCHR)g, (UCHR)b);
    */
}


void    _oper_gray_color_655(UHINT *fore_color, UHINT *back_color)
{
    UCHR r,g,b,gray;

    _hi2rgb_655(*fore_color,&r,&g,&b);
    gray=(unsigned char)((r*30+g*59+b*11)/100);
    *back_color = _rgb2hi_655(gray,gray,gray);
}

void    _oper_subtractive_color_655(UHINT *fore_color,UHINT *back_color)
{
    SLONG t;
    ULONG dest=0;

    t=(SLONG)(*fore_color & R_MASK_655) - (SLONG)(*back_color & R_MASK_655);
    if(t<0) t=0;
    dest |= ((ULONG)t & R_MASK_655);

    t=(SLONG)(*fore_color & G_MASK_655) - (SLONG)(*back_color & G_MASK_655);
    if(t<0) t=0;
    dest |= ((ULONG)t & G_MASK_655);

    t=(SLONG)(*fore_color & B_MASK_655) - (SLONG)(*back_color & B_MASK_655);
    if(t<0) t=0;
    dest |= ((ULONG)t & B_MASK_655);

    *back_color = (UHINT)dest;
}

void    _oper_minimum_color_655(UHINT *fore_color,UHINT *back_color)
{
    ULONG dest=0;

    dest |= MIN((*fore_color & R_MASK_655),(*back_color & R_MASK_655));
    dest |= MIN((*fore_color & G_MASK_655),(*back_color & G_MASK_655));
    dest |= MIN((*fore_color & B_MASK_655),(*back_color & B_MASK_655));
    *back_color = (UHINT)dest;
}

void    _oper_maximum_color_655(UHINT *fore_color,UHINT *back_color)
{
    ULONG dest=0;

    dest |= MAX((*fore_color & R_MASK_655),(*back_color & R_MASK_655));
    dest |= MAX((*fore_color & G_MASK_655),(*back_color & G_MASK_655));
    dest |= MAX((*fore_color & B_MASK_655),(*back_color & B_MASK_655));
    *back_color = (UHINT)dest;
}

void    _oper_half_color_655(UHINT *fore_color,UHINT *back_color)
{
    /* old version
    ULONG t,dest=0;

    t=((*fore_color & R_MASK_655)+(*back_color & R_MASK_655))>>1;
    dest |= t&R_MASK_655;
    t=((*fore_color & G_MASK_655)+(*back_color & G_MASK_655))>>1;
    dest |= t&G_MASK_655;
    t=((*fore_color & B_MASK_655)+(*back_color & B_MASK_655))>>1;
    dest |= t&B_MASK_655;

    *back_color = (UHINT)dest;
    */
    (*back_color) = (UHINT)( ( ( *fore_color & RGB_HALF_655 ) >> 1 ) + ( ( *back_color & RGB_HALF_655) >> 1 ) );
}


void    _oper_dark_color_655(UHINT *fore_color, SLONG dark)
{
    /* C version
    UCHR r,g,b;

    _hi2rgb_655( *fore_color, &r, &g, &b );
    r = (UCHR)((ULONG)(r*dark)>>5);
    g = (UCHR)((ULONG)(g*dark)>>5);
    b = (UCHR)((ULONG)(b*dark)>>5);
    *fore_color = _rgb2hi_655( r, g, b );
    */
    _asm
    {
        //把 fore_color 中的内容移入 ax
        mov edx, fore_color
        xor eax, eax
        mov ax, WORD PTR [edx]

        //准备 eax 成 _fg_fr_fb 格式
        mov ecx, eax

        and eax, G_MASK_655
        shl eax, 16
        and ecx, RB_MASK_655
        or  eax, ecx

        //计算 ( f * dark )/32
        imul eax, dark
        shr eax, 5

        //把 ax 转化成 16 位 rgb
        and eax, GRB32_MASK_655
        mov cx , ax
        shr eax, 16
        or  ax , cx

        //给 fore_color 赋值
        //mov edx, fore_color
        mov WORD PTR [edx], ax
    }
}


void    _oper_red_color_655(UHINT *fore_color, UHINT *back_color)
{
    UCHR r,g,b;

    _hi2rgb_655(*fore_color, &r, &g, &b);
    if(r + r > 0xff) r = 0xff;
    else r = r + r;
    g = (UCHR)( g >> 1 );
    b = (UCHR)( b >> 1 );
    *back_color = _rgb2hi_655( r, g, b );
}


void    _oper_green_color_655(UHINT *fore_color, UHINT *back_color)
{
    UCHR r,g,b;

    _hi2rgb_655(*fore_color, &r, &g, &b);
    if( g + g > 0xff) g = 0xff;
    else g = g + g;
    r = (UCHR)( r >> 1 );
    b = (UCHR)( b >> 1 );
    *back_color = _rgb2hi_655( r, g, b );
}


void    _oper_blue_color_655(UHINT *fore_color, UHINT *back_color)
{
    UCHR r,g,b;

    _hi2rgb_655(*fore_color, &r, &g, &b);
    if( b + b > 0xff) b = 0xff;
    else b = b + b;
    r = (UCHR)( r >> 1 );
    g = (UCHR)( g >> 1 );
    *back_color = _rgb2hi_655( r, g, b );
}


void    _oper_yellow_color_655(UHINT *fore_color, UHINT *back_color)
{
    UCHR r,g,b;

    _hi2rgb_655(*fore_color, &r, &g, &b);
    if(r + r > 0xff) r = 0xff;
    else r = r + r;
    if( g + g > 0xff) g = 0xff;
    else g = g + g;
    b = (UCHR)( b >> 1 );
    *back_color = _rgb2hi_655( r, g, b );
}


void    _oper_merge_color_655(UHINT *fore_color, UHINT *back_color, SLONG alpha)
{
    UCHR fr,fg,fb;
    UCHR br,bg,bb;
    UCHR r,g,b;
    SLONG lr, lg, lb;

    _hi2rgb_655(*fore_color, &fr, &fg, &fb);
    _hi2rgb_655(*back_color, &br, &bg, &bb);
    lr = ( br + fr * alpha / 255 );
    lg = ( bg + fg * alpha / 255 );
    lb = ( bb + fb * alpha / 255 );
    if(lr > 0xff) r = 0xff;
    else r = (UCHR) lr;

    if(lg > 0xff) g = 0xff;
    else g = (UCHR) lg;

    if(lb > 0xff) b = 0xff;
    else b = (UCHR) lb;

    *back_color = _rgb2hi_655(r,g,b);
}


void    _oper_partial_color_655(UHINT *fore_color, UHINT *back_color, SLONG alpha)
{
    UCHR fr,fg,fb;
    UCHR r,g,b;

    _hi2rgb_655(*fore_color,&fr,&fg,&fb);
    r = (UCHR)( fr * alpha / 255 );
    g = (UCHR)( fg * alpha / 255 );
    b = (UCHR)( fb * alpha / 255 );
    *back_color = _rgb2hi_655(r,g,b);
}


void    _oper_complement_color_655(UHINT *fore_color, UHINT *back_color, SLONG alpha)
{
    UCHR fr,fg,fb;
    UCHR r,g,b;

    _hi2rgb_655(*fore_color,&fr,&fg,&fb);
    r = (UCHR)( fr - fr * alpha / 255 );
    g = (UCHR)( fg - fg * alpha / 255 );
    b = (UCHR)( fb - fb * alpha / 255 );
    *back_color = _rgb2hi_655(r,g,b);
}

void	_oper_adulterate_color_655(UHINT *fore_color, SLONG r, SLONG g, SLONG b)
{
	UCHR	fr, fg, fb;
	SLONG	nr, ng, nb;

	_hi2rgb_655(*fore_color, &fr, &fg, &fb);
	nr = fr + r;
	fr = (nr > 0xff) ? 0xff : (UCHR)nr;
	ng = fg + g;
	fg = (ng > 0xff) ? 0xff : (UCHR)ng;
	nb = fb + b;
	fb = (nb > 0xff) ? 0xff : (UCHR)nb;
	*fore_color = _rgb2hi_655(fr, fg, fb);
}


void	_oper_eclipse_color_655(UHINT *fore_color, SLONG r, SLONG g, SLONG b)
{
	UCHR	fr, fg, fb;
	SLONG	nr, ng, nb;

	_hi2rgb_655(*fore_color, &fr, &fg, &fb);
	nr = fr - r;
	fr = (nr < 0x00) ? 0x00 : (UCHR)nr;
	ng = fg - g;
	fg = (ng < 0x00) ? 0x00 : (UCHR)ng;
	nb = fb - b;
	fb = (nb < 0x00) ? 0x00 : (UCHR)nb;
	*fore_color = _rgb2hi_655(fr, fg, fb);
}


void    _oper_blue_alpha_color_655(UHINT *fore_color, UHINT *back_color, SLONG alpha)
{
    UCHR    fr,fg,fb;
    UCHR    br,bg,bb;
    SLONG   nr,ng,nb;
    UCHR    r,g,b;

    _hi2rgb_655(*fore_color,&fr,&fg,&fb);
    _hi2rgb_655(*back_color,&br,&bg,&bb);
    nr = fr >> 1;
    ng = fg >> 1;
    nb = min((SLONG)fb << 1, 255);
    r = (UCHR)( br + (nr-br)*alpha/255 );
    g = (UCHR)( bg + (ng-bg)*alpha/255 );
    b = (UCHR)( bb + (nb-bb)*alpha/255 );
    *back_color = _rgb2hi_655(r,g,b);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VGA 565 FUNCTIONS 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
UHINT   _true2hi_565(ULONG c)
{
    return (UHINT)(((c>>8)&0xf800) | ((c>>5)&0x07e0) | ((c>>3)&0x001f));
}

ULONG   _hi2true_565(UHINT c)
{
    return (ULONG)((((ULONG)c<<8)&0xf80000) | (((ULONG)c<<5)&0xfc00) | (((ULONG)c<<3)&0xf8));
}

UCHR    _get_r_565(UHINT c)
{
    return (UCHR)((c&R_MASK_565)>>8);
}

UCHR    _get_g_565(UHINT c)
{
    return (UCHR)((c&G_MASK_565)>>3);
}

UCHR    _get_b_565(UHINT c)
{
    return (UCHR)((c&B_MASK_565)<<3);
}

void    _set_r_565(UHINT *c,UCHR r)
{
    (*c) &= ~R_MASK_565;
    (*c) |= ((UHINT)r << 8) & R_MASK_565;
}

void    _set_g_565(UHINT *c,UCHR g)
{
    (*c) &= ~G_MASK_565;
    (*c) |= ((UHINT)g << 3) & G_MASK_565;
}

void    _set_b_565(UHINT *c,UCHR b)
{
    (*c) &= ~B_MASK_565;
    (*c) |= ((UHINT)b >> 3) & B_MASK_565;
}


UHINT   _rgb2hi_565(UCHR r,UCHR g,UCHR b)
{
    return (UHINT)( (((UHINT)r<<8)&R_MASK_565) | (((UHINT)g<<3)&G_MASK_565) | (((UHINT)b>>3)&B_MASK_565) );
}

void    _hi2rgb_565(UHINT c,UCHR *r,UCHR *g,UCHR *b)
{
    *r=(UCHR)((c&R_MASK_565)>>8);
    *g=(UCHR)((c&G_MASK_565)>>3);
    *b=(UCHR)((c&B_MASK_565)<<3);
}

UHINT   _fff2hi_565(UHINT c)
{
    UCHR r,g,b;
    _hi2rgb_555(c,&r,&g,&b);
    return _rgb2hi_565(r,g,b);
}

UHINT   _hi2fff_565(UHINT c)
{
    UCHR r,g,b;
    _hi2rgb_565(c,&r,&g,&b);
    return _rgb2hi_555(r,g,b);
}


void    _oper_alpha_color_565(UHINT *fore_color,UHINT *back_color,SLONG alpha)
{

    /*  C Version
    UCHR fr,fg,fb;
    UCHR br,bg,bb;
    UCHR r,g,b;

    _hi2rgb_565(*fore_color,&fr,&fg,&fb);
    _hi2rgb_565(*back_color,&br,&bg,&bb);
    r = (UCHR)( br + (fr-br)*alpha/255 );
    g = (UCHR)( bg + (fg-bg)*alpha/255 );
    b = (UCHR)( bb + (fb-bb)*alpha/255 );
    *back_color = _rgb2hi_565(r,g,b);
    */
 //   *back_color = *fore_color;
	
	alpha >>= 3;
    _asm
    {
        //把 fore_color 中的内容移入 ax
        //把 back_color 中的内容移入 bx
//mmx版本
/*
        mov edx, fore_color
        mov ecx, back_color
        xor eax, eax
        xor ebx, ebx
        mov ax, WORD PTR [edx]
        mov bx, WORD PTR [ecx]	//eax中的数据是fore，ebx中是back
		movd mm0, eax
		movd mm1, ebx
		psslq mm0, 32
		paddd mm0, mm1
		movq mm1, mm0	//备份一下数据
		//对mm0中的数据用mask
		pand mm0,  0x000007e0000007e0	//G_MASK_565
		pslld mm0, 16
		pand mm1,  0x0000f81f0000f81f	//RB_MASK_565
		por mm0, mm1		//mm1中的数据已经没有用了
		movq mm1, mm0		//mm1是mm0的备份
*/		
	

        mov edx, fore_color
        mov ecx, back_color
        xor eax, eax
        xor ebx, ebx
        mov ax, WORD PTR [edx]
        mov bx, WORD PTR [ecx]
		

        //准备 eax 成 _fg_fr_fb 格式
        //准备 ebx 成 _bg_br_bb 格式
        mov ecx, eax
        mov edx, ebx	//备份 两个颜色 到ecx和edx

        and eax, G_MASK_565
        and ebx, G_MASK_565
        shl eax, 16
        shl ebx, 16
        and ecx, RB_MASK_565
        and edx, RB_MASK_565
        or  eax, ecx
        or  ebx, edx		//这一段是把16位变成32位

        //计算 (( f - b ) * alpha)/32 + b
        sub eax, ebx
        imul eax, alpha
        shr eax, 5
        add eax, ebx

        //把 ax 转化成 16 位 rgb
        and eax, GRB32_MASK_565
        mov cx , ax
        shr eax, 16
        or  ax , cx

        //给 back_color 赋值
        mov edx, back_color
        mov WORD PTR [edx], ax

    }

}


void    _oper_additive_color_565( UHINT *fore_color, UHINT *back_color)
{
    ULONG t,dest=0;

    t = (*fore_color & R_MASK_565) + (*back_color & R_MASK_565);
    if(t > R_MASK_565)
        t = R_MASK_565;
    dest |= t;

    t = (*fore_color & G_MASK_565) + (*back_color & G_MASK_565);
    if(t > G_MASK_565)
        t = G_MASK_565;
    dest |= t;

    t = (*fore_color & B_MASK_565) + (*back_color & B_MASK_565);
    if( t > B_MASK_565)
        t = B_MASK_565;
    dest |= t;
    *back_color = (UHINT)dest;
    /*
    UCHR    fr, fg, fb;
    UCHR    br, bg, bb;
    UHINT   r, g, b;

    _hi2rgb_565(*fore_color, &fr, &fg, &fb);
    _hi2rgb_565(*back_color, &br, &bg, &bb);
    r = fr + br;
    if(r > 255) r = 255;
    g = fg + bg;
    if(g > 255) g = 255;
    b = fb + bb;
    if(b > 255) b = 255;
    *back_color = _rgb2hi_565((UCHR)r, (UCHR)g, (UCHR)b);
    */
}


void    _oper_gray_color_565(UHINT *fore_color, UHINT *back_color)
{
    UCHR r,g,b,gray;

    _hi2rgb_565(*fore_color,&r,&g,&b);
    gray=(unsigned char)((r*30+g*59+b*11)/100);
    *back_color = _rgb2hi_565(gray,gray,gray);
}


void    _oper_subtractive_color_565(UHINT *fore_color,UHINT *back_color)
{
    SLONG t;
    ULONG dest=0;

    t=(SLONG)(*fore_color & R_MASK_565) - (SLONG)(*back_color & R_MASK_565);
    if(t<0) t=0;
    dest |= ((ULONG)t & R_MASK_565);

    t=(SLONG)(*fore_color & G_MASK_565) - (SLONG)(*back_color & G_MASK_565);
    if(t<0) t=0;
    dest |= ((ULONG)t & G_MASK_565);

    t=(SLONG)(*fore_color & B_MASK_565) - (SLONG)(*back_color & B_MASK_565);
    if(t<0) t=0;
    dest |= ((ULONG)t & B_MASK_565);

    *back_color = (UHINT)dest;
}

void    _oper_minimum_color_565(UHINT *fore_color,UHINT *back_color)
{
    ULONG dest=0;

    dest |= MIN((*fore_color & R_MASK_565),(*back_color & R_MASK_565));
    dest |= MIN((*fore_color & G_MASK_565),(*back_color & G_MASK_565));
    dest |= MIN((*fore_color & B_MASK_565),(*back_color & B_MASK_565));
    *back_color = (UHINT)dest;
}

void    _oper_maximum_color_565(UHINT *fore_color,UHINT *back_color)
{
    ULONG dest=0;

    dest |= MAX((*fore_color & R_MASK_565),(*back_color & R_MASK_565));
    dest |= MAX((*fore_color & G_MASK_565),(*back_color & G_MASK_565));
    dest |= MAX((*fore_color & B_MASK_565),(*back_color & B_MASK_565));
    *back_color = (UHINT)dest;
}

void    _oper_half_color_565(UHINT *fore_color,UHINT *back_color)
{
    /* old version
    ULONG t,dest=0;

    t=((*fore_color & R_MASK_565)+(*back_color & R_MASK_565))>>1;
    dest |= t&R_MASK_565;
    t=((*fore_color & G_MASK_565)+(*back_color & G_MASK_565))>>1;
    dest |= t&G_MASK_565;
    t=((*fore_color & B_MASK_565)+(*back_color & B_MASK_565))>>1;
    dest |= t&B_MASK_565;

    *back_color = (UHINT)dest;
    */
    (*back_color) = (UHINT)( ( ( *fore_color & RGB_HALF_565 ) >> 1 ) + ( ( *back_color & RGB_HALF_565) >> 1 ) );
}

void    _oper_dark_color_565(UHINT *fore_color, SLONG dark)
{
    /* C version
    UCHR r,g,b;

    _hi2rgb_565( *fore_color, &r, &g, &b );
    r = (UCHR)((ULONG)(r*dark)>>5);
    g = (UCHR)((ULONG)(g*dark)>>5);
    b = (UCHR)((ULONG)(b*dark)>>5);
    *fore_color = _rgb2hi_565( r, g, b );
    */
    _asm
    {
        //把 fore_color 中的内容移入 ax
        mov edx, fore_color
        xor eax, eax
        mov ax, WORD PTR [edx]

        //准备 eax 成 _fg_fr_fb 格式
        mov ecx, eax

        and eax, G_MASK_565
        shl eax, 16
        and ecx, RB_MASK_565
        or  eax, ecx

        //计算 ( f * dark )/32
        imul eax, dark
        shr eax, 5

        //把 ax 转化成 16 位 rgb
        and eax, GRB32_MASK_565
        mov cx , ax
        shr eax, 16
        or  ax , cx

        //给 fore_color 赋值
        //mov edx, fore_color
        mov WORD PTR [edx], ax
    }
}


void    _oper_red_color_565(UHINT *fore_color, UHINT *back_color)
{
    UCHR r,g,b;

    _hi2rgb_565(*fore_color, &r, &g, &b);
    if(r + r > 0xff) r = 0xff;
    else r = r + r;
    g = (UCHR)( g >> 1 );
    b = (UCHR)( b >> 1 );
    *back_color = _rgb2hi_565( r, g, b );
}


void    _oper_green_color_565(UHINT *fore_color, UHINT *back_color)
{
    UCHR r,g,b;

    _hi2rgb_565(*fore_color, &r, &g, &b);
    if( g + g > 0xff) g = 0xff;
    else g = g + g;
    r = (UCHR)( r >> 1 );
    b = (UCHR)( b >> 1 );
    *back_color = _rgb2hi_565( r, g, b );
}


void    _oper_blue_color_565(UHINT *fore_color, UHINT *back_color)
{
    UCHR r,g,b;

    _hi2rgb_565(*fore_color, &r, &g, &b);
    if( b + b > 0xff) b = 0xff;
    else b = b + b;
    r = (UCHR)( r >> 1 );
    g = (UCHR)( g >> 1 );
    *back_color = _rgb2hi_565( r, g, b );
}


void    _oper_yellow_color_565(UHINT *fore_color, UHINT *back_color)
{
    UCHR r,g,b;

    _hi2rgb_565(*fore_color, &r, &g, &b);
    if(r + r > 0xff) r = 0xff;
    else r = r + r;
    if( g + g > 0xff) g = 0xff;
    else g = g + g;
    b = (UCHR)( b >> 1 );
    *back_color = _rgb2hi_565( r, g, b );
}


void    _oper_merge_color_565(UHINT *fore_color, UHINT *back_color, SLONG alpha)
{
    UCHR fr,fg,fb;
    UCHR br,bg,bb;
    UCHR r,g,b;
    SLONG lr, lg, lb;

    _hi2rgb_565(*fore_color, &fr, &fg, &fb);
    _hi2rgb_565(*back_color, &br, &bg, &bb);
    lr = ( br + fr * alpha / 255 );
    lg = ( bg + fg * alpha / 255 );
    lb = ( bb + fb * alpha / 255 );
    if(lr > 0xff) r = 0xff;
    else r = (UCHR) lr;

    if(lg > 0xff) g = 0xff;
    else g = (UCHR) lg;

    if(lb > 0xff) b = 0xff;
    else b = (UCHR) lb;

    *back_color = _rgb2hi_565(r,g,b);
}


void    _oper_partial_color_565(UHINT *fore_color, UHINT *back_color, SLONG alpha)
{
    UCHR fr,fg,fb;
    UCHR r,g,b;

    _hi2rgb_565(*fore_color,&fr,&fg,&fb);
    r = (UCHR)( fr * alpha / 255 );
    g = (UCHR)( fg * alpha / 255 );
    b = (UCHR)( fb * alpha / 255 );
    *back_color = _rgb2hi_565(r,g,b);
}


void    _oper_complement_color_565(UHINT *fore_color, UHINT *back_color, SLONG alpha)
{
    UCHR fr,fg,fb;
    UCHR r,g,b;

    _hi2rgb_565(*fore_color,&fr,&fg,&fb);
    r = (UCHR)( fr - fr * alpha / 255 );
    g = (UCHR)( fg - fg * alpha / 255 );
    b = (UCHR)( fb - fb * alpha / 255 );
    *back_color = _rgb2hi_565(r,g,b);
}

void	_oper_adulterate_color_565(UHINT *fore_color, SLONG r, SLONG g, SLONG b)
{
	UCHR	fr, fg, fb;
	SLONG	nr, ng, nb;

	_hi2rgb_565(*fore_color, &fr, &fg, &fb);
	nr = fr + r;
	fr = (nr > 0xff) ? 0xff : (UCHR)nr;
	ng = fg + g;
	fg = (ng > 0xff) ? 0xff : (UCHR)ng;
	nb = fb + b;
	fb = (nb > 0xff) ? 0xff : (UCHR)nb;
	*fore_color = _rgb2hi_565(fr, fg, fb);
}


void	_oper_eclipse_color_565(UHINT *fore_color, SLONG r, SLONG g, SLONG b)
{
	UCHR	fr, fg, fb;
	SLONG	nr, ng, nb;

	_hi2rgb_565(*fore_color, &fr, &fg, &fb);
	nr = fr - r;
	fr = (nr < 0x00) ? 0x00 : (UCHR)nr;
	ng = fg - g;
	fg = (ng < 0x00) ? 0x00 : (UCHR)ng;
	nb = fb - b;
	fb = (nb < 0x00) ? 0x00 : (UCHR)nb;
	*fore_color = _rgb2hi_565(fr, fg, fb);
}


void    _oper_blue_alpha_color_565(UHINT *fore_color, UHINT *back_color, SLONG alpha)
{
    UCHR    fr,fg,fb;
    UCHR    br,bg,bb;
    SLONG   nr,ng,nb;
    UCHR    r,g,b;

    _hi2rgb_565(*fore_color,&fr,&fg,&fb);
    _hi2rgb_565(*back_color,&br,&bg,&bb);
    nr = fr >> 1;
    ng = fg >> 1;
    nb = min((SLONG)fb << 1, 255);
    r = (UCHR)( br + (nr-br)*alpha/255 );
    g = (UCHR)( bg + (ng-bg)*alpha/255 );
    b = (UCHR)( bb + (nb-bb)*alpha/255 );
    *back_color = _rgb2hi_565(r,g,b);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VGA 556 FUNCTIONS 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
UHINT   _true2hi_556(ULONG c)
{
    return (UHINT)(((c>>8)&0xf800) | ((c>>5)&0x07e0) | ((c>>2)&0x001f));
}

ULONG   _hi2true_556(UHINT c)
{
    return (ULONG)((((ULONG)c<<8)&0xf80000) | (((ULONG)c<<5)&0xf800) | (((ULONG)c<<2)&0xfc));
}

UCHR    _get_r_556(UHINT c)
{
    return (UCHR)((c&R_MASK_556)>>8);
}

UCHR    _get_g_556(UHINT c)
{
    return (UCHR)((c&G_MASK_556)>>3);
}

UCHR    _get_b_556(UHINT c)
{
    return (UCHR)((c&B_MASK_556)<<2);
}

void    _set_r_556(UHINT *c,UCHR r)
{
    (*c) &= ~R_MASK_556;
    (*c) |= ((UHINT)r << 8) & R_MASK_556;
}

void    _set_g_556(UHINT *c,UCHR g)
{
    (*c) &= ~G_MASK_556;
    (*c) |= ((UHINT)g << 3) & G_MASK_556;
}

void    _set_b_556(UHINT *c,UCHR b)
{
    (*c) &= ~B_MASK_556;
    (*c) |= ((UHINT)b >> 2) & B_MASK_556;
}


UHINT   _rgb2hi_556(UCHR r,UCHR g,UCHR b)
{
    return (UHINT)( (((UHINT)r<<8)&R_MASK_556) | (((UHINT)g<<3)&G_MASK_556) | (((UHINT)b>>2)&B_MASK_556) );
}

void    _hi2rgb_556(UHINT c,UCHR *r,UCHR *g,UCHR *b)
{
    *r=(UCHR)((c&R_MASK_556)>>8);
    *g=(UCHR)((c&G_MASK_556)>>3);
    *b=(UCHR)((c&B_MASK_556)<<2);
}

UHINT   _fff2hi_556(UHINT c)
{
    UCHR r,g,b;
    _hi2rgb_555(c,&r,&g,&b);
    return _rgb2hi_556(r,g,b);
}

UHINT   _hi2fff_556(UHINT c)
{
    UCHR r,g,b;
    _hi2rgb_556(c,&r,&g,&b);
    return _rgb2hi_555(r,g,b);
}


void    _oper_alpha_color_556(UHINT *fore_color,UHINT *back_color,SLONG alpha)
{
    /* C Version
    UCHR fr,fg,fb;
    UCHR br,bg,bb;
    UCHR r,g,b;

    _hi2rgb_556(*fore_color,&fr,&fg,&fb);
    _hi2rgb_556(*back_color,&br,&bg,&bb);
    r = (UCHR)( br + (fr-br)*alpha/255 );
    g = (UCHR)( bg + (fg-bg)*alpha/255 );
    b = (UCHR)( bb + (fb-bb)*alpha/255 );
    *back_color = _rgb2hi_556(r,g,b);
    */
    alpha >>= 3;
    _asm
    {
        //把 fore_color 中的内容移入 ax
        //把 back_color 中的内容移入 bx
        mov edx, fore_color
        mov ecx, back_color
        xor eax, eax
        xor ebx, ebx
        mov ax, WORD PTR [edx]
        mov bx, WORD PTR [ecx]

        //准备 eax 成 _fg_fr_fb 格式
        //准备 ebx 成 _bg_br_bb 格式
        mov ecx, eax
        mov edx, ebx

        and eax, G_MASK_556
        and ebx, G_MASK_556
        shl eax, 16
        shl ebx, 16
        and ecx, RB_MASK_556
        and edx, RB_MASK_556
        or  eax, ecx
        or  ebx, edx

        //计算 (( f - b ) * alpha)/32 + b
        sub eax, ebx
        imul eax, alpha
        shr eax, 5
        add eax, ebx

        //把 ax 转化成 16 位 rgb
        and eax, GRB32_MASK_556
        mov cx , ax
        shr eax, 16
        or  ax , cx

        //给 back_color 赋值
        mov edx, back_color
        mov WORD PTR [edx], ax
    }
}


void    _oper_additive_color_556( UHINT *fore_color, UHINT *back_color)
{
    ULONG t,dest=0;

    t = (*fore_color & R_MASK_556) + (*back_color & R_MASK_556);
    if(t > R_MASK_556)
        t = R_MASK_556;
    dest |= t;

    t = (*fore_color & G_MASK_556) + (*back_color & G_MASK_556);
    if(t > G_MASK_556)
        t = G_MASK_556;
    dest |= t;

    t = (*fore_color & B_MASK_556) + (*back_color & B_MASK_556);
    if( t > B_MASK_556)
        t = B_MASK_556;
    dest |= t;
    *back_color = (UHINT)dest;
    /*
    UCHR    fr, fg, fb;
    UCHR    br, bg, bb;
    UHINT   r, g, b;

    _hi2rgb_556(*fore_color, &fr, &fg, &fb);
    _hi2rgb_556(*back_color, &br, &bg, &bb);
    r = fr + br;
    if(r > 255) r = 255;
    g = fg + bg;
    if(g > 255) g = 255;
    b = fb + bb;
    if(b > 255) b = 255;
    *back_color = _rgb2hi_556((UCHR)r, (UCHR)g, (UCHR)b);
    */
}


void    _oper_gray_color_556(UHINT *fore_color, UHINT *back_color)
{
    UCHR r,g,b,gray;

    _hi2rgb_556(*fore_color,&r,&g,&b);
    gray=(unsigned char)((r*30+g*59+b*11)/100);
    *back_color = _rgb2hi_556(gray,gray,gray);
}


void    _oper_subtractive_color_556(UHINT *fore_color,UHINT *back_color)
{
    SLONG t;
    ULONG dest=0;

    t=(SLONG)(*fore_color & R_MASK_556) - (SLONG)(*back_color & R_MASK_556);
    if(t<0) t=0;
    dest |= ((ULONG)t & R_MASK_556);

    t=(SLONG)(*fore_color & G_MASK_556) - (SLONG)(*back_color & G_MASK_556);
    if(t<0) t=0;
    dest |= ((ULONG)t & G_MASK_556);

    t=(SLONG)(*fore_color & B_MASK_556) - (SLONG)(*back_color & B_MASK_556);
    if(t<0) t=0;
    dest |= ((ULONG)t & B_MASK_556);

    *back_color = (UHINT)dest;
}

void    _oper_minimum_color_556(UHINT *fore_color,UHINT *back_color)
{
    ULONG dest=0;

    dest |= MIN((*fore_color & R_MASK_556),(*back_color & R_MASK_556));
    dest |= MIN((*fore_color & G_MASK_556),(*back_color & G_MASK_556));
    dest |= MIN((*fore_color & B_MASK_556),(*back_color & B_MASK_556));
    *back_color = (UHINT)dest;
}

void    _oper_maximum_color_556(UHINT *fore_color,UHINT *back_color)
{
    ULONG dest=0;

    dest |= MAX((*fore_color & R_MASK_556),(*back_color & R_MASK_556));
    dest |= MAX((*fore_color & G_MASK_556),(*back_color & G_MASK_556));
    dest |= MAX((*fore_color & B_MASK_556),(*back_color & B_MASK_556));
    *back_color = (UHINT)dest;
}

void    _oper_half_color_556(UHINT *fore_color,UHINT *back_color)
{
    /* old version
    ULONG t,dest=0;

    t=((*fore_color & R_MASK_556)+(*back_color & R_MASK_556))>>1;
    dest |= t&R_MASK_556;
    t=((*fore_color & G_MASK_556)+(*back_color & G_MASK_556))>>1;
    dest |= t&G_MASK_556;
    t=((*fore_color & B_MASK_556)+(*back_color & B_MASK_556))>>1;
    dest |= t&B_MASK_556;

    *back_color = (UHINT)dest;
    */
    (*back_color) = (UHINT)( ( ( *fore_color & RGB_HALF_556 ) >> 1 ) + ( ( *back_color & RGB_HALF_556) >> 1 ) );
}


void    _oper_dark_color_556(UHINT *fore_color, SLONG dark)
{
    /* C version
    UCHR r,g,b;

    _hi2rgb_556( *fore_color, &r, &g, &b );
    r = (UCHR)((ULONG)(r*dark)>>5);
    g = (UCHR)((ULONG)(g*dark)>>5);
    b = (UCHR)((ULONG)(b*dark)>>5);
    *fore_color = _rgb2hi_556( r, g, b );
    */
    _asm
    {
        //把 fore_color 中的内容移入 ax
        mov edx, fore_color
        xor eax, eax
        mov ax, WORD PTR [edx]

        //准备 eax 成 _fg_fr_fb 格式
        mov ecx, eax

        and eax, G_MASK_556
        shl eax, 16
        and ecx, RB_MASK_556
        or  eax, ecx

        //计算 ( f * dark )/32
        imul eax, dark
        shr eax, 5

        //把 ax 转化成 16 位 rgb
        and eax, GRB32_MASK_556
        mov cx , ax
        shr eax, 16
        or  ax , cx

        //给 fore_color 赋值
        //mov edx, fore_color
        mov WORD PTR [edx], ax
    }
}


void    _oper_red_color_556(UHINT *fore_color, UHINT *back_color)
{
    UCHR r,g,b;

    _hi2rgb_556(*fore_color, &r, &g, &b);
    if(r + r > 0xff) r = 0xff;
    else r = r + r;
    g = (UCHR)( g >> 1 );
    b = (UCHR)( b >> 1 );
    *back_color = _rgb2hi_556( r, g, b );
}


void    _oper_green_color_556(UHINT *fore_color, UHINT *back_color)
{
    UCHR r,g,b;

    _hi2rgb_556(*fore_color, &r, &g, &b);
    if( g + g > 0xff) g = 0xff;
    else g = g + g;
    r = (UCHR)( r >> 1 );
    b = (UCHR)( b >> 1 );
    *back_color = _rgb2hi_556( r, g, b );
}


void    _oper_blue_color_556(UHINT *fore_color, UHINT *back_color)
{
    UCHR r,g,b;

    _hi2rgb_556(*fore_color, &r, &g, &b);
    if( b + b > 0xff) b = 0xff;
    else b = b + b;
    r = (UCHR)( r >> 1 );
    g = (UCHR)( g >> 1 );
    *back_color = _rgb2hi_556( r, g, b );
}


void    _oper_yellow_color_556(UHINT *fore_color, UHINT *back_color)
{
    UCHR r,g,b;

    _hi2rgb_556(*fore_color, &r, &g, &b);
    if(r + r > 0xff) r = 0xff;
    else r = r + r;
    if( g + g > 0xff) g = 0xff;
    else g = g + g;
    b = (UCHR)( b >> 1 );
    *back_color = _rgb2hi_556( r, g, b );
}


void    _oper_merge_color_556(UHINT *fore_color, UHINT *back_color, SLONG alpha)
{
    UCHR fr,fg,fb;
    UCHR br,bg,bb;
    UCHR r,g,b;
    SLONG lr, lg, lb;

    _hi2rgb_556(*fore_color, &fr, &fg, &fb);
    _hi2rgb_556(*back_color, &br, &bg, &bb);
    lr = ( br + fr * alpha / 255 );
    lg = ( bg + fg * alpha / 255 );
    lb = ( bb + fb * alpha / 255 );
    if(lr > 0xff) r = 0xff;
    else r = (UCHR) lr;

    if(lg > 0xff) g = 0xff;
    else g = (UCHR) lg;

    if(lb > 0xff) b = 0xff;
    else b = (UCHR) lb;

    *back_color = _rgb2hi_556(r,g,b);
}


void    _oper_partial_color_556(UHINT *fore_color, UHINT *back_color, SLONG alpha)
{
    UCHR fr,fg,fb;
    UCHR r,g,b;

    _hi2rgb_556(*fore_color,&fr,&fg,&fb);
    r = (UCHR)( fr * alpha / 255 );
    g = (UCHR)( fg * alpha / 255 );
    b = (UCHR)( fb * alpha / 255 );
    *back_color = _rgb2hi_556(r,g,b);
}

void    _oper_complement_color_556(UHINT *fore_color, UHINT *back_color, SLONG alpha)
{
    UCHR fr,fg,fb;
    UCHR r,g,b;

    _hi2rgb_556(*fore_color,&fr,&fg,&fb);
    r = (UCHR)( fr - fr * alpha / 255 );
    g = (UCHR)( fg - fg * alpha / 255 );
    b = (UCHR)( fb - fb * alpha / 255 );
    *back_color = _rgb2hi_556(r,g,b);
}

void	_oper_adulterate_color_556(UHINT *fore_color,  SLONG r, SLONG g, SLONG b)
{
	UCHR	fr, fg, fb;
	SLONG	nr, ng, nb;

	_hi2rgb_556(*fore_color, &fr, &fg, &fb);
	nr = fr + r;
	fr = (nr > 0xff) ? 0xff : (UCHR)nr;
	ng = fg + g;
	fg = (ng > 0xff) ? 0xff : (UCHR)ng;
	nb = fb + b;
	fb = (nb > 0xff) ? 0xff : (UCHR)nb;
	*fore_color = _rgb2hi_556(fr, fg, fb);
}


void	_oper_eclipse_color_556(UHINT *fore_color, SLONG r, SLONG g, SLONG b)
{
	UCHR	fr, fg, fb;
	SLONG	nr, ng, nb;

	_hi2rgb_556(*fore_color, &fr, &fg, &fb);
	nr = fr - r;
	fr = (nr < 0x00) ? 0x00 : (UCHR)nr;
	ng = fg - g;
	fg = (ng < 0x00) ? 0x00 : (UCHR)ng;
	nb = fb - b;
	fb = (nb < 0x00) ? 0x00 : (UCHR)nb;
	*fore_color = _rgb2hi_556(fr, fg, fb);
}


void    _oper_blue_alpha_color_556(UHINT *fore_color, UHINT *back_color, SLONG alpha)
{
    UCHR    fr,fg,fb;
    UCHR    br,bg,bb;
    SLONG   nr,ng,nb;
    UCHR    r,g,b;

    _hi2rgb_556(*fore_color,&fr,&fg,&fb);
    _hi2rgb_556(*back_color,&br,&bg,&bb);
    nr = fr >> 1;
    ng = fg >> 1;
    nb = min((SLONG)fb << 1, 255);
    r = (UCHR)( br + (nr-br)*alpha/255 );
    g = (UCHR)( bg + (ng-bg)*alpha/255 );
    b = (UCHR)( bb + (nb-bb)*alpha/255 );
    *back_color = _rgb2hi_556(r,g,b);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VGA ANY FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
UHINT   _true2hi_any(ULONG c)
{
    UCHR r=(UCHR)((c&0xff0000)>>16),g=(UCHR)((c&0xff00)>>8),b=(UCHR)(c&0xff);
    return _rgb2hi_any(r,g,b);
}

inline ULONG   _hi2true_any(UHINT c)
{
    static UCHR r,g,b;
    _hi2rgb_any(c,&r,&g,&b);
    return (((ULONG)r<<16)&0xff0000) | (((ULONG)g<<8)&0xff00) | (b&0xff);
}

UCHR    _get_r_any(UHINT c)
{
    return (UCHR)( ((UCHR)(c & vga_info.r_mask) >> vga_info.r_bit_low) << vga_info.r_shift );
}

UCHR    _get_g_any(UHINT c)
{
    return (UCHR)( ((UCHR)(c & vga_info.g_mask) >> vga_info.g_bit_low) << vga_info.g_shift );
}

UCHR    _get_b_any(UHINT c)
{
    return (UCHR)( ((UCHR)(c & vga_info.b_mask) >> vga_info.b_bit_low) << vga_info.b_shift );
}

void    _set_r_any(UHINT *c,UCHR r)
{
    (*c) &= ~vga_info.r_mask;
    (*c) |= ((UHINT)(r >> vga_info.r_shift) << vga_info.r_bit_low) & vga_info.r_mask;
}

void    _set_g_any(UHINT *c,UCHR g)
{
    (*c) &= ~vga_info.g_mask;
    (*c) |= ((UHINT)(g >> vga_info.g_shift) << vga_info.g_bit_low) & vga_info.g_mask;
}

void    _set_b_any(UHINT *c,UCHR b)
{
    (*c) &= ~vga_info.b_mask;
    (*c) |= ((UHINT)(b >> vga_info.b_shift) << vga_info.b_bit_low) & vga_info.b_mask;
}


UHINT   _rgb2hi_any(UCHR r,UCHR g,UCHR b)
{
    static  UHINT r1,g1,b1;
    
    r1=(UHINT)(((UHINT)(r>>vga_info.r_shift)<<vga_info.r_bit_low) & vga_info.r_mask);
    g1=(UHINT)(((UHINT)(g>>vga_info.g_shift)<<vga_info.g_bit_low) & vga_info.g_mask);
    b1=(UHINT)(((UHINT)(b>>vga_info.b_shift)<<vga_info.b_bit_low) & vga_info.b_mask);
    return (UHINT)(r1|g1|b1);
}

inline void    _hi2rgb_any(UHINT c,UCHR *r,UCHR *g,UCHR *b)
{
    (*r)=(UCHR)( (UCHR)((c & vga_info.r_mask) >> vga_info.r_bit_low) << vga_info.r_shift );
    (*g)=(UCHR)( (UCHR)((c & vga_info.g_mask) >> vga_info.g_bit_low) << vga_info.g_shift );
    (*b)=(UCHR)( (UCHR)((c & vga_info.b_mask) >> vga_info.b_bit_low) << vga_info.b_shift );
}

UHINT   _fff2hi_any(UHINT c)
{
    UCHR r,g,b;
    _hi2rgb_555(c,&r,&g,&b);
    return _rgb2hi_any(r,g,b);
}

UHINT   _hi2fff_any(UHINT c)
{
    UCHR r,g,b;
    _hi2rgb_any(c,&r,&g,&b);
    return _rgb2hi_555(r,g,b);
}


void    _oper_alpha_color_any(UHINT *fore_color,UHINT *back_color,SLONG alpha)
{
    UCHR fr,fg,fb;
    UCHR br,bg,bb;
    UCHR r,g,b;

    _hi2rgb_any(*fore_color,&fr,&fg,&fb);
    _hi2rgb_any(*back_color,&br,&bg,&bb);
    r = (UCHR)( br + (fr-br)*alpha/255 );
    g = (UCHR)( bg + (fg-bg)*alpha/255 );
    b = (UCHR)( bb + (fb-bb)*alpha/255 );
    *back_color = _rgb2hi_any(r,g,b);
}


void    _oper_additive_color_any( UHINT *fore_color, UHINT *back_color)
{
    ULONG t,dest=0;

    t=(*fore_color & vga_info.r_mask) + (*back_color & vga_info.r_mask);
    if(t>vga_info.r_mask)
        t=vga_info.r_mask;
    dest |= t;

    t=(*fore_color & vga_info.g_mask) + (*back_color & vga_info.g_mask);
    if(t>vga_info.g_mask)
        t=vga_info.g_mask;
    dest |= t;

    t=(*fore_color & vga_info.b_mask) + (*back_color & vga_info.b_mask);
    if(t>vga_info.b_mask)
        t=vga_info.b_mask;
    dest |= t;

    *back_color = (UHINT)dest;
    /*
    UCHR    fr, fg, fb;
    UCHR    br, bg, bb;
    UHINT   r, g, b;

    _hi2rgb_any(*fore_color, &fr, &fg, &fb);
    _hi2rgb_any(*back_color, &br, &bg, &bb);
    r = fr + br;
    if(r > 255) r = 255;
    g = fg + bg;
    if(g > 255) g = 255;
    b = fb + bb;
    if(b > 255) b = 255;
    *back_color = _rgb2hi_any((UCHR)r, (UCHR)g, (UCHR)b);
    */
}


void    _oper_gray_color_any(UHINT *fore_color, UHINT *back_color)
{
    UCHR r,g,b,gray;

    _hi2rgb_any(*fore_color,&r,&g,&b);
    gray=(unsigned char)((r*30+g*59+b*11)/100);
    *back_color = _rgb2hi_any(gray,gray,gray);
}

void    _oper_subtractive_color_any(UHINT *fore_color,UHINT *back_color)
{
    SLONG t;
    ULONG dest=0;

    t=(SLONG)(*fore_color & vga_info.r_mask) - (SLONG)(*back_color & vga_info.r_mask);
    if(t<0) t=0;
    dest |= ((ULONG)t&vga_info.r_mask);

    t=(SLONG)(*fore_color & vga_info.g_mask) - (SLONG)(*back_color & vga_info.g_mask);
    if(t<0) t=0;
    dest |= ((ULONG)t&vga_info.g_mask);

    t=(SLONG)(*fore_color & vga_info.b_mask) - (SLONG)(*back_color & vga_info.b_mask);
    if(t<0) t=0;
    dest |= ((ULONG)t&vga_info.b_mask);

    *back_color = (UHINT)dest;
}

void    _oper_minimum_color_any(UHINT *fore_color,UHINT *back_color)
{
    ULONG dest=0;

    dest |= MIN((*fore_color & vga_info.r_mask),(*back_color & vga_info.r_mask));
    dest |= MIN((*fore_color & vga_info.g_mask),(*back_color & vga_info.g_mask));
    dest |= MIN((*fore_color & vga_info.b_mask),(*back_color & vga_info.b_mask));
    *back_color = (UHINT)dest;
}

void    _oper_maximum_color_any(UHINT *fore_color,UHINT *back_color)
{
    ULONG dest=0;

    dest |= MAX((*fore_color & vga_info.r_mask),(*back_color & vga_info.r_mask));
    dest |= MAX((*fore_color & vga_info.g_mask),(*back_color & vga_info.g_mask));
    dest |= MAX((*fore_color & vga_info.b_mask),(*back_color & vga_info.b_mask));
    *back_color = (UHINT)dest;
}

void    _oper_half_color_any(UHINT *fore_color,UHINT *back_color)
{
    ULONG t,dest=0;

    t=((*fore_color & vga_info.r_mask)+(*back_color & vga_info.r_mask))>>1;
    dest |= t&vga_info.r_mask;
    t=((*fore_color & vga_info.g_mask)+(*back_color & vga_info.g_mask))>>1;
    dest |= t&vga_info.g_mask;
    t=((*fore_color & vga_info.b_mask)+(*back_color & vga_info.b_mask))>>1;
    dest |= t&vga_info.b_mask;

    *back_color = (UHINT)dest;
}


void    _oper_dark_color_any(UHINT *fore_color, SLONG dark)
{
    UCHR r,g,b;

    _hi2rgb_any( *fore_color, &r, &g, &b );
    r = (UCHR)((ULONG)(r*dark)>>5);
    g = (UCHR)((ULONG)(g*dark)>>5);
    b = (UCHR)((ULONG)(b*dark)>>5);
    *fore_color = _rgb2hi_any( r, g, b );
}


void    _oper_red_color_any(UHINT *fore_color, UHINT *back_color)
{
    UCHR r,g,b;

    _hi2rgb_any(*fore_color, &r, &g, &b);
    if(r + r > 0xff) r = 0xff;
    else r = r + r;
    g = (UCHR)( g >> 1 );
    b = (UCHR)( b >> 1 );
    *back_color = _rgb2hi_any( r, g, b );
}


void    _oper_green_color_any(UHINT *fore_color, UHINT *back_color)
{
    UCHR r,g,b;

    _hi2rgb_any(*fore_color, &r, &g, &b);
    if( g + g > 0xff) g = 0xff;
    else g = g + g;
    r = (UCHR)( r >> 1 );
    b = (UCHR)( b >> 1 );
    *back_color = _rgb2hi_any( r, g, b );
}


void    _oper_blue_color_any(UHINT *fore_color, UHINT *back_color)
{
    UCHR r,g,b;

    _hi2rgb_any(*fore_color, &r, &g, &b);
    if( b + b > 0xff) b = 0xff;
    else b = b + b;
    r = (UCHR)( r >> 1 );
    g = (UCHR)( g >> 1 );
    *back_color = _rgb2hi_any( r, g, b );
}


void    _oper_yellow_color_any(UHINT *fore_color, UHINT *back_color)
{
    UCHR r,g,b;

    _hi2rgb_any(*fore_color, &r, &g, &b);
    if(r + r > 0xff) r = 0xff;
    else r = r + r;
    if( g + g > 0xff) g = 0xff;
    else g = g + g;
    b = (UCHR)( b >> 1 );
    *back_color = _rgb2hi_any( r, g, b );
}


void    _oper_merge_color_any(UHINT *fore_color, UHINT *back_color, SLONG alpha)
{
    UCHR fr,fg,fb;
    UCHR br,bg,bb;
    UCHR r,g,b;
    SLONG lr, lg, lb;

    _hi2rgb_any(*fore_color, &fr, &fg, &fb);
    _hi2rgb_any(*back_color, &br, &bg, &bb);
    lr = ( br + fr * alpha / 255 );
    lg = ( bg + fg * alpha / 255 );
    lb = ( bb + fb * alpha / 255 );
    if(lr > 0xff) r = 0xff;
    else r = (UCHR) lr;

    if(lg > 0xff) g = 0xff;
    else g = (UCHR) lg;

    if(lb > 0xff) b = 0xff;
    else b = (UCHR) lb;

    *back_color = _rgb2hi_any(r,g,b);
}


void    _oper_partial_color_any(UHINT *fore_color, UHINT *back_color, SLONG alpha)
{
    UCHR fr,fg,fb;
    UCHR r,g,b;

    _hi2rgb_any(*fore_color,&fr,&fg,&fb);
    r = (UCHR)( fr * alpha / 255 );
    g = (UCHR)( fg * alpha / 255 );
    b = (UCHR)( fb * alpha / 255 );
    *back_color = _rgb2hi_any(r,g,b);
}


void    _oper_complement_color_any(UHINT *fore_color, UHINT *back_color, SLONG alpha)
{
    UCHR fr,fg,fb;
    UCHR r,g,b;

    _hi2rgb_any(*fore_color,&fr,&fg,&fb);
    r = (UCHR)( fr - fr * alpha / 255 );
    g = (UCHR)( fg - fg * alpha / 255 );
    b = (UCHR)( fb - fb * alpha / 255 );
    *back_color = _rgb2hi_any(r,g,b);
}


void	_oper_adulterate_color_any(UHINT *fore_color, SLONG r, SLONG g, SLONG b)
{
	UCHR	fr, fg, fb;
	SLONG	nr, ng, nb;

	_hi2rgb_any(*fore_color, &fr, &fg, &fb);
	nr = fr + r;
	fr = (nr > 0xff) ? 0xff : (UCHR)nr;
	ng = fg + g;
	fg = (ng > 0xff) ? 0xff : (UCHR)ng;
	nb = fb + b;
	fb = (nb > 0xff) ? 0xff : (UCHR)nb;
	*fore_color = _rgb2hi_any(fr, fg, fb);
}


void	_oper_eclipse_color_any(UHINT *fore_color, SLONG r, SLONG g, SLONG b)
{
	UCHR	fr, fg, fb;
	SLONG	nr, ng, nb;

	_hi2rgb_any(*fore_color, &fr, &fg, &fb);
	nr = fr - r;
	fr = (nr < 0x00) ? 0x00 : (UCHR)nr;
	ng = fg - g;
	fg = (ng < 0x00) ? 0x00 : (UCHR)ng;
	nb = fb - b;
	fb = (nb < 0x00) ? 0x00 : (UCHR)nb;
	*fore_color = _rgb2hi_any(fr, fg, fb);
}

void    _oper_blue_alpha_color_any(UHINT *fore_color, UHINT *back_color, SLONG alpha)
{
    UCHR    fr,fg,fb;
    UCHR    br,bg,bb;
    SLONG   nr,ng,nb;
    UCHR    r,g,b;

    _hi2rgb_any(*fore_color,&fr,&fg,&fb);
    _hi2rgb_any(*back_color,&br,&bg,&bb);
    nr = fr >> 1;
    ng = fg >> 1;
    nb = min((SLONG)fb << 1, 255);
    r = (UCHR)( br + (nr-br)*alpha/255 );
    g = (UCHR)( bg + (ng-bg)*alpha/255 );
    b = (UCHR)( bb + (nb-bb)*alpha/255 );
    *back_color = _rgb2hi_any(r,g,b);
}

