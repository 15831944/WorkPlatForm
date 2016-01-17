// DlgViewRunFlow.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgViewRunFlow.h"
#include "afxdialogex.h"
#include "PeraProcessDesignerUtility.h"
#include "UtilityFunc.h"
#include "Tools.h"
#include "AttachWindow.h"
#include "WebEventData.h"

// CDlgViewRunFlow �Ի���

BEGIN_MESSAGE_MAP(CDlgViewRunFlow, CCxBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDlgViewRunFlow::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgViewRunFlow::OnBnClickedCancel)
	ON_REGISTERED_MESSAGE(WM_AW_PROCESS_ATTACHING, &CDlgViewRunFlow::OnAwProcessAttaching)
	ON_WM_SIZE()
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()

void CDlgViewRunFlow::DoDataExchange(CDataExchange* pDX)
{
	CCxBCGPDialog::DoDataExchange(pDX);
}

IMPLEMENT_DYNAMIC(CDlgViewRunFlow, CCxBCGPDialog)

CDlgViewRunFlow::CDlgViewRunFlow(CWnd* pParent /*=NULL*/)
	: CCxBCGPDialog(CDlgViewRunFlow::IDD, pParent)
{
	//m_hwndWebPage = NULL;
	//m_pStartWebPageEvent = NULL;
	m_dwExStyleWebPage = 0;
	m_dwStyleWebPage = 0;
	m_nWidth = 640;
	m_nHeight = 480;
}

CDlgViewRunFlow::~CDlgViewRunFlow()
{
	//theWebPageMgr.DeleteEvent( m_pStartWebPageEvent );
	theApp.m_hwndViewRunFlow = NULL;
}

void CDlgViewRunFlow::OnBnClickedOk()
{
	//�� Enter ���رնԻ���
}

void CDlgViewRunFlow::OnBnClickedCancel()
{
	//::SendMessage(this->GetSafeHwnd(),BM_CLICK,IDCANCEL,IDCANCEL);
	//�� ESC ���رնԻ���
	//ע�⣺����Ϣ���Զ����� WM_CLOSE ��Ϣ
	CCxBCGPDialog::OnCancel();
	//20140530,lc,���ع��ļ��󣬹ز鿴����ͼ�Ĵ��ڻᵼ�´���˳��һ�µ�����
	theApp.m_pMainWnd->SetFocus();
}

BOOL CDlgViewRunFlow::OnInitDialog()
{
	CCxBCGPDialog::OnInitDialog();
	CRect rc(0,0,m_nWidth,m_nHeight);
	CRect rcParent;
	theApp.m_pMainWnd->GetWindowRect(&rcParent);
	int x = (rcParent.Width() - m_nWidth) / 2 + rcParent.left;
	int y = (rcParent.Height() - m_nHeight) / 2 + rcParent.top;
	rc.MoveToXY(x,y);
	MoveWindow( &rc );

	theApp.m_hwndViewRunFlow = this->GetSafeHwnd();

	CString strPage = m_strUrl;
	if ( strPage.IsEmpty() )
		strPage = "about:blank";
	else 
		AttachTicket( strPage, theApp.m_LoginData.m_strTicket );

	//CCxWebPageEventExData *pData = new CCxWPENavigate();
	//((CCxWPENavigate*)pData)->m_strUrl = strPage;
	//m_pStartWebPageEvent = theWebPageMgr.Require( CPeraWebPageMgr::FW_VIEW_RUNFLOW
	//	, CPeraWebPageMgr::FE_START, pData );
	//delete pData; pData = NULL;

	CString sWebContainerPath = GetExecDir() + "\\PeraPDWebContainer.exe";
	CAttachWindowExData exData;
	exData.m_sUrl = strPage;
	CString sId = GetNewEventNameWin7();
	if ( !m_AttachWindow.CreateClient( sId,
										WEBTYPE_RUNFLOW_VIEW,
										CPMSG_AW_PROCESS_ATTACHING,
										GetCurrentProcessId(),
										GetMainWnd(),
										sWebContainerPath,
										exData.GetJsonStr() ) )
	{
		ZTools::WriteZToolsLog( "CDlgViewRunFlow::OnInitDialog() ���� WebContainer ʧ��" );
	}

	ShowWindow(SW_SHOWMAXIMIZED);
	return TRUE;
}

