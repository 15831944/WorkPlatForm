// NetDiskCom.cpp : CNetDiskCom 的实现

#include "stdafx.h"
#include "NetDiskCom.h"
#include "Tools.h"
#include <comdef.h>
#include <tlhelp32.h>
#include "../Common/FilePro.h"
//JSON 头文件
#include "Json/value.h" 
#include "Json/reader.h"
#include "Json/writer.h"

// CNetDiskCom

static std::string GetDefaultErrMsgJson(const char * pszErrMsg)
{
	Json::Value objOut;
	objOut["flag"]=  Json::Value("false");
	objOut["msg"] =  Json::Value(pszErrMsg);
	return  objOut.toStyledString().c_str();
}
static BOOL FindProcessByName(LPCTSTR szFileName)
{
	// 采用进程快照枚举进程的方法查找指定名称进程
	HANDLE hProcesses;
	PROCESSENTRY32 lpe =
	{
		sizeof(PROCESSENTRY32)
	};
	CString strFileName(szFileName);
	strFileName.MakeLower();

	// 创建进程快照
	hProcesses = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcesses == INVALID_HANDLE_VALUE)
		return FALSE;
	// 获取第一个进程实例
	BOOL isExist = ::Process32First(hProcesses, &lpe);
	BOOL isRunning = FALSE;
	CString strName;
	while (isExist)
	{
		strName = lpe.szExeFile;
		strName.MakeLower();
		if (-1 != strFileName.Find(strName))
		{
			isRunning = TRUE;
			break;
		}
		// 遍历下一个进程实例
		isExist = ::Process32Next(hProcesses, &lpe);
	}

	CloseHandle(hProcesses);
	return isRunning;
}

static void StartDaemon()
{
	if (!FindProcessByName("PeraDCNetDiskDaemon.exe"))
	{
		SHELLEXECUTEINFO si;
		memset(&si, NULL, sizeof(si));
		si.cbSize = sizeof(si);
		si.nShow = SW_HIDE;
		si.fMask = SEE_MASK_NOCLOSEPROCESS;
		si.lpVerb = _T("open");
		std::string strDaemonPath = ZTools::FormatString("%sPeraDCNetDiskDaemon.exe", ZTools::GetCurrentModulePath("PeraNetDiskCom.dll").c_str());		
		si.lpFile = strDaemonPath.c_str();
		si.lpParameters = NULL;
		ShellExecuteEx(&si);
		Sleep(100);
		CloseHandle(si.hProcess);
	}
}

HRESULT CNetDiskCom::SelectFile(string strInJson, string & strOutJson)
{
	CString strFilter = "所有文件(*.*)|*.*||";

	bool bMutilSelect = false;
	Json::Reader reader;
	Json::Value obj( Json::objectValue );
	CString str;
	try
	{
		if (!reader.parse(strInJson, obj, true))
		{
			strOutJson =  GetDefaultErrMsgJson("解析Json出错!");
			return S_FALSE;
		}
		if ( obj.isNull() || !obj.isObject() )
		{
			strOutJson =  GetDefaultErrMsgJson("Json对象不正确!");
			return S_FALSE;
		}

		if (obj["filter"].isString())
			strFilter = obj["filter"].asCString();

		if (!obj["multiSelect"].empty())
		{
			if (obj["multiSelect"].asString() == "true")
				bMutilSelect = true;
			else if (obj["multiSelect"].asString() == "false")
				bMutilSelect = false;
		}
	}
	catch (...)
	{
		strOutJson =  GetDefaultErrMsgJson("解析Json发生异常!");
		return S_FALSE;
	}

	int iLen = strFilter.GetLength();
	if (iLen >2)
	{
		if (strFilter[iLen-1] !='|' && strFilter[iLen-2] !='|')
			strFilter +="||";
	}

	CFilePro filePro;
	vector<CString> vSelectFiles = filePro.GetSelectedFile(bMutilSelect,NULL,strFilter);

	Json::Value objOut;
	Json::Value jsonPathItems(Json::arrayValue);
	for (size_t i=0;i<vSelectFiles.size();i++)
		jsonPathItems.append(LPCTSTR(vSelectFiles[i]));

	objOut["path"] = jsonPathItems;
	objOut["flag"] = Json::Value(vSelectFiles.empty()? "false":"true");
	objOut["msg"] = Json::Value(LPCTSTR(filePro.GetErrMsg()));

	strOutJson = objOut.toStyledString().c_str();
	return S_OK;
}
HRESULT CNetDiskCom::SelectDir (string strInJson, string & strOutJson)
{
	CFilePro filePro;
	CString strDir = filePro.GetSelectedFolder();

	Json::Value objOut;
	objOut["path"]=  Json::Value(LPCTSTR(strDir));
	objOut["flag"]=  Json::Value(strDir.IsEmpty()? "false":"true");
	objOut["msg"]=  Json::Value(LPCTSTR(filePro.GetErrMsg()));
	if (filePro.GetErrMsg()== "非法路径")
			objOut["errCode"]=  Json::Value("2");
	else if (filePro.GetErrMsg()== "用户取消")
			objOut["errCode"] =  Json::Value("1");
	else
		objOut["errCode"]= Json::Value("0");

	strOutJson = objOut.toStyledString().c_str();
	return S_OK;
}
STDMETHODIMP CNetDiskCom::InvokeMethod(BSTR bstrMethodName, BSTR bstrJsonIn, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	std::string strMethodName = (bstrMethodName != NULL) ? _bstr_t(bstrMethodName) : "";
	std::string strJsonIn = (bstrJsonIn != NULL) ? _bstr_t(bstrJsonIn) : "";
	ZTools::WriteZToolsFormatLog("------CNetDiskCom::InvokeMethod------\nstrMethodName: %s\nstrJsonIn: %s", strMethodName.c_str(), strJsonIn.c_str());

	std::string strRet;
	CComVariant vRet(strRet.c_str());

	if (strMethodName == "SelectFile")
	{
		SelectFile(strJsonIn,strRet);
		vRet = strRet.c_str();
		vRet.Detach(pvRet);
		ZTools::WriteZToolsFormatLog("retrun:\n%s", strRet.c_str());
		return S_OK;
	}
	else if (strMethodName == "SelectDir")
	{
		SelectDir(strJsonIn,strRet);
		vRet = strRet.c_str();
		vRet.Detach(pvRet);
		ZTools::WriteZToolsFormatLog("retrun:\n%s", strRet.c_str());
		return S_OK;
	}
	else if (strMethodName =="SelectItem")
	{
		SelectItem(strJsonIn,strRet);
		vRet = strRet.c_str();
		vRet.Detach(pvRet);
		ZTools::WriteZToolsFormatLog("retrun:\n%s", strRet.c_str());
		return S_OK;
	}
	
	strRet = m_peraIceNetDisk.InvokeMethod(strMethodName, strJsonIn);

	ZTools::WriteZToolsFormatLog("retrun:\n%s", strRet.c_str());

	vRet = strRet.c_str();
	vRet.Detach(pvRet);

	return S_OK;
}

