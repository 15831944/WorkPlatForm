#pragma once


// CServerSetDlg �Ի���

class CServerSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CServerSetDlg)

public:
	CServerSetDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CServerSetDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_SET_PORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_strServerPort;
	virtual BOOL OnInitDialog();
};
