/*
**	mmx.cpp
**	mmx cpu support functions.
**
**	lrw, 2001.12.17.
**
*/
#include "rays.h"
#include "xdraw.h"
#include "xvga.h"
#include "xgrafx.h"
#include "xrle.h"
#include "mmx.h"


#define RLE_SHADOW_DARK         22

//GLOBALS --------------------------------------------------------------------------------
__int64 mmx_mask_red;
__int64 mmx_mask_green;
__int64 mmx_mask_blue;
__int64 mmx_mask_half;
__int64	mmx_offset_bit_red;
__int64	mmx_offset_bit_green;
__int64	mmx_offset_bit_blue;
BOOL	is_mmx;


//STATIC FUNCTIONS -----------------------------------------------------------------------
static	bool	is_cpu_mmx(void);
static	void    _range_put_rle_asm(SLONG des_x,SLONG des_y,RLE *rle,BMP *bmp);
static	void    _direct_put_rle_asm(SLONG des_x,SLONG des_y,RLE *rle,BMP *bmp);


//FUNCTIONS ------------------------------------------------------------------------------
int		init_mmx(void)
{
	is_mmx = is_cpu_mmx();
	return	0;
}


void	active_mmx(int active)
{
}


void	free_mmx(void)
{
}


void	init_mmx_grafx_associated_data(SLONG type)
{
	switch(type)
	{
    case VGA_TYPE_555:
        mmx_mask_red = MMX_MASK_RED_555;
		mmx_mask_green = MMX_MASK_GREEN_555;
		mmx_mask_blue = MMX_MASK_BLUE_555;
		mmx_mask_half = MMX_MASK_HALF_555;
		mmx_offset_bit_red = 1;
		mmx_offset_bit_green = 6;
		mmx_offset_bit_blue = 3;
		break;
    case VGA_TYPE_655:
        mmx_mask_red = MMX_MASK_RED_655;
		mmx_mask_green = MMX_MASK_GREEN_655;
		mmx_mask_blue = MMX_MASK_BLUE_655;
		mmx_mask_half = MMX_MASK_HALF_655;
		mmx_offset_bit_red = 0;
		mmx_offset_bit_green = 6;
		mmx_offset_bit_blue = 3;
        break;
    case VGA_TYPE_565:
        mmx_mask_red = MMX_MASK_RED_565;
		mmx_mask_green = MMX_MASK_GREEN_565;
		mmx_mask_blue = MMX_MASK_BLUE_565;
		mmx_mask_half = MMX_MASK_HALF_565;
		mmx_offset_bit_red = 0;
		mmx_offset_bit_green = 5;
		mmx_offset_bit_blue = 3;
        break;
    case VGA_TYPE_556:
        mmx_mask_red = MMX_MASK_RED_556;
		mmx_mask_green = MMX_MASK_GREEN_556;
		mmx_mask_blue = MMX_MASK_BLUE_556;
		mmx_mask_half = MMX_MASK_HALF_556;
		mmx_offset_bit_red = 0;
		mmx_offset_bit_green = 5;
		mmx_offset_bit_blue = 2;
        break;
    case VGA_TYPE_ANY:
        break;
	}
	return;
}


__declspec(naked) bool _cdecl is_cpu_mmx(void)
{
	__asm
	{
		pushfd
		pop   eax
		xor   eax,00200000h
		push  eax
		popfd
		pushfd
		pop   edx
		cmp   eax,edx
		jne   _not_support
		xor   eax,eax
		_emit 0x0f
		_emit 0xa2
		or    eax,eax
		jz    _not_support
		mov   eax,1
		_emit 0x0f
		_emit 0xa2
		test  edx,00800000h
		jz    _not_support
		mov   eax,TRUE
		ret
_not_support:
		mov  eax,FALSE
		ret
	}
}


void	_alpha_put_bar_mmx(SLONG sx,SLONG sy,SLONG xl,SLONG yl,PIXEL c,BMP *bmp,SLONG alpha)
{
	__int64 buf;
	int		rx,by;

	rx=sx+xl-1;
	by=sy+yl-1;
	__asm
	{
		mov   ecx,alpha
		mov   esi,bmp
		shr   ecx,3
		mov   eax,sy
		movd  mm6,ecx
		movd  mm5,ecx
		packssdw mm6,mm5
		mov  ebx,by
		movq  mm5,mm6

		/*
		if(sy<0) sy=0;
		*/
		cmp  eax,0
		mov  ecx,[esi]BMP.h
		jge  _sy_ge0
		xor  eax,eax
_sy_ge0:

		/*
		if(by>=(h=bmp->h)) by=h-1;
		*/
		cmp  ebx,ecx
		jl   _inc_h
		mov  ebx,ecx
		jmp  _h_le_h_end
_inc_h:
		inc	 ebx
_h_le_h_end:
		mov  edx,sx
		sub  ebx,eax

		cmp  ebx,0
		jle  _end
		mov  edi,[esi+eax*4]BMP.line
		mov  ecx,rx
		mov  eax,[esi]BMP.w
		/*
		if(sx<0) sx=0;
		*/
		cmp  edx,0
		jge  _sx_ge0
		xor  edx,edx
_sx_ge0:

		/*
		if(rx>=(w=bmp->w)) rx=w-1;
		*/
		cmp  ecx,eax
		jl   _rx_le_w
		mov  ecx,eax
		jmp  _rx_le_w_end
		ALIGN  4
_rx_le_w:
		inc  ecx
_rx_le_w_end:
		mov  esi,[esi]BMP.pitch
		sub  ecx,edx

		cmp  ecx,0
		jle  _end
		sub  esi,ecx
		xor  eax,eax
		lea  edi,[edi+edx*2]
		mov  ax,word ptr c
		sub  esi,ecx
		mov  edx,eax
		shl  eax,16
		packssdw mm6,mm5
		or   eax,edx
		mov  edx,ecx
		movd mm7,eax
		movd mm0,eax
		test ecx,3
		punpckldq mm7,mm0
		jz   _line_no_odd_loop

		ALIGN  4
_line_odd_loop:
		movq mm0,mm7
		movq mm3,[edi]
		movq mm1,mm0
		movq mm4,mm3
		movq mm2,mm0
		movq mm5,mm3
		pand mm0,mmx_mask_red
		pand mm1,mmx_mask_green
		pand mm2,mmx_mask_blue
		pand mm3,mmx_mask_red
		pand mm4,mmx_mask_green
		pand mm5,mmx_mask_blue
		psrlw mm0,5
		psrlw mm1,5
		psrlw mm3,5
		psrlw mm4,5
		psubsw mm0,mm3
		psubsw mm1,mm4
		psubsw mm2,mm5
		pmullw mm0,mm6
		pmullw mm1,mm6
		pmullw mm2,mm6
		psrlw  mm0,5
		psrlw  mm1,5
		psrlw  mm2,5
		paddw mm0,mm3
		paddw mm1,mm4
		paddw mm2,mm5
		psllw mm0,5
		psllw mm1,5
		pand mm0,mmx_mask_red
		pand mm1,mmx_mask_green
		pand mm2,mmx_mask_blue
		por mm0,mm1
		por mm0,mm2

		cmp  ecx,3
		jle  _line_end
		movq [edi],mm0
		sub  ecx,4
		add  edi,8
		jmp  _line_odd_loop
		ALIGN  4
_line_end:
		mov  dword ptr c,esi
		movq buf,mm0
		lea  esi,buf
		rep  movsw
		mov  esi,dword ptr c

		add  edi,esi
		mov  ecx,edx
		dec  ebx
		jnz  _line_odd_loop
        ALIGN 4
		jmp  _end

		ALIGN 4
_line_no_odd_loop:
		movq mm0,mm7
		movq mm3,[edi]
		movq mm1,mm0
		movq mm4,mm3
		movq mm2,mm0
		movq mm5,mm3
		pand mm0,mmx_mask_red
		pand mm1,mmx_mask_green
		pand mm2,mmx_mask_blue
		pand mm3,mmx_mask_red
		pand mm4,mmx_mask_green
		pand mm5,mmx_mask_blue
		psrlw mm0,5
		psrlw mm1,5
		psrlw mm3,5
		psrlw mm4,5
		psubsw mm0,mm3
		psubsw mm1,mm4
		psubsw mm2,mm5
		pmullw mm0,mm6
		pmullw mm1,mm6
		pmullw mm2,mm6
		psrlw  mm0,5
		psrlw  mm1,5
		psrlw  mm2,5
		paddw mm0,mm3
		paddw mm1,mm4
		paddw mm2,mm5
		psllw mm0,5
		psllw mm1,5
		pand mm0,mmx_mask_red
		pand mm1,mmx_mask_green
		pand mm2,mmx_mask_blue
		por mm0,mm1
		por mm0,mm2

		movq [edi],mm0
		add  edi,8
		sub  ecx,4
		jnz  _line_no_odd_loop
		add  edi,esi
		mov  ecx,edx
		dec  ebx
		jnz  _line_no_odd_loop
_end:
		emms
	}
}


