#pragma once

#include "CxBCGPDialog.h"
#include "MsgCoreDlg.h"
// CMsgDlg �Ի���

class CMsgDlg : public CCxBCGPDialog
{
	DECLARE_DYNAMIC(CMsgDlg)

public:
	CMsgDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMsgDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_MSG };
	CMsgCoreDlg m_CoreDlg;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	void RefreshMsg();
	void OpenUrl(LPCTSTR strUrl);
};
