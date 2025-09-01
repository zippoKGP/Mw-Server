// SceneManager.cpp: implementation of the CSceneManager class.
//
//////////////////////////////////////////////////////////////////////

#include "incall.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSceneManager::CSceneManager()
{
	m_mapScene.clear();
	m_pScene = NULL;
}

CSceneManager::~CSceneManager()
{
	m_mapScene.clear();
	UGDel<CScene*>(m_pScene);
}

UG_LONG CSceneManager::init(UG_INT32 n32Count,UG_PINT32 pn32ID)
{
	m_pScene = UGNew<CScene>(n32Count,__FILE__,__LINE__);
	for(UG_INT32 i = 0; i < n32Count; i ++)
	{
		m_mapScene[*(pn32ID + i)] = m_pScene + i;
	}
	return 0;
}

UG_LONG CSceneManager::cleanup()
{
	m_mapScene.clear();
	UGDel<CScene*>(m_pScene);
	return 0;
}

UG_PVOID CSceneManager::addPlayer(INT32 n32Playerid,INT32 n32Sceneid,UG_PVOID pvPlayer)
{
	map<UG_INT32,CScene*>::iterator it = m_mapScene.find(n32Sceneid);
	if(it == m_mapScene.end())
	{
		g_pLog->UGLog("addPlayer in CSceneManager error because not find scendid = %d.",n32Sceneid);
		return NULL;
	}
	else
	{
		return (it->second)->addPlayer(n32Playerid,pvPlayer);
	}
}

UG_PVOID CSceneManager::delPlayer(INT32 n32Playerid,INT32 n32Sceneid)
{
	map<UG_INT32,CScene*>::iterator it = m_mapScene.find(n32Sceneid);
	if(it == m_mapScene.end())
	{
		g_pLog->UGLog("delPlayer error in CSceneManager because not find scendid = %d.",n32Sceneid);
		return NULL;
	}
	else
	{
		return (it->second)->delPlayer(n32Playerid);
	}
}

UG_LONG CSceneManager::cout(FILE* p)
{
	fprintf(p,"the scene count is %d.\n",m_mapScene.size());
	map<UG_INT32,CScene*>::iterator it;
	for(it = m_mapScene.begin(); it != m_mapScene.end(); it ++)
	{
		fprintf(p,"the scene id is %d.\n",it->first);
		it->second->cout(p);
	}
	return 0;
}
