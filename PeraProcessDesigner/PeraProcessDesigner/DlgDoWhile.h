#pragma once
#include "DlgWhile.h"


// CDlgDoWhile �Ի���

class CDlgDoWhile : public CDlgWhile
{
	DECLARE_DYNAMIC(CDlgDoWhile)

public:
	CDlgDoWhile(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgDoWhile();

// �Ի�������
	enum { IDD = IDD_WHILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
