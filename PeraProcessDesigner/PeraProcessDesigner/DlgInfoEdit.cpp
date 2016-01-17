// DlgInfoEdit.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgInfoEdit.h"
#include "afxdialogex.h"


// CDlgInfoEdit �Ի���

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


// CDlgInfoEdit ��Ϣ�������


void CDlgInfoEdit::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���
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

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	GetDlgItem(IDC_EDIT)->SetWindowText(m_strText.c_str());
	SetWindowText(m_strCaption.c_str());

	EnableVisualManagerStyle (TRUE, TRUE);

	GetDlgItem(IDC_EDIT)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
