#pragma once


// CCreateTreeDirDlg �Ի���

class CCreateTreeDirDlg : public CDialog
{
	DECLARE_DYNAMIC(CCreateTreeDirDlg)

public:
	CCreateTreeDirDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCreateTreeDirDlg();

// �Ի�������
	enum { IDD = IDD_ADD_DIR };
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_nCreateDirType;
	CString m_csDirName;
	CString m_PK_ID;
	CString m_FK_PID;
	afx_msg void OnBnClickedOk();
	BOOL FindRobotDirName();
	BOOL FindProcessDirName();
	void SetCreateDir(int nCreateDirType);
	virtual BOOL OnInitDialog();
};
