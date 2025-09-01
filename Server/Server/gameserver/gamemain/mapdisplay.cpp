// MapDisplay.cpp: implementation of the CMapDisplay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <d3d8.h>
#include "MapDisplay.h"
#include "dxutil.h"
#include "MapOperate.h"
#include "ZoneServerDoc.h"
#include "MyUtil.h"

static float fTempPos[3] = {0, 0, 0};

CMapDisplay::CMapDisplay()
{
	int i, j;

	m_bDisplayMode = false;
	m_bCirlcleLine = false;
	m_pActMap = NULL;
	m_wLayerIndex = 0;
	m_pOldActMap = NULL;
	m_tmrDraw.BeginTimer(timer_draw);

	for(i = 0; i < map_max; i++)
	{
		m_DummyDraw[i] = NULL;
		m_nDummyDrawNum[i] = 0;
	}

	CDummyDraw::InitPen();
	CCollLineDraw::InitPen();
	m_hPenBorder = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));

	for(i = 0; i < kind_num; i++)
	{
		for(j = 0; j < surface_max; j++)
			m_pSFObj[i][j] = NULL;
	}
	m_pSFMap = NULL;
	m_pSFSelect = NULL;
	m_pSFCircle = NULL;
	m_pSFBuf = NULL;
	m_pSFCorpse = NULL;
	
	m_Font.CreateFont( 15,                        // nHeight
					   0,                         // nWidth
					   0,                         // nEscapement
					   0,                         // nOrientation
					   FW_NORMAL,                 // nWeight
					   FALSE,                     // bItalic
					   FALSE,                     // bUnderline
					   0,                         // cStrikeOut
					   ANSI_CHARSET,              // nCharSet
					   OUT_DEFAULT_PRECIS,        // nOutPrecision
					   CLIP_DEFAULT_PRECIS,       // nClipPrecision
					   DEFAULT_QUALITY,           // nQuality
					   DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
					   "Arial");                  // lpszFacename

	m_MapExtend.Init(&m_pSFMap);
}

CMapDisplay::~CMapDisplay()
{
	if(m_bDisplayMode)
		ReleaseDisplay();

	CDummyDraw::DeletePen();
	CCollLineDraw::DeletePen();
	DeleteObject(m_hPenBorder);
	m_Font.DeleteObject(); 

	for(int i = 0; i < map_max; i++)
	{
		if(m_DummyDraw[i])
			delete [] m_DummyDraw[i];
	}
}

