/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : data.h
  *owner  		: Stephen
  *description  : 
  *modified     : 2004/12/16
******************************************************************************/ 

#ifndef _DATA_H_
#define _DATA_H_


// ---------------- Version Data ----------------------
#define GAME_VERSION	00003
#define GAME_NAME		MSG_GAME_NAME
// ----------------------------------------------------


/*******************************************************************************************************************}
{																													}
{		ͨ���Ͷ���																									}
{																													}
********************************************************************************************************************/
// --- ���ֳ��� define
#define MAX_FILENAME_LENGTH							16		// �������Ƴ���
#define MAX_NAME_LENGTH								14		// �ǳƳ���
#define MAX_TITLE_LENGTH							20		// ��ν����
#define MAX_TITLE_INST_LENGTH						40		// ��ν��������
#define MAX_MAP_NAME_LENGTH							12		// ��ͼ���Ƴ���
#define MAX_GROUP_NAME_LENGTH						14		// �������Ƴ���
#define MAX_BAOBAO_NAME_LENGTH						14		// �������Ƴ���
#define MAX_ITEM_NAME_LENGTH						10		// ��Ʒ���Ƴ���
#define MAX_ITEM_INST_LENGTH						40		// ��Ʒ��������
#define MAX_SKILL_NAME_LENGTH						14		// �������Ƴ���
#define MAX_SKILL_INST_LENGTH						40		// ������������
#define MAX_DETAIL_INST_LENGTH						256		// ��ϸ˵������


#define MAX_POCKET_ITEM								24		// �ڴ��������
#define MAX_EQUIP_ITEM								7		// װ���������
#define MAX_POPSHOP_ITEM							24		// �����������

#define MAX_BAOBAO_STACK							20		// ���� stack �������

// --- ����record define
#define MAX_CHAT_FRIEND_LIST						100		// ����������


#define MAX_TEAM_PLAYER								5		// ����������
#define MAX_TEAM_REQUEST							20		// �����б� 20


// ---- ���ԶԻ��ľ���
#define NPC_TALK_RANGE								300

#define MAX_MENU_ITEM_LIST							12		// ��� Macro ѡ��


#define NPC_ID_MASK						0x80000000		// 1000 0000 0000 0000 0000 0000 0000 0000
#define BAOBAO_ID_MASK					0xC0000000		// 1100 0000 0000 0000 0000 0000 0000 0000


#define BAOBAO_ID_CHECK_MASK			0x40000000		// 0100 0000 0000 0000 0000 0000 0000 0000

#define FOLLOW_NEAR_RANGE							50		// ������� pixel
#define FOLLOW_JUMP_RANGE							600		// ���������Զ���ҫ����


#define MAIN_LOOP_MAP_MODE							0	// ����·��ͼģʽ
#define MAIN_LOOP_FIGHT_MODE						1	// ����·ս��ģʽ


#define MAX_PHYSICS_DOUBLE_TIMES					7	// ���������������
#define MAX_MAGIC_DOUBLE_TIMES						7	// �������������
#define MAX_FIGHT_UNIT								20	// ���ս����λ

// -------- Item control
#define MAX_ITEM_IMAGE								250 
#define MAX_ITEM_OVERFLOW							30


#define PLAYER_MOVE_CHECK_TIMER						250

#define DETAIL_DELAY_TIME							1000


#define MAX_CHARACTER_SKILL_NO						20			// ��Ҽ����������
#define MAX_BAOBAO_SKILL_NO							8			// ���������������



#define MAX_BASE_CHARGE_TABLE						20			// ����ٶ��б���Ŀ

static SLONG base_charge_time_table[MAX_BASE_CHARGE_TABLE]=
{
		5000,					// 0 
		5080,					// 1
		5160,					// 2
		5240,					// 3
		5320,					// 4
		5400,					// 5
		5480,					// 6
		5560,					// 7
		5640,					// 8
		5720,					// 9
		5800,					// 10
		5880,					// 11
		5960,					// 12
		6040,					// 13
		6120,					// 14
		6200,					// 15
		6280,					// 16
		6360,					// 17
		6440,					// 18
		6520					// 19
};


typedef struct PUBLIC_SKILL_BASE_STRUCT				// ���ܻ�������
{
	// ---- ��������
	UCHR name[MAX_SKILL_NAME_LENGTH+1];				// ����
	UCHR inst[MAX_SKILL_INST_LENGTH+1];				// ����
	SHINT effect;									// ʹ��Ч��  ( see SKILL_EFFECT enum )
	SHINT mp;										// ����Ҫ MP
	SHINT add_mp;									// ÿ��ʹ����� MP
	SHINT count;									// �����غ�
	SHINT number;									// Ч������
	SHINT type;										// �ҷ��з��б� 0 -> �ҷ�, 1 -> �з�  
	ULONG lithand;									// ÿ��ʹ����������
	// ---- ����
	SHINT mda_filename_id;							// MDA ����
	SHINT ani_filename_id;							// ANI ����
	
} PUBLIC_SKILL_BASE;




#define TOTAL_SKILL								(ARRAYSIZE(skill_base_data))

