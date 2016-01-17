#include "StdAfx.h"
#include "PeraHttpDownloadMgr.h"
#include "UtilityFunc.h"
#include "Tools.h"
#include "UserDefine.h"
#include "CPeraDataPersist.h"
#include "PeraNetDiskSetting.h"
#include "FileFunc.h"

extern CPeraNetDiskSetting theSetting;

CPeraHttpDownloadMgr::CPeraHttpDownloadMgr(void)
{
}


CPeraHttpDownloadMgr::~CPeraHttpDownloadMgr(void)
{
}

CHttpDownloadThread* CPeraHttpDownloadMgr::NewThreadObj( void )
{
	return new CPeraDownloadThread( this );
}

/*
非线程安全，需要外部调用锁。
*/
CPeraDownloadThread* CPeraHttpDownloadMgr::Find( LPCTSTR lpszLocalPath, LPCTSTR lpszIdPath )
{
	CPeraDownloadThread * pTask = NULL;
	
	vector< CPeraDownloadThread* > vResult;
	vector< CPeraDownloadThread* > vTasksByLocalPath;
	vector< CPeraDownloadThread* > vTasksByIdPath;

	CString sLocalPath = lpszLocalPath == NULL ? "" : lpszLocalPath;
	CString sIdPath = lpszIdPath == NULL ? "" : lpszIdPath;

	if ( !sLocalPath.IsEmpty() )
	{
		for ( HTTPDOWNLOAD_TASKS::iterator itrTask = m_Tasks.begin();
			itrTask != m_Tasks.end(); ++itrTask )
		{
			pTask = (CPeraDownloadThread *)(*itrTask);
			if ( pTask->m_pData->GetLocalPath() == sLocalPath )
			{
				vTasksByLocalPath.push_back( pTask );
			}
		}
	}

	if ( !sIdPath.IsEmpty() )
	{
		for ( HTTPDOWNLOAD_TASKS::iterator itrTask = m_Tasks.begin();
			itrTask != m_Tasks.end(); ++itrTask )
		{
			pTask = (CPeraDownloadThread *)(*itrTask);
			if ( pTask->m_pData->m_sIdPath == sIdPath )
			{
				vTasksByIdPath.push_back( pTask );
			}
		}
	}

	if ( !sLocalPath.IsEmpty() && sIdPath.IsEmpty() )
	{
		vResult = vTasksByLocalPath;
	}
	else if ( sLocalPath.IsEmpty() && !sIdPath.IsEmpty() )
	{
		vResult = vTasksByIdPath;
	}
	else
	{
		for (int i=0; i<(int)vTasksByLocalPath.size(); i++)
		{
			if ( vTasksByIdPath.end() != 
				find( vTasksByIdPath.begin(), vTasksByIdPath.end(), vTasksByLocalPath[i] ) )
			{
				vResult.push_back( vTasksByLocalPath[i] );
			}
		}
	}

	if ( vResult.size() == 0 ) return NULL;
	return *vResult.begin();
	
}


