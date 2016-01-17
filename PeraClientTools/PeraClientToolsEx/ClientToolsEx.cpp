// ClientToolsEx.cpp : CClientToolsEx 的实现

#include "stdafx.h"
#include "ClientToolsEx.h"
#include "Tools.h"
#include <comdef.h>
#include "GetGlobalDataFromReg.h"
#include "FileFunc.h"


// CClientToolsEx



STDMETHODIMP CClientToolsEx::CheckCustomProtocol(BSTR bstrProtocolName, VARIANT* pvRet)
{
	// TODO: 在此添加实现代码
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ZTools::WriteZToolsLog(">>>>>>PeraClientToolsEx::CClientToolsEx::CheckCustomProtocol");
	CComVariant vRet(_bstr_t("").GetBSTR());
	std::string strMsg;
	BOOL bInstalled = FALSE;

	std::string strProtocolName = (bstrProtocolName != NULL) ? _bstr_t(bstrProtocolName) : "";
	ZTools::WriteZToolsFormatLog("strProtocolName:%s", strProtocolName.c_str());
	
	CString sRegKey = "";
	CString sRegValue;
	CString sRegPath;
	sRegPath.Format(_T("%s\\shell\\open\\command"), strProtocolName.c_str());

	CRegKey keyUrlProtocol;
	if (keyUrlProtocol.Open(HKEY_CLASSES_ROOT, sRegPath, KEY_READ) == ERROR_SUCCESS)
	{
		DWORD pCount = 1024;
		char szValue[1024];
		memset(szValue, 0, 1024);
		if(keyUrlProtocol.QueryStringValue(sRegKey, szValue, &pCount)== ERROR_SUCCESS)
		{
			sRegValue = szValue;
			if (sRegValue.IsEmpty())
			{
				strMsg = "value is empty";
				ZTools::WriteZToolsLog(strMsg);
				bInstalled = FALSE;
			}
			else
			{				
				sRegValue = sRegValue.Left(sRegValue.Find(" %1"));
				sRegValue.Trim("\"");
				if (PathFileExists(sRegValue))
				{
					bInstalled = TRUE;
				}
				else
				{
					bInstalled = FALSE;
					strMsg = ZTools::FormatString("value is invalidate: %s", sRegValue);
					ZTools::WriteZToolsLog(strMsg);
				}
			}
		}
		else
		{
			strMsg = ZTools::FormatString(_T("Query HKEY_CLASSES_ROOT\\%s failed"), sRegPath);
			ZTools::WriteZToolsLog(strMsg);
			bInstalled = FALSE;
		}
	}
	else
	{
		strMsg = ZTools::FormatString(_T("open HKEY_CLASSES_ROOT\\%s failed"), sRegPath);
		ZTools::WriteZToolsLog(strMsg);
		bInstalled = FALSE;
	}

	keyUrlProtocol.Close();

	Json::Value jsonRet;
	jsonRet["flag"] = bInstalled ? Json::Value("true") : Json::Value("false");
	jsonRet["msg"] = Json::Value(strMsg);

	vRet = _bstr_t(jsonRet.toStyledString().c_str()).GetBSTR();

	vRet.Detach(pvRet);
	return S_OK;
}


STDMETHODIMP CClientToolsEx::InvokeWorkspace(void)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	CString strPath = GetPeraGlobalDataFromReg( "lm", "PeraProcessDesigner" );
	if ( !IsAccessibleDir( strPath ) )
	{
		return S_OK;
	}
	strPath += "\\PeraProcessDesigner.exe";
	if ( !IsAccessibleFile( strPath ) )
	{
		return S_OK;
	}

	ShellExecute(NULL, "open", strPath, NULL, NULL, SW_SHOW);

	return S_OK;
}


STDMETHODIMP CClientToolsEx::InvokePeraComponent(void)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	CString strPath = GetPeraGlobalDataFromReg( "lm", "PeraComponent" );
	if ( !IsAccessibleDir( strPath ) )
	{
		return S_OK;
	}
	strPath += "\\PeraComponent.exe";
	if ( !IsAccessibleFile( strPath ) )
	{
		return S_OK;
	}

	ShellExecute(NULL, "open", strPath, NULL, NULL, SW_SHOW);


	return S_OK;
}
