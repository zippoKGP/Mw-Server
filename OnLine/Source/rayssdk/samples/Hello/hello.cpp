/*
**    HELLO.CPP
*/
#include "rays.h"
#include "winmain.h"
#include "xsystem.h"
#include "xmodule.h"
#include "xgrafx.h"
#include "xdraw.h"
#include "xinput.h"
#include "xtimer.h"
#include "xcdrom.h"
#include "xsound.h"
#include "xplay.h"
#include "xkiss.h"
#include "xfont.h"
//
#include "vtga.h"
#include "vpcx.h"
#include "vbmp.h"
#include "vpsd.h"
#include "vflic.h"
#include "mp3.h"
#include "jpeg.h"

#include "resource.h"


#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   600



#ifdef	DEBUG
#pragma comment(lib,"2denginedbg.lib")
#pragma	comment(lib,"mp3dbg.lib")
#pragma	comment(lib,"jpegdbg.lib")
#else//!DEBUG
#pragma comment(lib,"2dengine.lib")
#pragma	comment(lib,"mp3.lib")
#pragma	comment(lib,"jpeg.lib")
#endif//DEBUG


//#define ONLY_DDWIN      1


HINSTANCE   g_hInstance;


void    show_win_error(HWND hWnd)
{
    LPVOID  lpMsgBuf;
    
    FormatMessage( 
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM | 
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR) &lpMsgBuf,
        0,
        NULL 
        );
    // Process any inserts in lpMsgBuf.
    // ...
    // Display the string.
    MessageBox( hWnd, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
    // Free the buffer.
    LocalFree( lpMsgBuf );
}


