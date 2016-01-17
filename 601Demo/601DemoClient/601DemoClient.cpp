
// 601DemoClient.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "601DemoClient.h"
#include "MainFrm.h"

#include "601DemoClientDoc.h"
#include "601DemoClientView.h"
#include "ConnectionSetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy601DemoClientApp

BEGIN_MESSAGE_MAP(CMy601DemoClientApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CMy601DemoClientApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()


// CMy601DemoClientApp 构造

CMy601DemoClientApp::CMy601DemoClientApp()
{
	m_bHiColorIcons = TRUE;
	m_bUseIp = TRUE;
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则:
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("601DemoClient.AppID.NoVersion"));

	m_bUseIp = TRUE;
	m_strServerPort = "20131";
	m_strServerIp = "127.0.0.1";
	m_strServerDomain = "localhost";
	m_strLocalServicePort = "20133";
	m_strConfigFile.Empty();
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CMy601DemoClientApp 对象

CMy601DemoClientApp theApp;


// CMy601DemoClientApp 初始化

BOOL CMy601DemoClientApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// 使用 RichEdit 控件需要  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	if (!GetConfigFromIni()) // 配置文件中的配置信息不完整时才弹出配置对话框
	{
		CConnectionSetDlg dlg;
		dlg.m_strServerPort = m_strServerPort;
		dlg.m_strServerIp   = m_strServerIp ;
		dlg.m_strLocalPort  = m_strLocalServicePort;
		dlg.m_strServerDomain = m_strServerDomain;
		if (dlg.DoModal()!=IDOK)
			return FALSE;

		m_bUseIp = dlg.m_iUseIp==0 ? TRUE:FALSE;

		if (m_bUseIp)
			m_strServerIp  = dlg.m_strServerIp;
		else
			m_strServerDomain = dlg.m_strServerDomain;

		m_strServerPort = dlg.m_strServerPort;
		m_strLocalServicePort = dlg.m_strLocalPort;
		WriteSettingToIni();
	}

	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MENU_MAIN_CLIENT,
		RUNTIME_CLASS(CMy601DemoClientDoc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(CMy601DemoClientView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 SDI 应用程序中，这应在 ProcessShellCommand 之后发生
	return TRUE;
}

int CMy601DemoClientApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CMy601DemoClientApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CMy601DemoClientApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CMy601DemoClientApp 自定义加载/保存方法

void CMy601DemoClientApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CMy601DemoClientApp::LoadCustomState()
{
}

void CMy601DemoClientApp::SaveCustomState()
{
}

// CMy601DemoClientApp 消息处理程序

void CMy601DemoClientApp::TraceRunLog(CString strLogMsg)
{
	CMainFrame * pMain = (CMainFrame *)GetMainWnd();
	if (pMain == NULL) return;

	if (!::IsWindow(pMain->m_hWnd)) return;

	CMy601DemoClientView * pView = (CMy601DemoClientView*)pMain->GetActiveView();

	if (pView == NULL) return;
	if (!::IsWindow(pView->m_hWnd)) return;

	std::string *pstrMsg = new std::string;
	*pstrMsg = strLogMsg;
	pView->PostMessage(WM_LOG_MSG,0,(LPARAM)pstrMsg);
}
CString CMy601DemoClientApp::GetConfigFilePath()
{
	if (!m_strConfigFile.IsEmpty())
		return m_strConfigFile;

	char szDir[MAX_PATH*2] = {0};
	GetModuleFileName(AfxGetInstanceHandle(),szDir,MAX_PATH*2);
	CString strConfigINI = szDir;
	strConfigINI.MakeLower();
	strConfigINI = strConfigINI.Left(strConfigINI.ReverseFind('.') + 1) + "ini";
	m_strConfigFile = strConfigINI;
	return m_strConfigFile;
}
BOOL CMy601DemoClientApp::GetConfigFromIni()
{
	CString strConfigFile = GetConfigFilePath();
	CString strServerIp;
	GetPrivateProfileString("601DemoClient","ServerIp","",strServerIp.GetBuffer(1000),1000,strConfigFile);
	strServerIp.ReleaseBuffer();

	CString strServerDomain;
	GetPrivateProfileString("601DemoClient","ServerDomain","",strServerDomain.GetBuffer(1000),1000,strConfigFile);
	strServerDomain.ReleaseBuffer();

	CString strServerPort;
	GetPrivateProfileString("601DemoClient","ServerPort","",strServerPort.GetBuffer(1000),1000,strConfigFile);
	strServerPort.ReleaseBuffer();

	CString strUseIp;
	GetPrivateProfileString("601DemoClient","UseIp","",strUseIp.GetBuffer(50),50,strConfigFile);
	strUseIp.ReleaseBuffer();

	CString strLocalServicePort;
	GetPrivateProfileString("601DemoClient","LocalPort","",strLocalServicePort.GetBuffer(1000),1000,strConfigFile);
	strLocalServicePort.ReleaseBuffer();

	if (strServerIp.IsEmpty()
		|| strServerDomain.IsEmpty() 
		|| strServerPort.IsEmpty()
		|| strUseIp.IsEmpty()
		|| strLocalServicePort.IsEmpty())

		return FALSE;

	m_strServerIp = strServerIp;
	m_strServerDomain = strServerDomain; 
	m_strServerPort = strServerPort;
	m_bUseIp = atoi(strUseIp); 
	m_strLocalServicePort = strLocalServicePort; 
	return TRUE;
}
void CMy601DemoClientApp::WriteSettingToIni()
{
	CString strConfigFile = GetConfigFilePath();

	WritePrivateProfileString("601DemoClient","ServerIp",m_strServerIp,strConfigFile);
	WritePrivateProfileString("601DemoClient","ServerDomain",m_strServerDomain,strConfigFile);
	WritePrivateProfileString("601DemoClient","ServerPort",m_strServerPort,strConfigFile);
	WritePrivateProfileString("601DemoClient","UseIp",(m_bUseIp?"1":"0"),strConfigFile);
	WritePrivateProfileString("601DemoClient","LocalPort",m_strLocalServicePort,strConfigFile);
}