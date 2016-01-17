#pragma once
#include "CxBCGPDialog.h"


// CDlgInfoEdit 对话框

class CDlgInfoEdit : public CCxBCGPDialog
{
	DECLARE_DYNAMIC(CDlgInfoEdit)

public:
	CDlgInfoEdit(std::string str = "", std::string strCaption = "", int nMax = -1, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgInfoEdit();

// 对话框数据
	enum { IDD = IDD_INFO_EDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();

public:
	std::string m_strText; //文本内容

	std::string m_strCaption; //标题

	int m_nLimitText; //字数限制
	int GetLimitText() const { return m_nLimitText; }
	void SetLimitText(int val) { m_nLimitText = val; }
	virtual BOOL OnInitDialog();
};
