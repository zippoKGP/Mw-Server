/*
**      XDRAW.CPP
**      DirectDraw functions.
**      PS: Use DirectX7.0 or upper to compile this file.
**
**      ZJian, 2000.08.30.
**          Base functions.
**      ZJian, 2000.10.20.
**          Add update_screen type control.
**      ZJian, 2001.04.11.  Advise from Walker.
**          Add DDSCAPS_VIDEOMEMORY flag to ddscaps for create primary surface.
**
*/
#include "rays.h"
#include "2dlocal.h"
#include "winmain.h"
#include "xgrafx.h"
#include "xsystem.h"
#include "xdraw.h"
#include "xvga.h"
#include "xinput.h"

#pragma comment(lib,"ddraw")


//if define this, we can debug the specified vga type
//#define VGA_TYPE_DEBUG      VGA_TYPE_565

LPDIRECTDRAW7           lpDD7 = NULL;
LPDIRECTDRAWSURFACE7    lpDDSPrimary7 = NULL;
LPDIRECTDRAWSURFACE7    lpDDSBack7 = NULL;
LPDIRECTDRAWSURFACE7    lpDDSMemory7 = NULL;
DDSURFACEDESC2          ddsd;
DDSCAPS2                ddscaps;
DDPIXELFORMAT           ddpf;
ULONG                   nBackBuffers = 0;
SLONG                   vga_type = VGA_TYPE_555;

static  HDC     WndDC;
static  HDC     MemDC;
static  HBITMAP HBMP;
static  int     update_startline = 0;
static  int     update_height = SCREEN_HEIGHT;

static  void    _normal_update_screen(BMP *bmp);
static  void    _pest_update_screen(BMP *bmp);
static  ULONG   get_low_bit_position(ULONG data);
static  ULONG   get_high_bit_position(ULONG data);
//
static  int     _create_ddraw(DWORD dwMode);
static  int     _create_screen_surface(DWORD dwMode);
static  int     _create_memory_surface(void);
static  int     _get_pixel_format(void);
static  void    _destroy_ddraw(DWORD dwMode);
static  void    _destroy_screen_surface(DWORD dwMode);
static  void    _destroy_memory_surface(void);
static  void    _fit_window_size(DWORD dwMode);




EXPORT  int     FNBACK  init_draw(void)
{
    enum { INIT_OK = 0, INIT_ERROR = 1, INIT_SKIP = 2 };
    HRESULT ret=0;
    HRESULT init_result;
    BOOL    keep_try;

    ret=0;
    init_result = INIT_OK;
    keep_try = TRUE;

    ret = init_grafx();
    if( FAILED(ret) )
        return FailMsg("init_grafx() failed");

    sprintf((char *)print_rec,"DRAW module initialize starting...");
    log_error(1,print_rec);
    sprintf((char *)print_rec,"    Update Screen : %s", "DirectDraw" );
    log_error(1,print_rec);

    _create_ddraw( g_dwScreenMode );
    _create_screen_surface(g_dwScreenMode);
    _get_pixel_format();
    _create_memory_surface();

    setup_vga_function(ddpf.dwRBitMask, ddpf.dwGBitMask, ddpf.dwBBitMask);
    set_system_color();

    sprintf((char *)print_rec,"DRAW module initialized OKay.");
    log_error(1,print_rec);
    sprintf((char *)print_rec,"\n");
    log_error(1,print_rec);
    
    return 0;
}



EXPORT  void    FNBACK  free_draw(void)
{
    _destroy_memory_surface();
    _destroy_screen_surface(g_dwScreenMode);
    _destroy_ddraw(g_dwScreenMode);
    free_grafx();

    sprintf((char *)print_rec,"DRAW module free OKay.");
    log_error(1,print_rec);
}



EXPORT  void    FNBACK  active_draw(int bActive)
{
   if (lpDDSPrimary7 && lpDDSBack7 && bActive)
   {
       if(g_dwScreenMode == FULL_SCREEN_MODE)
       {
           if (lpDDSPrimary7->IsLost() == DDERR_SURFACELOST)
           {
               lpDDSPrimary7->Restore();
               if( FAILED(lpDDSPrimary7->Restore()) )
                   FailMsg("PrimarySurface restore failed");
               else
                   update_screen(screen_buffer);
           }
       }
       else if(g_dwScreenMode == WINDOWS_SCREEN_MODE)
       {
           if(lpDDSBack7->IsLost() == DDERR_SURFACELOST)
           {
               if( FAILED(lpDDSBack7->Restore()) )
                   FailMsg("BackSurface restore failed");
               else
                   update_screen(screen_buffer);
           }
       }
   }
}



