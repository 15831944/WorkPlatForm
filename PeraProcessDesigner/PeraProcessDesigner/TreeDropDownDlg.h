#pragma once
#include "IfEditTree.h"

// CTreePopDlg �Ի���

class CTreeDropDownDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTreeDropDownDlg)

public:
	CTreeDropDownDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTreeDropDownDlg();
// �Ի�������
	enum { IDD = IDD_TREE_DLG };
public:
	CIfEditTree m_IfEditTree;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	void    InitTree();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
};
