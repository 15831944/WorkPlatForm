// DlgParamEditFile.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgParamEditFile.h"
#include "afxdialogex.h"


// CDlgParamEditFile �Ի���

IMPLEMENT_DYNAMIC(CDlgParamEditFile, CCxBCGPDialog)

CDlgParamEditFile::CDlgParamEditFile(CWnd* pParent /*=NULL*/)
	: CCxBCGPDialog(CDlgParamEditFile::IDD, pParent)
{

}

CDlgParamEditFile::~CDlgParamEditFile()
{
}

void CDlgParamEditFile::DoDataExchange(CDataExchange* pDX)
{
	CCxBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgParamEditFile, CCxBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_FILE, &CDlgParamEditFile::OnBnClickedButtonFile)
END_MESSAGE_MAP()


// CDlgParamEditFile ��Ϣ�������


void CDlgParamEditFile::OnBnClickedButtonFile()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFileDialog dlgFile(TRUE
		, NULL
		, NULL
		, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT
		, "All Files (*.*)|*.*||"
		//, AfxGetMainWnd()
		);

	CString strFilePath;

	if (dlgFile.DoModal() == IDOK)
	{
		strFilePath = dlgFile.GetPathName();
		if (!strFilePath.IsEmpty() && !PathFileExists(strFilePath))
		{
			CString sMsg;
			sMsg.Format("�ļ� %s ������", strFilePath);
			MessageBox(sMsg, g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		}
		else
		{
			GetDlgItem(IDC_EDIT)->SetWindowText(strFilePath);
		}
	}
}


BOOL CDlgParamEditFile::OnInitDialog()
{
	CCxBCGPDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE); // Set big icon

	((CEdit*)GetDlgItem(IDC_EDIT))->SetReadOnly(TRUE);
	GetDlgItem(IDC_EDIT)->SetWindowText(m_strFilePath.c_str());
	GetDlgItem(IDC_EDIT)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CDlgParamEditFile::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���
	CString strNewValue;
	GetDlgItem(IDC_EDIT)->GetWindowText( strNewValue );

	if (!strNewValue.IsEmpty() && !PathFileExists(strNewValue))
	{
		CString sMsg;
		sMsg.Format("�ļ� %s ������", strNewValue);
		MessageBox(sMsg, g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return;
	}

	m_strFilePath = strNewValue;

	CCxBCGPDialog::OnOK();
}