EXPORT  void    FNBACK  get_bitmap_from_memory_surface(BMP *bmp, RECT rect, SLONG left_top_flag)
{
    HRESULT hRet=0;
    LPSTR pBits=NULL;
    LONG width_bytes;
    LONG i;

    ZeroMemory(&ddsd,sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    hRet = lpDDSMemory7->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
    if (DD_OK != hRet)
        FailMsg("MemorySurface lock failed");

    if(left_top_flag)
    {
        bmp->w=rect.right-rect.left;
        bmp->h=rect.bottom-rect.top;
        
        pBits = (LPSTR) ddsd.lpSurface;
        pBits +=  ddsd.lPitch * rect.top + SIZEOFPIXEL * rect.left ;
        width_bytes = (rect.right - rect.left) * SIZEOFPIXEL ;
        for (i = 0; i < bmp->h; i++)
        {
            memcpy(&bmp->line[i][0], pBits, width_bytes);
            pBits += ddsd.lPitch;
        }
    }
    else
    {
        pBits = (LPSTR) ddsd.lpSurface;
        pBits +=  ddsd.lPitch * rect.top + SIZEOFPIXEL * rect.left ;
        width_bytes = (rect.right - rect.left) * SIZEOFPIXEL ;
        for (i = rect.top; i < rect.bottom; i++)
        {
            memcpy(&bmp->line[i][rect.left], pBits, width_bytes);
            pBits += ddsd.lPitch;
        }
    }
    
    hRet=lpDDSMemory7->Unlock(NULL);
    if(DD_OK != hRet)
        FailMsg("MemorySurface unlock failed");
}


EXPORT  void    FNBACK  switch_screen_mode(void)
{
    ShowWindow( g_hDDWnd, SW_SHOWMINIMIZED);
    switch(g_dwScreenMode)
    {
    case FULL_SCREEN_MODE:
        {
            _destroy_memory_surface();
            _destroy_screen_surface(g_dwScreenMode);
            _destroy_ddraw(g_dwScreenMode);

            g_dwScreenMode = WINDOWS_SCREEN_MODE;
            _create_ddraw( g_dwScreenMode );
            _create_screen_surface(g_dwScreenMode);
            _create_memory_surface();
            break;
        }
    case WINDOWS_SCREEN_MODE:
        {
            _destroy_memory_surface();
            _destroy_screen_surface(g_dwScreenMode);
            _destroy_ddraw(g_dwScreenMode);

            g_dwScreenMode = FULL_SCREEN_MODE;
            _create_ddraw( g_dwScreenMode );
            _create_screen_surface(g_dwScreenMode);
            _create_memory_surface();
            break;
        }
    }
    ShowWindow( g_hDDWnd, SW_SHOWMAXIMIZED);
    RaysDDFitWindowSize(g_dwScreenMode);
}


static  void    _normal_update_screen(BMP *bmp)
{
    HRESULT hRet=0;
    LPSTR pBits=NULL;
    int i=0;

    pBits=NULL;
    i=0;
    hRet=0;
    if(g_hDDWnd)
    {
        if( (g_dwScreenMode == FULL_SCREEN_MODE) && (g_bProgramActive) )
        {
            ZeroMemory(&ddsd,sizeof(ddsd));
            ddsd.dwSize = sizeof(ddsd);
            hRet = lpDDSBack7->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
            if (DD_OK != hRet)
                FailMsg("BackSurface lock failed");
            
            pBits = (LPSTR) ddsd.lpSurface;
            pBits += (ddsd.lPitch * update_startline);
            for (i = 0; i < update_height; i++)
            {
                memcpy(pBits, bmp->line[update_startline+i], bmp->pitch);
                pBits += ddsd.lPitch;
            }
            
            pBits = (LPSTR) ddsd.lpSurface;
            redraw_mouse_image_cursor(pBits,ddsd.lPitch,(long)ddsd.dwWidth,(long)ddsd.dwHeight);
            hRet=lpDDSBack7->Unlock(NULL);
            if(DD_OK != hRet)
                FailMsg("BackSurface unlock failed");
            hRet = lpDDSPrimary7->Flip(NULL,DDFLIP_WAIT);
            if(DD_OK != hRet)   FailMsg("PrimarySurface Flip failed");
        }
        else if(g_dwScreenMode == WINDOWS_SCREEN_MODE)
        {
            HDC     hDC;
            HDC     hDC2;
            
            hDC = GetDC( g_hDDWnd);
            
            if( lpDDSBack7 == NULL )
                return;
            
            ddsd.dwSize = sizeof( ddsd );
            ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
            
            lpDDSBack7->GetSurfaceDesc(&ddsd);
            hRet = lpDDSBack7->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL ) ;
            if( FAILED(hRet) )
                FailMsg("BackSurface lock failed");

            pBits = (LPSTR) ddsd.lpSurface;
            pBits += (ddsd.lPitch * update_startline);
            for (i = 0; i < update_height; i++)
            {
                memcpy(pBits, bmp->line[update_startline+i], bmp->pitch);
                pBits += ddsd.lPitch;
            }
            
            pBits = (LPSTR) ddsd.lpSurface;
            redraw_mouse_image_cursor(pBits,ddsd.lPitch,(long)ddsd.dwWidth,(long)ddsd.dwHeight);
            
            if FAILED(lpDDSBack7->Unlock( NULL ) )
                FailMsg("BackSurface unlock fail");
            
            if(lpDDSBack7->GetDC(&hDC2) == DD_OK )
            {
                BitBlt(hDC, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,hDC2, 0, 0, SRCCOPY);
                lpDDSBack7->ReleaseDC(hDC2);
            }
            
            ReleaseDC(g_hDDWnd, hDC);
        }
    }
}



