/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2005 ALL RIGHTS RESERVED.

  *file name    : door_srv.h
  *owner  		: Andy
  *description  : 
  *modified     : 2005/1/6
				: 2005/1/18 rename door_info.h
******************************************************************************/ 

#ifndef DOOR_INFO_H
#define DOOR_INFO_H

//////////////////////////////////////////////////////////////////////////
// DoorServer��WorldServer��ͨ����Ϣ
#define		NET_MSG_DOOR_WORLD				0x00010000
enum // NET_MSG_DOOR_WORLD ��Ϣ����
{
	DOOR_WORLD_ADD_USER,	// ����һ���û�
	DOOR_WORLD_DROP_USER	// ����һ���û�
};

#endif