#pragma once
#include "curl.h"
#include "StlHeader.h"

class CHttpDownloadMgr;

class CHttpDownloadThreadData
{
	friend class CHttpDownloadMgr;
	friend class CPeraHttpDownloadMgr;
	
public:
	static const DWORD S_NULL						= 0x00000000; //
	static const DWORD S_WAIT						= 0x00000001; //等待处理
	static const DWORD S_RUN						= 0x00000002; //正在处理
	static const DWORD S_FINISHED					= 0x00000004; //处理完成
	static const DWORD S_STOP						= 0x00000008; //被停止
	static const DWORD S_PAUSE						= 0x00000010; //被暂停
	static const DWORD S_DELETE						= 0x00000020; //被删除

	static const DWORD S_ERROR						= 0x10000000; //异常
	static const DWORD S_ERROR_CREATETHREAD			= 0x20000000; //异常，创建线程失败
	static const DWORD S_ERROR_CREATETHREAD_TIMEOUT	= 0x40000000; //异常，创建线程超时


public:
	CHttpDownloadThreadData();
	~CHttpDownloadThreadData();
	void ClearThreadData(void);
	void Clear(void);

	BOOL IsWait(void) const { return ( (m_dwStatus & S_WAIT) == S_WAIT ); }
	BOOL IsRunning(void) const { return ( (m_dwStatus & S_RUN) == S_RUN ); }
	BOOL IsFinished(void) const { return ( (m_dwStatus & S_FINISHED) == S_FINISHED ); }
	BOOL IsPause(void) const { return ( (m_dwStatus & S_PAUSE) == S_PAUSE ); }
	BOOL IsError(void) const { return ( (m_dwStatus & S_ERROR) == S_ERROR ); }
	BOOL IsStop(void) const { return ( (m_dwStatus & S_STOP) == S_STOP ); }

	BOOL SetStatusWait(void);
	BOOL SetStatusRun(void);
	BOOL SetStatusFinished(void);
	BOOL SetStatusError(void);
	BOOL SetStatusPause(void);
	BOOL SetStatusStop(void);
	BOOL SetStatusNull(void);

	long long ConvertDouble2LongLong ( double dValue );
	CString ConvertLongLong2Str( long long llValue );
	CString ConvertDouble2Str( double dValue );
	long long ConvertStr2LongLong( LPCTSTR lpszValue );

	CHttpDownloadMgr* GetMgr(void) const;

protected:
	virtual void	OnStatusChanged(void);

public:
	BOOL				m_bPause;				//主线程控制 下载任务暂停
	BOOL				m_bStop;				//主线程控制 下载任务停止

	double				m_dProgressTotal;		//下载进度-总字节数
	double				m_dProgressNow;			//下载进度-已下载的字节数
	DWORD				m_dwStatus;				//状态

protected:
	HANDLE	m_hEventRun;

	HANDLE	m_hEventExit;

	HANDLE	m_hThread;				//线程句柄

	DWORD	m_dwThreadId;			//线程ID

	CString m_sId;					//唯一标识
	
	CHttpDownloadMgr *m_pMgr;		//管理类指针

	CURL*	m_pEasyCurl;			//Easy模式用的Curl指针
	
	CURL*	m_pMultiCurl;			//Multy模式用的Curl指针，单文件下载的话，m_vEasyCurl中只有一个数据指针

	vector< CURL* > m_vEasyCurl;

	HANDLE	m_pLocalFile;			//下载的内容，写入本地文件的句柄
	
	long long m_llWrittenSize;		//每次 Curl 回调写数据函数时，写入数据的字节数。
	
	
	BOOL m_bErrBuf;					//是否需要 Curl 回调，传递 ErrorInfo 数据
	
	char* m_pCurlErrBuf;			//ErrorInfo 数据缓存

	
	BOOL m_bHeader;					//是否需要 Curl 回调，传递 Header 数据
	
	curl_slist* m_pHeader;			// Header 数据对象（列表）

	bool m_bBreakPointDown;			//断点下载
	
	//是否通过curl定期回调，传递进度
	//回调函数中，会出现全为0的情况，需要过滤。
	BOOL m_bProgress;

	//文件总字节数。
	//首先通过接口参数获得，如果为空，则从服务器请求获得
	long long m_llTotalSize;
	//已经下载的字节数
	long long m_llCurSize;
	//信息
	vector< CString > m_vInfo;

	CString m_strErrMsg;
};