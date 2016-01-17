// DlgLoginMain.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgLoginMain.h"
#include "DlgLoginBk.h"
#include "ResLibImageSet.h"
#include "PeraDeskClient.h"
#include "MainFrm.h"
#include "PeraDeskClientUtility.h"
#include "LoginFunc.h"
#include "RegFunc.h"

// CDlgLoginMain 对话框

IMPLEMENT_DYNAMIC(CDlgLoginMain, CBCGPDialog)

	CDlgLoginMain::CDlgLoginMain(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgLoginMain::IDD, pParent)
{
}

CDlgLoginMain::~CDlgLoginMain()
{
}

void CDlgLoginMain::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_USERNAME, m_EditUser);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_EditPwd);
	DDX_Control(pDX, IDC_STATIC_ERROR, m_ErrorStatic);
	DDX_Control(pDX, IDC_BUTTON_LOGIN, m_btnLogin);
	DDX_Control(pDX, IDC_BUTTON_RESET, m_btnReset);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
}


BEGIN_MESSAGE_MAP(CDlgLoginMain, CBCGPDialog)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CDlgLoginMain::OnBnClickedButtonLogin)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CDlgLoginMain::OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgLoginMain::OnBnClickedButtonClose)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgLoginMain 消息处理程序

BOOL CDlgLoginMain::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	m_btnClose.ShowWindow(SW_HIDE);
	
	HBITMAP hBitmapBk = NULL;
	GetNewBitmap(GetDlgLoginResImageData(CCxStaticRes::RT_DLG_LOGIN_MAIN_BK)->m_strPath)->GetHBITMAP(Color::White, &hBitmapBk);
	SetBackgroundImage(hBitmapBk);

	CustomResizeDlg();

	CString	str = GetRegForDeskClient( "LastUser" );
	if (str.IsEmpty())
	{
		m_EditUser.SetFocus();
	}
	else
	{
		m_EditUser.SetWindowText (str);
		m_EditPwd.SetFocus();
	}
	m_ErrorStatic.ShowWindow(SW_HIDE);

	EnableVisualManagerStyle(TRUE, TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgLoginMain::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	::SystemParametersInfo(SPI_SETDRAGFULLWINDOWS,TRUE,NULL,0);
	GetParent()->PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
	CBCGPDialog::OnLButtonDown(nFlags, point);
}

void CDlgLoginMain::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CBCGPDialog::OnCancel();
}

void CDlgLoginMain::OnBnClickedButtonLogin()
{
	CString strUser, strPassword;
	m_EditUser.GetWindowText(strUser);
	m_EditPwd.GetWindowText(strPassword);

	if ( strUser.IsEmpty() || strPassword.IsEmpty() )
	{
		MessageBox("用户名或者密码不能为空", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return;
	}

	//登录
	CString sLoginData = Login(strUser, strPassword);
	if ( !theApp.m_LoginData.Parse(sLoginData) )
	{
		MessageBox(theApp.m_LoginData.m_strErrInfo, g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return;
	}

	WriteRegForDeskClient( _T("LastUser"), theApp.m_LoginData.m_strUser );

	//theApp.WriteSectionString (_T("Flag"), _T("LastUser"), theApp.m_LoginData.m_strUser);

	((CDlgLoginBk*)GetParent())->OnMyOk();
	EndDialog(IDOK);
	CMainFrame* pMainFrame = NULL;
	pMainFrame = dynamic_cast<CMainFrame*>(theApp.m_pMainWnd);
	if (pMainFrame)
	{
		pMainFrame->m_wndLoginBar.RedrawWindow();
	}
	return;
}

void CDlgLoginMain::OnBnClickedButtonReset()
{
	// TODO: 在此添加控件通知处理程序代码
	m_EditUser.SetWindowText(_T(""));
	m_EditPwd.SetWindowText(_T(""));
	m_EditUser.SetFocus();
}

void CDlgLoginMain::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CBCGPDialog::OnOK();
}

void CDlgLoginMain::OnBnClickedButtonClose()
{
	// TODO: 在此添加控件通知处理程序代码
	CBCGPDialog::OnCancel();
	((CDlgLoginBk*)GetParent())->OnMyCancle();
}

BOOL CDlgLoginMain::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	//输入时鼠标消失的问题
	if ((pMsg->hwnd == m_EditUser.GetSafeHwnd())
		|| (pMsg->hwnd == m_EditPwd.GetSafeHwnd()))
	{
		if (pMsg->message == WM_KEYDOWN)
		{
			CPoint point;
			GetCursorPos(&point);
			CRect rectUser;
			m_EditUser.GetWindowRect(&rectUser);
			CRect rectPwd;
			m_EditPwd.GetWindowRect(&rectPwd);
			if (!rectUser.PtInRect(point) && !rectPwd.PtInRect(point))
			{
				SetCursorPos(point.x, point.y);
			}

			if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_TAB)
			{
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
				SetCursorPos(point.x, point.y);
			}
		}
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}

void CDlgLoginMain::CustomResizeDlg()
{
	if (!IsWindow(GetSafeHwnd())
		|| !IsWindow(m_btnLogin.GetSafeHwnd())
		|| !IsWindow(m_btnReset.GetSafeHwnd())
		|| !IsWindow(m_btnClose.GetSafeHwnd())
		|| !IsWindow(m_EditUser.GetSafeHwnd())
		|| !IsWindow(m_EditPwd.GetSafeHwnd())
		)
	{
		return;
	}

	CRect rcClient;
	GetClientRect(&rcClient);

	m_btnLogin.MoveWindow(110, 180, 84, 28);
	m_btnReset.MoveWindow(200, 180, 69, 28);
	m_EditUser.MoveWindow(110, 90, 160, 24);
	m_EditPwd.MoveWindow(110, 135, 160, 24);
}


void CDlgLoginMain::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CustomResizeDlg();
}
