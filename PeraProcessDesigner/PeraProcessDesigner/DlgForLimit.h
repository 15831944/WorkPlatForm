#pragma once
#include "CxBCGPDialog.h"
#include "DlgForBase.h"


// CDlgForLimit �Ի���

class CDlgForLimit : public CDlgForBase
{
	DECLARE_DYNAMIC(CDlgForLimit)

public:
	CDlgForLimit(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgForLimit();

// �Ի�������
	enum { IDD = IDD_FOR_LIMIT };

	int m_nConut; //���ѭ������

	BOOL m_bContinue; //ѭ��ִ��ʧ���Ƿ����

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeCombo();
	afx_msg void OnEnKillfocusEdit();
};
