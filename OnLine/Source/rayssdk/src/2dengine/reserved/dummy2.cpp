#if 0
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

static	void	_normal_update_screen(BMP *bmp)
{
	HRESULT hRet=0;
	LPSTR pBits=NULL;
	int i=0;

	pBits=NULL;
	i=0;
	hRet=0;
	MessageBox(NULL,"YUCD","",MB_OK);
#ifdef OFFDD
	MessageBox(NULL,"GDI","",MB_OK);
	update_to_GDI();
#else//!OFFDD
	if(g_bProgramActive)
	{
		ZeroMemory(&ddsd,sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		hRet = lpDDSBack7->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
		//hRet = lpDDSPrimary7->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
		if (DD_OK != hRet)
			FailMsg("PrimarySurface lock failed");

//old version
        /*
		pBits = (LPSTR) ddsd.lpSurface;
		for (i = 0; i < bmp->h; i++)
		{
			memcpy(pBits, bmp->line[i], bmp->pitch);
			pBits += ddsd.lPitch;
		}
        */
// ZJian,2000.10.20
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
		//hRet=lpDDSPrimary7->Unlock(NULL);
		if(DD_OK != hRet)
			FailMsg("PrimarySurface unlock failed");
		hRet = lpDDSPrimary7->Flip(NULL,DDFLIP_WAIT);
		if(DD_OK != hRet)	FailMsg("PrimarySurface Flip failed");
	}
#endif//OFFDD
}

EXPORT  void    FNBACK  clear_bitmap(BMP *bmp)
{
	if(NULL==bmp) return;
	for(SLONG y=0;y<bmp->h;y++)
	{
		memset(bmp->line[y],0x00,bmp->pitch);
/*
#ifdef	DEBUG
		for(SLONG x=0;x<bmp->w;x++)
        {
            bmp->line[y][x]=0x00;
        }
#else//!DEBUG
		memset(bmp->line[y],0x00,bmp->pitch);
#endif//DEBUG
*/
	}
}

static	int init_GDI()
{
   int     i=0;
   PIXEL * buf=NULL;
   BITMAPINFO BMPINFO=
   {
      {
         sizeof(BITMAPINFOHEADER),
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            1,SCREEN_BPP,BI_RGB,0,0,0,0,0
      }
   };
   WndDC=GetDC(g_hWnd);
   MemDC = CreateCompatibleDC(WndDC);
   HBMP=CreateDIBSection(WndDC,&BMPINFO,DIB_PAL_COLORS,
      (void **)&buf,NULL,0);
   SelectObject(MemDC, HBMP);

   destroy_bitmap(&screen_buffer);
   screen_buffer=create_mirror_bitmap(SCREEN_WIDTH,SCREEN_HEIGHT);
   for (i=SCREEN_HEIGHT-1;i>=0;i--,buf+=SCREEN_WIDTH)
      screen_buffer->line[i]=buf;
   screen_buffer->pitch=-screen_buffer->pitch;
   return 0;
}



static	void    update_to_GDI(void)
{
   SelectObject(MemDC, HBMP);
   BitBlt(WndDC,0,update_startline,SCREEN_WIDTH,update_height,MemDC,0,update_startline,SRCCOPY);
}



static	void    free_GDI()
{
   DeleteObject(HBMP);
   ReleaseDC(g_hWnd,MemDC);
   ReleaseDC(g_hWnd,WndDC);
}


static  void    _fit_window_size(DWORD dwMode)
{
    RECT    rcl;
#if 1
    DWORD   dwStyle;
    HBRUSH  hbrBackground;

    SetRect(&rcl, 0, 0, SCREEN_WIDTH,SCREEN_HEIGHT);

    if( dwMode == FULL_SCREEN_MODE)
    {
        hbrBackground = NULL;
        dwStyle = WS_POPUP;
        AdjustWindowRect( &rcl,	dwStyle, g_bMenuPresent );
    }
    else if( dwMode == WINDOWS_SCREEN_MODE)
    {
        hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
        dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX ;
        AdjustWindowRect( &rcl,	dwStyle, g_bMenuPresent );
    }
    else
    {
        hbrBackground = NULL;
        dwStyle = WS_POPUP;
        AdjustWindowRect( &rcl,	dwStyle, g_bMenuPresent );
    }

	g_rcSystemWindow.left = rcl.left+(GetSystemMetrics(SM_CXSCREEN)-SCREEN_WIDTH)/2;
	g_rcSystemWindow.top = rcl.top+(GetSystemMetrics(SM_CYSCREEN)-SCREEN_HEIGHT)/2;
	g_rcSystemWindow.right = rcl.right+(GetSystemMetrics(SM_CXSCREEN)-SCREEN_WIDTH)/2;
	g_rcSystemWindow.bottom = rcl.bottom+(GetSystemMetrics(SM_CYSCREEN)-SCREEN_HEIGHT)/2;

    /*
    {
        int system_cx = GetSystemMetrics(SM_CXSCREEN);
        int system_cy = GetSystemMetrics(SM_CYSCREEN);
        CHAR msg[1024];

        sprintf((char *)msg, "rcl: left=%d, top=%d, right=%d, bottom=%d\n\
            g_rcSystemWindow: left=%d, top=%d, right=%d, bottom=%d\n\
            system: cx=%d, cy=%d\n",
            rcl.left, rcl.top, rcl.right, rcl.bottom,\
            g_rcSystemWindow.left, g_rcSystemWindow.top, g_rcSystemWindow.right, g_rcSystemWindow.bottom, \
            system_cx, system_cy
            );
        MessageBox(g_hWnd, (LPCSTR)msg, (LPCSTR)"Information", MB_OK );
    }
    */


    SetClassLong( g_hWnd, GCL_HBRBACKGROUND, (LONG)hbrBackground );
    //MessageBox(g_hWnd, (LPCSTR)"After SetClassLong", (LPCSTR)"Information", MB_OK );

    SetWindowLong( g_hWnd, GWL_STYLE, dwStyle);
    //MessageBox(g_hWnd, (LPCSTR)"After SetWindowLong", (LPCSTR)"Information", MB_OK );

    SetWindowPos( g_hWnd, NULL, 
		g_rcSystemWindow.left,
		g_rcSystemWindow.top,
		g_rcSystemWindow.right - g_rcSystemWindow.left,
		g_rcSystemWindow.bottom - g_rcSystemWindow.top,
        SWP_SHOWWINDOW
        );
    //MessageBox(g_hWnd, (LPCSTR)"After SetWindowPos", (LPCSTR)"Information", MB_OK );

#else
    SetClassLong( g_hWnd, GCL_HBRBACKGROUND, (LONG)screenmode_info[dwMode].hbrBackground );
//    MessageBox(g_hWnd, (LPCSTR)"After SetClassLong", (LPCSTR)"Information", MB_OK );

    SetWindowLong( g_hWnd, GWL_STYLE, screenmode_info[dwMode].dwWindowStyle );
//    MessageBox(g_hWnd, (LPCSTR)"After SetWindowLong", (LPCSTR)"Information", MB_OK );

    SetRect( &rcl, 0, 0, SCREEN_WIDTH,SCREEN_HEIGHT);
    AdjustWindowRect( &rcl, screenmode_info[dwMode].dwWindowStyle, g_bMenuPresent );
    SetRect( &screenmode_info[g_dwMode].rcWindow,
        rcl.left + (GetSystemMetrics(SM_CXSCREEN)-SCREEN_WIDTH)/2,
        rcl.top + (GetSystemMetrics(SM_CYSCREEN)-SCREEN_HEIGHT)/2,
        rcl.right + (GetSystemMetrics(SM_CXSCREEN)-SCREEN_WIDTH)/2,
        rcl.bottom + (GetSystemMetrics(SM_CYSCREEN)-SCREEN_HEIGHT)/2 );

    SetWindowPos( g_hWnd, NULL, 
		screenmode_info[dwMode].rcWindow.left,
		screenmode_info[dwMode].rcWindow.top,
		screenmode_info[dwMode].rcWindow.right - screenmode_info[dwMode].rcWindow.left,
		screenmode_info[dwMode].rcWindow.bottom - screenmode_info[dwMode].rcWindow.top,
        SWP_SHOWWINDOW
        );
//    MessageBox(g_hWnd, (LPCSTR)"After SetWindowPos", (LPCSTR)"Information", MB_OK );

#endif
}


static	BOOL	doInit(int nCmdShow)
{
    WNDCLASSEX  wc;
    DWORD   dwMainWndStyle;

#if 0
    //(1) MainWindow ---------------------------------------------------------------
    // register main window class
    dwMainWndStyle = WS_OVERLAPPEDWINDOW;

	wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = local_pfnMainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = g_hInstance;
	wc.hIcon = local_hIcon;     //  LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_GODLAST) );
	wc.hIconSm = local_hIconSm; //  LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_GODLAST) );
    wc.hCursor = NULL;
    wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
    wc.lpszMenuName = local_MenuName;
    wc.lpszClassName = local_MainWndClassName;
    if( ! RegisterClassEx(&wc) )
    {
        MessageBox(NULL, "RegisterClassEx for MainWnd failed", "INFO", MB_OK);
        return FALSE;
    }

    // create main window
    g_hMainWnd = CreateWindowEx(
        0,
        local_MainWndClassName,  // class name
        local_AppName,
        dwMainWndStyle,
        0,
        0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        NULL,           // parent
        local_hMenu,    // menu
        g_hInstance,    // instance
        NULL            // lpPara
        );
    if( !g_hMainWnd)
    {
        MessageBox(NULL, "CreateWindowEx for MainWnd failed", "INFO", MB_OK);
        return FALSE;
    }

    ShowWindow(g_hMainWnd, nCmdShow);
    UpdateWindow(g_hMainWnd);
#endif//0


    //(2) DirectDraw Window -----------------------------------------------------
	wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = g_hInstance;
	wc.hIcon = local_hIcon;     //  LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_GODLAST) );
	wc.hIconSm = local_hIconSm; //  LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_GODLAST) );
    wc.hCursor = NULL;
    wc.hbrBackground =  screenmode_info[g_dwScreenMode].hbrBackground ;
    wc.lpszMenuName = NULL;//local_MenuName;
    wc.lpszClassName = local_DDWndClassName;//local_AppName;
    if(! RegisterClassEx(&wc) )
    {
        MessageBox(NULL, "RegisterClassEx for DDWnd failed", "INFO", MB_OK);
        return FALSE;
    }

    ReviewScreenModeInfo( g_dwScreenMode );

    g_hWnd = CreateWindowEx(
        WS_EX_TOPMOST,//0,
        local_DDWndClassName,  // class name
        local_AppName,
		screenmode_info[g_dwScreenMode].dwWindowStyle, // | WS_CHILD,
        screenmode_info[g_dwScreenMode].rcWindow.left,
		screenmode_info[g_dwScreenMode].rcWindow.top,
		screenmode_info[g_dwScreenMode].rcWindow.right - screenmode_info[g_dwScreenMode].rcWindow.left,
		screenmode_info[g_dwScreenMode].rcWindow.bottom - screenmode_info[g_dwScreenMode].rcWindow.top,
        NULL,//g_hMainWnd,     // parent
        NULL, //local_hMenu,    // menu
        g_hInstance,    // instance
        NULL            // lpPara
        );
    
    if(!g_hWnd)
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
        MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
        // Free the buffer.
        LocalFree( lpMsgBuf );

        return FALSE;
    }
    

    ShowWindow(g_hMainWnd, nCmdShow);
    UpdateWindow(g_hMainWnd);

    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    init_modules();
    local_bIsInitialized = TRUE;

    /*
    {
        CHAR msg[1024];
        sprintf((char *)msg, "rcl.left=%d, rcl.top=%d, rcl.right=%d, rcl.bottom=%d", 
            rcl.left, rcl.top, rcl.right, rcl.bottom);
        MessageBox(g_hWnd, (LPCSTR)msg, (LPCSTR)"Information", MB_OK );
    }
    */

    return TRUE;
} 


///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif//0
