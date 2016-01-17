#pragma once
#include "afxwin.h"
#include "BCGProMigrate.h"
//#include "StructTreeNodeData.h"
#include "CxStructTreeNodeParam.h"

class CCxStructTreeNodeParam;

// DlgStructTreeFileParamEditor �Ի���

class CDlgStructTreeFileParamEditor : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgStructTreeFileParamEditor)

public:
	CDlgStructTreeFileParamEditor(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgStructTreeFileParamEditor();

// �Ի�������
	enum { IDD = IDD_STRUCTTREE_FILEPARAM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedStfpOk();
	afx_msg void OnBnClickedStfpCancel();
	afx_msg void OnBnClickedStfpView();
	afx_msg void OnBnClickedStfpPathsel();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_wndPath;
	CButton m_wndPathSel;
	CBCGMenuButton m_wndViewFile;
	CButton m_wndOk;
	CButton m_wndCancel;
	CCxStructTreeNodeParam * m_pNode;
	afx_msg void OnClose();
	CMenu m_menu;
};
