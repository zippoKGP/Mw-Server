// WorldbaseManagerConfig.h: interface for the CWorldbaseManagerConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORLDBASEMANAGERCONFIG_H__943A3EBD_7798_4B32_B76E_FC486E2B842F__INCLUDED_)
#define AFX_WORLDBASEMANAGERCONFIG_H__943A3EBD_7798_4B32_B76E_FC486E2B842F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWorldbaseManagerConfig  
{
public:
	CWorldbaseManagerConfig();
	virtual ~CWorldbaseManagerConfig();

public:
	UG_LONG			init(UG_PCHAR pchPathName);
	UG_LONG			cleanup();
	
public:
	UG_LONG			cout(FILE* p);
	
public:
	UG_DWORD			m_dwIPForWorldbase; //供worldbase连接的ip
	UG_DWORD			m_dwPortForWorldbase; //供worldbase连接的port
	UG_LONG				m_lWorldbaseCount;
	CWorldbaseConfig*	m_pWorldbaseConfig;
	
};

#endif // !defined(AFX_WORLDBASEMANAGERCONFIG_H__943A3EBD_7798_4B32_B76E_FC486E2B842F__INCLUDED_)
