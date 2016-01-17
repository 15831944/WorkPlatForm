#include "stdafx.h"
#include "DlgPopIE.h"
//#include "skinse.h"
#include <comutil.h>
#include "BaseHtmlDialog.h"

IMPLEMENT_DYNCREATE(CDlgPopIE, CBaseHtmlDialog)

BEGIN_MESSAGE_MAP(CDlgPopIE, CBaseHtmlDialog)
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CDlgPopIE)
END_DHTML_EVENT_MAP()

BEGIN_DISPATCH_MAP(CDlgPopIE, CBaseHtmlDialog)
	DISP_FUNCTION(CDlgPopIE, "returnValue2", returnValue2, VT_EMPTY, VTS_VARIANT)
	DISP_FUNCTION(CDlgPopIE, "dialogArguments2", dialogArguments2, VT_VARIANT, VTS_NONE) 
	DISP_FUNCTION(CDlgPopIE, "CustomModalNavigate2", CustomModalNavigate2, VT_VARIANT, VTS_BSTR VTS_I4 VTS_I4 VTS_VARIANT) 
	DISP_FUNCTION(CDlgPopIE, "CustomModalNavigateFullScreen2", CustomModalNavigateFullScreen2, VT_VARIANT, VTS_BSTR VTS_VARIANT)
END_DISPATCH_MAP()

BEGIN_EVENTSINK_MAP(CDlgPopIE, CBaseHtmlDialog)
	ON_EVENT(CDlgPopIE, AFX_IDC_BROWSER, 263, _WindowClosing, VTS_BOOL VTS_PBOOL)	
END_EVENTSINK_MAP()

CDlgPopIE::CDlgPopIE(CString sUrl, CWnd* pParent /*=NULL*/)
	: CBaseHtmlDialog(CDlgPopIE::IDD, pParent)
{
	m_sUrl = sUrl;
	VariantInit(&m_varArg);
}

CDlgPopIE::~CDlgPopIE(void)
{
	VariantClear(&m_varArg);
}

void CDlgPopIE::DoDataExchange(CDataExchange* pDX)
{
	CBaseHtmlDialog::DoDataExchange(pDX);
}

BOOL CDlgPopIE::OnInitDialog()
{
	CBaseHtmlDialog::OnInitDialog();

//	SkinSE_Init(m_hWnd, TRUE);

	CRect rc1;
	GetWindowRect(&rc1);

	if (m_bFullScreen/* && m_bModal == FALSE*/)
	{
		//SetWindowPos(NULL, -4, -3, m_nWidth + 7, m_nHeight + 6, SWP_NOZORDER);
		//SetWindowPos(NULL, 0, 0, m_nWidth, m_nHeight, SWP_NOZORDER);
		SetWindowPos(NULL, -1, -1, m_nWidth+2, m_nHeight+2, SWP_NOZORDER);
	}
	else
	{
		CRect rcArce;
		SystemParametersInfo(SPI_GETWORKAREA,0,&rcArce,SPIF_SENDCHANGE);
		int screenHeight = rcArce.Height();
		int screenWidth = rcArce.Width();
		SetWindowPos(NULL, (screenWidth - m_nWidth)/2, (screenHeight - m_nHeight)/2, m_nWidth, m_nHeight, SWP_NOZORDER);
	}

//	WRITELOG(m_sUrl);
	Navigate(m_sUrl);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDlgPopIE::returnValue2( VARIANT* value )
{
	m_varReturnValue = *value;
}

VARIANT CDlgPopIE::dialogArguments2()
{
	_variant_t varReturn = m_varArg;
	return varReturn.Detach();
}

void CDlgPopIE::SetDialogArguments(VARIANT* var)
{
	if (var && var->vt != VT_ERROR)
	{
		VariantCopy(&m_varArg, var);
	}
}

void CDlgPopIE::_WindowClosing( BOOL /*IsChildWindow*/, BOOL* Cancel )
{
	*Cancel = TRUE;

	EndDialog(IDOK); return;
}

void CDlgPopIE::OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
	CBaseHtmlDialog::OnDocumentComplete(pDisp, szUrl);
}
void CDlgPopIE::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	if (nState == FALSE)
	{
		::SetFocus(0);
	}

	CBaseHtmlDialog::OnActivate(nState, pWndOther, bMinimized);

	// TODO: 在此处添加消息处理程序代码
}

CComVariant CDlgPopIE::CustomModalNavigate2( char* sURL, int nWidth, int nHeight, VARIANT* var )
{
	CDlgPopIE dlg(sURL);
	dlg.m_nWidth = nWidth;
	dlg.m_nHeight = nHeight;
	dlg.m_bFullScreen = FALSE;
	dlg.SetDialogArguments(var);

	dlg.DoModal();

	/*CComVariant v("");*/
	return dlg.m_varReturnValue;
}
CComVariant CDlgPopIE::CustomModalNavigateFullScreen2( char* sURL, VARIANT* var )
{
	CDlgPopIE dlg(sURL);

	CRect rcArce;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rcArce,SPIF_SENDCHANGE);
	dlg.m_nHeight = rcArce.Height();
	dlg.m_nWidth = rcArce.Width();
	dlg.m_bFullScreen = TRUE;
	dlg.SetDialogArguments(var);

	dlg.DoModal();

	/*CComVariant v("");*/
	return dlg.m_varReturnValue;
}