SLONG	test_poly(void)
{
    enum 
    {	DO_LINE_1	=	1,
    DO_LINE_2		=	2,
    DO_POLY			=	3,
    
    INFO_SX		=	4,
    INFO_SY		=	4,
    INFO_DY		=	18,
    };
    SLONG main_pass;
    SLONG mx,my;
    UCHR ch;
    D3POINT pt1,pt2;
    D3POINT pt3,pt4;
    D3POINT pt5,pt6;
    D3POINT pt7,pt8,pt9,pt10,pt11,pt12,pt13,pt14;
    SLONG do_flag;
    SLONG info_x,info_y;
    SLONG same_side;
    SLONG inter_seg;
    SLONG draw_poly_flags;
    SLONG inter_poly;
    SLONG include_point;
    SLONG include_segment;
    SLONG over_poly;
    SLONG deep_seg;
    SLONG inter_line_line;
    SLONG dist_ptpt;
    SLONG dist_ptln;
    SLONG nip_lnln;
    SLONG inter_sgsg;
    SLONG inter_sgpl;
    SLONG n1,n2;
    
    POLY poly;
    POLY poly2;
    SLONG deep_plpl;
    
    pt1.x=pt1.y=pt1.z=0;
    pt6=pt2=pt3=pt4=pt5=pt1;
    pt7=pt8=pt9=pt10=pt11=pt12=pt13=pt14=pt1;
    do_flag = DO_LINE_1;
    same_side = FALSE;
    inter_seg = FALSE;
    inter_poly = FALSE;
    include_point = FALSE;
    over_poly = FALSE;
    
    draw_poly_flags = PDP_VALID | PDP_BASE | PDP_HEIGHT | PDP_ARRIS 
        | PDP_TOP | PDP_CROSS | PDP_PROJECTION;//| PDP_ZHEIGHT;
    D3_make_diamond_poly(&poly,SCREEN_WIDTH/2,SCREEN_HEIGHT/2,80,40);
    D3_make_diamond_poly(&poly2,0,0,100,20);
    
    main_pass=0;
    reset_key();
    reset_mouse();
    while(0==main_pass && g_hDDWnd )
    {
//        /*
        log_error(1, "before idle_loop() of test_poly() ");
        idle_loop();
        log_error(1, "after idle_loop() of test_poly() ");
//        */

        ch = (UCHR) toupper( read_data_key() );
        if(ch) reset_data_key();
        else ch=get_mouse_key();
        get_mouse_position(&mx,&my);
        reset_mouse();
        
        fill_bitmap(screen_buffer,SYSTEM_DARK2);
        
        D3_draw_line(pt1.x,pt1.y,pt1.z,pt2.x,pt2.y,pt2.z,screen_buffer);
        D3_draw_line(pt3.x,pt3.y,pt3.z,pt4.x,pt4.y,pt4.z,screen_buffer);
        D3_draw_poly(0,0,0,draw_poly_flags,&poly,screen_buffer);
        D3_draw_poly(0,0,0,draw_poly_flags,&poly2,screen_buffer);
        
        inter_line_line = D2_intersect_line_line(pt1.x,pt1.y,pt2.x,pt2.y,pt3.x,pt3.y,pt4.x,pt4.y,&pt5.x,&pt5.y);
        if(inter_line_line == INTERSECT_OK)
            D3_draw_point(pt5.x,pt5.y,pt5.z,screen_buffer);
        
        inter_sgsg = D2_intersect_segment_segment(pt1.x,pt1.y,pt2.x,pt2.y,pt3.x,pt3.y,pt4.x,pt4.y,&pt6.x,&pt6.y);
        if(inter_sgsg == INTERSECT_OK)
            D3_draw_point(pt6.x,pt6.y,pt6.z,screen_buffer);
        
        dist_ptpt = D2_distance_point_point(pt1.x,pt1.y,pt2.x,pt2.y);
        dist_ptln = D2_distance_point_line(pt3.x,pt3.y,pt1.x,pt1.y,pt2.x,pt2.y);
        
        nip_lnln = D2_nipangle_line_line(pt1.x,pt1.y,pt2.x,pt2.y,pt3.x,pt3.y,pt4.x,pt4.y);
        
        pt7 = pt1;
        pt8 = pt2;
        inter_sgpl = D2_intersect_segment_poly(&pt7.x,&pt7.y,&pt8.x,&pt8.y,&poly);
        if(inter_sgpl & INTERSECT_TWO)
        {
            D3_draw_line(pt7.x,pt7.y,pt7.z,pt8.x,pt8.y,pt8.z,screen_buffer);
        }
        else if(inter_sgpl & INTERSECT_ONE)
        {
            D3_draw_point(pt7.x,pt7.y,pt7.z,screen_buffer);
        }
        
        D2_chip_point_poly(mx,my,&poly,&n1,&n2);
        {
            D3_draw_line(mx,my,0,poly.px[n1],poly.py[n1],poly.pz[n1],screen_buffer);
            D3_draw_line(mx,my,0,poly.px[n2],poly.py[n2],poly.pz[n2],screen_buffer);
        }
        
        deep_plpl = D3_check_deeply_poly_poly(&poly,0,0,0,&poly2,0,0,0);
        
        
        same_side = D2_is_sameside_point_line(pt3.x,pt3.y,pt4.x,pt4.y,pt1.x,pt1.y,pt2.x,pt2.y);
        inter_seg = D2_is_intersect_segment_segment(pt3.x,pt3.y,pt4.x,pt4.y,pt1.x,pt1.y,pt2.x,pt2.y);
        inter_poly = D2_is_intersect_line_poly(pt3.x,pt3.y,pt4.x,pt4.y,&poly);
        include_point = D2_is_point_in_poly(mx,my,&poly);
        include_segment = D2_is_segment_in_poly(pt3.x,pt3.y,pt4.x,pt4.y,&poly);
        over_poly = D2_is_overlap_poly_poly(&poly,&poly2);
        
        deep_seg = D2_check_deeply_segment_segment(pt1.x,pt1.y,pt2.x,pt2.y,pt3.x,pt3.y,pt4.x,pt4.y);
        
        info_x = INFO_SX;
        info_y = INFO_SY;
        sprintf((char *)print_rec,"~C0~O3DO:%d~C0~O0",do_flag);
        print16(info_x,info_y,(USTR *)print_rec,PEST_PUT,screen_buffer);
        info_y+=INFO_DY;
        
        sprintf((char *)print_rec,"~C0~O3SAMESIDE:%d    INTERSEG:%d    INTERPOLY:%d    INCLUDEPOINT:%d~C0~O0",
            same_side,inter_seg,inter_poly,include_point);
        print16(info_x,info_y,(USTR *)print_rec,PEST_PUT,screen_buffer);
        info_y+=INFO_DY;
        
        sprintf((char *)print_rec,"~C0~O3INCLUDESEG:%d    OVERPOLY=%d~C0~O0",
            include_segment,over_poly);
        print16(info_x,info_y,(USTR *)print_rec,PEST_PUT,screen_buffer);
        info_y+=INFO_DY;
        
        sprintf((char *)print_rec,"~C0~O3DEEPSEG:%d    INTERLNLN:%d    INTERSGSG:%d   INTERSGPL:%d~C0~O0",
            deep_seg,inter_line_line,inter_sgsg,inter_sgpl);
        print16(info_x,info_y,(USTR *)print_rec,PEST_PUT,screen_buffer);
        info_y+=INFO_DY;
        
        sprintf((char *)print_rec,"~C0~O3DISTPTPT:%d   DISTPTLN:%d~C0~O0",
            dist_ptpt,dist_ptln);
        print16(info_x,info_y,(USTR *)print_rec,PEST_PUT,screen_buffer);
        info_y+=INFO_DY;
        
        sprintf((char *)print_rec,"~C0~O3NIPLNLN:%d   DEEPPLPL:%d~C0~O0",
            nip_lnln,deep_plpl);
        print16(info_x,info_y,(USTR *)print_rec,PEST_PUT,screen_buffer);
        info_y+=INFO_DY;
        
        
        update_screen(screen_buffer);
        
        switch(ch)
        {
        case '1':
            do_flag = DO_LINE_1;
            break;
        case '2':
            do_flag = DO_LINE_2;
            break;
        case '3':
            do_flag = DO_POLY;
            break;
        case S_Esc:
            main_pass=1;
            break;
        default:
            break;
        }
        
        switch(do_flag)
        {
        case DO_LINE_1:
            {
                switch(ch)
                {
                case MS_LDn:
                    pt1.x=mx;
                    pt1.y=my;
                    pt1.z=0;
                    pt2=pt1;
                    break;
                case MS_LDrag:
                    pt2.x=mx;
                    pt2.y=my;
                    break;
                case MS_LUp:
                    pt2.x=mx;
                    pt2.y=my;
                    break;
                default:
                    break;
                }
            }
            break;
        case DO_LINE_2:
            {
                switch(ch)
                {
                case MS_LDn:
                    pt3.x=mx;
                    pt3.y=my;
                    pt3.z=0;
                    pt4=pt3;
                    break;
                case MS_LDrag:
                    pt4.x=mx;
                    pt4.y=my;
                    break;
                case MS_LUp:
                    pt4.x=mx;
                    pt4.y=my;
                    break;
                default:
                    break;
                }
            }
            break;
        case DO_POLY:
            {
                switch(ch)
                {
                case MS_LDn:
                case MS_LDrag:
                default:
                    D3_make_diamond_poly(&poly2,0,0,100,20);
                    D3_translate_poly_world_coordinates(&poly2,mx,my,0);
                    break;
                }
            }
            break;
        default:
            break;
        }

//        /*
        log_error(1, "before idle_loop() of test_poly() ");
        idle_loop();
        log_error(1, "after idle_loop() of test_poly() ");
//        */
    }
    return TTN_OK;
}


