// MapData.h: interface for the CMapData class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _MapData_h_
#define _MapData_h_

#include "Level.h"
#include "MapDummy.h"
#include "GameObjectList.h"
#include "MapFld.h"
#include "PortalFld.h"
#include "MyClassUtil.h"

#define max_itemstore_num	10
#define max_layer_num		500
#define max_multi_block_num	30
#define max_block_num		120
#define max_act_num			200
#define dungeon_active_time	3*60*1000 //3분.. 600000	//10분

class CItemStore;

//map의 bsp정보를 담는 구조체
struct _bsp_info
{
	char m_szBspName[50];	//map의 이름
	int m_nMapMaxSize[3];	//+방향의 최대값
	int m_nMapMinSize[3];	//-방향의 최소값
	int m_nMapSize[3];	//절대크기
	int m_nLeafNum;		//리프수	==> server에서는 미사용
};

//map의  sector정보를 담는 구조체
struct _sec_info
{
	int m_nSecNumW;	//가로의 sector수
	int m_nSecNumH;	//세로의 sector수
	int m_nSecNum;	//전체 sector수
};

//map의 전체 sector중에서 일정면적을 담는 구조체
struct _pnt_rect
{
	int nStartx;	//사각면적에서 left점 sector의 가로 index
	int nStarty;	//사각면적에서 top점 sector의 세로 index
	int nEndx;		//사각면적에서 right점 sector의 가로 index
	int nEndy;		//사각면적에서 bottom점 sector의 세로 index
};

struct _MULTI_BLOCK
{	
	int					m_nBlockNum;
	CRecordData*		m_ptbMonBlock;
	_mon_active**		m_ppMonAct;					//block의 수 만큼 memory allocation 그다음 block index마다 dummy수 만큼 memory allocation

	_MULTI_BLOCK()
	{
		m_nBlockNum = 0;
		m_ptbMonBlock = NULL;
		m_ppMonAct = NULL;
	}

	~_MULTI_BLOCK()
	{
		if(m_ptbMonBlock)
			delete [] m_ptbMonBlock;

		if(m_ppMonAct)
		{
			for(int i = 0; i < m_nBlockNum; i++)
				delete [] m_ppMonAct[i];
			delete [] m_ppMonAct;
		}		
	}
};

struct _LAYER_SET
{
	int				m_nSecNum;
	
	//sector마다 존재할 object list ==> sector 수만큼 memory allocation
	CObjectList*	m_pListSectorObj;		
	CObjectList*	m_pListSectorPlayer;	
	CObjectList*	m_pListSectorTower;		

	_MULTI_BLOCK* 	m_pMB;
	_mon_active		m_MonAct[max_block_num][max_act_num];		
	DWORD			m_dwStartActiveTime;
	DWORD			m_dwLastInertTime;

	_LAYER_SET()
	{
		m_pListSectorObj = NULL;	
		m_pListSectorPlayer = NULL;	
		m_pListSectorTower = NULL;
		m_pMB = NULL;
		m_nSecNum = 0;
		m_dwLastInertTime = 0;
	}

	~_LAYER_SET()
	{
		m_pMB = NULL;
		if(m_pListSectorObj)
			delete [] m_pListSectorObj;
		if(m_pListSectorPlayer)
			delete [] m_pListSectorPlayer;
		if(m_pListSectorTower)
			delete [] m_pListSectorTower;
	}

	void CreateLayer(int nSecNum)
	{
		m_pMB = NULL;
		m_nSecNum = nSecNum;
		m_pListSectorObj = new CObjectList [nSecNum];
		m_pListSectorPlayer = new CObjectList [nSecNum];
		m_pListSectorTower = new CObjectList [nSecNum];
	}

	void ActiveLayer(_MULTI_BLOCK* pMB)
	{
		m_pMB = pMB;

		for(int i = 0; i < m_pMB->m_nBlockNum; i++)
		{
			int nActNum = m_pMB->m_ptbMonBlock[i].GetRecordNum();

			memcpy(m_MonAct[i], m_pMB->m_ppMonAct[i], sizeof(_mon_active)*nActNum);

			for(int a = 0; a < nActNum; a++)
			{
				m_MonAct[i][a].SetZeroMonNum();
			}
		}
		
		m_dwStartActiveTime = timeGetTime();
	}

	bool InertLayer()
	{
		m_pMB = NULL;
		return true;
	}

	bool IsActiveLayer()
	{
		if(m_pMB)
			return true;
		return false;
	}

};

