// PeraConfigToolDlg.h : header file
//

#pragma once


// CPeraConfigToolDlg dialog
class CPeraConfigToolDlg : public CBCGPDialog
{
// Construction
public:
	CPeraConfigToolDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PERACONFIGTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnSelDir();
	void KillStartProcess();
	// »º´æÂ·¾¶
	CString m_strCacheDir;
	CString m_strPeraTaskServicePath;
	CString m_strOldCacheDir;
};