static PUBLIC_SKILL_BASE skill_base_data[]={
/*	{
		"0000",											// ����
		"˵��0",										// ����
		0,												// ʹ��Ч��  ( see SKILL_EFFECT enum )
		200,											// ����Ҫ MP
		0,												// ÿ��ʹ����� MP
		0,												// �����غ�
		0,												// Ч������
		1,												// �ҷ��з��б� 0 -> �ҷ�, 1 -> �з�  
		0,												// ÿ��ʹ����������
		0,												// MDA ����
		0												// ANI ����
	},

	{	"0001",	"˵��1",0,0,0,0,0,0,0,0,0 	},
	{	"0002",	"˵��2",0,0,0,0,0,0,0,0,0 	},
	{	"0003",	"˵��3",0,0,0,0,0,0,0,0,0 	},
	{	"0004",	"˵��4",0,0,0,0,0,0,0,0,0 	},
	{	"0005",	"˵��5",0,0,0,0,0,0,0,0,0 	},
	{	"0006",	"˵��6",0,0,0,0,0,0,0,0,0 	},
	{	"0007",	"˵��7",0,0,0,0,0,0,0,0,0 	},
	{	"0008",	"˵��8",0,0,0,0,0,0,0,0,0 	},
	{	"0009",	"˵��9",0,0,0,0,0,0,0,0,0 	},
	{	"0010",	"˵��10",0,0,0,0,0,0,0,0,0 	},
	{	"0011",	"˵��11",0,0,0,0,0,0,0,0,0 	},
	{	"0012",	"˵��12",0,0,0,0,0,0,0,0,0 	},
	{	"0013",	"˵��13",0,0,0,0,0,0,0,0,0 	},
	{	"0014",	"˵��14",0,0,0,0,0,0,0,0,0 	},
	{	"0015",	"˵��15",0,0,0,0,0,0,0,0,0 	},
	{	"0016",	"˵��16",0,0,0,0,0,0,0,0,0 	},
	{	"0017",	"˵��17",0,0,0,0,0,0,0,0,0 	},
	{	"0018",	"˵��18",0,0,0,0,0,0,0,0,0 	},
	{	"0019",	"˵��19",0,0,0,0,0,0,0,0,0 	}
*/
	{	"˥����һ��",     	"����˥��",    	     4,	     5,	       0.080,	     3,	     1,	     1,	     1,	    201,	 202 },
	{	"˥��������",     	"Ⱥ��˥��",    	     4,	    15,	       0.130,	     3,	     2,	     1,	     1,	     1,	     1	},
	{	"˥��������",     	"����˥��",    	     4,	    10,	       0.120,	     3,	     1,	     1,	     1,	     1,	     1	},
	{	"˥�����Ľ�",     	"Ⱥ��˥��",    	     4,	    30,	       0.170,	     3,	     2,	     1,	     1,	     1,	     1	},
	{	"��ϵһ��",       	"�����ж�",    	     3,	     5,	       0.080,	     3,	     1,	     1,	     1,	     1,	     1	},
	{	"��ϵ����",       	"Ⱥ���ж�",    	     3,	    15,	       0.130,	     3,	     2,	     1,	     1,	     1,	     1	},
	{	"��ϵ����",       	"�����ж�",    	     3,	    10,	       0.120,	     3,	     1,	     1,	     1,	     1,	     1	},
	{	"��ϵ�Ľ�",       	"Ⱥ���ж�",    	     3,	    30,	       0.170,	     3,	     2,	     1,	     1,	     1,	     1	},
	{	"������һ��",     	"�������",    	     4,	     5,	       0.080,	     3,	     1,	     1,	     1,	     1,	     1	},
	{	"����������",     	"Ⱥ�����",    	     4,	    15,	       0.130,	     3,	     2,	     1,	     1,	     1,	     1	},
	{	"����������",     	"�������",    	     4,	    10,	       0.120,	     3,	     1,	     1,	     1,	     1,	     1	},
	{	"�������Ľ�",     	"Ⱥ�����",    	     4,	    30,	       0.170,	     3,	     2,	     1,	     1,	     1,	     1	},
	{	"������һ��",     	"�����˯",    	     2,	     5,	       0.080,	     3,	     1,	     1,	     1,	     1,	     1	},
	{	"����������",     	"Ⱥ���˯",    	     2,	    15,	       0.130,	     3,	     2,	     1,	     1,	     1,	     1	},
	{	"����������",     	"�����˯",    	     2,	    10,	       0.120,	     3,	     1,	     1,	     1,	     1,	     1	},
	{	"�������Ľ�",     	"Ⱥ���˯",    	     2,	    30,	       0.170,	     3,	     2,	     1,	     1,	     1,	     1	},
	{	"ʯ����һ��",     	"����ʯ��",    	     0,	     5,	       0.080,	     3,	     1,	     1,	     1,	     1,	     1	},
	{	"ʯ��������",     	"Ⱥ��ʯ��",    	     0,	    15,	       0.130,	     3,	     2,	     1,	     1,	     1,	     1	},
	{	"ʯ��������",     	"����ʯ��",    	     0,	    10,	       0.120,	     3,	     1,	     1,	     1,	     1,	     1	},
	{	"ʯ�����Ľ�",     	"Ⱥ��ʯ��",    	     0,	    30,	       0.170,	     3,	     2,	     1,	     1,	     1,	     1	},
	{	"�������һ��",   	"����������",	    10,	     7,	       0.080,	     0,	     1,	     0,	     1,	     1,	     1	},
	{	"������Ҷ���",   	"Ⱥ��������",	    10,	    20,	       0.130,	     0,	     2,	     0,	     1,	     1,	     1	},
	{	"�����������",   	"����������",	    10,	    12,	       0.120,	     0,	     1,	     0,	     1,	     1,	     1	},
	{	"��������Ľ�",   	"Ⱥ��������",	    10,	    35,	       0.170,	     0,	     2,	     0,	     1,	     1,	     1	},
	{	"���ʯ��һ��",   	"������ʯ��",	    10,	     7,	       0.080,	     0,	     1,	     0,	     1,	     1,	     1	},
	{	"���ʯ������",   	"Ⱥ����ʯ��",	    10,	    20,	       0.130,	     0,	     2,	     0,	     1,	     1,	     1	},
	{	"���ʯ������",   	"������ʯ��",	    10,	    12,	       0.120,	     0,	     1,	     0,	     1,	     1,	     1	},
	{	"���ʯ���Ľ�",   	"Ⱥ����ʯ��",	    10,	    35,	       0.170,	     0,	     2,	     0,	     1,	     1,	     1	},
	{	"��ʸ",           	"�����ι���",	    12,	     7,	       0.060,	     0,	     1,	     1,	     1,	     1,	     1	},
	{	"����",           	"Ⱥ���ι���",	    12,	    20,	       0.100,	     0,	     2,	     1,	     1,	     1,	     1	},
	{	"ħ��",           	"�����ι���",	    12,	    12,	       0.120,	     0,	     1,	     1,	     1,	     1,	     1	},
	{	"����",           	"Ⱥ���ι���",	    12,	    35,	       0.160,	     0,	     2,	     1,	     1,	     1,	     1	},
	{	"�ٶ�����һ��",   	"�������",    	    13,	     7,	       0.080,	     3,	     1,	     0,	     1,	     1,	     1	},
	{	"�ٶ���������",   	"Ⱥ�����",    	    13,	    20,	       0.130,	     3,	     2,	     0,	     1,	     1,	     1	},
	{	"�ٶ���������",   	"�������",    	    13,	    12,	       0.120,	     3,	     1,	     0,	     1,	     1,	     1	},
	{	"�ٶ������Ľ�",   	"Ⱥ�����",    	    13,	    35,	       0.170,	     3,	     2,	     0,	     1,	     1,	     1	},
	{	"������һ��",     	"����ظ�����",	    14,	     7,	       0.080,	     0,	     1,	     0,	     1,	     1,	     1	},
	{	"����������",     	"Ⱥ��ظ�����",	    14,	    20,	       0.130,	     0,	     2,	     0,	     1,	     1,	     1	},
	{	"����������",     	"����ظ�����",	    14,	    12,	       0.120,	     0,	     1,	     0,	     1,	     1,	     1	},
	{	"�������Ľ�",     	"Ⱥ��ظ�����",	    14,	    35,	       0.170,	     0,	     2,	     0,	     1,	     1,	     1	},
	{	"��ϵ����һ��",   	"�����ϵ����",	    23,	     8,	       0.100,	     0,	     1,	     1,	     1,	     1,	     1	},
	{	"��ϵ��������",   	"Ⱥ���ϵ����",	    23,	    12,	       0.150,	     0,	     2,	     1,	     1,	     1,	     1	},
	{	"��ϵ��������",   	"�����ϵ����",	    23,	    18,	       0.180,	     0,	     1,	     1,	     1,	     1,	     1	},
	{	"��ϵ�����Ľ�",   	"Ⱥ���ϵ����",	    23,	    30,	       0.230,	     0,	     2,	     1,	     1,	     1,	     1	},
	{	"ˮϵ����һ��",   	"����ˮϵ����",	    22,	     8,	       0.100,	     0,	     1,	     1,	     1,	     1,	     1	},
	{	"ˮϵ��������",   	"Ⱥ��ˮϵ����",	    22,	    12,	       0.150,	     0,	     2,	     1,	     1,	     1,	     1	},
	{	"ˮϵ��������",   	"�����ϵ����",	    22,	    18,	       0.180,	     0,	     1,	     1,	     1,	     1,	     1	},
	{	"ˮϵ�����Ľ�",   	"Ⱥ���ϵ����",	    22,	    30,	       0.230,	     0,	     2,	     1,	     1,	     1,	     1	},
	{	"��ϵ����һ��",   	"���尵ϵ����",	    20,	     8,	       0.100,	     0,	     1,	     1,	     1,	     1,	     1	},
	{	"��ϵ��������",   	"Ⱥ�尵ϵ����",	    20,	    12,	       0.150,	     0,	     2,	     1,	     1,	     1,	     1	},
	{	"��ϵ��������",   	"���尵ϵ����",	    20,	    18,	       0.180,	     0,	     1,	     1,	     1,	     1,	     1	},
	{	"��ϵ�����Ľ�",   	"Ⱥ�尵ϵ����",	    20,	    30,	       0.230,	     0,	     2,	     1,	     1,	     1,	     1	},
	{	"��ϵ����һ��",   	"�����ϵ����",	    21,	     8,	       0.100,	     0,	     1,	     1,	     1,	     1,	     1	},
	{	"��ϵ��������",   	"Ⱥ���ϵ����",	    21,	    12,	       0.150,	     0,	     2,	     1,	     1,	     1,	     1	},
	{	"��ϵ��������",   	"�����ϵ����",	    21,	    18,	       0.180,	     0,	     1,	     1,	     1,	     1,	     1	},
	{	"��ϵ�����Ľ�",   	"Ⱥ���ϵ����",	    21,	    30,	       0.230,	     0,	     2,	     1,	     1,	     1,	     1	},
	{	"��������һ��",   	"������������",	    24,	     8,	       0.100,	     9,	     1,	     1,	     1,	     1,	     1	},
	{	"�����������",   	"Ⱥ����������",	    24,	    12,	       0.150,	    10,	     2,	     1,	     1,	     1,	     1	},
	{	"������������",   	"������������",	    24,	    18,	       0.180,	     6,	     1,	     1,	     1,	     1,	     1	},
	{	"���������Ľ�",   	"Ⱥ����������",	    24,	    30,	       0.230,	     8,	     2,	     1,	     1,	     1,	     1	},
	{	"��������һ��",   	"������������",	    33,	     3,	       0.040,	     3,	     1,	     0,	     1,	     1,	     1	},
	{	"������������",   	"Ⱥ����������",	    33,	     5,	       0.090,	     3,	     2,	     0,	     1,	     1,	     1	},
	{	"������������",   	"������������",	    33,	     6,	       0.100,	     3,	     1,	     0,	     1,	     1,	     1	},
	{	"���������Ľ�",   	"Ⱥ����������",	    33,	    10,	       0.150,	     3,	     2,	     0,	     1,	     1,	     1	},
	{	"��������һ��",   	"������������",	    34,	     3,	       0.040,	     3,	     1,	     0,	     1,	     1,	     1	},
	{	"������������",   	"Ⱥ����������",	    34,	     5,	       0.090,	     3,	     2,	     0,	     1,	     1,	     1	},
	{	"������������",   	"������������",	    34,	     6,	       0.100,	     3,	     1,	     0,	     1,	     1,	     1	},
	{	"���������Ľ�",   	"Ⱥ����������",	    34,	    10,	       0.150,	     3,	     2,	     0,	     1,	     1,	     1	},
	{	"������һ��",     	"��������",    	    30,	     3,	       0.040,	     0,	     1,	     1,	     1,	     1,	     1	},
	{	"����������",     	"Ⱥ������",    	    30,	     5,	       0.090,	     0,	     2,	     1,	     1,	     1,	     1	},
	{	"����������",     	"��������",    	    30,	     6,	       0.100,	     0,	     1,	     1,	     1,	     1,	     1	},
	{	"�������Ľ�",     	"Ⱥ������",    	    30,	    10,	       0.150,	     0,	     2,	     1,	     1,	     1,	     1	},
	{	"������һ��",   	"����������",	    31,	     3,	       0.040,	     3,	     1,	     0,	     1,	     1,	     1	},
	{	"����������",   	"Ⱥ��������",	    31,	     5,	       0.090,	     3,	     2,	     0,	     1,	     1,	     1	},
	{	"����������",   	"����������",	    31,	     6,	       0.100,	     3,	     1,	     0,	     1,	     1,	     1	},
	{	"�������Ľ�",   	"Ⱥ��������",	    31,	    10,	       0.150,	     3,	     2,	     0,	     1,	     1,	     1	},
	{	"ħ������һ��",   	"���巨������",	    32,	     3,	       0.040,	     3,	     1,	     0,	     1,	     1,	     1	},
	{	"ħ���������",   	"Ⱥ�巨������",	    32,	     5,	       0.090,	     3,	     2,	     0,	     1,	     1,	     1	},
	{	"ħ����������",   	"���巨������",	    32,	     6,	       0.100,	     3,	     1,	     0,	     1,	     1,	     1	},
	{	"ħ�������Ľ�",   	"Ⱥ�巨������",	    32,	    10,	       0.150,	     3,	     2,	     0,	     1,	     1,	     1	},		
};




