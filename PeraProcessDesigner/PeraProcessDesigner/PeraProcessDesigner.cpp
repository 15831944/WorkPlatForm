// PeraProcessDesigner.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "PeraProcessDesignerDoc.h"
#include "PeraProcessDesignerView.h"
#include "CxDataType.h"
//#include "CxArray3D.h"
#include "ResLibImageSet.h"
#include "CxArrayND.h"
#include "DlgStart.h"
#include <ReadRobotDB.h>
#include "CxBCGPXMLSettings.h"
#include "CxBCGPVisualManager2007.h"
#include "Tools.h"
#include "GetFlexwareBin.h"
#include "CrashHandler.h"
#include "KeepTicketFunc.h"
#include "LicenseMgr.h"
#include "peraicebase.h"
#include "PeraProcessDesignerUtility.h"
#include "LoadingWindow.h"
#include "GetGlobalDataFromReg.h"
#include "UtilityFunc.h"
#include "AppFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CReadRobotDB g_ReadRobotDB;
// CPeraProcessDesignerApp

BEGIN_MESSAGE_MAP(CPeraProcessDesignerApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
END_MESSAGE_MAP()


// CPeraProcessDesignerApp construction

CPeraProcessDesignerApp::CPeraProcessDesignerApp() :
	CBCGPWorkspace (TRUE /* m_bResourceSmartUpdate */)
#ifdef USE_DLGLOG
		, m_dlgLog("", "��־����")
#endif // USE_DLGLOG
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_pCurViewData = NULL;
	m_pBarJianmoTree = NULL;
	m_pBarJianmoComponentLib = NULL;
	m_pDlgResLibTree = NULL;
	m_pDlgResLibImage = NULL;
	m_pDlgDragLayer = NULL;
	
	m_nSpliterViewFlag = 0;
	//m_nFlagFillBarBackground = 0;
	m_bInitMDI = FALSE;

	m_hwndRunFlow = NULL;
	m_hwndViewRunFlow = NULL;

	m_bCreate = FALSE;
	m_hMetux = NULL;

	m_bAloneVersionWs = FALSE;

	m_bGetClientSubFlowId = FALSE;

#ifdef PROJECT_805
	//m_strProject = "805";
	//805�����޸ģ����ڽ�ģ�й���רҵ���ݿ⣬����805�������û����
#else
	m_strProject = "611";
#endif
}


// The one and only CPeraProcessDesignerApp object

CPeraProcessDesignerApp theApp;
/*
����ģ�͸��� WebC ��ʱ���ݡ�
���� WebContainer ����ʱ ��������ʼ���˱�����
OnAttaching ʱ���� �˱������ݣ�
view OnCreate ʱ����ͼ��չ����ָ��õ�ַ���˱���ָ��գ�
view OnDestroy ʱ�����ڴ档
*/
CAttachWindow* g_pawRunProcess = NULL;


// CPeraProcessDesignerApp initialization

