#include "StdAfx.h"
#include "HttpDownloadMgr.h"
#include "curl.h"
#include "PeraNetDiskSetting.h"
#include "Tools.h"
#include "UserDefine.h"
#include "UtilityFunc.h"

//#define DBGLOG_DOWNMGR_LOCK

CHttpDownloadMgr::CHttpDownloadMgr(void)
{
	m_dwMaxTask = 0;
	m_dwControlThreadId = 0;
	m_hControlThread = NULL;
	m_bStopControlThread = FALSE;
	m_dwWaitTaskCount = 0;
	m_dwRunningTaskCount = 0;
	m_llMaxBufferSizeForDownload = 0;
	m_dwCurlConnTimeoutForDownload = 30;
	m_nMaxSpeedDownloadGlobal = 0;
	m_nMaxSpeedDownloadTask = 0;
	m_hControlStartEvent = NULL;
	m_hControlExitEvent = NULL;
}


CHttpDownloadMgr::~CHttpDownloadMgr(void)
{
	
}

BOOL CHttpDownloadMgr::Init( void )
{
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsLog( " CHttpDownloadMgr::Init \n" );
#endif

	if ( m_dwControlThreadId != 0 ) return FALSE;

	m_llMaxBufferSizeForDownload = CURL_MAX_WRITE_SIZE;

	curl_global_init( CURL_GLOBAL_ALL );

	InitializeCriticalSection( &m_csTask );

	//创建控制线程
	m_hControlThread = ::CreateThread(
		NULL
		, 0
		, CHttpDownloadMgr::_ControlThread
		, (LPVOID)this
		, CREATE_SUSPENDED
		, &m_dwControlThreadId);

	if (m_hControlThread == NULL || m_dwControlThreadId == 0)
	{
		AfxMessageBox("创建线程失败，CHttpDownloadMgr::_ControlThread");
		ClearControlThread();
		return FALSE;
	}
	m_hControlStartEvent = CreateEvent( NULL, TRUE, FALSE, GetNewGUID() );
	if ( !m_hControlStartEvent )
	{
		AfxMessageBox( "管理类初始化创建启动事件失败");
		return FALSE;
	}
	m_hControlExitEvent = CreateEvent( NULL, TRUE, FALSE, GetNewGUID() );
	if ( !m_hControlExitEvent )
	{
		AfxMessageBox( "管理类初始化创建退出事件失败");
		return FALSE;
	}

	ResumeThread( m_hControlThread );
	
	if ( WaitForSingleObject( m_hControlStartEvent, TIMEOUT_PERSIST_DOWNLOAD_THREAD_INIT_TIME ) != WAIT_OBJECT_0 )
	{
		AfxMessageBox( "控制线程启动超时" );
		return FALSE;
	}

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsLog( "CHttpDownloadMgr::Init Finished \n" );
#endif
	return TRUE;
}

void CHttpDownloadMgr::Release( void )
{
	curl_global_cleanup();

	m_bStopControlThread = TRUE;
	
	int iCount = 0;
	
	if ( WaitForSingleObject( m_hControlExitEvent, TIMEOUT_PERSIST_DOWNLOAD_THREAD_INIT_TIME ) != WAIT_OBJECT_0 )
	{
		return;
	}
	if ( m_hControlExitEvent )
	{
		CloseHandle( m_hControlExitEvent );
		m_hControlExitEvent = NULL;
	}
	if ( m_hControlStartEvent )
	{
		CloseHandle( m_hControlStartEvent );
		m_hControlStartEvent = NULL;
	}
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsLog( "CHttpDownloadMgr::Release Finished \n" );
#endif
	DeleteCriticalSection( &m_csTask );
}

CHttpDownloadThread* CHttpDownloadMgr::NewThreadObj( void )
{
	return new CHttpDownloadThread( this );
}

void CHttpDownloadMgr::ClearControlThread(void)
{
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsLog( "CHttpDownloadMgr::ClearControlThread " );
#endif
	if ( m_dwControlThreadId != 0 )
	{
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
		ZTools::WriteZToolsFormatLog( "CHttpDownloadMgr::ClearControlThread ThreadId[%d]", m_dwControlThreadId );
#endif
		m_dwControlThreadId = 0;
	}
	if ( m_hControlThread )
	{
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
		ZTools::WriteZToolsLog( "CHttpDownloadMgr::ClearControlThread 释放句柄" );
#endif
		CloseHandle( m_hControlThread );

		m_hControlThread = NULL;
	}

	m_bStopControlThread = FALSE;
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsLog( "CHttpDownloadMgr::ClearControlThread 退出" );
#endif
}

