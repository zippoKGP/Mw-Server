/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : Friends.cpp
*owner        : Ben
*description  : 好友管理类，增加或修改好友，删除好友，找好友。
*modified     : 2004/12/20
******************************************************************************/

#include "incall.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFriends::CFriends()
{
	m_mapFriends.clear();
}

CFriends::~CFriends()
{
	m_mapFriends.clear();
}

UG_LONG CFriends::addFriends(INT32 n32Friendsid,UG_INT32 n32Attribute)
{
	if(m_mapFriends.size() >= MAX_CHAT_FRIEND_LIST)
	{
		return -1;
	}
	m_mapFriends[n32Friendsid] = n32Attribute;
	return 0;
}

UG_LONG CFriends::delFriends(INT32 n32Friendsid)
{
	map<INT32,INT32>::iterator it = m_mapFriends.find(n32Friendsid);
	if(it != m_mapFriends.end())
	{
		m_mapFriends.erase(it);
		return 0;
	}
	return -1;
}

UG_BOOL CFriends::findFriends(INT32 n32Friendsid,UG_INT32& n32Attribute)
{
	n32Attribute = 0;
	map<INT32,INT32>::iterator it = m_mapFriends.find(n32Friendsid);
	if(m_mapFriends.end() != it)
	{
		n32Attribute = it->second;
		return TRUE;
	}
	return FALSE;
}

UG_LONG CFriends::cout(FILE* p)
{
	fprintf(p,"friends num = %d.\n",m_mapFriends.size());
	map<INT32,INT32>::iterator it;
	for(it = m_mapFriends.begin(); it != m_mapFriends.end(); it ++)
	{
		fprintf(p,"friend id = %d, attribute = %d.\n",it->first,it->second);
	}
	fprintf(p,"\n");
	return 0;
}

UG_LONG CFriends::cleanup()
{
	m_mapFriends.clear();
	return 0;
}
