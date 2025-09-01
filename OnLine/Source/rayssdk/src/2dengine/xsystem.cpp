/*
**      SYSTEM.CPP
**      system functions.
**
**      ZJian, 2000.7.10.
**          Created.
**
*/
#include "rays.h"
#include "winmain.h"
#include "xkiss.h"
#include "xsystem.h"
#include "xcdrom.h"
#include "vbmp.h"
#include "mmx.h"

//
// defines------------------------------------------------------------------------------------------------------------
//
#define SYSTEM_INIT         0x01
#define SYSTEM_ACTIVE       0x02

//
// globals------------------------------------------------------------------------------------------------------------
//
USTR    print_rec[2048];
USTR    game_path[_MAX_PATH];
USTR    game_filename[_MAX_PATH+_MAX_FNAME];
ULONG   game_capture_no;
ULONG   game_now_time;
ULONG   game_start_time;

ULONG   mp3_loop_flag = 0;

//
// locals-------------------------------------------------------------------------------------------------------------
//
static  CRITICAL_SECTION    g_csLogError;
static  DWORD               dwSystemFlag = 0;

static  CRITICAL_SECTION    g_csSystemRand;
static  int                 g_nSystemRandSeed = 11;  //must be an odd number

//
// functions----------------------------------------------------------------------------------------------------------
//
EXPORT  int     FNBACK  init_system(void)
{
    //(1) initialize system datas
    dwSystemFlag = 0;
    InitializeCriticalSection( &g_csLogError );
    store_game_path((USTR*)game_path);
    game_capture_no = 0;
    game_start_time = timeGetTime();
    game_now_time = game_start_time;

    InitializeCriticalSection( &g_csSystemRand );
    g_nSystemRandSeed = 11;  //must be an odd number

	//(1a) initialize mmx datas
	init_mmx();

    remove(ERRORINFO_FILENAME);
    dwSystemFlag |= SYSTEM_INIT;

    //(2) analyst system configurations
    sprintf((char *)print_rec,"SYSTEM module initialize starting...");
    log_error(1,print_rec);
    analyst_system();

    sprintf((char *)print_rec,"SYSTEM module initialized OKay.");
    log_error(1,print_rec);
    sprintf((char *)print_rec,"\n");
    log_error(1,print_rec);
    return 0;
}


EXPORT  void    FNBACK  active_system(int active)
{
    if(active)      
        dwSystemFlag |= SYSTEM_ACTIVE;
    else 
        dwSystemFlag &= ~SYSTEM_ACTIVE;

	active_mmx(active);

    return;
}


EXPORT  void    FNBACK  free_system(void)
{
    if(! (dwSystemFlag & SYSTEM_INIT) )
        return;

    DeleteCriticalSection( &g_csLogError );
    DeleteCriticalSection( &g_csSystemRand );

	free_mmx();

    dwSystemFlag = 0;

    sprintf((char *)print_rec,"SYSTEM module free OKay.");
    log_error(1,print_rec);
}



EXPORT void FNBACK log_error(int p, USTR *strMsg )
{
    char filename[_MAX_PATH+_MAX_FNAME];
    FILE *logfile=NULL;

    if(! (dwSystemFlag & SYSTEM_INIT) )
        return;

    EnterCriticalSection( &g_csLogError );
    if (!p) 
        goto ok;
    sprintf((char*)filename,"%s\\%s",game_path,ERRORINFO_FILENAME);
    if(NULL==(logfile=fopen((const char *)filename,"a+")))
    {
        sprintf(filename,"C:\\%s",ERRORINFO_FILENAME);
        logfile=fopen((const char *)filename,"a+");
        if (NULL==logfile)
            goto error;
    }
    fputs((const char *)strMsg, logfile);
    fputs("\n",logfile);
    if(logfile) fclose(logfile);

error:
#ifdef  DEBUG
    OutputDebugString((LPCTSTR)strMsg);
    OutputDebugString((LPCTSTR)"\n");
#endif//DEBUG
ok:
    LeaveCriticalSection( &g_csLogError );
}



