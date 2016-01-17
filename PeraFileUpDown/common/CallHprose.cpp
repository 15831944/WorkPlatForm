#include "StdAfx.h"
#include "CallHprose.h"
#include "Tools.h"
#include <sys/stat.h>
#include <sstream>
#include "hprose/httpclient.hpp"
#include <afxinet.h>
#include "DownUploadCommonFunc.h"
#include "DownUploadFileCacheFinder.h"
#include "ConvertData.h"

CallHprose::CallHprose(void)
{
}


CallHprose::~CallHprose(void)
{
}

std::string CallHprose::UploadUseCache(CDownUploadDataFileObject* pFileObject, bool bMove/* = false*/)
{
	if (CDownUploadFileCacheFinder::ReadUploadCache(pFileObject))
	{
		if (!pFileObject->m_strFileId.empty())
		{
			ZTools::WriteZToolsFormatLog("上传使用本地缓存中的记录");

			//pFileObject->m_strFileId = m_strFileId;
			pFileObject->m_nFileSizeUploaded = pFileObject->m_nFileSize;
			return pFileObject->m_strFileId;
		}
	}	

	std::string strFileId = Upload(pFileObject);

	if (!strFileId.empty())
	{
		CDownUploadFileCacheFinder::WriteUploadCache(pFileObject);
		CDownUploadFileCacheFinder::WriteDownloadCacheInUpload(pFileObject, bMove);
	}

	return strFileId;
}

std::string CallHprose::Upload(CDownUploadDataFileObject* pFileObject)
{
	if (pFileObject->CheckInfo() == FALSE)
	{
		return "";
	}

	ZTools::WriteZToolsFormatLog("单个文件上传前");
	
	m_strFileId = "";
	
	DataFiles oFiles;
	DataFile oFile;
	oFile.name = pFileObject->m_strFileName;
	oFile.size = ZTools::FormatString("%ld", pFileObject->m_nFileSize);
	oFile.lastModified = pFileObject->m_strLastModifyTime;
	oFile.md5 = pFileObject->m_strMd5;
	oFile.index = pFileObject->m_strIndex;
	oFiles.files.push_back(oFile);

	std::vector<hprose::any> vecAny;
	vecAny.clear();
	vecAny.push_back(oFiles.ToJson());
	if (!init(pFileObject->m_strHproseRpc, vecAny))
	{
		pFileObject->m_strMsg = m_strErrMsg;
		ZTools::WriteZToolsLog(pFileObject->m_strMsg);
		return "";
	}

	//文件大小为0或者文件MD5在数据库中找到重复的，init方法会直接返回fileId,不用走cwrite逻辑
	if (!m_strFileId.empty())
	{
		ZTools::WriteZToolsFormatLog("单个文件秒传完成");

		pFileObject->m_strFileId = m_strFileId;
		pFileObject->m_nFileSizeUploaded = pFileObject->m_nFileSize;

		return pFileObject->m_strFileId;
	}


	std::wstring wstrFilePath = s2ws_1(pFileObject->m_strFilePath);
	std::ifstream fIn(wstrFilePath, std::ios::binary);
	if (fIn == NULL || fIn.is_open() == false)
	{
		pFileObject->m_strMsg = ZTools::FormatString("read file error: %s", pFileObject->m_strFilePath.c_str());
		ZTools::WriteZToolsLog(pFileObject->m_strMsg);
		return "";
	}

	int nBlockSize = 256 * 1024;
	std::vector<char> vchar;
	vchar.resize(nBlockSize, '\0');
	std::streamsize nBlockRead = 0;

	pFileObject->m_nFileSizeUploaded = 0;

	while (true)
	{
		nBlockRead = fIn.read(reinterpret_cast<char*>(&vchar[0]), 256*1024).gcount();
		if (nBlockRead == 0)
		{
			break;
		}

		vecAny.clear();
		vecAny.push_back(oFile.index);
		vecAny.push_back(vchar);
		vecAny.push_back(nBlockRead);

		if (!cwrite(pFileObject->m_strHproseRpc, vecAny))
		{
			pFileObject->m_strMsg = m_strErrMsg;
			ZTools::WriteZToolsLog(pFileObject->m_strMsg);
			return "";
		}

		pFileObject->m_nFileSizeUploaded += nBlockRead;
	}	

	if (m_strFileId.empty())
	{
		pFileObject->m_strMsg = "unkown error";
		ZTools::WriteZToolsLog(pFileObject->m_strMsg);
		return "";
	}

	ZTools::WriteZToolsFormatLog("单个文件上传完成");

	pFileObject->m_strFileId = m_strFileId;
	pFileObject->m_nFileSizeUploaded = pFileObject->m_nFileSize;

	return pFileObject->m_strFileId;	
}

