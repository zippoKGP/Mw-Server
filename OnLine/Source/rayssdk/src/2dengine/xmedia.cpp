/*
**      XMEDIA.CPP
**      direct media functions.
**
**      PS: AVI(exclude MPEG method avi)
**          MPEG(need some downloaded dll driver...)
**
**      Dixon, 2000.9.12.
**          Created.
**      ZJian, 2000.10.14.
**          Added to workspace.
**      ZJian, 2000.10.20.
**          Added our own video control function.
**              We use "LPDIRECTDRAWSURFACE7 lpDDSPrimary7" to querry DirectDrawSurface before,
**              so we can't access video before. Now we use "lpDDSMemory7" to do it, and add 
**              some specially update_screen action.
**          Added seek functions.
**              If we want to seek the xmedia file for play, we can get use the following data to 
**              access it:
**                  stDuration      = duration of xmedia file
**                  stStartTime     = now start of xmedia file(seek).
**
**
*/
#include "rays.h"
#include "winmain.h"
#include "xsystem.h"
#include "xinput.h"
#include "xdraw.h"
#include "xsound.h"
#include "xmedia.h"

/* needed linking library --------------------------------------------------------------*/
#pragma comment(lib,"vfw32")
#pragma comment(lib,"amstrmid")
#pragma comment(lib,"quartz")



#define XMEDIA_STREAM           0x01
#define XMEDIA_READY            0x02

/* need these two external variable ----------------------------------------------------*/
extern  LPDIRECTDRAW7           lpDD7;          /* system directdraw object             */
//extern  LPDIRECTDRAWSURFACE7    lpPrimary7;     /* used to render directly to screen    */
//extern  LPDIRECTDRAWSURFACE7    lpDDBack7;      /* back directdraw surface              */
extern  LPDIRECTDRAWSURFACE7    lpDDSMemory7;   /* memory directdraw surface            */

/* datas needed in this file -----------------------------------------------------------*/
IAMMultiMediaStream     *m_pMMStream=NULL;
IMediaStream            *m_pPMStream=NULL;  
IDirectDrawMediaStream  *m_pDDStream=NULL;
IDirectDrawStreamSample *m_pSample=NULL;
DWORD                   dwXMediaFlag=0;
RECT                    rcXMedia;
STREAM_TIME             stStartTime;
STREAM_TIME             stEndTime;
STREAM_TIME             stCurrentTime;
STREAM_TIME             stDuration;

PFNREDRAW   xmedia_over_draw=NULL;

/*
**  function:   if we found some error, restore last status
**  parameter:  void
**  return:     void
*/
void    XMediaErrorRestore(void)
{
    if(m_pSample) { m_pSample->Release(); m_pSample=NULL; }
    if(m_pMMStream) { m_pMMStream->Release(); m_pMMStream=NULL; }
    if(m_pPMStream) { m_pPMStream->Release(); m_pPMStream=NULL; }
    if(m_pDDStream) { m_pDDStream->Release(); m_pDDStream=NULL; }
    CoUninitialize();
}

