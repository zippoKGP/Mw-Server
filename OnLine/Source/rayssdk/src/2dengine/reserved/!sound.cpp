/*
**  sound.cpp
**  sound functions.
**
**  Jack, 2002.4.6.
*/
#include "rays.h"
#include "winmain.h"
#include "xsystem.h"
#include "packfile.h"
#include "sound.h"

#pragma comment(lib,"dsound")



///////////////////////////////////////////////////////////////////////////////////////////////////////
// W A V E    F I L E     F U N C T I O N S                                                          //
///////////////////////////////////////////////////////////////////////////////////////////////////////
EXPORT  SLONG   FNBACK  get_wav_format(PACK_FILE *hfile, WAVEFORMATEX *wf, SLONG *block_size, SLONG *wave_size)
{
    SLONG   i, point;
    DWORD   waveid;
    struct BLOCKHEAD
    {
        DWORD id;
        DWORD size;
    } blockhead,riffhead;
    
    
    *wave_size=0;
    while(1)
    {
        i = 0;
        if( (pack_fread(&riffhead,1, sizeof(riffhead), hfile)) != sizeof(riffhead) )
            return TTN_ERROR;
        if(riffhead.id != 'FFIR')
        {
            pack_fseek(hfile,riffhead.size,SEEK_CUR);
            continue;
        }
        if( (pack_fread(&waveid,1, sizeof(waveid), hfile)) != sizeof(waveid) )
            return TTN_ERROR;

        i += sizeof(waveid);
        if(waveid!='EVAW')
        {
            pack_fseek(hfile, riffhead.size-i, SEEK_CUR);
            continue;
        }
        while(1)
        {
            if(i >= (SLONG)riffhead.size) break;
            if((pack_fread(&blockhead,1, sizeof(blockhead), hfile)) != sizeof(blockhead))
                return TTN_ERROR;

            i += sizeof(blockhead);
            if(blockhead.id!=' tmf')
            {
                pack_fseek(hfile, blockhead.size, SEEK_CUR);
                i+=blockhead.size;
                continue;
            }
            if((pack_fread(wf,1, blockhead.size, hfile)) != blockhead.size)
                return TTN_ERROR;

            i += blockhead.size;
            wf->cbSize=0;
            *block_size=(SLONG)riffhead.size-i;
            
            point = pack_ftell(hfile);
            while(1)
            {
                if(i >= (int)riffhead.size) break;
                if((pack_fread(&blockhead, 1, sizeof(blockhead), hfile)) != sizeof(blockhead))
                    return FALSE;
                i += sizeof(blockhead);
                if(blockhead.id == 'atad') *wave_size += blockhead.size;
                pack_fseek(hfile, blockhead.size, SEEK_CUR);
                i += blockhead.size;
            }
            pack_fseek(hfile,point,SEEK_SET);
            return TTN_OK;
        }
    }
}


EXPORT  SLONG   FNBACK  get_wav_data(PACK_FILE *hfile, USTR *data, SLONG blocksize)
{
    SLONG   i = 0, j = 0;
    struct 
    {
        DWORD id;
        DWORD size;
    } blockhead;
    
    while(1)
    {
        if(i >= blocksize) break;
        if((pack_fread(&blockhead,1, sizeof(blockhead), hfile)) != sizeof(blockhead))
            return TTN_ERROR;

        i += sizeof(blockhead);
        if(blockhead.id != 'atad')
        {
            pack_fseek(hfile, blockhead.size, SEEK_CUR);
            i += blockhead.size;
            continue;
        }
        if((pack_fread(data += j,1, blockhead.size, hfile)) != blockhead.size)
            return TTN_ERROR;

        j += blockhead.size;
        i += blockhead.size;
    }
    return TTN_OK;
}


