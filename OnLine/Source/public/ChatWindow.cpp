

#include "chatwindow.h"
#include <mmsystem.h>

//////////////////////////////////////////////////////////////////////////
//
#include "rays.h"
#include "winmain.h"

#include "engine_global.h"
#include "utility.h"

#include "xvga.h"
#include "vari-ext.h"

static ChatWindow  g_sChatWindow;

ChatWindow* getChatWindow()
{
	return &g_sChatWindow;
}

LRESULT CALLBACK MainWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

#define WINDOW_HEIGHT		600
#define WINDOW_MIN_WIDTH	100
#define WINDOW_MAX_WIDTH	760
#define WINDOW_DEF_WIDTH	220
#define DRAW_MIN_TIME		100
#define CHAT_TITLE_WIDTH	40
#define CHAT_MAX_BACK_NUM	5
#define BACK_MAX_PIXEL_WIDTH	128


#define PAUSE_BACK_COLOR	RGB(50,50,50,)
#define NORMAL_BACK_COLOR	RGB(0, 0, 0)
#define LINE_BACK_COLOR		RGB(80, 80,80 )

#define CHAT_WHITE_COLOR  RGB(255,255,255)
#define CHAT_BLACK_COLOR  RGB(0,0,0)
#define CHAT_RED_COLOR  RGB(255,0,0)
#define CHAT_GREEN_COLOR  RGB(0,255,0)
#define CHAT_BLUE_COLOR  RGB(0,0,255)
#define CHAT_YELLOW_COLOR  RGB(255,255,0)
#define CHAT_PINK_COLOR  RGB(255,0,255)
#define CHAT_CYAN_COLOR  RGB(0,255,255)
#define CHAT_GRAY_COLOR  RGB(128,128,128)
#define CHAT_LIGHT_GREEN_COLOR  RGB(128,255,128)
#define CHAT_LIGHT_BLUE_COLOR  RGB(128,128,255)
#define CHAT_ORINGE_COLOR  RGB(255,102,0)
#define CHAT_LOWGRAY_COLOR  RGB(64,64,64)

//////////////////////////////////////////////////////////////////////////
//
BOOL g_bChatWndMove = false;	//窗口移动
BOOL g_bChatWndSize = false;	//窗口大小变化开始
BOOL g_bMouseShow = false;		//鼠标状态
BOOL g_bPause = false;
BOOL g_bFocus =	false;
BOOL g_bScrn = false;
BOOL g_bVisable = false;

SLONG g_nChatWndNCHeight = 0;	//窗口边框高
SLONG g_nChatWndNCWidth = 0;	//窗口边框宽
SLONG g_nChatWndCaptionHeight = 0;	//标题栏高
SLONG g_nChatCurrBack = 0;	//窗口边框高
//////////////////////////////////////////////////////////////////////////
//
ChatWindow::ChatWindow()
{
	m_hWnd = 0;
	
	m_nScrnX = 0;
	m_nScrnY = 0;
	
	m_nWndWidth = 0;

	m_nSaveTime = 0;
	
	m_hdc = 0;	
	m_hdcMe = 0;
	m_hdcTitle = 0;	

	m_bSelectPerson = false;

	memset(&m_ChatSave, 0, sizeof(m_ChatSave));

	memset(m_nTitleList,  0, sizeof(m_nTitleList));
	
	m_brBitmap= NULL;
	
	memset(&m_brBM, 0,sizeof(m_brBM));
}

ChatWindow::~ChatWindow()
{
	DeleteFont(m_hfontNormal);	

	DeleteDC(m_hdc);
	DeleteDC(m_hdcTitle);

	ReleaseDC(m_hWnd, m_hdcMe);
	
	DeleteObject(m_brBitmap);
}


