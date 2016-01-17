#include "StdAfx.h"
#include "DataDefine.h"
#include "Tools.h"
#include "DownUploadCommonFunc.h"
#include <comdef.h>
#include "ConfigFunc.h"
using namespace ATL;

CPfudeTheadData::CPfudeTheadData(void)
{
	m_strFilePath.clear();
	m_bIsUploadThisFile = FALSE;
	m_strFileName.clear();
	m_strFileType.clear();
	//m_strFileMd5.clear();
	m_strFlag.clear();
	m_strMsg.clear();
	m_strIndex.clear();
	m_strFileIdListJson.clear();
	m_strMainFileId.clear();
	m_strThisFileId.clear();
// 	m_nRate = 0;
 	m_n64AllSize = 0;
// 	m_n64FinishedSize = -1;
	m_bCancel = FALSE;
	m_strHproseRpc.clear();
	m_strUnzipDir.clear();

	m_vJobs.clear();
	m_strIndex = GuidToString(CreateGuid());
	m_strIndex = m_strIndex.substr(m_strIndex.rfind('-')+1);
	m_strHproseRpc = GetHproseRpcEx();

	//本地缓存路径
	CString strCacheRoot =  GetConfig("Pera","RunCache");
	if (strCacheRoot.IsEmpty())
		strCacheRoot = "C:\\WorkSpace\\Data";

	m_strUnzipDir = ZTools::FormatString("%s\\%s\\", (LPCTSTR)strCacheRoot, m_strIndex.c_str());
	//m_strUnzipDir = ZTools::FormatString("%s%s\\", GetSpacialCacheBasePath().c_str(), m_strIndex.c_str());
}

CPfudeTheadData::~CPfudeTheadData(void)
{
	Clear();
}

void CPfudeTheadData::Clear()
{
}

std::string CPfudeTheadData::ToStartReturnJson()
{
	Json::Value json;
	json["flag"] = Json::Value(m_strFlag);
	if (m_bCancel)
	{
		json["flag"] = Json::Value("cancel");
	}
	json["msg"] = Json::Value(m_strMsg);
	json["index"] = Json::Value(m_strIndex);
	json["fileType"] = Json::Value(m_strFileType);
	json["filePath"] = Json::Value(m_strFilePath);
	json["fileName"] = Json::Value(m_strFileName);

	Json::FastWriter writer;
	return writer.write(json);
}

void CPfudeTheadData::DetachToStartVariant( VARIANT* pvRet )
{
	std::string strJson = ToStartReturnJson();
	ZTools::WriteZToolsLog(strJson);
	CComVariant v(_bstr_t(strJson.c_str()).GetBSTR());
	v.Detach(pvRet);
}
/*
{
"index": "123abc",
"flag": "error",	//error,有错误,uploading,已开始正在上传,finish,已结束,cancel,取消
"msg": "相关提示信息",
"rate": "50",
"fileType": "Process",	//Process设计过程,Component组件
"result": {
"mainFile": "1231313131213",
"thisFile": "2123113213221132",
"allFiles": {
"main.xpdl": "afafadsfasdf",
"aaa\\1.txt": "afafadsfasdfaa",
"bbb\\1.bot": "aaaafafadsfasdf"
}
}
}
*/
std::string CPfudeTheadData::ToQueryReturnJson()
{
	Json::Value json;
	json["index"] = Json::Value(m_strIndex);
	json["flag"] = Json::Value(m_strFlag);
	if (m_bCancel)
	{
		json["flag"] = Json::Value("cancel");
	}
	json["msg"] = Json::Value(m_strMsg);
	json["fileType"] = Json::Value(m_strFileType);
	json["filePath"] = Json::Value(m_strFilePath);
	json["fileName"] = Json::Value(m_strFileName);

	json["rate"] = Json::Value("0");;
 	if (m_strFlag.compare("uploading") == 0)
 	{
		if (m_n64AllSize <= 0)
		{
			json["rate"] = Json::Value("0");
		}
		else
		{
			__int64 nUploadedSize = m_thisFileObject.m_nFileSizeUploaded;
			nUploadedSize += m_componentFileObject.m_nFileSizeUploaded;
			nUploadedSize += m_componentManifestObject.m_nFileSizeUploaded;
			nUploadedSize += m_formFileObject.m_nFileSizeUploaded;
			nUploadedSize += m_formManifestObject.m_nFileSizeUploaded;
			for (std::vector<CDataUpladJob>::iterator it = m_vJobs.begin(); it != m_vJobs.end(); ++it)
			{
				nUploadedSize += it->fileObject.m_nFileSizeUploaded;
			}
			json["rate"] = Json::Value(ZTools::FormatString("%d", nUploadedSize / (m_n64AllSize / 100)));
		}
 	}
	else if (m_strFlag.compare("finish") == 0)
	{
		json["rate"] = Json::Value("100");

		Json::Value jsonResult;
		jsonResult["mainFile"] = Json::Value(m_strMainFileId);
		jsonResult["thisFile"] = Json::Value(m_strThisFileId);

		Json::Value jsonAllFiles;
		Json::Reader reader;
		if (reader.parse(m_strFileIdListJson, jsonAllFiles))
		{
			jsonResult["allFiles"] = jsonAllFiles;
		}

		//BOTW
		Json::Value jsonComponent;
		jsonComponent["fileName"] = Json::Value(m_componentFileObject.m_strFileName);
		jsonComponent["fileId"] = Json::Value(m_componentFileObject.m_strFileId);
		jsonComponent["manifest"] = Json::Value(m_componentManifestObject.m_strFileId);
		jsonResult["component"] = jsonComponent;
		Json::Value jsonForm;
		jsonForm["fileName"] = Json::Value(m_formFileObject.m_strFileName);
		jsonForm["fileId"] = Json::Value(m_formFileObject.m_strFileId);
		jsonForm["manifest"] = Json::Value(m_formManifestObject.m_strFileId);
		jsonResult["form"] = jsonForm;

		json["result"] = jsonResult;

		//MD5值，用于杨波判断多次上传生成的方案是不是属于同一模板
		json["md5"] = Json::Value(m_thisFileObject.m_strMd5);
	}

// 	json["flag"] = Json::Value(m_strFlag);
// 	json["msg"] = Json::Value(m_strMsg);
// 	json["index"] = Json::Value(m_strIndex);
// 	json["fileType"] = Json::Value(m_strFileType);

	Json::FastWriter writer;
	return writer.write(json);
}

