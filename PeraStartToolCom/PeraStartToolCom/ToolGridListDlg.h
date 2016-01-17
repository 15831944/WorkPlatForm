#pragma once
#include "CxBCGPDialog.h"
#include "ToolsProUnity.h"
#include "CxBCGPGrid.h"
#include "afxwin.h"
// CToolGridListDlg 对话框

class CToolGridListDlg : public CCxBCGPDialog
{
	DECLARE_DYNAMIC(CToolGridListDlg)

public:
	CToolGridListDlg(map<CString,ToolObject> toolmap,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CToolGridListDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_MANAGER_BCG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	BOOL InitGridList();
	void LoadTooList();

	CStatic m_wndGridListPos; // 定位表格控件的静态控件
	CCxBCGPGrid m_wndGridTools; // 表格控件
	map<CString,ToolObject> m_ToolMap; // 本地工具集

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	
	afx_msg void OnBnClickedButtonEditSel();
	afx_msg void OnBnClickedButtonDelSel();
	afx_msg void OnBnClickedButtonDelAll();
	afx_msg HRESULT OnMsgGridDoubleClick(WPARAM wp,LPARAM lp);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
