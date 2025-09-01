// MapDummy.h: interface for the CMapDummy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPDUMMY_H__4FB5B2ED_6284_4293_9B75_C509E7C3DB0B__INCLUDED_)
#define AFX_MAPDUMMY_H__4FB5B2ED_6284_4293_9B75_C509E7C3DB0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyUtil.h"
#include "basefld.h"
#include "MonBlockFld.h"
#include <math.h>

class CMapData; struct _mon_active_fld; struct _portal_fld; struct _dummy_position;

struct _dummy_position
{
	_str_code	m_szCode;
	WORD		m_wLineIndex;
	bool		m_bPosAble;		//위치가 제대로찍힌 더미인지..(false면 그자리에 몬스터를 배치안시킴)
	
	short		m_zLocalMin[3];	//local..
	short		m_zLocalMax[3];	//local..

	float		m_fMin[3];	//┌
	float		m_fMax[3];	//	┘
	float		m_fRT[3];	//ㄱ
	float		m_fLB[3];	//ㄴ
	float		m_fCenterPos[3];
	
	_dummy_position(){ m_bPosAble = true; }
};

class CDummyPosTable
{
private:
	_dummy_position* m_pDumPos;
	int m_nDumPosDataNum;

public:
	CDummyPosTable();
	virtual ~CDummyPosTable();

	bool LoadDummyPosition(char* szTextFileName, char* szPrefix);
	int GetRecordNum();
	_dummy_position* GetRecord(char* szCode);	
	_dummy_position* GetRecord(int nIndex);	
//	_dummy_position* GetRecordAllCase(char* szCode);	
};

//하나의 monster block을 담을 구조체
struct _mon_block
{
	_mon_block_fld* m_pBlkRec;	//xls에서 로딩한 block record구조체 pointer
	_dummy_position* m_pDumPos[_mon_block_fld::_max_list];
	CMapData* m_pMap;	//위치한 map pointer
	
	bool SetBlock(_mon_block_fld* pBlkRec, CMapData* pMap, _dummy_position** ppDumPos)
	{
		m_pBlkRec = pBlkRec;
		m_pMap = pMap;

		for(int i= 0; i < pBlkRec->m_dwDummyNum; i++)//_mon_block_fld::_max_list; i++)
			m_pDumPos[i] = ppDumPos[i];
		
		return true;
	}

	int SelectDummyIndex()
	{
		WORD wRand = ::rand()%100;
		WORD wProp[_mon_block_fld::_max_list];
		WORD wPreProp = 0;

		for(int i = 0; i < m_pBlkRec->m_dwDummyNum; i++)
		{
			if(i > 0)
				wPreProp = wProp[i-1];

			wProp[i] = wPreProp+m_pBlkRec->m_DummyInfo[i].m_dwSelectProp;
			if(wProp[i] > wRand)
				return i;
		}
		return -1;
	}
};

//하나의 monster active 데이터를 담을 구조체
struct _mon_active
{
	_mon_active_fld* m_pActRec;	//xls에서 로딩한 dummy record구조체 pointer
	_mon_block* m_pBlk;	//자신을 포함하는 _mon_block pointer
	WORD	m_wMonRecIndex;

	short	m_zCurMonNum;			//현재 살아있는 개체수
	DWORD	m_dwCumMonNum;			//누적 생성후
	DWORD	m_dwLastRespawnTime;	//최근 생성한 시간

	DWORD	m_dwKillNum;			//체크주기동안의 킬링수
	int		m_nCurRegenNum;		//체크주기동안의 리젠수

	_mon_active()
	{
		m_pActRec = NULL;
		m_pBlk = NULL;
		m_zCurMonNum = 0;
		m_dwCumMonNum = 0;
		m_dwLastRespawnTime = 0;
	}

	bool SetActive(_mon_active_fld* pRec, _mon_block* pBlk, int nMonRecIndex)
	{
		if(m_pBlk)
			return false;

		m_pActRec = pRec;
		m_pBlk = pBlk;
		m_wMonRecIndex = nMonRecIndex;

		m_dwKillNum = 0;
		m_nCurRegenNum = pRec->m_dwRegenLimNum;
		
		return true;
	}

	void SetZeroMonNum()
	{
		m_zCurMonNum = 0;
		m_dwCumMonNum = 0;
		m_dwLastRespawnTime = 0;
	}

	bool SetCurMonNum(int nAlter)
	{
		m_zCurMonNum += nAlter;

		if(nAlter > 0)
			m_dwCumMonNum += nAlter;

		return true;
	}

	bool AddKillMonNum()
	{
		m_dwKillNum ++;

		return true;
	}
	
	void CalcNextRegenNum()
	{
		if(m_pActRec->m_dwStdKill == 0)
			m_nCurRegenNum = 0;
		else
		{
			if(m_dwKillNum > m_pActRec->m_dwStdKill)
			{
				m_nCurRegenNum += m_dwKillNum/m_pActRec->m_dwStdKill*1.2;
				if(m_nCurRegenNum > m_pActRec->m_dwRegenMaxNum)
					m_nCurRegenNum = m_pActRec->m_dwRegenMaxNum;
			}
			else
			{
				if(m_nCurRegenNum > m_pActRec->m_dwRegenMinNum)
				{
					m_nCurRegenNum--;
				}
			}
		}
		m_dwKillNum = 0;
	}
};

