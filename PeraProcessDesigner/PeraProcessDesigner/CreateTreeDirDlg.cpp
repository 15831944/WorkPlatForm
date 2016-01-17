// CreateTreeDirDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "CreateTreeDirDlg.h"
#include "afxdialogex.h"
#include "CStringChecker.h"
#include <ReadRobotDB.h>
#include <ResLibData.h>

extern CReadRobotDB g_ReadRobotDB;
// CCreateTreeDirDlg �Ի���

IMPLEMENT_DYNAMIC(CCreateTreeDirDlg, CDialog)

CCreateTreeDirDlg::CCreateTreeDirDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateTreeDirDlg::IDD, pParent)
	, m_csDirName(_T(""))
{
	m_nCreateDirType = CResLibData::RT_ROBOT;
}
void CCreateTreeDirDlg::SetCreateDir(int nCreateDirType)
{
	m_nCreateDirType = nCreateDirType;
}
CCreateTreeDirDlg::~CCreateTreeDirDlg()
{
}

void CCreateTreeDirDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DIR_NAME, m_csDirName);
}


BEGIN_MESSAGE_MAP(CCreateTreeDirDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CCreateTreeDirDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCreateTreeDirDlg ��Ϣ�������


void CCreateTreeDirDlg::OnBnClickedOk()
{
	UpdateData();
	switch (m_nCreateDirType)
	{
	case CResLibData::RT_ROBOT:
		{
			if (FindRobotDirName())
			{
				CDialog::OnOK();
			}			
		}
		break;
	case CResLibData::RT_PROCESS:
		{
			if(FindProcessDirName())
			{
				CDialog::OnOK();
			}
		}
		break;
	default:
		break;
	}
	UpdateData(FALSE);
}
BOOL CCreateTreeDirDlg::FindRobotDirName()
{
	//m_csDirName.Replace(" ","");

	if(true)
	{//���������
		BOOL b = CStringChecker::CheckDirName(m_csDirName);
		if(!b)
		{//���ʧ��
			
			MessageBox("Ŀ¼����" + CStringChecker::GetLastErr(), g_lpszAppTitle, MB_ICONERROR);
			CWnd* hCedit = GetDlgItem(IDC_DIR_NAME);
			if (hCedit != NULL)
			{
				((CEdit*)hCedit)->SetFocus();
				((CEdit*)hCedit)->SetSel(0,-1);
			}
			return FALSE;
		}
	}

	if (g_ReadRobotDB.FindRobotName(m_FK_PID.GetBuffer(),m_csDirName.GetBuffer()))
	{
		
		MessageBox("Ŀ¼�����ظ���", g_lpszAppTitle, MB_ICONERROR);
		CWnd* hCedit = GetDlgItem(IDC_DIR_NAME);
		if (hCedit != NULL)
		{
			((CEdit*)hCedit)->SetFocus();
			((CEdit*)hCedit)->SetSel(0,-1);
		}
		return FALSE;
	}
	return TRUE;
}
BOOL CCreateTreeDirDlg::FindProcessDirName()
{
	//m_csDirName.Replace(" ","");

	if(true)
	{//���������
		BOOL b = CStringChecker::CheckDirName(m_csDirName);
		if(!b)
		{//���ʧ��
			MessageBox("Ŀ¼����" + CStringChecker::GetLastErr(), g_lpszAppTitle, MB_ICONERROR);
			CWnd* hCedit = GetDlgItem(IDC_DIR_NAME);
			if (hCedit != NULL)
			{
				((CEdit*)hCedit)->SetFocus();
				((CEdit*)hCedit)->SetSel(0,-1);
			}
			return FALSE;
		}
	}
	if (g_ReadRobotDB.FindProcessName(m_FK_PID.GetBuffer(),m_csDirName.GetBuffer()))
	{
		MessageBox("Ŀ¼�����ظ���", g_lpszAppTitle, MB_ICONERROR);
		CWnd* hCedit = GetDlgItem(IDC_DIR_NAME);
		if (hCedit != NULL)
		{
			((CEdit*)hCedit)->SetFocus();
			((CEdit*)hCedit)->SetSel(0,-1);
		}
		return FALSE;
	}
	return TRUE;
}

BOOL CCreateTreeDirDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon,TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