/*******************************************************************************************************************}
{																													}
{		Define ����ִ������ & Control Code																			}
{																													}
********************************************************************************************************************/
enum	SYSTEM_MAIN_TASK_CODE
{
		TASK_SYSTEM_INITIALIZE=						0,			// ϵͳ����
		TASK_SHOW_TITLE=							1,			// ��ʾƬͷ Avi
		TASK_SELECT_MAINFUNCTION=					2,			// ��Ϸѡ��������
		TASK_SYSTEM_BROADCAST=						3,			// ϵͳ����
		TASK_SERVER_SELECT=							4,			// ������ѡ�����
		TASK_ACCOUNT_INPUT=							5,			// �ʺ��������
		TASK_SELECT_CHARACTER=						6,			// ѡ�����ǽ���
		TASK_SELECT_CHARACTER_IN=					7,			// ѡ�����ǽ���
		TASK_CREATE_CHARACTER=						8,			// �������ǽ���
		TASK_WORK_TEAM=								9,			// ������Ա����
		TASK_GAME_LOGIN=							10,			// ��Ϸ����½
// ---------
		TASK_CHANGE_SCENE=							99,
		TASK_GAME_MAIN=								100,		// ������Ϸ

		TASK_SYSTEM_QUIT=							9999		// ����ϵͳ
};

/*******************************************************************************************************************}
{																													}
{		Define ����ִ�лش�ֵ																						}
{																													}
********************************************************************************************************************/
enum	SYSTEM_MAIN_TASK_RETURN_CODE
{
		RETURN_ERROR=								-1,			// ִ�й�������  (�൱�� TTN_ERROR )
		RETURN_CANCEL=								-2,			// ִ�й���ȡ��  (�൱�� TTN_NOT_OK )
		RETURN_OK=									0,			// ִ�й������  (�൱�� TTN_OK )
		RETURN_CHANGE_TASK=							1000		// ���� task
};


/*******************************************************************************************************************}
{																													}
{		Define ���� control code																					}
{																													}
********************************************************************************************************************/
enum	WEATHER_CONTROL_CODE
{
		WEATHER_TYPE_NONE=		0x00000000,			// ����
		WEATHER_TYPE_RAIN=		0x00000001,			// ����
		WEATHER_TYPE_SNOW=		0x00000002,			// ��ѩ
		WEATHER_TYPE_CLOUD=		0x00000004, 		// �����
		WEATHER_TYPE_THUNDER=	0x00000008, 		// ����
			
		WEATHER_RANGE_SMALL=	0x00000000,			// С �b�Ƶ����   ����
		WEATHER_RANGE_MIDDLE=	0x01000000,			// ��			   ����
		WEATHER_RANGE_LARGE=	0x02000000			// ��			   ����
};


/*******************************************************************************************************************}
{																													}
{		Define chat control code																					}
{																													}
********************************************************************************************************************/
enum	CHAT_CHANNEL_CODE
{
		CHAT_CHANNEL_SCREEN=			0,			// ��ǰƵ��
		CHAT_CHANNEL_TEAM=				1,			// ����
		CHAT_CHANNEL_PERSON=			2,			// ˽��
		CHAT_CHANNEL_GROUP=				3,			// ����
		CHAT_CHANNEL_SELLBUY=			4,			// ����
		CHAT_CHANNEL_WORLD=				5,			// ����
		CHAT_CHANNEL_SYSTEM=			6,			// ϵͳ˵��
		CHAT_CHANNEL_MESSGAE=			7,			// ���ѶϢ
		CHAT_CHANNEL_FRIEND=			8,			// ����Ƶ��
		CHAT_CHANNEL_NONE=				99
};


#define CHAT_CHANNEL_SCREEN_TIMER		1000*2			// ��ǰƵ��
#define CHAT_CHANNEL_TEAM_TIMER			1000*2			// ����
#define CHAT_CHANNEL_PERSON_TIMER		1000*2			// ˽��
#define CHAT_CHANNEL_GROUP_TIMER		1000*3			// ����
#define CHAT_CHANNEL_SELLBUY_TIMER		1000*5			// ����
#define CHAT_CHANNEL_WORLD_TIMER		1000*5			// ����



#define CHARACTER_SEX_MAN							0			// ����
#define CHARACTER_SEX_WOMEN							1			// Ů��

#define CHARACTER_PHYLE_HUMAN						0			// ����
#define CHARACTER_PHYLE_GENUS						1			// ����
#define CHARACTER_PHYLE_MAGIC						2			// ��ʦ
#define CHARACTER_PHYLE_ROBOT						3			// ��е


#define CHARACTER_BASE_TYPE_HUMAN_MAN				CHARACTER_PHYLE_HUMAN*2+CHARACTER_SEX_MAN
#define CHARACTER_BASE_TYPE_HUMAN_WOMAN				CHARACTER_PHYLE_HUMAN*2+CHARACTER_SEX_WOMEN
#define CHARACTER_BASE_TYPE_GENUS_MAN				CHARACTER_PHYLE_GENUS*2+CHARACTER_SEX_MAN
#define CHARACTER_BASE_TYPE_GENUS_WOMAN				CHARACTER_PHYLE_GENUS*2+CHARACTER_SEX_WOMEN
#define CHARACTER_BASE_TYPE_MAGIC_MAN				CHARACTER_PHYLE_MAGIC*2+CHARACTER_SEX_MAN
#define CHARACTER_BASE_TYPE_MAGIC_WOMAN				CHARACTER_PHYLE_MAGIC*2+CHARACTER_SEX_WOMEN
#define CHARACTER_BASE_TYPE_ROBOT_MAN				CHARACTER_PHYLE_ROBOT*2+CHARACTER_SEX_MAN
#define CHARACTER_BASE_TYPE_ROBOT_WOMAN				CHARACTER_PHYLE_ROBOT*2+CHARACTER_SEX_WOMEN


#define BAOBAO_PHYLE_HUMAN							5			// ����ϵ
#define BAOBAO_PHYLE_FLY							2			// ����ϵ
#define BAOBAO_PHYLE_DEVIL							1			// ��ħϵ
#define BAOBAO_PHYLE_DEAD							4			// ����ϵ
#define BAOBAO_PHYLE_DRAGON							6			// ��ϵ
#define BAOBAO_PHYLE_SPECIAL						3			// ����ϵ


// ---------- Define NPC CONTROL command
// -------- ��ͼNPC ������ʽ
#define NPC_MOTION_STANDBY1				0
#define NPC_MOTION_STANDBY2				1
#define NPC_MOTION_WALK					2
#define NPC_MOTION_RUN					3				// CPU NPC û���ܲ�

#define NPC_MOVE_HOLD					0				// ���ƶ�
#define NPC_MOVE_PATH					1				// ѭ���ƶ�
#define NPC_MOVE_AUTO					2				// �ڸ���(�༭���趨)�����ƶ�
#define NPC_MOVE_FOLLOW					3				// �����ƶ�(ֻ�ʺϱ���)

#define NPC_DIR_UP						0
#define NPC_DIR_DOWN					1
#define NPC_DIR_LEFT					2
#define NPC_DIR_RIGHT					3
#define NPC_DIR_UP_RIGHT				4
#define NPC_DIR_DOWN_LEFT				5
#define NPC_DIR_UP_LEFT					6
#define NPC_DIR_DOWN_RIGHT				7

#define MAX_NPC_DIR						8




// --------- Define Chat Control code 
#define MAX_CHAT_LENGTH					160
#define MAX_CHAT_RECORD					1024
#define MAX_FRIEND_DATA_RECORD			512				

#define SCREEN_TEXT_TIMER				1000*8			// 8 ��
#define SCREEN_TEXT_WINDOW_WIDTH		12*8			// 6��1������




/*******************************************************************************************************************}
{																													}
{		Define ���ѹ�ϵ																								}
{																													}
********************************************************************************************************************/
enum
{
	NEXUS_FRIEND=						0,				// һ�����ѹ�ϵ
	NEXUS_MARRIAGE=						1,				// ����
	NEXUS_BROTHER=						2,				// ���
	NEXUS_MASTER=						3				// ʦͽ
};









typedef struct CHAT_RECORD_STRUCT
{
	ULONG user_id;										// ˵���˵� ID  �����λΪ1��ʱ��Ϊϵͳ˵��,����ʾ
	ULONG channel;										// chat Ƶ��
	UCHR nickname[MAX_NAME_LENGTH+1];					// nicknane
	UCHR text[MAX_CHAT_LENGTH];							// ��������
	
} CHAT_RECORD;


typedef struct CHAT_RECEIVE_DATA_STRUCT
{
	ULONG user_id;										// ˵���˵� ID  �����λΪ1��ʱ��Ϊϵͳ˵��,����ʾ
	ULONG channel;										// chat Ƶ��
	UCHR nickname[MAX_NAME_LENGTH+1];					// nicknane
	UCHR text[MAX_CHAT_LENGTH];							// ��������
	UCHR time[80];										// ʱ��
} CHAT_RECEIVE_DATA;


typedef struct CHAT_DATA_BUFFER_STRUCT
{
	SLONG top_index;									// ���� point
	SLONG bottom_index;									// �ײ� point
	SLONG colume;										// ��ʾ����
	SLONG display_index;								// ��ʾ point
	CHAT_RECORD record[MAX_CHAT_RECORD];				// ��������
} CHAT_DATA_BUFFER;



#define FRIEND_NORMAL				0					// һ�����
#define FRIEND_TEMP					1					// ��ʱ����
#define FRIEND_MASK					2					// ������

typedef struct CHAT_FRIEND_LIST_STRUCT
{
	char online;										// �Ƿ�����
	ULONG id;
	char friend_type;									// �������� ( 0 -> һ��, 1 ->  ��ʱ����  2 -> ������)
	UCHR nickname[MAX_NAME_LENGTH+1];					// nicknane
	UCHR title[MAX_TITLE_LENGTH+1];						// ��ν
	SLONG level;										// �ȼ�
	SLONG base_type;									// ������̬
	UCHR group[MAX_GROUP_NAME_LENGTH+1];				// ����
	SLONG nexus;										// ��ϵ
	ULONG amity;										// �Ѻö�
	
} CHAT_FRIEND_LIST;