//-----------------------------------------------------------------------------
// Name: WinInit()
// Desc: Init the window
//-----------------------------------------------------------------------------
HRESULT ChatWindow::WinInit( HINSTANCE hInst)
{
    WNDCLASSEX wc;

    // Register the Window Class
	memset(&wc,0,sizeof(wc));
    wc.cbSize        = sizeof(wc);
    wc.lpszClassName = TEXT("WindowedMode");
    wc.lpfnWndProc   = MainWndProc;
    wc.style         = CS_VREDRAW | CS_HREDRAW|CS_DBLCLKS;//|WS_CLIPSIBLINGS ;//|WS_CLIPCHILDREN;//CS_VREDRAW | CS_HREDRAW
    wc.hInstance     = hInst;
    wc.hCursor       = LoadCursorFromFile("system/chat/chat_curor.ani");//LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;	

    if(  RegisterClassEx( &wc ) == 0 )
        return E_FAIL;

    // Load keyboard accelerators
    // Calculate the proper size for the window given a client
		
	//////////////////////////////////////////////////////////////////////////
	//得到屏幕分辨率，建立窗口
	setCurrentScrn();

	GetWindowRect(g_hDDWnd, &m_rtMain);

    g_nChatWndNCWidth    = GetSystemMetrics( SM_CXSIZEFRAME );
    g_nChatWndNCHeight   = GetSystemMetrics( SM_CYSIZEFRAME );
    g_nChatWndCaptionHeight = GetSystemMetrics( SM_CYCAPTION );
    SLONG dwWindowWidth   =  m_nScrnX - m_rtMain.right - (g_nChatWndNCWidth * 2);
    SLONG dwWindowHeight  = WINDOW_HEIGHT + (g_nChatWndNCHeight * 2) + 
                            g_nChatWndCaptionHeight;

	if (dwWindowWidth > WINDOW_DEF_WIDTH + g_nChatWndNCWidth * 2) 
	{
		dwWindowWidth = WINDOW_DEF_WIDTH + g_nChatWndNCWidth * 2;
	}
    // Create and show the main window
    DWORD dwStyle = WS_OVERLAPPED|WS_CAPTION|WS_THICKFRAME|WS_CLIPSIBLINGS  ;
    m_hWnd = CreateWindowEx( 0, TEXT("WindowedMode"), TEXT(""),
                           dwStyle, m_rtMain.right, m_rtMain.top,
  	                       dwWindowWidth, dwWindowHeight, g_hDDWnd, NULL, hInst, NULL );
    if( m_hWnd == NULL )
    	return E_FAIL;	

 	GetWindowRect(m_hWnd, &m_rtChild);
	GetClientRect(m_hWnd, &m_rtClient);

	ShowWindow( m_hWnd,  SW_HIDE);

	if(initClass())
	{
		return E_FAIL;
	}

	if(initTitle())
	{
		return E_FAIL;
	}

//	ShowWindow( m_hWnd,  SW_SHOW);
	
//	UpdateWindow( m_hWnd );

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: MainWndProc()
// Desc: The main window procedure
//-----------------------------------------------------------------------------
LRESULT CALLBACK MainWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch (msg)
    {
        case WM_GETMINMAXINFO:
            {
                // Don't allow resizing in windowed mode.  
                // Fix the size of the window to 640x480 (client size)

                MINMAXINFO* pMinMax = (MINMAXINFO*) lParam;

                pMinMax->ptMinTrackSize.y = WINDOW_HEIGHT + g_nChatWndNCHeight * 2 + 
                                            g_nChatWndCaptionHeight;
                pMinMax->ptMaxTrackSize.y = pMinMax->ptMinTrackSize.y;

				pMinMax->ptMaxTrackSize.x  = WINDOW_MAX_WIDTH + (g_nChatWndNCWidth * 2);

            }
            return 0L;
			break;

        case WM_MOVE:
            return 0L;
			break;


        case WM_EXITMENULOOP:
            // Ignore time spent in menu
			//check bob
            break;
		case WM_ERASEBKGND:
			return 0;			
			break;

		case WM_ENTERSIZEMOVE:
			g_bChatWndSize = true;
			break;

        case WM_SIZE:
			g_sChatWindow.updateChatWindow();
            break;			
			
		case WM_DISPLAYCHANGE:
			g_sChatWindow.setCurrentScrn();
			break;
            
        case WM_DESTROY:
            // Cleanup and close the app
            PostQuitMessage( 0 );
            return 0L;

  		//鼠标消息
		case WM_LBUTTONDBLCLK:
			g_sChatWindow.modiBackBmp();
			break;

		case WM_RBUTTONDOWN:
			g_sChatWindow.selectPerson();
			break;			

		//屏蔽键盘消息
        case WM_KEYDOWN:  
        case WM_KEYUP: 
        case WM_CHAR: 
        case WM_DEADCHAR: 
        case WM_SYSKEYDOWN: 
        case WM_SYSKEYUP: 
        case WM_SYSCHAR:  
        case WM_SYSDEADCHAR: 
        case WM_KEYLAST: 
			PostMessage(g_hDDWnd, msg, wParam, lParam);
			SetFocus(g_hDDWnd);
			return 0;
			break;

		case WM_PAINT:
			{
				g_sChatWindow.draw();
			}
			break;

		case WM_SHOWWINDOW:		 
			{
				if (wParam != SW_HIDE && (g_bScrn || !g_bVisable))
				{
					return 0;
				}					
			}
			break;
			
		case WM_NCHITTEST:
			if(g_bChatWndMove)
			{
				g_sChatWindow.setChatWindowPos(1);
				g_bChatWndMove = false;
			}
			break;

		case WM_NCLBUTTONDOWN:
			{
				g_bChatWndMove = true;				
			}
			break;
    }
 
    return DefWindowProc(hWnd, msg, wParam, lParam);
}


