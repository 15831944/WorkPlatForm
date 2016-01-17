// DlgParamEditFile.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgParamEditFile.h"
#include "afxdialogex.h"


// CDlgParamEditFile 对话框

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


// CDlgParamEditFile 消息处理程序


void CDlgParamEditFile::OnBnClickedButtonFile()
{
	// TODO: 在此添加控件通知处理程序代码
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
			sMsg.Format("文件 %s 不存在", strFilePath);
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

	// TODO:  在此添加额外的初始化
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE); // Set big icon

	((CEdit*)GetDlgItem(IDC_EDIT))->SetReadOnly(TRUE);
	GetDlgItem(IDC_EDIT)->SetWindowText(m_strFilePath.c_str());
	GetDlgItem(IDC_EDIT)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgParamEditFile::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	CString strNewValue;
	GetDlgItem(IDC_EDIT)->GetWindowText( strNewValue );

	if (!strNewValue.IsEmpty() && !PathFileExists(strNewValue))
	{
		CString sMsg;
		sMsg.Format("文件 %s 不存在", strNewValue);
		MessageBox(sMsg, g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return;
	}

	m_strFilePath = strNewValue;

	CCxBCGPDialog::OnOK();
}
