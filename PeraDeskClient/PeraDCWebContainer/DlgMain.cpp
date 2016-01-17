
// WebPageDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraDCWebContainer.h"
#include "DlgMain.h"
#include "afxdialogex.h"
//#include "CxWebRunFlow.h"
#include "UtilityFunc.h"
#include "Tools.h"
#include "CxWebNavigate.h"
#include <PeraWebPageEventExData.h>
#include "UserDefine.h"
#include "HookFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWebPageDlg 对话框


CEdit * g_pDummyInput = NULL;

CDlgMain::CDlgMain(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgMain::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pWeb = NULL;
}

void CDlgMain::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgMain, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_MESSAGE(WM_ATTACH_TO_CLIENT, &CDlgMain::OnAttachToClient)
	ON_REGISTERED_MESSAGE(WM_WEBPAGE_PROCESS_ATTACHED, OnWebPageProcessAttached)
	ON_WM_COPYDATA()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &CDlgMain::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgMain::OnBnClickedCancel)
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


// CWebPageDlg 消息处理程序

BOOL CDlgMain::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_HIDE);
	MoveWindow(0,0,0,0);

	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW);

	::PostMessage(this->GetSafeHwnd(), WM_ATTACH_TO_CLIENT, 0, 0);
	HookWebPageTitle(TRUE,"");
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDlgMain::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDlgMain::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDlgMain::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if ( m_pWeb != NULL && ::IsWindow( m_pWeb->GetSafeHwnd() ) )
	{
		m_pWeb->SetWindowPos (NULL, 0, 0, cx, cy
			, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

LRESULT CDlgMain::OnAttachToClient( WPARAM wParam, LPARAM lParam )
{
//	Sleep(100);
	CRect rcClient;
	GetClientRect(rcClient);

	switch ( theApp.m_pStartEvent->GetModuleFlag() )
	{
	case CPeraWebPageMgr::FW_TABVIEW_TASK_MODEL:
		m_pWeb = new CCxWebNavigate(this);
		m_pWeb->m_hwndRecvMsg = this->GetSafeHwnd();
		m_pWeb->Create( IDD_WEB, this );
		m_pWeb->Navigate( "about:blank" );
		m_pWeb->SetParent( this );
		m_pWeb->ShowWindow(SW_SHOW);
		{
			CCxWebPageEvent * pEvent = theWebPageMgr.NewEvent( 
				CPeraWebPageMgr::FW_TABVIEW_TASK_MODEL, CPeraWebPageMgr::FE_START );
			pEvent->m_bAnswer = TRUE;
			pEvent->m_strEventId = theApp.m_pStartEvent->m_strEventId;
			pEvent->m_dwWebPageProcessId = GetCurrentProcessId();
			pEvent->m_hWndWebPage = theApp.m_pMainWnd->GetSafeHwnd();
			pEvent->SendCopyData( theApp.m_pStartEvent->m_hWndMain );
			theWebPageMgr.DeleteEvent( pEvent );
		}
		break;

	default:
		break;
	}

	CCxWebPageEvent* e = theWebPageMgr.NewEvent( theApp.m_pStartEvent->GetModuleFlag()
		, CPeraWebPageMgr::FE_ATTACH );
	e->m_strEventId = theApp.m_pStartEvent->m_strEventId;
	e->SendCopyData( theApp.m_pStartEvent->m_hWndMain );
	
	return 0;
}


BOOL CDlgMain::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	if (pCopyDataStruct == NULL)
	{
		return __super::OnCopyData(pWnd, pCopyDataStruct);
	}

	switch ( pCopyDataStruct->dwData )
	{
	case 1:
		OnWebPageEvent( pCopyDataStruct->lpData );
		break;
	case CPMSG_LOGIN_OUT:
		InvokeJsModifyDefaultTask();
		break;
	case CPMSG_REFRESH:
		InvokeJsRefreshWindow();
		break;
	default:
		break;
	}

	return CDialogEx::OnCopyData(pWnd, pCopyDataStruct);
}


void CDlgMain::OnWebPageEvent( LPVOID lpData )
{
	if ( lpData == NULL ) return;
	LPCTSTR lpszData = (LPCTSTR)lpData;

	ZTools::WriteZToolsFormatLog( "CDlgMain::OnWebPageEvent begin >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n %s", (LPCTSTR)lpData );

	CCxWebPageEvent * e = theWebPageMgr.NewEvent( lpszData );
	if ( e == NULL ) { AfxMessageBox("CDlgMain::OnWebPageEvent 解析失败"); theWebPageMgr.DeleteEvent( e ); return; }
	CCxWebPageEvent *pSpecEvent = NULL;
	//应答事件////////////////////////////////////////////////////////////////////////
	if ( e->m_bAnswer )
	{
		if ( e->m_strEventId.IsEmpty() )  { theWebPageMgr.DeleteEvent( e ); return; }
		pSpecEvent = theWebPageMgr.Find( e->m_strEventId );
		if ( pSpecEvent == NULL )  { theWebPageMgr.DeleteEvent( e ); return; }

		//switch ( e.m_nModuleFlag )
		//{
		//case CCxWebPageEvent::FW_RUNFLOW:
		//	switch ( e.m_nEventFlag )
		//	{
		//	case CCxWebPageEvent::FE_START:
		//		pSpecEvent->m_hWndWebPage = e.m_hWndWebPage;
		//		pSpecEvent->m_dwWebPageProcessId = e.m_dwWebPageProcessId;
		//		break;

		//	default:
		//		break;
		//	}
		//	break;

		//default:
		//	break;
		//}
	}

	//请求事件////////////////////////////////////////////////////////////////////////
	else
	{
		switch ( e->GetEventFlag() )
		{
		case CPeraWebPageMgr::FE_NAVIGATE:
			if ( m_pWeb != NULL )
			{
				m_pWeb->Navigate( ((CCxWPENavigate*)e->GetExData())->m_strUrl );
			}
			break;

		default:
			break;
		}
		//switch ( e->GetEventFlag() )
		//{
		//case CPeraWebPageMgr::FW_RUNFLOW:
		//	if ( m_pWeb != NULL )
		//	{
		//		((CCxWebRunFlow*)m_pWeb)->OnWebPageEvent( e );
		//		ZTools::WriteZToolsFormatLog( "CCxWebRunFlow::OnWebPageEvent 处理完成 <<<<<<<<<<<<<<<<<<<<<< %s", "\r\n" );
		//	}
		//	break;

		//default:
		//	break;
		//}
	}

	theWebPageMgr.DeleteEvent( e );
	ZTools::WriteZToolsFormatLog( "CCxWebRunFlow::OnWebPageEvent end <<<<<<<<<<<<<<<<<<<<<< %s", "\r\n" );
	return;
}

LRESULT CDlgMain::OnWebPageProcessAttached( WPARAM,LPARAM )
{
	Sleep(100);
	ZTools::WriteZToolsFormatLog( "CDlgMain::OnWebPageProcessAttached begin >>>>>>>>>>> %s", "\r\n" );

	if ( m_pWeb == NULL )
	{
		ZTools::WriteZToolsFormatLog( "CDlgMain::OnWebPageProcessAttached %s", "异常，m_pWeb为空\r\n" );
		return 0;
	}
	if ( !::IsWindow( m_pWeb->GetSafeHwnd() ) )
	{
		ZTools::WriteZToolsFormatLog( "CDlgMain::OnWebPageProcessAttached %s", "异常，m_pWeb不是窗口\r\n" );
		return 0;
	}
	if ( theApp.m_pStartEvent->GetExData() == NULL )
	{
		ZTools::WriteZToolsFormatLog( "CDlgMain::OnWebPageProcessAttached %s", "异常，theApp.m_pStartEvent->m_pData为空\r\n" );
		return 0;
	}

	CRect rcEdit(0,0,1,1);
	g_pDummyInput = new CEdit();
	g_pDummyInput->Create(WS_CHILD|WS_VISIBLE, rcEdit, this, 39999);
	g_pDummyInput->SetFocus();


	CString strUrl;
	switch ( theApp.m_pStartEvent->GetModuleFlag() )
	{
	case CPeraWebPageMgr::FW_TABVIEW_TASK_MODEL:
		strUrl = ((CCxWPENavigate*)theApp.m_pStartEvent->GetExData())->m_strUrl;
		break;
	default:
		break;
	}

	ZTools::WriteZToolsFormatLog( "CDlgMain::OnWebPageProcessAttached m_pWeb->Navigate Url: %s\r\n", strUrl );
	
	m_pWeb->Navigate( (LPCTSTR)strUrl );

	ZTools::WriteZToolsFormatLog( "CDlgMain::OnWebPageProcessAttached end <<<<<<<<<<<<<<: %s\r\n", "\r\n" );
	return 0;
}


void CDlgMain::OnDestroy()
{
	ZTools::WriteZToolsFormatLog( "CDlgMain::OnDestroy begin >>>>>>>>>> %s", "\r\n" );
	HookWebPageTitle(FALSE,"");
	if ( m_pWeb != NULL )
	{
		m_pWeb->Navigate("about:blank");
		m_pWeb->DestroyWindow();
		delete m_pWeb;
		m_pWeb = NULL;
	}
// 	if ( GetCapture() == this )
// 		::ReleaseCapture();

	CDialogEx::OnDestroy();

	ZTools::WriteZToolsFormatLog( "CDlgMain::OnDestroy end >>>>>>>>>> %s", "\r\n" );
}


void CDlgMain::OnBnClickedOk()
{
}


void CDlgMain::OnBnClickedCancel()
{
}

void CDlgMain::OnSetFocus(CWnd* pOldWnd)
{
	//CDialogEx::OnSetFocus(pOldWnd);

	// TODO: 在此处添加消息处理程序代码
}
void CDlgMain::InvokeJsModifyDefaultTask()
{
	if (m_pWeb)
	{
		if (m_pWeb->m_hWnd)
		{
			CWebMethodData wmd;
			wmd.m_strMethodName = "modifyDefaultTask";
			BOOL bOK = m_pWeb->InvokeJsMethod(&wmd);
			ZTools::WriteZToolsFormatLog("调用JS方法modifyDefaultTask%s",(bOK ? "成功!":"失败!"));
		}
	}

}
void CDlgMain::InvokeJsRefreshWindow()
{
	if (m_pWeb)
	{
		if (m_pWeb->m_hWnd)
		{
			CWebMethodData wmd;
			wmd.m_strMethodName = "refreshWindow";
			BOOL bOK = m_pWeb->InvokeJsMethod(&wmd);
			ZTools::WriteZToolsFormatLog("调用JS方法refreshWindow%s",(bOK ? "成功!":"失败!"));
		}
	}
}