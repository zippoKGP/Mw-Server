
#if !defined(AFX_UGLOG_H__F527E0F9_8299_4030_AE00_BCD6594AF4D4__INCLUDED_)
#define AFX_UGLOG_H__F527E0F9_8299_4030_AE00_BCD6594AF4D4__INCLUDED_

#include "UGBenDef.h"

class CUGLog  
{
public:
	CUGLog();
	virtual ~CUGLog();

public:
	UG_BOOL			init(UG_PCHAR pchPathName);
	UG_BOOL			UGLog(UG_BOOL bFlush, UG_PCHAR pchMsg, ...);
	UG_BOOL			UGLog(UG_PCHAR pchMsg, ...);
	UG_PCHAR		getTime();
	UG_BOOL			cleanup();
	
protected:

private:	
	UG_PVOID		m_pFile;

};

#endif // !defined(AFX_UGLOG_H__F527E0F9_8299_4030_AE00_BCD6594AF4D4__INCLUDED_)
