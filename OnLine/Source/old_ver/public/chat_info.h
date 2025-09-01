/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.

  *file name    : chat_info.h
  *owner        : Ben
  *description  : chat and worldbase serverͨ����Ϣ����Ϣ��������ͷ�ļ�
				  ����������������������Ϣ����Ϊ 0x0004xxxx
				  chat server�����ṹ����ͷ�ļ�
  *modified     : 2004/12/06
******************************************************************************/ 
#ifndef __CHAT_SERVER_BASE_INFO__
#define __CHAT_SERVER_BASE_INFO__
//-------------------------------------------------------------------------------------
#define		UNION_MAX_PLAYER			250	//����(����)���� 20-250��
#define		TEAM_MAX_PLAYER				5	//�������� 2-5��
#define		CHAT_ROOM_MAX_PLAYER		20	//�������������� 1-20��
#define		MAX_MAIL_MSG_COUNT			250 //�û�����ܽ��յĶ���Ϣ������
#define		MAX_MAIL_MSG_SQL			50 //�����д����ݿ��е����Ķ���Ϣ������

//------Ƶ��������----------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------
enum
{
	CHECK_DATA_MAILOUT = -4, //�ʼ�������ȥ
	CHECK_DATA_TIMEOUT = -3, //ʱ��������
	CHECK_DATA_LEN_ERROR = -2, //���ݳ��ȳ���
	CHECK_ERROR = -1,
	CHECK_OK = 0,
};

//----------------chatserver�������õ�worldbase�Ļ�������:�������ݣ���������---------
#define	NET_MSG_WBCHAT_BASEINFO						0x00040001
#define	NET_MSG_WBCHAT_BASEINFO_RETURN				0x00040002
typedef struct _CHAT_BASEINFO
{
	struct
	{
		int		nSceneNum; //��������
		int		nUnionCurrentNum; //Ŀǰ�������		
	}base;
	
	int* scenelist; //�����б�
	int* unionlist; //Ŀǰ�����б�
	
} CHAT_BASEINFO, *PCHAT_BASEINFO;

//chatserver��worldbase����У������,��ҵ�worldbaseУ���Ƿ�Ϊ�Ϸ���ҡ�
#define	NET_MSG_WBCHAT_CHECK_RETURN					0x00040005
//worldbase��chatserver����У����
#define	NET_MSG_WBCHAT_CHECK						0x00040006
#define	NET_MSG_WBCHAT_RECV_CHECK_RETURN			0x00040007
typedef struct _CHAT_CHECK
{
	int		nPlayerid; //���id
	int		nPlayerPointer; //��ұ�־������ԭ�ⲻ���Ĵ�������
	
} CHAT_CHECK, *PCHAT_CHECK;


typedef struct _CHAT_CHECK_RETURN
{
	int		nPlayerid; //���id
	int 	nSceneID; //���ڳ���id
	int		nPlayerPointer; //��ұ�־������ԭ�ⲻ���Ĵ�������
	int		nAttribute; //�������,GM or other.
	int		nLevel; //��ҵȼ�
	char	szNickName[17]; //����ǳ�
	
} CHAT_CHECK_RETURN, *PCHAT_CHECK_RETURN;

//---------���ѻ�����Ϣ-------------------------------------------------------
#define	NET_MSG_BASE_FRIEND_LIST_RETURN				0x00040011
#define	NET_MSG_BASE_FRIEND_LIST					0x00040012

struct friend_t
{
	int id;
	char type;
};

typedef struct _BASE_FRIEDN_LIST
{
	int num; //���Ѹ���

	friend_t *friendlist; //id���б�

} BASE_FRIEDN_LIST, *PBASE_FRIEDN_LIST;

