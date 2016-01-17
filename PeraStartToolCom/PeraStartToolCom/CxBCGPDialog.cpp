#include "StdAfx.h"
#include "CxBCGPDialog.h"
#include "resource.h"

IMPLEMENT_DYNAMIC(CCxBCGPDialog, CBCGPDialog)
	
BEGIN_MESSAGE_MAP(CCxBCGPDialog, CBCGPDialog)
END_MESSAGE_MAP()

CCxBCGPDialog::CCxBCGPDialog() :
	CBCGPDialog()
{
}
//*************************************************************************************
CCxBCGPDialog::CCxBCGPDialog (UINT nIDTemplate, CWnd *pParent/*= NULL*/) : 
	CBCGPDialog (nIDTemplate, pParent)
{
}
//*************************************************************************************
CCxBCGPDialog::CCxBCGPDialog (LPCTSTR lpszTemplateName, CWnd *pParentWnd/*= NULL*/) : 
	CBCGPDialog(lpszTemplateName, pParentWnd)
{
}

BOOL CCxBCGPDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	return __super::PreTranslateMessage(pMsg);
}


BOOL CCxBCGPDialog::OnInitDialog()
{
	__super::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle (TRUE, TRUE);
	SetIcon(AfxGetApp()->LoadIcon(IDI_ICON1),TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
