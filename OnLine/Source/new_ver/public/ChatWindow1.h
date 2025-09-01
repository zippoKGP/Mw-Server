
#ifndef __CHAT_WINDOW_H__
#define __CHAT_WINDOW_H__

#include <windows.h>

#define  CHAT_MAX_TITLE  8	//����������
#define  CHAT_MAX_ROW	30 //���������

typedef struct _CHAR_WND_TEXT_STRUCT
{
	char sztext[1024];	//�������1024�ֽ�
	long nTitle;
	long ntextid;
	long ntextHeight;
	long ntextTitleOffset;
}CHAT_WND_TEXT;

typedef struct _CHAT_WND_SAVE_STRUCT
{
	CHAT_WND_TEXT text[CHAT_MAX_ROW];
	UINT nStart:12;	//���챣�����4096��
	UINT nEnd:12;
	UINT nCount:7;
	UINT nFlag:1;
	
}CHAT_WND_SAVE;


class ChatWindow
{
public:
	ChatWindow();
	virtual ~ChatWindow();

	HRESULT WinInit( HINSTANCE hInst);	//��ʼ����������

//////////////////////////////////////////////////////////////////////////
//
	void setChatWindowPos(int nflag = 0);	//���ô���λ��

	HRESULT drawChatWindow(void);	//���ͻ���

	BOOL setChatWindowState(BOOL bVisable); //���ô����Ƿ�ɼ�
	
	void setCurrentScrn(void);	//�ֱ��ʸı�ʱ����

	void updateChatWindow(void);	//���´�������ͻ�������

	void selectPerson(void) {m_bSelectPerson = true;}	//����Ҽ�ʱ����

	void draw(void);	//��һ���õ��ڴ�hdc���Ƶ���ʾhdc

	void drawChat(void);	//���ݲ���ʱ���ͻ���

	BOOL preDraw(void);	//�ж��Ƿ�����ͻ���

	void initTitle(void); //��ʼ����������

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

