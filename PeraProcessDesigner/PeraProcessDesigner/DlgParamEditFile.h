#pragma once
#include "CxBCGPDialog.h"


// CDlgParamEditFile �Ի���

class CDlgParamEditFile : public CCxBCGPDialog
{
	DECLARE_DYNAMIC(CDlgParamEditFile)

public:
	CDlgParamEditFile(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgParamEditFile();

// �Ի�������
	enum { IDD = IDD_PARAM_EDIT_FILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonFile();
	virtual BOOL OnInitDialog();

	std::string m_strFilePath;
	virtual void OnOK();
};
