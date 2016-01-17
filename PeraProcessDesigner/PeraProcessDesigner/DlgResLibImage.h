#pragma once
#include "afxcmn.h"
#include "CxResLibImgList.h"


// CDlgResLibImage �Ի���

class CDlgResLibImage : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgResLibImage)

public:
	CDlgResLibImage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgResLibImage();

// �Ի�������
	enum { IDD = IDD_JIANMO_COMPONENT_LIST };

	void UpdateContent(void);
	vector< CString > m_vImgPath;
	vector< CString > m_vComponentName;
	vector< LPARAM > m_vComponentParam;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnNotifySelchanged(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CxResLibImgList m_wndList;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
