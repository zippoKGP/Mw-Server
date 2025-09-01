// MapData.cpp: implementation of the CMapData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MapData.h"
#include "MyUtil.h"
#include "MonsterCharacterFld.h"
#include "MainThread.h"
#include "ItemStore.h"

CRecordData CMapData::s_tbItemStore;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapData::CMapData()
{
	m_pMapSet = NULL;
	m_pPortal = NULL;
	m_pItemStoreDummy = NULL; 
	m_pStartDummy = NULL;
	m_pResDummy = NULL;
	m_pMonBlock = NULL;
	m_ItemStore = NULL;
	m_pExtDummy_Town = NULL;
	m_ls = NULL;
	m_mb = NULL;

	m_nMonBlockNum = 0;
	m_nMonDumNum = 0;
	m_nPortalNum = 0;
	m_nItemStoreDumNum = 0;
	m_nMapInPlayerNum = 0;
	m_nMapInMonsterNum = 0;
	m_bLoad = false;
	m_bUse = false;
}

CMapData::~CMapData()
{
	if(m_bLoad)
	{
		if(m_pMonBlock)
			delete [] m_pMonBlock;

		if(m_pPortal)
			delete m_pPortal;

		if(m_pItemStoreDummy)
			delete [] m_pItemStoreDummy;

		if(m_pStartDummy)
			delete m_pStartDummy;

		if(m_pResDummy)
			delete [] m_pResDummy;

		if(m_ItemStore)
			delete [] m_ItemStore;

		if(m_ls)
			delete [] m_ls;

		if(m_mb)
			delete [] m_mb;

	//	if(m_Level.IsLoadedBsp())
			m_Level.ReleaseLevel();
	}
}

void CMapData::Init(_map_fld* pMapSet)
{
	m_nMapCode = pMapSet->m_dwIndex;
	m_pMapSet = pMapSet;
}

bool CMapData::OpenMap(LPTSTR pszMapCode, _map_fld* pMapSet, bool bUse)
{
	if(pMapSet->m_nLayerNum > max_layer_num)
	{
		::MyMessageBox("CMapData Error", "%s: layer 수(%d > %d)가 많다..", pszMapCode, pMapSet->m_nLayerNum, max_layer_num);
		return false;
	}

	//_map_fld에서 m_nMapType일 일반맵이면 레이어는 반드시 하나..
	if(pMapSet->m_nMapType == MAP_TYPE_STD && pMapSet->m_nLayerNum > 1)
		pMapSet->m_nLayerNum = 1;
	//_map_fld에서 m_nMapType일 일반맵이면 몬스터세팅파일도 반드시 하나..
	if(pMapSet->m_nMapType == MAP_TYPE_STD && pMapSet->m_nMonsterSetFileNum > 1)
		pMapSet->m_nMonsterSetFileNum = 1;	

	m_ls = new _LAYER_SET [pMapSet->m_nLayerNum];
	m_mb = new _MULTI_BLOCK [pMapSet->m_nMonsterSetFileNum];

	m_pMapSet = pMapSet;
	m_bUse = bUse;
	m_nMapIndex = pMapSet->m_dwIndex;

	//더미리스트 세팅
	char szExtDmyFileName[128];
	sprintf(szExtDmyFileName, ".\\map\\%s\\%s.spt", pszMapCode, pszMapCode);
	if(!m_Dummy.LoadExtDummy(szExtDmyFileName))
	{
		::MyMessageBox("CMapData Error", "더미리스트(%s) 읽기 실패", pszMapCode);
		return false;
	}

	//마을더미가 있나.. 있으면 세팅..
	sprintf(szExtDmyFileName, ".\\map\\%s\\%sEXT.spt", pszMapCode, pszMapCode);
	if(::CalcFileSize(szExtDmyFileName) > 0)
	{
		m_pExtDummy_Town = new CExtDummy;
		if(!m_pExtDummy_Town->LoadExtDummy(szExtDmyFileName))
		{
			delete m_pExtDummy_Town;
			m_pExtDummy_Town = NULL;

			::MyMessageBox("CMapData Error", "마을더미리스트(%s) 읽기 실패", pszMapCode);
			return false;
		}		
	}

	if(!_LoadBspSec(pszMapCode))
		return false;

	//sector수 만큼 m_pObjList를 할당
	for(int i = 0; i < pMapSet->m_nLayerNum; i++)
		m_ls[i].CreateLayer(m_SecInfo.m_nSecNum);

	if(!_LoadMonBlk(pszMapCode, pMapSet))
		return false;

	if(!_LoadPortal(pszMapCode))
		return false;

	if(!_LoadStore(pszMapCode))
		return false;

	if(!_LoadStart(pszMapCode))
		return false;

	if(!_LoadResource(pszMapCode))
		return false;

	m_tmrMineGradeReSet.BeginTimer(mine_grade_reset_timer);

	m_bLoad = true;

	if(pMapSet->m_nMapType == MAP_TYPE_STD)	//일반맵은 0번 몬스터세팅으로한다..
		m_ls[0].ActiveLayer(&m_mb[0]);
											//던전맵은 실시간으로 액티브시킨다..

	return true;
}

