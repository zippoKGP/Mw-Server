// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__BEE6CEA0_9AC9_4933_B153_F8087DD6A375__INCLUDED_)
#define AFX_STDAFX_H__BEE6CEA0_9AC9_4933_B153_F8087DD6A375__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#pragma warning(disable:4786)
#include <winsock2.h>
#include <assert.h>
#include <list>
#include <map>
#include <vector>
using namespace std;

#include "utillib/utillib.h"
#include "Interface.h"

#include "net/UGBenDef.h"
#include "net/uggcp.h"
#include "net/UGBenGlobalFun.h"
#include "net/IClientNet.h"
#include "net/UGBenMemoryPool.h"
#include "net/UGBenProgram.h"
#include "net/UGBenIni.h"
#include "net/UGBenLog.h"
#include "net/BenTCPComm.h"
#include "ClientNet.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__BEE6CEA0_9AC9_4933_B153_F8087DD6A375__INCLUDED_)
