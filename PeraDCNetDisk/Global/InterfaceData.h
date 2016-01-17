#pragma once
#include "StlHeader.h"
#include "value.h"
#include <atlstr.h>
#include "FilePro.h"
#include "../PeraDCNetDiskDaemon/TaskXml.h"

/*
�����ļ��Ľӿ����ݡ�
*/
class CInterfaceData_DownloadFile
{
public:
	CInterfaceData_DownloadFile();
	~CInterfaceData_DownloadFile();
	void Clear(void);
	BOOL Parse( LPCTSTR lpszData );
	CString GetJsonStr(void);
	CInterfaceData_DownloadFile* GetCopied(void);
	BOOL CopyFromTaskData( CTaskData* pTaskData );

	CString GetLocalPath(void);
	CString GetUrl(void);

public:
	CString m_sUserName; //�û���
	CString m_sFlowId; //������/����ID
	CString m_sNetDiskPath; //���ȫ·��
	CString m_sIdPath; //ID���ȫ·��
	CString m_sFileId; //�ļ�ID
	CString m_sFileSize; //�ļ���С(�ֽ�)
	CString m_sFileMd5; //�ļ�MD5
	CString m_sLocalPath;
};



class CInterfaceData_DownloadFiles
{
public:
	CInterfaceData_DownloadFiles();
	~CInterfaceData_DownloadFiles();
	void	Clear();
	BOOL	Parse( LPCTSTR lpszData );
public:
	vector<CInterfaceData_DownloadFile*> m_vecFiles;
};

/////////////////////////////////////////////////////////////////

/*
�����ļ��еĽӿ����ݡ�
*/
class CInterfaceData_DownloadDir
{
public:
	CInterfaceData_DownloadDir();
	~CInterfaceData_DownloadDir();
	void Clear(void);
	BOOL Parse( LPCTSTR lpszData );
	CString GetJsonStr(void);

public:
};
/*
�ϴ��ļ��ӿ����ݡ�
*/
class CInterfaceData_UploadFile
{
public:
	CInterfaceData_UploadFile();
	CInterfaceData_UploadFile(CTaskData *);
	~CInterfaceData_UploadFile();
	void Clear(void);
	BOOL Parse( LPCTSTR lpszData );
	CTaskData * GetTaskCopied();
public:
	CString m_sUserName; //�û���
	CString m_sFlowId; //������/����ID
	CString m_sDataId; //����ID
	CString m_sNetdiskPath; //���ȫ·��
	CString m_sIdPath; //ID���ȫ·��
	CString m_strErrMsg; // ��¼����������Ϣ
};

class CInterfaceData_QueryFilesInfoResult
{
public:
	CString m_sFlag;
	CString m_sMsg;
	CString m_sPath;
	CString m_sFullPath;
	CString m_sFileName;
	CString m_sFileSize;
	CString m_sCreateTime;
	CString m_sModitifyTime;
	CString m_sFileType; // �ļ�����
	CString m_sOpenWithType; // �򿪷�ʽ����
	CString m_sFileIcon; // �ļ�ͼ��
};
class CInterfaceData_QueryFilesInfoFile
{
public:
	CString m_sUserName; //�û���
	CString m_sFlowId;  //������/����ID
	CString m_sPath;   //�ļ�·��
	CInterfaceData_QueryFilesInfoResult m_Result;
};
/*
��ѯ�ļ�������Ϣ�ӿ�����
*/

class CInterfaceData_QueryFilesInfo
{
public:
	CInterfaceData_QueryFilesInfo();
	~CInterfaceData_QueryFilesInfo();
	void Clear(void);
	BOOL Parse( LPCTSTR lpszData );
	CString GetJsonStrResult(void);
public:
	vector<CInterfaceData_QueryFilesInfoFile *> m_vFiles; // ·������
	CString       m_strErrMsg;
};

/*
�½������ļ����Json
*/
class CInterfaceData_CreateFileResult
{
public:
	CString m_sflag;
	CString m_sMsg;
	CString m_sPath;
	CString m_sFullPath;
	CString m_sFileName;
	CString m_sFileSize;
	CString m_sCreatetime;
	CString m_sModifyTime;
	int m_iErrCode;
};
/*
�½������ļ��ӿ�����
*/
class  CInterfaceData_CreateFile
{
public:
	CInterfaceData_CreateFile();
	~CInterfaceData_CreateFile();
	void Clear(void);
	BOOL Parse( LPCTSTR lpszData );
	CString GetJsonStrResult(void);
public:
	CString m_sUserName; //�û���
	CString m_sFlowId;   //������/����ID
	CString m_sPath;     //�ļ�·��
	CString m_sFileName; //�½��ļ�������
	CString m_sType;   //�½��ļ�����չ������ ".bmp"��
	CString       m_strErrMsg;
	CInterfaceData_CreateFileResult  m_Result;
};

