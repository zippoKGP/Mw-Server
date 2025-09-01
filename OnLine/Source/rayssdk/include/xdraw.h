/*
**      XDRAW.H
**      DirectDraw functions header.
**      PS: need DXSDK7.0 or upper.
**      
**      ZJian,2000/7/10.
*/
#ifndef XDRAW_H_INCLUDE
#define XDRAW_H_INCLUDE    1
#include "xgrafx.h"

/* system video card type defines ------------------------------------------------------*/
#define VGA_TYPE_555            1
#define VGA_TYPE_655            2
#define VGA_TYPE_565            3
#define VGA_TYPE_556            4
#define VGA_TYPE_ANY            5

/* types define for update to screen ---------------------------------------------------*/
#define NORMAL_UPDATE_SCREEN    0
#define PEST_UPDATE_SCREEN      1



/* extern datas ------------------------------------------------------------------------*/
extern  LPDIRECTDRAW7           lpDD7;          /* system directdraw object             */
extern  LPDIRECTDRAWSURFACE7    lpDDSPrimary7;  /* system directdraw primary surface    */ 
extern  LPDIRECTDRAWSURFACE7    lpDDSBack7;     /* system directdraw back surface       */
extern  LPDIRECTDRAWSURFACE7    lpDDSMemory7;   /* memory directdraw surface            */
extern  ULONG                   nBackBuffers;   /* system directdraw back surface count */
extern  SLONG                   vga_type;       /* system video card type               */


/* extern functions prototype ----------------------------------------------------------*/
EXPORT  int     FNBACK  init_draw(void);
EXPORT  void    FNBACK  free_draw(void);
EXPORT  void    FNBACK  active_draw(int bActive);
EXPORT  void    FNBACK  set_update_area(int start,int height);
EXPORT  void    FNBACK  set_update_type(int type);
EXPORT  void    FNBACK  get_bitmap_from_memory_surface(BMP *bmp, RECT rect, SLONG left_top_flag);
EXPORT  void    FNBACK  switch_screen_mode(void);
EXPORT  void    FNBACK  setup_vga_function(DWORD dwRBitMask, DWORD dwGBitMask, DWORD dwBBitMask);
//
void    init_display(void);
void    free_display(void);
void    destroy_display_objects(void);
HRESULT update_display_bounds(void);
HRESULT create_full_screen_display( HWND hWnd, DWORD dwWidth, DWORD dwHeight, DWORD dwBPP);
HRESULT create_windowed_display( HWND hWnd, DWORD dwWidth, DWORD dwHeight );
HRESULT init_directdraw_mode( HWND hWnd, DWORD dwMode );
HRESULT present_display( void );
HRESULT blt_display( BMP *bmp );
HRESULT pest_blt_display( BMP *bmp );
HRESULT get_display_pixel_format(void);
HRESULT restore_display(void);


#endif/*XDRAW_H_INCLUDE */
