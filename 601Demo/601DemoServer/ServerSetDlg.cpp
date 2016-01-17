// ServerSetDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "601DemoServer.h"
#include "ServerSetDlg.h"
#include "afxdialogex.h"


// CServerSetDlg �Ի���

IMPLEMENT_DYNAMIC(CServerSetDlg, CDialogEx)

CServerSetDlg::CServerSetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CServerSetDlg::IDD, pParent)
	, m_strServerPort(_T(""))
{

}

CServerSetDlg::~CServerSetDlg()
{
}

void CServerSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SERVER_PORT, m_strServerPort);
	DDV_MaxChars(pDX, m_strServerPort, 5);
}


BEGIN_MESSAGE_MAP(CServerSetDlg, CDialogEx)
END_MESSAGE_MAP()


// CServerSetDlg ��Ϣ�������


BOOL CServerSetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME),FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
