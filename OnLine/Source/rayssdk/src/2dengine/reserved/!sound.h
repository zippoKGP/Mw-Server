/*
**  sound.h
**  sound functions.
**
**  Jack, 2002.4.6.
*/
#ifndef _SOUND_H_
#define _SOUND_H_


// DEFINES ///////////////////////////////////////////////////////////////////////////////////////////////////


// STRUCTURES ////////////////////////////////////////////////////////////////////////////////////////////////
#pragma pack(push)
#pragma pack(1)
typedef struct  tagSAMPLE
{
    WAVEFORMATEX    wf;
    SLONG   buffer_bytes;
    SLONG   number;
    USTR    buffer[1];
} SAMPLE, *LPSAMPLE;
#pragma pack(pop)






// FUNCTION PROTOTYPES ////////////////////////////////////////////////////////////////////////////////////////
EXPORT  SLONG   FNBACK  get_wav_format(PACK_FILE *hfile, WAVEFORMATEX *wf, SLONG *block_size, SLONG *wave_size);
EXPORT  SLONG   FNBACK  get_wav_data(PACK_FILE *hfile, USTR *data, SLONG blocksize);
EXPORT  SLONG   FNBACK  get_buffer_wav_format(USTR *buffer, SLONG *buffer_offset, WAVEFORMATEX *wf, SLONG *block_size, SLONG *wave_size);
EXPORT  SLONG   FNBACK  get_buffer_wav_data(USTR *buffer, SLONG *buffer_index, USTR *data, SLONG blocksize);
EXPORT  SAMPLE* FNBACK  load_wav_from_file(USTR *filename);
EXPORT  SAMPLE* FNBACK  load_wav_from_buffer(USTR *buffer, SLONG buffer_size);
EXPORT  SLONG   FNBACK  get_wavfile_information(USTR *filename, ULONG *channels, ULONG *sample_rate, 
                                                ULONG *bps, ULONG *size, ULONG *play_time);
//









#endif//_SOUND_H_