void CMapData::OnLoop()
{
	if(m_tmrMineGradeReSet.CountingTimer())
	{
		for(int i = 0; i < m_nResDumNum; i++)
			m_pResDummy[i].SetRangeGrade();
	}
}

bool CMapData::_LoadBspSec(LPTSTR pszMapCode)
{
	if(m_Level.IsLoadedBsp())
	{
		::MyMessageBox("CMapData Error", "m_Level.IsLoadedBsp() == false");
		return false;
	}

	char strFile[128];

	//BSP Load..
	sprintf(strFile, ".\\Map\\%s\\%s.bsp", pszMapCode, pszMapCode);
	m_Level.LoadLevel(strFile);

	sprintf(m_BspInfo.m_szBspName, "%s", pszMapCode);
	m_BspInfo.m_szBspName[strlen(pszMapCode)] = NULL;

	m_BspInfo.m_nLeafNum = m_Level.mBsp->mLeafNum;

	for(int i = 0; i < 3; i++)
	{
		m_BspInfo.m_nMapMaxSize[i] = m_Level.mBsp->mNode[1].bb_max[i];
		m_BspInfo.m_nMapMinSize[i] = m_Level.mBsp->mNode[1].bb_min[i];
		m_BspInfo.m_nMapSize[i] = m_BspInfo.m_nMapMaxSize[i]-m_BspInfo.m_nMapMinSize[i];
	}

	if(m_pMapSet->m_bSizeAdjust)
	{
		m_BspInfo.m_nMapMinSize[0] = m_pMapSet->m_nMinX;
		m_BspInfo.m_nMapMinSize[2] = m_pMapSet->m_nMinY;
		m_BspInfo.m_nMapMaxSize[0] = m_pMapSet->m_nMaxX;
		m_BspInfo.m_nMapMaxSize[2] = m_pMapSet->m_nMaxY;
		m_BspInfo.m_nMapSize[0] = m_BspInfo.m_nMapMaxSize[0]-m_BspInfo.m_nMapMinSize[0];
		m_BspInfo.m_nMapSize[2] = m_BspInfo.m_nMapMaxSize[2]-m_BspInfo.m_nMapMinSize[2];
	}

	m_SecInfo.m_nSecNumW = m_BspInfo.m_nMapSize[0]/MAP_STD_SEC_SIZE+1;
	m_SecInfo.m_nSecNumH = m_BspInfo.m_nMapSize[2]/MAP_STD_SEC_SIZE+1;
	m_SecInfo.m_nSecNum = m_SecInfo.m_nSecNumW*m_SecInfo.m_nSecNumH;
	
	return true;
}

