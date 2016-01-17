// PeraDeskClientView.cpp : implementation of the CPeraDeskClientView class
//

#include "stdafx.h"
#include "PeraDeskClient.h"

#include "PeraDeskClientDoc.h"
#include "PeraDeskClientView.h"
//#include "DlgJianMo.h"
//#include "DlgSvg.h"
//#include "DlgWebPage.h"
//#include "DlgHistoryMonitor.h"
//#include "DlgDistribution.h"
#include "UtilityFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPeraDeskClientView

IMPLEMENT_DYNCREATE(CPeraDeskClientView, CxViewExt)

BEGIN_MESSAGE_MAP(CPeraDeskClientView, CxViewExt)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CPeraDeskClientView construction/destruction

CPeraDeskClientView::CPeraDeskClientView()
{
	// TODO: add construction code here
}

CPeraDeskClientView::~CPeraDeskClientView()
{
}

BOOL CPeraDeskClientView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	return CxViewExt::PreCreateWindow(cs);
}

// CPeraDeskClientView drawing

void CPeraDeskClientView::OnDraw(CDC* pDC)
{
	CPeraDeskClientDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here

	CRect rc;
	GetWindowRect(&rc);
	rc.MoveToXY(0,0);
	
	CBCGPDrawManager dm (*pDC);
	dm.DrawRect( rc, g_crBarBorderIn, g_crBarBorderOut );

	if ( m_pViewData != NULL )
	{
		//if ( m_pViewData->m_strFlag.CollateNoCase( viewflagModel ) == 0 )
		//{
		//	//有时候结构树标题栏下边有条边线不绘制问题
		//	if ( theApp.m_pBarJianmoTree != NULL 
		//		&& IsWindow( theApp.m_pBarJianmoTree->GetSafeHwnd() ) )
		//	{
		//		::PostMessage( theApp.m_pBarJianmoTree->GetSafeHwnd(), WM_NCPAINT, 0, 0 );
		//	}
		//}
	}
}


// CPeraDeskClientView diagnostics

#ifdef _DEBUG
void CPeraDeskClientView::AssertValid() const
{
	CView::AssertValid();
}

void CPeraDeskClientView::Dump(CDumpContext& dc) const
{
	CxViewExt::Dump(dc);
}

CPeraDeskClientDoc* CPeraDeskClientView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPeraDeskClientDoc)));
	return (CPeraDeskClientDoc*)m_pDocument;
}
#endif //_DEBUG


// CPeraDeskClientView message handlers


int CPeraDeskClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

	if ( m_pViewData->m_strId == g_lpszViewTitleTaskModel )
	{
		Sleep(200);
		AttachWindowTestWebContainer( this->GetSafeHwnd(), m_pViewData->m_hwndInner );

		//CDlgSvg * pDlg = new CDlgSvg();
		//pDlg->m_hwndRecvMsg = theApp.m_pMainWnd->GetSafeHwnd();
		//pDlg->Create(pDlg->IDD, m_pViewData->m_pView);
		//m_pViewData->m_dwExData = reinterpret_cast< DWORD_PTR > ( pDlg );
		//pDlg->ModifyStyle(DS_3DLOOK, 0);
		//pDlg->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
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
	//else if ( m_pViewData->m_strId == g_lpszViewTitleHistoryMonitor )
	//{
	//	CDlgHistoryMonitor * pDlg = new CDlgHistoryMonitor();
	//	pDlg->Create(pDlg->IDD, m_pViewData->m_pView);
	//	m_pViewData->m_dwExData = reinterpret_cast< DWORD_PTR > ( pDlg );
	//	pDlg->ModifyStyle(DS_3DLOOK, 0);
	//	pDlg->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
	//}


	this->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);

	return 0;
}


void CPeraDeskClientView::OnSize(UINT nType, int cx, int cy)
{
	CxViewExt::OnSize(nType, cx, cy);
	// TODO: 在此处添加消息处理程序代码
}


void CPeraDeskClientView::OnDestroy()
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


BOOL CPeraDeskClientView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return TRUE;//CxViewExt::OnEraseBkgnd(pDC);
}

void CPeraDeskClientView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView )
{
	CxViewExt::OnActivateView( bActivate, pActivateView, pDeactiveView );

	if ( bActivate && m_pViewData != NULL )
	{
		if ( m_pViewData->m_strFlag.CollateNoCase( viewflagTaskModel ) == 0 )
		{
			//有时候结构树标题栏下边有条边线不绘制问题
			//if ( theApp.m_pBarJianmoTree != NULL 
			//	&& IsWindow( theApp.m_pBarJianmoTree->GetSafeHwnd() ) )
			//{
			//	::PostMessage( theApp.m_pBarJianmoTree->GetSafeHwnd(), WM_NCPAINT, 0, 0 );
			//}
		}
	}

	if ( pDeactiveView != NULL && m_pViewData != NULL )
	{
		if ( m_pViewData->m_strFlag.CollateNoCase( viewflagTaskModel ) == 0 )
		{
			//if ( theApp.m_pBarJianmoTree != NULL )
			//{
			//	theApp.m_pBarJianmoTree->m_wndGridTree.m_ToolTip2.Deactivate ();
			//}
		}
	}
}