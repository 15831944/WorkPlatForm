#pragma once
#include "CxBCGPDialog.h"


// CDlgInfoEdit �Ի���

class CDlgInfoEdit : public CCxBCGPDialog
{
	DECLARE_DYNAMIC(CDlgInfoEdit)

public:
	CDlgInfoEdit(std::string str = "", std::string strCaption = "", int nMax = -1, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgInfoEdit();

// �Ի�������
	enum { IDD = IDD_INFO_EDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();

public:
	std::string m_strText; //�ı�����

	std::string m_strCaption; //����

	int m_nLimitText; //��������
	int GetLimitText() const { return m_nLimitText; }
	void SetLimitText(int val) { m_nLimitText = val; }
	virtual BOOL OnInitDialog();
};
