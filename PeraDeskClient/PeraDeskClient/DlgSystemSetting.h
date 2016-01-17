#pragma once
#include "afxwin.h"
#include "CxBCGPDialog.h"
#include "CxBCGPButton.h"
#include "BCGPEdit.h"

// CDlgSystemSetting 对话框

class CDlgSystemSetting : public CCxBCGPDialog
{
	DECLARE_DYNAMIC(CDlgSystemSetting)

public:
	CDlgSystemSetting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSystemSetting();

// 对话框数据
	enum { IDD = IDD_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnSelDir(); // 浏览按钮
	BOOL IsCacheDirEmpty(const CString & strDir);
	CCxBCGPButton m_wndOk;
	CCxBCGPButton m_wndCancel;
	CCxBCGPButton m_wndCacheDirSel;
	CBCGPEdit m_EditCtrlCacheDir;
	CString m_strEditCacheDir;
	CString   m_strOldCacheDir;
};
