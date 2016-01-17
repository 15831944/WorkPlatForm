#pragma once
#include "BaseHtmlDialog.h"
#include "resource.h"

#ifdef _WIN32_WCE
#error "CBaseHtmlDialog is not supported for Windows CE."
#endif 

class CDlgPopIE : public CBaseHtmlDialog
{
	DECLARE_DYNCREATE(CDlgPopIE)
public:
	CDlgPopIE(CString sUrl = "", CWnd* pParent = NULL);
	virtual ~CDlgPopIE(void);

	// Dialog Data
	enum { IDD = IDD_POPIE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_EVENTSINK_MAP()

public:
	CString m_sUrl;
	CComVariant m_varReturnValue;
	VARIANT m_varArg;

	void returnValue2( VARIANT* value );
	VARIANT dialogArguments2();
	void SetDialogArguments(VARIANT* var);
	CComVariant CustomModalNavigate2( char* sURL, int nWidth, int nHeight, VARIANT* var );
	CComVariant CustomModalNavigateFullScreen2( char* sURL, VARIANT* var );

	int m_nWidth;
	int m_nHeight;
	BOOL m_bFullScreen;
	void _WindowClosing(BOOL IsChildWindow, BOOL* Cancel);
	virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};