void	some_test(void)
{
	SLONG main_pass;
	SLONG x,y,xl,yl;
	PIXEL color;
	UCHR ch,r,g,b;

	main_pass=0;
	reset_key();
    while(0==main_pass && g_hDDWnd )
//	while(main_pass==0)
	{
		idle_loop();
		clear_bitmap(screen_buffer);
		x=rand()%SCREEN_WIDTH;
		y=rand()%SCREEN_HEIGHT;
		xl=10+rand()%(SCREEN_WIDTH);
		yl=10+rand()%(SCREEN_HEIGHT);
		r=rand()%255;
		g=rand()%255;
		b=rand()%255;
		color = rgb2hi(r,g,b);

		put_bar(x,y,xl,yl,color,screen_buffer);
		update_screen(screen_buffer);

		ch=read_data_key();
		if(ch) reset_key();

		if(ch == S_Esc)
			main_pass=1;
	}
}



void    do_menu_dot(void)
{
    test_poly();
}

void    do_menu_line(void)
{
    some_test();
}

void    do_menu_circle(void)
{
}


BOOL    bIsFinishTask = TRUE;

long FAR WINAPI MainWindowProc(HWND hWnd,UINT message, WPARAM wParam,LPARAM lParam );
long FAR WINAPI DDWindowProc(HWND hWnd,UINT message, WPARAM wParam,LPARAM lParam );
extern  int WINAPI RaysDDWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow);


