#pragma once
#include <ShoshTreeCtrl.h>




class CDlgResLibTree : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgResLibTree)

public:
	CDlgResLibTree(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgResLibTree();

// �Ի�������
	enum { IDD = IDD_JIANMO_COMPONENT_TREE };
	HICON m_hIcon;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnNotifySelchanged(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	CShoshTreeCtrl m_tree;
	


	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
