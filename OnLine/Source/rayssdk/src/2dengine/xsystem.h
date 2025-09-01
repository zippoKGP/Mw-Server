/*
**    XSYSTEM.H
**    system functions header.
**    ZJian,2000/7/10.
*/
#ifndef _XSYSTEM_H_INCLUDE_
#define _XSYSTEM_H_INCLUDE_      1

#define ERRORINFO_FILENAME  "error.txt"

typedef enum    WINDOWS_TYPE_ENUMS
{ WINDOWS_NT        =   3,
WINDOWS_32          =   2,
WINDOWS_95          =   1,
} WINDOWS_TYPE;


#define TRACE(s)    { sprintf((char *)print_rec,"TRACE:%s",#s); log_error(1,print_rec); }



//==================================================================================
extern  USTR    print_rec[2048];
extern  USTR    game_path[_MAX_PATH];
extern  USTR    game_filename[_MAX_PATH+_MAX_FNAME];
extern  ULONG   game_capture_no;
extern  ULONG   game_now_time;
extern  ULONG   game_start_time;
extern  ULONG	mp3_loop_flag;

//==================================================================================
EXPORT  int     FNBACK  init_system(void);
EXPORT  void    FNBACK  active_system(int active);
EXPORT  void    FNBACK  free_system(void);
//
EXPORT  void    FNBACK  log_error(int p, USTR *strMsg );
EXPORT  void    FNBACK  log_error(int p, char *strMsg, ...);
EXPORT  void    FNBACK  system_rand_init(void);
EXPORT  int     FNBACK  system_rand(void);
//
EXPORT  void    FNBACK  idle_loop(void);
EXPORT  SLONG   FNBACK  is_gb_windows(void);
EXPORT  SLONG   FNBACK  get_cdrom_drive(void);
EXPORT  SLONG   FNBACK  check_cdrom_volume(USTR *title);
EXPORT  USTR *  FNBACK  get_cdrom_volume(SLONG drive);
EXPORT  void    FNBACK  store_game_path(USTR *path);
EXPORT  void    FNBACK  capture_screen(void);
//
EXPORT  ULONG   FNBACK  get_fps(void);
EXPORT  USTR *  FNBACK  get_computer_name(void);
EXPORT  USTR *  FNBACK  get_user_name(void);
EXPORT  USTR *  FNBACK  get_windows_directory(void);
EXPORT  SLONG   FNBACK  get_windows_version(void);
EXPORT  void    FNBACK  get_memory_status(ULONG *total_phys,ULONG *avail_phys);
EXPORT  ULONG   FNBACK  get_disk_serial_no(void);
EXPORT  ULONG   FNBACK  get_cpu_clock(void);
EXPORT  char *  FNBACK  get_cpu_id(void);
EXPORT  void    FNBACK  analyst_system(void);
//


#endif//_XSYSTEM_H_INCLUDE_

