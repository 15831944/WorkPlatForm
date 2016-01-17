#pragma once

#include "CxBCGPDialog.h"
#include "MsgCoreDlg.h"
// CMsgDlg 对话框

class CMsgDlg : public CCxBCGPDialog
{
	DECLARE_DYNAMIC(CMsgDlg)

public:
	CMsgDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMsgDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_MSG };
	CMsgCoreDlg m_CoreDlg;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	void RefreshMsg();
	void OpenUrl(LPCTSTR strUrl);
};
