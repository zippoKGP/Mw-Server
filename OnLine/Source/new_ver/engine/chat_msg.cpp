/******************************************************************************
  *@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2004 ALL RIGHTS RESERVED.

  *file name    : chat_msg.cpp
  *owner  		: Andy
  *description  : 
  *modified     : 2005/1/17
******************************************************************************/ 

#include "engine_global.h"
#include "chat_msg.h"
#include "chat_info.h"
#include "system.h"
#include "net.h"
#include "game.h"
#include "vari-ext.h"
#include "data_proc.h"
#include <time.h>

bool chat_net_lock = false;

//////////////////////////////////////////////////////////////////////////
//
static time_t system_time = 0;
static unsigned long  tick_time = 0;


time_t getServerTime(void)
{
	unsigned long elapsed = Plat_MSTime() - tick_time;

	elapsed /= 1000;

	return system_time + elapsed;
}

//////////////////////////////////////////////////////////////////////////
//
UG_ULONG UG_FUN_CALLBACK ChatNetHandle(UG_DWORD dwMsgID,UG_DWORD dwParam,
										const UG_PCHAR pchData,UG_ULONG ulLen)
{
	chat_net_lock = true;

	switch (dwMsgID) 
	{
	case NET_MSG_DISCONNECT:
		{

		}
		break;
		
	case NET_MSG_CONNECT:
		{

		}
		break;

	case NET_MSG_WORLD_CHANNEL_RETURN:
		{
			if (pchData)
			{
				WORLD_CHANNEL Channel = {0};
				
				Q_memcpy(&Channel, pchData, sizeof(Channel.base));
			
				char nickname[17];

				Q_strncpy(nickname, pchData+sizeof(Channel.base), Channel.base.name_len+1);

				Channel.nickname = nickname;

				char content[1024] = {0};

				int offset = Channel.base.name_len + sizeof(Channel.base);

				Q_strncpy(content, pchData+offset, Channel.base.content_len+1);

				Channel.content = content;

				push_chat_data(CHAT_CHANNEL_WORLD,Channel.base.id,(UCHR *)nickname,(UCHR *)Channel.content);
			}
		}
		break;

	case NET_MSG_PRIVATE_CHANNEL_RETURN:
		{
			if (pchData)
			{
				PRIVATE_CHANNEL Channel = {0};

				Q_memcpy(&Channel, pchData, sizeof(Channel.base));

				char nickname[MAX_NAME_LENGTH+1] = {0};
				
				Q_strncpy(nickname, pchData+sizeof(Channel.base), Channel.base.name_len+1);
				
				Channel.nickname = nickname;
				
				char content[1024] = {0};

				int size = sizeof(Channel.base)+Channel.base.name_len;

				Q_strncpy(content, pchData + size, Channel.base.content_len+1);

				Channel.content = content;

				push_chat_data(CHAT_CHANNEL_PERSON,Channel.base.id,(UCHR *)Channel.nickname,(UCHR *)Channel.content);
			}
		}
		break;

	case NET_MSG_CURRENT_CHANNEL_RETURN: //µ±Ç°ÆµµÀ
		{
			char nickname[MAX_NAME_LENGTH+1] = {0};

			SLONG idx;
			
			if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)
			{
				idx = get_fight_npc_index(dwParam);
				
				if (idx >= 0)
				{
					Q_strncpy(nickname, fight_npc_group[idx].base.name, sizeof(nickname));
					
					char content[1024] = {0};
					
					Q_strncpy(content, pchData, ulLen+1);
					
					push_chat_data(CHAT_CHANNEL_SCREEN, dwParam, (UCHR *)nickname, (UCHR *)content);
					
					npc_talk(dwParam, (UCHR*)content);
				}
			}
			else
			{
				idx = get_map_npc_index(dwParam);
				
				if (idx >= 0)
				{
					Q_strncpy(nickname, map_npc_group[idx].npc_info.name, sizeof(nickname));
					
					char content[1024] = {0};
					
					Q_strncpy(content, pchData, ulLen+1);
					
					push_chat_data(CHAT_CHANNEL_SCREEN, dwParam, (UCHR *)nickname, (UCHR *)content);
					
					npc_talk(dwParam, (UCHR*)content);
				}
			}
			
		}
		break;

	case NET_MSG_TEAM_CHANNEL_RETURN:
		{
			char nickname[MAX_NAME_LENGTH+1] = {0};
			
			SLONG idx = get_map_npc_index(dwParam);

			if (idx >= 0)
			{
				Q_strncpy(nickname, map_npc_group[idx].npc_info.name, sizeof(nickname));
				
				char content[1024] = {0};
				
				Q_strncpy(content, pchData, ulLen+1);
								
				push_chat_data(CHAT_CHANNEL_TEAM, dwParam, (UCHR *)nickname, (UCHR *)content);
			}
		}
		break;

	case NET_MSG_UNION_CHANNEL_RETURN:
		{

		}
		break;

	case NET_MSG_TRADE_CHANNEL_RETURN:
		{
			if (pchData)
			{
				WORLD_CHANNEL Channel = {0};
				
				Q_memcpy(&Channel, pchData, sizeof(Channel.base));
				
				char nickname[17];
				
				Q_strncpy(nickname, pchData+sizeof(Channel.base), Channel.base.name_len+1);
				
				Channel.nickname = nickname;
				
				char content[1024] = {0};
				
				int offset = Channel.base.name_len + sizeof(Channel.base);
				
				Q_strncpy(content, pchData+offset, Channel.base.content_len+1);
				
				Channel.content = content;
				
				push_chat_data(CHAT_CHANNEL_SELLBUY,Channel.base.id,(UCHR *)nickname,(UCHR *)Channel.content);
			}			
		}
		break;

	case NET_MSG_SYSTEM_CHANNEL_RETURN:
		{

		}
		break;

	case NET_MSG_CHAT_MAIL_COUNT_RETURN:
		{
			game_control.chat_message_in = dwParam;
		}
		break;

	case NET_MSG_CHAT_GET_MAIL_RETURN:
		{
			if (pchData)
			{
				MAIL_MSG Channel = {0};
				
				Q_memcpy(&Channel.base, pchData, sizeof(Channel.base));

				char nickname[MAX_NAME_LENGTH+1] = {0};

				Q_strncpy(nickname, pchData+sizeof(Channel.base), Channel.base.byNameLen+1);
				
				Channel.pchNickname = nickname;
				
				char content[1024] = {0};
				
				int size = sizeof(Channel.base)+Channel.base.byNameLen;
				
				Q_strncpy(content, pchData + size, Channel.base.byContentLen+1);
				
				Channel.pchContent = content;

				chat_receive_data.channel=CHAT_CHANNEL_FRIEND;
				chat_receive_data.user_id=Channel.base.nSendid;

				strcpy((char *)chat_receive_data.time,format_time(Channel.base.lTimer));
					
				
				strcpy((char *)chat_receive_data.nickname,Channel.pchNickname);
				strcpy((char *)chat_receive_data.text,Channel.pchContent);

				push_friend_data(chat_receive_data.user_id,system_control.control_user_id,Channel.base.lTimer,chat_receive_data.nickname,chat_receive_data.text);

				if(game_control.chat_message_in>0)
					game_control.chat_message_in--;


//				push_chat_data(CHAT_CHANNEL_WORLD, Channel.base.nSendid,(UCHR *)Channel.pchNickname,(UCHR *)Channel.pchContent);

			}				
		}
		break;

	case NET_MSG_CHAT_SYS_TIME_RETURN:
		{
			system_time = (time_t)dwParam;

			tick_time = Plat_MSTime();
		}
		break;
			
		
	default:
		break;
	}

	chat_net_lock = false;

	return 0;
}


