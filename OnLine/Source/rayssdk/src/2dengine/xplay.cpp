/*
**      XPLAY.CPP
**      Net support functions.
**
**      ZJian,2000.7.8.
*/
#include "rays.h"
#include "xplay.h"
#include "xsystem.h"
#include "winmain.h"

#define IID_IDirectPlayNA           IID_IDirectPlay4A
#define IID_IDirectPlayLobbyNA      IID_IDirectPlayLobby3A
#define LPDIRECTPLAYNA              LPDIRECTPLAY4A
#define LPDIRECTPLAYLOBBYNA         LPDIRECTPLAYLOBBY3A

// something interface ============================
// {38314186-D20D-4458-A231-7EDF83A124C6}
static  GUID    NETGAME = 
{ 0x38314186, 0xd20d, 0x4458, { 0xa2, 0x31, 0x7e, 0xdf, 0x83, 0xa1, 0x24, 0xc6 } };


static  LPDIRECTPLAYNA          lpdp=NULL;
static  LPDIRECTPLAYLOBBYNA     lpdpl=NULL;
static  HANDLE  PlayerEvent,ExitEvent,Thread=NULL;
static  DWORD   ThreadID;
static  DPCAPS  caps;
static  int     NetFlag  = 0 ;
static  HRESULT NetErr = 0;
static  char *  NetErrChar = NULL;

// something connections ==========================
typedef struct  DPCINFO_STRUCT  DPCINFO,*LPDPCINFO;
struct  DPCINFO_STRUCT
{
    LPVOID  connect;
    char    *name;
    GUID    guid;
};
static  int     DPCCount=0;
static  DPCINFO DPCInfo[MAX_XPLAY_CONNECTS];
static  DPCNAME DPCName[MAX_XPLAY_CONNECTS];
static  int     DConnect;
static  LPVOID  serial = NULL ;
static  LPVOID  tcp_ip = NULL ;
static  LPVOID  modem  = NULL ;

// something sessions =============================
typedef struct  DPSINFO_STRUCT  DPSINFO,*LPDPSINFO;
struct  DPSINFO_STRUCT
{
    DPSESSIONDESC2  dpsd;
    char           *name;
    char           *password;
};
static  int     DPSCount=0;
static  DPSINFO DPSInfo[MAX_XPLAY_SESSIONS];
static  char   *DPSName[MAX_XPLAY_SESSIONS];

// something players ===============================
static  DPID        Player;
static  int         MaxPlayers;
static  ENUMPLAYER  *enumcb = NULL;
static  FUNCDOMSG   *pfnDoSystemMsg = NULL;
static  FUNCDOMSG   *pfnDoAppMsg = NULL;

// send & receive messages =========================
static  LPVOID      lpReceiveBuffer=NULL;
static  DWORD       dwReceiveBufferSize = 0;

// static functions prototypes =====================
static  BOOL CALLBACK   DPECCallBack(LPCGUID guid,LPVOID lpconnect,DWORD size,LPCDPNAME name,DWORD flags,LPVOID p);
static  BOOL CALLBACK   DPESCallBack(LPCDPSESSIONDESC2 lpdesc,LPDWORD timeout,DWORD flags,LPVOID p);
static  BOOL CALLBACK   DPEPCallBack(DPID id,DWORD Type,LPCDPNAME lpname,DWORD Flags,LPVOID p);
static  DWORD   WINAPI  ReceiveThread(LPVOID lpParameter);
static  DWORD   WINAPI  ReceiveMessages(LPVOID lpParameter);
static  void            DestroySessionList(void);
static  char *          GetDirectPlayErrStr(HRESULT hr);

// functions ======================================================================================
EXPORT  int     FNBACK  init_net(void)
{
    CoInitialize(NULL);
    PlayerEvent = CreateEvent( NULL,FALSE,FALSE,NULL);
    ExitEvent = CreateEvent( NULL,FALSE,FALSE,NULL);
    Thread = CreateThread( NULL,0,ReceiveThread,NULL,0,&ThreadID);
    if(NULL==Thread)
        return 0;
    NetFlag = NETDP_INIT; // so we init net successful
    return 0;
}

EXPORT  void    FNBACK  free_net(void)
{
    if(Thread)
    { 
        SetEvent(ExitEvent); 
        Sleep(200); 
    }
    CloseHandle(PlayerEvent);
    CloseHandle(ExitEvent);
    if(lpdp)
    {
        IDirectPlayX_Release(lpdp);
        lpdp=NULL;
    }
    if(lpdpl)
    {
        IDirectPlayLobby_Release(lpdpl);
        lpdpl=NULL;
    }
    CoUninitialize();
}


EXPORT  void    FNBACK  active_net(int active)
{
    if(active)
    {
    }
}


EXPORT  void    FNBACK  set_net_func( FUNCDOMSG *do_sys, FUNCDOMSG *do_app)
{
    pfnDoSystemMsg = do_sys;
    pfnDoAppMsg = do_app;
}


