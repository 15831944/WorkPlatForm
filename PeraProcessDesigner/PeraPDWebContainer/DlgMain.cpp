
// WebPageDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PeraPDWebContainer.h"
#include "DlgMain.h"
#include "afxdialogex.h"
#include "UtilityFunc.h"
#include "Tools.h"
#include "HookFunc.h"
#include "UserDefine.h"
#include "AttachWindow.h"
#include "AttachWindowClient.h"
#include "WebEventData.h"
#include "WebCommnunicationClient.h"
#include "PeraPDWeb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWebPageDlg �Ի���


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
	ON_REGISTERED_MESSAGE(WM_AW_PROCESS_ATTACHING, &CDlgMain::OnAwProcessAttaching)
	ON_REGISTERED_MESSAGE(WM_AW_PROCESS_ATTACHED, OnAwProcessAttached)
	ON_WM_COPYDATA()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &CDlgMain::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgMain::OnBnClickedCancel)
	ON_WM_SETFOCUS()
	ON_REGISTERED_MESSAGE(WM_WEBEVENT_ASYNC,OnWebEventAsync)
END_MESSAGE_MAP()


// CWebPageDlg ��Ϣ�������

BOOL CDlgMain::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	ShowWindow(SW_HIDE);
	MoveWindow(0,0,0,0);

	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW);

	::PostMessage(this->GetSafeHwnd(), WM_AW_PROCESS_ATTACHING, 0, 0);
	HookWebPageTitle(TRUE,"");
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CDlgMain::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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

LRESULT CDlgMain::OnAwProcessAttaching( WPARAM wParam, LPARAM lParam )
{
//	Sleep(100);
	CRect rcClient;
	GetClientRect(rcClient);

	m_pWeb = new CPeraPDWeb(this);
	m_pWeb->Create( IDD_WEB, this );
	m_pWeb->Navigate( "about:blank" );
	m_pWeb->SetParent( this );
	m_pWeb->ShowWindow(SW_SHOW);

	ResponseAttachProcessWnd( GetSafeHwnd() );

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
	case CPMSG_WEBEVENT:
		{
			ZTools::WriteZToolsLog( " CDlgMain::OnCopyData CPMSG_WEBEVENT" );
			if( !theCommClient.Receive( (LPCTSTR)pCopyDataStruct->lpData ) )
			{
				return FALSE;
			}
		}
		break;
	//case CPMSG_WEBPAGE_getClientSubFlowId:
	//{
	//	CString sFlowId = ((CCxWebRunFlow*)m_pWeb)->getClientSubFlowId();
	//	if ( sFlowId.IsEmpty() ) break;
	//	//if ( theApp.m_pStartEvent == NULL ) break;
	//	if ( !IsWindow( g_aw.m_hHostWnd ) ) break;

	//	TCHAR* pBuf = new TCHAR[ sFlowId.GetLength() + 1 ];
	//	memcpy( pBuf, (LPCTSTR)sFlowId, sFlowId.GetLength() );
	//	pBuf[ sFlowId.GetLength() ] = NULL;

	//	SendCopyData( g_aw.m_hHostWnd, CPMSG_WEBPAGE_getClientSubFlowId, (LPVOID)pBuf, sFlowId.GetLength() + 1 );

	//	delete[] pBuf; pBuf = NULL;
	//	break;
	//}
	default:
		break;
	}

	return CDialogEx::OnCopyData(pWnd, pCopyDataStruct);
}

//
//void CDlgMain::OnWebPageEvent( LPVOID lpData )
//{
//	if ( lpData == NULL ) return;
//	LPCTSTR lpszData = (LPCTSTR)lpData;
//
//	ZTools::WriteZToolsFormatLog( "CDlgMain::OnWebPageEvent begin >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n %s", (LPCTSTR)lpData );
//
//	CCxWebPageEvent * e = theWebPageMgr.NewEvent( lpszData );
//	if ( e == NULL ) { AfxMessageBox("CDlgMain::OnWebPageEvent ����ʧ��"); theWebPageMgr.DeleteEvent( e ); return; }
//	CCxWebPageEvent *pSpecEvent = NULL;
//	//Ӧ���¼�////////////////////////////////////////////////////////////////////////
//	if ( e->m_bAnswer )
//	{
//		if ( e->m_strEventId.IsEmpty() )  { theWebPageMgr.DeleteEvent( e ); return; }
//		pSpecEvent = theWebPageMgr.Find( e->m_strEventId );
//		if ( pSpecEvent == NULL )  { theWebPageMgr.DeleteEvent( e ); return; }
//
//		//switch ( e.m_nModuleFlag )
//		//{
//		//case CCxWebPageEvent::FW_RUNFLOW:
//		//	switch ( e.m_nEventFlag )
//		//	{
//		//	case CCxWebPageEvent::FE_START:
//		//		pSpecEvent->m_hWndWebPage = e.m_hWndWebPage;
//		//		pSpecEvent->m_dwWebPageProcessId = e.m_dwWebPageProcessId;
//		//		break;
//
//		//	default:
//		//		break;
//		//	}
//		//	break;
//
//		//default:
//		//	break;
//		//}
//	}
//
//	//�����¼�////////////////////////////////////////////////////////////////////////
//	else
//	{
//		switch ( e->GetEventFlag() )
//		{
//		case CPeraWebPageMgr::FW_RUNFLOW:
//			if ( m_pWeb != NULL )
//			{
//				((CCxWebRunFlow*)m_pWeb)->OnWebPageEvent( e );
//				ZTools::WriteZToolsFormatLog( "CCxWebRunFlow::OnWebPageEvent ������� <<<<<<<<<<<<<<<<<<<<<< %s", "\r\n" );
//			}
//			break;
//
//		default:
//			break;
//		}
//	}
//
//	theWebPageMgr.DeleteEvent( e );
//	ZTools::WriteZToolsFormatLog( "CCxWebRunFlow::OnWebPageEvent end <<<<<<<<<<<<<<<<<<<<<< %s", "\r\n" );
//	return;
//}

