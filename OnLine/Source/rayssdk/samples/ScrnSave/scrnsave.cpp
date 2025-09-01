/*
**  scrnsave.cpp
**
**  scrnsave main functions.
**
**  geaan, 2001.8.27.
*/
//#include <windows.h>
//#include <scrnsave.h>
#include "rays.h"
#include <scrnsave.h>
//
#include "xtimer.h"
#include "xdraw.h"
#include "xgrafx.h"
#include "xsystem.h"
#include "xfont.h"
#include "xvga.h"
#include "vpcx.h"
#include "xtimer.h"
#include "particle.h"

#pragma comment(lib, "scrnsave")
#ifdef  _DEBUG
#pragma comment(lib, "2denginedbg")
#else
#pragma comment(lib, "2dengine")
#endif//_DEBUG


typedef struct  tagSCRNSAVE_DATA
{
    int w;
    int h;
    BITMAPINFO  bmi;
    unsigned short int  *buffer;
    BMP *screen;
    BMP *fore_bitmap;
    BMP *back_bitmap;
} SCRNSAVE_DATA;


// LOCAL VARIABLES /////////////////////////////////////////////////////////////////////////////////////
HWND    g_hWndScrnSave;
SCRNSAVE_DATA   scrnsave_data;


int     init_scrnsave(void);
void    free_scrnsave(void);
void    update_scrnsave_screen(void);
void    refresh_scrnsave(void);