void CMapDisplay::InitDummy(CRect* prcWnd)
{
	int nMapNum = g_Main.m_MapOper.m_nMapNum;

	for(int m = 0; m < nMapNum; m++)
	{
		CMapData* pMap = &g_Main.m_MapOper.m_Map[m];

		m_nDummyDrawNum[m] = pMap->m_nMonDumNum + pMap->m_nPortalNum + pMap->m_nItemStoreDumNum + pMap->m_nStartDumNum + pMap->m_nResDumNum;
		if(m_nDummyDrawNum[m] > max_dummy_num)
		{
			::MyMessageBox("CMapDisplay::InitDummy", "%sMap 더미수[%d]가 %d보다 많다. ", pMap->m_pMapSet->m_strCode, m_nDummyDrawNum[m], max_dummy_num);
			continue;
		}
		
		if(!m_DummyDraw[m])
		{
			m_DummyDraw[m] = new CDummyDraw [m_nDummyDrawNum[m]];
		}
		else
		{
			continue;
		}
		
		int nDumCnt = 0;

		for(int b = 0; b < pMap->m_nMonBlockNum; b++)
		{
			_mon_block* pBlk = &pMap->m_pMonBlock[b];

			for(int d = 0; d < pBlk->m_pBlkRec->m_dwDummyNum; d++)
			{
				_dummy_position* pPos = pBlk->m_pDumPos[d];						
				if(!pPos->m_bPosAble)	//*.spt파일안에 더미위치를 로딩하지못한경우..
					m_DummyDraw[m][nDumCnt].SetDummyRange(pMap, pPos->m_fMin, pPos->m_fMax, pPos->m_fRT, pPos->m_fLB, CDummyDraw::type_fail_dummy, prcWnd);
				else
					m_DummyDraw[m][nDumCnt].SetDummyRange(pMap, pPos->m_fMin, pPos->m_fMax, pPos->m_fRT, pPos->m_fLB, CDummyDraw::type_mon_dummy, prcWnd);

				nDumCnt++;
			}
		}

		for(int p = 0; p < pMap->m_nPortalNum; p++)
		{
			_dummy_position* pPos = pMap->m_pPortal[p].m_pDumPos;
			if(!pPos->m_bPosAble)	//*.spt파일안에 더미위치를 로딩하지못한경우..
				m_DummyDraw[m][nDumCnt].SetDummyRange(pMap, pPos->m_fMin, pPos->m_fMax, pPos->m_fRT, pPos->m_fLB, CDummyDraw::type_fail_dummy, prcWnd);
			else
				m_DummyDraw[m][nDumCnt].SetDummyRange(pMap, pPos->m_fMin, pPos->m_fMax, pPos->m_fRT, pPos->m_fLB, CDummyDraw::type_potal_dummy, prcWnd);

			nDumCnt++;
		}
		for(int s = 0; s < pMap->m_nItemStoreDumNum; s++)
		{
			_dummy_position* pPos = pMap->m_pItemStoreDummy[s].m_pDumPos;
			if(!pPos->m_bPosAble)	
				m_DummyDraw[m][nDumCnt].SetDummyRange(pMap, pPos->m_fMin, pPos->m_fMax, pPos->m_fRT, pPos->m_fLB, CDummyDraw::type_fail_dummy, prcWnd);
			else
				m_DummyDraw[m][nDumCnt].SetDummyRange(pMap, pPos->m_fMin, pPos->m_fMax, pPos->m_fRT, pPos->m_fLB, CDummyDraw::type_item_store_dummy, prcWnd);

			nDumCnt++;
		}

		if(pMap->m_nStartDumNum > 0)
		{
			_dummy_position* pPos = pMap->m_pStartDummy->m_pDumPos;
			if(!pPos->m_bPosAble)	
				m_DummyDraw[m][nDumCnt].SetDummyRange(pMap, pPos->m_fMin, pPos->m_fMax, pPos->m_fRT, pPos->m_fLB, CDummyDraw::type_fail_dummy, prcWnd);
			else
				m_DummyDraw[m][nDumCnt].SetDummyRange(pMap, pPos->m_fMin, pPos->m_fMax, pPos->m_fRT, pPos->m_fLB, CDummyDraw::type_start_dummy, prcWnd);
			nDumCnt++;
		}

/*		for(int r = 0; r < pMap->m_nResDumNum; r++)
		{
			_res_dummy* pDum = &pMap->m_pResDummy[r];
			if(!pDum->m_pDumPos)
			{
				m_DummyDraw[m][nDumCnt].SetDummyRange(pMap, pDum->m_pDumPos->m_fMin, pDum->m_pDumPos->m_fMax, CDummyDraw::type_fail_dummy, prcWnd);
			}
			else
			{
				m_DummyDraw[m][nDumCnt].SetDummyRange(pMap, pDum->m_pDumPos->m_fMin, pDum->m_pDumPos->m_fMax, CDummyDraw::type_res_dummy, prcWnd);
			}

			nDumCnt++;
		}*/
	}
}
/*
void CMapDisplay::ReleaseDummy(int nMapIndex)
{
	memset(m_nDummyDrawNum, 0, sizeof(m_nDummyDrawNum));
	for(int m = 0; m < nMapNum; m++)
	{
		if(m_DummyDraw[m])
		{
			delete [] m_DummyDraw[m];
			m_DummyDraw[m] = NULL;
		}
	}
}*/

