#pragma once
#include "DlgForBase.h"


// CDlgWhile �Ի���

class CDlgWhile : public CDlgForBase
{
	DECLARE_DYNAMIC(CDlgWhile)

public:
	CDlgWhile(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgWhile();

// �Ի�������
	enum { IDD = IDD_WHILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