//////////////////////////////////////////////////////////////////////////
void ChatWindow::setChatWindowPos(int  nflag)
{
	//////////////////////////////////////////////////////////////////////////
	//当前主窗口坐标

	if (!g_bVisable && !nflag) 
	{
		return;
	}

	RECT rtmain = {0};

	GetWindowRect(g_hDDWnd, &rtmain);

	if (!nflag && !memcmp(&rtmain, &m_rtMain, sizeof(rtmain))) 
	{
		return;
	}

	GetWindowRect(m_hWnd, &m_rtChild);
	
	memcpy(&m_rtMain, &rtmain, sizeof(rtmain));

	m_rtChild.bottom += rtmain.top - m_rtChild.top;
	
	m_rtChild.top = rtmain.top;

	if (rtmain.left < WINDOW_MIN_WIDTH+(g_nChatWndNCWidth*2) || (rtmain.right < m_nScrnX - WINDOW_MIN_WIDTH && m_rtChild.left > m_nScrnX/2)) 
	{
		int right = m_rtChild.right - m_rtChild.left + rtmain.right;

		m_rtChild.right =  right > m_nScrnX	? m_nScrnX : right;

		if (right > m_nScrnX) 
		{
			g_bChatWndSize = true;
		}
				
		m_rtChild.left = rtmain.right;	
	}
	else
	{
		int left = m_rtMain.left - m_rtChild.right + m_rtChild.left; 
		m_rtChild.left = left < 0 ? 0 : left;

		if (left < 0) 
		{
			g_bChatWndSize = true;
		}

		m_rtChild.right = m_rtMain.left;		
	}
	

	SetWindowPos(m_hWnd, NULL, m_rtChild.left, m_rtChild.top, 
		m_rtChild.right-m_rtChild.left,m_rtChild.bottom- m_rtChild.top,SWP_NOACTIVATE);

	updateChatWindow();	
}


void ChatWindow::updateChatWindow(void)
{
	GetClientRect( m_hWnd, &m_rtClient );
	GetWindowRect(m_hWnd, &m_rtChild);
}

void ChatWindow::draw(void)
{	
	BitBlt(m_hdcMe, m_rtClient.left,m_rtClient.top,m_rtClient.right,m_rtClient.bottom,
		m_hdc,0,0,SRCCOPY);
}

BOOL ChatWindow::setChatWindowState(BOOL bVisable)
{
	g_bVisable = bVisable;

	if (bVisable)
	{
		if (g_bScrn) 
		{
			ShowWindow( m_hWnd,  SW_HIDE);//nCmdShow
		}
		else
		{
			memset(&m_ChatSave, 0, sizeof(m_ChatSave));

			setChatWindowPos(1);		

			ShowWindow( m_hWnd,  SW_SHOWNOACTIVATE);//nCmdShow
		}		
	}
	else
	{
		ShowWindow( m_hWnd,  SW_HIDE);//nCmdShow
	}	

	UpdateWindow( m_hWnd );	
	return bVisable;
}


void ChatWindow::setCurrentScrn(void)
{
	//////////////////////////////////////////////////////////////////////////
	//当前分辨率
	HDC       hScrDC;     
	
	// 屏幕分辨率
	
	//为屏幕创建设备描述表
	hScrDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	// 获得当前屏幕分辨率
	m_nScrnX = GetDeviceCaps(hScrDC, HORZRES);
	m_nScrnY = GetDeviceCaps(hScrDC, VERTRES);

	DeleteDC(hScrDC);

	if (m_nScrnX < 900)
	{
		g_bScrn = true;
	}
	else
	{
		g_bScrn = false;
	}

	setChatWindowState(g_bVisable);
}

BOOL ChatWindow::preDraw(HWND hwnd)
{
	if (!g_bVisable) 
	{
	}
	else if(g_bScrn)
	{
	}
	else if(hwnd && !g_bFocus)
	{
		g_bFocus = true;
		draw();
	}
	else if(!hwnd && g_bFocus)
	{
		g_bFocus = false;
	}	
	else if (game_now_time < m_nSaveTime + DRAW_MIN_TIME)  
	{
	}
	else 
	{
		m_nSaveTime = game_now_time;
		return true;
	}
	
	return false;
}

