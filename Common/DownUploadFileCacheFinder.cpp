#include "StdAfx.h"
#include "DownUploadFileCacheFinder.h"
#include "Tools.h"
#include "DownUploadCommonFunc.h"

#include "tinyxml.h"
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")

CDownUploadFileCacheFinder::CDownUploadFileCacheFinder(void)
{
}


CDownUploadFileCacheFinder::~CDownUploadFileCacheFinder(void)
{
}

BOOL CDownUploadFileCacheFinder::ReadUploadCache( CDownUploadDataFileObject* pFileObject )
{
	if (pFileObject->CheckInfo() == FALSE)
	{
		return FALSE;
	}

	std::string strCacheBasePath = GetUploadCacheBasePath();
	if (strCacheBasePath.find("\\default\\") != std::string::npos && !pFileObject->m_strHproseRpc.empty())
	{
		strCacheBasePath.resize(strCacheBasePath.length() - 8);
		strCacheBasePath += FormatUrl(pFileObject->m_strHproseRpc);
		strCacheBasePath += "\\";
	}

	std::string strInfoXmlPath;
	strInfoXmlPath = ZTools::FormatString("%s%s_%s_%ld\\info.xml", strCacheBasePath.c_str(), pFileObject->m_strFileName.c_str(), pFileObject->m_strMd5.c_str(), pFileObject->m_nFileSize);
	
	if (!PathFileExists(strInfoXmlPath.c_str()))
	{
		return FALSE;
	}

 	std::string strFileMD5Info; //MD5
	std::string strFileIdInfo; //ID
	std::string strFileSizeInfo;
	std::string strFileNameInfo;

	TiXmlBase::SetCondenseWhiteSpace(false);
	TiXmlDocument doc;

#ifdef TIXML_USE_STL
	doc.LoadFile(strInfoXmlPath, TIXML_ENCODING_UTF8);
#else
	doc.LoadFile(strInfoXmlPath.c_str(), TIXML_ENCODING_UTF8);
#endif

	TiXmlElement* rootElement = doc.RootElement();
	if (rootElement)
	{
#ifdef TIXML_USE_STL
		rootElement->QueryStringAttribute("fileID", &strFileIdInfo);
		rootElement->QueryStringAttribute("fileMD5", &strFileMD5Info);
		rootElement->QueryStringAttribute("fileSize", &strFileSizeInfo);
		rootElement->QueryStringAttribute("fileName", &strFileNameInfo);
#else
		const char* pStrFileIdInfo = rootElement->Attribute("fileID");
		if (pStrFileIdInfo != NULL)
		{
			strFileIdInfo = std::string(pStrFileIdInfo);
		}
		const char* pStrFileMD5Info = rootElement->Attribute("fileMD5");
		if (pStrFileMD5Info != NULL)
		{
			strFileMD5Info = std::string(pStrFileMD5Info);
		}
		const char* pStrFileSizeInfo = rootElement->Attribute("fileSize");
		if (pStrFileSizeInfo != NULL)
		{
			strFileSizeInfo = std::string(pStrFileSizeInfo);
		}
		const char* pStrFileNameInfo = rootElement->Attribute("fileName");
		if (pStrFileNameInfo != NULL)
		{
			strFileNameInfo = std::string(pStrFileNameInfo);
		}
#endif

		ZTools::UTF8ToMB(strFileIdInfo);
		ZTools::UTF8ToMB(strFileMD5Info);
		ZTools::UTF8ToMB(strFileSizeInfo);
		ZTools::UTF8ToMB(strFileNameInfo);

		if (strFileMD5Info.compare(pFileObject->m_strMd5) == 0
			&& strFileSizeInfo.compare(ZTools::FormatString("%ld", pFileObject->m_nFileSize)) == 0
			&& strFileNameInfo.compare(pFileObject->m_strFileName) == 0
			)
		{
			pFileObject->m_strFileId = strFileIdInfo;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CDownUploadFileCacheFinder::WriteUploadCache( CDownUploadDataFileObject* pFileObject )
{
	if (pFileObject->m_strFileId.empty())
	{
		return FALSE;
	}

	if (pFileObject->CheckInfo() == FALSE)
	{
		return FALSE;
	}

	std::string strCacheBasePath = GetUploadCacheBasePath();
	if (strCacheBasePath.find("\\default\\") != std::string::npos && !pFileObject->m_strHproseRpc.empty())
	{
		strCacheBasePath.resize(strCacheBasePath.length() - 8);
		strCacheBasePath += FormatUrl(pFileObject->m_strHproseRpc);
		strCacheBasePath += "\\";
	}

	std::string strInfoXmlPath;
	strInfoXmlPath = ZTools::FormatString("%s%s_%s_%ld\\info.xml", strCacheBasePath.c_str(), pFileObject->m_strFileName.c_str(), pFileObject->m_strMd5.c_str(), pFileObject->m_nFileSize);

	if (!MakeSureDirectoryPathExists(strInfoXmlPath.c_str()))
	{
		return FALSE;
	}

	//记录信息
	TiXmlBase::SetCondenseWhiteSpace(false);
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "utf-8", "" );
	doc.LinkEndChild( decl ); 
	TiXmlElement* root = new TiXmlElement("file");
#ifdef TIXML_USE_STL
	root->SetAttribute("fileMD5", ZTools::MBToUTF8(ZTools::FormatString("%s", pFileObject->m_strMd5.c_str())));
	root->SetAttribute("fileSize", ZTools::MBToUTF8(ZTools::FormatString("%ld", pFileObject->m_nFileSize)));
	root->SetAttribute("fileID", ZTools::MBToUTF8(ZTools::FormatString("%s", pFileObject->m_strFileId.c_str())));
	root->SetAttribute("fileName", ZTools::MBToUTF8(ZTools::FormatString("%s", pFileObject->m_strFileName.c_str())));
#else
	root->SetAttribute("fileMD5", ZTools::MBToUTF8(ZTools::FormatString("%s", pFileObject->m_strMd5.c_str())).c_str());
	root->SetAttribute("fileSize", ZTools::MBToUTF8(ZTools::FormatString("%ld", pFileObject->m_nFileSize)).c_str());
	root->SetAttribute("fileID", ZTools::MBToUTF8(ZTools::FormatString("%s", pFileObject->m_strFileId.c_str())).c_str());
	root->SetAttribute("fileName", ZTools::MBToUTF8(ZTools::FormatString("%s", pFileObject->m_strFileName.c_str())).c_str());
#endif
	doc.LinkEndChild(root);

#ifdef TIXML_USE_STL
	doc.SaveFile(strInfoXmlPath);
#else
	doc.SaveFile(strInfoXmlPath.c_str());
#endif

	return TRUE;
}

BOOL CDownUploadFileCacheFinder::ReadDownloadCache( CDownUploadDataFileObject* pFileObject )
{
	if (pFileObject->m_strFileId.empty())
	{
		return FALSE;
	}

	std::string strCacheBasePath = GetDownloadCacheBasePath();
	if (strCacheBasePath.find("\\default\\") != std::string::npos && !pFileObject->m_strHproseRpc.empty())
	{
		strCacheBasePath.resize(strCacheBasePath.length() - 8);
		strCacheBasePath += FormatUrl(pFileObject->m_strHproseRpc);
		strCacheBasePath += "\\";
	}

	std::string strInfoXmlPath;
	strInfoXmlPath = ZTools::FormatString("%s%s\\%s_info.xml", strCacheBasePath.c_str(), pFileObject->m_strFileId.c_str(), pFileObject->m_strFileId.c_str());

	if (!PathFileExists(strInfoXmlPath.c_str()))
	{
		return FALSE;
	}

	std::string strFileNameInfo;
	std::string strFileSizeInfo;

	TiXmlBase::SetCondenseWhiteSpace(false);
	TiXmlDocument doc;

#ifdef TIXML_USE_STL
	doc.LoadFile(strInfoXmlPath, TIXML_ENCODING_UTF8);
#else
	doc.LoadFile(strInfoXmlPath.c_str(), TIXML_ENCODING_UTF8);
#endif

	TiXmlElement* rootElement = doc.RootElement();
	if (rootElement == NULL)
	{
		return FALSE;
	}

#ifdef TIXML_USE_STL
	rootElement->QueryStringAttribute("fileName", &strFileNameInfo);
	rootElement->QueryStringAttribute("fileSize", &strFileSizeInfo);
#else
	const char* pStrFileNameInfo = rootElement->Attribute("fileName");
	if (pStrFileNameInfo != NULL)
	{
		strFileNameInfo = std::string(pStrFileNameInfo);
	}
	const char* pStrFileSizeInfo = rootElement->Attribute("fileSize");
	if (pStrFileSizeInfo != NULL)
	{
		strFileSizeInfo = std::string(pStrFileSizeInfo);
	}
#endif
	ZTools::UTF8ToMB(strFileNameInfo);
	ZTools::UTF8ToMB(strFileSizeInfo);

	if (strFileNameInfo.empty() || strFileSizeInfo.empty())
	{
		return FALSE;
	}

	std::string strReturnFilePath = ZTools::FormatString("%s%s\\%s", strCacheBasePath.c_str(), pFileObject->m_strFileId.c_str(), strFileNameInfo.c_str());
	if (!PathFileExists(strReturnFilePath.c_str()))
	{
		return FALSE;
	}

	pFileObject->m_strFilePath = strReturnFilePath;
	if (pFileObject->CheckInfo(FALSE) == FALSE)
	{
		pFileObject->m_strFilePath.clear();
		return FALSE;
	}

	if (strFileSizeInfo.compare(ZTools::FormatString("%ld", pFileObject->m_nFileSize)) != 0)
	{
		pFileObject->m_strFilePath.clear();
		return FALSE;
	}

	return TRUE;
}

BOOL CDownUploadFileCacheFinder::WriteDownloadCache( CDownUploadDataFileObject* pFileObject )
{
	if (pFileObject->m_strFileId.empty()
		|| pFileObject->m_strFileName.empty()
		|| pFileObject->m_nFileSize < 0
		)
	{
		return FALSE;
	}

	std::string strCacheBasePath = GetDownloadCacheBasePath();
	if (strCacheBasePath.find("\\default\\") != std::string::npos && !pFileObject->m_strHproseRpc.empty())
	{
		strCacheBasePath.resize(strCacheBasePath.length() - 8);
		strCacheBasePath += FormatUrl(pFileObject->m_strHproseRpc);
		strCacheBasePath += "\\";
	}

	std::string strInfoXmlPath;
	strInfoXmlPath = ZTools::FormatString("%s%s\\%s_info.xml", strCacheBasePath.c_str(), pFileObject->m_strFileId.c_str(), pFileObject->m_strFileId.c_str());

	if (!MakeSureDirectoryPathExists(strInfoXmlPath.c_str()))
	{
		return FALSE;
	}

	TiXmlBase::SetCondenseWhiteSpace(false);
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "utf-8", "" );
	doc.LinkEndChild( decl ); 
	TiXmlElement* root = new TiXmlElement("file");
#ifdef TIXML_USE_STL
	root->SetAttribute("fileSize", ZTools::MBToUTF8(ZTools::FormatString("%ld", pFileObject->m_nFileSize)));
	root->SetAttribute("fileID", ZTools::MBToUTF8(ZTools::FormatString("%s", pFileObject->m_strFileId.c_str())));
	root->SetAttribute("fileName", ZTools::MBToUTF8(ZTools::FormatString("%s", pFileObject->m_strFileName.c_str())));
#else
	root->SetAttribute("fileSize", ZTools::MBToUTF8(ZTools::FormatString("%ld", pFileObject->m_nFileSize)).c_str());
	root->SetAttribute("fileID", ZTools::MBToUTF8(ZTools::FormatString("%s", pFileObject->m_strFileId.c_str())).c_str());
	root->SetAttribute("fileName", ZTools::MBToUTF8(ZTools::FormatString("%s", pFileObject->m_strFileName.c_str())).c_str());
#endif
	doc.LinkEndChild(root);

#ifdef TIXML_USE_STL
	doc.SaveFile(strInfoXmlPath);
#else
	doc.SaveFile(strInfoXmlPath.c_str());
#endif
	
	return TRUE;
}

BOOL CDownUploadFileCacheFinder::WriteDownloadCacheInUpload( CDownUploadDataFileObject* pFileObject, bool bMove /*= false*/ )
{
	if (pFileObject->m_strFileId.empty()
		|| pFileObject->m_strFileName.empty()
		|| pFileObject->m_nFileSize < 0
		|| pFileObject->m_strFilePath.empty()
		|| !PathFileExists(pFileObject->m_strFilePath.c_str())
		)
	{
		return FALSE;
	}
	
	std::string strCacheBasePath = GetDownloadCacheBasePath();
	if (strCacheBasePath.find("\\default\\") != std::string::npos && !pFileObject->m_strHproseRpc.empty())
	{
		strCacheBasePath.resize(strCacheBasePath.length() - 8);
		strCacheBasePath += FormatUrl(pFileObject->m_strHproseRpc);
		strCacheBasePath += "\\";
	}

	std::string strCurFilePath;
	strCurFilePath = ZTools::FormatString("%s%s\\%s", strCacheBasePath.c_str(), pFileObject->m_strFileId.c_str(), pFileObject->m_strFileName.c_str());

	if (PathFileExists(strCurFilePath.c_str()))
	{
		return TRUE;
	}

	if (!MakeSureDirectoryPathExists(strCurFilePath.c_str()))
	{
		return FALSE;
	}
	
	//拷贝
	if (bMove)
	{
		rename(pFileObject->m_strFilePath.c_str(), strCurFilePath.c_str());
	}
	else
	{
		CopyFile(pFileObject->m_strFilePath.c_str(), strCurFilePath.c_str(), FALSE);
	}
	//记录信息
	if (PathFileExists(strCurFilePath.c_str()))
	{
		return WriteDownloadCache(pFileObject);
	}

	return FALSE;
}