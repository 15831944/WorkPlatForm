// FileUpDown.cpp : CFileUpDown 的实现

#include "stdafx.h"
#include "FileUpDown.h"
#include "CallHprose.h"
#include "Tools.h"
#include <comdef.h>
#include "GetFlexwareBin.h"
#include <sys/stat.h>
#include "IniClass.h"
#include "DownUploadFileCacheFinder.h"
#include "DownUploadCommonFunc.h"
#include "ConfigFunc.h"

// CFileUpDown

std::string CFileUpDown::GetHproseRpc()
{
	CString sUrl = GetConfig("hprose", "hproseRpc");
	return std::string((LPCTSTR)sUrl);
}

std::string CFileUpDown::GetHproseDownloadUrl()
{
	CString sUrl = GetConfig("hprose", "hproseDownloadUrl");
	return std::string((LPCTSTR)sUrl);
}

STDMETHODIMP CFileUpDown::Upload(BSTR bstrHprose, BSTR bstrFilePath, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ZTools::WriteZToolsLog(">>>>>>PeraFileUpDown::CFileUpDown::Upload");
	CComVariant vRet(_bstr_t("").GetBSTR());

	std::string strHprose = (bstrHprose != NULL) ? _bstr_t(bstrHprose) : "";
	if (strHprose.empty())
	{
		strHprose = GetHproseRpc();
	}
	ZTools::WriteZToolsFormatLog("strHprose:%s", strHprose.c_str());
	std::string strFilePath = (bstrFilePath != NULL) ? _bstr_t(bstrFilePath) : "";
	ZTools::WriteZToolsFormatLog("strFilePath:%s", strFilePath.c_str());
	
	CallHprose ch;
	std::string strReturn = ch.Upload(strHprose, strFilePath);
	
	vRet = _bstr_t(strReturn.c_str()).GetBSTR();
	vRet.Detach(pvRet);

	return S_OK;
}


STDMETHODIMP CFileUpDown::Download(BSTR bstrHprose, BSTR bstrDir, BSTR bstrFileName, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	ZTools::WriteZToolsLog(">>>>>>PeraFileUpDown::CFileUpDown::Download");
	CComVariant vRet(_bstr_t("").GetBSTR());
	
	std::string strHprose = (bstrHprose != NULL) ? _bstr_t(bstrHprose) : "";
	ZTools::WriteZToolsFormatLog("strHprose:%s", strHprose.c_str());
	std::string strDir = (bstrDir != NULL) ? _bstr_t(bstrDir) : "";
	ZTools::WriteZToolsFormatLog("strDir:%s", strDir.c_str());
	std::string strFileName = (bstrFileName != NULL) ? _bstr_t(bstrFileName) : "";
	ZTools::WriteZToolsFormatLog("strFileName:%s", strFileName.c_str());

	CallHprose ch;
	std::string strReturn = ch.DownloadHprose(strHprose, strDir, strFileName);

	vRet = _bstr_t(strReturn.c_str()).GetBSTR();
	vRet.Detach(pvRet);

	return S_OK;
}