//����Ϊ���id������Ϊ����id
#define	NET_MSG_CHAT_ADD_FRIEND_RETURN				0x00040015 //�Ӻ���
#define	NET_MSG_CHAT_ADD_FRIEND						0x00040016
#define	NET_MSG_CHAT_SUB_FRIEND_RETURN				0x00040017 //ɾ������
#define	NET_MSG_CHAT_SUB_FRIEND						0x00040018
//����Ϊ���id������Ϊfriend_t
#define	NET_MSG_CHAT_CHANG_FRIEND_RETURN			0x00040020 //�Ӻ���
#define	NET_MSG_CHAT_CHANG_FRIEND					0x00040021

//-------------------------------------------------------------------------------------
//��ҽ���������Ϣ
#define	NET_MSG_WBCHAT_SCENE_ENTER					0x00040035
#define	NET_MSG_WBCHAT_SCENE_ENTER_RETURN			0x00040036
#define	NET_MSG_WBCHAT_SCENE_EXIT					0x00040037
#define	NET_MSG_WBCHAT_SCENE_EXIT_RETURN			0x00040038
typedef struct _CHAT_SCENE
{
	int		nPlayerid; //���id
	int		nSceneid; //����id

} CHAT_SCENE, *PCHAT_SCENE;

//-------------------------------------------------------------------------------------
//worldbase��chatserver���ͶϿ�ĳ�����Ϣ���������ڽ�Ǯ���������ԭ�򣬲���Ϊplayerid
#define	NET_MSG_WBCHAT_DISCONNECT					0x00040064
#define	NET_MSG_WBCHAT_DISCONNECT_RETURN			0x00040066

//-------------------------------------------------------------------------------------
//chatserver��ͻ��˳���֮���ͨ��Э��
//chatserver�����ͻ��˳����Ƶ����Ϣ
#define	NET_MSG_CURRENT_CHANNEL_RETURN			0x00040074  //��ǰƵ�����������˷���
#define	NET_MSG_CURRENT_CHANNEL					0x00040075  //��ǰƵ�����ͻ��˷���
//����Ƶ��������Ϊ������id������Ϊ��������
#define	NET_MSG_TEAM_CHANNEL_RETURN				0x00040076  //����Ƶ�����������˷���
#define	NET_MSG_TEAM_CHANNEL					0x00040077  //����Ƶ�����ͻ��˷���
#define	NET_MSG_UNION_CHANNEL_RETURN			0x00040078  //����Ƶ�����������˷���
#define	NET_MSG_UNION_CHANNEL					0x00040079  //����Ƶ�����ͻ��˷���
#define	NET_MSG_TRADE_CHANNEL_RETURN			0x00040080  //����Ƶ�����������˷���
#define	NET_MSG_TRADE_CHANNEL					0x00040081  //����Ƶ�����ͻ��˷���
#define	NET_MSG_WORLD_CHANNEL_RETURN			0x00040082  //����Ƶ�����������˷���
#define	NET_MSG_WORLD_CHANNEL					0x00040083  //����Ƶ�����ͻ��˷���
#define	NET_MSG_PRIVATE_CHANNEL_RETURN			0x00040084  //˽��Ƶ�����������˷���
#define	NET_MSG_PRIVATE_CHANNEL					0x00040085  //˽��Ƶ�����ͻ��˷���
#define	NET_MSG_SYSTEM_CHANNEL_RETURN			0x00040086  //ϵͳƵ�����������˷���
#define	NET_MSG_SYSTEM_CHANNEL					0x00040087  //ϵͳƵ�����ͻ��˷���

//����Ϊ�û��Լ���id
#define	NET_MSG_CHAT_CURRENT_RETURN				0x00040088 //��ȡ��ǰƵ���û�id
#define	NET_MSG_CHAT_CURRENT					0x00040089 //���ص�ǰƵ���û�id

typedef struct _CURRENT_ID
{
	struct  
	{
		int		idx; //�û������͵�Ƶ������id
		int		num; //��ǰ�û�id��
	}base;

	int*	id_list;

} CURRENT_ID, *PCURRENT_ID;

