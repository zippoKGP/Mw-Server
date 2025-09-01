/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : worldbase.h
*owner        : Ben
*description  : ����worldbase�Ļ������ݣ����ڼ�¼worldbase����Ϣ�����������
*modified     : 2004/12/29
******************************************************************************/ 
#ifndef __WORLD_BASE__
#define __WORLD_BASE__

#include "UGBenDef.h"

#define		WORLD_BASE_NAME_SIZE	16

typedef struct _tag_world_base
{
	UG_ULONG		ulID; //id��
	UG_DWORD		dwIP;
	UG_DWORD		dwPort;
	UG_ULONG		ulOnlineCount; //��ǰ��������
	UG_ULONG		ulHistoryMaxCount; //��ʷ�����������
	UG_ULONG		ulCPU; //CPUʱռ�ñ���(0--100)
	UG_CHAR			szName[WORLD_BASE_NAME_SIZE + 1]; //����
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
