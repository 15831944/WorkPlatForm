#include "StdAfx.h"
#include "CallFileUpDown.h"
#include "Tools.h"
//#import "PeraFileUpDown.tlb"
#include "PeraFileUpDown.tlh"

std::string CallUploadHprose( std::string strHproseRpc, std::string strFilePath, std::string strMd5, BOOL bMove )
{
	HRESULT hr = ::CoInitialize(NULL);
	std::string strOut(""); 
	try
	{
		PeraFileUpDown::IFileUpDownPtr piFileUpDown(__uuidof(PeraFileUpDown::FileUpDown));
		_bstr_t bstrHproseRpc(strHproseRpc.c_str());
		_bstr_t bstrFilePath(strFilePath.c_str());
		_bstr_t bstrFileMd5(strMd5.c_str());
		_bstr_t bstrBoolMove = bMove ? "true" : "false";
		_bstr_t bstrOut = piFileUpDown->Upload3(bstrHproseRpc, bstrFilePath, bstrFileMd5, bstrBoolMove);
		strOut = bstrOut;
	}
	catch (_com_error e)
	{
		CString strErrorMsg = e.ErrorMessage();
		ZTools::WriteZToolsFormatLog("文件上传组件调用异常,%s", strErrorMsg);
		return "";
	}
	catch (COleException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("文件上传组件调用异常,%s", szCause);
		return "";
	}
	catch (CException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("文件上传组件调用异常,%s", szCause);
		return "";
	}
	if (SUCCEEDED(hr))
	{
		::CoUninitialize();
	}

	return strOut;
}

std::string CallUploadFilesHprose( std::string strHproseRpc, std::string strJson, BOOL bMove )
{
	HRESULT hr = ::CoInitialize(NULL);
	std::string strOut(""); 
	try
	{
		PeraFileUpDown::IFileUpDownPtr piFileUpDown(__uuidof(PeraFileUpDown::FileUpDown));
		_bstr_t bstrHproseRpc(strHproseRpc.c_str());
		_bstr_t bstrJson(strJson.c_str());
		_bstr_t bstrBoolMove = bMove ? "true" : "false";
		_bstr_t bstrOut = piFileUpDown->UploadFiles(bstrHproseRpc, bstrJson, bstrBoolMove);
		strOut = bstrOut;
	}
	catch (_com_error e)
	{
		CString strErrorMsg = e.ErrorMessage();
		ZTools::WriteZToolsFormatLog("文件上传组件调用异常,%s", strErrorMsg);
		return "";
	}
	catch (COleException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("文件上传组件调用异常,%s", szCause);
		return "";
	}
	catch (CException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("文件上传组件调用异常,%s", szCause);
		return "";
	}
	if (SUCCEEDED(hr))
	{
		::CoUninitialize();
	}

	return strOut;
}

std::string CallDownloadHprose( std::string strHproseBaseUrl, std::string strFileId, std::string strFileName )
{
	HRESULT hr = ::CoInitialize(NULL);
	std::string strOut(""); 
	try
	{
		PeraFileUpDown::IFileUpDownPtr piFileUpDown(__uuidof(PeraFileUpDown::FileUpDown));
		_bstr_t bstrHproseBaseUrl(strHproseBaseUrl.c_str());
		_bstr_t bstrFileId(strFileId.c_str());
		_bstr_t bstrFileName(strFileName.c_str());
		_bstr_t bstrOut = piFileUpDown->Download2(bstrHproseBaseUrl, bstrFileId, bstrFileName);
		strOut = bstrOut;
	}
	catch (_com_error e)
	{
		CString strErrorMsg = e.ErrorMessage();
		ZTools::WriteZToolsFormatLog("文件下载组件调用异常,%s", strErrorMsg);
		return "";
	}
	catch (COleException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("文件下载组件调用异常,%s", szCause);
		return "";
	}
	catch (CException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("文件下载组件调用异常,%s", szCause);
		return "";
	}
	if (SUCCEEDED(hr))
	{
		::CoUninitialize();
	}

	return strOut;
}

std::string CallDownloadFilesHprose( std::string strHproseBaseUrl, std::string strJson )
{
	HRESULT hr = ::CoInitialize(NULL);
	std::string strOut(""); 
	try
	{
		PeraFileUpDown::IFileUpDownPtr piFileUpDown(__uuidof(PeraFileUpDown::FileUpDown));
		_bstr_t bstrHproseBaseUrl(strHproseBaseUrl.c_str());
		_bstr_t bstrJson(strJson.c_str());
		_bstr_t bstrOut = piFileUpDown->DownloadFiles(bstrHproseBaseUrl, bstrJson);
		strOut = bstrOut;
	}
	catch (_com_error e)
	{
		CString strErrorMsg = e.ErrorMessage();
		ZTools::WriteZToolsFormatLog("文件下载组件调用异常,%s", strErrorMsg);
		return "";
	}
	catch (COleException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("文件下载组件调用异常,%s", szCause);
		return "";
	}
	catch (CException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("文件下载组件调用异常,%s", szCause);
		return "";
	}
	if (SUCCEEDED(hr))
	{
		::CoUninitialize();
	}

	return strOut;
}