void CMapDisplay::InitCollLine(CRect* prcWnd)
{
	int nMapNum = g_Main.m_MapOper.m_nMapNum;

	for(int m = 0; m < nMapNum; m++)
	{
		CMapData* pMap = &g_Main.m_MapOper.m_Map[m];
		m_CollLineDraw[m].InitLine(pMap, prcWnd);	
	}
}

HRESULT CMapDisplay::InitSurface(CMapData* pMap)
{
	HRESULT hr;

	if(m_bDisplayMode)
	{
		hr = ReleaseDisplay();
		if(hr != DD_OK)
		{
			::MyMessageBox("CMapDisplay Error", "Release Failure");
			return hr;
		}
	}

	CWnd* pWnd = (CWnd*)&g_pDoc->m_DisplayView;

	g_pDoc->m_DisplayView.GetDrawableRect(&m_rcWnd);

	if(FAILED(hr = CDisplay::CreateWindowedDisplay(pWnd->m_hWnd, m_rcWnd.right, m_rcWnd.bottom)))
	{
		::MyMessageBox("CMapDisplay Error", "CreateWindowedDisplay Failure");
		return hr;
	}

	if(FAILED(hr = CDisplay::CreateSurface(&m_pSFMap, m_rcWnd.right, m_rcWnd.bottom)))
	{
		::MyMessageBox("CMapDisplay Error", "CreateSurface Failure");
		return hr;
	}

	if(FAILED(hr = CDisplay::CreateSurface(&m_pSFBuf, m_rcWnd.right, m_rcWnd.bottom)))
	{
		::MyMessageBox("CMapDisplay Error", "CreateSurface Failure");
		return hr;
	}

	if(FAILED(hr = m_pSFBuf->FillColor(RGB(82, 189, 155))))
	{
		::MyMessageBox("CMapDisplay Error", "FillColor Failure");
		return hr;
	}

	if(FAILED(hr = CreateObjSurface()))
	{
		::MyMessageBox("CMapDisplay Error", "CreateObjSurface Failure");
		return hr;
	}

	return hr;
}

HRESULT CMapDisplay::ReleaseDisplay()
{
    SAFE_DELETE(m_pSFMap);
	SAFE_DELETE(m_pSFBuf);

	for(int j = 0; j < kind_num; j++)
	{
		for(int i = 0; i < surface_max; i++)
			SAFE_DELETE(m_pSFObj[j][i]);
	}

	SAFE_DELETE(m_pSFSelect);
	SAFE_DELETE(m_pSFCircle);

	m_MapExtend.m_bExtendMode = false;

	return CDisplay::DestroyObjects();
}

bool CMapDisplay::OnDisplay(CMapData* pMap, WORD wLayerIndex)
{
	if(m_bDisplayMode)
		return false;

	m_bDisplayMode = true;

	HRESULT hr = InitSurface(pMap);
	if(hr != DD_OK)
		return false;

	m_pActMap = pMap;
	m_wLayerIndex = 0;

	if(wLayerIndex >= pMap->m_pMapSet->m_nLayerNum)
		m_wLayerIndex = 0;

	return true;
}

bool CMapDisplay::OffDisplay()
{
	if(!m_bDisplayMode)
		return false;

	HRESULT hRet = ReleaseDisplay();

	if(hRet != DD_OK)
		return false;

	memset(m_nDummyDrawNum, 0, sizeof(m_nDummyDrawNum));
	for(int m = 0; m < map_max; m++)
	{
		if(m_DummyDraw[m])
		{
			delete [] m_DummyDraw[m];
			m_DummyDraw[m] = NULL;
		}
	}

	m_bDisplayMode = false;

	m_pOldActMap = m_pActMap;
	m_wOldLayerIndex = m_wLayerIndex;
	m_pActMap = NULL;
	m_wLayerIndex = 0;

	return true;
}

