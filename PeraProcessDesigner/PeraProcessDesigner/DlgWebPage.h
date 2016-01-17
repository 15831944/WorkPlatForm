#pragma once


// CDlgWebPage 对话框

class CDlgWebPage : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgWebPage)

public:
	CDlgWebPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgWebPage();

// 对话框数据
	enum { IDD = IDD_WEBPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