BOOL CPeraHttpDownloadMgr::_Add( CInterfaceData_DownloadFile* pDownloadFile )
{
	ZTools::WriteZToolsFormatLog("------CPeraHttpDownloadMgr::Add go in FileId[%s] Address[%s]------\n", pDownloadFile->m_sFileId, pDownloadFile->m_sLocalPath );

	if ( pDownloadFile == NULL ) return FALSE;
	CInterfaceData_DownloadFile* pDownloadFile2 = pDownloadFile->GetCopied();
	pDownloadFile2->m_sLocalPath = pDownloadFile2->GetLocalPath();
	CPeraDownloadThread * pTask = Find( NULL, pDownloadFile2->m_sIdPath );
	BOOL bIsExist = TRUE;
	if ( pTask == NULL)
	{
		pTask = (CPeraDownloadThread*)NewThreadObj();

		bIsExist = FALSE;
	} 
	pDownloadFile2->m_sLocalPath.Replace( "/", "\\" );
	pDownloadFile2->m_sNetDiskPath.Replace( "/", "\\" );

	CString strPassUrl;
	strPassUrl = theSetting.GetDownloadUrl()  + pDownloadFile2->m_sFileId;
	CTaskData* pTaskData = new CTaskData;
	pTaskData->From( pDownloadFile2 );
	pTaskData->SetUrl( strPassUrl );
	pTaskData->SetStatus( CHttpDownloadThread::S_WAIT );
	pTaskData->SetDelFlag(FALSE);
	thePersistMrg.DoOperation( pTaskData );
	delete pTaskData;
	pTaskData = NULL;

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsLog( "CPeraHttpDownloadMgr::Add 将进临界区");
#endif

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsLog( "CPeraHttpDownloadMgr::Add 进临界区");
#endif

	if ( !bIsExist )   //任务中不存在
	{
		pTask->m_pData = pDownloadFile2;
		m_Tasks.push_back( pTask );
		pTask->SetStatusWait();
		m_dwWaitTaskCount++;

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
		ZTools::WriteZToolsFormatLog( "CPeraHttpDownloadMgr::Add 1 WaitTaskCount[%d]RunningTaskCount[%d] [%s] ", m_dwWaitTaskCount,m_dwRunningTaskCount, pTask->GetLocalPathForDownload());
#endif
	}
	else if( pTask->IsRunning() )  //已存在，并且正在下载中
	{		
		if ( !pTask->Stop() )
		{
			ZTools::WriteZToolsLog( "CPeraHttpDownloadMgr::_Add 停止正在运行的任务失败" );
			return FALSE;
		}
		pTask->ClearAllData();
		pTask->m_pData = pDownloadFile2;
		pTask->Resume();
		m_dwWaitTaskCount++;
		m_dwRunningTaskCount--; //Resume之后 任务处于等待状态，会进入线程run函数，重设event，控制线程将得不到信号。所以才在这里操作
		DelRunVector( pTask->GetidPath() );

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
		ZTools::WriteZToolsFormatLog( "CPeraHttpDownloadMgr::Add 2 WaitTaskCount[%d]RunningTaskCount[%d] [%s] ", m_dwWaitTaskCount,m_dwRunningTaskCount, pTask->GetLocalPathForDownload());
#endif
	}
	else  if( pTask->IsStop() || pTask->IsPause() )//已存在，但任务不在下载中
	{
		if ( pTask->IsThreadRun() )
		{
			return FALSE;
		}
		pTask->ClearAllData();
		pTask->m_pData = pDownloadFile2;
		pTask->Resume();
		m_dwWaitTaskCount++;
	}
	else if ( pTask->IsWait() )
	{
		pTask->ClearAllData();
		pTask->m_pData = pDownloadFile2;
	}
	else
	{
		pTask->m_pData = pDownloadFile2;
		pTask->ClearAllData();
		pTask->SetStatusNull();
		pTask->SetStatusWait();
		m_dwWaitTaskCount++;

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
		ZTools::WriteZToolsFormatLog( "CPeraHttpDownloadMgr::Add 3 WaitTaskCount[%d]RunningTaskCount[%d] [%s] ", m_dwWaitTaskCount,m_dwRunningTaskCount, pTask->GetLocalPathForDownload());
#endif
	}

	ZTools::WriteZToolsLog("------CPeraHttpDownloadMgr::Add go out------\n" );

	return TRUE;
}

BOOL  CPeraHttpDownloadMgr::Add( CInterfaceData_DownloadFiles* pDownLoadFiles )
{
	ZTools::WriteZToolsLog("------CPeraHttpDownloadMgr::Add DownFiles go in------\n" );

	BOOL bOK = TRUE;

	Lock( "CPeraHttpDownloadMgr::Add" );
	for ( vector<CInterfaceData_DownloadFile*>::iterator it = pDownLoadFiles->m_vecFiles.begin();
		it != pDownLoadFiles->m_vecFiles.end();	it++)
	{
		if( !_Add(*it) ) bOK = FALSE;
	}
	Unlock( "CPeraHttpDownloadMgr::Add" );

	ZTools::WriteZToolsLog("------CPeraHttpDownloadMgr::Add DownFiles go out------\n" );

	return bOK;
}