/*
���Ʊ����ļ��ӿ�����
*/
class CInterfaceData_CopyFiles
{
public:
	typedef struct
	{
		CString m_sLocalPath;
		CString m_sNetDiskPath;
		CString m_sIdPath;
	}SCopyFile;
public:
	CInterfaceData_CopyFiles();
	~CInterfaceData_CopyFiles();
	void Clear(void);
	BOOL Parse( LPCTSTR lpszData );
	CTaskData * GetTaskCopied(int iIndex);
	static SCopyFile FromTaskData(CTaskData *);
public:
	CString m_sUserName; //�û���
	CString m_sFlowId;  //������/����ID
	vector<SCopyFile> m_vFiles; // ·������
};

/*
��ѯ�ļ��������Ƚӿ�����
*/

class CInterfaceData_QueryFilesOperationResult
{
public:
	CInterfaceData_QueryFilesOperationResult();
	~CInterfaceData_QueryFilesOperationResult();
	void Clear(void);
public:
	CString m_sNetDiskPath;
	CString m_sIdPath;
	CString m_sOperation;
	CString m_sTotalSize;
	CString m_sFinishedSize;
	CString m_sProgress;
	CString m_sStatus;
	CString m_sMsg;
	CString m_sFileId;//�ϴ���ɺ�ḳֵ
	CString m_sRealPath;
};

class CInterfaceData_QueryFilesOperationPath
{
public:
	CInterfaceData_QueryFilesOperationPath();
	~CInterfaceData_QueryFilesOperationPath();
	void Clear(void);
public:
	CString m_sMsg;
	CString m_sNetdiskPath;
	CString m_sIdPath;
	BOOL m_bIsFile;
	vector< CInterfaceData_QueryFilesOperationResult* > m_vResult;
};

class CInterfaceData_QueryFilesOperation
{
public:
	CInterfaceData_QueryFilesOperation();
	~CInterfaceData_QueryFilesOperation();
	void Clear(void);
	BOOL Parse( LPCTSTR lpszData );
	CString GetJsonStrForResult(void);
public:
	BOOL m_bFlag; //��ʾ�Ƿ���óɹ�
	CString m_sMsg;
	CString m_sUserName; //�û���
	CString m_sFlowId; //������/����ID
	vector< CInterfaceData_QueryFilesOperationPath* > m_vPath;
};

/*
�ļ��������ƽӿ�����
*/
class CInterfaceData_DoFilesOperation
{
public:
	CInterfaceData_DoFilesOperation();
	~CInterfaceData_DoFilesOperation();
	void Clear(void);
	BOOL Parse( LPCTSTR lpszData );

public:
	CString m_sUserName;	//�û���
	CString m_sFlowId;			//������/����ID
	CString m_sNetdiskPath;		//���ȫ·��
	CString m_sIdPath;			//ID���ȫ·��
	BOOL    m_bIsFile;			//�ļ������ļ���
	CString m_soperationFlag;	//������־�����������֣�delete��stop��pause��resume
};
/*
���û�������ӿ�����
*/
class CInterfaceData_SetCacheInputData
{
public:
	CInterfaceData_SetCacheInputData();
	~CInterfaceData_SetCacheInputData();
	void Clear(void);
	BOOL Parse( LPCTSTR lpszData );
	CString GetJsonStr(void);

public:
	CString m_sCacheDir; //����·��
};

/*��ѯ�����ļ����
*/
class CInterfaceData_QueryLocalFileType
{
public:
	CInterfaceData_QueryLocalFileType();
	~CInterfaceData_QueryLocalFileType();
	void Clear(void);
	CString GetJsonStrResult(void);
public:
	vector<NewType *> m_vItemList;
};

/*
ֹͣ���ؽӿ�
*/
class CInterfaceData_StopDownload
{
public:
	CInterfaceData_StopDownload();
	~CInterfaceData_StopDownload();
	void Clear();
	BOOL Parse( LPCTSTR lpszData );

public:
	CString m_sUserName; //�û���
	CString m_sFlowId;   //������/����ID
	CString m_sNetdiskPath;    //���ȫ·��
	CString m_sIdPath; //ID���ȫ·��
	CString m_sIsFile; //�ļ������ļ���
};

