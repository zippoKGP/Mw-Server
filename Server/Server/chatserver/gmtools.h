// GMTools.h: interface for the CGMTools class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GMTOOLS_H__CA7515E1_F644_42A3_8D30_1EA187A5AE07__INCLUDED_)
#define AFX_GMTOOLS_H__CA7515E1_F644_42A3_8D30_1EA187A5AE07__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGMTools  
{
public:
	CGMTools();
	virtual ~CGMTools();

public:
	UGLONG			dispatchMsg(UGDWORD dwMsgID,UGDWORD dwParam,UGPCHAR pchData,UGDWORD dwLen);
	UGLONG			sendMessage(UGDWORD dwMsgID,UGDWORD dwParam,UGPCHAR pchData = NULL,UGDWORD dwLen = 0);
	
protected:
	UGLONG			sendGM(GMMSG* p);
	
public:
	UGDWORD			m_stepGM;
	UGDWORD			m_dwIP;
	UGWORD			m_wPort;
	UGBOOL			m_bOnline;
	UGDWORD			m_dwTimer;
	UGPVOID			m_pvConnect;
	UGPVOID			m_pvParent;
	INetServer*		m_pTCPComm;

};

#endif // !defined(AFX_GMTOOLS_H__CA7515E1_F644_42A3_8D30_1EA187A5AE07__INCLUDED_)
