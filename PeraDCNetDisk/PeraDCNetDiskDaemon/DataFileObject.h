#pragma once

class DataFileObject
{
public:
	DataFileObject();

	BOOL CheckInfo(BOOL bMd5 = TRUE);

	std::string m_strHproseRpc;
	std::string m_strFilePath;
	std::string m_strFileName;
	__int64 m_nFileSize;
	__int64 m_nFileSizeUploaded;
	std::string m_strMd5;
	std::string m_strFileId;
	std::string m_strIndex;
	std::string m_strLastModifyTime;
	std::string m_strMsg;
};