EXPORT void FNBACK log_error(int p, char *strMsg, ... )
{
    char filename[_MAX_PATH+_MAX_FNAME];
    char strBuffer[2048];
    FILE *logfile=NULL;
    va_list args;

    if(! (dwSystemFlag & SYSTEM_INIT) )
        return;

    EnterCriticalSection( &g_csLogError );
    if (!p)
        goto ok;

    va_start(args, strMsg);
    _vsnprintf( strBuffer, 2048, strMsg, args );
    va_end(args);

    sprintf((char*)filename,"%s\\%s",game_path,ERRORINFO_FILENAME);
    if(NULL==(logfile=fopen((const char *)filename,"a+")))
    {
        sprintf(filename,"C:\\%s",ERRORINFO_FILENAME);
        logfile=fopen((const char *)filename,"a+");
        if (NULL==logfile)
            goto error;
    }
    fputs((const char *)strBuffer, logfile);
    fputs("\n",logfile);
    if(logfile) fclose(logfile);

error:
#ifdef  DEBUG
    OutputDebugString((LPCTSTR)strBuffer);
    OutputDebugString((LPCTSTR)"\n");
#endif//DEBUG

ok:
    LeaveCriticalSection( &g_csLogError );
}



EXPORT  void    FNBACK  system_rand_init(void)
{
    time_t  tnow;
    USTR    disp_rec[80];
    USTR    temp[4];

    EnterCriticalSection( &g_csSystemRand );
    {
        tnow = time(NULL);
        strcpy((char *)disp_rec,ctime(&tnow));
        temp[0] = disp_rec[17];
        temp[1] = disp_rec[18];
        temp[2] = '\0';
        
        g_nSystemRandSeed = 2 * atoi((const char *)temp) + 1;
    }
    LeaveCriticalSection( &g_csSystemRand );
}


EXPORT  int     FNBACK  system_rand(void)
{
    int min_data = 0;
    int max_data = RAND_MAX;
    int k, len, m, i;
    int p;

    if(! (dwSystemFlag & SYSTEM_INIT) )
        return 0;
    
    EnterCriticalSection( &g_csSystemRand );
    {
        k = max_data - min_data + 1;
        len = 2;
        while (len < k) len = len + len;
        m = 4 * len; 
        k = g_nSystemRandSeed;
        i = 1;
        while (i <= 1)
        { 
            k = k + k + k + k + k;
            k = k % m; 
            len = k / 4 + min_data;
            if (len <= max_data) 
            { 
                p = len;
                i = i + 1;
            }
        }
        
        g_nSystemRandSeed = k;
    }
    LeaveCriticalSection( &g_csSystemRand );
    
    return(p);
}



EXPORT  void    FNBACK  idle_loop(void)
{
    MSG     msg;
    
    while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    game_now_time = timeGetTime();
    //loop_cdrom_music();
}


EXPORT  void    FNBACK  app_idle_loop(void)
{
    MSG     msg;
    
    while(PeekMessage(&msg, g_hDDWnd, 0, 0, PM_REMOVE))
    {
        switch(msg.message)
        {
        case WM_ACTIVATE:
            //mouse client message
        case WM_SETCURSOR:
        case WM_MOUSEMOVE:
        case WM_LBUTTONUP:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONDBLCLK:
        case WM_MOUSEWHEEL:
//        case WM_XBUTTONDBLCLK:
//        case WM_XBUTTONDOWN:
//        case WM_XBUTTONUP:
            //mouse non-client message
        case WM_NCHITTEST:
            break;
        case WM_NCLBUTTONDBLCLK:
            break;
        case WM_NCLBUTTONDOWN:
            break;
        case WM_NCLBUTTONUP:
            break;
        case WM_NCMBUTTONDBLCLK:
        case WM_NCMBUTTONDOWN:
        case WM_NCMBUTTONUP:
        //case WM_NCMOUSEHOVER:
        //case WM_NCMOUSELEAVE:
        case WM_NCMOUSEMOVE:
        case WM_NCRBUTTONDBLCLK:
        case WM_NCRBUTTONDOWN:
        case WM_NCRBUTTONUP:
//        case WM_NCXBUTTONDBLCLK:
  //      case WM_NCXBUTTONDOWN:
    //    case WM_NCXBUTTONUP:
            //keyboard message
        case WM_KEYDOWN:
        case WM_KEYUP:
            // app message
        case WM_PAINT:
        case WM_CREATE:
        case WM_CLOSE:
        case WM_DESTROY:
        case WM_ACTIVATEAPP:
        case WM_SYSCOMMAND:
        //default:
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            break;
        }
    }
    game_now_time = timeGetTime();
    //loop_cdrom_music();
}