/*
��ͣ���ؽӿ�
*/
class CInterfaceData_PauseDownload
{
public:
	CInterfaceData_PauseDownload();
	~CInterfaceData_PauseDownload();
	void Clear(void);
	BOOL Parse( LPCTSTR lpszData );

public:
	CString m_sUserName; //�û���
	CString m_sFlowId;   //������/����ID
	CString m_sNetdiskPath;    //���ȫ·��
	CString m_sIdPath; //ID���ȫ·��
	CString m_sIsFile; //�ļ������ļ���
};


/*
ɾ����������ӿ�
*/
class CInterfaceData_DeleteDownload
{
public:
	CInterfaceData_DeleteDownload();
	~CInterfaceData_DeleteDownload();
	void Clear(void);
	BOOL Parse( LPCTSTR lpszData );

public:
	CString m_sUserName; //�û���
	CString m_sFlowId;   //������/����ID
	CString m_sNetdiskPath;    //���ȫ·��
	CString m_sIdPath; //ID���ȫ·��
	CString m_sIsFile; //�ļ������ļ���
};

class CInterfaceData_OpenFile
{
public:
	BOOL Parse( LPCTSTR lpszData );
	CString GetResultJson();
	CInterfaceData_DownloadFiles * ConvertToDownloadObj(CString strLocalPath);
	CInterfaceData_UploadFile   * ConvertToUploadObj();
	CInterfaceData_QueryFilesOperation *ConvertToQueryFilesOperationObj();
public: // ����
	CString m_sUserName;      //�û���
	CString m_sFlowId;      //������/����ID
	CString m_sNetDiskpath; //����·��
	CString m_sIdPath;      //ID���ȫ·��
	CString m_sOpenMode;    //��ģʽ
	CString m_sFileId;      //�ļ�ID
	CString m_sFileSize;    //�ļ���С(�ֽ�)
	CString m_sFileMd5;     //�ļ�MD5
	CString m_sDataId;      //����ID
public: // ���
	CString m_sFlag;
	CString m_sMsg;
};
/************************************************************************/
/* ��ѯĿ¼���ݺͲ�ѯĿ¼������Ľӿ����ݶ���                               */
/************************************************************************/
class CInterfaceData_QueryDir
{
public:
	void Clear(void);
	BOOL Parse( LPCTSTR lpszData );
	CString GetQueryDirResultJson(const struct _Folder_Item &);
	CString GetQueryDirSubCountResultJson(const struct _Folder_Item &);
public: // ����
	bool    m_bRecursion;      //�Ƿ�ݹ�
	vector<CString> m_Dirs;    //����Ҫ��ѯ��Ŀ¼�б�
public: // ���
	CString m_sFlag;
	CString m_sMsg;
};
/************************************************************************/
/* �����������ļ������ļ��еĽӿ����ݶ���                                 */
/************************************************************************/
class CInterfaceData_ReName
{
public:
	void Clear(void);
	BOOL Parse( LPCTSTR lpszData );
public: // ����
	CString m_sPath;         // �ļ�·��
	CString m_sUserName;      //�û���
	CString m_sFlowId;      //������/����ID
	CString m_sNewName;     //�µ�����
public: // ���
	CString m_sFlag;
	CString m_sMsg;
	int m_iErrCode;
};
/************************************************************************/
/* ɾ������ļ����ļ��еĽӿ����ݶ���                                    */
/************************************************************************/
class CInterfaceData_DeleteItem
{
public:
	typedef struct 
	{
		CString m_strPath;
		BOOL  m_bIsFile;
	}SDeleteItem;

	void Clear(void);
	BOOL Parse( LPCTSTR lpszData );
public: // ����
	CString m_sUserName;      //�û���
	CString m_sFlowId;      //������/����ID
	vector<SDeleteItem>   m_vItems; //ɾ�����
public: // ���
	CString m_sFlag;
	CString m_sMsg;
};
/************************************************************************/
/* ����ճ��ʱ���ļ����ƽӿ����ݶ���                                       */
/************************************************************************/
class CInterfaceData_PasteFiles
{
public:
	typedef struct
	{
		CString m_sSrcFlowId;
		CString m_sSrcNetDiskPath;
		CString m_sDstFlowId;
		CString m_sDstNetDiskPath;
		CString m_sIdPath;
	}SPasteFile;
public:
	void Clear(void);
	BOOL Parse( LPCTSTR lpszData );
public:
	CString m_sUserName; //�û���
	vector<SPasteFile> m_vFiles; // ճ����������
};

typedef CInterfaceData_PasteFiles::SPasteFile SMoveFile;
typedef CInterfaceData_PasteFiles CInterfaceData_MoveFiles;