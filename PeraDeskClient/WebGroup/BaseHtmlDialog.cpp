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
	//���س����رնԻ���
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

	//�������нű�������ʾ������ֻ�ڸ��ͻ��汾��ʱ���ã�ƽʱ����ʱ�ſ�
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

		//ģ����һ��
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

		//�������Ϊ��û�п�ܵ�ҳ����Ƶģ��п�ܵ�ҳ����ܻ��в���Ԥ�ϵ�����
		//Assert��Ϊ��ȷ��ҳ����û�п�ܡ�
		//ASSERT(m_spHtmlDoc==NULL);
		//CComPtr<IHTMLDocument2> spHtmlDoc;
		m_spHtmlDoc = NULL;

		pdispDoc->QueryInterface(IID_IHTMLDocument2, (void **) &m_spHtmlDoc);

		if (m_bUseHtmlTitle)
		{
			CComBSTR bstrTitle;
			m_spHtmlDoc->get_title(&bstrTitle);
			//lc,���ʻ���������
			CString str = (LPCTSTR)_bstr_t(bstrTitle);
			SetWindowText(str);
		}

		m_strCurrentUrl = szUrl;

		ConnectDHtmlEvents(pdispDoc);
		pdispDoc->Release();
	}
	catch (...)
	{
		//WRITELOG("�����쳣 CBaseHtmlDialog::OnNavigateComplete")
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
				//lc,���ʻ���������
				sTitle = (LPCTSTR)_bstr_t(bstrTitle);
				if (sTitle.IsEmpty())
				{
					sTitle = "�ޱ���";
				}
				SetWindowText(sTitle);

				//ConnectDHtmlEvents(m_spHtmlDoc);
			}

			m_normalPageLoad = false;
			//ģ����һ��
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
		//WRITELOG("�����쳣 CBaseHtmlDialog::OnDocumentComplete")
	}
}

//BOOL CBaseHtmlDialog::CanAccessExternal()
//{
//	return TRUE;
//}

BOOL CBaseHtmlDialog::IsExternalDispatchSafe()
{
	//���ε���ҳ������ActiveX��ȫ����Ի���
	return TRUE;
}

//��ֹ���ļ�������
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
		//WRITELOG("�����쳣 CBaseHtmlDialog::FindId");
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
		//WRITELOG("�����쳣 CBaseHtmlDialog::InvokeMethod");
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
		//WRITELOG("�����쳣 CBaseHtmlDialog::GetProperty");
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
		//WRITELOG("�����쳣 CBaseHtmlDialog::SetProperty");
		return E_FAIL;
	}
}

/*
���ܣ�
	��IE�У����� DSE �еķ�����
������
	nCode: �¼����롣
	lpData: �¼��Ĳ�����Ŀǰֻ���ܽṹ�壬�ṹ���������£�
		var st = {};
		st.paramnamelist = "p1,p2";
		st.p1 = 123;
		st.p2 = "����2";
���÷�����
	var ret = external.InvokeAppMethod(1, st);
	alert(ret);
*/
CComVariant CBaseHtmlDialog::WebCallback(int nCode, LPVARIANT lpData)
{
	CComVariant varReturn;
	if (lpData == NULL)
	{
		::AfxMessageBox( "�쳣��CBaseHtmlDialog::WebCallback lpData == NULL" );
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
			::AfxMessageBox( "�쳣��CBaseHtmlDialog::WebCallback varData.vt != VT_DISPATCH" );
			return varReturn;
		}

		OnWebCallback(nCode, mapParams, varReturn);
		mapParams.clear();
		return varReturn;
	}
	catch (...)
	{
		//WRITELOG("�����쳣 CBaseHtmlDialog::WebCallback");
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
		::AfxMessageBox( "�쳣��CBaseHtmlDialog::ParseWebCallbackParams ��ȡ paramnamelist ����ʧ��" );
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
			::AfxMessageBox( "�쳣��CBaseHtmlDialog::ParseWebCallbackParams ��ȡ paramnamelist ����ʧ��" );
			return 0;
		}
		CString strParamName;
		vector<CStringW> vParamNames = CStringW2Vector(varParamNameListStr.bstrVal);
		for (int i=0; i<(int)vParamNames.size(); i++)
		{
			CComVariant varParam;
			hr = GetProperty(lpParamObj, (LPOLESTR)(LPCWSTR)vParamNames[i], &varParam);
			if (FAILED(hr)) continue;
			//lc,���ʻ���������
			strParamName = CStrW2CStrT(vParamNames[i]);
			mapParams.insert( make_pair( strParamName, varParam ) );
		}
	}
	catch (...)
	{
		::AfxMessageBox( "�쳣��CBaseHtmlDialog::ParseWebCallbackParams try-catch" );
		mapParams.clear();
		return 0;
	}
	return (int)mapParams.size();
}

