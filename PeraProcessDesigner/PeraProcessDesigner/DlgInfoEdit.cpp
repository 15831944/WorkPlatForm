// DlgInfoEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgInfoEdit.h"
#include "afxdialogex.h"


// CDlgInfoEdit 对话框

IMPLEMENT_DYNAMIC(CDlgInfoEdit, CCxBCGPDialog)

CDlgInfoEdit::CDlgInfoEdit(std::string str, std::string strCaption, int nMax, CWnd* pParent /*=NULL*/)
	: CCxBCGPDialog(CDlgInfoEdit::IDD, pParent)
{
	m_strText = str;
	m_strCaption = strCaption;
	m_nLimitText = nMax;
}

CDlgInfoEdit::~CDlgInfoEdit()
{
}

void CDlgInfoEdit::DoDataExchange(CDataExchange* pDX)
{
	CCxBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgInfoEdit, CCxBCGPDialog)
END_MESSAGE_MAP()


// CDlgInfoEdit 消息处理程序


void CDlgInfoEdit::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	CString str;
	GetDlgItem(IDC_EDIT)->GetWindowText(str);
	m_strText = str;

	CCxBCGPDialog::OnOK();
}


BOOL CDlgInfoEdit::OnInitDialog()
{
	CCxBCGPDialog::OnInitDialog();

	if(m_nLimitText >= 0)
	{
		((CEdit*)GetDlgItem(IDC_EDIT))->SetLimitText(m_nLimitText);
	}

	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);

	// TODO:  在此添加额外的初始化
	GetDlgItem(IDC_EDIT)->SetWindowText(m_strText.c_str());
	SetWindowText(m_strCaption.c_str());

	EnableVisualManagerStyle (TRUE, TRUE);

	GetDlgItem(IDC_EDIT)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
