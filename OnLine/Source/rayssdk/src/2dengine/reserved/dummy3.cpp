#if 0
///////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
static  HRESULT CreateWindowedDisplay( HWND hWnd, DWORD dwWidth, DWORD dwHeight );
static  HRESULT CreateFullScreenDisplay(HWND hWnd, DWORD dwWidth, DWORD dwHeight, DWORD dwBPP );




//geaan, 2001.8.1.
static  HRESULT CreateWindowedDisplay( HWND hWnd, DWORD dwWidth, DWORD dwHeight )
{
    HRESULT hr;

    // Cleanup anything from a previous call
    DestroyDisplayObjects();

    // DDraw stuff begins here
    if(FAILED( hr = DirectDrawCreateEx(NULL, (VOID **)&lpDD7, IID_IDirectDraw7, NULL) ) )
    {
        return FailMsg("DirectDraw init failed");
    }


}

static  HRESULT CreateFullScreenDisplay(HWND hWnd, DWORD dwWidth, DWORD dwHeight, DWORD dwBPP )
{
    HRESULT hr;

    // Cleanup anything from a previous call
    DestroyDisplayObjects();

    // DDraw stuff begins here
    if(FAILED( hr = DirectDrawCreateEx(NULL, (VOID **)&lpDD7, IID_IDirectDraw7, NULL) ) )
    {
        return FailMsg("DirectDraw init failed");
    }

    //Set cooperative level
    if(FAILED( hr = lpDD7->SetCooperativeLevel( hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN ) ) )
    {
        return FailMsg("DirectDraw SetCooperativelevel failed");
    }

    //Set display mode
    if(FAILED( hr = lpDD7->SetDisplayMode( dwWidth, dwHeight, dwBPP, 0, 0 ) ) )
    {
        return FailMsg("Display Mode set failed");
    }

    // Create primary surface (with backbuffer attached)
    DDSURFACEDESC2 ddsd;

    nBackBuffers = 1;
    ZeroMemory( &ddsd, sizeof( ddsd ) );
    ddsd.dwSize            = sizeof( ddsd );
    ddsd.dwFlags           = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    ddsd.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP |
                             DDSCAPS_COMPLEX | DDSCAPS_3DDEVICE;
    ddsd.dwBackBufferCount = nBackBuffers;

    if( FAILED( hr = lpDD7->CreateSurface( &ddsd, &lpDDSPrimary7, NULL ) ) )
    {
        return FailMsg("Create primary surface failed");
    }

    // Get a pointer to the back buffer
    DDSCAPS2 ddscaps;

    ZeroMemory( &ddscaps, sizeof( ddscaps ) );
    ddscaps.dwCaps = DDSCAPS_BACKBUFFER;

    if( FAILED( hr = lpDDSPrimary7->GetAttachedSurface( &ddscaps, &lpDDSBack7 ) ) )
    {
        return FailMsg("Get attached surface failed");
    }
    lpDDSBack7->AddRef();

    /*
    sprintf((char *)print_rec,"     Display Mode : %dx%dx%d",SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP );
    log_error(1,print_rec);
    */
    return S_OK;
}



//INTERNAL FUNCTIONS =======================================================================================
//
extern  void    RaysDDInitScreenModeInfo(void)
{
    LONG    nMode;

    //full screen mode
    nMode = FULL_SCREEN_MODE;
    screenmode_info[nMode].hbrBackground = NULL;
    screenmode_info[nMode].dwWindowStyle = WS_POPUP;
    //the (rcWindow)'s value must be set dynamically when run

    //windows screen mode
    nMode = WINDOWS_SCREEN_MODE;
    screenmode_info[nMode].hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
    screenmode_info[nMode].dwWindowStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX ;
    //the (rcWindow)'s value must be set dynamically when run
}


extern  void    RaysDDReviewScreenModeInfo( DWORD dwMode )
{
    RECT    rcl;

    SetRect( &rcl, 0, 0, SCREEN_WIDTH,SCREEN_HEIGHT);
    AdjustWindowRect( &rcl, 
        screenmode_info[dwMode].dwWindowStyle, 
        FALSE );
    SetRect( &screenmode_info[dwMode].rcWindow,
        rcl.left + (GetSystemMetrics(SM_CXSCREEN)-SCREEN_WIDTH)/2,
        rcl.top + (GetSystemMetrics(SM_CYSCREEN)-SCREEN_HEIGHT)/2,
        rcl.right + (GetSystemMetrics(SM_CXSCREEN)-SCREEN_WIDTH)/2,
        rcl.bottom + (GetSystemMetrics(SM_CYSCREEN)-SCREEN_HEIGHT)/2 
        );
}


extern  void    RaysDDFitWindowSize(DWORD dwMode)
{
    SetClassLong( g_hDDWnd, GCL_HBRBACKGROUND, (LONG)screenmode_info[dwMode].hbrBackground );
    SetWindowLong( g_hDDWnd, GWL_STYLE, screenmode_info[dwMode].dwWindowStyle );
    RaysDDReviewScreenModeInfo( dwMode );
    SetWindowPos( g_hDDWnd, NULL, 
        screenmode_info[dwMode].rcWindow.left,
        screenmode_info[dwMode].rcWindow.top,
        screenmode_info[dwMode].rcWindow.right - screenmode_info[dwMode].rcWindow.left,
        screenmode_info[dwMode].rcWindow.bottom - screenmode_info[dwMode].rcWindow.top,
        SWP_SHOWWINDOW );
}

            SetRect( &rc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );
            AdjustWindowRect( &rc, dwStyle, FALSE);
            SystemParametersInfo( SPI_GETWORKAREA, 0, &rcWork, 0 );
            SetRect( &rcWin, rcWork.left + ( (rcWork.right - rcWork.left) - (rc.right - rc.left) ) / 2,
                rcWork.top + ( (rcWork.bottom - rcWork.top) - (rc.bottom - rc.top) ) / 2,
                rcWork.left + ( (rcWork.right - rcWork.left) - (rc.right - rc.left) ) / 2 + (rc.right - rc.left),
                rcWork.top + ( (rcWork.bottom - rcWork.top) - (rc.bottom - rc.top) ) / 2 + (rc.bottom - rc.top) );
            SetWindowPos( hWnd, NULL,
                rcWin.left,
                rcWin.top,
                rcWin.right - rcWin.left,
                rcWin.bottom - rcWin.top,
                SWP_SHOWWINDOW );

    /*
    sprintf((char *)print_rec, "g_rcWork = (%d, %d)-(%d, %d)=(%d x %d)", 
        rcWork.left, rcWork.top,
        rcWork.right, rcWork.bottom, 
        rcWork.right - rcWork.left, rcWork.bottom - rcWork.top);
    MessageBox( NULL, (LPCTSTR)print_rec, "INFO", MB_OK | MB_ICONINFORMATION );
    */

    /*
    sprintf((char *)print_rec, "rcWin = (%d, %d)-(%d, %d)=(%d x %d)", rcWin.left, rcWin.top,
        rcWin.right, rcWin.bottom, rcWin.right - rcWin.left, rcWin.bottom - rcWin.top);
    MessageBox( NULL, (LPCTSTR)print_rec, "INFO", MB_OK | MB_ICONINFORMATION );
    */


///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif//0