DWORD WINAPI CHttpDownloadMgr::_ControlThread(LPVOID lpParameter)
{
	CHttpDownloadMgr* pThis = (CHttpDownloadMgr*)lpParameter;

	if ( pThis == NULL ) return 0;

	CHttpDownloadThread* pNextTask = NULL;

	CHttpDownloadThread* pTask = NULL;

	SetEvent( pThis->m_hControlStartEvent );

	//在队列中找到处于等待状态的线程，在允许的最大线程数内，创建下载任务线程。
	while ( !pThis->m_bStopControlThread )
	{
		pThis->Lock( " CHttpDownloadMgr::_ControlThread " );

		MonitorWork( pThis );

Label_CHttpDownloadMgr_ControlThread_GetNextTask:

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
		ZTools::WriteZToolsFormatLog( "CHttpDownloadMgr::_ControlThread RunningTaskCount[%d] WaitTaskCount[%d] \n",pThis->m_dwRunningTaskCount, pThis->m_dwWaitTaskCount );
#endif

		if (
			( pThis->m_dwMaxTask == 0 || pThis->m_dwRunningTaskCount < pThis->m_dwMaxTask ) 
			&& pThis->m_dwWaitTaskCount > 0  && pThis->m_dwRunningTaskCount < MAX_TASK_THREAD
			)
		{
			pNextTask = GetTaskForRun( pThis );
			if ( pNextTask != NULL )
			{
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
				ZTools::WriteZToolsFormatLog( "CHttpDownloadMgr::_ControlThread Run Begin [%s]", pNextTask->GetLocalPathForDownload());
#endif
				if ( !pNextTask->Run() )
				{
					pNextTask->SetStatusError();
					pNextTask->Release();
					pThis->m_dwWaitTaskCount--;

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
					ZTools::WriteZToolsFormatLog( "CHttpDownloadMgr::_ControlThread Run Error WaitTaskCount[%d] Path[%s] \n", pThis->m_dwWaitTaskCount, pNextTask->GetLocalPathForDownload() );
#endif

					goto Label_CHttpDownloadMgr_ControlThread_GetNextTask;
				}
				else
				{
					pThis->m_dwRunningTaskCount++; 
					pThis->m_dwWaitTaskCount--;
					pThis->m_vecRunning.push_back( pNextTask );

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
					ZTools::WriteZToolsFormatLog( "CHttpDownloadMgr::_ControlThread Run RunningTaskCount[%d] WaitTaskCount[%d] Path[%s]\n",pThis->m_dwRunningTaskCount, pThis->m_dwWaitTaskCount, pNextTask->GetLocalPathForDownload() );
#endif
				}
			}
			else
			{
				AfxMessageBox( "控制线程获取任务失败" );
				return 0;
			}
		}
		else
		{
			goto Label_CHttpDownloadMgr_ControlThread_LoopEmpty;
		}

		pThis->Unlock( " CHttpDownloadMgr::_ControlThread " );
		Sleep( 0 );
		continue;

Label_CHttpDownloadMgr_ControlThread_LoopEmpty:

		pThis->Unlock( " CHttpDownloadMgr::_ControlThread" );
		Sleep(100);
		continue;
	}

	pThis->Lock( " CHttpDownloadMgr::_ControlThread 退出" );
	for ( HTTPDOWNLOAD_TASKS::iterator IterRun = pThis->m_vecRunning.begin(); IterRun != pThis->m_vecRunning.end(); )
	{
		if ( (*IterRun)->Stop() )
		{
			(*IterRun)->SetStatusWait();
			pThis->m_dwRunningTaskCount--;
			pThis->m_dwWaitTaskCount++;
			IterRun = pThis->m_vecRunning.erase( IterRun );
		}
		else
		{
			ZTools::WriteZToolsLog( " CHttpDownloadMgr::_ControlThread 退出时停止正在运行的任务失败" );
			IterRun++;
		}
	}
	for ( HTTPDOWNLOAD_TASKS::iterator itrTask = pThis->m_Tasks.begin();itrTask != pThis->m_Tasks.end(); )
	{
		pTask = (*itrTask);

		itrTask = pThis->m_Tasks.erase( itrTask );

		if ( !pTask->IsThreadRun() )
		{
			delete pTask;

			pTask = NULL;
		}
	}

	pThis->Unlock( " CHttpDownloadMgr::_ControlThread 退出" );
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsLog( "CHttpDownloadMgr::_ControlThread Dispose Finished \n" );
#endif

	SetEvent( pThis->m_hControlExitEvent );

	return 0;
}