int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
    g_hInstance = hInstance;

#ifdef  ONLY_DDWIN
    return RaysDDWinMain (hInstance, hPrevInstance,
        szCmdLine, iCmdShow);
#else

    HWND       hwnd ;
    MSG        msg ;
    WNDCLASSEX wndclass ;

    //g_hInstance = hInstance;
    
    wndclass.cbSize        = sizeof (wndclass) ;
    wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
    wndclass.lpfnWndProc   = MainWindowProc ;
    wndclass.cbClsExtra    = 0 ;
    wndclass.cbWndExtra    = 0 ;
    wndclass.hInstance     = hInstance ;
    wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
    wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
    wndclass.lpszMenuName  = "MenuSamples" ;
    wndclass.lpszClassName = "ClassSamples" ;
    wndclass.hIconSm       = LoadIcon (NULL, IDI_APPLICATION) ;
    RegisterClassEx (&wndclass) ;
    
    hwnd = CreateWindowEx (
        0,
        "ClassSamples",//local_MainWndClassName,  // class name
        "App Samples",//local_AppName,
        WS_OVERLAPPEDWINDOW,//dwMainWndStyle,
        0,
        0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        NULL,           // parent
        LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1)), //local_hMenu,    // menu
        hInstance, //g_hInstance,    // instance
        NULL            // lpPara
        );
    
    ShowWindow (hwnd, iCmdShow) ;
    UpdateWindow (hwnd) ;

//    RaysDDWinMain (hInstance, NULL, szCmdLine, iCmdShow);


    while (GetMessage (&msg, NULL, 0, 0))
    {
        TranslateMessage (&msg) ;
        DispatchMessage (&msg) ;
    }
    return msg.wParam ;
#endif
} 



long FAR WINAPI DDWindowProc(HWND hWnd,UINT message, WPARAM wParam,LPARAM lParam )
{
    switch(message)
    {
    case WM_USER+1:
        bIsFinishTask = FALSE;
        do_menu_dot();
        bIsFinishTask = TRUE;
        break;
    case WM_USER+2:
        bIsFinishTask = FALSE;
        do_menu_line();
        bIsFinishTask = TRUE;
        break;
    case WM_USER+3:
        do_menu_circle();
        break;
    }
    return DefWindowProc( hWnd, message, wParam, lParam );
}


