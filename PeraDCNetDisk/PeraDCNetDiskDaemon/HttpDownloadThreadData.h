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
	static const DWORD S_WAIT						= 0x00000001; //�ȴ�����
	static const DWORD S_RUN						= 0x00000002; //���ڴ���
	static const DWORD S_FINISHED					= 0x00000004; //�������
	static const DWORD S_STOP						= 0x00000008; //��ֹͣ
	static const DWORD S_PAUSE						= 0x00000010; //����ͣ
	static const DWORD S_DELETE						= 0x00000020; //��ɾ��

	static const DWORD S_ERROR						= 0x10000000; //�쳣
	static const DWORD S_ERROR_CREATETHREAD			= 0x20000000; //�쳣�������߳�ʧ��
	static const DWORD S_ERROR_CREATETHREAD_TIMEOUT	= 0x40000000; //�쳣�������̳߳�ʱ


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
	BOOL				m_bPause;				//���߳̿��� ����������ͣ
	BOOL				m_bStop;				//���߳̿��� ��������ֹͣ

	double				m_dProgressTotal;		//���ؽ���-���ֽ���
	double				m_dProgressNow;			//���ؽ���-�����ص��ֽ���
	DWORD				m_dwStatus;				//״̬

protected:
	HANDLE	m_hEventRun;

	HANDLE	m_hEventExit;

	HANDLE	m_hThread;				//�߳̾��

	DWORD	m_dwThreadId;			//�߳�ID

	CString m_sId;					//Ψһ��ʶ
	
	CHttpDownloadMgr *m_pMgr;		//������ָ��

	CURL*	m_pEasyCurl;			//Easyģʽ�õ�Curlָ��
	
	CURL*	m_pMultiCurl;			//Multyģʽ�õ�Curlָ�룬���ļ����صĻ���m_vEasyCurl��ֻ��һ������ָ��

	vector< CURL* > m_vEasyCurl;

	HANDLE	m_pLocalFile;			//���ص����ݣ�д�뱾���ļ��ľ��
	
	long long m_llWrittenSize;		//ÿ�� Curl �ص�д���ݺ���ʱ��д�����ݵ��ֽ�����
	
	
	BOOL m_bErrBuf;					//�Ƿ���Ҫ Curl �ص������� ErrorInfo ����
	
	char* m_pCurlErrBuf;			//ErrorInfo ���ݻ���

	
	BOOL m_bHeader;					//�Ƿ���Ҫ Curl �ص������� Header ����
	
	curl_slist* m_pHeader;			// Header ���ݶ����б�

	bool m_bBreakPointDown;			//�ϵ�����
	
	//�Ƿ�ͨ��curl���ڻص������ݽ���
	//�ص������У������ȫΪ0���������Ҫ���ˡ�
	BOOL m_bProgress;

	//�ļ����ֽ�����
	//����ͨ���ӿڲ�����ã����Ϊ�գ���ӷ�����������
	long long m_llTotalSize;
	//�Ѿ����ص��ֽ���
	long long m_llCurSize;
	//��Ϣ
	vector< CString > m_vInfo;

	CString m_strErrMsg;
};