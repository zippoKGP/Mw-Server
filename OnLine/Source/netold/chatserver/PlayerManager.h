/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : PlayerManager.h
*owner        : Ben
*description  : 玩家管理类，只管理已经成功登入的玩家。
*modified     : 2004/12/20
******************************************************************************/ 

#if !defined(AFX_PLAYERMANAGER_H__64D1F9D0_BA74_4AC1_8EEE_875E8F636564__INCLUDED_)
#define AFX_PLAYERMANAGER_H__64D1F9D0_BA74_4AC1_8EEE_875E8F636564__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPlayerManager  
{
public:
	CPlayerManager();
	virtual ~CPlayerManager();

public:
	UG_LONG			cleanup();
	
public:
	//如果找到相同的玩家id，则将以前玩家的状态置为不在线。
	UG_LONG			addPlayer(UG_INT32 n32Playerid,UG_PVOID pPlayer);
	UG_PVOID		delPlayer(UG_INT32 n32Playerid); //返回被删除玩家的指针或NULL
	UG_PVOID		findPlayer(UG_INT32 n32Playerid); //返回找到的玩家指针或NULL
	
public:
	map<UG_INT32,UG_PVOID>		m_mapPlayer; //玩家id为索引，玩家指针为内容
	
public:
	UG_LONG			cout(FILE* p);
	
};

#endif // !defined(AFX_PLAYERMANAGER_H__64D1F9D0_BA74_4AC1_8EEE_875E8F636564__INCLUDED_)
