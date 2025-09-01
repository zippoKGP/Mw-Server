// UGIni.h: interface for the CUGIni class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UGINI_H__6D574BE5_6EDF_4853_910E_9A6B6C69E33B__INCLUDED_)
#define AFX_UGINI_H__6D574BE5_6EDF_4853_910E_9A6B6C69E33B__INCLUDED_

#include "UGBenDef.h"

class CUGIni  
{
public:
	CUGIni();
	virtual ~CUGIni();

public:
	UG_ULONG		init(UG_PCHAR pchPathName);
	UG_ULONG		getValue(UG_PCHAR pchApp,UG_PCHAR pchKey,UG_PCHAR& pchValue);
	UG_ULONG		getValue(UG_PCHAR pchApp,UG_PCHAR pchKey,UG_INT32& n32Value);
	UG_ULONG		getValue(UG_PCHAR pchApp,UG_PCHAR pchKey,UG_ULONG& ulValue);
	UG_ULONG		getValue(UG_PCHAR pchApp,UG_PCHAR pchKey,UG_WORD& wValue);
	UG_ULONG		getValue(UG_PCHAR pchApp,UG_PCHAR pchKey,UG_FLOAT64& f32Value);
	UG_LONG			getCount(UG_PCHAR pchApp);
	UG_LONG			getKeyValue(UG_LONG lIndex,UG_PCHAR pchApp,UG_PCHAR& pchKey,UG_PCHAR& pchValue);
	UG_ULONG		cleanup();

private:	
	UG_PVOID		m_pUGIni;

};

#endif // !defined(AFX_UGINI_H__6D574BE5_6EDF_4853_910E_9A6B6C69E33B__INCLUDED_)
