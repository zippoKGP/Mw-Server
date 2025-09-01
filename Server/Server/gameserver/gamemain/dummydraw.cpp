// DummyDraw.cpp: implementation of the CDummyDraw class.
//	
//////////////////////////////////////////////////////////////////////
	
#include "stdafx.h"
#include "DummyDraw.h"
#include "MapData.h"
#include "ZoneServerDoc.h"
	
HPEN CDummyDraw::s_hPen[max_dummy_type];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
	
CDummyDraw::CDummyDraw()
{	
	m_pMap = NULL;
}	
	
CDummyDraw::~CDummyDraw()
{	
	
}	
	
void CDummyDraw::SetDummyPoint(CMapData* pMap, float* pCenterPos, int nType, CRect* prcWnd)
{	
	m_pMap = pMap;
	m_nType = nType;
	
	_bsp_info* pBspInfo = pMap->GetBspInfo();

	float fLeft = -(float)pBspInfo->m_nMapMinSize[0] + (pCenterPos[0] - 80);//   fMinBuf[0];
	float fTop = (float)pBspInfo->m_nMapMaxSize[2] - (pCenterPos[2] + 80);//fMaxBuf[2];
	float fRight = -(float)pBspInfo->m_nMapMinSize[0] + (pCenterPos[0] + 80);//fMaxBuf[0];
	float fBottom = (float)pBspInfo->m_nMapMaxSize[2] - (pCenterPos[2] - 80);//fMinBuf[2];

	m_fPosAbs[0*2+0] = fLeft;
	m_fPosAbs[0*2+1] = fTop;
	m_fPosAbs[1*2+0] = fRight;
	m_fPosAbs[1*2+1] = fTop;
	m_fPosAbs[2*2+0] = fLeft;
	m_fPosAbs[2*2+1] = fBottom;
	m_fPosAbs[3*2+0] = fRight;
	m_fPosAbs[3*2+1] = fBottom;
	
	for(int i = 0; i < 4; i++)
	{
		m_fScrNor[i*2+0] = (m_fPosAbs[i*2+0]*prcWnd->right)/pBspInfo->m_nMapSize[0];
		m_fScrNor[i*2+1] = (m_fPosAbs[i*2+1]*prcWnd->bottom)/pBspInfo->m_nMapSize[2];
	}
}	

void CDummyDraw::SetDummyRange(CMapData* pMap, float* pLT, float* pRB, float* pRT, float* pLB, int nType, CRect* prcWnd)
{
	m_pMap = pMap;
	m_nType = nType;
	
	_bsp_info* pBspInfo = pMap->GetBspInfo();

	m_fPosAbs[0*2+0] = -(float)pBspInfo->m_nMapMinSize[0] + pLT[0];
	m_fPosAbs[0*2+1] = (float)pBspInfo->m_nMapMaxSize[2] - pLT[2];

	m_fPosAbs[1*2+0] = -(float)pBspInfo->m_nMapMinSize[0] + pRT[0];
	m_fPosAbs[1*2+1] = (float)pBspInfo->m_nMapMaxSize[2] - pRT[2];

	m_fPosAbs[2*2+0] = -(float)pBspInfo->m_nMapMinSize[0] + pLB[0];
	m_fPosAbs[2*2+1] = (float)pBspInfo->m_nMapMaxSize[2] - pLB[2];

	m_fPosAbs[3*2+0] = -(float)pBspInfo->m_nMapMinSize[0] + pRB[0];
	m_fPosAbs[3*2+1] = (float)pBspInfo->m_nMapMaxSize[2] - pRB[2];
	
	for(int i = 0; i < 4; i++)
	{
		m_fScrNor[i*2+0] = (m_fPosAbs[i*2+0]*prcWnd->right)/pBspInfo->m_nMapSize[0];
		m_fScrNor[i*2+1] = (m_fPosAbs[i*2+1]*prcWnd->bottom)/pBspInfo->m_nMapSize[2];
	}
}
	
