#pragma once
#include "afxwin.h"
#include "CxBCGPDialog.h"
#include "CxBCGPButton.h"
#include "BCGPEdit.h"

// CDlgSystemSetting �Ի���

class CDlgSystemSetting : public CCxBCGPDialog
{
	DECLARE_DYNAMIC(CDlgSystemSetting)

public:
	CDlgSystemSetting(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSystemSetting();

// �Ի�������
	enum { IDD = IDD_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnSelDir(); // �����ť
	BOOL IsCacheDirEmpty(const CString & strDir);
	CCxBCGPButton m_wndOk;
	CCxBCGPButton m_wndCancel;
	CCxBCGPButton m_wndCacheDirSel;
	CBCGPEdit m_EditCtrlCacheDir;
	CString m_strEditCacheDir;
	CString   m_strOldCacheDir;
};