typedef struct CLIENT_CONFIG_STRUCT
{
	char fight_flag;			// �д迪�� ( 0 -> �ر�, 1 -> �д�� )
	char mail_flag;				// �ż����� ( 0 -> �ر�, 1 -> �ż����ܴ� )
	char item_flag;				// ��Ʒ���� ( 0 -> �ر�, 1 -> ��Ʒ���ܴ� )
	char friend_flag;			// ���ѿ��� ( 0 -> �ر�, 1 -> ������Ѵ� )
	char team_flag;				// ��ӿ��� ( 0 -> �ر�, 1 -> ������Ӽ��� )
	char strange_flag;			// İ���˿��� ( 0 -> �ر�, 1 -> �ܾ�İ������Ϣ )

	char channel_screen_flag;	// ��ǰƵ������
	char channel_team_flag;		// ����Ƶ������
	char channel_person_flag;	// ˽��Ƶ������
	char channel_group_flag;	// ����Ƶ������
	char channel_sellbuy_flag;	// ����Ƶ������
	char channel_world_flag;	// ����Ƶ������
	
	
} CLIENT_CONFIG;


#define MAX_MISSION				20
#define MAX_MISSION_TEXT_LEN	120

typedef struct MISSION_DATA_STRUCT
{
	SLONG mission_id;
	UCHR text[MAX_MISSION_TEXT_LEN+2];

} MISSION_DATA;







/*******************************************************************************************************************}
{																													}
{																													}
{																													}
{	SERVER  �� ֵ �� ��																								}
{																													}
{																													}
{																													}
********************************************************************************************************************/
/*******************************************************************************************************************}
{																													}
{	�� �� �� ֵ �� ��																								}
{																													}
********************************************************************************************************************/
enum	ITEM_TYPE_CODE 	// װ������ ( ����ѡ�����ܷ��� )
{
		ITEM_TYPE_EQUIP=								0,			// ����װ�� (����װ�������ϵ�)
		ITEM_TYPE_DRUP=									1,			// ҩƷ
		ITEM_TYPE_ITEM=									2,			// ����
		ITEM_TYPE_SOURCE=								3,			// ԭ��
		ITEM_TYPE_SPECIAL=								4			// ������Ʒ ( �����õ�,���紫������,���鵤,�㿨...)
};

enum	ITEM_STORE_CODE	// �������
{																	// ���ж��������Է������ϣ����Բ�����ڴ�����
		ITEM_STORE_POPSHOP=								0x01,		// ���Դ�ŵ���
		ITEM_STORE_STAFF=								0x02,		// ���Դ��Ӷ��
		ITEM_STORE_NPC=									0x04,		// ���Դ�� Macro NPC
		ITEM_STORE_GIVE=								0x08,		// ���Ը���
		ITEM_STORE_DEAL=								0x10		// ���Խ���
};


enum	ITEM_USE_CODE	// ʹ������
{
		ITEM_USE_NONE=									0x00,		// û��ָ��
		ITEM_USE_MAP=									0x01,		// ��ͼʹ�� ( ֻ���ڵ�ͼʹ��,û��ָ�����Ƕ�����,ս��ҲOK )
		ITEM_USE_FIGHT=									0x02,		// ս��ʹ��
		ITEM_USE_MACRO=									0x04,		// Macro ��Ʒ, ֻ�ܶ�Macro npc ʹ��,����������Ʒ

};

enum	ITEM_POSITION_CODE	// ʹ�ò�λ
{
		ITEM_POSITION_NONE=								0,			// û��ָ�� ( ֱ��ʹ�� )
		ITEM_POSITION_HAND=								0x01,		// �ֲ� or ��������
		ITEM_POSITION_HEAD=								0x02,		// ͷ�� or ����ñ��
		ITEM_POSITION_BODY=								0x04,		// ���� or �����·�
		ITEM_POSITION_FOOT=								0x08,		// �Ų� or ����Ь��
		ITEM_POSITION_FINGER=							0x10,		// ��ָ 
		ITEM_POSITION_WRIST=							0x20,		// ���� or �������� ( ħ��ʯͷ )
		ITEM_POSITION_RING=								0x40,		// ���� or �������� ( ħ��ʯͷ )
		
};


enum	ITEM_PHYLE_CODE	// ʹ������
{
		ITEM_PHYLE_HUMAN_MAN=							0x01,		// ����
		ITEM_PHYLE_HUMAN_WOMAN=							0x02,		// Ů��
		ITEM_PHYLE_GENUS_MAN=							0x04,		// ������
		ITEM_PHYLE_GENUS_WOMAN=							0x08,		// Ů����
		ITEM_PHYLE_MAGIC_MAN=							0x10,		// �з�ʦ
		ITEM_PHYLE_MAGIC_WOMAN=							0x20,		// Ů��ʦ
		ITEM_PHYLE_ROBOT_MAN=							0x40,		// �л���
		ITEM_PHYLE_ROBOT_WOMAN=							0x80,		// Ů����
		ITEM_PHYLE_NONE=								0xff		// û��ָ��(ȫ������ʹ��)
};


enum	ITEM_LIMIT_CODE	// ʹ������
{
		ITEM_LIMIT_NONE=								0,			// û������
		ITEM_LIMIT_LEVEL=								0x01000000,	// �ȼ����� ( ����� �ȼ����� )
		ITEM_LIMIT_HP=									0x02000000,	// Ѫ������ ( ����� Ѫ�������� )
		ITEM_LIMIT_MP=									0x04000000,	// �������� ( ����� ���������� )
		ITEM_LIMIT_ATT=									0x08000000,	// �������� ( ����� ������������ )
		ITEM_LIMIT_SP=									0x10000000,	// �ٶ����� ( ����� �ٶȵ������� )
		ITEM_LIMIT_CREDIT=								0x20000000	// �������� ( ����� �������� )
};


enum	ITEM_SPECIAL_CODE	// ��������
{
		ITEM_SPECIAL_NONE=								0,			// û�����ƴ���
		ITEM_SPECIAL_SERVICE=							0x10000,	// �߱��;� ( ����� �;���ֵ ) ������10��-1 ,��Ŵ���*10, show ��ʱ��/10
		ITEM_SPECIAL_MAGIC=								0x20000,	// �߱����� ( ����� ������ֵ ) ÿһ��ս�� -1
		ITEM_SPECIAL_USE=								0x40000		// �߱����� ( ����� ������ֵ ) ʹ��һ�� -1
};


enum	ITEM_EFFECT_CODE	// Ч������
{
		ITEM_EFFECT_NONE=								0,			// û��Ч��
		ITEM_EFFECT_HP=									1,			// ��Ѫ ( ֱ����ֵ )
		ITEM_EFFECT_HP_PERCENT=							2,			// ��Ѫ ( �ٷֱ� )
		ITEM_EFFECT_MP=									3,			// �ӷ� ( ֱ����ֵ )
		ITEM_EFFECT_MP_PERCENT=							4,			// �ӷ� ( �ٷֱ� )
		ITEM_EFFECT_HPMP=								5,			// ��Ѫ�� ( ֱ����ֵ )
		ITEM_EFFECT_HPMP_PERCENT=						6,			// ��Ѫ�� ( �ٷֱ� )
		ITEM_EFFECT_SPECIFY=							7,			// ����ָ�� ( ��ͬ����,��ͬ���� )

		ITEM_EFFECT_MAX_HP=								8,			// ���Ѫ
		ITEM_EFFECT_MAX_MP=								9,			// ���
		ITEM_EFFECT_ATT=								10,			// ������
		ITEM_EFFECT_SPEED=								11,			// �ٶ�
		ITEM_EFFECT_PROTECT=							12,			// ������

// ----
		ITEM_EFFECT_SHOOT=								100,		// ������
		ITEM_EFFECT_DODGE=								101,		// �����
		ITEM_EFFECT_SUCK=								102,		// ����
		ITEM_EFFECT_CREZY_PERCENT=						103,		// �񱩼���
		ITEM_EFFECT_CRAZY_VALUE=						104,		// ��Ч��
		ITEM_EFFECT_DEADLINESS_PERCENT=					105,		// ��������
		ITEM_EFFECT_DEADLINESS_VALUE=					106,		// ����Ч��
		ITEM_EFFECT_REDEADLINESS=						107,		// ����������
		ITEM_EFFECT_DOUBLEATT_PERCENT=					108,		// ��������
		ITEM_EFFECT_DOUBLEATT_VALUE=					109,		// ��������
		ITEM_EFFECT_BACKATT_PERCENT=					110,		// ��������
		ITEM_EFFECT_BACKATT_VALUE=						111,		// ��������
		ITEM_EFFECT_BREAK_PHYSICS_PERCENT=				112,		// ��������
		ITEM_EFFECT_BREAK_PHYSICS_VALUE=				113,		// ������̶�
		ITEM_EFFECT_MAGIC_FEEDBACK_PERCENT=				114,		// ������������
		ITEM_EFFECT_MAGIC_FEEDBACK_VALUE=				115,		// ���������̶�
		ITEM_EFFECT_PHYSICS_FEEDBACK_PERCENT=			116,		// ����������
		ITEM_EFFECT_PHYSICS_FEEDBACK_VALUE=				117,		// �������̶�
		ITEM_EFFECT_DEAD_PERCENT=						118,		// ���������漸��
		ITEM_EFFECT_MAGIC_DARK=							119,		// ���������̶�
		ITEM_EFFECT_MAGIC_LIGHT=						120,		// ���ⷨ���̶�
		ITEM_EFFECT_MAGIC_WATER=						121,		// ��ˮ�����̶�
		ITEM_EFFECT_MAGIC_FIRE=							122,		// �������̶�
		ITEM_EFFECT_PHYSICS=							123,		// ������̶�
		ITEM_EFFECT_VIRUS=								124,		// ���ж��̶�
		ITEM_EFFECT_CHAOS=								125,		// �����ҳ̶�
		ITEM_EFFECT_STONE=								126,		// ��ʯ���̶�
		ITEM_EFFECT_SLEEP=								127,		// ����˯�̶�
		ITEM_EFFECT_WEEK=								128,		// ��˥�ϳ̶�

// ----	
		ITEM_EFFECT_PLAYER_EXP=							200,		// player ����
		ITEM_EFFECT_PLAYER_MAGIC=						201,		// ��������
		ITEM_EFFECT_PLAYER_HP_POINT=					202,		// ���Ѫ����
		ITEM_EFFECT_PLAYER_MP_POINT=					203,		// ��ҷ�����
		ITEM_EFFECT_PLAYER_ATT_POINT=					204,		// �����������
		ITEM_EFFECT_PLAYER_SP_POINT=					205,		// ����ٶȵ���
		
// ----
		ITEM_EFFECT_BAOBAO_EXP=							300,		// ��������
		ITEM_EFFECT_BAOBAO_LOYALITY=					301,		// �ҳ϶�
		ITEM_EFFECT_BAOBAO_CLOSED=						302,		// ���ܶ�
		ITEM_EFFECT_BAOBAO_HP_POINT=					303,		// ����Ѫ����
		ITEM_EFFECT_BAOBAO_MP_POINT=					304,		// ����������
		ITEM_EFFECT_BAOBAO_ATT_POINT=					305,		// ������������
		ITEM_EFFECT_BAOBAO_SP_POINT=					306,		// �����ٶȵ���
		ITEM_EFFECT_BAOBAO_GROW=						307,		// �����ɳ�ϵ��

// ---- 
		ITEM_EFFECT_SERVICE=							400,		// �;�
		ITEM_EFFECT_MAGIC=								401,		// ����
		ITEM_EFFECT_USE=								402			// ����

};