HRESULT ChatWindow::initClass(void)
{
	m_brBitmap = (HBITMAP)LoadImage(0, "system/chat/chat_back0.bmp",IMAGE_BITMAP,
		0,0,LR_CREATEDIBSECTION|LR_LOADFROMFILE);
	
	if (!m_brBitmap)
	{
		return E_FAIL;
	}
	
	GetObject(m_brBitmap,sizeof(m_brBM),&m_brBM);
	
	if (m_brBM.bmBitsPixel != 24) 
	{
		DeleteObject(m_brBitmap);
		return E_FAIL;		
	}
	
	HBRUSH hbrush = CreatePatternBrush(m_brBitmap);
	
	//////////////////////////////////////////////////////////////////////////
	//建立hdc
	
	m_hdcMe = GetDC(m_hWnd);
	
	SetBkMode(m_hdcMe, TRANSPARENT); 
	
	HDC       hScrDC;
	
	char fontname[100] = {0};
	DWORD fonttype = 0;
	SLONG i = 0;

	//check
	BYTE backWidth[BACK_MAX_PIXEL_WIDTH*3] = {0};
	
	//为屏幕创建设备描述表
	hScrDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	
	HBITMAP pOldBit = CreateCompatibleBitmap(hScrDC,WINDOW_MAX_WIDTH,WINDOW_HEIGHT);
	
	m_hdc = CreateCompatibleDC(NULL);
	
	SelectObject(m_hdc, pOldBit);
	
	DeleteObject(pOldBit);
	
	SetBkMode(m_hdc, TRANSPARENT);
	
	RECT rt  = {0 , 0, WINDOW_MAX_WIDTH, WINDOW_HEIGHT};
	FillRect(m_hdc, &rt, hbrush);

	DeleteObject(hbrush);
	
	//title
	
	pOldBit = CreateCompatibleBitmap(hScrDC,CHAT_TITLE_WIDTH+WINDOW_MAX_WIDTH,WINDOW_HEIGHT);
	
	m_hdcTitle = CreateCompatibleDC(NULL);
	
	SelectObject(m_hdcTitle, pOldBit);
	
	DeleteObject(pOldBit);
	
	SetBkMode(m_hdcTitle, TRANSPARENT);	
	
	SetRect(&rt, CHAT_TITLE_WIDTH,0, CHAT_TITLE_WIDTH+WINDOW_MAX_WIDTH,WINDOW_HEIGHT);
	
	BitBlt(m_hdcTitle, CHAT_TITLE_WIDTH,0,WINDOW_MAX_WIDTH+CHAT_TITLE_WIDTH,WINDOW_HEIGHT,
		m_hdc,0,0,SRCCOPY);		
	
	DeleteDC(hScrDC);
	
	//////////////////////////////////////////////////////////////////////////
	//创建字体
	
	if(is_gb_windows())
	{
		fonttype = GB2312_CHARSET;
		strcpy(fontname, GAME_FONT_NAME);
	}
	else 
	{
		fonttype = CHINESEBIG5_CHARSET;
		strcpy(fontname, GAME_FONT_NAME); //check bob没有新的字体要修改
	}
	
	HFONT hfont = CreateFont(
		12,
		6, 
		0, 
		0,
		FW_NORMAL,
		FALSE, 
		FALSE, 
		FALSE,
		fonttype, 
		OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		FIXED_PITCH|FF_MODERN , 
		(const char *)fontname ) ;
	
	m_hfontNormal = CreateFont(
		16,
		8, 
		0, 
		0,
		FW_NORMAL,
		FALSE, 
		FALSE, 
		FALSE,
		fonttype, 
		OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		FIXED_PITCH|FF_MODERN , 
		(const char *)fontname ) ;
	
	SelectObject(m_hdc, m_hfontNormal);
	
	SelectObject(m_hdcTitle, hfont);

	DeleteFont(hfont);

	//BMP倒转
	for(i = 0; i < (m_brBM.bmHeight/2); i++)
	{
		int nWidthStart = i*m_brBM.bmWidthBytes;
		int nWidthEnd = (m_brBM.bmHeight-1-i)*m_brBM.bmWidthBytes;
		memcpy(backWidth, (BYTE*)m_brBM.bmBits+nWidthStart, m_brBM.bmWidthBytes);
		memcpy((BYTE*)m_brBM.bmBits+nWidthStart, (BYTE*)m_brBM.bmBits+nWidthEnd, m_brBM.bmWidthBytes);
		memcpy((BYTE*)m_brBM.bmBits+nWidthEnd, backWidth, m_brBM.bmWidthBytes);
	}	
	
	return S_OK;
}