bool CDummyDraw::CalcAbsExt(CRect* prcArea)
{	
	int i;
	
	bool bPass[4];

	memcpy(m_fReAbs, m_fPosAbs, sizeof(float)*8);
	
	for(i = 0; i < 4; i++)
	{
		if(m_fPosAbs[i*2+0] < prcArea->left)
		{
			m_fReAbs[i*2+0] = (float)prcArea->left;
			bPass[i] = false;
		}
		if(m_fPosAbs[i*2+0] > prcArea->right)
		{
			m_fReAbs[i*2+0] = (float)prcArea->right;
			bPass[i] = false;
		}
		if(m_fPosAbs[i*2+1] < prcArea->top)
		{
			m_fReAbs[i*2+1] = (float)prcArea->top;
			bPass[i] = false;
		}
		if(m_fPosAbs[i*2+1] > prcArea->bottom)
		{
			m_fReAbs[i*2+1] = (float)prcArea->bottom;
			bPass[i] = false;
		}
	}
	
	for(i = 0; i < 4; i++)
	{
		if(bPass[i])
			return true;
	}
	
	return false;
}	

void CDummyDraw::CalcScrExt(CRect* prcArea, CRect* prcWnd)
{
	float fExtendPos[2];

	for(int i = 0; i < 4; i++)
	{
		fExtendPos[0] = m_fReAbs[i*2+0]-prcArea->left;
		fExtendPos[1] = m_fReAbs[i*2+1]-prcArea->top;

		m_fScrExt[i*2+0] = (fExtendPos[0]*prcWnd->right)/(prcArea->right-prcArea->left);
		m_fScrExt[i*2+1] = (fExtendPos[1]*prcWnd->bottom)/(prcArea->bottom-prcArea->top);
	}
}

HRESULT CDummyDraw::Draw(CSurface* pSF, CRect* prcArea/* = NULL*/)
{
	if(!m_pMap)
		return -1;

	HDC hdc;
	float* pScrPos = m_fScrNor;

	if(prcArea)
	{
		if(!CalcAbsExt(prcArea))
			return -2;
		else
		{
			CRect rc;
//			g_pDoc->m_DisplayView.GetClientRect(&rc);
			g_pDoc->m_DisplayView.GetDrawableRect(&rc);

			LONG* pl = (LONG*)&rc;
			for(int i = 0; i < 4; i++)
			{
				if(*pl < 0)
					return -1;
				else if(*pl > 10000)
					return -1;
				pl++;
			}		

			CalcScrExt(prcArea, &rc);
			pScrPos = m_fScrExt;
		}
	}

	HRESULT hRet = (pSF->GetDDrawSurface())->GetDC(&hdc);
	if(hRet == DD_OK)
	{
		SelectObject(hdc, s_hPen[m_nType]);

		if(m_nType == type_fail_dummy)
			m_nType = type_fail_dummy;


		MoveToEx(hdc, (int)pScrPos[0*2+0], (int)pScrPos[0*2+1], NULL);
		LineTo(hdc, (int)pScrPos[1*2+0], (int)pScrPos[1*2+1]);
		LineTo(hdc, (int)pScrPos[3*2+0], (int)pScrPos[3*2+1]);
		LineTo(hdc, (int)pScrPos[2*2+0], (int)pScrPos[2*2+1]);
		LineTo(hdc, (int)pScrPos[0*2+0], (int)pScrPos[0*2+1]);

		(pSF->GetDDrawSurface())->ReleaseDC(hdc);
	}
	
	return hRet;	
}

void CDummyDraw::InitPen()
{
	HPEN hBuf;

	DWORD dwRGB[max_dummy_type] = {RGB(242, 74, 120), RGB(255, 255, 255), RGB(102, 239, 78), RGB(242, 174, 20), RGB(96, 74, 242), RGB(5, 5, 5)};

	for(int i = 0; i < max_dummy_type; i++)
	{
		hBuf = CreatePen(PS_SOLID, 1, dwRGB[i]);
		if(hBuf != NULL)
			s_hPen[i] = hBuf;
	}
}

void CDummyDraw::DeletePen()
{
	for(int i = 0; i < max_dummy_type; i++)
	{
		DeleteObject(s_hPen[i]);
	}
}