enum	SKILL_EFFECT_CODE	// ��������
{
// --- skill for ����
		SKILL_EFFECT_STONE_MAGIC=						0,			// ʯ������
		SKILL_EFFECT_CHAOS_MAGIC=						1,			// ����
		SKILL_EFFECT_SLEEP_MAGIC=						2,			// ��˯
		SKILL_EFFECT_VIRUS_MAGIC=						3,			// �ж�
		SKILL_EFFECT_WEEK_MAGIC=						4,			// ˥��
	
// --- skill for ����
		SKILL_EFFECT_ATTACK_MAGIC=						10,			// �ӹ���
		SKILL_EFFECT_FASTNESS_MAGIC=					11,			// �ӿ���
		SKILL_EFFECT_MULTY_MAGIC=						12,			// ��������
		SKILL_EFFECT_SPEED_MAGIC=						13,			// ����
		SKILL_EFFECT_RESCUE_MAGIC=						14,			// �ָ�

// --- skill for ��ʦ
		SKILL_EFFECT_DARK_MAGIC=						20,			// �ڰ�ϵ��������
		SKILL_EFFECT_LIGHT_MAGIC=						21,			// ����ϵ��������
		SKILL_EFFECT_WATER_MAGIC=						22,			// ˮϵ��������
		SKILL_EFFECT_FIRE_MAGIC=						23,			// ��ϵ��������
		SKILL_EFFECT_DEAD_MAGIC=						24,			// ��������

// --- skill for ��е
		SKILL_EFFECT_SUCK_MAGIC=						30,			// ��
		SKILL_EFFECT_PHYSICS_FEEDBACK_MAGIC=			31,			// �������
		SKILL_EFFECT_MAGIC_FEEDBACK_MAGIC=				32,			// ��������
		SKILL_EFFECT_LOCK_MAGIC=						33,			// ����
		SKILL_EFFECT_BREAK_MAGIC=						34			// ����쳣״̬
		
};


enum	TITLE_TYPE_CODE	// ͷ������
{
		TITLE_TYPE_NONE=								0,			// û����˼
		TITLE_TYPE_MACRO=								1,			// ����ͷ��
		TITLE_TYPE_GROUP=								2,			// ����ͷ��
		TITLE_TYPE_MARRAGE=								3,			// ����ͷ��
		TITLE_TYPE_GAME=								4,			// ����ͷ��
		TITLE_TYPE_HOLD=								5			// ����ͷ��

};



enum	ITEM_INST_CODE
{
		ITEM_INST_POCKET=								0,			// ��ҿڴ�
		ITEM_INST_DEAL_OTHER_ITEM=						1,			// ����ʱ���ҷ������б�
		ITEM_INST_DEAL_OTHER_BAOBAO=					2,			// ����ʱ��Է������б�
		ITEM_INST_POPSHOP=								3,			// ����
		ITEM_INST_SELL_STORE=							4,			// ���������̵�
		ITEM_INST_EQUIP=								5,			// װ�� 
		ITEM_INST_MY_BAOBAO=							6,			// �Լ��ı���(���������ô���)
		ITEM_INST_SKILL=								7,			// ����˵��

		ITEM_INST_HOUSEKEEPER=							10			// �ܼ� 
		
};



/*******************************************************************************************************************}
{	���Ի�����ֵ																									}
********************************************************************************************************************/
typedef struct PUBLIC_FASTNESS_DATA_STRUCT			// ��������
{
	SHINT protect;									// ������
	SHINT shoot;									// ������
	SHINT dodge;									// �����
	SHINT suck;										// ����
	SHINT crazy_percent;							// �񱩼���
	SHINT crazy_value;								// ��Ч��
	SHINT deadliness_percent;						// ��������
	SHINT deadliness_value;							// ����Ч��
	SHINT redeadliness_percent;						// ����������
	SHINT doubleatt_percent;						// ��������
	SHINT doubleatt_times;							// ��������
	SHINT backatt_percent;							// ��������
	SHINT backatt_times;							// ��������
	SHINT break_physics_percent;					// ��������
	SHINT break_physics_value;						// ������̶�
	SHINT magic_feedback_percent;					// ������������
	SHINT magic_feedback_value;						// ���������̶�
	SHINT physics_feedback_percent;					// ����������
	SHINT physics_feedback_value;					// �������̶�
	SHINT dead_percent;								// ���������漸��
	SHINT magic_dark;								// ���������̶�
	SHINT magic_light;								// ���ⷨ���̶�
	SHINT magic_water;								// ��ˮ�����̶�
	SHINT magic_fire;								// �������̶�
	SHINT physics;									// ������̶�
	SHINT virus;									// ���ж��̶�
	SHINT chaos;									// �����ҳ̶�
	SHINT stone;									// ��ʯ���̶�
	SHINT sleep;									// ����˯�̶�
	SHINT week;										// ��˥�ϳ̶�

} PUBLIC_FASTNESS_DATA;



/*******************************************************************************************************************}
{	��Ʒ����װ��������ֵ																							}
********************************************************************************************************************/
// --- ����Ҫ������ѯ��ʱ��
typedef struct PUBLIC_ITEM_BASE_STRUCT				// ������Ʒ��������
{
	// ---- ��������	
	UCHR name[MAX_ITEM_NAME_LENGTH+1];				// ����
	UCHR inst[MAX_ITEM_INST_LENGTH+1];				// ��Ʒ����
	SHINT item_type;								// ��Ʒ���� ( see ITEM_TYPE enum  )
	SHINT level;									// ��Ʒ�ȼ�
	SHINT priority;									// ����˳�� ( ����ԽСԽ����ʹ��)
	
	// ---- �������
	SHINT store_type;								// ������� ( see ITEM_STORE enum )

	
	// ---- ʹ������
	SHINT use_type;									// ʹ������ ( see ITEM_USE enum )
	SHINT use_position;								// ʹ�ò�λ ( see ITEM_POSITION enum )
	SHINT use_phyle;								// ʹ��������� ( see ITEM_PHYLE enum )

	ULONG use_limit;								// ʹ������ ( see ITEM_LIMIT enum )
	ULONG use_special;								// �������� ( see ITEM_SPECIAL enum )
	
	// ---- Ч������
	SHINT effect_type[3];							// Ч������ ( see ITEM_EFFECT enum ) ����Ч��0+����Ч��1..3
	SLONG effect_value[3];							// Ч����ֵ

	SHINT effect_stone_type[3];						// ʯͷЧ������ ( see ITEM_EFFECT enum ) ����Ч��0+����Ч��1..3
	SLONG effect_stone_value[3];					// ʯͷЧ����ֵ
	
} PUBLIC_ITEM_BASE;

// ----- ��һ����,�ڴ�ֱ�� update ----------
typedef struct PUBLIC_ITEM_DATA_STRUCT				// ��Ʒ��������
{
	SLONG item_id;									// < 0 ����û������ >= 0 ��������
	SHINT number;									// ��Ʒ����
// ---- ���ò���
	SHINT filename_id;								// ��Ʒʹ�� MDA ���� id
	SHINT single_price;								// ��Ʒ����
	
} PUBLIC_ITEM_DATA;




/*******************************************************************************************************************}
{	���ܻ�����ֵ																									}
********************************************************************************************************************/
typedef struct PUBLIC_SKILL_DATA_STRUCT				// ��������
{
	char idx;										// ���� idx
	char level;										// �����ȼ�
	SHINT lithhand;									// ������
	char action;									// ���ö���
	SHINT need_mp;									// ����ҪMP
} PUBLIC_SKILL_DATA;





