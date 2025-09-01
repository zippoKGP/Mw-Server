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
	UCHR map_name[MAX_MAP_NAME_LENGTH+1];				// 地图名称
	SLONG map_filename_id;								// 登入地图名称
	SLONG music_filename_id;							// 音乐档案名称
	SLONG small_map_filename_id;						// 小地图名称
// ---
	SLONG fight_music_id;								// 一般战斗,战斗背景音乐
	SLONG fight_map_filename_id;						// 一般战斗,战斗背景
// ---	
	SLONG boss_fight_music_id;							// Boss 战斗背景音乐
	SLONG boss_fight_map_filename_id;					// Boss 战斗背景
// ---
	
} MAP_BASE_INFO;


typedef struct MAP_ATTRIB_INFO_STRUCT
{
	SLONG fight_flag;									// 是否可以发生战斗(暗雷)
	SLONG control_fight_flag;							// 是否可以吃香避免战斗
	SLONG pk_flag;										// 是否可以发生 PK ( 杀人 )
	SLONG small_map_flag;								// 是否提供缩小地图

} MAP_ATTRIB_INFO;


typedef struct MAP_WEATHER_BASE_STRUCT
{
	SLONG weather_type;									// 天气 id
	SLONG gold;											// 金钱
	SLONG exp;											// 经验
	SLONG att;											// 攻击力
	SLONG pt;											// 防御
	SLONG base;											// 基本抗性
	SLONG boss;											// boss 出现机率
} MAP_WEATHER_BASE;


#endif // #ifndef MAP_INFO_H

