// BaseHtmlDialog.cpp : implementation file
//

#include "stdafx.h"
#include "BaseHtmlDialog.h"
#include "PtrVectorHeader.h"
#include <atlcomcli.h>
#include <afxstr.h>
#include "ConvertData.h"
#include "Tools.h"

UINT WM_WEBPAGE_PROCESS_ATTACHED = ::RegisterWindowMessage (_T("WM_WEBPAGE_PROCESS_ATTACHED"));
UINT WM_WEBPAGE_LOADWEB_FINISHED = ::RegisterWindowMessage (_T("WM_WEBPAGE_LOADWEB_FINISHED"));

BEGIN_MESSAGE_MAP(CBaseHtmlDialog, CDHtmlDialog)
	ON_BN_CLICKED(IDOK, &CBaseHtmlDialog::OnBnClickedOk)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CBaseHtmlDialog, CDHtmlDialog)
	DISP_FUNCTION(CBaseHtmlDialog, "InvokeAppMethod", WebCallback, VT_VARIANT, VTS_I4 VTS_VARIANT)
END_DISPATCH_MAP()

BEGIN_EVENTSINK_MAP(CBaseHtmlDialog, CDHtmlDialog)
	ON_EVENT(CBaseHtmlDialog, AFX_IDC_BROWSER, 106, DownloadBeginExplorer1, VTS_NONE)
END_EVENTSINK_MAP()

IMPLEMENT_DYNAMIC(CBaseHtmlDialog, CDHtmlDialog)

CBaseHtmlDialog::CBaseHtmlDialog(UINT nID, CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(nID, NULL, pParent)
{
	EnableAutomation();
	m_htmlWindowEventSink=NULL;
	m_lpsHtmlWindow = NULL;
	m_hwndRecvMsg = NULL;
}

CBaseHtmlDialog::~CBaseHtmlDialog()
{
}

void CBaseHtmlDialog::OnBnClickedOk()
{
	//按回车不关闭对话框
}

void CBaseHtmlDialog::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BOOL CBaseHtmlDialog::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();
	SetExternalDispatch(GetIDispatch(TRUE));
	//SetHostFlags(0);

#ifdef _NO_JS_ERROR_
	m_pBrowserApp->put_Silent(VARIANT_TRUE);
#endif

	//屏掉所有脚本错误提示，建议只在给客户版本的时候用，平时测试时放开
	//m_pBrowserApp->put_Silent(VARIANT_TRUE);

	return TRUE;
}

void CBaseHtmlDialog::OnNavigateComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
	//CDHtmlDialog::OnNavigateComplete(pDisp, szUrl);
	try
	{
		if (pDisp != m_pBrowserApp)
			return;

		//模拟点击一次
		HWND hWndIE = m_wndBrowser.m_hWnd;
		if (hWndIE)
		{
			hWndIE = ::FindWindowEx(hWndIE, NULL, "Shell DocObject View", NULL);
			if (hWndIE)
			{
				hWndIE = ::FindWindowEx(hWndIE, NULL, "Internet Explorer_Server", NULL);
				if (hWndIE)
				{
					::PostMessage(hWndIE, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(-3000,-3000));
					::PostMessage(hWndIE, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(-3000,-3000));
				}
			}
		}

		IDispatch *pdispDoc = NULL;
		m_pBrowserApp->get_Document(&pdispDoc);
		if (!pdispDoc)
			return;

		//这个类是为了没有框架的页面设计的，有框架的页面可能会有不可预料的问题
		//Assert是为了确保页面上没有框架。
		//ASSERT(m_spHtmlDoc==NULL);
		//CComPtr<IHTMLDocument2> spHtmlDoc;
		m_spHtmlDoc = NULL;

		pdispDoc->QueryInterface(IID_IHTMLDocument2, (void **) &m_spHtmlDoc);

		if (m_bUseHtmlTitle)
		{
			CComBSTR bstrTitle;
			m_spHtmlDoc->get_title(&bstrTitle);
			//lc,国际化乱码问题
			CString str = (LPCTSTR)_bstr_t(bstrTitle);
			SetWindowText(str);
		}

		m_strCurrentUrl = szUrl;

		ConnectDHtmlEvents(pdispDoc);
		pdispDoc->Release();
	}
	catch (...)
	{
		//WRITELOG("发生异常 CBaseHtmlDialog::OnNavigateComplete")
	}
}

