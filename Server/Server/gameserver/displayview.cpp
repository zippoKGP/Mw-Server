// DisplayView.cpp : implementation file
//

#include "stdafx.h"
#include "ZoneServer.h"
#include "DisplayView.h"
#include "ZoneServerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

float g_fPoint[2];

/////////////////////////////////////////////////////////////////////////////
// CDisplayView dialog


CDisplayView::CDisplayView(CWnd* pParent /*=NULL*/)
	: CDialog(CDisplayView::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDisplayView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

}

void CDisplayView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDisplayView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDisplayView, CDialog)
	//{{AFX_MSG_MAP(CDisplayView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
//	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDisplayView message handlers

void CDisplayView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	SetCapture();
	SetFocus();

	if(g_Main.m_MapDisplay.m_bDisplayMode)
	{
		g_Main.m_GameMsg.PackingMsg(gm_interface_character_select);

		m_ptLBtn = point;

		CRect rcWnd;
		GetDrawableRect(&rcWnd);
		g_Main.m_MapDisplay.m_MapExtend.StartScreenPoint(&point, g_Main.m_MapDisplay.m_pActMap, &rcWnd);
	}
	
	CDialog::OnLButtonDown(nFlags, point);
}

void CDisplayView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	CMapExtend* pEx = &g_Main.m_MapDisplay.m_MapExtend;
	CMapData* pMap = g_Main.m_MapDisplay.m_pActMap;

	ReleaseCapture();

	if(g_Main.m_MapDisplay.m_bDisplayMode)
	{
		_bsp_info* pBspInfo = pMap->GetBspInfo();
		CSize szMap(pBspInfo->m_nMapSize[0], pBspInfo->m_nMapSize[2]);

		if(pEx->EndScreenPoint(&szMap))
		{
			m_nHorzPage = pEx->GetExtendSize()->cx;
			m_nHorzRange = pBspInfo->m_nMapSize[0]-m_nHorzPage;	
			m_nHorzPos	= (m_nHorzRange*pEx->m_ptCenter.x)/m_nHorzRange-m_nHorzPage/2;

			SetScrollRange(SB_HORZ, 0, m_nHorzRange);
			SetScrollPos(SB_HORZ, m_nHorzPos);

			m_nVerPage = pEx->GetExtendSize()->cy;
			m_nVerRange = pBspInfo->m_nMapSize[2]-m_nVerPage;	
			m_nVertPos	= (m_nVerRange*pEx->m_ptCenter.y)/m_nVerRange-m_nVerPage/2;

			SetScrollRange(SB_VERT, 0, m_nVerRange);
			SetScrollPos(SB_VERT, m_nVertPos);
		}
	}
	
	CDialog::OnLButtonUp(nFlags, point);
}

void CDisplayView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	if(g_Main.m_MapDisplay.m_bDisplayMode)
	{
		g_Main.m_MapDisplay.m_MapExtend.MoveScreenPoint(&point);
	}
		
	CDialog::OnMouseMove(nFlags, point);
}

void CDisplayView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	if(g_Main.m_MapDisplay.m_bDisplayMode)
	{
		g_Main.m_MapDisplay.m_MapExtend.m_bExtendMode = false;
//		SetScrollPos(SB_VERT, 0);
//		SetScrollPos(SB_HORZ, 0);

		SetExtendMode(FALSE);
	}

	CDialog::OnLButtonDblClk(nFlags, point);
}

void CDisplayView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

	CMapExtend* pEx = &g_Main.m_MapDisplay.m_MapExtend;
	if(!pEx)
		return;

	if(!pEx->m_bExtendMode)
		return;

	CMapData* pMap= g_Main.m_MapDisplay.m_pActMap;
	int nMapY = pMap->GetBspInfo()->m_nMapSize[2];

	switch(nSBCode)
	{
	case SB_PAGEDOWN:
		pEx->ScrollMapDown(nMapY, m_nVerPage/3*2);
		break;

	case SB_PAGEUP:
		pEx->ScrollMapUp(m_nVerPage/3*2);
		break;

	case SB_LINEDOWN:
		pEx->ScrollMapDown(nMapY, m_nVerPage/10);
		break;

	case SB_LINEUP:
		pEx->ScrollMapUp(m_nVerPage/10);
		break;

	case SB_THUMBTRACK:
		if((m_nVertPos-nPos) < 0)
			pEx->ScrollMapDown(nMapY, nPos-m_nVertPos);
		else if((m_nVertPos-nPos) > 0)
			pEx->ScrollMapUp(m_nVertPos-nPos);
		break;
	}	
	m_nVertPos = (m_nVerRange*pEx->m_ptCenter.y)/m_nVerRange-m_nVerPage/2;
	SetScrollPos(SB_VERT, m_nVertPos);

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CDisplayView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

	CMapExtend* pEx = &g_Main.m_MapDisplay.m_MapExtend;
	if(!pEx)
		return;

	if(!pEx->m_bExtendMode)
		return;

	CMapData* pMap= g_Main.m_MapDisplay.m_pActMap;
	int nMapX = pMap->GetBspInfo()->m_nMapSize[0];
	
	switch(nSBCode)
	{
	case SB_PAGEDOWN:
		pEx->ScrollMapRight(nMapX, m_nHorzPage/3*2);
		break;

	case SB_PAGEUP:
		pEx->ScrollMapLeft(m_nHorzPage/3*2);
		break;

	case SB_LINEDOWN:
		pEx->ScrollMapRight(nMapX, m_nHorzPage/10);
		break;

	case SB_LINEUP:
		pEx->ScrollMapLeft(m_nHorzPage/10);
		break;

	case SB_THUMBTRACK:
		if((m_nHorzPos-nPos) < 0)
			pEx->ScrollMapRight(nMapX, nPos-m_nHorzPos);
		else if((m_nHorzPos-nPos) > 0)
			pEx->ScrollMapLeft(m_nHorzPos-nPos);
		break;
	}
	
	m_nHorzPos = (m_nHorzRange*pEx->m_ptCenter.x)/m_nHorzRange-m_nHorzPage/2;
	SetScrollPos(SB_HORZ, m_nHorzPos);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CDisplayView::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

