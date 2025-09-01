/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : TextMsg.h
  *owner  		: Stephen
  *description  : 
  *modified     : 2004/12/29
******************************************************************************/ 

#ifndef _TEXTMSG_H_
#define _TEXTMSG_H_


#ifdef  GB_VERSION  //{




// -------- Error Message ---------------
// --- connect
#define MSG_CONNECT_SERVER_ERROR		"������æµ��,���Ժ�����..."
#define MSG_ACCOUND_ERROR				"�ʺŻ����������,����������!"
#define MSG_CHAT_SERVER_ERROR			"�������������!"
#define MSG_NICKNANE_ERROR				"�ǳ��Ѿ�����ʹ��"
#define MSG_CREATE_CHARACTER_ERROR		"�޷���������"
#define MSG_EXIST_SCENE_ERROR			"ͬһ���ʺŲ�������һ�����ϵĽ�ɫ�������Ƴ�"
#define MSG_OTHER_ENTER_SCENE_ERROR		"����ʺ��Ѿ���������ҵ�¼��"
// --- File
#define MSG_FILE_CREAT_ERROR			"�޷����� %s ����"
#define MSG_FILE_OPEN_ERROR				"�޷����� %s ����"
#define MSG_FILE_CHECK_ERROR			"%s ����������"
#define MSG_FILE_RANGE_ERROR			"%s ������С����"
#define MSG_FILE_DECODE_ERROR			"%s �����������"
// --- Memory 
#define MSG_MEMORY_ALLOC_ERROR			"�޷������ڴ�"
// --- Graph
#define MSG_IMG256_DECODE_ERROR			"IMG256 �������(%x)"
#define MSG_BUFFER_NOT_IMG256_FORMAT	"���������� IMG256 ��ʽ"
// --- MDA dile
#define MSG_FILE_NOT_MDA_ERROR			"%s ���� MDA ����"
#define MSG_MDA_COMMAND_ERROR			"MDA ��������(%s)"
#define MSG_MDA_DATABASE_FRAME_ERROR	"MDA ֡������(%d)"
#define MSG_MDA_UNKNOW_COMMAND			"�޷������ MDA �ⲿ����(%s)"
// --- MAP 
#define MSG_MAP_FILE_LOAD_ERROR			"�޷������ͼ����(%s)"
#define MSG_MAP_VERSION_ERROR			"��ͼ����(%s)�汾����"
#define MSG_MAP_FORMAT_ERROR			"��ͼ����(%s)��ʽ����"
// --- Game rule
#define MSG_BAOBAO_FOLLOW_ERROR			"����������!"
#define MSG_BAOBAO_RELEASE_ERROR		"����Ŀǰ���ڹۿ����޷��������������ò��ۿ�"
#define MSG_BAOBAO_RELEASE_ERROR1		"����Ŀǰ���ڲ�ս���޷��������������ò���ս"

#define MSG_ADD_BAOBAO_ERROR			"���ӱ������ݴ���"
#define MSG_DELETE_BAOBAO_ERROR			"ɾ�����������������������"
#define MSG_ITEM_DETAIL_ERROR			"��ʾ��Ʒ��������"
#define MSG_GIVE_ITEM_ERROR				"������Ʒ����"
#define MSG_REQUEST_DEAL_ERROR			"������ʧ��"
#define MSG_DEAL_CANCEL_ERROR			"����ȡ������"
#define MSG_USE_EQUIP_ERROR				"ʹ��װ������"
#define MSG_BUY_ITEM_ERROR				"����Ʒ����"
#define MSG_SAVE_MONEY_ERROR			"��Ǯ����"
#define MSG_WITHDRAW_ERROR				"ȡǮ����"
#define MSG_PUSH_POPSHOP_ERROR			"���̴���Ʒ����"
#define MSG_POP_POPSHOP_ERROR			"�ӵ���ȡ��Ʒ����"
#define MSG_MDA_ERROR					"MDA ���Ͽ⵵������̫��, system error"




// -------------- Message text -------------------
// --- system
#define MSG_GAME_NAME					"���"
#define GAME_FONT_NAME                  "����"


// --- Game base
#define CHARACTER_SEX_MAN_TEXT			"��"			// ����
#define CHARACTER_SEX_WOMAN_TEXT		"Ů"			// Ů��

#define CHARACTER_PHYLE_HUMAN_TEXT		"����"
#define CHARACTER_PHYLE_GENUS_TEXT		"����"
#define CHARACTER_PHYLE_MAGIC_TEXT		"��ʦ"
#define CHARACTER_PHYLE_ROBET_TEXT		"��е"