EXPORT  SLONG   is_gb_windows(void)
{
    static USTR  shell_str[MAX_PATH];
    HKEY  hReadKey;
    ULONG slRet;
    ULONG dwlength;
    ULONG dwDataType;

    slRet=RegOpenKeyEx(HKEY_CURRENT_USER,"Control Panel\\International",0,KEY_READ,&hReadKey);
    if(slRet==ERROR_SUCCESS)
    {
        dwlength=sizeof(shell_str);
        memset(shell_str,0,dwlength);
        dwlength=MAX_PATH;
        RegQueryValueEx(hReadKey,"locale",NULL,&dwDataType,shell_str,&dwlength);
        RegCloseKey(hReadKey);
        if(strcmp((char *)shell_str,(char*)"00000804")==0)
        {
            return TRUE;
        }
    }
    return FALSE;
}




EXPORT  SLONG   FNBACK  get_cdrom_drive(void)
{
    UCHR    i;
    SLONG   drive;
    USTR    str[5];
    
    drive=-1;
    for(i='A';i<='Z';i++)
    {
        memset((char *)str,0,5);
        sprintf((char *)str,"%c:\\",i);
        if(GetDriveType((const char *)str)==DRIVE_CDROM)
        {
            drive=(SLONG)(i-'A');
            break;
        }
    }
    return (drive);
}

EXPORT  SLONG   FNBACK  check_cdrom_volume(USTR *title)
{
    UCHR    i;
    SLONG   drive;
    USTR    str[5];
    USTR    volume_name[256];
    ULONG   serial_no;
    ULONG   max_filename_length;
    ULONG   file_system_flags;
    USTR    file_system_name[256];
    
    drive=-1;
    for(i='A';i<='Z';i++)
    {
        memset((char *)str,0,5);
        sprintf((char *)str,"%c:\\",i);
        if(GetDriveType((const char *)str)==DRIVE_CDROM)
        {
            GetVolumeInformation((const char *)str,(char *)volume_name,255,
                &serial_no,&max_filename_length,&file_system_flags,
                (char *)file_system_name,255);
            if(strcmpi((const char *)volume_name,(const char *)title)==0)
            {
                drive=(SLONG)(i-'A');
                break;
            }
        }
    }
    return (drive);
}

EXPORT  USTR *  FNBACK  get_cdrom_volume(SLONG drive)
{
    USTR    str[5];
    ULONG   serial_no;
    ULONG   max_filename_length;
    ULONG   file_system_flags;
    USTR    file_system_name[256];
    static  USTR    volume_name[256];
    
    memset(volume_name,0x00,256);
    if(drive<0)
        return((USTR *)volume_name);
    memset((char *)str,0,5);
    sprintf((char *)str,"%c:\\",drive+'A');
    if(GetDriveType((const char *)str)!=DRIVE_CDROM)
        return((USTR *)volume_name);
    GetVolumeInformation((const char *)str,(char *)volume_name,255,
        &serial_no,&max_filename_length,&file_system_flags,
        (char *)file_system_name,255);
    return((USTR *)volume_name);
}


