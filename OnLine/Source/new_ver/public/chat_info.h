/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.

  *file name    : chat_info.h
  *owner        : Ben
  *description  : chat and worldbase server通信消息和消息参数定义头文件
				  聊天服务器与聊天服务器消息规则为 0x0004xxxx
				  chat server基本结构定义头文件
  *modified     : 2004/12/06
******************************************************************************/ 
#ifndef __CHAT_SERVER_BASE_INFO__
#define __CHAT_SERVER_BASE_INFO__
//-------------------------------------------------------------------------------------
#define		UNION_MAX_PLAYER			250	//工会(国家)人数 20-250人
#define		TEAM_MAX_PLAYER				5	//队伍人数 2-5人
#define		CHAT_ROOM_MAX_PLAYER		20	//交易聊天室人数 1-20人
#define		MAX_MAIL_MSG_COUNT			250 //用户最大能接收的短消息最大个数
#define		MAX_MAIL_MSG_SQL			50 //程序中从数据库中导出的短消息最大个数

//------频道聊天间隔----------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------
enum
{
	CHECK_DATA_MAILOUT = -4, //邮件发不出去
	CHECK_DATA_TIMEOUT = -3, //时间间隔不足
	CHECK_DATA_LEN_ERROR = -2, //数据长度出错
	CHECK_ERROR = -1,
	CHECK_OK = 0,
};

//----------------chatserver启动后获得的worldbase的基本数据:场景数据，工会数据---------
#define	NET_MSG_WBCHAT_BASEINFO						0x00040001
#define	NET_MSG_WBCHAT_BASEINFO_RETURN				0x00040002
typedef struct _CHAT_BASEINFO
{
	struct
	{
		int		nSceneNum; //场景数据
		int		nUnionCurrentNum; //目前工会个数		
	}base;
	
	int* scenelist; //场景列表
	int* unionlist; //目前工会列表
	
} CHAT_BASEINFO, *PCHAT_BASEINFO;

//chatserver向worldbase发送校验数据,玩家到worldbase校验是否为合法玩家。
#define	NET_MSG_WBCHAT_CHECK_RETURN					0x00040005
//worldbase向chatserver发送校验结果
#define	NET_MSG_WBCHAT_CHECK						0x00040006
#define	NET_MSG_WBCHAT_RECV_CHECK_RETURN			0x00040007
typedef struct _CHAT_CHECK
{
	int		nPlayerid; //玩家id
	int		nPlayerPointer; //玩家标志，必须原封不动的传回来。
	
} CHAT_CHECK, *PCHAT_CHECK;


typedef struct _CHAT_CHECK_RETURN
{
	int		nPlayerid; //玩家id
	int 	nSceneID; //所在场景id
	int		nPlayerPointer; //玩家标志，必须原封不动的传回来。
	int		nAttribute; //玩家属性,GM or other.
	int		nLevel; //玩家等级
	char	szNickName[17]; //玩家昵称
	
} CHAT_CHECK_RETURN, *PCHAT_CHECK_RETURN;

//---------好友基本信息-------------------------------------------------------
#define	NET_MSG_BASE_FRIEND_LIST_RETURN				0x00040011
#define	NET_MSG_BASE_FRIEND_LIST					0x00040012

struct friend_t
{
	int id;
	char type;
};

typedef struct _BASE_FRIEDN_LIST
{
	int num; //好友个数

	friend_t *friendlist; //id号列表

} BASE_FRIEDN_LIST, *PBASE_FRIEDN_LIST;

//参数为玩家id，数据为好友id
#define	NET_MSG_CHAT_ADD_FRIEND_RETURN				0x00040015 //加好友
#define	NET_MSG_CHAT_ADD_FRIEND						0x00040016
#define	NET_MSG_CHAT_SUB_FRIEND_RETURN				0x00040017 //删除好友
#define	NET_MSG_CHAT_SUB_FRIEND						0x00040018
//参数为玩家id，数据为friend_t
#define	NET_MSG_CHAT_CHANG_FRIEND_RETURN			0x00040020 //加好友
#define	NET_MSG_CHAT_CHANG_FRIEND					0x00040021

//-------------------------------------------------------------------------------------
//玩家进出场景信息
#define	NET_MSG_WBCHAT_SCENE_ENTER					0x00040035
#define	NET_MSG_WBCHAT_SCENE_ENTER_RETURN			0x00040036
#define	NET_MSG_WBCHAT_SCENE_EXIT					0x00040037
#define	NET_MSG_WBCHAT_SCENE_EXIT_RETURN			0x00040038
typedef struct _CHAT_SCENE
{
	int		nPlayerid; //玩家id
	int		nSceneid; //场景id

} CHAT_SCENE, *PCHAT_SCENE;

//-------------------------------------------------------------------------------------
//worldbase向chatserver发送断开某玩家消息，可能由于金钱不足或其他原因，参数为playerid
#define	NET_MSG_WBCHAT_DISCONNECT					0x00040064
#define	NET_MSG_WBCHAT_DISCONNECT_RETURN			0x00040066

