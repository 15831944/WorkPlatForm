#include "StdAfx.h"
#include "CxViewMgr.h"
#include "MainFrm.h"
#include <afxmdichildwndex.h>
#include <afxwin.h>
#include <afxtabctrl.h>
#include "PeraProcessDesigner.h"
#include "PeraProcessDesignerUtility.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


BEGIN_MESSAGE_MAP(CxViewExt, CView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CxViewExt, CView)

CxViewExt::CxViewExt()
{
	m_pViewData = NULL;

	if (theApp.m_pCurViewData != NULL)
	{
		if (theApp.m_pCurViewData->m_pView == NULL)
		{
			theApp.m_pCurViewData->m_pView = this;
			m_pViewData = theApp.m_pCurViewData;
		}
		else
		{
			::MessageBox(GetMainWnd(), "当前 ViewData::pView 对象不为空 CWsViewExt::OnConstruction"
				, g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		}
	}
	else
	{
		::MessageBox(GetMainWnd(), "theApp.m_pCurViewData 对象为空 CWsViewExt::OnConstruction", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
	}
}

CxViewExt::~CxViewExt( void )
{
	if (theApp.m_pMainWnd == NULL) return;
	if ( !::IsWindow( theApp.m_pMainWnd->GetSafeHwnd() ) ) return;
	CMainFrame * pMainFrame = reinterpret_cast< CMainFrame * > ( theApp.m_pMainWnd );
	if (pMainFrame == NULL) return;

	if (m_pViewData != NULL)
	{
		theViewMgr.CViewSet::Remove((LPCTSTR)m_pViewData->m_strId);
		m_pViewData = NULL;
	}
}

void CxViewExt::OnDraw(CDC* /*pDC*/)
{
	CDocument* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}

BOOL CxViewExt::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~WS_VISIBLE;
	return CView::PreCreateWindow(cs);
}

int CxViewExt::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	ModifyStyle(DS_3DLOOK, 0);

	return 0;
}

void CxViewExt::OnDestroy()
{
	if (m_pViewData == NULL) goto Label_CView_Destroy;
	if ( m_pViewData->m_dwExData == NULL ) goto Label_CView_Destroy;

	//CWnd * pWnd = reinterpret_cast < CWnd * > ( m_pViewData->m_dwExData );
	//if ( pWnd == NULL ) goto Label_CView_Destroy;
	//if ( ! ::IsWindow( pWnd->GetSafeHwnd() ) ) goto Label_CView_Destroy;

	//pWnd->DestroyWindow();
	//delete pWnd;
	//m_pViewData->m_dwExData = NULL;

	if ( m_pViewData->m_strFlag == viewflagModel )
	{
		CWnd* pDlg = (CWnd*)m_pViewData->m_dwExData;
		if ( IsWindow( pDlg->GetSafeHwnd() ) )
			pDlg->DestroyWindow();
		delete pDlg; pDlg = NULL;
	}
	else if ( m_pViewData->m_strFlag == viewflagRunProcess )
	{
		CAttachWindow* pAttachWindow = (CAttachWindow*)m_pViewData->m_dwExData;
		delete pAttachWindow; pAttachWindow = NULL;
		m_pViewData->m_dwExData = NULL;
	}
	else if ( m_pViewData->m_strFlag == viewflagJobMgr )
	{
		CWnd* pDlg = (CWnd*)m_pViewData->m_dwExData;
		if ( IsWindow( pDlg->GetSafeHwnd() ) )
			pDlg->DestroyWindow();
		delete pDlg; pDlg = NULL;
	}
	else if ( m_pViewData->m_strFlag == viewflagDistribution )
	{
		CWnd* pDlg = (CWnd*)m_pViewData->m_dwExData;
		if ( IsWindow( pDlg->GetSafeHwnd() ) )
			pDlg->DestroyWindow();
		delete pDlg; pDlg = NULL;
	}
	m_pViewData->m_dwExData = NULL;

Label_CView_Destroy:
	CView::OnDestroy();
}