//获取文件长度
long GetFileSize(const char *filename)
{
	struct stat     f_stat;
	if (stat(filename, &f_stat) == -1) {
		return -1;
	}
	return (long)f_stat.st_size;
}
//获取上传文件info.xml缓存信息中FileID
std::string CFileUpDown::LoadFileID(std::string strUrl, std::string strFilePath)
{
	std::string strRet;

	long nFileSize = GetFileSize(strFilePath.c_str());
	if(nFileSize == -1)
	{
		return strRet;
	}

	std::string strFileMD5 = GetFileMd5(strFilePath); //MD5
	ZTools::WriteZToolsFormatLog("file Md5: %s", strFileMD5.c_str());
	std::string strFileName = GetFileName(strFilePath);

// 	std::ostringstream oss;
// 	oss << nFileSize;
// 
// 	std::string strFileSize = oss.str(); //文件大小

	CString str;
	str.Format("%d", nFileSize);
	std::string strFileSize = str;

	//本地缓存路径
	CString strCacheRoot =  GetConfig("Pera","RunCache");
	if (strCacheRoot.IsEmpty())
		strCacheRoot = "C:\\WorkSpace\\Data";
	//本地缓存+fileid
	std::string strFileCacheBaseDir;

	{//文件路径计算
		strFileCacheBaseDir = ZTools::FormatString("%s\\DmsFileCache\\Upload\\%s\\", strCacheRoot, strUrl.c_str());
	}

	std::string strInfoDir = ZTools::FormatString("%s%s_%s_%s\\", strFileCacheBaseDir.c_str(), strFileName.c_str(), strFileMD5.c_str(), strFileSize.c_str());

	std::string strFileInfoXmlPath;
	strFileInfoXmlPath = ZTools::FormatString("%sinfo.xml", strInfoDir.c_str());

	std::string strFileNameInfo; //名字
	std::string strFileMD5Info; //MD5
	std::string strFileIdInfo; //ID
	int nFileSizeInfo = 0; //大小

	if (PathFileExists(strFileInfoXmlPath.c_str()))
	{
		TiXmlBase::SetCondenseWhiteSpace(false);
		TiXmlDocument doc;
		doc.LoadFile(strFileInfoXmlPath, TIXML_ENCODING_UTF8);

		TiXmlElement* rootElement = doc.RootElement();
		if (rootElement)
		{
// 			rootElement->QueryStringAttribute("fileName", &strFileNameInfo);
// 			ZTools::UTF8ToMB(strFileNameInfo);
			rootElement->QueryIntAttribute("fileSize", &nFileSizeInfo);
			rootElement->QueryStringAttribute("fileID", &strFileIdInfo);
			rootElement->QueryStringAttribute("fileMD5", &strFileMD5Info);

			if (nFileSizeInfo == nFileSize && strFileMD5Info == strFileMD5)
			{
				return strFileIdInfo;
			}
		}
	}

	return strRet;
}

//获取上传文件info.xml缓存信息中FileID
BOOL CFileUpDown::SaveFileID(std::string strUrl, std::string strFilePath, std::string strFileId)
{
	std::string strRet;

	long nFileSize = GetFileSize(strFilePath.c_str());
	if(nFileSize == -1)
	{
		return FALSE;
	}

	std::string strFileMD5 = GetFileMd5(strFilePath); //MD5
	std::string strFileName = GetFileName(strFilePath);

	// 	std::ostringstream oss;
	// 	oss << nFileSize;
	// 
	// 	std::string strFileSize = oss.str(); //文件大小

	CString str;
	str.Format("%d", nFileSize);
	std::string strFileSize = str;

	//本地缓存路径
	CString strCacheRoot =  GetConfig("Pera","RunCache");
	if (strCacheRoot.IsEmpty())
		strCacheRoot = "C:\\WorkSpace\\Data";

	//本地缓存+fileid
	std::string strFileCacheBaseDir;

	{//文件路径计算
		strFileCacheBaseDir = ZTools::FormatString("%s\\DmsFileCache\\Upload\\%s\\", strCacheRoot, strUrl.c_str());
	}

	std::string strInfoDir = ZTools::FormatString("%s%s_%s_%s\\", strFileCacheBaseDir.c_str(), strFileName.c_str(), strFileMD5.c_str(), strFileSize.c_str());

	std::string strFileInfoXmlPath;
	strFileInfoXmlPath = ZTools::FormatString("%sinfo.xml", strInfoDir.c_str());

	if (MakeSureDirectoryPathExists(strFileInfoXmlPath.c_str()))
	{//创建目录
		//记录信息
		TiXmlBase::SetCondenseWhiteSpace(false);
		TiXmlDocument doc;
		TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "utf-8", "" );
		doc.LinkEndChild( decl ); 
		TiXmlElement* root = new TiXmlElement("file");
		root->SetAttribute("fileMD5", ZTools::MBToUTF8(strFileMD5));
		root->SetAttribute("fileSize", ZTools::MBToUTF8(strFileSize));
		root->SetAttribute("fileID", ZTools::MBToUTF8(strFileId));
		doc.LinkEndChild(root);
		doc.SaveFile(strFileInfoXmlPath);
	}

	return TRUE;
}

