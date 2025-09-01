#ifndef IBASETEXTURE_H
#define IBASETEXTURE_H

class IBaseTexture
{
public:
	virtual HRESULT Bind(int usage) = 0;

	virtual int getTransType(void) = 0;
	
	virtual HRESULT Blt2D(RECT *pSrc, RECT *pDest, DWORD dwColor, float flip = -1.0f) = 0;

	virtual int getWidth(void) = 0;

	virtual int getHeight(void) = 0;
	
	virtual bool IsValid(void) = 0;

	virtual HRESULT LockRect(DWORD level, LPRECT pRect, DWORD flag, VOID*& pBits, int& pitch) = 0;

	virtual HRESULT UnlockRect(DWORD level) = 0;
};

#endif