EXPORT  int     FNBACK  NET_get_connects(int ID, DPCNAME **lpname, int *count)
{
    if( !(NetFlag & NETDP_INIT) ) 
        return NETERR_INIT;
    if( NetFlag & NETDP_ENUMCONNECT) 
        return NET_DOK;

    NETGAME.Data1 += ID;
    NetErr = CoCreateInstance(CLSID_DirectPlay,NULL,CLSCTX_INPROC_SERVER,IID_IDirectPlayNA,(LPVOID *)&lpdp);
    if(FAILED(NetErr)) 
        return NETERR_DNODXX;

    NetErr = CoCreateInstance(CLSID_DirectPlayLobby,NULL,CLSCTX_INPROC_SERVER,IID_IDirectPlayLobbyNA,(LPVOID *)&lpdpl);
    if(FAILED(NetErr)) 
        return NETERR_DNODXX;

    NetErr = IDirectPlayX_EnumConnections(lpdp,NULL,DPECCallBack,g_hDDWnd,0);
    if(FAILED(NetErr)) 
        return NETERR_DENUMERR;

    *lpname = DPCName;
    *count = DPCCount;
    NetFlag |= NETDP_ENUMCONNECT;
    return NET_DOK;
}



static  BOOL CALLBACK   DPECCallBack(LPCGUID guid,LPVOID lpconnect,DWORD size,LPCDPNAME name,DWORD flags,LPVOID p)
{
    LPDIRECTPLAYNA  temp;
    
    if(DPCCount>MAX_XPLAY_CONNECTS) 
        return FALSE;

    if(FAILED(CoCreateInstance(CLSID_DirectPlay,NULL,CLSCTX_INPROC_SERVER,IID_IDirectPlayNA,(LPVOID *)&temp)))
        return FALSE;

    if(FAILED(IDirectPlayX_InitializeConnection(temp,lpconnect,0)))
    {
        IDirectPlayX_Release(temp);
        return TRUE;
    }

    IDirectPlayX_Release(temp);
    if((DPCInfo[DPCCount].connect = GlobalAlloc(GPTR, size)) == NULL) 
        return TRUE;
    memcpy(DPCInfo[DPCCount].connect,lpconnect,size);
    DPCInfo[DPCCount].guid=*guid;
    if( IsEqualGUID(*guid,DPSPGUID_TCPIP) )
        DPCName[DPCCount].type = NETCONNECTTCPIP;
    else if(IsEqualGUID(*guid,DPSPGUID_SERIAL))
        DPCName[DPCCount].type = NETCONNECTSERIAL;
    else if(IsEqualGUID(*guid,DPSPGUID_MODEM))
        DPCName[DPCCount].type = NETCONNECTMODEM;
    else if(IsEqualGUID(*guid,DPSPGUID_IPX))
        DPCName[DPCCount].type = NETCONNECTIPX;
    else
        DPCName[DPCCount].type = NETCONNECTOTHER;
    
    DPCInfo[DPCCount].name = (char *)GlobalAlloc(GPTR, 1+strlen(name->lpszShortNameA));
    if(DPCInfo[DPCCount].name == NULL)
    {
        GlobalFree(DPCInfo[DPCCount].connect);
        return TRUE;
    }

    strcpy(DPCInfo[DPCCount].name,(const char *)name->lpszShortNameA);
    DPCName[DPCCount].name=DPCInfo[DPCCount].name;
    DPCCount++;
    return TRUE;
}

EXPORT  void    FNBACK  NET_set_com_port(int port,int speed)
{
    DPCOMPORTADDRESS comport={port,CBR_115200,ONESTOPBIT,NOPARITY,DPCPA_RTSFLOW};
    DPCOMPOUNDADDRESSELEMENT dpcp[2];
    DWORD size=0;
    
    if( !(NetFlag & NETDP_ENUMCONNECT) ) 
        return;
    if(serial)
    { 
        GlobalFree(serial); 
        serial = NULL; 
    }
    if(port<=0) 
        return;
    switch(speed)
    {
    case 256000:
        comport.dwBaudRate=CBR_256000;
        break;
    case 128000:
        comport.dwBaudRate=CBR_128000;
        break;
    case 115200:
        break;
    case 57600:
        comport.dwBaudRate=CBR_57600;
        break;
    case 56000:
        comport.dwBaudRate=CBR_56000;
        break;
    case 38400:
        comport.dwBaudRate=CBR_38400;
        break;
    case 19200:
        comport.dwBaudRate=CBR_19200;
        break;
    case 14400:
        comport.dwBaudRate=CBR_14400;
        break;
    }
    dpcp[0].guidDataType = DPAID_ServiceProvider;
    dpcp[0].dwDataSize = sizeof(GUID);
    dpcp[0].lpData = (LPVOID)&DPSPGUID_SERIAL;
    dpcp[1].guidDataType = DPAID_ComPort;
    dpcp[1].dwDataSize = sizeof(comport);
    dpcp[1].lpData = (LPVOID)&comport;
    NetErr = IDirectPlayLobby_CreateCompoundAddress(lpdpl,dpcp,2,NULL,&size);
    if( (serial=GlobalAlloc(GPTR, size)) == NULL ) 
        return;
    NetErr = IDirectPlayLobby_CreateCompoundAddress(lpdpl,dpcp,2,serial,&size);
    if(FAILED(NetErr)) 
    { 
        GlobalFree(serial); 
        serial=NULL; 
        return; 
    }
}

