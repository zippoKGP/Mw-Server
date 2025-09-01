/*
**	mp3file.h
**	mp3file functions header.
**	
**	GNU open source.
**	Jack, 2002.6.4.
**
*/
#ifndef	_MP3FILE_H_
#define	_MP3FILE_H_


void *  mp3_open(char *filename);
int     mp3_read(void *vfp, void *sampbuf, int len);
void    mp3_close(void *vfp);
void *  mp3_waveformatex(void *vfp);
int     mp3_to_wave(char *mp3_filename, char *wave_filename);


#endif//_MP3FILE_H_
