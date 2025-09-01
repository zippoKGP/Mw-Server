// IndexKeyValue.h: interface for the CIndexKeyValue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INDEXKEYVALUE_H__831103EF_E2B7_40B7_B048_34D61FDA700C__INCLUDED_)
#define AFX_INDEXKEYVALUE_H__831103EF_E2B7_40B7_B048_34D61FDA700C__INCLUDED_

#include "net/UGBenDef.h"

class CIndexKeyValue  
{
public:
	CIndexKeyValue();
	virtual ~CIndexKeyValue();

public:
	UG_ULONG		cleanup();
	UG_ULONG		getValue(UG_PCHAR& pchValue);
	UG_ULONG		getValue(FILE* pFile,UG_PCHAR pchTemp,UG_PCHAR& pchValue);
	
public:
	UG_ULONG		m_ulPos;
	UG_ULONG		m_ulCount;
	UG_PCHAR		m_pchValue;
	
};

#endif // !defined(AFX_INDEXKEYVALUE_H__831103EF_E2B7_40B7_B048_34D61FDA700C__INCLUDED_)