EXPORT  void    FNBACK  NET_set_ip_address(char * address)
{
    DPCOMPOUNDADDRESSELEMENT dpcp[2];
    DWORD  size=0;
    
    if( !(NetFlag & NETDP_ENUMCONNECT) ) 
        return;
    if(tcp_ip)
    { 
        GlobalFree(tcp_ip); 
        tcp_ip=NULL; 
    }
    if(address==NULL) 
        return;
    dpcp[0].guidDataType = DPAID_ServiceProvider;
    dpcp[0].dwDataSize = sizeof(GUID);
    dpcp[0].lpData = (LPVOID)&DPSPGUID_TCPIP;
    dpcp[1].guidDataType = DPAID_INet;
    dpcp[1].dwDataSize = 1+strlen(address);
    dpcp[1].lpData = address;
    NetErr = IDirectPlayLobby_CreateCompoundAddress(lpdpl,dpcp,2,NULL,&size);
    if((tcp_ip=GlobalAlloc(GPTR, size))==NULL) 
        return;
    NetErr=IDirectPlayLobby_CreateCompoundAddress(lpdpl,dpcp,2,tcp_ip,&size);
    if(FAILED(NetErr))
    { 
        GlobalFree(tcp_ip); 
        tcp_ip=NULL; 
        return; 
    }
}

EXPORT  void    FNBACK  NET_set_phone(char * phone)
{
    DPCOMPOUNDADDRESSELEMENT dpcp[2];
    DWORD  size=0;
    
    if(!(NetFlag&NETDP_ENUMCONNECT)) 
        return;
    if(modem){ GlobalFree(modem); modem=NULL; }
    if(phone==NULL) 
        return;
    dpcp[0].guidDataType = DPAID_ServiceProvider;
    dpcp[0].dwDataSize = sizeof(GUID);
    dpcp[0].lpData = (LPVOID)&DPSPGUID_MODEM;
    dpcp[1].guidDataType = DPAID_Phone;
    dpcp[1].dwDataSize = 1+strlen(phone);
    dpcp[1].lpData = phone;
    NetErr=IDirectPlayLobby_CreateCompoundAddress(lpdpl,dpcp,2,NULL,&size);
    if((modem=GlobalAlloc(GPTR, size))==NULL) 
        return;
    NetErr=IDirectPlayLobby_CreateCompoundAddress(lpdpl,dpcp,2,modem,&size);
    if(FAILED(NetErr))
    { 
        GlobalFree(modem); 
        modem=NULL; 
        return; 
    }
}

EXPORT  int     FNBACK  NET_set_connect(int num)
{
    LPVOID  lpData=NULL;
    
    if(!(NetFlag&NETDP_ENUMCONNECT)) 
        return -1;
    if(num<0 || num>=DPCCount) 
        return -1;
    NetFlag &= ~NETDP_CONNECT; 
    DConnect = num;
    if(lpdp)
    {
        IDirectPlayX_Release(lpdp);
        lpdp=NULL;
    }
    if(DPSCount)
    {
        DestroySessionList();
        DPSCount=0;
    }
    NetErr = CoCreateInstance(CLSID_DirectPlay,NULL,CLSCTX_INPROC_SERVER,IID_IDirectPlayNA,(LPVOID *)&lpdp);
    if(FAILED(NetErr)) 
    { 
        NetErrChar=GetDirectPlayErrStr(NetErr); 
        return -2; 
    }
    switch(DPCName[num].type)
    {
    case NETCONNECTTCPIP:
        lpData=tcp_ip;
        break;
    case NETCONNECTSERIAL:
        lpData=serial;
        break;
    case NETCONNECTMODEM:
        lpData=modem;
        break;
    }
    if(!lpData)
        IDirectPlayX_InitializeConnection(lpdp,DPCInfo[num].connect,0);
    else
    {
        NetErr=IDirectPlayX_InitializeConnection(lpdp,lpData,0);
        if(FAILED(NetErr))
        {
            NetErr=IDirectPlayX_InitializeConnection(lpdp,DPCInfo[num].connect,0);
            if(FAILED(NetErr))
                return -3;
        }
    }
    caps.dwSize=sizeof(caps);
    IDirectPlayX_GetCaps(lpdp,&caps,0);
    NetFlag |= NETDP_CONNECT;
    return 0;
}

static  void    DestroySessionList(void)
{
    int     i;
    
    for(i=0;i<DPSCount;i++)
    {
        GlobalFree(DPSInfo[i].name);
        if(DPSInfo[i].password) 
            GlobalFree(DPSInfo[i].password);
    }
    DPSCount=0;
    NetFlag &= ~NETDP_SESSION;
}