bool CMapData::_LoadMonBlk(LPTSTR pszMapCode, _map_fld* pMapFld)
{
	char strFile[128];
	static _dummy_position* ppPos[0xFFFF];

	//Dummys Position Load..
	sprintf(strFile, ".\\map\\%s\\%s.spt", pszMapCode, pszMapCode);
	if(!m_tbMonDumPos.LoadDummyPosition(strFile, "*dm"))
	{
		::MyMessageBox("CMapData Error", "m_tbMonDumPos.LoadDummyPosition(%s) == false", strFile);
		return false;
	}
	if(!ConvertLocalToWorldDummy(pszMapCode, &m_tbMonDumPos))
		return false;

	//Block Table Load..
	sprintf(strFile, ".\\Map\\%s\\%s-[BLOCK].dat", pszMapCode, pszMapCode);
	if(!m_tbMonBlk.ReadRecord(strFile, sizeof(_mon_block_fld)))
	{
		::MyMessageBox("CMapData Error", "m_tbMonBlk.ReadRecord(%s) == false", strFile);
		return false;
	}

	/////////////////////
	//블럭별 더미 세팅..
	m_nMonBlockNum = m_tbMonBlk.GetRecordNum();
	if(m_nMonBlockNum > max_block_num)
	{
		::MyMessageBox("CMapData Error", "%s: m_nMonBlockNum(%d) > max_block_num(%d)", pszMapCode, m_nMonBlockNum, max_block_num);
		return false;
	}

	m_pMonBlock = new _mon_block [m_nMonBlockNum];

	for(int i = 0; i < m_nMonBlockNum; i++)//Block의 수만큼..
	{
		_mon_block_fld* pBlkFld = (_mon_block_fld*)m_tbMonBlk.GetRecord(i);
		if(!pBlkFld)
		{
			::MyMessageBox("CMapData Error", "(_mon_block_fld*)m_tbMonBlk.GetRecord(%d) == NULL", i);
			return false;
		}
		if(pBlkFld->m_dwDummyNum > 0xFFFF)
		{
			::MyMessageBox("CMapData Error", "_LoadMonBlk(%s).. if(pBlkRec(%d)->m_dwDummyNum(%d) > 0xFFFF)", pszMapCode, pBlkFld->m_dwIndex, pBlkFld->m_dwDummyNum);
			return false;
		}

		m_nMonDumNum += pBlkFld->m_dwDummyNum;

		//m_pMonBlock 을 작성..
		for(int p = 0; p < pBlkFld->m_dwDummyNum; p++)
		{
			if(!strcmp(pBlkFld->m_DummyInfo[p].m_strDummyCode, "0"))
			{
				//::MyMessageBox("MonsterBlock Warning", "Map:%s, MonBlock:%s.. 더미수(%d->%d으로 수정)", pszMapCode, pBlkRec->m_strCode, pBlkRec->m_dwDummyNum, p);
				g_Main.m_logSystemError.Write("Map:%s, MonBlock:%s.. 더미수(%d->%d으로 수정)", pszMapCode, pBlkFld->m_strCode, pBlkFld->m_dwDummyNum, p);
				pBlkFld->m_dwDummyNum = p;
				break;
			}

			ppPos[p] = m_tbMonDumPos.GetRecord(pBlkFld->m_DummyInfo[p].m_strDummyCode);							
			if(!ppPos[p])
			{
				::MyMessageBox("CMapData Error", "m_tbMonDumPos.GetRecord(%s) == NULL", pBlkFld->m_DummyInfo[p].m_strDummyCode);
				return false;
			}

			CheckCenterPosDummy(ppPos[p]);
		}

		if(!m_pMonBlock[i].SetBlock(pBlkFld, this, ppPos))
		{
			::MyMessageBox("CMapData Error", "%s: m_pMonBlock[%d].SetBlock(%s) == false", pszMapCode, i, pBlkFld->m_strCode);
			return false;
		}
	}

	////////////////////////
	//블럭별 몬스터 세팅..
	if(pMapFld->m_nMonsterSetFileNum > max_multi_block_num)
	{
		::MyMessageBox("CMapData Error", "%s: 블럭별 몬스터 세팅파일수가 많다.(%d > %d)", pszMapCode, pMapFld->m_nMonsterSetFileNum, max_multi_block_num);
		return false;
	}

	for(int s = 0; s < pMapFld->m_nMonsterSetFileNum; s++)
	{	
		m_mb[s].m_nBlockNum = m_nMonBlockNum;
		m_mb[s].m_ptbMonBlock = new CRecordData [m_nMonBlockNum];
		m_mb[s].m_ppMonAct = new _mon_active* [m_nMonBlockNum];

		for(i = 0; i < m_nMonBlockNum; i++)//Block의 수만큼..
		{
			_mon_block_fld* pBlkFld = (_mon_block_fld*)m_tbMonBlk.GetRecord(i);

			//Block의 Dummy Table Load..
			if(pMapFld->m_nMapType == MAP_TYPE_STD)
				sprintf(strFile, ".\\Map\\%s\\%s.dat", pszMapCode, ((_mon_block_fld*)m_tbMonBlk.GetRecord(i))->m_strCode);
			else
				sprintf(strFile, ".\\Map\\%s\\%s_%d.dat", pszMapCode, ((_mon_block_fld*)m_tbMonBlk.GetRecord(i))->m_strCode, s);

			if(!m_mb[s].m_ptbMonBlock[i].ReadRecord(strFile, sizeof(_mon_active_fld)))
			{
				::MyMessageBox("CMapData Error", "m_ptbMonDum[%d].ReadRecord(%s) == false", i, strFile);
				return false;
			}

			int nActNum = m_mb[s].m_ptbMonBlock[i].GetRecordNum();//블럭별 ..액션수..

			if(nActNum > max_act_num)
			{
				::MyMessageBox("CMapData Error", "%s: nActNum(%d) > max_act_num(%d)", pszMapCode, nActNum, max_act_num);
				return false;
			}

			//위까지는 공용,,,
			//아래부턴 맵 몬스터spawn별..
			m_mb[s].m_ppMonAct[i] = new _mon_active [nActNum];

			//m_ppMonDummy 작성..
			for(int n = 0; n < nActNum; n++)//위 한Block의 nActNum 수만큼..
			{
				_mon_active_fld* pRec = (_mon_active_fld*)m_mb[s].m_ptbMonBlock[i].GetRecord(n);
				if(!pRec)
				{
					::MyMessageBox("CMapData Error", "(_mon_dummy_fld*)m_ptbMonDum[%d]->GetRecord(%d) == NULL", i, n);
					return false;
				}		

				_monster_fld* pMonRec = (_monster_fld*)g_Main.m_tblMonster.GetRecord(pRec->m_strCode);
				int nMonIndex;
				if(!pMonRec)
				{
					::MyMessageBox("CMapData Error", "(_monster_record*)g_Main.m_tblMonsterCharacter.GetRecord(%s) == NULL", pRec->m_strCode);
					return false;
				}
				else
					nMonIndex = pMonRec->m_dwIndex;

				if(!m_mb[s].m_ppMonAct[i][n].SetActive(pRec, &m_pMonBlock[i], nMonIndex))	
				{
					::MyMessageBox("CMapData Error", "m_ppMonDummy[%d][%d].SetDummy(pRec, pPos, &m_pMonBlock[%d]) == false", i, n, i);
					return false;
				}
			}
		}
	}

	return true;
}

