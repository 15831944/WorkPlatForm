#pragma once
#include "DlgForBase.h"


// CDlgForEachListFrame 对话框

class CDlgForEachListFrame : public CDlgForBase
{
	DECLARE_DYNAMIC(CDlgForEachListFrame)

public:
	CDlgForEachListFrame(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgForEachListFrame();

// 对话框数据
	enum { IDD = IDD_FOR_EACH_LIST_FRAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	void RefreshScrollBar();
	void ReSetScrollBar();
	afx_msg void OnPaint();
};
