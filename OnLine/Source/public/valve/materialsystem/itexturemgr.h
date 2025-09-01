#ifndef ITEXTRUEMGR_H
#define ITEXTRUEMGR_H

#define TEXTURE_MANAGER_INTERFACE_VERSION "texture_mgr_001"

#include "ibasetexture.h"

class ITextureMgr
{
public:
	virtual HRESULT Init(void) = 0;

	virtual HRESULT Shutdown(void) = 0;

public:
	virtual IBaseTexture *AddTexture(LPCTSTR strFileName, DWORD dwOptions) = 0;

	virtual IBaseTexture *AddTexture(LPCTSTR strName, int w, int h, ImageFormat format) = 0;
	
	virtual HRESULT RemoveTexture(IBaseTexture *pTex) = 0;
	
	virtual HRESULT EnumTextureToFile(LPCTSTR strFileName) = 0;
	
	virtual HRESULT setPath(LPCTSTR strPathName, LPCTSTR strPathID = NULL) = 0;

	virtual LPCTSTR getPathName(void) = 0;

	virtual LPCTSTR getPathID(void) = 0;
};

extern ITextureMgr *g_pTextureMgr;

#if !defined (SAFE_DELETE_TEXTURE)
#define SAFE_DELETE_TEXTURE(m, p) { if (p && m) { m->RemoveTexture(p); (p)=NULL;} }
#endif

#endif