long FAR WINAPI MainWindowProc(HWND hWnd,UINT message, WPARAM wParam,LPARAM lParam )
{
    static  HMENU   hMenu;

    switch(message)
    {
    case WM_CLOSE:
        if(hMenu) DestroyMenu(hMenu);
        PostQuitMessage(0);
        DestroyWindow( hWnd );
        exit(0);
        //return 0;
        break;
    case WM_CREATE:
        hMenu = LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_MENU1));
        SetMenu(hWnd, hMenu );
        break;
    case WM_COMMAND:
        {
            switch( LOWORD( wParam ) )
            {
            case ID_MENU_OPEN:
                if(! g_hDDWnd)
                    RaysDDWinMain (g_hInstance, 0, 0, SW_SHOW);
                //MessageBox( hWnd, "RaysDDWinMain OK", "MainWnd", MB_OK);
                return 0;
                break;
            case ID_MENU_CLOSE:
                if( g_hDDWnd )
                    DestroyWindow( g_hDDWnd );
                return 0;
                break;
            case ID_MENU_EXIT:
                PostQuitMessage(0);
                DestroyWindow( hWnd );
                exit(0);
                //return 0;
                break;
            case ID_MENU_DOT:
                if( bIsFinishTask )
                {
                    ShowWindow( g_hDDWnd, SW_SHOW );
                    SendMessage(g_hDDWnd, WM_USER+1, 0, 0);
                }
                else
                {
                    MessageBox(g_hDDWnd, "Wait for Last Task", "Warning", MB_OK);
                }
                break;
            case ID_MENU_LINE:
                if( bIsFinishTask )
                {
                    ShowWindow( g_hDDWnd, SW_SHOW );
                    SendMessage(g_hDDWnd, WM_USER+2, 0, 0);
                }
                else
                {
                    MessageBox(g_hDDWnd, "Wait for Last Task", "Warning", MB_OK);
                }
                break;
            case ID_MENU_CIRCLE:
                SendMessage(g_hDDWnd, WM_USER+3, 0, 0);
                break;
            case ID_MENU_CHKSHOWDDWINDOW:
                {
                    BOOL bRet;
                    UINT uState;
                    UINT fState;
                    MENUITEMINFO    mii;
                    int nCmdShow;
                    
                    if(! hMenu )
                    {
                        MessageBox(g_hDDWnd, "hMenu is null", "Info", MB_OK);
                    }
                    uState = GetMenuState(
                        hMenu, // handle to menu
                        ID_MENU_CHKSHOWDDWINDOW,    // menu item to query
                        MF_BYCOMMAND  // options
                        );
                    fState = (uState & MF_CHECKED) ? MFS_UNCHECKED : MFS_CHECKED;
                    nCmdShow = (uState & MF_CHECKED) ? SW_HIDE : SW_SHOW;

                    ZeroMemory(&mii, sizeof(mii));
                    mii.cbSize = sizeof(mii);
                    mii.fMask = MIIM_STATE ;
                    mii.fState = fState;
                    bRet = SetMenuItemInfo(
                        hMenu,          // handle to menu
                        ID_MENU_CHKSHOWDDWINDOW,           // identifier or position
                        FALSE, //BOOL fByPosition,     // meaning of uItem
                        &mii  // menu item information
                        );
                    if(! bRet )
                    {
                        show_win_error( g_hDDWnd );
                    }
                    ShowWindow( g_hDDWnd, nCmdShow );
                }
                break;
            }
            break;
        }
    }
    return DefWindowProc( hWnd, message, wParam, lParam );
}


int	gameSet(void)
{
	SetDDWndName( (char *)"HELLO" );
	SetDDWndIcon( LoadIcon(g_hInstance, NULL), LoadIcon(g_hInstance, NULL) );
	SetDDWndRunOnce( TRUE );
    SetDDWndScreenMode( WINDOWS_SCREEN_MODE );
    //SetDDWndProc( DDWindowProc );
    SetDDWndEnableExit( FALSE );
#ifdef  ONLY_DDWIN
    SetDDWndEnableExit( TRUE );
#endif
    SetDDWndScreenSize( SCREEN_WIDTH, SCREEN_HEIGHT );

	return 0;
}



int gameInit(void)
{
    if( FAILED( install(system)			)) FailMsg("install system failed");
    if( FAILED( install(draw)			)) FailMsg("install draw failed");
    if( FAILED( install(timer)			)) FailMsg("install timer failed");
    if( FAILED( install(input)			)) FailMsg("install input failed");
    if( FAILED( install(winfont)		)) FailMsg("install winfont failed");

	//if( FAILED( install(net)			)) FailMsg("install net failed");
//	if( FAILED( install(sound)			)) FailMsg("install sound failed");
//	if( FAILED( install(mp3)			)) FailMsg("install mp3 failed");

	//if( FAILED( install(cdrom_music)	)) FailMsg("install cdrom music failed");

    //load_mouse_image_cursor(0,(USTR *)"CURSOR.ANI");
    //show_mouse(SHOW_IMAGE_CURSOR);
    show_mouse(SHOW_WINDOW_CURSOR);

	sprintf((char *)print_rec,"	");
	log_error(1,print_rec);

    return 0;
}



USTR	mem_plane[SCREEN_WIDTH * SCREEN_HEIGHT];
PIXEL	pixel_color[256];
USTR	palette[768];