LRESULT CDlgMain::OnAwProcessAttached( WPARAM,LPARAM )
{
	Sleep(100);
	ZTools::WriteZToolsFormatLog( "CDlgMain::OnAwProcessAttached begin >>>>>>>>>>> %s", "\r\n" );

	if ( m_pWeb == NULL )
	{
		ZTools::WriteZToolsFormatLog( "CDlgMain::OnAwProcessAttached %s", "�쳣��m_pWebΪ��\r\n" );
		return 0;
	}
	if ( !::IsWindow( m_pWeb->GetSafeHwnd() ) )
	{
		ZTools::WriteZToolsFormatLog( "CDlgMain::OnAwProcessAttached %s", "�쳣��m_pWeb���Ǵ���\r\n" );
		return 0;
	}
	//if ( theApp.m_pStartEvent->GetExData() == NULL )
	//{
	//	ZTools::WriteZToolsFormatLog( "CDlgMain::OnAwProcessAttached %s", "�쳣��theApp.m_pStartEvent->m_pDataΪ��\r\n" );
	//	return 0;
	//}
	if ( g_aw.m_sExData.IsEmpty() )
	{
		ZTools::WriteZToolsLog( "CDlgMain::OnAwProcessAttached g_aw.m_sExData Ϊ��" );
		return 0;
	}

	CRect rcEdit(0,0,1,1);
	g_pDummyInput = new CEdit();
	g_pDummyInput->Create(WS_CHILD|WS_VISIBLE, rcEdit, this, 39999);
	g_pDummyInput->SetFocus();


	CString strUrl;
	//switch ( theApp.m_pStartEvent->GetModuleFlag() )
	//{
	//case CPeraWebPageMgr::FW_RUNFLOW:
	//	strUrl = ((CWPERunFlowStart*)theApp.m_pStartEvent->GetExData())->m_strUrl;
	//	break;
	//case CPeraWebPageMgr::FW_VIEW_RUNFLOW:
	//	strUrl = ((CCxWPENavigate*)theApp.m_pStartEvent->GetExData())->m_strUrl;
	//	break;
	//case CPeraWebPageMgr::FW_TAB_VIEW:
	//	strUrl = ((CCxWPENavigate*)theApp.m_pStartEvent->GetExData())->m_strUrl;
	//	break;
	//default:
	//	break;
	//}

	CAttachWindowExData exData;
	if ( exData.Parse( g_aw.m_sExData ) )
	{
		strUrl = exData.m_sUrl;
	}
	else
	{
		ZTools::WriteZToolsLog( "CDlgMain::OnAwProcessAttached CAttachWindowExData ����ʧ��" );
	}

	ZTools::WriteZToolsFormatLog( "CDlgMain::OnAwProcessAttached m_pWeb->Navigate Url: %s\r\n", strUrl );
	
	m_pWeb->Navigate( (LPCTSTR)strUrl );

	ZTools::WriteZToolsFormatLog( "CDlgMain::OnAwProcessAttached end <<<<<<<<<<<<<<: %s\r\n", "\r\n" );
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

	// TODO: �ڴ˴������Ϣ����������
}

LRESULT CDlgMain::OnWebEventAsync( WPARAM wParam, LPARAM lParam )
{
	ZTools::WriteZToolsLog( "CDlgMain::OnWebEventAsync begin" );
	CWebEventData* pWebEventData = (CWebEventData*)wParam;
	if ( pWebEventData == NULL )
	{
		ZTools::WriteZToolsLog( "CDlgMain::OnWebEventAsync ����Ϊ��" );
		return 0;
	}
	if ( !theCommClient.Handle( pWebEventData ) )
	{
		ZTools::WriteZToolsLog( "CDlgMain::OnWebEventAsync Handle Failed" );
	}
	delete pWebEventData; pWebEventData = NULL;
	ZTools::WriteZToolsLog( "CDlgMain::OnWebEventAsync  end" );
	return 0;
}