BOOL CPeraHttpDownloadMgr::SetSysConfig( int nMaxThreadCount, int nConnectTimeOut )
{
	BOOL bOK = TRUE;

	HTTPDOWNLOAD_TASKS RunTaskCopy;

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsFormatLog( " CHttpDownloadMgr::SetSysConfig [%d][%d]\n ",nMaxThreadCount, nConnectTimeOut );
#endif

	Lock( "CPeraHttpDownloadMgr::SetSysConfig " );
	if ( nMaxThreadCount > (int)m_dwMaxTask || nMaxThreadCount == 0 )
	{
		m_dwMaxTask = nMaxThreadCount;
	}
	else if ( nMaxThreadCount < (int)m_dwMaxTask && nMaxThreadCount > 0  )
	{
		int nStopCount = m_dwRunningTaskCount - nMaxThreadCount;

		for ( HTTPDOWNLOAD_TASKS::reverse_iterator IterSet = m_Tasks.rbegin(); IterSet != m_Tasks.rend(); IterSet++ )
		{
			if ( (*IterSet)->m_dwStatus == CHttpDownloadThread::S_RUN )
			{
				if ( nStopCount <= 0 )
				{
					break;
				}
				nStopCount--;
				if( (*IterSet)->Stop() )
				{
					((CPeraDownloadThread*)(*IterSet))->SetBreakPointDownload();
					(*IterSet)->SetStatusWait();
					m_dwWaitTaskCount++;
				}
				else
				{
					ZTools::WriteZToolsLog( "CPeraHttpDownloadMgr::SetSysConfig 停止线程失败1" );
				}
			}
		}

		m_dwMaxTask = nMaxThreadCount;
	}

	if ( nConnectTimeOut > -1 )
	{
		for ( HTTPDOWNLOAD_TASKS::reverse_iterator IterSet = m_Tasks.rbegin(); IterSet != m_Tasks.rend(); IterSet++ )
		{
			if ( (*IterSet)->m_dwStatus == CHttpDownloadThread::S_RUN )
			{
				if( (*IterSet)->Stop() )
				{
					(*IterSet)->SetStatusWait();
					m_dwWaitTaskCount++;
				}
				else
				{
					ZTools::WriteZToolsLog( "CPeraHttpDownloadMgr::SetSysConfig 停止线程失败2" );
				}
			}
		}
		m_dwConnectTimeout = nConnectTimeOut;
	}
	Unlock( "CPeraHttpDownloadMgr::SetSysConfig " );

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsLog( " CHttpDownloadMgr::SetSysConfig Finished\n " );
#endif
	return bOK;
}

BOOL CPeraHttpDownloadMgr::Pause( LPCTSTR lpszIdPath )
{
	BOOL bOk = FALSE;

	Lock( "CPeraHttpDownloadMgr::Pause" );

	for ( HTTPDOWNLOAD_TASKS::iterator it = m_Tasks.begin(); it != m_Tasks.end(); it++ )
	{
		if (IsParentPath( lpszIdPath, ((CPeraDownloadThread *)(*it))->m_pData->m_sIdPath ) )
		{
			if( !((CPeraDownloadThread *)(*it))->Pause() )
			{
				bOk = FALSE;
				ZTools::WriteZToolsLog( "CPeraHttpDownloadMgr::Pause 停止线程失败");
			}
			else
			{
				((CPeraDownloadThread*)(*it))->SetBreakPointDownload();
			}
		}
	}
	Unlock( "CPeraHttpDownloadMgr::Pause" );

	return bOk;
}

BOOL CPeraHttpDownloadMgr::Stop( LPCTSTR lpszIdPath )
{
	BOOL bOk = FALSE;

	Lock( "CPeraHttpDownloadMgr::Stop" );

	for ( HTTPDOWNLOAD_TASKS::iterator it = m_Tasks.begin(); it != m_Tasks.end(); it++ )
	{
		if (IsParentPath( lpszIdPath, ((CPeraDownloadThread *)(*it))->m_pData->m_sIdPath ) )
		{
			if( !((CPeraDownloadThread *)(*it))->Stop() )
			{
				bOk = FALSE;
				ZTools::WriteZToolsLog( "CPeraHttpDownloadMgr::Stop 停止线程失败");
			}
			else
			{
				((CPeraDownloadThread*)(*it))->SetBreakPointDownload();
			}
		}
	}

	Unlock( "CPeraHttpDownloadMgr::Stop" );

	return bOk;
}

BOOL CPeraHttpDownloadMgr::Resume( LPCTSTR lpszIdPath )
{
	BOOL bOk = FALSE;

	Lock( "CPeraHttpDownloadMgr::Resume" );

	for ( HTTPDOWNLOAD_TASKS::iterator it = m_Tasks.begin(); it != m_Tasks.end(); it++ )
	{
		if (IsParentPath( lpszIdPath, ((CPeraDownloadThread *)(*it))->m_pData->m_sIdPath ) )
		{
			if( !((CPeraDownloadThread *)(*it))->Resume() )
			{
				bOk = FALSE;
				ZTools::WriteZToolsLog( "CPeraHttpDownloadMgr::Resume 线程失败");
			}
			else
			{
				m_dwWaitTaskCount++;
			}
		}
	}

	Unlock( "CPeraHttpDownloadMgr::Resume " );

	return bOk;
}