/*
���ܣ�
	��Dse�У�����IE�е� js������ע�⣬����ж����������Ҫ���򴫵ݲ�����
ʾ����
	CComVariant vParam[3];
	vParam[0] = L"���ǵ�3������";
	vParam[1] = L"���ǵ�2������";
	vParam[2] = L"���ǵ�1������";

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
				pMethodData->m_strInfo += "\r\nFindId ���� -1";
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
			//lc,���ʻ���������
			pMethodData->m_strResult = (LPCTSTR)_bstr_t(varResult.bstrVal);

			str.Format("Result: %s\r\n", pMethodData->m_strResult);
			strLog += str;
			ZTools::WriteZToolsFormatLog( "%s", strLog );
	
			if ( FAILED(hr) )
			{
				pMethodData->m_strInfo += "\r\npHtmlWindow->Invoke ʧ��";
				return FALSE;
			}
		}

		//////////////////////////////////////////////////////////////////////////

		return TRUE;
	}
	catch (...)
	{
		pMethodData->m_strInfo += "\r\nCBaseHtmlDialog::InvokeJsMethod �����쳣";
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
		//WRITELOG("�����쳣 CBaseHtmlDialog::DownloadBeginExplorer1");
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
		//WRITELOG("�����쳣 CBaseHtmlDialog::setWindowHandler");
	}
}

void CBaseHtmlDialog::onLoadOccured()
{
	try
	{
		//ģ����һ��
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
		//WRITELOG("�����쳣 CBaseHtmlDialog::setWindowHandler");
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
		//WRITELOG("�����쳣 CBaseHtmlDialog::releasePointer");
	}
}

void CBaseHtmlDialog::OnBeforeNavigate( LPDISPATCH pDisp, LPCTSTR szUrl )
{
	try
	{
		if (pDisp == m_pBrowserApp)
		{
			//lichao,20121025,zhd-1234,ĳЩ�����ˢ����ҳ�滹����1char1�Ľű���������
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
		//WRITELOG("�����쳣 CBaseHtmlDialog::OnBeforeNavigate");
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

	//����̨��ҳ����תʱ�ر���ʷ��¼��ʹ�ú��˼����ᵼ��ҳ����ˣ�������Ǳ�BUG��
	DWORD dwNewFlags = dwFlags | navNoHistory;

	m_pBrowserApp->Navigate(bstrURL,
		COleVariant((long) dwNewFlags, VT_I4),
		COleVariant(lpszTargetFrameName, VT_BSTR),
		vPostData,
		COleVariant(lpszHeaders, VT_BSTR));
}

/************************************************************************
ȡ�ÿ�ܵ� IID_IHTMLDocument2 ָ�롣
ע�⣺�˺���ֻ�������߳��е��á�
�����߳���ֱ�ӵ��ûᵼ��ĳЩIE�����������
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
			m_strLog += "\r\nCBaseHtmlDialog::m_pBrowserApp->get_Document ��ѯ�ӿ�ʧ��";
			return pHtmlWindow;
		}

		if (m_spHtmlDoc == NULL)
		{
			m_strLog += "\r\nCBaseHtmlDialog::m_spHtmlDoc == NULL";
			pdispDoc->QueryInterface(IID_IHTMLDocument2, (void **) &m_spHtmlDoc);

			if (m_spHtmlDoc == NULL)
			{
				m_strLog += "\r\n��ѯIID_IHTMLDocument2�ӿں�CBaseHtmlDialog::m_spHtmlDoc == NULL";
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
		m_strLog += "\r\n�쳣��CBaseHtmlDialog::GetIHTMLWindow2";
		return pHtmlWindow;
	}
	return pHtmlWindow;
}

/************************************************************************
���л� pHtmlWindow��
��� ��Ҫ�ڹ����߳���ʹ�� pHtmlWindow ָ�룬����Ҫ�����߳������л� pHtmlWindow��
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
�����л� pHtmlWindow��
�ڹ����߳���ʹ�� pHtmlWindow ָ�룬��Ҫ�ڹ����߳��з����л� LPSTREAM �����á�
*/
CComPtr<IHTMLWindow2> CBaseHtmlDialog::ReleaseAndGetHtmlWindow( LPSTREAM& lps )
{
	CComPtr<IDispatch> script;
	// ���м��õ�IDisPatchָ��
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
		//���owner�ǶԻ��򣬻ᴦ������ơ�ճ���ȿ�ݼ�
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

	//��F5ˢ��	
	if( pMsg->message ==WM_KEYDOWN)
	{
		if(pMsg-> wParam==VK_F5)
		{
			return TRUE;
		}
	}

	return __super::PreTranslateMessage(pMsg);
}