#define BAOBAO_PHYLE_HUMAN_TEXT			"����ϵ"
#define BAOBAO_PHYLE_FLY_TEXT			"����ϵ"
#define BAOBAO_PHYLE_DEVIL_TEXT			"��ħϵ"
#define BAOBAO_PHYLE_DEAD_TEXT			"����ϵ"
#define BAOBAO_PHYLE_DRAGON_TEXT		"��ϵ"
#define BAOBAO_PHYLE_SPECIAL_TEXT		"����ϵ"


#define WEATHER_TEXT_NONE				"����"
#define WEATHER_TEXT_RANG				"��"
#define WEATHER_TEXT_SNOW				"ѩ"
#define WEATHER_TEXT_CLOUD				"��"
#define WEATHER_TEXT_THUNDER			"��"

#define WEATHER_TEXT_SMALL				"С"
#define WEATHER_TEXT_MIDDLE				"��"
#define WEATHER_TEXT_LARGE				"��"

#define WEATHER_TEXT_DOWN				"��"
#define WEATHER_TEXT_EN					"����"
#define WEATHER_TEXT_WN					"����"

#define WEATHER_TEXT_REPORT				"#Y%s#N����,�������,Ŀǰ#Y%s#N,����#Y%s#N"
#define WEATHER_TEXT_SAMPLE_REPORT		"%s����%s"

#define CHARACTER_INST_HUMAN_MAN		"������,�ó������༼��#0"
#define CHARACTER_INST_HUMAN_WOMAN		"Ů����,�ó������༼��#0"
#define CHARACTER_INST_GENUS_MAN		"�о���,�ó����˹�������#0"
#define CHARACTER_INST_GENUS_WOMAN		"Ů����,�ó��ָ�ϵ����#0"
#define CHARACTER_INST_MAGIC_MAN		"�з�ʦ,�ó�������������#0"
#define CHARACTER_INST_MAGIC_WOMAN		"Ů��ʦ,�ó�������������#0"
#define CHARACTER_INST_ROBET_MAN		"�л���,�ó�����ϵ����#0"
#define CHARACTER_INST_ROBET_WOMAN		"Ů����,�ó�����ϵ����#0"


// --- Connect
#define MSG_SERVER_NOT_FIND				"�޷����ӵ�ָ���ķ�����"
#define MSG_CONNECT_WAITTING			"���ӷ�����!���Դ�......"
#define MSG_ACCOUNT_EMERY				"��������ȷ���ʺ���������"
#define MSG_CONNECT_SERVER				"�������ӷ�����"
#define MSG_LAN_DISCONNECT				"����Ͽ�����!!!"
#define MSG_SERVER_SHUTDOWN				"����������ά������,���Ժ�����!"

// --- Game data
#define MSG_CHAR_LEVEL					"%d ��"
#define MSG_CHARACTER_CREATE_FULL		"~C2���Ѿ�����5������,�޷��ٴ����µ�����~C0"
#define MSG_NOT_SELECT_PHYLE			"��ѡ��һ������!"
#define MSG_NOT_NICK_NAME				"�������ǳ�!"

#define MSG_MAP_MAIN_MENU_INST_ITEM			"��Ʒ(I)"
#define MSG_MAP_MAIN_MENU_INST_TEAM			"���(T)"
#define MSG_MAP_MAIN_MENU_INST_ATTACK		"����(A)"
#define MSG_MAP_MAIN_MENU_INST_GIVE			"����(G)"
#define MSG_MAP_MAIN_MENU_INST_DEAL			"����(X)"
#define MSG_MAP_MAIN_MENU_INST_MISSION		"����(Q)"
#define MSG_MAP_MAIN_MENU_INST_GROUP		"����(B)"
#define MSG_MAP_MAIN_MENU_INST_SYSTEM		"ϵͳ(S)"
#define MSG_MAP_MAIN_MENU_INST_CHAT			"������(F)"

#define MSG_MAP_MAIN_MENU_INST_CHATWIN		"�����л�"
#define MSG_MAP_MAIN_MENI_INST_HISTORY		"��ʷ��¼"
#define MSG_MAP_MAIN_MENU_INST_DISCARE		"����"
#define MSG_MAP_MAIN_MENU_INST_PHIZ			"�������(0)"
#define MSG_MAP_MAIN_MENU_INST_SCROLL		"��������"
#define MSG_MAP_MAIN_MENU_INST_VIEW			"���ڼӴ�"
#define MSG_MAP_MAIN_MENU_INST_UP			"�Ϸ�"
#define MSG_MAP_MAIN_MENU_INST_DOWN			"�·�"