BOOL CPeraHttpDownloadMgr::Delete( LPCTSTR lpIdPath )
{
	BOOL bOK = TRUE;

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsFormatLog( "CPeraHttpDownloadMgr::Delete [%s]\n",lpIdPath );
#endif

	Lock( "CPeraHttpDownloadMgr::Delete" );

	for ( HTTPDOWNLOAD_TASKS::iterator it = m_Tasks.begin(); it != m_Tasks.end();  )
	{
		if ( ((CPeraDownloadThread *)(*it))->m_pData->m_sIdPath.CompareNoCase( lpIdPath ) == 0 )
		{
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
			ZTools::WriteZToolsFormatLog( " CPeraHttpDownloadMgr::Delete idPath[%s][%s] \n", lpIdPath, ((CPeraDownloadThread *)(*it))->m_pData->m_sIdPath );
#endif
			DWORD dwWaitObject = 0;

			if ( ((CPeraDownloadThread *)(*it))->IsRunning() )
			{
				if( !((CPeraDownloadThread *)(*it))->Stop() )
				{
					dwWaitObject = WaitForSingleObject( ((CPeraDownloadThread *)(*it))->m_hEventExit, TIMEOUT_PERSIST_DOWNLOAD_THREAD_INIT_TIME );
					bOK = FALSE;
				}
				else
				{
					((CPeraDownloadThread *)(*it))->SetStatusFinished();
				}
			}

			if ( ((CPeraDownloadThread *)(*it))->IsThreadRun() )
			{
				continue;
			}

			if ( ((CPeraDownloadThread *)(*it))->IsFinished() || ((CPeraDownloadThread *)(*it))->IsError() )
			{
				dwWaitObject = WaitForSingleObject( ((CPeraDownloadThread *)(*it))->m_hEventExit, TIMEOUT_PERSIST_DOWNLOAD_THREAD_INIT_TIME );
				if ( dwWaitObject == WAIT_OBJECT_0 )
				{
					m_dwRunningTaskCount--;

					DelRunVector( (*it)->GetidPath() );
				}
			}
			else if ( ((CPeraDownloadThread *)(*it))->IsWait() )
			{
				m_dwWaitTaskCount--;
			}

			CTaskData* pTaskData = new CTaskData;

			pTaskData->From( ((CPeraDownloadThread *)(*it))->m_pData );

			pTaskData->SetDelFlag( TRUE );

			thePersistMrg.DoOperation( pTaskData );

			delete pTaskData; pTaskData = NULL;

			((CPeraDownloadThread *)(*it))->ClearThreadData();
			
			CPeraDownloadThread* pTemp = (CPeraDownloadThread*)(*it);

			it = m_Tasks.erase(it);

			delete pTemp;

			pTemp = NULL;

			break;
		}
		else
		{
			it++;
		}
	}

	Unlock( "CPeraHttpDownloadMgr::Delete" );

	ZTools::WriteZToolsFormatLog( "CPeraHttpDownloadMgr::Delete Finished [%s]\n", lpIdPath );

	return TRUE;
}