//	SetTimer(0, 1000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDisplayView::SetExtendMode(BOOL bExtend)
{
//	EnableScrollBarCtrl(SB_HORZ, bExtend);
//	EnableScrollBarCtrl(SB_VERT, bExtend);
	SetScrollPos(SB_VERT, 0);
	SetScrollPos(SB_HORZ, 0);
}

int CDisplayView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

void CDisplayView::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here

//	KillTimer(0);
	
}

BOOL CDisplayView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default

//	CMapExtend* pEx = &g_Main.m_MapDisplay.m_MapExtend;
//	if(!pEx)
//		return FALSE;

//	if(!pEx->m_bExtendMode)
//		return FALSE;
	
	if(g_Main.m_MapDisplay.m_bDisplayMode)
	{
		if(zDelta > 0)
		{
			g_Main.m_MapDisplay.ChangeLayer(g_Main.m_MapDisplay.m_wLayerIndex-1);
		}
		else
		{
			g_Main.m_MapDisplay.ChangeLayer(g_Main.m_MapDisplay.m_wLayerIndex+1);
		}
	}

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CDisplayView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	CMapDisplay* pDisp = &g_Main.m_MapDisplay;
	CRect rcWnd;
//	GetClientRect(rcWnd);
	GetDrawableRect(&rcWnd);


//	if(pDisp->m_MapExtend.m_bExtendMode)
//		g_Main.OffPlayerMove(&point, &rcWnd, &pDisp->m_MapExtend.m_rcExtend);
//	else
//		g_Main.OffPlayerMove(&point, &rcWnd);


	float fAbsPos[3];
	int cx = rcWnd.right;
	int cy = rcWnd.bottom;
	_bsp_info* pBspInfo = g_Main.m_MapDisplay.m_pActMap->GetBspInfo();

	if(!pDisp->m_MapExtend.m_bExtendMode)
	{
		fAbsPos[0] = (float)(point.x*pBspInfo->m_nMapSize[0])/cx;
		fAbsPos[2] = (float)(point.y*pBspInfo->m_nMapSize[2])/cy;
	}
	else
	{
		int nExtendXSize = pDisp->m_MapExtend.m_rcExtend.right-pDisp->m_MapExtend.m_rcExtend.left;
		int nExtendYSize = pDisp->m_MapExtend.m_rcExtend.top;

		fAbsPos[0] = (float)((point.x*nExtendXSize)/rcWnd.right)+pDisp->m_MapExtend.m_rcExtend.left;
		fAbsPos[2] = (float)((point.y*nExtendYSize)/rcWnd.bottom)+pDisp->m_MapExtend.m_rcExtend.top;
	}

	g_fPoint[0] = pBspInfo->m_nMapMinSize[0]+fAbsPos[0];
	g_fPoint[1] = pBspInfo->m_nMapMaxSize[2]-fAbsPos[2];

	CDialog::OnRButtonDown(nFlags, point);
}

void CDisplayView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
/*
	if(nChar == 65)//'a'
	{
		if(g_Main.m_MapDisplay.m_bDisplayMode)
		{
			CMapData* pMap =  g_Main.m_MapDisplay.m_pActMap;
			WORD wLayerIndex = g_Main.m_MapDisplay.m_wLayerIndex;

			if(pMap->m_pMapSet->m_nMapType == MAP_TYPE_DUNGEON)
			{
				if(pMap->m_ls[wLayerIndex].IsActiveLayer())
				{
					for(int i = 0; i < MAX_MONSTER; i++)
					{
						CMonster* pMon = &g_Monster[i];
						if(pMon->m_bLive && pMon->m_pCurMap == pMap && pMon->m_wMapLayerIndex == wLayerIndex)
							pMon->Destroy(mon_destroy_code_respawn, NULL);
					}
					pMap->m_ls[wLayerIndex].InertLayer();
				}
				else
					pMap->m_ls[wLayerIndex].ActiveLayer(&pMap->m_mb[0], dungeon_active_time);
			}
		}		
	}	
*/	
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CDisplayView::GetDrawableRect(CRect* rcOut)
{
	GetClientRect(rcOut);
}

void CDisplayView::OnTimer(UINT nIDEvent) 
{
	if(g_Main.m_MapDisplay.m_bDisplayMode)
		return;

	CClientDC dc(this); // device context for painting

	// TODO: Add your message handler code here and/or call default

	CDialog::OnTimer(nIDEvent);
}
