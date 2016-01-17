#pragma once
#include "CallHprose.h"

class CDataUpladJob
{
public:
	CDataUpladJob();

	BOOL m_bStarted;
	BOOL m_bFinished;
	std::string m_strJsonKey;
	CDownUploadDataFileObject fileObject;
};

class CPfudeTheadData
{
public:
	CPfudeTheadData(void);
	~CPfudeTheadData(void);

	void Clear();
	BOOL IsProcess();
	BOOL IsComponent();
	BOOL IsComponentWithForm();

	void Lock();
	void Unlock();

	std::string ToStartReturnJson();
	void DetachToStartVariant(VARIANT* pvRet);

	std::string ToQueryReturnJson();
	void DetachToQueryVariant(VARIANT* pvRet);

	std::string ToCancelReturnJson();
	void DetachToCancelVariant(VARIANT* pvRet);

	//上传任务标识
	std::string m_strIndex;
	//文件路径
	std::string m_strFilePath;
	//是否上传本文件
	BOOL m_bIsUploadThisFile;
	//文件名称
	std::string m_strFileName;
	//md5
	//std::string m_strFileMd5;
	//文件类型，Process/Component
	std::string m_strFileType;
	//状态
	std::string m_strFlag;
	BOOL m_bCancel;
	//信息
	std::string m_strMsg;
	//所有文件的fileid键值对
	std::string m_strFileIdListJson;
	//manifest或main.xpdl的fileid
	std::string m_strMainFileId;
	//当前文件的fileid
	std::string m_strThisFileId;
// 	//进度
// 	int m_nRate;
 	//总大小
 	__int64 m_n64AllSize;
// 	//已完成大小
// 	__int64 m_n64FinishedSize;
	//上传的远程RPC地址
	std::string m_strHproseRpc;
	//解压的临时目录
	std::string m_strUnzipDir;

	CCriticalSection criticalSectionWrite;
	//压缩包中所有文件任务
	std::vector<CDataUpladJob> m_vJobs;
	//当前压缩包
	CDownUploadDataFileObject m_thisFileObject;

	//新增BOTW类型上传，约定JSON格式如下，对应增加几个变量
	/*{
		"fileName": "测试.botw",
			"filePath": "C:\\Users\\chao.li\\Desktop\\测试.botw",
			"fileType": "ComponentWithForm",                                                      带表单的组件
			"flag": "finish",
			"index": "90FD9230-2F7A-4873-976F-1FECE24A6E9C",
			"msg": "",
			"rate": "100",
			"result": {
				"allFiles": {
					"\\Manifest.xml": "53fbf4f513834f98a20a4c51",
						"\\Reference\\fi1.txt": "53fbf4f513834f98a20a4c55",
						"\\数据解析组件.flexproj": "53fbf4f513834f98a20a4c53"
				},
				"mainFile": "",
				"thisFile": "",
				"component": {
					"fileName": "数据解析组件.bot",                                      组件文件的相关信息
						"fileId": "12312321312312342",
						"manifest": "1111443211"
					},
					"form": {
						"fileName": "自定义界面组件.bot",                                  表单文件的相关信息
							"fileId": "12312321322212312",
							"manifest": "111334111"
					}
		}
	}*/

	//组件、组件描述、表单、表单描述
	CDownUploadDataFileObject m_componentFileObject;
	CDownUploadDataFileObject m_componentManifestObject;
	CDownUploadDataFileObject m_formFileObject;
	CDownUploadDataFileObject m_formManifestObject;
};

class CDataStartIn
{
public:
	CDataStartIn(void);
	~CDataStartIn(void);

	void Clear();
	BOOL Init(BSTR bstrJson);

	std::string m_strFilePath;
	//是否上传本文件
	BOOL m_bIsUploadThisFile;
};

class CDataQueryIn
{
public:
	CDataQueryIn(void);
	~CDataQueryIn(void);

	void Clear();
	BOOL Init(BSTR bstrJson);

	std::string m_strIndex;
};

class CDataCancelIn
{
public:
	CDataCancelIn(void);
	~CDataCancelIn(void);

	void Clear();
	BOOL Init(BSTR bstrJson);

	std::string m_strIndex;
};

class CSpecialZipCache
{
public:
	CSpecialZipCache(void);
	~CSpecialZipCache(void);
	//所有文件的fileid键值对
	std::string m_strFileIdListJson;
	//manifest或main.xpdl的fileid
	std::string m_strMainFileId;
	//当前文件的fileid
	//std::string m_strThisFileId;
	//当前文件的md5
	std::string m_strThisFileMd5;
	//当前文件的大小
	std::string m_strThisFileSize;

	//组件文件信息
	std::string m_strComponentFileName;
	std::string m_strComponentFileId;
	std::string m_strComponentManifest;
	//表单文件信息
	std::string m_strFormFileName;
	std::string m_strFormFileId;
	std::string m_strFormManifest;

	void Clear();
	BOOL ReadCache();
	BOOL WriteCache();
};