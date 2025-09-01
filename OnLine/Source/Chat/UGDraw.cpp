// UGDraw.cpp: implementation of the CUGDraw class.
//
//////////////////////////////////////////////////////////////////////
#include "ChatDef.h"
#include "UGDraw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static CUGDraw  g_sUGDraw;
CUGDraw* getUGDraw(){return &g_sUGDraw;}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUGDraw::CUGDraw()
{

}

CUGDraw::~CUGDraw()
{

}


void CUGDraw::drawLine(RECT* pRect,BMP *screen_buf,UINT16 color)
{
	//D3_draw_line(pRect->left,pRect->top,0,pRect->right,pRect->bottom,0,screen_buf);
	if (getChatData()->m_bOuter)
	{
		MoveToEx(getChatData()->m_hDC,pRect->left,pRect->top,NULL);
		LineTo(getChatData()->m_hDC,pRect->right,pRect->bottom);
	}
	else
	{
		BUF_draw_line(pRect->left,pRect->top,pRect->right,pRect->bottom,color,(*screen_buf->line),SCREEN_WIDTH,SCREEN_HEIGHT);
	}
}

void CUGDraw::getStrSize(wstring wstr, SIZE &size)
{
	UINT  nLen=0;

	if (getChatData()->m_bOuter)
	{
		char szChat[256]={0};
		WideCharToMultiByte(CP_ACP,0,wstr.data(),-1,szChat,
			sizeof(szChat),NULL,NULL);
		
		memset(&size,0,sizeof(SIZE));
		GetTextExtentExPoint(getChatData()->m_hDC,szChat,
			strlen(szChat),0,NULL,NULL,&size);
		
		size.cy=size.cy+3;
	}
	else
	{
		if (wstr.size()!=0)
		{
			for (int i=0;i<wstr.size();i++)
			{
				if ((int)wstr.at(i)>128)
				{
					nLen=nLen+16;//get_word_width();
				}
				else
				{
					nLen=nLen+8;//getAscWidth();
				}
			}
		}

		size.cx=nLen;
		size.cy=19;
	}
}

void CUGDraw::SetWordColor(UHINT color)
{
	if (getChatData()->m_bOuter)
	{
		if (getChatData()->m_hPen)
		{
			DeleteObject(getChatData()->m_hPen);
			getChatData()->m_hPen=NULL;
		}

		if (getChatData()->m_hFont)
		{
			DeleteObject(getChatData()->m_hFont);
			getChatData()->m_hFont=NULL;
		}
		

		DWORD     tmp=color;
		byte     n1=(tmp>>8)&0xff;
		byte     n2=(tmp>>3)&0xff;
		byte     n3=(tmp<<3)&0xff;

		COLORREF  dwColor= RGB(n1,n2,n3);//;0x000000FF;
		getChatData()->m_hPen=CreatePen(PS_SOLID,1,dwColor);
		::SelectObject(getChatData()->m_hDC,getChatData()->m_hPen);

		::SetTextColor(getChatData()->m_hDC,dwColor);

		
	}
	else
	{
		set_word_color(color);
	}
}

void CUGDraw::BUF_draw_line(SLONG x1,SLONG y1,SLONG x2,SLONG y2,UHINT color,UHINT *bitmap,SLONG bit_xl,SLONG bit_yl)
{
	SLONG d, dx, dy;
	SLONG aincr, bincr;
	SLONG xincr, yincr;
	SLONG x,y;
	
	if( abs(x2-x1) < abs(y2-y1) )            // X- or Y-axis overflow?
	{                                      // Check Y-axis
		if ( y1 > y2 )                         // y1 > y2?
		{
			fnV16SwapInt( &x1, &x2 );           // Yes --> Swap X1 with X2
			fnV16SwapInt( &y1, &y2 );           //          and Y1 with Y2
		}
		xincr = ( x2 > x1 ) ?  1 : -1;         // Set X-axis increment
		dy = y2 - y1;
		dx = abs( x2-x1 );
		d  = 2 * dx - dy;
		aincr = 2 * (dx - dy);
		bincr = 2 * dx;
		x = x1;
		y = y1;
		
		
		if((x>=0)&&(y>=0)&&
			(x<bit_xl)&&
			(y<bit_yl))
			bitmap[y*bit_xl+x]=color;
		
		for (y=y1+1; y<= y2; ++y )              // Execute line on Y-axes
		{
			if ( d >= 0 )
			{
                x += xincr;
                d += aincr;
			}
			else
			{
                d += bincr;
			}
			if((x>=0)&&(y>=0)&&
				(x<bit_xl)&&
				(y<bit_yl))
				bitmap[y*bit_xl+x]=color;
		}
	}
	else                                      // Check X-axes
	{
		if ( x1 > x2 )                         // x1 > x2?
		{
			fnV16SwapInt( &x1, &x2 );           // Yes --> Swap X1 with X2
			fnV16SwapInt( &y1, &y2 );           //         and Y1 with Y2
		}
		yincr = ( y2 > y1 ) ? 1 : -1;          // Set Y-axis increment
		dx = x2 - x1;
		dy = abs( y2-y1 );
		d  = 2 * dy - dx;
		aincr = 2 * (dy - dx);
		bincr = 2 * dy;
		x = x1;
		y = y1;
		
		if((x>=0)&&(y>=0)&&
			(x<bit_xl)&&
			(y<bit_yl))
			bitmap[y*bit_xl+x]=color;
		
		for (x=x1+1; x<=x2; ++x )              // Execute line on X-axes
		{
			if ( d >= 0 )
			{
                y += yincr;
                d += aincr;
			}
			else
                d += bincr;
			
			if((x>=0)&&(y>=0)&&
				(x<bit_xl)&&
				(y<bit_yl))
				bitmap[y*bit_xl+x]=color;
		}
	}
}


void CUGDraw::fnV16SwapInt( SLONG *i1, SLONG *i2 )
{
	SLONG dummy;
	
	dummy=*i2;
	*i2=*i1;
	*i1=dummy;
}

void CUGDraw::ShowText(RECT &rect, LPCWSTR lpWstr)
{
	char szChat[256]={0};
	WideCharToMultiByte(CP_ACP,0,lpWstr,-1,szChat,
		sizeof(szChat),NULL,NULL);

	if (getChatData()->m_bOuter)
	{
	
		RECT rectShow={0};
		SetRect(&rectShow,rect.left,rect.top+2,rect.right,rect.bottom);
		DrawText(getChatData()->m_hDC,szChat,strlen(szChat),&rectShow,DT_LEFT|DT_TOP);
	}
	else
	{
		print16(rect.left,rect.top+2,
			szChat,PEST_PUT,getChatData()->m_screenBuffer);
	}
	
}
