#pragma once


// CDlgWebPage �Ի���

class CDlgWebPage : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgWebPage)

public:
	CDlgWebPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgWebPage();

// �Ի�������
	enum { IDD = IDD_WEBPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
