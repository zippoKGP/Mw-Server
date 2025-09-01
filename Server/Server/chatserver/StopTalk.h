// StopTalk.h: interface for the CStopTalk class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STOPTALK_H__CFE495FF_BD48_4541_8640_3A25A266846D__INCLUDED_)
#define AFX_STOPTALK_H__CFE495FF_BD48_4541_8640_3A25A266846D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStopTalk
{
public:
	CStopTalk();
	virtual ~CStopTalk();

public:
	UGINT		m_nPlayerID;
	UGDWORD		m_dwBeginer;
	UGDWORD		m_dwTimes;
	UGBOOL		m_bRemove;

};

#endif // !defined(AFX_STOPTALK_H__CFE495FF_BD48_4541_8640_3A25A266846D__INCLUDED_)