/*
**  function:   add video stream to directdraw interface & open a media stream file
**  parameter:  LPCTSTR szFilename  -> the media stream filename
**  return:     TRUE if success, FALSE else
*/
BOOL    RenderFileToStream(LPCTSTR  szFilename )
{
    HRESULT  hr;
    IAMMultiMediaStream*   pMMStream;
    WCHAR wFile[MAX_PATH];

    dwXMediaFlag = 0;
    
    MultiByteToWideChar(
        CP_ACP,                             /* UHINT    CodePage        */  
        0,                                  /* DWORD    dwFlags         */
        szFilename,                         /* LPCSTR   lpMultiByteStr  */
        -1,                                 /* int      cchMultiByte    */
        wFile,                              /* LPWSTR   lpWideCharStr   */
        sizeof(wFile)/sizeof(wFile[0]));    /* int      cchWideChar     */

    /*
    ** init Multi Function
    */
    CoInitialize(NULL);
    /*
    ** create a IAMMultiMediaStream Interface
    */
    hr =CoCreateInstance(
        CLSID_AMMultiMediaStream, NULL, CLSCTX_INPROC_SERVER,
        IID_IAMMultiMediaStream, (void **)&pMMStream);  
    if (FAILED(hr))
    {
        sprintf((char *)print_rec,"IAMMutiMediaStream create failed");
        log_error(1,print_rec);
        goto error;
    }
    /*
    ** define to open file state
    */
    hr=pMMStream->Initialize(STREAMTYPE_READ,0,NULL);
    if (FAILED(hr))
    {
        sprintf((char *)print_rec,"Initialize MMStream failed");
        log_error(1,print_rec);
        goto error;
    }
    else
    { 
        hr=pMMStream->AddMediaStream(NULL,&MSPID_PrimaryAudio,AMMSF_ADDDEFAULTRENDERER ,NULL); 
        if (FAILED(hr))
        {
            sprintf((char *)print_rec,"MMStream AddMediaStream filed");
            log_error(1,print_rec);
            goto error;
        }
        
    }    
    /*
    ** for DirectDraw ,Create Video Stream
    */
    hr=pMMStream->AddMediaStream(lpDD7,&MSPID_PrimaryVideo,0,NULL); 
    if (FAILED(hr))
    {
        sprintf((char *)print_rec,"MMStream AddMediaStream failed");
        log_error(1,print_rec);
        goto error;
    }
    /*
    ** open stream file
    */
    hr=pMMStream->OpenFile(wFile,0);
    if (FAILED(hr))
    {
        sprintf((char *)print_rec,"MMStream OpenFile failed");
        log_error(1,print_rec);
        goto error;
    }

    m_pMMStream=pMMStream;
    m_pMMStream->AddRef();

    /*
    ** get stream duration
    */
    m_pMMStream->GetDuration(&stDuration);

    dwXMediaFlag |= XMEDIA_STREAM ;
    return TRUE;
error:
    XMediaErrorRestore();
    return FALSE;
}

/*
**  function:   prepare for render stream to surface
**  parameter:  
**  return:     void
*/
SLONG   ReadyRenderStreamToSurface(RECT *rect)
{
    HRESULT  hr;
    DDSURFACEDESC   ddsd;
    IDirectDrawSurface * pds;

    /* have not render file to stream       */
    if( !(dwXMediaFlag & XMEDIA_STREAM) )   
        return TTN_NOT_OK;

    /* have been start  */
    if( dwXMediaFlag & XMEDIA_READY )   
        return TTN_OK;

    dwXMediaFlag &= ~XMEDIA_READY;

    hr=m_pMMStream->GetMediaStream(MSPID_PrimaryVideo,&m_pPMStream);
    if (FAILED(hr))
    {
        sprintf((char *)print_rec,"MMStream GetMediaStream failed");
        log_error(1,print_rec);
        goto error;
    }

    hr=m_pPMStream->QueryInterface(IID_IDirectDrawMediaStream,(void**)&m_pDDStream);
    if (FAILED(hr))
    {
        sprintf((char *)print_rec,"PMStream QueryInterface failed");
        log_error(1,print_rec);
        goto error;
    }

    ZeroMemory(&ddsd,sizeof(ddsd));
    ddsd.dwSize=sizeof(ddsd);
    hr=m_pDDStream->GetFormat(&ddsd,NULL,NULL,NULL);

    rcXMedia.left=(g_nScreenWidth-ddsd.dwWidth)/2;
    rcXMedia.top=(g_nScreenHeight-ddsd.dwHeight)/2;
    rcXMedia.right=rcXMedia.left+ddsd.dwWidth;
    rcXMedia.bottom=rcXMedia.top+ddsd.dwHeight;

    rect->left=rcXMedia.left;
    rect->right=rcXMedia.right;
    rect->top=rcXMedia.top;
    rect->bottom=rcXMedia.bottom;

    /*
    ** need DirectDrawSurface to access play
    ** we use lpDDSMemory7 to do it.
    */
    if(NULL==lpDDSMemory7)
    {
        sprintf((char *)print_rec,"lpDDSMemory7 didn't initialized");
        log_error(1,print_rec);
        goto error;
    }
    hr=lpDDSMemory7->QueryInterface(IID_IDirectDrawSurface,(void **)&pds);
    if(FAILED(hr))
    {
        sprintf((char *)print_rec,"lpDDSMemory7 QueryInterface failed");
        log_error(1,print_rec);
        goto error;
    }
    /*
    ** connect frame to DirectDrawSurface
    */
    hr=m_pDDStream->CreateSample(pds,&rcXMedia,0,&m_pSample);
    if (FAILED(hr))
    {
        switch(hr)
        {
        case DDERR_INVALIDRECT :
            sprintf((char *)print_rec,"DDERR_INVALIDRECT");
            break;
        case DDERR_INVALIDPIXELFORMAT :
            sprintf((char *)print_rec,"DDERR_INVALIDPIXELFORMAT");
            break;
        case DDERR_INVALIDSURFACETYPE :
            sprintf((char *)print_rec,"DDERR_INVALIDSURFACETYPE");
            break;
        case E_POINTER :
            sprintf((char *)print_rec,"E_POINTER");
            break;
        case MS_E_SAMPLEALLOC :
            sprintf((char *)print_rec,"MS_E_SAMPLEALLOC");
            break;
        default:
            sprintf((char *)print_rec,"UNKNOWN 0x%0x(%d)",hr,hr);
            break;
        }
        log_error(1,print_rec);
        goto error;
    }

    /*
    **  get stream times
    */
    //m_pSample->GetSampleTimes(&stStartTime,&stEndTime,&stCurrentTime);
    //m_pSample->SetSampleTimes(&stStartTime,&stEndTime);
    
    /*
    ** start play stream to video
    */
    m_pMMStream->SetState(STREAMSTATE_RUN);

    dwXMediaFlag |= XMEDIA_READY;
    return TTN_OK;
error:
    XMediaErrorRestore();
    return TTN_ERROR;
}

