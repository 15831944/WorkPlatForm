#pragma once
#include "afxcmn.h"


// CConnectionSetDlg 对话框

class CConnectionSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CConnectionSetDlg)

public:
	CConnectionSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CConnectionSetDlg();

// 对话框数据
	enum { IDD = IDD_DLG_CONN_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CIPAddressCtrl m_IpAddressCtrl;
	CString m_strServerPort;
	CString m_strLocalPort;
	CString m_strServerIp;
	CString m_strServerDomain;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	int m_iUseIp;
	int m_iUseDomain;
	afx_msg void OnBnClickedRadioIp();
	afx_msg void OnBnClickedRadioDomain();
};
