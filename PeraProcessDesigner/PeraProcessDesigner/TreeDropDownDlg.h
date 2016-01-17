#pragma once
#include "IfEditTree.h"

// CTreePopDlg 对话框

class CTreeDropDownDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTreeDropDownDlg)

public:
	CTreeDropDownDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTreeDropDownDlg();
// 对话框数据
	enum { IDD = IDD_TREE_DLG };
public:
	CIfEditTree m_IfEditTree;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void    InitTree();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
};
