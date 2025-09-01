// FileLine.cpp: implementation of the CFileLine class.
//
//////////////////////////////////////////////////////////////////////

#include "IncAll.h"

CFileLine::CFileLine()
{
	m_ulMaxLen = 0;
	m_list.clear();
}

CFileLine::~CFileLine()
{
	cleanup();
}

UG_ULONG CFileLine::init(UG_PCHAR pchPathName,UG_ULONG ulLen)
{
	if(ulLen < 2)
	{
		return -1;
	}
	m_ulMaxLen = 0;
	FILE* pFile = fopen(pchPathName,"rb");
	if(!pFile)
	{
		return -1;
	}
	UG_PCHAR pchRead = new UG_CHAR[ulLen + 1];
	UG_ULONG ulRead = 0;
	UG_ULONG ulFilePos = 0;
	UG_ULONG ulFileLine = 0;
	UG_ULONG ulTemp = 0;
	UG_PCHAR pchTemp = NULL;
	for(;;)
	{
		if(pchTemp)
		{
			if('\r' == *(pchTemp + strlen(pchTemp) - 1))
			{
				*pchRead = '\r';
				ulRead = fread(pchRead + 1,sizeof(UG_CHAR),ulLen - 1,pFile);
				ulRead ++;
				ulTemp --;
			}
			else
			{
				ulRead = fread(pchRead,sizeof(UG_CHAR),ulLen,pFile);
			}
		}
		else
		{
			ulRead = fread(pchRead,sizeof(UG_CHAR),ulLen,pFile);
		}
		*(pchRead + ulRead) = '\0';
		pchTemp = pchRead;
		for(;;)
		{
			UG_PCHAR pchFind = strstr(pchTemp,"\r\n");
			if(pchFind)
			{
				FILE_LINE fl;
				fl.ulFilePos = ulFilePos;
				fl.ulLineCount = pchFind - pchTemp + ulTemp;
				fl.ulLine = ulFileLine;
				m_list.push_back(fl);
				if(m_ulMaxLen < fl.ulLineCount)
				{
					m_ulMaxLen = fl.ulLineCount;
				}
				ulFilePos = ulFilePos + fl.ulLineCount + 2;
				pchTemp = pchTemp + ((pchFind - pchTemp) + 2);
				ulFileLine ++;
				ulTemp = 0;
			}
			else
			{
				break;
			}
		}
		if(ulRead < ulLen)
		{
			FILE_LINE fl;
			fl.ulFilePos = ulFilePos;
			fl.ulLineCount = strlen(pchTemp) + ulTemp;
			fl.ulLine = ulFileLine;
			m_list.push_back(fl);
			if(m_ulMaxLen < fl.ulLineCount)
			{
				m_ulMaxLen = fl.ulLineCount;
			}
			break;
		}
		ulTemp += strlen(pchTemp);
	}
	ReleasePA<UG_PCHAR>(pchRead);
	if(pFile)
	{
		fclose(pFile);
		pFile = NULL;
	}
	return 0;
}

UG_ULONG CFileLine::cleanup()
{
	m_list.clear();
	return 0;
}

