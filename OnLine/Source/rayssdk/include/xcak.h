/*
**      XCAK.H
**      grafx CAK write to/read from disk files functions. 
**      ZJian,2000/10/09.
*/
#ifndef _XCAK_H_INCLUDE
#define _XCAK_H_INCLUDE   1
#include "xgrafx.h"
#include "xpoly.h"
#include "xrle.h"
#include "packfile.h"


typedef enum    CAKE_ID_VALUE_ENUMS
{   CAKE_ID_HEAD    =   MAKEFOURCC('C','A','K','\x1a'), /* head of file             */
CAKE_ID_COMMON      =   MAKEFOURCC('C','O','M','N'),    /* common data for the file */
CAKE_ID_OFFSET      =   MAKEFOURCC('O','F','F','S'),    /* frame data offsets       */
CAKE_ID_FRAME       =   MAKEFOURCC('F','R','A','M'),    /* head of frame            */
CAKE_ID_END         =   MAKEFOURCC('E','N','D',' '),    /* end of file              */

CAKE_ID_DUMMY       =   MAKEFOURCC(' ',' ',' ',' '),    /* dummy cake               */
CAKE_ID_INFO        =   MAKEFOURCC('I','N','F','O'),    /* text string information  */
CAKE_ID_ALF         =   MAKEFOURCC('A','L','F',' '),    /* alpha data of frame      */
CAKE_ID_BMP         =   MAKEFOURCC('B','M','P',' '),    /* bmp (bitmap) data        */
CAKE_ID_IMG         =   MAKEFOURCC('I','M','G',' '),    /* 256 colors img data      */
CAKE_ID_PAL         =   MAKEFOURCC('P','A','L',' '),    /* palette data             */
CAKE_ID_POLY        =   MAKEFOURCC('P','O','L','Y'),    /* poly data                */ 
CAKE_ID_RLE         =   MAKEFOURCC('R','L','E',' '),    /* rle data of frame        */
} CAKE_ID_VALUE;


typedef enum    GET_CAKE_FRAME_ANI_FLAGS_ENUMS
{   GET_HOME_FRAME  =   0x01,
GET_END_FRAME       =   0x02,
GET_LAST_FRAME      =   0x03,
GET_NEXT_FRAME      =   0x04
} GET_CAKE_FRAME_ANI_FLAGS;



typedef struct  tagCAKE_ID
{
    ULONG   id;
} CAKE_ID,*LPCAKE_ID;



typedef struct  tagCAKE_LEN
{
    SLONG   length;
} CAKE_LEN,*LPCAKE_LEN;



typedef struct  tagHEAD_CAKE
{
    SLONG   version;
} HEAD_CAKE,*LPHEAD_CAKE;



typedef struct  tagCOMMON_CAKE
{
    SLONG   flag;
} COMMON_CAKE,*LPCOMMON_CAKE;



typedef struct  tagOFFSET_CAKE
{
    SLONG   count;
    SLONG   offset[1];
} OFFSET_CAKE,*LPOFFSET_CAKE;



typedef struct  tagFRAME_CAKE
{
    SLONG   frame_rx;
    SLONG   frame_ry;
    SLONG   poly_rx;
    SLONG   poly_ry;
    SLONG   poly_rz;
} FRAME_CAKE,*LPFRAME_CAKE;


typedef struct  tagEND_CAKE
{
    SLONG   flag;
} END_CAKE,*LPEND_CAKE;


typedef struct  tagDUMMY_CAKE
{
    SLONG   flag;
} DUMMY_CAKE,*LPDUMMY_CAKE;


typedef struct  tagINFO_CAKE
{
    USTR    str[80];
} INFO_CAKE,*LPINFO_CAKE;


typedef struct  tagCAKE_FRAME
{
    SLONG   frame_rx;   // frame image sx
    SLONG   frame_ry;   // frame image sy
    SLONG   poly_rx;    // poly barycenter rx
    SLONG   poly_ry;    // poly barycenter ry
    SLONG   poly_rz;    // poly barycenter rz
    POLY   *poly;       // poly data
    RLE    *rle;        // rle image
} CAKE_FRAME,*LPCAKE_FRAME;


typedef struct  tagCAKE_FRAME_ANI   CAKE_FRAME_ANI,*LPCAKE_FRAME_ANI;
struct  tagCAKE_FRAME_ANI
{
    CAKE_FRAME      *cake_frame;    // pointer to cake frame data 
    CAKE_FRAME_ANI  *last,*next;    // pointer to last & next cake frame
};


SLONG   compute_cake_data_length(ULONG cake_id,void *p);
SLONG   write_cake_to_file(FILE *fp,ULONG cake_id,void *p);
SLONG   read_cake_from_file(FILE *fp,ULONG *cake_id,void **p);

CAKE_FRAME *        create_cake_frame(void);
void                destroy_cake_frame(CAKE_FRAME **p);
CAKE_FRAME *        duplicate_cake_frame(CAKE_FRAME *cf);

CAKE_FRAME_ANI *    create_cake_frame_ani(void);
void                destroy_cake_frame_ani(CAKE_FRAME_ANI **p);
SLONG               count_cake_frames(CAKE_FRAME_ANI *cfa);
SLONG               get_cake_frame_no(CAKE_FRAME_ANI *p,CAKE_FRAME_ANI *cfa);
CAKE_FRAME_ANI *    get_cake_frame_ani(CAKE_FRAME_ANI *cfa,CAKE_FRAME_ANI *p,SLONG get_flag);
CAKE_FRAME_ANI *    get_specify_cake_frame_ani(CAKE_FRAME_ANI *cfa,SLONG frame);
SLONG               get_cake_frame_ani_size(CAKE_FRAME_ANI *cfa, SLONG *sx, SLONG *sy, SLONG *ex, SLONG *ey);

SLONG   save_cake_frame_ani_to_file(USTR *filename,CAKE_FRAME_ANI *cfa);
SLONG   load_cake_frame_ani_from_file(USTR *filename,CAKE_FRAME_ANI **cfa);
SLONG   save_cake_frame_ani_to_opened_file(FILE *fp,CAKE_FRAME_ANI *cfa);
SLONG   load_cake_frame_ani_from_opened_file(FILE *fp,CAKE_FRAME_ANI **cfa);
SLONG   get_cak_frames(USTR *filename);
//
SLONG   load_cake_frame_from_file(USTR *filename, SLONG frame_index, CAKE_FRAME **cf);
//
void    scale_redraw_cake_frame_ani(SLONG sx,SLONG sy,SLONG scale_xl,SLONG scale_yl,CAKE_FRAME_ANI *cfa,BMP *bitmap);
//
SLONG   naked_read_cake_frame_ani(PACK_FILE *fp, CAKE_FRAME_ANI **cfa);



#endif//_XCAK_H_INCLUDE