void CBaseHtmlDialog::OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
	try
	{
		CDHtmlDialog::OnDocumentComplete(pDisp, szUrl);

		if (pDisp == m_pBrowserApp)
		{
			if (m_spHtmlDoc)
			{
				CComBSTR bstrTitle;
				m_spHtmlDoc->get_title(&bstrTitle);
				CString sTitle;
				//lc,国际化乱码问题
				sTitle = (LPCTSTR)_bstr_t(bstrTitle);
				if (sTitle.IsEmpty())
				{
					sTitle = "无标题";
				}
				SetWindowText(sTitle);

				//ConnectDHtmlEvents(m_spHtmlDoc);
			}

			m_normalPageLoad = false;
			//模拟点击一次
			HWND hWndIE = m_wndBrowser.m_hWnd;
			if (hWndIE)
			{
				hWndIE = ::FindWindowEx(hWndIE, NULL, "Shell DocObject View", NULL);
				if (hWndIE)
				{
					hWndIE = ::FindWindowEx(hWndIE, NULL, "Internet Explorer_Server", NULL);
					if (hWndIE)
					{
						::PostMessage(hWndIE, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(-3000,-3000));
						::PostMessage(hWndIE, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(-3000,-3000));
					}
				}
			}
		}

	}
	catch (...)
	{
		//WRITELOG("发生异常 CBaseHtmlDialog::OnDocumentComplete")
	}
}

//BOOL CBaseHtmlDialog::CanAccessExternal()
//{
//	return TRUE;
//}

BOOL CBaseHtmlDialog::IsExternalDispatchSafe()
{
	//屏蔽掉网页弹出的ActiveX安全警告对话框
	return TRUE;
}

//禁止拖文件到容器
STDMETHODIMP CBaseHtmlDialog::GetDropTarget(IDropTarget *pDropTarget, IDropTarget **ppDropTarget)
{
	return S_FALSE;
}


STDMETHODIMP CBaseHtmlDialog::TranslateAccelerator(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID)
{
// 	if ( ::IsWindow( m_hwndRecvMsg ) )
// 	::PostMessage(m_hwndRecvMsg, lpMsg->message, lpMsg->wParam, lpMsg->lParam);
// 	CString s;
// 	s.Format("\n---------------------------------------------\n%d---%d---%d----%d", lpMsg->lParam, lpMsg->message, lpMsg->wParam);
// 	OutputDebugString(s);
	return S_FALSE;
}

//
//CComVariant CBaseHtmlDialog::CString2Variant(LPCSTR lpszValue)
//{
//	CComVariant variant((LPCSTR)lpszValue);
//	return variant;
//}
//
//CComVariant CBaseHtmlDialog::CStringW2Variant(LPCWSTR lpszValue)
//{
//	CComVariant variant((LPCOLESTR)lpszValue);
//	return variant;
//}

DISPID CBaseHtmlDialog::FindId(LPDISPATCH pObj, LPOLESTR pName)
{
	try
	{
		DISPID id = 0;
		if(FAILED(pObj->GetIDsOfNames(IID_NULL,&pName,1,LOCALE_SYSTEM_DEFAULT,&id))) id = -1;
		return id;
	}
	catch (...)
	{
		//WRITELOG("发生异常 CBaseHtmlDialog::FindId");
		return 0;
	}
}