LRESULT CDlgViewRunFlow::OnAwProcessAttaching( WPARAM wParam, LPARAM lParam )
{
	//CCxWebPageEvent * pEvent = (CCxWebPageEvent *)wParam;
	//if ( pEvent == NULL ) return 0;

	//m_hwndWebPage = pEvent->m_hWndWebPage;
	//if ( !IsWindow( m_hwndWebPage ) )
	//{
	//	m_hwndWebPage = NULL; return 0;
	//}

	ZTools::WriteZToolsLog( "CDlgViewRunFlow::OnAwProcessAttaching begin" );
	CAttachWindow* pAttachWindow = (CAttachWindow*)wParam;
	if ( pAttachWindow == NULL )
	{
		ZTools::WriteZToolsLog( "CDlgViewRunFlow::OnAwProcessAttaching ������Ч" );
		return 0;
	}
	if ( pAttachWindow->m_sId.CompareNoCase( m_AttachWindow.m_sId ) != 0 )
	{
		ZTools::WriteZToolsLog( "CDlgViewRunFlow::OnAwProcessAttaching ���� sid ��һ��" );
		delete pAttachWindow; pAttachWindow = NULL;
		return 0;
	}
	if ( !IsWindow( pAttachWindow->m_hClientWnd ) )
	{
		ZTools::WriteZToolsLog( "CDlgViewRunFlow::OnAwProcessAttaching webc ������Ч" );
		delete pAttachWindow; pAttachWindow = NULL;
		return 0;
	}
	m_AttachWindow.m_hClientWnd = pAttachWindow->m_hClientWnd;
	m_AttachWindow.m_dwClientProcessId = pAttachWindow->m_dwClientProcessId;
	delete pAttachWindow; pAttachWindow = NULL;

	//m_hwndWebPage = m_AttachWindow.m_hClientWnd;
	ZTools::WriteZToolsFormatLog( "CDlgViewRunFlow::OnAwProcessAttaching \r\n %s", "AttachWindow begin" );

	m_dwExStyleWebPage = ::GetWindowLong(m_AttachWindow.m_hClientWnd, GWL_EXSTYLE);
	m_dwStyleWebPage = ::GetWindowLong(m_AttachWindow.m_hClientWnd, GWL_STYLE);

	AttachWindow( this->GetSafeHwnd(), m_AttachWindow.m_hClientWnd, FALSE );
	ZTools::WriteZToolsFormatLog( "CDlgViewRunFlow::OnAwProcessAttaching \r\n %s", "AttachWindow end" );

	CRect rcClient;
	GetClientRect(&rcClient);
	//rcClient.bottom -= 38;

	::MoveWindow(m_AttachWindow.m_hClientWnd, 0, 0
		, rcClient.Width(), rcClient.Height(), TRUE);

	::ShowWindow( m_AttachWindow.m_hClientWnd, SW_SHOW );

	::PostMessage( m_AttachWindow.m_hClientWnd, WM_AW_PROCESS_ATTACHED, 0, 0 );
	ZTools::WriteZToolsFormatLog( "CDlgViewRunFlow::OnAwProcessAttaching \r\n %s", "PostMessage WM_AW_PROCESS_ATTACHED end" );

	return 0;
}

void CDlgViewRunFlow::OnSize(UINT nType, int cx, int cy)
{
	CCxBCGPDialog::OnSize(nType, cx, cy);

	if ( IsWindow( m_AttachWindow.m_hClientWnd ) )
	{
		::SetWindowPos ( m_AttachWindow.m_hClientWnd, NULL, 0, 0, cx, cy 
		, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}


void CDlgViewRunFlow::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	/*
	lc,20140609,�޸�����BUG����ô�Ŀ��Խ���������󣬵�����ԭ��δ֪
	PERA13.1-611 PERASI-16833��13.1V2���ͻ���-��ģ������������ʵ�����棬�ϴ��ļ����޸��ַ����Ͳ����������뷨�����أ��������޷����루ʵ�����ǿ��Եģ�
	*/
	if (nState == FALSE)
	{
		::SetFocus(0);
	}
	CCxBCGPDialog::OnActivate(nState, pWndOther, bMinimized);

	// TODO: �ڴ˴������Ϣ����������
}
