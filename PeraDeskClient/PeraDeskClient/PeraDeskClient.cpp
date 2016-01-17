// PeraDeskClient.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PeraDeskClient.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "PeraDeskClientDoc.h"
#include "PeraDeskClientView.h"
#include "CxDataType.h"
//#include "CxArray3D.h"
#include "ResLibImageSet.h"
//#include "CxArrayND.h"
//#include "DlgStart.h"
//#include <ReadRobotDB.h>
#include "CxBCGPXMLSettings.h"
#include "CxBCGPVisualManager2007.h"
#include "Tools.h"
//#include "GetFlexwareBin.h"
#include "CrashHandler.h"
//#include "KeepTicketFunc.h"
#include <UserDefine.h>
#include <UtilityFunc.h>

#include <DlgLoginBk.h>
#include "PeraDeskClientUtility.h"
#include "ConfigFunc.h"
#include "AppFunc.h"
#include "PtrVectorHeader.h"

#include "PeraIceBase.h"
#include "LoadingWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//extern CReadRobotDB g_ReadRobotDB;
// CPeraDeskClientApp

BEGIN_MESSAGE_MAP(CPeraDeskClientApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
END_MESSAGE_MAP()


// CPeraDeskClientApp construction

CPeraDeskClientApp::CPeraDeskClientApp() :
	CBCGPWorkspace (TRUE /* m_bResourceSmartUpdate */)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_pCurViewData = NULL;
	//m_pBarJianmoTree = NULL;
	//m_pBarJianmoComponentLib = NULL;
	//m_pDlgResLibTree = NULL;
	//m_pDlgResLibImage = NULL;
	//m_pDlgDragLayer = NULL;
	
	m_nSpliterViewFlag = 0;
	//m_nFlagFillBarBackground = 0;
	m_bInitMDI = FALSE;

	m_hwndRunFlow = NULL;
	m_hwndViewRunFlow = NULL;

	m_bCreate = FALSE;

	m_hMutex = NULL;
	m_bAppExisted = FALSE;
	m_bCloseByLogin = FALSE;

	m_pMsgDlg = NULL;

#ifdef PROJECT_805
	m_strProject = "805";
#else
	m_strProject = "611";
#endif
}


// The one and only CPeraDeskClientApp object

CPeraDeskClientApp theApp;


// CPeraDeskClientApp initialization

