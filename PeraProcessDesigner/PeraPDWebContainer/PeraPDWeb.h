#pragma once
#include <BaseHtmlDialog.h>
#include "resource.h"

class CPeraPDWeb : public CBaseHtmlDialog
{
	DECLARE_DYNAMIC(CPeraPDWeb)
public:
	CPeraPDWeb(CWnd* pParent = NULL);
	virtual ~CPeraPDWeb(void);

	enum { IDD = IDD_WEB };
	DECLARE_DISPATCH_MAP()

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Ö§³Ö
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	virtual BOOL OnWebCallback( int nCode, WEBCALLBACK_METHOD_PARAM_MAP& mapParams
		, CComVariant& varResult );

	BOOL CancelRunFlow(void);

	CString getClientSubFlowId( void );

	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

