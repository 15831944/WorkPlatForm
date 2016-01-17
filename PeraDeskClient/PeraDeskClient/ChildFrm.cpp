// ChildFrm.cpp : implementation of the CChildFrame class
//
#include "stdafx.h"
#include "PeraDeskClient.h"

#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CxChildFrameExt)

BEGIN_MESSAGE_MAP(CChildFrame, CxChildFrameExt)
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
}

CChildFrame::~CChildFrame()
{
}


BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
	if( !CxChildFrameExt::PreCreateWindow(cs) )
		return FALSE;

	cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
		/*| FWS_ADDTOTITLE */| WS_THICKFRAME;

	//cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	cs.style &= ~WS_SYSMENU;
	cs.style &= ~FWS_ADDTOTITLE;

	return TRUE;
}


// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CxChildFrameExt::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CxChildFrameExt::Dump(dc);
}

#endif //_DEBUG


// CChildFrame message handlers

void CChildFrame::ActivateFrame(int nCmdShow)
{
	CxChildFrameExt::ActivateFrame(SW_SHOWMAXIMIZED);
}


int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CxChildFrameExt::OnCreate(lpCreateStruct) == -1)
		return -1;

	CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
	if ( theApp.m_pCurViewData->m_strTitle == g_lpszViewTitleTaskModel )
	{
//		CreateJiaomoBar();
	}

	ModifyStyle(DS_3DLOOK, 0);
	//ModifyStyle(DS_3DLOOK, WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
	return 0;
}
//
//void CChildFrame::CreateJiaomoBar()
//{
//	if (theApp.m_pBarJianmoTree == NULL)
//	{
//		theApp.m_pBarJianmoTree = new CBarStructTree();
//	}
//	if (!IsWindow(theApp.m_pBarJianmoTree->GetSafeHwnd()))
//	{
//		if ( !theApp.m_pBarJianmoTree->Create (
//			_T("结构树")
//			, this
//			, CRect (0, 0, 330, 200)
//			, TRUE // Has gripper
//			, ID_BAR_JIANMO_TREE
//			, WS_CHILD | WS_VISIBLE | CBRS_LEFT
//			, CBRS_BCGP_REGULAR_TABS
//			, /*CBRS_BCGP_FLOAT |*/ /*CBRS_BCGP_CLOSE |*/ CBRS_BCGP_RESIZE | CBRS_BCGP_AUTOHIDE
//			) )
//		{
//			return;      // fail to create
//		}
//	}
//
//	//////////////////////////////////////////////////////////////////////////
//	if (theApp.m_pBarJianmoComponentLib == NULL)
//	{
//		theApp.m_pBarJianmoComponentLib = new CBarResLib();
//	}
//	if (!IsWindow(theApp.m_pBarJianmoComponentLib->GetSafeHwnd()))
//	{
//		if ( !theApp.m_pBarJianmoComponentLib->Create (
//			_T("组件资源库")
//			, this
//			, CRect (0, 0, 200, 200)
//			, TRUE // Has gripper
//			, ID_BAR_JIANMO_COMPONENTLIB
//			, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM
//			, CBRS_BCGP_REGULAR_TABS
//			, /*CBRS_BCGP_FLOAT |*/ /*CBRS_BCGP_CLOSE |*/ CBRS_BCGP_RESIZE | CBRS_BCGP_AUTOHIDE
//			) )
//		{
//			return;      // fail to create
//		}
//	}
//
//
//	ShowControlBar (theApp.m_pBarJianmoComponentLib
//		, TRUE //!(theApp.m_pBarJianmoTree->GetStyle () & WS_VISIBLE)
//		, FALSE
//		, TRUE );
//	ShowControlBar (theApp.m_pBarJianmoTree
//		, TRUE //!(theApp.m_pBarJianmoTree->GetStyle () & WS_VISIBLE)
//		, FALSE
//		, TRUE );
//
//	theApp.m_pBarJianmoComponentLib->EnableDocking(CBRS_ALIGN_ANY);
//	theApp.m_pBarJianmoTree->EnableDocking(CBRS_ALIGN_ANY);
//	EnableDocking(CBRS_ALIGN_ANY);
//	EnableAutoHideBars(CBRS_ALIGN_ANY);
//	
//	//DockControlBar( theApp.m_pBarJianmoComponentLib );
//	//DockControlBar( theApp.m_pBarJianmoTree );
//
//	//theApp.m_pBarJianmoComponentLib->SetLastPercentInContainer(30);
//
//	//ShowControlBar (theApp.m_pBarJianmoTree
//	//	, TRUE //!(theApp.m_pBarJianmoTree->GetStyle () & WS_VISIBLE)
//	//	, FALSE
//	//	, TRUE );
//	//AdjustDockingLayout();
//	
//
//	//CString strUrl;
//	//CRect rcClient;
//	//GetClientRect(&rcClient);
//
//	////任务列表////////////////////////////////////////////////////////////////////////
//	//if (theApp.m_pBarExecEnvTaskTree == NULL)
//	//{
//	//	theApp.m_pBarExecEnvTaskTree = new CGfxFloatBarWeb();
//	//	theApp.m_pBarExecEnvTaskTree->m_strWebFlag = WEBFLAG_TASKTREE;
//	//}
//	//if (!IsWindow(theApp.m_pBarExecEnvTaskTree->GetSafeHwnd()))
//	//{
//	//	//ZHD-618
//	//	if (!theApp.m_pBarExecEnvTaskTree->Create (
//	//		IDC_BAR_EXECENV_TASKTREE
//	//		, _T("任务列表")
//	//		, this
//	//		, CBRS_ALIGN_LEFT
//	//		, CBRS_ALIGN_LEFT
//	//		, CBRS_ALIGN_LEFT
//	//		, 0, 0, 290, 290
//	//		, NULL
//	//		, CBRS_BCGP_REGULAR_TABS
//	//		, CGfxFloatBar::DEFAULT_STYLE | CBRS_BCGP_FLOAT
//	//		, CBRS_BCGP_RESIZE | CBRS_BCGP_AUTOHIDE | CBRS_BCGP_FLOAT))
//	//	{
//	//		TRACE0("Failed to create dialogbar\n");
//	//		return FALSE;      // fail to create
//	//	}
//	//	theApp.m_pBarExecEnvTaskTree->Navigate(theApp.m_strTaskTreeUrl);
//	//}
//}
//
//void CChildFrame::DestroyJianmoBar()
//{
//	if (theApp.m_pBarJianmoTree != NULL)
//	{
//		if ( IsWindow( theApp.m_pBarJianmoTree->GetSafeHwnd() ) )
//			theApp.m_pBarJianmoTree->DestroyWindow();
//		delete theApp.m_pBarJianmoTree;
//		theApp.m_pBarJianmoTree = NULL;
//	}
//	if (theApp.m_pBarJianmoComponentLib != NULL)
//	{
//		if ( IsWindow( theApp.m_pBarJianmoComponentLib->GetSafeHwnd() ) )
//			theApp.m_pBarJianmoComponentLib->DestroyWindow();
//		delete theApp.m_pBarJianmoComponentLib;
//		theApp.m_pBarJianmoComponentLib = NULL;
//	}
//}

void CChildFrame::OnDestroy()
{
	if ( m_pViewData != NULL && m_pViewData->m_strTitle.CompareNoCase( g_lpszViewTitleTaskModel ) == 0 )
	{
	//	DestroyJianmoBar();
		theApp.m_strXpdlName = "";
		theApp.m_strXpdlWorkPath = "";
	}
	
	CxChildFrameExt::OnDestroy();
}


void CChildFrame::OnUpdateFrameMenu(BOOL bActive, CWnd* pActiveWnd, HMENU hMenuAlt)
{
	// TODO: 在此添加专用代码和/或调用基类

	//CxChildFrameExt::OnUpdateFrameMenu(bActive, pActiveWnd, hMenuAlt);
}
