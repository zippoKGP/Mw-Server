/*
**    XPLAY.H
**    DirectPlay functions header.
**    ZJian,2000/7/10.
*/
#ifndef XPLAY_H_INCLUDE
#define XPLAY_H_INCLUDE    1

#define IDIRECTPLAY2_OR_GREATER
#include <dplay.h>
#include <dplobby.h>

#define MAX_XPLAY_CONNECTS      8
#define MAX_XPLAY_SESSIONS      16
#define MAX_XPLAY_PLAYERS       8

#define NETDP_JOIN              0x01
#define NETDP_SESSION           0x02
#define NETDP_CONNECT           0x04
#define NETDP_ENUMCONNECT       0x08
#define NETDP_INIT              0x10
#define NETDP_CREATE            0x20
#define NETDP_ENDJOIN           0x40

#define NETDSYS_CHAT            1       // �r�Ů���
#define NETDSYS_CREATEPLAYER    2       // �s�C���̥[�J��e�|��
#define NETDSYS_DESTROYPLAYER   3       // ���������}��e�|��
#define NETDSYS_HOST            4       // ���C���̺٬��|�ܥD
#define NETDSYS_SESSIONLOST     5       // �|�ܥᥢ,�p�I�_�}
#define NETDSYS_PLAYERNAME      6       // �C���̦W�٧���
#define NETDSYS_SESSIONDESC     7       // �|�ܴy�z����
#define NETDSYS_SEND            8       // �����o�X(���ϥ�)
#define NETDSYS_USERMSG         65536   // �۩w�q����

#define NET_DOK                 0
#define NETERR_DNODXX           -1
#define NETERR_DENUMERR         -2
#define NETERR_INIT             -3

#define NETCONNECTOTHER         1       // �䥦�s��
#define NETCONNECTIPX           2       // IPX�s��
#define NETCONNECTTCPIP         3       // TCP/IP�s��
#define NETCONNECTMODEM         4       // MODEM�s��
#define NETCONNECTSERIAL        5       // SERIAL�s��


#define PLAYERID                DPID
#define ALLPLAYER               DPID_ALLPLAYERS // �Ҧ�������ID

typedef struct  NETMSG_STRUCT           NETMSG,*LPNETMSG;
typedef struct  DPDRIVER_STRUCT         DPDRIVER,*LPDPDRIVER;
typedef struct  DPSESSION_STRUCT        DPSESSION,*LPDPSESSION;
typedef struct  DPPLAYER_STRUCT         DPPLAYER,*LPDPPLAYER;

typedef void    (ENUMPLAYER)(DPID id, char *names, char *namel);
typedef void    (FUNCDOMSG)(DPMSG_GENERIC* pMsg, DWORD dwMsgSize, DPID idFrom, DPID idTo );


struct  NETMSG_STRUCT       // ��������
{
    DWORD       dwType;         // ��������
    union
    {
        DWORD   Flags;
        DWORD   size;           // ��������
    };
    PLAYERID    idFrom;         // �o����������ID
    PLAYERID    idTo;           // ������������ID
    DWORD       dwCurrentPlayers;
    union
    {
        LPVOID  data;           // �������u�a�}
        char *  message;        // �H0�������r�Ŧ�
        char *  ShortName;
    };
    union
    {
        char *  LongName;
        LPDPSESSIONDESC2 desc;
    };
};

typedef struct  DPCNAME_STRUCT  DPCNAME,LPDPCNAME;
struct  DPCNAME_STRUCT
{
    char    *name;      // connection name
    int     type;       // connection type
};


EXPORT  int     FNBACK  init_net(void);
EXPORT  void    FNBACK  free_net(void);
EXPORT  void    FNBACK  active_net(int active);
EXPORT  void    FNBACK  set_net_func( FUNCDOMSG *do_sys, FUNCDOMSG *do_app);

EXPORT  int     FNBACK  NET_get_connects(int ID, DPCNAME **lpname, int *count);
EXPORT  int     FNBACK  NET_set_connect(int num);
EXPORT  int     FNBACK  NET_off_connect(void);
EXPORT  void    FNBACK  NET_set_phone(char *phone);
EXPORT  void    FNBACK  NET_set_ip_address(char *address);
EXPORT  void    FNBACK  NET_set_com_port(int port,int speed);

EXPORT  int     FNBACK  NET_get_sessions(char ***lpname, int *count);
EXPORT  int     FNBACK  NET_create_session(char *sname, int max_player, DPID *play_id, char *names, char *namel);
EXPORT  int     FNBACK  NET_join_session(int num, DPID *play_id, char *names, char *namel);
EXPORT  int     FNBACK  NET_session_name(char *name);

EXPORT  int     FNBACK  NET_send(NETMSG *netmsg);
EXPORT  int     FNBACK  NET_send_data( LPVOID lpBuffer, DWORD dwSize, DPID idFrom, DPID idTo );
EXPORT  int     FNBACK  NET_send_chat(NETMSG *netmsg);
EXPORT  int     FNBACK  NET_send_chat( LPVOID lpBuffer, DWORD dwSize, DPID idFrom, DPID idTo );
EXPORT  int     FNBACK  NET_close(void);
EXPORT  int     FNBACK  NET_end_join(void);

EXPORT  int     FNBACK  NET_enum_players(int num,ENUMPLAYER *p);
EXPORT  int     FNBACK  NET_player_name(char *names,char *namel);
EXPORT  void    FNBACK  NET_get_player_name(DPID player_id,char **friendlyname,char **formalname);


#endif//XPLAY_H_INCLUDE

//========================================================================