HRESULT CBaseHtmlDialog::InvokeMethod(LPDISPATCH pObj, LPOLESTR pName, VARIANT *pVarResult, VARIANT *p, int cArgs)
{
	try
	{
		DISPID dispid = FindId(pObj, pName);
		if(dispid == -1) return E_FAIL;

		DISPPARAMS ps;
		ps.cArgs = cArgs;
		ps.rgvarg = p;
		ps.cNamedArgs = 0;
		ps.rgdispidNamedArgs = NULL;

		return pObj->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &ps, pVarResult, NULL, NULL);
	}
	catch (...)
	{
		//WRITELOG("发生异常 CBaseHtmlDialog::InvokeMethod");
		return E_FAIL;
	}
}

HRESULT CBaseHtmlDialog::GetProperty(LPDISPATCH pObj, LPOLESTR pName, VARIANT *pValue)
{
	try
	{
		DISPID dispid = FindId(pObj, pName);
		if(dispid == -1) return E_FAIL;

		DISPPARAMS ps;
		ps.cArgs = 0;
		ps.rgvarg = NULL;
		ps.cNamedArgs = 0;
		ps.rgdispidNamedArgs = NULL;

		return pObj->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &ps, pValue, NULL, NULL);
	}
	catch (...)
	{
		//WRITELOG("发生异常 CBaseHtmlDialog::GetProperty");
		return E_FAIL;
	}
}

HRESULT CBaseHtmlDialog::SetProperty(LPDISPATCH pObj, LPOLESTR pName, VARIANT *pValue)
{
	try
	{
		DISPID dispid = FindId(pObj, pName);
		if(dispid == -1) return E_FAIL;

		DISPPARAMS ps;
		ps.cArgs = 1;
		ps.rgvarg = pValue;
		ps.cNamedArgs = 0;
		ps.rgdispidNamedArgs = NULL;

		return pObj->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYPUT, &ps, NULL, NULL, NULL);
	}
	catch (...)
	{
		//WRITELOG("发生异常 CBaseHtmlDialog::SetProperty");
		return E_FAIL;
	}
}

/*
功能：
	在IE中，调用 DSE 中的方法。
参数：
	nCode: 事件代码。
	lpData: 事件的参数，目前只接受结构体，结构体声明如下：
		var st = {};
		st.paramnamelist = "p1,p2";
		st.p1 = 123;
		st.p2 = "参数2";
调用方法：
	var ret = external.InvokeAppMethod(1, st);
	alert(ret);
*/
CComVariant CBaseHtmlDialog::WebCallback(int nCode, LPVARIANT lpData)
{
	CComVariant varReturn;
	if (lpData == NULL)
	{
		::AfxMessageBox( "异常，CBaseHtmlDialog::WebCallback lpData == NULL" );
		return varReturn;
	}
	//vector<CComVariant> vParams;

	WEBCALLBACK_METHOD_PARAM_MAP mapParams;

	try
	{
		CComVariant varData = *lpData;
		switch (varData.vt)
		{
		case VT_DISPATCH:
			ParseWebCallbackParams(mapParams, varData.pdispVal);
			break;
		default:
			::AfxMessageBox( "异常，CBaseHtmlDialog::WebCallback varData.vt != VT_DISPATCH" );
			return varReturn;
		}

		OnWebCallback(nCode, mapParams, varReturn);
		mapParams.clear();
		return varReturn;
	}
	catch (...)
	{
		//WRITELOG("发生异常 CBaseHtmlDialog::WebCallback");
		mapParams.clear();
		return varReturn;
	}
}

BOOL CBaseHtmlDialog::OnWebCallback( int nCode, WEBCALLBACK_METHOD_PARAM_MAP& mapParams
	, CComVariant& varResult )
{
	return TRUE;
}

int CBaseHtmlDialog::ParseWebCallbackParams(vector<CComVariant>& vParams, LPDISPATCH lpParamObj)
{
	vParams.clear();
	if (lpParamObj == NULL) return 0;

	CComVariant v("");
	HRESULT hr = GetProperty(lpParamObj, L"paramnamelist", &v);
	if (FAILED(hr))
	{
		::AfxMessageBox( "异常，CBaseHtmlDialog::ParseWebCallbackParams 获取 paramnamelist 属性失败" );
		return 0;
	}
	CStringW str;
	vector<CStringW> vParamNameList = CStringW2Vector(v.bstrVal);
	for (int i=0; i<(int)vParamNameList.size(); i++)
	{
		hr = GetProperty(lpParamObj, (LPOLESTR)(LPCWSTR)vParamNameList[i], &v);
		if (FAILED(hr)) continue;
		vParams.push_back(v);
	}
	
	return (int)vParams.size();
	return 0;
}

