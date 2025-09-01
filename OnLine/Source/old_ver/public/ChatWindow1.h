
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

	HRESULT WinInit( HINSTANCE hInst);	//初始化窗口与类

//////////////////////////////////////////////////////////////////////////
//
	void setChatWindowPos(int nflag = 0);	//设置窗口位置

	HRESULT drawChatWindow(void);	//画客户区

	BOOL setChatWindowState(BOOL bVisable); //设置窗口是否可见
	
	void setCurrentScrn(void);	//分辨率改变时设置

	void updateChatWindow(void);	//更新窗体与与客户区坐标

	void selectPerson(void) {m_bSelectPerson = true;}	//点击右键时设置

	void draw(void);	//把一画好的内存hdc复制到显示hdc

	void drawChat(void);	//数据不变时画客户区

	BOOL preDraw(void);	//判断是否画聊天客户区

	void initTitle(void); //初始化标题数据

public:
private:


private:
    MSG		 m_msg;
    HWND     m_hWnd;

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

	HBRUSH m_hbrush;

	HBRUSH m_hbrushWhite;
	HBRUSH m_hbrushBlack;
	HBRUSH m_hbrushRed;
	HBRUSH m_hbrushGreen;
	HBRUSH m_hbrushBlue;
	HBRUSH m_hbrushYellow;
	HBRUSH m_hbrushPink;
	HBRUSH m_hbrushCyan;
	HBRUSH m_hbrushGray;
	HBRUSH m_hbrushLightGreen;
	HBRUSH m_hbrushLightBlue;
	HBRUSH m_hbrushOringe;
	HBRUSH m_hbrushLowGray;

	HFONT m_hfont;
	HFONT m_hfontNormal;

	DWORD m_nSaveTime; 

	BOOL m_bSelectPerson;

	BOOL m_bPause;

	int  m_nTitleList[CHAT_MAX_TITLE];

	CHAT_WND_SAVE m_ChatSave;
};

ChatWindow* getChatWindow();

#endif

