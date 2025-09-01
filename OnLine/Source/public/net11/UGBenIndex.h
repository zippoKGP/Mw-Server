// UGIndex.h: interface for the CUGIndex class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UGINDEX_H__5F11838D_F784_4389_A24F_EDD4BAEA9836__INCLUDED_)
#define AFX_UGINDEX_H__5F11838D_F784_4389_A24F_EDD4BAEA9836__INCLUDED_

#include "UGBenDef.h"

class CUGIndex  
{
public:
	CUGIndex();
	virtual ~CUGIndex();

public:
	UG_ULONG		init(UG_PCHAR pchPathName,UG_BOOL bNumberIndex = TRUE,UG_BOOL bLoadInMem = TRUE);
	UG_ULONG		getValue(UG_PCHAR pchKey,UG_PCHAR& pchValue);
	UG_ULONG		getValue(UG_ULONG ulKey,UG_PCHAR& pchValue);
	UG_ULONG		cleanup();

private:
	UG_PVOID		m_pNumberIndex;
	UG_PVOID		m_pStringIndex;
	
};

#endif // !defined(AFX_UGINDEX_H__5F11838D_F784_4389_A24F_EDD4BAEA9836__INCLUDED_)
