/************************************************************************* 
*****
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.
*file name    : UGImage.h
*owner        : Andy
*description  : Image belong to  control class 
*modified     : 1. created on 2003/8/12 by Andy
*************************************************************************
*****/  

#if !defined(AFX_UGIMAGE_H__8A463E3E_671C_406D_BBD9_E06D5273EE2F__INCLUDED_)
#define AFX_UGIMAGE_H__8A463E3E_671C_406D_BBD9_E06D5273EE2F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "tier0/platform.h"

#ifdef _3D_RENDER
class IBaseTexture; 
#else
struct IMG256_GROUP_STRUCT;
typedef IMG256_GROUP_STRUCT IBaseTexture;
#endif

class CUGImage
{
private:
    RECT m_rectPos;							//位置矩形

    RECT m_rectClip;						//在图片中位置矩形

	IBaseTexture *m_pTexture;				//句柄

	int m_nIndex;

	BOOL m_bDraw;							//是否显示

	int m_nTexID;
public:

	CUGImage();

    CUGImage(LPCTSTR strFileName, LPRECT pPos, LPRECT pClip, int nIndex = 0);

    virtual ~CUGImage();

public:
	HRESULT Init(LPCTSTR strFileName, LPRECT pPos, LPRECT pClip, int nIndex = 0);

	HRESULT Init(LPCTSTR strFileName, int nIndex, int frame);

    HRESULT loadTexture(LPCTSTR strFileName);
	
	inline void setDraw(BOOL bDraw = TRUE) { m_bDraw = bDraw; }

	inline BOOL getDraw(void) { return m_bDraw; }

	inline int getIndex(void) { return m_nIndex; }

	inline BOOL ptInRect(POINT pt) { return ::PtInRect(&m_rectPos, pt); }

	virtual void    setPos(LPPOINT pPt);
	virtual POINT	getPos();

	RECT& getDstRect(void) { return m_rectPos; }

	int getWidth();
	int getHeight();

	int getTextureWidth();
	int getTextureHeight();

	void setClipRect(LPRECT pRect);
	void setDestRect(LPRECT pRect);

	INLINE IBaseTexture *getTexture(void) { return m_pTexture; }

public:

	virtual HRESULT Paint(LPPOINT ptOffset, void *bmp = NULL);

public:

    virtual HRESULT Load(FILE *fp);
    virtual HRESULT Save(FILE *fp);
};

#endif // !defined(AFX_UGIMAGE_H__8A463E3E_671C_406D_BBD9_E06D5273EE2F__INCLUDED_)