//上传文件，并备份到本地文件缓存中
//bstrHprose,dms提供的服务的地址
//bstrFilePath,要上传的文件路径
//bstrBoolMove,指定备份时移动还是复制，移动的话效率会高一些，参数值应为true或false，默认为false，执行复制
//返回值为fileid，如为空，表示失败
STDMETHODIMP CFileUpDown::Upload2( BSTR bstrHprose, BSTR bstrFilePath, BSTR bstrBoolMove, VARIANT* pvRet )
{
	ZTools::WriteZToolsLog(">>>>>>PeraFileUpDown::CFileUpDown::Upload2");
	CComVariant vRet(_bstr_t("").GetBSTR());

	std::string strHprose = (bstrHprose != NULL) ? _bstr_t(bstrHprose) : "";
	ZTools::WriteZToolsFormatLog("strHprose:%s", strHprose.c_str());
	std::string strFilePath = (bstrFilePath != NULL) ? _bstr_t(bstrFilePath) : "";
	ZTools::WriteZToolsFormatLog("strFilePath:%s", strFilePath.c_str());
	std::string strBoolMove = (bstrBoolMove != NULL) ? _bstr_t(bstrBoolMove) : "";
	ZTools::WriteZToolsFormatLog("strBoolMove:%s", strBoolMove.c_str());

	//保存到本地文件缓存
	bool bMove = false;
	ZTools::LowerString(strBoolMove);
	if (strBoolMove.compare("true") == 0)
	{
		bMove = true;
	}

	CDownUploadDataFileObject dataFileObject;
	dataFileObject.m_strFilePath = strFilePath;
	dataFileObject.m_strHproseRpc = strHprose;

	CallHprose ch;
	ch.UploadUseCache(&dataFileObject, bMove);

	if (dataFileObject.m_strFileId.empty())
	{
		ZTools::WriteZToolsFormatLog("上传失败: %s", dataFileObject.m_strMsg.c_str());
	}

	ZTools::WriteZToolsFormatLog("finish,fileid: %s", dataFileObject.m_strFileId.c_str());
	vRet = _bstr_t(dataFileObject.m_strFileId.c_str()).GetBSTR();
	vRet.Detach(pvRet);

	return S_OK;
}

//上传文件，并备份到本地文件缓存中
//bstrHprose,dms提供的服务的地址
//bstrFilePath,要上传的文件路径
//bstrFileMd5,要上传的文件的md5值
//bstrBoolMove,指定备份时移动还是复制，移动的话效率会高一些，参数值应为true或false，默认为false，执行复制
//返回值为fileid，如为空，表示失败
STDMETHODIMP CFileUpDown::Upload3(BSTR bstrHprose, BSTR bstrFilePath, BSTR bstrFileMd5, BSTR bstrBoolMove, VARIANT* pvRet)
{
	ZTools::WriteZToolsLog(">>>>>>PeraFileUpDown::CFileUpDown::Upload3");
	CComVariant vRet(_bstr_t("").GetBSTR());

	std::string strHprose = (bstrHprose != NULL) ? _bstr_t(bstrHprose) : "";
	ZTools::WriteZToolsFormatLog("strHprose:%s", strHprose.c_str());
	std::string strFilePath = (bstrFilePath != NULL) ? _bstr_t(bstrFilePath) : "";
	ZTools::WriteZToolsFormatLog("strFilePath:%s", strFilePath.c_str());
	std::string strFileMd5 = (bstrFileMd5 != NULL) ? _bstr_t(bstrFileMd5) : "";
	ZTools::WriteZToolsFormatLog("strFileMd5:%s", strFileMd5.c_str());
	std::string strBoolMove = (bstrBoolMove != NULL) ? _bstr_t(bstrBoolMove) : "";
	ZTools::WriteZToolsFormatLog("strBoolMove:%s", strBoolMove.c_str());

	//保存到本地文件缓存
	bool bMove = false;
	ZTools::LowerString(strBoolMove);
	if (strBoolMove.compare("true") == 0)
	{
		bMove = true;
	}

	CDownUploadDataFileObject dataFileObject;
	dataFileObject.m_strFilePath = strFilePath;
	dataFileObject.m_strHproseRpc = strHprose;
	dataFileObject.m_strMd5 = strFileMd5;

	CallHprose ch;
	ch.UploadUseCache(&dataFileObject, bMove);

	if (dataFileObject.m_strFileId.empty())
	{
		ZTools::WriteZToolsFormatLog("上传失败: %s", dataFileObject.m_strMsg.c_str());
	}

	ZTools::WriteZToolsFormatLog("finish,fileid: %s", dataFileObject.m_strFileId.c_str());
	vRet = _bstr_t(dataFileObject.m_strFileId.c_str()).GetBSTR();
	vRet.Detach(pvRet);

	return S_OK;
}