void    _alpha_put_bitmap_mmx(SLONG sx,SLONG sy,BMP *src,BMP *dest,SLONG alpha)
{
	int  src_x,src_y;
	int  w,h;
	void *t;
	__int64 buf;

	__asm
	{
		mov  esi,src  /* 取源bmp的地址 */
		mov  edi,dest /* 取目标bmp的地址 */
		mov  eax,sx
		mov  ebx,sy

		/*
		if(sx<0)
			src_x=-sx,sx=0;
		else if(sx<des->w)
			src_x=0;
		else
			return;
		*/
		cmp  eax,0
		jge  _x_ge
		neg  eax
		mov  sx,0
		mov  src_x,eax
		jmp  _x_end
_x_ge:
		cmp  eax,[edi]BMP.w
		jge  _end
		mov  src_x,0
_x_end:


		/*
		if(sy<0)
			src_y=-sy,sy=0;
		else if(sy<des->h)
			src_y=0;
		else
			return;
		*/
		cmp  ebx,0
		jge  _y_ge
		neg  ebx
		mov  sy,0
		mov  src_y,ebx
		jmp  _y_end
_y_ge:
		cmp  ebx,[edi]BMP.h
		jge  _end
		mov  src_y,0
_y_end:


		/*
		h=src->h-src_y;
		if(h>des->h-sy) h=des->h-sy;
		if(h<=0) return;
		*/
		mov  eax,[esi]BMP.h
		mov  ebx,src_y
		mov  ecx,[edi]BMP.h
		mov  edx,sy
		sub  eax,ebx
		sub  ecx,edx
		cmp  eax,ecx
		jle  _h_le
		mov  eax,ecx
_h_le:
		cmp  eax,0
		jle  _end
		mov  h,eax

		/*
		w=src->w-src_x;
		if(w>des->w-sx) w=des->w-sx;
		if(w<=0) return;
		*/
		mov  eax,[esi]BMP.w
		mov  ebx,src_x
		mov  ecx,[edi]BMP.w
		mov  edx,sx
		sub  eax,ebx
		sub  ecx,edx
		cmp  eax,ecx
		jle  _w_le
		mov  eax,ecx
_w_le:
		cmp  eax,0
		jle  _end
		mov  w,eax

		mov  ebx,[esi]BMP.pitch
		shl  eax,1 /* w*2=>eax */
		mov  edx,[edi]BMP.pitch
		sub  ebx,eax
		sub  edx,eax
		mov  ecx,sy
		mov  eax,src_y
		mov  esi,[esi+eax*4+BMP.line]
		mov  edi,[edi+ecx*4+BMP.line]
		mov  eax,src_x
		mov  ecx,sx
		lea  esi,[esi+eax*2]
		lea  edi,[edi+ecx*2]

		mov   ecx,alpha
		shr   ecx,3
		mov   eax,h
		movd  mm6,ecx
		movd  mm5,ecx
		packssdw mm6,mm5
		mov   ecx,w
		movq  mm5,mm6
		packssdw mm6,mm5

		ALIGN 4
_loop:
		movq mm0,[esi]
		movq mm1,mm0
		movq mm3,[edi]
		movq mm2,mm0
		movq mm4,mm3
		movq mm5,mm3
		pand mm0,mmx_mask_red
		pand mm1,mmx_mask_green
		pand mm2,mmx_mask_blue
		pand mm3,mmx_mask_red
		pand mm4,mmx_mask_green
		pand mm5,mmx_mask_blue
		psrlw mm0,5
		psrlw mm1,5
		psrlw mm3,5
		psrlw mm4,5
		psubsw mm0,mm3
		psubsw mm1,mm4
		psubsw mm2,mm5
		pmullw mm0,mm6
		pmullw mm1,mm6
		pmullw mm2,mm6
		psrlw  mm0,5
		psrlw  mm1,5
		psrlw  mm2,5
		paddw mm0,mm3
		paddw mm1,mm4
		paddw mm2,mm5
		psllw mm0,5
		psllw mm1,5
		pand mm0,mmx_mask_red
		pand mm1,mmx_mask_green
		pand mm2,mmx_mask_blue
		por mm0,mm1
		por mm0,mm2

		cmp  ecx,4
		jl   _line_end

		movq [edi],mm0
		add  esi,8
		add  edi,8
		sub  ecx,4
		jnz  _loop
		ALIGN 4
		jmp  _next_line

		ALIGN 4
_line_end:
		lea  esi,[esi+ecx*2]
		mov  t,esi
		lea  esi,buf
		movq buf,mm0
		rep  movsw
		mov  esi,t

		ALIGN 4
_next_line:
		add  esi,ebx
		add  edi,edx
		dec  eax
		mov  ecx,w
		jnz  _loop
		emms
		ALIGN 4
_end:
	}
}


void	_half_put_bitmap_mmx(SLONG sx,SLONG sy,BMP *src,BMP *dest)
{
	int  src_x,src_y;
	int  w,h;
	void *t;
	__int64 buf;

	__asm
	{
		mov  esi,src  /* 取源bmp的地址 */
		mov  edi,dest /* 取目标bmp的地址 */
		mov  eax,sx
		mov  ebx,sy

		/*
		if(sx<0)
			src_x=-sx,sx=0;
		else if(sx<des->w)
			src_x=0;
		else
			return;
		*/
		cmp  eax,0
		jge  _x_ge
		neg  eax
		mov  sx,0
		mov  src_x,eax
		jmp  _x_end
_x_ge:
		cmp  eax,[edi]BMP.w
		jge  _end
		mov  src_x,0
_x_end:


		/*
		if(sy<0)
			src_y=-sy,sy=0;
		else if(sy<des->h)
			src_y=0;
		else
			return;
		*/
		cmp  ebx,0
		jge  _y_ge
		neg  ebx
		mov  sy,0
		mov  src_y,ebx
		jmp  _y_end
_y_ge:
		cmp  ebx,[edi]BMP.h
		jge  _end
		mov  src_y,0
_y_end:


		/*
		h=src->h-src_y;
		if(h>des->h-sy) h=des->h-sy;
		if(h<=0) return;
		*/
		mov  eax,[esi]BMP.h
		mov  ebx,src_y
		mov  ecx,[edi]BMP.h
		mov  edx,sy
		sub  eax,ebx
		sub  ecx,edx
		cmp  eax,ecx
		jle  _h_le
		mov  eax,ecx
_h_le:
		cmp  eax,0
		jle  _end
		mov  h,eax

		/*
		w=src->w-src_x;
		if(w>des->w-sx) w=des->w-sx;
		if(w<=0) return;
		*/
		mov  eax,[esi]BMP.w
		mov  ebx,src_x
		mov  ecx,[edi]BMP.w
		mov  edx,sx
		sub  eax,ebx
		sub  ecx,edx
		cmp  eax,ecx
		jle  _w_le
		mov  eax,ecx
_w_le:
		cmp  eax,0
		jle  _end
		mov  w,eax

		mov  ebx,[esi]BMP.pitch
		shl  eax,1 /* w*2=>eax */
		mov  edx,[edi]BMP.pitch
		sub  ebx,eax
		sub  edx,eax
		mov  ecx,sy
		mov  eax,src_y
		mov  esi,[esi+eax*4+BMP.line]
		mov  edi,[edi+ecx*4+BMP.line]
		mov  eax,src_x
		mov  ecx,sx
		lea  esi,[esi+eax*2]
		lea  edi,[edi+ecx*2]

		mov   eax,h
		mov   ecx,w
		movq  mm2,mmx_mask_half

		ALIGN 4
_loop:
		movq mm0,[esi]
		movq mm1,[edi]
		pand mm0,mm2
		pand mm1,mm2
		psrlw mm0,1
		psrlw mm1,1
		paddw mm0,mm1

		cmp  ecx,4
		jl   _line_end

		movq [edi],mm0
		add  esi,8
		add  edi,8
		sub  ecx,4
		jnz  _loop
		ALIGN 4
		jmp  _next_line

		ALIGN 4
_line_end:
		lea  esi,[esi+ecx*2]
		mov  t,esi
		lea  esi,buf
		movq buf,mm0
		rep  movsw
		mov  esi,t

		ALIGN 4
_next_line:
		add  esi,ebx
		add  edi,edx
		dec  eax
		mov  ecx,w
		jnz  _loop
		emms
		ALIGN 4
_end:
	}
}


