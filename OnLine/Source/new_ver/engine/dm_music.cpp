/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
 2003 ALL RIGHTS RESERVED.

*file name    : dm_music.cpp
*owner        : Andy
*description  : 
*modified     : 2004/1/13
******************************************************************************/ 

#include "engine_global.h"
#include <dshow.h>
#include "dm_music.h"
#include "ugdef.h"
#include "xmedia.h"

#pragma comment(lib, "strmiids.lib")

static int s_lVolume = 0;

//////////////////////////////////////////////////////////////////////////
//
static IGraphBuilder *m_pGraph = NULL;
static IMediaControl *m_pMediaControl = NULL;
static IMediaEventEx *m_pEvent = NULL;
static IMediaSeeking *m_pMediaSeek = NULL;
static IBasicAudio   *m_pBA = NULL;

//////////////////////////////////////////////////////////////////////////
//
static BOOL m_bMusicLoop = true;

void setVolume(void);

HRESULT initDirectShow(void);

//////////////////////////////////////////////////////////////////////////
//
void musicPlay() 
{
	setVolume();

	if (m_pMediaControl)
	{
		m_pMediaControl->Run();
	}
}

//////////////////////////////////////////////////////////////////////////
//
void musicPause() 
{
	if (m_pMediaControl)
	{
		m_pMediaControl->Pause();
	}
}

//////////////////////////////////////////////////////////////////////////
//
HRESULT musicOpen(LPCTSTR strFileName) 
{
	destoryDirectShow();

	initDirectShow();

	if (m_pGraph)
	{
		WCHAR wFileName[MAX_PATH] = {0};
		
		::MultiByteToWideChar(CP_ACP, 0, strFileName, -1, wFileName, MAX_PATH);
		
		if (FAILED(m_pGraph->RenderFile(wFileName, NULL)))
		{
			return E_FAIL;
		}		
	}
	
	return S_OK;
}

void musicStop(void)
{
	if (m_pMediaControl)
	{
		m_pMediaControl->Stop();
	}
}

HRESULT handleMusicEvent(void)
{
    LONG evCode, evParam1, evParam2;
	
    HRESULT hr=S_OK;
	

		
    // Make sure that we don't access the media event interface
    // after it has already been released.
    if (!m_pEvent) return S_OK;
	
    // Process all queued events
    while(SUCCEEDED(m_pEvent->GetEvent(&evCode, (LONG_PTR*)&evParam1,
		(LONG_PTR*) &evParam2, 0)))
    {
        // Free memory associated with callback, since we're not using it
        hr = m_pEvent->FreeEventParams(evCode, evParam1, evParam2);
		
        // If this is the end of the clip, reset to beginning
        if (EC_COMPLETE == evCode && m_bMusicLoop)
        {
            LONGLONG pos=0;
			
            // Reset to first frame of movie
            hr = m_pMediaSeek->SetPositions(&pos, AM_SEEKING_AbsolutePositioning ,
				NULL, AM_SEEKING_NoPositioning);
            if (FAILED(hr))
            {
                // Some custom filters (like the Windows CE MIDI filter)
                // may not implement seeking interfaces (IMediaSeeking)
                // to allow seeking to the start.  In that case, just stop
                // and restart for the same effect.  This should not be
                // necessary in most cases.
                if (FAILED(hr = m_pMediaControl->Stop()))
                {
                    break;
                }
				
                if (FAILED(hr = m_pMediaControl->Run()))
                {
                    break;
                }
            }
        }
    }

    return hr;
}

void setVolume(void)
{
	if (m_pBA)
	{
		m_pBA->put_Volume(s_lVolume);
	}
}

#define VOLUME 2500

void setMusicVolume(int volume)
{
	volume %= 101;

	s_lVolume = (float(volume)/100) * VOLUME - VOLUME;

	setVolume();
}

//////////////////////////////////////////////////////////////////////////
//
HRESULT initDirectShow(void)
{
	CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
		IID_IGraphBuilder, (void **)&m_pGraph);

	if (m_pGraph)
	{
		m_pGraph->QueryInterface(IID_IMediaControl, (void **)&m_pMediaControl);
		m_pGraph->QueryInterface(IID_IMediaEventEx, (void **)&m_pEvent);
		m_pGraph->QueryInterface(IID_IMediaSeeking, (void **)&m_pMediaSeek);	
		m_pGraph->QueryInterface(IID_IBasicAudio, (void**)&m_pBA);
		
		if (m_pEvent)
		{
			m_pEvent->SetNotifyWindow((OAHWND)g_hDDWnd, WM_GRAPHNOTIFY, 0);					
		}
	}
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//
HRESULT destoryDirectShow(void)
{
	musicStop();
	
	SAFE_RELEASE(m_pMediaSeek);
	
	SAFE_RELEASE(m_pEvent);
	
	SAFE_RELEASE(m_pMediaControl);
	
	SAFE_RELEASE(m_pBA);
	
	SAFE_RELEASE(m_pGraph);
	
	return S_OK;
}