//下载文件，先从本地文件缓存中找
//bstrHproseBaseUrl,要下载的文件的基础URL，不带fileid
//bstrFileId,要下载的文件的fileid
//bstrFileName,指定的文件名，如从文件缓存中有，则该参数不起作用，只有真正下载时用到，如为空，则从URL的头信息中取文件名，如还没有，则下载失败
//返回值为字符串，空表示失败，成功的话，返回值为下载后的文件完整路径，该路径是在文件缓存中的文件路径，调用者需根据需要来确定是否复制
STDMETHODIMP CFileUpDown::Download2( BSTR bstrHproseBaseUrl, BSTR bstrFileId, BSTR bstrFileName, VARIANT* pvRet )
{
	ZTools::WriteZToolsLog(">>>>>>PeraFileUpDown::CFileUpDown::Download2");
	CComVariant vRet(_bstr_t("").GetBSTR());

	std::string strHproseBaseUrl = (bstrHproseBaseUrl != NULL) ? _bstr_t(bstrHproseBaseUrl) : "";
	if (strHproseBaseUrl.empty())
	{
		strHproseBaseUrl = GetHproseDownloadUrl();
	}
	ZTools::WriteZToolsFormatLog("strHproseBaseUrl:%s", strHproseBaseUrl.c_str());
	std::string strFileId = (bstrFileId != NULL) ? _bstr_t(bstrFileId) : "";
	ZTools::WriteZToolsFormatLog("strFileId:%s", strFileId.c_str());
	std::string strFileName = (bstrFileName != NULL) ? _bstr_t(bstrFileName) : "";
	ZTools::WriteZToolsFormatLog("strFileName:%s", strFileName.c_str());

	CDownUploadDataFileObject dataFileObject;
	dataFileObject.m_strHproseRpc = strHproseBaseUrl;
	dataFileObject.m_strFileId = strFileId;

	if (strFileId.empty())
	{
		vRet = "";
		vRet.Detach(pvRet);
		return S_OK;
	}

	if (CDownUploadFileCacheFinder::ReadDownloadCache(&dataFileObject))
	{
		vRet = _bstr_t(dataFileObject.m_strFilePath.c_str()).GetBSTR();
		vRet.Detach(pvRet);
		return S_OK;
	}

	//同一个fileid在一个时刻只有一个真实下载
	CString sMutexName;
	sMutexName.Format("download_hprose_fileid_%s", strFileId.c_str());
	HANDLE hMutex = CreateMutex(NULL, TRUE, sMutexName);
	WaitForSingleObject(hMutex, INFINITE);
	//得到下载互斥后，再检查一遍缓存中是否已有下载
	if (CDownUploadFileCacheFinder::ReadDownloadCache(&dataFileObject))
	{
		ZTools::WriteZToolsFormatLog("得到互斥资源后，再次检查缓存，文件已下载，%s", strFileId.c_str());
		CloseHandle(hMutex);
		vRet = _bstr_t(dataFileObject.m_strFilePath.c_str()).GetBSTR();
		vRet.Detach(pvRet);
		return S_OK;
	}
	else
	{
		ZTools::WriteZToolsFormatLog("得到互斥资源后，再次检查缓存，文件仍未下载，开始下载，%s", strFileId.c_str());
	}
	
	std::string strCacheBasePath = GetDownloadCacheBasePath();
	if (strCacheBasePath.find("\\default\\") != std::string::npos && !dataFileObject.m_strHproseRpc.empty())
	{
		strCacheBasePath.resize(strCacheBasePath.length() - 8);
		strCacheBasePath += FormatUrl(dataFileObject.m_strHproseRpc);
		strCacheBasePath += "\\";
	}

	std::string strCurFileDir;
	strCurFileDir = strCacheBasePath + dataFileObject.m_strFileId + "\\";//获取缓存路径

	std::string strReturnFilePath;
	//下载
	std::string strHprose = ZTools::FormatString("%s%s", strHproseBaseUrl.c_str(), strFileId.c_str());
	CallHprose ch;
	strReturnFilePath = ch.DownloadHprose(strHprose, strCurFileDir, strFileName);

	if (!strReturnFilePath.empty() && PathFileExists(strReturnFilePath.c_str()))
	{
		//记录信息
		dataFileObject.m_strFilePath = strReturnFilePath;
		dataFileObject.CheckInfo(FALSE);
		CDownUploadFileCacheFinder::WriteDownloadCache(&dataFileObject);
	}
	else
	{
		strReturnFilePath.clear();
	}

	CloseHandle(hMutex);

	vRet = _bstr_t(strReturnFilePath.c_str()).GetBSTR();
	vRet.Detach(pvRet);

	return S_OK;
}

struct StructUpDown
{
	enum Type
	{
		TYPE_UPLOAD, //上传
		TYPE_DOWNLOAD  //下载
	};