#define MSG_MAP_MAIN_MENU_INST_SWITCH		"�����л�"
#define MSG_MAP_MAIN_MENU_INST_WORLD		"�����ͼ"
#define MSG_MAP_MAIN_MENU_INST_SMALL		"���Ե�ͼ(Tab)"


#define MSG_WINDOW_CLOSE					"�ر�"

#define MSG_VOLUME_SUB						"��������"
#define MSG_VOLUME_ADD						"�Ӵ�����"


#define MSG_CHANNEL_NAME_SCREEN				"��ǰ"
#define MSG_CHANNEL_NAME_TEAM				"����"
#define MSG_CHANNEL_NAME_PERSON				"˽��"
#define MSG_CHANNEL_NAME_GROUP				"����"
#define MSG_CHANNEL_NAME_SELLBUY			"����"
#define MSG_CHANNEL_NAME_WORLD				"����"
#define MSG_CHANNEL_NAME_SYSTEM				"ϵͳ"
#define MSG_CHANNEL_NAME_MESSAGE			"ѶϢ"
			


#define MSG_CHAT_TIMER_ERROR				"#R#S�벻Ҫ̫��˵��#N"
#define MSG_SET_PERSON_ID					"#R˽�Ķ����趨Ϊ#N(#Y%s#N)#G%d"

#define MSG_PERSON_SEND						"�����ĵĶ�#Y%s#N˵��"
#define MSG_PERSON_RECEIVE					"#Y%s#N���ĵĶ���˵��"


#define MSG_PLAYER_INFO_ID					" ID ��%d"
#define MSG_PLAYER_INFO_SEX					"�Ա�%s"
#define MSG_PLAYER_INFO_PHYLE				"���壺%s"
#define MSG_PLAYER_INFO_TITLE				"ͷ�Σ�%s"
#define MSG_PLAYER_INFO_NAME				"������%s"
#define MSG_PLAYER_INFO_LEVEL				"�ȼ���%d"


#define MSG_MAX_CHAT_FRIEND					"��ĺ������Ѿ�����,�޷�ִ���������!"
#define MSG_ADD_FRIEND						"#8���Ѿ���#Y%s#N��Ϊ������!#21"
#define MSG_ADD_SAME_FRIEND					"#Y%s#N�Ѿ�����ĺ�����!#47"
#define MSG_OTHER_ADD_FRIEND				"%s �����Ϊ����"

#define MSG_SYSTEM_ADD_FRIEND				"���Ѿ���%s��Ϊ������!"
#define MSG_SYSTEM_ADD_SAME_FRIEND			"%s�Ѿ�����ĺ�����!"
#define MSG_FRIEND_ONLINE					"#G��ĺ���#Y%s#G�Ѿ�������Ϸ#9#N"
#define MSG_FRIEND_OFFLINE					"#R��ĺ���#Y%s#R�Ѿ��뿪��Ϸ#N#8"



#define MSG_NEXUS_FRIEND0					"Ƽˮ���"				// һ�����ѹ�ϵ
#define MSG_NEXUS_FRIEND1					"ƽ��֮��"				
#define MSG_NEXUS_FRIEND2					"��ͨ����"				
#define MSG_NEXUS_FRIEND3					"�������"				
#define MSG_NEXUS_FRIEND4					"��������"				

#define MSG_NEXUS_MARRIAGE0					"�������"			
#define MSG_NEXUS_MARRIAGE1					"�»����"				// ����
#define MSG_NEXUS_MARRIAGE2					"����֮��"						
#define MSG_NEXUS_MARRIAGE3					"��������"			
#define MSG_NEXUS_MARRIAGE4					"�Ϸ�����"		

#define MSG_NEXUS_BROTHER0					"�״��ֵ�"				// ���
#define MSG_NEXUS_BROTHER1					"��֮�ֵ�"		
#define MSG_NEXUS_BROTHER2					"��֮�ֵ�"		
#define MSG_NEXUS_BROTHER3					"��֮�ֵ�"
#define MSG_NEXUS_BROTHER4					"��ʯ�ֵ�"

#define MSG_NEXUS_MASTER0					"¶ˮʦͽ"				// ʦͽ
#define MSG_NEXUS_MASTER1					"�һ�ʦͽ"			
#define MSG_NEXUS_MASTER2					"��ʦ��ͽ"
#define MSG_NEXUS_MASTER3					"��ʦ����"
#define MSG_NEXUS_MASTER4					"����֮��"
		
