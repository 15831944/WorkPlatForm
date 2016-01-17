#pragma once
#include <string>
#include <vector>
#include "hprose\common\any.hpp"
#include "DataFileObject.h"
class UploadInPutData
{
public:
	std::string m_sPath;
	std::string m_sIdPath;
};

class CallHprose
{
public:
	typedef enum{
		Upload_Waiting = 0,  // 等待开始
		Upload_Runing  = 1,  // 正在上传
		Upload_Finish  = 2,  // 上传完成
		Upload_DELET   = 3,  // 删除上传
		Upload_STOP    = 4,  // 停止上传
		Upload_PAUSE   = 5,  // 暂停上传
		Upload_RESUME  = 6,  // 再次执行
		Upload_Error   = 7   // 上传出错
	}UploadTaskStatus;

	CallHprose(std::string strHposeRpc,std::string strFilepath,std::string strIndex);
	~CallHprose(void);
	std::string UploadDefault();
	BOOL deltask(std::string strIndex);
	CString GetStatusMsg(UploadTaskStatus status);
	void ResetErrFlag();
private:
	BOOL init(std::string strUrl, std::vector<hprose::any> vecAny);
	BOOL cwrite(std::string strUrl, std::vector<hprose::any> vecAny);
	std::string UploadFileObject(); //返回fileId
	std::string DownloadHprose(std::string strHproseDownloadUrl, std::string strFileDir, std::string strFileName);
public:
	std::string m_strFileSize;
	UploadTaskStatus m_TaskStatus;
	DataFileObject dataFileObject;
	UploadInPutData m_DataPathObj;
	BOOL m_bCreateIndexForFileInfo;
	std::string m_strNetDiskFileID;
private: 
	bool m_bUploadErr;
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
	long long curentSize;
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

class DeltaskReturn
{
public: 
	std::string msg;
	std::string error;
	void FromJson(std::string strJson);
};