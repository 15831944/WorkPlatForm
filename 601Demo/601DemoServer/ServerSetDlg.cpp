// ServerSetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "601DemoServer.h"
#include "ServerSetDlg.h"
#include "afxdialogex.h"


// CServerSetDlg 对话框

IMPLEMENT_DYNAMIC(CServerSetDlg, CDialogEx)

CServerSetDlg::CServerSetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CServerSetDlg::IDD, pParent)
	, m_strServerPort(_T(""))
{

}

CServerSetDlg::~CServerSetDlg()
{
}

void CServerSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SERVER_PORT, m_strServerPort);
	DDV_MaxChars(pDX, m_strServerPort, 5);
}


BEGIN_MESSAGE_MAP(CServerSetDlg, CDialogEx)
END_MESSAGE_MAP()


// CServerSetDlg 消息处理程序


BOOL CServerSetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME),FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
