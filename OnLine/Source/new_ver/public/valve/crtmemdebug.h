//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $Workfile:     $
// $Date: 2004/12/02 04:29:09 $
//
//-----------------------------------------------------------------------------
// $Log: crtmemdebug.h,v $
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

#ifndef CRTMEMDEBUG_H
#define CRTMEMDEBUG_H
#pragma once

#ifdef USECRTMEMDEBUG

#include <crtdbg.h>
#define MEMCHECK CheckHeap()
void CheckHeap( void );

#else

#define MEMCHECK

#endif

void InitCRTMemDebug( void );


#endif // CRTMEMDEBUG_H