void	_gray_put_bitmap_mmx(SLONG sx,SLONG sy,BMP *src,BMP *dest)
{
	int  src_x,src_y;
	int  w,h;
	void   *t;
	__int64 buf;

	/*static int  r=30,g=59,b=11; */
	/*((r*38.4+g*75.52+b*14.08)/128);*/
	__int64  quotiety_red;
	__int64  quotiety_green;
	__int64  quotiety_blue;

	quotiety_red=0x0026002600260026,
	quotiety_green=0x004c004c004c004c,
	quotiety_blue=0x000e000e000e000e;
	__asm
	{
		mov  eax,sx /* sx=>eax */
		mov  ebx,sy /* src_x=>ebx */
		mov  edi,dest  /* des=>edi   */
		mov  esi,src   /* src=>esi   */

		/*
		if(sx<0)
			src_x=-sx,sx=0;
		else if(sx<des->w)
			src_x=0;
		else
			return;
		*/
		cmp  eax,0
		jge  _x_ge0
		neg  eax
		mov  sx,0
		mov  src_x,eax
		jmp  _x_end
_x_ge0:
		cmp  eax,[edi]BMP.w
		jge  _end
		mov  src_x,0
_x_end:

		/*
		if(sy<0)
			src_y=-sy,sy=0;
		else if(sy<des->h)
			src_y=0;
		else
			return;
		*/
		cmp  ebx,0
		jge  _y_ge0
		neg  ebx
		mov  sy,0
		mov  src_y,ebx
		jmp  _y_end
_y_ge0:
		cmp  ebx,[edi]BMP.h
		jge  _end
		mov  src_y,0
_y_end:

		/*
		h=src->h-src_y;
		if(h>des->h-sy) h=des->h-sy;
		if(h<=0) return;
		*/
		mov  eax,[esi]BMP.h   /* src->h=>eax */
		mov  ecx,[edi]BMP.h   /* des->h=>ecx */
		mov  ebx,src_y          /* src_y=>ebx  */
		mov  edx,sy          /* sy=>edx  */
		sub  eax,ebx
		sub  ecx,edx
		cmp  eax,ecx
		jle  _h_le
		mov  eax,ecx
_h_le:
		cmp  eax,0
		jle  _end
		mov  h,eax

		/*
		w=src->w-src_x;
		if(w>des->w-sx) w=des->w-sx;
		if(w<=0) return;
		*/
		mov  eax,[esi]BMP.w /* src->w=>eax */
		mov  ecx,[edi]BMP.w /* des->w=>ecx */
		mov  ebx,src_x        /* src_x=>ebx  */
		mov  edx,sx        /* sx=>edx  */
		sub  eax,ebx
		sub  ecx,edx
		cmp  eax,ecx
		jle  _w_le
		mov  eax,ecx
_w_le:
		cmp  eax,0
		jle  _end
		mov  w,eax

		mov  ebx,[esi]BMP.pitch /* src->pitch=>ebx */
		shl  eax,1                /* w*sizeof(pixel_t)=>eax */
		mov  edx,[edi]BMP.pitch /* des->pitch=>edx */
		sub  ebx,eax
		sub  edx,eax
		mov  eax,src_y /* src_y=>eax */
		mov  ecx,sy /* sy=>ecx */
		mov  esi,[esi+eax*4+BMP.line] /* src->line[src_y]=>esi */
		mov  edi,[edi+ecx*4+BMP.line] /* des->line[sy]=>edi */
		mov  eax,src_x /* src_x=>eax */
		mov  ecx,sx /* sx=>ecx */
		lea  esi,[esi+eax*2] /* &src->line[src_y][src_x]=>esi */
		lea  edi,[edi+ecx*2] /* &des->line[sy][sx]=>edi */

		movq  mm5,quotiety_red
		mov   eax,h
		movq  mm6,quotiety_green
		mov   ecx,w
		movq  mm7,quotiety_blue
		/*((r*30+g*59+b*11)/100);*/
		/*((r*38.4+g*75.52+b*14.08)/128);*/
		ALIGN 4
_loop:
		movq mm0,[esi]
		movq mm1,mm0
		movq mm2,mm0

		pand mm0,mmx_mask_red
		pand mm1,mmx_mask_green
		pand mm2,mmx_mask_blue

		psrlw mm0,11
		psrlw mm1,6
		psrlw mm2,0

		pmullw mm0,mm5
		pmullw mm1,mm6
		pmullw mm2,mm7

		paddusw mm0,mm1
		paddusw mm0,mm2

		psrlw mm0,7
		movq  mm1,mm0
		movq  mm2,mm0
		psllw mm0,11
		psllw mm1,6

		por   mm0,mm1
		por   mm0,mm2

		cmp  ecx,4
		jl   _line_end

		movq [edi],mm0
		add  esi,8
		add  edi,8
		sub  ecx,4
		jnz  _loop
		ALIGN 4
		jmp  _next_line

		ALIGN 4
_line_end:
		add  esi,ecx
		movq buf,mm0
		add  esi,ecx
		mov  t,esi
		lea  esi,buf
		rep  movsw
		mov  esi,t

		ALIGN 4
_next_line:
		add  esi,ebx
		add  edi,edx
		dec  eax
		mov  ecx,w
		jnz  _loop
		emms
_end:
	}
}


