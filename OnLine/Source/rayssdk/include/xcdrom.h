/*
**      XCDROM.H
**      CDROM functions header.
*/
#ifndef _XCDROM_H_INCLUDE_
#define _XCDROM_H_INCLUDE_     1


EXPORT  int     FNBACK init_cdrom_music(void);
EXPORT  void    FNBACK free_cdrom_music(void);
EXPORT  void    FNBACK active_cdrom_music(int active);
EXPORT  void    FNBACK pause_cdrom_music(void);
EXPORT  void    FNBACK resume_cdrom_music(void);
EXPORT  void    FNBACK stop_cdrom_music(void);
EXPORT  void    FNBACK play_cdrom_music(SLONG track);
EXPORT  SLONG   FNBACK status_cdrom_music(void);
EXPORT  void    FNBACK loop_cdrom_music(void);
EXPORT  SLONG   FNBACK total_cdrom_music(void);


#endif//_XCDROM_H_INCLUDE_


