
// PeraTaskServiceDlg.h : ͷ�ļ�
//

#pragma once


// CPeraTaskServiceDlg �Ի���
class CPeraTaskServiceDlg : public CDialog
{
// ����
public:
	CPeraTaskServiceDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_PERATASKSERVICE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
