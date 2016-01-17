
// ICEClientDemoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ICEClientDemo.h"
#include "ICEClientDemoDlg.h"
#include "afxdialogex.h"
#include "InterfaceData.h"
#include "Xpdl.h"
#include <Strsafe.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CICEClientDemoDlg �Ի���


/*
����Ŀ¼
*/
static CString GetCacheDirectory()
{
#if 1
	char szModule[MAX_PATH*2] = {0};
	GetModuleFileName( NULL, szModule, MAX_PATH*2 );
	if ( !PathFileExists( szModule ) )
	{
		return "";
	}
	char* pTemp = strrchr( szModule, '\\' );
	if ( pTemp == NULL )
	{
		return "";
	}
	szModule[ pTemp-szModule ] = '\0';
	StringCbCat( szModule, MAX_PATH*2, "\\RunCache" );
	if ( !PathFileExists( szModule ) )
	{
		if ( !MakeSureDirectoryPathExists( szModule ) )
		{
			return "";
		}
	}
	return szModule;
#else
	CString sCacheDirectory = GetPeraGlobalDataFromReg( "LM", "PeraProcessRunner" );
	if ( sCacheDirectory.GetAt( sCacheDirectory.GetLength()-1 ) != '\\' )
	{
		sCacheDirectory += "\\";
	}
	sCacheDirectory += "RunCache";
	return sCacheDirectory;
#endif
}


CICEClientDemoDlg::CICEClientDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CICEClientDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pDlgProgress = NULL;
}

void CICEClientDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, LIST_INFO, m_list);
	DDX_Control(pDX, IDC_COMBO1, m_combox);
}

BEGIN_MESSAGE_MAP(CICEClientDemoDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(BTN_ZIP, &CICEClientDemoDlg::OnBnClickedZip)
	ON_BN_CLICKED(BTN_UNZIP, &CICEClientDemoDlg::OnBnClickedUnzip)
	ON_BN_CLICKED(BTN_COPY, &CICEClientDemoDlg::OnBnClickedCopy)
	ON_BN_CLICKED(BTN_DELETE, &CICEClientDemoDlg::OnBnClickedDelete)
	ON_BN_CLICKED(BTN_STOP, &CICEClientDemoDlg::OnBnClickedStop)
	ON_WM_SIZE()
	ON_MESSAGE( WM_ICE_MESSAGE, OnIceMessage )
	ON_MESSAGE( WM_PROGRESS_SHOW, OnShowProgress )
	ON_BN_CLICKED(BTN_CLEAR, &CICEClientDemoDlg::OnBnClickedClear)
	ON_BN_CLICKED(BTN_CREATEWS, &CICEClientDemoDlg::OnBnClickedCreatews)
	ON_BN_CLICKED(BTN_CREATECACHE, &CICEClientDemoDlg::OnBnClickedCreatecache)
	ON_BN_CLICKED(BTN_JIANMO, &CICEClientDemoDlg::OnBnClickedJianmo)
END_MESSAGE_MAP()


// CICEClientDemoDlg ��Ϣ�������

BOOL CICEClientDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_list.InsertColumn( 0, "��Ϣ", LVCFMT_CENTER, 1000 );

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CICEClientDemoDlg::OnPaint()
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
HCURSOR CICEClientDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CICEClientDemoDlg::OnBnClickedZip() //����
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	int nCurSel = m_combox.GetCurSel();
	if ( nCurSel == -1 )
	{
		MessageBox( "���������б�ѡ�������е�����" );
		return;
	}
	CString sRunId;
	m_combox.GetLBText( nCurSel, sRunId );


	CFileDialog dlgFile(FALSE
		, "ws"
		, NULL
		, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT
		, "Process File (*.ws)|*.ws||"
		, theApp.m_pMainWnd
		);

	int nRet = dlgFile.DoModal();

	if (nRet == IDCANCEL)
	{
		ZTools::WriteZToolsFormatLog("------OnBnClickedZip Cancel---------------\n" );

		return ;
	}
	else if (nRet == IDOK)
	{
		CString sFileName = dlgFile.GetFileName();
		CString sFilePath = dlgFile.GetPathName();
		if ( !m_IceClient_I.CreateWSByCache( sRunId, sFilePath ) )
		{
			ZTools::WriteZToolsLog( "CICEClientDemoDlg::OnBnClickedZip() Failed \n" ) ;
			return;
		}
		CString sMsg;
		sMsg.Format( "ѹ��Ŀ¼%s��%s�ļ���ʼ", sRunId, sFileName );
		InsertListData( sMsg );
	}
}


