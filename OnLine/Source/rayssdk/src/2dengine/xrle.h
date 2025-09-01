/*
**  XRLE.H
**  RLE compressed image functions header.
**
**  ZJian,2000.12.12.
*/
#ifndef _XRLE_H_INCLUDE_
#define _XRLE_H_INCLUDE_
#include "xgrafx.h"
#include "packfile.h"
#include "xrle.h"


// D E F I N E S ///////////////////////////////////////////////////////////////////////////////////////
#define RLE_ALPHA               0xf8
#define RLE_EDGE                0x04
#define RLE_SHADOW              0x02
#define RLE_IMAGE               0x01

#define RLE_LO_ALPHA            0x08
#define RLE_HI_ALPHA            0xf8

// S T R U C T S ///////////////////////////////////////////////////////////////////////////////////////
#pragma pack(push)
#pragma pack(1)
typedef struct  tagRLE
{
    SLONG       w;          // image width
    SLONG       h;          // image height
    SLONG       size;       // data size
    USTR      * line[1];    // ptr to each line start datas
} RLE,*LPRLE;
#pragma pack(pop)


extern	void	(*put_rle) (SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap);


// P R O T O T Y P E S /////////////////////////////////////////////////////////////////////////////////
EXPORT  RLE*    FNBACK  encode_rle(BMP *merge_bmp, IMG *info_img);
EXPORT  RLE *   FNBACK  make_rle(ABMP *image_abmp,ABMP *shadow_abmp);
EXPORT  RLE *   FNBACK  create_rle(SLONG w,SLONG h,SLONG data_size);
EXPORT  void    FNBACK  destroy_rle(RLE **rle);
EXPORT  RLE *   FNBACK  duplicate_rle(RLE *rle);
EXPORT  void    FNBACK  convert_rle_hi2fff(RLE *rle);
EXPORT  void    FNBACK  convert_rle_fff2hi(RLE *rle);
//
EXPORT  void    FNBACK  mask_edge_put_rle(SLONG sx,SLONG sy,PIXEL mask_color,RLE *src_rle,BMP *dest_bmp);
EXPORT  void    FNBACK  gray_put_rle(SLONG sx,SLONG sy,RLE *src_rle,BMP *dest_bmp);
EXPORT  void    FNBACK  gray_edge_put_rle(SLONG sx,SLONG sy,RLE *src_rle,BMP *dest_bmp);
EXPORT  void    FNBACK  alpha_put_rle(SLONG sx,SLONG sy,RLE *src_rle,BMP *dest_bmp,SLONG alpha);
EXPORT  void    FNBACK  alpha_edge_put_rle(SLONG sx,SLONG sy,RLE *src_rle,BMP *dest_bmp,SLONG alpha);
EXPORT  void    FNBACK  additive_put_rle(SLONG sx,SLONG sy,RLE *src_rle,BMP *dest_bmp);
EXPORT  void    FNBACK  additive_edge_put_rle(SLONG sx,SLONG sy,RLE *src_rle,BMP *dest_bmp);
EXPORT  void    FNBACK  subtractive_put_rle(SLONG sx,SLONG sy,RLE *src_rle,BMP *dest_bmp);
EXPORT  void    FNBACK  subtractive_edge_put_rle(SLONG sx,SLONG sy,RLE *src_rle,BMP *dest_bmp);
EXPORT  void    FNBACK  minimum_put_rle(SLONG sx,SLONG sy,RLE *src_rle,BMP *dest_bmp);
EXPORT  void    FNBACK  minimum_edge_put_rle(SLONG sx,SLONG sy,RLE *src_rle,BMP *dest_bmp);
EXPORT  void    FNBACK  maximum_put_rle(SLONG sx,SLONG sy,RLE *src_rle,BMP *dest_bmp);
EXPORT  void    FNBACK  maximum_edge_put_rle(SLONG sx,SLONG sy,RLE *src_rle,BMP *dest_bitmap);
EXPORT  void    FNBACK  half_put_rle(SLONG sx,SLONG sy,RLE *src_rle,BMP *dest_bmp);
EXPORT  void    FNBACK  half_edge_put_rle(SLONG sx,SLONG sy,RLE *src_rle,BMP *dest_bmp);
EXPORT  void    FNBACK  mesh_put_rle(SLONG sx,SLONG sy,RLE *src_rle,BMP *dest_bmp);
EXPORT  void    FNBACK  mask_put_rle(SLONG sx,SLONG sy,PIXEL mask_color,RLE *rle,BMP *dest_bitmap);
EXPORT  void    FNBACK  filter_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap,SLONG filter);
EXPORT  void    FNBACK  red_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap);
EXPORT  void    FNBACK  green_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap);
EXPORT  void    FNBACK  blue_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap);
EXPORT  void    FNBACK  yellow_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap);
EXPORT  void    FNBACK  merge_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap);
EXPORT  void    FNBACK  blue_alpha_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap, SLONG alpha);
//
EXPORT  void    FNBACK  mask_put_rle_edge(SLONG sx,SLONG sy,PIXEL mask_color,RLE *rle,BMP *dest_bitmap);
//
EXPORT  void	FNBACK  noshadow_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap);
//
EXPORT  void    FNBACK  scale_put_rle(SLONG sx,SLONG sy,SLONG scalex,SLONG scaley,RLE *src_rle,BMP *dest_bmp);
EXPORT  void    FNBACK  scale_blue_put_rle(SLONG sx,SLONG sy,SLONG scalex,SLONG scaley,RLE *src_rle,BMP *dest_bitmap);
EXPORT  void    FNBACK  scale_gray_put_rle(SLONG sx,SLONG sy,SLONG scalex,SLONG scaley,RLE *src_rle,BMP *dest_bitmap);
//
EXPORT  double  FNBACK  analyse_rle(RLE *rle);
EXPORT  void    FNBACK  put_rle_to_buffer(SLONG sx,SLONG sy,RLE *rle,char *pbuffer,long pitch, long w, long h);
EXPORT  void    FNBACK  clip_direct_put_rle(SLONG sx, SLONG sy, SLONG clip_sx, SLONG clip_sy, SLONG clip_xl, SLONG clip_yl, RLE *rle,BMP *dest_bitmap);
EXPORT  void	FNBACK  remake_rle_line_pointer(RLE *rle);
//
EXPORT  SLONG   FNBACK  naked_write_rle(FILE *fp, RLE *rle);
EXPORT  SLONG   FNBACK  naked_read_rle(PACK_FILE *fp, RLE **rle);
//
//optimized associated functions
EXPORT  void    FNBACK  _put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap);


#endif//_XRLE_H_INCLUDE_