void CxViewExt::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if ( m_pViewData == NULL ) return;

	if ( m_pViewData->m_strFlag == viewflagRunProcess )
	{
		CWnd::FromHandle(m_pViewData->m_hwndInner)->MoveWindow(1, 1, cx - 2, cy - 2);
	}
	else if ( m_pViewData->m_strFlag == viewflagModel )
	{
		CWnd * pWnd = reinterpret_cast < CWnd * > ( m_pViewData->m_dwExData );
		if ( pWnd == NULL ) return;
		if ( ! ::IsWindow( pWnd->GetSafeHwnd() ) ) return;

		pWnd->MoveWindow(1, 1, cx - 2, cy - 2);
		pWnd->ShowWindow(SW_SHOW);
	}
	else if ( m_pViewData->m_strFlag == viewflagJobMgr )
	{
		CWnd * pWnd = reinterpret_cast < CWnd * > ( m_pViewData->m_dwExData );
		if ( pWnd == NULL ) return;
		if ( ! ::IsWindow( pWnd->GetSafeHwnd() ) ) return;

		pWnd->MoveWindow(1, 1, cx - 2, cy - 2);
		pWnd->ShowWindow(SW_SHOW);
	}
	else if ( m_pViewData->m_strFlag == viewflagDistribution )
	{
		CWnd * pWnd = reinterpret_cast < CWnd * > ( m_pViewData->m_dwExData );
		if ( pWnd == NULL ) return;
		if ( ! ::IsWindow( pWnd->GetSafeHwnd() ) ) return;

		pWnd->MoveWindow(1, 1, cx - 2, cy - 2);
		pWnd->ShowWindow(SW_SHOW);
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CxChildFrameExt, CBCGPMDIChildWnd)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CxChildFrameExt, CBCGPMDIChildWnd)

