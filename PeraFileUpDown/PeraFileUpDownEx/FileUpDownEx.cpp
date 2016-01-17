// FileUpDownEx.cpp : CFileUpDownEx 的实现

#include "stdafx.h"
#include "FileUpDownEx.h"
#include "Tools.h"
#include "DownUploadFileCacheFinder.h"
#include "ZipTool.h"
#include "DownUploadCommonFunc.h"
#include "CallHprose.h"
#include <comdef.h>
#include "FileIdXpdl.h"
#include "InvokeWsFunc.h"


// CFileUpDownEx



STDMETHODIMP CFileUpDownEx::StartUpload(BSTR bstrJson, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ZTools::WriteZToolsLog(">>>>>>PeraFileUpDownEx::CFileUpDownEx::StartUpload");

	CComVariant vReturn(_bstr_t("").GetBSTR());

	CPfudeTheadData* pTheadData = NULL;
	pTheadData = new CPfudeTheadData();
	g_mapUploads.insert(std::pair<std::string, CPfudeTheadData*>(pTheadData->m_strIndex, pTheadData));

	CDataStartIn dataUploadIn;
	if (dataUploadIn.Init(bstrJson) == FALSE)
	{
		pTheadData->m_strFlag = "error";
		pTheadData->m_strMsg = "com接口入参不合法";
		pTheadData->DetachToStartVariant(pvRet);
		return S_OK;
	}
	
	/*
	选文件*
	预判断格式是否正确，看是否有manifest或main.xpdl，如有，记下标记为组件或设计过程*
	起线程
	线程中算MD5，保存变量
	查专用缓存，如有，返回
	创建线程池，处理上传列表
	将WS文件记入上传列表
	解压所有文件，同时记入上传列表
	上传列表全部完成后，清临时文件，拼最终JSON，记专用缓存
	返回JSON
	*/
 	//打开选择对话框
//  	CFileDialog dlgFile(TRUE
//  		, NULL
//  		, NULL
//  		, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT
//  		, "设计过程文件、组件文件 (*.ws、*.bot)|*.ws;*.bot||"
//  		, NULL
//  		);
//  	int nRet = dlgFile.DoModal();
// 	if (nRet == IDCANCEL)
// 	{
// 		pTheadData->m_strFlag = "error";
// 		pTheadData->m_strMsg = "未选文件";
// 		pTheadData->DetachToStartVariant(pvRet);
// 		return S_OK;
// 	}

	//std::string strFilePath = dlgFile.GetPathName();
	std::string strFilePath = dataUploadIn.m_strFilePath;
	pTheadData->m_strFilePath = strFilePath;
	pTheadData->m_bIsUploadThisFile = dataUploadIn.m_bIsUploadThisFile;

	std::replace(strFilePath.begin(), strFilePath.end(), '/', '\\');
	pTheadData->m_strFileName = strFilePath.substr(strFilePath.find_last_of('\\') + 1, strFilePath.length() - strFilePath.find_last_of('\\') - 1);	

	if (!PathFileExists(pTheadData->m_strFilePath.c_str()))
	{
		pTheadData->m_strFlag = "error";
		pTheadData->m_strMsg = "文件不存在";
		pTheadData->DetachToStartVariant(pvRet);
		return S_OK;
	}

	if (ZipTool::FindFileInZip(pTheadData->m_strFilePath, "main.xpdl") == true)
	{
		pTheadData->m_strFileType = "Process";
	}
	else if (ZipTool::FindFileInZip(pTheadData->m_strFilePath, "Manifest.xml") == true)
	{
		pTheadData->m_strFileType = "Component";
	}
	else if (ZipTool::FindFileInZip(pTheadData->m_strFilePath, "main.xml") == true)
	{
		pTheadData->m_strFileType = "ComponentWithForm";
	}
	else
	{
		pTheadData->m_strFileType = "Unknown";
		pTheadData->m_strFlag = "error";
		pTheadData->m_strMsg = "文件不是有效设计过程文件或组件文件";
		pTheadData->DetachToStartVariant(pvRet);
		return S_OK;
	}
	
	DWORD dwThreadId = 0;
	HANDLE hThread = ::CreateThread(
		NULL
		, 0
		, StartUploadThread
		, (LPVOID)pTheadData
		, CREATE_SUSPENDED
		, &dwThreadId);

	if (hThread == NULL || hThread == 0)
	{
		pTheadData->m_strFlag = "error";
		pTheadData->m_strMsg = "创建上传线程失败";
		pTheadData->DetachToStartVariant(pvRet);
		return S_OK;
	}
	ResumeThread(hThread);
	::CloseHandle(hThread);

	pTheadData->m_strFlag = "uploading";
	pTheadData->DetachToStartVariant(pvRet);
	return S_OK;
}