BOOL CPeraDeskClientApp::InitInstance()
{
	m_hMutex = CreateMutex(NULL, TRUE, m_pszAppName);
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		m_bAppExisted = TRUE;
	}

	ZTools::InitZToolsLog();

	//网页中启动协同工作台，使用自定义协议，此函数取出JSON串放入g_sParamLine变量中
	if (!ParseJsonCommandLine())
	{
		MessageBox(NULL, "解析命令行失败", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		CloseHandle( m_hMutex );
		return FALSE;
	}

	if ( !g_sParamLine.IsEmpty() )
	{
		if ( !m_cxPeraDCData.Parse( g_sParamLine ) )
		{
			MessageBox(NULL, "命令行参数异常，无法启动客户端", g_lpszAppTitle, MB_OK|MB_ICONERROR);
			CloseHandle( m_hMutex );
			return FALSE;
		}
	}

	HWND hwndPeraDeskClientCopied = FindPeraDeskClientMainWindow();
	//当有互斥，但是没找到窗口时，认为之前的进程还在启动中，简单处理，直接退出，不将新的URL传过去////////////////////////////////////////////////////////////////////////
	if (( hwndPeraDeskClientCopied == NULL && m_bAppExisted) || (hwndPeraDeskClientCopied!=NULL && ::IsWindowVisible(hwndPeraDeskClientCopied)== FALSE))
	{
		HWND hWndLogin = FindPeraDeskClientLonginWindow();
		CloseHandle( m_hMutex );
		if (hWndLogin == NULL)
			return FALSE; // 应该不会走到这里吧

		if ( m_cxPeraDCData.m_sTicket.IsEmpty() ) //不带登录参数的情况
		{
			if (IsIconic(hWndLogin))
				ShowWindow(hWndLogin,SW_RESTORE);

			SetForegroundWindow(hWndLogin);
			return FALSE;
		}

		SendCopyData( hWndLogin
			, CPMSG_LOGIN_DESKCLIENT
			, (LPVOID)(LPCTSTR)g_sParamLine
			, g_sParamLine.GetLength());

		return FALSE;
	}
	if ( hwndPeraDeskClientCopied != NULL )
	{
		if ( m_cxPeraDCData.m_sTicket.IsEmpty() )
		{
			//MessageBox(NULL, "协同工作台已经启动", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		}
		else
		{
			SendCopyData( hwndPeraDeskClientCopied
				, CPMSG_RUN_PERADESKCLIENT_BY_PERADC
				, (LPVOID)(LPCTSTR)g_sParamLine
				, g_sParamLine.GetLength() );
		}
		if (IsIconic(hwndPeraDeskClientCopied))
			ShowWindow(hwndPeraDeskClientCopied,SW_RESTORE);

		SetForegroundWindow(hwndPeraDeskClientCopied);
		CloseHandle( m_hMutex );
		return FALSE;
	}

	CCrashHandler ch;
	ch.SetProcessExceptionHandlers();
	ch.SetThreadExceptionHandlers();
	
	//_CrtSetBreakAlloc(1300);
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	//hModule = ::LoadLibrary("C:\\Users\\kunmiao-li\\Desktop\\TestBuild\\PeraLicMgr\\Release\\PeraLicMgr.dll");
//#ifndef _DEBUG

	//////////////////////////////////////////////////////////////////////////
	//协同工作台暂时不加授权，20140321，lc
// 	if (!InitLicense("PeraDeskClient"))
// 		return FALSE;
	//////////////////////////////////////////////////////////////////////////

// 	if(!m_FlexNetMgr.CheckOutLicense("PeraDeskClient"))
// 	{
// 		return FALSE;
// 	}



//	hModule = ::LoadLibrary(ZTools::FormatString("%s\\PeraLicMgr.dll", GetFlexwareBinPathMethod()).c_str());
//	if( hModule != NULL ) 
//	{
//		lpFnDLLfunc = (LPFNDLLFUNC) ::GetProcAddress(hModule, "CheckOutLicense");
//		if(lpFnDLLfunc != NULL)
//		{
//			if(!lpFnDLLfunc("PeraDeskClient"))
//			{
//				return FALSE;
//			}
//		}
//		else	
//		{
//			return FALSE;
//		}
//	}
//	else
//	{
//		AfxMessageBox("检测到系统安装不完整，请联系管理员重新安装！", MB_OK|MB_ICONINFORMATION);
//		return FALSE;
//	}
//#endif
	
	//if(!m_FlexNetMgr.InitNewJob("PeraDeskClient"))
	//{
	//	return FALSE;
	//}
	
	CWinApp::InitInstance();
	BOOL bOK = AddDropFileAuthorization();
	if (!bOK)
		ZTools::WriteZToolsLog("PeraDeskClient 增加拖放文件权限失败!");

	vector<PROCESSENTRY32 *> vData;
	if ( 0 == GetProcessEntry32(vData, "PeraDCNetDiskDaemon.exe" ) )
	{
		CString strPath;
		strPath.Format("%s\\%s", GetExecDir(), "PeraDCNetDiskDaemon.exe");
		::ShellExecute( NULL, "open", strPath, NULL, NULL, SW_HIDE );
	}

	PtrVectorClear(vData);

	InitGDIPlus();
	LoadResLibImageSet();
	//LoadDefaultFont();

	CBCGPPopupMenuBar::SetPeraPaint( g_crToolbarBg );

	CBCGPRegistrySP::SetRuntimeClass (RUNTIME_CLASS (CBCGPXMLSettings));

	// Read settings:
	//注释掉，不保留上次的状态
	//CBCGPXMLSettings::ReadXMLFromFile (FALSE, GetExecDir() + _T("\\user.xml"));

	m_bSaveState = FALSE;

	CCxBCGPVisualManager2007::SetStyle (CCxBCGPVisualManager2007::VS2007_Silver);
	CCxBCGPVisualManager2007::SetDefaultManager (RUNTIME_CLASS (CCxBCGPVisualManager2007));



	if ( m_cxPeraDCData.m_sTicket.IsEmpty() )
	{
		CDlgLoginBk dlgLogin;
		dlgLogin.DoModal();
		if ( theApp.m_LoginData.m_strTicket.IsEmpty() )
		{
			return FALSE;
		}
	}
	else
	{
		theApp.m_LoginData.m_strTicket = m_cxPeraDCData.m_sTicket;
		theApp.m_LoginData.m_strRealName = m_cxPeraDCData.m_sRealName;
		theApp.m_LoginData.m_strUser = m_cxPeraDCData.m_sUser;

	//	WriteSectionString (_T("Flag"), _T("LastUser"), m_LoginData.m_strUser);
	}

	//CXmlBase xml;
	//MSXML2::IXMLDOMDocument2Ptr pdoc = xml.GetXmlDocPtrByFile(GetExecDir() + "\\Manifest.xml");

	//MSXML2::IXMLDOMNodeListPtr pModels;
	//MSXML2::IXMLDOMNodePtr pModel;
	//long lModelCount;
	//HRESULT hr = S_OK;

	//map<CString, CString> mapAttrs;
	//map<CString, CString>::iterator iterAttrs;
	//CString str;

	//pModels = xml.GetNodeListPtr("/Manifest/Component", pdoc);
	//if (pModels != NULL)
	//{
	//	lModelCount = pModels->Getlength();
	//	for (long i=0; i<lModelCount; i++)
	//	{
	//		hr = pModels->get_item(i, &pModel);
	//		if (FAILED(hr) || NULL == pModel)
	//		{
	//			continue;
	//		}
	//		if (0 == xml.GetAttrsOfNode(mapAttrs, pModel))
	//		{
	//			continue;
	//		}

	//		iterAttrs = mapAttrs.find("RobotName");
	//		if (iterAttrs != mapAttrs.end()) str= iterAttrs->second;
	//	}
	//}

	//CCxArray3D a3d(DT_ARRAY3D_INT);
	//CString str3d = "[1,2,3,4,5,](4,3,2)"

	//CString str = "[1e150,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24](2,3,4)";
//	CString str = "[\"\\\\\\\",,1e150\",\"2\"](2)";
	//{
	//	CString str1;
	//	for (int a=0;a<2;a++)
	//	{
	//		for (int b=0;b<3;b++)
	//		{
	//			for (int c=0;c<4;c++)
	//			{

	//			}
	//		}
	//	}
	//}

	//
	//CxArrayND nd( DT_ARRAY1D_STRING );
	//nd.Parse( str );
	//vector<int> vPos;
	//vPos.push_back(0);
	////vPos.push_back(0);
	////vPos.push_back(0);
	//LPARAM lpData = nd.GetDataPtr( vPos );
	//CString * pData = reinterpret_cast< CString * > ( lpData );
	//nd.Clear();


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	EnableTaskBarInteraction(FALSE);

	globalData.SetDPIAware ();
	//globalData.bDisableAero = TRUE;

	//Bar挺靠后，Slider宽度，由于Splitter最小宽度为4，所以此值不应小于4，否则与Splitter不协调。
	CBCGPSlider::m_nDefaultWidth = 4;

	// All registry read/write operations will be produced via CBCGPXMLSettings gateway:
	





	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	//SetRegistryKey(_T("PeraDeskClient"));
	//LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

	//SetRegistryBase (_T("Settings"));

	RECT rectDesktop;
	SystemParametersInfo(SPI_GETWORKAREA,0,(PVOID)&rectDesktop,0);
	m_rcLastWindowPlacement = rectDesktop;
	CRect rectDesktop2 = rectDesktop;
	int nFlag = 0;
	int nShowCmd = 0;
	m_rcLastWindowPlacement.DeflateRect(
		(int)( m_rcLastWindowPlacement.Width() * 0.1 )
		, (int)( m_rcLastWindowPlacement.Height() * 0.1 )
		);
	LoadWindowPlacement( m_rcLastWindowPlacement, nFlag, nShowCmd );

	//StoreWindowPlacement (rectDesktop2, 2, SW_SHOWMAXIMIZED);

	theGlobalConfig.Load();

	//globalData.bDisableAero = TRUE;
	//globalData.bIsOSAlphaBlendingSupport = TRUE;

	// Initialize all Managers for usage. They are automatically constructed
	// if not yet present
	InitMouseManager();
	//InitContextMenuManager();
	InitKeyboardManager();

	// Enable user-defined tools. If you want allow more than 10 tools,
	// add tools entry to resources (ID_USER_TOOL11, ID_USER_TOOL12,...)
	EnableUserTools (ID_TOOLS_ENTRY, ID_USER_TOOL1, ID_USER_TOOL10);

	// TODO: Remove this if you don't want extended tooltips:
	InitTooltipManager();

	CBCGPToolTipParams params;
	params.m_bVislManagerTheme = TRUE;

	theApp.GetTooltipManager ()->SetTooltipParams (
		BCGP_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS (CBCGPToolTipCtrl),
		&params);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_MAIN,
		RUNTIME_CLASS(CPeraDeskClientDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CPeraDeskClientView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAIN))
		return FALSE;
	
	m_bCreate = TRUE;

	m_pMainWnd = pMainFrame;

	g_pLoadingWindow = new CLoadingWindow( TIMER_LOADING_WINDOW, IDD_DIALOG_LOGIN_BK, m_pMainWnd );
	g_pLoadingWindow->Create( IDD_DIALOG_LOGIN_BK, m_pMainWnd );
	g_pLoadingWindow->Init( GetExecDir() + "\\Res\\DlgProgress\\DlgProgress.png" );

	pMainFrame->m_wndMenuBar.LoadData();

	//pMainFrame->OnMenuModuleOpenjianmo();

	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd


	// Parse command line for standard shell commands, DDE, file open
	//CCommandLineInfo cmdInfo;
	//ParseCommandLine(cmdInfo);


	//if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)
	//{
	//	if (!pMainFrame->LoadMDIState (GetRegSectionPath ()) || 
	//		DYNAMIC_DOWNCAST(CMDIChildWnd, pMainFrame->GetActiveFrame()) == NULL)
	//	{
	//		if (!ProcessShellCommand(cmdInfo))
	//			return FALSE;
	//	}
	//}
	//else
	//{
	//	// Dispatch commands specified on the command line
	//	if (!ProcessShellCommand(cmdInfo))
	//		return FALSE;
	//}
	// The main window has been initialized, so show and update it

	//启动webpage页面，
	{
		CString strUrl;

#ifdef _DEBUG
		strUrl = GetConfig("PeraDeskClient", "test");
#endif
		CCxWPENavigate * pData = new CCxWPENavigate();

		if ( strUrl.IsEmpty() )
		{
			if ( m_cxPeraDCData.m_sUrl.IsEmpty() )
			{
				strUrl = GetConfig("PeraDeskClient", "TaskModel");
				if (
					strUrl.IsEmpty()
					|| theApp.m_LoginData.m_strTicket.IsEmpty()
					)
				{
					strUrl = "about:blank";
					//strUrl = "www.baidu.com";
				}
				else
				{
					theApp.m_cxPeraDCData.m_sUrl = strUrl;
					AttachTicket(strUrl, theApp.m_LoginData.m_strTicket);
				}
			}
			else
			{
				strUrl = m_cxPeraDCData.m_sUrl;
			}

			pData->m_strUrl = strUrl;
			AttachTicket( pData->m_strUrl, theApp.m_LoginData.m_strTicket );
		}
		else
		{
			pData->m_strUrl = strUrl;
		}

		if ( !theWebPageMgr.Require( CPeraWebPageMgrHost::FW_TABVIEW_TASK_MODEL
			, CPeraWebPageMgrHost::FE_START, pData ) )
		{
			//失败
		}
		delete pData; pData = NULL;
	}


	

	//pMainFrame->OnMenuModuleOpenjianmo();

	m_bInitMDI = TRUE;
	m_nCmdShow = SW_SHOWMAXIMIZED;
	//m_nCmdShow = SW_SHOWNORMAL;
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	//////////////////////////////////////////////////////////////////////////
	//延长票据有效期
//	StartKeepTicketThread();
	//////////////////////////////////////////////////////////////////////////
	//在线编辑用到的共享内存初始化
	//m_pSharedMemoryOnlineEdit = NULL;
//	m_pSharedMemoryOnlineEdit = new SharedMemoryOnlineEdit();
	//////////////////////////////////////////////////////////////////////////
	//g_ReadRobotDB.Load();
//	CDlgStart lgn;
//	lgn.DoModal();

	//////////////////////////////////////////////////////////////////////////
	WINDOWPLACEMENT wp;
	memset(&wp, NULL, sizeof(wp));
	wp.length = sizeof (WINDOWPLACEMENT);

	if (pMainFrame->GetWindowPlacement (&wp))
	{
		wp.rcNormalPosition = m_rcLastWindowPlacement;

		RECT rectDesktop;
		SystemParametersInfo(SPI_GETWORKAREA,0,(PVOID)&rectDesktop,0);
		OffsetRect(&wp.rcNormalPosition, -rectDesktop.left, -rectDesktop.top);

		pMainFrame->SetWindowPlacement (&wp);

	}

	return TRUE;
}

