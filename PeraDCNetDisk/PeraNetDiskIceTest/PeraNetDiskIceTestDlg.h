
// PeraNetDiskIceTestDlg.h : ͷ�ļ�
//

#pragma once


// CPeraNetDiskIceTestDlg �Ի���
class CPeraNetDiskIceTestDlg : public CDialog
{
// ����
public:
	CPeraNetDiskIceTestDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_PERANETDISKICETEST_DIALOG };

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