void	test_play_flic(USTR *filename)
{
	SLONG	frames;
	SLONG	i;
	SLONG	x,y,offset;
	UCHR	c;

	memset(mem_plane, 0, SCREEN_WIDTH * SCREEN_HEIGHT);
	frames = FLIC_open_flic_file((USTR*)filename, 0, mem_plane);
	for(i=0; i<frames; i++)
	{
		idle_loop();
		FLIC_read_flic_data();
		FLIC_play_flic_frame();
		if(i == 0)
		{
			FLIC_export_palette(palette);
			make_indexed_color_table(palette, pixel_color);
		}

		offset = 0;
		for(y=0; y<SCREEN_HEIGHT; y++)
		{
			for(x=0; x<SCREEN_WIDTH; x++)
			{
				c = mem_plane[offset+x];
				put_pixel(x,y,pixel_color[c],screen_buffer);
			}
			offset += SCREEN_WIDTH;
		}

		sprintf((char *)print_rec,"FILE:%s   FRAME: %d/%d ",filename, i, frames);
		print16(0,0,(USTR*)print_rec,COPY_PUT_COLOR(SYSTEM_BLUE),screen_buffer);


		update_screen(screen_buffer);

		wait_any_key();

	}
	FLIC_close_flic_file();
}



void	test_play_mp3(void)
{
	char *	mp3_filename = "TRACK1.MP3";
	char *	tga_filename = "TEST.TGA";
	char *	jpg_filename = "TEST.JPG";
	SLONG	main_pass;
	UCHR	ch;
	BMP *	fore_bitmap = NULL;
	SLONG	sx,sy;
	SLONG	volume = 0;
	unsigned char *jpg_pic = NULL;
	int jpg_width, jpg_height;
	int x,y,offset;
	unsigned char r,g,b,a;
	DWORD	first_time,after_load_time;


	main_pass = 0;
	reset_data_key();
	reset_mouse_key();


	first_time = timeGetTime();
/**/
	if(0 == LoadJPG( (const char *)jpg_filename, &jpg_pic, &jpg_width, &jpg_height ) )
	{
		after_load_time = timeGetTime();
		sprintf((char *)print_rec,"Load : %d", after_load_time - first_time);
		log_error(1, (USTR*)print_rec);

		fore_bitmap = create_bitmap(jpg_width, jpg_height);
		offset = 0;
		for(y=0; y<jpg_height; y++)
		{
			for(x=0; x<jpg_width; x++)
			{
				r = jpg_pic[offset + (x << 2) + 0];
				g = jpg_pic[offset + (x << 2) + 1];
				b = jpg_pic[offset + (x << 2) + 2];
				a = jpg_pic[offset + (x << 2) + 3];

				//abgr
				fore_bitmap->line[y][x] = rgb2hi(r,g,b);// true2hi( (r<<16) | (g<<8) | b );
				//fore_bitmap->line[y][x] = true2hi( *(ULONG*)&jpg_pic[offset + (x << 2) + 0]);
			}
			offset += (jpg_width << 2);
		}

		sprintf((char *)print_rec,"Convert : %d", timeGetTime() - after_load_time );
		log_error(1, (USTR*)print_rec);
	}
	else
/**/
	{
		fore_bitmap = TGA_load_file((char *)tga_filename);
		if(NULL == fore_bitmap)
		{
			fore_bitmap = create_bitmap(320,200);
			fill_bitmap(fore_bitmap, SYSTEM_BLUE);
			return;
		}
	}
	sx = sy = 0;


    while(0==main_pass && g_hDDWnd )
//	while(main_pass == 0)
	{
		idle_loop();
/*
if(timer_tick05 > 0)
{
	destroy_bitmap(&fore_bitmap);
	if(jpg_pic)
	{
		free(jpg_pic);
		jpg_pic = NULL;
	}
	if(0 == LoadJPG( (const char *)jpg_filename, &jpg_pic, &jpg_width, &jpg_height ) )
	{
		fore_bitmap = create_bitmap(jpg_width, jpg_height);
		offset = 0;
		for(y=0; y<jpg_height; y++)
		{
			for(x=0; x<jpg_width; x++)
			{
				r = jpg_pic[offset + (x << 2) + 0];
				g = jpg_pic[offset + (x << 2) + 1];
				b = jpg_pic[offset + (x << 2) + 2];
				a = jpg_pic[offset + (x << 2) + 3];

				//abgr
				fore_bitmap->line[y][x] = rgb2hi(r,g,b);// true2hi( (r<<16) | (g<<8) | b );
				//fore_bitmap->line[y][x] = true2hi( *(ULONG*)&jpg_pic[offset + (x << 2) + 0]);
			}
			offset += (jpg_width << 2);
		}
	}
	else
	{
		fore_bitmap = TGA_load_file((char *)tga_filename);
		if(NULL == fore_bitmap)
		{
			fore_bitmap = create_bitmap(320,200);
			fill_bitmap(fore_bitmap, SYSTEM_BLUE);
			return;
		}
	}
	timer_tick05 = 0;
}
*/

		clear_bitmap(screen_buffer);
		put_bitmap(sx, sy, fore_bitmap, screen_buffer);
		sy += 2;
		if(sy > SCREEN_HEIGHT)
			sy = -fore_bitmap->h;

		sprintf((char *)print_rec,"~C0~O3PLAY MP3 TEST: FPS = %d~C0~O0",get_fps());
		print16(0,0,(USTR*)print_rec,PEST_PUT,screen_buffer);
		sprintf((char *)print_rec,"~C0~O3VOL: %d~C0~O0",volume );
		print16(0,20,(USTR*)print_rec,PEST_PUT,screen_buffer);

        put_box(1,1,SCREEN_WIDTH-2,SCREEN_HEIGHT-2,SYSTEM_RED,screen_buffer);
        put_box(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,SYSTEM_YELLOW,screen_buffer);
		update_screen(screen_buffer);

		ch = read_data_key();
		if(ch) reset_data_key();
		switch(ch)
		{
		case S_Enter:
			play_mp3(1, (USTR*)mp3_filename);
			break;
		case ' ':
			play_mp3(0, (USTR*)mp3_filename);
			break;
		case 'P':case 'p':
			stop_mp3();
			break;
		case 'X':case 'Q':case 'x':case 'q':
			break;
		case S_Esc:
			main_pass = 1;
			break;
		case '+':case '=':
			volume += 50;
			if(volume > 0)
				volume = 0;
			set_mp3_volume(volume);
			break;
		case '-':case '_':
			volume -= 50;
			if(volume < -10000)
				volume = -10000;
			set_mp3_volume(volume);
			break;
		}
	}

	reset_data_key();
	reset_mouse_key();

	destroy_bitmap(&fore_bitmap);

	if(jpg_pic)
	{
		free(jpg_pic);
		jpg_pic = NULL;
	}
}


