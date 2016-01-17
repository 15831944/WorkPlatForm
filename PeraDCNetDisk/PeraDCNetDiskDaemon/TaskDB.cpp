#include "StdAfx.h"
#include "TaskDB.h"
#include "PtrVectorHeader.h"


CTaskDB::CTaskDB(void)
{
}


CTaskDB::~CTaskDB(void)
{
}

BOOL CTaskDB::Init( LPCTSTR lpszXmlPath )
{
	if ( !m_Xml.Open( lpszXmlPath ) ) return FALSE;
	if ( !m_Xml.ParseXml() ) return FALSE;
	return TRUE;
}

void CTaskDB::Release( void )
{
	m_Xml.Clear();
}

BOOL CTaskDB::Save( void )
{
	return m_Xml.Save();
}

BOOL CTaskDB::Add( CTaskData* pNewData )
{
	if ( pNewData == NULL ) return FALSE;
	if ( !pNewData->CheckValue() ) return FALSE;
	if ( m_Xml.m_pDoc == NULL ) return FALSE;
//	if ( CheckExisted( pNewData->m_sIdPath ) ) return FALSE;

	CTaskData* pTaskCopied = pNewData->GetCopied();
	m_Xml.m_vTasks.push_back( pTaskCopied );
//	m_Xml.Add( pTaskCopied );
	
	return TRUE;
}

BOOL CTaskDB::InsertAfter( const CTaskData* pNewData )
{
	if ( pNewData == NULL ) return FALSE;

	return TRUE;
}

BOOL CTaskDB::Delete( LPCTSTR lpszIdPath )
{
	CString sIdPath = lpszIdPath == NULL ? "" : lpszIdPath;
	if ( sIdPath.IsEmpty() ) return FALSE;
	
	CTaskData* pTask = Find( lpszIdPath );

	if ( pTask == NULL )
	{
		return FALSE;
	} 

//	m_Xml.Delete( pTask );

	return PtrVectorRemove( m_Xml.m_vTasks, pTask );
}

BOOL CTaskDB::Update( CTaskData* pNewData )
{
	if ( pNewData == NULL ) return FALSE;
	if ( !pNewData->CheckValue() ) return FALSE;
	if ( m_Xml.m_pDoc == NULL ) return FALSE;

	CTaskData* pOldData = Find( pNewData->m_sIdPath );
	if ( pOldData == NULL ) return FALSE;
	pOldData->Copy( pNewData );
//	m_Xml.Update( pOldData );
	return TRUE;
}

CTaskData* CTaskDB::Find( LPCTSTR lpszIdPath )
{
	CString sIdPath = lpszIdPath == NULL ? "" : lpszIdPath;
	if ( sIdPath.IsEmpty() ) return NULL;
	
	CTaskData* pTask = NULL;

	for ( vector< CTaskData* >::iterator itrTask = m_Xml.m_vTasks.begin();
		itrTask != m_Xml.m_vTasks.end(); ++itrTask )
	{
		pTask = (*itrTask);
		if ( pTask->m_sIdPath == sIdPath )
		{
			return pTask;
		}
	}

	return NULL;
}

CTaskData* CTaskDB::GetTaskDataCopied( LPCTSTR lpszIdPath )
{
	CTaskData* pData = Find( lpszIdPath );
	if (pData == NULL) return NULL;
	return pData->GetCopied();
}

BOOL CTaskDB::CheckExisted( LPCTSTR lpszIdPath )
{
	return ( Find( lpszIdPath ) != NULL );
}
