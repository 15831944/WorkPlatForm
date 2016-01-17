#include "StdAfx.h"
#include "InvokeIceFunc.h"
#include "Tools.h"
#import "ProcessClientServices.tlb"

/*
startLocalModelByByteXpdl
pkgid
xpdl����
ip
�û���
��ʵ��
SubFlow��Process//������ʱ�����������У��������ֵ
*/
CString InvokeWsMethod( CString sMethod, CString sJson )
{
	HRESULT hr = ::CoInitialize(NULL);
	CString sOut(""); 
	try
	{
		ProcessClientServicesLib::IRpcCallHelperPtr piClientHelper(__uuidof(ProcessClientServicesLib::RpcCallHelper));
		_bstr_t bstrMethod(sMethod);
		_bstr_t bstrJson(sJson);
		CString sUpResult = (LPCTSTR)_bstr_t(piClientHelper->InvokeWsMethod(bstrMethod, bstrJson));
		sOut = sUpResult;
	}
	catch (_com_error e)
	{
		CString strErrorMsg = e.ErrorMessage();
		ZTools::WriteZToolsFormatLog("ICE��������쳣,%s", strErrorMsg);
		return "";
	}
	catch (COleException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("ICE��������쳣,%s", szCause);
		return "";
	}
	catch (CException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("ICE��������쳣,%s", szCause);
		return "";
	}
	if (SUCCEEDED(hr))
	{
		::CoUninitialize();
	}

	return sOut;
}


CString InvokeWsMethodOrignal( CString sMethod, CString sJson )
{
	HRESULT hr = ::CoInitialize(NULL);
	CString sOut(""); 
	try
	{
		ProcessClientServicesLib::IRpcCallHelperPtr piClientHelper(__uuidof(ProcessClientServicesLib::RpcCallHelper));
		_bstr_t bstrMethod(sMethod);
		_bstr_t bstrJson(sJson);
		CString sUpResult = (LPCTSTR)_bstr_t(piClientHelper->InvokeWsMethodOrignal(bstrMethod, bstrJson));
		sOut = sUpResult;
	}
	catch (_com_error e)
	{
		CString strErrorMsg = e.ErrorMessage();
		ZTools::WriteZToolsFormatLog("ICE��������쳣,%s", strErrorMsg);
		return "";
	}
	catch (COleException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("ICE��������쳣,%s", szCause);
		return "";
	}
	catch (CException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("ICE��������쳣,%s", szCause);
		return "";
	}
	if (SUCCEEDED(hr))
	{
		::CoUninitialize();
	}

	return sOut;
}

CString InvokeLocalWsMethod( CString sMethod, CString sJson )
{
	HRESULT hr = ::CoInitialize(NULL);
	CString sOut(""); 
	try
	{
		ProcessClientServicesLib::IRpcCallHelperPtr piClientHelper(__uuidof(ProcessClientServicesLib::RpcCallHelper));
		_bstr_t bstrMethod(sMethod);
		_bstr_t bstrJson(sJson);
		CString sUpResult = (LPCTSTR)_bstr_t(piClientHelper->InvokeLocalWsMethod(bstrMethod, bstrJson));
		sOut = sUpResult;
	}
	catch (_com_error e)
	{
		CString strErrorMsg = e.ErrorMessage();
		ZTools::WriteZToolsFormatLog("ICE��������쳣,%s", strErrorMsg);
		return "";
	}
	catch (COleException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("ICE��������쳣,%s", szCause);
		return "";
	}
	catch (CException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("ICE��������쳣,%s", szCause);
		return "";
	}
	if (SUCCEEDED(hr))
	{
		::CoUninitialize();
	}

	return sOut;
}

CString InvokeLocalWsMethodOrignal( CString sMethod, CString sJson )
{
	HRESULT hr = ::CoInitialize(NULL);
	CString sOut(""); 
	try
	{
		ProcessClientServicesLib::IRpcCallHelperPtr piClientHelper(__uuidof(ProcessClientServicesLib::RpcCallHelper));
		_bstr_t bstrMethod(sMethod);
		_bstr_t bstrJson(sJson);
		CString sUpResult = (LPCTSTR)_bstr_t(piClientHelper->InvokeLocalWsMethodOrignal(bstrMethod, bstrJson));
		sOut = sUpResult;
	}
	catch (_com_error e)
	{
		CString strErrorMsg = e.ErrorMessage();
		ZTools::WriteZToolsFormatLog("ICE��������쳣,%s", strErrorMsg);
		return "";
	}
	catch (COleException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("ICE��������쳣,%s", szCause);
		return "";
	}
	catch (CException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("ICE��������쳣,%s", szCause);
		return "";
	}
	if (SUCCEEDED(hr))
	{
		::CoUninitialize();
	}

	return sOut;
}