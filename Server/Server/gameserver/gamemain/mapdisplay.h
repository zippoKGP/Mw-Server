// MapDisplay.h: interface for the CMapDisplay class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _MapDisplay_h_
#define _MapDisplay_h_

#include "MapData.h"
#include "GameObject.h"
#include "Character.h"
#include "MapExtend.h"
#include "DummyDraw.h"
#include "ddutil.h"
#include "MyClassUtil.h"
#include "CollLineDraw.h"

class CMapDisplay : public CDisplay
{
public:
	enum {	kind_num = obj_kind_num	};
	enum {  surface_max = 10	};
	enum {	map_max = 10 };
	enum {	timer_draw = 100	};
	enum {  max_dummy_num = 2000 };
	
private:

	CSurface* m_pSFMap;
	CSurface* m_pSFObj[kind_num][surface_max];
	CSurface* m_pSFCorpse;
	CSurface* m_pSFSelect;
	CSurface* m_pSFCircle;
	CSurface* m_pSFBuf;

	int			m_nDummyDrawNum[map_max];
	CDummyDraw*	m_DummyDraw[map_max];//[max_dummy_num];
	CCollLineDraw m_CollLineDraw[map_max];

	CRect m_rcWnd;
	
	CMyTimer m_tmrDraw;
	CFont m_Font;
	HPEN m_hPenBorder;

public:
	bool		m_bDisplayMode;
	bool		m_bCirlcleLine;
	CMapData*	m_pActMap;
	WORD		m_wLayerIndex;
	CMapData*	m_pOldActMap;
	WORD		m_wOldLayerIndex;
	CMapExtend	m_MapExtend;

public:
	CMapDisplay();
	virtual ~CMapDisplay();
	
	void InitDummy(CRect* prcWnd);
	void InitCollLine(CRect* prcWnd);
	HRESULT ReleaseDisplay();
//	void ReleaseDummy(int nMapIndex = -1);

	bool OnDisplay(CMapData* pMap, WORD wLayerIndex);
	bool OffDisplay();	

	bool ChangeMap(CMapData* pMap);
	bool ChangeLayer(WORD wLayerIndex);

	HRESULT InitSurface(CMapData* pMap);
	void DrawDisplay();

	CGameObject* SelectObject(CPoint* pt);//, CMapData* pActMap);

private:
	HRESULT CreateObjSurface();

	void DrawMap();
	void DrawObject();
	void DrawDummy();
	void DrawCollisionLine();
	void DrawText();

	HRESULT _DrawObject(CGameObject* pObj, CSurface* pSF);
};


#endif 