bool CMapData::_LoadPortal(LPTSTR pszMapCode)
{
	char strFile[128];

	//Dummys Position Load..
	sprintf(strFile, ".\\map\\%s\\%s.spt", pszMapCode, pszMapCode);
	if(!m_tbPortalDumPos.LoadDummyPosition(strFile, "*dp"))
	{
		::MyMessageBox("CMapData Error", "m_tbPortalDumPos.LoadDummyPosition(%s) == false", strFile);
		return false;
	}
	if(!ConvertLocalToWorldDummy(pszMapCode, &m_tbPortalDumPos))
		return false;
	
	sprintf(strFile, ".\\map\\%s\\%s-[Portal].dat", pszMapCode, pszMapCode);
	if(!m_tbPortal.ReadRecord(strFile, sizeof(_portal_fld)))
	{
		::MyMessageBox("CMapData Error", "m_tbPortal.ReadRecord(%s) == false", strFile);
		return false;
	}

	m_nPortalNum = m_tbPortal.GetRecordNum();
	m_pPortal = new _portal_dummy [m_nPortalNum];

	for(int i = 0; i < m_nPortalNum; i++)
	{
		_portal_fld* pRec = (_portal_fld*)m_tbPortal.GetRecord(i);
		if(!pRec)
		{
			::MyMessageBox("CMapData-LoadPortal(LPTSTR pszMapCode) Error", "Map(%s): (_portal_record*)m_tbPortal.GetRecord(%d) = NULL", pszMapCode, i);
			return false;
		}
		_dummy_position* pPos = m_tbPortalDumPos.GetRecord(pRec->m_strCode);		
		if(!pPos)
		{
			::MyMessageBox("CMapData-LoadPortal(LPTSTR pszMapCode) Error", "Map(%s): tbPortalPos.GetRecord(%s) = NULL", pszMapCode, pRec->m_strCode);
			g_Main.m_logSystemError.Write("Map(%s): tbPortalPos.GetRecord(%s) = NULL", pszMapCode, pRec->m_strCode);
			return false;
		}

		if(pRec && pPos)
		{
			m_pPortal[i].SetDummy(pRec, pPos);

			CheckCenterPosDummy(pPos);
		}
	}

	return true;
}

bool CMapData::_LoadStore(LPTSTR pszMapCode)
{
	//1. *.spt file load
	char strFile[128];
	sprintf(strFile, ".\\map\\%s\\%s.spt", pszMapCode, pszMapCode);
	if(!m_tbStoreDumPos.LoadDummyPosition(strFile, "*sd"))
	{
		::MyMessageBox("CMapData Error", "m_tbStoreDumPos.LoadDummyPosition(%s) == false", strFile);
		return false;
	}
	if(!ConvertLocalToWorldDummy(pszMapCode, &m_tbStoreDumPos))
		return false;

	//2. store table load
	if(!s_tbItemStore.IsTableOpen())
	{
		if(!s_tbItemStore.ReadRecord(".\\Script\\StoreList.dat", sizeof(_StoreList_fld)))
		{
			::MyMessageBox("CMapData Error", "s_tbItemStore.ReadRecord("".\\Script\\StoreList.dat"") == false");
			return false;
		}
	}

	//3. store number countting in this map
	for(int r = 0; r < s_tbItemStore.GetRecordNum(); r++)
	{
		_StoreList_fld* pRec = (_StoreList_fld*)s_tbItemStore.GetRecord(r);
		if(!strcmp(pRec->m_strStore_MAPcode, pszMapCode))
			m_nItemStoreDumNum++;
	}

	//4. pre check store exist in this map
	if(m_tbStoreDumPos.GetRecordNum() == 0)
		return true;

	if(m_nItemStoreDumNum > 0)
	{
		//5. _store_dummy struct memory load
		m_pItemStoreDummy = new _store_dummy [m_nItemStoreDumNum];
		m_ItemStore = new CItemStore [m_nItemStoreDumNum];

		//6. _store_dummy struct setting
		int nCnt = 0;
		for(r = 0; r < s_tbItemStore.GetRecordNum(); r++)
		{
			_StoreList_fld* pRec = (_StoreList_fld*)s_tbItemStore.GetRecord(r);

			if(!strcmp(pRec->m_strStore_MAPcode, pszMapCode))
			{
				_dummy_position* pPos = m_tbStoreDumPos.GetRecord(pRec->m_strCode);
				if(!pPos)
				{
					::MyMessageBox("CMapData-LoadStore(LPTSTR pszMapCode) Error", "Map(%s): m_tbStoreDumPos.GetRecord(%s) = NULL", pszMapCode, pRec->m_strCode);
					return false;					
				}

				_store_dummy* pStoreDum = &m_pItemStoreDummy[nCnt];

				pStoreDum->SetDummy(_store_dummy::item_store, (_base_fld*)pRec, pPos);

				CheckCenterPosDummy(pPos);

				m_ItemStore[nCnt].Init(nCnt, this, pStoreDum, (_base_fld*)pRec);
				nCnt++;
			}			
		}
	}

	return true;
}

