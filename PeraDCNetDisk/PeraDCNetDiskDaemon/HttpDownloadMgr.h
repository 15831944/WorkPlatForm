#pragma once
#include "HttpDownloadThread.h"

typedef vector< CHttpDownloadThread* > HTTPDOWNLOAD_TASKS;
typedef vector< CHttpDownloadThread* > RUNNING_TASKS;

class CHttpDownloadMgr
{
	friend class CHttpDownloadThread;

public:
	//最大任务线程个数
	static const int MAX_TASK_THREAD = 100;

	CHttpDownloadMgr(void);
	virtual ~CHttpDownloadMgr(void);

	BOOL Init(void);
	void Release(void);
	
	virtual CHttpDownloadThread* NewThreadObj(void);//创建一个新的下载任务对象。


	DWORD	m_dwConnectTimeout;						//下载线程连接超时

	DWORD	m_dwMaxSpeed;							//下载最大速度
	
protected:
	
	static DWORD WINAPI			_ControlThread(LPVOID lpParameter);		//控制任务运行的线程函数

	static DWORD				MonitorWork( CHttpDownloadMgr* pThis );	//监控下载线程状态，控制当前运行数等

	void						ClearControlThread(void);

	static CHttpDownloadThread* GetTaskForRun( CHttpDownloadMgr* pMgr );//取得下一个要运行的任务

	void Lock( LPCTSTR lpszFuncName = "" );
	void Unlock( LPCTSTR lpszFuncName = "" );

protected:
	BOOL				m_bStopControlThread;

	DWORD				m_dwRunningTaskCount;					//正在运行的任务个数

	DWORD				m_dwWaitTaskCount;						//等待运行的任务

	RUNNING_TASKS		m_vecRunning;
	/*
	变量功能：
		任务资源锁。
	使用说明：
	主线程：
		当主线程中，执行添加、删除、查询、停止、暂停任务等操作前，加锁；完成后解锁。
	控制线程：
		当从任务队列中获取任务数据时，加锁。
	任务线程：
		当任务线程改变任务状态前，加锁，改变后，解锁。
	*/
	CRITICAL_SECTION m_csTask;
	HTTPDOWNLOAD_TASKS m_Tasks; //任务全集
	//DWORD m_dwNextTaskOffset; // 下一个将要运行的任务的偏移量

	//CURRUN_TASKS m_CurRunTasks; //当前下载中的任务
	//CRITICAL_SECTION m_csRunTask;

	//同时可以运行的最大线程个数
	//为0时，自动设置为 MAX_TASK_THREAD
	DWORD m_dwMaxTask;
	// 下载时，最大块尺寸（只能小于 CURL_MAX_WRITE_SIZE ？）
	long long m_llMaxBufferSizeForDownload;
	//链接超时（单位：秒）
	DWORD m_dwCurlConnTimeoutForDownload;
	//全局下载速度限制
	//为0，表示不限制
	int m_nMaxSpeedDownloadGlobal;
	//每个线程下载速度限制
	//为0，表示不限制
	int m_nMaxSpeedDownloadTask;

	DWORD m_dwControlThreadId;
	HANDLE m_hControlThread;
	HANDLE m_hControlStartEvent;
	HANDLE m_hControlExitEvent;
};