	CFileUpDown* m_pCFUD; //操作类

	Type m_type; //类型

	std::string m_strFilePath; //文件路径（上传）
	std::string m_strHprose; //地址
	std::string m_strBoolMove; //移动标志
	std::string m_strFileID; //下载文件ID 
	std::string m_strFileDir; //下载目录
	std::string m_strFileName; //下载名字
};

static UINT RunThread( LPVOID pParam )
{
	StructUpDown* pSUD = (StructUpDown*)pParam;
	CFileUpDown* pFUD = pSUD->m_pCFUD;

	CComBSTR bstrHprose(_bstr_t(pSUD->m_strHprose.c_str()).GetBSTR());

	switch (pSUD->m_type)
	{
	case pSUD->TYPE_UPLOAD:
		{//上传
			CComBSTR bstrFilePath(_bstr_t(pSUD->m_strFilePath.c_str()).GetBSTR());
			CComBSTR bstrBoolMove(_bstr_t(pSUD->m_strBoolMove.c_str()).GetBSTR());
			CComVariant var;
			pFUD->Upload2(bstrHprose, bstrFilePath, bstrBoolMove, &var);
			CString strFileID = (LPCTSTR)_bstr_t(var);

			g_cCriticalSection.Lock();//线程同步
			//pFUD->m_Jv.append(Json::Value()[pSUD->m_strFilePath] = (Json::Value()["fileID"] = strFileID.GetBuffer()));
			Json::Value jv;
			jv["fileID"] = strFileID.GetBuffer();
			pFUD->m_Jv[pSUD->m_strFilePath] = jv;
			g_cCriticalSection.Unlock();

			break;
		}
	case pSUD->TYPE_DOWNLOAD:
		{//下载
			CComBSTR bstrFileId(_bstr_t(pSUD->m_strFileID.c_str()).GetBSTR());
			CComBSTR bstrFileName(_bstr_t(pSUD->m_strFileName.c_str()).GetBSTR());
			CComVariant var;
			pFUD->Download2(bstrHprose, bstrFileId, bstrFileName, &var);
			CString strFilePath = (LPCTSTR)_bstr_t(var);

			std::string strDstFilePath = strFilePath;
			if(!pSUD->m_strFileDir.empty())
			{//复制缓存文件到目标目录
				std::string strRefFilePath = strFilePath;
				if (pSUD->m_strFileName.empty())
				{
					pSUD->m_strFileName = strRefFilePath.substr(strRefFilePath.find_last_of('\\') + 1, strRefFilePath.length() - strRefFilePath.find_last_of('\\') - 1);
				}

				std::replace(pSUD->m_strFileDir.begin(), pSUD->m_strFileDir.end(), '/', '\\');
				pSUD->m_strFileDir.erase(pSUD->m_strFileDir.find_last_not_of('\\') + 1);
				strDstFilePath = ZTools::FormatString("%s\\%s", pSUD->m_strFileDir.c_str(), pSUD->m_strFileName.c_str());

				MakeSureDirectoryPathExists(strDstFilePath.c_str());
				CopyFile(strRefFilePath.c_str(), strDstFilePath.c_str(), FALSE);
				if (PathFileExists(strDstFilePath.c_str()))
				{
					//return strDstFilePath;
				}
				else
				{
					strDstFilePath = "";
				}
			}

			g_cCriticalSection.Lock();//线程同步
			//pFUD->m_Jv.append(Json::Value()[pSUD->m_strFilePath] = (Json::Value()["fileID"] = strFileID.GetBuffer()));
			Json::Value jv;
			jv["filePath"] = strDstFilePath;
			pFUD->m_Jv[pSUD->m_strFileID] = (jv);
			g_cCriticalSection.Unlock();

			break;
		}
	}
	
	delete pSUD;
	return 0;
}