EXPORT  int FNBACK  NET_get_sessions(char ***lpname, int *count)
{
    DPSESSIONDESC2  dpsd;
    
    if(!(NetFlag & NETDP_CONNECT)) 
        return -2;
    if(NetFlag & NETDP_JOIN) 
        return -3;
    if(DPSCount) 
        DestroySessionList();
    ZeroMemory(&dpsd,sizeof(dpsd));
    dpsd.dwSize=sizeof(dpsd);
    dpsd.guidApplication = NETGAME;
    // dpsd.guidApplication = GUID_NULL;
    switch(DPCName[DConnect].type)
    {
    case NETCONNECTMODEM:
        if(modem)
            NetErr=IDirectPlayX_EnumSessions(lpdp,&dpsd,0,DPESCallBack,NULL,DPENUMSESSIONS_ASYNC|DPENUMSESSIONS_AVAILABLE|DPENUMSESSIONS_RETURNSTATUS);
        else
            NetErr=IDirectPlayX_EnumSessions(lpdp,&dpsd,0,DPESCallBack,NULL,DPENUMSESSIONS_ASYNC|DPENUMSESSIONS_AVAILABLE);
        break;
    default:
        NetErr=IDirectPlayX_EnumSessions(lpdp,&dpsd,0,DPESCallBack,NULL,DPENUMSESSIONS_ASYNC|DPENUMSESSIONS_AVAILABLE|DPENUMSESSIONS_RETURNSTATUS);
        break;
    }
    if(NetErr==DPERR_CONNECTING) 
        return 1;
    if(FAILED(NetErr)) 
    { 
        NetErrChar=GetDirectPlayErrStr(NetErr); 
        return -1; 
    }
    *lpname = DPSName;
    *count = DPSCount;
    NetFlag |= NETDP_SESSION;
    return 0;
}

static  BOOL CALLBACK   DPESCallBack(LPCDPSESSIONDESC2 lpdesc,LPDWORD timeout,DWORD flags,LPVOID p)
{
    char *  n;
    
    if(flags & DPESC_TIMEDOUT) 
        return FALSE;
    if(DPSCount>MAX_XPLAY_SESSIONS) 
        return FALSE;
    DPSInfo[DPSCount].dpsd = *lpdesc;
    if((n=(char *)GlobalAlloc(GPTR, 1+strlen(lpdesc->lpszSessionNameA)))==NULL) 
        return FALSE;
    strcpy(n,lpdesc->lpszSessionNameA);
    DPSInfo[DPSCount].name=n;
    DPSName[DPSCount]=n;
    if(lpdesc->lpszPasswordA)
    {
        if((n=(char *)GlobalAlloc(GPTR, 1+strlen(lpdesc->lpszPasswordA)))==NULL)
        {
            GlobalFree(DPSInfo[DPSCount].name);
            return FALSE;
        }
        strcpy(n,lpdesc->lpszPasswordA);
        DPSInfo[DPSCount].password = n; 
    }
    else
    {
        DPSInfo[DPSCount].password = NULL; 
    }
    DPSCount++;
    return TRUE;
}

EXPORT  int     FNBACK  NET_join_session(int num,DPID * playid,char * names,char * namel)
{
    DPSESSIONDESC2  dpsd;
    DPNAME  dpname;
    
    if((!(NetFlag&NETDP_SESSION)) || (NetFlag & NETDP_JOIN) ) 
        return -1;
    if( num<0 || num>=DPSCount ) 
        return -2;
    ZeroMemory(&dpsd,sizeof(dpsd));
    dpsd.dwSize = sizeof(dpsd);
    dpsd.guidInstance = DPSInfo[num].dpsd.guidInstance;
    NetErr=IDirectPlayX_Open(lpdp,&dpsd,DPOPEN_JOIN);
    if(FAILED(NetErr))
    { 
        NetErrChar=GetDirectPlayErrStr(NetErr); 
        return -1; 
    }
    ZeroMemory(&dpname,sizeof(dpname));
    dpname.dwSize = sizeof(dpname);
    dpname.lpszShortNameA = names;
    dpname.lpszLongNameA = namel;
    NetErr=IDirectPlayX_CreatePlayer(lpdp,playid,&dpname,PlayerEvent,NULL,0,0);
    Player=*playid;
    NetFlag |= NETDP_JOIN;
    return 0;
}

EXPORT  int     FNBACK  NET_send(NETMSG * msg)
{
    if(!(NetFlag&NETDP_JOIN)) 
        return -1;
    if(caps.dwFlags&DPCAPS_ASYNCSUPPORTED)
        NetErr=IDirectPlayX_SendEx(lpdp,Player,msg->idTo,DPSEND_ASYNC,msg->data,msg->size,0,0,NULL,NULL);
    else 
        NetErr=IDirectPlayX_SendEx(lpdp,Player,msg->idTo,0,msg->data,msg->size,0,0,NULL,NULL);
    if(NetErr==DP_OK || NetErr==DPERR_PENDING) 
        return 0;
    NetErrChar=GetDirectPlayErrStr(NetErr);
    return -2;
}


EXPORT  int     FNBACK  NET_send_data( LPVOID lpBuffer, DWORD dwSize, DPID idFrom, DPID idTo )
{
    if(!(NetFlag & NETDP_JOIN)) 
        return -1;
    if(caps.dwFlags & DPCAPS_ASYNCSUPPORTED)
        NetErr=IDirectPlayX_SendEx(lpdp, Player, idTo, DPSEND_ASYNC, lpBuffer, dwSize, 0, 0, NULL, NULL);
    else 
        NetErr=IDirectPlayX_SendEx(lpdp, Player, idTo, 0, lpBuffer, dwSize, 0, 0, NULL, NULL);
    if(NetErr==DP_OK || NetErr==DPERR_PENDING) 
        return 0;
    NetErrChar=GetDirectPlayErrStr(NetErr);
    return -2;
}


