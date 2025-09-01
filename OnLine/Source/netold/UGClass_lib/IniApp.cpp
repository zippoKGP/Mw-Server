// IniApp.cpp: implementation of the CIniApp class.
//
//////////////////////////////////////////////////////////////////////

#include "IncAll.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIniApp::CIniApp()
{
	m_list.clear();
	m_pchAPP = NULL;
}

CIniApp::~CIniApp()
{
	cleanup();
}

UG_ULONG CIniApp::cleanup()
{
	list<CIniKeyValue*>::iterator it;
	for(it = m_list.begin(); it != m_list.end(); it ++)
	{
		ReleaseP<CIniKeyValue*>(*it);
	}
	m_list.clear();
	ReleasePA<UG_PCHAR>(m_pchAPP);
	return 0;
}

UG_ULONG CIniApp::convertAppAndKeyLower()
{
	CUGChar::toLower(m_pchAPP);
	CIniKeyValue* pKeyValue = NULL;
	list<CIniKeyValue*>::iterator it;
	for(it = m_list.begin(); it != m_list.end(); it ++)
	{
		pKeyValue = *it;
		if(pKeyValue)
		{
			pKeyValue->convertAppAndKeyLower();
		}
	}
	return 0;
}

UG_ULONG CIniApp::getValue(UG_PCHAR pchApp,UG_PCHAR pchKey,UG_PCHAR& pchValue)
{
	if(strcmp(pchApp,m_pchAPP))
	{
		return -1;
	}
	CIniKeyValue* pKeyValue = NULL;
	list<CIniKeyValue*>::iterator it;
	for(it = m_list.begin(); it != m_list.end(); it ++)
	{
		pKeyValue = *it;
		if(pKeyValue)
		{
			if(!pKeyValue->getValue(pchKey,pchValue))
			{
				return 0;
			}
		}
	}
	return -1;
}

UG_ULONG CIniApp::getMaxAppKey(UG_ULONG& ulMaxApp,UG_ULONG& ulMaxKey)
{
	if(ulMaxApp < strlen(m_pchAPP))
	{
		ulMaxApp = strlen(m_pchAPP);
	}
	CIniKeyValue* pKeyValue = NULL;
	list<CIniKeyValue*>::iterator it;
	for(it = m_list.begin(); it != m_list.end(); it ++)
	{
		pKeyValue = *it;
		if(pKeyValue)
		{
			pKeyValue->getMaxKey(ulMaxKey);
		}
	}
	return 0;
}

UG_LONG CIniApp::getKeyValue(UG_LONG lIndex,UG_PCHAR pchApp,UG_PCHAR& pchKey,UG_PCHAR& pchValue)
{
	if(strcmp(pchApp,m_pchAPP))
	{
		return -1;
	}
	CIniKeyValue* pKeyValue = NULL;
	list<CIniKeyValue*>::iterator it;
	UG_LONG l = 0;
	for(it = m_list.begin(); it != m_list.end(); it ++)
	{
		if(l == lIndex)
		{
			pchKey = (*it)->m_pchKey;
			pchValue = (*it)->m_pchValue;
			return 0;
		}
		l ++;
	}
	return -1;
}