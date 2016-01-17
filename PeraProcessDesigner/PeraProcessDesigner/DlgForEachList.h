#pragma once
#include "DlgForBase.h"


// CDlgForEachList 对话框
#define IDC_COMBO_ID  9

class CDlgForEachList : public CDlgForBase
{
	DECLARE_DYNAMIC(CDlgForEachList)

public:
	CDlgForEachList(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgForEachList();

// 对话框数据
	enum { IDD = IDD_FOR_EACH_LIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	afx_msg void OnCbnFocusCombo();
};
