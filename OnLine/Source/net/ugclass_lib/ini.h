// Ini.h: interface for the CIni class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INI_H__7FB596AF_A72E_4A68_A04B_422A49944E41__INCLUDED_)
#define AFX_INI_H__7FB596AF_A72E_4A68_A04B_422A49944E41__INCLUDED_

#include "net/UGBenDef.h"

class CIni  
{
public:
	CIni();
	virtual ~CIni();

public:
	UG_ULONG		init(UG_PCHAR pchPathName);
	UG_ULONG		getValue(UG_PCHAR pchApp,UG_PCHAR pchKey,UG_PCHAR& pchValue);
	UG_LONG			getCount(UG_PCHAR pchApp);
	UG_LONG			getKeyValue(UG_LONG lIndex,UG_PCHAR pchApp,UG_PCHAR& pchKey,UG_PCHAR& pchValue);
	UG_ULONG		cleanup();

protected:
	UG_ULONG		parseKeyAndValue(UG_PCHAR pchLine,CIniApp** ppIniApp);
	UG_ULONG		parseApp(UG_PCHAR pchLine,CIniApp** ppIniApp);
	UG_ULONG		removeSpace(UG_PCHAR pchLine,UG_INT32 n32Count,UG_INT32& n32Left,UG_INT32& n32Right);
	UG_ULONG		getMaxAppKey(UG_ULONG& ulMaxApp,UG_ULONG& ulMaxKey);
	
private:
	list<CIniApp*>	m_list;
	UG_ULONG		m_ulMaxApp;
	UG_ULONG		m_ulMaxKey;
	UG_PCHAR		m_pchMaxApp;
	UG_PCHAR		m_pchMaxKey;
	
};

#endif // !defined(AFX_INI_H__7FB596AF_A72E_4A68_A04B_422A49944E41__INCLUDED_)
