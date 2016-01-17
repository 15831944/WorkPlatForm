// ZComboBox.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "ZComboBox.h"
#include "WndIf.h"
#include "DlgWndIf.h"
#include "ZButtonDraw.h"

CString CZComboBox::g_strEditText = "";
BranchExpress CZComboBox::g_BranchExpress;
// CZComboBox

IMPLEMENT_DYNAMIC(CZComboBox, CBCGPComboBox)

CZComboBox::CZComboBox()
{
	g_strEditText.Empty();
	g_BranchExpress.Clear();
}

CZComboBox::~CZComboBox()
{
	g_strEditText.Empty();
	g_BranchExpress.Clear();
}


BEGIN_MESSAGE_MAP(CZComboBox, CBCGPComboBox)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, &CZComboBox::OnCbnSelchange)
END_MESSAGE_MAP()



// CZComboBox 消息处理程序




void CZComboBox::OnCbnSelchange()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlgWndIf* pWndParent = ((CDlgWndIf*)GetParent());
	CWnd* pDlg = pWndParent->GetParent();
	int i = GetCurSel();
	if (pWndParent->m_nCurSelect==i)
		return; // 没有切换，则不走后续逻辑，否则，会把保存的表达式切丢失

	pWndParent->m_nCurSelect = i;
	
	if(i == 0)
	{//else if
		pWndParent->m_vBCs[pWndParent->m_vBCs.size() - 1]->m_editExp.SetReadOnly(TRUE);
		pWndParent->m_vBCs[pWndParent->m_vBCs.size() - 1]->m_btnEditor.EnableWindow(TRUE);
		pWndParent->m_vBCs[pWndParent->m_vBCs.size() - 1]->m_editExp.SetWindowText(g_strEditText);
		pWndParent->m_vBCs[pWndParent->m_vBCs.size() - 1]->m_BranchExpress = g_BranchExpress;
	}
	else
	{
		pWndParent->m_vBCs[pWndParent->m_vBCs.size() - 1]->m_editExp.GetWindowText(g_strEditText);
		g_BranchExpress = pWndParent->m_vBCs[pWndParent->m_vBCs.size() - 1]->m_BranchExpress;

		pWndParent->m_vBCs[pWndParent->m_vBCs.size() - 1]->m_editExp.SetReadOnly(TRUE);
		pWndParent->m_vBCs[pWndParent->m_vBCs.size() - 1]->m_editExp.SetWindowText(NULL);
		pWndParent->m_vBCs[pWndParent->m_vBCs.size() - 1]->m_btnEditor.EnableWindow(FALSE);
		pWndParent->m_vBCs[pWndParent->m_vBCs.size() - 1]->m_editExp.m_strText = "";
		pWndParent->m_vBCs[pWndParent->m_vBCs.size() - 1]->m_BranchExpress.Clear();
	}

	CZButtonDraw* pBtnUp = (CZButtonDraw*)pDlg->GetDlgItem(IDC_BUTTON_UP);
	CZButtonDraw* pBtnDown = (CZButtonDraw*)pDlg->GetDlgItem(IDC_BUTTON_DOWN);

	int nCurSelect = pWndParent->m_nCurSelect;
	int nMax;
	int nIndex = pWndParent->m_nBranchSelect;

	if(nIndex < 0)
	{//无焦点
		return;
	}

	if(nCurSelect == 0)
	{//if OR else if
		nMax = pWndParent->m_vBCs.size() - 1;
	}
	else
	{//else
		nMax = pWndParent->m_vBCs.size() - 2;
	}

	if(nIndex == 0)
	{//第一个
		pBtnUp->EnableWindow(FALSE);
	}
	else
	{
		pBtnUp->EnableWindow(TRUE);
		if(nCurSelect == 1 && nIndex == pWndParent->m_vBCs.size() - 1)
		{
			pBtnUp->EnableWindow(FALSE);
		}
	}

	if(nIndex >= nMax)
	{//倒数第1个
		pBtnDown->EnableWindow(FALSE);
	}
	else
	{
		pBtnDown->EnableWindow(TRUE);
	}

}
