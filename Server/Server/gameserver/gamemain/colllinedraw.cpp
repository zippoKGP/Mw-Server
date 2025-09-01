// CollLineDraw.cpp: implementation of the CCollLineDraw class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ZoneServer.h"
#include "CollLineDraw.h"
#include "ddutil.h"

HPEN CCollLineDraw::s_hPen;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCollLineDraw::CCollLineDraw()
{
	m_pMap = NULL;
	m_nPointNum = 0;
	m_Point = NULL;
	m_nLineNum = 0;
	m_pLine = NULL;
}

CCollLineDraw::~CCollLineDraw()
{
	if(m_Point)
		delete [] m_Point;
}

bool CCollLineDraw::InitLine(CMapData* pMap, CRect* prcWnd)
{
	if(m_pMap)
		return false;

	m_pMap = pMap;
	m_nPointNum = pMap->m_Level.mBsp->mCFVertexNum;
	if(m_nPointNum > max_coll_point)
	{
		::MyMessageBox("CCollLineDraw::InitLine", "%d멥에 충돌포인트[%d]가 %d보다 많다..", pMap->m_pMapSet->m_strCode, m_nPointNum, max_coll_point);
		return false;
	}

	if(!m_Point)
		m_Point = new _coll_point [m_nPointNum];

	m_rcWnd = *prcWnd;

	for(int i = 0; i < m_nPointNum; i++)
	{
		float* pPos = pMap->m_Level.mBsp->mCFVertex[i];
		m_Point[i].InitPoint(pMap, pPos, &m_rcWnd);
	}

	m_nLineNum = pMap->m_Level.mBsp->mCFLineNum;
	m_pLine = pMap->m_Level.mBsp->mCFLine;

	return true;
}

bool CCollLineDraw::Draw(CSurface* pSF, CRect* prcArea/* = NULL*/)
{
	if(!m_pMap)
		return false;

	HDC hdc;

	HRESULT hRet = (pSF->GetDDrawSurface())->GetDC(&hdc);
	if(hRet == DD_OK)
	{
		SelectObject(hdc, s_hPen);

		for(int i = 0; i < m_nLineNum; i++)
		{
			if(!prcArea)
			{
				float* pfStart = (float*)m_Point[m_pLine[i].start_v].m_fScrNor;
				float* pfEnd = (float*)m_Point[m_pLine[i].end_v].m_fScrNor;

				MoveToEx(hdc, (int)pfStart[0], (int)pfStart[1], NULL);
				LineTo(hdc, (int)pfEnd[0], (int)pfEnd[1]);
			}
			else
			{
				DrawEx(i, &hdc, prcArea);
			}
		}

		(pSF->GetDDrawSurface())->ReleaseDC(hdc);
		return true;
	}

	return false;	
}

bool CCollLineDraw::DrawEx(int nLineIndex, HDC* pDC, CRect* prcArea)
{
	int nStaPot = m_pLine[nLineIndex].start_v;
	int nEndPot = m_pLine[nLineIndex].end_v;

	float* pfAbsStart = m_Point[nStaPot].m_fPosAbs;
	float* pfAbsEnd = m_Point[nEndPot].m_fPosAbs;

	float fExStaPosX = m_Point[nStaPot].m_fPosAbs[0]-prcArea->left;
	float fExStaPosY = m_Point[nStaPot].m_fPosAbs[1]-prcArea->top;
	float fExEndPosX = m_Point[nEndPot].m_fPosAbs[0]-prcArea->left;
	float fExEndPosY = m_Point[nEndPot].m_fPosAbs[1]-prcArea->top;

	m_Point[nStaPot].m_fScrExt[0] = (fExStaPosX*m_rcWnd.right)/(prcArea->right-prcArea->left);
	m_Point[nStaPot].m_fScrExt[1] = (fExStaPosY*m_rcWnd.bottom)/(prcArea->bottom-prcArea->top);
	m_Point[nEndPot].m_fScrExt[0] = (fExEndPosX*m_rcWnd.right)/(prcArea->right-prcArea->left);
	m_Point[nEndPot].m_fScrExt[1] = (fExEndPosY*m_rcWnd.bottom)/(prcArea->bottom-prcArea->top);

	MoveToEx(*pDC, (int)m_Point[nStaPot].m_fScrExt[0], (int)m_Point[nStaPot].m_fScrExt[1], NULL);
	LineTo(*pDC, (int)m_Point[nEndPot].m_fScrExt[0], (int)m_Point[nEndPot].m_fScrExt[1]);
	
	return true;
}

void CCollLineDraw::InitPen()
{
	DWORD dwRGB = RGB(120, 120, 120);

	HPEN hBuf = CreatePen(PS_SOLID, 1, dwRGB);
	if(hBuf != NULL)
		s_hPen = hBuf;
}

void CCollLineDraw::DeletePen()
{
	DeleteObject(s_hPen);
}