bool CMapDisplay::ChangeMap(CMapData* pMap)
{
	if(m_bDisplayMode)
	{
		if(m_pActMap == pMap)
			return false;

		InitSurface(pMap);

		m_pActMap = pMap;
		m_wLayerIndex = 0;

		return true;
	}
	
	return false;
}

bool CMapDisplay::ChangeLayer(WORD wLayerIndex)
{
	if(m_bDisplayMode)
	{
		bool bChange = false;
		if(wLayerIndex < m_pActMap->m_pMapSet->m_nLayerNum)
		{
			if(m_wLayerIndex != wLayerIndex)
			{
				m_wLayerIndex = wLayerIndex;
				bChange = true;
			}
		}

		return true;
	}
	
	return false;
}

void CMapDisplay::DrawDisplay()
{
	if(!m_bDisplayMode)
		return;

	if(m_tmrDraw.CountingTimer())
	{
		CDisplay::UpdateBounds();

		DrawMap();
		DrawCollisionLine();
		DrawDummy();
		DrawObject();
		DrawText();

		m_MapExtend.DrawRect();

		CDisplay::Blt(0, 0, m_pSFMap, NULL);
		CDisplay::Present();
	}
}

CGameObject* CMapDisplay::SelectObject(CPoint* pt)//, CMapData* pActMap)
{
	for(int i = 0; i < CGameObject::s_nTotalObjectNum; i++)
	{
		CGameObject* p = CGameObject::s_pTotalObject[i];

		if(!p->m_bLive || p->m_pCurMap != m_pActMap || p->m_wMapLayerIndex != m_wLayerIndex)
			continue;

		if(pt->x >= p->m_nScreenPos[0] && pt->x <= p->m_nScreenPos[0]+6 &&          
			pt->y >= p->m_nScreenPos[1] && pt->y <= p->m_nScreenPos[1]+6) 
		{
			return p;
		}
	}
	return NULL;
}


void CMapDisplay::DrawMap()
{
	m_pSFMap->GetDDrawSurface()->BltFast(0, 0, m_pSFBuf->GetDDrawSurface(),
										NULL, DDBLTFAST_NOCOLORKEY);
}

void CMapDisplay::DrawObject()
{
	for(int i= 0; i < CGameObject::s_nTotalObjectNum; i++)
	{
		CGameObject* pObj = CGameObject::s_pTotalObject[i];

		if(!pObj->m_bLive || pObj->m_pCurMap != m_pActMap || pObj->m_wMapLayerIndex != m_wLayerIndex)
			continue;

		_object_id* pID = &pObj->m_ObjID;
		CSurface* pSF = m_pSFObj[pID->m_byKind][pID->m_byID];

		if(pObj->m_bCorpse)
			pSF = m_pSFCorpse;

		_DrawObject(pObj, pSF);
	}
	if(CGameObject::s_pSelectObject != NULL && CGameObject::s_pSelectObject->m_pCurMap == m_pActMap && CGameObject::s_pSelectObject->m_wMapLayerIndex == m_wLayerIndex)
	{
		if(m_bCirlcleLine)
		{
			float fTX = CGameObject::s_pSelectObject->m_fCurPos[0] - (MAP_STD_RADIUS+0.5) * MAP_STD_SEC_SIZE;
			float fTY = CGameObject::s_pSelectObject->m_fCurPos[2] - (MAP_STD_RADIUS+0.5) * MAP_STD_SEC_SIZE;
			float fBX = CGameObject::s_pSelectObject->m_fCurPos[0] + (MAP_STD_RADIUS+0.5) * MAP_STD_SEC_SIZE;
			float fBY = CGameObject::s_pSelectObject->m_fCurPos[2] + (MAP_STD_RADIUS+0.5) * MAP_STD_SEC_SIZE;

			for(int i= 0; i < CGameObject::s_nTotalObjectNum; i++)
			{
				CGameObject* pObj = CGameObject::s_pTotalObject[i];

				if(!pObj->m_bLive || pObj->m_pCurMap != m_pActMap || pObj->m_wMapLayerIndex != m_wLayerIndex)
					continue;

				if(pObj->m_fCurPos[0] >= fTX && pObj->m_fCurPos[0] <= fBX && pObj->m_fCurPos[2] >= fTY && pObj->m_fCurPos[2] <= fBY)
					_DrawObject(pObj, m_pSFCircle);
			}
		}
		_DrawObject(CGameObject::s_pSelectObject, m_pSFSelect);
	}
}