//-------------------------------------------------------------------------------------
//chatserver与客户端程序之间的通信协议
//chatserver发给客户端程序的频道消息
#define	NET_MSG_CURRENT_CHANNEL_RETURN			0x00040074  //当前频道，服务器端发送
#define	NET_MSG_CURRENT_CHANNEL					0x00040075  //当前频道，客户端发送
//队伍频道，参数为发送者id，数据为发送内容
#define	NET_MSG_TEAM_CHANNEL_RETURN				0x00040076  //队伍频道，服务器端发送
#define	NET_MSG_TEAM_CHANNEL					0x00040077  //队伍频道，客户端发送
#define	NET_MSG_UNION_CHANNEL_RETURN			0x00040078  //工会频道，服务器端发送
#define	NET_MSG_UNION_CHANNEL					0x00040079  //工会频道，客户端发送
#define	NET_MSG_TRADE_CHANNEL_RETURN			0x00040080  //经济频道，服务器端发送
#define	NET_MSG_TRADE_CHANNEL					0x00040081  //经济频道，客户端发送
#define	NET_MSG_WORLD_CHANNEL_RETURN			0x00040082  //世界频道，服务器端发送
#define	NET_MSG_WORLD_CHANNEL					0x00040083  //世界频道，客户端发送
#define	NET_MSG_PRIVATE_CHANNEL_RETURN			0x00040084  //私聊频道，服务器端发送
#define	NET_MSG_PRIVATE_CHANNEL					0x00040085  //私聊频道，客户端发送
#define	NET_MSG_SYSTEM_CHANNEL_RETURN			0x00040086  //系统频道，服务器端发送
#define	NET_MSG_SYSTEM_CHANNEL					0x00040087  //系统频道，客户端发送

//参数为用户自己的id
#define	NET_MSG_CHAT_CURRENT_RETURN				0x00040088 //获取当前频道用户id
#define	NET_MSG_CHAT_CURRENT					0x00040089 //返回当前频道用户id

typedef struct _CURRENT_ID
{
	struct  
	{
		int		idx; //用户待发送的频道内容id
		int		num; //当前用户id数
	}base;

	int*	id_list;

} CURRENT_ID, *PCURRENT_ID;

typedef struct _WORLD_CHANNEL
{
	struct
	{
		int  id; //发送者id
		unsigned char name_len; //发送者昵称长度
		unsigned char content_len; //发送内容长度
	}base;
	
	char *nickname;
	char *content;

} WORLD_CHANNEL, *PWORLD_CHANNEL;

typedef struct _TRADE_CHANNEL
{
	struct
	{
		int  n32Playerid; //发送者id
		unsigned char byNameLen; //发送者昵称长度
		unsigned char byContentLen; //发送内容长度
	}base;
	
	char *pchNickname;
	char *pchContent;
	
} TRADE_CHANNEL, *PTRADE_CHANNEL;

typedef struct _PRIVATE_CHANNEL
{
	struct 
	{
		int  id;  //发送者id
		int  rec_id; //接收者id
		unsigned char name_len; //发送者的昵称长度(接收方)或接收者昵称长度(发送方)
		unsigned char content_len;
	}base;
	
	char *nickname;
	char *content;

}PRIVATE_CHANNEL, *PPRIVATE_CHANNEL;

//---------worldbase队伍消息，参数为队长id，数据为玩家id(创建队伍消息时没有数据)----------------------
#define	NET_MSG_CHAT_CREATE_TEAM_RETURN				0x00040115 //创建一个队伍
#define	NET_MSG_CHAT_CREATE_TEAM					0x00040116

#define	NET_MSG_CHAT_EXIT_TEAM_RETURN				0x00040117 //退出队伍
#define	NET_MSG_CHAT_EXIT_TEAM						0x00040118

#define	NET_MSG_CHAT_ADD_TO_TEAM_RETURN				0x00040121 //加入一个队伍
#define	NET_MSG_CHAT_ADD_TO_TEAM					0x00040122

//---------worldbase工会消息，参数为玩家id，数据为工会id----------------------
#define	NET_MSG_CHAT_ADD_UNION_RETURN				0x00040140 //加一个工会
#define	NET_MSG_CHAT_ADD_UNION						0x00040141
#define	NET_MSG_CHAT_SUB_UNION_RETURN				0x00040142 //删一个工会
#define	NET_MSG_CHAT_SUB_UNION						0x00040143
#define	NET_MSG_CHAT_UNION_ENTER_RETURN				0x00040144 //进入一个工会
#define	NET_MSG_CHAT_UNION_ENTER					0x00040145
#define	NET_MSG_CHAT_UNION_EXIT_RETURN				0x00040146 //退出工会
#define	NET_MSG_CHAT_UNION_EXIT						0x00040147

//-----------mail and message-------------------------------------------
//参数为邮件或短信条数
#define	NET_MSG_CHAT_MAIL_COUNT						0x00040246
#define	NET_MSG_CHAT_MAIL_COUNT_RETURN				0x00040247
//参数为玩家id
#define	NET_MSG_CHAT_GET_MAIL						0x00040248
//数据为MAIL_MSG
#define	NET_MSG_CHAT_GET_MAIL_RETURN				0x00040249
#define	NET_MSG_CHAT_SEND_MAIL						0x00040250
#define	NET_MSG_CHAT_SEND_MAIL_RETURN				0x00040251

typedef struct _MAIL_MSG
{
	struct 
	{
		int		nSendid;  //发送者id
		int		nRecvid; //接收者id
		long	lTimer; //发送的时间，发送者不必填写。
		unsigned char byNameLen; //发送者的昵称长度(接收方)或接收者昵称长度(发送方)
		unsigned char byContentLen;
	} base;
	
	char* pchNickname;
	char* pchContent;

} MAIL_MSG, *PMAIL_MSG;

//-----------------------------------------------------------------------
#define NET_MSG_CHAT_SYS_TIME						0x00040260
#define NET_MSG_CHAT_SYS_TIME_RETURN				0x00040260

#endif//(__CHAT_SERVER_BASE_INFO__)