EXPORT  void    FNBACK  capture_screen(void)
{
    USTR filename[_MAX_FNAME];
    sprintf((char *)filename,"%s\\CAP%05d.BMP",game_path,game_capture_no);
    BMP_save_file(screen_buffer,(char *)filename);
    game_capture_no++;
}



EXPORT  void    FNBACK  store_game_path(USTR *path)
{
    DWORD   dwResult;
    CHAR    szFileName[_MAX_FNAME];
    int     drive;
    UCHR    drive_char;

    // Jack, changed. [6/19/2002]
    dwResult = GetModuleFileName(
        NULL,                   // NULL for self module
        (LPTSTR) szFileName,    // file name of module
        _MAX_FNAME              // size of filename length
        );
    if(dwResult > 0)
    {
        strcpy((char *)path, (const char *)get_file_path((USTR *)szFileName));
        drive_char = char_upper(szFileName[0]);
        drive = drive_char - 'A' + 1;
        _chdrive(drive);
        _chdir((const char *)path);
    }
    else
    {
        strcpy((char *)path,"");
        _getdcwd(_getdrive(),(char *)path,_MAX_PATH);
    }
    
}

EXPORT  ULONG   FNBACK  get_fps(void)
{
    static ULONG start_time=0;
    static ULONG fps=0;
    static ULONG count=0;
    static SLONG init_flag=0;
    ULONG time;

    count++;
    if(0==init_flag)
    {
        start_time=timeGetTime();
        init_flag=1;
    }
    else
    {
        time=timeGetTime();
        if(time-start_time>1000)
        {
            fps=count*1000/(time-start_time);
            init_flag=0;
            count=0;
        }
    }
    return(fps);
}

EXPORT  USTR *  FNBACK  get_computer_name(void)
{
    static  USTR    buffer[MAX_COMPUTERNAME_LENGTH+1];
    ULONG   length=MAX_COMPUTERNAME_LENGTH+1;
    GetComputerName((char *)buffer,&length);
    return((USTR *)buffer);
}

EXPORT  USTR *  FNBACK  get_user_name(void)
{
    static  USTR    buffer[UNLEN+1];
    ULONG   length=UNLEN+1;
    GetUserName((char *)buffer,&length);
    return((USTR *)buffer);
}

EXPORT  USTR *  FNBACK  get_windows_directory(void)
{
    static  USTR    buffer[MAX_PATH+1];
    GetWindowsDirectory((char *)buffer,MAX_PATH+1);
    return((USTR *)buffer);
}

EXPORT  SLONG   FNBACK  get_windows_version(void)
{
    DWORD dwVersion;
    DWORD dwWindowsMajorVersion;
    DWORD dwWindowsMinorVersion;
    DWORD dwBuild;
    SLONG ret;

    dwVersion = GetVersion();
    // Get major and minor version numbers of Windows
    dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
    dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
    // Get build numbers for Windows NT or Win32s
    if (dwVersion < 0x80000000)                // Windows NT
    {
        dwBuild = (DWORD)(HIWORD(dwVersion));
        ret = WINDOWS_NT;
    }
    else if (dwWindowsMajorVersion < 4)        // Win32s
    {
        dwBuild = (DWORD)(HIWORD(dwVersion) & ~0x8000);
        ret = WINDOWS_32;
    }
    else         // Windows 95 -- No build numbers provided
    {
        dwBuild =  0;
        ret = WINDOWS_95;
    }
    return(ret);
}





EXPORT  void    FNBACK  get_memory_status(ULONG *total_phys,ULONG *avail_phys)
{
    MEMORYSTATUS ms;
    GlobalMemoryStatus(&ms);
    *total_phys = ms.dwTotalPhys;
    *avail_phys = ms.dwAvailPhys;
}



