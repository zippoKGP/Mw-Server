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
// DoorServer和WorldServer的通信消息
#define		NET_MSG_DOOR_WORLD_PLAYER_NUMBER				0x00010000

//扣钱
#define		NET_MSG_DROP_MONEY				0x00010001
#define		NET_MSG_DROP_MONEY_RETURN		0x00010002
enum
{
	DROP_MONEY,
	SAVE_MONEY
};


#endif