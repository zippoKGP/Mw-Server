// GameServerView.cpp : implementation of the CGameServerView class
//

#include "stdafx.h"
#include "ZoneServer.h"

#include "ZoneServerDoc.h"
#include "ZoneServerView.h"
#include "MainFrm.h"
#include "OpenDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGameServerView

IMPLEMENT_DYNCREATE(CGameServerView, CFormView)

BEGIN_MESSAGE_MAP(CGameServerView, CFormView)
	//{{AFX_MSG_MAP(CGameServerView)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_LOGFILE, OnButtonLogfile)
	ON_BN_CLICKED(IDC_BUTTON_MONSTER, OnButtonMonster)
	ON_BN_CLICKED(IDC_BUTTON_OFFPLAYER, OnButtonOffplayer)
	ON_BN_CLICKED(IDC_BUTTON_DISPLAYALL, OnButtonDisplayall)
	ON_BN_CLICKED(IDC_BUTTON_PreClose, OnBUTTONPreClose)
	ON_BN_CLICKED(IDC_BUTTON_ServerClose, OnBUTTONServerClose)
	ON_BN_CLICKED(IDC_BUTTON_WorldConnect, OnBUTTONWorldConnect)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGameServerView construction/destruction

CGameServerView::CGameServerView()
	: CFormView(CGameServerView::IDD)
{
	//{{AFX_DATA_INIT(CGameServerView)
	//}}AFX_DATA_INIT
	// TODO: add construction code here
}

CGameServerView::~CGameServerView()
{
//	g_Main.Release();
}

void CGameServerView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGameServerView)
	//}}AFX_DATA_MAP
}

BOOL CGameServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CGameServerView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
}

/////////////////////////////////////////////////////////////////////////////
// CGameServerView printing

BOOL CGameServerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CGameServerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CGameServerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CGameServerView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: add customized printing code here
}

/////////////////////////////////////////////////////////////////////////////
// CGameServerView diagnostics

#ifdef _DEBUG
void CGameServerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CGameServerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CGameServerDoc* CGameServerView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGameServerDoc)));
	return (CGameServerDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGameServerView message handlers

BOOL CGameServerView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

int CGameServerView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	CGameServerDoc* pDoc = GetDocument();
	
	COpenDlg Dlg;
	Dlg.Create(IDD_DIALOG_Loading, this);

	if(!g_Main.Init())
	{
		Dlg.DestroyWindow();
		exit(0);
	}
	Dlg.DestroyWindow();

	pDoc->CreateDisplayView(this);
	pDoc->CreateSheetView(this);
	
	pDoc->m_pwndMainView = (CWnd*)this;

	return 0;
}

void CGameServerView::OnButtonDisplaymode() 
{
	// TODO: Add your control notification handler code here
	g_Main.m_GameMsg.PackingMsg(gm_interface_displaymode_change);
}

void CGameServerView::OnButtonLogfile() 
{
	// TODO: Add your control notification handler code here

//	g_Main.m_Network.NetLogFile(g_Main.m_bTurn);
}

void CGameServerView::OnButtonMonster() 
{
	// TODO: Add your control notification handler code here
	g_Main.m_GameMsg.PackingMsg(gm_interface_monster_int);
}

void CGameServerView::OnButtonDummy() 
{
	// TODO: Add your control notification handler code here
	CRect rcWnd;

	g_pDoc->m_DisplayView.GetDrawableRect(&rcWnd);
	g_Main.m_MapDisplay.InitDummy(&rcWnd);
}

void CGameServerView::OnButtonOffplayer() 
{
	// TODO: Add your control notification handler code here

	static bool bCreate = true;

//	if(bCreate)
//		g_Main.CreateOffPlayer();	

//	else
//		g_Main.DestoryOffPlayer();

	bCreate = !bCreate;
}

void CGameServerView::OnButtonCollline() 
{
	// TODO: Add your control notification handler code here

	CRect rcWnd;

	g_pDoc->m_DisplayView.GetDrawableRect(&rcWnd);
	g_Main.m_MapDisplay.InitCollLine(&rcWnd);
}

void CGameServerView::OnButtonDisplayall() 
{
	// TODO: Add your control notification handler code here

	g_Main.m_GameMsg.PackingMsg(gm_interface_display_all);
}

void CGameServerView::OnBUTTONPreClose() 
{
	// TODO: Add your control notification handler code here

	g_Main.m_GameMsg.PackingMsg(gm_interface_PreClose);
	
}

void CGameServerView::OnBUTTONServerClose() 
{
	// TODO: Add your control notification handler code here
	
	g_Main.m_GameMsg.PackingMsg(gm_interface_ServerClose);
}

void CGameServerView::OnBUTTONWorldConnect() 
{

}
