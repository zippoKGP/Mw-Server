#include "stdafx.h"
#include "StorageList.h"
#include "MainThread.h"

_STORAGE_LIST::_STORAGE_LIST()
{
	m_nListNum = 0;
	m_pStorageList = NULL; 
}

//리스트 초기에 생성할때 한번 파생클래스에서 호출..
void _STORAGE_LIST::SetMemory(_db_con* pList, int nListName, int nListNum, int nUsedNum)// = 0)
{
	m_pStorageList = pList;
	m_nListCode = nListName;
	m_nListNum = nListNum;	
	if(nUsedNum == 0)
		m_nUsedNum = nListNum;
	else
		m_nUsedNum = nUsedNum;

	for(int i = 0; i < m_nListNum; i++)
	{
		m_pStorageList[i].m_pInList = this;
		m_pStorageList[i].m_byStorageIndex = i;
	}
}

//사용할 리스트를 따로 정할때..
bool _STORAGE_LIST::SetUseListNum(int nUsedNum)
{
	if(m_nUsedNum > m_nListNum)
		return false;

	m_nUsedNum = nUsedNum;
	return true;
}

//모든슬롯의 내용을 비운다..
void _STORAGE_LIST::SetAllEmpty()
{
	for(int i = 0; i < m_nListNum; i++)
	{
		m_pStorageList[i].m_bLoad = false;
	}
}

//리스트에서 빈슬롯의 수
int _STORAGE_LIST::GetNumEmptyCon()
{
	int nCnt = 0;
	for(int i = 0; i < m_nUsedNum; i++)
	{
		if(!m_pStorageList[i].m_bLoad)
			nCnt++;
	}

	return nCnt;
}

int _STORAGE_LIST::GetNumUseCon()
{
	int nCnt = 0;
	for(int i = 0; i < m_nUsedNum; i++)
	{
		if(m_pStorageList[i].m_bLoad)
			nCnt++;
	}

	return nCnt;
}

//빈슬롯의 첫번째 인덱스 가져오기..
int _STORAGE_LIST::GetIndexEmptyCon()
{
	for(int i = 0; i < m_nUsedNum; i++)
	{
		if(!m_pStorageList[i].m_bLoad)
			return i;
	}
	return __NSI;
}

//아이템시리얼번호로 슬롯인덱스를 가져온다.
int _STORAGE_LIST::GetIndexFromSerial(WORD wSerial)
{
	for(int i = 0; i < m_nUsedNum; i++)
	{
		if(m_pStorageList[i].m_bLoad && m_pStorageList[i].m_wSerial == wSerial)
		{
			m_pStorageList[i].m_pInList = this;//확인차원
			m_pStorageList[i].m_byStorageIndex = i;
			return i;
		}
	}
	return __NSI;
}

_STORAGE_LIST::_db_con* _STORAGE_LIST::GetPtrFromSerial(WORD wSerial)
{
	int nIndex = GetIndexFromSerial(wSerial);
	if(nIndex == __NSI)
		return NULL;
	return &m_pStorageList[nIndex];
}

//n번째 슬롯을 비운다. 
bool _STORAGE_LIST::EmptyCon(int n)
{
	if(m_nListCode == _STORAGE_POS::EQUIP)
	{
		if(n >= equip_fix_num)
			return false;
	}

	if(!m_pStorageList[n].m_bLoad)
		return false;

	m_pStorageList[n].m_bLoad = false;
	m_pStorageList[n].m_bLock = false;
	return true;
}

//pCon아이템을 첫번째 빈 슬롯에 삽입한다.
DWORD _STORAGE_LIST::TransInCon(_storage_con* pCon)
{
	int n;
	if(m_nListCode == _STORAGE_POS::EQUIP)
	{
		n = pCon->m_byTableCode;
		if(n >= equip_fix_num)
			return __NSI;
		if(m_pStorageList[n].m_bLoad)
			return __NSI;
	}
	else
	{
		n = GetIndexEmptyCon();
		if(n == __NSI)
			return __NSI;
	}

	memcpy(&m_pStorageList[n], pCon, sizeof(_storage_con));
	m_pStorageList[n].m_pInList = this;
	m_pStorageList[n].m_byStorageIndex = n;
	m_pStorageList[n].m_bLoad = true;
	m_pStorageList[n].m_byClientIndex = 0;
	m_pStorageList[n].m_bLock = false;

	return (DWORD)n;
}

//n번째 슬롯아이템의 m_dwDur를 만큼 더한다
bool _STORAGE_LIST::AlterCurDur(int n, int nAlter, DWORD* OUT pdwLeftDur)
{
	if(m_nListCode == _STORAGE_POS::EQUIP)
	{
		if(n >= equip_fix_num)
			return false;
	}

	if(!m_pStorageList[n].m_bLoad)
		return false;

	int nAfter = m_pStorageList[n].m_dwDur+nAlter;
	if(nAfter < 0)
		nAfter = 0;

	m_pStorageList[n].m_dwDur = nAfter;

	if(m_pStorageList[n].m_dwDur == 0)
		EmptyCon(n);
	
	*pdwLeftDur = m_pStorageList[n].m_dwDur;		
	return true;
}

//n번째 슬롯아이템의 m_dwDur과 m_dwLimDurPoint를 dwSetDur로 세팅한다.
void _STORAGE_LIST::SetLimCurDur(int n, DWORD dwSetDur)
{
	m_pStorageList[n].m_dwDur = dwSetDur;
}

//n번째 슬롯아이템의 다음업그레이드단계에 byLvCode로 업그레이드시킨다.
bool _STORAGE_LIST::GradeUp(int n, DWORD dwUptInfo)
{
	m_pStorageList[n].m_dwLv = dwUptInfo;
	return true;
}

//n번째 슬롯아이템의 현재업그레이드단계에 업그레이드코드를 삭제한다.
bool _STORAGE_LIST::GradeDown(int n, DWORD dwUptInfo)
{
	m_pStorageList[n].m_dwLv = dwUptInfo;
	return true;
}

bool _STORAGE_LIST::SetGrade(int n, BYTE byLv, DWORD dwUptInfo)
{
	m_pStorageList[n].m_dwLv = dwUptInfo;
	return true;
}

void _STORAGE_LIST::SetLock(int n, bool bLock)
{
	m_pStorageList[n].m_bLock = bLock;
}

//시리얼번호로 해당아이템의 클라이언트의 인덱스를 세팅한다.
BYTE _STORAGE_LIST::SetClientIndexFromSerial(WORD wSerial, BYTE byClientIndex, BYTE* pbyoutOldClientIndex)
{
	int nIndex = GetIndexFromSerial(wSerial);
	if(nIndex == __NSI)
		return __NSI;

	if(pbyoutOldClientIndex)
		*pbyoutOldClientIndex = m_pStorageList[nIndex].m_byClientIndex;
	if(m_pStorageList[nIndex].m_byClientIndex != byClientIndex)
		m_pStorageList[nIndex].m_byClientIndex = byClientIndex;

	return (BYTE)nIndex;
}

