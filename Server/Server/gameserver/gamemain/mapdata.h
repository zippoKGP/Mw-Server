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
#define dungeon_active_time	3*60*1000 //3��.. 600000	//10��

class CItemStore;

//map�� bsp������ ��� ����ü
struct _bsp_info
{
	char m_szBspName[50];	//map�� �̸�
	int m_nMapMaxSize[3];	//+������ �ִ밪
	int m_nMapMinSize[3];	//-������ �ּҰ�
	int m_nMapSize[3];	//����ũ��
	int m_nLeafNum;		//������	==> server������ �̻��
};

//map��  sector������ ��� ����ü
struct _sec_info
{
	int m_nSecNumW;	//������ sector��
	int m_nSecNumH;	//������ sector��
	int m_nSecNum;	//��ü sector��
};

//map�� ��ü sector�߿��� ���������� ��� ����ü
struct _pnt_rect
{
	int nStartx;	//�簢�������� left�� sector�� ���� index
	int nStarty;	//�簢�������� top�� sector�� ���� index
	int nEndx;		//�簢�������� right�� sector�� ���� index
	int nEndy;		//�簢�������� bottom�� sector�� ���� index
};

struct _MULTI_BLOCK
{	
	int					m_nBlockNum;
	CRecordData*		m_ptbMonBlock;
	_mon_active**		m_ppMonAct;					//block�� �� ��ŭ memory allocation �״��� block index���� dummy�� ��ŭ memory allocation

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
	
	//sector���� ������ object list ==> sector ����ŭ memory allocation
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

//�ϳ��� map�� ��� ���� �� interface�� ��� class
class CMapData
{
public:	

	bool				m_bUse;
	bool				m_bLoad;
	int					m_nMapIndex;

	CLevel				m_Level;					//bsp load �� interface api class	==> R3Engine
	int					m_nMapCode;					//��Ÿ�Ժ� �ε���..		

	_LAYER_SET*			m_ls;
	_MULTI_BLOCK*		m_mb;
	CExtDummy			m_Dummy;

	int					m_nMapInPlayerNum;
	int					m_nMapInMonsterNum;

	int					m_nMonBlockNum;				//monster block ��
	_mon_block*			m_pMonBlock;				//monster block�� pointer ==> monster block ����ŭ memory allocation

	int					m_nMonDumNum;				//monster dummy ��

	int					m_nPortalNum;				//portal ��
	_portal_dummy*		m_pPortal;					//portal �� ��ŭ memory allocation
	
	int					m_nItemStoreDumNum;	
	_store_dummy*		m_pItemStoreDummy;
	
	int					m_nStartDumNum;
	_start_dummy*		m_pStartDummy;

	int					m_nResDumNum;
	_res_dummy*			m_pResDummy;

	_map_fld*			m_pMapSet;					//xls���� loading�� _map_record�� pointer

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

	//server ���� �ʱ⿡ map�� load 
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
	int GetResDummySector(int nDummyIndex, float* pCurPos);//�ڿ����̿��� ����ã��..

	bool CheckCenterPosDummy(_dummy_position* pPos);
	bool GetRandPosInDummy(_dummy_position* pPos, float* OUT pNewPos, bool bRePos);
	void GetRandPosInRange(float* pStdPos, int nRange, float* OUT pNewPos);

	bool IsMapIn(float* fPos);
	BYTE  GetMapCode(){ return  (BYTE)m_pMapSet->m_dwIndex;	}
	bool IsBaseTown(float* fPos);                    //���� �ΰ�?

	bool UpdateSecterList(CGameObject* pObj, DWORD dwOldSec, DWORD dwNewSec);
	void EnterMap(CGameObject* pObj, DWORD dwSecIndex);	//�ʸ���Ʈ �� ���͸���Ʈ�� �߰�
	void ExitMap(CGameObject* pObj, DWORD dwSecIndex);	//�ʸ���Ʈ �� ���͸���Ʈ�� ����

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
