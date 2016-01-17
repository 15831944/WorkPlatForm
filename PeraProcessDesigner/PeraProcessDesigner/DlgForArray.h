#pragma once
#include "DlgForBase.h"


// CDlgForArray 对话框

class CDlgForArray : public CDlgForBase
{
	DECLARE_DYNAMIC(CDlgForArray)

public:
	CDlgForArray(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgForArray();

// 对话框数据
	enum { IDD = IDD_FOR_ARRAY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
