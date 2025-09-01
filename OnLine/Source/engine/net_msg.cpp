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
#include "data_proc.h"

bool net_lock = false;

int sync_count = 0;

UG_ULONG UG_FUN_CALLBACK ClientNetHandle(UG_DWORD dwMsgID,UG_DWORD dwParam,
										 const UG_PCHAR pchData,UG_ULONG ulLen)
{
 
	net_lock = true;
//	UCHAR s[13]="调试信息收到";
//	UCHAR t[20];
//	itoa(dwMsgID,(char *)t,16);
//	push_chat_data(CHAT_CHANNEL_SCREEN,0,s,t);

	switch (dwMsgID) 
	{
	case NET_MSG_CHECK_USER_RETURN:
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

	case NET_MSG_PLAYER_CHARACTER_LIST_RETURN:
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
	case NET_MSG_PLAYER_SELECT_CHARACTER_RETURN:
		{
			if (dwParam == SELECT_CHARACTER_OK)
			{
				extern SLONG select_character_return_ok;

				select_character_return_ok = 1;

//				if (base_character_info)
//				{
//					free(base_character_info);
//
//					base_character_info = NULL;
//				}

//				postNetMessage(NET_MSG_PLAYER_ENTER_SCENE, 0);
			}
			else
			{
				display_error_message((UCHR*)"select character return error, exit !", true);
			}
		}
		break;

	case NET_MSG_PLAYER_ADD_PLAYER_RETURN:
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

	case NET_MSG_PLAYER_ENTER_SCENE_RETURN:
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
	case NET_MSG_PLAYER_MOVE_END_RETURN:
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
		
	case NET_MSG_PLAYER_EXIT_SCENE_RETURN:
		{
			int *pID = (int*)pchData;
			
			for (int i = 0; i < (int)dwParam; i ++)
			{
				delete_map_npc_group(pID[i]);
			}
		}
		break;

	case NET_MSG_PLAYER_BASE_SCENE_INFO_RETURN:
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
					
					add_map_npc(*player);
					system_control.control_user_id=change_map_info.main_character.id;
					system_control.control_npc_idx=get_map_npc_index(system_control.control_user_id);

					if (system_task_table[now_execute_task_id].task_id==TASK_GAME_MAIN)
					{
						if(game_control.main_loop_mode==MAIN_LOOP_MAP_MODE)
						{
							redraw_map(screen_channel0);
							auto_change_screen_effect();
						}
					}
				}
			}
			else
			{

			}
		}
		break;

	case NET_MSG_PLAYER_CHANGE_SCENE_RETURN:
		{
			if (dwParam == 0)
			{
				postNetMessage(NET_MSG_PLAYER_ENTER_SCENE, 0);
			}
		}
		break;

	case NET_MSG_PLAYER_NPC_LIST_RETURN:
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

	case NET_MSG_PLAYER_WEATHER_RETURN:
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
	case NET_MSG_PLAYER_FRIEND_LIST_RETURN:
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
				
				if ((idx >= 0)&&(idx<total_map_npc))
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

				if ((idx >= 0)&&(idx<total_map_npc))
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
				
				if ((idx >= 0)&&(idx<total_map_npc))
				{
					map_npc_group[idx].npc_info.status |= NPC_STATUS_FOLLOW;						
					sprintf((char *)print_rec,MSG_ADD_TEAM_READY,map_npc_group[idx].npc_info.name);
					push_chat_data(CHAT_CHANNEL_MESSAGE,0,(UCHR *)"",print_rec);
				}					
				
			}			
		}
		break;

	case NET_MSG_PLAYER_EXIT_GROUP_RETURN:
		{
			if (dwParam == system_control.control_user_id)
			{
				int idx = get_map_npc_index(dwParam);
				
				if ((idx >= 0)&&(idx<total_map_npc))
				{
					if(map_npc_group[system_control.control_npc_idx].npc_info.status&NPC_STATUS_LEADER)
						push_chat_data(CHAT_CHANNEL_MESSAGE,0,(UCHR *)"",(UCHR *)MSG_BREAK_TEAM);
						
					if(map_npc_group[system_control.control_npc_idx].npc_info.status&NPC_STATUS_FOLLOW)
					{
						sprintf((char *)print_rec,MSG_LEAVE_TEAM,map_npc_group[system_control.control_npc_idx].npc_info.name);
						push_chat_data(CHAT_CHANNEL_MESSAGE,0,(UCHR *)"",print_rec);
					}
						
					map_npc_group[system_control.control_npc_idx].npc_info.status &= ~(NPC_STATUS_FOLLOW|NPC_STATUS_LEADER);
				}

				clear_team_list_data();				
			}
			else
			{
				int idx = get_map_npc_index(dwParam);

				if ((idx >= 0)&&(idx<total_map_npc))
				{
					map_npc_group[idx].npc_info.status &= ~(NPC_STATUS_FOLLOW|NPC_STATUS_LEADER);
				}
				
				delete_team_list_data(dwParam);
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

	case NET_MSG_PLAYER_DATA_CHAR_INFO_RETURN:
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

			if (ulLen == sizeof(net_level_up_t) && pchData)
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

				log_error(1, "exp up = %d, %d\n", base_character_data.data.final.now_hp, base_character_data.data.final.max_hp);
				
				
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

	case NET_MSG_PLAYER_BAOBAO_LIST_RETURN:
		{
			for (DWORD i = 0; i < dwParam; i ++)
			{
				insert_baobao_data(((SERVER_CHARACTER_BAOBAO*)pchData)+i);
			}


			base_baobao_data.active=true;	
			check_baobao_stack();
			if(base_baobao_data.data.active_idx>=0)
				baobao_stack_index=base_baobao_data.data.active_idx;
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
					
					SERVER_BAOBAO_BASE& base = base_baobao_data.data.baobao_list[idx].base;

					Q_memcpy(&final, pchData, sizeof(final));					

					base.point += baobao_point_back[idx].total_point;
					
					base.hp_point += baobao_point_back[idx].hp_point;
					base.mp_point += baobao_point_back[idx].mp_point;
					base.att_point += baobao_point_back[idx].att_point;
					base.speed_point += baobao_point_back[idx].speed_point;
				}
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

	case NET_MSG_PLAYER_BAOBAO_FOLLOW_RETURN:
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
		break;

	case NET_MSG_PLAYER_BAOBAO_ACTION_ON_RETURN:
		{
			if (dwParam > 0)
			{
				base_baobao_data.data.action_baobao_id = dwParam;

				base_baobao_data.data.active_idx = get_baobao_data_idx(dwParam);

				check_baobao_stack();
				if(base_baobao_data.data.active_idx>=0)
					baobao_stack_index=base_baobao_data.data.active_idx;
				
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

	case NET_MSG_PLAYER_ITEM_LIST_RETURN:
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

	case NET_MSG_PLAYER_ITEM_MONEY_RETURN:
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

				Q_strncpy(detail_item_inst.inst,(char *)pchData, sizeof(detail_item_inst.inst));
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
			switch (dwParam) 
			{
			case -1:
				{
					display_error_message((UCHR*)MSG_GIVE_ITEM_ERROR, 1);
				}
				break;

			case -2:
				{
					display_system_message((UCHR*)MSG_REJECT_GIVE_ITEM);
				}
				break;
			}
		}
		break;

	case NET_MSG_PLAYER_DEAL_SEND_REQUEST_RETURN:
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
				Q_memset(character_pocket_data.other_deal_filename_id, 0xff, sizeof(character_pocket_data.other_deal_filename_id));
			
				Q_memset(character_pocket_data.other_deal_number, 0, sizeof(character_pocket_data.other_deal_number));
				
				character_pocket_data.other_deal_money = 0;

				character_pocket_data.other_deal_ready = false;
			}
			else if (dwParam == system_control.control_user_id) // 自己重新设置的
			{
				Q_memset(character_pocket_data.my_deal_idx, 0xff, sizeof(character_pocket_data.my_deal_idx));
				
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
			switch (dwParam)
			{
			case 1:
				{
					ITEM_DATA_EX *pIde = (ITEM_DATA_EX*)pchData;
					
					for (DWORD i = 0; i < dwParam; i ++)
					{
						int idx = pIde[i].idx;

						character_equip_data.equip[idx] = pIde[i].imd;
					}
				}
				break;

			case EQUIPE_CHANGE_ERROR_PHYLE:
				{
					display_system_message((UCHR*)MSG_EQUIP_PHYLE_ERROR);
				}
				break;

			case EQUIPE_CHANGE_ERROR_LEVEL:
				{
					display_system_message((UCHR*)MSG_EQUIP_LEVEL_ERROR);
				}
				break;

			case EQUIPE_CHANGE_ERROR_HP:
				{
					display_system_message((UCHR*)MSG_EQUIP_HP_ERROR);
				}
				break;

			case EQUIPE_CHANGE_ERROR_MP:
				{
					display_system_message((UCHR*)MSG_EQUIP_MP_ERROR);
				}
				break;

			case EQUIPE_CHANGE_ERROR_ATT:
				{
					display_system_message((UCHR*)MSG_EQUIP_ATT_ERROR);
				}
				break;

			case EQUIPE_CHANGE_ERROR_SP:
				{
					display_system_message((UCHR*)MSG_EQUIP_SPEED_ERROR);
				}
				break;

			default:
				display_error_message((UCHR*)MSG_USE_EQUIP_ERROR, 1);
				break;
			}
		}
		break;

	case NET_MSG_PLAYER_EQUIPE_LIST_RETURN:
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

	case NET_MSG_PLAYER_STORE_BUY_RETURN:
		{
			if (dwParam == -1)
			{
				display_error_message((UCHR*)MSG_BUY_ITEM_ERROR, 1);
			}
			else if (dwParam == -2)
			{
				display_system_message((UCHR*)MSG_MONEY_NOT_ENOUGH);
			}
			else if (dwParam == -3)
			{
				display_system_message((UCHR*)MSG_POCKET_FULL);
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

	case NET_MSG_PLAYER_STORE_SELL_RETURN:
		{
			if (dwParam == -1)
			{
				display_system_message((UCHR*)MSG_SELL_ITEM_ERROR);
			}
			else if (dwParam == -2)
			{
				display_system_message((UCHR*)MSG_SELL_ITEM_ERROR1);				
			}
		}
		break;
		
		//////////////////////////////////////////////////////////////////////////
		//
	case NET_MSG_PLAYER_SKILL_LIST_RETURN:
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

					game_control.character_skill_ready =true;					
				}
			}
		}
		break;

	case NET_MSG_PLAYER_PET_SKILL_LIST_RETURN:
		{
			if (pchData)
			{
				for(uint i=0;i<MAX_BAOBAO_SKILL_NO;i++)
				{
					baobao_skill_data[i].skill_id=-1;
				}
				
				for (i = 0; i < dwParam; i ++)
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

	case NET_MSG_PLAYER_FIGHT_CHARGE_PAUSE_RETURN: // 服务器通知充电暂停
		{
			fight_charge_pause();
		}
		break;

	case NET_MSG_PLAYER_FIGHT_RESULT_RETURN:
		{
			if (pchData)
			{
				fight_result_t result = {0};

				memcpy(&result.base, pchData, sizeof(result.base));

				int offset = sizeof(result.base);

				if (result.base.value_num > 0)
				{
					result.value = (FIGHT_VALUE_RESULT*)(pchData+offset);
					
					offset += result.base.value_num*sizeof(*result.value);
				}

				if (result.base.magic_num > 0) 
				{
					result.magic = (fight_magic_data_t*)(pchData+offset);

					offset += result.base.magic_num*sizeof(*result.magic);				
				}

				result.cust_buf = pchData+offset;
				
				switch (result.base.action.command)
				{
				case FIGHT_ACTION_SINGLE_PHYSICS:		// 物理单攻击 ( 含 feedback,致命, 狂暴 )
				case FIGHT_ACTION_SINGLE_PHYSICS_DOUBLE:		// 物理连击 ( 含 feedback, 致命，狂暴 )
					{
						unsigned char loop = *result.cust_buf;

						result.cust_buf ++;

						int* hp = (int*)result.cust_buf;

						if (loop == 1)
						{
							fight_action_single_physics_data.hp = *hp ++;
							fight_action_single_physics_data.feed_back_hp = *hp ++;
							fight_action_single_physics_data.back_hp = *hp ++;
						}
						else
						{
							fight_action_single_physics_double_data.double_count = loop;

							for (int i = 0; i < loop; i ++)
							{
								fight_action_single_physics_double_data.hp[i] = *hp ++;
								fight_action_single_physics_double_data.feedback[i] = *hp ++;
								fight_action_single_physics_double_data.back_hp[i] = *hp ++;
							}
						}

						fight_action_single_physics_data.protect_id = *(int*)hp ++;
					}
					break;

				case FIGHT_ACTION_CHANGE:
					{
						FIGHT_BASE_DATA_EX* tm_data = (FIGHT_BASE_DATA_EX*)result.cust_buf;

						fight_action_change_baobao_data.data.charge_speed = tm_data->data.charge_speed;
						fight_action_change_baobao_data.filename_id = tm_data->data.filename_id;
						fight_action_change_baobao_data.data.id = tm_data->data.id;
						fight_action_change_baobao_data.life_base = tm_data->data.life_base;
						fight_action_change_baobao_data.data.max_hp = tm_data->data.max_hp;
						fight_action_change_baobao_data.data.max_mp = tm_data->data.max_mp;
						strcpy((char *)fight_action_change_baobao_data.name, tm_data->data.name);
						fight_action_change_baobao_data.data.now_hp = tm_data->data.now_hp;
						fight_action_change_baobao_data.data.now_mp = tm_data->data.now_mp;
						fight_action_change_baobao_data.data.status = tm_data->data.status;
						fight_action_change_baobao_data.unit_type = tm_data->data.unit_type;

						result.cust_buf += sizeof(FIGHT_BASE_DATA_EX);

						char num = *result.cust_buf;
						
						result.cust_buf ++;

						charge_list_t* charge_list = (charge_list_t*)result.cust_buf;
						
						for (int i = 0; i < num; i ++)
						{
							for (int j = 0; j < MAX_FIGHT_UNIT; j ++)
							{
								if (fight_npc_group[j].base.id == charge_list[i].id)
								{
									fight_npc_group[j].base.charge_speed = charge_list[i].idx;

									break;
								}
							}
						}
					}
					break;

				case FIGHT_ACTION_SINGLE_MAGIC:
					{
						if (result.base.magic_num == 1)
						{
							fight_action_single_magic_data.hp = result.magic[0].hurt_hp;

							fight_action_single_magic_data.feed_back_hp = result.magic[0].back_hp;
						}

						if (result.cust_buf)
						{
							char num = *result.cust_buf;
							
							result.cust_buf ++;
														
							charge_list_t* charge_list = (charge_list_t*)result.cust_buf;
							
							for (int i = 0; i < num; i ++)
							{
								for (int j = 0; j < MAX_FIGHT_UNIT; j ++)
								{
									if (fight_npc_group[j].base.id == charge_list[i].id)
									{
										fight_npc_group[j].base.charge_speed = charge_list[i].idx;
										
										break;
									}
								}
							}
						}
					}
					break;

				case FIGHT_ACTION_MULTI_MAGIC:
					{
						for (int i = 0; i < result.base.magic_num; i ++)
						{
							fight_action_multi_magic_data.id[i] = result.magic[i].id;

							fight_action_multi_magic_data.hp[i] = result.magic[i].hurt_hp;
							
							fight_action_multi_magic_data.feed_back_hp[i] = result.magic[i].back_hp;
						}

						fight_action_multi_magic_data.number = result.base.magic_num;

						if (result.cust_buf)
						{
							char num = *result.cust_buf;
							
							result.cust_buf ++;
							
							charge_list_t* charge_list = (charge_list_t*)result.cust_buf;
							
							for (int i = 0; i < num; i ++)
							{
								for (int j = 0; j < MAX_FIGHT_UNIT; j ++)
								{
									if (fight_npc_group[j].base.id == charge_list[i].id)
									{
										fight_npc_group[j].base.charge_speed = charge_list[i].idx;
										
										break;
									}
								}
							}
						}
					}
					break;

				case FIGHT_ACTION_SINGLE_STATUS:
					{

					}
					break;

				case FIGHT_ACTION_MULTI_STATUS:
					{
						for (int i = 0; i < result.base.value_num; i ++)
						{
							fight_action_multi_magic_data.id[i] = result.value[i].id;
						}

						fight_action_multi_magic_data.number = result.base.value_num;
					}
					break;
				}

				update_fight_action_result(result.base.action);
				
				for (int i = 0; i < result.base.value_num; i ++)
				{
					update_fight_value_result(result.value[i]);
				}				
			}
		}
		break;

	case NET_MSG_PLAYER_FIGHT_END_RETURN:
		{
			fight_control_data.fight_end = true;
		}
		break;

	case NET_MSG_PLAYER_FIGHT_CHARGE_CONTINUE_RETURN:
		{
			unsigned int id = dwParam;
	
			if(fight_control_data.character_idx>=0 && id==fight_npc_group[fight_control_data.character_idx].base.id)
			{
				charge_time_control(FIGHT_COMMAND_CHARACTER, CHARGE_TIME_RESET);
			}
			else if(fight_control_data.baobao_idx>=0 && id==fight_npc_group[fight_control_data.baobao_idx].base.id)
			{
				charge_time_control(FIGHT_COMMAND_BAOBAO, CHARGE_TIME_RESET);
			}

			fight_charge_continue();
		}
		break;

	case NET_MSG_PLAYER_FIGHT_STATUS_RETURN:
		{
			if (pchData)
			{
				FIGHT_STATUS_RESULT *pStatus = (FIGHT_STATUS_RESULT *)pchData;

				for (uint i = 0; i < dwParam; i ++)
				{
					update_fight_status_result(pStatus[i]);
				}
			}
		}
		break;
		
	case NET_MSG_PLAYER_FIGHT_VALUE_RETURN:
		{
			if (pchData)
			{
				FIGHT_VALUE_RESULT *pValue = (FIGHT_VALUE_RESULT *)pchData;
				
				for (uint i = 0; i < dwParam; i ++)
				{
					update_fight_value_result(pValue[i]);
				}
			}
		}
		break;


	case NET_MSG_PLAYER_NOW_HP_CHANGE_RETURN:
		{
			base_character_data.data.final.now_hp = dwParam;
			map_npc_group[system_control.control_npc_idx].npc_info.status|=NPC_STATUS_FULL;	
		}
		break;

	case NET_MSG_PLAYER_NOW_MP_CHANGE_RETURN:
		{
			base_character_data.data.final.now_mp = dwParam;
			map_npc_group[system_control.control_npc_idx].npc_info.status|=NPC_STATUS_FULL;	
		}
		break;

	case NET_MSG_PLAYER_PET_NOW_HP_CHANGE_RETURN:
		{
			if (base_baobao_data.data.active_idx >= 0)
			{
				base_baobao_data.data.baobao_list[base_baobao_data.data.active_idx].data.now_hp = dwParam;
				map_npc_group[system_control.control_npc_idx].npc_info.status|=NPC_STATUS_FULL;	
			}
		}
		break;

	case NET_MSG_PLAYER_PET_NOW_MP_CHANGE_RETURN:
		{
			if (base_baobao_data.data.active_idx >= 0)
			{
				base_baobao_data.data.baobao_list[base_baobao_data.data.active_idx].data.now_mp = dwParam;
				map_npc_group[system_control.control_npc_idx].npc_info.status|=NPC_STATUS_FULL;	
			}
		}
		break;

	case NET_MSG_PLAYER_BAOBAO_LEVLE_UP_RETURN:
		{
			int idx = get_baobao_data_idx(dwParam);

			if (idx >= 0 && pchData)
			{
				struct BAOBAO_INFO
				{
					SERVER_BAOBAO_BASE  base;

					SERVER_BAOBAO_FINAL final;
				};

				BAOBAO_INFO info = *(BAOBAO_INFO*)pchData;

				base_baobao_data.data.baobao_list[idx].base = info.base;
				base_baobao_data.data.baobao_list[idx].data = info.final;

				sprintf((char *)print_rec,MSG_BAOBAO_LEVEL_UP,base_baobao_data.data.baobao_list[idx].name, base_baobao_data.data.baobao_list[idx].data.level);
				push_chat_data(CHAT_CHANNEL_SYSTEM, 0, (UCHR*)"", (UCHR *)print_rec);
			}
		}
		break;

	case NET_MSG_PLAYER_BAOBAO_FINAL_DATA_RETURN:
		{
			int idx = get_baobao_data_idx(dwParam);
			
			if (idx >= 0 && pchData)
			{
				SERVER_BAOBAO_FINAL final = *(SERVER_BAOBAO_FINAL*)pchData;
				
				base_baobao_data.data.baobao_list[idx].data = final;
			}
		}
		break;

	case NET_MSG_PLAYER_BAOBAO_EXP_UP_RETURN:
		{
			int idx = get_baobao_data_idx(dwParam);

			if (idx >= 0 && pchData)
			{
				CLIENT_UPDATE_EXP Update = {0};

				Update = *(CLIENT_UPDATE_EXP*)pchData;


				if(Update.total_exp>base_baobao_data.data.baobao_list[idx].data.total_exp)
				{
					sprintf((char *)print_rec,MSG_BAOBAO_ADD_EXP,base_baobao_data.data.baobao_list[idx].name, Update.total_exp-base_baobao_data.data.baobao_list[idx].data.total_exp);
					push_chat_data(CHAT_CHANNEL_SYSTEM, 0, (UCHR*)"", (UCHR *)print_rec);
				}
				

				base_baobao_data.data.baobao_list[idx].data.total_exp = Update.total_exp;
				base_baobao_data.data.baobao_list[idx].data.now_exp = Update.now_exp;
				base_baobao_data.data.baobao_list[idx].data.need_exp = Update.need_exp;
			}
		}
		break;

	case NET_MSG_PLAYER_SYSTEM_MESSAGE_RETURN:
		{
			if (pchData)
			{
				if (dwParam == 0)
				{
					display_error_message((UCHR*)pchData, 1);
				}
				else if (dwParam == 1)
				{
					display_system_message((UCHR*)pchData);
				}
				else if (dwParam == 2)
				{
					char buf[1024];

					Q_strncpy(buf, pchData, sizeof(buf));

					push_chat_data(CHAT_CHANNEL_SYSTEM, 0, (UCHR*)"", (UCHR *)buf);
				}
			}
		}
		break;

	case NET_MSG_PLAYER_ATTACK_NPC_RETURN:
		{

		}
		break;

	case NET_MSG_PLAYER_SKILL_UPDATE_RETURN:
		{
			for (unsigned int i = 0; i < ARRAYSIZE(character_skill_data); i ++)
			{
				if (character_skill_data[i].idx == ((PUBLIC_SKILL_DATA*)pchData)->idx)
				{
					character_skill_data[i] = *(PUBLIC_SKILL_DATA*)pchData;
				}
			}
		}
		break;

	case NET_MSG_PLAYER_BASE_DATA_RETURN:
		{
			if (pchData)
			{
				Q_memcpy(&base_character_data.data.base, pchData, sizeof(base_character_data.data.base));
			}	
		}
		break;

	case NET_MSG_PLAYER_FINAL_DATA_RETURN:
		{
			if (pchData)
			{
				Q_memcpy(&base_character_data.data.final, pchData, sizeof(base_character_data.data.final));
			}
		}
		break;

	case NET_MSG_PLAYER_NEIGHBOR_START_FIGHT_RETURN:
		{
			int idx = get_map_npc_index(dwParam);

			if ((idx >= 0)&&(idx<total_map_npc))
			{
				map_npc_group[idx].npc_info.status |= NPC_STATUS_FIGHT;
			}
		}
		break;
		
	case NET_MSG_PLAYER_NEIGHBOR_END_FIGHT_RETURN:
		{
			int idx = get_map_npc_index(dwParam);
			
			if ((idx >= 0)&&(idx<total_map_npc))
			{
				map_npc_group[idx].npc_info.status &= ~NPC_STATUS_FIGHT;
			}
		}
		break;

	case NET_MSG_PLAYER_LEAVE_ATTACK_VEIW_RETURN:
		{

		}
		break;

	case NET_MSG_PLAYER_REFRESH_MAP_RETURN:
		{
			redraw_map(screen_channel0);
			change_screen_effect_flc(rand()%9);
		}
		break;

	case NET_MSG_PLAYER_CHARGE_LIST_RETURN:
		{
			if (pchData && dwParam)
			{
				charge_list_t* charge_list = (charge_list_t*)pchData;
				
				for (uint i = 0; i < dwParam; i ++)
				{
					for (int j = 0; j < MAX_FIGHT_UNIT; j ++)
					{
						if (fight_npc_group[j].base.id == charge_list[i].id)
						{
							fight_npc_group[j].base.charge_speed = charge_list[i].idx;
							
							break;
						}
					}
				}				
			}			
		}
		break;

	case NET_MSG_PLAYER_FIGHT_ADD_VIEW_RETURN:
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
			
//			if(game_control.main_loop_mode==MAIN_LOOP_FIGHT_MODE)
			{
				for (int i = 0; i < (int)dwParam; i ++)
				{
					char msg[128] = {0};
					
					Q_snprintf(msg, sizeof(msg), MSG_ADD_SEE_FIGHT, pInfo[i].name);
					
					push_chat_data(CHAT_CHANNEL_SYSTEM, 0, (UCHR*)"", (UCHR*)msg);
					display_system_message((UCHR *)msg);
					game_control.see_fight=true;
				}
			}			
		}
		break;

	case NET_MSG_PLAYER_PET_USE_ITEM_RETURN:
		{
			if (dwParam == -2)
			{
				display_system_message((UCHR*)MSG_BAOBAO_DATA_FULL);
			}
			else if (dwParam == -1)
			{
				display_system_message((UCHR*)MSG_BAOBAO_ITEM_ERROR);
			}
		}
		break;

	case NET_MSG_PLAYER_MEMBER_LEVEL_UP_RETURN:
		{
			for (int i = 0; i < ARRAYSIZE(team_list_data); i ++)
			{
				if (team_list_data[i].char_id == dwParam)
				{
					team_list_data[i].level = *(int*)pchData;

					break;
				}
			}
		}
		break;

	case NET_MSG_PLAYER_OTHER_DATA_RETURN:
		{
			if (pchData && ulLen == sizeof(base_character_data.data.other))
			{
				memcpy(&base_character_data.data.other, pchData, sizeof(base_character_data.data.other));
			}
		}
		break;

	case NET_MSG_PLAYER_TITLE_ADD_RETURN:
		{
			if (pchData)
			{
				for (DWORD i = 0; i < dwParam; i ++)
				{
					insert_title_data(*((PUBLIC_TITLE_BASE*)pchData+i));
				}

				base_title_data.active=true;
			}
		}
		break;

	case NET_MSG_PLAYER_TITLE_DELETE_RETURN:
		{
			delete_title_data(dwParam);
		}
		break;

	case NET_MSG_PLAYER_TITLE_LIST_RETURN:
		{
			if (pchData)
			{
				for (DWORD i = 0; i < dwParam; i ++)
				{
					insert_title_data(*((PUBLIC_TITLE_BASE*)pchData+i));
				}

				base_title_data.active=true;
			}
		}
		break;

	case NET_MSG_PLAYER_TITLE_CHANGE_RETURN:
		{
			int idx = get_map_npc_index(dwParam);

			if (idx >= 0)
			{
				if (dwParam == system_control.control_user_id)
				{
					Q_strncpy(base_character_data.title, pchData, sizeof(base_character_data.title));

				}

				Q_strncpy(map_npc_group[idx].npc_info.title, pchData, sizeof(map_npc_group[idx].npc_info.title));
			}
		}
		break;

	case NET_MSG_PLAYER_TITLE_HIDE_RETURN:
		{
			int idx = get_map_npc_index(dwParam);
			
			if (idx >= 0)
			{				
				*map_npc_group[idx].npc_info.title = 0;
			}			
		}
		break;

	default:
		break;
	}

//	writeDebugFile("end net handle\n\n");

	net_lock = false;

	return 0;
}
