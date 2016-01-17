#pragma once

#include "InvokeServerMethod.h"
// CTestInvokeDlg �Ի���

class CTestInvokeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTestInvokeDlg)

public:
	CTestInvokeDlg(InvokeServerMethod *pInvokeObj,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTestInvokeDlg();

// �Ի�������
	enum { IDD = IDD_DLG_TEST_INVOKE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString m_strFunName;
	CString m_strParam;
private:
	InvokeServerMethod * m_pInvokeObj;
public:
	afx_msg void OnNcDestroy();
};