std::string CallHprose::Upload( std::string strHproseRpc, std::string strFilePath )
{
	m_strFileId = "";
	DataInterfaceReturn oReturn;

	CDownUploadDataFileObject dataFileObject;
	dataFileObject.m_strHproseRpc = strHproseRpc;
	dataFileObject.m_strFilePath = strFilePath;
	Upload(&dataFileObject);

	if (dataFileObject.m_strFileId.empty())
	{
		oReturn.error = "1";
		oReturn.msg = ZTools::FormatString("error msg: %s", dataFileObject.m_strMsg.c_str());
	}
	else
	{
		oReturn.error = "0";
		oReturn.file = m_strFileId;
	}

	return oReturn.ToJson();
}

//该接口成功回空字符串，失败返回错误详细信息
BOOL CallHprose::init(std::string strUrl, std::vector<hprose::any> vecAny)
{
	std::string strReturn;
	//std::locale old_loc;
	try
	{
		//old_loc = std::locale::global(std::locale("C"));
		hprose::httpclient hc(strUrl);
		hc.invoke(strReturn, "init", vecAny);
		//std::locale::global(old_loc);

		DataInitReturn oInitReturn;
		oInitReturn.FromJson(strReturn);

		if (oInitReturn.flag.compare("0") == 0)
		{
			return TRUE;
		}
		else if (oInitReturn.flag.compare("1") == 0)
		{
			m_strFileId = oInitReturn.fileID;
			if (m_strFileId.empty())
			{
				m_strErrMsg = ZTools::FormatString("init error: flag is 1, but fileID is NULL");
				ZTools::WriteZToolsLog(m_strErrMsg);
				return FALSE;
			}
			else
			{
				return TRUE;
			}
		}
		else
		{
			m_strErrMsg = ZTools::FormatString("init error: flag is not 0 or 1");
			ZTools::WriteZToolsLog(m_strErrMsg);
			return FALSE;
		}
	}
	catch (hprose::exception e)
	{
		//std::locale::global(old_loc);
		m_strErrMsg = ZTools::FormatString("init error: %s", e.what());
		ZTools::WriteZToolsLog(m_strErrMsg);
		return FALSE;
	}
	catch (...)
	{

	}
	return TRUE;
}

//该接口成功回空字符串，失败返回错误详细信息
BOOL CallHprose::cwrite(std::string strUrl, std::vector<hprose::any> vecAny)
{
	std::string strReturn;
	//std::locale old_loc;	
	try
	{
		//old_loc = std::locale::global(std::locale("C"));
		hprose::httpclient hc(strUrl);
		hc.invoke(strReturn, "cwrite", vecAny);
		//std::locale::global(old_loc);

		DataCwriteReturn oCwriteReturn;
		oCwriteReturn.FromJson(strReturn);
		if (oCwriteReturn.error.compare("0") != 0)
		{
			m_strErrMsg = ZTools::FormatString("cwrite error: %s", oCwriteReturn.msg.c_str());
			ZTools::WriteZToolsLog(m_strErrMsg);
			return FALSE;
		}

		if (!oCwriteReturn.fileID.empty())
		{
			m_strFileId = oCwriteReturn.fileID;
			m_strFileSize = oCwriteReturn.end;
		}
	}
	catch (hprose::exception e)
	{
		//std::locale::global(old_loc);
		strReturn = ZTools::FormatString("cwrite error: %s", e.what());
		ZTools::WriteZToolsLog(strReturn);
		return FALSE;
	}
	catch (...)
	{

	}
	return TRUE;
}
char dec2hexChar(short int n) {
	if ( 0 <= n && n <= 9 ) {
		return char( short('0') + n );
	} else if ( 10 <= n && n <= 15 ) {
		return char( short('A') + n - 10 );
	} else {
		return char(0);
	}
}

short int hexChar2dec(char c) {
	if ( '0'<=c && c<='9' ) {
		return short(c-'0');
	} else if ( 'a'<=c && c<='f' ) {
		return ( short(c-'a') + 10 );
	} else if ( 'A'<=c && c<='F' ) {
		return ( short(c-'A') + 10 );
	} else {
		return -1;
	}
}