EXPORT  int     FNBACK  NET_send_chat(NETMSG * msg)
{
    DPCHAT  chat={sizeof(DPCHAT),0,NULL};

    if(!(NetFlag&NETDP_JOIN)) 
        return -1;
    chat.lpszMessageA = msg->message;
    NetErr = IDirectPlayX_SendChatMessage(lpdp,msg->idFrom,msg->idTo,0,&chat);
    if(FAILED(NetErr))
    { 
        NetErrChar=GetDirectPlayErrStr(NetErr); 
        return -2; 
    }
    return 0;
}


EXPORT  int     FNBACK  NET_send_chat( LPVOID lpBuffer, DWORD dwSize, DPID idFrom, DPID idTo)
{
    DPCHAT  chat={sizeof(DPCHAT),0,NULL};

    if(!(NetFlag&NETDP_JOIN)) 
        return -1;
    chat.lpszMessageA = (char *)lpBuffer;
    NetErr = IDirectPlayX_SendChatMessage(lpdp, idFrom, idTo, 0, &chat);
    if(FAILED(NetErr))
    { 
        NetErrChar=GetDirectPlayErrStr(NetErr); 
        return -2; 
    }
    return 0;
}


EXPORT  int     FNBACK  NET_close(void)
{
    if(!(NetFlag&NETDP_JOIN)) return -1;
    IDirectPlayX_DestroyPlayer(lpdp,Player);
    IDirectPlayX_Close(lpdp);
    Sleep(200);
    NetFlag = NETDP_INIT|NETDP_ENUMCONNECT|NETDP_CONNECT;
    return 0;
}

EXPORT  int     FNBACK  NET_off_connect(void)
{
    if((!(NetFlag&NETDP_CONNECT)) || (NetFlag & NETDP_JOIN)) 
        return -1;
    IDirectPlayX_Release(lpdp);
    lpdp = NULL;
    NetFlag = NETDP_INIT|NETDP_ENUMCONNECT;
    return 0;
}

EXPORT  int     FNBACK  NET_enum_players(int num,ENUMPLAYER *p)
{
    LPGUID  lpGuid;
    
    if(!(NetFlag&NETDP_CONNECT)) 
        return -1;
    enumcb = p;
    if(num>=0 && num<DPSCount)    // enumerate players in the specified session
        lpGuid = (LPGUID) &DPSInfo[num-1].dpsd.guidInstance;
    else                                // enumerate players in the current opened session
        lpGuid = (LPGUID) &GUID_NULL;
    NetErr = IDirectPlayX_EnumPlayers(lpdp,lpGuid,( LPDPENUMPLAYERSCALLBACK2 )DPEPCallBack,NULL,DPENUMPLAYERS_ALL);
    if(FAILED(NetErr))
    { 
        NetErrChar=GetDirectPlayErrStr(NetErr); 
        return -2; 
    }
    return 0;
}

static  BOOL CALLBACK   DPEPCallBack(DPID id,DWORD Type,LPCDPNAME lpname,DWORD Flags,LPVOID p)
{
    enumcb(id,lpname->lpszShortNameA,lpname->lpszShortNameA);
    return TRUE;
}

EXPORT  void    FNBACK  NET_get_player_name(DPID player_id,char **friendlyname,char **formalname)
{
    DWORD       dwDataSize;
    LPDPNAME    lpName;
    static  char    friendly[80];
    static  char    formal[80];
    
    // Get the data size and allocate a buffer for it.
    lpdp->GetPlayerName( player_id, NULL, &dwDataSize );
    lpName = ( LPDPNAME )GlobalAlloc(GPTR,  dwDataSize );
    lpdp->GetPlayerName( player_id, lpName, &dwDataSize );
    memset(friendly,0x00,80);
    memset(formal,0x00,80);
    if(lpName->lpszShortNameA)
        strcpy(friendly,(const char *)lpName->lpszShortNameA);
    if(lpName->lpszLongNameA)
        strcpy(formal,(const char *)lpName->lpszLongNameA);
    GlobalFree( lpName );
    *friendlyname = &friendly[0];
    *formalname = &formal[0];
}

EXPORT  int     FNBACK  NET_create_session(char *sname, int max_player, DPID *play_id, char *names, char *namel)
{
    DPSESSIONDESC2  dpsd;
    DPNAME  dpname;
    
    if(!(NetFlag&NETDP_CONNECT)) 
        return -3;
    if(NetFlag&(NETDP_JOIN|NETDP_CREATE)) 
        return -2;
    ZeroMemory(&dpsd,sizeof(dpsd));
    dpsd.dwSize = sizeof(dpsd);
    dpsd.guidApplication = NETGAME;
    //    dpsd.dwFlags = DPSESSION_MIGRATEHOST | DPSESSION_KEEPALIVE;
    dpsd.dwFlags = DPSESSION_KEEPALIVE;
    dpsd.dwMaxPlayers = max_player;
    dpsd.lpszSessionNameA = sname;
    //  NetErr=IDirectPlayX_Open(lpdp,&dpsd,DPOPEN_CREATE | DPOPEN_RETURNSTATUS);
    //  if(NetErr==DPERR_CONNECTING) return 1;
    NetErr = IDirectPlayX_Open(lpdp,&dpsd,DPOPEN_CREATE);
    if(FAILED(NetErr))
    { 
        NetErrChar=GetDirectPlayErrStr(NetErr); 
        return -1; 
    }
    ZeroMemory(&dpname,sizeof(dpname));
    dpname.dwSize = sizeof(dpname);
    dpname.dwFlags = 0;
    dpname.lpszShortNameA = names;
    dpname.lpszLongNameA = namel;
    NetErr=IDirectPlayX_CreatePlayer(lpdp,play_id,&dpname,PlayerEvent,NULL,0,0);
    Player=*play_id;
    NetFlag |= NETDP_JOIN|NETDP_CREATE;
    return 0;
}