BOOL CPeraHttpDownloadMgr::Query( CInterfaceData_QueryFilesOperation& pQueryFiles )
{
	ZTools::WriteZToolsLog("------CPeraHttpDownloadMgr::Query go in ------\n" );

	BOOL bOK = TRUE;

	Lock( "CPeraHttpDownloadMgr::Query" );

	vector< CPeraDownloadThread* > vecThread;

	for (vector< CInterfaceData_QueryFilesOperationPath* >::iterator it = pQueryFiles.m_vPath.begin();
		it != pQueryFiles.m_vPath.end(); it++)
	{
		for ( HTTPDOWNLOAD_TASKS::iterator itTask = m_Tasks.begin(); itTask != m_Tasks.end(); itTask++ )
		{
			if ( (*it)->m_bIsFile )
			{
				if ( (*it)->m_sIdPath.CompareNoCase( ((CPeraDownloadThread *)(*itTask))->m_pData->m_sIdPath ) == 0 )
				{
					vecThread.push_back( ((CPeraDownloadThread *)(*itTask))->GetCopied() );
					break;
				}
			}
			else
			{
				if (IsParentPath( (*it)->m_sIdPath, ((CPeraDownloadThread *)(*itTask))->m_pData->m_sIdPath ) )
				{
					vecThread.push_back( ((CPeraDownloadThread *)(*itTask))->GetCopied() );
				}
			}
		}
	}

	Unlock( "CPeraHttpDownloadMgr::Query" );

	BOOL bFindOk = FALSE;

	for (vector< CInterfaceData_QueryFilesOperationPath* >::iterator itCopy = pQueryFiles.m_vPath.begin();
		itCopy != pQueryFiles.m_vPath.end(); itCopy++)
	{
		bFindOk = FALSE;
		for ( vector< CPeraDownloadThread* >::iterator IterThread = vecThread.begin(); IterThread != vecThread.end(); IterThread++ )
		{
			if ( (*itCopy)->m_bIsFile )
			{
				if ( (*itCopy)->m_sIdPath.CompareNoCase( (*IterThread)->m_pData->m_sIdPath ) == 0 )
				{
					bFindOk = TRUE;
				}
			}
			else
			{
				bFindOk = FALSE;
				if ( IsParentPath( (*itCopy)->m_sIdPath, (*IterThread)->m_pData->m_sIdPath ) )
				{
					bFindOk = TRUE;
				}
			}
			if ( bFindOk )
			{
				CInterfaceData_QueryFilesOperationResult* pResult = new CInterfaceData_QueryFilesOperationResult;
				pResult->Clear();
				pResult->m_sFileId = (*IterThread)->m_pData->m_sFileId;
				pResult->m_sIdPath = (*IterThread)->m_pData->m_sIdPath;
				pResult->m_sMsg = (*IterThread)->m_strErrMsg;
				pResult->m_sOperation = "download";
				pResult->m_sNetDiskPath = (*IterThread)->m_pData->m_sNetDiskPath;
				pResult->m_sFinishedSize.Format("%I64d",(__int64)(*IterThread)->m_dProgressNow);

				if ( (*IterThread)->m_pData->m_sFileSize.CompareNoCase( "" ) == 0 )
				{
					pResult->m_sTotalSize.Format( "0" );
				}
				else
				{
					pResult->m_sTotalSize.Format("%s", (*IterThread)->m_pData->m_sFileSize ); //(__int64)(*itTask)->m_dProgressTotal)
				}

				__int64 i64FileSize;

				sscanf( pResult->m_sTotalSize, "%I64d", &i64FileSize );

				if ( i64FileSize == (__int64)(*IterThread)->m_dProgressNow ) //总大小为0时，除数为0的情况
				{
					pResult->m_sProgress.Format("100");
				}
				else
				{
					pResult->m_sProgress.Format("%d",(DWORD)( 100 * ( (*IterThread)->m_dProgressNow / i64FileSize ) ) );
				}

				if ( (*IterThread)->m_dwStatus & CHttpDownloadThreadData::S_WAIT)
				{
					pResult->m_sProgress.Format("0");

					pResult->m_sStatus = "Wait";
				}
				else if ( (*IterThread)->m_dwStatus & CHttpDownloadThreadData::S_RUN )
				{
					pResult->m_sStatus = "Run";
				}
				else if ( (*IterThread)->m_dwStatus & CHttpDownloadThreadData::S_FINISHED )
				{
					pResult->m_sProgress.Format("100");

					pResult->m_sStatus = "Finished";
				}
				else if ( (*IterThread)->m_dwStatus & CHttpDownloadThreadData::S_PAUSE )
				{
					pResult->m_sStatus = "Pause";
				}
				else if ( (*IterThread)->m_dwStatus & CHttpDownloadThreadData::S_STOP )
				{
					pResult->m_sStatus = "Stop";
				}
				else
				{
					pResult->m_sProgress.Format("0");

					pResult->m_sStatus = "Error";
				}

				pResult->m_sRealPath = (*IterThread)->m_pData->m_sLocalPath;
				(*itCopy)->m_vResult.push_back(pResult);

				ZTools::WriteZToolsFormatLog("------CPeraHttpDownloadMgr::Query Path[%s]Progress[%s]Status[%s] ------\n",pResult->m_sRealPath, pResult->m_sProgress, pResult->m_sStatus );
			}
			if ( bFindOk && (*itCopy)->m_bIsFile )
			{
				break;
			}
		}
	}

	for ( vector< CPeraDownloadThread* >::iterator IterThread = vecThread.begin(); IterThread != vecThread.end(); IterThread++ )
	{
		CPeraDownloadThread* pThread = *IterThread;
		delete pThread;
		pThread = NULL;
	}

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsLog( "CPeraHttpDownloadMgr::Query  出临界区\n" );
#endif

	ZTools::WriteZToolsLog("------CPeraHttpDownloadMgr::Query go out ------\n" );

	return TRUE;
}

