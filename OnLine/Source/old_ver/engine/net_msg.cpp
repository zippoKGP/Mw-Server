/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2004 ALL RIGHTS RESERVED.

  *file name    : net_msg.cpp
  *owner  		: Andy
  *description  : 
  *modified     : 2005/1/6
******************************************************************************/ 

#include "engine_global.h"
#include "net_msg.h"
#include "login.h"
#include "tier0/dbg.h"
#include "player_info.h"
#include "vari-ext.h"
#include "net.h"
#include "utility.h"
#include "graph.h"
#include "utillib/utillib.h"
#include "chat_msg.h"
#include "chat_info.h"
#include "weather.h"
#include "macro.h"

bool net_lock = false;

int sync_count = 0;

UG_ULONG UG_FUN_CALLBACK ClientNetHandle(UG_DWORD dwMsgID,UG_DWORD dwParam,
										 const UG_PCHAR pchData,UG_ULONG ulLen)
{
	net_lock = true;

	switch (dwMsgID) 
	{
	case NET_MSG_CHECK_USER_RETURN://当用户输入密码进入触发
		{
			switch (dwParam)
			{
			case CHECK_USER_OK:
				{
					extern SLONG account_main_pass;

					account_main_pass = 1;
				}
				break;

			case CHECK_USER_ERROR:
			case CHECK_PWD_NOT:
			case CHECK_USER_NOT:
				display_error_message((UCHR*)MSG_ACCOUND_ERROR, true);
				break;
			}
		}
		break;

	case NET_MSG_PLAYER_CHAT_SERVER_RETURN:
		{
			CHAT_SERVER_INFO *pInfo = (CHAT_SERVER_INFO *)pchData;

			if (pInfo)
			{
				int ir = ConnectChatServer(pInfo->ip, pInfo->port, dwParam, (FnClientNet)ChatNetHandle);

				if (ir == -1)
				{
					display_error_message((UCHR*)MSG_CHAT_SERVER_ERROR, true);

					exit(0);
				}
				else
				{
					PostChatNetMessage(NET_MSG_CHAT_SYS_TIME, 0);

					PostChatNetMessage(NET_MSG_PLAYER_CLIENT_CONFIG, 0, (char*)&client_config, sizeof(client_config));							
					
					PostChatNetMessage(NET_MSG_CHAT_MAIL_COUNT, 0);
				}
			}
		}
		break;

	case NET_MSG_PLAYER_CHARACTER_LIST_RETURN://这个是得到玩家所拥有的角色列表
		{
			BASE_CHARACTER_INFO *pInfo = (BASE_CHARACTER_INFO *)pchData;

			total_base_character=dwParam;

			if(total_base_character>0)
			{
				base_character_info=(BASE_CHARACTER_INFO *)malloc(sizeof(struct _BASE_CHARACTER_INFO)*total_base_character);

				if(base_character_info==NULL)
				{
					display_error_message((UCHR *)MSG_MEMORY_ALLOC_ERROR,1);
					total_base_character=0;

					return (UG_ULONG)-1;
				}

				Q_memcpy(base_character_info, pInfo, sizeof(BASE_CHARACTER_INFO)*total_base_character);
			}
			else if (dwParam == -1)
			{
				display_error_message((UCHR *)"get char list error", 1);
			}
		}
		break;

		//////////////////////////////////////////////////////////////////////////
		//
	case NET_MSG_PLAYER_SELECT_CHARACTER_RETURN://当玩家选择了一个角色
		{
			if (dwParam == SELECT_CHARACTER_OK)
			{
				extern SLONG select_character_return_ok;

				select_character_return_ok = 1;

//				postNetMessage(NET_MSG_PLAYER_ENTER_SCENE, 0);
			}
			else
			{
				display_error_message((UCHR*)"select character return error, exit !", true);
			}
		}
		break;

		//////////////////////////////////////////////////////////////////////////
		//
	case NET_MSG_PLAYER_ADD_PLAYER_RETURN://有其他玩家加入
		{
			if ((dwParam * sizeof(BASE_NPC_INFO)) != ulLen)
			{
				Error("receive new player data error, exit !");
			}

			BASE_NPC_INFO *pInfo = (BASE_NPC_INFO *)pchData;

			for (int i = 0; i < (int)dwParam; i ++)
			{
				add_map_npc(pInfo[i]);
			}
		}
		break;

	case NET_MSG_PLAYER_ENTER_SCENE_RETURN://玩家进入场景返回
		{
			if (dwParam == ENTER_SCENE_ERROR)
			{
				Error("enter scene return error, exit !");
			}
			else
			{
				extern SLONG change_scene_return_ok;

				change_scene_return_ok = 1;
			}
		}
		break;

	case NET_MSG_PLAYER_CREATE_CHARACTER_RETURN:
		{
			if (dwParam == CREATE_CHARACTER_NICKNAME_EXIST)
			{
				display_error_message((UCHR*)MSG_NICKNANE_ERROR, true);
			}
			else if (dwParam == CREATE_CHARACTER_OK)
			{
				extern SLONG create_character_return_ok;
				
				create_character_return_ok = 1;
				
//				postNetMessage(NET_MSG_PLAYER_ENTER_SCENE, 0);
			}
			else
			{
				display_error_message((UCHR*)MSG_CREATE_CHARACTER_ERROR, true);
			}
		}
		break;

	case NET_MSG_PLAYER_MOVE_START_RETURN:
	case NET_MSG_PLAYER_MOVE_END_RETURN://玩家开始移动/结束移动的消息
//	case NET_MSG_PLAYER_MOVE_SYNC_RETURN:
		{
			sync_count ++;
 
			PLAYER_MOVE *pMove = (PLAYER_MOVE*)pchData;

			for (int i = 0; i < (int)dwParam; i ++)
			{
				update_player_move(pMove[i]);
			}
		}
		break;
		
	case NET_MSG_PLAYER_EXIT_SCENE_RETURN://玩家退出场景返回
		{
			int *pID = (int*)pchData;
			
			for (int i = 0; i < (int)dwParam; i ++)
			{
				delete_map_npc_group(pID[i]);
			}
		}
		break;

	case NET_MSG_PLAYER_CHANGE_SCENE_RETURN://玩家改变场景
		{
			if (pchData)
			{
				MAP_BASE_INFO *map_base_info = NULL;
				MAP_ATTRIB_INFO *map_attrib_info = NULL;
				MAP_WEATHER_BASE *map_weather_base = NULL;
				BASE_NPC_INFO *player = NULL;

				if (ulLen == sizeof(MAP_BASE_INFO) + sizeof(MAP_ATTRIB_INFO) + 
					sizeof(MAP_WEATHER_BASE) + sizeof(BASE_NPC_INFO))
				{
					char *buf = pchData;

					map_base_info = (MAP_BASE_INFO *)buf;
					buf += sizeof(*map_base_info);

					map_attrib_info = (MAP_ATTRIB_INFO*)buf;
					buf += sizeof(*map_attrib_info);

					map_weather_base = (MAP_WEATHER_BASE*)buf;
					buf += sizeof(*map_weather_base);

					player = (BASE_NPC_INFO*)buf;
					buf += sizeof(player);
					
					// --- ( 1 - 0 ) 
					convert_change_map_info(*map_base_info, *map_attrib_info, *map_weather_base, *player);

					if (system_task_table[now_execute_task_id].task_id==TASK_GAME_MAIN)
					{
						change_map();
					}

					// --- ( 2 - 0 ) NPC data
					// Clear all map npc
					clear_all_map_npc();
					
					add_map_npc( *player);
					system_control.control_user_id=change_map_info.main_character.id;
					system_control.control_npc_idx=get_map_npc_index(system_control.control_user_id);

					if (system_task_table[now_execute_task_id].task_id==TASK_GAME_MAIN)
					{
						redraw_map(screen_channel0);
						auto_change_screen_effect();					
					}					
				}
				
				postNetMessage(NET_MSG_PLAYER_ENTER_SCENE, 0);				
			}
			else
			{

			}
		}
		break;

	case NET_MSG_PLAYER_NPC_LIST_RETURN://玩家npc的列表
		{
			BASE_NPC_INFO *pInfo = (BASE_NPC_INFO*)pchData;

			for (unsigned int i = 0; i < dwParam; i ++)
			{
				add_map_npc(pInfo[i]);
			}
		}
		break;

	case NET_MSG_PLAYER_EXIST_SCENE_RETURN:
		{
			display_error_message((UCHR*)MSG_EXIST_SCENE_ERROR, true);			
		}
		break;

	case NET_MSG_PLAYER_OTHER_ENTER_RETURN:
		{
			display_error_message((UCHR*)MSG_OTHER_ENTER_SCENE_ERROR, true);
		}
		break;

	case NET_MSG_PLAYER_WEATHER_RETURN://玩家气候信息返回
		{
			if (pchData)
			{
				MAP_WEATHER_BASE *pBase = (MAP_WEATHER_BASE *)pchData;

				update_weather_data(*pBase);
			}
		}
		break;

		//////////////////////////////////////////////////////////////////////////
		// 好友部分消息
	case NET_MSG_PLAYER_FRIEND_LIST_RETURN://返回玩家当前所选角色的好友列表
		{
			clear_chat_friend_list();

			int copy_num = min(dwParam, MAX_CHAT_FRIEND_LIST);

			CHAT_FRIEND_LIST *friend_list = (CHAT_FRIEND_LIST *)pchData;

			Q_memcpy(chat_friend_list, friend_list, sizeof(CHAT_FRIEND_LIST)*copy_num);

			reload_chat_friend_list();
		}
		break;
	case NET_MSG_PLAYER_FRIEND_ADD_RETURN:
		{
			switch (dwParam) 
			{
			case FRIEND_ADD_NOT_ALLOW:
				{
					display_system_message((UCHR*)MSG_ADD_FRIEND_ERROR);	
				}
				break;

			case FRIEND_ADD_ERROR:
				{					
					display_system_message((UCHR*)MSG_CHAT_SEARCH_ERROR);
				}
				break;

			case FRIEND_ADD_OK:
				{
					if (sizeof(CHAT_FRIEND_LIST) == ulLen)
					{
						update_friend_list(*(CHAT_FRIEND_LIST*)pchData);
					}					
				}
				break;

			case FRIEND_ADD_BY_OTHER:
				{
					if (pchData)
					{
						char nickname[MAX_NAME_LENGTH+1] = {0};

						Q_strncpy(nickname, pchData, ulLen+1);

						char text[128] = {0};

						Q_snprintf(text, sizeof(text), MSG_OTHER_ADD_FRIEND, nickname);

						display_system_message((UCHR*)text);
					}
				}
				break;

			default:
				break;				
			}
		}
		break;

	case NET_MSG_PLAYER_FRIEND_SUB_RETURN:
		{
			delete_friend_list(dwParam);
		}
		break;
		
	case NET_MSG_PLAYER_FRIEND_UPDATE_RETURN:
		{
			if (dwParam == FRIEND_UPDATE_OK)
			{
				if (pchData && sizeof(CHAT_FRIEND_LIST) == ulLen)
				{
					refresh_friend_list(*((CHAT_FRIEND_LIST*)pchData));
				}				
			}
		}
		break;
		
	case NET_MSG_PLAYER_FRIEND_ONLINE_RETURN:
		{
			update_friend_list_online(dwParam, 1);
		}
		break;
		
	case NET_MSG_PLAYER_FRIEND_OFFLINE_RETURN:
		{
			update_friend_list_online(dwParam, 0);
		}
		break;

	case NET_MSG_PLAYER_FIND_FRIEND_RETURN:
		{
			if (dwParam == FIND_FRIEND_OK && ulLen == sizeof(chat_search_data))
			{
				Q_memcpy(&chat_search_data, pchData, sizeof(chat_search_data));
			}
			else
			{
				display_system_message((UCHR*)MSG_CHAT_SEARCH_ERROR);
			}
		}
		break;

	case NET_MSG_PLAYER_CLIENT_CONFIG_RETURN:
		{
			postNetMessage(NET_MSG_PLAYER_CLIENT_CONFIG, 0, (char*)&client_config, sizeof(client_config));			
		}
		break;

	case NET_MSG_PLAYER_FRIEND_TYPE_RETURN:
		{
			if (pchData)
			{
				int type = *((int*)pchData);

				update_friend_list_friend_type(dwParam, type);
			}
		}
		break;

		//////////////////////////////////////////////////////////////////////////
		//
	case NET_MSG_PLAYER_CREATE_GROUP_RETURN:
		{
			if (dwParam == CREATE_GROUP_OK && pchData)
			{
				int id = *((int*)pchData);
				
				int idx = get_map_npc_index(id);
				
				if (idx >= 0)
				{
					map_npc_group[idx].npc_info.status |= NPC_STATUS_LEADER;

					clear_team_list_data();
					clear_team_request_list_data();
					request_team_list_data();			
					
				}
			}
			else
			{
//				display_system_message((UCHR *)"NET_MSG_PLAYER_CREATE_GROUP_RETURN ERROR");
			}
		}
		break;

	case NET_MSG_PLAYER_ADD_TO_GROUP_RETURN:
		{
			if (dwParam == ADD_TO_GROUP_OK && pchData)
			{
				int id = *((int*)pchData);

				int idx = get_map_npc_index(id);

				if (idx >= 0)
				{
					map_npc_group[idx].npc_info.status |= NPC_STATUS_FOLLOW;
					
				}
			}
			else
			{
//				display_system_message((UCHR *)"NET_MSG_PLAYER_ADD_TO_GROUP_RETURN ERROR");
			}
		}
		break;

	case NET_MSG_PLAYER_GROUP_NEW_PLAYER_RETURN:
		{
 			if((LONG)dwParam>=0)
			{
				if(dwParam==system_control.control_user_id)		// 我被加入了
				{
					clear_team_list_data();
					request_team_list_data();
				}
				else											// 其他玩家加入我的队伍
				{
					insert_team_list_data(dwParam, (BASE_CHARACTER_INFO*)pchData);
					delete_team_request_list_data(dwParam);	
				}

				int idx = get_map_npc_index(dwParam);
				
				if (idx >= 0)
				{
					map_npc_group[idx].npc_info.status |= NPC_STATUS_FOLLOW;						
				}					
				
				sprintf((char *)print_rec,MSG_ADD_TEAM_READY,map_npc_group[idx].npc_info.name);
				push_chat_data(CHAT_CHANNEL_MESSGAE,0,(UCHR *)"",print_rec);
			}			
		}
		break;

	case NET_MSG_PLAYER_EXIT_GROUP_RETURN:
		{
			if (dwParam == system_control.control_user_id)
			{
				if(map_npc_group[system_control.control_npc_idx].npc_info.status&NPC_STATUS_LEADER)
					push_chat_data(CHAT_CHANNEL_MESSGAE,0,(UCHR *)"",(UCHR *)MSG_BREAK_TEAM);

				if(map_npc_group[system_control.control_npc_idx].npc_info.status&NPC_STATUS_FOLLOW)
				{
					sprintf((char *)print_rec,MSG_LEAVE_TEAM,map_npc_group[system_control.control_npc_idx].npc_info.name);
					push_chat_data(CHAT_CHANNEL_MESSGAE,0,(UCHR *)"",print_rec);
				}
				
				map_npc_group[system_control.control_npc_idx].npc_info.status &= ~(NPC_STATUS_FOLLOW|NPC_STATUS_LEADER);
				clear_team_list_data();
				
				
			}
			else
			{
				int idx = get_map_npc_index(dwParam);

				if (idx >= 0)
				{
					map_npc_group[idx].npc_info.status &= ~(NPC_STATUS_FOLLOW|NPC_STATUS_LEADER);

					delete_team_list_data(dwParam);
						
				}
			}

		}
		break;

	case NET_MSG_PLAYER_ADD_GROUP_LIST_RETURN:
		{
			if (dwParam == ADD_GROUP_LIST_NEW)
			{
				if (pchData)
				{
					insert_team_request_list_data((BASE_CHARACTER_INFO*)pchData);
					if(game_control.window_team_request<0)
					{
						game_control.team_message_in=1;
					}
					else
					{
						team_request_select_index=-1;
						team_request_select_start_index=0;
					}
				}				
			}
			else if (dwParam == ADD_GROUP_LIST_OK)
			{
				display_system_message((UCHR *)MSG_ADD_TEAM_OK);
			}
			else if (dwParam == ADD_GROUP_LIST_NOT_HEADER)
			{
				display_system_message((UCHR *)MSG_ADD_TEAM_NOT_LEADER_ERROR);
			}
			else if (dwParam == ADD_GROUP_LIST_ERROR)
			{
				display_system_message((UCHR *)MSG_ADD_TEAM_FULL);
			}
			else if (dwParam == ADD_GROUP_LIST_EXIST)
			{
				display_system_message((UCHR *)MSG_ALREADY_IN_TEAM);				
			}				 
		}
		break;

	case NET_MSG_PLAYER_SUB_GROUP_LIST_RETURN:
		{
			if (dwParam == SUB_GROUP_LIST_OK)
			{
				int id = *((int*)pchData);

				delete_team_request_list_data(id);
			}
			else if (dwParam == SUB_GROUP_LIST_ERROR)
			{
				// 不是队长
			}
			else if (dwParam == SUB_GROUP_LIST_NOT_EXIST)
			{
				// 不在列表中
			}
		}
		break;

	case NET_MSG_PLAYER_GET_REQUEST_LIST_RETURN:
		{
			if (dwParam > 0 && sizeof(BASE_CHARACTER_INFO)*dwParam == ulLen)
			{
				clear_team_request_list_data();
				
				Q_memcpy(team_request_list_data, pchData, ulLen);
			}
		}
		break;

	case NET_MSG_PLAYER_GET_GROUP_LIST_RETURN:
		{
			if (dwParam > 0 && dwParam < 6 && sizeof(BASE_CHARACTER_INFO)*dwParam == ulLen)
			{
				clear_team_list_data();

				Q_memcpy(team_list_data, pchData, ulLen);
			}
		}
		break;

	case NET_MSG_PLAYER_CLEAR_REQUEST_LIST:
		{
			if (dwParam == CLEAR_REQUEST_LIST_ERROR)
			{

			}
			else if (dwParam == CLEAR_REQUEST_LIST_OK)
			{
				clear_team_request_list_data();
			}
		}
		break;

		//////////////////////////////////////////////////////////////////////////
		//
	case NET_MSG_PLAYER_MACRO_CMD_RETURN:
		{
//			if (pchData)
			{
				exec_macro(dwParam, (UCHR*)pchData, ulLen);
			}
		}
		break;

	case NET_MSG_PLAYER_TASK_LIST_RETURN:
		{
			clear_mission_data();

			MISSION_DATA data;

			MACRO_BASE_INFO Info = {0};
			
			int text_off = dwParam * sizeof(Info.base);

			clear_mission_data();
			
			for (DWORD i = 0; i < dwParam && pchData; i ++)
			{
				Q_memcpy(&Info.base, pchData+i*sizeof(Info.base), sizeof(Info.base));

				Info.text = text_off + pchData;

				data.mission_id = Info.base.type;

				Q_strncpy((char*)data.text, Info.text, sizeof(data.text));

				insert_mission_data(data);

				text_off += Info.base.len;
			}

			if( dwParam == 0 )
			{
				data.mission_id=0;
				strcpy((char *)data.text,MSG_NO_MISSION);
				insert_mission_data(data);
			}

		}
		break;

	case NET_MSG_PLAYER_DATA_CHAR_INFO_RETURN://应该是玩家资料数据返回
		{
			if (dwParam == 0 && pchData && ulLen == sizeof(CLIENT_CHARACTER_MAP_WINDOW))
			{
				Q_memcpy(&base_character_data.data, pchData, sizeof(base_character_data.data));

				base_character_data.active = true;
			}
		}
		break;

	case NET_MSG_PLAYER_DATA_POINT_BACK_RETURN:
		{
			if (dwParam == POINT_BACK_OK && ulLen == sizeof(base_character_data.data.final))
			{
				Q_memcpy(&base_character_data.data.final, pchData, sizeof(base_character_data.data.final));

				base_character_data.data.base.point += character_point_back.total_point;
				
				base_character_data.data.base.hp_point += character_point_back.hp_point;
				base_character_data.data.base.mp_point += character_point_back.mp_point;
				base_character_data.data.base.att_point += character_point_back.att_point;
				base_character_data.data.base.speed_point += character_point_back.speed_point;
			}
			else
			{
				display_system_message((UCHR *)MSG_POINT_ERROR);
			}

			Q_memset(&character_point_back, 0, sizeof(character_point_back));

			base_character_data.active=true;
		}
		break;

	case NET_MSG_PLAYER_DATA_LEVLE_UP_RETURN:
		{
			bool level_up;

			if (ulLen == sizeof(SERVER_CHATACTER_BASE)+sizeof(SERVER_CHARACTER_FINAL) && pchData)
			{
				int size = sizeof(base_character_data.data.base);

				if(((SERVER_CHARACTER_FINAL*)(pchData+size))->total_exp>=base_character_data.data.final.total_exp)
					sprintf((char *)print_rec,MSG_ADD_EXP,((SERVER_CHARACTER_FINAL*)(pchData+size))->total_exp-base_character_data.data.final.total_exp);
				else
					sprintf((char *)print_rec,MSG_SUB_EXP,abs(base_character_data.data.final.total_exp-((SERVER_CHARACTER_FINAL*)(pchData+size))->total_exp));
				
				display_system_message(print_rec);					

				if(((SERVER_CHARACTER_FINAL*)(pchData+size))->level>=base_character_data.data.final.level)
					level_up=true;
				else
					level_up=false;
					

				Q_memcpy(&base_character_data.data.base, pchData, size);

				Q_memcpy(&base_character_data.data.final, pchData+size, sizeof(base_character_data.data.final));
		
				if(level_up)
					sprintf((char *)print_rec,MSG_LEVEL_UP,base_character_data.data.final.level);
				else
					sprintf((char *)print_rec,MSG_LEVEL_DOWN,base_character_data.data.final.level);
				display_system_message(print_rec);	
				
				map_npc_group[system_control.control_npc_idx].npc_info.status|=NPC_STATUS_BORN;
				
			}
		}
		break;

	case NET_MSG_PLAYER_DATA_EXP_UP_RETURN:
		{
			if (ulLen == sizeof(CLIENT_UPDATE_EXP) && pchData)
			{
				CLIENT_UPDATE_EXP *pExp = (CLIENT_UPDATE_EXP*)pchData;

				if(pExp->total_exp>=base_character_data.data.final.total_exp)
					sprintf((char *)print_rec,MSG_ADD_EXP,pExp->total_exp-base_character_data.data.final.total_exp);
				else
					sprintf((char *)print_rec,MSG_SUB_EXP,base_character_data.data.final.total_exp-pExp->total_exp);
				
				display_system_message(print_rec);					

				base_character_data.data.final.total_exp = pExp->total_exp;
				base_character_data.data.final.now_exp   = pExp->now_exp;
				base_character_data.data.final.need_exp  = pExp->need_exp;
			}
		}
		break;

	case NET_MSG_PLAYER_BAOBAO_LIST_RETURN://返回玩家宝宝的列表 ---- 关心的数据
		{
			for (DWORD i = 0; i < dwParam; i ++)
			{
				insert_baobao_data(((SERVER_CHARACTER_BAOBAO*)pchData)+i);
			}

			base_baobao_data.active=true;
		}
		break;

	case NET_MSG_PLAYER_BAOBAO_POINT_BACK_RETURN:
		{
			int idx = get_baobao_data_idx(dwParam);
			
			if (ulLen == sizeof(SERVER_BAOBAO_FINAL))
			{
				if (idx >= 0)
				{
					SERVER_BAOBAO_FINAL& final = base_baobao_data.data.baobao_list[idx].data;
					
					Q_memcpy(&final, pchData, sizeof(final));					

/*					base.point += baobao_point_back[idx].total_point;
					
					base.hp_point += baobao_point_back[idx].hp_point;
					base.mp_point += baobao_point_back[idx].mp_point;
					base.att_point += baobao_point_back[idx].att_point;
					base.speed_point += baobao_point_back[idx].speed_point;
*/				}
			}
			else
			{
				if (idx >= 0)
				{
					SERVER_BAOBAO_BASE& base = base_baobao_data.data.baobao_list[idx].base;
					
					base.point -= baobao_point_back[idx].total_point;
					
					base.hp_point -= baobao_point_back[idx].hp_point;
					base.mp_point -= baobao_point_back[idx].mp_point;
					base.att_point -= baobao_point_back[idx].att_point;
					base.speed_point -= baobao_point_back[idx].speed_point;					
				}

				display_system_message((UCHR *)MSG_POINT_ERROR);
			}
			
			if (idx >= 0)
			{
				Q_memset(baobao_point_back+idx, 0, sizeof(*baobao_point_back));				
			}
			
			base_character_data.active=true;			
		}
		break;

	case NET_MSG_PLAYER_BAOBAO_FOLLOW_RETURN://玩家宝宝跟随返回 ------------------- 关心的数据
		{
			if (dwParam == system_control.control_user_id)
			{
				int id = *(int*)pchData;

				int idx=get_baobao_data_idx(id);
				
				if(idx>=0)
					base_baobao_data.data.show[idx]=true;				

				CLIENT_BAOBAO_SHOW Show;
				
				Show.id = id;
				Show.follow_id = system_control.control_user_id;
				Show.filename_id = base_baobao_data.data.baobao_list[idx].base.filename_id;
				Show.life_base = base_baobao_data.data.baobao_list[idx].base.life_base;
				Q_strncpy((char *)Show.name, base_baobao_data.data.baobao_list[idx].name, sizeof(Show.name));			// 名称(玩家自定义名字)	
				
				add_baobao_show(Show);
			}
		
			if (dwParam == -1)
			{
				display_system_message((UCHR*)MSG_BAOBAO_FOLLOW_ERROR);
			}
			else if (dwParam != system_control.control_user_id && pchData)
			{
				add_baobao_show(*(CLIENT_BAOBAO_SHOW*)pchData);

				int idx=get_baobao_data_idx(((CLIENT_BAOBAO_SHOW*)pchData)->id);
				
				if(idx>=0)
					base_baobao_data.data.show[idx]=true;				
			}
		}
		break;

	case NET_MSG_PLAYER_BAOBAO_CLOSE_RETURN:
		{
			int idx=get_baobao_data_idx(dwParam);

			if(idx>=0)
			{
				base_baobao_data.data.show[idx]=false;	

			}
			delete_map_npc_group(dwParam);
		}
		break;

	case NET_MSG_PLAYER_BAOBAO_NAME_RETURN:
		{
			int id = *(int*)pchData;

			if (dwParam == 0) // ok
			{
				char chat_str[1024] = {0};
				
				g_PetNameEdit.getText(chat_str);	
				
				chat_str[MAX_BAOBAO_NAME_LENGTH] = 0;

				int idx = get_baobao_data_idx(id);

				if (idx >= 0)
				{
					Q_strncpy(base_baobao_data.data.baobao_list[idx].name, chat_str, MAX_BAOBAO_NAME_LENGTH+1);
					change_map_npc_name(id,	chat_str);
				}
			}
			else if (dwParam == -1)
			{
				int idx = get_baobao_data_idx(id);
				
				if (idx >= 0)
				{
					g_PetNameEdit.Erase();
					g_PetNameEdit.setText(base_baobao_data.data.baobao_list[idx].name);
				}				
			}
		}
	case NET_MSG_PLAYER_BAOBAO_ACTION_ON_RETURN:
		{
			if (dwParam > 0)
			{
				base_baobao_data.data.action_baobao_id = dwParam;

				base_baobao_data.data.active_idx = get_baobao_data_idx(dwParam);
			}
		}
		break;

	case NET_MSG_PLAYER_BAOBAO_ACTION_OFF_RETURN:
		{
			if (dwParam == base_baobao_data.data.action_baobao_id)
			{
				base_baobao_data.data.active_idx=-1;
				base_baobao_data.data.action_baobao_id=0;				
			}
		}
		break;

	case NET_MSG_PLAYER_BAOBAO_ADD_RETURN:
		{
			if (dwParam == 1 && ulLen == sizeof(SERVER_CHARACTER_BAOBAO)+sizeof(PUBLIC_FASTNESS_DATA))
			{
				insert_baobao_data((SERVER_CHARACTER_BAOBAO*)pchData);

				base_baobao_data.active=true;

				int idx = get_baobao_data_idx(((SERVER_CHARACTER_BAOBAO*)pchData)->baobao_id);

				if (idx >= 0)
				{
					memcpy(&baobao_fastness_data[idx].fastness, pchData+sizeof(SERVER_CHARACTER_BAOBAO), sizeof(PUBLIC_FASTNESS_DATA));

					baobao_fastness_data[idx].active = true;
				}
			}
			else
			{
				display_error_message((UCHR*)MSG_ADD_BAOBAO_ERROR, 1);	
			}
		}
		break;
		
	case NET_MSG_PLAYER_BAOBAO_REMOVE_RETURN:
		{
			if (dwParam == S_OK)
			{
				delete_baobao_data(*(int*)pchData);
			}
			else
			{
				display_error_message((UCHR*)MSG_DELETE_BAOBAO_ERROR, 1);
			}
		}
		break;

	case NET_MSG_PLAYER_ITEM_LIST_RETURN://当玩家点了物品拦时
		{
			ITEM_DATA_EX *pIde = (ITEM_DATA_EX*)pchData;
			
			for (DWORD i = 0; i < dwParam; i ++)
			{
				int idx = pIde[i].idx;
				
				character_pocket_data.item[idx] = pIde[i].imd;
			}
			
			character_pocket_data.active = true;
		}
		break;

	case NET_MSG_PLAYER_ITEM_ADD_RETURN:
		{
			ITEM_DATA_EX *pIde = (ITEM_DATA_EX*)pchData;

			for (DWORD i = 0; i < dwParam; i ++)
			{
				int idx = pIde[i].idx;

				if (idx < (1 << 24))
				{
					character_pocket_data.item[idx] = pIde[i].imd;					
				}
				else
				{
					idx &= 0x00ffffff;

					character_popshop_data.item[idx] = pIde[i].imd;										

//					character_popshop_data.active = true;
				}
			}

//			character_pocket_data.active = true;
		}
		break;
		
	case NET_MSG_PLAYER_ITEM_REMOVE_RETURN:
		{
			delete_character_item_pocket(dwParam,999);			
		}
		break;
		
	case NET_MSG_PLAYER_ITEM_CHANGE_RETURN:
		{
			if (dwParam != -1)
			{
				ITEM_DATA_EX *pIde = (ITEM_DATA_EX*)pchData;
				
				for (DWORD i = 0; i < dwParam; i ++)
				{
					int idx = pIde[i].idx;

					if (idx < (1 << 24))
					{
						character_pocket_data.item[idx] = pIde[i].imd;						
					}
					else
					{
						idx &= 0x00ffffff;

						character_popshop_data.item[idx] = pIde[i].imd;
					}
				}
			}
		}
		break;

	case NET_MSG_PLAYER_ITEM_USE_RETURN:
		{
			delete_character_item_pocket(dwParam,1);
		}
		break;

	case NET_MSG_PLAYER_ITEM_MONEY_RETURN://返回玩家的金钱数量
		{
			if (pchData)
			{
				SLONG *money_list = (SLONG*)pchData;

				character_bank_data.now_money = money_list[0];
				character_bank_data.bank_money = money_list[1];
				character_bank_data.max_money = money_list[2];

				character_bank_data.active = true;
			}
		}
		break;

	case NET_MSG_PLAYER_ITEM_INFO_RETURN:
		{
			if (dwParam == 0)
			{
				detail_item_inst.active=true;

				strcpy(detail_item_inst.inst,(char *)pchData);
			}
			else
			{
				display_error_message((UCHR*)MSG_ITEM_DETAIL_ERROR, 1);
			}
		}
		break;

	case NET_MSG_PLAYER_STORE_ITEM_LIST_RETURN:
		{
			if (pchData)
			{
				PUBLIC_ITEM_DATA *pData = (PUBLIC_ITEM_DATA *)pchData;

				for (unsigned int i = 0; i < dwParam; i ++)
				{
					store_data.item[i] = pData[i];
				}

				store_data.active = true;
			}
		}
		break;

		//////////////////////////////////////////////////////////////////////////
		//
	case NET_MSG_PLAYER_ITEM_GIVE_RETURN:
		{
			if (dwParam == 0)
			{
				if (pchData && ulLen == sizeof(deal_item_t))
				{
					deal_item_t *deal_item = (deal_item_t*)pchData;

					for (int i = 0; i < 3; i ++)
					{
						int idx = deal_item->item_idx[i];

						if (deal_item->item_num[i] == 0 && idx != -1)
						{
							character_pocket_data.item[idx].item_id = -1;
							character_pocket_data.item[idx].filename_id = -1;
							character_pocket_data.item[idx].number = 0;
						}
						else
						{
							character_pocket_data.item[idx].number = deal_item->item_num[i];
						}
					}

					character_bank_data.now_money = deal_item->money;
				}
			}
			else
			{
				display_error_message((UCHR*)MSG_GIVE_ITEM_ERROR, 1);
			}
		}
		break;

	case NET_MSG_PLAYER_DEAL_SEND_REQUEST_RETURN://请求交易的消息
		{
			if (dwParam == 0)
			{
				open_window_deal();				
			}
			else if (dwParam == -1)
			{
				display_error_message((UCHR*)MSG_REQUEST_DEAL_ERROR, 1);				
			}
		}
		break;
		
	case NET_MSG_PLAYER_DEAL_RECV_REQUEST_RETURN: // 收到交易开始请求
		{
			game_control.game_deal_id = dwParam;

			open_window_deal();
		}
		break;

	case NET_MSG_PLAYER_DEAL_CANCLE_RETURN:
		{
			if (dwParam == -1)
			{
				display_error_message((UCHR*)MSG_DEAL_CANCEL_ERROR, 1);
			}
			else
			{
				close_window_deal();		
				
				game_control.window_deal=-1;
				game_control.game_deal_id=0;				
			}
		}
		break;

	case NET_MSG_PLAYER_DEAL_ITEM_LIST_RETURN: 
		{
			character_pocket_data.my_deal_ready = true;
		}
		break;

	case NET_MSG_PLAYER_DEAL_REVC_ITEM_RETURN: // 收到对方的物品列表
		{
			if (dwParam == 0 && ulLen == sizeof(deal_item_t) && pchData)
			{
				deal_item_t *pItem = (deal_item_t *)pchData;

				memcpy(character_pocket_data.other_deal_filename_id, pItem->item_idx, sizeof(character_pocket_data.other_deal_filename_id));
				memcpy(character_pocket_data.other_deal_number, pItem->item_num, sizeof(character_pocket_data.other_deal_number));

				character_pocket_data.other_deal_money = pItem->money;

				character_pocket_data.other_deal_ready = true;

				// character_pocket_data.my_deal_idx[3];
				// character_pocket_data.my_deal_number[3];
				// character_pocket_data.my_deal_money;
				
			}
		}
		break;

	case NET_MSG_PLAYER_DEAL_RESET_ITEM_RETURN:
		{
			if (dwParam == game_control.game_deal_id) // 对方重新设置的
			{				
				Q_memset(character_pocket_data.other_deal_filename_id, 0, sizeof(character_pocket_data.other_deal_filename_id));
			
				Q_memset(character_pocket_data.other_deal_number, 0, sizeof(character_pocket_data.other_deal_number));
				
				character_pocket_data.other_deal_money = 0;

				character_pocket_data.other_deal_ready = false;
			}
			else if (dwParam == system_control.control_user_id) // 自己重新设置的
			{
				Q_memset(character_pocket_data.my_deal_idx, 0, sizeof(character_pocket_data.other_deal_filename_id));
				
				Q_memset(character_pocket_data.my_deal_number, 0, sizeof(character_pocket_data.other_deal_number));
				
				character_pocket_data.other_deal_money = 0;

				character_pocket_data.my_deal_ready = false;
			}
			else
			{
				
			}
		}
		break;

	case NET_MSG_PLAYER_EQUIPE_CHANGE_RETURN:
		{
			if (dwParam == 1)
			{
				ITEM_DATA_EX *pIde = (ITEM_DATA_EX*)pchData;
				
				for (DWORD i = 0; i < dwParam; i ++)
				{
					int idx = pIde[i].idx;

					character_equip_data.equip[idx] = pIde[i].imd;
				}
			}
			else
			{
				display_error_message((UCHR*)MSG_USE_EQUIP_ERROR, 1);
			}
		}
		break;

	case NET_MSG_PLAYER_EQUIPE_LIST_RETURN://玩家装备列表返回
		{
			ITEM_DATA_EX *pIde = (ITEM_DATA_EX*)pchData;
			
			for (DWORD i = 0; i < dwParam; i ++)
			{
				int idx = pIde[i].idx;
				
				character_equip_data.equip[idx] = pIde[i].imd;
			}
			
			character_equip_data.active = true;			
		}
		break;
		
		//////////////////////////////////////////////////////////////////////////
		//
	case NET_MSG_PLAYER_DEAL_OK_RETURN:
		{
			close_window_deal();
		}
		break;

	case NET_MSG_PLAYER_FASTNESS_RETURN:
		{
			if (pchData && ulLen == sizeof(character_fastness_data.fastness))
			{
				memcpy(&character_fastness_data.fastness, pchData, ulLen);

				character_fastness_data.active = true;
			}
		}
		break;
		
	case NET_MSG_PLAYER_BAOBAO_FASTNESS_RETURN:
		{
			if (dwParam == (DWORD)total_baobao_stack && pchData)
			{
				PUBLIC_FASTNESS_DATA *fastness = (PUBLIC_FASTNESS_DATA *)pchData;

				for (DWORD i = 0; i < dwParam; i ++)
				{
					memcpy(&baobao_fastness_data[i].fastness, fastness+i, sizeof(*fastness));
					
					baobao_fastness_data[i].active = true;
				}
			}
		}
		break;			

	case NET_MSG_PLAYER_STORE_BUY:
		{
			if (dwParam == -1)
			{
				display_error_message((UCHR*)MSG_BUY_ITEM_ERROR, 1);
			}
		}
		break;

	case NET_MSG_PLAYER_BANK_SAVE_RETURN:
		{
			if (dwParam == -1)
			{
				display_error_message((UCHR*)MSG_SAVE_MONEY_ERROR, 1);
			}
		}
		break;

	case NET_MSG_PLAYER_BANK_GIVE_RETURN:
		{
			if (dwParam == -1)
			{
				display_error_message((UCHR*)MSG_WITHDRAW_ERROR, 1);
			}			
		}
		break;

	case NET_MSG_PLAYER_POPSHOP_ITEM_LIST_RETURN:
		{
			if (pchData)
			{
				ITEM_DATA_EX *item_data = (ITEM_DATA_EX *)pchData;

				for (uint i = 0; i < dwParam; i ++)
				{
					int idx = item_data[i].idx;

					memcpy(character_popshop_data.item+idx, &item_data[i].imd, sizeof(item_data[i].imd));
				}
			}

			character_popshop_data.active = true;
		}
		break;

	case NET_MSG_PLAYER_POPSHOP_ITEM_PUSH_RETURN:
		{
			if (dwParam == -1)
			{
				display_error_message((UCHR*)MSG_PUSH_POPSHOP_ERROR, 1);
			}			
		}
		break;
		
	case NET_MSG_PLAYER_POPSHOP_ITEM_POP_RETURN:
		{
			if (dwParam == -1)
			{
				display_error_message((UCHR*)MSG_POP_POPSHOP_ERROR, 1);
			}
		}
		break;
		
		//////////////////////////////////////////////////////////////////////////
		//
	case NET_MSG_PLAYER_SKILL_LIST_RETURN://返回玩家所选择角色的技能信息
		{
			if (pchData)
			{
				for (unsigned int i = 0; i < dwParam; i ++)
				{
					character_skill_data[i] = *(((PUBLIC_SKILL_DATA*)pchData)+i);
				}
				
				game_control.character_skill_ready =true;				
			}
		}
		break;

	case NET_MSG_PLAYER_ADD_SKILL_RETURN:
		{
			if (pchData)
			{
				if (dwParam >= 0 && dwParam < MAX_CHARACTER_SKILL_NO)
				{
					character_skill_data[dwParam] = *(PUBLIC_SKILL_DATA*)pchData;
				}
			}
		}
		break;

	case NET_MSG_PLAYER_PET_SKILL_LIST_RETURN:
		{
			if (pchData)
			{
				for (unsigned int i = 0; i < dwParam; i ++)
				{
					baobao_skill_data[i].skill_id = *((int*)pchData+i);
				}
				
				game_control.baobao_skill_ready =true;				
			}
		}
		break;

	case NET_MSG_PLAYER_PET_ADD_SKILL_RETURN:
		{
			if (pchData)
			{
				baobao_skill_data[dwParam].skill_id = *(int*)pchData;
			}
		}
		break;

		//////////////////////////////////////////////////////////////////////////
		// 开始战斗
	case NET_MSG_PLAYER_START_FIGHT_RETURN:
		{
			if (pchData && dwParam > 0)
			{
				clear_fight_npc_group();

				FIGHT_BASE_DATA_EX *pData = (FIGHT_BASE_DATA_EX*)pchData;

				for (unsigned int i = 0; i < dwParam; i ++)
				{
					insert_fight_unit(pData[i].data, pData[i].idx);
				}

				change_to_fight();
			}
		}
		break;

	case NET_MSG_PLAYER_FIGHT_CLIENT_READY_RETURN:
		{

		}
		break;
		
	case NET_MSG_PLAYER_FIGHT_CHARGE_START_RETURN: // 服务器通知充电开始
		{
			fight_charge_start();
		}
		break;

	case NET_MSG_PLAYER_FIGHT_CHARGE_PAUSH_RETURN: // 服务器通知充电暂停
		{

		}
		break;

	case NET_MSG_PLAYER_FIGHT_STATUS_RESULT_RETURN:
		{
//			update_fight_status_result();
		}
		break;
		
	case NET_MSG_PLAYER_FIGHT_COMMAND_RETURN:
		{

		}
		break;
		
	case NET_MSG_PLAYER_FIGHT_RESULT_RETURN:
		{

		}
		break;
		
	case NET_MSG_PLAYER_FIGHT_PLAY_FINISH_RETURN:
		{

		}
		break;
		
	case NET_MSG_PLAYER_FIGHT_END_RETURN:
		{
			change_to_map();
		}
		break;

	default:
		break;
	}

//	writeDebugFile("end net handle\n\n");

	net_lock = false;

	return 0;
}