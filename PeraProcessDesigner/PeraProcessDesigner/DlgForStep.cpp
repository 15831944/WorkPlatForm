// DlgForStep.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgForStep.h"
#include "afxdialogex.h"
#include "CStringChecker.h"


// CDlgForStep 对话框

IMPLEMENT_DYNAMIC(CDlgForStep, CDlgForBase)

CDlgForStep::CDlgForStep(CWnd* pParent /*=NULL*/)
	: CDlgForBase(pParent)
{

}

CDlgForStep::~CDlgForStep()
{
}

void CDlgForStep::DoDataExchange(CDataExchange* pDX)
{
	CDlgForBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgForStep, CDlgForBase)
	ON_CBN_SELCHANGE(IDC_COMBO_FIRST_VALUE, &CDlgForStep::OnCbnSelchangeComboFirstValue)
	ON_CBN_SELCHANGE(IDC_COMBO_END_VALUE, &CDlgForStep::OnCbnSelchangeComboEndValue)
	ON_CBN_SELCHANGE(IDC_COMBO_STEP, &CDlgForStep::OnCbnSelchangeComboStep)
	ON_EN_KILLFOCUS(IDC_EDIT_FIRST_VALUE, &CDlgForStep::OnEnKillfocusEditFirstValue)
	ON_EN_KILLFOCUS(IDC_EDIT_END_VALUE, &CDlgForStep::OnEnKillfocusEditEndValue)
	ON_CBN_KILLFOCUS(IDC_COMBO_STEP, &CDlgForStep::OnCbnKillfocusComboStep)
END_MESSAGE_MAP()

void CDlgForStep::AddParam( LPCTSTR str )
{
	ZTreeParam* pParam = GetParamByName(str);
	if(pParam)
	{
		if((pParam->m_strType == "Int" || pParam->m_strType == "Double") && (pParam->m_strDirection == "In" || pParam->m_strDirection == "InOut"))
		{
			AddStringToComboBox(IDC_COMBO_PARAM, str);
			AddStringToComboBox(IDC_COMBO_FIRST_VALUE, str);
			AddStringToComboBox(IDC_COMBO_END_VALUE, str);
			AddStringToComboBox(IDC_COMBO_STEP, str);
		}
	}
}

void CDlgForStep::ParamChanged( ZTreeParam* pParam )
{
	CString str = pParam->m_strName.c_str();
	if((pParam->m_strType == "Int" || pParam->m_strType == "Double") && (pParam->m_strDirection == "In" || pParam->m_strDirection == "InOut"))
	{
		AddStringToComboBox(IDC_COMBO_PARAM, str);
		AddStringToComboBox(IDC_COMBO_FIRST_VALUE, str);
		AddStringToComboBox(IDC_COMBO_END_VALUE, str);
		AddStringToComboBox(IDC_COMBO_STEP, str);
	}
	else
	{
		DeleteStringFromComboBox(IDC_COMBO_PARAM, str);
		DeleteStringFromComboBox(IDC_COMBO_FIRST_VALUE, str);
		DeleteStringFromComboBox(IDC_COMBO_END_VALUE, str);
		DeleteStringFromComboBox(IDC_COMBO_STEP, str);
	}
}


// CDlgForStep 消息处理程序


void CDlgForStep::OnCbnSelchangeComboFirstValue()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox* pBox = (CComboBox*)GetDlgItem(IDC_COMBO_FIRST_VALUE);

	int n = pBox->GetCurSel();
	CString str;
	pBox->GetLBText(n, str);

	if(str == "常量")
	{
		GetDlgItem(IDC_EDIT_FIRST_VALUE)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_EDIT_FIRST_VALUE)->ShowWindow(SW_HIDE);
	}

}


void CDlgForStep::OnCbnSelchangeComboEndValue()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox* pBox = (CComboBox*)GetDlgItem(IDC_COMBO_END_VALUE);

	int n = pBox->GetCurSel();
	CString str;
	pBox->GetLBText(n, str);

	if(str == "常量")
	{
		GetDlgItem(IDC_EDIT_END_VALUE)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_EDIT_END_VALUE)->ShowWindow(SW_HIDE);
	}
}


void CDlgForStep::OnCbnSelchangeComboStep()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox* pBox = (CComboBox*)GetDlgItem(IDC_COMBO_STEP);

	int n = pBox->GetCurSel();
	CString str;
	pBox->GetLBText(n, str);

	if(str == "常量")
	{
		GetDlgItem(IDC_EDIT_STEP)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_EDIT_STEP)->ShowWindow(SW_HIDE);
	}
}

void CDlgForStep::DeleteParam( LPCTSTR str )
{
	DeleteStringFromComboBox(IDC_COMBO_PARAM, str);
	DeleteStringFromComboBox(IDC_COMBO_FIRST_VALUE, str);
	DeleteStringFromComboBox(IDC_COMBO_END_VALUE, str);
	DeleteStringFromComboBox(IDC_COMBO_STEP, str);
}