STDMETHODIMP CFileUpDown::UploadFiles( BSTR bstrHprose, BSTR bstrJson, BSTR bstrBoolMove, VARIANT* pvRet )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加实现代码
	ZTools::WriteZToolsLog(">>>>>>PeraFileUpDown::CFileUpDown::UploadFiles");
	CComVariant vRet(_bstr_t("").GetBSTR());

	std::string strJson = (bstrJson != NULL) ? _bstr_t(bstrJson) : "";
	std::string strJsonRet;
	ZTools::WriteZToolsFormatLog("bstrJson:%s", strJson.c_str());

	Json::Reader jReader;
	Json::Value jValue;
	Json::FastWriter jWriter;

	if(!jReader.parse(strJson, jValue) && !jValue.isArray())
	{//解析失败
		vRet.Detach(pvRet);
		return S_OK;
	}

	int nCount = jValue.size(); //上传个数

	HANDLE* handles = new HANDLE[nCount]; //线程句柄
	CWinThread** threads = new CWinThread*[nCount]; //WinThread

	//m_Jv = Json::Value(Json::arrayValue);
	m_Jv = Json::Value();

	for(int i = 0; i < nCount; i++)
	{
		StructUpDown* pSUD = new StructUpDown;
		pSUD->m_type = StructUpDown::TYPE_UPLOAD;
		pSUD->m_pCFUD = this;
		pSUD->m_strFilePath = jValue[i]["filePath"].asString();
		pSUD->m_strHprose = (bstrHprose != NULL) ? _bstr_t(bstrHprose) : "";
		pSUD->m_strBoolMove = (bstrBoolMove != NULL) ? _bstr_t(bstrBoolMove) : "";

		threads[i] = AfxBeginThread(RunThread, pSUD, 0, 0, CREATE_SUSPENDED, 0);
		threads[i]->m_bAutoDelete = FALSE;
		handles[i] = threads[i]->m_hThread; //传句柄	
		threads[i]->ResumeThread();
	}

	WaitForMultipleObjects(nCount, handles, TRUE, INFINITE); //等待全部线程结束

	for(int i = 0; i < nCount; i++)
	{
		delete threads[i];
		//CloseHandle(handles[i]);
	}
	delete [] threads;
	delete [] handles;

	strJsonRet = jWriter.write(m_Jv);
	vRet = _bstr_t(strJsonRet.c_str()).GetBSTR();
	vRet.Detach(pvRet);

	return S_OK;
}


STDMETHODIMP CFileUpDown::DownloadFiles(BSTR bstrHproseBaseUrl, BSTR bstrJson, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加实现代码
	ZTools::WriteZToolsLog(">>>>>>PeraFileUpDown::CFileUpDown::DownloadFiles");
	CComVariant vRet(_bstr_t("").GetBSTR());

	std::string strJson = (bstrJson != NULL) ? _bstr_t(bstrJson) : "";
	std::string strJsonRet;
	ZTools::WriteZToolsFormatLog("bstrJson:%s", strJson.c_str());

	Json::Reader jReader;
	Json::Value jValue;
	Json::FastWriter jWriter;

	if(!jReader.parse(strJson, jValue) && !jValue.isArray())
	{//解析失败
		vRet.Detach(pvRet);
		return S_OK;
	}

	int nCount = jValue.size(); //下载个数

	HANDLE* handles = new HANDLE[nCount]; //线程句柄
	CWinThread** threads = new CWinThread*[nCount]; //WinThread

	//m_Jv = Json::Value(Json::arrayValue);
	m_Jv = Json::Value();

	for(int i = 0; i < nCount; i++)
	{
		StructUpDown* pSUD = new StructUpDown;
		pSUD->m_type = StructUpDown::TYPE_DOWNLOAD;
		pSUD->m_pCFUD = this;
		pSUD->m_strFileID = jValue[i]["fileID"].asString();
		pSUD->m_strFileDir = jValue[i]["fileDir"].asString();
		pSUD->m_strFileName = jValue[i]["fileName"].asString();
		pSUD->m_strHprose = (bstrHproseBaseUrl != NULL) ? _bstr_t(bstrHproseBaseUrl) : "";

		threads[i] = AfxBeginThread(RunThread, pSUD, 0, 0, CREATE_SUSPENDED, 0);
		threads[i]->m_bAutoDelete = FALSE;
		handles[i] = threads[i]->m_hThread; //传句柄	
		threads[i]->ResumeThread();
	}

	WaitForMultipleObjects(nCount, handles, TRUE, INFINITE); //等待全部线程结束

	for(int i = 0; i < nCount; i++)
	{
		delete threads[i];
		//CloseHandle(handles[i]);
	}
	delete [] threads;
	delete [] handles;

	strJsonRet = jWriter.write(m_Jv);
	vRet = _bstr_t(strJsonRet.c_str()).GetBSTR();
	vRet.Detach(pvRet);

	return S_OK;
}

CFileUpDown::CFileUpDown()
{
	ZTools::InitZToolsLog("PeraFileUpDown", "PeraFileUpDown.dll");
}