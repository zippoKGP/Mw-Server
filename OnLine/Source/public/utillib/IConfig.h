// GConfig.h: interface for the CGConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GCONFIG_H__81758081_501C_4701_99FC_03C1F6A1DC22__INCLUDED_)
#define AFX_GCONFIG_H__81758081_501C_4701_99FC_03C1F6A1DC22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "tier0/platform.h"

class IConfig
{
public:
	virtual HRESULT Load(LPCTSTR strFileName) = 0;

	virtual HRESULT Save(LPCTSTR strFileName) = 0;

public:
	virtual void getValue(LPCTSTR strKeyWord, int& value) = 0;

	virtual void getValue(LPCTSTR strKeyWord, float& value) = 0;

	virtual void getValue(LPCTSTR strKeyWord, char*& value) = 0;

	virtual void setValue(LPCTSTR strKeyWord, int& value) = 0;
};

UTILLIB_INTERFACE IConfig *getConfig(void);

#endif // !defined(AFX_GCONFIG_H__81758081_501C_4701_99FC_03C1F6A1DC22__INCLUDED_)