BOOL CPeraHttpDownloadMgr::DoFileOperation( CInterfaceData_DoFilesOperation* pCancelFile )
{
	ZTools::WriteZToolsFormatLog("------CPeraHttpDownloadMgr::DoFileOperation go in sPath[%s] OperationName[%s] ------\n", pCancelFile->m_sIdPath, pCancelFile->m_soperationFlag );

	BOOL bOK = TRUE;

	if ( pCancelFile->m_soperationFlag.CompareNoCase("delete") == 0 )
	{
		return Delete( pCancelFile->m_sIdPath );  
	}
	else if ( pCancelFile->m_soperationFlag.CompareNoCase("stop") == 0 )
	{
		return Stop( pCancelFile->m_sIdPath );
	}
	else if ( pCancelFile->m_soperationFlag.CompareNoCase("pause") == 0 )
	{
		return Pause( pCancelFile->m_sIdPath );
	}
	else if ( pCancelFile->m_soperationFlag.CompareNoCase("resume") == 0 )
	{
		return Resume( pCancelFile->m_sIdPath );
	}
	else
	{
		bOK = FALSE;
	}

	ZTools::WriteZToolsLog("------CPeraHttpDownloadMgr::DoFileOperation go out ------\n" );

	return bOK;
}

BOOL CPeraHttpDownloadMgr::RecoverDownTask()
{
	vector< CTaskData* > vecData;
	
	thePersistMrg.GetDownLoadData( vecData );

	for ( vector< CTaskData* >::iterator it = vecData.begin(); it != vecData.end(); )
	{
		CTaskData* pTaskData = *it;

		CInterfaceData_DownloadFile* pDownload = new CInterfaceData_DownloadFile;

		pDownload->CopyFromTaskData( pTaskData );

		CPeraDownloadThread* pTask = (CPeraDownloadThread*)NewThreadObj();

		pTask->m_pData = pDownload;

		pTask->m_bBreakPointDown = true;

		__int64 i64FileSize = 0;

		sscanf( (char *)(LPCTSTR)pTask->m_pData->m_sFileSize, "%I64d", &i64FileSize );
		
		pTask->m_dProgressTotal = (double)i64FileSize;

		if ( PathFileExists( pTask->m_pData->m_sLocalPath ) )
		{
			LONGLONG iFileSize =  FileSize(pTask->m_pData->m_sLocalPath);
			pTask->m_dProgressNow = (double)iFileSize;
		}

		if ( ( pTaskData->m_dwStatus & CHttpDownloadThread::S_STOP ) )
		{
			pTask->SetStatusStop();
		}
		else if ( pTaskData->m_dwStatus & CHttpDownloadThread::S_PAUSE )
		{
			pTask->SetStatusPause();
		}
		else if ( pTaskData->m_dwStatus & CHttpDownloadThread::S_FINISHED )
		{
			pTask->SetStatusFinished();
		}
		else if ( pTaskData->m_dwStatus & CHttpDownloadThread::S_WAIT  ||  ( pTaskData->m_dwStatus & CHttpDownloadThread::S_RUN ) )
		{
			pTask->SetStatusWait();

			m_dwWaitTaskCount++;
		}
		else if ( pTaskData->m_dwStatus & CHttpDownloadThread::S_ERROR )
		{
			pTask->SetStatusError();
		}
		else
		{

		}

		Lock( "CPeraHttpDownloadMgr::RecoverDownTask" );
		m_Tasks.push_back( pTask );
		Unlock( "CPeraHttpDownloadMgr::RecoverDownTask" );

		it = vecData.erase( it );

		delete pTaskData;

		pTaskData = NULL;
	}

	return TRUE;
}