/*
**
**
*/
SLONG   SeekRenderStream(STREAM_TIME st)
{
    if( ! ( dwXMediaFlag & XMEDIA_READY ) )
        return TTN_OK;

    if(st<0) st=0;
    if(st>stDuration-1) st=stDuration-1;
    m_pMMStream->Seek(st);
    /*
    ** after we call m_pMMStream->Seek(st), st will asigned to stStartTime.
    ** so we needn't call m_pSample->SetSampleTimes(&st,&stEndTime)
    */
    // m_pSample->SetSampleTimes(&st,&stEndTime);   

    return TTN_OK;
}



/*
**  function:   render video stream frame by frame to ddsurface
**  parameter:
**  return:     
*/
SLONG   StepRenderStreamToSurface(BMP *bitmap,SLONG left_top_flag)
{
    HRESULT hr;
    SLONG ret;

    if( !(dwXMediaFlag & XMEDIA_READY) )
        return TTN_NOT_OK;

    ret=TTN_NOT_OK;
    hr = m_pSample->Update(0, NULL, NULL, NULL);
    if(S_OK == hr)
    {
        get_bitmap_from_memory_surface(bitmap,rcXMedia,left_top_flag);
        ret = TTN_OK;
    }
    m_pSample->GetSampleTimes(&stStartTime,&stEndTime,&stCurrentTime);
    return ret;
}

/*
**  function:   end render stream to surface
**  parameter:
**  return:
*/
SLONG   FinishRenderStreamToSurface()
{
    if( ! (dwXMediaFlag & XMEDIA_READY) )
        return TTN_OK;

    m_pMMStream->SetState(STREAMSTATE_STOP);
    XMediaErrorRestore();

    dwXMediaFlag = 0;

    return TTN_OK;
}



