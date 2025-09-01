// IniKeyValue.cpp: implementation of the CIniKeyValue class.
//
//////////////////////////////////////////////////////////////////////

#include "IncAll.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIniKeyValue::CIniKeyValue()
{
	m_pchKey = NULL;
	m_pchValue = NULL;
}

CIniKeyValue::~CIniKeyValue()
{
	cleanup();
}

UG_ULONG CIniKeyValue::cleanup()
{
	ReleasePA<UG_PCHAR>(m_pchKey);
	ReleasePA<UG_PCHAR>(m_pchValue);
	return 0;
}

UG_ULONG CIniKeyValue::convertAppAndKeyLower()
{
	CUGChar::toLower(m_pchKey);
	return 0;
}

UG_ULONG CIniKeyValue::getValue(UG_PCHAR pchKey,UG_PCHAR& pchValue)
{
	UG_ULONG ulRet = strcmp(pchKey,m_pchKey);
	if(ulRet)
	{
		return ulRet;
	}
	pchValue = m_pchValue;
	return 0;
}

UG_ULONG CIniKeyValue::getMaxKey(UG_ULONG& ulMax)
{
	if(ulMax < strlen(m_pchKey))
	{
		ulMax = strlen(m_pchKey);
	}
	return 0;
}