//////////////////////////////////////////////////////////////////////////
//
HRESULT WorldChat(const char *str, int id)
{
	if (!str || !*str)
	{
		return E_FAIL;
	}

	WORLD_CHANNEL Channel = {0};
	
	Channel.base.id = id;
	Channel.base.content_len = strlen(str);
	
	char buf[2048] = {0};
	
	Q_memcpy(buf, &Channel.base, sizeof(Channel.base));
	
	Q_strncpy(buf+sizeof(Channel.base), str, Channel.base.content_len+1);
	
	PostChatNetMessage(NET_MSG_WORLD_CHANNEL, 0, buf, sizeof(Channel.base)+
		Channel.base.content_len);	
	
	return S_OK;
}

HRESULT CurrentChat(const char *str, int id)
{
	if (!str || !*str)
	{
		return E_FAIL;
	}

	char buf[161] = {0};
	
	Q_strncpy(buf, str, sizeof(buf));
	
	PostChatNetMessage(NET_MSG_CURRENT_CHANNEL, id, buf, strlen(buf));
	
	return S_OK;
}

HRESULT PrivateChat(const char* str, int id, int rev_id)
{		
	if (!str || !*str)
	{
		return E_FAIL;
	}
	
	PRIVATE_CHANNEL Channel = {0};
	
	Channel.base.id = id;
	Channel.base.rec_id = rev_id;

	Channel.base.content_len = strlen(str);
	
	char buf[2048] = {0};
	
	Q_memcpy(buf, &Channel.base, sizeof(Channel.base));
	
	Q_strncpy(buf+sizeof(Channel.base), str, Channel.base.content_len+1);
	
	PostChatNetMessage(NET_MSG_PRIVATE_CHANNEL, 0, buf, sizeof(Channel.base)+
		Channel.base.content_len);
	
	return S_OK;
}

