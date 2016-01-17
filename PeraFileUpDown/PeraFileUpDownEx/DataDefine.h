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

	//�ϴ������ʶ
	std::string m_strIndex;
	//�ļ�·��
	std::string m_strFilePath;
	//�Ƿ��ϴ����ļ�
	BOOL m_bIsUploadThisFile;
	//�ļ�����
	std::string m_strFileName;
	//md5
	//std::string m_strFileMd5;
	//�ļ����ͣ�Process/Component
	std::string m_strFileType;
	//״̬
	std::string m_strFlag;
	BOOL m_bCancel;
	//��Ϣ
	std::string m_strMsg;
	//�����ļ���fileid��ֵ��
	std::string m_strFileIdListJson;
	//manifest��main.xpdl��fileid
	std::string m_strMainFileId;
	//��ǰ�ļ���fileid
	std::string m_strThisFileId;
// 	//����
// 	int m_nRate;
 	//�ܴ�С
 	__int64 m_n64AllSize;
// 	//����ɴ�С
// 	__int64 m_n64FinishedSize;
	//�ϴ���Զ��RPC��ַ
	std::string m_strHproseRpc;
	//��ѹ����ʱĿ¼
	std::string m_strUnzipDir;

	CCriticalSection criticalSectionWrite;
	//ѹ�����������ļ�����
	std::vector<CDataUpladJob> m_vJobs;
	//��ǰѹ����
	CDownUploadDataFileObject m_thisFileObject;

	//����BOTW�����ϴ���Լ��JSON��ʽ���£���Ӧ���Ӽ�������
	/*{
		"fileName": "����.botw",
			"filePath": "C:\\Users\\chao.li\\Desktop\\����.botw",
			"fileType": "ComponentWithForm",                                                      ���������
			"flag": "finish",
			"index": "90FD9230-2F7A-4873-976F-1FECE24A6E9C",
			"msg": "",
			"rate": "100",
			"result": {
				"allFiles": {
					"\\Manifest.xml": "53fbf4f513834f98a20a4c51",
						"\\Reference\\fi1.txt": "53fbf4f513834f98a20a4c55",
						"\\���ݽ������.flexproj": "53fbf4f513834f98a20a4c53"
				},
				"mainFile": "",
				"thisFile": "",
				"component": {
					"fileName": "���ݽ������.bot",                                      ����ļ��������Ϣ
						"fileId": "12312321312312342",
						"manifest": "1111443211"
					},
					"form": {
						"fileName": "�Զ���������.bot",                                  ���ļ��������Ϣ
							"fileId": "12312321322212312",
							"manifest": "111334111"
					}
		}
	}*/

	//������������������������
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
	//�Ƿ��ϴ����ļ�
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
	//�����ļ���fileid��ֵ��
	std::string m_strFileIdListJson;
	//manifest��main.xpdl��fileid
	std::string m_strMainFileId;
	//��ǰ�ļ���fileid
	//std::string m_strThisFileId;
	//��ǰ�ļ���md5
	std::string m_strThisFileMd5;
	//��ǰ�ļ��Ĵ�С
	std::string m_strThisFileSize;

	//����ļ���Ϣ
	std::string m_strComponentFileName;
	std::string m_strComponentFileId;
	std::string m_strComponentManifest;
	//���ļ���Ϣ
	std::string m_strFormFileName;
	std::string m_strFormFileId;
	std::string m_strFormManifest;

	void Clear();
	BOOL ReadCache();
	BOOL WriteCache();
};