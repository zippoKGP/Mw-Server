/*
**	mp3.h
**	mp3 decode functions header.
**	
**	GNU open source.
**	ZJian neatened, 2001.5.29.
**
*/
#ifndef	_MP3_H_INCLUDE_
#define	_MP3_H_INCLUDE_


EXPORT	int		FNBACK  init_mp3(void);
EXPORT	void	FNBACK  active_mp3(int active);
EXPORT	void	FNBACK  free_mp3(void);


EXPORT  void    FNBACK  play_mp3(SLONG loop, USTR *filename);
EXPORT  void    FNBACK  stop_mp3(void);
EXPORT  void    FNBACK  set_mp3_volume(SLONG volume);
EXPORT  void    FNBACK  set_mp3_pan(SLONG pan);
EXPORT	SLONG	FNBACK	is_mp3_playing(void);


#endif//_MP3_H_INCLUDE_