void CPfudeTheadData::DetachToQueryVariant( VARIANT* pvRet )
{
	std::string strJson = ToQueryReturnJson();
	ZTools::WriteZToolsLog(strJson);
	CComVariant v(_bstr_t(strJson.c_str()).GetBSTR());
	v.Detach(pvRet);

}

std::string CPfudeTheadData::ToCancelReturnJson()
{
	Json::Value json;
	json["flag"] = Json::Value(m_strFlag);
	if (m_bCancel)
	{
		json["flag"] = Json::Value("cancel");
	}
 	json["msg"] = Json::Value(m_strMsg);
 	json["index"] = Json::Value(m_strIndex);
	json["fileType"] = Json::Value(m_strFileType);
	json["filePath"] = Json::Value(m_strFilePath);
	json["fileName"] = Json::Value(m_strFileName);

	Json::FastWriter writer;
	return writer.write(json);
}

void CPfudeTheadData::DetachToCancelVariant( VARIANT* pvRet )
{
	std::string strJson = ToCancelReturnJson();
	ZTools::WriteZToolsLog(strJson);
	CComVariant v(_bstr_t(strJson.c_str()).GetBSTR());
	v.Detach(pvRet);

}

BOOL CPfudeTheadData::IsProcess()
{
	if (m_strFileType.compare("Process") == 0)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CPfudeTheadData::IsComponent()
{
	if (m_strFileType.compare("Component") == 0)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CPfudeTheadData::IsComponentWithForm()
{
	if (m_strFileType.compare("ComponentWithForm") == 0)
	{
		return TRUE;
	}
	return FALSE;
}

void CPfudeTheadData::Lock()
{
	criticalSectionWrite.Lock();
}

void CPfudeTheadData::Unlock()
{
	criticalSectionWrite.Unlock();
}

CDataStartIn::CDataStartIn(void)
{
	m_strFilePath.clear();
	m_bIsUploadThisFile = FALSE;
}


CDataStartIn::~CDataStartIn(void)
{
	Clear();
}

void CDataStartIn::Clear()
{
}

BOOL CDataStartIn::Init(BSTR bstrJson)
{
	if (bstrJson == NULL)
	{
		return FALSE;
	}

	std::string strJson = _bstr_t(bstrJson);
	ZTools::WriteZToolsFormatLog("strJson:%s", strJson.c_str());

	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(strJson, root))
	{
		return FALSE;
	}
	
	m_strFilePath = root["filePath"].asString();
	std::string strIsUploadThisFile = root["isUploadThisFile"].asString();
	m_bIsUploadThisFile = strIsUploadThisFile == "true" ? TRUE : FALSE;

	return TRUE;
}

void CSpecialZipCache::Clear()
{
}

BOOL CSpecialZipCache::ReadCache()
{
	std::string strInfoXmlPath;
	strInfoXmlPath = ZTools::FormatString("%s%s_%s\\info.xml", GetSpacialCacheBasePath().c_str(), m_strThisFileMd5.c_str(), m_strThisFileSize.c_str());

	if (!PathFileExists(strInfoXmlPath.c_str()))
	{
		return FALSE;
	}

	TiXmlBase::SetCondenseWhiteSpace(false);
	TiXmlDocument doc;

#ifdef TIXML_USE_STL
	doc.LoadFile(strInfoXmlPath, TIXML_ENCODING_UTF8);
#else
	doc.LoadFile(strInfoXmlPath.c_str(), TIXML_ENCODING_UTF8);
#endif


// 	std::string m_strFileIdListJson;
// 	std::string m_strMainFileId;
// 	std::string m_strThisFileId;
// 	std::string m_strThisFileMd5;

	TiXmlElement* rootElement = doc.RootElement();
	if (rootElement == NULL)
	{
		return FALSE;
	}

#ifdef TIXML_USE_STL
	rootElement->QueryStringAttribute("fileIdListJson", &m_strFileIdListJson);
	rootElement->QueryStringAttribute("mainFileId", &m_strMainFileId);
	//rootElement->QueryStringAttribute("thisFileId", &m_strThisFileId);
	rootElement->QueryStringAttribute("thisFileMd5", &m_strThisFileMd5);

	rootElement->QueryStringAttribute("componentFileName", &m_strComponentFileName);
	rootElement->QueryStringAttribute("componentFileId", &m_strComponentFileId);
	rootElement->QueryStringAttribute("componentManifest", &m_strComponentManifest);
	rootElement->QueryStringAttribute("formFileName", &m_strFormFileName);
	rootElement->QueryStringAttribute("formFileId", &m_strFormFileId);
	rootElement->QueryStringAttribute("formManifest", &m_strFormManifest);
#else
	const char* pStrFileIdListJson = rootElement->Attribute("fileIdListJson");
	if (pStrFileIdListJson != NULL)
	{
		m_strFileIdListJson = std::string(pStrFileIdListJson);
	}
	const char* pStrMainFileId = rootElement->Attribute("mainFileId");
	if (pStrMainFileId != NULL)
	{
		m_strMainFileId = std::string(pStrMainFileId);
	}
// 	const char* pStrThisFileId = rootElement->Attribute("thisFileId");
// 	if (pStrThisFileId != NULL)
// 	{
// 		m_strThisFileId = std::string(pStrThisFileId);
// 	}
	const char* pStrThisFileMd5 = rootElement->Attribute("thisFileMd5");
	if (pStrThisFileMd5 != NULL)
	{
		m_strThisFileMd5 = std::string(pStrThisFileMd5);
	}
	const char* pStrComponentFileName = rootElement->Attribute("componentFileName");
	if (pStrComponentFileName != NULL)
	{
		m_strComponentFileName = std::string(pStrComponentFileName);
	}
	const char* pStrComponentFileId = rootElement->Attribute("componentFileId");
	if (pStrComponentFileId != NULL)
	{
		m_strComponentFileId = std::string(pStrComponentFileId);
	}
	const char* pStrComponentManifest = rootElement->Attribute("componentManifest");
	if (pStrComponentManifest != NULL)
	{
		m_strComponentManifest = std::string(pStrComponentManifest);
	}
	const char* pStrFormFileName = rootElement->Attribute("formFileName");
	if (pStrFormFileName != NULL)
	{
		m_strFormFileName = std::string(pStrFormFileName);
	}
	const char* pStrFormFileId = rootElement->Attribute("formFileId");
	if (pStrFormFileId != NULL)
	{
		m_strFormFileId = std::string(pStrFormFileId);
	}
	const char* pStrFormManifest = rootElement->Attribute("formManifest");
	if (pStrFormManifest != NULL)
	{
		m_strFormManifest = std::string(pStrFormManifest);
	}
#endif

	ZTools::UTF8ToMB(m_strFileIdListJson);
	ZTools::UTF8ToMB(m_strMainFileId);
	ZTools::UTF8ToMB(m_strThisFileMd5);

	ZTools::UTF8ToMB(m_strComponentFileName);
	ZTools::UTF8ToMB(m_strComponentFileId);
	ZTools::UTF8ToMB(m_strComponentManifest);
	ZTools::UTF8ToMB(m_strFormFileName);
	ZTools::UTF8ToMB(m_strFormFileId);
	ZTools::UTF8ToMB(m_strFormManifest);
		
	return TRUE;
}

BOOL CSpecialZipCache::WriteCache()
{
	if (m_strFileIdListJson.empty()
		//|| m_strMainFileId.empty()
		|| m_strThisFileMd5.empty())
	{
		return FALSE;
	}

	Json::Value jsonAllFiles;
	Json::Reader reader;
	if (!reader.parse(m_strFileIdListJson, jsonAllFiles))
	{
		return FALSE;
	}

	Json::Value::Members JMembers =  jsonAllFiles.getMemberNames();	
	std::string strKey;
	std::string strValue;
	for(size_t j = 0; j < JMembers.size(); j++)
	{
		strKey = JMembers[j];
		strValue = jsonAllFiles[strKey].isString() ? jsonAllFiles[strKey].asString() : "";
		if (strValue.empty())
		{
			return FALSE;
		}
	}

	//写专用缓存
	/*
	拼路径
	创建XML
	*/
	std::string strInfoXmlPath;
	strInfoXmlPath = ZTools::FormatString("%s%s_%s\\info.xml", GetSpacialCacheBasePath().c_str(), m_strThisFileMd5.c_str(), m_strThisFileSize.c_str());

	if (!MakeSureDirectoryPathExists(strInfoXmlPath.c_str()))
	{
		return FALSE;
	}

	TiXmlBase::SetCondenseWhiteSpace(false);
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "utf-8", "" );
	doc.LinkEndChild( decl ); 
	TiXmlElement* root = new TiXmlElement("file");

	root->SetAttribute("fileIdListJson", ZTools::MBToUTF8(ZTools::FormatString("%s", m_strFileIdListJson.c_str())).c_str());
	root->SetAttribute("mainFileId", ZTools::MBToUTF8(ZTools::FormatString("%s", m_strMainFileId.c_str())).c_str());
	root->SetAttribute("thisFileMd5", ZTools::MBToUTF8(ZTools::FormatString("%s", m_strThisFileMd5.c_str())).c_str());

	root->SetAttribute("componentFileName", ZTools::MBToUTF8(ZTools::FormatString("%s", m_strComponentFileName.c_str())).c_str());
	root->SetAttribute("componentFileId", ZTools::MBToUTF8(ZTools::FormatString("%s", m_strComponentFileId.c_str())).c_str());
	root->SetAttribute("componentManifest", ZTools::MBToUTF8(ZTools::FormatString("%s", m_strComponentManifest.c_str())).c_str());
	root->SetAttribute("formFileName", ZTools::MBToUTF8(ZTools::FormatString("%s", m_strFormFileName.c_str())).c_str());
	root->SetAttribute("formFileId", ZTools::MBToUTF8(ZTools::FormatString("%s", m_strFormFileId.c_str())).c_str());
	root->SetAttribute("formManifest", ZTools::MBToUTF8(ZTools::FormatString("%s", m_strFormManifest.c_str())).c_str());

	doc.LinkEndChild(root);
	doc.SaveFile(strInfoXmlPath);	

	return TRUE;
}

