#pragma once
#include <string>
#include <Guiddef.h>

std::string FormatUrl(std::string strUrl);

std::string GetSpacialCacheBasePath();
std::string GetUploadCacheBasePath();
std::string GetDownloadCacheBasePath();

std::string GetHproseRpcEx();
std::string GetHproseRpc();

std::string GetFileMd5(std::string strFilePath);
std::string GetFileSize(std::string strFilePath);
std::string GetFileName(std::string strFilePath);
std::string GetFileLastModifyTime(std::string strFilePath);

GUID CreateGuid();
std::string GuidToString(const GUID &guid);