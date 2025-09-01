// UGImage.h: interface for the CUGImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UGIMAGE_H__8D183F3B_7278_4FB0_8682_6DFFC6D70782__INCLUDED_)
#define AFX_UGIMAGE_H__8D183F3B_7278_4FB0_8682_6DFFC6D70782__INCLUDED_

#include "UGBenDef.h"

class CUGImage  
{
public:
	CUGImage();
	virtual ~CUGImage();

public:
	static HBITMAP		loadBMP(HINSTANCE hInst,UG_PCHAR pchPathName); //文件
	static HBITMAP		loadBMP(HINSTANCE hInst,UG_UINT32 u32ID); //资源
	static HBITMAP		loadBMP(UG_INT32 n32Width,UG_INT32 u32Height,UG_PCHAR pchMem,UG_ULONG ulLen); //24位的颜色数据
	
public:
	static HBITMAP		loadJPG(UG_PCHAR pchPathName);
	static HBITMAP		loadJPG(HINSTANCE hInst,UG_PCHAR pchName,UG_PCHAR pchType = "jpg");
	static HBITMAP		loadJPG(UG_PCHAR pchMem,UG_ULONG ulLen);
	
public:
	static HBITMAP		loadTGA(UG_PCHAR pchPathName); //A值为0时RGB为(0,0,0)，RGB为(0,0,0)时改为(1,0,0)。
	static HBITMAP		loadTGA(HINSTANCE hInst,UG_PCHAR pchName,UG_PCHAR pchType = "tga");
	static HBITMAP		loadTGA(UG_PCHAR pchMem,UG_ULONG ulLen);
	
public:
	static UG_INT32		getBMP(HBITMAP hBitmap,LPBITMAP pBm);
	
protected:
	static HBITMAP		loadTGA16(UG_ULONG ulWidth,UG_ULONG ulHeight,UG_PCHAR pchMem);
	static HBITMAP		loadTGA16RunLength(UG_ULONG ulWidth,UG_ULONG ulHeight,UG_PCHAR pchMem);
	static HBITMAP		loadTGA24(UG_ULONG ulWidth,UG_ULONG ulHeight,UG_PCHAR pchMem);
	static HBITMAP		loadTGA24RunLength(UG_ULONG ulWidth,UG_ULONG ulHeight,UG_PCHAR pchMem);
	static HBITMAP		loadTGA32(UG_ULONG ulWidth,UG_ULONG ulHeight,UG_PCHAR pchMem);
	static HBITMAP		loadTGA32RunLength(UG_ULONG ulWidth,UG_ULONG ulHeight,UG_PCHAR pchMem);
	
};

#endif // !defined(AFX_UGIMAGE_H__8D183F3B_7278_4FB0_8682_6DFFC6D70782__INCLUDED_)
