// DlgRunFlow.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgRunFlow.h"
#include "afxdialogex.h"

#include "value.h"
#include "reader.h"
#include "writer.h"
#include "InvokeWsFunc.h"
#include "DlgSvg.h"
#include "PeraProcessDesignerUtility.h"
#include "UtilityFunc.h"

#include "Tools.h"
#include "PtrVectorHeader.h"
#include "CxStructTreeNode.h"
#include <BaseHtmlDialog.h>
#include "AttachWindow.h"
#include "WebEventData.h"
#include "WebCommunicationHost.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CRunFlowGetResult::CRunFlowGetResult()
{
	m_bParsed = FALSE;
}

CRunFlowGetResult::~CRunFlowGetResult()
{
	Clear();
}

void CRunFlowGetResult::Clear()
{
	m_strFlowId = "";
	m_bParsed = FALSE;
	PtrMapClear( mapParamValue );
}

CString CRunFlowGetResult::GetMethodParamStr( void )
{
	try
	{
		CString strResult;
		//Json::Value obj;
		//obj["processId"] = Json::Value( (LPCTSTR)m_strFlowId );

		//Json::FastWriter fast_writer;
		//strResult = fast_writer.write( obj ).c_str();

		strResult.Format( "[\"%s\"]", m_strFlowId );

		return strResult;
	}
	catch (...)
	{
		return "";
	}
}

CString CRunFlowGetResult::GetMethodName( void )
{
	return "getProcessParamsForIce";
}

BOOL CRunFlowGetResult::ParseResult( LPCTSTR lpszData, CCxStructTree * pTree )
{
	Clear();

	CString strSrc = lpszData == NULL ? "" : lpszData;
	if ( strSrc.IsEmpty() ) return FALSE;

	Json::Reader reader;
	Json::Value obj( Json::objectValue );
	Json::Value arrParam;
	CCxStructTreeRunResultParam * pValue = NULL;
	CCxStructTreeNode * pNode = NULL;
	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if (!obj.isObject()) return FALSE;

		if ( !obj["paramList"].isNull() && obj["paramList"].isArray() )
		{
			arrParam = obj["paramList"];
			for ( Json::Value::iterator itrParam = arrParam.begin();
				itrParam != arrParam.end(); ++itrParam )
			{
				pValue = new CCxStructTreeRunResultParam();
				pValue->m_strId = ( (*itrParam)["id"].isString()) ? (*itrParam)["id"].asCString() : _T("");
				pValue->m_strValue = ( (*itrParam)["value"].isString()) ? (*itrParam)["value"].asCString() : _T("");
				if ( pTree != NULL )
				{
					pNode = pTree->Find( pValue->m_strId );
					if ( pNode->IsParamNode() ) pValue->m_pNode = (CCxStructTreeNodeParam *)pNode;
				}
				mapParamValue.insert( make_pair( pValue->m_strId, pValue ) );
			}
		}
	}
	catch (...)
	{
		Clear();
		return FALSE;
	}

	m_bParsed = TRUE;
	return m_bParsed;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


CRunFlowExec::CRunFlowExec()
{

}

CRunFlowExec::~CRunFlowExec()
{
	Clear();
}

void CRunFlowExec::Clear()
{
	m_strFlowId = "";
}

CString CRunFlowExec::GetMethodParamStr( void )
{
	try
	{
		CString strResult;
		strResult.Format( "[\"%s\"]", m_strFlowId );
		return strResult;
	}
	catch (...)
	{
		return "";
	}
}

CString CRunFlowExec::GetMethodName( void )
{
	return "runLocalModelByProcessOid";
}

BOOL CRunFlowExec::ParseResult( LPCTSTR lpszData, CCxStructTree * pTree )
{
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////



// CDlgRunFlow 对话框

BEGIN_MESSAGE_MAP(CDlgRunFlow, CCxBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDlgRunFlow::OnBnClickedOk)
	//ON_BN_CLICKED(IDCANCEL, &CDlgRunFlow::OnBnClickedCancel)
	ON_MESSAGE(WM_RUNFLOW_COMPLETED, &CDlgRunFlow::OnRunFlowCompleted)
	ON_REGISTERED_MESSAGE(WM_AW_PROCESS_ATTACHING, &CDlgRunFlow::OnAwProcessAttaching)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_RUNFLOW_OK, &CDlgRunFlow::OnBnClickedRunflowOk)
	ON_BN_CLICKED(IDC_RUNFLOW_CANCEL, &CDlgRunFlow::OnBnClickedRunflowCancel)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

