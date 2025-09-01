// MapExtend.h: interface for the CMapExtend class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _MapExtend_h_
#define _MapExtend_h_

class CSurface;
class CMapData;

struct _map_rate
{
	float fSide[2];
	int nStandard;
	int nPartner;

	void Setting(int x, int y) 	
	{
		if(x >= y)
		{
			fSide[0] = 1;
			fSide[1] = (float)y/(float)x;
			nStandard = 0;
			nPartner = 1;
		}
		else
		{
			fSide[1] = 1;
			fSide[0] = (float)x/(float)y;
			nStandard = 1;
			nPartner = 0;
		}
	}
};

class CMapExtend  
{
public:
	CPoint m_ptStartMap;
	CPoint m_ptEndMap;
	CPoint m_ptCenter;
						
	bool  m_bExtendMode;
	CRect m_rcExtend;

private:
	CPoint m_ptStartScreen;
	CPoint m_ptEndScreen;
	CPoint m_ptMoveScreen;

	CSize m_sizeExtend;

	HPEN  m_hPen;
	CSurface** m_pSF;
	_map_rate m_Rate;
		
	BOOL m_bSetArea;
	BOOL m_bMove;

public:
	CMapExtend();
	CMapExtend(CSurface** pSF);
	~CMapExtend();

	void Init(CSurface** pSF);

	void StartScreenPoint(CPoint* pt, CMapData* pMap, CRect* rcWnd);	
	BOOL EndScreenPoint(CSize* szMap);
	void MoveScreenPoint(CPoint* pt);

	void DrawRect();

	void ConvertToMap(CSize* szMap);
	CSize* GetExtendSize() { return &m_sizeExtend; }

	void ScrollMapUp(int nInterval);
	void ScrollMapDown(int nMapY, int nInterval);
	void ScrollMapLeft(int nInterval);
	void ScrollMapRight(int nMapX, int nInterval);
};

#endif 