/*******************************************************************************************************************}
{	ͷ�λ�����ֵ																									}
********************************************************************************************************************/
typedef struct PUBLIC_TITLE_BASE_STRUCT				// ͷ�λ�������
{
	UCHR name[MAX_TITLE_LENGTH+1];					// ����
	UCHR inst[MAX_TITLE_INST_LENGTH+1];				// ����
	SHINT type;										// ������ --> Macro ������������ ( see TITLE_TYPE snum )

} PUBLIC_TITLE_BASE;


typedef struct PUBLIC_TITLE_DATA_STRUCT				// ͷ������
{
	PUBLIC_TITLE_BASE data;
} PUBLIC_TITLE_DATA;



typedef struct PUBLIC_GROUP_DATA_STRUCT				// ��������
{

} PUBLIC_GROUP_DATA;



/*******************************************************************************************************************}
{	�� �� �� ֵ �� ��																								}
********************************************************************************************************************/
/*******************************************************************************************************************}
{	����������ֵ																									}
********************************************************************************************************************/
typedef struct SERVER_CHARACTER_FINAL_STRUCT		// 
{
	ULONG level;									// �ȼ�
	SLONG max_hp;									// ��� HP
	SLONG now_hp;									// ��ǰ HP
	SLONG max_mp;									// ��� MP
	SLONG now_mp;									// ��ǰ MP
	SLONG att;										// ��ǰ������
	SLONG speed;									// ��ǰ�ٶ�
	unsigned long total_exp;						// Ŀǰ�ܾ���
	unsigned long now_exp;							// ��ǰ����
	unsigned long need_exp;							// ��������

	SLONG filename_id;								// Ŀǰ filename id (���ܱ���)
	
} SERVER_CHARACTER_FINAL;

/*******************************************************************************************************************}
{	���������ֵ																									}
********************************************************************************************************************/
typedef struct SERVER_CHARACTER_BASE_STRUCT			// 
{
	SHINT point;									// Ŀǰ���Է������

	SLONG base_hp;									// ���� HP
	SHINT hp_point;									// HP ����
	float hp_grow;									// HP �ɳ�ϵ��

	SLONG base_mp;									// ���� MP
	SHINT mp_point;									// MP ����
	float mp_grow;									// MP �ɳ�ϵ��

	SLONG base_att;									// ���� ATT
	SHINT att_point;								// ATT ����
	float att_grow;									// ATT �ɳ�ϵ��

	SLONG base_speed;								// ���� SPEED
	SHINT speed_point;								// SPEED ����
	float speed_grow;								// SPEED �ɳ�ϵ��

	SLONG filename_id;								// ����ʱ�� MDA filename_id;
	SHINT base_point;								// �����͵�
	char life_base;									// Ŀǰ��ת

} SERVER_CHATACTER_BASE;


/*******************************************************************************************************************}
{	���￹����ֵ																									}
********************************************************************************************************************/
typedef struct SERVER_CHARACTER_FASTNESS_STRUCT		// 
{
	bool active;									// ���� ready 
	PUBLIC_FASTNESS_DATA fastness;					// ��������
} SERVER_CHARACTER_FASTNESS;


/*******************************************************************************************************************}
{	����ڴ�����																									}
********************************************************************************************************************/
typedef struct SERVER_CHARACTER_POCKET_STRUCT		// 
{
	bool active;									// ���� ready 
	SLONG idx;										// Ŀǰ�ڴ� idx
// --- ����Ʒ
	SLONG sell_number;								// ��������
// --- give
	SLONG give_idx[3];								// ���� idx
	SLONG give_number[3];							// ��������
	SLONG give_money;								// �����Ǯ
// --- deal
// �ҷ�����
	bool my_deal_ready;								// ���� ok
	SLONG my_deal_idx[3];							// ���� idx
	SLONG my_deal_number[3];						// ��������
	SLONG my_deal_money;							// ���׽�Ǯ
// �Է�����
	bool other_deal_ready;							// ���� ok
	SLONG other_deal_filename_id[3];				// �Է���Ʒ filename_id
	SLONG other_deal_number[3];						// ��������
	SLONG other_deal_money;							// ���׽�Ǯ


// --- body 	
	PUBLIC_ITEM_DATA item[MAX_POCKET_ITEM];			// ��������
} SERVER_CHARACTER_POCKER;


/*******************************************************************************************************************}
{	����װ������																									}
********************************************************************************************************************/
typedef struct SERVER_CHARACTER_EQUIP_STRUCT		// 
{
	bool active;									// ���� ready 
	SLONG idx;										// �ڴ� idx
	PUBLIC_ITEM_DATA equip[MAX_EQUIP_ITEM];			// ����װ������
} SERVER_CHARACTER_EQUIP;


/*******************************************************************************************************************}
{	���ﵱ������																									}
********************************************************************************************************************/
typedef struct SERVER_CHARACTER_POPSHOP_STRUCT		// 
{
	bool active;									// ���� ready 
	SLONG idx;										// Ŀǰ ���� idx
	SLONG sell_number;
	PUBLIC_ITEM_DATA item[MAX_POPSHOP_ITEM];		// ��������
} SERVER_CHARACTER_POPSHOP;


/*******************************************************************************************************************}
{	������������																									}
********************************************************************************************************************/
typedef struct SERVER_CHARACTER_BANK_STRUCT			// 
{
	bool active;									// ���� ready 
	SLONG now_money;								// �ڴ���Ǯ����
	SLONG bank_money;								// �������
	SLONG max_money;								// ��Ǯ����
} SERVER_CHARACTER_BANK;


/*******************************************************************************************************************}
{	������������																									}
********************************************************************************************************************/
typedef struct SERVER_CHARACTER_OTHER_STRUCT		// 
{
	SLONG credit;									// ����
	SLONG dead_count;								// ��������
	SLONG kind;										// �ƶ�
	SLONG military;									// ս��
	SLONG group_id;									// ���빤�� id
	
} SERVER_CHARACTER_OTHER;


/*******************************************************************************************************************}
{	�����̵�����																									}
********************************************************************************************************************/
typedef struct SERVER_STORE_DATA_STRUCT
{ 
	bool active;
	SLONG idx;
	SLONG buy_number;
	SLONG total;
	PUBLIC_ITEM_DATA item[MAX_POCKET_ITEM];			// ��������, ����� number -> ��������
} SERVER_STORE_DATA;



/*******************************************************************************************************************}
{	��Ʒ˵������																									}
********************************************************************************************************************/
typedef struct CLIENT_ITEM_INST_STRUCT
{
	bool active;									// �����Ƿ� ready 
	char type;										// �������� see enum ( ITEM_INST_CODE )
	char idx;										// �ڴ����
	char old_idx;
	char inst[MAX_DETAIL_INST_LENGTH];				// ˵������
	ULONG timer;									// request timer;
	SLONG handle;									// mda handle
} CLIENT_ITEM_INST;


/*******************************************************************************************************************}
{	�� �� �� ֵ �� ��																								}
********************************************************************************************************************/
/*******************************************************************************************************************}
{	����������ֵ																									}
********************************************************************************************************************/
typedef struct SERVER_BAOBAO_FINAL_STRUCT		// 
{
	ULONG level;									// �ȼ�
	ULONG max_hp;									// ��� HP
	ULONG now_hp;									// ��ǰ HP
	ULONG max_mp;									// ��� MP
	ULONG now_mp;									// ��ǰ MP
	ULONG att;										// ��ǰ������
	ULONG speed;									// ��ǰ�ٶ�
	unsigned long total_exp;						// Ŀǰ�ܾ���
	unsigned long now_exp;							// ��ǰ����
	unsigned long need_exp;							// ��������
	
} SERVER_BAOBAO_FINAL;


/*******************************************************************************************************************}
{	����������ֵ																									}
********************************************************************************************************************/
typedef struct SERVER_BAOBAO_BASE_STRUCT			// 
{
	SLONG point;									// Ŀǰ���Է������

	SLONG base_hp;									// ���� HP
	SHINT hp_point;									// HP ����

	SLONG base_mp;									// ���� MP
	SHINT mp_point;									// MP ����

	SLONG base_att;									// ���� ATT
	SHINT att_point;								// ATT ����

	SLONG base_speed;								// ���� SPEED
	SHINT speed_point;								// SPEED ����

	float grow;										// �ɳ�ϵ��
	SHINT phyle;									// ϵ��
	SHINT filename_id;
	char life_base;									// Ŀǰ��ת
		
} SERVER_BAOBAO_BASE;


/*******************************************************************************************************************}
{	����������ֵ																									}
********************************************************************************************************************/
typedef struct SERVER_BAOBAO_FASTNESS_STRUCT		// 
{
	bool active;									// ���� ready 
	PUBLIC_FASTNESS_DATA fastness;					// ��������
} SERVER_BAOBAO_FASTNESS;



/*******************************************************************************************************************}
{	����������������																								}
********************************************************************************************************************/
typedef struct SERVER_CHARACTER_BAOBAO_STRUCT
{
	ULONG baobao_id;								// ���� ID ( 0 ����û������ )
	char name[MAX_BAOBAO_NAME_LENGTH+1];			// ����(����Զ�������)
	SHINT loyalty;									// �ҳ�
	SLONG closed;									// ���ܶ�
	SERVER_BAOBAO_FINAL data;						// ������ֵ
	SERVER_BAOBAO_BASE base;						// ����������������

} SERVER_CHARACTER_BAOBAO;


typedef struct SERVER_CHARACTER_BAOBAO_STACK_STRUCT
{
	ULONG action_baobao_id;									// Ŀǰ���������� ID
	SHINT active_idx;										// Ŀǰ�������� idx
	bool show[MAX_BAOBAO_STACK];							// �����Ƿ���ʾ
	SERVER_CHARACTER_BAOBAO baobao_list[MAX_BAOBAO_STACK];	// �����ı�������

} SERVER_CHARACTER_BAOBAO_STACK;