/*
**  function:   play video stream frame by frame to DirectDrawSurface
**  parameter:  sx,sy,ex,ey -> play position
**  return:     void
*/
void    RenderStreamToSurface(SLONG sx,SLONG sy,SLONG ex,SLONG ey,SLONG wait_flag)
{
    HRESULT  hr;
    DDSURFACEDESC   ddsd;
    RECT  rc;
    IDirectDrawSurface * pds;

    hr=m_pMMStream->GetMediaStream(MSPID_PrimaryVideo,&m_pPMStream);
    if (FAILED(hr))
    {
        sprintf((char *)print_rec,"MMStream GetMediaStream failed");
        log_error(1,print_rec);
        goto error;
    }

    hr=m_pPMStream->QueryInterface(IID_IDirectDrawMediaStream,(void**)&m_pDDStream);
    if (FAILED(hr))
    {
        sprintf((char *)print_rec,"PMStream QueryInterface failed");
        log_error(1,print_rec);
        goto error;
    }

    ZeroMemory(&ddsd,sizeof(ddsd));
    ddsd.dwSize=sizeof(ddsd);
    hr=m_pDDStream->GetFormat(&ddsd,NULL,NULL,NULL);

    rc.left=(g_nScreenWidth-ddsd.dwWidth)/2;
    rc.top=(g_nScreenHeight-ddsd.dwHeight)/2;
    rc.right=rc.left+ddsd.dwWidth;
    rc.bottom=rc.top+ddsd.dwHeight;

    /*
    **  need DirectDrawSurface to access play
    **
    **  if we want to update to primary surface directly, we must use lpDDSPrimary7.
    **  here we use lpDDSMemory7 to make the action of update to primary surface
    **  controled by ourselves.
    **
    */
    if(NULL==lpDDSMemory7)
    {
        sprintf((char *)print_rec,"lpDDSPrimary7 didn't initialized");
        log_error(1,print_rec);
        goto error;
    }
    hr=lpDDSMemory7->QueryInterface(IID_IDirectDrawSurface,(void **)&pds);
    if(FAILED(hr))
    {
        sprintf((char *)print_rec,"lpDDSPrimary7 QueryInterface failed");
        log_error(1,print_rec);
        goto error;
    }
    /*
    ** connect frame to DirectDrawSurface
    */
    hr=m_pDDStream->CreateSample(pds,&rc,0,&m_pSample);
    if (FAILED(hr))
    {
        switch(hr)
        {
        case DDERR_INVALIDRECT :
            sprintf((char *)print_rec,"DDERR_INVALIDRECT");
            break;
        case DDERR_INVALIDPIXELFORMAT :
            sprintf((char *)print_rec,"DDERR_INVALIDPIXELFORMAT");
            break;
        case DDERR_INVALIDSURFACETYPE :
            sprintf((char *)print_rec,"DDERR_INVALIDSURFACETYPE");
            break;
        case E_POINTER :
            sprintf((char *)print_rec,"E_POINTER");
            break;
        case MS_E_SAMPLEALLOC :
            sprintf((char *)print_rec,"MS_E_SAMPLEALLOC");
            break;
        default:
            sprintf((char *)print_rec,"UNKNOWN 0x%0x(%d)",hr,hr);
            break;
        }
        log_error(1,print_rec);
        goto error;
    }
    
    /*
    ** play stream to video
    */
    m_pMMStream->SetState(STREAMSTATE_RUN);
    while (m_pSample->Update(0, NULL, NULL, NULL) == S_OK)
    {
        idle_loop();
        if(wait_flag)
        {
            if(S_Esc==read_data_key())
                break;
            if(MS_RUp==get_mouse_key())
                break;
        }
        /*
        **  our own update screen functions.
        **  if we use lpDDSPrimary7 above,just do nothing.
        **  here we update screen by ourselves.
        */  
        clear_bitmap(screen_buffer);
        get_bitmap_from_memory_surface(screen_buffer,rc,0);
        if(xmedia_over_draw)
            xmedia_over_draw(screen_buffer);
        update_screen(screen_buffer);
    }
    m_pMMStream->SetState(STREAMSTATE_STOP);

/* end: */
error:
    XMediaErrorRestore();
}

/**************************************************************************************************/
/**************************************************************************************************/
/**************************************************************************************************/
void    set_xmedia_over_draw(PFNREDRAW my_redraw)
{
    xmedia_over_draw = my_redraw;
}

void    play_xmedia_movie(SLONG sx,SLONG sy,SLONG ex,SLONG ey,USTR *filename,SLONG wait_flag)
{
    reset_key();
    reset_mouse();
    if(TRUE==RenderFileToStream((char *)filename))
    {
        RenderStreamToSurface(sx,sy,ex,ey,wait_flag);
    }
    reset_key();
    reset_mouse();
}


SLONG   open_xmedia_file(USTR *filename,RECT *rect)
{
    if(TRUE==RenderFileToStream((char *)filename))
    {
        return ReadyRenderStreamToSurface(rect);
    }
    return TTN_NOT_OK;
}

SLONG   play_xmedia_frame(BMP *bitmap,SLONG left_top_flag)
{
    return StepRenderStreamToSurface(bitmap,left_top_flag);
}

SLONG   close_xmedia_file(void)
{
    return FinishRenderStreamToSurface();
}

void    seek_xmedia_file(STREAM_TIME st)
{
    SeekRenderStream(st);
}
