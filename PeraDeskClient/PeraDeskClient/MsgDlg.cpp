// MsgDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PeraDeskClient.h"
#include "MsgDlg.h"
#include "afxdialogex.h"


// CMsgDlg �Ի���

IMPLEMENT_DYNAMIC(CMsgDlg, CCxBCGPDialog)

CMsgDlg::CMsgDlg(CWnd* pParent /*=NULL*/)
	: CCxBCGPDialog(CMsgDlg::IDD, pParent)
{

}

CMsgDlg::~CMsgDlg()
{
}

void CMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CCxBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMsgDlg, CCxBCGPDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_NCDESTROY()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CMsgDlg ��Ϣ�������


int CMsgDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CCxBCGPDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	if (m_CoreDlg.m_hWnd==NULL)
	{
		m_CoreDlg.Create(CMsgCoreDlg::IDD,this);
		m_CoreDlg.ShowWindow(SW_SHOW);
	}
	return 0;
}


void CMsgDlg::OnSize(UINT nType, int cx, int cy)
{
	CCxBCGPDialog::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	if (m_CoreDlg.m_hWnd)
	{
		m_CoreDlg.SetWindowPos(NULL,0,0,cx,cy,SWP_NOZORDER);
	}
}


void CMsgDlg::OnNcDestroy()
{
	CCxBCGPDialog::OnNcDestroy();

	// TODO: �ڴ˴������Ϣ����������
}


BOOL CMsgDlg::OnInitDialog()
{
	CCxBCGPDialog::OnInitDialog();

	SetWindowPos(NULL,0,0,800,600,SWP_NOZORDER|SWP_NOMOVE);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CMsgDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	ShowWindow(SW_HIDE);
	return;

	//CCxBCGPDialog::OnClose();
}
void CMsgDlg::RefreshMsg()
{
	if (m_CoreDlg.m_hWnd == NULL)
		return;

	m_CoreDlg.OpeMsgUrl();
}
void CMsgDlg::OpenUrl(LPCTSTR strUrl)
{
	if (m_CoreDlg.m_hWnd == NULL)
		return;

	if (strUrl==NULL)
		return;

	m_CoreDlg.Navigate(strUrl);
}