bool CMapData::_LoadStart(LPTSTR pszMapCode)
{
	char strFile[128];
	sprintf(strFile, ".\\map\\%s\\%s.spt", pszMapCode, pszMapCode);
	if(!m_tbStartDumPos.LoadDummyPosition(strFile, "*ds"))
	{
		::MyMessageBox("CMapData Error", "m_tbStartDumPos.LoadDummyPosition(%s) == false", strFile);
		return false;
	}
	if(!ConvertLocalToWorldDummy(pszMapCode, &m_tbStartDumPos))
		return false;

	m_nStartDumNum = m_tbStartDumPos.GetRecordNum();
	if(m_nStartDumNum > 1)
	{
		::MyMessageBox("CMapData Error", "Map[%s]-m_tbStartDumPos.GetRecordNum()[%d] > 1", pszMapCode, m_nStartDumNum);
		return false;
	}
	if(m_pMapSet->m_nRaceVillageCode == 100 && m_nStartDumNum != 0)
	{
		::MyMessageBox("CMapData Error", "Not Start Village Map[%s]", pszMapCode);
		return false;
	}
	if(m_pMapSet->m_nRaceVillageCode != 100 && m_nStartDumNum != 1)
	{
		::MyMessageBox("CMapData Error", "Village Has not Start.. Map[%s]", pszMapCode);
		return false;
	}

	if(m_nStartDumNum == 0)
		return true;

	m_pStartDummy = new _start_dummy;

	_dummy_position* pPos = m_tbStartDumPos.GetRecord("dsstart");
	if(!pPos)
	{
		::MyMessageBox("CMapData-LoadStart(%s) Error", "m_tbStartDumPos.GetRecord(0) = NULL", pszMapCode);
		return false;
	}

	m_pStartDummy->SetDummy(pPos);

	CheckCenterPosDummy(pPos);
	
	return true;
}

bool CMapData::_LoadResource(LPTSTR pszMapCode)
{
	char strFile[128];
	sprintf(strFile, ".\\map\\%s\\%s.spt", pszMapCode, pszMapCode);
	if(!m_tbResDumPos.LoadDummyPosition(strFile, "*dr"))
	{
		::MyMessageBox("CMapData Error", "m_tbResDumPos.LoadDummyPosition(%s) == false", strFile);
		return false;
	}
	if(!ConvertLocalToWorldDummy(pszMapCode, &m_tbResDumPos))
		return false;

	m_nResDumNum = m_tbResDumPos.GetRecordNum();

	if(m_nResDumNum == 0)
	{
		return true;
	}

	m_pResDummy = new _res_dummy [m_nResDumNum];

	for(int i = 0; i < m_nResDumNum; i++)
	{
		_dummy_position* pPos = m_tbResDumPos.GetRecord(i);
		m_pResDummy[i].SetDummy(pPos);

		CheckCenterPosDummy(pPos);
	}
	
	return true;
}

_sec_info* CMapData::GetSecInfo()
{
	return &m_SecInfo;
}

_bsp_info* CMapData::GetBspInfo()
{
	return &m_BspInfo;
}

void CMapData::GetRectInRadius(_pnt_rect* pRect, int nRadius, int nSecNum)
{
	int nSecW = nSecNum%m_SecInfo.m_nSecNumW;
	int nSecH = nSecNum/m_SecInfo.m_nSecNumW;

	pRect->nStartx = nSecW-nRadius;
	if(pRect->nStartx < 0)
		pRect->nStartx = 0;

	pRect->nStarty = nSecH-nRadius;
	if(pRect->nStarty < 0)
		pRect->nStarty = 0;

	pRect->nEndx = nSecW+nRadius;
	if(pRect->nEndx > m_SecInfo.m_nSecNumW-1)
		pRect->nEndx = m_SecInfo.m_nSecNumW-1;

	pRect->nEndy = nSecH+nRadius;
	if(pRect->nEndy > m_SecInfo.m_nSecNumH-1)
		pRect->nEndy = m_SecInfo.m_nSecNumH-1;	
}

