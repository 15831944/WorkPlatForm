// DlgForLimit.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgForLimit.h"
#include "afxdialogex.h"


// CDlgForLimit �Ի���

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


// CDlgForLimit ��Ϣ�������


BOOL CDlgForLimit::OnInitDialog()
{
	CDlgForBase::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_bContinue = TRUE;

	m_nConut = 10;

	((CBCGPComboBox*)GetDlgItem(IDC_COMBO))->SetCurSel(0);

	CString str;
	str.Format("%d", m_nConut);

	((CBCGPEdit*)GetDlgItem(IDC_EDIT))->SetWindowText(str);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CDlgForLimit::OnCbnSelchangeCombo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_bContinue = !((CBCGPComboBox*)GetDlgItem(IDC_COMBO))->GetCurSel();
}


void CDlgForLimit::OnEnKillfocusEdit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	((CBCGPEdit*)GetDlgItem(IDC_EDIT))->GetWindowText(str);
	m_nConut = atoi(str);
}
