// PeraConfigToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PeraConfigTool.h"
#include "PeraConfigToolDlg.h"
#include "CommonMethod.h"
#include "ConfigFunc.h"
#include "Tools.h"
#include <imagehlp.h>
#include<tlhelp32.h>
#include<Psapi.h>
#pragma  comment(lib,"psapi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CPeraConfigToolDlg dialog




CPeraConfigToolDlg::CPeraConfigToolDlg(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CPeraConfigToolDlg::IDD, pParent)
	, m_strCacheDir(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPeraConfigToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1_CACHE_DIR, m_strCacheDir);
}

BEGIN_MESSAGE_MAP(CPeraConfigToolDlg, CBCGPDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CPeraConfigToolDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_SEL_DIR, &CPeraConfigToolDlg::OnBnClickedBtnSelDir)
END_MESSAGE_MAP()


// CPeraConfigToolDlg message handlers

BOOL CPeraConfigToolDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_strCacheDir = GetConfig("Pera","RunCache");
	ZTools::WriteZToolsFormatLog("读取原始配置缓存路径为：【%s】",m_strCacheDir);
	m_strOldCacheDir = m_strCacheDir;
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPeraConfigToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CBCGPDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPeraConfigToolDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CBCGPDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPeraConfigToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPeraConfigToolDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (m_strCacheDir.IsEmpty())
	{
		MessageBox("缓存路径不能为空！","提示",MB_OK|MB_ICONINFORMATION);
		return ;
	}

	CString strTmp = m_strCacheDir;
	if (strTmp.Right(1)!="\\")
		strTmp.AppendChar('\\');

	if (MakeSureDirectoryPathExists(strTmp) == FALSE)
	{
		MessageBox("指定路径不存在，且不能创建！","提示",MB_OK|MB_ICONINFORMATION);
		return ;
	}
	if (m_strOldCacheDir.CollateNoCase(m_strCacheDir)==0)
	{
		return CBCGPDialog::OnOK(); // 配置未改变则不写入，也不提示
	}

	int iRev = MessageBox("为保证运行数据一致性，修改运行缓存路径将重启综合设计客户端，请先保存所有工作，然后继续！","提示",MB_OKCANCEL|MB_ICONQUESTION);
	if (iRev==IDCANCEL)
		return;

	KillStartProcess();
	BOOL bOK = SetConfig("Pera","RunCache",m_strCacheDir);
	CString strMsg;
	strMsg.Format("配置%s！",bOK ? "成功":"失败");
	MessageBox(strMsg,"提示",MB_OK|MB_ICONINFORMATION);
	if (!m_strPeraTaskServicePath.IsEmpty())
	{
		ShellExecute(NULL,"open",m_strPeraTaskServicePath,NULL,NULL,SW_SHOWNORMAL);
		ZTools::WriteZToolsFormatLog("重启PeraTaskService完成,路径【%s】",m_strPeraTaskServicePath);
	}
	

	CBCGPDialog::OnOK();
}


void CPeraConfigToolDlg::OnBnClickedBtnSelDir()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strDir = GetSelectedFolder();
	if (strDir.IsEmpty())
		return;

	m_strCacheDir = strDir;
	UpdateData(FALSE);
}
void CPeraConfigToolDlg::KillStartProcess()
{
	HANDLE           hSnapshot;   

	//创建结构体用来存放进程快照信息
	PROCESSENTRY32   processinfo;   
	processinfo.dwSize = sizeof(processinfo);   

	//进程快照获取进程列表
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  

	if( NULL == hSnapshot )  
		return ;   

	//从Snapshot（上面获得的进程快照）得到第一个进程记录信息
	BOOL bStatus = Process32First( hSnapshot, &processinfo );

	int nLen     = strlen( processinfo.szExeFile ) + 1;  


	while( bStatus )   
	{
		BOOL bKill = FALSE;
		if( _stricmp("PeraProcessDesigner.exe",processinfo.szExeFile) == 0 ) 
		{
			bKill = TRUE;	
		}
		else if( _stricmp("peracomponent.exe",processinfo.szExeFile) == 0 ) 
		{
			bKill = TRUE;	
		}
		else if( _stricmp("PeraProcessRunner.exe",processinfo.szExeFile) == 0 ) 
		{
			bKill = TRUE;		
		}
		else if( _stricmp("PeraTaskService.exe",processinfo.szExeFile) == 0 ) 
		{
			HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |PROCESS_VM_READ,FALSE, processinfo.th32ProcessID );
			GetModuleFileNameEx(hProcess,NULL,m_strPeraTaskServicePath.GetBuffer(MAX_PATH*4),MAX_PATH*4); 
			m_strPeraTaskServicePath.ReleaseBuffer();
			CloseHandle( hProcess );
			bKill = TRUE;		
		}
		else if( _stricmp("PeraProcessService.exe",processinfo.szExeFile) == 0 ) 
		{
			bKill = TRUE;	
		}
		else if( _stricmp("PeraComponentRunner.exe",processinfo.szExeFile) == 0 ) 
		{
			bKill = TRUE;	
		}
		if(bKill)
			KillProcess(processinfo.th32ProcessID); 
		
		//从Snapshot（上面获得的进程快照）得到下一个进程记录信息
		bStatus = Process32Next(hSnapshot,   &processinfo);   
	}   

	//关闭进程快照句柄
	CloseHandle(hSnapshot);
}