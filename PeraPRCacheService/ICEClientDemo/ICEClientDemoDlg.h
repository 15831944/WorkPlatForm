
// ICEClientDemoDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "PeraWsAstIceClient_I.h"
#include "afxwin.h"
#include "DlgProgress.h"


// CICEClientDemoDlg 对话框
class CICEClientDemoDlg : public CDialogEx
{
// 构造
public:
	CICEClientDemoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_ICECLIENTDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedZip();
	afx_msg void OnBnClickedUnzip();
	afx_msg void OnBnClickedCopy();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedStop();

	BOOL	InsertListData( LPCTSTR lpszItemData );
	LRESULT	OnIceMessage( WPARAM wparam, LPARAM lparam );
	LRESULT OnShowProgress( WPARAM wParam, LPARAM );
	void	StartProgress();
	void	EndProgress();

protected:
	CPeraWsAstIceClient_I m_IceClient_I;
	CComboBox m_combox;
	CDlgProgress* m_pDlgProgress;
	CListCtrl m_list;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedClear();
	afx_msg void OnBnClickedCreatews();
	afx_msg void OnBnClickedCreatecache();
	afx_msg void OnBnClickedJianmo();
};