_portal_dummy* CMapData::GetPortal(char* pPortalCode)
{
	for(int i = 0; i < m_nPortalNum; i++)
	{
		if(!strcmp(pPortalCode, m_pPortal[i].m_pPortalRec->m_strCode))
			return &m_pPortal[i];
	}
	return NULL;
}

_portal_dummy* CMapData::GetPortal(int nPortalIndex)
{
	if(nPortalIndex >= m_nPortalNum)
		return NULL;

	return &m_pPortal[nPortalIndex];
}

_portal_dummy* CMapData::GetLinkPortal(char* pPortalCode)
{
	for(int i = 0; i < m_nPortalNum; i++)
	{
		if(!strcmp(m_pPortal[i].m_pPortalRec->m_strLinkPortalCode, pPortalCode))
			return &m_pPortal[i];		
	}
	return NULL;
}

char* CMapData::GetServerCode()
{
	return m_pMapSet->m_strServerCode;
}

int	CMapData::GetSectorIndex(float* pPos)
{
	float fAbsPos[3];

	fAbsPos[0] = -m_BspInfo.m_nMapMinSize[0]+pPos[0];
	fAbsPos[2] = m_BspInfo.m_nMapMaxSize[2]-pPos[2];

	DWORD nSecX = fAbsPos[0]/MAP_STD_SEC_SIZE;
	DWORD nSecY = fAbsPos[2]/MAP_STD_SEC_SIZE;

	if(nSecX >= m_SecInfo.m_nSecNumW || nSecY >= m_SecInfo.m_nSecNumH)
	{
		return -1;
	}

	return nSecX+nSecY*m_SecInfo.m_nSecNumW;
}

CObjectList* CMapData::GetSectorListObj(WORD wLayerIndex, DWORD dwSecIndex)
{
	return &m_ls[wLayerIndex].m_pListSectorObj[dwSecIndex];
}

CObjectList* CMapData::GetSectorListPlayer(WORD wLayerIndex, DWORD dwSecIndex)
{
	return &m_ls[wLayerIndex].m_pListSectorPlayer[dwSecIndex];
}

CObjectList* CMapData::GetSectorListTower(WORD wLayerIndex, DWORD dwSecIndex)
{
	return &m_ls[wLayerIndex].m_pListSectorTower[dwSecIndex];
}

int CMapData::GetResDummySector(int nDummyIndex, float* pCurPos)
{//실패는 -1
	if(m_nResDumNum <= 0)
		return -1;
	_res_dummy* pDum = &m_pResDummy[nDummyIndex];
	_dummy_position* pPos = pDum->m_pDumPos;

	_EXT_DUMMY* pExtDum = m_Dummy.GetDummy(pPos->m_wLineIndex);
	if(!pExtDum)
		return -1;

	int nSector = -1;
	for(int i = 0; i < _res_dummy::range_grade_num; i++)
	{
		memcpy(pExtDum->mBBmin, pDum->m_fMinLocal[i], sizeof(float)*3);
		memcpy(pExtDum->mBBmax, pDum->m_fMaxLocal[i], sizeof(float)*3);

		if(m_Dummy.IsInBBox(pPos->m_wLineIndex, pCurPos))
		{
			nSector = i;
			break;
		}
	}

	//되돌리기..
	memcpy(pExtDum->mBBmin, pDum->m_fMinLocal[2], sizeof(float)*3);
	memcpy(pExtDum->mBBmax, pDum->m_fMinLocal[2], sizeof(float)*3);

	return nSector;
}

bool CMapData::IsMapIn(float* fPos)
{
	for(int i = 0; i < 3; i++)
	{
		if(fPos[i] > m_BspInfo.m_nMapMaxSize[i] || fPos[i] < m_BspInfo.m_nMapMinSize[i])
			return false;
	}

	return true;
}

bool CMapData::UpdateSecterList(CGameObject* pObj, DWORD dwOldSec, DWORD dwNewSec)
{	
	if(dwOldSec != 0xFFFFFFFF)
	{
		GetSectorListObj(pObj->m_wMapLayerIndex, dwOldSec)->DeleteItem(&pObj->m_SectorPoint);
	}
	GetSectorListObj(pObj->m_wMapLayerIndex, dwNewSec)->PushItem(&pObj->m_SectorPoint);

	if(pObj->m_ObjID.m_byKind == obj_kind_char && pObj->m_ObjID.m_byID == obj_id_player)
	{
		if(dwOldSec != 0xFFFFFFFF)
		{
			GetSectorListPlayer(pObj->m_wMapLayerIndex, dwOldSec)->DeleteItem(&pObj->m_SectorNetPoint);
		}
		GetSectorListPlayer(pObj->m_wMapLayerIndex, dwNewSec)->PushItem(&pObj->m_SectorNetPoint);
	}

	return true;
}