//BEGIN_DISPATCH_MAP(CDlgRunFlow, CCxBCGPDialog)
//END_DISPATCH_MAP()

IMPLEMENT_DYNAMIC(CDlgRunFlow, CCxBCGPDialog)

CDlgRunFlow::CDlgRunFlow(CWnd* pParent /*=NULL*/)
	: CCxBCGPDialog(CDlgRunFlow::IDD, pParent)
{
	m_lpszUrl = NULL;
	m_lpszFlowId = NULL;
//	m_hwndWebPage = NULL;
	m_bFinished = FALSE;
	//m_pStartWebPageEvent = NULL;
	//m_bValidData = FALSE;
	m_bGetResult = FALSE;

	m_dwExStyleWebPage = 0;
	m_dwStyleWebPage = 0;
}

CDlgRunFlow::~CDlgRunFlow()
{
	//theWebPageMgr.DeleteEvent( m_pStartWebPageEvent );
	theApp.m_hwndRunFlow = NULL;
	ZTools::WriteZToolsFormatLog( "CDlgRunFlow::~CDlgRunFlow %s", "\r\n" );
}

void CDlgRunFlow::DoDataExchange(CDataExchange* pDX)
{
	CCxBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RUNFLOW_OK, m_wndOk);
	DDX_Control(pDX, IDC_RUNFLOW_CANCEL, m_wndCancel);
}



BOOL CDlgRunFlow::OnInitDialog()
{
	CCxBCGPDialog::OnInitDialog();
	m_wndOk.EnableWindow(FALSE);
	m_wndCancel.EnableWindow(TRUE);
	CRect rc;
	GetWindowRect(&rc);
	CRect rcParent;
	theApp.m_pMainWnd->GetWindowRect(&rcParent);
	int x = (rcParent.Width() - rc.Width()) / 2 + rcParent.left;
	int y = (rcParent.Height() - rc.Height()) / 2 + rcParent.top;
	rc.MoveToXY(x,y);
	MoveWindow( &rc );
	theApp.m_hwndRunFlow = this->GetSafeHwnd();

	CString strPage = m_lpszUrl;//= GetSvgDefaultPage();
	//if ( !IsAccessibleFile(strPage) ) strPage = "about:blank";

	AttachTicket(strPage, theApp.m_LoginData.m_strTicket);
	
	//CCxWebPageEventExData* pData = new CWPERunFlowStart();
	//((CWPERunFlowStart*)pData)->m_strUrl = strPage;
	//((CWPERunFlowStart*)pData)->m_strFlowId = m_lpszFlowId;

	//m_pStartWebPageEvent = theWebPageMgr.Require( CPeraWebPageMgr::FW_RUNFLOW, CPeraWebPageMgr::FE_START
	//	, pData );

	//delete pData; pData = NULL;

	CString sWebContainerPath = GetExecDir() + "\\PeraPDWebContainer.exe";
	CAttachWindowExData exData;
	exData.m_sUrl = strPage;
	CString sId = GetNewEventNameWin7();
	if ( !m_AttachWindow.CreateClient( sId,
										WEBTYPE_RUNFLOW,
										CPMSG_AW_PROCESS_ATTACHING,
										GetCurrentProcessId(),
										GetMainWnd(),
										sWebContainerPath,
										exData.GetJsonStr() ))
	{
		ZTools::WriteZToolsLog( "CDlgRunFlow::OnInitDialog() 创建WebContainer进程失败" );
	}
	ShowWindow(SW_SHOWMAXIMIZED);
	//Navigate( strPage );
	return TRUE;
}


void CDlgRunFlow::OnBnClickedOk()
{
	//按 Enter 不关闭对话框
}


void CDlgRunFlow::OnBnClickedCancel()
{
	//::SendMessage(this->GetSafeHwnd(),BM_CLICK,IDCANCEL,IDCANCEL);
	//按 ESC 不关闭对话框
	//注意：此消息将自动屏蔽 WM_CLOSE 消息
}