// CPeraDeskClientApp message handlers

int CPeraDeskClientApp::ExitInstance() 
{
	// Save settings:
	CBCGPXMLSettings::WriteXMLToFile (FALSE, GetExecDir() +_T("\\user.xml"));

	ClearPeraIce();
	if (m_pMsgDlg)
	{
		delete m_pMsgDlg;
		m_pMsgDlg = NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	//在线编辑用到的共享内存清理
	//if (theApp.m_pSharedMemoryOnlineEdit)
	//{
	//	delete theApp.m_pSharedMemoryOnlineEdit;
	//	theApp.m_pSharedMemoryOnlineEdit = NULL;
	//}
	//////////////////////////////////////////////////////////////////////////

	ClearResLibImageSet();

	BCGCBProCleanUp();
	//////////////////////////////////////////////////////////////////////////
	//延长票据有效期
	//StopKeepTicketThread();
	//关闭时注销当前票据
	if (!m_LoginData.m_strTicket.IsEmpty())
	{
//		Logout(m_LoginData.m_strTicket);
		m_LoginData.Clear();
	}
	//if(lpFnDLLfunc != NULL)
	//{
	//	::FreeLibrary(hModule);
	//}
	
	return CWinApp::ExitInstance();
}

void CPeraDeskClientApp::OnFileNew()
{
	CWinApp::OnFileNew();
}



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };
	CBCGPURLLinkButton m_btnURL;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMPANY_URL, m_btnURL);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CPeraDeskClientApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CPeraDeskClientApp message handlers


