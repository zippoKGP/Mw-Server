/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : worldbase.h
*owner        : Ben
*description  : 定义worldbase的基本数据，用于记录worldbase的信息供监测程序调用
*modified     : 2004/12/29
******************************************************************************/ 
#ifndef __WORLD_BASE__
#define __WORLD_BASE__

#include "UGBenDef.h"

#define		WORLD_BASE_NAME_SIZE	16

typedef struct _tag_world_base
{
	UG_ULONG		ulID; //id号
	UG_DWORD		dwIP;
	UG_DWORD		dwPort;
	UG_ULONG		ulOnlineCount; //当前在线人数
	UG_ULONG		ulHistoryMaxCount; //历史最大在线人数
	UG_ULONG		ulCPU; //CPU时占用比例(0--100)
	UG_CHAR			szName[WORLD_BASE_NAME_SIZE + 1]; //名称
	_tag_world_base()
	{
		ulID = 0;
		dwIP = 0;
		dwPort = 0;
		ulOnlineCount = 0;
		ulHistoryMaxCount = 0;
		ulCPU = 0;
	}

} WORLD_BASE, *PWORLD_BASE;

#endif//( __WORLD_BASE__)