DWORD WINAPI CDlgRunFlow::GetResultThread( LPVOID lpParameter )
{
	CDlgRunFlow* pThis = (CDlgRunFlow*)lpParameter;
	pThis->m_strResultData = "";

	ZTools::WriteZToolsFormatLog("启动查询运行结果线程 %s", "CDlgRunFlow::GetResultThread begin" );

	//if ( pThis->m_pStartWebPageEvent == NULL )
	//{
	//	return 0;
	//}
	//if ( !IsWindow( pThis->m_pStartWebPageEvent->m_hWndWebPage ) )
	//{
	//	return 0;
	//}
	if ( !IsWindow( pThis->m_AttachWindow.m_hClientWnd ) )
	{
		return 0;
	}
	theApp.m_bGetClientSubFlowId = FALSE;

	//SendCopyData( pThis->m_AttachWindow.m_hClientWnd, CPMSG_WEBPAGE_getClientSubFlowId, NULL, 0 );

	CWERunFlowSubFlowId subFlowId;
	subFlowId.m_dwWebEvent = WE_RUNFLOW_SUBFLOWID;
	subFlowId.m_dwWebType = WEBTYPE_RUNFLOW;
	subFlowId.m_sWebId = pThis->m_AttachWindow.m_sId;
	theCommHost.Send( pThis->m_AttachWindow.m_hClientWnd, &subFlowId );
	
	CRunFlowGetResult& e = pThis->m_Result;
	e.m_strFlowId = theApp.m_processMgr.m_sFlowId; //pThis->m_lpszFlowId;
	CString strResult = InvokeWsMethod( e.GetMethodName(), e.GetMethodParamStr() );

	ZTools::WriteZToolsFormatLog("查询结果数据 CDlgRunFlow::GetResultThread \r\n Method: %s\r\n FlowId: %s\r\n %s\r\n"
		, e.GetMethodName()
		, e.GetMethodParamStr()
		, strResult );

	//CCxStructTreeRunReturn rr;
	//if ( !rr.Parse( strResult, &theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree ) )
	//{
	//	ZTools::WriteZToolsFormatLog("解析运行结果失败 %s", "\r\n" );
	//}
	//else
	//{
	//	rr.UpdateFileParam();
	//	pThis->m_strResultData = rr.GetSvgResultJsonStr();
	//}

	pThis->m_strResultData = strResult;

	//此处调用 js 方法，某些ie版本浏览器会导致主程序崩溃。
	//CDlgSvg * pSvg = GetSvgDilogPtr();
	//if ( !pSvg->JsMethod_parseSubProcess( strResult ) )
	//{
	//	return 0;
	//}

//	theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.UpdateParams( &e.mapParamValue );
	
	if ( ::IsWindow( pThis->GetSafeHwnd() ) ) 
		::PostMessage(pThis->GetSafeHwnd(), WM_RUNFLOW_COMPLETED, (WPARAM)0, (LPARAM)0);

	ZTools::WriteZToolsFormatLog("调用查询运行结果线程返回 %s"
	, "CDlgRunFlow::GetResultThread end" );

	return TRUE;
}

BOOL CDlgRunFlow::GetResult( void )
{
	if ( m_bGetResult ) return FALSE;
	m_bGetResult = TRUE;

	m_wndOk.EnableWindow(FALSE);
	m_wndCancel.EnableWindow(FALSE);

	DWORD dwThreadId = 0;
	HANDLE hThread = ::CreateThread(
		NULL
		, 0
		, GetResultThread
		, (LPVOID)this
		, CREATE_SUSPENDED
		, &dwThreadId);

	if (hThread == NULL || hThread == 0)
	{
		//std::string strError("执行设计过程时创建线程失败");
		//ZTools::WriteZToolsLog(strError);
		return FALSE;	
	}
	ResumeThread(hThread);
	::CloseHandle(hThread);
	return TRUE;
}


LRESULT CDlgRunFlow::OnRunFlowCompleted( WPARAM wParam, LPARAM lParam )
{
	ZTools::WriteZToolsFormatLog("CDlgRunFlow::OnRunFlowCompleted %s"
		, "\r\n" );

	PostMessage(WM_CLOSE,0,0);

	return 0;
}


void CDlgRunFlow::OnClose()
{
	ZTools::WriteZToolsFormatLog( "CDlgRunFlow::OnClose %s", "\r\n" );
	CCxBCGPDialog::OnClose();
}