void CPeraDeskClientApp::PreLoadState ()
{

	//GetMouseManager()->AddView (IDR_PeraDeskClientTYPE, _T("My view"), IDR_PeraDeskClientTYPE);

	// TODO: add another views and windows were mouse double click
	// customization is required

	//GetContextMenuManager()->AddMenu (_T("My menu"), IDR_CONTEXT_MENU);

	// TODO: add another context menus here
}



void CPeraDeskClientApp::LoadDefaultFont()
{
	return;

	m_font.DeleteObject();

	LOGFONT logfont;
	memset(&logfont, NULL, sizeof(LOGFONT));
	logfont.lfCharSet = ANSI_CHARSET;
	strcpy(logfont.lfFaceName, "宋体");
	logfont.lfWeight = FW_NORMAL;
	logfont.lfHeight = 12;
	m_font.CreateFontIndirect(&logfont);
	globalData.SetMenuFont(&logfont, TRUE);

	//CFont * pFont = CFont::FromHandle (
	//	(HFONT) ::GetStockObject (DEFAULT_GUI_FONT)
	//	);

	//LOGFONT lf;
	//memset (&lf, 0, sizeof (LOGFONT));
	//pFont->GetObject(sizeof(LOGFONT), &lf);

	//globalData.SetMenuFont(&lf, TRUE);

	//globalData.fontRegular.DeleteObject ();
	//globalData.fontRegular.CreateFontIndirect( &logfont );

	globalData.fontCaption.DeleteObject ();
	globalData.fontCaption.CreateFontIndirect( &logfont );

	globalData.fontTooltip.DeleteObject ();
	globalData.fontTooltip.CreateFontIndirect( &logfont );

	//globalData.fontBold.DeleteObject ();
	//globalData.fontBold.CreateFontIndirect( &logfont );

	//globalData.fontDefaultGUIBold.DeleteObject ();
	//globalData.fontDefaultGUIBold.CreateFontIndirect( &logfont );

	globalData.fontUnderline.DeleteObject ();
	globalData.fontUnderline.CreateFontIndirect( &logfont );

	globalData.fontDefaultGUIUnderline.DeleteObject ();
	globalData.fontDefaultGUIUnderline.CreateFontIndirect( &logfont );

}