int CBaseHtmlDialog::ParseWebCallbackParams( WEBCALLBACK_METHOD_PARAM_MAP& mapParams , LPDISPATCH lpParamObj )
{
	mapParams.clear();
	if (lpParamObj == NULL) return 0;

	try
	{
		CComVariant varParamNameListStr("");
		HRESULT hr = GetProperty(lpParamObj, L"paramnamelist", &varParamNameListStr);
		if (FAILED(hr))
		{
			::AfxMessageBox( "异常，CBaseHtmlDialog::ParseWebCallbackParams 获取 paramnamelist 属性失败" );
			return 0;
		}
		CString strParamName;
		vector<CStringW> vParamNames = CStringW2Vector(varParamNameListStr.bstrVal);
		for (int i=0; i<(int)vParamNames.size(); i++)
		{
			CComVariant varParam;
			hr = GetProperty(lpParamObj, (LPOLESTR)(LPCWSTR)vParamNames[i], &varParam);
			if (FAILED(hr)) continue;
			//lc,国际化乱码问题
			strParamName = CStrW2CStrT(vParamNames[i]);
			mapParams.insert( make_pair( strParamName, varParam ) );
		}
	}
	catch (...)
	{
		::AfxMessageBox( "异常，CBaseHtmlDialog::ParseWebCallbackParams try-catch" );
		mapParams.clear();
		return 0;
	}
	return (int)mapParams.size();
}

/*
功能：
	在Dse中，调用IE中的 js方法。注意，如果有多个参数，需要倒序传递参数。
示例：
	CComVariant vParam[3];
	vParam[0] = L"这是第3个参数";
	vParam[1] = L"这是第2个参数";
	vParam[2] = L"这是第1个参数";

	CComVariant vResult;
	BOOL bOk = CallWebMethod(L"testCallWebMethod", vResult, (CComVariant *)vParam, 3);
*/
BOOL CBaseHtmlDialog::InvokeJsMethod( CWebMethodData * pMethodData
	, CComPtr<IHTMLWindow2> pHtmlWindow )
{
	CComVariant varResult;
	if (pMethodData == NULL) return FALSE;

	//////////////////////////////////////////////////////////////////////////
		CString str, strLog;
		str.Format("MethodName: %s\r\n", pMethodData->m_strMethodName);
		strLog += str;
		for (int i=0; i<(int)pMethodData->m_vParams.size(); i++)
		{
			str.Format("Param[%i]:\r\n    name: %s\r\n    value: %s\r\n"
				, i
				, pMethodData->m_vParams[i]->m_strName
				, pMethodData->m_vParams[i]->m_strValue);
			strLog += str;
		}
		//ZTools::WriteZToolsFormatLog("InvokeJsMethod\r\n%s\r\n", strLog);
	//////////////////////////////////////////////////////////////////////////

	if ( pHtmlWindow == NULL )
	{
		pHtmlWindow = GetIHTMLWindow2();

		if ( pHtmlWindow == NULL )
		{
			::MessageBox( NULL, m_strLog, NULL, MB_OK|MB_ICONINFORMATION );
			pMethodData->m_strInfo = m_strLog;
			m_strLog = "";
			return FALSE;
		}
	}

	try
	{
		varResult = L"";
		
		CStringW strMethodNameW;
		strMethodNameW = pMethodData->m_strMethodName;

		//////////////////////////////////////////////////////////////////////////

		{
			DISPID dispid = FindId( pHtmlWindow, const_cast<LPOLESTR>((LPCWSTR)strMethodNameW) );
			if ( dispid == -1 )
			{
				pMethodData->m_strInfo += "\r\nFindId 返回 -1";
				return FALSE;
			}

			HRESULT hr = S_OK;
			CComBSTR bstrTemp;
			DISPPARAMS ps;
			memset( &ps, NULL, sizeof(DISPPARAMS) );
			ps.cArgs = (UINT)pMethodData->m_vParams.size();
			ps.rgvarg = new VARIANTARG[ps.cArgs];
			int nPos = 0;
			for (int i = ps.cArgs - 1; i >= 0; i--)
			{
				bstrTemp = _bstr_t(pMethodData->m_vParams[i]->m_strValue).GetBSTR();
				ps.rgvarg[nPos].vt = VT_BSTR;
				ps.rgvarg[nPos].bstrVal = SysAllocString( bstrTemp.m_str );
				nPos++;
			}

			hr = pHtmlWindow->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD
				, &ps, (LPVARIANT)&varResult, NULL, NULL);

			for (int i = (int)pMethodData->m_vParams.size() - 1; i>=0; i--)
			{
				SysFreeString( ps.rgvarg[i].bstrVal );
			}
			delete[] ps.rgvarg;

			varResult.ChangeType(VT_BSTR);
			//pMethodData->m_strResult = varResult.bstrVal;
			//lc,国际化乱码问题
			pMethodData->m_strResult = (LPCTSTR)_bstr_t(varResult.bstrVal);

			str.Format("Result: %s\r\n", pMethodData->m_strResult);
			strLog += str;
			ZTools::WriteZToolsFormatLog( "%s", strLog );
	
			if ( FAILED(hr) )
			{
				pMethodData->m_strInfo += "\r\npHtmlWindow->Invoke 失败";
				return FALSE;
			}
		}

		//////////////////////////////////////////////////////////////////////////

		return TRUE;
	}
	catch (...)
	{
		pMethodData->m_strInfo += "\r\nCBaseHtmlDialog::InvokeJsMethod 发生异常";
		return FALSE;
	}
}



