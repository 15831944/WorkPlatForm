// DlgForLimit.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgForLimit.h"
#include "afxdialogex.h"


// CDlgForLimit 对话框

IMPLEMENT_DYNAMIC(CDlgForLimit, CDlgForBase)

CDlgForLimit::CDlgForLimit(CWnd* pParent /*=NULL*/)
	: CDlgForBase(pParent)
{

}

CDlgForLimit::~CDlgForLimit()
{
}

void CDlgForLimit::DoDataExchange(CDataExchange* pDX)
{
	CDlgForBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgForLimit, CDlgForBase)
	ON_CBN_SELCHANGE(IDC_COMBO, &CDlgForLimit::OnCbnSelchangeCombo)
	ON_EN_KILLFOCUS(IDC_EDIT, &CDlgForLimit::OnEnKillfocusEdit)
END_MESSAGE_MAP()


// CDlgForLimit 消息处理程序


BOOL CDlgForLimit::OnInitDialog()
{
	CDlgForBase::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_bContinue = TRUE;

	m_nConut = 10;

	((CBCGPComboBox*)GetDlgItem(IDC_COMBO))->SetCurSel(0);

	CString str;
	str.Format("%d", m_nConut);

	((CBCGPEdit*)GetDlgItem(IDC_EDIT))->SetWindowText(str);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgForLimit::OnCbnSelchangeCombo()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bContinue = !((CBCGPComboBox*)GetDlgItem(IDC_COMBO))->GetCurSel();
}


void CDlgForLimit::OnEnKillfocusEdit()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	((CBCGPEdit*)GetDlgItem(IDC_EDIT))->GetWindowText(str);
	m_nConut = atoi(str);
}