EXPORT  int     FNBACK  NET_end_join(void)
{
    LPDPSESSIONDESC2 dpsd;
    unsigned long    size=0;
    
    if(!(NetFlag&NETDP_CREATE)) 
        return -3;
    NetErr = IDirectPlayX_GetSessionDesc(lpdp,NULL,&size);
    if((dpsd=(DPSESSIONDESC2 *)GlobalAlloc(GPTR, size))==NULL) 
        return -2;
    NetErr = IDirectPlayX_GetSessionDesc(lpdp,dpsd,&size);
    dpsd->dwFlags |= DPSESSION_JOINDISABLED;
    NetErr = IDirectPlayX_SetSessionDesc(lpdp,dpsd,0);
    if(FAILED(NetErr))
    { 
        NetErrChar=GetDirectPlayErrStr(NetErr); 
        return -1; 
    }
    NetFlag |= NETDP_ENDJOIN;
    return 0;
}

EXPORT  int     FNBACK  NET_session_name(char * name)
{
    LPDPSESSIONDESC2 dpsd;
    unsigned long    size=0;
    
    if(!(NetFlag&NETDP_CREATE)) 
        return -3;
    NetErr = IDirectPlayX_GetSessionDesc(lpdp,NULL,&size);
    if((dpsd=(DPSESSIONDESC2 *)GlobalAlloc(GPTR, size))==NULL) 
        return -2;
    NetErr = IDirectPlayX_GetSessionDesc(lpdp,dpsd,&size);
    dpsd->lpszSessionNameA=name;
    NetErr = IDirectPlayX_SetSessionDesc(lpdp,dpsd,0);
    if(FAILED(NetErr))
    { 
        NetErrChar=GetDirectPlayErrStr(NetErr); 
        return -1; 
    }
    return 0;
}

EXPORT  int     FNBACK  NET_player_name(char * names,char * namel)
{
    DPNAME  dpname;
    
    if(!(NetFlag&NETDP_JOIN)) 
        return -1;
    dpname.dwSize=sizeof(dpname);
    dpname.dwFlags=0;
    dpname.lpszShortNameA=names;
    dpname.lpszLongNameA=namel;
    NetErr = IDirectPlayX_SetPlayerName(lpdp,Player,&dpname,DPSET_REMOTE);
    if(FAILED(NetErr))
    { 
        NetErrChar=GetDirectPlayErrStr(NetErr); 
        return -1; 
    }
    return 0;
}

/******************************************************************************/
static  DWORD WINAPI ReceiveThread( LPVOID lpParameter )
{
    HANDLE  eventHandles[2]={PlayerEvent,ExitEvent};
    
    // Wait for either the player or kill event to fire.  If it
    // is the player event (WAIT_OBJECT_0), process the messages
    // and wait again.  If it's the kill event, shut down the
    // thread and exit
    while ( WaitForMultipleObjects( 2, eventHandles, FALSE,INFINITE ) == WAIT_OBJECT_0 )
    {
        ReceiveMessages(lpParameter);
    }
    return 0;
}

static  DWORD   WINAPI  ReceiveMessages(LPVOID p)
{
    DPID    idFrom,idTo;
    DWORD   nBytes;
    HRESULT dprval;
    BOOL    dwLoop;
    
    // Don't let Receive work use the global value directly,
    // as it changes it.
    nBytes = dwReceiveBufferSize;
    
    dwLoop = TRUE;
    while( dwLoop ) //while( TRUE )
    {
        dprval = IDirectPlayX_Receive(lpdp, &idFrom,&idTo,
            DPRECEIVE_ALL,lpReceiveBuffer,&nBytes);
        if(DPERR_BUFFERTOOSMALL == dprval)
            // The recieve buffer size must be adjusted.
        {
            if(lpReceiveBuffer == NULL)
            {
                lpReceiveBuffer = GlobalAlloc(GPTR, nBytes);
                if(lpReceiveBuffer == NULL)
                {
                    // MEMORY ALLOC ERROR
                    sprintf((char *)print_rec,"%s","MSG receive buffer alloc error");
                    log_error(1,print_rec);
                    return 0xffffffff;//DPERR_NOMEMORY;
                }
            }
            else
            {
                GlobalFree(lpReceiveBuffer);
                lpReceiveBuffer = GlobalAlloc(GPTR, nBytes);
                if(lpReceiveBuffer == NULL)
                {
                    // MEMORY ALLOC ERROR
                    sprintf((char *)print_rec,"%s","MSG receive buffer alloc error");
                    log_error(1,print_rec);
                    return 0xffffffff;//DPERR_NOMEMORY;
                }
            }
            
            // Update our global to the new buffer size.
            dwReceiveBufferSize = nBytes;
        }
        else if( ( dprval == DP_OK) && (dwReceiveBufferSize >= sizeof(DPMSG_GENERIC)) )
            // A message was successfully retrieved.
        {
            if( DPID_SYSMSG == idFrom)
            {
                if( pfnDoSystemMsg )
                {
                    pfnDoSystemMsg((DPMSG_GENERIC *)lpReceiveBuffer,dwReceiveBufferSize,
                        idFrom, idTo);
                }
                //DoSystemMessage((DPMSG_GENERIC *)lpReceiveBuffer,dwReceiveBufferSize,
                //  idFrom, idTo);
            }
            else // user message
            {
                if( pfnDoAppMsg )
                {
                    pfnDoAppMsg((DPMSG_GENERIC *)lpReceiveBuffer,dwReceiveBufferSize,
                        idFrom, idTo);
                }
                //DoApplicationMessage((DPMSG_GENERIC *)lpReceiveBuffer,dwReceiveBufferSize,
                //  idFrom, idTo);
            }
        }
        else
        {
            dwLoop = FALSE;
        }
    }
    return 0xffffffff;//return 0
}

