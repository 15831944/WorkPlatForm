#pragma once
#include "StlHeader.h"
#include "HttpDownloadThreadData.h"

class CHttpDownloadThread : public CHttpDownloadThreadData
{
	friend class CHttpDownloadMgr;

public:
	CHttpDownloadThread( CHttpDownloadMgr* pMgr );
	virtual ~CHttpDownloadThread(void);

	BOOL Stop(void);
	BOOL Pause( void );
	BOOL Resume( void );

	virtual CString GetUrlForDownload(void);
	virtual CString GetLocalPathForDownload(void);
	virtual CString GetFileTotalSize(void);
	virtual BOOL	IsCopy( void );
	virtual BOOL	ReNameExistFile( void );
	virtual CString GetCopySourcePath( void );
	virtual bool	CheckFileType( const  curl_slist* m_pHeader );
	virtual CString GetFileId( void );
	virtual void	OnStatusChanged(void);
	virtual CString	GetidPath();

	//通过Curl方法取得文件总尺寸
	long long GetFileTotalSizeByCurl(void);
	//取得本地文件真实尺寸
	long long GetLocalFileRealSize(void);
	//是否需要重新下载
	BOOL CheckReDownload(void);
	//判断线程是否正在运行
	BOOL IsThreadRun( void );

protected:
	//下载线程启动前调用
	BOOL Init(void);
	//下载线程返回后调用
	BOOL Release(void);
	//管理类调用
	BOOL Run(void);

	BOOL CreateMultiDirectoryEx(const char* ,bool bIsFile);

	BOOL GetLocalFileSize();

	//下载线程
	static DWORD WINAPI _DownloadThread(LPVOID lpParameter);
	//curl写文件的回调函数
	static size_t OnCurlWriteFile( char *ptr, size_t size, size_t nmemb, void *userdata );
	//curl header回调函数
	static size_t OnCurlWriteHeader( void *ptr, size_t size, size_t nmemb, void *userdata );
	//curl 进度回调函数
	static int OnCurlProgress( void *clientp, double dltotal, double dlnow
		, double ultotal, double ulnow );
};