void	test_xsound(void)
{
	ULONG	channels, sample_rate, bps, size, play_time;
	USTR	filename[]="test.wav";

	if(TTN_OK == get_wavfile_information((USTR*)filename, &channels, &sample_rate, &bps, &size, &play_time))
	{
		log_error(1, "%s : channels=%d : sample_rate=%d : bps=%d : size=%d :play_time=%d",
			filename, channels, sample_rate, bps, size, play_time);
	}
}


void    idle_main(void)
{
//    extern  void    windows_idle_loop();
//    while(TRUE)
    while( TRUE && g_hDDWnd )
//    while( TRUE )
    {
        clear_bitmap(screen_buffer);
        update_screen(screen_buffer);
//        windows_idle_loop();
        log_error(1, "before idle_loop() of idle_main() ");
        idle_loop();
        log_error(1, "after idle_loop() of idle_main() ");
    }
}


int	gameMain(void)
{
    idle_main();
//	test_xsound();
//	test_play_mp3();
	//some_test();
	/*
	test_play_flic((USTR*)"001.flc");
	test_play_flic((USTR*)"002.flc");
	test_play_flic((USTR*)"003.flc");
	test_play_flic((USTR*)"004.flc");
	*/
    return 1;
}

void gameExit(void)
{
	sprintf((char *)print_rec,"	");
	log_error(1,print_rec);
}


//============================================================================