#define MSG_MAX_FRIEND_GROUP_SEND			"Ⱥ��������������"
#define MSG_NO_MESSAGE						"������ѶϢ����!"


#define MSG_ADD_FRIEND_ERROR				"�Է�û�д򿪺��ѿ���"	

#define MSG_CHAT_SEARCH_ERROR				"û�������ң�������Ҳ�����"

#define MSG_TEAM_LEAVE						"#Y%s#R�뿪����"

#define MSG_ADD_TEAM_NOT_LEADER_ERROR		"�Է����Ƕӳ�"
#define MSG_ADD_TEAM_OK						"�Ѿ���ӳ�������룬��ȴ�"
#define MSG_ADD_TEAM_FULL					"�Է������б�����"

#define MSG_ADD_TEAM_READY					"%s ���������"
#define MSG_LEAVE_TEAM						"��ע�⣡%s �뿪����"
#define MSG_BREAK_TEAM						"��ע�⣡����ɢ������"
#define MSG_ALREADY_IN_TEAM					"���������������ˣ����Ե�"

#define MSG_NO_MISSION						"#Y��Ŀǰû�н����κ�����#32"


#define MSG_HP_TEXT							"����"
#define MSG_MP_TEXT							"ħ��"
#define MSG_EXP_TEXT						"����"


#define MSG_BAOBAO_TITLE					"%s�ĳ���"



// ----- ս����ֵ����
#define MSG_ADD_EXP							"       ~C2��ϲ������� ~C4%u~C2 ����~C0       "
#define	MSG_LEVEL_UP						"       ~C1��ϲ�������� ~C4%u~C1 ����~C0       "


#define MSG_SUB_EXP							"       ~C1�������� ~C4%d~C1 ����~C0       "
#define	MSG_LEVEL_DOWN						"       ~C1�������� ~C4%d~C1 ����~C0       "

#define MSG_RELEASE_BAOBAO					"#G��ȷ��Ҫ#R����#G����#Y %s #G��#83"

#define MSG_POINT_ERROR						"~C1ϵͳ�ڲ������������,��� GM ����!~C0"


#define MSG_ITEM_DROP						"��ȷ������? #83"

#define MSG_NO_GIVE_DATA					" û�и���Ķ������߽�Ǯ! "


#define MSG_FIGHT_IMAGE_ERROR				"ս��ʹ�õ�����ʧ, �޷�����������Ϸ, ϵͳ���˳�, ���� GM ����,�����°�װ"


// --- show �������� ----
#define MSG_FASTNESS_PROTECT				"~C4����~C0��%d"
#define MSG_FASTNESS_SHOOT					"~C4����~C0��%d"
#define MSG_FASTNESS_DODGE					"~C4���~C0��%d"
#define MSG_FASTNESS_SUCK					"~C4����~C0��%d"
#define MSG_FASTNESS_CRAZY					"~C4��~C0��%d%% %d"
#define MSG_FASTNESS_DEADLINESS				"~C4����~C0��%d%% %d"
#define MSG_FASTNESS_REDEADLINESS			"~C4������~C0��%d%%"
#define MSG_FASTNESS_DOUBLEATT				"~C4����~C0��%d%% %d"
#define MSG_FASTNESS_BACKATT				"~C4����~C0��%d%% %d"
#define MSG_FASTNESS_BREAK_PHYSICS			"~C4������~C0��%d%% %d"
#define MSG_FASTNESS_MAGIC_FEEDBACK			"~C4��������~C0��%d%% %d"
#define MSG_FASTNESS_PHYSICS_FEEDBACK		"~C4������~C0��%d%% %d"
#define MSG_FASTNESS_DEAD					"~C4����������~C0��%d%%"
#define MSG_FASTNESS_MAGIC_DARK				"~C4����~C0��%d"
#define MSG_FASTNESS_MAGIC_LIGHT			"~C4�ⷨ~C0��%d"
#define MSG_FASTNESS_MAGIC_WATER			"~C4ˮ��~C0��%d"
#define MSG_FASTNESS_MAGIC_FIRE				"~C4��~C0��%d"
#define MSG_FASTNESS_PHYSICS				"~C4����~C0��%d"
#define MSG_FASTNESS_VIRUS					"~C4�ж�~C0��%d"
#define MSG_FASTNESS_CHAOS					"~C4����~C0��%d"
#define MSG_FASTNESS_STONE					"~C4ʯ��~C0��%d"
#define MSG_FASTNESS_SLEEP					"~C4��˯~C0��%d"
#define MSG_FASTNESS_WEEK					"~C4˥��~C0��%d"



