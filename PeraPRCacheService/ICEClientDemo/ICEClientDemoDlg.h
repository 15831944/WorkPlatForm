
// ICEClientDemoDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "PeraWsAstIceClient_I.h"
#include "afxwin.h"
#include "DlgProgress.h"


// CICEClientDemoDlg �Ի���
class CICEClientDemoDlg : public CDialogEx
{
// ����
public:
	CICEClientDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_ICECLIENTDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedZip();
	afx_msg void OnBnClickedUnzip();
	afx_msg void OnBnClickedCopy();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedStop();

	BOOL	InsertListData( LPCTSTR lpszItemData );
	LRESULT	OnIceMessage( WPARAM wparam, LPARAM lparam );
	LRESULT OnShowProgress( WPARAM wParam, LPARAM );
	void	StartProgress();
	void	EndProgress();

protected:
	CPeraWsAstIceClient_I m_IceClient_I;
	CComboBox m_combox;
	CDlgProgress* m_pDlgProgress;
	CListCtrl m_list;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedClear();
	afx_msg void OnBnClickedCreatews();
	afx_msg void OnBnClickedCreatecache();
	afx_msg void OnBnClickedJianmo();
};