static  char *  GetDirectPlayErrStr(HRESULT hr)
{
    static  char    szTempStr[128];
    
    switch (hr)
    {
    case DP_OK:
        strcpy((char *)szTempStr,"DP_OK");
        break;
    case DPERR_ALREADYINITIALIZED:
        strcpy((char *)szTempStr,"DPERR_ALREADYINITIALIZED");
        break;
    case DPERR_ACCESSDENIED:
        strcpy((char *)szTempStr,"DPERR_ACCESSDENIED");
        break;
    case DPERR_ACTIVEPLAYERS:
        strcpy((char *)szTempStr,"DPERR_ACTIVEPLAYERS");
        break;
    case DPERR_BUFFERTOOSMALL:
        strcpy((char *)szTempStr,"DPERR_BUFFERTOOSMALL");
        break;
    case DPERR_CANTADDPLAYER:
        strcpy((char *)szTempStr,"DPERR_CANTADDPLAYER");
        break;
    case DPERR_CANTCREATEGROUP:
        strcpy((char *)szTempStr,"DPERR_CANTCREATEGROUP");
        break;
    case DPERR_CANTCREATEPLAYER:
        strcpy((char *)szTempStr,"DPERR_CANTCREATEPLAYER");
        break;
    case DPERR_CANTCREATESESSION:
        strcpy((char *)szTempStr,"DPERR_CANTCREATESESSION");
        break;
    case DPERR_CAPSNOTAVAILABLEYET:
        strcpy((char *)szTempStr,"DPERR_CAPSNOTAVAILABLEYET");
        break;
    case DPERR_EXCEPTION:
        strcpy((char *)szTempStr,"DPERR_EXCEPTION");
        break;
    case DPERR_GENERIC:
        strcpy((char *)szTempStr,"DPERR_GENERIC");
        break;
    case DPERR_INVALIDFLAGS:
        strcpy((char *)szTempStr,"DPERR_INVALIDFLAGS");
        break;
    case DPERR_INVALIDOBJECT:
        strcpy((char *)szTempStr,"DPERR_INVALIDOBJECT");
        break;
    case DPERR_INVALIDPARAMS:
        strcpy((char *)szTempStr,"DPERR_INVALIDPARAMS");
        break;
    case DPERR_INVALIDPLAYER:
        strcpy((char *)szTempStr,"DPERR_INVALIDPLAYER");
        break;
    case DPERR_INVALIDGROUP:
        strcpy((char *)szTempStr,"DPERR_INVALIDGROUP");
        break;
    case DPERR_NOCAPS:
        strcpy((char *)szTempStr,"DPERR_NOCAPS");
        break;
    case DPERR_NOCONNECTION:
        strcpy((char *)szTempStr,"DPERR_NOCONNECTION");
        break;
    case DPERR_OUTOFMEMORY:
        strcpy((char *)szTempStr,"DPERR_OUTOFMEMORY");
        break;
    case DPERR_NOMESSAGES:
        strcpy((char *)szTempStr,"DPERR_NOMESSAGES");
        break;
    case DPERR_NONAMESERVERFOUND:
        strcpy((char *)szTempStr,"DPERR_NONAMESERVERFOUND");
        break;
    case DPERR_NOPLAYERS:
        strcpy((char *)szTempStr,"DPERR_NOPLAYERS");
        break;
    case DPERR_NOSESSIONS:
        strcpy((char *)szTempStr,"DPERR_NOSESSIONS");
        break;
    case DPERR_PENDING:
        strcpy((char *)szTempStr,"DPERR_PENDING");
        break;
    case DPERR_SENDTOOBIG:
        strcpy((char *)szTempStr,"DPERR_SENDTOOBIG");
        break;
    case DPERR_TIMEOUT:
        strcpy((char *)szTempStr,"DPERR_TIMEOUT");
        break;
    case DPERR_UNAVAILABLE:
        strcpy((char *)szTempStr,"DPERR_UNAVAILABLE");
        break;
    case DPERR_UNSUPPORTED:
        strcpy((char *)szTempStr,"DPERR_UNSUPPORTED");
        break;
    case DPERR_BUSY:
        strcpy((char *)szTempStr,"DPERR_BUSY");
        break;
    case DPERR_USERCANCEL:
        strcpy((char *)szTempStr,"DPERR_USERCANCEL");
        break;
    case DPERR_NOINTERFACE:
        strcpy((char *)szTempStr,"DPERR_NOINTERFACE");
        break;
    case DPERR_CANNOTCREATESERVER:
        strcpy((char *)szTempStr,"DPERR_CANNOTCREATESERVER");
        break;
    case DPERR_PLAYERLOST:
        strcpy((char *)szTempStr,"DPERR_PLAYERLOST");
        break;
    case DPERR_SESSIONLOST:
        strcpy((char *)szTempStr,"DPERR_SESSIONLOST");
        break;
    case DPERR_UNINITIALIZED:
        strcpy((char *)szTempStr,"DPERR_UNINITIALIZED");
        break;
    case DPERR_NONEWPLAYERS:
        strcpy((char *)szTempStr,"DPERR_NONEWPLAYERS");
        break;
    case DPERR_INVALIDPASSWORD:
        strcpy((char *)szTempStr,"DPERR_INVALIDPASSWORD");
        break;
    case DPERR_CONNECTING:
        strcpy((char *)szTempStr,"DPERR_CONNECTING");
        break;
    case DPERR_CONNECTIONLOST:
        strcpy((char *)szTempStr,"DPERR_CONNECTIONLOST");
    case DPERR_UNKNOWNMESSAGE:
        strcpy((char *)szTempStr,"DPERR_UNKNOWNMESSAGE");
        break;
    case DPERR_CANCELFAILED:
        strcpy((char *)szTempStr,"DPERR_CANCELFAILED");
        break;
    case DPERR_INVALIDPRIORITY:
        strcpy((char *)szTempStr,"DPERR_INVALIDPRIORITY");
        break;
    case DPERR_NOTHANDLED:
        strcpy((char *)szTempStr,"DPERR_NOTHANDLED");
        break;
    case DPERR_CANCELLED:
        strcpy((char *)szTempStr,"DPERR_CANCELLED");
        break;
    case DPERR_ABORTED:
        strcpy((char *)szTempStr,"DPERR_ABORTED");
        break;
    case DPERR_BUFFERTOOLARGE:
        strcpy((char *)szTempStr,"DPERR_BUFFERTOOLARGE");
        break;
    case DPERR_CANTCREATEPROCESS:
        strcpy((char *)szTempStr,"DPERR_CANTCREATEPROCESS");
        break;
    case DPERR_APPNOTSTARTED:
        strcpy((char *)szTempStr,"DPERR_APPNOTSTARTED");
        break;
    case DPERR_INVALIDINTERFACE:
        strcpy((char *)szTempStr,"DPERR_INVALIDINTERFACE");
        break;
    case DPERR_NOSERVICEPROVIDER:
        strcpy((char *)szTempStr,"DPERR_NOSERVICEPROVIDER");
        break;
    case DPERR_UNKNOWNAPPLICATION:
        strcpy((char *)szTempStr,"DPERR_UNKNOWNAPPLICATION");
        break;
    case DPERR_NOTLOBBIED:
        strcpy((char *)szTempStr,"DPERR_NOTLOBBIED");
        break;
    case DPERR_SERVICEPROVIDERLOADED:
        strcpy((char *)szTempStr,"DPERR_SERVICEPROVIDERLOADED");
        break;
    case DPERR_ALREADYREGISTERED:
        strcpy((char *)szTempStr,"DPERR_ALREADYREGISTERED");
        break;
    case DPERR_NOTREGISTERED:
        strcpy((char *)szTempStr,"DPERR_NOTREGISTERED");
        break;
    case DPERR_AUTHENTICATIONFAILED:
        strcpy((char *)szTempStr,"DPERR_AUTHENTICATIONFAILED");
        break;
    case DPERR_CANTLOADSSPI:
        strcpy((char *)szTempStr,"DPERR_CANTLOADSSPI");
        break;
    case DPERR_ENCRYPTIONFAILED:
        strcpy((char *)szTempStr,"DPERR_ENCRYPTIONFAILED");
        break;
    case DPERR_SIGNFAILED:
        strcpy((char *)szTempStr,"DPERR_SIGNFAILED");
        break;
    case DPERR_CANTLOADSECURITYPACKAGE:
        strcpy((char *)szTempStr,"DPERR_CANTLOADSECURITYPACKAGE");
        break;
    case DPERR_ENCRYPTIONNOTSUPPORTED:
        strcpy((char *)szTempStr,"DPERR_ENCRYPTIONNOTSUPPORTED");
        break;
    case DPERR_CANTLOADCAPI:
        strcpy((char *)szTempStr,"DPERR_CANTLOADCAPI");
        break;
    case DPERR_NOTLOGGEDIN:
        strcpy((char *)szTempStr,"DPERR_NOTLOGGEDIN");
        break;
    case DPERR_LOGONDENIED:
        strcpy((char *)szTempStr,"DPERR_LOGONDENIED");
        break;
    default:
        wsprintf(szTempStr, "0x%08X", hr);
        break;
    }
    sprintf((char *)print_rec,"ERROR :%s",szTempStr);
    log_error(1,print_rec);
    return (szTempStr);
}
//============================================================================================
