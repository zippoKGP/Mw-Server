/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : Friends.h
*owner        : Ben
*description  : 好友管理类，增加或修改好友，删除好友，找好友。
*modified     : 2004/12/20
******************************************************************************/

#if !defined(AFX_FRIENDS_H__BD9226BA_5A90_4EE9_9BE5_E5A249335353__INCLUDED_)
#define AFX_FRIENDS_H__BD9226BA_5A90_4EE9_9BE5_E5A249335353__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFriends
{
public:
	CFriends();
	virtual ~CFriends();

public:
	//增加好友，存在时修改其属性
	UG_LONG				addFriends(INT32 n32Friendsid,UG_INT32 n32Attribute); //返回值0表示正确
	UG_LONG				delFriends(INT32 n32Friendsid); //返回值1表示错误
	//查找好友，找到返回TRUE和属性，否则返回FALSE
	UG_BOOL				findFriends(INT32 n32Friendsid,UG_INT32& n32Attribute);
	UG_LONG				cleanup();
	
public:
	UG_LONG				cout(FILE* p);
	
public:
	map<INT32,INT32>	m_mapFriends; //id和属性,0表示好友,1表示临时好友,2表示屏蔽好好友

};

#endif // !defined(AFX_FRIENDS_H__BD9226BA_5A90_4EE9_9BE5_E5A249335353__INCLUDED_)