//하나의 portal을 담을 구조체
struct _portal_dummy
{
	_portal_fld* m_pPortalRec;	//xls에서 로딩한 portal record구조체 pointer
	_dummy_position* m_pDumPos;

	_portal_dummy()
	{
		m_pPortalRec = NULL;
		m_pDumPos = NULL;
	}

	bool SetDummy(_portal_fld* pRec, _dummy_position* pDumPos)
	{
		if(m_pPortalRec)
			return false;

		m_pPortalRec = pRec;
		m_pDumPos = pDumPos;
		return true;
	}
};

struct _store_dummy
{
	enum { item_store = 0, used_store };

	int m_nStoreType;
	_base_fld* m_pStoreRec;
	_dummy_position* m_pDumPos;

	_store_dummy()
	{
		m_pStoreRec = NULL;
		m_pDumPos = NULL;
		m_nStoreType = item_store;
	}

	bool SetDummy(int nType, _base_fld* pRec, _dummy_position* pDumPos)
	{
		if(m_pStoreRec)
			return false;

		m_nStoreType = nType;
		m_pStoreRec = pRec;
		m_pDumPos = pDumPos;

		return true;
	}
};

struct _start_dummy
{
	_dummy_position* m_pDumPos;

	_start_dummy()
	{
		m_pDumPos = NULL;
	}

	bool SetDummy(_dummy_position* pDumPos)
	{
		if(m_pDumPos)
			return false; 
		m_pDumPos = pDumPos;
		return true;
	}
};

struct _res_dummy
{
	enum { range_grade_num = 3 };

	_dummy_position*	m_pDumPos;

	float				m_fMinLocal[range_grade_num][3];
	float				m_fMaxLocal[range_grade_num][3];

	BYTE				m_byGrade[range_grade_num];
	DWORD				m_dwDelay[range_grade_num][2];	//섹터별 지연시간..
	
	_res_dummy()
	{
		m_pDumPos = NULL;
		memset(m_fMinLocal, 0, sizeof(float)*3*3);
		memset(m_fMaxLocal, 0, sizeof(float)*3*3);
	}

	bool SetDummy(_dummy_position* pDumPos)
	{
		if(m_pDumPos)
			return false; 

		m_pDumPos = pDumPos;

		m_dwDelay[0][0] = 5*1000;
		m_dwDelay[0][1] = 8*1000;	
		m_dwDelay[1][0] = 9*1000;
		m_dwDelay[1][1] = 12*1000;	
		m_dwDelay[2][0] = 13*1000;
		m_dwDelay[2][1] = 15*1000;	

		float fFullSize[3] = {m_pDumPos->m_zLocalMax[0]-m_pDumPos->m_zLocalMin[0], 0, m_pDumPos->m_zLocalMax[2]-m_pDumPos->m_zLocalMin[2]};
		float fMidleSize[3] = {sqrt((2*(fFullSize[0]*fFullSize[0]))/3), 0, sqrt((2*(fFullSize[2]*fFullSize[2]))/3)};
		float fSmallSize[3] = {sqrt((fFullSize[0]*fFullSize[0])/3), 0, sqrt((fFullSize[2]*fFullSize[2])/3)};

		//섹터별 로컬좌표 세팅..
		//큰거 2 
		for(int i = 0; i < 3; i++)
		{
			m_fMinLocal[2][i] = m_pDumPos->m_zLocalMin[i];
			m_fMaxLocal[2][i] = m_pDumPos->m_zLocalMax[i];
		}

		//중간 1
		m_fMinLocal[1][0] = m_fMinLocal[2][0]+((fFullSize[0]-fMidleSize[0])/2);
		m_fMinLocal[1][2] = m_fMinLocal[2][2]+((fFullSize[2]-fMidleSize[2])/2);
		m_fMaxLocal[1][0] = m_fMaxLocal[2][0]-((fFullSize[0]-fMidleSize[0])/2);
		m_fMaxLocal[1][2] = m_fMaxLocal[2][2]-((fFullSize[2]-fMidleSize[2])/2);

		//작은거 0
		m_fMinLocal[0][0] = m_fMinLocal[2][0]+((fFullSize[0]-fSmallSize[0])/2);
		m_fMinLocal[0][2] = m_fMinLocal[2][2]+((fFullSize[2]-fSmallSize[2])/2);
		m_fMaxLocal[0][0] = m_fMaxLocal[2][0]-((fFullSize[0]-fSmallSize[0])/2);
		m_fMaxLocal[0][2] = m_fMaxLocal[2][2]-((fFullSize[2]-fSmallSize[2])/2);

		for(i = 0; i < 10; i++)
			SetRangeGrade();

		return true;
	}

	void SetRangeGrade()
	{
		BYTE byRange = ::rand()%3;
		BYTE byGrade = ::rand()%2;
		m_byGrade[byRange] = 0;
		m_byGrade[(++byRange)%3] = byGrade+1;
		m_byGrade[(++byRange)%3] = !byGrade+1;
	}

	int GetDelay(int nSector)
	{
		BYTE byGrd = m_byGrade[nSector];

		return ::rand()%(m_dwDelay[byGrd][1]-m_dwDelay[byGrd][0])+m_dwDelay[byGrd][0];
	}
};
#endif // !defined(AFX_MAPDUMMY_H__4FB5B2ED_6284_4293_9B75_C509E7C3DB0B__INCLUDED_)
