#pragma once
#include "DlgForBase.h"


// CDlgForEachList �Ի���
#define IDC_COMBO_ID  9

class CDlgForEachList : public CDlgForBase
{
	DECLARE_DYNAMIC(CDlgForEachList)

public:
	CDlgForEachList(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgForEachList();

// �Ի�������
	enum { IDD = IDD_FOR_EACH_LIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	afx_msg void OnCbnFocusCombo();
};
