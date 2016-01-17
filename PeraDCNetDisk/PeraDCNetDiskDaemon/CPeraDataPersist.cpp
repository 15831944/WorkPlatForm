#include "stdafx.h"
#include "CPeraDataPersist.h"
#include "PtrVectorHeader.h"
#include "UtilityFunc.h"
#include "Tools.h"
#include "UserDefine.h"


CPeraDataPersist::CPeraDataPersist()
{
	m_hWorkThread = NULL;
	m_dwThreadID = 0;
	m_bStopFlag = FALSE;
	m_hEventInit = NULL;
}

CPeraDataPersist::~CPeraDataPersist()
{
	m_dwThreadID = 0;
}

BOOL CPeraDataPersist::Init()
{
	InitializeCriticalSection( &m_criLock );

	CString sXmlPath = GetXmlPath();

	if ( sXmlPath.IsEmpty() )
	{
		ZTools::WriteZToolsLog( "获取xml文件路径为空\n" );
		return FALSE;
	}

	if( !m_XmlDataObj.Init( sXmlPath ) )  //初始化Xml
	{
		AfxMessageBox( "XML对象初始化失败" );
		return FALSE;
	}


	m_hWorkThread = ::CreateThread( NULL
									, 0
									, _WorksThread
									, (LPVOID)this
									, CREATE_SUSPENDED
									, &m_dwThreadID);

	if (m_hWorkThread == NULL || m_dwThreadID == 0)
	{
		AfxMessageBox( "持久化线程开启失败" );
		return FALSE;
	}

	m_hEventInit = CreateEvent( NULL, TRUE, FALSE, GetNewGUID() ); //

	if ( !m_hEventInit )
	{
		AfxMessageBox( "CPeraDataPersist::Init 事件创建失败" );
		return FALSE;
	}

	ResumeThread( m_hWorkThread );

	if ( WaitForSingleObject( m_hEventInit, TIMEOUT_PERSIST_DOWNLOAD_THREAD_INIT_TIME ) != WAIT_OBJECT_0 )
	{
		AfxMessageBox( "CPeraDataPersist::Init WaitForSingleObject Failed" );
		return FALSE;
	}

	CloseHandle( m_hEventInit );

	m_hEventInit = NULL;

	return TRUE;
}

CString CPeraDataPersist::GetXmlPath()
{
	char szModule[MAX_PATH*2] = {0};

	GetModuleFileName( NULL, szModule, MAX_PATH*2 );

	if ( !PathFileExists( szModule ) )
	{
		return "";
	}

	char* pTemp = strrchr( szModule, '\\' );

	if ( !pTemp )
	{
		return "";
	}

	szModule[ pTemp-szModule ] = '\0';

	strcat( szModule, "\\TaskXml.xml" );

	if ( !PathFileExists( szModule ) )
	{
		AfxMessageBox( "XML文件不存在" );
		return "";
	}
	return szModule;
}

BOOL CPeraDataPersist::Release()
{
	m_bStopFlag = TRUE;

	if ( WaitForSingleObject( m_hWorkThread, 10000 ) == WAIT_TIMEOUT )
	{
		return FALSE;
	}

	m_XmlDataObj.Release();

	if ( m_hWorkThread )
	{
		CloseHandle( m_hWorkThread );

		m_hWorkThread = NULL;
	}
	if ( m_hEventInit )
	{
		CloseHandle( m_hEventInit );

		m_hEventInit = NULL;
	}

	PtrVectorClear( m_vecData );

	DeleteCriticalSection( &m_criLock );
	return TRUE;
}

BOOL CPeraDataPersist::DoOperation( CTaskData* pData )
{
	if ( m_bStopFlag )
	{
		return FALSE;
	}
	if (pData->m_bDelete)
	{
		BOOL bSuccess = FALSE;
		EnterCriticalSection( &m_criLock );
		CTaskData *pFind = m_XmlDataObj.Find(pData->m_sIdPath);
		if (pFind)
			bSuccess = m_XmlDataObj.Delete(pFind->m_sIdPath);
		LeaveCriticalSection( &m_criLock );	
		return bSuccess;
	}

	CTaskData* pNewData = pData->GetCopied();

	if ( !pNewData )
	{
		return FALSE;
	}

	EnterCriticalSection( &m_criLock );
	m_vecData.push_back( pNewData );
	LeaveCriticalSection( &m_criLock );

	return TRUE;
}


BOOL CPeraDataPersist::Save()
{
	m_XmlDataObj.Save();

	return TRUE;
}

