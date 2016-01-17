#include "stdafx.h"
#include "ConfigFunc.h"
#include "Tools.h"
//#import "PeraClientConfig.tlb"
#include "peraclientconfig.tlh"

#include <map>

typedef std::map<CString, CString> MAP_KEY;
typedef std::map<CString, MAP_KEY> MAP_SECTION;

static MAP_SECTION s_mapSection;

CString GetConfig(CString sSection, CString sKey, BOOL bReRead /*= FALSE*/)
{
	if (sSection.IsEmpty() || sKey.IsEmpty())
	{
		return "";
	}

	CString sValue("");
 
 	if (!bReRead)
	{
		sValue = s_mapSection[sSection][sKey];
 		if (sValue.IsEmpty())
 		{
 			bReRead = TRUE;
 		}
 	}
 
 	if (bReRead)
	{
		HRESULT hr = ::CoInitialize(NULL);
		try
		{
			PeraClientConfigLib::IClientConfigPtr piClientConfig(__uuidof(PeraClientConfigLib::ClientConfig));
			_bstr_t bstrSection(sSection);
			_bstr_t bstrKey(sKey);
			_bstr_t bstrOut = piClientConfig->Get(bstrSection, bstrKey);
			sValue = (LPCTSTR)bstrOut;
		}
		catch (_com_error e)
		{
			CString strErrorMsg = e.ErrorMessage();
			ZTools::WriteZToolsFormatLog("配置管理组件调用异常,%s", strErrorMsg);
			if (SUCCEEDED(hr))
			{
				::CoUninitialize();
			}
			return "";
		}
		catch (COleException* e)
		{
			TCHAR szCause[255];
			CString strFormatted;
			e->GetErrorMessage(szCause, 255);
			ZTools::WriteZToolsFormatLog("配置管理组件调用异常,%s", szCause);
			if (SUCCEEDED(hr))
			{
				::CoUninitialize();
			}
			return "";
		}
		catch (CException* e)
		{
			TCHAR szCause[255];
			CString strFormatted;
			e->GetErrorMessage(szCause, 255);
			ZTools::WriteZToolsFormatLog("配置管理组件调用异常,%s", szCause);
			if (SUCCEEDED(hr))
			{
				::CoUninitialize();
			}
			return "";
		}
		if (SUCCEEDED(hr))
		{
			::CoUninitialize();
		}

		s_mapSection[sSection][sKey] = sValue;
	}

	return sValue;
}

BOOL SetConfig(CString sSection, CString sKey, CString sValue)
{
	if (sSection.IsEmpty() || sKey.IsEmpty())
	{
		return FALSE;
	}

	BOOL bSucess = FALSE;

	HRESULT hr = ::CoInitialize(NULL);
	try
	{
		PeraClientConfigLib::IClientConfigPtr piClientConfig(__uuidof(PeraClientConfigLib::ClientConfig));
		_bstr_t bstrSection(sSection);
		_bstr_t bstrKey(sKey);
		_bstr_t bstrValue(sValue);
		_bstr_t bstrOut = piClientConfig->Set(bstrSection, bstrKey, bstrValue);
		CString sOut = (LPCTSTR)bstrOut;
		if (sOut.IsEmpty())
		{
			bSucess = TRUE;
			s_mapSection[sSection][sKey] = sValue;
		}
	}
	catch (_com_error e)
	{
		CString strErrorMsg = e.ErrorMessage();
		ZTools::WriteZToolsFormatLog("配置管理组件调用异常,%s", strErrorMsg);
		if (SUCCEEDED(hr))
		{
			::CoUninitialize();
		}
		return bSucess;
	}
	catch (COleException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("配置管理组件调用异常,%s", szCause);
		if (SUCCEEDED(hr))
		{
			::CoUninitialize();
		}
		return bSucess;
	}
	catch (CException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("配置管理组件调用异常,%s", szCause);
		if (SUCCEEDED(hr))
		{
			::CoUninitialize();
		}
		return bSucess;
	}
	if (SUCCEEDED(hr))
	{
		::CoUninitialize();
	}

	return bSucess;
}

