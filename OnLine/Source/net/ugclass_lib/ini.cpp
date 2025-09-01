// Ini.cpp: implementation of the CIni class.
//
//////////////////////////////////////////////////////////////////////

#include "IncAll.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIni::CIni()
{
	m_list.clear();
	m_pchMaxKey = NULL;
	m_pchMaxApp = NULL;
	m_ulMaxApp = 0;
	m_ulMaxKey = 0;
}

CIni::~CIni()
{
	cleanup();
}

UG_ULONG CIni::init(UG_PCHAR pchPathName)
{
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
	CIniApp* pIniApp = NULL;
	ulRet = 0;
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
			if(parseKeyAndValue(pchLine,&pIniApp))
			{
				parseApp(pchLine,&pIniApp);
			}
		}
	}
	if(pFile)
	{
		fclose(pFile);
		pFile = NULL;
	}
	ReleasePA<UG_PCHAR>(pchLine);
	if(pIniApp)
	{
		m_list.push_back(pIniApp);
	}
	m_ulMaxApp = 0;
	m_ulMaxKey = 0;
	getMaxAppKey(m_ulMaxApp,m_ulMaxKey);
	m_pchMaxApp = new UG_CHAR[m_ulMaxApp + 1];
	m_pchMaxKey = new UG_CHAR[m_ulMaxKey + 1];
	return ulRet;
}

UG_ULONG CIni::getValue(UG_PCHAR pchApp,UG_PCHAR pchKey,UG_PCHAR& pchValue)
{
	if((m_ulMaxApp < strlen(pchApp)) || (m_ulMaxKey < strlen(pchKey)))
	{
		return -1;
	}
	strcpy(m_pchMaxApp,pchApp);
	strcpy(m_pchMaxKey,pchKey);
	CUGChar::toLower(m_pchMaxApp);
	CUGChar::toLower(m_pchMaxKey);
	CIniApp* pApp = NULL;
	list<CIniApp*>::iterator it;
	for(it = m_list.begin(); it != m_list.end(); it ++)
	{
		pApp = *it;
		if(pApp)
		{
			if(!pApp->getValue(m_pchMaxApp,m_pchMaxKey,pchValue))
			{
				return 0;
			}
		}
	}
	return -1;
}

UG_ULONG CIni::cleanup()
{
	list<CIniApp*>::iterator it;
	for(it = m_list.begin(); it != m_list.end(); it ++)
	{
		ReleaseP<CIniApp*>(*it);
	}
	m_list.clear();
	ReleasePA<UG_PCHAR>(m_pchMaxKey);
	ReleasePA<UG_PCHAR>(m_pchMaxApp);
	return 0;
}

UG_ULONG CIni::parseKeyAndValue(UG_PCHAR pchLine,CIniApp** ppIniApp)
{
	UG_PCHAR pchFind = strstr(pchLine,"=");
	if(pchFind)
	{
		if(*ppIniApp)
		{
			UG_INT32 n32Left = 0;
			UG_INT32 n32Right = 0;
			UG_INT32 n32Count = 0;
			removeSpace(pchLine,pchFind - pchLine,n32Left,n32Right);
			if(n32Left >= n32Right)
			{
				return 0;
			}
			CIniKeyValue* pKV = new CIniKeyValue;
			pKV->m_pchKey = new UG_CHAR[n32Right - n32Left + 1];
			memcpy(pKV->m_pchKey,pchLine + n32Left,n32Right - n32Left);
			*(pKV->m_pchKey + n32Right - n32Left) = '\0';
			CUGChar::toLower(pKV->m_pchKey);
			removeSpace(pchFind + 1,strlen(pchLine) - (pchFind - pchLine) - 1,n32Left,n32Right);
			if(n32Left >= n32Right)
			{
				n32Count = 0;
				pKV->m_pchValue = new UG_CHAR[1];
				*(pKV->m_pchValue) = '\0';
			}
			else
			{
				n32Count = n32Right - n32Left;
				pKV->m_pchValue = new UG_CHAR[n32Count + 1];
				memcpy(pKV->m_pchValue,pchFind + 1 + n32Left,n32Count);
				*(pKV->m_pchValue + n32Count) = '\0';
			}
			(*ppIniApp)->m_list.push_back(pKV);
		}
		return 0;
	}
	return -1;
}