typedef struct _WORLD_CHANNEL
{
	struct
	{
		int  id; //������id
		unsigned char name_len; //�������ǳƳ���
		unsigned char content_len; //�������ݳ���
	}base;
	
	char *nickname;
	char *content;

} WORLD_CHANNEL, *PWORLD_CHANNEL;

typedef struct _TRADE_CHANNEL
{
	struct
	{
		int  n32Playerid; //������id
		unsigned char byNameLen; //�������ǳƳ���
		unsigned char byContentLen; //�������ݳ���
	}base;
	
	char *pchNickname;
	char *pchContent;
	
} TRADE_CHANNEL, *PTRADE_CHANNEL;

typedef struct _PRIVATE_CHANNEL
{
	struct 
	{
		int  id;  //������id
		int  rec_id; //������id
		unsigned char name_len; //�����ߵ��ǳƳ���(���շ�)��������ǳƳ���(���ͷ�)
		unsigned char content_len;
	}base;
	
	char *nickname;
	char *content;

}PRIVATE_CHANNEL, *PPRIVATE_CHANNEL;

//---------worldbase������Ϣ������Ϊ�ӳ�id������Ϊ���id(����������Ϣʱû������)----------------------
#define	NET_MSG_CHAT_CREATE_TEAM_RETURN				0x00040115 //����һ������
#define	NET_MSG_CHAT_CREATE_TEAM					0x00040116

#define	NET_MSG_CHAT_EXIT_TEAM_RETURN				0x00040117 //�˳�����
#define	NET_MSG_CHAT_EXIT_TEAM						0x00040118

#define	NET_MSG_CHAT_ADD_TO_TEAM_RETURN				0x00040121 //����һ������
#define	NET_MSG_CHAT_ADD_TO_TEAM					0x00040122

//---------worldbase������Ϣ������Ϊ���id������Ϊ����id----------------------
#define	NET_MSG_CHAT_ADD_UNION_RETURN				0x00040140 //��һ������
#define	NET_MSG_CHAT_ADD_UNION						0x00040141
#define	NET_MSG_CHAT_SUB_UNION_RETURN				0x00040142 //ɾһ������
#define	NET_MSG_CHAT_SUB_UNION						0x00040143
#define	NET_MSG_CHAT_UNION_ENTER_RETURN				0x00040144 //����һ������
#define	NET_MSG_CHAT_UNION_ENTER					0x00040145
#define	NET_MSG_CHAT_UNION_EXIT_RETURN				0x00040146 //�˳�����
#define	NET_MSG_CHAT_UNION_EXIT						0x00040147

//-----------mail and message-------------------------------------------
//����Ϊ�ʼ����������
#define	NET_MSG_CHAT_MAIL_COUNT						0x00040246
#define	NET_MSG_CHAT_MAIL_COUNT_RETURN				0x00040247
//����Ϊ���id
#define	NET_MSG_CHAT_GET_MAIL						0x00040248
//����ΪMAIL_MSG
#define	NET_MSG_CHAT_GET_MAIL_RETURN				0x00040249
#define	NET_MSG_CHAT_SEND_MAIL						0x00040250
#define	NET_MSG_CHAT_SEND_MAIL_RETURN				0x00040251

typedef struct _MAIL_MSG
{
	struct 
	{
		int		nSendid;  //������id
		int		nRecvid; //������id
		long	lTimer; //���͵�ʱ�䣬�����߲�����д��
		unsigned char byNameLen; //�����ߵ��ǳƳ���(���շ�)��������ǳƳ���(���ͷ�)
		unsigned char byContentLen;
	} base;
	
	char* pchNickname;
	char* pchContent;

} MAIL_MSG, *PMAIL_MSG;

//-----------------------------------------------------------------------
#define NET_MSG_CHAT_SYS_TIME						0x00040260
#define NET_MSG_CHAT_SYS_TIME_RETURN				0x00040260

#endif//(__CHAT_SERVER_BASE_INFO__)
