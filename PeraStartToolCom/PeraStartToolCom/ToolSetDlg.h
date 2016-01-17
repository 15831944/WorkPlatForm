#pragma once
#include "afxwin.h"
#include "ToolsProUnity.h"
#include "CxBCGPDialog.h"

// CToolSetDlg �Ի���

class CToolSetDlg : public CCxBCGPDialog
{
	DECLARE_DYNAMIC(CToolSetDlg)

public:
	CToolSetDlg(ToolObject *pTool,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CToolSetDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_TOOLSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedButtonSelect();
	afx_msg void OnBnClickedButtonTest();
	afx_msg void OnBnClickedOk();
private:
	ToolObject *m_pTool;
public:
	// ���ID
	CString m_strToolId;
	// �������
	CString m_strToolName;
	// ����·��
	CString m_strAbsPath;
	// Ĭ������
	CString m_strDefaultKey;
	// ��������
	CString m_strReverseKey;
	// ע�����
	CString m_strKeyName;
	// ע����ֵ��
	CString m_strKeyValueName;
	// ����·��
	CString m_strRunPath;
	// ��������
	CString m_strRunParam;
	// ��������
	CString m_strRunName;

	// �������þ���·��
	CString m_strLocalPath;
	// ����������������
	CString m_strLocalRunParam;
};
