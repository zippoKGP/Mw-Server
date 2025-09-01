/*
**  XWAVREAD.H
**  Wav file read & playing use directsound buffers.
**
**  ZJian,2001.5.14.
*/
#ifndef _XWAVEREAD_H_INCLUDE_
#define _XWAVEREAD_H_INCLUDE_       1


#include <mmreg.h>
#include <mmsystem.h>


//-----------------------------------------------------------------------------
// Name: class CWaveSoundRead
// Desc: A class to read in sound data from a Wave file
//-----------------------------------------------------------------------------
class CWaveSoundRead
{
public:
    WAVEFORMATEX* m_pwfx;        // Pointer to WAVEFORMATEX structure
    HMMIO         m_hmmioIn;     // MM I/O handle for the WAVE
    MMCKINFO      m_ckIn;        // Multimedia RIFF chunk
    MMCKINFO      m_ckInRiff;    // Use in opening a WAVE file

public:
    CWaveSoundRead();
    ~CWaveSoundRead();

    HRESULT Open( CHAR* strFilename );
    HRESULT Reset();
    HRESULT Read( UINT nSizeToRead, BYTE* pbData, UINT* pnSizeRead );
    HRESULT Close();

};


#endif//_XWAVEREAD_H_INCLUDE_

