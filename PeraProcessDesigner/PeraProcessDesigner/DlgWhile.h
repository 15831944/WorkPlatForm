#pragma once
#include "DlgForBase.h"


// CDlgWhile 对话框

class CDlgWhile : public CDlgForBase
{
	DECLARE_DYNAMIC(CDlgWhile)

public:
	CDlgWhile(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgWhile();

// 对话框数据
	enum { IDD = IDD_WHILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