void CBaseHtmlDialog::DownloadBeginExplorer1()
{
	try
	{

		if (!m_normalPageLoad)
		{
			bool isRefresh=true;
			CComPtr<IDispatch> spDisp;
			HRESULT hr=m_pBrowserApp->get_Document(&spDisp);
			if (SUCCEEDED(hr) && spDisp)
			{
				//ATLTRACE("---Refresh - got document\n");
				CComQIPtr<IHTMLDocument2> spHtmlDoc=spDisp;
				if (spHtmlDoc)
				{
					//ATLTRACE("---Refresh - got html document\n");
					CComBSTR state;
					hr=spHtmlDoc->get_readyState(&state);
					if (SUCCEEDED(hr) && state==L"complete")
						isRefresh=false;					
				}
			}
			// not normal, so looks like we've got a refresh here
			if (!m_htmlWindowEventSink && isRefresh)
			{
				//ATLTRACE("---got Refresh\n");
				setWindowHandler();
				//MessageBox(m_hWnd, L"Main Document is being refreshed", L"Refresh Started", MB_OK);
			}
		}
	}
	catch (...)
	{
		//WRITELOG("发生异常 CBaseHtmlDialog::DownloadBeginExplorer1");
	}
}
// set the window event handler to catch OnLoad for refreshes
void CBaseHtmlDialog::setWindowHandler()
{
	try
	{
		HRESULT hr;
		CComPtr<IDispatch> spDisp;
		hr=m_pBrowserApp->get_Document(&spDisp);
		if (SUCCEEDED(hr) && spDisp)
		{
			CComQIPtr<IHTMLDocument2> spHtmlDoc=spDisp;
			if (spHtmlDoc)
			{
				CComPtr<IHTMLWindow2> spWin;
				hr=spHtmlDoc->get_parentWindow(&spWin);
				if (SUCCEEDED(hr) && spWin){
					m_htmlWindowEventSink=new CHtmlWindowEventSink(this);
					m_htmlWindowEventSink->advise(spWin);
				}
			}
		}

	}
	catch (...)
	{
		//WRITELOG("发生异常 CBaseHtmlDialog::setWindowHandler");
	}
}

