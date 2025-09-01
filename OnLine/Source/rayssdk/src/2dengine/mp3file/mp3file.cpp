/*
**  mp3file.cpp
**  mp3 audio-mpeg functions.
**
**  Jack, 2002.6.4.
*/
#include "rays.h"
#include "xsystem.h"
#include "mp3file.h"
#include "binfstd.h"
#include "binfarc.h"
#include "ampdec.h"
#include "mpgsplit.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// STRUCTURES                                                                                           //
//////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct  tagMP3FILE
{
    char            filename[_MAX_FNAME];
    ampegdecoder    *dec;           //mp3解碼
    binfile         *musicin;       //輸入文件
    binfile         *wavedata;      //解碼後的數據
    WAVEFORMATEX    *wfx;           //解碼後WAVE格式
    //
    sbinfile        *smusicin;      //
    abinfile        *awavedata;     //
    mpegsplitstream *mpgsplit;      //
} MP3FILE, *LPMP3FILE;



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS                                                                                            //
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void  * mp3_open(char *filename)
{
    MP3FILE *fp = NULL;

    fp = (MP3FILE *)malloc(sizeof(*fp));
    if(NULL == fp)
    {
        log_error(1, "memory alloc error");
        return  NULL;
    }
    fp->dec = 0;
    fp->musicin = 0;
    fp->wavedata = 0;
    fp->wfx = 0;
    fp->smusicin = 0;
    fp->awavedata = 0;
    fp->mpgsplit = 0;


    //binfile *musicin=0;
    //sbinfile smusicin;
    fp->musicin = 0;
    fp->smusicin = new sbinfile;

    if (fp->smusicin->open(filename, sbinfile::openro)<0)
    {
        log_error(1, "open file %s error", filename);
        return NULL;
    }
    //musicin=&smusicin;
    fp->musicin=fp->smusicin;
    
    //abinfile awavedata;
    //mpegsplitstream mpgsplit;
    //binfile *wavedata = musicin;
    fp->awavedata = new abinfile;
    fp->mpgsplit = new mpegsplitstream;
    fp->wavedata = fp->musicin;

    if (peekub4(*fp->musicin)==0x1BA) // full mpeg stream
    {
        if (fp->mpgsplit->open(*fp->musicin, 0xC0, 0))
        {
            log_error(1, "mpeg stream does not contain audio data");
            return  NULL;
        }
        fp->wavedata = fp->mpgsplit;
    }
    else if (peekub4(*fp->musicin)==0x52494646) // RIFF aka .WAV
    {
        char riff[4];
        int blklen=12;
        while (blklen&&!fp->musicin->ioctl(fp->musicin->ioctlreof))
            blklen -= fp->musicin->read(0, blklen);
        while (1)
        {
            if (fp->musicin->read(riff, 4)!=4)
            {
                log_error(1, "Could not find data chunk in RIFF file");
                return  NULL;
            }
            blklen = getil4(*fp->musicin);
            if (!memcmp(riff, "data", 4))
                break;
            while (blklen&&!fp->musicin->ioctl(fp->musicin->ioctlreof))
                blklen -= fp->musicin->read(0, blklen);
        }
        fp->awavedata->open(*fp->musicin, fp->musicin->tell(), blklen);
        fp->wavedata = fp->awavedata;
    }
    else if (peekub4(*fp->musicin)==0x494E464F) // what's this? ah: I N F O ... ? did i do this?
    {
        getib4(*fp->musicin);
        int blklen = getib4(*fp->musicin);
        while (blklen&&!fp->musicin->ioctl(fp->musicin->ioctlreof))
            blklen -= fp->musicin->read(0, blklen);
        if (fp->musicin->getmode()&binfile::modeseek)
        {
            char tag[3];
            int pos=fp->musicin->tell();
            fp->musicin->seekend(-128);
            fp->musicin->read(tag, 3);
            fp->awavedata->open(*fp->musicin, pos, fp->musicin->length()-pos-(memcmp(tag, "TAG", 3)?0:128));
            fp->wavedata = fp->awavedata;
        }
    }
    else if (peekub4(*fp->musicin)&0xFFFFFF00==0x49443300) // ID3v2
    {
        getib4(*fp->musicin);
        getib2(*fp->musicin);
        int blklen=getu1(*fp->musicin)<<21;
        blklen+=getu1(*fp->musicin)<<14;
        blklen+=getu1(*fp->musicin)<<7;
        blklen+=getu1(*fp->musicin);
        while (blklen&&!fp->musicin->ioctl(fp->musicin->ioctlreof))
            blklen-=fp->musicin->read(0, blklen);
        if (fp->musicin->getmode()&binfile::modeseek)
        {
            fp->awavedata->open(*fp->musicin, fp->musicin->tell(), fp->musicin->length()-fp->musicin->tell());
            fp->wavedata = fp->awavedata;
        }
    }
    else if (peekub2(*fp->musicin)<0xFFF0)
    {
        int blklen=1024;
        while (blklen&&!fp->musicin->ioctl(fp->musicin->ioctlreof))
        {
            int s=peekub2(*fp->musicin);
            if ((s>=0xFFF0)&&(s!=0xFFFF))
                break;
            blklen-=fp->musicin->read(0, 1);
        }
        if (fp->musicin->getmode()&binfile::modeseek)
        {
            char tag[3];
            int pos=fp->musicin->tell();
            fp->musicin->seekend(-128);
            fp->musicin->read(tag, 3);
            fp->awavedata->open(*fp->musicin, pos, fp->musicin->length()-pos-(memcmp(tag, "TAG", 3)?0:128));
            fp->wavedata = fp->awavedata;
        }
    }
    else if (fp->musicin->getmode()&binfile::modeseek)
    {
        char tag[3];
        fp->musicin->seekend(-128);
        fp->musicin->read(tag, 3);
        fp->awavedata->open(*fp->musicin, 0, fp->musicin->length()-(memcmp(tag, "TAG", 3)?0:128));
        fp->wavedata = fp->awavedata;
    }
    
    int freq,stereo,fmt,down,chn;
    
    //static ampegdecoder dec;
    fp->dec = new ampegdecoder;
    
    down = 0; 
    chn = 0;
    fmt = 1;
    if (fp->dec->open(*fp->wavedata, freq, stereo, fmt, down, chn))
    {
        log_error(1, "Could not open audio mpeg");
        return  NULL;
    }

    fp->wfx = new WAVEFORMATEX;
    fp->wfx->wFormatTag = WAVE_FORMAT_PCM;
    fp->wfx->nChannels = stereo ? 2 : 1;
    fp->wfx->nSamplesPerSec = freq;
    fp->wfx->wBitsPerSample = 16;
    fp->wfx->nBlockAlign = fp->wfx->nChannels * fp->wfx->wBitsPerSample / 8;
    fp->wfx->nAvgBytesPerSec = fp->wfx->nSamplesPerSec * fp->wfx->nBlockAlign;
    fp->wfx->cbSize = 0;

    return  (void*)fp;
}
		

