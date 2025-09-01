/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : Cout.h
*owner        : Ben
*description  : 基本信息到处类。
*modified     : 2004/12/20
******************************************************************************/ 

#if !defined(AFX_COUT_H__00924DCA_5DEB_4A9A_9C10_8F53DED038B3__INCLUDED_)
#define AFX_COUT_H__00924DCA_5DEB_4A9A_9C10_8F53DED038B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCout  
{
public:
	CCout();
	virtual ~CCout();

public:
	static UG_LONG		coutConfig();
	static UG_LONG		coutWorldbase();
	static UG_LONG		coutPlayerManager();
	static UG_LONG		coutSceneManager();
	static UG_LONG		coutPlayerCount();
	static UG_LONG		coutAll();

};

#endif // !defined(AFX_COUT_H__00924DCA_5DEB_4A9A_9C10_8F53DED038B3__INCLUDED_)