HRESULT CMapDisplay::CreateObjSurface()
{
	HRESULT hr = DD_OK;
	char* pszCharFile[OBJ_CHAR_ID_NUM] = {".\\Bitmap\\Player.bmp", ".\\Bitmap\\Monster.bmp", ".\\Bitmap\\NPC.bmp", ".\\Bitmap\\Animus.bmp", ".\\Bitmap\\Tower.bmp"};
	char* pszItemFile[OBJ_ITEM_ID_NUM] = {".\\Bitmap\\Item.bmp", ".\\Bitmap\\DungeonGate.bmp", ".\\Bitmap\\ParkingUnit.bmp"};
	int i;

	//케릭터로드..
	for(i = 0; i < OBJ_CHAR_ID_NUM; i++)
	{
		if(SUCCEEDED(hr = CDisplay::CreateSurfaceFromBitmap(&m_pSFObj[obj_kind_char][i], pszCharFile[i], 7, 7)))
			m_pSFObj[obj_kind_char][i]->SetColorKey(RGB(0, 0, 0));
		else
			goto RESULT;
	}

	//아이템로드
	for(i = 0; i < OBJ_ITEM_ID_NUM; i++)
	{
		if(SUCCEEDED(hr = CDisplay::CreateSurfaceFromBitmap(&m_pSFObj[obj_kind_item][i], pszItemFile[i], 7, 7)))
			m_pSFObj[obj_kind_item][i]->SetColorKey(RGB(0, 0, 0));
		else
			goto RESULT;
	}

	if(SUCCEEDED(hr = CDisplay::CreateSurfaceFromBitmap(&m_pSFSelect, ".\\Bitmap\\Select.bmp", 7, 7)))
		m_pSFSelect->SetColorKey(RGB(0, 0, 0));
	else
		goto RESULT;

	if(SUCCEEDED(hr = CDisplay::CreateSurfaceFromBitmap(&m_pSFCircle, ".\\Bitmap\\Circle.bmp", 7, 7)))
		m_pSFCircle->SetColorKey(RGB(0, 0, 0));
	else
		goto RESULT;

	if(SUCCEEDED(hr = CDisplay::CreateSurfaceFromBitmap(&m_pSFCorpse, ".\\Bitmap\\Corpse.bmp", 7, 7)))
		m_pSFCircle->SetColorKey(RGB(0, 0, 0));
	else
		goto RESULT;

RESULT:

	return hr;
}

HRESULT CMapDisplay::_DrawObject(CGameObject* pObj, CSurface* pSF)
{
	HRESULT hr = DD_OK;
	if(!pSF || !pObj)
		return FALSE;

	if(m_MapExtend.m_bExtendMode)
	{
		if(pObj->m_fAbsPos[0] > m_MapExtend.m_ptStartMap.x && 
			pObj->m_fAbsPos[0] < m_MapExtend.m_ptEndMap.x &&
			pObj->m_fAbsPos[2] > m_MapExtend.m_ptStartMap.y &&
			pObj->m_fAbsPos[2] < m_MapExtend.m_ptEndMap.y)
		{
			pObj->CalcScrExtendPoint(&m_rcWnd, &m_MapExtend.m_rcExtend);		

			hr = m_pSFMap->GetDDrawSurface()->BltFast(pObj->m_nScreenPos[0], pObj->m_nScreenPos[1], 
									pSF->GetDDrawSurface(),
									NULL, DDBLTFAST_SRCCOLORKEY);
		}
		else
		{
			pObj->m_nScreenPos[0] = -1;
			pObj->m_nScreenPos[1] = -1;
		}
	}
	else 
	{
		pObj->CalcScrNormalPoint(&m_rcWnd);

		hr = m_pSFMap->GetDDrawSurface()->BltFast(pObj->m_nScreenPos[0], pObj->m_nScreenPos[1], 
										pSF->GetDDrawSurface(),
										NULL, DDBLTFAST_SRCCOLORKEY);
	}

	return hr;
}

