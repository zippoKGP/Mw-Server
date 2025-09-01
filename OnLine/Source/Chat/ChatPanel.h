/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
 2003 ALL RIGHTS RESERVED.

*file name    : chatpanel.h
*owner        : Robert
*description  : ���������
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
///��Ա����
//////////////////////////////////////////////////////////////////////////

	//IBaseTexture *                m_pChatPic;            //����ͼƬ��Դ

	vector<chat_word_t>           s_chat_list;            //�����б�

	FACEANDSTRINGLIST             m_faceandstring_list;   //��������������б�

	chat_faceandstring_t		  m_faceandstring;        //��������

	ARFACEANDSTRINGLIST::iterator m_pointerforfaceandstring;   //�������������Ϣ�б�Ķ�λ��

	CHATWORDLIST::iterator        m_pointerchatword;

	CHATWORDLIST                  m_chatword_list;

	LINEINFO                      m_lineinfo;             //�������������Ϣ�б�

	LINEINFO::iterator            pointer;                //�������������Ϣ�б�Ķ�λ��

	UINT16                        m_ChatBottom;           //�������ʱ������ʾ�����λ��

	UINT8                         linenum;                //���촦���е�ǰ����Ϣ

	POINT                         m_ptMouse;              //��ǰ���λ��                             

	UINT16                        m_dwColor;              //��ǰ�������ݵ���ɫ

	UINT8                         m_underline;            //��ǰ�������ݵ��»���                  

	UINT8                         m_flash;                //��ǰ�������ݵ�����

	ULONG                         m_flashTimeOpen;        //�������ݵ���ʾʱ��

	ULONG                         m_flashTimeOff;         //�������ݵĲ���ʾʱ��
	
	ULONG                         m_scrollTime;           //�������ݵĻع�ʱ��

	ULONG                         m_flashChat;            //��������ˢ��

	UINT8                         m_scrollFlag;           //�������ݵĻع���־

	UINT8                         m_showType[4];          //���������־
	
	int                           linewidth;              //��ǰ�������ݵ��п�

	POINT                         pt;                     //��ǰ�������Ķ�������


	int                           m_nFaceShowChatNum;      //��ǰ������

	UINT32                        m_nBgArgb;               //������ɫ

	bool                          m_bFlash;

	char                          m_szChat[1024];

	RECT                          m_panelRect;            //�����������

	RECT                          m_chatRect;             //������������

	bool                          m_bMiddle;              //����

	   


//////////////////////////////////////////////////////////////////////////
///��Ա����
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
