
// WebPageDlg.h : ͷ�ļ�
//

#pragma once
#include <BaseHtmlDialog.h>


// CWebPageDlg �Ի���
class CDlgMain : public CDialogEx
{
// ����
public:
	CDlgMain(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MAIN };
	CBaseHtmlDialog * m_pWeb;

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
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnAwProcessAttaching(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWebEventAsync( WPARAM wParam, LPARAM lParam );
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	//void OnWebPageEvent( LPVOID lpData );
	afx_msg LRESULT OnAwProcessAttached(WPARAM,LPARAM);
	afx_msg void OnDestroy();

protected:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};