BOOL CPeraDataPersist::GetDownLoadData( PERSIST_DATA & vecData )
{
	if ( m_bStopFlag )
	{
		return FALSE;
	}

	vecData.clear();

	for ( PERSIST_DATA::iterator it = m_XmlDataObj.m_Xml.m_vTasks.begin(); it != m_XmlDataObj.m_Xml.m_vTasks.end(); it++ )
	{
		CTaskData* pTaskData = *it;
		if ( pTaskData->m_sOperatorFlag.CompareNoCase("download") == 0)
		{
			vecData.push_back( pTaskData->GetCopied() );
		}
	}

	return TRUE;
}

BOOL CPeraDataPersist::GetUpLoadData( PERSIST_DATA & vecData )
{
	if ( m_bStopFlag )
	{
		return FALSE;
	}

	vecData.clear();

	for ( PERSIST_DATA::iterator it = m_XmlDataObj.m_Xml.m_vTasks.begin(); it != m_XmlDataObj.m_Xml.m_vTasks.end(); it++ )
	{
		CTaskData* pTaskData = *it;
		if ( pTaskData->m_sOperatorFlag.CompareNoCase("upload") == 0)
		{
			vecData.push_back( pTaskData->GetCopied() );
		}
	}

	return TRUE;
}

DWORD CPeraDataPersist::_WorksThread( LPVOID lpParameter )
{
	CPeraDataPersist* pThis = (CPeraDataPersist*)lpParameter;

	if ( pThis == NULL )
	{
		return 1;
	}

	SetEvent( pThis->m_hEventInit );
	
	while( !pThis->m_bStopFlag )
	{
		EnterCriticalSection( &pThis->m_criLock );

		PERSIST_DATA::iterator it = pThis->m_vecData.begin();

		if (it == pThis->m_vecData.end() )
		{
			LeaveCriticalSection( &pThis->m_criLock );

			Sleep(100);

			continue;
		}

		CTaskData* pTask = *it;

		it = pThis->m_vecData.erase( it );

		LeaveCriticalSection( &pThis->m_criLock );

		if ( pTask->m_bDelete )
		{
			AfxMessageBox("错误的删除逻辑");
		}
		else
		{
			if( pThis->m_XmlDataObj.CheckExisted( pTask->m_sIdPath ) )
			{
				BOOL bOK = pThis->m_XmlDataObj.Update( pTask );
				ZTools::WriteZToolsFormatLog("Update任务ID为【%s】【%s】",pTask->m_sIdPath,bOK ? "成功":"失败");
			}
			else
			{
				BOOL bOK = pThis->m_XmlDataObj.Add( pTask );
				ZTools::WriteZToolsFormatLog("Add任务ID为【%s】【%s】",pTask->m_sIdPath,bOK ? "成功":"失败");
			}
		}
		//pThis->m_XmlDataObj.Save();
		delete pTask;

		pTask = NULL;
	}

	EnterCriticalSection( &pThis->m_criLock );
	for ( PERSIST_DATA::iterator itDel = pThis->m_vecData.begin();itDel != pThis->m_vecData.end(); )
	{
		if ( (*itDel)->m_bDelete )
		{
			AfxMessageBox("错误的删除逻辑");
		}
		else
		{
			if( pThis->m_XmlDataObj.CheckExisted( (*itDel)->m_sIdPath ) )
			{
				pThis->m_XmlDataObj.Update( (*itDel) );
			}
			else
			{
				pThis->m_XmlDataObj.Add( (*itDel) );
			}
		}
		CTaskData* pData = *itDel;
		itDel = pThis->m_vecData.erase( itDel );
		delete pData;
		pData = NULL;
	}
	LeaveCriticalSection( &pThis->m_criLock );

	pThis->m_XmlDataObj.Save();
	
	return 0;
}
CTaskData* CPeraDataPersist::GetTaskDataCopied( LPCTSTR lpszIdPath )
{
	return m_XmlDataObj.GetTaskDataCopied(lpszIdPath);
}
BOOL  CPeraDataPersist::GetFileCopyData( PERSIST_DATA & vecData )
{
	if ( m_bStopFlag )
	{
		return FALSE;
	}

	vecData.clear();

	for ( PERSIST_DATA::iterator it = m_XmlDataObj.m_Xml.m_vTasks.begin(); it != m_XmlDataObj.m_Xml.m_vTasks.end(); it++ )
	{
		CTaskData* pTaskData = *it;
		if ( pTaskData->m_sOperatorFlag.CompareNoCase("copy") == 0)
		{
			vecData.push_back( pTaskData->GetCopied());
		}
	}
	return TRUE;
}
CPeraDataPersist thePersistMrg;