HRESULT ChatWindow::initTitle(void)
{
	SLONG word_color = 0;

	SLONG i;
	UCHR title[32];
	
	SLONG display_y = 0;	

//////////////////////////////////////////////////////////////////////////
//颜色刷子
	
	HBRUSH hbrushWhite = CreateSolidBrush(CHAT_WHITE_COLOR);
	HBRUSH hbrushBlack = CreateSolidBrush(CHAT_BLACK_COLOR);
	HBRUSH hbrushRed = CreateSolidBrush(CHAT_RED_COLOR);
	HBRUSH hbrushGreen = CreateSolidBrush(CHAT_GREEN_COLOR);
	HBRUSH hbrushBlue = CreateSolidBrush(CHAT_BLUE_COLOR);
	HBRUSH hbrushYellow = CreateSolidBrush(CHAT_YELLOW_COLOR);
	HBRUSH hbrushPink = CreateSolidBrush(CHAT_PINK_COLOR);
	HBRUSH hbrushCyan = CreateSolidBrush(CHAT_CYAN_COLOR);
	HBRUSH hbrushGray = CreateSolidBrush(CHAT_GRAY_COLOR);
	HBRUSH hbrushLightGreen = CreateSolidBrush(CHAT_LIGHT_GREEN_COLOR);
	HBRUSH hbrushLightBlue= CreateSolidBrush(CHAT_LIGHT_BLUE_COLOR);
	HBRUSH hbrushOringe = CreateSolidBrush(CHAT_ORINGE_COLOR);
	HBRUSH hbrushLowGray= CreateSolidBrush(CHAT_LOWGRAY_COLOR);	

	//建立标题	
	for(i = 0; i < CHAT_MAX_TITLE; i ++)
	{		
		switch(i)
		{
		case CHAT_CHANNEL_SCREEN:			// 当前频道
			strcpy((char *)title,(char *)MSG_CHANNEL_NAME_SCREEN);
			word_color=CHAT_LIGHT_GREEN_COLOR;
			break;
		case CHAT_CHANNEL_TEAM:				// 队伍
			strcpy((char *)title,(char *)MSG_CHANNEL_NAME_TEAM);
			word_color=CHAT_ORINGE_COLOR;
			break;
		case CHAT_CHANNEL_PERSON:			// 私聊
			strcpy((char *)title,(char *)MSG_CHANNEL_NAME_PERSON);
			word_color=CHAT_WHITE_COLOR;
			break;
		case CHAT_CHANNEL_GROUP:			// 公会
			strcpy((char *)title,(char *)MSG_CHANNEL_NAME_GROUP);
			word_color=CHAT_LIGHT_BLUE_COLOR;
			break;
		case CHAT_CHANNEL_SELLBUY:			// 经济
			strcpy((char *)title,(char *)MSG_CHANNEL_NAME_SELLBUY);
			word_color=CHAT_YELLOW_COLOR;
			break;
		case CHAT_CHANNEL_WORLD:			// 世界
			strcpy((char *)title,(char *)MSG_CHANNEL_NAME_WORLD);
			word_color=CHAT_PINK_COLOR;
			break;
		case CHAT_CHANNEL_SYSTEM:			// 系统说话
			strcpy((char *)title,(char *)MSG_CHANNEL_NAME_SYSTEM);
			word_color=CHAT_RED_COLOR;
			break;
		case CHAT_CHANNEL_MESSAGE:			// 随机讯息
			strcpy((char *)title,(char *)MSG_CHANNEL_NAME_MESSAGE);
			word_color=CHAT_RED_COLOR;
			break;
		}

		RECT rt = {0};

		display_y = 19*i;

		m_nTitleList[i] = display_y;
		
		SetRect(&rt, 1,display_y+1, 1 + 37,17 + display_y+1);
		FillRect(m_hdcTitle, &rt, hbrushBlack);
		
		SetRect(&rt, 2,display_y+2, 2 + 34,14 + display_y+2);
		FillRect(m_hdcTitle, &rt, hbrushLowGray);
		
		SetRect(&rt, 0,display_y, 36,16 + display_y);
		FrameRect(m_hdcTitle, &rt, hbrushWhite);

		int len = strlen((char*)title);
	
		SetRect(&rt,5+1,display_y+3+1,30+5+1, 12+ display_y+3+1);
		SetTextColor(m_hdcTitle, CHAT_BLACK_COLOR);		
		DrawText(m_hdcTitle,(char*)title,len,&rt, DT_LEFT|DT_TOP);		
		
		SetRect(&rt,5,display_y+3, 30+5, 12+ display_y+3);
		SetTextColor(m_hdcTitle, word_color);
		DrawText(m_hdcTitle,(char*)title,len,&rt, DT_LEFT|DT_TOP);		
	}

	//delete
	DeleteObject(hbrushWhite);
	DeleteObject(hbrushBlack);
	DeleteObject(hbrushRed);
	DeleteObject(hbrushGreen);
	DeleteObject(hbrushBlue);
	DeleteObject(hbrushYellow);
	DeleteObject(hbrushPink);
	DeleteObject(hbrushCyan);
	DeleteObject(hbrushGray);
	DeleteObject(hbrushLightGreen);
	DeleteObject(hbrushLightBlue);
	DeleteObject(hbrushOringe);
	DeleteObject(hbrushLowGray);

	return	 S_OK;	
}

