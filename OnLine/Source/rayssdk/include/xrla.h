/*
**  xrla.h
**  rle array functions header.
**  
**  Jack, 2002.3.27.
*/
#ifndef _XRLA_H_
#define _XRLA_H_

// DEFINES ////////////////////////////////////////////////////////////////////////
#define SECT_RLE_WIDTH          100
#define SECT_RLE_HEIGHT         100


// STURCTURES /////////////////////////////////////////////////////////////////////
//rle array structure.
//#include <pshpack1.h>
#pragma pack(push)
#pragma pack(1)
typedef struct  tagRLA
{
    SLONG   w;      //expand width of rla
    SLONG   h;      //expand height of rla
    SLONG   rw;     //width of each rle
    SLONG   rh;     //height of each rle
    SLONG   cw;     //count of row rles
    SLONG   ch;     //count of column rles
    RLE*    rle[1]; //each rle pointers
} RLA, *LPRLA;
#pragma pack(pop)
//#include <poppack.h>



// FUNCTIONS //////////////////////////////////////////////////////////////////////
EXPORT  RLA*    FNBACK  create_rla(SLONG w, SLONG h);
EXPORT  void    FNBACK  destroy_rla(RLA **rla);
EXPORT  void    FNBACK  convert_rla_hi2fff(RLA *rla);
EXPORT  void    FNBACK  convert_rla_fff2hi(RLA *rla);
EXPORT  RLA *   FNBACK  make_rla(ABMP *image_abmp, ABMP *shadow_abmp);
//
EXPORT  void    FNBACK  put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap);
EXPORT  void    FNBACK  mask_edge_put_rla(SLONG sx, SLONG sy, PIXEL mask_color, RLA *rla, BMP *bitmap);
EXPORT  void    FNBACK  gray_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap);
EXPORT  void    FNBACK  gray_edge_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap);
EXPORT  void    FNBACK  alpha_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap, SLONG alpha);
EXPORT  void    FNBACK  alpha_edge_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap, SLONG alpha);
EXPORT  void    FNBACK  additive_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap);
EXPORT  void    FNBACK  additive_edge_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap);
EXPORT  void    FNBACK  subtractive_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap);
EXPORT  void    FNBACK  subtractive_edge_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap);
EXPORT  void    FNBACK  minimum_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap);
EXPORT  void    FNBACK  minimum_edge_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap);
EXPORT  void    FNBACK  maximum_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap);
EXPORT  void    FNBACK  maximum_edge_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap);
EXPORT  void    FNBACK  half_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap);
EXPORT  void    FNBACK  half_edge_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap);
EXPORT  void    FNBACK  mesh_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap);
EXPORT  void    FNBACK  mask_put_rla(SLONG sx, SLONG sy, PIXEL mask_color, RLA *rla, BMP *bitmap);
EXPORT  void    FNBACK  red_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap);
EXPORT  void    FNBACK  green_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap);
EXPORT  void    FNBACK  blue_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap);
EXPORT  void    FNBACK  yellow_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap);
EXPORT  void    FNBACK  merge_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap);
EXPORT  void    FNBACK  mask_put_rla_edge(SLONG sx,SLONG sy,PIXEL mask_color,RLA *rla,BMP *dest_bitmap);
EXPORT  void	FNBACK  noshadow_put_rla(SLONG sx,SLONG sy,RLA *rla,BMP *dest_bitmap);
EXPORT  void    FNBACK  scale_put_rla(SLONG sx, SLONG sy, SLONG scalex, SLONG scaley, RLA *rla, BMP *bitmap);
//
EXPORT  SLONG   FNBACK  naked_write_rla(FILE *fp, RLA *rla);
EXPORT  SLONG   FNBACK  naked_read_rla(PACK_FILE *fp, RLA **rla);




#endif//_XRLA_H_
