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

	//ͨ��Curl����ȡ���ļ��ܳߴ�
	long long GetFileTotalSizeByCurl(void);
	//ȡ�ñ����ļ���ʵ�ߴ�
	long long GetLocalFileRealSize(void);
	//�Ƿ���Ҫ��������
	BOOL CheckReDownload(void);
	//�ж��߳��Ƿ���������
	BOOL IsThreadRun( void );

protected:
	//�����߳�����ǰ����
	BOOL Init(void);
	//�����̷߳��غ����
	BOOL Release(void);
	//���������
	BOOL Run(void);

	BOOL CreateMultiDirectoryEx(const char* ,bool bIsFile);

	BOOL GetLocalFileSize();

	//�����߳�
	static DWORD WINAPI _DownloadThread(LPVOID lpParameter);
	//curlд�ļ��Ļص�����
	static size_t OnCurlWriteFile( char *ptr, size_t size, size_t nmemb, void *userdata );
	//curl header�ص�����
	static size_t OnCurlWriteHeader( void *ptr, size_t size, size_t nmemb, void *userdata );
	//curl ���Ȼص�����
	static int OnCurlProgress( void *clientp, double dltotal, double dlnow
		, double ultotal, double ulnow );
};