void CICEClientDemoDlg::OnBnClickedUnzip() //��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	CFileDialog dlgFile(TRUE
		, "ws"
		, NULL
		, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT
		, "Process File (*.ws)|*.ws||"
		, theApp.m_pMainWnd
		);

	int nRet = dlgFile.DoModal();

	if (nRet == IDCANCEL)
	{
		ZTools::WriteZToolsFormatLog("------OnBnClickedUnzip Cancel---------------\n" );

		return ;
	}
	else if (nRet == IDOK)
	{
		if( ( (CButton*)GetDlgItem( CHKBTN_SHOWWAIT ) )->GetCheck() )
		{
			PostMessage( WM_PROGRESS_SHOW, (WPARAM)TRUE );
		}
		CString sFileName = dlgFile.GetFileName();
		CString sFilePath = dlgFile.GetPathName();
		std::string sRunId = m_IceClient_I.GetRunId();
		if ( sRunId.size() <= 0 )
		{
			return;
		}
		if ( !m_IceClient_I.CreateCacheByWS( sRunId.c_str(), sFilePath ) )
		{
			PostMessage( WM_PROGRESS_SHOW, (WPARAM)FALSE );
			ZTools::WriteZToolsLog( "CICEClientDemoDlg::OnBnClickedUnzip() Failed \n" ) ;
			return;
		}
		CString sMsg;
		sMsg.Format( "��ѹ%s������Ŀ¼%s��ʼ", sFileName, sRunId.c_str() ); m_combox.AddString( sRunId.c_str() );
		InsertListData( sMsg );
	}
}


void CICEClientDemoDlg::OnBnClickedCopy() //������
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nCurSel = m_combox.GetCurSel();
	if ( nCurSel == -1 )
	{
		MessageBox( "���������б�ѡ�������е�����" );
		return;
	}
	CString sOrgRunId;
	m_combox.GetLBText( nCurSel, sOrgRunId );

	std::string sRet = m_IceClient_I.GetRunId();
	if ( sRet.size() <= 0 )
	{
		return;
	}

	if ( !m_IceClient_I.CopyCache( sOrgRunId, sRet.c_str() ) )
	{
		ZTools::WriteZToolsLog( "CICEClientDemoDlg::OnBnClickedCopy() Failed \n" ) ;
		return;
	}
	CString sMsg;
	sMsg.Format( "��������%s��%s��ʼ", sOrgRunId, sRet.c_str() );m_combox.AddString( sRet.c_str() );
	InsertListData( sMsg );
}


void CICEClientDemoDlg::OnBnClickedDelete()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������


	int nCurSel = m_combox.GetCurSel();
	if ( nCurSel == -1 )
	{
		MessageBox( "���������б�ѡ�������е�����" );
		return;
	}
	CString sRunId;
	m_combox.GetLBText( nCurSel, sRunId );
	if ( !m_IceClient_I.DeleteCache( sRunId ) )
	{
		ZTools::WriteZToolsLog( "CICEClientDemoDlg::OnBnClickedCopy() Failed \n" ) ;
		return;
	}
	CString sMsg;
	sMsg.Format( "ɾ������%s��ʼ", sRunId );
	InsertListData( sMsg );
}


void CICEClientDemoDlg::OnBnClickedStop()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������


	int nCurSel = m_combox.GetCurSel();
	if ( nCurSel == -1 )
	{
		MessageBox( "���������б�ѡ�������е�����" );
		return;
	}
	CString sRunId;
	m_combox.GetLBText( nCurSel, sRunId );
	m_IceClient_I.Stop( sRunId );
	CString sMsg;
	sMsg.Format( "��ֹ%s��ʼ", sRunId );
	InsertListData( sMsg );
}

BOOL CICEClientDemoDlg::InsertListData( LPCTSTR lpszItemData )
{
	if ( m_list.GetSafeHwnd() == NULL )
	{
		return FALSE;
	}
	if ( lpszItemData == NULL )
	{
		return FALSE;
	}
	CTime t = CTime::GetCurrentTime();
	CString sMsg;
	sMsg.Format( " %04d-%02d-%02d %02d:%02d:%02d %s ",t.GetYear(), t.GetMonth(), t.GetDay(), t.GetHour(), t.GetMinute(), t.GetSecond(), lpszItemData );
	int nCount = m_list.GetItemCount();
	m_list.InsertItem( nCount, sMsg );
	return TRUE;
}


void CICEClientDemoDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������

	if ( m_list.GetSafeHwnd() != NULL )
	{
		CRect rcClient;
		m_list.GetClientRect( rcClient );
		m_list.SetColumnWidth( 0, rcClient.Width() );
	}
}

