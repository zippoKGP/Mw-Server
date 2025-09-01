// UnionManager.cpp: implementation of the CUnionManager class.
//
//////////////////////////////////////////////////////////////////////

#include "incall.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUnionManager::CUnionManager()
{
	m_pUnion = NULL;
	m_listUnionIdle.clear();
	m_mapUnion.clear();
	m_ulMaxUnion = 0;
}

CUnionManager::~CUnionManager()
{
	UGDel<CUnion*>(m_pUnion);
	m_listUnionIdle.clear();
	m_mapUnion.clear();
}

UG_ULONG CUnionManager::init(UG_ULONG ulCount)
{
	m_ulMaxUnion = ulCount;
	m_listUnionIdle.clear();
	m_mapUnion.clear();
	m_pUnion = UGNew<CUnion>(ulCount,__FILE__,__LINE__);
	for(UG_ULONG i = 0; i < ulCount; i ++)
	{
		m_listUnionIdle.push_back(m_pUnion + i);
	}
	return 0;
}

UG_ULONG CUnionManager::cleanup()
{
	m_listUnionIdle.clear();
	m_mapUnion.clear();
	for(UG_ULONG i = 0; i < m_ulMaxUnion; i ++)
	{
		m_listUnionIdle.push_back(m_pUnion + i);
	}
	return 0;
}

UG_PVOID CUnionManager::addUnion(INT32 n32Unionid)
{
	UG_PVOID pvUnion = NULL;
	list<CUnion*>::iterator it = m_listUnionIdle.begin();
	if(it == m_listUnionIdle.end())
	{
		return pvUnion;
	}
	else
	{
		m_mapUnion[n32Unionid] = *it;
		pvUnion = *it;
		m_listUnionIdle.erase(it);
	}
	return pvUnion;
}

UG_PVOID CUnionManager::delUnion(INT32 n32Unionid)
{
	UG_PVOID pvUnion = NULL;
	map<UG_INT32,CUnion*>::iterator it = m_mapUnion.find(n32Unionid);
	if(it == m_mapUnion.end())
	{
		return pvUnion;
	}
	else
	{
		pvUnion = it->second;
		it->second->clearupPlayer();
		m_listUnionIdle.push_back(it->second);
		m_mapUnion.erase(it);
	}
	return pvUnion;
}

UG_PVOID CUnionManager::addPlayer(INT32 n32Playerid,INT32 n32Unionid,UG_PVOID pvPlayer)
{
	map<UG_INT32,CUnion*>::iterator it = m_mapUnion.find(n32Unionid);
	if(it == m_mapUnion.end())
	{
		return NULL;
	}
	else
	{
		return (it->second)->addPlayer(n32Playerid,pvPlayer);
	}
}

UG_PVOID CUnionManager::delPlayer(INT32 n32Playerid,INT32 n32Unionid)
{
	map<UG_INT32,CUnion*>::iterator it = m_mapUnion.find(n32Unionid);
	if(it == m_mapUnion.end())
	{
		return NULL;
	}
	else
	{
		return (it->second)->delPlayer(n32Playerid);
	}
}

UG_ULONG CUnionManager::cout(FILE* p)
{
	fprintf(p,"the union max count is %d.\n",m_ulMaxUnion);
	map<UG_INT32,CUnion*>::iterator it;
	for(it = m_mapUnion.begin(); it != m_mapUnion.end(); it ++)
	{
		it->second->cout(p);
	}
	return 0;
}
