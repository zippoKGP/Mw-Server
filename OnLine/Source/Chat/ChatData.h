// ChatData.h: interface for the CChatData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHATDATA_H__4B5C6378_357A_48A0_A1B4_EC21DAD94394__INCLUDED_)
#define AFX_CHATDATA_H__4B5C6378_357A_48A0_A1B4_EC21DAD94394__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define    USERBMPWIDTH               60
#define    USERBMPHEIGHT              60

class CChatData  
{
public:
	void shutDown();
	void init();
	CChatData();
	virtual ~CChatData();

	BMP*      m_screenBuffer; 
	RECT      m_panelRect;            //聊天面板区域
	RECT      m_chatRect;             //聊天文字区域

	ARFACEANDSTRINGLIST           m_arrfaceandstringlist;  //解析过的聊天信息

	bool      m_bFrame;

	ULONG s_nOldtime;
	ULONG s_nNewtime;
	bool  m_bOuter;

	CHATWORDLIST                  m_chat_list;
	ARFACEANDSTRINGLIST           m_arrlist;
	UINT                          m_nVal;

	CHATWORDLIST                  m_uchat_list;
	ARFACEANDSTRINGLIST           m_uarrlist;
	
	CHATWORDLIST                  m_outer_chat_list;
	ARFACEANDSTRINGLIST           m_outer_arrlist;
	HDC                           m_hDC;
	HPEN                          m_hPen;
	HFONT                         m_hFont;
	BMP24*                        m_pBmp; 
	UCHAR*                        m_pBuffer;

};

CChatData* getChatData();

#endif // !defined(AFX_CHATDATA_H__4B5C6378_357A_48A0_A1B4_EC21DAD94394__INCLUDED_)