LRESULT CDlgRunFlow::OnAwProcessAttaching( WPARAM wParam, LPARAM lParam )
{
	Sleep(200);

	//CCxWebPageEvent * pEvent = (CCxWebPageEvent *)wParam;
	//if ( pEvent == NULL ) return 0;

	//m_hwndWebPage = pEvent->m_hWndWebPage;
	//if ( !IsWindow( m_hwndWebPage ) )
	//{
	//	::MessageBox(GetMainWnd(), "异常，WebPage窗口无效", g_lpszAppTitle, MB_OK|MB_ICONERROR );
	//	m_hwndWebPage = NULL; return 0;
	//}
	//ZTools::WriteZToolsFormatLog( "CDlgRunFlow::OnAwProcessAttaching begin >>>>>>>>>>>>>> %s", "" );

	//m_dwExStyleWebPage = ::GetWindowLong(m_hwndWebPage, GWL_EXSTYLE);
	//m_dwStyleWebPage = ::GetWindowLong(m_hwndWebPage, GWL_STYLE);
	//AttachWindow( this->GetSafeHwnd(), m_hwndWebPage, FALSE );
	//SetMouseClick( CPoint(48,1), m_hWnd );

	ZTools::WriteZToolsLog( "CDlgRunFlow::OnAwProcessAttaching  begin" );
	CAttachWindow* pAttachWindow = (CAttachWindow*)wParam;
	if ( pAttachWindow == NULL )
	{
		ZTools::WriteZToolsLog( "CDlgRunFlow::OnAwProcessAttaching 参数无效" );
		return 0;
	}
	if ( pAttachWindow->m_sId.CompareNoCase( m_AttachWindow.m_sId ) != 0 )
	{
		ZTools::WriteZToolsLog( "CDlgRunFlow::OnAwProcessAttaching 请求附加的 WebContainer sid不一致" );
		delete pAttachWindow; pAttachWindow = NULL;
		return 0;
	}
	if( !IsWindow( pAttachWindow->m_hClientWnd ) )
	{
		ZTools::WriteZToolsLog( "CDlgRunFlow::OnAwProcessAttaching 窗口无效" );
		delete pAttachWindow; pAttachWindow = NULL;
		return 0;
	}
	m_AttachWindow.m_hClientWnd = pAttachWindow->m_hClientWnd;
	m_AttachWindow.m_dwClientProcessId = pAttachWindow->m_dwClientProcessId;
	delete pAttachWindow; pAttachWindow = NULL;

	//m_hwndWebPage = m_AttachWindow.m_hClientWnd;

	m_dwExStyleWebPage = ::GetWindowLong(m_AttachWindow.m_hClientWnd, GWL_EXSTYLE);
	m_dwStyleWebPage = ::GetWindowLong(m_AttachWindow.m_hClientWnd, GWL_STYLE);

	AttachWindow( this->GetSafeHwnd(), m_AttachWindow.m_hClientWnd, FALSE );

	ZTools::WriteZToolsFormatLog( "CDlgRunFlow::OnAwProcessAttaching end <<<<<<<<<<<<< %s", "" );

	CRect rcClient;
	GetClientRect(&rcClient);
	rcClient.bottom -= 38;

	::MoveWindow(m_AttachWindow.m_hClientWnd, 0, 0
		, rcClient.Width(), rcClient.Height(), TRUE);
	::ShowWindow( m_AttachWindow.m_hClientWnd, SW_SHOW );

	{
		CRect rc1, rc2;
		CString slog, s1,s2;
		CWnd::FromHandle(m_AttachWindow.m_hClientWnd)->GetWindowText(s1);
		CWnd::FromHandle(m_AttachWindow.m_hClientWnd)->GetWindowRect(&rc1);
		slog.Format(" hwndRunFlow:%08X\r\n hwndWebPage:%08X\r\n hwndWebPageParent:%08X\r\n WebPageTitle:%s\r\n WebPageRect:(x:%d, y:%d, cx:%d, cy:%d)\r\n WebPageVisible:%s\r\n"
			, this->GetSafeHwnd()
			, m_AttachWindow.m_hClientWnd
			, ::GetParent( m_AttachWindow.m_hClientWnd )
			, s1
			, rc1.left, rc1.top, rc1.Width(), rc1.Height()
			, CWnd::FromHandle(m_AttachWindow.m_hClientWnd)->IsWindowVisible() ? "true" : "false"
		);
		ZTools::WriteZToolsFormatLog( "CDlgRunFlow::OnAwProcessAttaching 绑定窗口信息\r\n%s", slog );
	}

	////通知后台运行流程
	//CRunFlowExec eExec;
	//eExec.m_strFlowId = m_lpszFlowId;
	//CString strResult = InvokeWsMethod( eExec.GetMethodName(), eExec.GetMethodParamStr() );

	::PostMessage( m_AttachWindow.m_hClientWnd, WM_AW_PROCESS_ATTACHED, 0, 0 );
	
	ZTools::WriteZToolsFormatLog( "CDlgRunFlow::OnAwProcessAttaching %s", "PostMessage to WebPage, WM_AW_PROCESS_ATTACHED\r\n" );

	return 0;
}

void CDlgRunFlow::OnBnClickedRunflowOk()
{
	GetResult();
}

void CDlgRunFlow::OnBnClickedRunflowCancel()
{
	PostMessage(WM_CLOSE,0,0);
}


