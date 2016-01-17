// DlgForBase.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgForBase.h"
#include "afxdialogex.h"
#include "DlgParamSetBase.h"


// CDlgForBase 对话框

IMPLEMENT_DYNAMIC(CDlgForBase, CCxBCGPDialog)

CDlgForBase::CDlgForBase(CWnd* pParent /*=NULL*/)
	: CCxBCGPDialog(CDlgForBase::IDD, pParent)
{
	m_brush.CreateSolidBrush(RGB(251, 251, 251));
}

CDlgForBase::~CDlgForBase()
{
}

void CDlgForBase::DoDataExchange(CDataExchange* pDX)
{
	CCxBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgForBase, CCxBCGPDialog)
	//ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CDlgForBase 消息处理程序


HBRUSH CDlgForBase::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if(nCtlColor == CTLCOLOR_DLG)
	{
		return m_brush;
	}

	HBRUSH hbr = CCxBCGPDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


BOOL CDlgForBase::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CBrush backBrush(RGB(251, 251, 251));

	//保存旧的brush
	CBrush* pOldBrush = pDC->SelectObject(&backBrush);
	CRect rect;
	pDC->GetClipBox(&rect);

	//画需要的区域
	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
	pDC->SelectObject(pOldBrush);

	return TRUE;

	return CCxBCGPDialog::OnEraseBkgnd(pDC);
}


void CDlgForBase::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CCxBCGPDialog::OnOK();
}


void CDlgForBase::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CCxBCGPDialog::OnCancel();
}

ZTreeParam* CDlgForBase::GetParamByName( LPCTSTR strName )
{
	vector<ZTreeParam*>& vParams = ((CDlgParamSetBase*)GetParent())->m_wndGridTree.m_vParams;

	for(int i = 0; i < (int)vParams.size(); i++)
	{
		if(vParams[i]->m_strName == strName)
		{
			return vParams[i];
		}
	}
	return NULL;
}

void CDlgForBase::AddStringToComboBox(int nID, LPCTSTR str)
{
	AddStringToComboBox((CComboBox*)GetDlgItem(nID), str);
}

void CDlgForBase::DeleteStringFromComboBox(int nID, LPCTSTR str)
{
	DeleteStringFromComboBox((CComboBox*)GetDlgItem(nID), str);
}

void CDlgForBase::SelectStringFromComboBox(int nID, LPCTSTR str)
{
	SelectStringFromComboBox((CComboBox*)GetDlgItem(nID), str);
}

CString CDlgForBase::GetSelStrFromComboBox(int nID)
{
	return GetSelStrFromComboBox((CComboBox*)GetDlgItem(nID));
}

void CDlgForBase::AddStringToComboBox(CComboBox* pBox, LPCTSTR str)
{
	int n;
	if((n = pBox->FindStringExact(0, str)) == -1)
	{
		pBox->AddString(str);
	}
}

void CDlgForBase::DeleteStringFromComboBox(CComboBox* pBox, LPCTSTR str)
{
	int n;
	if((n = pBox->FindStringExact(0, str)) != -1)
	{
		int nSelect = pBox->GetCurSel();
		pBox->DeleteString(n);
		if(nSelect == n)
			pBox->RedrawWindow();
	}
}

void CDlgForBase::SelectStringFromComboBox(CComboBox* pBox, LPCTSTR str)
{
	int n;
	if((n = pBox->FindStringExact(0, str)) != -1)
	{
		pBox->SetCurSel(n);
		pBox->RedrawWindow();
	}
}

CString CDlgForBase::GetSelStrFromComboBox(CComboBox* pBox)
{
	CString str;
	int nSelect = pBox->GetCurSel();
	if(nSelect != -1)
	{
		pBox->GetLBText(nSelect, str);
	}
	return str;
}