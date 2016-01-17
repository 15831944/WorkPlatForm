#pragma once
#include "StlHeader.h"
#include "value.h"
#include <atlstr.h>
#include "FilePro.h"
#include "../PeraDCNetDiskDaemon/TaskXml.h"

/*
下载文件的接口数据。
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
	CString m_sUserName; //用户名
	CString m_sFlowId; //工作项/流程ID
	CString m_sNetDiskPath; //相对全路径
	CString m_sIdPath; //ID相对全路径
	CString m_sFileId; //文件ID
	CString m_sFileSize; //文件大小(字节)
	CString m_sFileMd5; //文件MD5
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
下载文件夹的接口数据。
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
上传文件接口数据。
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
	CString m_sUserName; //用户名
	CString m_sFlowId; //工作项/流程ID
	CString m_sDataId; //数据ID
	CString m_sNetdiskPath; //相对全路径
	CString m_sIdPath; //ID相对全路径
	CString m_strErrMsg; // 记录解析错误信息
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
	CString m_sFileType; // 文件类型
	CString m_sOpenWithType; // 打开方式名称
	CString m_sFileIcon; // 文件图标
};
class CInterfaceData_QueryFilesInfoFile
{
public:
	CString m_sUserName; //用户名
	CString m_sFlowId;  //工作项/流程ID
	CString m_sPath;   //文件路径
	CInterfaceData_QueryFilesInfoResult m_Result;
};
/*
查询文件基本信息接口数据
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
	vector<CInterfaceData_QueryFilesInfoFile *> m_vFiles; // 路径数组
	CString       m_strErrMsg;
};

/*
新建本地文件结果Json
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
新建本地文件接口数据
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
	CString m_sUserName; //用户名
	CString m_sFlowId;   //工作项/流程ID
	CString m_sPath;     //文件路径
	CString m_sFileName; //新建文件的名称
	CString m_sType;   //新建文件的扩展名（如 ".bmp"）
	CString       m_strErrMsg;
	CInterfaceData_CreateFileResult  m_Result;
};

/*
复制本地文件接口数据
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
	CString m_sUserName; //用户名
	CString m_sFlowId;  //工作项/流程ID
	vector<SCopyFile> m_vFiles; // 路径数组
};

/*
查询文件操作进度接口数据
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
	CString m_sFileId;//上传完成后会赋值
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
	BOOL m_bFlag; //表示是否调用成功
	CString m_sMsg;
	CString m_sUserName; //用户名
	CString m_sFlowId; //工作项/流程ID
	vector< CInterfaceData_QueryFilesOperationPath* > m_vPath;
};

/*
文件操作控制接口数据
*/
class CInterfaceData_DoFilesOperation
{
public:
	CInterfaceData_DoFilesOperation();
	~CInterfaceData_DoFilesOperation();
	void Clear(void);
	BOOL Parse( LPCTSTR lpszData );

public:
	CString m_sUserName;	//用户名
	CString m_sFlowId;			//工作项/流程ID
	CString m_sNetdiskPath;		//相对全路径
	CString m_sIdPath;			//ID相对全路径
	BOOL    m_bIsFile;			//文件还是文件夹
	CString m_soperationFlag;	//操作标志，有如下四种：delete、stop、pause、resume
};
/*
设置缓存输入接口数据
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
	CString m_sCacheDir; //缓存路径
};

/*查询本地文件结果
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
停止下载接口
*/
class CInterfaceData_StopDownload
{
public:
	CInterfaceData_StopDownload();
	~CInterfaceData_StopDownload();
	void Clear();
	BOOL Parse( LPCTSTR lpszData );

public:
	CString m_sUserName; //用户名
	CString m_sFlowId;   //工作项/流程ID
	CString m_sNetdiskPath;    //相对全路径
	CString m_sIdPath; //ID相对全路径
	CString m_sIsFile; //文件还是文件夹
};

/*
暂停下载接口
*/
class CInterfaceData_PauseDownload
{
public:
	CInterfaceData_PauseDownload();
	~CInterfaceData_PauseDownload();
	void Clear(void);
	BOOL Parse( LPCTSTR lpszData );

public:
	CString m_sUserName; //用户名
	CString m_sFlowId;   //工作项/流程ID
	CString m_sNetdiskPath;    //相对全路径
	CString m_sIdPath; //ID相对全路径
	CString m_sIsFile; //文件还是文件夹
};


/*
删除下载任务接口
*/
class CInterfaceData_DeleteDownload
{
public:
	CInterfaceData_DeleteDownload();
	~CInterfaceData_DeleteDownload();
	void Clear(void);
	BOOL Parse( LPCTSTR lpszData );

public:
	CString m_sUserName; //用户名
	CString m_sFlowId;   //工作项/流程ID
	CString m_sNetdiskPath;    //相对全路径
	CString m_sIdPath; //ID相对全路径
	CString m_sIsFile; //文件还是文件夹
};

class CInterfaceData_OpenFile
{
public:
	BOOL Parse( LPCTSTR lpszData );
	CString GetResultJson();
	CInterfaceData_DownloadFiles * ConvertToDownloadObj(CString strLocalPath);
	CInterfaceData_UploadFile   * ConvertToUploadObj();
	CInterfaceData_QueryFilesOperation *ConvertToQueryFilesOperationObj();
public: // 输入
	CString m_sUserName;      //用户名
	CString m_sFlowId;      //工作项/流程ID
	CString m_sNetDiskpath; //网盘路径
	CString m_sIdPath;      //ID相对全路径
	CString m_sOpenMode;    //打开模式
	CString m_sFileId;      //文件ID
	CString m_sFileSize;    //文件大小(字节)
	CString m_sFileMd5;     //文件MD5
	CString m_sDataId;      //数据ID
public: // 输出
	CString m_sFlag;
	CString m_sMsg;
};
/************************************************************************/
/* 查询目录内容和查询目录内子项的接口数据定义                               */
/************************************************************************/
class CInterfaceData_QueryDir
{
public:
	void Clear(void);
	BOOL Parse( LPCTSTR lpszData );
	CString GetQueryDirResultJson(const struct _Folder_Item &);
	CString GetQueryDirSubCountResultJson(const struct _Folder_Item &);
public: // 输入
	bool    m_bRecursion;      //是否递归
	vector<CString> m_Dirs;    //输入要查询的目录列表
public: // 输出
	CString m_sFlag;
	CString m_sMsg;
};
/************************************************************************/
/* 重命名单个文件或者文件夹的接口数据定义                                 */
/************************************************************************/
class CInterfaceData_ReName
{
public:
	void Clear(void);
	BOOL Parse( LPCTSTR lpszData );
public: // 输入
	CString m_sPath;         // 文件路径
	CString m_sUserName;      //用户名
	CString m_sFlowId;      //工作项/流程ID
	CString m_sNewName;     //新的名称
public: // 输出
	CString m_sFlag;
	CString m_sMsg;
	int m_iErrCode;
};
/************************************************************************/
/* 删除多个文件、文件夹的接口数据定义                                    */
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
public: // 输入
	CString m_sUserName;      //用户名
	CString m_sFlowId;      //工作项/流程ID
	vector<SDeleteItem>   m_vItems; //删除项集合
public: // 输出
	CString m_sFlag;
	CString m_sMsg;
};
/************************************************************************/
/* 复制粘贴时的文件复制接口数据定义                                       */
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
	CString m_sUserName; //用户名
	vector<SPasteFile> m_vFiles; // 粘贴任务数组
};

typedef CInterfaceData_PasteFiles::SPasteFile SMoveFile;
typedef CInterfaceData_PasteFiles CInterfaceData_MoveFiles;