// ZButton.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "ZButton.h"
#include "WndIf.h"
#include "DlgExpEditor.h"
#include "DlgWndIf.h"
#include "DlgIf.h"
#include "IfExpressEditorDlg.h"

// CZButton
int CZButton::m_staCount = 0;

IMPLEMENT_DYNAMIC(CZButton, CBCGButton)

CZButton::CZButton()
{
	m_index = m_staCount ++;
}

CZButton::~CZButton()
{
}


BEGIN_MESSAGE_MAP(CZButton, CBCGButton)
	ON_CONTROL_REFLECT(BN_CLICKED, &CZButton::OnBnClicked)
END_MESSAGE_MAP()



// CZButton 消息处理程序




void CZButton::OnBnClicked()
{
	// TODO: 在此添加控件通知处理程序代码

	CDlgWndIf* pWndParent = ((CDlgWndIf*)GetParent());
	CWnd* pDlg = pWndParent->GetParent();
	CString strSymBol = pWndParent->m_vBCs[m_index]->m_BranchExpress.m_strExpressSymbol;
	CIfExpressEditorDlg dlg(pWndParent->m_vBCs[m_index]->m_BranchExpress,this);
	if(dlg.DoModal() != IDOK)
		return;

	pWndParent->m_vBCs[m_index]->m_editExp.SetWindowText(dlg.m_Express.m_strFullPathExpress);
	pWndParent->m_vBCs[m_index]->m_BranchExpress = dlg.m_Express;
}
