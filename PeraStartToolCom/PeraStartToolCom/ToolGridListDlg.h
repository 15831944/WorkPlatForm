#pragma once
#include "CxBCGPDialog.h"
#include "ToolsProUnity.h"
#include "CxBCGPGrid.h"
#include "afxwin.h"
// CToolGridListDlg �Ի���

class CToolGridListDlg : public CCxBCGPDialog
{
	DECLARE_DYNAMIC(CToolGridListDlg)

public:
	CToolGridListDlg(map<CString,ToolObject> toolmap,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CToolGridListDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_MANAGER_BCG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	BOOL InitGridList();
	void LoadTooList();

	CStatic m_wndGridListPos; // ��λ���ؼ��ľ�̬�ؼ�
	CCxBCGPGrid m_wndGridTools; // ���ؼ�
	map<CString,ToolObject> m_ToolMap; // ���ع��߼�

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	
	afx_msg void OnBnClickedButtonEditSel();
	afx_msg void OnBnClickedButtonDelSel();
	afx_msg void OnBnClickedButtonDelAll();
	afx_msg HRESULT OnMsgGridDoubleClick(WPARAM wp,LPARAM lp);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