/*
起线程
线程中算MD5，保存变量
查专用缓存，如有，返回
创建线程池，处理上传列表
将WS文件记入上传列表
解压所有文件，同时记入上传列表
上传列表全部完成后，清临时文件，拼最终JSON，记专用缓存
返回JSON
*/
DWORD WINAPI CFileUpDownEx::StartUploadThread( LPVOID lpParameter )
{
	CPfudeTheadData* pTheadData = (CPfudeTheadData*)lpParameter;
	
	pTheadData->m_thisFileObject.m_strHproseRpc = pTheadData->m_strHproseRpc;
	pTheadData->m_thisFileObject.m_strFilePath = pTheadData->m_strFilePath;
	if (pTheadData->m_thisFileObject.CheckInfo() == FALSE)
	{
		pTheadData->m_strFlag = "error";
		pTheadData->m_strMsg = pTheadData->m_thisFileObject.m_strMsg;
		return 0;
	}

	//pTheadData->m_strFileMd5 = pTheadData->m_thisFileObject.m_strMd5;

	CSpecialZipCache specialZipCache;
	specialZipCache.m_strThisFileMd5 = pTheadData->m_thisFileObject.m_strMd5;
	specialZipCache.m_strThisFileSize = ZTools::FormatString("%ld", pTheadData->m_thisFileObject.m_nFileSize);
	specialZipCache.ReadCache();

	pTheadData->m_strFileIdListJson = specialZipCache.m_strFileIdListJson;
	pTheadData->m_strMainFileId = specialZipCache.m_strMainFileId;

	pTheadData->m_componentFileObject.m_strFileName = specialZipCache.m_strComponentFileName;
	pTheadData->m_componentFileObject.m_strFileId = specialZipCache.m_strComponentFileId;
	pTheadData->m_componentManifestObject.m_strFileId = specialZipCache.m_strComponentManifest;
	pTheadData->m_formFileObject.m_strFileName = specialZipCache.m_strFormFileName;
	pTheadData->m_formFileObject.m_strFileId = specialZipCache.m_strFormFileId;
	pTheadData->m_formManifestObject.m_strFileId = specialZipCache.m_strFormManifest;

	if (
		!pTheadData->m_strFileIdListJson.empty()
		//&& !pTheadData->m_strMainFileId.empty()
		)
	{	
		if (pTheadData->m_bIsUploadThisFile)
		{
			CallHprose ch;
			pTheadData->m_strThisFileId = ch.UploadUseCache(&pTheadData->m_thisFileObject);
			if (pTheadData->m_strThisFileId.empty())
			{
				pTheadData->m_strFlag = "error";
				pTheadData->m_strMsg = "文件上传失败";
				return 0;
			}
		}
		
		pTheadData->m_strFlag = "finish";
		pTheadData->m_strMsg = "上传完成，上传缓存中存在，直接使用缓存";
		return 0;
	}

	if (!PrepareUplaodInfo(lpParameter))
	{
		pTheadData->m_strFlag = "error";
		pTheadData->m_strMsg = "解压WS文件失败";
		return 0;
	}

	UploadAllFiles(lpParameter);

	ClearTempDir(lpParameter);
	
	ZTools::WriteZToolsFormatLog("任务完成");

	return 0;
}

static std::string GetStringAttributeFromXmlElement(TiXmlElement* xmlElement, std::string strKey)
{
	std::string strValue;
#ifdef TIXML_USE_STL
	xmlElement->QueryStringAttribute(strKey.c_str(), &strValue);
#else
	const char* pStrValue = xmlElement->Attribute(strKey.c_str());
	if (pStrValue != NULL)
	{
		strValue = std::string(pStrValue);
	}
#endif
	return strValue;
}

