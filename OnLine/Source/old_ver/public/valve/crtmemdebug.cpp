//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $Workfile:     $
// $Date: 2004/12/02 04:29:09 $
//
//-----------------------------------------------------------------------------
// $Log: crtmemdebug.cpp,v $
// Revision 1.1  2004/12/02 04:29:09  andy
// *** empty log message ***
//
// Revision 1.1  2004/11/12 12:36:13  andy
// *** empty log message ***
//
// Revision 1.1  2004/04/16 02:25:07  bob
// *** empty log message ***
//
// Revision 1.2  2004/03/07 09:36:40  andy
// *** empty log message ***
//
//
// $NoKeywords: $
//=============================================================================

#include "crtmemdebug.h"
#ifdef USECRTMEMDEBUG
#include <crtdbg.h>
#endif

void CheckHeap( void )
{
#ifdef USECRTMEMDEBUG
	_CrtCheckMemory();
#endif
}

// undone: this needs to be somehow made to construct before everything else.
// see http://msdn.microsoft.com/library/periodic/period97/dembugs.htm for info
void InitCRTMemDebug( void )
{
#ifdef USECRTMEMDEBUG
	_CrtSetDbgFlag( 
//	_CRTDBG_ALLOC_MEM_DF  | 
	_CRTDBG_CHECK_ALWAYS_DF |
	_CRTDBG_CHECK_CRT_DF | 
	_CRTDBG_DELAY_FREE_MEM_DF );
#endif
}