HRESULT FriendChat(const char* str, int id, int rev_id)
{
	if (!str || !*str)
	{
		return E_FAIL;
	}
	
	MAIL_MSG Channel = {0};
	
	Channel.base.nSendid = id;
	Channel.base.nRecvid = rev_id;
	
	Channel.base.byContentLen = strlen(str) % 256;
	
	char buf[2048] = {0};
	
	char *buf_tmp = buf;

	Q_memcpy(buf_tmp, &Channel.base, sizeof(Channel.base));
	buf_tmp += sizeof(Channel.base);
	
	Q_strncpy(buf_tmp, str, Channel.base.byContentLen+1);
	buf_tmp += Channel.base.byContentLen;
	
	PostChatNetMessage(NET_MSG_CHAT_SEND_MAIL, 0, buf, buf_tmp-buf);


	push_friend_data(id,rev_id,getServerTime(),(UCHR *)base_character_data.name,(UCHR *)str);
	

	return S_OK;
}

HRESULT TradeChat(const char *str, int id)
{
	if (!str || !*str)
	{
		return E_FAIL;
	}
	
	WORLD_CHANNEL Channel = {0};
	
	Channel.base.id = id;
	Channel.base.content_len = strlen(str);
	
	char buf[2048] = {0};
	
	Q_memcpy(buf, &Channel.base, sizeof(Channel.base));
	
	Q_strncpy(buf+sizeof(Channel.base), str, Channel.base.content_len+1);
	
	PostChatNetMessage(NET_MSG_TRADE_CHANNEL, 0, buf, sizeof(Channel.base)+
		Channel.base.content_len);	
	
	return S_OK;	
}

HRESULT GroupChat(const char *str, int id)
{
	if (!str || !*str)
	{
		return E_FAIL;
	}
	
	char buf[161] = {0};
	
	Q_strncpy(buf, str, sizeof(buf));
	
	PostChatNetMessage(NET_MSG_TEAM_CHANNEL, id, buf, strlen(buf));
		
	return S_OK;	
}