void ChatWindow::setPersonObject(const long& display_y, const POINT& pt)
{
	SLONG text_yl = 0;
	int i = 0;

	m_bSelectPerson = false;
	
	if (!m_ChatSave.nFlag) 
	{
		return;
	}
	
	text_yl = display_y;
	
	int nCursorY = pt.y-m_rtChild.top-g_nChatWndNCHeight-g_nChatWndCaptionHeight;
	
	for(i = 0; i < (SLONG)m_ChatSave.nCount; i++)
	{
		if (nCursorY < text_yl && nCursorY > text_yl - m_ChatSave.text[i].ntextHeight) 
		{
			int id = m_ChatSave.text[i].ntextid;
			
			ULONG channel = chat_data_buffer.record[id].channel;
			ULONG userid = chat_data_buffer.record[id].user_id;
			
			if(channel != CHAT_CHANNEL_SYSTEM && channel != CHAT_CHANNEL_MESSAGE 
				&& channel != CHAT_CHANNEL_NONE && 
				userid != system_control.control_user_id)
			{
				system_control.person_npc_id= userid;

				strcpy((char *)system_control.person_name, (char*)chat_data_buffer.record[id].nickname);
				
				sprintf((char *)print_rec,MSG_SET_PERSON_ID,chat_data_buffer.record[id].nickname,userid);
				push_chat_data(CHAT_CHANNEL_MESSAGE,0,(UCHR *)"",print_rec);					
			}
			
			return;
		}
		
		text_yl -= m_ChatSave.text[i].ntextHeight;
	}
}

void ChatWindow::setDrawState(HWND hwnd, const POINT& pt)
{	
	setChatWindowPos(0);	

	if (!g_bMouseShow && hwnd && PtInRect(&m_rtChild, pt)) 
	{
		g_bMouseShow = true;
		show_mouse(SHOW_WINDOW_CURSOR);
	}
	else if(g_bMouseShow && hwnd && PtInRect(&m_rtMain, pt))
	{
		g_bMouseShow = false;
		show_mouse(SHOW_IMAGE_CURSOR);
	}
	
	if (game_control.chat_pause_flag != g_bPause) 
	{
		if (!game_control.chat_pause_flag) //暂定后重新保存
		{
			memset(&m_ChatSave, 0, sizeof(m_ChatSave));
		}
		
		g_bPause = !g_bPause;
	}	
}