UG_ULONG CIni::parseApp(UG_PCHAR pchLine,CIniApp** ppIniApp)
{
	UG_INT32 n32Left = 0;
	UG_INT32 n32Right = 0;
	UG_INT32 n32Count = 0;
	removeSpace(pchLine,strlen(pchLine),n32Left,n32Right);
	if(n32Right - n32Left < 3)
	{
		return 0;
	}
	if('[' != *(pchLine + n32Left))
	{
		return 0;
	}
	if(']' != *(pchLine + n32Right - 1))
	{
		return 0;
	}
	UG_PCHAR pchApp = pchLine + n32Left + 1;
	n32Count = n32Right - n32Left - 2;
	n32Left = 0;
	n32Right = 0;
	removeSpace(pchApp,n32Count,n32Left,n32Right);
	n32Count = n32Right - n32Left;
	if(n32Count < 1)
	{
		return 0;
	}
	if(*ppIniApp)
	{
		m_list.push_back(*ppIniApp);
	}
	*ppIniApp = new CIniApp;
	(*ppIniApp)->m_pchAPP = new UG_CHAR[n32Count + 1];
	memcpy((*ppIniApp)->m_pchAPP,pchApp + n32Left,n32Count);
	*((*ppIniApp)->m_pchAPP + n32Count) = '\0';
	CUGChar::toLower((*ppIniApp)->m_pchAPP);
	return 0;
}

UG_ULONG CIni::removeSpace(UG_PCHAR pchLine,UG_INT32 n32Count,UG_INT32& n32Left,UG_INT32& n32Right)
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

UG_ULONG CIni::getMaxAppKey(UG_ULONG& ulMaxApp,UG_ULONG& ulMaxKey)
{
	ulMaxApp = 0;
	ulMaxKey = 0;
	CIniApp* pApp = NULL;
	list<CIniApp*>::iterator it;
	for(it = m_list.begin(); it != m_list.end(); it ++)
	{
		pApp = *it;
		if(pApp)
		{
			pApp->getMaxAppKey(ulMaxApp,ulMaxKey);
		}
	}
	return 0;
}

UG_LONG CIni::getCount(UG_PCHAR pchApp)
{
	if(m_ulMaxApp < strlen(pchApp))
	{
		return -1;
	}
	strcpy(m_pchMaxApp,pchApp);
	CUGChar::toLower(m_pchMaxApp);
	CIniApp* pApp = NULL;
	list<CIniApp*>::iterator it;
	for(it = m_list.begin(); it != m_list.end(); it ++)
	{
		pApp = *it;
		if(pApp)
		{
			if(!strcmp(m_pchMaxApp,pApp->m_pchAPP))
			{
				return pApp->m_list.size();
			}
		}
	}
	return -1;
}

UG_LONG CIni::getKeyValue(UG_LONG lIndex,UG_PCHAR pchApp,UG_PCHAR& pchKey,UG_PCHAR& pchValue)
{
	pchKey = NULL;
	pchValue = NULL;
	if(m_ulMaxApp < strlen(pchApp))
	{
		return -1;
	}
	strcpy(m_pchMaxApp,pchApp);
	CUGChar::toLower(m_pchMaxApp);
	CIniApp* pApp = NULL;
	list<CIniApp*>::iterator it;
	for(it = m_list.begin(); it != m_list.end(); it ++)
	{
		pApp = *it;
		if(pApp)
		{
			if(!pApp->getKeyValue(lIndex,m_pchMaxApp,pchKey,pchValue))
			{
				return 0;
			}
		}
	}
	return -1;
}