static  void    _pest_update_screen(BMP *bmp)
{
    HRESULT hRet=0;
    LPSTR pBits=NULL;
    int i,j;

    pBits=NULL;
    i=0;
    j=0;
    hRet=0;

    if(g_bProgramActive)
    {
        ZeroMemory(&ddsd,sizeof(ddsd));
        ddsd.dwSize = sizeof(ddsd);
        hRet = lpDDSBack7->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
        if (DD_OK != hRet)
            FailMsg("PrimarySurface lock failed");

        pBits = (LPSTR) ddsd.lpSurface;
        pBits += (ddsd.lPitch * update_startline);
        for (i = 0; i < update_height; i++)
        {
            for(j = 0; j<bmp->w; j++)
            {
                if(bmp->line[update_startline+i][j])
                    *(PIXEL *)(pBits+j*SIZEOFPIXEL) = bmp->line[update_startline+i][j];
                // memcpy(pBits, bmp->line[update_startline+i], bmp->pitch);
            }
            pBits += ddsd.lPitch;
        }

        pBits = (LPSTR) ddsd.lpSurface;
        redraw_mouse_image_cursor(pBits,ddsd.lPitch,(long)ddsd.dwWidth,(long)ddsd.dwHeight);
        hRet=lpDDSBack7->Unlock(NULL);
        if(DD_OK != hRet)
            FailMsg("PrimarySurface unlock failed");
        hRet = lpDDSPrimary7->Flip(NULL,DDFLIP_WAIT);
        if(DD_OK != hRet)
            FailMsg("PrimarySurface Flip failed");
    }
}