LRESULT CICEClientDemoDlg::OnIceMessage( WPARAM wparam, LPARAM lparam )
{
	CString sRet = (char*)wparam;
	CString sFlag = (LPCTSTR)lparam;
	delete [] (char*)wparam;
	delete [] (char*)lparam;
	CString sMsg;
	sMsg.Format( "�յ����������ص���Ϣ��json�ַ�������%s", sRet );
	InsertListData( sMsg );
	if ( sFlag.CompareNoCase( "open" ) == 0 )
	{
		PostMessage( WM_PROGRESS_SHOW, (WPARAM)FALSE );
		CInterfaceData_CacheServer clsInter;
		if ( !clsInter.Parse( sRet ) )
		{
			ZTools::WriteZToolsLog( "OnIceMessage ����ʧ��\n" );
			return -1;
		}
		if ( clsInter.m_sErrMsg.IsEmpty() )
		{
		//	m_combox.AddString( clsInter.m_sRunId );
		}
	}
	else if ( sFlag.CompareNoCase( "resume" ) == 0 )
	{
		CInterfaceData_CacheServer clsInter;
		if ( !clsInter.Parse( sRet ) )
		{
			ZTools::WriteZToolsLog( "OnIceMessage ����ʧ��\n" );
			return -1;
		}
		if ( clsInter.m_sErrMsg.IsEmpty() )
		{
		//	m_combox.AddString( clsInter.m_sNewRunId );
		}
	}
	else if ( sFlag.CompareNoCase( "delete" ) == 0 )
	{
		CInterfaceData_CacheServer clsInter;
		if ( !clsInter.Parse( sRet ) )
		{
			ZTools::WriteZToolsLog( "OnIceMessage ����ʧ��\n" );
			return -1;
		}
		CString sLbText;
		int nTotal = m_combox.GetCount();
		for ( int i = 0; i < nTotal; i++ )
		{
			m_combox.GetLBText( i, sLbText );
			if ( sLbText.CompareNoCase( clsInter.m_sRunId ) == 0 )
			{
				m_combox.DeleteString( i );
				m_combox.RedrawWindow();
				break;
			}
		}
	}
	return 0;
}


void CICEClientDemoDlg::OnBnClickedClear()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if ( m_list.GetSafeHwnd() == NULL )
	{
		return;
	}
	m_list.DeleteAllItems();
}

LRESULT CICEClientDemoDlg::OnShowProgress( WPARAM wParam, LPARAM )
{
	if (wParam == TRUE)
	{
		StartProgress();
	}
	else
	{
		EndProgress();
	}
	return 0;
}

void CICEClientDemoDlg::StartProgress()
{
	if(m_pDlgProgress != NULL) return;
	UpdateWindow();

	EnableWindow(FALSE); //�����仰�ŵ� m_pBCGPProgressDlg �����ĺ��棬������ m_pBCGPProgressDlg����ʱ�������� CMainFrame �����ܵ�����Ӧ�ó��򴰿ڵĺ��档
	m_pDlgProgress = new CDlgProgress(this);
	m_pDlgProgress->Create(CDlgProgress::IDD, this);
	m_pDlgProgress->ShowWindow(SW_SHOW);

	this->SetFocus();
}

void CICEClientDemoDlg::EndProgress()
{
	SetForegroundWindow();
	if (m_pDlgProgress != NULL)
	{
		m_pDlgProgress->DestroyWindow();
		delete m_pDlgProgress;
		m_pDlgProgress = NULL;
	}
	EnableWindow(TRUE);
}

static LPCTSTR lpszMorenDir;

static	int CALLBACK  BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData)
{
	switch(uMsg)
	{
	case BFFM_INITIALIZED:    //��ʼ����Ϣ
		::SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)lpszMorenDir);   //  m_filePath Ϊ��ľ�̬����
		break;
	case BFFM_SELCHANGED:    //ѡ��·���仯��
		{
			TCHAR curr[MAX_PATH];   
			SHGetPathFromIDList((LPCITEMIDLIST)lParam,curr);   
			::SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)curr);   
		}
		break;
	default:
		break;
	}
	return 0;   
}
void CICEClientDemoDlg::OnBnClickedCreatecache()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	char szPath[MAX_PATH*2] = {0};
	ZeroMemory(szPath, sizeof(szPath));  
	CString sCacheDir = GetCacheDirectory();
	lpszMorenDir = sCacheDir;
	BROWSEINFO bi;   
	bi.hwndOwner = m_hWnd;   
	bi.pidlRoot = NULL;   
	bi.pszDisplayName = szPath;   
	bi.lpszTitle = "��ѡ����Ҫ���Ե�Ŀ¼��";   
	bi.ulFlags = 0;   
	bi.lpfn = BrowseCallbackProc ;   
	bi.lParam = long(&sCacheDir);   
	bi.iImage = 0;   
	LPITEMIDLIST lp = SHBrowseForFolder(&bi);   

	if(lp && SHGetPathFromIDList(lp, szPath))   
	{
		sCacheDir = szPath;
		sCacheDir += "\\main.xpdl";
		if ( !PathFileExists( sCacheDir ) )
		{
			MessageBox( "��ȷ��ѡ���Ŀ¼�ǻ���Ŀ¼", "tips", MB_OK | MB_TOPMOST );
			return;
		}
	}

	CXpdl xpdl;
	
	if( !xpdl.Load( sCacheDir ) )
	{
		return;
	}
	if ( !xpdl.Parse() )
	{
		return;
	}
	if ( !xpdl.ConvertToAbsolute() )
	{
		return ;
	}
	string sStream;

	xpdl.GetXpdlStream( sStream );

	string sRunId = m_IceClient_I.GetRunId();

	if( !m_IceClient_I.CreateCacheByXpdlStream( sRunId.c_str(), sStream.c_str() ) )
	{
		return;
	}
	m_combox.AddString( sRunId.c_str() );
}