BOOL  CPeraHttpDownloadMgr::Replace( char** pszChar )
{
	if ( !pszChar )
	{
		return FALSE;
	}
	char* pszFront = NULL;
	char* pszSource = NULL;

	pszFront = new char[MAX_PATH*2];
	pszSource = new char[MAX_PATH*2];

	if ( !pszFront || !pszSource )
	{
		return FALSE;
	}

	ZeroMemory( pszFront, MAX_PATH*2 );
	ZeroMemory( pszSource, MAX_PATH*2 );

	strcpy( pszSource, *pszChar );

	while(1)
	{
		char* pTemp = strchr( pszSource, '/');
		if ( !pTemp )
		{
			break;
		}
		ZeroMemory( pszFront, MAX_PATH*2 );
		memcpy( pszFront, pszSource, strlen( pszSource )-strlen( pTemp ) );
		strcat( pszFront, "\\" );
		if ( strlen( pTemp ) > 1)
		{
			memcpy( &pszFront[ strlen( pszFront ) ], pTemp+1, strlen( pTemp )-1 );
		}
		ZeroMemory( pszSource, MAX_PATH*2 );
		strcpy( pszSource, pszFront );
	}

	*pszChar = pszSource;

	if ( pszFront )
	{
		delete [] pszFront;
		pszFront = NULL;
	}
	return TRUE;
}

