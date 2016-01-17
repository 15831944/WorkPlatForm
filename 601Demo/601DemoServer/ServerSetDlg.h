#pragma once


// CServerSetDlg 对话框

class CServerSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CServerSetDlg)

public:
	CServerSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CServerSetDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_SET_PORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strServerPort;
	virtual BOOL OnInitDialog();
};