void	_put_bitmap_mmx(SLONG sx,SLONG sy,BMP *src,BMP *des)
{
	SLONG  src_x,src_y;
	SLONG  w,h;

	__asm
	{
		mov  eax,sx  /* sx=>eax */
		mov  ebx,sy  /* sy=>ebx */
		mov  edi,des    /* des=>edi */
		mov  esi,src    /* src=>esi */

		/*
		if(sx<0)
			src_x=-sx,sx=0;
		else if(sx<des->w)
			src_x=0;
		else
			return;
		*/
		cmp  eax,0
		jge  _x_ge0
		neg  eax
		mov  sx,0
		mov  src_x,eax
		jmp  _x_end
_x_ge0:
		cmp  eax,[edi]BMP.w
		jge  _end
		mov  src_x,0
_x_end:


		/*
		if(sy<0)
			src_y=-sy,sy=0;
		else if(sy<des->h)
			src_y=0;
		else
			return;
		*/
		cmp  ebx,0
		jge  _y_ge0
		neg  ebx
		mov  sy,0
		mov  src_y,ebx
		jmp  _y_end
_y_ge0:
		cmp  ebx,[edi]BMP.h
		jge  _end
		mov  src_y,0
_y_end:


		/*
		h=src->h-src_y;
		if(h>des->h-sy) h=des->h-sy;
		if(h<=0) return;
		*/
		mov  eax,[esi]BMP.h
		mov  ecx,[edi]BMP.h
		mov  ebx,src_y
		mov  edx,sy
		sub  eax,ebx
		sub  ecx,edx
		cmp  eax,ecx
		jle  _h_le
		mov  eax,ecx
_h_le:
		cmp  eax,0
		jle  _end
		mov  h,eax

		/*
		w=src->w-src_x;
		if(w>des->w-sx) w=des->w-sx;
		if(w<=0) return;
		*/
		mov  eax,[esi]BMP.w
		mov  ecx,[edi]BMP.w
		mov  ebx,src_x
		mov  edx,sx
		sub  eax,ebx
		sub  ecx,edx
		cmp  eax,ecx
		jle  _w_le
		mov  eax,ecx
_w_le:
		cmp  eax,0
		jle  _end
		mov  w,eax

		mov  ebx,[esi]BMP.pitch /* src->pitch=>ebx */
		shl  eax,1                /* w*sizeof(pixel_t)=>eax */
		mov  edx,[edi]BMP.pitch /* des->pitch=>edx */
		sub  ebx,eax
		sub  edx,eax
		mov  eax,src_y
		mov  ecx,sy
		mov  esi,[esi+eax*4+BMP.line]
		mov  edi,[edi+ecx*4+BMP.line]
		mov  eax,src_x
		mov  ecx,sx
		lea  esi,[esi+eax*2]  /* &src->line[src_y][src_x]=>esi */
		lea  edi,[edi+ecx*2]  /* &des->line[sy][sx]=>edi */

		mov  ecx,w
		mov  eax,h

		cmp  ecx,8
		jl   _next_line

		ALIGN 4
_loop:
		movq mm0,[esi]
		movq mm1,[esi+8]
		sub  ecx,8
		movq [edi],mm0
		add  esi,16
		movq [edi+8],mm1
		add  edi,16
		cmp  ecx,8
		jge  _loop
		rep  movsw
		add  esi,ebx
		add  edi,edx
		mov  ecx,w
		dec  eax
		jnz  _loop
		emms
		jmp  _end

		ALIGN 4
_next_line:
		cmp  ecx,4
		jl   _last_pixel
		movq mm0,[esi]
		sub  ecx,4
		movq [edi],mm0
		add  esi,8
		add  edi,8
		ALIGN 4
_last_pixel:
		rep  movsw
		add  esi,ebx
		add  edi,edx
		mov  ecx,w
		dec  eax
		jnz  _next_line
		emms
_end:
	}
}


void	_additive_put_bitmap_mmx(SLONG sx,SLONG sy,BMP *src,BMP *des)
{
	SLONG  src_x,src_y;
	SLONG  w,h;
	void *t;
	__int64 buf;

	__asm
	{
		mov  eax,sx /* sx=>eax */
		mov  ebx,sy /* src_x=>ebx */
		mov  edi,des   /* des=>edi   */
		mov  esi,src   /* src=>esi   */

		/*
		if(sx<0)
			src_x=-sx,sx=0;
		else if(sx<des->w)
			src_x=0;
		else
			return;
		*/
		cmp  eax,0
		jge  _x_ge0
		neg  eax
		mov  sx,0
		mov  src_x,eax
		jmp  _x_end
_x_ge0:
		cmp  eax,[edi]BMP.w
		jge  _end
		mov  src_x,0
_x_end:

		/*
		if(sy<0)
			src_y=-sy,sy=0;
		else if(sy<des->h)
			src_y=0;
		else
			return;
		*/
		cmp  ebx,0
		jge  _y_ge0
		neg  ebx
		mov  sy,0
		mov  src_y,ebx
		jmp  _y_end
_y_ge0:
		cmp  ebx,[edi]BMP.h
		jge  _end
		mov  src_y,0
_y_end:

		/*
		h=src->h-src_y;
		if(h>des->h-sy) h=des->h-sy;
		if(h<=0) return;
		*/
		mov  eax,[esi]BMP.h   /* src->h=>eax */
		mov  ecx,[edi]BMP.h   /* des->h=>ecx */
		mov  ebx,src_y          /* src_y=>ebx  */
		mov  edx,sy          /* sy=>edx  */
		sub  eax,ebx
		sub  ecx,edx
		cmp  eax,ecx
		jle  _h_le
		mov  eax,ecx
_h_le:
		cmp  eax,0
		jle  _end
		mov  h,eax

		/*
		w=src->w-src_x;
		if(w>des->w-sx) w=des->w-sx;
		if(w<=0) return;
		*/
		mov  eax,[esi]BMP.w /* src->w=>eax */
		mov  ecx,[edi]BMP.w /* des->w=>ecx */
		mov  ebx,src_x        /* src_x=>ebx  */
		mov  edx,sx        /* sx=>edx  */
		sub  eax,ebx
		sub  ecx,edx
		cmp  eax,ecx
		jle  _w_le
		mov  eax,ecx
_w_le:
		cmp  eax,0
		jle  _end
		mov  w,eax

		mov  ebx,[esi]BMP.pitch /* src->pitch=>ebx */
		shl  eax,1                /* w*sizeof(pixel_t)=>eax */
		mov  edx,[edi]BMP.pitch /* des->pitch=>edx */
		sub  ebx,eax
		sub  edx,eax
		mov  eax,src_y /* src_y=>eax */
		mov  ecx,sy /* sy=>ecx */
		mov  esi,[esi+eax*4+BMP.line] /* src->line[src_y]=>esi */
		mov  edi,[edi+ecx*4+BMP.line] /* des->line[sy]=>edi */
		mov  eax,src_x /* src_x=>eax */
		mov  ecx,sx /* sx=>ecx */
		lea  esi,[esi+eax*2] /* &src->line[src_y][src_x]=>esi */
		lea  edi,[edi+ecx*2] /* &des->line[sy][sx]=>edi */

		mov   eax,h
		mov   ecx,w

		movq  mm6,mmx_mask_red
		movq  mm7,mmx_mask_green

		ALIGN 4
_loop:
		movq mm0,[esi]
		movq mm3,[edi]
		movq mm1,mm0
		movq mm4,mm3
		movq mm2,mm0
		movq mm5,mm3

		pand mm0,mm6
		pand mm1,mm7
		pand mm2,mmx_mask_blue
		pand mm3,mm6
		pand mm4,mm7
		pand mm5,mmx_mask_blue

		psllw mm0,mmx_offset_bit_red
		psllw mm1,mmx_offset_bit_green
		psllw mm2,mmx_offset_bit_blue
		psllw mm3,mmx_offset_bit_red
		psllw mm4,mmx_offset_bit_green
		psllw mm5,mmx_offset_bit_blue

		paddusb mm0,mm3
		paddusb mm1,mm4
		paddusb mm2,mm5

		pand mm0,mm6
		pand mm1,mm7
		pand mm2,mmx_mask_blue

		psrlw mm0,mmx_offset_bit_red
		psrlw mm1,mmx_offset_bit_green
		psrlw mm2,mmx_offset_bit_blue

		por mm0,mm1
		por mm0,mm2

		cmp  ecx,4
		jl   _line_end

		movq [edi],mm0
		add  esi,8
		add  edi,8
		sub  ecx,4
		jnz  _loop
		ALIGN 4
		jmp  _next_line

		ALIGN 4
_line_end:
		add  esi,ecx
		movq buf,mm0
		add  esi,ecx
		mov  t,esi
		lea  esi,buf
		rep  movsw
		mov  esi,t

		ALIGN 4
_next_line:
		add  esi,ebx
		add  edi,edx
		dec  eax
		mov  ecx,w
		jnz  _loop
		emms
_end:
	}
}

