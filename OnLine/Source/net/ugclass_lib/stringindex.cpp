// StringIndex.cpp: implementation of the CStringIndex class.
//
//////////////////////////////////////////////////////////////////////

#include "IncAll.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStringIndex::CStringIndex()
{
	m_ulMaxLen = 0;
	m_map.clear();
	m_bLoadInMem = FALSE;
	m_pFile = NULL;
	m_pchMax = NULL;
}

CStringIndex::~CStringIndex()
{
	cleanup();
}

UG_ULONG CStringIndex::init(UG_PCHAR pchPathName,UG_BOOL bLoadInMem)
{
	m_bLoadInMem = bLoadInMem;
	CFileLine fl;
	UG_ULONG ulRet = fl.init(pchPathName);
	if(ulRet)
	{
		return ulRet;
	}
	FILE* pFile = fopen(pchPathName,"rb");
	if(!pFile)
	{
		return -1;
	}
	UG_PCHAR pchLine = new UG_CHAR[fl.m_ulMaxLen + 1];
	list<FILE_LINE>::iterator it;
	for(it = fl.m_list.begin(); it != fl.m_list.end(); it ++)
	{
		if(((*it).ulLineCount))
		{
			fseek(pFile,(*it).ulFilePos,SEEK_SET);
			UG_ULONG ulRead = fread(pchLine,sizeof(UG_CHAR),(*it).ulLineCount,pFile);
			if(ulRead != (*it).ulLineCount)
			{
				ulRet = -1;
				break;
			}
			*(pchLine + ulRead) = '\0';
			parseKeyAndValue(pchLine,(*it).ulFilePos);
		}
	}
	if(m_bLoadInMem)
	{
		if(pFile)
		{
			fclose(pFile);
			pFile = NULL;
		}
	}
	ReleasePA<UG_PCHAR>(pchLine);
	return 0;
}

UG_ULONG CStringIndex::getValue(UG_PCHAR pchKey,UG_PCHAR& pchValue)
{
	map<UG_PCHAR,CIndexKeyValue*,CUGChar>::iterator it = m_map.find(pchKey);
	if(m_map.end() == it)
	{
		return -1;
	}
	CIndexKeyValue* pKV = (*it).second;
	if(!pKV)
	{
		return -1;
	}
	if(m_bLoadInMem)
	{
		return pKV->getValue(pchValue);
	}
	if(m_ulMaxLen < pKV->m_ulCount)
	{
		return -1;
	}
	return pKV->getValue(m_pFile,m_pchMax,pchValue);
}

UG_ULONG CStringIndex::cleanup()
{
	if(m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
	ReleasePA<UG_PCHAR>(m_pchMax);
	map<UG_PCHAR,CIndexKeyValue*,CUGChar>::iterator it;
	for(it = m_map.begin(); it != m_map.end(); it ++)
	{
		UG_PCHAR pchKey = (*it).first;
		ReleasePA<UG_PCHAR>(pchKey);
		ReleaseP<CIndexKeyValue*>((*it).second);
	}
	m_map.clear();
	return 0;
}

UG_ULONG CStringIndex::parseKeyAndValue(UG_PCHAR pchLine,UG_ULONG ulPos)
{
	UG_PCHAR pchFind = strstr(pchLine,"=");
	if(pchFind)
	{
		UG_INT32 n32Left = 0;
		UG_INT32 n32Right = 0;
		UG_INT32 n32Count = 0;
		removeSpace(pchLine,pchFind - pchLine,n32Left,n32Right);
		if(n32Left >= n32Right)
		{
			return -1;
		}
 		CIndexKeyValue* pKV = new CIndexKeyValue;
		UG_PCHAR pchKey = new UG_CHAR[n32Right - n32Left + 1];
		memcpy(pchKey,pchLine + n32Left,n32Right - n32Left);
		*(pchKey + n32Right - n32Left) = '\0';
		removeSpace(pchFind + 1,strlen(pchLine) - (pchFind - pchLine) - 1,n32Left,n32Right);
		if(n32Left >= n32Right)
		{
			pKV->m_ulCount = 0;
			if(m_ulMaxLen < pKV->m_ulCount)
			{
				m_ulMaxLen = pKV->m_ulCount;
			}
			pKV->m_ulPos = ulPos + (pchFind - pchLine);
			if(m_bLoadInMem)
			{
				pKV->m_pchValue = new UG_CHAR[pKV->m_ulCount + 1];
				*(pKV->m_pchValue + pKV->m_ulCount) = '\0';
			}
		}
		else
		{
			pKV->m_ulCount = n32Right - n32Left;
			if(m_ulMaxLen < pKV->m_ulCount)
			{
				m_ulMaxLen = pKV->m_ulCount;
			}
			pKV->m_ulPos = ulPos + (pchFind - pchLine) + n32Left;
			if(m_bLoadInMem)
			{
				pKV->m_pchValue = new UG_CHAR[pKV->m_ulCount + 1];
				memcpy(pKV->m_pchValue,pchFind + 1 + n32Left,pKV->m_ulCount);
				*(pKV->m_pchValue + pKV->m_ulCount) = '\0';
			}
		}
		CUGChar::toLower(pchKey);
		if(m_map.end() == m_map.find(pchKey))
		{
			m_map[pchKey] = pKV;
		}
		else
		{
			ReleasePA<UG_PCHAR>(pchKey);
			ReleaseP<CIndexKeyValue*>(pKV);
			return -1;
		}
		return 0;
	}
	return -1;
}

UG_ULONG CStringIndex::removeSpace(UG_PCHAR pchLine,UG_INT32 n32Count,UG_INT32& n32Left,UG_INT32& n32Right)
{
	n32Left = 0;
	n32Right = n32Count;
	for(UG_INT32 i = 0; i < n32Count; i ++)
	{
		if(' ' == *(pchLine + i))
		{
			n32Left ++;
		}
		else
		{
			break;
		}
	}
	for(i = 0; i < n32Count; i ++)
	{
		if(' ' == *(pchLine + n32Count - 1 - i))
		{
			n32Right --;
		}
		else
		{
			break;
		}
	}
	return 0;
}

