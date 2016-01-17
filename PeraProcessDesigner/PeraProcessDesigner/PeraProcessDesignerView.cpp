// PeraProcessDesignerView.cpp : implementation of the CPeraProcessDesignerView class
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"

#include "PeraProcessDesignerDoc.h"
#include "PeraProcessDesignerView.h"
//#include "DlgJianMo.h"
#include "DlgSvg.h"
#include "DlgWebPage.h"
#include "DlgJobMgr.h"
#include "DlgDistribution.h"
#include "UtilityFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPeraProcessDesignerView

IMPLEMENT_DYNCREATE(CPeraProcessDesignerView, CxViewExt)

BEGIN_MESSAGE_MAP(CPeraProcessDesignerView, CxViewExt)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CPeraProcessDesignerView construction/destruction

CPeraProcessDesignerView::CPeraProcessDesignerView()
{
	// TODO: add construction code here
}

CPeraProcessDesignerView::~CPeraProcessDesignerView()
{
}

BOOL CPeraProcessDesignerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	return CxViewExt::PreCreateWindow(cs);
}

// CPeraProcessDesignerView drawing

void CPeraProcessDesignerView::OnDraw(CDC* pDC)
{
	CPeraProcessDesignerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here

	CRect rc;
	GetWindowRect(&rc);
	rc.MoveToXY(0,0);
	
	CBCGPDrawManager dm (*pDC);
	dm.DrawRect( rc, g_crBarBorderIn, g_crBarBorderOut );

	if ( m_pViewData != NULL )
	{
		if ( m_pViewData->m_strFlag.CollateNoCase( viewflagModel ) == 0 )
		{
			//有时候结构树标题栏下边有条边线不绘制问题
			if ( theApp.m_pBarJianmoTree != NULL 
				&& IsWindow( theApp.m_pBarJianmoTree->GetSafeHwnd() ) )
			{
				::PostMessage( theApp.m_pBarJianmoTree->GetSafeHwnd(), WM_NCPAINT, 0, 0 );
			}
		}
	}
}


// CPeraProcessDesignerView diagnostics

#ifdef _DEBUG
void CPeraProcessDesignerView::AssertValid() const
{
	CView::AssertValid();
}

void CPeraProcessDesignerView::Dump(CDumpContext& dc) const
{
	CxViewExt::Dump(dc);
}

CPeraProcessDesignerDoc* CPeraProcessDesignerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPeraProcessDesignerDoc)));
	return (CPeraProcessDesignerDoc*)m_pDocument;
}
#endif //_DEBUG


// CPeraProcessDesignerView message handlers


int CPeraProcessDesignerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CxViewExt::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	if (m_pViewData == NULL)
	{
		return 0;
	}
	if (m_pViewData->m_pView == NULL)
	{
		return 0;
	}
	if ( ! ::IsWindow( m_pViewData->m_pView->GetSafeHwnd() ) )
	{
		return 0;
	}

	if ( m_pViewData->m_strId == g_lpszViewTitleModel )
	{
		CDlgSvg * pDlg = new CDlgSvg();
		//pDlg->m_hwndRecvMsg = theApp.m_pMainWnd->GetSafeHwnd();
		pDlg->Create(pDlg->IDD, m_pViewData->m_pView);
		m_pViewData->m_dwExData = reinterpret_cast< DWORD_PTR > ( pDlg );
		pDlg->ModifyStyle(DS_3DLOOK, 0);
		pDlg->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
	}
	else if ( m_pViewData->m_strFlag == viewflagRunProcess )
	{
		Sleep(200);
		AttachWindowTestWebContainer(this->GetSafeHwnd(), m_pViewData->m_hwndInner);

		if ( theApp.m_processMgr.m_bProcessStarting && 
			g_pawRunProcess != NULL &&
			g_pawRunProcess->m_hClientWnd == m_pViewData->m_hwndInner )
		{
			m_pViewData->m_dwExData = reinterpret_cast< DWORD_PTR > ( g_pawRunProcess );
			g_pawRunProcess = NULL;
		}
//		Sleep(200);
//		AttachWindow(this->GetSafeHwnd(), m_pViewData->m_hwndInner);

// 		CWnd::FromHandle(m_pViewData->m_hwndInner)->ModifyStyle(
// 			DS_3DLOOK
// 			|DS_MODALFRAME
// 			|WS_POPUPWINDOW
// 			|WS_OVERLAPPEDWINDOW
// 			, WS_VISIBLE|WS_CHILD);
// 		CWnd::FromHandle(m_pViewData->m_hwndInner)->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
// 		::SetParent( m_pViewData->m_hwndInner, this->GetSafeHwnd() );
	}
	//else if ( m_pViewData->m_strFlag == viewflagWebPage )
	//{
	//	CDlgWebPage * pDlg = new CDlgWebPage();
	//	pDlg->Create(pDlg->IDD, m_pViewData->m_pView);
	//	m_pViewData->m_dwExData = reinterpret_cast< DWORD_PTR > ( pDlg );
	//	m_pViewData->m_strFlag = "CDlgWebPage";
	//	pDlg->ModifyStyle(DS_3DLOOK, 0);
	//	pDlg->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
	//}
	else if ( m_pViewData->m_strId == g_lpszViewTitleJobMgr )
	{
		CDlgJobMgr * pDlg = new CDlgJobMgr();
		pDlg->Create(pDlg->IDD, m_pViewData->m_pView);
		m_pViewData->m_dwExData = reinterpret_cast< DWORD_PTR > ( pDlg );
		pDlg->ModifyStyle(DS_3DLOOK, 0);
		pDlg->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
	}
	else if ( m_pViewData->m_strId == g_lpszViewTitleDistribution)
	{
		CDlgDistribution * pDlg = new CDlgDistribution();
		pDlg->Create(pDlg->IDD, m_pViewData->m_pView);
		m_pViewData->m_dwExData = reinterpret_cast< DWORD_PTR > ( pDlg );
		pDlg->ModifyStyle(DS_3DLOOK, 0);
		pDlg->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
	}

	this->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);

	return 0;
}


void CPeraProcessDesignerView::OnSize(UINT nType, int cx, int cy)
{
	CxViewExt::OnSize(nType, cx, cy);
	// TODO: 在此处添加消息处理程序代码
}


void CPeraProcessDesignerView::OnDestroy()
{
// 	if (m_pViewData != NULL)
// 	{
// 		if (m_pViewData->m_strFlag == viewflagModel)
// 		{
// 			delete CStructTreeNodeData::m_pRoot;
// 			CStructTreeNodeData::m_pRoot = NULL;
// 		}
// 	}
	CxViewExt::OnDestroy();
	// TODO: 在此处添加消息处理程序代码
}


BOOL CPeraProcessDesignerView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return TRUE;//CxViewExt::OnEraseBkgnd(pDC);
}

void CPeraProcessDesignerView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView )
{
	CxViewExt::OnActivateView( bActivate, pActivateView, pDeactiveView );

	if ( bActivate && m_pViewData != NULL )
	{
		if ( m_pViewData->m_strFlag.CollateNoCase( viewflagModel ) == 0 )
		{
			//有时候结构树标题栏下边有条边线不绘制问题
			if ( theApp.m_pBarJianmoTree != NULL 
				&& IsWindow( theApp.m_pBarJianmoTree->GetSafeHwnd() ) )
			{
				::PostMessage( theApp.m_pBarJianmoTree->GetSafeHwnd(), WM_NCPAINT, 0, 0 );
			}
		}
	}

	if ( pDeactiveView != NULL && m_pViewData != NULL )
	{
		if ( m_pViewData->m_strFlag.CollateNoCase( viewflagModel ) == 0 )
		{
			if ( theApp.m_pBarJianmoTree != NULL )
			{
				theApp.m_pBarJianmoTree->m_wndGridTree.m_ToolTip2.Deactivate ();
			}
		}
	}
}