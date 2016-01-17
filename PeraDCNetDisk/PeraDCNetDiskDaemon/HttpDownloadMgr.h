#pragma once
#include "HttpDownloadThread.h"

typedef vector< CHttpDownloadThread* > HTTPDOWNLOAD_TASKS;
typedef vector< CHttpDownloadThread* > RUNNING_TASKS;

class CHttpDownloadMgr
{
	friend class CHttpDownloadThread;

public:
	//��������̸߳���
	static const int MAX_TASK_THREAD = 100;

	CHttpDownloadMgr(void);
	virtual ~CHttpDownloadMgr(void);

	BOOL Init(void);
	void Release(void);
	
	virtual CHttpDownloadThread* NewThreadObj(void);//����һ���µ������������


	DWORD	m_dwConnectTimeout;						//�����߳����ӳ�ʱ

	DWORD	m_dwMaxSpeed;							//��������ٶ�
	
protected:
	
	static DWORD WINAPI			_ControlThread(LPVOID lpParameter);		//�����������е��̺߳���

	static DWORD				MonitorWork( CHttpDownloadMgr* pThis );	//��������߳�״̬�����Ƶ�ǰ��������

	void						ClearControlThread(void);

	static CHttpDownloadThread* GetTaskForRun( CHttpDownloadMgr* pMgr );//ȡ����һ��Ҫ���е�����

	void Lock( LPCTSTR lpszFuncName = "" );
	void Unlock( LPCTSTR lpszFuncName = "" );

protected:
	BOOL				m_bStopControlThread;

	DWORD				m_dwRunningTaskCount;					//�������е��������

	DWORD				m_dwWaitTaskCount;						//�ȴ����е�����

	RUNNING_TASKS		m_vecRunning;
	/*
	�������ܣ�
		������Դ����
	ʹ��˵����
	���̣߳�
		�����߳��У�ִ����ӡ�ɾ������ѯ��ֹͣ����ͣ����Ȳ���ǰ����������ɺ������
	�����̣߳�
		������������л�ȡ��������ʱ��������
	�����̣߳�
		�������̸߳ı�����״̬ǰ���������ı�󣬽�����
	*/
	CRITICAL_SECTION m_csTask;
	HTTPDOWNLOAD_TASKS m_Tasks; //����ȫ��
	//DWORD m_dwNextTaskOffset; // ��һ����Ҫ���е������ƫ����

	//CURRUN_TASKS m_CurRunTasks; //��ǰ�����е�����
	//CRITICAL_SECTION m_csRunTask;

	//ͬʱ�������е�����̸߳���
	//Ϊ0ʱ���Զ�����Ϊ MAX_TASK_THREAD
	DWORD m_dwMaxTask;
	// ����ʱ������ߴ磨ֻ��С�� CURL_MAX_WRITE_SIZE ����
	long long m_llMaxBufferSizeForDownload;
	//���ӳ�ʱ����λ���룩
	DWORD m_dwCurlConnTimeoutForDownload;
	//ȫ�������ٶ�����
	//Ϊ0����ʾ������
	int m_nMaxSpeedDownloadGlobal;
	//ÿ���߳������ٶ�����
	//Ϊ0����ʾ������
	int m_nMaxSpeedDownloadTask;

	DWORD m_dwControlThreadId;
	HANDLE m_hControlThread;
	HANDLE m_hControlStartEvent;
	HANDLE m_hControlExitEvent;
};