#define MSG_BAOBAO_DETAIL_INST				"#Y%s#N#E�ȼ���%d#E������%d/%d#Eħ����%d/%d#E������%d#E�ٶȣ�%d#Eת��������%d#E�ɳ���%.3f#E"

#define MSG_SKILL_DETAIL_INST				"#Y%s#N#E%s#E"



#define MSG_FIGHT_COMMAND_NONE			"~C1  ���´�����~C0"
#define MSG_FIGHT_COMMAND_ATTACK		"ʹ��~C5����~C0����"

#define MSG_FIGHT_COMMAND_MAGIC_SEL		"��ѡ��~C1����~C0..."
#define MSG_FIGHT_COMMAND_MAGIC			"ʹ��~C5%s~C0"

#define MSG_FIGHT_COMMAND_ITEM_SEL		"��ѡ��~C1��Ʒ~C0..."
#define MSG_FIGHT_COMMAND_ITEM			"ʹ��~C5��Ʒ~C0"

#define MSG_FIGHT_COMMAND_PROTECT		"ʹ��~C5����~C0����"

#define MSG_FIGHT_COMMAND_RECOVER		"ʹ��~C5����~C0����"

#define MSG_FIGHT_COMMAND_CHANGE_SEL	"��ѡ��~C1����~C0..."
#define MSG_FIGHT_COMMAND_CHANGE		"ʹ��~C5��������~C0"

#define MSG_FIGHT_COMMAND_RUN			"ʹ��~C5����~C0����"
#define MSG_FIGHT_COMMAND_AUTO_HP		"ʹ��~C5�Զ���~C1Ѫ~C0"
#define MSG_FIGHT_COMMAND_AUTO_MP		"ʹ��~C5�Զ���~C2��~C0"
#define MSG_FIGHT_COMMAND_AUTO_ATTACK	"ʹ��~C5�Զ�~C0����"


#define MSG_FIGHT_TARGET				"����~C5%s~C0"
#define MSG_FIGHT_LEFT_TARGET_NONE		"~C1��~C4���~C1ѡ�����~C0"
#define MSG_FIGHT_RIGHT_TARGET_NONE		"~C1��~C4�Ҽ�~C1ѡ�����~C0"



#define MSG_FIGHT_FIRST_INST			"��һ�غ�ս��,�ӳ�~C4%d~C0��,���´�����"


#define MSG_FIGHT_MAGIC_ATTACK			"~C4%s~C0ʹ�ü���~C5%s~C0����!!!"
#define MSG_FIGHT_ITEM_USE				"~C4%s~C0ʹ����Ʒ����!!!"






#define MSG_AUTO_TALK_0					"#54����Ů��Ү,�㻹����,�Ҹ���ƴ��#91"
#define MSG_AUTO_TALK_1					"��ʹ��,�ٺٺ�#18#18"
#define MSG_AUTO_TALK_2					"�°�#83֪����ô������������#18"
#define MSG_AUTO_TALK_3					"#28"
#define MSG_AUTO_TALK_4					"#44"
#define MSG_AUTO_TALK_5					"#29"
#define MSG_AUTO_TALK_6					"#99�����Ǹ�����#Y%s#N��,��Ĺ���̫����#80"
#define MSG_AUTO_TALK_7					"#50#Y%s#N���������˵����ʵ����̫ǿ��#17"
#define MSG_AUTO_TALK_BOSS				"�ֵ���!!#4��!!#78,�ٺ�#28"
#define MSG_AUTO_TALK_BOSS_FOLLOW		"ɱ!!!#31"



#define MSG_FIGHT_TALK_0				"����Ҳ�Ѫ!"
#define MSG_FIGHT_TALK_1				"���Ҽӷ�!"
#define MSG_FIGHT_TALK_2				"������"
#define MSG_FIGHT_TALK_3				"��һ���ҳ���"
#define MSG_FIGHT_TALK_4				"��������!"




// -------- emote ���� ----------------
#define TOTAL_EMOTE						2
#define MSG_EMOTE_0						"hello","���","$1�Դ����������#32","$1���ڳ��Ĵ��˵:��Һ�#32","$1��$2˵:��ð�!!"
#define MSG_EMOTE_1						"?","�ʺ�","$1�е����ɻ�","$1���ڳ��Ĵ��˵:��ҽ̽��Ұ�#32","$1��$2˵:���!��̽��Һ���#17"



#endif		// GB Version






#endif	// TEXTMSG_H