HRESULT ChatWindow::drawChatWindow(void)
{
	HWND hwnd = GetFocus( );
	
	if(!preDraw(hwnd)) return S_OK;

	POINT ptCursorPos = {0};
	
	GetCursorPos(&ptCursorPos);	
	
	setDrawState(hwnd, ptCursorPos); 

	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	SLONG i;
	SLONG word_count;
	SLONG main_pass;
	SLONG idx;
	SLONG text_yl;
	SLONG title_offset;
	SLONG font_height;
	UCHR text[1024];
	UCHR inst[1024];
	RECT window_rect;
	SLONG display_x,display_y;
	bool show_title;
	bool show_name;	
	
	window_xl=m_rtClient.right;
	window_yl=m_rtClient.bottom; 
	window_y=m_rtClient.top;

	int nTitleHeight = 0;

	HGDIOBJ holdobj =  SelectObject(m_hdc, m_hfontNormal);
	
	
	if(g_bChatWndSize) //改变窗口大小时重画内存hdc
	{
		BitBlt(m_hdc, 0,0,WINDOW_MAX_WIDTH,WINDOW_HEIGHT,
			m_hdcTitle,CHAT_TITLE_WIDTH,0,SRCCOPY);	
	}
	else
	{
		BitBlt(m_hdc,0,0,m_rtClient.right,m_rtClient.bottom,
			m_hdcTitle,CHAT_TITLE_WIDTH,0,SRCCOPY);	
	}

	window_yl=window_yl-25;
	window_x=CHAT_TITLE_WIDTH;
	window_xl=window_xl-38;
	
	window_rect.left=window_x;
	window_rect.top=window_y;
	window_rect.right=window_x+window_xl;
	window_rect.bottom=window_y+window_yl;	
	
	word_count=0;
	main_pass=0;
	text_yl=0;
	idx=chat_data_buffer.display_index-1;

	display_x=window_x;
	display_y=window_y+window_yl;

	if(chat_data_buffer.bottom_index==chat_data_buffer.display_index)
		main_pass=1;			// first time enter

	CHAT_WND_SAVE chatSave = {0};

	if (!main_pass) 
	{
		chatSave.nFlag = 1;

		if ( idx == (SLONG)m_ChatSave.nStart && m_ChatSave.nFlag && !g_bChatWndSize) 
		{
			drawChat();

			main_pass = 2;	//main_pass=2 没有修改m_chatSave
		}
	}

	if(m_bSelectPerson)
	{
		setPersonObject(display_y,ptCursorPos);
	}

	while(main_pass==0)
	{
		if(idx<0)
			idx=MAX_CHAT_RECORD-1;
		if(idx==chat_data_buffer.bottom_index)
			main_pass=1;

		window_rect.left=window_x;
		window_rect.top=window_y;
		window_rect.right=window_x+window_xl;
		window_rect.bottom=window_y+window_yl;	
		
		//是否是已保存的聊天
		if (m_ChatSave.nFlag && ( ( (SLONG)m_ChatSave.nStart >= idx && (SLONG)m_ChatSave.nEnd <= idx) ||
			(m_ChatSave.nStart < m_ChatSave.nEnd && ((SLONG)m_ChatSave.nStart >= idx ||
				(SLONG)m_ChatSave.nEnd <= idx) ) ) ) 
		{
			for(i = 0; i< (SLONG)m_ChatSave.nCount; i++)
			{
				if (idx == m_ChatSave.text[i].ntextid) break;
			}
			
			if (g_bChatWndSize) //窗口改变时重设高度
			{
				m_ChatSave.text[i].ntextTitleOffset = title_offset = text_out_data.g_pChat->insertString((char *)m_ChatSave.text[i].sztext,window_rect,false,m_hdc,true);	
				
				text_yl+= m_ChatSave.text[i].ntextHeight = window_rect.bottom-window_rect.top;
			}
			else
			{
				title_offset = m_ChatSave.text[i].ntextTitleOffset;
				text_yl+= font_height = m_ChatSave.text[i].ntextHeight;
			}			
			
			if(text_yl>=window_yl)
			{
				main_pass = 1;
				continue;
			}
			else //画一条聊天数据
			{				
				display_y=display_y-m_ChatSave.text[i].ntextHeight;

				memcpy(&chatSave.text[chatSave.nCount++],&m_ChatSave.text[i],sizeof(CHAT_WND_TEXT));

				if(m_ChatSave.text[i].nTitle != -1)
				{
					BitBlt(m_hdc, 0,display_y+title_offset,38,18,
						m_hdcTitle,0,m_ChatSave.text[i].nTitle,SRCCOPY);	
				}
				
				text_out_data.g_pChat->updateOuterString(display_x,display_y,window_xl,window_yl,(char*)m_ChatSave.text[i].sztext ,m_hdc);
				
				idx--;

				continue;
			}				
		}	
		
		strcpy((char *)inst,"");
		show_title=true;
		show_name=true;

		nTitleHeight = -1;

		switch(chat_data_buffer.record[idx].channel) //设置标题等数据
		{
		case CHAT_CHANNEL_SCREEN:			// 当前频道
			nTitleHeight = m_nTitleList[0];
			break;
		case CHAT_CHANNEL_TEAM:				// 队伍
			nTitleHeight = m_nTitleList[1];
			break;
		case CHAT_CHANNEL_PERSON:			// 私聊
			nTitleHeight = m_nTitleList[2];
			if(chat_data_buffer.record[idx].user_id==system_control.control_user_id)
				sprintf((char *)inst,MSG_PERSON_SEND,chat_data_buffer.record[idx].nickname);
			else
				sprintf((char *)inst,MSG_PERSON_RECEIVE,chat_data_buffer.record[idx].nickname);
			show_name=false;			
			break;
		case CHAT_CHANNEL_GROUP:			// 公会
			nTitleHeight = m_nTitleList[3];
			break;
		case CHAT_CHANNEL_SELLBUY:			// 经济
			nTitleHeight = m_nTitleList[4];
			break;
		case CHAT_CHANNEL_WORLD:			// 世界
			nTitleHeight = m_nTitleList[5];
			break;
		case CHAT_CHANNEL_SYSTEM:			// 系统说话
			nTitleHeight = m_nTitleList[6];
			show_name=false;
			break;
		case CHAT_CHANNEL_MESSAGE:			// 随机讯息
			nTitleHeight = m_nTitleList[7];
			show_name=false;
			break;
		default:
			show_title=false;
			show_name=false;
			break;
		}		
		
		if(show_name)
			sprintf((char *)text,"#C00FFFF%s：#N%s",chat_data_buffer.record[idx].nickname,chat_data_buffer.record[idx].text);
		else
			sprintf((char *)text,"%s%s",inst,chat_data_buffer.record[idx].text);
		
		title_offset=text_out_data.g_pChat->insertString((char *)text,window_rect,false,m_hdc,true);	

		text_yl+= font_height = window_rect.bottom-window_rect.top;

		display_y=display_y-font_height;

		if(text_yl>=window_yl)
		{
			main_pass=1;
		}
		else	//画一条聊天数据
		{
			chatSave.text[chatSave.nCount].ntextHeight = font_height;
			chatSave.text[chatSave.nCount].ntextid = idx;
			chatSave.text[chatSave.nCount].ntextTitleOffset = title_offset;
			chatSave.text[chatSave.nCount].nTitle = nTitleHeight;
			strcpy(chatSave.text[chatSave.nCount].sztext, (char*)text);
			chatSave.nCount++;

			if(show_title)
			{
				BitBlt(m_hdc, 0,display_y+title_offset,38,18,
					m_hdcTitle,0,nTitleHeight,SRCCOPY);	
			}
			
			text_out_data.g_pChat->updateOuterString(display_x,display_y,window_xl,window_yl,(char*)text,m_hdc);
			
			idx--;
		}		
	}
	OutputDebugString("before");
	text_out_data.g_pChat->updateframe(m_hdc, &m_brBM);
	OutputDebugString("after");
	//保存聊天数据

	if (chatSave.nFlag && chatSave.nCount) 
	{
		chatSave.nStart = chatSave.text[0].ntextid;
		chatSave.nEnd = chatSave.text[chatSave.nCount-1].ntextid;

		memcpy(&m_ChatSave, &chatSave, sizeof(m_ChatSave));
	}
	else if(main_pass!= 2)
	{
		memset(&m_ChatSave, 0, sizeof(m_ChatSave));
	}

	draw(); 

	g_bChatWndSize = false;

	SelectObject(m_hdc, holdobj);

	return S_OK;
}


