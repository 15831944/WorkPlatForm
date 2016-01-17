#pragma once
#include "CxBCGPDialog.h"

// CDlgViewRunFlow �Ի���

class CDlgViewRunFlow : public CCxBCGPDialog
{
	DECLARE_DYNAMIC(CDlgViewRunFlow)

public:
	CDlgViewRunFlow(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgViewRunFlow();

// �Ի�������
	enum { IDD = IDD_VIEW_RUNFLOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg LRESULT OnAwProcessAttaching( WPARAM wParam, LPARAM lParam );

	//CCxWebPageEvent * m_pStartWebPageEvent;
	CAttachWindow m_AttachWindow;
	//HWND m_hwndWebPage;
	DWORD m_dwExStyleWebPage;
	DWORD m_dwStyleWebPage;
	CString m_strUrl;
	int m_nWidth;
	int m_nHeight;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};