//하나의 map의 모든 정보 및 interface를 담는 class
class CMapData
{
public:	

	bool				m_bUse;
	bool				m_bLoad;
	int					m_nMapIndex;

	CLevel				m_Level;					//bsp load 및 interface api class	==> R3Engine
	int					m_nMapCode;					//맵타입별 인덱스..		

	_LAYER_SET*			m_ls;
	_MULTI_BLOCK*		m_mb;
	CExtDummy			m_Dummy;

	int					m_nMapInPlayerNum;
	int					m_nMapInMonsterNum;

	int					m_nMonBlockNum;				//monster block 수
	_mon_block*			m_pMonBlock;				//monster block의 pointer ==> monster block 수만큼 memory allocation

	int					m_nMonDumNum;				//monster dummy 수

	int					m_nPortalNum;				//portal 수
	_portal_dummy*		m_pPortal;					//portal 수 만큼 memory allocation
	
	int					m_nItemStoreDumNum;	
	_store_dummy*		m_pItemStoreDummy;
	
	int					m_nStartDumNum;
	_start_dummy*		m_pStartDummy;

	int					m_nResDumNum;
	_res_dummy*			m_pResDummy;

	_map_fld*			m_pMapSet;					//xls에서 loading한 _map_record의 pointer

	CExtDummy*			m_pExtDummy_Town;
	CItemStore*			m_ItemStore;

	static CRecordData	s_tbItemStore;

private:

	enum			{	mine_grade_reset_timer =	30*60*1000		};

	CRecordData			m_tbMonBlk;
	CRecordData			m_tbPortal;

	CDummyPosTable		m_tbMonDumPos;
	CDummyPosTable		m_tbPortalDumPos;
	CDummyPosTable		m_tbStoreDumPos;
	CDummyPosTable		m_tbStartDumPos;
	CDummyPosTable		m_tbResDumPos;

	_bsp_info			m_BspInfo;
	_sec_info			m_SecInfo;
	CMyTimer			m_tmrMineGradeReSet;

public:

	CMapData();
	virtual ~CMapData();
	void Init(_map_fld* pMapSet);

	//server 시작 초기에 map을 load 
	bool OpenMap(LPTSTR pszMapCode, _map_fld* pMapSet, bool bUse);
	void OnLoop();
	
	_sec_info* GetSecInfo();
	_bsp_info* GetBspInfo();

	void GetRectInRadius(_pnt_rect* pRect, int nRadius, int nSecNum);
	_portal_dummy* GetPortal(char* pPortalCode);
	_portal_dummy* GetPortal(int nPortalIndex);
	_portal_dummy* GetLinkPortal(char* pPortalCode);
	char* GetServerCode();
	int	GetSectorIndex(float* pPos);
	CObjectList* GetSectorListObj(WORD wLayerIndex, DWORD dwSecIndex);
	CObjectList* GetSectorListPlayer(WORD wLayerIndex, DWORD dwSecIndex);
	CObjectList* GetSectorListTower(WORD wLayerIndex, DWORD dwSecIndex);
	int GetResDummySector(int nDummyIndex, float* pCurPos);//자원더미에서 섹터찾기..

	bool CheckCenterPosDummy(_dummy_position* pPos);
	bool GetRandPosInDummy(_dummy_position* pPos, float* OUT pNewPos, bool bRePos);
	void GetRandPosInRange(float* pStdPos, int nRange, float* OUT pNewPos);

	bool IsMapIn(float* fPos);
	BYTE  GetMapCode(){ return  (BYTE)m_pMapSet->m_dwIndex;	}
	bool IsBaseTown(float* fPos);                    //마을 인가?

	bool UpdateSecterList(CGameObject* pObj, DWORD dwOldSec, DWORD dwNewSec);
	void EnterMap(CGameObject* pObj, DWORD dwSecIndex);	//맵리스트 및 섹터리스트에 추가
	void ExitMap(CGameObject* pObj, DWORD dwSecIndex);	//맵리스트 및 섹터리스트에 삭제

	bool ConvertLocalToWorldDummy(LPTSTR pszMapCode, CDummyPosTable* pPosTable);

private:

	bool _LoadBspSec(LPTSTR pszMapCode);
	bool _LoadMonBlk(LPTSTR pszMapCode, _map_fld* pMapFld);
	bool _LoadPortal(LPTSTR pszMapCode);
	bool _LoadStore(LPTSTR pszMapCode);
	bool _LoadStart(LPTSTR pszMapCode);
	bool _LoadResource(LPTSTR pszMapCode);
};

#endif 