//***********************************************************************************
BOOL CPeraDeskClientApp::ReloadWindowPlacement (CFrameWnd* pFrameWnd)
{
	ASSERT_VALID (pFrameWnd);

	CCommandLineInfo cmdInfo;
	AfxGetApp ()->ParseCommandLine (cmdInfo);
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		//Don't show the main window if Application 
		//was run with /Embedding or /Automation.  
		return FALSE;
	}

	CRect rectNormal;
	int nFlags = 0;
	//int nShowCmd = SW_SHOWNORMAL;
	int nShowCmd = m_bInitMDI ? SW_SHOWNORMAL : SW_HIDE;

	if (LoadWindowPlacement (rectNormal, nFlags, nShowCmd))
	{
		nShowCmd = m_bInitMDI ? nShowCmd : SW_HIDE;
		WINDOWPLACEMENT wp;
		wp.length = sizeof (WINDOWPLACEMENT);

		if (pFrameWnd->GetWindowPlacement (&wp))
		{
			wp.rcNormalPosition = rectNormal;
			wp.showCmd = nShowCmd;

			RECT rectDesktop;
			SystemParametersInfo(SPI_GETWORKAREA,0,(PVOID)&rectDesktop,0);
			OffsetRect(&wp.rcNormalPosition, -rectDesktop.left, -rectDesktop.top);

			pFrameWnd->SetWindowPlacement (&wp);

			return TRUE;
		}
	}

	return FALSE;
}


