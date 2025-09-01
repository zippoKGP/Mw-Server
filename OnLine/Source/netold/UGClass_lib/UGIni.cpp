// UGIni.cpp: implementation of the CUGIni class.
//
//////////////////////////////////////////////////////////////////////

#include "IncAll.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUGIni::CUGIni()
{
	m_pUGIni = NULL;
}

CUGIni::~CUGIni()
{
	cleanup();
}

UG_ULONG CUGIni::init(UG_PCHAR pchPathName)
{
	cleanup();
	m_pUGIni = new CIni;
	return ((CIni*)m_pUGIni)->init(pchPathName);
}

UG_ULONG CUGIni::getValue(UG_PCHAR pchApp,UG_PCHAR pchKey,UG_PCHAR& pchValue)
{
	if(m_pUGIni)
	{
		pchValue = NULL;
		return ((CIni*)m_pUGIni)->getValue(pchApp,pchKey,pchValue);
	}
	return -1;
}

UG_ULONG CUGIni::cleanup()
{
	if(m_pUGIni)
	{
		CIni* p = (CIni*)m_pUGIni;
		ReleaseP<CIni*>(p);
		m_pUGIni = NULL;
	}
	return 0;
}

UG_ULONG CUGIni::getValue(UG_PCHAR pchApp,UG_PCHAR pchKey,UG_INT32& n32Value)
{
	n32Value = 0;
	if(m_pUGIni)
	{
		UG_PCHAR pchValue = NULL;
		((CIni*)m_pUGIni)->getValue(pchApp,pchKey,pchValue);
		if(pchValue)
		{
			n32Value = atoi(pchValue);
			return 0;
		}
		return -1;
	}
	return -1;
}

UG_ULONG CUGIni::getValue(UG_PCHAR pchApp,UG_PCHAR pchKey,UG_WORD& wValue)
{
	wValue = 0;
	if(m_pUGIni)
	{
		UG_PCHAR pchValue = NULL;
		((CIni*)m_pUGIni)->getValue(pchApp,pchKey,pchValue);
		if(pchValue)
		{
			wValue = atoi(pchValue);
			return 0;
		}
		return -1;
	}
	return -1;
}

UG_ULONG CUGIni::getValue(UG_PCHAR pchApp,UG_PCHAR pchKey,UG_ULONG& ulValue)
{
	ulValue = 0;
	if(m_pUGIni)
	{
		UG_PCHAR pchValue = NULL;
		((CIni*)m_pUGIni)->getValue(pchApp,pchKey,pchValue);
		if(pchValue)
		{
			ulValue = atoi(pchValue);
			return 0;
		}
		return -1;
	}
	return -1;
}

UG_ULONG CUGIni::getValue(UG_PCHAR pchApp,UG_PCHAR pchKey,UG_FLOAT64& f64Value)
{
	f64Value = 0.0;
	if(m_pUGIni)
	{
		UG_PCHAR pchValue = NULL;
		((CIni*)m_pUGIni)->getValue(pchApp,pchKey,pchValue);
		if(pchValue)
		{
			f64Value = atof(pchValue);
			return 0;
		}
		return -1;
	}
	return -1;
}

UG_LONG CUGIni::getCount(UG_PCHAR pchApp)
{
	if(m_pUGIni)
	{
		return ((CIni*)m_pUGIni)->getCount(pchApp);
	}
	return -1;
}

UG_LONG CUGIni::getKeyValue(UG_LONG lIndex,UG_PCHAR pchApp,UG_PCHAR& pchKey,UG_PCHAR& pchValue)
{
	if(m_pUGIni)
	{
		return ((CIni*)m_pUGIni)->getKeyValue(lIndex,pchApp,pchKey,pchValue);
	}
	return -1;
}