BOOL CPeraHttpDownloadMgr::IsParentPath( const char* pszParent, const char* pszUnkown )//判断是否是父路径
{
	DWORD dwRet = 0;
	char* pszDirectory = NULL;
	char* pszDirUnkown = NULL;
	char* pszTempParent = NULL;
	char* pszTempUnkown = NULL;
	char* pszOrgParent = NULL;
	char* pszOrgUnkown = NULL;
	char* pszReplaceParent = NULL;
	char* pszReplaceUnkown = NULL;

	__try
	{
		pszDirectory =	new char[MAX_PATH*2];
		pszTempParent =	new char[MAX_PATH*2];
		pszTempUnkown = new char[MAX_PATH*2];
		pszOrgUnkown =	new char[MAX_PATH*2];
		pszOrgParent =	new char[MAX_PATH*2];
		pszDirUnkown = new char[MAX_PATH*2];

		if ( !pszTempParent || !pszDirectory || !pszOrgParent || !pszOrgUnkown || !pszTempUnkown || !pszDirUnkown )
		{
			dwRet = 1;
			__leave;
		}

		ZeroMemory( pszDirectory, MAX_PATH*2 );
		ZeroMemory( pszTempParent, MAX_PATH*2 );
		ZeroMemory( pszOrgUnkown, MAX_PATH*2 );
		ZeroMemory( pszOrgParent, MAX_PATH*2 );
		ZeroMemory( pszTempUnkown, MAX_PATH*2 );
		ZeroMemory( pszDirUnkown, MAX_PATH*2 );


		strcpy( pszOrgParent, pszParent );
		strcpy( pszOrgUnkown, pszUnkown );

		pszReplaceParent = pszOrgParent;
		pszReplaceUnkown = pszOrgUnkown;

		Replace( &pszReplaceParent );
		Replace( &pszReplaceUnkown );

		ZeroMemory( pszOrgUnkown, MAX_PATH*2 );
		ZeroMemory( pszOrgParent, MAX_PATH*2 );

		strcpy( pszOrgUnkown, pszReplaceUnkown );
		strcpy( pszOrgParent, pszReplaceParent );

		if (pszOrgParent[strlen(pszOrgParent)-1] == '\\')//去掉字符串尾部的反斜线
		{
			pszOrgParent[strlen(pszOrgParent)-1] = '\0';
		}
		if (pszOrgUnkown[strlen(pszOrgUnkown)-1] == '\\')
		{
			pszOrgUnkown[strlen(pszOrgUnkown)-1] = '\0';
		}

		while(1)
		{

			char* pTempParent = strchr(pszOrgParent, '\\');
			char* pTempUnkown = strchr(pszOrgUnkown, '\\');

			if ( !pTempParent && !pTempUnkown )					//都没有反斜线
			{
				if (stricmp( pszOrgUnkown, pszOrgParent ) == 0)
				{
					__leave;
				}
				dwRet = 5;
				__leave;
			}
			else if (!pTempParent && pTempUnkown )				//父目录没有反斜线
			{
				ZeroMemory( pszDirectory, MAX_PATH*2 );
				memcpy( pszDirectory, pszOrgUnkown, strlen(pszOrgUnkown)-strlen(pTempUnkown));

				if (stricmp( pszOrgParent, pszDirectory ) == 0)
				{
					__leave;
				}
				dwRet = 6;
				__leave;
			}
			else if (pTempParent && !pTempUnkown)				//被判断路径不存在反斜线
			{
				dwRet = 7;
				__leave;
			}

			int lenParent = pTempParent-pszOrgParent ;
			int lenUnkown = pTempUnkown-pszOrgUnkown ;

			if (lenParent != 0 && lenUnkown != 0)
			{
				ZeroMemory( pszDirectory, MAX_PATH*2 );
				ZeroMemory( pszDirUnkown, MAX_PATH*2 );
				memcpy( pszDirectory, pszOrgParent, lenParent );
				memcpy( pszDirUnkown, pszOrgUnkown, lenUnkown );
			}
			else 
			{
				memcpy( pszTempParent, &pszOrgParent[ 1 ], strlen( pszOrgParent)-1 );
				memcpy( pszTempUnkown, &pszOrgUnkown[ 1 ], strlen( pszOrgUnkown)-1 );

				ZeroMemory( pszOrgUnkown, MAX_PATH*2 );
				ZeroMemory( pszOrgParent, MAX_PATH*2 );

				strcpy( pszOrgParent, pszTempParent );
				strcpy( pszOrgUnkown, pszTempUnkown );

				ZeroMemory( pszTempUnkown, MAX_PATH*2 );
				ZeroMemory( pszTempParent, MAX_PATH*2 );

				continue;
			}

			if (stricmp( pszDirectory, pszDirUnkown ) != 0 )
			{
				dwRet = 3;
				__leave;
			}

			memcpy( pszTempParent, &pszOrgParent[ strlen( pszDirectory )+1 ], strlen(pszOrgParent)-lenParent );
			memcpy( pszTempUnkown, &pszOrgUnkown[ strlen( pszDirectory )+1 ], strlen(pszOrgUnkown)-lenParent );

			ZeroMemory( pszOrgUnkown, MAX_PATH*2 );
			ZeroMemory( pszOrgParent, MAX_PATH*2 );

			strcpy( pszOrgParent, pszTempParent );
			strcpy( pszOrgUnkown, pszTempUnkown );

			ZeroMemory( pszTempUnkown, MAX_PATH*2 );
			ZeroMemory( pszTempParent, MAX_PATH*2 );

		}

	}
	__except(1)
	{
		dwRet = 100;
	}
	if ( pszReplaceParent )
	{
		delete [] pszReplaceParent;
		pszReplaceParent = NULL;
	}

	if ( pszReplaceUnkown )
	{
		delete [] pszReplaceUnkown;
		pszReplaceUnkown = NULL;
	}
	if (pszDirectory)
	{
		delete [] pszDirectory;
		pszDirectory = NULL;
	}

	if (pszTempParent)
	{
		delete [] pszTempParent;
		pszTempParent = NULL;
	}
	if (pszTempUnkown)
	{
		delete [] pszTempUnkown;
		pszTempUnkown = NULL;
	}
	if (pszOrgParent)
	{
		delete [] pszOrgParent;
		pszOrgParent = NULL;
	}
	if (pszDirectory)
	{
		delete [] pszDirectory;
		pszDirectory = NULL;
	}
	if (pszOrgUnkown)
	{
		delete [] pszOrgUnkown;
		pszOrgUnkown = NULL;
	}
	if (pszDirUnkown)
	{
		delete [] pszDirUnkown;
		pszDirUnkown = NULL;
	}
	return !dwRet;
}

BOOL CPeraHttpDownloadMgr::GetSysConfig()
{
	m_dwMaxSpeed = theSetting.GetDownloadSpeedLimit();

	m_dwMaxTask = theSetting.GetDownloadMaxTask();

	m_dwConnectTimeout = theSetting.GetDownloadConnectTimeout();

	ZTools::WriteZToolsFormatLog( "CHttpDownloadMgr::GetSysConfig [%d][%d][%d] \n",m_dwMaxTask, m_dwMaxTask, m_dwConnectTimeout );

	return TRUE;
}

BOOL CPeraHttpDownloadMgr::DelRunVector( LPCTSTR lpIdPath )
{
	if ( lpIdPath == NULL )
	{
		return TRUE;
	}
	for ( RUNNING_TASKS::iterator IterRun = m_vecRunning.begin(); IterRun != m_vecRunning.end(); )
	{
		if ( (*IterRun)->GetidPath().CompareNoCase( lpIdPath ) == 0 )
		{
			IterRun = m_vecRunning.erase( IterRun );
			break;
		}
		else
		{
			IterRun++;
		}
	}
	return TRUE;
}

CPeraHttpDownloadMgr theDownloadMgr;