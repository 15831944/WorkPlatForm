// DlgForBase.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgForBase.h"
#include "afxdialogex.h"
#include "DlgParamSetBase.h"


// CDlgForBase �Ի���

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


// CDlgForBase ��Ϣ�������


HBRUSH CDlgForBase::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if(nCtlColor == CTLCOLOR_DLG)
	{
		return m_brush;
	}

	HBRUSH hbr = CCxBCGPDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}


BOOL CDlgForBase::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CBrush backBrush(RGB(251, 251, 251));

	//����ɵ�brush
	CBrush* pOldBrush = pDC->SelectObject(&backBrush);
	CRect rect;
	pDC->GetClipBox(&rect);

	//����Ҫ������
	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
	pDC->SelectObject(pOldBrush);

	return TRUE;

	return CCxBCGPDialog::OnEraseBkgnd(pDC);
}


void CDlgForBase::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

	//CCxBCGPDialog::OnOK();
}


void CDlgForBase::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���

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