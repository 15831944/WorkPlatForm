
// 601DemoClient.cpp : ����Ӧ�ó��������Ϊ��
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
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()


// CMy601DemoClientApp ����

CMy601DemoClientApp::CMy601DemoClientApp()
{
	m_bHiColorIcons = TRUE;
	m_bUseIp = TRUE;
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// ���Ӧ�ó��������ù�����������ʱ֧��(/clr)�����ģ���:
	//     1) �����д˸������ã�������������������֧�ֲ�������������
	//     2) ��������Ŀ�У������밴������˳���� System.Windows.Forms ������á�
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: ������Ӧ�ó��� ID �ַ����滻ΪΨһ�� ID �ַ�����������ַ�����ʽ
	//Ϊ CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("601DemoClient.AppID.NoVersion"));

	m_bUseIp = TRUE;
	m_strServerPort = "20131";
	m_strServerIp = "127.0.0.1";
	m_strServerDomain = "localhost";
	m_strLocalServicePort = "20133";
	m_strConfigFile.Empty();
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

// Ψһ��һ�� CMy601DemoClientApp ����

CMy601DemoClientApp theApp;


// CMy601DemoClientApp ��ʼ��

BOOL CMy601DemoClientApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// ʹ�� RichEdit �ؼ���Ҫ  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	LoadStdProfileSettings(4);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	if (!GetConfigFromIni()) // �����ļ��е�������Ϣ������ʱ�ŵ������öԻ���
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

	// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MENU_MAIN_CLIENT,
		RUNTIME_CLASS(CMy601DemoClientDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ��ܴ���
		RUNTIME_CLASS(CMy601DemoClientView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// ������׼ shell ���DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// ��������������ָ����������
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// �������к�׺ʱ�ŵ��� DragAcceptFiles
	//  �� SDI Ӧ�ó����У���Ӧ�� ProcessShellCommand ֮����
	return TRUE;
}

int CMy601DemoClientApp::ExitInstance()
{
	//TODO: �����������ӵĸ�����Դ
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CMy601DemoClientApp ��Ϣ�������


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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

// �������жԻ����Ӧ�ó�������
void CMy601DemoClientApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CMy601DemoClientApp �Զ������/���淽��

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

// CMy601DemoClientApp ��Ϣ�������

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