void	_subtractive_put_bitmap_mmx(SLONG sx,SLONG sy,BMP *src,BMP *des)
{
	SLONG  src_x,src_y;
	SLONG  w,h;
	void   *t;
	__int64 buf;

	__asm
	{
		mov  eax,sx /* sx=>eax */
		mov  ebx,sy /* src_x=>ebx */
		mov  edi,des   /* des=>edi   */
		mov  esi,src   /* src=>esi   */

		/*
		if(sx<0)
			src_x=-sx,sx=0;
		else if(sx<des->w)
			src_x=0;
		else
			return;
		*/
		cmp  eax,0
		jge  _x_ge0
		neg  eax
		mov  sx,0
		mov  src_x,eax
		jmp  _x_end
_x_ge0:
		cmp  eax,[edi]BMP.w
		jge  _end
		mov  src_x,0
_x_end:

		/*
		if(sy<0)
			src_y=-sy,sy=0;
		else if(sy<des->h)
			src_y=0;
		else
			return;
		*/
		cmp  ebx,0
		jge  _y_ge0
		neg  ebx
		mov  sy,0
		mov  src_y,ebx
		jmp  _y_end
_y_ge0:
		cmp  ebx,[edi]BMP.h
		jge  _end
		mov  src_y,0
_y_end:

		/*
		h=src->h-src_y;
		if(h>des->h-sy) h=des->h-sy;
		if(h<=0) return;
		*/
		mov  eax,[esi]BMP.h   /* src->h=>eax */
		mov  ecx,[edi]BMP.h   /* des->h=>ecx */
		mov  ebx,src_y          /* src_y=>ebx  */
		mov  edx,sy          /* sy=>edx  */
		sub  eax,ebx
		sub  ecx,edx
		cmp  eax,ecx
		jle  _h_le
		mov  eax,ecx
_h_le:
		cmp  eax,0
		jle  _end
		mov  h,eax

		/*
		w=src->w-src_x;
		if(w>des->w-sx) w=des->w-sx;
		if(w<=0) return;
		*/
		mov  eax,[esi]BMP.w /* src->w=>eax */
		mov  ecx,[edi]BMP.w /* des->w=>ecx */
		mov  ebx,src_x        /* src_x=>ebx  */
		mov  edx,sx        /* sx=>edx  */
		sub  eax,ebx
		sub  ecx,edx
		cmp  eax,ecx
		jle  _w_le
		mov  eax,ecx
_w_le:
		cmp  eax,0
		jle  _end
		mov  w,eax

		mov  ebx,[esi]BMP.pitch /* src->pitch=>ebx */
		shl  eax,1                /* w*sizeof(pixel_t)=>eax */
		mov  edx,[edi]BMP.pitch /* des->pitch=>edx */
		sub  ebx,eax
		sub  edx,eax
		mov  eax,src_y /* src_y=>eax */
		mov  ecx,sy /* sy=>ecx */
		mov  esi,[esi+eax*4+BMP.line] /* src->line[src_y]=>esi */
		mov  edi,[edi+ecx*4+BMP.line] /* des->line[sy]=>edi */
		mov  eax,src_x /* src_x=>eax */
		mov  ecx,sx /* sx=>ecx */
		lea  esi,[esi+eax*2] /* &src->line[src_y][src_x]=>esi */
		lea  edi,[edi+ecx*2] /* &des->line[sy][sx]=>edi */

		mov   eax,h
		mov   ecx,w

		movq mm6,mmx_mask_red
		movq mm7,mmx_mask_green

_loop:
		ALIGN 4
		movq mm0,[esi]
		movq mm3,[edi]
		movq mm1,mm0
		movq mm4,mm3
		movq mm2,mm0
		movq mm5,mm3

		pand mm0,mm6
		pand mm1,mm7
		pand mm2,mmx_mask_blue
		pand mm3,mm6
		pand mm4,mm7
		pand mm5,mmx_mask_blue

		psllw mm0,mmx_offset_bit_red
		psllw mm1,mmx_offset_bit_green
		psllw mm2,mmx_offset_bit_blue
		psllw mm3,mmx_offset_bit_red
		psllw mm4,mmx_offset_bit_green
		psllw mm5,mmx_offset_bit_blue

		psubusb mm0,mm3
		psubusb mm1,mm4
		psubusb mm2,mm5

		psrlw mm0,mmx_offset_bit_red
		psrlw mm1,mmx_offset_bit_green
		psrlw mm2,mmx_offset_bit_blue

		por mm0,mm1
		por mm0,mm2

		cmp  ecx,4
		jl   _line_end

		movq [edi],mm0
		add  esi,8
		add  edi,8
		sub  ecx,4
		jnz  _loop
		ALIGN 4
		jmp  _next_line

		ALIGN 4
_line_end:
		add  esi,ecx
		movq buf,mm0
		add  esi,ecx
		mov  t,esi
		lea  esi,buf
		rep  movsw
		mov  esi,t

		ALIGN 4
_next_line:
		add  esi,ebx
		add  edi,edx
		dec  eax
		mov  ecx,w
		jnz  _loop
		emms
_end:
	}
}


