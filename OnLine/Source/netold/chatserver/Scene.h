// Scene.h: interface for the CScene class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENE_H__96D09E17_A131_44EC_BB73_D01B9A1847F8__INCLUDED_)
#define AFX_SCENE_H__96D09E17_A131_44EC_BB73_D01B9A1847F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CScene  
{
public:
	CScene();
	virtual ~CScene();

public:
	//成功返回该场景指针，如果玩家已存在，则返回NULL
	UG_PVOID		addPlayer(UG_INT32 n32Playerid,UG_PVOID pvPlayer);
	//成功返回该删除的玩家指针，如果玩家不存在，则返回NULL
	UG_PVOID		delPlayer(UG_INT32 n32Playerid);
	
public:
	map<UG_INT32,UG_PVOID>		m_mapPlayer; //玩家id为索引，玩家指针为内容

public:
	UG_LONG			cout(FILE* p);
	
};

#endif // !defined(AFX_SCENE_H__96D09E17_A131_44EC_BB73_D01B9A1847F8__INCLUDED_)
