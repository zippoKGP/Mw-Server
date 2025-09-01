#include "stdafx.h"
#include "MapDummy.h"

//////////////////////////////////////////////////////////////////////
// CDummyPosTable
//////////////////////////////////////////////////////////////////////

//대소문자 구문 하지않고 로드..

CDummyPosTable::CDummyPosTable()
{
	m_pDumPos = NULL;
	m_nDumPosDataNum = 0;
}

CDummyPosTable::~CDummyPosTable()
{
	if(m_pDumPos)
		delete [] m_pDumPos;
}

bool CDummyPosTable::LoadDummyPosition(char* szTextFileName, char* szPrefix)
{
	FILE *fp = fopen(szTextFileName, "rt");

	if(fp == NULL)
		return false;

	char szData[128];
	int nMax = 0;

	while(1)
	{
		if(fscanf(fp, "%s", szData) == EOF)
			break;

		else if(!strncmp(szData, szPrefix, strlen(szPrefix)))
			nMax++;

		else if(!strcmp(szData, "[HelperObjectEnd]"))
			break;
	}

	m_nDumPosDataNum = nMax;
	m_pDumPos = new _dummy_position [m_nDumPosDataNum];

	rewind(fp);

	int nCnt = 0;
	int i;
	int nLineIndex = 0;

	while(1)
	{
		if(fscanf(fp, "%s", szData) == EOF)
			break;

		if(szData[0] =='*')
		{
			if(!strncmp(szData, szPrefix, strlen(szPrefix)))
			{
				m_pDumPos[nCnt].m_wLineIndex = nLineIndex;//라인인덱스

				char* pszUpper = strlwr(strdup(&szData[1]));
				strcpy(m_pDumPos[nCnt].m_szCode, pszUpper);//더미코드(대소문자 구분안함

				for(i = 0; i < 3; i++)
					fscanf(fp, "%d", &m_pDumPos[nCnt].m_zLocalMin[i]);	//로컬 최소좌표
				for(i = 0; i < 3; i++)
					fscanf(fp, "%d", &m_pDumPos[nCnt].m_zLocalMax[i]);	//로컬 최대 좌표

				nCnt++;
			}
			nLineIndex++;
		}

		else if(!strcmp(szData, "[HelperObjectEnd]"))
			break;
	}

	fclose(fp);

	return true;
}

int CDummyPosTable::GetRecordNum()
{
	return m_nDumPosDataNum;
}

_dummy_position* CDummyPosTable::GetRecord(char* szCode)
{
	_str_code szParam;
	char* pszUpper = strlwr(strdup(szCode));
	strcpy(szParam, pszUpper);

	for(int i = 0; i < m_nDumPosDataNum; i++)
	{
		if(!strcmp(m_pDumPos[i].m_szCode, szParam))
			return &m_pDumPos[i];
	}
	return NULL;
}
/*
_dummy_position* CDummyPosTable::GetRecordAllCase(char* szCode)
{
	_str_code szParam;
	char* pszUpper = strlwr(strdup(szCode));
	strcpy(szParam, pszUpper);

	for(int i = 0; i < m_nDumPosDataNum; i++)
	{
		char* szCmp = strlwr(strdup(m_pDumPos[i].m_szCode));
		if(!strcmp(szCmp, szParam))
			return &m_pDumPos[i];
	}
	return NULL;
}*/

_dummy_position* CDummyPosTable::GetRecord(int i)
{
	if(i >= m_nDumPosDataNum)
		return NULL;

	return &m_pDumPos[i];
}
