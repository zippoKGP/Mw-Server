/*
**      XDRAW.CPP
**      DirectDraw functions.
**      PS: Use DirectX7.0 or upper to compile this file.
**
**      geaan, 2000.08.30.
**          Base functions.
**      geaan, 2000.10.20.
**          Add update_screen type control.
**      geaan, 2001.04.11.  Advise from Walker.
**          Add DDSCAPS_VIDEOMEMORY flag to ddscaps for create primary surface.
**      geaan, 2001.
**          Rewrite the directdraw related functions for debug old "Alt+TAB" surface error.
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
#include "mmx.h"
#include "xrle.h"

#include "ddutil.h"

#pragma comment(lib,"ddraw")

#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }


#include "tier0/platform.h"

//if define this, we can debug the specified vga type
//#define VGA_TYPE_DEBUG      VGA_TYPE_ANY

static CDisplay s_Display;

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
static  int     update_startline;
static  int     update_height;
static  RECT    m_rcWindow;

static  void    _normal_update_screen(BMP *bmp);
static  void    _pest_update_screen(BMP *bmp);
static  ULONG   get_low_bit_position(ULONG data);
static  ULONG   get_high_bit_position(ULONG data);
static  HRESULT create_memory_surface(void);
//

extern BOOL		g_bCenterWindow;


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

    init_display();
    init_directdraw_mode( g_hDDWnd, g_dwScreenMode );
    get_display_pixel_format();
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
    free_display();
    free_grafx();

    sprintf((char *)print_rec,"DRAW module free OKay.");
    log_error(1,print_rec);
}



EXPORT  void    FNBACK  active_draw(int bActive)
{
    /*
    if( NULL == lpDDSPrimary7 && NULL == lpDDSBack7 )
        return;

    if(bActive)
    {
        if (lpDDSPrimary7->IsLost() == DDERR_SURFACELOST)
        {
            restore_display();
        }
    }
    */
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
       
		if (ddsd.lPitch / ddsd.dwWidth == 2)
		{
			for (i = rect.top; i < rect.bottom; i++)
			{
				memcpy(&bmp->line[i][rect.left], pBits, width_bytes);
				pBits += ddsd.lPitch;
			}			
		}
		else if (ddsd.lPitch /ddsd.dwWidth == 4)
		{
			int w = rect.right-rect.left;
			
			int pitch = bmp->pitch;

			PIXEL *pixel = &bmp->line[rect.top][rect.left];
			
			static __int64 MASKB=0x000000FF000000FF;
			
			static __int64 MASKG=0x0000FF000000FF00;
			
			static __int64 MASKR=0x00FF000000FF0000;
						
			static __int64 MASKA=0xFF00FF00FF00FF00;

			static __int64 MASKB2=0x001F001F001F001F;
			
			static __int64 MASKG2=0x07E007E007E007E0;
			
			static __int64 MASKR2=0xF800F800F800F800;
			
			static __int64 MM3_MASK=0xffffffff00000000;
			static __int64 MM4_MASK=0x00000000ffffffff;
			
			int height = rect.bottom-rect.top;

			double old_time = Plat_FloatTime();			
			
			char buffer[16];

			char *buf = buffer;

			buf += (DWORD)buf % 8;

			_asm
			{
				//			pushf;
				
				mov esi, pBits; //src
				
				mov edi, pixel; //dst
				
				mov ecx, height; //ecx=number of lines to copy
				
new_line:	
				xor eax, eax; // surface pitch
				xor edx, edx; // bmp pitch
				
				mov ebx, w;
new_qword:
				movq mm1, [esi+eax+8]; // 2 pixel

				// 红颜色	
				movq  mm5, mm1;
				pand  mm5, MASKR;
				psllq mm5, 8;
				pand  mm5, MASKR2;
				
				// 绿颜色
				movq  mm6, mm1;
				pand  mm6, MASKG;
				psllq mm6, 11;
				pand  mm6, MASKG2;				
				
				// 蓝颜色
				movq  mm7, mm1;
				pand  mm7, MASKB;
				psllq mm7, 13;
				pand  mm7, MASKB2;
				
				por mm5, mm6;
				por mm5, mm7;

				movq mm3, mm5;
				psllq mm5, 16;
				por mm3, mm5;
				pand mm3, MM3_MASK;

				//////////////////////////////////////////////////////////////////////////

				movq mm1, [esi+eax]; // 2 pixel
				
				// 红颜色	
				movq  mm5, mm1;
				pand  mm5, MASKR;
				psllq mm5, 8;
				pand  mm5, MASKR2;
				
				// 绿颜色
				movq  mm6, mm1;
				pand  mm6, MASKG;
				psllq mm6, 11;
				pand  mm6, MASKG2;
				
				// 蓝颜色
				movq  mm7, mm1;
				pand  mm7, MASKB;
				psllq mm7, 13;
				pand  mm7, MASKB2;
				
				por mm5, mm6;
				por mm5, mm7;
				
				psrlq mm5, 16;
				movq mm4, mm5;
				psrlq mm4, 16;
				por mm4, mm5;
				pand mm4, MM4_MASK;

				por mm3, mm4;

				movq [edi+edx], mm3;
				
				add eax, 16;
				add edx, 8;

				sub ebx, 4;
				jnz new_qword;
				
				dec ecx; //nuke one line			
				jz done; //all done
				
				add esi, ddsd.lPitch;
				add edi, pitch;
				
				jmp new_line;
done:
				emms;
				
				//			popf;
			};		
			
			double new_time = Plat_FloatTime();			
			
			double t = new_time - old_time;	
			
//			log_error(1, "tim = %.6f \n", t);
		}
    }
    
    hRet=lpDDSMemory7->Unlock(NULL);
    if(DD_OK != hRet)
        FailMsg("MemorySurface unlock failed");
}


