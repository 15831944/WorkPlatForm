#pragma once
#include <string>
#include <vector>
#include "hprose\common\any.hpp"
#include "DownUploadDataFileObject.h"

class CallHprose
{
public:
	CallHprose(void);
	~CallHprose(void);

	std::string m_strErrMsg;
	std::string m_strFileId;
	std::string m_strFileSize;

	std::string Upload(std::string strHproseRpc, std::string strFilePath);
	std::string UploadHprose(std::string strHproseRpc, std::string strFilePath);
	BOOL init(std::string strUrl, std::vector<hprose::any> vecAny);
	BOOL cwrite(std::string strUrl, std::vector<hprose::any> vecAny);

	std::string Upload(CDownUploadDataFileObject* pFileObject);
	std::string UploadUseCache(CDownUploadDataFileObject* pFileObject, bool bMove = false);

	std::string DownloadHprose(std::string strHproseDownloadUrl, std::string strFileDir, std::string strFileName);
};

class DataInterfaceReturn
{
public:
	std::string error;
	std::string msg;
	std::string file;

	void Clear();
	void FromJson(std::string strJson);
	std::string ToJson();
};

class DataFile
{
public:
	std::string lastModified;
	std::string name; 
	std::string size;
	std::string index;
	std::string md5;

	void Clear();
};

class DataFiles
{
public:
	std::vector<DataFile> files;

	void Clear();
	void FromJson(std::string strJson);
	std::string ToJson();
};

class DataInitReturn
{
public:
	std::string index;
	std::string curentSize;
	std::string flag;
	std::string fileID;

	void FromJson(std::string strJson);
};
//{"error":"0","fileSize":28647,"fileID":"522578d3860be341a967d82c","fileName":"qq2.zip","end":"28647"}
class DataCwriteReturn
{
public:
	std::string error;
	std::string fileID;
	std::string fileName;
	std::string end;
	std::string msg;

	void FromJson(std::string strJson);
};