void CMapDisplay::DrawDummy()
{
	CRect rcWnd;
	CRect* prc = NULL;

	if(m_MapExtend.m_bExtendMode)
	{
		rcWnd = m_MapExtend.m_rcExtend;
		prc = &rcWnd;
	}

	CDummyDraw* pDraw;
	int nMapID = g_Main.m_MapOper.GetMap(m_pActMap);
	for(int i = 0; i < m_nDummyDrawNum[nMapID]; i++)
	{
		pDraw = &m_DummyDraw[nMapID][0];
		pDraw[i].Draw(m_pSFMap, prc);		
	}

	if(m_MapExtend.m_bExtendMode)
	{
		DDSURFACEDESC2* pddsd = m_pSFMap->GetDDSurfaceDesc();
		HDC hdc;

		HRESULT hRet = (m_pSFMap->GetDDrawSurface())->GetDC(&hdc);
		if(hRet == DD_OK)
		{
			::SelectObject(hdc, m_hPenBorder);

			MoveToEx(hdc, 0, 0, NULL);
			LineTo(hdc, pddsd->dwWidth, 0);
			LineTo(hdc, pddsd->dwWidth, pddsd->dwHeight);
			LineTo(hdc, 0, pddsd->dwHeight);
			LineTo(hdc, 0, 0);

			(m_pSFMap->GetDDrawSurface())->ReleaseDC(hdc);
		}
	}
}

void CMapDisplay::DrawCollisionLine()
{
	int nMapIndex = g_Main.m_MapOper.GetMap(m_pActMap);
	CRect* prc = NULL;

	if(m_MapExtend.m_bExtendMode)
	{		
		prc = &m_MapExtend.m_rcExtend;
	}
	m_CollLineDraw[nMapIndex].Draw(m_pSFMap, prc);
}

void CMapDisplay::DrawText()
{
    HDC hdc;
    HRESULT hRet = (m_pSFMap->GetDDrawSurface())->GetDC(&hdc);
    if(hRet == DD_OK )
    {
		SetBkMode(hdc, TRANSPARENT);
        SetTextAlign(hdc, TA_LEFT);
		SetTextColor(hdc, RGB(255, 255, 255));
		::SelectObject(hdc, m_Font);

		char szText[128];
		_map_fld* pSet = g_Main.m_MapDisplay.m_pActMap->m_pMapSet;
		
		sprintf(szText, "활성맵 : %s.. Layer: %d / %d", pSet->m_strCode, m_wLayerIndex+1, pSet->m_nLayerNum);
		TextOut(hdc, 0, 0, szText, strlen(szText));

		if(m_MapExtend.m_bExtendMode)
		{
			LONG* pPot = (LONG*)&(m_MapExtend.m_rcExtend);
			int* nMapMin = m_pActMap->GetBspInfo()->m_nMapMinSize;
			int* nMapMax = m_pActMap->GetBspInfo()->m_nMapMaxSize;

			SetTextColor(hdc, RGB(255, 255, 0));
	
			sprintf(szText, "MinX=%d, MinZ=%d, MaxX=%d, MaxZ=%d", 
				nMapMin[0]+pPot[0],  nMapMax[2]-pPot[3], nMapMin[0]+pPot[2], nMapMax[2]-pPot[1]);

			TextOut(hdc, 0, 12, szText, strlen(szText));
			
		}
        (m_pSFMap->GetDDrawSurface())->ReleaseDC(hdc);
    }
}