CxChildFrameExt::CxChildFrameExt( void )
{
	if (theApp.m_pCurViewData != NULL)
	{
		if (theApp.m_pCurViewData->m_pChildFrame == NULL)
		{
			theApp.m_pCurViewData->m_pChildFrame = reinterpret_cast < CChildFrame * > ( this );
			m_pViewData = theApp.m_pCurViewData;
		}
		else
		{
			::MessageBox(GetMainWnd(), "当前 ViewData::ChildFrame 对象不为空 CxChildFrameExt::OnChildFrameConstruction", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		}
	}
	else
	{
		::MessageBox(GetMainWnd(), "theApp.m_pCurViewData 对象为空 CxChildFrameExt::OnChildFrameConstruction", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
	}
}

CxChildFrameExt::~CxChildFrameExt( void )
{
	m_pViewData = NULL;
}

BOOL CxChildFrameExt::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	//cs.style &= ~FWS_ADDTOTITLE;
	cs.style &= ~WS_SYSMENU;

	if( !CBCGPMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CxDocumentExt, CDocument)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CxDocumentExt, CDocument)

CxDocumentExt::CxDocumentExt( void )
{
	if (theApp.m_pCurViewData != NULL)
	{
		if (theApp.m_pCurViewData->m_pDoc == NULL)
		{
			theApp.m_pCurViewData->m_pDoc = reinterpret_cast < CDocument * > ( this );
		}
		else
		{
			::MessageBox(GetMainWnd(),"当前 ViewData::pDoc 对象不为空 CxDocumentExt::OnDocumentConstruction", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		}
	}
	else
	{
		::MessageBox(GetMainWnd(),"theApp.m_pCurViewData 对象为空 CxDocumentExt::OnDocumentConstruction", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
	}
}

CxDocumentExt::~CxDocumentExt( void )
{

}

BOOL CxDocumentExt::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	if (theApp.m_pCurViewData == NULL) return TRUE;
	if (theApp.m_pCurViewData->m_pDoc == NULL) return TRUE;

	CxDocumentExt * pDoc = reinterpret_cast< CxDocumentExt * > ( theApp.m_pCurViewData->m_pDoc );
	if (pDoc == NULL) return TRUE;

	pDoc->SetTitle(theApp.m_pCurViewData->m_strTitle);

	return TRUE;
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CxViewMgr::CxViewMgr(void)
{
}


CxViewMgr::~CxViewMgr(void)
{
}

VIEW_DATA * CxViewMgr::NewView( LPCTSTR lpszViewName, LPCTSTR lpszFlag )
{
	CString strViewName = lpszViewName == NULL ? "" : lpszViewName;
	if (strViewName.IsEmpty()) return NULL;
	CString strFlag = lpszFlag == NULL ? "" : lpszFlag;
	if (strFlag.IsEmpty()) return NULL;
	if (theApp.m_pMainWnd == NULL) return NULL;
	if ( !::IsWindow( theApp.m_pMainWnd->GetSafeHwnd() ) ) return NULL;

	CMainFrame * pMainFrame = reinterpret_cast< CMainFrame * > ( theApp.m_pMainWnd );
	if (pMainFrame == NULL) return NULL;

	VIEW_DATA * pViewData = Find(strViewName);
	if (pViewData == NULL)
	{
		//创建新的Tab页面
		pViewData = new VIEW_DATA();
		theApp.m_pCurViewData = pViewData;
		theApp.m_pCurViewData->m_strFlag = strFlag;
		theApp.m_pCurViewData->m_strTitle = strViewName;

		theApp.m_pCurViewData->m_strId = strViewName;

		pMainFrame->SendMessage(WM_COMMAND, ID_FILE_NEW);
		if (theApp.m_pCurViewData->m_strErrInfo.IsEmpty())
		{
			Add(theApp.m_pCurViewData);
			theApp.m_pCurViewData->m_pView->ShowWindow(SW_SHOW);

			if (strViewName == g_lpszViewTitleModel)
			{
				pMainFrame->DockControlBar(theApp.m_pBarJianmoComponentLib);
				pMainFrame->DockControlBar(theApp.m_pBarJianmoTree);
			}
		}
		else
		{
			//创建过程中出现问题的处理

		}
		theApp.m_pCurViewData = NULL;
		return pViewData;
	}
	else
	{
		//激活Tab页面
		ActiveView(pViewData);
	}
	return pViewData;
}

void CxViewMgr::ActiveView( VIEW_DATA * pView )
{
	if (pView == NULL) return;
	if ( pView->m_pView == NULL ) return;
	if ( !::IsWindow( pView->m_pView->GetSafeHwnd() ) ) return;

	if (theApp.m_pMainWnd == NULL) return;
	if ( !::IsWindow( theApp.m_pMainWnd->GetSafeHwnd() ) ) return;

	CMainFrame * pMainFrame = reinterpret_cast< CMainFrame * > ( theApp.m_pMainWnd );
	if (pMainFrame == NULL) return;
	CMDIChildWndEx * pFrame = reinterpret_cast< CMDIChildWndEx * > ( pMainFrame->GetActiveFrame() );
	if (pFrame == NULL) return;
	CBCGPTabWnd * pTabCtrl = (CBCGPTabWnd * )pFrame->GetRelatedTabGroup();
	if (pTabCtrl == NULL) return;

	int nTabs = pTabCtrl->GetTabsNum();

	CString strTabLabel;
	for (int i=0; i<nTabs; i++)
	{
		pTabCtrl->GetTabLabel(i, strTabLabel);
		if (strTabLabel == pView->m_strTitle)
		{
			pTabCtrl->SetActiveTab(i);
			break;
		}
	}
}

void CxViewMgr::CloseView( VIEW_DATA * pView )
{
	if (pView == NULL) return;
	if ( pView->m_pView == NULL ) return;
	if ( !::IsWindow( pView->m_pView->GetSafeHwnd() ) ) return;

	if (theApp.m_pMainWnd == NULL) return;
	if ( !::IsWindow( theApp.m_pMainWnd->GetSafeHwnd() ) ) return;

	CMainFrame * pMainFrame = reinterpret_cast< CMainFrame * > ( theApp.m_pMainWnd );
	if (pMainFrame == NULL) return;
	CMDIChildWndEx * pFrame = reinterpret_cast< CMDIChildWndEx * > ( pMainFrame->GetActiveFrame() );
	if (pFrame == NULL) return;
	CBCGPTabWnd * pTabCtrl = (CBCGPTabWnd *)pFrame->GetRelatedTabGroup();
	if (pTabCtrl == NULL) return;

	//pMainFrame->SendMessage(WM_MDIDESTROY, (WPARAM)pView->m_pView->GetSafeHwnd());
	pView->m_pView->SendMessage(WM_COMMAND, ID_FILE_CLOSE);
}

LRESULT CxViewMgr::OnCustomHandler( VIEW_DATA * pViewData, UINT uMsg, WPARAM wp, LPARAM lp )
{
	if (pViewData == NULL) return -1;
	BOOL bExisted = FALSE;
	for (int i=0; i<(int)m_vViews.size(); i++)
	{
		if (m_vViews[i] != pViewData) continue;
		bExisted = TRUE;
		break;
	}
	if (!bExisted) return 0;

	CxViewExt * pView = reinterpret_cast< CxViewExt * > ( pViewData->m_pView );
	if ( pView == NULL ) return 0;
	//
	return 0;
}

CBCGPTabWnd * CxViewMgr::GetViewTab( LPCTSTR lpszViewName )
{
	VIEW_DATA * pViewData = Find(lpszViewName);
	if (pViewData == NULL) return NULL;

	if ( pViewData->m_pView == NULL ) return NULL;
	if ( !::IsWindow( pViewData->m_pView->GetSafeHwnd() ) ) return NULL;

	if (theApp.m_pMainWnd == NULL) return NULL;
	if ( !::IsWindow( theApp.m_pMainWnd->GetSafeHwnd() ) ) return NULL;

	CMainFrame * pMainFrame = reinterpret_cast< CMainFrame * > ( theApp.m_pMainWnd );
	if (pMainFrame == NULL) return NULL;
	CMDIChildWndEx * pFrame = reinterpret_cast< CMDIChildWndEx * > ( pMainFrame->GetActiveFrame() );
	if (pFrame == NULL) return NULL;
	CBCGPTabWnd * pTab = (CBCGPTabWnd *)pFrame->GetRelatedTabGroup();
	if (pTab == NULL) return NULL;

	int nTabs = pTab->GetTabsNum();

	CString strTabLabel;
	for (int i=0; i<nTabs; i++)
	{
		pTab->GetTabLabel(i, strTabLabel);
		if (strTabLabel == pViewData->m_strTitle)
		{
			return pTab;
		}
	}
	return NULL;
}

VIEW_DATA * CxViewMgr::NewView(LPCTSTR lpszViewName, HWND hwndApp)
{
	CString strViewName = lpszViewName == NULL ? "" : lpszViewName;
	if (strViewName.IsEmpty()) return NULL;
	if ( !IsWindow( hwndApp ) ) return NULL;

	if (theApp.m_pMainWnd == NULL) return NULL;
	if ( !::IsWindow( theApp.m_pMainWnd->GetSafeHwnd() ) ) return NULL;

	CMainFrame * pMainFrame = reinterpret_cast< CMainFrame * > ( theApp.m_pMainWnd );
	if (pMainFrame == NULL) return NULL;

	VIEW_DATA * pViewData = Find(strViewName);
	if (pViewData == NULL)
	{
		//创建新的Tab页面
		pViewData = new VIEW_DATA();
		theApp.m_pCurViewData = pViewData;
		theApp.m_pCurViewData->m_strId = strViewName;
		theApp.m_pCurViewData->m_strTitle = strViewName;
		theApp.m_pCurViewData->m_strFlag = viewflagRunProcess;
		theApp.m_pCurViewData->m_hwndInner = hwndApp;
		pMainFrame->SendMessage(WM_COMMAND, ID_FILE_NEW);
		if (theApp.m_pCurViewData->m_strErrInfo.IsEmpty())
		{
			Add(theApp.m_pCurViewData);
			theApp.m_pCurViewData->m_pView->ShowWindow(SW_SHOW);
		}
		else
		{
			//创建过程中出现问题的处理

		}
		theApp.m_pCurViewData = NULL;
		return pViewData;
	}
	else
	{
		//激活Tab页面
		ActiveView(pViewData);
	}
	return pViewData;
}

CxViewMgr theViewMgr;