// IniApp.h: interface for the CIniApp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INIAPP_H__D569D317_5840_4D16_A7A6_E3E301C409CD__INCLUDED_)
#define AFX_INIAPP_H__D569D317_5840_4D16_A7A6_E3E301C409CD__INCLUDED_

#include "net/UGBenDef.h"

class CIniApp  
{
public:
	CIniApp();
	virtual ~CIniApp();

public:
	UG_ULONG		cleanup();
	UG_ULONG		convertAppAndKeyLower();
	UG_ULONG		getValue(UG_PCHAR pchApp,UG_PCHAR pchKey,UG_PCHAR& pchValue);
	UG_LONG			getKeyValue(UG_LONG lIndex,UG_PCHAR pchApp,UG_PCHAR& pchKey,UG_PCHAR& pchValue);
	UG_ULONG		getMaxAppKey(UG_ULONG& ulMaxApp,UG_ULONG& ulMaxKey);
	
public:
	list<CIniKeyValue*>		m_list;
	UG_PCHAR				m_pchAPP;
	
};

#endif // !defined(AFX_INIAPP_H__D569D317_5840_4D16_A7A6_E3E301C409CD__INCLUDED_)
