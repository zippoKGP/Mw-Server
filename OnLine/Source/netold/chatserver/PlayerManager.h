/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : PlayerManager.h
*owner        : Ben
*description  : ��ҹ����ֻ࣬�����Ѿ��ɹ��������ҡ�
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
	//����ҵ���ͬ�����id������ǰ��ҵ�״̬��Ϊ�����ߡ�
	UG_LONG			addPlayer(UG_INT32 n32Playerid,UG_PVOID pPlayer);
	UG_PVOID		delPlayer(UG_INT32 n32Playerid); //���ر�ɾ����ҵ�ָ���NULL
	UG_PVOID		findPlayer(UG_INT32 n32Playerid); //�����ҵ������ָ���NULL
	
public:
	map<UG_INT32,UG_PVOID>		m_mapPlayer; //���idΪ���������ָ��Ϊ����
	
public:
	UG_LONG			cout(FILE* p);
	
};

#endif // !defined(AFX_PLAYERMANAGER_H__64D1F9D0_BA74_4AC1_8EEE_875E8F636564__INCLUDED_)
