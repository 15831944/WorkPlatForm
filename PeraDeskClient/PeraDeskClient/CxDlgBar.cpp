// CxDlgView.cpp : 实现文件
//

#include "stdafx.h"
#include "CxDlgBar.h"


// CxDlgView 对话框

vector<CxDlgBar *> CxDlgBar::m_vRef;

BEGIN_MESSAGE_MAP(CxDlgBar, CDialogEx)
	ON_BN_CLICKED(IDOK, &CxDlgBar::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CxDlgBar::OnBnClickedCancel)
	ON_WM_DESTROY()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CxDlgBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

IMPLEMENT_DYNAMIC(CxDlgBar, CDialogEx)

CxDlgBar::CxDlgBar(UINT nID, CWnd* pParent /*=NULL*/)
	: CDialogEx(nID, pParent)
{
	m_vRef.push_back(this);
}

CxDlgBar::~CxDlgBar()
{
	vector<CxDlgBar *>::iterator itr = find( m_vRef.begin(), m_vRef.end(), this );
	if (itr != m_vRef.end())
	{
		m_vRef.erase(itr);
	}
}

void CxDlgBar::OnBnClickedOk()
{
	//按 Enter 不关闭对话框
}

void CxDlgBar::OnBnClickedCancel()
{
	//::SendMessage(this->GetSafeHwnd(),BM_CLICK,IDCANCEL,IDCANCEL);
	//按 ESC 不关闭对话框
	//注意：此消息将自动屏蔽 WM_CLOSE 消息
}

BOOL CxDlgBar::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ModifyStyle(
		WS_POPUPWINDOW
		| WS_OVERLAPPEDWINDOW
		| WS_TILEDWINDOW
		| WS_SIZEBOX
		| WS_EX_APPWINDOW
		| WS_GROUP
		| WS_DLGFRAME
		, 
		WS_CLIPSIBLINGS
		| WS_CLIPCHILDREN
		);

	ModifyStyleEx(
		WS_EX_WINDOWEDGE
		| WS_EX_TOOLWINDOW
		| WS_EX_CLIENTEDGE
		| WS_EX_STATICEDGE
		| WS_EX_APPWINDOW
		| WS_EX_TOPMOST
		| WS_EX_LAYERED
		| WS_EX_DLGMODALFRAME
		, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CxDlgBar::OnDestroy()
{
	CDialogEx::OnDestroy();
}

void CxDlgBar::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
}
