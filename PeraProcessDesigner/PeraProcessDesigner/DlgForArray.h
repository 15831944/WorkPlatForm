#pragma once
#include "DlgForBase.h"


// CDlgForArray �Ի���

class CDlgForArray : public CDlgForBase
{
	DECLARE_DYNAMIC(CDlgForArray)

public:
	CDlgForArray(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgForArray();

// �Ի�������
	enum { IDD = IDD_FOR_ARRAY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
