#pragma once
#include "afxwin.h"
#include "CxArrayBase.h"


// CDlgNDArrayResize �Ի���

class CDlgNDArrayResize : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgNDArrayResize)

public:
	CDlgNDArrayResize(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgNDArrayResize();

// �Ի�������
	enum { IDD = IDD_NDAE_RESIZE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_wndDim1;
	CEdit m_wndDim2;
	CEdit m_wndDim3;
	CButton m_wndOk;
	CButton m_wndCancel;
	afx_msg void OnBnClickedNdaerOk();
	afx_msg void OnBnClickedNdaerCancel();
	afx_msg void OnBnClickedOk();

	CCxArrayBase * m_pArrayData;
};