void CDlgRunFlow::OnDestroy()
{
	ZTools::WriteZToolsFormatLog( "CDlgRunFlow::OnDestroy %s", "\r\n" );

	//m_strResultData = "";

	//if ( m_pStartWebPageEvent != NULL /*&& !m_bFinished */)
	//{
	if ( ::IsWindow( m_AttachWindow.m_hClientWnd ) )
	{
		//CCxWebPageEvent * e = theWebPageMgr.NewEvent(CPeraWebPageMgr::FW_RUNFLOW
		//	, CPeraWebPageMgr::FE_RUNFLOW_CANCEL);
		//e->SendCopyData( m_AttachWindow.m_hClientWnd );
		//theWebPageMgr.DeleteEvent( e );CWERunFlowCancel runFlowCancel;

		CWERunFlowCancel runFlowCancel;
		runFlowCancel.m_dwWebEvent = WE_RUNFLOW_CANCEL;
		runFlowCancel.m_dwWebType = WEBTYPE_RUNFLOW;
		runFlowCancel.m_sWebId = m_AttachWindow.m_sId;
		theCommHost.Send( m_AttachWindow.m_hClientWnd, &runFlowCancel );
	}
	//}

// 	if ( IsWindow(m_hwndWebPage) )
// 	{
// 		ZTools::WriteZToolsFormatLog( "CDlgRunFlow::OnDestroy %s", "正在关闭 WebPage 进程\r\n" );
// 
// 		m_dwStyleWebPage = WS_POPUP|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME|WS_OVERLAPPED|WS_BORDER;
// 		m_dwExStyleWebPage = WS_EX_DLGMODALFRAME|WS_EX_WINDOWEDGE|WS_EX_CONTROLPARENT;
// 		::SetWindowLong(m_hwndWebPage, GWL_EXSTYLE, m_dwExStyleWebPage);
// 		::SetWindowLong(m_hwndWebPage, GWL_STYLE, m_dwStyleWebPage);
// 		::SetParent( m_hwndWebPage, NULL );
// 
// 		//CWnd::FromHandle( m_hwndWebPage )->ShowWindow(SW_HIDE);
// 		//CWnd::FromHandle( m_hwndWebPage )->SetParent(NULL);
// 		//CWnd::FromHandle( m_hwndWebPage )->ModifyStyle(WS_CHILD|WS_VISIBLE, WS_POPUP|WS_CAPTION|WS_DLGFRAME|WS_SYSMENU);
// 		//	CWnd::FromHandle( m_hwndWebPage )->SetParent(NULL);
// 		::SendMessage(m_hwndWebPage, WM_CLOSE,0,0);
// 
// 		Sleep(100);
// 
// 		//HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, m_pStartWebPageEvent->m_dwWebPageProcessId);
// 		//if (hProcess != NULL && hProcess != INVALID_HANDLE_VALUE)
// 		//{
// 		//	ZTools::WriteZToolsLog(ZTools::FormatString("关闭WebPage进程%d",  m_pStartWebPageEvent->m_dwWebPageProcessId));
// 
// 		//	TerminateProcess(hProcess, 0);
// 		//	CloseHandle(hProcess);
// 		//	hProcess = NULL;
// 		//}
// 
// 		ZTools::WriteZToolsFormatLog( "CDlgRunFlow::OnDestroy %s", "关闭 WebPage 进程 end\r\n" );
// 	}
	//m_hwndWebPage = NULL;
	
	m_AttachWindow.m_hClientWnd = NULL;
	m_AttachWindow.m_dwClientProcessId = 0;

	CCxBCGPDialog::OnDestroy();
}


void CDlgRunFlow::OnSize(UINT nType, int cx, int cy)
{
	CCxBCGPDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if ( IsWindow( m_AttachWindow.m_hClientWnd ) )
	{
		::SetWindowPos ( m_AttachWindow.m_hClientWnd, NULL, 0, 0, cx, cy -38 
			, SWP_NOACTIVATE | SWP_NOZORDER);
	}

	
	if(m_wndOk.m_hWnd)
	{
		int iOkX = cx/2 - 100;
		int iOky = cy    - 32;
		m_wndOk.SetWindowPos(NULL,iOkX,iOky,0,0,SWP_NOZORDER|SWP_NOSIZE);
		m_wndOk.Invalidate(FALSE);
		m_wndCancel.SetWindowPos(NULL,iOkX + 150,iOky,0,0,SWP_NOZORDER|SWP_NOSIZE);
		m_wndCancel.Invalidate(FALSE);
	}
}


void CDlgRunFlow::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CCxBCGPDialog::OnGetMinMaxInfo(lpMMI);
}