void	_bound_put_bitmap_mmx(SLONG sx, SLONG sy, SLONG bound_sx, SLONG bound_sy, SLONG bound_xl, SLONG bound_yl, BMP *src_bmp, BMP *dest_bmp)
{
	__asm
	{
		mov  esi,dword ptr src_bmp
		mov  edi,dword ptr dest_bmp

		/*
		if(sx<0)
		{
			bound_sx-=sx,
			bound_xl+=sx,
			sx=0;
		}
		else if(sx<(i=dest_bmp->w))
		{
			if(sx+bound_xl>i)
				bound_xl=i-sx;
		}
		else
			return;
		*/
		mov  eax,dword ptr sx
		mov  ebx,dword ptr bound_sx
		cmp  eax,0
		jge  _elseif_sx
		mov  ecx,dword ptr bound_xl
		sub  ebx,eax
		add  ecx,eax
		mov  dword ptr bound_sx,ebx
		mov  dword ptr bound_xl,ecx
		mov  dword ptr sx,0
		jmp  _endif_sx
_elseif_sx:
		mov  edx,[edi]BMP.w
		mov  ecx,dword ptr bound_xl
		cmp  eax,edx
		jge  _end
		add  eax,ecx
		mov  ebx,dword ptr sx
		cmp  eax,edx
		jle  _endif_sx
		sub  edx,ebx
		mov  dword ptr bound_xl,edx
_endif_sx:

		/*
		if(sy<0)
		{
			bound_sy-=sy,
			bound_yl+=sy,
			sy=0;
		}
		else if(sy<(i=dest_bmp->h))
		{
			if(sy+bound_yl>i)
				bound_yl=i-sy;
		}
		else
			return;
		*/
		mov  eax,dword ptr sy
		mov  ebx,dword ptr bound_sy
		cmp  eax,0
		jge  _elseif_sy
		mov  ecx,dword ptr bound_yl
		sub  ebx,eax
		add  ecx,eax
		mov  dword ptr bound_sy,ebx
		mov  dword ptr bound_yl,ecx
		mov  dword ptr sy,0
		jmp  _endif_sy
_elseif_sy:
		mov  edx,[edi]BMP.h
		mov  ecx,dword ptr bound_yl
		cmp  eax,edx
		jge  _end
		add  eax,ecx
		mov  ebx,dword ptr sy
		cmp  eax,edx
		jle  _endif_sy
		sub  edx,ebx
		mov  dword ptr bound_yl,edx
_endif_sy:

		/*
		if(bound_xl>(i=src_bmp->w)) bound_xl=i;
		if(bound_sx<0)
		{
			sx-=bound_sx,
			bound_xl+=bound_sx,
			bound_sx=0;
		}
		else if(bound_sx<i)
		{
			if(bound_sx+bound_xl>i)
				bound_xl=i-bound_sx;
		}
		else return;
		if(bound_xl<=0) return;
		*/
		mov  ecx,dword ptr bound_xl
		mov  edx,[esi]BMP.w
		cmp  ecx,edx
		jle  _endif_bound_xl
		mov  ecx,edx
_endif_bound_xl:

		mov  eax,dword ptr bound_sx
		mov  ebx,dword ptr sx
		cmp  eax,0
		jge  _elseif_bound_sx
		sub  ebx,eax
		add  ecx,eax
		mov  dword ptr sx,ebx
		mov  dword ptr bound_sx,0
		jmp  _endif_bound_sx
_elseif_bound_sx:
		cmp  eax,edx
		jge  _end
		add  eax,ecx
		mov  ebx,dword ptr bound_sx
		cmp  eax,edx
		jle  _endif_bound_sx
		sub  edx,ebx
		mov  ecx,edx
_endif_bound_sx:

		cmp  ecx,0
		jle  _end
		mov  dword ptr bound_xl,ecx

		/*
		if(bound_yl>(i=src_bmp->h)) bound_yl=i;
		if(bound_sy<0)
		{
			sy-=bound_sy,
			bound_yl+=bound_sy,
			bound_sy=0;
		}
		else if(bound_sy<i)
		{
			if(bound_sy+bound_yl>i)
				bound_yl=i-bound_sy;
		}
		else return;
		if(bound_yl<=0) return;
		*/
		mov  ecx,dword ptr bound_yl
		mov  edx,[esi]BMP.h
		cmp  ecx,edx
		jle  _endif_bound_yl
		mov  ecx,edx
_endif_bound_yl:

		mov  eax,dword ptr bound_sy
		mov  ebx,dword ptr sy
		cmp  eax,0
		jge  _elseif_bound_sy
		sub  ebx,eax
		add  ecx,eax
		mov  dword ptr sy,ebx
		mov  dword ptr bound_sy,0
		jmp  _endif_bound_sy
_elseif_bound_sy:
		cmp  eax,edx
		jge  _end
		add  eax,ecx
		mov  ebx,dword ptr bound_sy
		cmp  eax,edx
		jle  _endif_bound_sy
		sub  edx,ebx
		mov  ecx,edx
_endif_bound_sy:

		cmp  ecx,0
		jle  _end
		mov  dword ptr bound_yl,ecx

		/*
		i=bound_xl<<1;
		src_bmp_line=&src_bmp->line[bound_sy][bound_sx];
		src_bmp_pitch=src_bmp->pitch;
		des_line=&dest_bmp->line[sy][sx];
		des_pitch=dest_bmp->pitch;
		while(bound_yl--)
		{
			memcpy(des_line,src_bmp_line,i);
			((char*)des_line)+=des_pitch,
			((char*)src_bmp_line)+=src_bmp_pitch;
		}
		*/
		mov  ecx,dword ptr bound_sy
		mov  eax,dword ptr bound_sx
		mov  ebx,[esi]BMP.pitch
		mov  esi,[esi+ecx*4]BMP.line
		mov  ecx,dword ptr bound_xl
		shl  ecx,1
		lea  esi,[esi+eax*2]
		sub  ebx,ecx
		
		mov  ecx,dword ptr sy
		mov  eax,dword ptr sx
		mov  edx,[edi]BMP.pitch
		mov  edi,[edi+ecx*4]BMP.line
		mov  ecx,dword ptr bound_xl
		shl  ecx,1
		lea  edi,[edi+eax*2]
		sub  edx,ecx

		mov  ecx,dword ptr bound_xl
		mov  eax,dword ptr bound_yl

		cmp  ecx,8
		jge  _loop

		ALIGN 4
_next_line:
		cmp  ecx,4
		jl   _last_pixel
		movq mm0,[esi]
		sub  ecx,4
		movq [edi],mm0
		add  esi,8
		add  edi,8
		ALIGN 4
_last_pixel:
		rep  movsw
		add  esi,ebx
		add  edi,edx
		mov  ecx,dword ptr bound_xl
		dec  eax
		jnz  _next_line
		emms
		jmp  _end

		ALIGN  4
_loop:
		movq mm0,[esi]
		movq mm1,[esi+8]
		sub  ecx,8
		movq [edi],mm0
		add  esi,16
		movq [edi+8],mm1
		add  edi,16
		cmp  ecx,8
		jge  _loop
		rep  movsw
		add  esi,ebx
		add  edi,edx
		mov  ecx,dword ptr bound_xl
		dec  eax
		jnz  _loop
		emms
_end:
	}
}



void	_adulterate_bitmap_mmx(SLONG r,SLONG g,SLONG b,BMP *bmp)
{
	__asm
	{
		mov  esi,bmp
		mov  eax,r
		mov  ecx,g
		shl  eax,8
		shl  ecx,8
		mov  ebx,eax
		mov  edx,ecx
		shl  eax,16
		shl  ecx,16
		or   eax,ebx
		or   ecx,edx
		mov  ebx,b
		movd mm0,eax
		mov  edx,ebx
		movd mm1,ecx
		shl  ebx,16
		movd mm3,eax
		or   ebx,edx
		movd mm4,ecx
		movd mm2,ebx
		punpckldq mm0,mm3
		movd mm5,ebx
		punpckldq mm1,mm4
		punpckldq mm2,mm5

		mov  ecx,[esi]BMP.h
		mov  eax,[esi]BMP.pitch

		mul  ecx

		shr  eax,3
		mov  edi,[esi]BMP.line

		cmp  eax,0
		jle  _end

		movq mm6,mmx_mask_red
		movq mm7,mmx_mask_green

_loop:
		ALIGN 4
		movq mm3,[edi]
		movq mm4,mm3
		movq mm5,mm3
		pand mm3,mm6
		pand mm4,mm7
		pand mm5,mmx_mask_blue
		psllw mm4,mmx_offset_bit_green
		psllw mm5,mmx_offset_bit_blue
		paddusb mm3,mm0
		paddusb mm4,mm1
		paddusb mm5,mm2
		psrlw mm4,mmx_offset_bit_green
		psrlw mm5,mmx_offset_bit_blue
		pand mm3,mm6
		pand mm4,mm7
		pand mm5,mmx_mask_blue
		por mm3,mm4
		por mm3,mm5
		movq [edi],mm3
		add  edi,8
		dec  eax
		jnz  _loop
_end:
		emms
	}
}