EXPORT  SLONG   FNBACK  get_buffer_wav_format(USTR *buffer, SLONG *buffer_offset, WAVEFORMATEX *wf, SLONG *block_size, SLONG *wave_size)
{
    SLONG   i, point, offset;
    DWORD   waveid;
    struct BLOCKHEAD
    {
        DWORD id;
        DWORD size;
    } blockhead,riffhead;
    
    offset = 0;
    *buffer_offset = 0;
    *wave_size=0;
    while(1)
    {
        i = 0;

        //if( (pack_fread(&riffhead,1, sizeof(riffhead), hfile)) != sizeof(riffhead) )
            //return TTN_ERROR;
        memcpy(&riffhead, &buffer[offset], sizeof(riffhead));
        offset += sizeof(riffhead);

        if(riffhead.id != 'FFIR')
        {
            //pack_fseek(hfile,riffhead.size,SEEK_CUR);
            offset += riffhead.size;

            continue;
        }
        //if( (pack_fread(&waveid,1, sizeof(waveid), hfile)) != sizeof(waveid) )
          //  return TTN_ERROR;
        memcpy(&waveid, &buffer[offset], sizeof(waveid));
        offset += sizeof(waveid);

        i += sizeof(waveid);
        if(waveid!='EVAW')
        {
            //pack_fseek(hfile, riffhead.size-i, SEEK_CUR);
            offset += ((SLONG)riffhead.size - i);

            continue;
        }
        while(1)
        {
            if(i >= (SLONG)riffhead.size) break;

            //if((pack_fread(&blockhead,1, sizeof(blockhead), hfile)) != sizeof(blockhead))
              //  return TTN_ERROR;
            memcpy(&blockhead, &buffer[offset], sizeof(blockhead));
            offset += sizeof(blockhead);

            i += sizeof(blockhead);
            if(blockhead.id!=' tmf')
            {
                //pack_fseek(hfile, blockhead.size, SEEK_CUR);
                offset += blockhead.size;

                i += blockhead.size;
                continue;
            }
            //if((pack_fread(wf,1, blockhead.size, hfile)) != blockhead.size)
              //  return TTN_ERROR;
            memcpy(wf, &buffer[offset], blockhead.size);
            offset += blockhead.size;

            i += blockhead.size;
            wf->cbSize=0;
            *block_size=(SLONG)riffhead.size-i;
            
            //point = pack_ftell(hfile);
            point = offset;

            while(1)
            {
                if(i >= (int)riffhead.size) break;
                //if((pack_fread(&blockhead, 1, sizeof(blockhead), hfile)) != sizeof(blockhead))
                  //  return FALSE;
                memcpy(&blockhead, &buffer[offset], sizeof(blockhead));
                offset += sizeof(blockhead);

                i += sizeof(blockhead);
                if(blockhead.id == 'atad') *wave_size += blockhead.size;
                //pack_fseek(hfile, blockhead.size, SEEK_CUR);
                offset += blockhead.size;

                i += blockhead.size;
            }
            //pack_fseek(hfile,point,SEEK_SET);
            *buffer_offset = point;

            return TTN_OK;
        }
    }
}


EXPORT  SLONG   FNBACK  get_buffer_wav_data(USTR *buffer, SLONG *buffer_index, USTR *data, SLONG blocksize)
{
    SLONG   i, j, offset, index;
    struct 
    {
        DWORD id;
        DWORD size;
    } blockhead;
    
    offset = *buffer_index;
    index = 0;
    i = j = 0;
    while(1)
    {
        if(i >= blocksize) break;
        //if((pack_fread(&blockhead,1, sizeof(blockhead), hfile)) != sizeof(blockhead))
          //  return TTN_ERROR;
        memcpy(&blockhead, &buffer[offset], sizeof(blockhead));
        offset += sizeof(blockhead);

        i += sizeof(blockhead);
        if(blockhead.id != 'atad')
        {
            //pack_fseek(hfile, blockhead.size, SEEK_CUR);
            offset += blockhead.size;

            i += blockhead.size;
            continue;
        }
        //if((pack_fread(data += j,blockhead.size, hfile)) != blockhead.size)
          //  return TTN_ERROR;
        memcpy(&data[index], &buffer[offset], blockhead.size);
        index += j;
        offset += blockhead.size;

        j += blockhead.size;
        i += blockhead.size;
    }
    return TTN_OK;
}