void CBaseHtmlDialog::onLoadOccured()
{
	try
	{
		//模拟点击一次
		HWND hWndIE = m_wndBrowser.m_hWnd;
		if (hWndIE)
		{
			hWndIE = ::FindWindowEx(hWndIE, NULL, "Shell DocObject View", NULL);
			if (hWndIE)
			{
				hWndIE = ::FindWindowEx(hWndIE, NULL, "Internet Explorer_Server", NULL);
				if (hWndIE)
				{
					::PostMessage(hWndIE, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(-3000,-3000));
					::PostMessage(hWndIE, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(-3000,-3000));
				}
			}
		}

	}
	catch (...)
	{
		//WRITELOG("发生异常 CBaseHtmlDialog::setWindowHandler");
	}
}

void CBaseHtmlDialog::onUnloadOccured()
{
}

void CBaseHtmlDialog::releasePointer()
{
	try
	{
		m_htmlWindowEventSink=NULL;

	}
	catch (...)
	{
		//WRITELOG("发生异常 CBaseHtmlDialog::releasePointer");
	}
}

void CBaseHtmlDialog::OnBeforeNavigate( LPDISPATCH pDisp, LPCTSTR szUrl )
{
	try
	{
		if (pDisp == m_pBrowserApp)
		{
			//lichao,20121025,zhd-1234,某些情况下刷新完页面还有行1char1的脚本错误问题
			CString sCheckUrl(szUrl);
			sCheckUrl.MakeLower();
			if (sCheckUrl.Find("javascript:") != 0)
			{
				m_normalPageLoad = true;
			}
			CDHtmlDialog::OnBeforeNavigate(pDisp, szUrl);
		}

	}
	catch (...)
	{
		//WRITELOG("发生异常 CBaseHtmlDialog::OnBeforeNavigate");
	}
}

void CBaseHtmlDialog::Navigate(LPCTSTR lpszURL, DWORD dwFlags /*= 0*/, 
	LPCTSTR lpszTargetFrameName /*= NULL*/, LPCTSTR lpszHeaders /*= NULL*/,
	LPVOID lpvPostData /*= NULL*/, DWORD dwPostDataLen /*= 0*/)
{
	CComBSTR bstrURL = _bstr_t(lpszURL).GetBSTR();

	COleSafeArray vPostData;
	if (lpvPostData != NULL)
	{
		if (dwPostDataLen == 0)
			dwPostDataLen = lstrlen((LPCTSTR) lpvPostData);

		vPostData.CreateOneDim(VT_UI1, dwPostDataLen, lpvPostData);
	}

	//工作台中页面跳转时关闭历史记录，使得后退键不会导致页面后退，（马贺那边BUG）
	DWORD dwNewFlags = dwFlags | navNoHistory;

	m_pBrowserApp->Navigate(bstrURL,
		COleVariant((long) dwNewFlags, VT_I4),
		COleVariant(lpszTargetFrameName, VT_BSTR),
		vPostData,
		COleVariant(lpszHeaders, VT_BSTR));
}