void CMapData::EnterMap(CGameObject* pObj, DWORD dwSecIndex)
{
	if(dwSecIndex >= m_SecInfo.m_nSecNum)
		return;

	GetSectorListObj(pObj->m_wMapLayerIndex, dwSecIndex)->PushItem(&pObj->m_SectorPoint);

	if(pObj->m_ObjID.m_byKind == obj_kind_char)
	{
		if(pObj->m_ObjID.m_byID == obj_id_player)
			m_nMapInPlayerNum++;
		else if(pObj->m_ObjID.m_byID == obj_id_monster)
			m_nMapInMonsterNum++;
	}

	if(pObj->m_ObjID.m_byKind == obj_kind_char && pObj->m_ObjID.m_byID == obj_id_player)
	{
		GetSectorListPlayer(pObj->m_wMapLayerIndex, dwSecIndex)->PushItem(&pObj->m_SectorNetPoint);
	}
	else if(pObj->m_ObjID.m_byKind == obj_kind_char && pObj->m_ObjID.m_byID == obj_id_tower)
	{
		GetSectorListTower(pObj->m_wMapLayerIndex, dwSecIndex)->PushItem(&pObj->m_SectorNetPoint);
	}
}

void CMapData::ExitMap(CGameObject* pObj, DWORD dwSecIndex)
{
	if(dwSecIndex >= m_SecInfo.m_nSecNum)
		return;

	GetSectorListObj(pObj->m_wMapLayerIndex, dwSecIndex)->DeleteItem(&pObj->m_SectorPoint);

	if(pObj->m_ObjID.m_byKind == obj_kind_char)
	{
		if(pObj->m_ObjID.m_byID == obj_id_player)
			m_nMapInPlayerNum--;
		else if(pObj->m_ObjID.m_byID == obj_id_monster)
			m_nMapInMonsterNum--;
	}

	if(pObj->m_ObjID.m_byKind == obj_kind_char && pObj->m_ObjID.m_byID == obj_id_player)
	{
		GetSectorListPlayer(pObj->m_wMapLayerIndex, dwSecIndex)->DeleteItem(&pObj->m_SectorNetPoint);
	}
	else if(pObj->m_ObjID.m_byKind == obj_kind_char && pObj->m_ObjID.m_byID == obj_id_tower)
	{
		GetSectorListTower(pObj->m_wMapLayerIndex, dwSecIndex)->DeleteItem(&pObj->m_SectorNetPoint);
	}
}

bool CMapData::ConvertLocalToWorldDummy(LPTSTR pszMapCode, CDummyPosTable* pPosTable)
{
	int nDummyNum = pPosTable->GetRecordNum();

	for(int i = 0; i < nDummyNum; i++)
	{
		_dummy_position* pPos = pPosTable->GetRecord(i);

		//min, max를 월드 좌표로..
		float fLocalMin[3] = {pPos->m_zLocalMin[0], pPos->m_zLocalMin[1], pPos->m_zLocalMin[2]};
		float fLocalMax[3] = {pPos->m_zLocalMax[0], pPos->m_zLocalMax[1], pPos->m_zLocalMax[2]};

		if(!m_Dummy.GetWorldFromLocal((Vector3f*)pPos->m_fMin, pPos->m_wLineIndex, fLocalMin))
		{
			::MyMessageBox("CMapData Error", "ConvertLocalToWorldDummy map:%s, dummy:%s", pszMapCode, pPos->m_szCode);
			return false;
		}

		if(!m_Dummy.GetWorldFromLocal((Vector3f*)pPos->m_fMax, pPos->m_wLineIndex, fLocalMax))
		{
			::MyMessageBox("CMapData Error", "ConvertLocalToWorldDummy map:%s, dummy:%s", pszMapCode, pPos->m_szCode);
			return false;
		}

		float fLocalRT[3] = {pPos->m_zLocalMax[0], pPos->m_zLocalMin[1], pPos->m_zLocalMin[2]};
		float fLocalLB[3] = {pPos->m_zLocalMin[0], pPos->m_zLocalMax[1], pPos->m_zLocalMax[2]};

		if(!m_Dummy.GetWorldFromLocal((Vector3f*)pPos->m_fRT, pPos->m_wLineIndex, fLocalRT))
		{
			::MyMessageBox("CMapData Error", "ConvertLocalToWorldDummy map:%s, dummy:%s", pszMapCode, pPos->m_szCode);
			return false;
		}

		if(!m_Dummy.GetWorldFromLocal((Vector3f*)pPos->m_fLB, pPos->m_wLineIndex, fLocalLB))
		{
			::MyMessageBox("CMapData Error", "ConvertLocalToWorldDummy map:%s, dummy:%s", pszMapCode, pPos->m_szCode);
			return false;
		}
	}

	return true;
}

