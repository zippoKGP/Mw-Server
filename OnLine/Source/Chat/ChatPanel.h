/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
 2003 ALL RIGHTS RESERVED.

*file name    : chatpanel.h
*owner        : Robert
*description  : 主聊天面板
*modified     : 2004/05/01
******************************************************************************/ 

#if !defined(AFX_CHATPANEL_H__89BA9BA1_2D23_410D_9E1C_15F0FAA2ED1E__INCLUDED_)
#define AFX_CHATPANEL_H__89BA9BA1_2D23_410D_9E1C_15F0FAA2ED1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define     CHATCODE            (wchar_t)35          //'#'

#define     CHATSEP             (wchar_t)58          //':'




class CChatPanel  
{
private:

//////////////////////////////////////////////////////////////////////////
///成员变量
//////////////////////////////////////////////////////////////////////////

	//IBaseTexture *                m_pChatPic;            //聊天图片资源

	vector<chat_word_t>           s_chat_list;            //聊天列表

	FACEANDSTRINGLIST             m_faceandstring_list;   //单句聊天的内容列表

	chat_faceandstring_t		  m_faceandstring;        //聊天内容

	ARFACEANDSTRINGLIST::iterator m_pointerforfaceandstring;   //单句聊天的行信息列表的定位器

	CHATWORDLIST::iterator        m_pointerchatword;

	CHATWORDLIST                  m_chatword_list;

	LINEINFO                      m_lineinfo;             //单句聊天的行信息列表

	LINEINFO::iterator            pointer;                //单句聊天的行信息列表的定位器

	UINT16                        m_ChatBottom;           //聊天滚动时候能显示的最底位置

	UINT8                         linenum;                //聊天处理中当前行信息

	POINT                         m_ptMouse;              //当前鼠标位置                             

	UINT16                        m_dwColor;              //当前聊天内容的颜色

	UINT8                         m_underline;            //当前聊天内容的下划线                  

	UINT8                         m_flash;                //当前聊天内容的闪动

	ULONG                         m_flashTimeOpen;        //聊天内容的显示时间

	ULONG                         m_flashTimeOff;         //聊天内容的不显示时间
	
	ULONG                         m_scrollTime;           //聊天内容的回滚时间

	ULONG                         m_flashChat;            //聊天内容刷新

	UINT8                         m_scrollFlag;           //聊天内容的回滚标志

	UINT8                         m_showType[4];          //四种聊天标志
	
	int                           linewidth;              //当前聊天内容的行宽

	POINT                         pt;                     //当前聊天表情的顶点坐标


	int                           m_nFaceShowChatNum;      //当前表情数

	UINT32                        m_nBgArgb;               //背景颜色

	bool                          m_bFlash;

	char                          m_szChat[1024];

	RECT                          m_panelRect;            //聊天面板区域

	RECT                          m_chatRect;             //聊天文字区域

	bool                          m_bMiddle;              //居中

	   


//////////////////////////////////////////////////////////////////////////
///成员函数
//////////////////////////////////////////////////////////////////////////
	HRESULT  drawTextAndFace(chat_word_t char_word,int& line_width,int& height,int &line_height);
	
	HRESULT  drawChatText(wstring char_word,int& line_width,int& height,int &line_height,int lineFlag,int chat_type_id=-1);
	
	wstring  CheckTextOrFace(wstring checkStr,int &position,UINT8 &nKey);
	
//	void     ShowChat(chat_word_t &char_word,FACEANDSTRINGLIST& m_faceandstring_list);

	void     ShowUserDef(chat_word_t &char_word,FACEANDSTRINGLIST& m_faceandstring_list, BITMAP* pbrbm = NULL);
	
	void     SetChatRect(RECT & destRect,RECT  scrRect);
	
	void     InitChatRect(RECT & destRect,RECT scrRect);
	
	
	void     SetLineHeight(int lineheight);
	
	void     DisposeEffect(wstring wStr,wstring disposeStr,int &position ,int &line_width,int &height,int &line_height);
	
	UHINT    GetColor(wstring colorStr,int& pos);
	
//	HRESULT  setChatLine(chat_word_t &char_word,int &line_height);

	HRESULT  setUseDefLine(chat_word_t &char_word,int &line_height);

//	bool     getFlashChatNow();
	

public:
	void CreateOuterUseDefText(chat_word_t &chat_word,RECT rectFormat);
	void AutoShow(chat_word_t &chat_word,RECT rectFormat);
	void showUserDefText(CHATWORDLIST& chat_list,ARFACEANDSTRINGLIST& arrlist, BITMAP* pbrbm = NULL);
	int  CreateUseDefText(chat_word_t& chat_word,RECT rectFormat,bool bShow);
	void CreateUseDefText(chat_word_t &chat_word,RECT rectFormat);

	CChatPanel();
	
	virtual ~CChatPanel();	
	
	HRESULT  ShowChatFrame();
	
	int      SetChat_List(vector<chat_word_t> chat_list){s_chat_list=chat_list;return S_OK;}

	int      destory();
	
	int      init();
	
};

#endif // !defined(AFX_CHATPANEL_H__89BA9BA1_2D23_410D_9E1C_15F0FAA2ED1E__INCLUDED_)