void ChatWindow::drawChat(void)
{
	if (!m_ChatSave.nFlag)
		return;	

	SLONG window_x,window_y;
	SLONG window_xl,window_yl;
	SLONG i;

	SLONG display_y;
	
	window_xl=m_rtClient.right-38 ;
	window_yl=m_rtClient.bottom-25; 
	window_x=CHAT_TITLE_WIDTH;
	window_y=m_rtClient.top;

	display_y=window_y+window_yl;
	
	for(i = 0; i < (SLONG)m_ChatSave.nCount; i ++)
	{
		display_y=display_y-m_ChatSave.text[i].ntextHeight;
		
		if(m_ChatSave.text[i].nTitle != -1)
		{
			BitBlt(m_hdc, 0,display_y+m_ChatSave.text[i].ntextTitleOffset,38,18,
				m_hdcTitle,0,m_ChatSave.text[i].nTitle,SRCCOPY);	
		}
		
		text_out_data.g_pChat->updateOuterString(window_x,display_y,window_xl,window_yl,(char*)m_ChatSave.text[i].sztext ,m_hdc);
	}
}

HRESULT ChatWindow::modiBackBmp(void)
{	
	char sztext[256] = {0};

	//check
	BYTE backWidth[BACK_MAX_PIXEL_WIDTH*3] = {0};

	BITMAP btbm = {0};

	int i = 0;

	g_bVisable = false;

	g_nChatCurrBack = (g_nChatCurrBack+1)%CHAT_MAX_BACK_NUM;

	sprintf(sztext,"system/chat/chat_back%d.bmp",g_nChatCurrBack);	

	HBITMAP pBit = (HBITMAP)LoadImage(0, sztext,IMAGE_BITMAP,
		0,0,LR_CREATEDIBSECTION|LR_LOADFROMFILE);	

	if (!pBit)
	{
		g_bVisable = true;

		display_message((UCHR*)"bmp erro", true);

		return E_FAIL;
	}
	
	GetObject(pBit,sizeof(btbm),&btbm);
	
	if (btbm.bmBitsPixel != 24) 
	{
		g_bVisable = true;
		
		display_message((UCHR*)"bmp bitsPixel erro", true);		
		
		DeleteObject(pBit);
		
		return E_FAIL;		
	}

	memcpy(&m_brBM, &btbm, sizeof(m_brBM));

	DeleteObject(m_brBitmap);	

	m_brBitmap = pBit;

	HBRUSH hbrush = CreatePatternBrush(m_brBitmap);	
	
	RECT rt  = {0 , 0, WINDOW_MAX_WIDTH, WINDOW_HEIGHT};
	FillRect(m_hdc, &rt, hbrush);

	DeleteObject(hbrush);	

	OffsetRect(&rt,CHAT_TITLE_WIDTH,0);	
	
	BitBlt(m_hdcTitle, CHAT_TITLE_WIDTH,0,WINDOW_MAX_WIDTH+CHAT_TITLE_WIDTH,WINDOW_HEIGHT,
		m_hdc,0,0,SRCCOPY);		
	

	//转化bmp为正

	for(i = 0; i < (m_brBM.bmHeight/2); i++)
	{
		int nWidthStart = i*m_brBM.bmWidthBytes;
		int nWidthEnd = (m_brBM.bmHeight-1-i)*m_brBM.bmWidthBytes;
		memcpy(backWidth, (BYTE*)m_brBM.bmBits+nWidthStart, m_brBM.bmWidthBytes);
		memcpy((BYTE*)m_brBM.bmBits+nWidthStart, (BYTE*)m_brBM.bmBits+nWidthEnd, m_brBM.bmWidthBytes);
		memcpy((BYTE*)m_brBM.bmBits+nWidthEnd, backWidth, m_brBM.bmWidthBytes);
	}

	
	g_bVisable =true;

	return S_OK;
}







