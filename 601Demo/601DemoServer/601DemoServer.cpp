
// 601DemoServer.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "601DemoServer.h"
#include "MainFrm.h"
#include <string>
#include "601DemoServerDoc.h"
#include "601DemoServerView.h"
#include "ServerSetDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy601DemoServerApp

BEGIN_MESSAGE_MAP(CMy601DemoServerApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CMy601DemoServerApp::OnAppAbout)
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()


// CMy601DemoServerApp ����

CMy601DemoServerApp::CMy601DemoServerApp()
{
	m_bHiColorIcons = TRUE;

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
	SetAppID(_T("601DemoServer.AppID.NoVersion"));
	m_strConfigFile.Empty();
	m_strServerPort = "20131";
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

// Ψһ��һ�� CMy601DemoServerApp ����

CMy601DemoServerApp theApp;


// CMy601DemoServerApp ��ʼ��

BOOL CMy601DemoServerApp::InitInstance()
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

	if (!GetConfigFromIni())
	{
		CServerSetDlg dlg;
		dlg.m_strServerPort = m_strServerPort;
		if (dlg.DoModal()!=IDOK)
			return FALSE;

		m_strServerPort = dlg.m_strServerPort;
		WriteSettingToIni();
	}
	
	// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MENU_MAIN_SERVER,
		RUNTIME_CLASS(CMy601DemoServerDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ��ܴ���
		RUNTIME_CLASS(CMy601DemoServerView));
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

int CMy601DemoServerApp::ExitInstance()
{
	//TODO: �����������ӵĸ�����Դ
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CMy601DemoServerApp ��Ϣ�������


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
void CMy601DemoServerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CMy601DemoServerApp �Զ������/���淽��

void CMy601DemoServerApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CMy601DemoServerApp::LoadCustomState()
{
}

void CMy601DemoServerApp::SaveCustomState()
{
}

// CMy601DemoServerApp ��Ϣ�������

void CMy601DemoServerApp::TraceRunLog(CString strLogMsg)
{

	CMainFrame * pMain = (CMainFrame *)GetMainWnd();
	if (pMain == NULL) return;

	if (!::IsWindow(pMain->m_hWnd)) return;
		
	CMy601DemoServerView * pView = (CMy601DemoServerView*)pMain->GetActiveView();

	if (pView == NULL) return;
	if (!::IsWindow(pView->m_hWnd)) return;

	std::string *pstrMsg = new std::string;
	*pstrMsg = strLogMsg;
	pView->PostMessage(WM_LOG_MSG,0,(LPARAM)pstrMsg);
}
CString CMy601DemoServerApp::GetConfigFilePath()
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
BOOL CMy601DemoServerApp::GetConfigFromIni()
{
	CString strConfigFile = GetConfigFilePath();
	CString strServerPort;
	GetPrivateProfileString("601DemoServer","ServerPort","",strServerPort.GetBuffer(1000),1000,strConfigFile);
	strServerPort.ReleaseBuffer();


	if (strServerPort.IsEmpty())
		return FALSE;

	m_strServerPort = strServerPort;
	return TRUE;
}
void CMy601DemoServerApp::WriteSettingToIni()
{
	CString strConfigFile = GetConfigFilePath();

	WritePrivateProfileString("601DemoServer","ServerPort",m_strServerPort,strConfigFile);
}