void CDlgForStep::RenameParam( LPCTSTR strOld, LPCTSTR strNew )
{
	BOOL bSelectParam = FALSE;
	BOOL bSelectFirstValue = FALSE;
	BOOL bSelectEndValue = FALSE;
	BOOL bSelectStep = FALSE;

	CComboBox* pBox;
	CString str;
	int n;

	pBox = (CComboBox*)GetDlgItem(IDC_COMBO_PARAM);
	n = pBox->GetCurSel();
	if(n != -1)
	{
		pBox->GetLBText(n, str);
		if(str == strOld) bSelectParam = TRUE;
	}

	pBox = (CComboBox*)GetDlgItem(IDC_COMBO_FIRST_VALUE);
	n = pBox->GetCurSel();
	if(n != -1)
	{
		pBox->GetLBText(n, str);
		if(str == strOld) bSelectFirstValue = TRUE;
	}

	pBox = (CComboBox*)GetDlgItem(IDC_COMBO_END_VALUE);
	n = pBox->GetCurSel();
	if(n != -1)
	{
		pBox->GetLBText(n, str);
		if(str == strOld) bSelectEndValue = TRUE;
	}

	pBox = (CComboBox*)GetDlgItem(IDC_COMBO_STEP);
	n = pBox->GetCurSel();
	if(n != -1)
	{
		pBox->GetLBText(n, str);
		if(str == strOld) bSelectStep = TRUE;
	}

	DeleteStringFromComboBox(IDC_COMBO_PARAM, strOld);
	DeleteStringFromComboBox(IDC_COMBO_FIRST_VALUE, strOld);
	DeleteStringFromComboBox(IDC_COMBO_END_VALUE, strOld);
	DeleteStringFromComboBox(IDC_COMBO_STEP, strOld);

	AddStringToComboBox(IDC_COMBO_PARAM, strNew);
	AddStringToComboBox(IDC_COMBO_FIRST_VALUE, strNew);
	AddStringToComboBox(IDC_COMBO_END_VALUE, strNew);
	AddStringToComboBox(IDC_COMBO_STEP, strNew);

	if(bSelectParam) SelectStringFromComboBox(IDC_COMBO_PARAM, strNew);

	if(bSelectFirstValue) SelectStringFromComboBox(IDC_COMBO_FIRST_VALUE, strNew);

	if(bSelectEndValue) SelectStringFromComboBox(IDC_COMBO_END_VALUE, strNew);

	if(bSelectStep) SelectStringFromComboBox(IDC_COMBO_STEP, strNew);
}

BOOL CDlgForStep::CheckEditValue(int nID)
{
	CEdit* pEdit = (CEdit*)GetDlgItem(nID);
	CString str;
	pEdit->GetWindowText(str);
	if(str.IsEmpty()) return TRUE;

	if(!CStringChecker::CheckInt(str) && !CStringChecker::CheckDouble(str))
	{
		return FALSE;
	}
	return TRUE;
}

void CDlgForStep::OnEnKillfocusEditFirstValue()
{
	// TODO: 在此添加控件通知处理程序代码
	int nID = IDC_EDIT_FIRST_VALUE;
	if(!CheckEditValue(nID))
	{
		AfxMessageBox("常量必须是整型或浮点型数据！", MB_ICONWARNING);
		CEdit* pEdit = (CEdit*)GetDlgItem(nID);
		pEdit->SetWindowText("0");
	}
}


void CDlgForStep::OnEnKillfocusEditEndValue()
{
	// TODO: 在此添加控件通知处理程序代码
	int nID = IDC_EDIT_END_VALUE;
	if(!CheckEditValue(nID))
	{
		AfxMessageBox("常量必须是整型或浮点型数据！", MB_ICONWARNING);
		CEdit* pEdit = (CEdit*)GetDlgItem(nID);
		pEdit->SetWindowText("0");
	}
}


void CDlgForStep::OnCbnKillfocusComboStep()
{
	// TODO: 在此添加控件通知处理程序代码
	int nID = IDC_EDIT_STEP;
	if(!CheckEditValue(nID))
	{
		AfxMessageBox("常量必须是整型或浮点型数据！", MB_ICONWARNING);
		CEdit* pEdit = (CEdit*)GetDlgItem(nID);
		pEdit->SetWindowText("0");
	}
}


BOOL CDlgForStep::OnInitDialog()
{
	CDlgForBase::OnInitDialog();

	// TODO:  在此添加额外的初始化
	GetDlgItem(IDC_EDIT_FIRST_VALUE)->SetWindowText("0");
	GetDlgItem(IDC_EDIT_END_VALUE)->SetWindowText("0");
	GetDlgItem(IDC_EDIT_STEP)->SetWindowText("0");

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

ForStepData CDlgForStep::GetData()
{
	ForStepData data;

	data.strParam = GetSelStrFromComboBox(IDC_COMBO_PARAM);
	data.strFirstValue = GetSelStrFromComboBox(IDC_COMBO_FIRST_VALUE);
	data.strEndValue = GetSelStrFromComboBox(IDC_COMBO_END_VALUE);
	data.strStep = GetSelStrFromComboBox(IDC_COMBO_STEP);

	GetDlgItem(IDC_EDIT_FIRST_VALUE)->GetWindowText(data.strFirstValueConst);
	GetDlgItem(IDC_EDIT_END_VALUE)->GetWindowText(data.strEndValueConst);
	GetDlgItem(IDC_EDIT_STEP)->GetWindowText(data.strStepConst);

	return data;
}