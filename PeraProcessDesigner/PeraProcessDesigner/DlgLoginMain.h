#pragma once
#include "resource.h"
#include "afxwin.h"
#include "CxBCGPButton.h"

// CDlgLoginMain 对话框

class CDlgLoginMain : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgLoginMain)

public:
	CDlgLoginMain(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLoginMain();

	// 对话框数据
	enum { IDD = IDD_DIALOG_LOGIN_MAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
protected:
	virtual void OnCancel();
public:
	afx_msg void OnBnClickedButtonLogin();
	afx_msg void OnBnClickedButtonReset();
	CEdit m_EditUser;
	CEdit m_EditPwd;
	CStatic m_ErrorStatic;
protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedButtonClose();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void CustomResizeDlg();
	CCxBCGPButton m_btnLogin;
	CCxBCGPButton m_btnReset;
	CCxBCGPButton m_btnClose;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