static  void    _get_screen(BMP *bitmap)
{
    HRESULT hRet=0;
    LPSTR pBits=NULL;
    int i=0;

    ZeroMemory(&ddsd,sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    hRet = lpDDSBack7->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
    if (DD_OK != hRet)
        FailMsg("PrimarySurface lock failed");
    
    pBits = (LPSTR) ddsd.lpSurface;
    bitmap->h = (SLONG)ddsd.dwHeight;
    bitmap->w = (SLONG)ddsd.dwWidth;
    bitmap->pitch = (SLONG)ddsd.lPitch;
    for (i = 0; i < bitmap->h; ++i)
    {
        memcpy(bitmap->line[i], pBits, bitmap->pitch);
        pBits += ddsd.lPitch;
    }
    hRet=lpDDSBack7->Unlock(NULL);
    if(DD_OK != hRet)
        FailMsg("PrimarySurface unlock failed");
}



EXPORT  void    FNBACK  set_update_area(int start,int height)
{
   update_startline=(start<0 || start>=SCREEN_HEIGHT)?0:start;
   update_height=(height<0 || update_startline+height>=SCREEN_HEIGHT)?SCREEN_HEIGHT-update_startline:height;
}



EXPORT  void    FNBACK  set_update_type(int type)
{
    switch(type)
    {
    case NORMAL_UPDATE_SCREEN:
        update_screen = _normal_update_screen;
        break;
    case PEST_UPDATE_SCREEN:
        update_screen = _pest_update_screen;
        break;
    default:
        update_screen = _normal_update_screen;
        break;
    }
}


static  ULONG   get_low_bit_position(ULONG data)
{
    ULONG mask=1;
    for(ULONG i=0;i<32;i++)
    {
        if(data & mask)
            return i;
        mask<<=1;
    }
    return(0);
}


static  ULONG   get_high_bit_position(ULONG data)
{
    ULONG mask=(ULONG)(1<<31);
    for(ULONG i=0;i<32;i++)
    {
        if(data & mask)
            return (31-i);
        mask>>=1;
    }
    return(0);
}


void    setup_vga_function(DWORD dwRBitMask, DWORD dwGBitMask, DWORD dwBBitMask)
{
    USTR id[4];

    // setup vga information ======================================
    vga_info.r_mask = dwRBitMask;
    vga_info.r_bit_high = get_high_bit_position(dwRBitMask);
    vga_info.r_bit_low = get_low_bit_position(dwRBitMask);
    vga_info.r_bit_num = vga_info.r_bit_high-vga_info.r_bit_low+1;
    vga_info.r_shift = 8-vga_info.r_bit_num;

    vga_info.g_mask = dwGBitMask;
    vga_info.g_bit_high = get_high_bit_position(dwGBitMask);
    vga_info.g_bit_low = get_low_bit_position(dwGBitMask);
    vga_info.g_bit_num = vga_info.g_bit_high-vga_info.g_bit_low+1;
    vga_info.g_shift = 8-vga_info.g_bit_num;

    vga_info.b_mask = dwBBitMask;
    vga_info.b_bit_high = get_high_bit_position(dwBBitMask);
    vga_info.b_bit_low = get_low_bit_position(dwBBitMask);
    vga_info.b_bit_num = vga_info.b_bit_high-vga_info.b_bit_low+1;
    vga_info.b_shift = 8-vga_info.b_bit_num;

    // check vga is defined type ==================================
    memset(id,0,4);
    id[0]=(UCHR)('0'+get_high_bit_position(dwRBitMask)-get_low_bit_position(dwRBitMask)+1);
    id[1]=(UCHR)('0'+get_high_bit_position(dwGBitMask)-get_low_bit_position(dwGBitMask)+1);
    id[2]=(UCHR)('0'+get_high_bit_position(dwBBitMask)-get_low_bit_position(dwBBitMask)+1);
    if(0==strcmpi((const char *)id,"555"))
        vga_type=VGA_TYPE_555;
    else if(0==strcmpi((const char *)id,"655"))
        vga_type=VGA_TYPE_655;
    else if(0==strcmpi((const char *)id,"565"))
        vga_type=VGA_TYPE_565;
    else if(0==strcmpi((const char *)id,"556"))
        vga_type=VGA_TYPE_556;
    else
        vga_type=VGA_TYPE_ANY;

#ifdef  VGA_TYPE_DEBUG
    vga_type=VGA_TYPE_DEBUG;
#endif

    // set functions always =======================================
    //  update_screen = _normal_update_screen;
    set_update_type(NORMAL_UPDATE_SCREEN);
    get_screen = _get_screen;

    // set functions by vga type ==================================
    switch(vga_type)
    {
    case VGA_TYPE_555:
        true2hi =   _true2hi_555;
        hi2true =   _hi2true_555;
        get_r   =   _get_r_555;
        get_g   =   _get_g_555;
        get_b   =   _get_b_555;
        set_r   =   _set_r_555;
        set_g   =   _set_g_555;
        set_b   =   _set_b_555;
        rgb2hi  =   _rgb2hi_555;
        hi2rgb  =   _hi2rgb_555;
        hi2fff  =   _hi2fff_555;
        fff2hi  =   _fff2hi_555;
        oper_alpha_color = _oper_alpha_color_555;
        oper_additive_color = _oper_additive_color_555;
        oper_gray_color = _oper_gray_color_555;
        oper_subtractive_color = _oper_subtractive_color_555;
        oper_minimum_color = _oper_minimum_color_555;
        oper_maximum_color = _oper_maximum_color_555;
        oper_half_color = _oper_half_color_555;
        oper_dark_color = _oper_dark_color_555;
        oper_red_color = _oper_red_color_555;
        oper_green_color = _oper_green_color_555;
        oper_blue_color = _oper_blue_color_555;
        oper_yellow_color = _oper_yellow_color_555;
        oper_merge_color = _oper_merge_color_555;
        oper_partial_color = _oper_partial_color_555;
        oper_complement_color = _oper_complement_color_555;

        sprintf((char *)print_rec," VGA Pixel Format : %s","555");
        log_error(1,print_rec);
        break;
    case VGA_TYPE_655:
        true2hi =   _true2hi_655;
        hi2true =   _hi2true_655;
        get_r   =   _get_r_655;
        get_g   =   _get_g_655;
        get_b   =   _get_b_655;
        set_r   =   _set_r_655;
        set_g   =   _set_g_655;
        set_b   =   _set_b_655;
        rgb2hi  =   _rgb2hi_655;
        hi2rgb  =   _hi2rgb_655;
        hi2fff  =   _hi2fff_655;
        fff2hi  =   _fff2hi_655;
        oper_alpha_color = _oper_alpha_color_655;
        oper_additive_color = _oper_additive_color_655;
        oper_gray_color = _oper_gray_color_655;
        oper_subtractive_color = _oper_subtractive_color_655;
        oper_minimum_color = _oper_minimum_color_655;
        oper_maximum_color = _oper_maximum_color_655;
        oper_half_color = _oper_half_color_655;
        oper_dark_color = _oper_dark_color_655;
        oper_red_color = _oper_red_color_655;
        oper_green_color = _oper_green_color_655;
        oper_blue_color = _oper_blue_color_655;
        oper_yellow_color = _oper_yellow_color_655;
        oper_merge_color = _oper_merge_color_655;
        oper_partial_color = _oper_partial_color_655;
        oper_complement_color = _oper_complement_color_655;

        sprintf((char *)print_rec," VGA Pixel Format : %s","655");
        log_error(1,print_rec);
        break;
    case VGA_TYPE_565:
        true2hi =   _true2hi_565;
        hi2true =   _hi2true_565;
        get_r   =   _get_r_565;
        get_g   =   _get_g_565;
        get_b   =   _get_b_565;
        set_r   =   _set_r_565;
        set_g   =   _set_g_565;
        set_b   =   _set_b_565;
        rgb2hi  =   _rgb2hi_565;
        hi2rgb  =   _hi2rgb_565;
        hi2fff  =   _hi2fff_565;
        fff2hi  =   _fff2hi_565;
        oper_alpha_color = _oper_alpha_color_565;
        oper_additive_color = _oper_additive_color_565;
        oper_gray_color = _oper_gray_color_565;
        oper_subtractive_color = _oper_subtractive_color_565;
        oper_minimum_color = _oper_minimum_color_565;
        oper_maximum_color = _oper_maximum_color_565;
        oper_half_color = _oper_half_color_565;
        oper_dark_color = _oper_dark_color_565;
        oper_red_color = _oper_red_color_565;
        oper_green_color = _oper_green_color_565;
        oper_blue_color = _oper_blue_color_565;
        oper_yellow_color = _oper_yellow_color_565;
        oper_merge_color = _oper_merge_color_565;
        oper_partial_color = _oper_partial_color_565;
        oper_complement_color = _oper_complement_color_565;

        sprintf((char *)print_rec," VGA Pixel Format : %s","565");
        log_error(1,print_rec);
        break;
    case VGA_TYPE_556:
        true2hi =   _true2hi_556;
        hi2true =   _hi2true_556;
        get_r   =   _get_r_556;
        get_g   =   _get_g_556;
        get_b   =   _get_b_556;
        set_r   =   _set_r_556;
        set_g   =   _set_g_556;
        set_b   =   _set_b_556;
        rgb2hi  =   _rgb2hi_556;
        hi2rgb  =   _hi2rgb_556;
        hi2fff  =   _hi2fff_556;
        fff2hi  =   _fff2hi_556;
        oper_alpha_color = _oper_alpha_color_556;
        oper_additive_color = _oper_additive_color_556;
        oper_gray_color = _oper_gray_color_556;
        oper_subtractive_color = _oper_subtractive_color_556;
        oper_minimum_color = _oper_minimum_color_556;
        oper_maximum_color = _oper_maximum_color_556;
        oper_half_color = _oper_half_color_556;
        oper_dark_color = _oper_dark_color_556;
        oper_red_color = _oper_red_color_556;
        oper_green_color = _oper_green_color_556;
        oper_blue_color = _oper_blue_color_556;
        oper_yellow_color = _oper_yellow_color_556;
        oper_merge_color = _oper_merge_color_556;
        oper_partial_color = _oper_partial_color_556;
        oper_complement_color = _oper_complement_color_556;

        sprintf((char *)print_rec," VGA Pixel Format : %s","556");
        log_error(1,print_rec);
        break;
    case VGA_TYPE_ANY:
        true2hi =   _true2hi_any;
        hi2true =   _hi2true_any;
        get_r   =   _get_r_any;
        get_g   =   _get_g_any;
        get_b   =   _get_b_any;
        set_r   =   _set_r_any;
        set_g   =   _set_g_any;
        set_b   =   _set_b_any;
        rgb2hi  =   _rgb2hi_any;
        hi2rgb  =   _hi2rgb_any;
        hi2fff  =   _hi2fff_any;
        fff2hi  =   _fff2hi_any;
        oper_alpha_color = _oper_alpha_color_any;
        oper_additive_color = _oper_additive_color_any;
        oper_gray_color = _oper_gray_color_any;
        oper_subtractive_color = _oper_subtractive_color_any;
        oper_minimum_color = _oper_minimum_color_any;
        oper_maximum_color = _oper_maximum_color_any;
        oper_half_color = _oper_half_color_any;
        oper_dark_color = _oper_dark_color_any;
        oper_red_color = _oper_red_color_any;
        oper_green_color = _oper_green_color_any;
        oper_blue_color = _oper_blue_color_any;
        oper_yellow_color = _oper_yellow_color_any;
        oper_merge_color = _oper_merge_color_any;
        oper_partial_color = _oper_partial_color_any;
        oper_complement_color = _oper_complement_color_any;

        sprintf((char *)print_rec," VGA Pixel Format : %s","Any Other");
        log_error(1,print_rec);
        break;
    }
}


//geaan, 2001.7.3.
//---------------
static  int     _create_ddraw(DWORD dwMode)
{
    HRESULT ret;
    DWORD   dwLevelFlags;

    // create DirectDraw7 object
    {
        ret = DirectDrawCreateEx(NULL, (VOID **)&lpDD7, IID_IDirectDraw7, NULL);
        if( FAILED(ret) )
            return FailMsg("DirectDraw init failed");
    }

    // set DirectDraw coop & display mode.
    {
        dwLevelFlags = DDSCL_NORMAL ;    // default for windows screen mode
        if(dwMode == FULL_SCREEN_MODE)
        {
            dwLevelFlags = DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN ;
            ret=lpDD7->SetCooperativeLevel(g_hDDWnd, dwLevelFlags );
            if( FAILED(ret) )
                return FailMsg("DirectDraw SetCooperativelevel failed");

            ret=lpDD7->SetDisplayMode(
                SCREEN_WIDTH,   // dwWidth
                SCREEN_HEIGHT,  // dwHeight
                SCREEN_BPP,     // dwBPP    
                0,              // dwRefreshRate
                0);             // dwFlags
            if ( FAILED(ret))
                return FailMsg("Display Mode set failed");

            sprintf((char *)print_rec,"     Display Mode : %dx%dx%d",SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP );
            log_error(1,print_rec);
        }
        else if(dwMode == WINDOWS_SCREEN_MODE)
        {
            dwLevelFlags = DDSCL_NORMAL ;
            ret=lpDD7->SetCooperativeLevel(g_hDDWnd, dwLevelFlags );
            if( FAILED(ret) )
                return FailMsg("DirectDraw SetCooperativelevel failed");
        }
    }
    return S_OK;
}

static  int     _create_screen_surface(DWORD dwMode)
{
    enum { INIT_OK = 0, INIT_ERROR = 1, INIT_SKIP = 2 };
    HRESULT ret = 0;
    HRESULT init_result;
    BOOL    keep_try;

    if(dwMode == FULL_SCREEN_MODE)
    {
        nBackBuffers = 2;
        init_result = INIT_OK ;
        keep_try = TRUE;
        while( keep_try )
        {
            ZeroMemory(&ddsd,sizeof(ddsd));
            ddsd.dwSize = sizeof(ddsd);
            ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
            ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
                DDSCAPS_VIDEOMEMORY |
                DDSCAPS_FLIP |
                DDSCAPS_3DDEVICE |
                DDSCAPS_COMPLEX;
            ddsd.dwBackBufferCount = nBackBuffers;
            ret=lpDD7->CreateSurface(&ddsd,&lpDDSPrimary7,NULL);
            if ( FAILED(ret) )
            {
                nBackBuffers--;
                if(nBackBuffers==0)
                {
                    keep_try = FALSE;
                    init_result = INIT_ERROR;
                    return FailMsg("CreateSurface failed");
                }
            }
            else
            {
                init_result = INIT_OK;
                keep_try = FALSE;
            }
        }
        sprintf((char *)print_rec,"     Back Buffers : %d", nBackBuffers );
        log_error(1,print_rec);
        
        // get a pointer to the back buffer
        ZeroMemory(&ddscaps, sizeof(ddscaps));
        ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
        ret = lpDDSPrimary7->GetAttachedSurface(&ddscaps, &lpDDSBack7);
        if (DD_OK != ret)
            return FailMsg("GetAttachedSurface failed");
    }
    else if(dwMode == WINDOWS_SCREEN_MODE)
    {
        LPDIRECTDRAWCLIPPER  lpDDClipper;

        // Create the primary surface
        ZeroMemory( &ddsd, sizeof(ddsd) );
        ddsd.dwSize = sizeof(ddsd);
        ddsd.dwFlags = DDSD_CAPS;
        ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
        if(FAILED( lpDD7->CreateSurface(&ddsd, &lpDDSPrimary7, NULL) ) )
        {
            return FailMsg("CreateSurface failed");
        }
        
        // Create the backbuffer surface
        ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
        ddsd.dwWidth = SCREEN_WIDTH;
        ddsd.dwHeight = SCREEN_HEIGHT;
        ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE; //DDSCAPS_SYSTEMMEMORY;
        if(FAILED( lpDD7->CreateSurface(&ddsd, &lpDDSBack7, NULL) ) )
        {
            return FailMsg("CreateSurface back failed");
        }

        // Clipper
        if(FAILED( lpDD7->CreateClipper(0, &lpDDClipper, NULL) ) )
        {
            return FailMsg("CreateClipper failed");
        }
        
        if(FAILED( lpDDClipper->SetHWnd(0, g_hDDWnd) ) )
        {
            lpDDClipper->Release();
            return FailMsg("Clipper SetHWnd failed");
        }

        if(FAILED( lpDDSPrimary7->SetClipper(lpDDClipper) ) )
        {
            lpDDClipper->Release();
            return FailMsg("PrimarySurface SetClipper failed");
        }

        // Done with clipper
        lpDDClipper->Release();
    }
    return S_OK;
}


static  int _create_memory_surface(void)
{
    HRESULT ret;

    // create a memory directdraw surface
    ZeroMemory(&ddsd,sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
    ddsd.dwWidth = SCREEN_WIDTH;
    ddsd.dwHeight = SCREEN_HEIGHT;
    ret = lpDD7->CreateSurface(&ddsd,&lpDDSMemory7,NULL);
    if ( FAILED(ret) )
        return FailMsg("CreateSurface (lpDDSMemory7) failed");
    return S_OK;
}


static  int     _get_pixel_format(void)
{
    HRESULT ret;

    // get pixel format
    ZeroMemory(&ddpf,sizeof(ddpf));
    ddpf.dwSize = sizeof(ddpf);
    ret = lpDDSPrimary7->GetPixelFormat(&ddpf);
    if ( FAILED(ret) )
        return FailMsg("GetPixelFormat failed");
    return S_OK;
}


static  void    _destroy_ddraw(DWORD dwMode)
{
    if(lpDD7)
    {
        lpDD7->Release();
        lpDD7 = NULL;
    }
}


static  void    _destroy_screen_surface(DWORD dwMode)
{
    if( lpDDSBack7 )
    {
        lpDDSBack7->Release();
        lpDDSBack7 = NULL;
    }
    if( lpDDSPrimary7 )
    {
        lpDDSPrimary7->Release();
        lpDDSPrimary7=NULL;
    }
}

static  void    _destroy_memory_surface(void)
{
    if(lpDDSMemory7)
    {
        lpDDSMemory7->Release();
        lpDDSMemory7 = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
