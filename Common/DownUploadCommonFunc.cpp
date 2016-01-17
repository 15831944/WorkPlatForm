#include "stdafx.h"
#include "DownUploadCommonFunc.h"
#include "Tools.h"
#include "ConvertData.h"
#include "FileFunc.h"
#ifdef WIN32
#include <objbase.h>
#else
#include <uuid/uuid.h>
#endif
#include "ConfigFunc.h"

#include <atlbase.h>
using namespace ATL;

#ifndef CRYPTOPP_MD5_H
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include "cryptopp561/md5.h"
#include "cryptopp561/filters.h"
#include "cryptopp561/channels.h"
#include "cryptopp561/files.h"
#include "cryptopp561/hex.h"
#ifdef _DEBUG
#pragma comment(lib, "cryptlibd.lib")
#else
#pragma comment(lib, "cryptlib.lib")
#endif
#endif

std::string FormatUrl(std::string strUrl)
{
	std::string strHead = "http://";
	int nPosition = strUrl.find(strHead);
	if (nPosition == std::string::npos)
	{
		return "";
	}
	int nStart = nPosition + strHead.length();
	int nEnd = strUrl.find('/', nStart);
	int nCount = nEnd - nStart;
	if (nCount < 1)
	{
		return "";
	}
	strUrl = strUrl.substr(nStart, nCount);
	std::replace(strUrl.begin(), strUrl.end(), ':', '.');
	return strUrl;
}

std::string GetCacheBasePathTemp(std::string strFlag)
{
	//本地缓存路径
	CString strCacheRoot =  GetConfig("Pera","RunCache");
	if (strCacheRoot.IsEmpty())
		strCacheRoot = "C:\\WorkSpace\\Data";

	//本地缓存+fileid
	std::string strFileCacheBaseDir;

	std::string strUrl = GetHproseRpcEx();
	strUrl = FormatUrl(strUrl);
	if (strUrl.empty())
	{
		strUrl = "default";
	}

	//文件路径计算
	strFileCacheBaseDir = ZTools::FormatString("%s\\DmsFileCache\\%s\\%s\\", strCacheRoot, strFlag.c_str(), strUrl.c_str());

	return strFileCacheBaseDir;
}

std::string GetSpacialCacheBasePath()
{
	return GetCacheBasePathTemp("UploadZip");
}

std::string GetUploadCacheBasePath()
{
	return GetCacheBasePathTemp("Upload");
}

std::string GetDownloadCacheBasePath()
{
	return GetCacheBasePathTemp("Download");
}

std::string GetHproseRpcEx()
{
	std::string sRet = (LPCTSTR)GetConfig("hprose", "hproseRpc");
	return sRet;
}

std::string GetHproseRpc()
{
	std::string sRet = (LPCTSTR)GetConfig("hprose", "hproseRpc");
	return sRet;
}

std::string GetFileMd5(std::string strFilePath)
{
	ZTools::WriteZToolsFormatLog("开始计算MD5");
	std::string strResult;
	CryptoPP::Weak::MD5 md5;
	CryptoPP::HashFilter filter(md5);

	std::auto_ptr<CryptoPP::ChannelSwitch> channelSwitch(new CryptoPP::ChannelSwitch);
	channelSwitch->AddDefaultRoute(filter);
	//CryptoPP::FileSource(strFilePath.c_str(), true, channelSwitch.release());
	std::wstring wstr = s2ws_1(strFilePath);
	CryptoPP::FileSource(wstr.c_str(), true, channelSwitch.release());

	std::stringstream result;
	CryptoPP::HexEncoder encoder(new CryptoPP::FileSink(result), false);
	filter.TransferTo(encoder);
	strResult = result.str();

	ZTools::WriteZToolsFormatLog("完成计算MD5");
	return strResult;
}

std::string GetFileSize(std::string strFilePath)
{
	LONGLONG iFileSize = FileSize(strFilePath.c_str());
	if (iFileSize < 0)
		return "";
	return ZTools::FormatString("%ld", iFileSize);
}

std::string GetFileName( std::string strFilePath )
{
	std::replace(strFilePath.begin(), strFilePath.end(), '/', '\\');
	return strFilePath.substr(strFilePath.find_last_of('\\') + 1, strFilePath.length() - strFilePath.find_last_of('\\') - 1);
}

std::string GetFileLastModifyTime( std::string strFilePath )
{
	WIN32_FIND_DATAW DataFind;
	CStringW strFileW = (WCHAR*)_bstr_t(strFilePath.c_str());
	HANDLE hFind = FindFirstFileW(strFileW,&DataFind);
	if(hFind == INVALID_HANDLE_VALUE)
		return "";

	FindClose(hFind);
	if(DataFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		return "";

	LONGLONG nLL;
	ULARGE_INTEGER ui;
	ui.LowPart  = DataFind.ftLastWriteTime.dwLowDateTime;
	ui.HighPart = DataFind.ftLastWriteTime.dwHighDateTime;

	nLL = (ULONGLONG(ui.HighPart) << 32) + ui.LowPart;
	time_t pt = (long)((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
	return ZTools::FormatString("%ld", pt);
}

GUID CreateGuid()
{
	GUID guid;
#ifdef WIN32
	CoCreateGuid(&guid);
#else
	uuid_generate(reinterpret_cast<unsigned char *>(&guid));
#endif
	return guid;
}

std::string GuidToString(const GUID &guid)
{
	char buf[64] = {0};
#ifdef __GNUC__
	snprintf(
#else // MSVC
	_snprintf_s(
#endif
		buf,
		sizeof(buf),
		"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1],
		guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5],
		guid.Data4[6], guid.Data4[7]);
	return std::string(buf);
}