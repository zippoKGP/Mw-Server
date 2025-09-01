
#ifndef __CHAT_WINDOW_H__
#define __CHAT_WINDOW_H__

#include <windows.h>

#define  CHAT_MAX_TITLE  8	//最多标题类型
#define  CHAT_MAX_ROW	30 //最大聊天行

typedef struct _CHAR_WND_TEXT_STRUCT
{
	char sztext[1024];	//聊天最大1024字节
	long nTitle;
	long ntextid;
	long ntextHeight;
	long ntextTitleOffset;
}CHAT_WND_TEXT;

typedef struct _CHAT_WND_SAVE_STRUCT
{
	CHAT_WND_TEXT text[CHAT_MAX_ROW];
	UINT nStart:12;	//聊天保存最多4096条
	UINT nEnd:12;
	UINT nCount:7;
	UINT nFlag:1;
	
}CHAT_WND_SAVE;


class ChatWindow
{
public:
	ChatWindow();
	virtual ~ChatWindow();

	HRESULT WinInit( HINSTANCE hInst);

//////////////////////////////////////////////////////////////////////////
//
	void setChatWindowPos(int nflag = 0);

	HRESULT drawChatWindow(void);

	BOOL setChatWindowState(BOOL bVisable);
	
	void setCurrentScrn(void);

	void updateChatWindow(void);

	void selectPerson(void) {m_bSelectPerson = true;}

	void draw(void);

	HRESULT modiBackBmp(void);

public:

private:
	BOOL preDraw(HWND hwnd);
	
	HRESULT initTitle(void);

	HRESULT initClass(void);

	void setPersonObject(const long& display_y, const POINT& pt);

	void setDrawState(HWND hwnd, const POINT& pt);

	void drawChat(void);
	
private:

    HWND m_hWnd;

	RECT m_rtMain;
	RECT m_rtChild;
	RECT m_rtClient;

	BOOL m_bVisable;

	int m_nScrnX;
	int m_nScrnY;

	int m_nWndWidth;

	HDC m_hdc;
	HDC m_hdcMe;
	HDC m_hdcTitle;

	DWORD m_nSaveTime; 

	BOOL m_bSelectPerson;

	int  m_nTitleList[CHAT_MAX_TITLE];

	CHAT_WND_SAVE m_ChatSave;

	HBITMAP m_brBitmap;
	
	BITMAP m_brBM;
};

ChatWindow* getChatWindow();

#endif

