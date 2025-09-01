/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : Friends.h
*owner        : Ben
*description  : ���ѹ����࣬���ӻ��޸ĺ��ѣ�ɾ�����ѣ��Һ��ѡ�
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
	//���Ӻ��ѣ�����ʱ�޸�������
	UG_LONG				addFriends(INT32 n32Friendsid,UG_INT32 n32Attribute); //����ֵ0��ʾ��ȷ
	UG_LONG				delFriends(INT32 n32Friendsid); //����ֵ1��ʾ����
	//���Һ��ѣ��ҵ�����TRUE�����ԣ����򷵻�FALSE
	UG_BOOL				findFriends(INT32 n32Friendsid,UG_INT32& n32Attribute);
	UG_LONG				cleanup();
	
public:
	UG_LONG				cout(FILE* p);
	
public:
	map<INT32,INT32>	m_mapFriends; //id������,0��ʾ����,1��ʾ��ʱ����,2��ʾ���κú���

};

#endif // !defined(AFX_FRIENDS_H__BD9226BA_5A90_4EE9_9BE5_E5A249335353__INCLUDED_)
