#pragma once
#include <BaseHtmlDialog.h>
#include "resource.h"
#include "CxWebPageEvent.h"

class CCxWebNavigate : public CBaseHtmlDialog
{
	DECLARE_DYNAMIC(CCxWebNavigate)
public:
	CCxWebNavigate(CWnd* pParent = NULL);
	virtual ~CCxWebNavigate(void);

	enum { IDD = IDD_WEB };
	DECLARE_DISPATCH_MAP()

	DECLARE_EVENTSINK_MAP()

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	virtual BOOL OnWebCallback( int nCode, WEBCALLBACK_METHOD_PARAM_MAP& mapParams
		, CComVariant& varResult );

	void OnWebPageEvent( CCxWebPageEvent * pEvent );

	void NavigateErrorExplorer1(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Frame, VARIANT* StatusCode, BOOL* Cancel);
	// 取消右键菜单
	STDMETHOD(ShowContextMenu)(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved);
};