/*******************************************************************************************************************}
{																													}
{																													}
{																													}
{	Client �� ֵ �� ��																								}
{																													}
{																													}
{																													}
********************************************************************************************************************/
/*******************************************************************************************************************}
{																													}
{	�� �� �� ֵ �� ��																								}
{																													}
********************************************************************************************************************/
typedef struct CLIENT_CHARACTER_MAP_WINDOW_STRUCT	// ��ͼģʽ window Ҫ�õ�����
{
	char group[MAX_GROUP_NAME_LENGTH+1];			// ��������
	SERVER_CHARACTER_FINAL final;					// ���� final ����	
	SERVER_CHATACTER_BASE base;						// �����������
	SERVER_CHARACTER_OTHER other;					// ������������
	
} CLIENT_CHARACTER_MAP_WINDOW;



typedef struct CLIENT_CHARACTER_POINT_BACK_STRUCT
{
	SHINT total_point;
	SHINT hp_point;
	SHINT mp_point;
	SHINT att_point;
	SHINT speed_point;

} CLIENT_CHARACTER_POINT_BACK;


typedef struct CLIENT_UPDATE_EXP_STRUCT
{
	unsigned long total_exp;						// Ŀǰ�ܾ���
	unsigned long now_exp;							// ��ǰ����
	unsigned long need_exp;							// ��������
	
} CLIENT_UPDATE_EXP;


typedef struct CLIENT_UPDATE_POINT_STRUCT
{
	SHINT point;									// Ŀǰ���Է������
	SHINT hp_point;									// HP ����
	SHINT mp_point;									// MP ����
	SHINT att_point;								// ATT ����
	SHINT speed_point;								// SPEED ����
	
} CLIENT_UPDATE_POINT;



/*******************************************************************************************************************}
{																													}
{	�� �� �� ֵ �� ��																								}
{																													}
********************************************************************************************************************/

typedef struct CLIENT_BAOBAO_SHOW_STRUCT
{
	ULONG id;
	ULONG follow_id;								// ���� id
	SHINT filename_id;								// ���� filename id
	char life_base;									// ת������
	char name[MAX_BAOBAO_NAME_LENGTH+1];			// ����(����Զ�������)
	
} CLIENT_BAOBAO_SHOW;










/*******************************************************************************************************************}
{																													}
{																													}
{																													}
{	ս �� �� ��																										}
{																													}
{																													}
{																													}
********************************************************************************************************************/
/* ����˵������:

	step 1: ����ս��
		1-1. Server ֪ͨ����ս�������� clear_fight_npc_group(), ���� FIGHT_BASE_DATA ( ���20��), ���� insert_fight_unit();
					���� fill ��ս���� skill,-> baobao_skill_data[];
		1-2. Server ֪ͨ����ս�� ���� change_to_fight();  ����
		1-3. client show ��һ�غ�������ȴ�
		1-4. client ���� FIGHT_READY
		1-5. server ֪ͨ��翪ʼ fight_charge_start();

	step 2: ÿ�غϳ�����
		2-1. client ������,���� fight_command_data ,���� client ֹͣ���
		
	step 3:	�ȴ���������
		3.0 server ���������˳����ͣ
		3.1 server ���� FIGHT_STATUS_RESULT ����Ŀǰս��״̬ update_fight_status_result();
		3.1 server ���� FIGHT_ACTION_RESULT					 update_fight_action_result();
		3.2 server ���� FIGHT_VALUE_RESULT					 update_fight_value_result();
		3.3 server ���� ACTION ��ʼ ( ���� client ս��)
		3.4 client ���Ͳ������

	step 4: ȫ���˲������,���� client time out( ��첥�����+ 3 sec Ϊ time_out ʱ��)
		4-1. server �ͳ�֪ͨ������
		
	step 5: ս������
		5-1. server ����ս������
		5-2. ���� change_to_map() ����

	step 6: server ���ͽ���
		6-1. server ���;��������������
		6-2. ����ս��




 
// --------- ����˵��
   FIGHT_ACTION_RESULT
   FIGHT_VALUE_RESULT

  source_id= ������ ID,�Ͳ�˵����


*****   ������������   ********
( 0 - 0 ) FIGHT_ACTION_SINGLE_PHYSICS				�������� ( �� feedback,����, �� )

	target_id= ������� ID

	FIGHT_ACTION_SINGLE_PHYSICS_DATA
	{
		SLONG hp;										// ����Ѫ��
		SLONG feed_back_hp;								// ����Ѫ��
		ULONG protect_id;								// ���� id, id= 0 û���˱���
		SLONG back_hp;									// ����Ѫ�� > 0 ��ʱ��������
	}


=============================================================================================
( 0 - 1 ) FIGHT_ACTION_SINGLE_PHYSICS_DOUBLE		�������� ( �� feedback, �������� )

	target_id= ������� ID
  
	FIGHT_ACTION_SINGLE_PHYSICS_DOUBLE_DATA
	{
		char double_count;								// ��������
		SLONG hp[MAX_PHYSICS_DOUBLE_TIMES];				// ÿ��Ѫ��
		SLONG feedback[MAX_PHYSICS_DOUBLE_TIMES];		// ÿ�η���
		SLONG back_hp[MAX_PHYSICS_DOUBLE_TIMES];		// ����Ѫ��
	}

=============================================================================================
( 0 - 3 ) FIGHT_ACTION_SINGLE_MAGIC				���������� ( �� feedback, �� )

	target_id= ������� ID
	idx= ���� ID
	FIGHT_ACTION_SINGLE_MAGIC_DATA
	{
		SLONG hp;										// �������� HP
		SLONG feed_back_hp;								// ����Ѫ��
	}


=============================================================================================
( 0 - 4 ) FIGHT_ACTION_SINGLE_MAGIC_DOUBLE			�������������� ( �� feedback, �� )

	target_id= ������� ID
	idx= ���� ID
	FIGHT_ACTION_SINGLE_MAGIC_DOUBLE_DATA
	{
		char double_count;								// ��������
		SLONG hp[MAX_PHYSICS_DOUBLE_TIMES];				// ÿ��Ѫ��
		SLONG feedback[MAX_PHYSICS_DOUBLE_TIMES];		// ÿ�η���
	}


=============================================================================================
( 0 - 5 ) FIGHT_ACTION_ITEM							ʹ�õ���
	target_id= ������� ID



=============================================================================================
( 0 - 6 ) FIGHT_ACTION_RUN							����
	target_id= ������� ID
	idx=0 -> ����ʧ��, 1-> ���ܳɹ�


=============================================================================================
( 0 - 7 ) FIGHT_ACTION_CHANGE						��������
	target_id= ���������� id



=============================================================================================
( 0 - 8 ) FIGHT_ACTION_SINGLE_VIRUS					�ж�����
	target_id= ������� ID

		FIGHT_ACTION_SINGLE_VIRUS_DATA
		{
			SLONG hp;										// ��Ѫ��
			SLONG loop_hp;									// ��غϵ�Ѫ��
		}


=============================================================================================
( 0 - 9 ) FIGHT_ACTION_SINGLE_STATUS				״̬����(ʯ��,��,˯,��������.... )
	target_id= ������� ID
		
	״̬���� fight_value_result ����		
		  
			
			  
// ������������
// ---- �������ܲ��� 
				
		FIGHT_ACTION_MULTI_MAGIC=						100,	// �����๥�� ( �� feedback, �� )
		FIGHT_ACTION_MULTI_MAGIC_DOUBLE=				101,	// ���������๥�� ( �� feedback, �� )
				  
		FIGHT_ACTION_MULTI_STATUS=						102,	// ״̬����(ʯ��,��,˯,��������.... )
					




*/


enum	FIGHT_UNIT_TYPE_CODE
{
	FIGHT_UNIT_TYPE_CHARACTER=						0,		// �������
	FIGHT_UNIT_TYPE_BAOBAO=							1,		// ����
	FIGHT_UNIT_TYPE_MONSTER=						2,		// С����
	FIGHT_UNIT_TYPE_BOSS=							3,		// BOSS ����
	FIGHT_UNIT_TYPE_AVATAR=							4		// ��ұ��� ( ʹ�����ֻ� BOSS �� MDA )
};


enum	FIGHT_STATUS_CODE
{
	FIGHT_STATUS_NORMAL=							0x0000,	// ����
	FIGHT_STATUS_VIRUS=								0x0001,	// �ж�
	FIGHT_STATUS_SLEEP=								0x0002,	// ��˯
	FIGHT_STATUS_STONE=								0x0004,	// ʯ��
	FIGHT_STATUS_WEEK=								0x0008,	// ˥��
	FIGHT_STATUS_CHAOS=								0x0010,	// ����
	FIGHT_STATUS_PLAGUE=							0x0020,	// ����
	FIGHT_STATUS_DARK=								0x0040,	// �ڰ�
	FIGHT_STATUS_DELAY=								0x0080,	// �ٻ�
	FIGHT_STATUS_HURT=								0x0100,	// �˺�����
	FIGHT_STATUS_DEAD_CALL=							0x0200,	// ��������
	FIGHT_STATUS_POWER=								0x0400,	// ����
	FIGHT_STATUS_FASTNESS=							0x0800,	// �Ό
	FIGHT_STATUS_SPEED=								0x1000,	// ����
	FIGHT_STATUS_PHYSICS_FEEDBACK=					0x2000,	// ������
	FIGHT_STATUS_MAGIC_FEEDBACK=					0x4000, // ��������
	FIGHT_STATUS_DEAD=								0x8000,	// ս������

	FIGHT_STATUS_LEAVE=								0x10000,
	FIGHT_STATUS_DEFEND=							0x20000,
};



