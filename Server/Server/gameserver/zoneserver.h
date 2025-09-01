// GameServer.h : main header file for the GAMESERVER application
//

#if !defined(AFX_GAMESERVER_H__A464D1F0_D37E_424C_A7F4_6C03FD3C6908__INCLUDED_)
#define AFX_GAMESERVER_H__A464D1F0_D37E_424C_A7F4_6C03FD3C6908__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
/////////////////////////////////////////////////////////////////////////////
// CGameServerApp:
// See GameServer.cpp for the implementation of this class
//

class CGameServerApp : public CWinApp
{
public:
	CGameServerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGameServerApp)
	public:
	virtual BOOL InitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CGameServerApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAMESERVER_H__A464D1F0_D37E_424C_A7F4_6C03FD3C6908__INCLUDED_)
