// UGDraw.h: interface for the CUGDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UGDRAW_H__90EE6762_9F54_41D0_A886_B75260FF1BAF__INCLUDED_)
#define AFX_UGDRAW_H__90EE6762_9F54_41D0_A886_B75260FF1BAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CUGDraw  
{
public:
	void ShowText(RECT& rect,LPCWSTR lpWstr);
	void SetWordColor(UHINT color);
	void getStrSize(wstring wstr,SIZE& size);
	void drawLine(RECT* pRect,BMP *screen_buf,UINT16 color);

	CUGDraw();
	virtual ~CUGDraw();
	
private:
	void fnV16SwapInt( SLONG *i1, SLONG *i2 );
	void BUF_draw_line(SLONG x1,SLONG y1,SLONG x2,SLONG y2,UHINT color,UHINT *bitmap,SLONG bit_xl,SLONG bit_yl);
		
};

CUGDraw* getUGDraw();
#endif // !defined(AFX_UGDRAW_H__90EE6762_9F54_41D0_A886_B75260FF1BAF__INCLUDED_)
