/*
**    VTGA.H
**    TGA file functions header.
*/
#ifndef _VTGA_H_INCLUDE_
#define _VTGA_H_INCLUDE_      1

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


//----------------------------------------------------------------------------------------------------------------
//功　能：  讀入TGA的影像資料到內存中
//參　數：  USTR    *filename;          //TGA文件名
//          SLONG   *image_width;       //存放TGA影像寬度的數據地址
//          SLONG   *image_height;      //存放TGA影像高度的數據地址
//          USTR    **image_buffer;     //存放TGA影像RGB資料的內存緩衝區
//          USTR    **alpha_buffer;     //存放TGA影像ALPHA資料的內存緩衝區
//返回值：  如果讀取成功，返回 TTN_OK；
//          否則，返回 TTN_ERROR。
//說　明：  (1) 影像RGB資料在image_buffer中的存放按照R,G,B的順序。
//          (2) 如果TGA中不含ALPHA資訊，獲得的alpha_buffer中每個數據都將為255。
//          (3) 本函數將自動分配內存給image_buffer和alpha_buffer，所以，我們在使用完image_buffer和alpha_buffer後，
//              必須釋佔該內存。
//----------------------------------------------------------------------------------------------------------------
SLONG   TGA_load_file_image(USTR *filename, SLONG *image_width, SLONG *image_height, 
                            USTR **image_buffer, USTR **alpha_buffer);



#endif//_VTGA_H_INCLUDE_

