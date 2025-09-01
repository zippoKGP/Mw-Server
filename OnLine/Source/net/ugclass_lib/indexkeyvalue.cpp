// IndexKeyValue.cpp: implementation of the CIndexKeyValue class.
//
//////////////////////////////////////////////////////////////////////

#include "IncAll.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIndexKeyValue::CIndexKeyValue()
{
	m_pchValue = NULL;
	m_ulPos = 0;
	m_ulCount = 0;
}

CIndexKeyValue::~CIndexKeyValue()
{
	cleanup();
}

UG_ULONG CIndexKeyValue::cleanup()
{
	ReleasePA<UG_PCHAR>(m_pchValue);
	return 0;
}

UG_ULONG CIndexKeyValue::getValue(UG_PCHAR& pchValue)
{
	if(m_pchValue)
	{
		pchValue = m_pchValue;
		return 0;
	}
	return -1;
}

UG_ULONG CIndexKeyValue::getValue(FILE* pFile,UG_PCHAR pchTemp,UG_PCHAR& pchValue)
{
	if(pFile)
	{
		if(m_ulCount)
		{
			if(!fseek(pFile,m_ulPos,SEEK_SET))
			{
				if(m_ulCount * sizeof(UG_CHAR) == fread(pchTemp,sizeof(UG_CHAR),m_ulCount,pFile))
				{
					*(pchTemp + m_ulCount) = '\0';
					pchValue = pchTemp;
					return 0;
				}
			}
		}
	}
	return -1;
}