HWND GetMainWnd( void )
{
	if ( theApp.m_pMainWnd == NULL ) return NULL;
	if ( !::IsWindow( theApp.m_pMainWnd->GetSafeHwnd() ) ) return NULL;
	return theApp.m_pMainWnd->GetSafeHwnd();
}

#include "BuilDefine.h"
#pragma comment(lib, "version.lib")
static const char BuilTime[] =
{
	BUILD_YEAR_CH0, BUILD_YEAR_CH1, BUILD_YEAR_CH2, BUILD_YEAR_CH3,
	'-',
	BUILD_MONTH_CH0, BUILD_MONTH_CH1,
	'-',
	BUILD_DAY_CH0, BUILD_DAY_CH1,
	' ',
	BUILD_HOUR_CH0, BUILD_HOUR_CH1,
	':',
	BUILD_MIN_CH0, BUILD_MIN_CH1,
	':',
	BUILD_SEC_CH0, BUILD_SEC_CH1,
	'\0'
};
CString GetCurrentFileVersion()
{
	char szFileName[500] ={0};
	GetModuleFileName(NULL,szFileName,500);
	DWORD dwSize = GetFileVersionInfoSize(szFileName,NULL);
	if (dwSize==0)
		return "";

	char *pVersionData = new char[dwSize];
	if (!GetFileVersionInfo(szFileName,0,dwSize,pVersionData))
	{
		delete pVersionData;
		return "";
	}

	UINT nQuerySize;
	DWORD* pTransTable;
	if (!::VerQueryValue(pVersionData, _T("\\VarFileInfo\\Translation"),(void **)&pTransTable, &nQuerySize) )
	{
		delete pVersionData;
		return "";
	}

	DWORD dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));

	LPVOID lpData;
	CString strValue, strBlockName;
	strBlockName.Format(_T("\\StringFileInfo\\%08lx\\%s"),dwLangCharset, _T("ProductVersion"));

	if ( ::VerQueryValue((void **)pVersionData, strBlockName.GetBuffer(0),&lpData, &nQuerySize))
		strValue = (LPCTSTR)lpData;

	strBlockName.ReleaseBuffer();

	delete pVersionData;
	return strValue;
}
BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CString strVersion =  GetCurrentFileVersion();
	CString strBuildInfo = CString(" 构建时间：")+ CString(BuilTime) + CString("\n 版本号：  ")+ strVersion;
	SetDlgItemText(IDC_STATIC_BUIL_INFO,strBuildInfo);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