EXPORT  ULONG   FNBACK  get_disk_serial_no(void)
{
    LPCTSTR lpRootPathName="c:\\";
    CHAR    VolumeNameBuffer[12];
    DWORD   nVolumeNameSize=12;
    DWORD   VolumeSerialNumber;
    DWORD   MaximumComponentLength;
    CHAR    FileSystemNameBuffer[10];
    DWORD   nFileSystemNameSize=10;
    DWORD   FileSystemFlags;
    
    GetVolumeInformation(
        (LPCTSTR)lpRootPathName,
        (LPTSTR)&VolumeNameBuffer, 
        (DWORD)nVolumeNameSize,
        (LPDWORD)&VolumeSerialNumber,
        (LPDWORD)&MaximumComponentLength,
        (LPDWORD)&FileSystemFlags,
        (LPTSTR)&FileSystemNameBuffer,
        (DWORD)nFileSystemNameSize);
    
    //VolumeSerialNumber ^= 0x19750411;// encrypt
    return  VolumeSerialNumber;
}



EXPORT  ULONG   FNBACK  get_cpu_clock(void)
{
    static __int64 clock = 0, temp;
    
    if( clock != 0 )
        return (ULONG)clock;
    _asm
    {
        rdtsc                   // get cpu clock count from power on, stored at (eax n edx)
        lea ebx, clock          // get the store address of clock 
        mov [ ebx + 0 ], eax    // save the clock count into clock(__int64 !!)
        mov [ ebx + 4 ], edx    //
    }
    Sleep( 1000 );              // sleep for 1 second
    _asm
    {
        rdtsc                   // get cpu clock count just like the upper 
        lea ebx, temp           // but save to temp
        mov [ ebx + 0 ], eax    //
        mov [ ebx + 4 ], edx    //
    }
    clock  = temp - clock;
    clock /= 1000000;           // convert it to M(10^6)
    return (ULONG)clock;
}



EXPORT  char *  FNBACK  get_cpu_id(void)
{
    static char id[13] = {0};

    if(id[0] == 0)
    {
        _asm
        {
            push  ebp
            lea   ebp,id[0]

            mov   eax,0             // set eax to 0 for get cpuid 
            cpuid                   // cpuid will store at (ebx n edx n ecx)

            mov   [ebp + 0], ebx    // save cpuid into id
            mov   [ebp + 4], edx    //
            mov   [ebp + 8], ecx    //
            pop   ebp
        }
        id[12] = '\0';
    }
    return (char *)id;
}


EXPORT  void    FNBACK  analyst_system(void)
{
    //check cpu type (vender)
    sprintf((char *)print_rec,"           Vender : %s",get_cpu_id());
    log_error(1,print_rec);

    //check cpu clock & is mmx
	if(is_mmx)
		sprintf((char *)print_rec,"            Speed : Approx. %d MHz(MMX)",get_cpu_clock());
	else
		sprintf((char *)print_rec,"            Speed : Approx. %d MHz(!MMX)",get_cpu_clock());
    log_error(1,print_rec);

    //check physical memory status
    ULONG total_phys, avail_phys;
    get_memory_status(&total_phys, &avail_phys);
    sprintf((char *)print_rec,"           Memory : %d M",total_phys/1024/1024);
    log_error(1,print_rec);

    //check windows version
    SLONG win_ver;
    win_ver = get_windows_version();
    if(win_ver == WINDOWS_NT)
        sprintf((char *)print_rec," Operation System : WinNT");
    else if(win_ver == WINDOWS_32)
        sprintf((char *)print_rec," Operation System : Win32");
    else if(win_ver == WINDOWS_95)
        sprintf((char *)print_rec," Operation System : Win95/Win98");
    else
        sprintf((char *)print_rec," Operation System : Unknown");
    log_error(1,print_rec);

    //check windows gb/big5
    SLONG is_gb;
    is_gb = is_gb_windows();
    if(is_gb)
        sprintf((char *)print_rec,"           Locale : Simplified");
    else
        sprintf((char *)print_rec,"           Locale : Traditional");
    log_error(1,print_rec);


    //check game working path
    sprintf((char *)print_rec,"     Working Path : %s",game_path);
    log_error(1,print_rec);
}


//END====================================================================================