int mp3_read(void *vfp, void *sampbuf, int len)
{
    MP3FILE *fp = (MP3FILE *)vfp;
    int readlen;

    if(!fp) return  -1;
    if(!fp->dec) return -2;
    if(fp->dec->eof())  return  -3;

	readlen = fp->dec->read(sampbuf, len);
    if (! readlen)  return  -4;

    return  readlen;
}


void    mp3_close(void *vfp)
{
    MP3FILE *fp = (MP3FILE *)vfp;

    if(!fp) return;

    if(fp->dec) 
    {
        fp->dec->close();
        delete fp->dec;
        fp->dec = NULL;
    }
    if(fp->wavedata)
    {
        fp->wavedata->close();
    }
    if(fp->musicin)
    {
        fp->musicin->close();
    }
    if(fp->wfx)
    {
        delete fp->wfx;
        fp->wfx = NULL;
    }
    if(fp->smusicin)
    {
        delete fp->smusicin;
        fp->smusicin = NULL;
    }
    if(fp->awavedata)
    {
        delete  fp->awavedata;
        fp->awavedata = NULL;
    }
    if(fp->mpgsplit)
    {
        delete  fp->mpgsplit;
        fp->mpgsplit = NULL;
    }
    free(fp);
}


void *  mp3_waveformatex(void *vfp)
{
    MP3FILE *fp = (MP3FILE *)vfp;

    if(!fp) return NULL;
    return  (void*)fp->wfx;
}


int     mp3_to_wave(char *mp3_filename, char *wave_filename)
{
    void    *mp3file = NULL;
    FILE    *fp = NULL;
    WAVEFORMATEX    *wfx;
    DWORD   waveid;
    SLONG   data_block_start, data_start;
    struct BLOCKHEAD
    {
        DWORD id;
        DWORD size;
    } blockhead,riffhead;
    char    samp_buffer[4608];
    int     samp_len = 4608, read_len;


    // Open MP3 file for read.
    if(NULL == (mp3file = mp3_open((char *)mp3_filename)))
    {
        log_error(1, "open mp3 file %s error", mp3_filename);
        goto _error;
    }

    // Open wave file for write.
    if(NULL == (fp = fopen((const char *)wave_filename, "wb")))
    {
        log_error(1, "open wave file %s error", wave_filename);
        goto _error;
    }

    // Get MP3 's wave format.
    wfx = (WAVEFORMATEX *)mp3_waveformatex(mp3file);
    if(!wfx)
    {
        log_error(1, "get mp3 %s 's waveformat error", mp3_filename);
        goto _error;
    }

    // Write 'RIFF' head. we simply set riffhead.size to 0, later we will update
    // the size.
    riffhead.id = 'FFIR';
    riffhead.size = 0;
    fwrite(&riffhead, 1, sizeof(riffhead), fp);

    // 'WAVE'.
    waveid = 'EVAW';
    fwrite(&waveid, 1, sizeof(waveid), fp);

    // Write wave format('fmt ' + WAVEFORMATEX) block.
    blockhead.id = ' tmf';
    blockhead.size = sizeof(*wfx) - sizeof(WORD);   //- (WORD cbSize)
    fwrite(&blockhead, 1, sizeof(blockhead), fp);
    fwrite(wfx, 1, sizeof(*wfx) - sizeof(WORD), fp);

    // Write 'data' block.
    data_block_start = ftell(fp);
    blockhead.id = 'atad';
    blockhead.size = 0;
    fwrite(&blockhead, 1, sizeof(blockhead), fp);
    data_start = ftell(fp);

    // Write wave data stream.

    read_len = mp3_read(mp3file, samp_buffer, samp_len);
    while(read_len > 0)
    {
        fwrite(samp_buffer, 1, read_len, fp);
        read_len = mp3_read(mp3file, samp_buffer, samp_len);
    }
    riffhead.id = 'FFIR';
    riffhead.size = ftell(fp) - sizeof(riffhead);
    blockhead.id = 'atad';
    blockhead.size = ftell(fp) - data_start;
    
    fseek(fp, 0, SEEK_SET);
    fwrite(&riffhead, 1, sizeof(riffhead), fp);
    fseek(fp, data_block_start, SEEK_SET);
    fwrite(&blockhead, 1, sizeof(blockhead), fp);
    
    if(fp) fclose(fp);
    if(mp3file) mp3_close(mp3file);

    return  0;
_error:
    if(mp3file) mp3_close(mp3file);
    if(fp) fclose(fp);
    return  -1;
}