bool CMapData::CheckCenterPosDummy(_dummy_position* pPos)
{
	float fLocalBuffer[3];

	for(int d = 0; d < 3; d++)
		fLocalBuffer[d] = (pPos->m_zLocalMax[d] + pPos->m_zLocalMin[d])/2;

	if(!m_Dummy.GetWorldFromLocal((Vector3f*)pPos->m_fCenterPos, pPos->m_wLineIndex, fLocalBuffer))
	{
		::MyMessageBox("CMapData Error", "CheckCenterPosDummy map:%s, dummy:%s", m_pMapSet->m_strCode, pPos->m_szCode);
		return false;
	}

	pPos->m_fCenterPos[1] = m_Level.GetFirstYpos(pPos->m_fCenterPos, pPos->m_fMin, pPos->m_fMax);	//바운딩 박스 영역안에서 구한다.구할수 없는경우 65535을 리턴
	pPos->m_bPosAble = true;

	if(pPos->m_fCenterPos[1] == -65535)
	{
	//	::MyMessageBox("CheckPosDummy", "Map(%s), Dum(%s)\n", m_pMapSet->m_strCode, pPos->m_szCode);
	//	::__trace("CheckPosDummy: Map(%s), Dum(%s)\n", m_pMapSet->m_strCode, pPos->m_szCode);
		g_Main.m_logSystemError.Write("CheckPosDummy: Map(%s), Dum(%s)", m_pMapSet->m_strCode, pPos->m_szCode);
		pPos->m_bPosAble = false;

	/*	pPos->m_fMin[1] -= 100;
		pPos->m_fMax[1] += 100;
		pPos->m_fCenterPos[1] = m_Level.GetFirstYpos(pPos->m_fCenterPos, pPos->m_fMin, pPos->m_fMax);
		if(pPos->m_fCenterPos[1] == -65535)
		{
			pPos->m_bPosAble = false;
			return false;
		}*/
	}

	return true;
}

bool CMapData::GetRandPosInDummy(_dummy_position* pPos, float* OUT pNewPos, bool bRePos)
{
	if(!pPos->m_bPosAble)
		return false;

	float fLocalBuffer[3];

	for(int l = 0; l < 3; l++)
	{
		//로컬좌표에서 랜덤좌표를 ..
		fLocalBuffer[0] = ::rand()%(int)(pPos->m_zLocalMax[0] - pPos->m_zLocalMin[0]) + pPos->m_zLocalMin[0];
		fLocalBuffer[2] = ::rand()%(int)(pPos->m_zLocalMax[2] - pPos->m_zLocalMin[2]) + pPos->m_zLocalMin[2];
		fLocalBuffer[1] = (pPos->m_zLocalMax[1] + pPos->m_zLocalMin[1])/2;

		if(!m_Dummy.GetWorldFromLocal((Vector3f*)pNewPos, pPos->m_wLineIndex, fLocalBuffer))
		{
			::MyMessageBox("CMapData Error", "GetRandPosInDummy map:%s, dummy:%s", m_pMapSet->m_strCode, pPos->m_szCode);
			return false;
		}

		pNewPos[1] = m_Level.GetFirstYpos(pNewPos, pPos->m_fMin, pPos->m_fMax);
		if(pNewPos[1] == -65535)
			continue;

		Vector3f fNew;
		if(!m_Level.mBsp->CanYouGoThere(pPos->m_fCenterPos, pNewPos, &fNew))
		{
			if(bRePos)//한번에 갈수없는 곳이라면 최대한 앞으로 땡겨준다..
			{
				pNewPos[0] = fNew[0];
				pNewPos[2] = fNew[2];
				pNewPos[1] = m_Level.GetFirstYpos(pNewPos, pPos->m_fMin, pPos->m_fMax);
				if(pNewPos[1] == -65535)
					continue;
			}
			else
				continue;
		}
		return true;
	}

//	g_Main.m_logSystemError.Write("GetRandPosInDummy: Map(%s), Dum(%s)", m_pMapSet->m_strCode, pPos->m_szCode);
	memcpy(pNewPos, pPos->m_fCenterPos, sizeof(float)*3);	//위치잡기실패하면 중앙으로..

	return true;
}

void CMapData::GetRandPosInRange(float* pStdPos, int nRange, float* OUT pNewPos)
{
	static Vector3f fNew;
	
	int nCnt = 0;
	while(1)
	{
		pNewPos[0] = (pStdPos[0]-nRange/2) + ::rand()%nRange;
		pNewPos[2] = (pStdPos[2]-nRange/2) + ::rand()%nRange;
		pNewPos[1] = pStdPos[1];

		if(!m_Level.mBsp->CanYouGoThere(pStdPos, pNewPos, &fNew))
		{
			if(nCnt++ > 50)
				break;
		}
		else
			break;
	}
}

bool CMapData::IsBaseTown(float* fPos)                    //마을 인가?
{
	if(!m_pExtDummy_Town)
		return false;

	DWORD dwCnt;

	dwCnt = m_pExtDummy_Town->GetTotalNum();

	for(DWORD i = 0; i < dwCnt; i++)
	{
		if(m_pExtDummy_Town->IsInBBox(i, (Vector3f&)fPos))
			return true;
	}

	return false;
}