/************************************************************************
取得框架的 IID_IHTMLDocument2 指针。
注意：此函数只能在主线程中调用。
工作线程中直接调用会导致某些IE浏览器崩溃。
*/
CComPtr<IHTMLWindow2> CBaseHtmlDialog::GetIHTMLWindow2( void )
{
	CComPtr<IHTMLWindow2> pHtmlWindow;
	
	try
	{
		IDispatch *pdispDoc = NULL;
		if (m_pBrowserApp == NULL)
		{
			m_strLog += "\r\nGetIHTMLWindow2::m_pBrowserApp == NULL";
			return pHtmlWindow;
		}
		m_pBrowserApp->get_Document(&pdispDoc);
		if (pdispDoc == NULL)
		{
			m_strLog += "\r\nCBaseHtmlDialog::m_pBrowserApp->get_Document 查询接口失败";
			return pHtmlWindow;
		}

		if (m_spHtmlDoc == NULL)
		{
			m_strLog += "\r\nCBaseHtmlDialog::m_spHtmlDoc == NULL";
			pdispDoc->QueryInterface(IID_IHTMLDocument2, (void **) &m_spHtmlDoc);

			if (m_spHtmlDoc == NULL)
			{
				m_strLog += "\r\n查询IID_IHTMLDocument2接口后，CBaseHtmlDialog::m_spHtmlDoc == NULL";
				return pHtmlWindow;
			}
		}

		m_spHtmlDoc->get_parentWindow(&pHtmlWindow);

		if (pHtmlWindow == NULL)
		{
			m_strLog += "\r\npHtmlWindow == NULL";
			return pHtmlWindow;
		}
	}
	catch (...)
	{
		m_strLog += "\r\n异常，CBaseHtmlDialog::GetIHTMLWindow2";
		return pHtmlWindow;
	}
	return pHtmlWindow;
}

/************************************************************************
序列化 pHtmlWindow。
如果 需要在工作线程中使用 pHtmlWindow 指针，首先要在主线程中序列化 pHtmlWindow。
*/
LPSTREAM CBaseHtmlDialog::CreateHtmlWindowStream( CComPtr<IHTMLWindow2>& pHtmlWindow )
{
	LPSTREAM lps = NULL;
	if ( S_OK != CoMarshalInterThreadInterfaceInStream(IID_IDispatch, pHtmlWindow, &lps) )
	{
		return NULL;
	}
	return lps;
}

/************************************************************************
反序列化 pHtmlWindow。
在工作线程中使用 pHtmlWindow 指针，需要在工作线程中反序列化 LPSTREAM 对象获得。
*/
CComPtr<IHTMLWindow2> CBaseHtmlDialog::ReleaseAndGetHtmlWindow( LPSTREAM& lps )
{
	CComPtr<IDispatch> script;
	// 反列集得到IDisPatch指针
	HRESULT hr = CoGetInterfaceAndReleaseStream( lps, IID_IDispatch, (LPVOID *)&script );
	if ( S_OK != hr )
	{
		CComQIPtr<IHTMLWindow2> pHtmlWindowNull;
		return pHtmlWindowNull;
	}
	lps = NULL;

	CComQIPtr<IHTMLWindow2> pHtmlWindow( script );
	if ( pHtmlWindow == NULL )
	{
		CComQIPtr<IHTMLWindow2> pHtmlWindowNull;
		return pHtmlWindowNull;
	}
	return pHtmlWindow;
}


BOOL CBaseHtmlDialog::PreTranslateMessage(MSG* pMsg)
{

	ASSERT_VALID(this);
	ASSERT(m_hWnd != NULL);
	
	CWnd* pOwner = GetOwner();

	// don't translate dialog messages when in Shift+F1 help mode
// 	CFrameWnd* pFrameWnd = GetTopLevelFrame();
// 	if (pFrameWnd != NULL && pFrameWnd->m_bHelpMode)
// 		return FALSE;

	// since 'IsDialogMessage' will eat frame window accelerators,
	//   we call all frame windows' PreTranslateMessage first
	while (pOwner != NULL)
	{
		//如果owner是对话框，会处理掉复制、粘贴等快捷键
		if (pOwner->GetRuntimeClass()->IsDerivedFrom(RUNTIME_CLASS (CDialog)))
		{
			break;
		}
		// allow owner & frames to translate before IsDialogMessage does
		if (pOwner->PreTranslateMessage(pMsg))
			return TRUE;

		// try parent frames until there are no parent frames
		if (IsWindow (pOwner->GetSafeHwnd ()))
		{
			pOwner = pOwner->GetParentFrame();
		}
		else
		{
			break;
		}
	}

	//屏F5刷新	
	if( pMsg->message ==WM_KEYDOWN)
	{
		if(pMsg-> wParam==VK_F5)
		{
			return TRUE;
		}
	}

	return __super::PreTranslateMessage(pMsg);
}
