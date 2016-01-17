#pragma once
#include "afxwin.h"
#include <BCGPGridCtrl.h>
#include "XmlHistoryModel.h"
#include "CxBCGPGrid.h"

// CDlgJianmoLogin 对话框

class CDlgJianmoLogin : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgJianmoLogin)

public:
	CDlgJianmoLogin(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgJianmoLogin();

// 对话框数据
	enum { IDD = IDD_JIANMO_LOGIN };

	enum {
		COL_NAME = 0,
		COL_PATH,
		COL_OP
	};

public:

	CButton m_wndNew;
	CButton m_wndOpen;
	CStatic m_wndHistoryList;
	CCxBCGPGrid m_wndGridHistory;

public:
	BOOL InitHistoryList(void);
	BOOL LoadHistoryList(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnGridButtonClicked(UINT uID);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedJmlNew();
	afx_msg void OnBnClickedJmlOpen();

protected:
	CXmlHistoryModel m_xhm;
};
