#pragma once
#include "afxwin.h"
#include "CxArrayBase.h"


// CDlgNDArrayResize 对话框

class CDlgNDArrayResize : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgNDArrayResize)

public:
	CDlgNDArrayResize(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgNDArrayResize();

// 对话框数据
	enum { IDD = IDD_NDAE_RESIZE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
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
