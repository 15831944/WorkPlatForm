#include "StdAfx.h"
#include "LoginFunc.h"
#include "Tools.h"
//#import "ProcessClientServices.tlb"
#include "processclientservices.tlh"

CString Login( CString sUser, CString sPassword )
{
	HRESULT hr = ::CoInitialize(NULL);
	CString sOut(""); 
	try
	{
		ProcessClientServicesLib::IRpcCallHelperPtr piClientHelper(__uuidof(ProcessClientServicesLib::RpcCallHelper));
		_bstr_t bstrUser(sUser);
		_bstr_t bstrPassword(sPassword);
		CString sUpResult = (LPCTSTR)_bstr_t(piClientHelper->ClientLogin(bstrUser, bstrPassword));
		sOut = sUpResult;
	}
	catch (_com_error e)
	{
		CString strErrorMsg = e.ErrorMessage();
		ZTools::WriteZToolsFormatLog("RPC组件调用异常,%s", strErrorMsg);
		return "";
	}
	catch (COleException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("PRC组件调用异常,%s", szCause);
		return "";
	}
	catch (CException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("RPC组件调用异常,%s", szCause);
		return "";
	}
	if (SUCCEEDED(hr))
	{
		::CoUninitialize();
	}

	//return "{\"error\":\"0\",\"userName\":\"admin\",\"password\":\"111\",\"ticket\":\"ticket-proxy=ZzxSJrzPhvKJCElqycem\",\"realName\":\"超管\",\"roles\":\"asdf\"}";
	return sOut;
}

CString Logout( CString sTicket )
{
	HRESULT hr = ::CoInitialize(NULL);
	CString sOut(""); 
	try
	{
		ProcessClientServicesLib::IRpcCallHelperPtr piClientHelper(__uuidof(ProcessClientServicesLib::RpcCallHelper));
		_bstr_t bstrTicket(sTicket);
		CString sUpResult = (LPCTSTR)_bstr_t(piClientHelper->ERMLogout(bstrTicket));
		sOut = sUpResult;
	}
	catch (_com_error e)
	{
		CString strErrorMsg = e.ErrorMessage();
		ZTools::WriteZToolsFormatLog("RPC组件调用异常,%s", strErrorMsg);
		return "";
	}
	catch (COleException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("PRC组件调用异常,%s", szCause);
		return "";
	}
	catch (CException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("RPC组件调用异常,%s", szCause);
		return "";
	}
	if (SUCCEEDED(hr))
	{
		::CoUninitialize();
	}

	return sOut;
}

CString KeepTicket( CString sTicket )
{
	HRESULT hr = ::CoInitialize(NULL);
	CString sOut(""); 
	try
	{
		ProcessClientServicesLib::IRpcCallHelperPtr piClientHelper(__uuidof(ProcessClientServicesLib::RpcCallHelper));
		_bstr_t bstrTicket(sTicket);
		CString sUpResult = (LPCTSTR)_bstr_t(piClientHelper->ERMKeepTicket(bstrTicket));
		sOut = sUpResult;
	}
	catch (_com_error e)
	{
		CString strErrorMsg = e.ErrorMessage();
		ZTools::WriteZToolsFormatLog("RPC组件调用异常,%s", strErrorMsg);
		return "";
	}
	catch (COleException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("PRC组件调用异常,%s", szCause);
		return "";
	}
	catch (CException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("RPC组件调用异常,%s", szCause);
		return "";
	}
	if (SUCCEEDED(hr))
	{
		::CoUninitialize();
	}

	return sOut;
}