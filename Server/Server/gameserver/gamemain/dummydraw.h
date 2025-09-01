// DummyDraw.h: interface for the CDummyDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DUMMYDRAW_H__2E06CFA7_6E30_4CDA_85A2_592278A5F061__INCLUDED_)
#define AFX_DUMMYDRAW_H__2E06CFA7_6E30_4CDA_85A2_592278A5F061__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSurface;
class CMapData;

#define max_dummy_type 6

class CDummyDraw  
{
public:
	enum { type_mon_dummy = 0,
			type_potal_dummy = 1,
			type_item_store_dummy = 2, 
			type_start_dummy = 3,
			type_res_dummy = 4,
			type_fail_dummy = 5	};

private:	
	CMapData* m_pMap;
	int m_nType;

	float m_fPosAbs[4*2];
	float m_fReAbs[4*2];
	float m_fScrNor[4*2];
	float m_fScrExt[4*2];

	static HPEN s_hPen[max_dummy_type];
	
public:
	CDummyDraw();
	virtual ~CDummyDraw();

	void SetDummyPoint(CMapData* pMap, float* pCenterPos, int nType, CRect* prcWnd);	
	void SetDummyRange(CMapData* pMap, float* pLT, float* pRB, float* pRT, float* pLB, int nType, CRect* prcWnd);	
	HRESULT Draw(CSurface* pSF, CRect* prcArea = NULL);

	static void InitPen();
	static void DeletePen();

private:
	bool CalcAbsExt(CRect* prcArea);
	void CalcScrExt(CRect* prcArea, CRect* prcWnd); 
};

#endif // !defined(AFX_DUMMYDRAW_H__2E06CFA7_6E30_4CDA_85A2_592278A5F061__INCLUDED_)
