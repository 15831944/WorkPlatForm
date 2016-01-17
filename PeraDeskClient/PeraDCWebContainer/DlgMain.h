
// WebPageDlg.h : 头文件
//

#pragma once
#include <BaseHtmlDialog.h>


// CWebPageDlg 对话框
class CDlgMain : public CDialogEx
{
// 构造
public:
	CDlgMain(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MAIN };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	CBaseHtmlDialog * m_pWeb;

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnAttachToClient(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	void OnWebPageEvent( LPVOID lpData );
	afx_msg LRESULT OnWebPageProcessAttached(WPARAM,LPARAM);
	afx_msg void OnDestroy();
protected:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	void InvokeJsModifyDefaultTask();
	void InvokeJsRefreshWindow();
};
