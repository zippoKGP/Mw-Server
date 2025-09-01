// SceneManager.h: interface for the CSceneManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEMANAGER_H__00AC3637_EC94_482C_B87C_D97DA729A828__INCLUDED_)
#define AFX_SCENEMANAGER_H__00AC3637_EC94_482C_B87C_D97DA729A828__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSceneManager  
{
public:
	CSceneManager();
	virtual ~CSceneManager();

public:
	UG_LONG			init(UG_INT32 n32Count,UG_PINT32 pn32ID);
	UG_LONG			cleanup();

public:
	//�ɹ����ظó���ָ�룬�������Ѵ��ڣ��򷵻�NULL
	UG_PVOID		addPlayer(INT32 n32Playerid,INT32 n32Sceneid,UG_PVOID pvPlayer);
	//�ɹ����ظ�ɾ�������ָ�룬�����Ҳ����ڣ��򷵻�NULL
	UG_PVOID		delPlayer(INT32 n32Playerid,INT32 n32Sceneid);
	
public:
	map<UG_INT32,CScene*>		m_mapScene;  //����idΪ����������ָ��Ϊ����
	CScene*						m_pScene;

public:
	UG_LONG			cout(FILE* p);

};

#endif // !defined(AFX_SCENEMANAGER_H__00AC3637_EC94_482C_B87C_D97DA729A828__INCLUDED_)