CNetDiskCom::CNetDiskCom()
{
	ZTools::InitZToolsLog("PeraNetDiskCom", "PeraNetDiskCom.dll");

	StartDaemon();
	
	Ice::InitializationData initData;
	initData.properties = Ice::createProperties();
	initData.properties->setProperty("Ice.MessageSizeMax", "1024");//默认是1024，单位KB

	m_pCommunicator = Ice::initialize(initData);

	m_peraIceNetDisk.SetCommunicator(m_pCommunicator);
}

void CNetDiskCom::FinalRelease()
{
	if (m_pCommunicator)
	{
		m_pCommunicator->destroy();
		m_pCommunicator = NULL;
	}
}
HRESULT CNetDiskCom::SelectItem(string strInJson, string & strOutJson)
{
	CString strFilter = "所有文件(*.*)|*.*||";
	CString strMode   = "fileAndFolder";
	Json::Reader reader;
	Json::Value obj( Json::objectValue );
	CString str;
	try
	{
		if (!reader.parse(strInJson, obj, true))
		{
			strOutJson =  GetDefaultErrMsgJson("解析Json出错!");
			return S_FALSE;
		}
		if ( obj.isNull() || !obj.isObject() )
		{
			strOutJson =  GetDefaultErrMsgJson("Json对象不正确!");
			return S_FALSE;
		}

		if (obj["filter"].isString()) strFilter = obj["filter"].asCString();
		if (obj["mode"].isString()) strMode = obj["mode"].asCString();
	}
	catch (...)
	{
		strOutJson =  GetDefaultErrMsgJson("解析Json发生异常!");
		return S_FALSE;
	}

	int iLen = strFilter.GetLength();
	if (iLen >2)
	{
		if (strFilter[iLen-1] !='|' && strFilter[iLen-2] !='|')
			strFilter +="||";
	}

	CFilePro filePro;
	vector<_SelectItem> vSelectFiles = filePro.GetSelectItems(strMode,strFilter);

	Json::Value objOut;
	Json::Value jsonPathItems(Json::arrayValue);
	for (size_t i=0;i<vSelectFiles.size();i++)
	{
		Json::Value jPath(Json::objectValue);
		jPath["isFile"] = (vSelectFiles[i].m_bIsFile ? "true":"false");
		jPath["path"]   = Json::Value(LPCTSTR(vSelectFiles[i].m_strPath));
		jsonPathItems.append(jPath);
	}

	objOut["paths"] = jsonPathItems;
	objOut["flag"] = Json::Value(vSelectFiles.empty()? "false":"true");
	objOut["msg"] = Json::Value(LPCTSTR(filePro.GetErrMsg()));

	strOutJson = objOut.toStyledString().c_str();
	return S_OK;
}