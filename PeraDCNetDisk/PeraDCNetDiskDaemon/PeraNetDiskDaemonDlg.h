
// PeraDCNetDiskDaemonDlg.h : ͷ�ļ�
//

#pragma once


// CPeraDCNetDiskDaemonDlg �Ի���
class CPeraDCNetDiskDaemonDlg : public CDialog
{
// ����
public:
	CPeraDCNetDiskDaemonDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_PERADCNETDISKDAEMON_DIALOG };

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