std::string escapeURL(const std::string &URL)
{
	std::string result = "";
	for ( unsigned int i=0; i<URL.size(); i++ ) {
		char c = URL[i];
		if (
			( '0'<=c && c<='9' ) ||
			( 'a'<=c && c<='z' ) ||
			( 'A'<=c && c<='Z' ) ||
			c=='/' || c=='.'
			) {
				result += c;
		} else {
			int j = (short int)c;
			if ( j < 0 ) {
				j += 256;
			}
			int i1, i0;
			i1 = j / 16;
			i0 = j - i1*16;
			result += '%';
			result += dec2hexChar(i1);
			result += dec2hexChar(i0);
		}
	}
	return result;
}

std::string deescapeURL(const std::string &URL) {
	std::string result = "";
	for ( unsigned int i=0; i<URL.size(); i++ ) {
		char c = URL[i];
		if ( c != '%' ) {
			result += c;
		} else {
			char c1 = URL[++i];
			char c0 = URL[++i];
			int num = 0;
			num += hexChar2dec(c1) * 16 + hexChar2dec(c0);
			result += char(num);
		}
	}
	return result;
}
std::string CallHprose::DownloadHprose( std::string strHproseDownloadUrl, std::string strFileDir, std::string strFileName )
{
	CInternetSession sess;
	CHttpFile* pHttpFile;
	try
	{
		pHttpFile=(CHttpFile*)sess.OpenURL(strHproseDownloadUrl.c_str());
	}
	catch(CException* e)
	{
		pHttpFile = 0;
		TCHAR msg[1024];
		memset(msg, 0, 1024);
		e->GetErrorMessage(msg, 1023, NULL);
		ZTools::WriteZToolsFormatLog("打开URL失败：%s", msg);
		return "";
	}

	DWORD dwStatus;
	DWORD dwBuffLen = sizeof(dwStatus);
	BOOL bSuccess = pHttpFile->QueryInfo(HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER, &dwStatus, &dwBuffLen);

	int nReadBlockSize = 256*1024;
	char* pBuffer = new char[nReadBlockSize];
	if( bSuccess && dwStatus>= 200&& dwStatus<300 )
	{
		if (strFileName.length() == 0)
		{
			CString sContentDisposition;
			DWORD dwIndex = 0;
			bSuccess = pHttpFile->QueryInfo(HTTP_QUERY_CONTENT_DISPOSITION, sContentDisposition, &dwIndex);
			std::string strContentDisposition((LPCTSTR)sContentDisposition);
			std::string strKeyBegin("filename=\"");
			std::string strKeyEnd("\"");

			if (!strContentDisposition.empty())
			{
				size_t nBegin = strContentDisposition.find(strKeyBegin) + strKeyBegin.length();
				if (nBegin < strContentDisposition.length())
				{
					size_t nEnd = strContentDisposition.find(strKeyEnd, nBegin);
					strFileName = strContentDisposition.substr(nBegin, nEnd - nBegin);
				}
				strFileName = deescapeURL(strFileName);
				ZTools::UTF8ToMB(strFileName);
			}
		}

		if (strFileName.length() == 0)
		{
			delete[] pBuffer;
			pBuffer = NULL;
			pHttpFile->Close();
			//delete pHttpFile;
			sess.Close();
			ZTools::WriteZToolsFormatLog("没有找到文件名");
			return "";
		}

		std::replace(strFileDir.begin(), strFileDir.end(), '/', '\\');
		strFileDir.erase(strFileDir.find_last_not_of('\\') + 1);
		std::replace(strFileName.begin(), strFileName.end(), '/', '\\');
		strFileName.erase(0, strFileName.find_first_not_of('\\'));
		std::string strFilePath = ZTools::FormatString("%s\\%s", strFileDir.c_str(), strFileName.c_str());
		//先下载到临时文件中，下载成功后重命名
		std::string strFilePathTemp = ZTools::FormatString("%s\\%s", strFileDir.c_str(), GuidToString(CreateGuid()).c_str());

		if (!MakeSureDirectoryPathExists(strFilePath.c_str()))
		{
			delete[] pBuffer;
			pBuffer = NULL;
			pHttpFile->Close();
			//delete pHttpFile;
			sess.Close();
			ZTools::WriteZToolsFormatLog("创建文件夹失败：%s", strFilePath.c_str());
			return "";
		}

		CFile fileWrite;
		ULONGLONG dwFileLen = 0;
		DWORD dwWriteIndex = 0;
		dwFileLen = pHttpFile->SeekToEnd();
		pHttpFile->SeekToBegin();
		if(fileWrite.Open(strFilePathTemp.c_str(), CFile::modeWrite|CFile::modeCreate))
		{
			int nCount = 0;
			while(dwWriteIndex < dwFileLen)
			{
				nCount = pHttpFile->Read(pBuffer, nReadBlockSize);
				fileWrite.Write(pBuffer, nCount);
				dwWriteIndex += nCount;                
			}
			fileWrite.Close();

			rename(strFilePathTemp.c_str(), strFilePath.c_str());

			delete[] pBuffer;
			pBuffer = NULL;
			pHttpFile->Close();
			delete pHttpFile;
			sess.Close();

			if (PathFileExists(strFilePath.c_str()))
			{
				return strFilePath;
			}
			else
			{
				return "";
			}
		}
		else
		{
			delete[] pBuffer;
			pBuffer = NULL;
			pHttpFile->Close();
			//delete pHttpFile;
			sess.Close();
			ZTools::WriteZToolsFormatLog("本地文件%s打开出错", strFilePath.c_str());
			return "";
		}
	}
	else
	{
		delete[] pBuffer;
		pBuffer = NULL;
		pHttpFile->Close();
		//delete pHttpFile;
		sess.Close();
		ZTools::WriteZToolsFormatLog("打开网页文件出错，错误码：%d", dwStatus);
		return "";
	}

	delete[] pBuffer;
	pBuffer = NULL;
	pHttpFile->Close();
	//delete pHttpFile;
	sess.Close();	
	return "";
}

