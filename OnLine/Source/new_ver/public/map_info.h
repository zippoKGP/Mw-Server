/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
 2005 ALL RIGHTS RESERVED.

*file name    : map_info.h
*owner        : Stephen
*description  : 
*modified     : 2005/1/17
******************************************************************************/ 

#ifndef MAP_INFO_H
#define MAP_INFO_H

#include "data.h"



typedef struct MAP_BASE_INFO_STRUCT
{
	UCHR map_name[MAX_MAP_NAME_LENGTH+1];				// ��ͼ����
	SLONG map_filename_id;								// �����ͼ����
	SLONG music_filename_id;							// ���ֵ�������
	SLONG small_map_filename_id;						// С��ͼ����
// ---
	SLONG fight_music_id;								// һ��ս��,ս����������
	SLONG fight_map_filename_id;						// һ��ս��,ս������
// ---	
	SLONG boss_fight_music_id;							// Boss ս����������
	SLONG boss_fight_map_filename_id;					// Boss ս������
// ---
	
} MAP_BASE_INFO;


typedef struct MAP_ATTRIB_INFO_STRUCT
{
	SLONG fight_flag;									// �Ƿ���Է���ս��(����)
	SLONG control_fight_flag;							// �Ƿ���Գ������ս��
	SLONG pk_flag;										// �Ƿ���Է��� PK ( ɱ�� )
	SLONG small_map_flag;								// �Ƿ��ṩ��С��ͼ

} MAP_ATTRIB_INFO;


typedef struct MAP_WEATHER_BASE_STRUCT
{
	SLONG weather_type;									// ���� id
	SLONG gold;											// ��Ǯ
	SLONG exp;											// ����
	SLONG att;											// ������
	SLONG pt;											// ����
	SLONG base;											// ��������
	SLONG boss;											// boss ���ֻ���
} MAP_WEATHER_BASE;


#endif // #ifndef MAP_INFO_H