DWORD  CHttpDownloadMgr::MonitorWork( CHttpDownloadMgr* pThis )
{
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsFormatLog( "CHttpDownloadMgr::MonitorWork RunCount[%d] RunningTaskCount[%d] WaitTaskCount[%d]\n", pThis->m_vecRunning.size(), pThis->m_dwRunningTaskCount, pThis->m_dwWaitTaskCount );
#endif

	if ( pThis->m_dwRunningTaskCount > 0 )
	{
		for (RUNNING_TASKS::iterator itRun = pThis->m_vecRunning.begin(); itRun != pThis->m_vecRunning.end();  )
		{
			DWORD dwRet = WaitForSingleObject( (*itRun)->m_hEventExit, 0 );

			if ( dwRet == WAIT_OBJECT_0 )
			{
				pThis->m_dwRunningTaskCount--;

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
				ZTools::WriteZToolsFormatLog( "CHttpDownloadMgr::MonitorWork RunningTaskCount[%d] WaitTaskCount[%d] [%s][%d]\n", pThis->m_dwRunningTaskCount, pThis->m_dwWaitTaskCount,(*itRun)->GetLocalPathForDownload(), (*itRun)->m_dwStatus );
#endif

				(*itRun)->Release();

				itRun = pThis->m_vecRunning.erase(itRun);
			}
			else if( dwRet == WAIT_TIMEOUT )
			{
				itRun++;
			}
			else
			{
				if ( GetLastError() == ERROR_INVALID_HANDLE )
				{
				}
				itRun = pThis->m_vecRunning.erase(itRun);

				AfxMessageBox( "监控线程获取了无效的数据" );

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
				ZTools::WriteZToolsLog( "CHttpDownloadMgr::MonitorWork Invalid \n" );
#endif
			}

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
			ZTools::WriteZToolsFormatLog( "CHttpDownloadMgr::MonitorWork RunCount[%d] \n", pThis->m_vecRunning.size() );
#endif
		}
	}
	return 0;
}

/*
非线程安全，需外部调用锁。
仅由CHttpDownloadMgr::_ControlThread调用。
*/
CHttpDownloadThread* CHttpDownloadMgr::GetTaskForRun( CHttpDownloadMgr* pMgr )
{
	CHttpDownloadThread* pTask = NULL;

	for ( HTTPDOWNLOAD_TASKS::iterator itrTask = pMgr->m_Tasks.begin();
		itrTask != pMgr->m_Tasks.end(); ++itrTask )
	{
		pTask = (CHttpDownloadThread*)(*itrTask);
		if ( pTask->IsError() ) continue;
		if ( pTask->IsWait() )
		{
			return pTask;
		}
	}

	return NULL;
}

void CHttpDownloadMgr::Lock( LPCTSTR lpszFuncName )
{
#ifdef DBGLOG_DOWNMGR_LOCK
	ZTools::WriteZToolsFormatLog( "%s before lock", lpszFuncName );
#endif
	EnterCriticalSection( &m_csTask );
#ifdef DBGLOG_DOWNMGR_LOCK
	ZTools::WriteZToolsFormatLog( "%s locked", lpszFuncName );
#endif
}

void CHttpDownloadMgr::Unlock( LPCTSTR lpszFuncName )
{
#ifdef DBGLOG_DOWNMGR_LOCK
	ZTools::WriteZToolsFormatLog( "%s before unlock", lpszFuncName );
#endif
	LeaveCriticalSection( &m_csTask );
#ifdef DBGLOG_DOWNMGR_LOCK
	ZTools::WriteZToolsFormatLog( "%s unlocked", lpszFuncName );
#endif
}


