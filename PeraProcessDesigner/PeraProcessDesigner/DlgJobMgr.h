#pragma once
#include "BaseHtmlDialog.h"




// CDlgJobMgr 对话框

class CDlgJobMgr : public CBaseHtmlDialog
{
	DECLARE_DYNAMIC(CDlgJobMgr)
	DECLARE_DISPATCH_MAP()
public:
	CDlgJobMgr(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgJobMgr();

// 对话框数据
	enum { IDD = IDD_JOBMGR };

	CString LoadUrl(void);
	void JsMethod_reloadGrid( void );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	//
	virtual BOOL OnWebCallback( int nCode, WEBCALLBACK_METHOD_PARAM_MAP& mapParams
		, CComVariant& varResult );

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
};
