#pragma once
#include "CxBCGPDialog.h"
#include "DlgForBase.h"


// CDlgForLimit 对话框

class CDlgForLimit : public CDlgForBase
{
	DECLARE_DYNAMIC(CDlgForLimit)

public:
	CDlgForLimit(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgForLimit();

// 对话框数据
	enum { IDD = IDD_FOR_LIMIT };

	int m_nConut; //最大循环次数

	BOOL m_bContinue; //循环执行失败是否继续

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeCombo();
	afx_msg void OnEnKillfocusEdit();
};
