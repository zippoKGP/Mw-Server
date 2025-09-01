/*
**    VBMP.H
**    BMP file support functions header.
**    ZJian,2000/7/11.
*/
#ifndef _VBMP_H_INCLUDE_
#define _VBMP_H_INCLUDE_      1
#include "xgrafx.h"

#pragma pack (push)
#pragma pack (2)
typedef struct tagBITMAPHEADER
{
    UHINT type;
    ULONG size;
    ULONG reserved;
    ULONG off_bits;
    ULONG head_size;
    ULONG width;
    ULONG height;
    UHINT planes;
    UHINT bit; 
} BITMAPHEADER,*LPBITMAPHEADER;;
#pragma pack (pop)


EXPORT  BMP*    FNBACK  BMP_load_file(char *filename);
EXPORT  SLONG   FNBACK  BMP_save_file(BMP *bmp, char *filename);
EXPORT  SLONG   FNBACK  BMP_save_file(UHINT *buffer, SLONG xl, SLONG yl, char *filename);


#endif//_VBMP_H_INCLUDE_
//==================================================================
