// ConnectionSetDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "601DemoClient.h"
#include "ConnectionSetDlg.h"
#include "afxdialogex.h"


// CConnectionSetDlg �Ի���

IMPLEMENT_DYNAMIC(CConnectionSetDlg, CDialogEx)

CConnectionSetDlg::CConnectionSetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CConnectionSetDlg::IDD, pParent)
	, m_strServerPort(_T(""))
	, m_strLocalPort(_T(""))
	,m_strServerDomain(_T("localhost"))
	, m_iUseIp(0)
	, m_iUseDomain(-1)
{

}

CConnectionSetDlg::~CConnectionSetDlg()
{
}

void CConnectionSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS_SERVER, m_IpAddressCtrl);
	DDX_Radio(pDX, IDC_RADIO_IP, m_iUseIp);
	DDX_Radio(pDX, IDC_RADIO_Domain, m_iUseDomain);
	DDX_Text(pDX, IDC_EDIT_SERVER_PORT, m_strServerPort);
	DDV_MaxChars(pDX, m_strServerPort, 5);

	DDX_Text(pDX, IDC_EDIT_SERVER_DOMAIN, m_strServerDomain);

	DDX_Text(pDX, IDC_EDIT_CLIENT_PORT, m_strLocalPort);
	DDV_MaxChars(pDX, m_strLocalPort, 5);
}


BEGIN_MESSAGE_MAP(CConnectionSetDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CConnectionSetDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_IP, &CConnectionSetDlg::OnBnClickedRadioIp)
	ON_BN_CLICKED(IDC_RADIO_Domain, &CConnectionSetDlg::OnBnClickedRadioDomain)
END_MESSAGE_MAP()


// CConnectionSetDlg ��Ϣ�������


BOOL CConnectionSetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME),FALSE);
	if (m_iUseIp==0)
	{
		m_iUseDomain = -1;
		GetDlgItem(IDC_EDIT_SERVER_DOMAIN)->ShowWindow(SW_HIDE);
		m_IpAddressCtrl.ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_INFO)->SetWindowText("������IP��ַ��");
	}
	else
	{
		m_iUseDomain = 0;
		GetDlgItem(IDC_EDIT_SERVER_DOMAIN)->ShowWindow(SW_SHOW);
		m_IpAddressCtrl.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_INFO)->SetWindowText("������������");
	}

	int IPP1,IPP2,IPP3,IPP4;
	sscanf_s(m_strServerIp,"%d.%d.%d.%d",&IPP1,&IPP2,&IPP3,&IPP4);
	m_IpAddressCtrl.SetAddress(IPP1,IPP2,IPP3,IPP4);
	GetDlgItem(IDC_EDIT_SERVER_DOMAIN)->SetWindowText(m_strServerDomain);

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CConnectionSetDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	if (m_iUseIp == 0)
	{
		BYTE IPP1(0),IPP2(0),IPP3(0),IPP4(0);
		m_IpAddressCtrl.GetAddress(IPP1,IPP2,IPP3,IPP4);
		m_strServerIp.Format("%d.%d.%d.%d",IPP1,IPP2,IPP3,IPP4);
	}
	else if(m_iUseDomain ==0)
	{
		if (m_strServerDomain.IsEmpty())
		{
			MessageBox("��������Ϊ�գ�","��ʾ",MB_OK|MB_ICONINFORMATION);
			return;
		}
	}
	CDialogEx::OnOK();
}


void CConnectionSetDlg::OnBnClickedRadioIp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	m_iUseDomain = TRUE;
	UpdateData(FALSE);
	m_IpAddressCtrl.ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT_SERVER_DOMAIN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_INFO)->SetWindowText("������IP��ַ��");
}


void CConnectionSetDlg::OnBnClickedRadioDomain()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	m_iUseIp = TRUE;
	UpdateData(FALSE);
	m_IpAddressCtrl.ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_SERVER_DOMAIN)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_INFO)->SetWindowText("������������");
}