bool CFileUpDownEx::PrepareUplaodInfo(LPVOID lpParameter)
{
	CPfudeTheadData* pTheadData = (CPfudeTheadData*)lpParameter;
		
	ZTools::WriteZToolsFormatLog("解压前");

	//如果是BOTW，需要准备一些额外信息，文件需先解压一次，之后做二次解压
	if (pTheadData->IsComponentWithForm())
	{
		std::string strComponentPath, strFormPath;
		std::string strMainXml = ZipTool::GetContentFromZip(pTheadData->m_strFilePath, "main.xml");
		{
			TiXmlBase::SetCondenseWhiteSpace(false);
			TiXmlDocument doc;
			doc.Parse(strMainXml.c_str());

			if (doc.Error())
			{
				ZTools::WriteZToolsFormatLog("解析mainxml出错");
				return false;
			}
			TiXmlElement* pRootEle = doc.RootElement();//mainxml
			if (pRootEle)
			{
				//取组件信息
				TiXmlElement* pComponentEle = pRootEle->FirstChildElement("component");
				if (pComponentEle)
				{
					strComponentPath = GetStringAttributeFromXmlElement(pComponentEle, "Path");
					ZTools::UTF8ToMB(strComponentPath);
				}

				//取表单信息
				TiXmlElement* pFormEle = pRootEle->FirstChildElement("form");
				if (pFormEle)
				{
					strFormPath = GetStringAttributeFromXmlElement(pFormEle, "Path");
					ZTools::UTF8ToMB(strFormPath);
				}
			}
		}
		if (strComponentPath == "" || strFormPath == "")
		{
			return false;
		}

		pTheadData->m_componentFileObject.m_strFilePath = ZTools::FormatString("%sUnzipBOTW\\component\\%s", pTheadData->m_strUnzipDir.c_str(), strComponentPath.c_str());
		if (!ZipTool::UnzipFile(pTheadData->m_strFilePath, strComponentPath, pTheadData->m_componentFileObject.m_strFilePath))
		{
			ZTools::WriteZToolsFormatLog("解压BOTW中组件文件失败");
			return false;
		}
		if (!pTheadData->m_componentFileObject.CheckInfo())
		{
			ZTools::WriteZToolsFormatLog("检查BOTW中组件文件信息失败");
			return false;
		}

		pTheadData->m_componentManifestObject.m_strFilePath = ZTools::FormatString("%sUnzipBOTW\\component\\Manifest.xml", pTheadData->m_strUnzipDir.c_str());
		if (!ZipTool::UnzipFile(pTheadData->m_componentFileObject.m_strFilePath, "Manifest.xml", pTheadData->m_componentManifestObject.m_strFilePath))
		{
			ZTools::WriteZToolsFormatLog("解压BOTW中组件文件的描述文件失败");
			return false;
		}
		if (!pTheadData->m_componentManifestObject.CheckInfo())
		{
			ZTools::WriteZToolsFormatLog("检查BOTW中组件文件的描述文件信息失败");
			return false;
		}

		pTheadData->m_formFileObject.m_strFilePath = ZTools::FormatString("%sUnzipBOTW\\form\\%s", pTheadData->m_strUnzipDir.c_str(), strFormPath.c_str());
		if (!ZipTool::UnzipFile(pTheadData->m_strFilePath, strFormPath, pTheadData->m_formFileObject.m_strFilePath))
		{
			ZTools::WriteZToolsFormatLog("解压BOTW中表单文件失败");
			return false;
		}
		if (!pTheadData->m_formFileObject.CheckInfo())
		{
			ZTools::WriteZToolsFormatLog("检查BOTW中表单文件信息失败");
			return false;
		}

		pTheadData->m_formManifestObject.m_strFilePath = ZTools::FormatString("%sUnzipBOTW\\form\\Manifest.xml", pTheadData->m_strUnzipDir.c_str());
		if (!ZipTool::UnzipFile(pTheadData->m_formFileObject.m_strFilePath, "Manifest.xml", pTheadData->m_formManifestObject.m_strFilePath))
		{
			ZTools::WriteZToolsFormatLog("解压BOTW中表单文件的描述文件失败");
			return false;
		}
		if (!pTheadData->m_formManifestObject.CheckInfo())
		{
			ZTools::WriteZToolsFormatLog("检查BOTW中表单文件的描述文件信息失败");
			return false;
		}
	}

	if(MakeSureDirectoryPathExists(pTheadData->m_strUnzipDir.c_str()) == FALSE)
	{//目录创建失败
		return false;
	}

	char currentDir[255];
	GetCurrentDirectory(255, currentDir); //保存当前目录
	if(SetCurrentDirectory(pTheadData->m_strUnzipDir.c_str()) == FALSE)
	{//切换工作目录失败
		return false;
	}

	HZIP hz = NULL;
	if (pTheadData->IsComponentWithForm())
	{
		hz = OpenZip((void*)pTheadData->m_componentFileObject.m_strFilePath.c_str(), 0, ZIP_FILENAME);
	}
	else
	{
		hz = OpenZip((void*)pTheadData->m_strFilePath.c_str(), 0, ZIP_FILENAME);
	}
	if(hz == NULL)
	{//打开失败
		return false;
	}

	ZIPENTRY ze; 
	GetZipItem(hz, -1, &ze); 
	int numitems = ze.index; //元素数目
	
	for (int i=0; i<numitems; i++)
	{//遍历解压
		memset(ze.name,0,ZIP_MAX_PATH); // 不初始化，后面strlen取字符串长度就不正确了
		GetZipItem(hz, i, &ze);
		ZRESULT rs = UnzipItem(hz, i, ze.name, 0, ZIP_FILENAME);
		if (rs != ZR_OK)
			return false;

		std::string strKey(ze.name);

		if (strKey.back() != '/')
		{
			std::replace(strKey.begin(), strKey.end(), '/', '\\');
			CDataUpladJob uploadJob;
			uploadJob.fileObject.m_strHproseRpc = pTheadData->m_strHproseRpc;
			uploadJob.fileObject.m_strFilePath = ZTools::FormatString("%s%s", pTheadData->m_strUnzipDir.c_str(), strKey.c_str());
			uploadJob.fileObject.CheckInfo(FALSE);
			uploadJob.m_strJsonKey = "\\" + strKey;
			pTheadData->m_vJobs.push_back(uploadJob);
		}
	}

	CloseZip(hz);
	SetCurrentDirectory(currentDir);

	__int64 nAllSize = 0;
	if (pTheadData->m_bIsUploadThisFile)
	{
		nAllSize += pTheadData->m_thisFileObject.m_nFileSize;
	}
	if (pTheadData->IsComponentWithForm())
	{
		nAllSize += pTheadData->m_componentFileObject.m_nFileSize;
		nAllSize += pTheadData->m_componentManifestObject.m_nFileSize;
		nAllSize += pTheadData->m_formFileObject.m_nFileSize;
		nAllSize += pTheadData->m_formManifestObject.m_nFileSize;
	}
	for (std::vector<CDataUpladJob>::iterator it = pTheadData->m_vJobs.begin(); it != pTheadData->m_vJobs.end(); ++it)
	{
		nAllSize += it->fileObject.m_nFileSize;
	}
	pTheadData->m_n64AllSize = nAllSize;

	ZTools::WriteZToolsFormatLog("解压完成");

	return true;
}

bool CFileUpDownEx::UploadAllFiles(LPVOID lpParameter)
{
	CPfudeTheadData* pTheadData = (CPfudeTheadData*)lpParameter;

	if (pTheadData->m_bIsUploadThisFile)
	{
		CallHprose ch;
		pTheadData->m_strThisFileId = ch.UploadUseCache(&pTheadData->m_thisFileObject);
	}

	if (pTheadData->IsComponentWithForm())
	{
		{
			CallHprose ch;
			ch.UploadUseCache(&pTheadData->m_componentFileObject);
		}

		{
			CallHprose ch;
			ch.UploadUseCache(&pTheadData->m_componentManifestObject);
		}

		{
			CallHprose ch;
			ch.UploadUseCache(&pTheadData->m_formFileObject);
		}

		{
			CallHprose ch;
			ch.UploadUseCache(&pTheadData->m_formManifestObject);
		}
	}
	
	Json::Value root;

	for (std::vector<CDataUpladJob>::iterator it = pTheadData->m_vJobs.begin(); it != pTheadData->m_vJobs.end(); ++it)
	{
		CallHprose ch;
		ch.UploadUseCache(&it->fileObject, true);
		root[it->m_strJsonKey] = Json::Value(it->fileObject.m_strFileId);
		if (it->m_strJsonKey.compare("\\main.xpdl") == 0 || it->m_strJsonKey.compare("\\Manifest.xml") == 0)
		{
			pTheadData->m_strMainFileId = it->fileObject.m_strFileId;
		}
	}

	Json::FastWriter writer;
	pTheadData->m_strFileIdListJson = writer.write(root);
	pTheadData->m_strFlag = "finish";

	CSpecialZipCache specialZipCache;
	specialZipCache.m_strThisFileMd5 = pTheadData->m_thisFileObject.m_strMd5;
	specialZipCache.m_strThisFileSize = ZTools::FormatString("%ld", pTheadData->m_thisFileObject.m_nFileSize);
	specialZipCache.m_strMainFileId = pTheadData->m_strMainFileId;
	specialZipCache.m_strFileIdListJson = pTheadData->m_strFileIdListJson;

	specialZipCache.m_strComponentFileName = pTheadData->m_componentFileObject.m_strFileName;
	specialZipCache.m_strComponentFileId = pTheadData->m_componentFileObject.m_strFileId;
	specialZipCache.m_strComponentManifest = pTheadData->m_componentManifestObject.m_strFileId;
	specialZipCache.m_strFormFileName = pTheadData->m_formFileObject.m_strFileName;
	specialZipCache.m_strFormFileId = pTheadData->m_formFileObject.m_strFileId;
	specialZipCache.m_strFormManifest = pTheadData->m_formManifestObject.m_strFileId;

	if (pTheadData->IsComponentWithForm())
	{
		if (
			specialZipCache.m_strComponentFileName == ""
			|| specialZipCache.m_strComponentFileId == ""
			|| specialZipCache.m_strComponentManifest == ""
			|| specialZipCache.m_strFormFileName == ""
			|| specialZipCache.m_strFormFileId == ""
			|| specialZipCache.m_strFormManifest == ""			
			)
		{
			return true;
		}
	}

	specialZipCache.WriteCache();

	return true;
}

