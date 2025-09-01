// MapExtend.cpp: implementation of the CMapExtend class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MapExtend.h"
#include "MapData.h"
#include "ddutil.h"
#include "ZoneServerDoc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapExtend::CMapExtend()
{
	m_bSetArea = FALSE;
	m_bExtendMode = FALSE;
	m_hPen = CreatePen(PS_SOLID, 1, RGB(100, 100, 100));
}	

CMapExtend::CMapExtend(CSurface** pSF)
{
	m_bSetArea = FALSE;
	m_bExtendMode = FALSE;
	m_hPen = CreatePen(PS_SOLID, 1, RGB(100, 100, 100));

	m_pSF = pSF;
}	

CMapExtend::~CMapExtend()
{
	DeleteObject(m_hPen);
}

void CMapExtend::Init(CSurface** pSF)
{
	m_pSF = pSF;
}

void CMapExtend::StartScreenPoint(CPoint* pt, CMapData* pMap, CRect* rcWnd)
{
	if(m_bExtendMode)
		return;

	m_bSetArea = TRUE;
	m_bMove = FALSE;

	m_ptStartScreen = *pt;
	m_ptMoveScreen = *pt;

	_bsp_info* pBspInfo = pMap->GetBspInfo();

	m_Rate.Setting(rcWnd->right, rcWnd->bottom); 
}

void CMapExtend::MoveScreenPoint(CPoint* pt)
{	
	if(!m_bSetArea)
		return;
	
	if(m_ptStartScreen.x == pt->x && m_ptStartScreen.y == pt->y)
		return;
	
	m_bMove = TRUE;
		
	int* npt = (int*)pt;
	int* nStart = (int*)&m_ptStartScreen;	
	int* nMove = (int*)&m_ptMoveScreen;
	int nAbs[2];
	
	//기준변(긴쪽) 길이 계산..
	nMove[m_Rate.nStandard] = npt[m_Rate.nStandard];

	nAbs[m_Rate.nStandard] = nStart[m_Rate.nStandard]-npt[m_Rate.nStandard];
	if(nAbs[m_Rate.nStandard] < 0)
		nAbs[m_Rate.nStandard] = -nAbs[m_Rate.nStandard];

	if(nAbs[m_Rate.nStandard] > 200)
		int aa = 0;
	
	//상대변(짧은 쪽) 길이 계산..
	nAbs[m_Rate.nPartner] = int(nAbs[m_Rate.nStandard]*m_Rate.fSide[m_Rate.nPartner]);
	
	if(nStart[m_Rate.nPartner] >= npt[m_Rate.nPartner])
		nMove[m_Rate.nPartner] = nStart[m_Rate.nPartner]-nAbs[m_Rate.nPartner];
	else 
		nMove[m_Rate.nPartner] = nStart[m_Rate.nPartner]+nAbs[m_Rate.nPartner];

}		

BOOL CMapExtend::EndScreenPoint(CSize* szMap)
{
	if(!m_bSetArea)
		return m_bExtendMode;

	if(!m_bMove)
	{
		m_bSetArea = FALSE;
		return m_bExtendMode;
	}
	CPoint* pt = &m_ptMoveScreen;

	CPoint ptStart = m_ptStartScreen;
	CPoint ptLength = *pt - ptStart;

	if(ptLength.x < 0 && ptLength.y < 0)
	{
		m_ptStartScreen.x = pt->x;
		m_ptStartScreen.y = pt->y;
		m_ptEndScreen.x = ptStart.x;
		m_ptEndScreen.y = ptStart.y;
	}
	else if(ptLength.x > 0 && ptLength.y < 0)
	{
		m_ptStartScreen.y = pt->y;
		m_ptEndScreen.x = pt->x;
		m_ptEndScreen.y = ptStart.y;
	}
	else if(ptLength.x < 0 && ptLength.y > 0)
	{
		m_ptStartScreen.x = pt->x;
		m_ptEndScreen.x = ptStart.x;
		m_ptEndScreen.y = pt->y;
	}
	else if(ptLength.x > 0 && ptLength.y > 0)
	{
		m_ptEndScreen.x = pt->x;
		m_ptEndScreen.y = pt->y;
	}
	else
	{
		m_bSetArea = FALSE;
		return m_bExtendMode;
	}

	if(ptLength.x < 0)
		ptLength.x = -ptLength.x;
	if(ptLength.y < 0)
		ptLength.y = -ptLength.y;

	if(ptLength.x < 10 && ptLength.y < 10)
	{
		m_bSetArea = FALSE;
		return m_bExtendMode;
	}

	ConvertToMap(szMap);

	m_bSetArea = FALSE;
	m_bExtendMode = TRUE;

	m_rcExtend.SetRect(m_ptStartMap, m_ptEndMap);

	return m_bExtendMode;
}

