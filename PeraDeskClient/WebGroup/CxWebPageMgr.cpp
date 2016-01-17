#include "StdAfx.h"
#include "CxWebPageMgr.h"
#include <UtilityFunc.h>

CCxWebPageMgr::CCxWebPageMgr(void)
{
}

CCxWebPageMgr::~CCxWebPageMgr(void)
{
	ClearEventMap();
}

void CCxWebPageMgr::ClearEventMap( void )
{
	for ( WEBPAGE_EVENT_MAP::iterator itr = m_mapEvent.begin(); 
		itr != m_mapEvent.end(); /*++itr*/ )
	{
		CString strId = itr->first;
		DeleteEvent( (LPCTSTR)strId );
		itr = m_mapEvent.begin();
	}
}

CCxWebPageEvent * CCxWebPageMgr::NewEvent( int nModuleFlag, int nEventFlag, BOOL bMgr )
{
	CString strId = GetNewEventNameWin7();
	while ( _Find( (LPCTSTR)strId ) != m_mapEvent.end() )
	{
		strId = GetNewEventNameWin7();
	}

	CCxWebPageEvent * pEvent = _GetNew( nModuleFlag, nEventFlag );
	if ( pEvent == NULL ) return NULL;

	pEvent->m_strId = strId;
	pEvent->m_strEventId = strId;
	pEvent->m_pData = _OnNewEventExData( nModuleFlag, nEventFlag );

	if ( !_OnNewEvent( pEvent ) )
	{
		delete pEvent; pEvent = NULL; return NULL;
	}

	if ( bMgr )
	{
		pEvent->m_pMgr = this;
		m_mapEvent.insert( make_pair( pEvent->m_strId, pEvent ) );
	}
	return pEvent;
}

CCxWebPageEvent * CCxWebPageMgr::NewEvent( int nModuleFlag, int nEventFlag
	, CCxWebPageEventExData * pExData, BOOL bMgr )
{
	CString strId = GetNewEventNameWin7();
	while ( _Find( (LPCTSTR)strId ) != m_mapEvent.end() )
	{
		strId = GetNewEventNameWin7();
	}

	CCxWebPageEvent * pEvent = _GetNew( nModuleFlag, nEventFlag );
	if ( pEvent == NULL ) return NULL;

	pEvent->m_strId = strId;
	pEvent->m_strEventId = strId;
	pEvent->m_pData = _OnNewEventExData( nModuleFlag, nEventFlag );

	if ( pExData != NULL )
	{
		if ( !pExData->GetCopy( pEvent->m_pData ) )
		{
			delete pEvent; pEvent = NULL; return NULL;
		}
	}

	if ( !_OnNewEvent( pEvent ) )
	{
		delete pEvent; pEvent = NULL; return NULL;
	}

	if ( bMgr )
	{
		pEvent->m_pMgr = this;
		m_mapEvent.insert( make_pair( pEvent->m_strId, pEvent ) );
	}
	return pEvent;
}

CCxWebPageEvent * CCxWebPageMgr::NewEvent( LPCTSTR lpszData )
{
	CString strId = GetNewEventNameWin7();
	while ( _Find( (LPCTSTR)strId ) != m_mapEvent.end() )
	{
		strId = GetNewEventNameWin7();
	}

	CCxWebPageEvent * pEvent = _GetNew( 0, 0 );
	if ( pEvent == NULL ) return NULL;

	if ( !pEvent->Parse( lpszData ) )
	{
		delete pEvent; pEvent = NULL; return NULL;
	}

	pEvent->m_strId = strId;
	pEvent->m_pData = _OnNewEventExData( pEvent->m_nModuleFlag, pEvent->m_nEventFlag );

	if ( !pEvent->ParseExData() )
	{
		//delete pEvent; pEvent = NULL; return NULL;
	}

	if ( !_OnNewEvent( pEvent ) )
	{
		delete pEvent; pEvent = NULL; return NULL;
	}

	return pEvent;
}

CCxWebPageEvent * CCxWebPageMgr::_GetNew( int nModuleFlag, int nEventFlag )
{
	return new CCxWebPageEvent( nModuleFlag, nEventFlag );
}

CCxWebPageEventExData* CCxWebPageMgr::_OnNewEventExData( int nModuleFlag, int nEventFlag )
{
	switch ( nEventFlag )
	{
	case CCxWebPageMgr::FE_NAVIGATE:
		return new CCxWPENavigate;
	default:
		return NULL;
	}
	return NULL;
}

BOOL CCxWebPageMgr::_OnNewEvent( CCxWebPageEvent * pEvent )
{
	return TRUE;
}

BOOL CCxWebPageMgr::DeleteEvent( CCxWebPageEvent*& pEvent )
{
	if ( pEvent == NULL ) return FALSE;
	CString strId = pEvent->m_strId;

	WEBPAGE_EVENT_MAP::iterator itr = _Find( (LPCTSTR)strId );
	if ( itr != m_mapEvent.end() )
	{
		m_mapEvent.erase( itr );
	}
	delete pEvent; pEvent = NULL;
	return TRUE;
}

BOOL CCxWebPageMgr::DeleteEvent( LPCTSTR lpszEventId )
{
	CString strId = lpszEventId == NULL ? "" : lpszEventId;
	if ( strId.IsEmpty() ) return FALSE;

	WEBPAGE_EVENT_MAP::iterator itr = _Find( (LPCTSTR)strId );
	if ( itr == m_mapEvent.end() ) return FALSE;
	//
	CCxWebPageEvent * pEvent = itr->second;
	m_mapEvent.erase( itr );
	delete pEvent; pEvent = NULL;
	return TRUE;
}

CCxWebPageEvent * CCxWebPageMgr::Find( LPCTSTR lpszEventId )
{
	WEBPAGE_EVENT_MAP::iterator itr = _Find( lpszEventId );
	if ( itr == m_mapEvent.end() ) return NULL;
	return itr->second;
}

WEBPAGE_EVENT_MAP::iterator CCxWebPageMgr::_Find( LPCTSTR lpszEventId )
{
	CString strId = lpszEventId == NULL ? "" : lpszEventId;
	if ( strId.IsEmpty() ) return m_mapEvent.end();
	return m_mapEvent.find( strId );
}

WEBPAGE_EVENT_MAP::iterator CCxWebPageMgr::_Find( CCxWebPageEvent * pEvent )
{
	if ( pEvent == NULL ) return m_mapEvent.end();
	return _Find( pEvent->m_strId );
}

//
BOOL CCxWebPageMgr::OnWebPageEvent( LPCTSTR lpszData )
{
	return TRUE;
}

CCxWebPageEvent * CCxWebPageMgr::Require( int nModuleFlag, int nEventFlag
	, CCxWebPageEventExData * pExData )
{
	return NULL;
}

