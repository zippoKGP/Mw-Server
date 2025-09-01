/*
**	mmx.h
**	mmx cpu support functions header.
**
**	lrw, 2001.12.17.
*/
#ifndef	_MMX_H_
#define	_MMX_H_
#include "rays.h"
#include "xgrafx.h"
#include "xrle.h"

#define	MMX_MASK_RED_555		0xfc00fc00fc00fc00
#define	MMX_MASK_GREEN_555		0x03e003e003e003e0
#define	MMX_MASK_BLUE_555		0x001f001f001f001f
#define	MMX_MASK_HALF_555		0x7bde7bde7bde7bde

#define	MMX_MASK_RED_655		0xfc00fc00fc00fc00
#define	MMX_MASK_GREEN_655		0x03e003e003e003e0
#define	MMX_MASK_BLUE_655		0x001f001f001f001f
#define	MMX_MASK_HALF_655		0xfbdefbdefbdefbde

#define	MMX_MASK_RED_565		0xf800f800f800f800
#define	MMX_MASK_GREEN_565		0x07e007e007e007e0
#define	MMX_MASK_BLUE_565		0x001f001f001f001f
#define	MMX_MASK_HALF_565		0xf7def7def7def7de

#define	MMX_MASK_RED_556		0xf800f800f800f800
#define	MMX_MASK_GREEN_556		0x07c007c007c007c0
#define	MMX_MASK_BLUE_556		0x003f003f003f003f
#define	MMX_MASK_HALF_556		0xf7bef7bef7bef7be


//GLOBALS -----------------------------------------------------------------------------
extern	BOOL	is_mmx;

//FUNCTIONS ---------------------------------------------------------------------------
extern	int		init_mmx(void);
extern	void	active_mmx(int active);
extern	void	free_mmx(void);
//
extern	void	init_mmx_grafx_associated_data(SLONG type);
//
extern	void	_put_bitmap_mmx(SLONG sx,SLONG sy,BMP *src,BMP *des);
extern	void    _alpha_put_bitmap_mmx(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp,SLONG alpha);
extern	void	_half_put_bitmap_mmx(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);
extern	void	_gray_put_bitmap_mmx(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);
extern	void	_additive_put_bitmap_mmx(SLONG sx,SLONG sy,BMP *src,BMP *des);
extern	void	_bound_put_bitmap_mmx(SLONG sx, SLONG sy, SLONG bound_sx, SLONG bound_sy, SLONG bound_xl, SLONG bound_yl, BMP *src_bmp, BMP *dest_bmp);
extern	void	_adulterate_bitmap_mmx(SLONG r,SLONG g,SLONG b,BMP *bmp);
extern	void	_eclipse_bitmap_mmx(SLONG r,SLONG g,SLONG b,BMP *bmp);
//
extern	void	_alpha_put_bar_mmx(SLONG sx,SLONG sy,SLONG xl,SLONG yl,PIXEL c,BMP *bmp,SLONG alpha);
//
extern	void    _put_rle_asm(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap);
//
extern  void    _scale_put_bitmap_asm(SLONG des_x,SLONG des_y,SLONG des_w,SLONG des_h,BMP *src,BMP *des);


#endif//_MMX_H_