EXPORT  SAMPLE* FNBACK  load_wav_from_file(USTR *filename)
{
    WAVEFORMATEX wf;
    PACK_FILE *hfile = NULL;
    SLONG   block_size,buffer_bytes;
    SAMPLE  * sample = NULL;
    
    if( NULL == (hfile = pack_fopen((const char *)filename, "rb")) )
    {
        log_error(1, "open file %s error", filename);
        goto _some_error;
    }
    if(TTN_OK != get_wav_format(hfile, &wf, &block_size, &buffer_bytes))
    {
        log_error(1, "get wav %s format error", filename);
        goto _some_error;
    }
    if(NULL == (sample = (SAMPLE *)malloc(sizeof(SAMPLE) + buffer_bytes - sizeof(USTR))))
    {
        log_error(1, "sample memory alloc error");
        goto _some_error;
    }

    memcpy(&sample->wf, &wf, sizeof(WAVEFORMATEX));
    sample->buffer_bytes = buffer_bytes;
    sample->number = -1;

    if(TTN_OK != get_wav_data(hfile, sample->buffer, block_size))
    {
        log_error(1, "get wav %s data error", filename);
        goto _some_error;
    }
    pack_fclose(hfile);
    return sample;

_some_error:
    if(hfile) { pack_fclose(hfile); hfile = NULL; }
    if(sample) { free(sample); sample = NULL; }
    return NULL;
}


EXPORT  SAMPLE* FNBACK  load_wav_from_buffer(USTR *buffer, SLONG buffer_size)
{
    WAVEFORMATEX wf;
    SLONG   block_size,buffer_bytes;
    SLONG   buffer_offset;
    SAMPLE  * sample = NULL;
    
    buffer_offset = 0;
    if(TTN_OK != get_buffer_wav_format(buffer, &buffer_offset, &wf, &block_size, &buffer_bytes))
    {
        log_error(1, "get buffer wav format error");
        goto _some_error;
    }
    if(NULL == (sample = (SAMPLE *)malloc(sizeof(SAMPLE) + buffer_bytes - sizeof(USTR))))
    {
        log_error(1, "sample memory alloc error");
        goto _some_error;
    }

    memcpy(&sample->wf, &wf, sizeof(WAVEFORMATEX));
    sample->buffer_bytes = buffer_bytes;
    sample->number = -1;

    if(TTN_OK != get_buffer_wav_data(buffer, &buffer_offset, sample->buffer, block_size))
    {
        log_error(1, "get buffer wav data error");
        goto _some_error;
    }
    return sample;

_some_error:
    if(sample) { free(sample); sample = NULL; }
    return NULL;
}


EXPORT  SLONG   FNBACK  get_wavfile_information2(USTR *filename, ULONG *channels, ULONG *sample_rate, 
                                                ULONG *bps, ULONG *size, ULONG *play_time)
{
    WAVEFORMATEX wf;
    PACK_FILE *hfile = NULL;
    SLONG   block_size, buffer_bytes;
    
    if( NULL == (hfile = pack_fopen((const char *)filename, "rb")) )
    {
        log_error(1, "open file %s error", filename);
        goto _some_error;
    }
    if(TTN_OK != get_wav_format(hfile, &wf, &block_size, &buffer_bytes))
    {
        log_error(1, "get wav %s format error", filename);
        goto _some_error;
    }
    pack_fclose(hfile);

    if(channels) *channels = wf.nChannels;
    if(sample_rate) *sample_rate = wf.nSamplesPerSec;
    if(bps) *bps = wf.wBitsPerSample;
    if(size) *size = buffer_bytes;
    if(play_time) *play_time = buffer_bytes * 1000 / wf.nAvgBytesPerSec;

    return TTN_OK;

_some_error:
    if(hfile) { pack_fclose(hfile); hfile = NULL; }
    return TTN_NOT_OK;
}