LRESULT WINAPI ScreenSaverProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL    WINAPI ScreenSaverConfigureDialog (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL    WINAPI RegisterDialogClasses (HANDLE hInst);

// FUNCTIONS ///////////////////////////////////////////////////////////////////////////////////////////
int     init_scrnsave(void)
{
    int w, h;

    //(1) ---------------------------------------------------------------
    setup_vga_function( R_MASK_555, G_MASK_555, B_MASK_555 );
    set_system_color();

    //init_system();
    set_winfont_weight(FW_NORMAL);
    init_winfont();
    set_winfont_convert_big5_to_gb(FALSE);

    //(2) ---------------------------------------------------------------
    w = GetSystemMetrics( SM_CXSCREEN );
    h = GetSystemMetrics( SM_CYSCREEN ) ;
    scrnsave_data.w = w;
    scrnsave_data.h = h;
    scrnsave_data.bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    scrnsave_data.bmi.bmiHeader.biWidth = w;
    scrnsave_data.bmi.bmiHeader.biHeight = 0 - h;
    scrnsave_data.bmi.bmiHeader.biPlanes = 1;
    scrnsave_data.bmi.bmiHeader.biBitCount = 16;
    scrnsave_data.bmi.bmiHeader.biCompression = BI_RGB;
    scrnsave_data.bmi.bmiHeader.biSizeImage = 0;
    scrnsave_data.bmi.bmiHeader.biXPelsPerMeter = 0;
    scrnsave_data.bmi.bmiHeader.biYPelsPerMeter = 0;
    scrnsave_data.bmi.bmiHeader.biClrUsed = 0;
    scrnsave_data.bmi.bmiHeader.biClrImportant = 0;
    //
    scrnsave_data.screen = create_bitmap(w, h);
    if(NULL == scrnsave_data.screen)
        goto error;
    clear_bitmap(scrnsave_data.screen);
    scrnsave_data.buffer = scrnsave_data.screen->line[0];

    scrnsave_data.back_bitmap = create_bitmap(w, h);
    if(NULL == scrnsave_data.back_bitmap)
        goto error;
    scrnsave_data.fore_bitmap = create_bitmap(w, h);
    if(NULL == scrnsave_data.fore_bitmap)
        goto error;

    //(3) ---------------------------------------------------------------
    SetTimer( g_hWndScrnSave, 1, 20, NULL);

    init_particle(scrnsave_data.w, scrnsave_data.h);


    return 0;
error:
    return -1;
}


void    free_scrnsave(void)
{
    //--------------------------------------------------------------
    destroy_bitmap(&scrnsave_data.fore_bitmap);
    destroy_bitmap(&scrnsave_data.back_bitmap);
    destroy_bitmap(&scrnsave_data.screen);
    scrnsave_data.buffer = NULL;

    //--------------------------------------------------------------
    free_winfont();
    //free_system();
    free_particle();
}


void    update_scrnsave_screen(void)
{
    HDC hDC;
    int lret;

    hDC = GetDC( g_hWndScrnSave );
    lret = StretchDIBits( hDC,
        0,
        0,
        scrnsave_data.w,
        scrnsave_data.h,
        0,
        0,
        scrnsave_data.w,
        scrnsave_data.h,
        scrnsave_data.buffer,
        &scrnsave_data.bmi,
        DIB_RGB_COLORS,
        SRCCOPY );
    ReleaseDC( g_hWndScrnSave, hDC);
}



void    refresh_scrnsave0(void)
{
    enum 
    {   MAX_PIC     =   5,
    };
    static  int y = 1200;
    int pic;
    BMP *bmp;

    y += 16;
    put_bitmap(0, y, scrnsave_data.back_bitmap, scrnsave_data.screen);
    if(y >= scrnsave_data.h)
    {
        pic = rand() % MAX_PIC;
        sprintf((char *)game_filename, "PIC%03d.PCX", pic);

        bmp = PCX_load_file((char *)game_filename);
        if(bmp)
        {
            scale_put_bitmap(0, 0, scrnsave_data.w, scrnsave_data.h, bmp, scrnsave_data.back_bitmap);
            destroy_bitmap(&bmp);
        }

        y = 0 - scrnsave_data.h;
    }
}


enum    
{   LETTER_LINE_LEN     =   64,
LETTER_LINE_DISTANCE    =   18,
};

USTR    letter[][LETTER_LINE_LEN] = 
{
    //GB
    ///*
    "亲爱的PMM,                                                  ",
    "                                                            ",
    "    曾经有一个美好的机会摆在我的面前, 我没有好好珍惜, 现在, ",
    "我已经後悔莫及. 假如上天能够再给我一次机会, 我想对你说, 我  ",
    "爱你, 如果一定要给这个爱一个期限的话, 我希望是----一万年!   ",
    "                                                            ",
    "                                               想你的       ",
    "                                                     LGG    ",
    //*/
    /*BIG5
    "克稲PMM,                                                  ",
    "                                                            ",
    "    纯竒Τ诀穦耚и玡, и⊿Τ堡, 瞷, ",
    "и竒馋の. 安ぱ镑倒иΩ诀穦, и稱癸弧, и  ",
    "稲, 狦﹚璶倒硂稲戳杠, и辨琌----窾!   ",
    "                                                            ",
    "                                               稱       ",
    "                                                     LGG    ",
    */
};

void    refresh_scrnsave(void)
{
    static  int init_flag = FALSE;
    static  int sx, sy;
    static  int explosion_tick = 0;
    int i,  lines;
    int x, y;

    if( ! init_flag)
    {
        sx = (scrnsave_data.w - LETTER_LINE_LEN * 8) / 2;
        sy = scrnsave_data.h;
        init_flag = TRUE;
    }

//    fill_bitmap(scrnsave_data.screen, rgb2hi(200, 10, 200));
    lines = sizeof(letter) / sizeof(letter[0]);

    if(++explosion_tick > 80)
    {
        explosion_particle(rand()%scrnsave_data.w, rand()%scrnsave_data.h);
        explosion_tick = 0;
    }
    render_particle(scrnsave_data.screen);

    print16(0, 0, (USTR*)"~C0", PEST_PUT, scrnsave_data.screen);
    for(i=0; i<lines; i++)
    {
        x = sx;
        y = sy + i * LETTER_LINE_DISTANCE;
        print16(x, y, (USTR*)letter[i], PEST_PUT, scrnsave_data.screen);
    }
    print16(0, 0, (USTR*)"~C0", PEST_PUT, scrnsave_data.screen);

    sy --;
    if(sy < -lines * LETTER_LINE_DISTANCE)
        sy = scrnsave_data.h;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// SCREEN SAVER FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT WINAPI ScreenSaverProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;

    switch(message)
    {
    case WM_TIMER:
        {
			//refresh_scrnsave0();
            refresh_scrnsave();
            update_scrnsave_screen();
            return 0;
        }
    case WM_CREATE:
        {
            g_hWndScrnSave = hWnd;
            init_scrnsave(); 
            break;
        }
    case WM_DESTROY:
        {
            free_scrnsave();
            break;
        }
    case WM_PAINT:
        {
            BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            update_scrnsave_screen();
            return 0;
        }
    }
    return DefScreenSaverProc( hWnd, message, wParam, lParam );
}


BOOL WINAPI ScreenSaverConfigureDialog (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
	{
	case WM_COMMAND:
		if ( LOWORD(wParam) == IDOK)
			EndDialog( hDlg, 0 );
		return TRUE;
	default:
		break;
	}
	return FALSE;
}


BOOL WINAPI RegisterDialogClasses (HANDLE hInst)
{
    return  TRUE;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 2DENGINE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void    gameExit(void)  {}
int     gameInit(void)  { return 0; }
int     gameMain(void)  { return 0; }
int     gameSet(void)   { return 0; }
//////////////////////////////////////////////////////////////////////////////////////////////////////////