enum	FIGHT_COMMAND_CODE
{
	FIGHT_COMMAND_ATTACK=							0,		// ������
	FIGHT_COMMAND_MAGIC=							1,		// ��������
	FIGHT_COMMAND_ITEM=								2,		// ʹ�õ���
	FIGHT_COMMAND_PROTECT=							3,		// ����
	FIGHT_COMMAND_RECOVER=							4,		// ����
	FIGHT_COMMAND_RUN=								5,		// ����
	FIGHT_COMMAND_CHANGE=							6,		// ��������
	FIGHT_COMMAND_AUTO_HP=							7,		// �Զ���Ѫ
	FIGHT_COMMAND_AUTO_MP=							8,		// �Զ�����
	FIGHT_COMMAND_AUTO_ATTACK=						9		// �Զ�����

};


enum	FIGHT_ACTION_CODE
{
// --- ������������
	FIGHT_ACTION_SINGLE_PHYSICS=					0,		// �������� ( �� feedback,����, �� )
	FIGHT_ACTION_SINGLE_PHYSICS_DOUBLE=				1,		// �������� ( �� feedback, �������� )

	FIGHT_ACTION_SINGLE_MAGIC=						2,		// ���������� ( �� feedback, �� )
	FIGHT_ACTION_SINGLE_MAGIC_DOUBLE=				3,		// �������������� ( �� feedback, �� )

	FIGHT_ACTION_ITEM=								4,		// ���ߵ���
	FIGHT_ACTION_RUN=								5,		// ���ܵ���
	FIGHT_ACTION_CHANGE=							6,		// ������������

	FIGHT_ACTION_SINGLE_STATUS=						7,		// ״̬����(ʯ��,��,˯,��������.... )




// ---- �������ܲ���
	FIGHT_ACTION_MULTI_MAGIC=						100,	// �����๥�� ( �� feedback, �� )
	FIGHT_ACTION_MULTI_MAGIC_DOUBLE=				101,	// ���������๥�� ( �� feedback, �� )

	FIGHT_ACTION_MULTI_STATUS=						102		// ״̬����(ʯ��,��,˯,��������.... )
	

};



enum	CHARGE_TIME_CONTROL_CODE
{
	CHARGE_TIME_INITIALIZE=							0,		// ��� ���ʱ��
	CHARGE_TIME_PAUSE=								1,		// ��ͣ���
	CHARGE_TIME_CONTINUE=							2,		// �������
	CHARGE_TIME_RESET=								3		// ���¿�ʼ
};



// ------------------
// ------ ������ҵ�����ҲҪ����,�����н���ս���ı�����ֵЧ��...
// ------------------
// --- ��һ�ν��� request 
typedef struct FIGHT_BASE_DATA_STRUCT				// ս��������Ԫ����
{
	char active;									// Ŀǰ�Ƿ���ս������
	ULONG id;										// ��Ԫ�� id
	char name[MAX_NAME_LENGTH+1];					// �ǳ�
	SLONG max_hp;									// ��� HP
	SLONG now_hp;									// Ŀǰ HP
	SLONG max_mp;									// ��� MP
	SLONG now_mp;									// Ŀǰ MP
	char charge_speed;								// Ŀǰ�����
	SHINT filename_id;								// ս�� MDA ���
	char unit_type;									// ս���������� see enum ( FIGHT_UNIT_TYPE_CODE )
	UHINT status;									// Ŀǰ����״̬ see enum ( FIGHT_STATUS_CODE )
	char life_base;									// ת������( show ���� color )
	
} FIGHT_BASE_DATA;


// --- ( 0 - 0 ) ÿһ�غϿ�ʼ ���� ACTION_READY, ���ҵȴ� STATUS_RESULT �ظ�
typedef struct FIGHT_STATUS_RESULT_STRUCT			// ���ܵ�ս��״̬���
{
	ULONG target_id;								// ���������
	UHINT status;									// ״̬��ֵ
	SLONG hp;										// �ж���ʱ�����ֵ
} FIGHT_STATUS_RESULT;


// --- ( 0 - 1 ) ���Ͷ�������
typedef struct FIGHT_COMMAND_DATA_STRUCT			// ���͵�ս������
{
	char command;									// ��Ҫ�������� see enum ( FIGHT_COMMAND_CODE )
	ULONG source_id;								// �������� ID
	ULONG target_id;								// �������� ID ( ��������,����,BOSS,NPC...)
	char type_no;									// ���ܱ��,�ڴ����
	
} FIGHT_COMMAND_DATA;



// --- ( 1 - 0 ) ���ܶ������ ( ƽ�����п���, һ������,ֹͣ�����˳��,ִ�н�� )
typedef struct FIGHT_ACTION_RESULE_STRUCT
{
	char command;									// �������� see enum ( FIGHT_ACTION_CODE )
	ULONG source_id;								// ������ id
	ULONG target_id;								// �ܶ����� id ����ж���Ķ���,�˲�����Ч ( �ջ����� ID )
	char idx;										// �������� ( �������...�ȵ�),����ʱ�� ( 0-> ���ɹ�,1-> �ɹ�)
} FIGHT_ACTION_RESULT;								// ���Ϸ��� ( 9 - 0 ) �Ժ���


// --- ( 1 - 1 ) ����ս����ֵ���
typedef struct FIGHT_VALUE_RESULT_STRUCT			// ���ܵ�ս����ֵ���
{													
	ULONG id;										// ������ id
	SLONG max_hp;									// ��� HP
	SLONG now_hp;									// Ŀǰ HP
	SLONG max_mp;									// ��� MP
	SLONG now_mp;									// Ŀǰ MP
	char charge_speed;								// Ŀǰ�ٶ�
	UHINT status;									// Ŀǰ״̬

} FIGHT_VALUE_RESULT;



// --- ( 9 - 9 ) Fight Action ����Ҫ�����Ͻṹ
typedef struct FIGHT_ACTION_SINGLE_PHYSICS_DATA_STRUCT
{													// �������� ( �� feedback,����, �� )
	SLONG hp;										// ����Ѫ��
	SLONG feed_back_hp;								// ����Ѫ��
	ULONG protect_id;								// ���� id, id= 0 û���˱���
	SLONG back_hp;									// ����Ѫ�� > 0 ��ʱ��������
} FIGHT_ACTION_SINGLE_PHYSICS_DATA;



typedef struct FIGHT_ACTION_SINGLE_PHYSICS_DOUBLE_DATA_STRUCT
{													// �������� ( �� feedback, �������� )
	char double_count;								// ��������
	SLONG hp[MAX_PHYSICS_DOUBLE_TIMES];				// ÿ��Ѫ��
	SLONG feedback[MAX_PHYSICS_DOUBLE_TIMES];		// ÿ�η���
	SLONG back_hp[MAX_PHYSICS_DOUBLE_TIMES];		// ����Ѫ��
} FIGHT_ACTION_SINGLE_PHYSICS_DOUBLE_DATA;



typedef struct FIGHT_ACTION_SINGLE_MAGIC_DATA_STRUCT
{													// ���������� ( �� feedback, �� )
	SLONG hp;										// �������� HP
	SLONG feed_back_hp;								// ����Ѫ��
} FIGHT_ACTION_SINGLE_MAGIC_DATA;



typedef struct FIGHT_ACTION_SINGLE_MAGIC_DOUBLE_DATA_STRUCT
{													// �������������� ( �� feedback, �� )
	char double_count;								// ��������
	SLONG hp[MAX_PHYSICS_DOUBLE_TIMES];				// ÿ��Ѫ��
	SLONG feedback[MAX_PHYSICS_DOUBLE_TIMES];		// ÿ�η���
} FIGHT_ACTION_SINGLE_MAGIC_DOUBLE_DATA;



typedef struct FIGHT_ACTION_CHANGE_BAOBAO_DATA_STRUCT
{
	char name[MAX_NAME_LENGTH+1];					// �ǳ�
	SHINT filename_id;								// ս�� MDA ���
	char unit_type;									// ս���������� see enum ( FIGHT_UNIT_TYPE_CODE )
	char life_base;									// ת������( show ���� color )
	FIGHT_VALUE_RESULT data;

} FIGHT_ACTION_CHANGE_BAOBAO_DATA;


// ---- ���˼��ܲ���
typedef struct FIGHT_ACTION_MULTI_MAGIC_DATA_STRUCT
{													// �����๥�� ( �� feedback, �� )
	char number;									// ��������
	ULONG id[MAX_FIGHT_UNIT];						// ��������id
	SLONG hp[MAX_FIGHT_UNIT];						// ����Ѫ��
	SLONG feed_back_hp[MAX_FIGHT_UNIT];				// ����Ѫ��
} FIGHT_ACTION_MULTI_MAGIC_DATA;



typedef struct FIGHT_ACTION_MULTI_MAGIC_DOUBLE_DATA_STRUCT
{													// ���������๥�� ( �� feedback, �� )
	char number;									// ��������
	char double_count;								// ��������
	ULONG id[MAX_FIGHT_UNIT];						// ��������id
	SLONG hp[MAX_FIGHT_UNIT][MAX_PHYSICS_DOUBLE_TIMES];	// ����Ѫ��
	SLONG feed_back_hp[MAX_FIGHT_UNIT][MAX_PHYSICS_DOUBLE_TIMES];	// ����Ѫ��
} FIGHT_ACTION_MULTI_MAGIC_DOUBLE_DATA;



#define CHARGE_DELAY_TIME							3000		// �������ʱ����ӳ�ʱ�� 3 ��

typedef struct FIGHT_CHARGE_TIME_DATA_STRUCT
{
	ULONG charge_start_time;						// ��ʼ���ʱ��
	ULONG charge_time;								// �ܹ���Ҫ���ʱ�� (�䵽 100%) ����Ҫ��ʱ��
	ULONG delay_time;								// �������ʱ����ӳ� conut 
} FIGHT_CHARGE_TIME_DATA;


typedef struct FIGHT_BAOBAO_SKILL_DATA_STRUCT		// ��ǰ���� ���ܱ�
{
	char skill_id;
} FIGHT_BAOBAO_SKILL_DATA;




#endif