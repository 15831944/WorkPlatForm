#pragma once
#include "DlgForBase.h"


// CDlgForEachListFrame �Ի���

class CDlgForEachListFrame : public CDlgForBase
{
	DECLARE_DYNAMIC(CDlgForEachListFrame)

public:
	CDlgForEachListFrame(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgForEachListFrame();

// �Ի�������
	enum { IDD = IDD_FOR_EACH_LIST_FRAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	void RefreshScrollBar();
	void ReSetScrollBar();
	afx_msg void OnPaint();
};
