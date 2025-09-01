// UGIndex.cpp: implementation of the CUGIndex class.
//
//////////////////////////////////////////////////////////////////////

#include "IncAll.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUGIndex::CUGIndex()
{
	m_pNumberIndex = NULL;
	m_pStringIndex = NULL;
}

CUGIndex::~CUGIndex()
{
	cleanup();
}

UG_ULONG CUGIndex::init(UG_PCHAR pchPathName,UG_BOOL bNumberIndex,UG_BOOL bLoadInMem)
{
	if(m_pNumberIndex || m_pStringIndex)
	{
		return -1;
	}
	if(bNumberIndex)
	{
		CNumberIndex* p = new CNumberIndex;
		m_pNumberIndex = p;
		return p->init(pchPathName,bNumberIndex);
	}
	else
	{
		CStringIndex* p = new CStringIndex;
		m_pStringIndex = p;
		return p->init(pchPathName,bNumberIndex);
	}
	return 0;
}

UG_ULONG CUGIndex::getValue(UG_PCHAR pchKey,UG_PCHAR& pchValue)
{
	if(m_pStringIndex)
	{
		CStringIndex* p = (CStringIndex*)m_pStringIndex;
		return p->getValue(pchKey,pchValue);
	}
	return -1;
}

UG_ULONG CUGIndex::getValue(UG_ULONG ulKey,UG_PCHAR& pchValue)
{
	if(m_pNumberIndex)
	{
		CNumberIndex* p = (CNumberIndex*)m_pNumberIndex;
		return p->getValue(ulKey,pchValue);
	}
	return -1;
}

UG_ULONG CUGIndex::cleanup()
{
	if(m_pNumberIndex)
	{
		CNumberIndex* p = (CNumberIndex*)m_pNumberIndex;
		ReleaseP<CNumberIndex*>(p);
		m_pNumberIndex = NULL;
	}
	if(m_pStringIndex)
	{
		CStringIndex* p = (CStringIndex*)m_pStringIndex;
		ReleaseP<CStringIndex*>(p);
		m_pStringIndex = NULL;
	}
	return 0;
}