std::string CallHprose::UploadHprose( std::string strHproseRpc, std::string strFilePath )
{
	std::string strJson = Upload(strHproseRpc, strFilePath);
	DataInterfaceReturn oReturn;
	oReturn.FromJson(strJson);
	return oReturn.file;
}

void DataInterfaceReturn::Clear()
{
	error.clear();
	msg.clear();
	file.clear();
}

void DataInterfaceReturn::FromJson( std::string strJson )
{
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(strJson, root))
	{
		error = root["error"].asString();
		msg = root["msg"].asString();
		file = root["file"].asString();
	}
}

std::string DataInterfaceReturn::ToJson()
{
	Json::FastWriter writer;
	Json::Value root;
	root["error"] = Json::Value(error);
	root["msg"] = Json::Value(msg);
	root["file"] = Json::Value(file);
	return writer.write(root);
}

void DataFile::Clear()
{
	lastModified.clear();
	name.clear(); 
	size.clear();
	index.clear();
	md5.clear();
}

void DataFiles::Clear()
{
	files.clear();
}

void DataFiles::FromJson( std::string strJson )
{

}

std::string DataFiles::ToJson()
{
	Json::FastWriter writer;
	Json::Value root;
	Json::Value jFiles(Json::arrayValue);
	for (std::vector<DataFile>::iterator it = files.begin(); it != files.end(); ++it)
	{
		Json::Value jFile;
		jFile["lastModified"] = Json::Value(it->lastModified);
		jFile["name"] = Json::Value(it->name);
		jFile["size"] = Json::Value(it->size);
		jFile["index"] = Json::Value(it->index);
		jFile["md5"] = Json::Value(it->md5);
		jFiles.append(jFile);
	}
	root["files"] = jFiles;
	return writer.write(root);
}

void DataInitReturn::FromJson( std::string strJson )
{
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(strJson, root))
	{
		int nLength = root.size();
		//ZTools::WriteZToolsFormatLog("崩掉的JSON字串：%s", strJson.c_str());
		if (nLength > 0)
		{
			Json::Value file = root[(Json::Value::ArrayIndex)0];
			index = file["index"].asString();
			curentSize = file["curentSize"].asString();
			flag = file["flag"].asString();
			fileID = file["fileID"].asString();
		}
	}
}

void DataCwriteReturn::FromJson( std::string strJson )
{
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(strJson, root))
	{
		error = root["error"].asString();
		fileID = root["fileID"].asString();
		fileName = root["fileName"].asString();
		end = root["end"].asString();
		msg = root["msg"].asString();
	}
	else
	{
		msg = strJson;
	}
}
