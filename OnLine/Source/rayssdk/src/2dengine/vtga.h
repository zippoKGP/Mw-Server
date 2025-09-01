/*
**    VTGA.H
**    TGA file functions header.
**    ZJian,2000/7/10.
*/
#ifndef _VTGA_H_INCLUDE_
#define _VTGA_H_INCLUDE_      1
#include "xgrafx.h"

#pragma pack(push)
#pragma pack(1)
typedef struct tagTGA_HEAD
{
   UCHR  bIdSize;
   UCHR  bColorMapType;
   UCHR  bImageType;
   UHINT iColorMapStart;
   UHINT iColorMapLength;
   UCHR  bColorMapBits;

   UHINT ixStart;
   UHINT iyStart;
   UHINT iWidth;
   UHINT iHeight;

   UCHR  bBitsPerPixel;
   UCHR  bDescriptor;
} TGA_HEAD,*LPTGA_HEAD;;
#pragma pack(pop)


EXPORT  BMP*    FNBACK  TGA_load_file(char *filename);
EXPORT  ABMP*   FNBACK  TGA_load_file_with_alpha(char *filename);
EXPORT  void    FNBACK  TGA_set_transparency_level(SLONG level);
EXPORT  SLONG   FNBACK  TGA_load_file_image(USTR *filename, SLONG *image_width, SLONG *image_height, 
                                            USTR **image_buffer, USTR **alpha_buffer);


#endif//_VTGA_H_INCLUDE_

