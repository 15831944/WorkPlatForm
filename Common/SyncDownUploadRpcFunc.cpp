#include "StdAfx.h"
#include "SyncDownUploadFunc.h"
#include "Tools.h"
//#import "PeraFileUpDown.tlb"
#include "perafileupdown.tlh"
#include "DownUploadFileCacheFinder.h"
#include "ConfigFunc.h"
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")

static CString Upload( CString sHproseRpc, CString sFilePath, CString sMd5 = "" )
{	
	//���ʹ�ô������̰߳汾��COM�������SVG��JSʧȥ��Ӧ������
	HRESULT hr = ::CoInitialize(NULL);
	CString sOut(""); 
	try
	{
		PeraFileUpDown::IFileUpDownPtr piFileUpDown(__uuidof(PeraFileUpDown::FileUpDown));
		//PeraFileUpDown::IFileUpDown *pFileUpDown = piFileUpDown;
		_bstr_t bstrHproseRpc(sHproseRpc);
		_bstr_t bstrFilePath(sFilePath);
		_bstr_t bstrFileMd5(sMd5);
		_bstr_t bstrBoolMove("false");
		CString sUpResult = (LPCTSTR)_bstr_t(piFileUpDown->Upload3(bstrHproseRpc, bstrFilePath, bstrFileMd5, bstrBoolMove));
		sOut = sUpResult;
	}
	catch (_com_error e)
	{
		CString strErrorMsg = e.ErrorMessage();
		ZTools::WriteZToolsFormatLog("�ļ��ϴ���������쳣,%s", strErrorMsg);
		return "";
	}
	catch (COleException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("�ļ��ϴ���������쳣,%s", szCause);
		return "";
	}
	catch (CException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("�ļ��ϴ���������쳣,%s", szCause);
		return "";
	}
	if (SUCCEEDED(hr))
	{
		::CoUninitialize();
	}

	return sOut;
}
CString UploadHprose( CString sFilePath )
{
	std::string strFilePath((LPCTSTR)sFilePath);

	CDownUploadDataFileObject dataFileObject;
	dataFileObject.m_strFilePath = strFilePath;
	CDownUploadFileCacheFinder::ReadUploadCache(&dataFileObject);

	CString sReturn;

	if(dataFileObject.m_strFileId.empty())
	{
		sReturn = Upload(dataFileObject.m_strHproseRpc.c_str(), dataFileObject.m_strFilePath.c_str(), dataFileObject.m_strMd5.c_str());
	}
	else
	{
		sReturn = dataFileObject.m_strFileId.c_str();
	}

	if (sReturn.IsEmpty())
	{
		ZTools::WriteZToolsFormatLog("�ϴ��ļ�ʧ�ܣ�%s", sFilePath);
	}

	return sReturn;
}

static CString Download( CString sHproseRpc, CString sFileId, CString sFileName )
{
	//���ʹ�ô������̰߳汾��COM�������SVG��JSʧȥ��Ӧ������
	HRESULT hr = ::CoInitialize(NULL);
	CString sOut(""); 
	try
	{
		PeraFileUpDown::IFileUpDownPtr piFileUpDown(__uuidof(PeraFileUpDown::FileUpDown));
		//PeraFileUpDown::IFileUpDown *pFileUpDown = piFileUpDown;
		_bstr_t bstrHproseRpc(sHproseRpc);
		_bstr_t bstrFileId(sFileId);
		_bstr_t bstrFileName(sFileName);
		CString sUpResult = (LPCTSTR)_bstr_t(piFileUpDown->Download2(bstrHproseRpc, bstrFileId, bstrFileName));
		sOut = sUpResult;
	}
	catch (_com_error e)
	{
		CString strErrorMsg = e.ErrorMessage();
		ZTools::WriteZToolsFormatLog("�ļ�������������쳣,%s", strErrorMsg);
		return "";
	}
	catch (COleException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("�ļ�������������쳣,%s", szCause);
		return "";
	}
	catch (CException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("�ļ�������������쳣,%s", szCause);
		return "";
	}
	if (SUCCEEDED(hr))
	{
		::CoUninitialize();
	}

	return sOut;
}

//����
CString DownloadHprose( CString sFileId, CString sFileDir, CString sFileName )
{
	if (sFileId.IsEmpty())
	{
		return "";
	}

	CString sHproseRpc = GetConfig("hprose", "hproseRpc");
	if(sHproseRpc.IsEmpty())
	{
		ZTools::WriteZToolsLog("δ�ҵ�hproseRpc");
		return "";
	}

	CDownUploadDataFileObject dataFileObject;
	dataFileObject.m_strHproseRpc = (LPCTSTR)sHproseRpc;
	dataFileObject.m_strFileId = (LPCTSTR)sFileId;
	
	std::string strRefFilePath;
	if (CDownUploadFileCacheFinder::ReadDownloadCache(&dataFileObject))
	{
		strRefFilePath = dataFileObject.m_strFilePath;
	}
	else
	{
		//δ�ҵ�����͵�com������أ����ܽ��ͣ�
		strRefFilePath = (LPCTSTR)Download(sHproseRpc, sFileId, sFileName);
	}
	
	if (strRefFilePath.empty())
	{
		ZTools::WriteZToolsFormatLog("�����ļ�ʧ�ܣ�%s, %s", sFileId, sFileName);
		return "";
	}

	if (sFileDir.IsEmpty())
	{
		return strRefFilePath.c_str();
	}
	else
	{
		std::string strFileDir = (LPCTSTR)sFileDir;
		std::string strFileName = (LPCTSTR)sFileName;
		std::string strDstFilePath;
		if (strFileName.empty())
		{
			strFileName = strRefFilePath.substr(strRefFilePath.find_last_of('\\') + 1, strRefFilePath.length() - strRefFilePath.find_last_of('\\') - 1);
		}

		std::replace(strFileDir.begin(), strFileDir.end(), '/', '\\');
		strFileDir.erase(strFileDir.find_last_not_of('\\') + 1);
		strDstFilePath = ZTools::FormatString("%s\\%s", strFileDir.c_str(), strFileName.c_str());

		MakeSureDirectoryPathExists(strDstFilePath.c_str());
		CopyFile(strRefFilePath.c_str(), strDstFilePath.c_str(), FALSE);
		if (PathFileExists(strDstFilePath.c_str()))
		{
			return strDstFilePath.c_str();
		}
		else
		{
			return "";
		}
	}	
}