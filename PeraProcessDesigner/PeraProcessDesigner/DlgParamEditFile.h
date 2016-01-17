#pragma once
#include "CxBCGPDialog.h"


// CDlgParamEditFile 对话框

class CDlgParamEditFile : public CCxBCGPDialog
{
	DECLARE_DYNAMIC(CDlgParamEditFile)

public:
	CDlgParamEditFile(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgParamEditFile();

// 对话框数据
	enum { IDD = IDD_PARAM_EDIT_FILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonFile();
	virtual BOOL OnInitDialog();

	std::string m_strFilePath;
	virtual void OnOK();
};