void	_eclipse_bitmap_mmx(SLONG r,SLONG g,SLONG b,BMP *bmp)
{
	__asm
	{
		mov  esi,bmp

		mov  eax,r
		mov  ecx,g
		shl  eax,8
		shl  ecx,8
		mov  ebx,eax
		mov  edx,ecx
		shl  eax,16
		shl  ecx,16
		or   eax,ebx
		or   ecx,edx
		mov  ebx,b
		movd mm0,eax
		mov  edx,ebx
		movd mm1,ecx
		shl  ebx,16
		movd mm3,eax
		or   ebx,edx
		movd mm4,ecx
		movd mm2,ebx
		punpckldq mm0,mm3
		movd mm5,ebx
		punpckldq mm1,mm4
		punpckldq mm2,mm5

		mov  ecx,[esi]BMP.h
		mov  eax,[esi]BMP.pitch

		mul  ecx

		shr  eax,3
		mov  edi,[esi]BMP.line

		cmp  eax,0
		jle  _end
		
		movq mm6,mmx_mask_red
		movq mm7,mmx_mask_green

_loop:
		ALIGN 4
		movq mm3,[edi]
		movq mm4,mm3
		movq mm5,mm3
		pand mm3,mm6
		pand mm4,mm7
		pand mm5,mmx_mask_blue
		psllw mm4,mmx_offset_bit_green
		psllw mm5,mmx_offset_bit_blue
		psubusb mm3,mm0
		psubusb mm4,mm1
		psubusb mm5,mm2
		psrlw mm4,mmx_offset_bit_green
		psrlw mm5,mmx_offset_bit_blue
		pand mm3,mm6
		pand mm4,mm7
		pand mm5,mmx_mask_blue
		por mm3,mm4
		por mm3,mm5
		movq [edi],mm3
		add  edi,8
		dec  eax
		jnz  _loop
_end:
		emms
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//NONE MMX(JUST ASM) FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#define _ASM_

static	void    _range_put_rle_asm(SLONG des_x,SLONG des_y,RLE *rle,BMP *bmp)
{
	int  w,h;
	int  src_x;
	int  src_y;
	int  src_ex;
	int  src_ey;
	int  i,x;
	unsigned char *data;
	int  count,info;
	PIXEL  pix;
	PIXEL  *line;

#ifndef _ASM_
	if(des_x<0)
		src_x=-des_x,des_x=0;
	else if(des_x<bmp->w)
		src_x=0;
	else
		return;
#else
	__asm
	{
		mov  esi,rle   // 取源rle的地址
		mov  edi,bmp   // 取目标bmp的地址
		mov  eax,des_x
		mov  ebx,des_y

		cmp  eax,0
		jge  _x_ge
		neg  eax
		mov  des_x,0
		mov  src_x,eax
		jmp  _x_end
_x_ge:
		cmp  eax,[edi]BMP.w
		jge  _end
		mov  src_x,0
_x_end:
	}
#endif

#ifndef _ASM_
	if(des_y<0)
		src_y=-des_y,des_y=0;
	else if(des_y<bmp->h)
		src_y=0;
	else
		return;
#else
	__asm
	{
		cmp  ebx,0
		jge  _y_ge
		neg  ebx
		mov  des_y,0
		mov  src_y,ebx
		jmp  _y_end
_y_ge:
		cmp  ebx,[edi]BMP.h
		jge  _end
		mov  src_y,0
_y_end:
	}
#endif

#ifndef _ASM_
	h=rle->h-src_y;
	if(h>bmp->h-des_y) h=bmp->h-des_y;
	if(h<=0) return;
#else
	__asm
	{
		mov  eax,[esi]RLE.h
		mov  ebx,src_y
		mov  ecx,[edi]BMP.h
		mov  edx,des_y
		sub  eax,ebx
		sub  ecx,edx
		cmp  eax,ecx
		jle  _h_le
		mov  eax,ecx
_h_le:
		cmp  eax,0
		jle  _end
		mov  h,eax
	}
#endif

#ifndef _ASM_
	w=rle->w-src_x;
	if(w>bmp->w-des_x) w=bmp->w-des_x;
	if(w<=0) return;
#else
	__asm
	{
		mov  eax,[esi]RLE.w
		mov  ebx,src_x
		mov  ecx,[edi]BMP.w
		mov  edx,des_x
		sub  eax,ebx
		sub  ecx,edx
		cmp  eax,ecx
		jle  _w_le
		mov  eax,ecx
_w_le:
		cmp  eax,0
		jle  _end
		mov  w,eax
	}
#endif

#ifndef _ASM_
	src_ex=src_x+w;
	src_ey=src_y+h;
#else
	__asm
	{
		mov  eax,src_x
		mov  ebx,w
		mov  ecx,src_y
		mov  edx,h
		add  eax,ebx
		add  ecx,edx
		mov  src_ex,eax
		mov  src_ey,ecx
	}
#endif

	while(src_y<src_ey)
	{
#ifndef _ASM_
		line=&bmp->line[des_y][des_x];
		data=rle->line[src_y];
		x=0;
#else
		__asm
		{
			mov  edi,bmp
			mov  eax,des_y
			mov  ebx,des_x
			mov  edx,[edi+eax*4]BMP.line
			mov  esi,rle
			mov  ecx,src_y
			lea  edi,[edx+ebx*2]
			mov  eax,[esi+ecx*4]RLE.line
			mov  line,edi
			xor  ecx,ecx
			mov  data,eax
			mov  x,ecx
		}
#endif

		while(x<src_x)
		{
#ifndef _ASM_
			count=data[0];
			info=data[1];
			data+=2;
#else
			__asm
			{
				mov  eax,data
				mov  ebx,data
				add  ebx,2
				xor  ecx,ecx
				xor  edx,edx
				mov  cl,byte ptr [eax]
				mov  data,ebx
				mov  dl,byte ptr [eax+1]
				mov  count,ecx
				mov  info,edx
			}
#endif

			if(count)
			{
				x+=count;
				if(x<=src_x)
				{
					data+=sizeof(PIXEL);
				}
				else
				{
					if(x<=src_ex)
						count=x-src_x;
					else
						count=w;
					pix=*((PIXEL*)data);
					data+=sizeof(PIXEL);
					if(info&RLE_SHADOW)
					{
						while(count-->0)
						{
                            // Jack, for SeaAge. [6/21/2002]
							//oper_dark_color(line++,15);
							oper_dark_color(line++,RLE_SHADOW_DARK);
						}
					}
					else if(info&RLE_IMAGE)
					{
#ifndef _ASM_
						while(count-->0)
						{
							*line++=pix;
						}
#else
						__asm
						{
							mov  ecx,count
							xor  eax,eax
							mov  edi,line
							mov  ax,pix
_while_0:
							ALIGN 4
							mov  [edi],ax
							add  edi,2
							dec  ecx
							jnz  _while_0
							mov  line,edi
						}
#endif
					}
					else
					{
						info&=RLE_ALPHA;
						while(count-->0)
						{
							oper_alpha_color(&pix,line++,info);
						}
					}
				}
			}
			else
			{
				switch(info)
				{
				case 0://end of line
					goto _next_line;
					break;
				case 1://end of image
					return;
					break;
				case 2://transfered meaning
#ifndef _ASM_
					x+=data[0];
					des_y+=data[1];
					src_y+=data[1];
					data+=2;
					if(x>src_x)
						line+=(x-src_x);
#else
					__asm
					{
						mov  esi,data
						xor  eax,eax
						mov  ecx,x
						mov  al,[esi]
						xor  ebx,ebx
						mov  edx,des_y
						mov  bl,[esi+1]
						add  ecx,eax
						add  edx,ebx
						mov  x,ecx
						mov  eax,src_y
						mov  des_y,edx
						add  eax,ebx
						mov  ebx,data
						mov  src_y,eax
						add  ebx,2
						mov  eax,src_x
						mov  data,ebx
						cmp  ecx,eax
						jle  _if_0
						sub  ecx,eax
						mov  ebx,line
						lea  eax,[ebx+ecx*2]
						mov  line,eax
_if_0:
					}
#endif
					break;
				default:// copy pixels
					x+=info;
					if(x<=src_x)
					{
						data+=(info*sizeof(PIXEL));
					}
					else
					{
						if(x<=src_ex)
						{
							count=x-src_x;
							i=info-count;
						}
						else
						{
							count=w;
							i=w;
						}
						memcpy(line,data+i*sizeof(PIXEL),count*SIZEOFPIXEL);
						line+=count;
						data+=(info*sizeof(PIXEL));
					}
					break;
				}
			}
		}
		while(x<src_ex)
		{
			count=data[0];
			info=data[1];
			data+=2;
			if(count)
			{
				x+=count;
				if(x>src_ex)
					count-=(x-src_ex);
				pix=*((PIXEL*)data);
				data+=sizeof(PIXEL);
				if(info&RLE_SHADOW)
				{
					while(count-->0)
					{
                        // Jack, for SeaAge. [6/21/2002]
						//oper_dark_color(line++,15);
						oper_dark_color(line++,RLE_SHADOW_DARK);
					}
				}
				else if(info&RLE_IMAGE)
				{
#ifndef _ASM_
					while(count-->0)
					{
						*line++=pix;
					}
#else
					__asm
					{
						mov  ecx,count
						xor  eax,eax
						mov  edi,line
						mov  ax,pix
_while_1:
						ALIGN 4
						mov  [edi],ax
						add  edi,2
						dec  ecx
						jnz  _while_1
						mov  line,edi
					}
#endif
				}
				else
				{
					info&=RLE_ALPHA;
					while(count-->0)
					{
						oper_alpha_color(&pix,line++,info);
					}
				}
			}
			else
			{
				switch(info)
				{
				case 0://end of line
					goto _next_line;
					break;
				case 1://end of image
					return;
					break;
				case 2://transfered meaning
#ifndef _ASM_
					x+=data[0];
					line+=data[0];
					des_y+=data[1];
					src_y+=data[1];
					data+=2;
#else
					__asm
					{
						mov  esi,data
						xor  eax,eax
						mov  ecx,x
						mov  al,[esi]
						xor  ebx,ebx
						mov  edx,des_y
						mov  bl,[esi+1]
						add  ecx,eax
						mov  edi,line
						add  edx,ebx
						lea  esi,[eax*2+edi]
						mov  x,ecx
						mov  line,esi
						mov  eax,src_y
						mov  des_y,edx
						mov  ecx,data
						add  eax,ebx
						add  ecx,2
						mov  src_y,eax
						mov  data,ecx
					}
#endif
					break;
				default:// copy pixels
					x+=info;
					if(x>src_ex)
						info-=(x-src_ex);
					memcpy(line,data,info*SIZEOFPIXEL);
					line+=info;
					data+=(info*sizeof(PIXEL));
					break;
				}
			}
		}
_next_line:
#ifndef _ASM_
		src_y++;
		des_y++;
#else
		__asm
		{
			mov  eax,src_y
			mov  ebx,des_y
			inc  eax
			inc  ebx
			mov  src_y,eax
			mov  des_y,ebx
		}
#endif
	}
#ifdef _ASM_
_end:;
#endif
}


static	void    _direct_put_rle_asm(SLONG des_x,SLONG des_y,RLE *rle,BMP *bmp)
{
	int  w,h;
	int  src_x;
	int  src_y;
	int  src_ex;
	int  src_ey;
	int  x;
	unsigned char *data;
	int  count,info;
	PIXEL  pix;
	PIXEL  *line;

#ifndef _ASM_
	src_x=0;
	src_y=0;
	w=rle->w;
	h=rle->h;
	src_ex=w;
	src_ey=h;
#else
	__asm
	{
		mov  src_x,0
		mov  edx,rle
		mov  src_y,0
		mov  eax,[edx]RLE.w
		mov  ebx,[edx]RLE.h
		mov  w,eax
		mov  h,ebx
		mov  src_ex,eax
		mov  src_ey,ebx
	}
#endif

	while(src_y<src_ey)
	{
#ifndef _ASM_
		line=&bmp->line[des_y][des_x];
		data=rle->line[src_y];
		x=0;
#else
		__asm
		{
			mov  edi,bmp
			mov  eax,des_y
			mov  ebx,des_x
			mov  edx,[edi+eax*4]BMP.line
			mov  esi,rle
			mov  ecx,src_y
			lea  edi,[edx+ebx*2]
			mov  eax,[esi+ecx*4]RLE.line
			mov  line,edi
			xor  ecx,ecx
			mov  data,eax
			mov  x,ecx
		}
#endif

		while(x<src_ex)
		{
			count=data[0];
			info=data[1];
			data+=2;
			if(count)
			{
				x+=count;
				pix=*((PIXEL*)data);
				data+=sizeof(PIXEL);
				if(info&RLE_SHADOW)
				{
					while(count-->0)
					{
                        // Jack, for SeaAge. [6/21/2002]
						//oper_dark_color(line++,15);
						oper_dark_color(line++,RLE_SHADOW_DARK);
					}
				}
				else if(info&RLE_IMAGE)
				{
#ifndef _ASM_
					while(count-->0)
					{
						*line++=pix;
					}
#else
					__asm
					{
						mov  ecx,count
						xor  eax,eax
						mov  edi,line
						mov  ax,pix
_while_1:
						ALIGN 4
						mov  [edi],ax
						add  edi,2
						dec  ecx
						jnz  _while_1
						mov  line,edi
					}
#endif
				}
				else
				{
					info&=RLE_ALPHA;
					while(count-->0)
					{
						oper_alpha_color(&pix,line++,info);
					}
				}
			}
			else
			{
				switch(info)
				{
				case 0://end of line
					goto _next_line;
					break;
				case 1://end of image
					return;
					break;
				case 2://transfered meaning
#ifndef _ASM_
					x+=data[0];
					line+=data[0];
					des_y+=data[1];
					src_y+=data[1];
					data+=2;
#else
					__asm
					{
						mov  esi,data
						xor  eax,eax
						mov  ecx,x
						mov  al,[esi]
						xor  ebx,ebx
						mov  edx,des_y
						mov  bl,[esi+1]
						add  ecx,eax
						mov  edi,line
						add  edx,ebx
						lea  esi,[eax*2+edi]
						mov  x,ecx
						mov  line,esi
						mov  eax,src_y
						mov  des_y,edx
						mov  ecx,data
						add  eax,ebx
						add  ecx,2
						mov  src_y,eax
						mov  data,ecx
					}
#endif
					break;
				default:// copy pixels
					x+=info;
					memcpy(line,data,info*SIZEOFPIXEL);
					line+=info;
					data+=(info*sizeof(PIXEL));
					break;
				}
			}
		}
_next_line:
#ifndef _ASM_
		src_y++;
		des_y++;
#else
		__asm
		{
			mov  eax,src_y
			mov  ebx,des_y
			inc  eax
			inc  ebx
			mov  src_y,eax
			mov  des_y,ebx
		}
#endif
	}
}


void    _put_rle_asm(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    if(!rle) return;
    if(sx <= -rle->w) return;
    if(sy <= -rle->h) return;
    if(sx >= dest_bitmap->w) return;
    if(sy >= dest_bitmap->h) return;

    if(sx>=0 && sy>=0 && sx+rle->w < dest_bitmap->w && sy+rle->h < dest_bitmap->h)
        _direct_put_rle_asm(sx,sy,rle,dest_bitmap);
    else
        _range_put_rle_asm(sx,sy,rle,dest_bitmap);
}


void    _scale_put_bitmap_asm(SLONG des_x,SLONG des_y,SLONG des_w,SLONG des_h,BMP *src,BMP *des)
{
/*
	pixel_t *des_line;
	pixel_t *src_line;
*/
	int    *table_x;
	float  scalex;
	float  scaley;
	float  src_fx;
	float  src_fy;
	int    sx,sy;
	int    ex,ey;
	int    src_x;
	int    src_y;
	int    i;

	if(des_w>0&&des_h>0)
	{
		i=des->w;
		if(des_x<0)
			sx=0,src_x=-des_x;
		else if(des_x<i)
			sx=des_x,src_x=0;
		else return;
		if((ex=des_x+des_w)>=i)
			ex=i;
		if(ex<sx) return;

		i=des->h;
		if(des_y<0)
			sy=0,src_y=-des_y;
		else if(des_y<i)
			sy=des_y,src_y=0;
		else return;
		if((ey=des_y+des_h)>=i)
			ey=i;
		if(ey<sy) return;

		scalex=(float)src->w/(float)des_w;
		scaley=(float)src->h/(float)des_h;
		src_fy=(float)src_y*scaley;
		src_fx=(float)src_x*scalex;
		table_x=(int*)malloc((ex)*sizeof(int));
		for(i=sx;i<ex;i++)
			table_x[i]=(int)src_fx,src_fx+=scalex;
		while(sy<ey)
		{
/*
			src_line=src->line[(int)src_fy];
			src_fy+=scaley;
			des_line=&des->line[sy][sx];
			for(i=sx;i<ex;i++)
				*des_line++=src_line[table_x[i]];
			sy++;
*/
			i=(int)src_fy;
			src_fy+=scaley;
			__asm
			{
				mov   esi,src
				mov   edi,des
				mov   eax,i
				mov   ebx,sy
				mov   ecx,sx
				mov   edx,table_x
				mov   edi,[edi+ebx*4]BMP.line
				mov   esi,[esi+eax*4]BMP.line
				inc   ebx
				lea   edi,[edi+ecx*2]
				mov   sy,ebx
				mov   eax,ex
				ALIGN 4
_for_loop:
				mov   ebx,[edx+ecx*4]
				xor   eax,eax
				mov   ax,word ptr [esi+ebx*2]
				inc   ecx
				mov   [edi],ax
				mov   eax,ex
				add   edi,2
				cmp   ecx,eax
				jl   _for_loop
			}
		}
		free(table_x);
	}
}