void CMapExtend::DrawRect()
{
	if(!m_bSetArea)
		return;

	HDC hdc;

	HRESULT hRet = ((*m_pSF)->GetDDrawSurface())->GetDC(&hdc);
	if(hRet == DD_OK)
	{
		SelectObject(hdc, m_hPen);

		MoveToEx(hdc, m_ptStartScreen.x, m_ptStartScreen.y, NULL);
		LineTo(hdc, m_ptMoveScreen.x, m_ptStartScreen.y);
		LineTo(hdc, m_ptMoveScreen.x, m_ptMoveScreen.y);
		LineTo(hdc, m_ptStartScreen.x, m_ptMoveScreen.y);
		LineTo(hdc, m_ptStartScreen.x, m_ptStartScreen.y);

		((*m_pSF)->GetDDrawSurface())->ReleaseDC(hdc);
	}
}

void CMapExtend::ConvertToMap(CSize* szMap)
{
	int cx = (*m_pSF)->GetDDSurfaceDesc()->dwWidth;
	int cy = (*m_pSF)->GetDDSurfaceDesc()->dwHeight;

	m_ptStartMap.x = (szMap->cx*m_ptStartScreen.x)/cx;
	m_ptStartMap.y = (szMap->cy*m_ptStartScreen.y)/cy;
	m_ptEndMap.x = (szMap->cx*m_ptEndScreen.x)/cx;
	m_ptEndMap.y = (szMap->cy*m_ptEndScreen.y)/cy;

	m_sizeExtend.cx = m_ptEndMap.x-m_ptStartMap.x;
	m_sizeExtend.cy = m_ptEndMap.y-m_ptStartMap.y;		

	m_ptCenter.x = m_ptStartMap.x+m_sizeExtend.cx/2;
	m_ptCenter.y = m_ptStartMap.y+m_sizeExtend.cy/2;
}

void CMapExtend::ScrollMapUp(int nInterval)
{
	if(!m_bExtendMode)
		return;

	if(m_ptStartMap.y-nInterval < 0)
	{
		m_ptStartMap.y = 0;
		m_ptEndMap.y = m_sizeExtend.cy;
		m_ptCenter.y = m_sizeExtend.cx/2;
	}
	else
	{
		m_ptStartMap.y -= nInterval;
		m_ptEndMap.y -= nInterval;
		m_ptCenter.y -= nInterval;
	}

	m_rcExtend.SetRect(m_ptStartMap, m_ptEndMap);
}

void CMapExtend::ScrollMapDown(int nMapY, int nInterval)
{
	if(!m_bExtendMode)
		return;

	if(m_ptEndMap.y+nInterval > nMapY)
	{
		m_ptEndMap.y = (long)nMapY;
		m_ptStartMap.y = m_ptEndMap.y-m_sizeExtend.cy;
		m_ptCenter.y = m_ptEndMap.y-m_sizeExtend.cy/2;
	}
	else
	{
		m_ptStartMap.y += nInterval;
		m_ptEndMap.y += nInterval;
		m_ptCenter.y += nInterval;
	}

	m_rcExtend.SetRect(m_ptStartMap, m_ptEndMap);
}

void CMapExtend::ScrollMapLeft(int nInterval)
{
	if(!m_bExtendMode)
		return;

	if(m_ptStartMap.x-nInterval < 0)
	{
		m_ptStartMap.x = 0;
		m_ptEndMap.x = m_sizeExtend.cx;
		m_ptCenter.x = m_sizeExtend.cx/2;
	}
	else
	{
		m_ptStartMap.x -= nInterval;
		m_ptEndMap.x -= nInterval;
		m_ptCenter.x -= nInterval;
	}

	m_rcExtend.SetRect(m_ptStartMap, m_ptEndMap);
}

void CMapExtend::ScrollMapRight(int nMapX, int nInterval)
{
	if(!m_bExtendMode)
		return;

	if(m_ptEndMap.x+nInterval > nMapX)
	{
		m_ptEndMap.x = (long)nMapX;
		m_ptStartMap.x = m_ptEndMap.x-m_sizeExtend.cx;
		m_ptCenter.x = m_ptEndMap.x-m_sizeExtend.cx/2;
	}
	else
	{
		m_ptStartMap.x += nInterval;
		m_ptEndMap.x += nInterval;
		m_ptCenter.x += nInterval;
	}

	m_rcExtend.SetRect(m_ptStartMap, m_ptEndMap);
}