static  void    _normal_update_screen(BMP *bmp)
{
    HRESULT hr;

    hr = blt_display( bmp );
    //if(FAILED(hr)) return;

    hr = present_display();
    if(hr != DDERR_SURFACELOST)
        return;

    //The surfaces were lost so restore them.
    restore_display();
}



static  void    _pest_update_screen(BMP *bmp)
{
    HRESULT hr;

    hr = pest_blt_display( bmp );
    //if(FAILED(hr)) return;

    hr = present_display();
    if(hr != DDERR_SURFACELOST)
        return;

    //The surfaces were lost so restore them.
    restore_display();
}



static  void    _get_screen(BMP *bitmap)
{
    HRESULT hRet = 0;
    LPSTR   pBits = NULL;
    int     i = 0;

    ZeroMemory(&ddsd,sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    // Jack,  [15:27,10/10/2002]
    //当用到DirectDraw加速时, 会与上图时的Flip冲突.
    //所以这里需要使用 PrimarySurface 来操作.
    //但是, 需要注意的是, 这时获得的屏幕会包含滑鼠图标.
    //
    //hRet = lpDDSBack7->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
    hRet = lpDDSPrimary7->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
    if (DD_OK != hRet)
        //FailMsg("PrimarySurface lock failed");
        return;
    
    pBits = (LPSTR) ddsd.lpSurface;
    bitmap->h = (SLONG)ddsd.dwHeight;
    bitmap->w = (SLONG)ddsd.dwWidth;
    bitmap->pitch = (SLONG)ddsd.lPitch;
    for (i = 0; i < bitmap->h; ++i)
    {
        memcpy(bitmap->line[i], pBits, bitmap->pitch);
        pBits += ddsd.lPitch;
    }
    lpDDSBack7->Unlock(NULL);
}



EXPORT  void    FNBACK  set_update_area(int start,int height)
{
   update_startline=(start<0 || start>=g_nScreenHeight)?0:start;
   update_height=(height<0 || update_startline+height>=g_nScreenHeight)?g_nScreenHeight-update_startline:height;
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

	sprintf((char *)print_rec," VGA Pixel R mask : 0x%08x",vga_info.r_mask);
	log_error(1,print_rec);
	sprintf((char *)print_rec," VGA Pixel G mask : 0x%08x",vga_info.g_mask);
	log_error(1,print_rec);
	sprintf((char *)print_rec," VGA Pixel B mask : 0x%08x",vga_info.b_mask);
	log_error(1,print_rec);

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
    else if(0==strcmpi((const char *)id,"888"))
		vga_type=VGA_TYPE_565;
	else
        vga_type=VGA_TYPE_ANY;

#ifdef  VGA_TYPE_DEBUG
    vga_type=VGA_TYPE_DEBUG;
#endif//VGA_TYPE_DEBUG

    // set functions always =======================================
    set_update_type(NORMAL_UPDATE_SCREEN);
    set_update_area(0, g_nScreenHeight);
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
		oper_adulterate_color = _oper_adulterate_color_555;
		oper_eclipse_color = _oper_eclipse_color_555;
        oper_blue_alpha_color = _oper_blue_alpha_color_555;

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
		oper_adulterate_color = _oper_adulterate_color_655;
		oper_eclipse_color = _oper_eclipse_color_655;
        oper_blue_alpha_color = _oper_blue_alpha_color_655;

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
		oper_adulterate_color = _oper_adulterate_color_565;
		oper_eclipse_color = _oper_eclipse_color_565;
        oper_blue_alpha_color = _oper_blue_alpha_color_565;

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
		oper_adulterate_color = _oper_adulterate_color_556;
		oper_eclipse_color = _oper_eclipse_color_556;
        oper_blue_alpha_color = _oper_blue_alpha_color_556;

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
//        rgb2hi  =   _rgb2hi_565;
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
		oper_adulterate_color = _oper_adulterate_color_any;
		oper_eclipse_color = _oper_eclipse_color_any;
        oper_blue_alpha_color = _oper_blue_alpha_color_any;

        sprintf((char *)print_rec," VGA Pixel Format : %s","Any Other");
        log_error(1,print_rec);
        break;
    }

	// init mmx grafx datas =======================================
	init_mmx_grafx_associated_data(vga_type);

	//init mmx grafx functons =====================================
	if(is_mmx)
	{
		put_bitmap = _put_bitmap_mmx;
		alpha_put_bitmap = _alpha_put_bitmap_mmx;
		half_put_bitmap = _half_put_bitmap_mmx;
		gray_put_bitmap = _gray_put_bitmap_mmx;
		additive_put_bitmap = _additive_put_bitmap_mmx;
		//bound_put_bitmap = _bound_put_bitmap_mmx;
		bound_put_bitmap = _bound_put_bitmap;
		adulterate_bitmap = _adulterate_bitmap_mmx;
		eclipse_bitmap = _eclipse_bitmap_mmx;
		//
		alpha_put_bar = _alpha_put_bar_mmx;
	}
	else
	{
		put_bitmap = _put_bitmap;
		alpha_put_bitmap = _alpha_put_bitmap;
		half_put_bitmap = _half_put_bitmap;
		gray_put_bitmap = _gray_put_bitmap;
		additive_put_bitmap = _additive_put_bitmap;
		bound_put_bitmap = _bound_put_bitmap;
		adulterate_bitmap = _adulterate_bitmap;
		eclipse_bitmap = _eclipse_bitmap;
		//
		alpha_put_bar = _alpha_put_bar;
	}

	//init asm functions ==========================================
	{
		put_rle = _put_rle_asm;
        scale_put_bitmap = _scale_put_bitmap_asm;
	}
	return;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
void    init_display(void)
{
    lpDD7 = NULL;
    lpDDSPrimary7 = NULL;
    lpDDSBack7 = NULL;
    lpDDSMemory7 = NULL;
    //
    nBackBuffers = 0;
    vga_type = VGA_TYPE_555;
}


void    free_display(void)
{
    destroy_display_objects();
}


void    destroy_display_objects(void)
{
//    SAFE_RELEASE( lpDDSMemory7 );
//   SAFE_RELEASE( lpDDSBack7 );
//    SAFE_RELEASE( lpDDSPrimary7 );

//    if( lpDD7 )
//        lpDD7->SetCooperativeLevel( g_hDDWnd, DDSCL_NORMAL );

//    SAFE_RELEASE( lpDD7 );

	s_Display.DestroyObjects();
}


HRESULT update_display_bounds(void)
{
    switch(g_dwScreenMode)
    {
    case WINDOWS_SCREEN_MODE:
        if(g_hDDWnd)
        {
            GetClientRect( g_hDDWnd, &m_rcWindow );
            ClientToScreen( g_hDDWnd, (POINT*)&m_rcWindow );
            ClientToScreen( g_hDDWnd, (POINT*)&m_rcWindow+1 );
        }
        break;
    case FULL_SCREEN_MODE:
        SetRect( &m_rcWindow, 0, 0, GetSystemMetrics(SM_CXSCREEN),
            GetSystemMetrics(SM_CYSCREEN) );
        break;
    }

    return S_OK;
}


HRESULT create_full_screen_display( HWND hWnd, DWORD dwWidth, DWORD dwHeight, DWORD dwBPP)
{
    HRESULT hr;

    // Cleanup anything from a previous call
    destroy_display_objects();

#if 0
    // DDraw stuff begins here
    if( FAILED( hr = DirectDrawCreateEx( NULL, (VOID**)&lpDD7, IID_IDirectDraw7, NULL ) ) )
        return E_FAIL;

    // Set cooperative level
    hr = lpDD7->SetCooperativeLevel( hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
    if( FAILED(hr) )
        return E_FAIL;

    // Set the display mode
    if( FAILED( lpDD7->SetDisplayMode( dwWidth, dwHeight, dwBPP, 0, 0 ) ) )
        return E_FAIL;

    // Create primary surface (with backbuffer attached)
    DDSURFACEDESC2 ddsd;
    ZeroMemory( &ddsd, sizeof( ddsd ) );
    ddsd.dwSize            = sizeof( ddsd );
    ddsd.dwFlags           = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    ddsd.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP |
                             DDSCAPS_COMPLEX | DDSCAPS_3DDEVICE;
    ddsd.dwBackBufferCount = 1;

    if( FAILED( hr = lpDD7->CreateSurface( &ddsd, &lpDDSPrimary7, NULL ) ) )
        return E_FAIL;

    // Get a pointer to the back buffer
    DDSCAPS2 ddscaps;
    ZeroMemory( &ddscaps, sizeof( ddscaps ) );
    ddscaps.dwCaps = DDSCAPS_BACKBUFFER;

    if( FAILED( hr = lpDDSPrimary7->GetAttachedSurface( &ddscaps,  &lpDDSBack7 ) ) )
        return E_FAIL;

    lpDDSBack7->AddRef();
#else

	hr = s_Display.CreateFullScreenDisplay(hWnd, dwWidth, dwHeight, dwBPP);

	if (FAILED(hr))
	{
		return E_FAIL;
	}

    DDCAPS ddcaps;
    ZeroMemory( &ddcaps, sizeof(ddcaps) );
    ddcaps.dwSize = sizeof(ddcaps);
    s_Display.GetDirectDraw()->GetCaps( &ddcaps, NULL );
    if( (ddcaps.dwCaps2 & DDCAPS2_CANRENDERWINDOWED) == 0 )
    {
        MessageBox( hWnd, TEXT("This display card can not render GDI."),
			TEXT("DirectDraw Sample"), MB_ICONERROR | MB_OK );
        return E_FAIL;
    }
	
    // Create a clipper so DirectDraw will not blt over the GDI dialog
    if( FAILED( hr = s_Display.InitClipper() ) )
        return hr;
	
	lpDDSPrimary7 = s_Display.GetFrontBuffer();
	lpDD7 = s_Display.GetDirectDraw();
	lpDDSBack7 = s_Display.GetBackBuffer();

#endif

    g_dwScreenMode = FULL_SCREEN_MODE;

    if( FAILED( hr = create_memory_surface() ) )
    {
        return  E_FAIL;
    }

    update_display_bounds();

    return S_OK;
}


HRESULT create_windowed_display( HWND hWnd, DWORD dwWidth, DWORD dwHeight )
{
    HRESULT hr;

    // Cleanup anything from a previous call
    destroy_display_objects();

#if 1
    RECT  rcWork;
    RECT  rc;
    DWORD dwStyle;

    // If we are still a WS_POPUP window we should convert to a normal app
    // window so we look like a windows app.
    dwStyle = WS_POPUP | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
    SetWindowLong( hWnd, GWL_STYLE, dwStyle );

    // Aet window size
    SetRect( &rc, 0, 0, dwWidth, dwHeight );

    AdjustWindowRectEx( &rc, GetWindowStyle(hWnd), GetMenu(hWnd) != NULL, GetWindowExStyle(hWnd) );

    SetWindowPos( hWnd, NULL, 0, 0, rc.right-rc.left, rc.bottom-rc.top,
                  SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE );

    SetWindowPos( hWnd, HWND_NOTOPMOST, 0, 0, 0, 0,
                  SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE );

    //  Make sure our window does not hang outside of the work area
    SystemParametersInfo( SPI_GETWORKAREA, 0, &rcWork, 0 );
    GetWindowRect( hWnd, &rc );
    rc.left = rcWork.left + ( (rcWork.right - rcWork.left) - (rc.right - rc.left) ) / 2;
    rc.top = rcWork.top + ( (rcWork.bottom - rcWork.top) - (rc.bottom - rc.top) ) / 2;
    if( rc.left < rcWork.left ) rc.left = rcWork.left;
    if( rc.top  < rcWork.top )  rc.top  = rcWork.top;
    SetWindowPos( hWnd, NULL, rc.left, rc.top, 0, 0,
                  SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );

	GetClientRect( hWnd, &rc );
	
    LPDIRECTDRAWCLIPPER pcClipper;
    
    // DDraw stuff begins here
    if( FAILED( hr = DirectDrawCreateEx( NULL, (VOID**)&lpDD7, IID_IDirectDraw7, NULL ) ) )
        return E_FAIL;
	
    // Set cooperative level
    hr = lpDD7->SetCooperativeLevel( hWnd, DDSCL_NORMAL );
    if( FAILED(hr) )
        return E_FAIL;

    // Create the primary surface
    DDSURFACEDESC2 ddsd;
    ZeroMemory( &ddsd, sizeof( ddsd ) );
    ddsd.dwSize         = sizeof( ddsd );
    ddsd.dwFlags        = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    if( FAILED( lpDD7->CreateSurface( &ddsd, &lpDDSPrimary7, NULL ) ) )
        return E_FAIL;

    // Create the backbuffer surface
    ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;    
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;
    ddsd.dwWidth        = dwWidth;
    ddsd.dwHeight       = dwHeight;

    if( FAILED( hr = lpDD7->CreateSurface( &ddsd, &lpDDSBack7, NULL ) ) )
        return E_FAIL;

    if( FAILED( hr = lpDD7->CreateClipper( 0, &pcClipper, NULL ) ) )
        return E_FAIL;

    if( FAILED( hr = pcClipper->SetHWnd( 0, hWnd ) ) )
    {
        pcClipper->Release();
        return E_FAIL;
    }

    if( FAILED( hr = lpDDSPrimary7->SetClipper( pcClipper ) ) )
    {
        pcClipper->Release();
        return E_FAIL;
    }

    // Done with clipper
    pcClipper->Release();
#else

	hr = s_Display.CreateWindowedDisplay(hWnd, dwWidth, dwHeight);

	if (FAILED(hr))
	{
		return E_FAIL;
	}

	lpDDSPrimary7 = s_Display.GetFrontBuffer();
	lpDD7 = s_Display.GetDirectDraw();
	lpDDSBack7 = s_Display.GetBackBuffer();
	
#endif

    if( FAILED( hr = create_memory_surface() ) )
    {
        return  E_FAIL;
    }

    g_dwScreenMode = WINDOWS_SCREEN_MODE;

    update_display_bounds();

    return S_OK;
}


HRESULT init_directdraw_mode( HWND hWnd, DWORD dwMode )
{
    HRESULT hr;
    RECT    rc, rcWork, rcWin;
    DWORD   dwStyle;

    // Release all existing surfaces
    destroy_display_objects();

    // The back buffer and primary surfaces need to be created differently 
    // depending on if we are in full-screen or windowed mode
    switch(dwMode)
    {
    case WINDOWS_SCREEN_MODE:
        {
            if( FAILED( hr = create_windowed_display( hWnd, g_nScreenWidth, g_nScreenHeight ) ) )
                return hr;
            
            // Reset the system window's style
            dwStyle = WS_POPUP | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
            SetWindowLong( hWnd, GWL_STYLE, dwStyle );
            /*
            HMENU hMenu = LoadMenu( g_hInstance, MAKEINTRESOURCE( IDR_MENU ) );
            SetMenu( hWnd, hMenu );
            */

			SetRect( &rc, 0, 0, g_nScreenWidth, g_nScreenHeight );
			AdjustWindowRect( &rc, dwStyle, FALSE);
			SystemParametersInfo( SPI_GETWORKAREA, 0, &rcWork, 0 );
			
            // Reset window position
			if (g_bCenterWindow)
			{
				SetRect( &rcWin, rcWork.left + ( (rcWork.right - rcWork.left) - (rc.right - rc.left) ) / 2,
					rcWork.top + ( (rcWork.bottom - rcWork.top) - (rc.bottom - rc.top) ) / 2,
					rcWork.left + ( (rcWork.right - rcWork.left) - (rc.right - rc.left) ) / 2 + (rc.right - rc.left),
					rcWork.top + ( (rcWork.bottom - rcWork.top) - (rc.bottom - rc.top) ) / 2 + (rc.bottom - rc.top) );     
			}
			else
			{
				SetRect( &rcWin, g_nScreenX, g_nScreenY, rc.right - rc.left+g_nScreenX, rc.bottom - rc.top+g_nScreenY );
			}

			SetWindowPos( g_hDDWnd, NULL,
				rcWin.left,
				rcWin.top,
				rcWin.right - rcWin.left,
				rcWin.bottom - rcWin.top,
				SWP_SHOWWINDOW );
			
            break;
        }
    case FULL_SCREEN_MODE:
        {
            if( FAILED( hr = create_full_screen_display( hWnd, g_nScreenWidth, g_nScreenHeight, SCREEN_BPP ) ) )
            {
                MessageBox( hWnd, TEXT("This display card does not support 640x480x16. "),
                    TEXT("DirectDraw Sample"), MB_ICONERROR | MB_OK );
                return hr;
            }
            
            // Disable the menu in full-screen since we are 
            // using a palette and a menu would look bad 
            SetMenu( hWnd, NULL );
            
            // Reset the system window's  style
            dwStyle = WS_POPUP;
            SetWindowLong( hWnd, GWL_STYLE, dwStyle );

            // Reset window position
            SetWindowPos( g_hDDWnd, NULL, 0, 0, g_nScreenWidth, g_nScreenHeight, SWP_SHOWWINDOW );

            break;
        }
    }

    hr = create_memory_surface();

    return S_OK;
}


HRESULT present_display( void )
{
    HRESULT hr;
	
    if( NULL == lpDDSPrimary7 && NULL == lpDDSBack7 )
        return E_POINTER;

    while( 1 )
    {
        switch(g_dwScreenMode)
        {
        case WINDOWS_SCREEN_MODE:
            hr = lpDDSPrimary7->Blt( &m_rcWindow, lpDDSBack7, NULL, DDBLT_WAIT, NULL );
            break;
        case FULL_SCREEN_MODE:
#if 0
            hr = lpDDSPrimary7->Flip( NULL, 0 );
#else
			if( FAILED( hr = lpDDSPrimary7->Blt( NULL, lpDDSBack7, NULL, 
				DDBLT_WAIT, NULL ) ) )
			{
				return hr;
			}
#endif			
            break;
        }
        if( hr == DDERR_SURFACELOST )
        {
            lpDDSPrimary7->Restore();
            lpDDSBack7->Restore();
        }
        if( hr != DDERR_WASSTILLDRAWING )
            return hr;
    }

    return  hr;
}


HRESULT blt_display( BMP *bmp )
{
    HRESULT hr = 0;
    LPSTR   pBits = NULL;
    int     i = 0;
	
    if( lpDDSBack7 == NULL)
        return E_POINTER;
	
	redraw_mouse_image_cursor((char *)bmp->line[0],bmp->pitch,(long)bmp->w,(long)bmp->h);
	
    ZeroMemory(&ddsd,sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    if( FAILED ( hr = lpDDSBack7->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL) ) )
        return hr;
    
    pBits = (LPSTR) ddsd.lpSurface;
    pBits += (ddsd.lPitch * update_startline);
//	MessageBox(NULL,"","",MB_OK);
	double old_time = Plat_FloatTime();
	
	if (ddsd.lPitch / ddsd.dwWidth == 2)
	{
		for (i = 0; i < update_height; i++)
		{
			memcpy(pBits, bmp->line[update_startline+i], bmp->pitch);
			pBits += ddsd.lPitch;
		}

//		memcpy(pBits, bmp->line[0], bmp->pitch*bmp->h);
	}
	else 
		if (ddsd.lPitch /ddsd.dwWidth == 4)		
	{

#if 0
		for (i = 0; i < bmp->h; i++)
		{
			DWORD *bits = (DWORD *)pBits;
			
			for (int j = 0; j < bmp->w; j ++)
			{
				PIXEL pixel = bmp->line[update_startline+i][j];

				// 565
				char r = ((pixel >> 11) << 3);
				char g = ((pixel >> 5) << 2 );
				char b = ((pixel << 3));

				bits[j] = (((ULONG)r<<16)&0xff0000) | (((ULONG)g<<8)&0xff00) | (b&0xff) | 0xff000000;

				pixel ++;
			}

			pBits += ddsd.lPitch;
		}
	
#else
		static __int64 MASKB=0x001F001F001F001F;
		
		static __int64 MASKG=0x07E007E007E007E0;
				
		static __int64 MASKR=0xF800F800F800F800;
		
		static __int64 MASKA=0xFF00FF00FF00FF00;
		
		PIXEL *pixel = bmp->line[0];

		static PIXEL *tmp_pixel = NULL;

		old_time = Plat_FloatTime();
		
		int w = bmp->w;

		int pitch = bmp->pitch;


		_asm
		{
//			pushf;

			mov esi, pixel; //src

			mov edi, ddsd.lpSurface; //dst
			
			mov ecx, update_height; //ecx=number of lines to copy

new_line:	
			xor eax, eax; // bmp pitch
			xor edx, edx; // surface pitch

			mov ebx, w;
new_qword:
			movq mm1, [esi+eax]; // 4 pixel

			// 蓝颜色
			movq  mm7, mm1;
			movq  mm2, MASKB;
			pand  mm7, mm2;
			psllw mm7, 3;

			// 绿颜色
			movq  mm6, mm1;
			movq  mm2, MASKG;
			pand  mm6, mm2;
			psrlw mm6, 3;

			// 红颜色	
			movq  mm5, mm1;
			movq  mm2, MASKR;
			pand  mm5, mm2;
			psrlw mm5, 8;
			
			// 绿和蓝组合
			pslld mm6, 8;
			por mm6, mm7;
			movq mm2, mm6;

			// 红和alpha组合
			movq mm4, MASKA;
			por mm5, mm4;

			punpckhwd mm2, mm5;
			punpcklwd mm6, mm5;

			movq [edi+edx], mm6;
			movq [edi+edx+8], mm2;

			add eax, 8;
			add edx, 16;
			
			sub ebx, 4;
			jnz new_qword;

			dec ecx; //nuke one line			
			jz done; //all done

			add esi, pitch;
			add edi, ddsd.lPitch;

			jmp new_line;
done:
			emms;

//			popf;
		};
	
#endif
	}
    lpDDSBack7->Unlock(NULL);

	double new_time = Plat_FloatTime();			
	
	double t = new_time - old_time;

//	log_error(1, "time = %.6f\n", t);
	

	
	extern void recoverMouseImageRect(BMP *img);

	recoverMouseImageRect(screen_buffer);
		
    return  NOERROR;
}


HRESULT pest_blt_display( BMP *bmp )
{
    HRESULT hr = 0;
    LPSTR   pBits = NULL;
    int     i, j ;

    if( lpDDSBack7 == NULL)
        return E_POINTER;

    ZeroMemory(&ddsd,sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    if( FAILED ( hr = lpDDSBack7->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL) ) )
        return hr;
    
    pBits = (LPSTR) ddsd.lpSurface;
    pBits += (ddsd.lPitch * update_startline);
    for (i = 0; i < update_height; i++)
    {
        for(j = 0; j<bmp->w; j++)
        {
            if(bmp->line[update_startline+i][j])
            {
                *(PIXEL *)(pBits+j*SIZEOFPIXEL) = bmp->line[update_startline+i][j];
            }
        }
        pBits += ddsd.lPitch;
    }
    
    pBits = (LPSTR) ddsd.lpSurface;
    redraw_mouse_image_cursor(pBits,ddsd.lPitch,(long)ddsd.dwWidth,(long)ddsd.dwHeight);

    lpDDSBack7->Unlock(NULL);

    return  NOERROR;
}


HRESULT restore_display(void)
{
    HRESULT hr;

    hr = lpDD7->RestoreAllSurfaces();

	if (FAILED(hr))
	{
		init_directdraw_mode( g_hDDWnd, g_dwScreenMode );
	}

    return  hr;
}


HRESULT get_display_pixel_format(void)
{
    HRESULT hr;

    ZeroMemory(&ddpf,sizeof(ddpf));
    ddpf.dwSize = sizeof(ddpf);
    if(FAILED ( hr = lpDDSPrimary7->GetPixelFormat(&ddpf) ) )
        return FailMsg("GetPixelFormat failed");
    return S_OK;
}


EXPORT  void    FNBACK  switch_screen_mode(void)
{
    ShowWindow( g_hDDWnd, SW_SHOWMINIMIZED);
    switch(g_dwScreenMode)
    {
    case FULL_SCREEN_MODE:
        init_directdraw_mode( g_hDDWnd, WINDOWS_SCREEN_MODE );
        break;
    case WINDOWS_SCREEN_MODE:
        init_directdraw_mode( g_hDDWnd, FULL_SCREEN_MODE );
        break;
    }
    ShowWindow( g_hDDWnd, SW_RESTORE);

    switch(g_dwScreenMode)
    {
    case FULL_SCREEN_MODE:
        {
            SetWindowPos( g_hDDWnd, NULL, 0, 0, g_nScreenWidth, g_nScreenHeight, SWP_SHOWWINDOW );
            break;
        }
    case WINDOWS_SCREEN_MODE:
        {
            DWORD   dwStyle;
            RECT    rc, rcWin, rcWork;
            
            dwStyle = GetWindowLong( g_hDDWnd, GWL_STYLE );
            
/*            SetRect( &rc, 0, 0, g_nScreenWidth, g_nScreenHeight );
            AdjustWindowRect( &rc, dwStyle, FALSE);
            SystemParametersInfo( SPI_GETWORKAREA, 0, &rcWork, 0 );
            SetRect( &rcWin, rcWork.left + ( (rcWork.right - rcWork.left) - (rc.right - rc.left) ) / 2,
                rcWork.top + ( (rcWork.bottom - rcWork.top) - (rc.bottom - rc.top) ) / 2,
                rcWork.left + ( (rcWork.right - rcWork.left) - (rc.right - rc.left) ) / 2 + (rc.right - rc.left),
                rcWork.top + ( (rcWork.bottom - rcWork.top) - (rc.bottom - rc.top) ) / 2 + (rc.bottom - rc.top) );
            SetWindowPos( g_hDDWnd, NULL,
                g_nScreenX,
                g_nScreenY,
                rcWin.right - rcWin.left,
                rcWin.bottom - rcWin.top,
                SWP_SHOWWINDOW );
 */
			SetRect( &rc, 0, 0, g_nScreenWidth, g_nScreenHeight );
			AdjustWindowRect( &rc, dwStyle, FALSE);
			SystemParametersInfo( SPI_GETWORKAREA, 0, &rcWork, 0 );
			
            // Reset window position
			if (g_bCenterWindow)
			{
				SetRect( &rcWin, rcWork.left + ( (rcWork.right - rcWork.left) - (rc.right - rc.left) ) / 2,
					rcWork.top + ( (rcWork.bottom - rcWork.top) - (rc.bottom - rc.top) ) / 2,
					rcWork.left + ( (rcWork.right - rcWork.left) - (rc.right - rc.left) ) / 2 + (rc.right - rc.left),
					rcWork.top + ( (rcWork.bottom - rcWork.top) - (rc.bottom - rc.top) ) / 2 + (rc.bottom - rc.top) );     
			}
			else
			{
				SetRect( &rcWin, g_nScreenX, g_nScreenY, rc.right - rc.left+g_nScreenX, rc.bottom - rc.top+g_nScreenY );
			}
			
			SetWindowPos( g_hDDWnd, NULL,
				rcWin.left,
				rcWin.top,
				rcWin.right - rcWin.left,
				rcWin.bottom - rcWin.top,
				SWP_SHOWWINDOW );
			
			break;
        }
    }
}


static  HRESULT create_memory_surface(void)
{
    HRESULT ret;

    // create a memory directdraw surface
    ZeroMemory(&ddsd,sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
    ddsd.dwWidth = g_nScreenWidth;
    ddsd.dwHeight = g_nScreenHeight;

    ret = lpDD7->CreateSurface(&ddsd,&lpDDSMemory7,NULL);
    if ( FAILED(ret) )
        return FailMsg("CreateSurface (lpDDSMemory7) failed");

    return S_OK;
}

