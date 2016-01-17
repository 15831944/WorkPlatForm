// MsgDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraDeskClient.h"
#include "MsgDlg.h"
#include "afxdialogex.h"


// CMsgDlg 对话框

IMPLEMENT_DYNAMIC(CMsgDlg, CCxBCGPDialog)

CMsgDlg::CMsgDlg(CWnd* pParent /*=NULL*/)
	: CCxBCGPDialog(CMsgDlg::IDD, pParent)
{

}

CMsgDlg::~CMsgDlg()
{
}

void CMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CCxBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMsgDlg, CCxBCGPDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_NCDESTROY()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CMsgDlg 消息处理程序


int CMsgDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CCxBCGPDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	if (m_CoreDlg.m_hWnd==NULL)
	{
		m_CoreDlg.Create(CMsgCoreDlg::IDD,this);
		m_CoreDlg.ShowWindow(SW_SHOW);
	}
	return 0;
}


void CMsgDlg::OnSize(UINT nType, int cx, int cy)
{
	CCxBCGPDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (m_CoreDlg.m_hWnd)
	{
		m_CoreDlg.SetWindowPos(NULL,0,0,cx,cy,SWP_NOZORDER);
	}
}


void CMsgDlg::OnNcDestroy()
{
	CCxBCGPDialog::OnNcDestroy();

	// TODO: 在此处添加消息处理程序代码
}


BOOL CMsgDlg::OnInitDialog()
{
	CCxBCGPDialog::OnInitDialog();

	SetWindowPos(NULL,0,0,800,600,SWP_NOZORDER|SWP_NOMOVE);
	// TODO:  在此添加额外的初始化
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CMsgDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ShowWindow(SW_HIDE);
	return;

	//CCxBCGPDialog::OnClose();
}
void CMsgDlg::RefreshMsg()
{
	if (m_CoreDlg.m_hWnd == NULL)
		return;

	m_CoreDlg.OpeMsgUrl();
}
void CMsgDlg::OpenUrl(LPCTSTR strUrl)
{
	if (m_CoreDlg.m_hWnd == NULL)
		return;

	if (strUrl==NULL)
		return;

	m_CoreDlg.Navigate(strUrl);
}