void CICEClientDemoDlg::OnBnClickedCreatews()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������


	char szPath[MAX_PATH*2] = {0};
	ZeroMemory(szPath, sizeof(szPath));  
	CString sCacheDir = GetCacheDirectory();
	lpszMorenDir = sCacheDir;
	BROWSEINFO bi;   
	bi.hwndOwner = m_hWnd;   
	bi.pidlRoot = NULL;   
	bi.pszDisplayName = szPath;   
	bi.lpszTitle = "��ѡ����Ҫ���Ե�Ŀ¼��";   
	bi.ulFlags = 0;   
	bi.lpfn = BrowseCallbackProc ;   
	bi.lParam = long(&sCacheDir);   
	bi.iImage = 0;   
	LPITEMIDLIST lp = SHBrowseForFolder(&bi);   

	if(lp && SHGetPathFromIDList(lp, szPath))   
	{
		sCacheDir = szPath;
		sCacheDir += "\\main.xpdl";
		if ( !PathFileExists( sCacheDir ) )
		{
			MessageBox( "��ȷ��ѡ���Ŀ¼�ǻ���Ŀ¼", "tips", MB_OK | MB_TOPMOST );
			return;
		}
	}

	CFileDialog dlgFile(FALSE
		, "ws"
		, NULL
		, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT
		, "Process File (*.ws)|*.ws||"
		, theApp.m_pMainWnd
		);

	int nRet = dlgFile.DoModal();

	if (nRet == IDCANCEL)
	{
		ZTools::WriteZToolsFormatLog("------OnBnClickedZip Cancel---------------\n" );

		return ;
	}
	else if (nRet == IDOK)
	{
		CString sFileName = dlgFile.GetFileName();
		CString sFilePath = dlgFile.GetPathName();

		CXpdl xpdl;

		if( !xpdl.Load( sCacheDir ) )
		{
			return;
		}
		if ( !xpdl.Parse() )
		{
			return;
		}
		if ( !xpdl.ConvertToAbsolute() )
		{
			return ;
		}
		string sStream;

		xpdl.GetXpdlStream( sStream );

		string sRunId = m_IceClient_I.GetRunId();

		if( !m_IceClient_I.CreateWSByXpdlStream( sRunId.c_str(), sFilePath, sStream.c_str() ) )
		{
			return;
		}
	}
}


void CICEClientDemoDlg::OnBnClickedJianmo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	int nCurSel = m_combox.GetCurSel();
	if ( nCurSel == -1 )
	{
		MessageBox( "���������б�ѡ�������е�����" );
		return;
	}
	CString sRunId;
	m_combox.GetLBText( nCurSel, sRunId );
	CString sSend;
	sSend.Format( "{\"runFlag\":\"modeling\",\"from\":\"��ģ\",\"runId\":\"%s\"}", sRunId );
	COPYDATASTRUCT CopyData = {0};
	CopyData.dwData = 3;
	CopyData.lpData = (LPVOID)(LPCTSTR)sSend;
	CopyData.cbData = sSend.GetLength()+1;

	HWND hRunner = ::FindWindowEx( NULL, NULL, NULL, NULL );
	while ( hRunner != NULL )
	{
		CString sTitle;
		::GetWindowText( hRunner, (LPSTR)(LPCTSTR)sTitle, MAX_PATH*2 );
		if ( sTitle.Find( "ִ�л���" ) >= 0 )
		{
			::SendMessage( hRunner, WM_COPYDATA, NULL, (WPARAM)&CopyData );
			break;
		}
		hRunner = ::GetNextWindow( hRunner, GW_HWNDNEXT );
	}
}