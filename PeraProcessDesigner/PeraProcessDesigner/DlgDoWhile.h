#pragma once
#include "DlgWhile.h"


// CDlgDoWhile 对话框

class CDlgDoWhile : public CDlgWhile
{
	DECLARE_DYNAMIC(CDlgDoWhile)

public:
	CDlgDoWhile(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgDoWhile();

// 对话框数据
	enum { IDD = IDD_WHILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
