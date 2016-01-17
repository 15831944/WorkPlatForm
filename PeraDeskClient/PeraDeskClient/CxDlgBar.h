#pragma once


// CxDlgView 对话框

class CxDlgBar : public CDialogEx
{
	DECLARE_DYNAMIC(CxDlgBar)

public:
	CxDlgBar(UINT nID, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CxDlgBar();

// 对话框数据
	//enum { IDD = IDD_VIEW };

private:
	static vector<CxDlgBar *> m_vRef;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
};