bool CFileUpDownEx::ClearTempDir(LPVOID lpParameter)
{
	CPfudeTheadData* pTheadData = (CPfudeTheadData*)lpParameter;

	std::string strDir = pTheadData->m_strUnzipDir;
	strDir += "*";
	int nDir = strDir.length() + 2;
	char* psDir = new char[nDir];
	memset(psDir, 0, nDir);
	memcpy(psDir, strDir.c_str(), strDir.length());

	SHFILEOPSTRUCT FileOp; 
	ZeroMemory((void*)&FileOp,sizeof(SHFILEOPSTRUCT));

	FileOp.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR ; 
	FileOp.hNameMappings = NULL; 
	FileOp.hwnd = NULL; 
	FileOp.lpszProgressTitle = NULL; 
	FileOp.pFrom = psDir;
	FileOp.pTo = NULL;
	FileOp.wFunc = FO_DELETE; 

	int nRet = SHFileOperation(&FileOp);

	delete[] psDir;

	if (nRet != 0)
	{
		return false;
	}

	strDir = strDir.substr(0, strDir.find_last_of('\\') + 1);
	if (!::RemoveDirectory(strDir.c_str()))
	{
		return false;
	}	

	return true;
}

STDMETHODIMP CFileUpDownEx::QueryUpload(BSTR bstrJson, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ZTools::WriteZToolsLog(">>>>>>PeraFileUpDownEx::CFileUpDownEx::QueryUpload");
	CComVariant vReturn(_bstr_t("").GetBSTR());

	CPfudeTheadData* pTheadData = NULL;
	pTheadData = new CPfudeTheadData();

	CDataQueryIn dataQueryIn;
	if (dataQueryIn.Init(bstrJson) == FALSE || dataQueryIn.m_strIndex.empty())
	{
		pTheadData->m_strFlag = "error";
		pTheadData->m_strMsg = "com接口入参不合法";
		pTheadData->DetachToStartVariant(pvRet);
		delete pTheadData;
		pTheadData = NULL;
		return S_OK;
	}

	pTheadData->m_strIndex = dataQueryIn.m_strIndex;

	std::map<std::string, CPfudeTheadData*>::const_iterator cIter;
	cIter = g_mapUploads.find(pTheadData->m_strIndex);
	if (cIter == g_mapUploads.end()) 
	{
		pTheadData->m_strFlag = "error";
		pTheadData->m_strMsg = "查询的上传任务不存在";
		pTheadData->DetachToStartVariant(pvRet);
		delete pTheadData;
		pTheadData = NULL;
		return S_OK;
	}

	delete pTheadData;
	pTheadData = NULL;

	cIter->second->DetachToQueryVariant(pvRet);

	return S_OK;
}

STDMETHODIMP CFileUpDownEx::CancelUpload(BSTR bstrJson, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ZTools::WriteZToolsLog(">>>>>>PeraFileUpDownEx::CFileUpDownEx::CancelUpload");
	CComVariant vReturn(_bstr_t("").GetBSTR());

	CPfudeTheadData* pTheadData = NULL;
	pTheadData = new CPfudeTheadData();

	CDataCancelIn dataCancelIn;
	if (dataCancelIn.Init(bstrJson) == FALSE || dataCancelIn.m_strIndex.empty())
	{
		pTheadData->m_strFlag = "error";
		pTheadData->m_strMsg = "com接口入参不合法";
		pTheadData->DetachToStartVariant(pvRet);
		delete pTheadData;
		pTheadData = NULL;
		return S_OK;
	}

	pTheadData->m_strIndex = dataCancelIn.m_strIndex;

	std::map<std::string, CPfudeTheadData*>::const_iterator cIter;
	cIter = g_mapUploads.find(pTheadData->m_strIndex);
	if (cIter == g_mapUploads.end()) 
	{
		pTheadData->m_strFlag = "error";
		pTheadData->m_strMsg = "取消的上传任务不存在";
		pTheadData->DetachToStartVariant(pvRet);
		delete pTheadData;
		pTheadData = NULL;
		return S_OK;
	}

	delete pTheadData;
	pTheadData = NULL;

	cIter->second->Lock();
	cIter->second->m_bCancel = TRUE;
	cIter->second->Unlock();

	cIter->second->DetachToCancelVariant(pvRet);

	return S_OK;
}

CFileUpDownEx::CFileUpDownEx()
{
}

HRESULT CFileUpDownEx::FinalConstruct()
{
	return S_OK;
}

void CFileUpDownEx::FinalRelease()
{
}