BOOL CPeraProcessDesignerApp::InitInstance()
{
	ZTools::InitZToolsLog();

	if ( !m_CmdLine.Parse() )
	{
		MessageBox( NULL, "����������ʧ�ܣ�", g_lpszAppTitle, MB_OK | MB_TOPMOST );
		return FALSE;
	}

	m_hMetux = CreateMutex(NULL,TRUE,"PeraProcessDesigner.exe");
	if (m_hMetux)
	{
		if (ERROR_ALREADY_EXISTS== GetLastError())
		{
			HWND hwndPeraProcessDesignerCopied = FindPeraProcessDesignerMainWindow();
			//���л��⣬����û�ҵ�����ʱ����Ϊ֮ǰ�Ľ��̻��������У��򵥴���ֱ���˳�
			if ( hwndPeraProcessDesignerCopied)
			{
				if ( !m_CmdLine.GetValue( NULL ).IsEmpty() )
				{
#define WS_OPENWS_SENDMSG
#ifdef WS_OPENWS_SENDMSG
					DWORD dwProcessId = 0;
					GetWindowThreadProcessId(hwndPeraProcessDesignerCopied, &dwProcessId); 
					if ( GetTopModalWindow( dwProcessId ) == NULL )
					{
						CSharedMemory Mem;
						CString sMemData = g_lpszDoubleOpenWsMemStr;
						Mem.Init( g_lpszDoubleOpenWsMemName, sMemData.GetLength()+MAX_PATH );
						SendCopyData( hwndPeraProcessDesignerCopied, CPMSG_WORKSPACE_MAKESUREINFO, (LPVOID)NULL, 0 );
						sMemData.Empty();
						sMemData = (LPCTSTR)Mem.GetData();
						if ( sMemData.CompareNoCase( g_lpszDoubleOpenWsMemStr ) == 0 )
						{
							MessageBox( NULL, "��ģ�������ڻ״̬�����ȱ���ģ�ͺ����ԣ�", g_lpszAppTitle, MB_OK | MB_TOPMOST );
						}
						else
						{
							CString sCmdLine = ::GetCommandLine();
							SendCopyData( hwndPeraProcessDesignerCopied, CPMSG_WORKSPACE_OPENWS, (LPVOID)(LPCTSTR)sCmdLine, sCmdLine.GetLength()+1 );
						}
					}
					else
					{
						MessageBox( NULL, "��ģ�������ڻ״̬�����ȱ���ģ�ͺ����ԣ�", g_lpszAppTitle, MB_OK | MB_TOPMOST );
					}
#else
					MessageBox( hwndPeraProcessDesignerCopied, "��ģ�����Ѵ򿪣����ڽ�ģ�����д򿪱��ļ���", g_lpszAppTitle, MB_OK | MB_TOPMOST );
#endif
				}
				if (IsIconic(hwndPeraProcessDesignerCopied)) 
					ShowWindow(hwndPeraProcessDesignerCopied,SW_RESTORE);

				SetForegroundWindow(hwndPeraProcessDesignerCopied);
				ZTools::WriteZToolsFormatLog("���Ѿ����еĽ�ģ���ڼ���,��ǰ����ʾ...");

			}
			CloseHandle(m_hMetux);
			m_hMetux = NULL;
			ZTools::WriteZToolsFormatLog("�Ѿ�����һ����ģ�ͻ���,������ʵ�����˳�...");

			return FALSE;
		}
	}

	m_LoginData.m_strRealName = m_CmdLine.GetValue( "realName" );
	m_LoginData.m_strUser = m_CmdLine.GetValue( "userName" );
	m_LoginData.m_strTicket = m_CmdLine.GetValue( "ticket-proxy" );

	WriteShareMemoryLoginInfo();

	CCrashHandler ch;
	ch.SetProcessExceptionHandlers();
	ch.SetThreadExceptionHandlers();

	//���PeraTaskService���̲����ڣ��Զ�����
	StartPeraTaskService();

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

	if (!InitLicense("PeraWorkSpace"))
		return FALSE;
// 	if(!m_FlexNetMgr.CheckOutLicense("PeraWorkSpace"))
// 	{
// 		return FALSE;
// 	}



//	hModule = ::LoadLibrary(ZTools::FormatString("%s\\PeraLicMgr.dll", GetFlexwareBinPathMethod()).c_str());
//	if( hModule != NULL ) 
//	{
//		lpFnDLLfunc = (LPFNDLLFUNC) ::GetProcAddress(hModule, "CheckOutLicense");
//		if(lpFnDLLfunc != NULL)
//		{
//			if(!lpFnDLLfunc("PeraProcessDesigner"))
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
//		AfxMessageBox("��⵽ϵͳ��װ������������ϵ����Ա���°�װ��", MB_OK|MB_ICONINFORMATION);
//		return FALSE;
//	}
//#endif
	
	//if(!m_FlexNetMgr.InitNewJob("PeraProcessDesigner"))
	//{
	//	return FALSE;
	//}
	
	CWinApp::InitInstance();

	InitGDIPlus();
	LoadResLibImageSet();
	//LoadDefaultFont();

	//CBCGPPopupMenuBar::SetPeraPaint( g_crToolbarBg );

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

	//Barͦ����Slider��ȣ�����Splitter��С���Ϊ4�����Դ�ֵ��ӦС��4��������Splitter��Э����
	CBCGPSlider::m_nDefaultWidth = 4;

	// All registry read/write operations will be produced via CBCGPXMLSettings gateway:
	CBCGPRegistrySP::SetRuntimeClass (RUNTIME_CLASS (CBCGPXMLSettings));

	// Read settings:
	CBCGPXMLSettings::ReadXMLFromFile (FALSE, GetExecDir() + _T("\\user.xml"));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	//SetRegistryKey(_T("PeraProcessDesigner"));
	//LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

	//SetRegistryBase (_T("Settings"));

	RECT rectDesktop;
	SystemParametersInfo(SPI_GETWORKAREA,0,(PVOID)&rectDesktop,0);
	m_rcLastWindowPlacement = rectDesktop;
	CRect rectDesktop2 = rectDesktop;
	int nFlag = 0;
	int nShowCmd = 0;
	LoadWindowPlacement( m_rcLastWindowPlacement, nFlag, nShowCmd );

	StoreWindowPlacement (rectDesktop2, 2, SW_SHOWMAXIMIZED);

	theGlobalConfig.Load();

	m_bAloneVersionWs = IsPeraProcessRunnerInstalled();

	CCxBCGPVisualManager2007::SetStyle (CCxBCGPVisualManager2007::VS2007_Silver);
	CCxBCGPVisualManager2007::SetDefaultManager (RUNTIME_CLASS (CCxBCGPVisualManager2007));

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
		RUNTIME_CLASS(CPeraProcessDesignerDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CPeraProcessDesignerView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAIN))
		return FALSE;
	
	m_bCreate = TRUE;

	m_pMainWnd = pMainFrame;

	//basehtmldialog�У����ο�ݼ�
	g_hwndMainWindow = pMainFrame->GetSafeHwnd();

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

	pMainFrame->OnMenuModuleOpenjianmo();

	m_bInitMDI = TRUE;
	m_nCmdShow = SW_SHOWMAXIMIZED;
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	//////////////////////////////////////////////////////////////////////////
	//��־�������
#ifdef USE_DLGLOG
	m_dlgLog.Create(CDlgLog::IDD, NULL);
	//m_dlgLog.ShowWindow(SW_SHOW);

// 	int x, y; 
// 	x = GetSystemMetrics(SM_CXFULLSCREEN); //��Ļ��� 
// 	y = GetSystemMetrics(SM_CYFULLSCREEN); //��Ļ�߶�  

	CRect rectFull;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rectFull,0);

	CRect rect;
	m_dlgLog.GetWindowRect(rect);
	rect.MoveToXY(0, rectFull.bottom - rect.Height());
	m_dlgLog.MoveWindow(rect);
#endif // USE_DLGLOG
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//�ӳ�Ʊ����Ч��
	StartKeepTicketThread();
	//////////////////////////////////////////////////////////////////////////
	//���߱༭�õ��Ĺ����ڴ��ʼ��
	m_pSharedMemoryOnlineEdit = NULL;
	m_pSharedMemoryOnlineEdit = new SharedMemoryOnlineEdit();
	//////////////////////////////////////////////////////////////////////////
	g_ReadRobotDB.Load();

	pMainFrame->m_wndLoginBar.RedrawWindow();

	CString sParamFile = m_CmdLine.GetValue( NULL );
	if ( !sParamFile.IsEmpty() )
	{
		ZTools::WriteZToolsFormatLog( "--------------********���ļ�[%s]**********-------------", sParamFile );
		theApp.m_processMgr.Open( sParamFile );
	}
	else
	{
		ZTools::WriteZToolsLog( "--------------********δ���ļ��߼�**********-------------" );
		CDlgStart lgn;
		lgn.DoModal();
	}

#ifdef USE_DLGLOG
	m_dlgLog.ShowWindow(SW_SHOW);
#endif // USE_DLGLOG

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

// CPeraProcessDesignerApp message handlers

int CPeraProcessDesignerApp::ExitInstance() 
{
	// Save settings:
	CBCGPXMLSettings::WriteXMLToFile (FALSE, GetExecDir() +_T("\\user.xml"));

	//����ICE�ӿ���ض���
	ClearPeraIce();

	//////////////////////////////////////////////////////////////////////////
	//���߱༭�õ��Ĺ����ڴ�����
	if (theApp.m_pSharedMemoryOnlineEdit)
	{
		delete theApp.m_pSharedMemoryOnlineEdit;
		theApp.m_pSharedMemoryOnlineEdit = NULL;
	}
	//////////////////////////////////////////////////////////////////////////

	ClearResLibImageSet();

	BCGCBProCleanUp();
	//////////////////////////////////////////////////////////////////////////
	//�ӳ�Ʊ����Ч��
	StopKeepTicketThread();
	//�ر�ʱע����ǰƱ��
	if (!m_LoginData.m_strTicket.IsEmpty())
	{
		//20140403�������άȷ�ϣ���ǰ�汾�½�ģ�͸��˹���ƽֱ̨�ӹر�ʱ��ע���û���ֻ���û����ע��ʱ��ע����lc
		//Logout(m_LoginData.m_strTicket);
		m_LoginData.Clear();
	}
	DeleteShareMemoryLoginInfo(); // �˳�����ʱɾ�������ڴ�
	//if(lpFnDLLfunc != NULL)
	//{
	//	::FreeLibrary(hModule);
	//}
	if (m_hMetux)
	{
		CloseHandle(m_hMetux);
		m_hMetux = NULL;
	}
	return CWinApp::ExitInstance();
}

void CPeraProcessDesignerApp::OnFileNew()
{
	CWinApp::OnFileNew();
}



// CAboutDlg dialog used for App About
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
BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CString strVersion =  GetCurrentFileVersion();
	CString strBuildInfo = CString(" ����ʱ�䣺")+ CString(BuilTime) + CString("\n �汾�ţ�  ")+ strVersion;
	SetDlgItemText(IDC_STATIC_BUIL_INFO,strBuildInfo);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CPeraProcessDesignerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CPeraProcessDesignerApp message handlers


void CPeraProcessDesignerApp::PreLoadState ()
{

	//GetMouseManager()->AddView (IDR_PeraProcessDesignerTYPE, _T("My view"), IDR_PeraProcessDesignerTYPE);

	// TODO: add another views and windows were mouse double click
	// customization is required

	//GetContextMenuManager()->AddMenu (_T("My menu"), IDR_CONTEXT_MENU);

	// TODO: add another context menus here
}



void CPeraProcessDesignerApp::LoadDefaultFont()
{
	return;

	m_font.DeleteObject();

	LOGFONT logfont;
	memset(&logfont, NULL, sizeof(LOGFONT));
	logfont.lfCharSet = ANSI_CHARSET;
	strcpy(logfont.lfFaceName, "����");
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
BOOL CPeraProcessDesignerApp::ReloadWindowPlacement (CFrameWnd* pFrameWnd)
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