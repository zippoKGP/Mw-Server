// CollLineDraw.h: interface for the CCollLineDraw class.
//
//////////////////////////////////////////////////////////////////////

#ifndef	_CollLineDraw_h_
#define _CollLineDraw_h_

#include "MapData.h"

class CSurface;

struct _coll_point
{
	//초기에 한번..load
	float m_fPosAbs[2];
	float m_fScrNor[2];

	//실시간으로..
	float m_fReAbs[2];
	float m_fScrExt[2];

	void InitPoint(CMapData* pMap, float* pPos, CRect* prcWnd)
	{
		_bsp_info* pBspInfo = pMap->GetBspInfo();
		
		m_fPosAbs[0] = -(float)pBspInfo->m_nMapMinSize[0]+pPos[0];
		m_fPosAbs[1] = (float)pBspInfo->m_nMapMaxSize[2]-pPos[2];
		
		m_fScrNor[0] = (m_fPosAbs[0]*prcWnd->right)/pBspInfo->m_nMapSize[0];
		m_fScrNor[1] = (m_fPosAbs[1]*prcWnd->bottom)/pBspInfo->m_nMapSize[2];
	}
};

class CCollLineDraw  
{
private:	
	enum { max_coll_point = 15000 };

	CMapData* m_pMap;
	CRect m_rcWnd;

	int m_nPointNum;
	_coll_point* m_Point;//[max_coll_point];

	int m_nLineNum;
	_TOOL_COL_LINE* m_pLine;

	static HPEN s_hPen;

public:
	CCollLineDraw();
	virtual ~CCollLineDraw();

	bool InitLine(CMapData* pMap, CRect* prcWnd);
	bool Draw(CSurface* pSF, CRect* prcArea = NULL);

	static void InitPen();
	static void DeletePen();

private:
	bool DrawEx(int nLineIndex, HDC* pDC, CRect* prcArea);
	void CalcScrExt(CRect* prcArea, CRect* prcWnd);
};

#endif 