CSpecialZipCache::CSpecialZipCache( void )
{
	m_strThisFileMd5.clear();
	m_strFileIdListJson.clear();
	m_strMainFileId.clear();
	//m_strThisFileId.clear();
	m_strThisFileSize.clear();

	m_strComponentFileName.clear();
	m_strComponentFileId.clear();
	m_strComponentManifest.clear();
	m_strFormFileName.clear();
	m_strFormFileId.clear();
	m_strFormManifest.clear();
}

CSpecialZipCache::~CSpecialZipCache( void )
{
	Clear();
}

CDataQueryIn::CDataQueryIn( void )
{
	m_strIndex.clear();
}

CDataQueryIn::~CDataQueryIn( void )
{
	Clear();
}

void CDataQueryIn::Clear()
{
}

BOOL CDataQueryIn::Init( BSTR bstrJson )
{
	if (bstrJson == NULL)
	{
		return FALSE;
	}

	std::string strJson = _bstr_t(bstrJson);
	ZTools::WriteZToolsFormatLog("strJson:%s", strJson.c_str());

	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(strJson, root))
	{
		return FALSE;
	}

	m_strIndex = root["index"].asString();

	return TRUE;
}

CDataCancelIn::CDataCancelIn( void )
{
	m_strIndex.clear();
}

CDataCancelIn::~CDataCancelIn( void )
{
	Clear();
}

void CDataCancelIn::Clear()
{
}

BOOL CDataCancelIn::Init( BSTR bstrJson )
{
	if (bstrJson == NULL)
	{
		return FALSE;
	}

	std::string strJson = _bstr_t(bstrJson);
	ZTools::WriteZToolsFormatLog("strJson:%s", strJson.c_str());

	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(strJson, root))
	{
		return FALSE;
	}

	m_strIndex = root["index"].asString();

	return TRUE;
}

CDataUpladJob::CDataUpladJob()
{
	m_bStarted = FALSE;
	m_bFinished = FALSE;
	m_strJsonKey.clear();
}