STDMETHODIMP CFileUpDownEx::parseXpdl(BSTR bstrJsonInfo, BSTR bstrJsonResult, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	CComVariant vReturn(_bstr_t("{\"errorMsg\":\"上传中间件处理XPDL出现异常!\"}").GetBSTR());

	std::string strJsonInfo = (bstrJsonInfo != NULL) ? _bstr_t(bstrJsonInfo) : "";
	ZTools::WriteZToolsFormatLog("strJsonInfo:%s", strJsonInfo.c_str());
	std::string strJsonResult = (bstrJsonResult != NULL) ? _bstr_t(bstrJsonResult) : "";
	ZTools::WriteZToolsFormatLog("strJsonResult:%s", strJsonResult.c_str());
	//strJsonResult = "{\"allFiles\":{\"\\\\F099g9n\\\\Manifest.xml\":\"5445cb27f10377f002261381\",\"\\\\F099g9n\\\\工具集成组件.bot\":\"5445cb27f10377f002261383\",\"\\\\F0xn4m7\\\\Manifest.xml\":\"5445cb25f10377f002261359\",\"\\\\F0xn4m7\\\\工具集成组件.bot\":\"5445cb25f10377f00226135b\",\"\\\\F27cffr\\\\Excel组件_1.bot\":\"5445cb27f10377f00226138b\",\"\\\\F27cffr\\\\Manifest.xml\":\"5445cb27f10377f002261389\",\"\\\\F2lx1l1\\\\Manifest.xml\":\"5445cb26f10377f00226136d\",\"\\\\F2lx1l1\\\\工具集成组件.bot\":\"5445cb26f10377f00226136f\",\"\\\\F30u720\\\\Manifest.xml\":\"5445cb26f10377f002261369\",\"\\\\F30u720\\\\工具集成组件.bot\":\"5445cb26f10377f00226136b\",\"\\\\F49ob53\\\\Manifest.xml\":\"5445cb24f10377f002261350\",\"\\\\F49ob53\\\\数据解析组件.bot\":\"5445cb24f10377f002261351\",\"\\\\F4e095g\\\\Manifest.xml\":\"5445cb25f10377f002261355\",\"\\\\F4e095g\\\\工具集成组件.bot\":\"5445cb25f10377f002261357\",\"\\\\F6l6w4l\\\\Manifest.xml\":\"5445cb26f10377f00226137d\",\"\\\\F6l6w4l\\\\数据解析组件.bot\":\"5445cb26f10377f00226137f\",\"\\\\F7066k8\\\\Manifest.xml\":\"5445cb24f10377f002261352\",\"\\\\F7066k8\\\\数据解析组件.bot\":\"5445cb24f10377f002261353\",\"\\\\F87431p\\\\Manifest.xml\":\"5445cb25f10377f00226135d\",\"\\\\F87431p\\\\工具集成组件.bot\":\"5445cb25f10377f00226135f\",\"\\\\Fakb301\\\\Excel组件_1.bot\":\"5445cb26f10377f00226137b\",\"\\\\Fakb301\\\\Manifest.xml\":\"5445cb26f10377f002261379\",\"\\\\Fbcr53f\\\\Excel组件.bot\":\"5445cb27f10377f002261387\",\"\\\\Fbcr53f\\\\Manifest.xml\":\"5445cb27f10377f002261385\",\"\\\\Ff9rz9d\\\\Excel组件.bot\":\"5445cb24f10377f00226134f\",\"\\\\Ff9rz9d\\\\Manifest.xml\":\"5445cb24f10377f00226134e\",\"\\\\Fjn0g5u\\\\Manifest.xml\":\"5445cb26f10377f002261375\",\"\\\\Fjn0g5u\\\\工具集成组件_1.bot\":\"5445cb26f10377f002261377\",\"\\\\Fmm0pfy\\\\Manifest.xml\":\"5445cb25f10377f002261365\",\"\\\\Fmm0pfy\\\\工具集成组件_1.bot\":\"5445cb25f10377f002261367\",\"\\\\Fodzd42\\\\Manifest.xml\":\"5445cb25f10377f002261361\",\"\\\\Fodzd42\\\\工具集成组件.bot\":\"5445cb25f10377f002261363\",\"\\\\Fu0cpe7\\\\Excel组件.bot\":\"5445cb24f10377f00226134d\",\"\\\\Fu0cpe7\\\\Manifest.xml\":\"5445cb24f10377f00226134c\",\"\\\\Fyl9992\\\\Manifest.xml\":\"5445cb27f10377f00226138d\",\"\\\\Fyl9992\\\\脚本驱动组件.bot\":\"5445cb27f10377f00226138f\",\"\\\\Fz23r2y\\\\Excel组件.bot\":\"5445cb26f10377f002261373\",\"\\\\Fz23r2y\\\\Manifest.xml\":\"5445cb26f10377f002261371\",\"\\\\main.xpdl\":\"5445cb24f10377f00226134b\",\"\\\\n0000002d212740-ced6-faac-1bac-6c798a645ec8\\\\二级飞行输出文件控制.xlsx\":\"5445cb28f10377f0022613b1\",\"\\\\n000000333f2101-fa41-f77b-8cb8-7f5d60225ff7\\\\一级飞行段弹性参数计算结果_模板.doc\":\"5445cb29f10377f0022613be\",\"\\\\n000000850a4b9e-2790-3ff0-dd18-d6629e4e4eee\\\\slosh.xls\":\"5445cb27f10377f002261392\",\"\\\\n0000008abc9bd2-bea9-fcdc-b698-96c31b4e3e6a\\\\一级飞行输出文件控制.xlsx\":\"5445cb28f10377f0022613af\",\"\\\\n000000a7ce3b46-8c30-a2a2-04e2-d28fad3ed5f9\\\\F06L.rar\":\"5445cb28f10377f002261398\",\"\\\\n000000acfa806e-5c94-9b81-15ea-0cbdb6fe29dc\\\\DatFile.rar\":\"5445cb28f10377f0022613a9\",\"\\\\n000000b9f5b577-3cc2-bd70-0131-2cf7bef2fa99\\\\constraint.txt\":\"5445cb28f10377f0022613a8\",\"\\\\n000000baa6af33-3eaf-46bf-bde6-3f7505efefa8\\\\二级飞行段弹性参数计算结果_模板.doc\":\"5445cb29f10377f0022613bd\",\"\\\\n000000c4affc9d-25bd-ab5e-ce0b-91b6cd64aeae\\\\横向弹性秒状态.txt\":\"5445cb27f10377f002261393\",\"\\\\n000000dce23a2e-15a1-95c5-6a17-eb26f6e90f27\\\\FbFile.rar\":\"5445cb27f10377f002261394\",\"\\\\n000000e58b77a7-5dad-b9c3-6a60-da8cebba3a04\\\\s2.input\":\"5445cb28f10377f0022613a1\",\"\\\\n000000e6276dfb-a581-59bc-011c-f8b1a04e7fb3\\\\s3.input\":\"5445cb28f10377f0022613a2\",\"\\\\n000000ecfecd6c-19c7-68c0-36df-237bd827ba76\\\\s1.input\":\"5445cb28f10377f00226139e\",\"\\\\n099g9n233544a9-56c5-44cf-be44-b42e6672dd6f\\\\ResOutFile.rar\":\"5445cb29f10377f0022613b6\",\"\\\\n099g9nb5754a71-a879-4b04-b8f2-0afd2d87e151\\\\AeroIn.txt\":\"5445cb29f10377f0022613c4\",\"\\\\n099g9ndefe3227-ff8b-4a93-a849-e2dfebb7e1cf\\\\横向弹性秒状态.txt\":\"5445cb27f10377f002261393\",\"\\\\n0xn4m70eac849f-a140-45f0-bf98-8f948d3123e9\\\\AeroIn.txt\":\"5445cb29f10377f0022613c4\",\"\\\\n0xn4m732dc0f65-ff0f-4ef0-9373-27e4f4d2fb99\\\\气动专业数据.xlsx\":\"5445cb28f10377f002261396\",\"\\\\n0xn4m74c14831e-d04c-4b1b-9f7e-9ab2dfc9be99\\\\尾翼数据模板.xls\":\"5445cb28f10377f002261397\",\"\\\\n0xn4m7aeefcf6f-4edb-4759-b697-0c2198b5ae18\\\\横向弹性秒状态.txt\":\"5445cb27f10377f002261393\",\"\\\\n0xn4m7f5307ea6-d19b-462f-9863-532fc98b3065\\\\FbFile.rar\":\"5445cb27f10377f002261394\",\"\\\\n0xn4m7fccbec1b-1140-4a11-acb3-c6295cecb019\\\\法向力系数模版.xls\":\"5445cb27f10377f002261395\",\"\\\\n27cffr53d260d7-8405-4405-9e79-a2fc239aae37\\\\2st_OUT文件数据组合输出结果.xls\":\"5445cb29f10377f0022613ba\",\"\\\\n2lx1l12bb23b2b-33e2-4556-8f4e-d28f9d072eb7\\\\一级飞行输出文件控制.xlsx\":\"5445cb28f10377f0022613af\",\"\\\\n2lx1l1675409cb-6b3c-4f8c-a29b-06322fce8ca8\\\\S1ResDatFile.rar\":\"5445cb28f10377f0022613ae\",\"\\\\n2lx1l1730bc428-cab6-4d69-a2ca-353b751bae3b\\\\AeroIn.txt\":\"5445cb29f10377f0022613c4\",\"\\\\n30u720134008ca-48e5-4d3b-a7c9-1a098695c2c6\\\\AeroIn.txt\":\"5445cb29f10377f0022613c4\",\"\\\\n30u7203f14fd42-4953-4d96-b33f-a619353c51e7\\\\横向弹性秒状态.txt\":\"5445cb27f10377f002261393\",\"\\\\n30u7205472dc04-62c5-4520-a967-3b77ad0b5ee4\\\\constraint.txt\":\"5445cb28f10377f0022613a8\",\"\\\\n30u720633c93d1-4b9b-4ecb-99a9-9ebecc6e33f7\\\\平台捷联冗余状态输入文件.dat\":\"5445cb28f10377f0022613ad\",\"\\\\n30u72098957e5a-6d89-44d0-8fe3-2d8fa69fd7e8\\\\TorqueOFile.rar\":\"5445cb28f10377f0022613ac\",\"\\\\n30u720a4145e39-8416-4dcd-9854-ec24f52904bb\\\\fFile.rar\":\"5445cb28f10377f0022613aa\",\"\\\\n30u720e0f5ce51-2fb8-4780-969a-8f8460b1772d\\\\F06L.rar\":\"5445cb28f10377f002261398\",\"\\\\n30u720fa61d7b3-5fc9-4296-8e14-ae2560b7b8ec\\\\OFile.rar\":\"5445cb28f10377f0022613ab\",\"\\\\n49ob5305e23f1b-bb74-43dd-aa29-389867699f4c\\\\二级摇摆发动机推力数据.txt\":\"5445cb29f10377f0022613bf\",\"\\\\n4e095g0daacf83-0320-4e5b-b815-972254b680db\\\\slosh.xls\":\"5445cb27f10377f002261392\",\"\\\\n4e095gab5cec0d-fb65-467c-af11-6bc52b6a688b\\\\AeroIn.txt\":\"5445cb29f10377f0022613c4\",\"\\\\n4e095gf91c4eff-2fac-4ff7-96b0-318443724fc8\\\\横向弹性秒状态.txt\":\"5445cb27f10377f002261393\",\"\\\\n6l6w4l146ff5fa-9fa3-49b9-99af-b7c210349b39\\\\Level1_tor.OUT\":\"5445cb29f10377f0022613c1\",\"\\\\n6l6w4l3d6d3a0e-dbc0-4489-8df5-60c44639eac9\\\\Level2_tor.OUT\":\"5445cb29f10377f0022613c2\",\"\\\\n6l6w4lb0ddc875-0c6f-4e3f-8844-947f5b6f1709\\\\Level2p_tor.OUT\":\"5445cb29f10377f0022613c3\",\"\\\\n7066k8eb445b36-b4ed-43c9-a138-485ea86d475b\\\\平台捷联冗余状态输入文件.dat\":\"5445cb29f10377f0022613c0\",\"\\\\n87431p003fff56-302d-491f-be6c-5a9f0e6b63f6\\\\AeroIn.txt\":\"5445cb29f10377f0022613c4\",\"\\\\n87431p5a25db1a-7323-4f3f-b43e-e7524573a3a8\\\\F06L.rar\":\"5445cb28f10377f002261398\",\"\\\\n87431p71a0f7d0-1ee1-44da-ae47-f045f0735183\\\\fbProDAT.rar\":\"5445cb28f10377f002261399\",\"\\\\n87431p927a0013-0548-45a6-9a55-f58dc91082fd\\\\横向弹性秒状态.txt\":\"5445cb27f10377f002261393\",\"\\\\n87431padf7bdfe-e07b-4925-862d-c64f2d66b4c8\\\\fbProTxt.rar\":\"5445cb28f10377f00226139a\",\"\\\\nakb301529adc98-9cfe-4ed3-83e5-e79252abd596\\\\二级飞行DAT文件数据.xlsx\":\"5445cb29f10377f0022613b5\",\"\\\\nbcr53f93582639-9851-4c78-be4c-601b11be801d\\\\1st_OUT文件数据组合输出结果.xls\":\"5445cb29f10377f0022613b8\",\"\\\\nf9rz9d432970e0-2954-47ce-9d89-18e735ef6ca7\\\\气动专业数据.xlsx\":\"5445cb27f10377f002261391\",\"\\\\njn0g5u780b06ff-4613-4d97-8621-6444eaa9812d\\\\AeroIn.txt\":\"5445cb29f10377f0022613c4\",\"\\\\njn0g5uac8be95c-7313-4b9c-a416-5c47fb3450b4\\\\二级飞行输出文件控制.xlsx\":\"5445cb28f10377f0022613b1\",\"\\\\njn0g5uf8cf4362-4d20-40b8-a8a3-a032307a8cae\\\\S2ResDatFile.rar\":\"5445cb28f10377f0022613b0\",\"\\\\nmm0pfy2e7e7d30-f529-4b10-bae2-9f8c3def94cb\\\\横向弹性秒状态.txt\":\"5445cb27f10377f002261393\",\"\\\\nmm0pfy62f9d592-222b-4490-ab1d-d3a88086f4bb\\\\FreSelFile.rar\":\"5445cb28f10377f00226139b\",\"\\\\nmm0pfy767f1635-2d7d-4be8-8abc-0c22dc6a38ad\\\\F06L.rar\":\"5445cb28f10377f002261398\",\"\\\\nmm0pfy7ee6c23a-ce07-44cd-bbbe-8c1914fe89da\\\\AeroIn.txt\":\"5445cb29f10377f0022613c4\",\"\\\\nmm0pfyc2f2bb85-b254-40da-8255-33692434afe4\\\\DatFile.rar\":\"5445cb28f10377f0022613a9\",\"\\\\nmm0pfyf7a95edb-0f13-428e-b949-e74ebedc4237\\\\input.txt\":\"5445cb28f10377f00226139c\",\"\\\\nmm0pfyff033dfb-e945-4fbe-931f-03f4d94e787b\\\\constraint.txt\":\"5445cb28f10377f0022613a8\",\"\\\\nodzd42060b7102-3681-40ba-aa01-2c5af18509e8\\\\初始数据模版.xls\":\"5445cb28f10377f0022613a3\",\"\\\\nodzd4207319528-384b-4803-8408-68d75535de4c\\\\AeroIn.txt\":\"5445cb29f10377f0022613c4\",\"\\\\nodzd421e9aebd6-f594-4ad4-9a08-a72c826d38ba\\\\s1_01_FBForPF.DAT\":\"5445cb28f10377f00226139d\",\"\\\\nodzd42291a9d87-c455-4fc0-aa5b-11c40ea02117\\\\法向力系数.xls\":\"5445cb28f10377f0022613a5\",\"\\\\nodzd422f3634b3-42a0-4d54-87da-b93e60b8b4b8\\\\input.txt\":\"5445cb28f10377f00226139c\",\"\\\\nodzd4243e92e00-afc2-4138-b4d6-056afe90d32e\\\\s2_02_FBForPF.DAT\":\"5445cb28f10377f0022613a0\",\"\\\\nodzd424e900873-5d56-4e0e-8f1f-da2284a54168\\\\尾翼数据.xls\":\"5445cb28f10377f0022613a7\",\"\\\\nodzd427373cc91-feec-4e24-b009-782ba9d9575f\\\\横向弹性秒状态.txt\":\"5445cb27f10377f002261393\",\"\\\\nodzd42740327a2-b131-4bb5-818f-0462bfe90607\\\\s2_01_FBForPF.DAT\":\"5445cb28f10377f00226139f\",\"\\\\nodzd428294faab-ef21-4780-81ce-bad73eace23e\\\\二级摇摆发动机推力数据.txt\":\"5445cb28f10377f0022613a4\",\"\\\\nodzd42988c7b6b-b1fc-447f-a903-af0920019910\\\\FreSelFile.rar\":\"5445cb28f10377f00226139b\",\"\\\\nodzd42baa73cae-9fe5-4c43-86e9-1be5960a3319\\\\s2.input\":\"5445cb28f10377f0022613a1\",\"\\\\nodzd42bfb1d97b-d89a-4b12-9cfb-3fd238f5c595\\\\FbFile.rar\":\"5445cb27f10377f002261394\",\"\\\\nodzd42ca42ce29-bab1-458a-bf14-3bb9361c971b\\\\s3.input\":\"5445cb28f10377f0022613a2\",\"\\\\nodzd42eef9bdce-e748-405a-ba61-9490a3d4748e\\\\s1.input\":\"5445cb28f10377f00226139e\",\"\\\\nodzd42f639584d-3654-4039-bd5c-fbe234e83ff3\\\\晃动计算结果.xls\":\"5445cb28f10377f0022613a6\",\"\\\\nu0cpe7cc634b52-9a16-41cb-8e1a-9fff04340173\\\\初始数据模版.xls\":\"5445cb27f10377f002261390\",\"\\\\nyl999208893049-a3f9-492e-a316-9e264a2ab660\\\\二级飞行段弹性参数计算结果_模板.doc\":\"5445cb29f10377f0022613bd\",\"\\\\nyl999234bc4966-dac4-4ca5-977b-1f3a9b7d814e\\\\S1ResDatFile.rar\":\"5445cb28f10377f0022613ae\",\"\\\\nyl99923813797c-be40-4a11-9c43-97cc1ce0a84a\\\\S2ResDatFile.rar\":\"5445cb28f10377f0022613b0\",\"\\\\nyl99924b39f812-b4bf-409a-8fa5-9e4d928cf6f3\\\\横向弹性秒状态.txt\":\"5445cb27f10377f002261393\",\"\\\\nyl99928cbbaac5-b3f4-4ee4-b509-997ef0e63084\\\\一级飞行段弹性参数计算结果_模板.doc\":\"5445cb29f10377f0022613be\",\"\\\\nyl99928fd777dd-db83-4a4f-ba2f-57fba6389b12\\\\AeroIn.txt\":\"5445cb29f10377f0022613c4\",\"\\\\nyl9992a2dfd150-6498-404c-bf77-860007cafa50\\\\S1TorDatFile.rar\":\"5445cb29f10377f0022613bb\",\"\\\\nyl9992cee8275a-5ed0-4226-860f-c4fa45f3b0b6\\\\S2TorDatFile.rar\":\"5445cb29f10377f0022613bc\",\"\\\\nz23r2y263debc5-9344-4953-bf26-04e9228dae16\\\\一级飞行DAT文件数据.xlsx\":\"5445cb28f10377f0022613b3\"},\"component\":{\"fileId\":\"\",\"fileName\":\"\",\"manifest\":\"\"},\"form\":{\"fileId\":\"\",\"fileName\":\"\",\"manifest\":\"\"},\"mainFile\":\"5445cb24f10377f00226134b\",\"thisFile\":\"\"}";

	//按fileid下载xpdl并读取内容
	//解析xpdl
	//替换fileId
	//压缩得到二进制数据
	//调后台接口
	
	Json::Value jsonResult;
	Json::Reader jsonReader;

	if (!jsonReader.parse(strJsonResult, jsonResult))
	{
		ZTools::WriteZToolsFormatLog("error: JSON格式解析失败");
		vReturn.Detach(pvRet);
		return S_OK;
	}

	//取xpdl的fileid
	std::string strXpdlFileId = jsonResult["mainFile"].asString();
	if (strXpdlFileId.empty())
	{
		ZTools::WriteZToolsFormatLog("error: xpdl的fileid为空");
		vReturn.Detach(pvRet);
		return S_OK;
	}
	
	//找xpdl文件路径
	CDownUploadDataFileObject dataFileObject;
	dataFileObject.m_strFileId = strXpdlFileId;

	std::string strXpdlFilePath;
	if (!CDownUploadFileCacheFinder::ReadDownloadCache(&dataFileObject))
	{
		ZTools::WriteZToolsFormatLog("error: 从缓存中查找xpdl文件失败，fileId:%s", strXpdlFileId.c_str());
		vReturn.Detach(pvRet);
		return S_OK;
	}
	strXpdlFilePath = dataFileObject.m_strFilePath;

	//解析xpdl
	ZTools::WriteZToolsFormatLog("加载xpdl");
	CFileIdXpdl xpdlObj;
	xpdlObj.Load(strXpdlFilePath.c_str());
	ZTools::WriteZToolsFormatLog("解析xpdl");
	if (!xpdlObj.Parse())
	{
		ZTools::WriteZToolsFormatLog("error: 解析xpdl内容失败");
		vReturn.Detach(pvRet);
		return S_OK;
	}

	//替换文件路径为fileId
	ZTools::WriteZToolsFormatLog("替换xpdl中文件路径为fileid");
	xpdlObj.ReplacePathToFileId(jsonResult);

	std::string strXpdlStream;
	xpdlObj.GetXpdlStream(strXpdlStream);

	ZTools::WriteZToolsFormatLog("替换后的xpdl文本\n%s", strXpdlStream.c_str());

	//压缩
	ZTools::WriteZToolsFormatLog("压缩xpdl");
	ZTools::MBToUTF8(strXpdlStream);
	int nSrcLength = strXpdlStream.length();
	byte* pBuf = new byte[nSrcLength];
	memcpy(pBuf, (byte*)strXpdlStream.c_str(), nSrcLength);

	HZIP hz = CreateZip(0,100000,ZIP_MEMORY);
	ZipAdd(hz, "main.xpdl", pBuf, nSrcLength, ZIP_MEMORY);

	delete [] pBuf;
	pBuf = NULL;
	
	unsigned long nl = 0;
	ZipGetMemory(hz, (void**)&pBuf, &nl);

	std::vector<byte> vByteData(pBuf, pBuf + nl);

	CloseZip(hz);
	ZTools::WriteZToolsFormatLog("xpdl原长：%d，压缩后：%d", nSrcLength, nl);

	//调后台ICE接口
	ZTools::WriteZToolsFormatLog("调用后台ICE接口");
	std::string strReturn = savePlanByByteXpdl(strJsonInfo, vByteData);

	vReturn = strReturn.c_str();
	vReturn.Detach(pvRet);

	return S_OK;
}
