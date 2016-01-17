#include "StdAfx.h"
#include "CxWebPageEvent.h"

#include "value.h"
#include "reader.h"
#include "writer.h"
#include <UtilityFunc.h>
#include <afxwin.h>
#include "Tools.h"
#include <UserDefine.h>

BOOL CanParseJsonObjPtr( LPVOID lpData )
{
	if ( lpData == NULL ) return FALSE;
	Json::Value * pObj = reinterpret_cast< Json::Value* >( lpData );
	if ( pObj->isNull() ) return FALSE;
	if ( !pObj->isObject() ) return FALSE;
	return TRUE;
}

CCxWebPageEventExData::CCxWebPageEventExData()
{
}

CCxWebPageEventExData::~CCxWebPageEventExData()
{
}

void CCxWebPageEventExData::Clear( void )
{
}

BOOL CCxWebPageEventExData::GetCopy( CCxWebPageEventExData *& pDest )
{
	if ( pDest == NULL ) return FALSE;
	return _OnGetCopy( pDest );
}

BOOL CCxWebPageEventExData::_OnGetCopy( CCxWebPageEventExData *& pDest )
{
	return TRUE;
}

BOOL CCxWebPageEventExData::Parse( LPCTSTR lpszData )
{
	Clear();
	CString strSrc = lpszData == NULL ? "" : lpszData;
	if ( strSrc.IsEmpty() ) return FALSE;

	Json::Reader reader;
	Json::Value obj( Json::objectValue );
	try
	{
		if ( !reader.parse((char*)(LPCTSTR)strSrc, obj, false) ) return FALSE;
		if ( !obj.isObject() ) return FALSE;

		LPVOID lpData = reinterpret_cast< LPVOID > ( (Json::Value *)&obj );
		if ( !OnParseJsonObj( lpData ) )
		{
			Clear(); return FALSE;
		}
	}
	catch (...)
	{
		Clear(); return FALSE;
	}
	return TRUE;
}

BOOL CCxWebPageEventExData::OnParseJsonObj( LPVOID lpJsonObj )
{
	if ( !CanParseJsonObjPtr( lpJsonObj ) ) return FALSE;
	Json::Value& obj = *reinterpret_cast< Json::Value* >( lpJsonObj );
	return TRUE;
}

BOOL CCxWebPageEventExData::OnGetJsonStr( LPVOID lpJsonObj )
{
	if ( !CanParseJsonObjPtr( lpJsonObj ) ) return FALSE;
	Json::Value& obj = *reinterpret_cast< Json::Value* >( lpJsonObj );
	Json::Value dataObj( Json::objectValue );
	obj["data"] = dataObj;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CCxWPENavigate::CCxWPENavigate()
{
}

CCxWPENavigate::~CCxWPENavigate()
{
	Clear();
}

void CCxWPENavigate::Clear( void )
{
	m_strUrl = "";
}

BOOL CCxWPENavigate::_OnGetCopy( CCxWebPageEventExData *& pDest )
{
	CCxWPENavigate * pOther = ( CCxWPENavigate* )pDest;
	pOther->m_strUrl = m_strUrl;
	return TRUE;
}

BOOL CCxWPENavigate::OnParseJsonObj( LPVOID lpJsonObj )
{
	if ( !CanParseJsonObjPtr( lpJsonObj ) ) return FALSE;
	Json::Value& obj = *reinterpret_cast< Json::Value* >( lpJsonObj );
	try
	{
		m_strUrl = ( obj["url"].isString()) ? obj["url"].asCString() : _T("");
	}
	catch (...)
	{
		Clear(); return FALSE;
	}
	return TRUE;
}

BOOL CCxWPENavigate::OnGetJsonStr( LPVOID lpJsonObj )
{
	if ( !CanParseJsonObjPtr( lpJsonObj ) ) return FALSE;
	Json::Value& obj = *reinterpret_cast< Json::Value* >( lpJsonObj );
	Json::Value dataObj( Json::objectValue );
	dataObj["url"] = Json::Value( (LPCTSTR)m_strUrl );
	obj["data"] = dataObj;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//WEBPAGE_EVENT_MAP CCxWebPageEvent::m_mapEvent;

CCxWebPageEvent::CCxWebPageEvent( int nModuleFlag, int nEventFlag )
{
	m_nModuleFlag = nModuleFlag;
	m_nEventFlag = nEventFlag;

	m_pMgr = NULL;
	m_pData = NULL;

	m_dwMainProcessId = 0;
	m_hWndMain = NULL;
	m_dwWebPageProcessId = 0;
	m_hWndWebPage = NULL;

	m_lpJsonObj = NULL;
	m_bAnswer = FALSE;
	m_bSync = FALSE;
}


CCxWebPageEvent::~CCxWebPageEvent(void)
{
	Clear();

	if ( m_pData != NULL )
	{
		delete m_pData; m_pData = NULL;
	}

	m_strId = "";
}

void CCxWebPageEvent::Clear( void )
{
	m_nModuleFlag = 0;
	m_nEventFlag = 0;

	m_strEventId = "";
	m_bAnswer = FALSE;
	m_bSync = FALSE;

	m_dwMainProcessId = 0;
	m_hWndMain = NULL;
	m_dwWebPageProcessId = 0;
	m_hWndWebPage = NULL;

	if ( m_lpJsonObj != NULL )
	{
		Json::Value * pObj = reinterpret_cast< Json::Value* >( m_lpJsonObj );
		if ( pObj->isObject() )
		{
			delete pObj; pObj = NULL;
		}
		else
		{
			::MessageBox( NULL ,"异常，m_lpJsonObj 指向非Json对象", g_lpszAppTitle, MB_OK|MB_ICONERROR);
		}
		m_lpJsonObj = NULL;
 	}

	if ( m_pData != NULL )
	{
		m_pData->Clear();
	}
}

BOOL CCxWebPageEvent::Parse( LPCTSTR lpszData )
{
	Clear();
	CString strSrc = lpszData == NULL ? "" : lpszData;
	if ( strSrc.IsEmpty() ) return FALSE;

	Json::Reader reader;
	Json::Value obj( Json::objectValue );
	try
	{
		if ( !reader.parse((char*)(LPCTSTR)strSrc, obj, false) ) return FALSE;
		if ( !obj.isObject() ) return FALSE;

		LPVOID lpData = reinterpret_cast< LPVOID > ( (Json::Value *)&obj );
		if ( !OnParsing( lpData ) )
		{
			Clear(); return FALSE;
		}
	}
	catch (...)
	{
		Clear(); return FALSE;
	}
	return TRUE;
}

CString CCxWebPageEvent::GetJsonStr( void )
{
	try
	{
		Json::Value obj( Json::objectValue );
		LPVOID lpData = reinterpret_cast< LPVOID > ( (Json::Value *)&obj );
		if ( !OnGetJsonStr( lpData ) ) return "";
		
		Json::FastWriter fast_writer;
		CString strResult = fast_writer.write( obj ).c_str();
		return strResult;
	}
	catch (...)
	{
		return "";
	}
}

BOOL CCxWebPageEvent::ParseExData( void )
{
	if ( m_pData == NULL ) return FALSE;
	m_pData->Clear();
	if ( !CanParseJsonObjPtr( m_lpJsonObj ) ) return FALSE;
	if ( !m_pData->OnParseJsonObj( m_lpJsonObj ) )
	{
		m_pData->Clear(); return FALSE;
	}
	return TRUE;
}

BOOL CCxWebPageEvent::OnParsing( LPVOID lpJsonObj )
{
	if ( !CanParseJsonObjPtr( lpJsonObj ) ) return FALSE;
	Json::Value& obj = *reinterpret_cast< Json::Value* >( lpJsonObj );
	CString str;
	try
	{
		str = ( obj["moduleFlag"].isString()) ? obj["moduleFlag"].asCString() : _T("");
		if ( !str.IsEmpty() )
			m_nModuleFlag = strtol( (char*)(LPCTSTR)str, NULL, 10 );

		str = ( obj["eventFlag"].isString()) ? obj["eventFlag"].asCString() : _T("");
		if ( !str.IsEmpty() )
			m_nEventFlag = strtol( (char*)(LPCTSTR)str, NULL, 10 );

		str = ( obj["mainProcessId"].isString()) ? obj["mainProcessId"].asCString() : _T("");
		if ( !str.IsEmpty() )
			m_dwMainProcessId = strtol( (char*)(LPCTSTR)str, NULL, 10 );

		str = ( obj["mainHwnd"].isString()) ? obj["mainHwnd"].asCString() : _T("");
		if ( !str.IsEmpty() )
			m_hWndMain = (HWND)strtol( (char*)(LPCTSTR)str, NULL, 10 );

		str = ( obj["webPageProcessId"].isString()) ? obj["webPageProcessId"].asCString() : _T("");
		if ( !str.IsEmpty() )
			m_dwWebPageProcessId = strtol( (char*)(LPCTSTR)str, NULL, 10 );

		str = ( obj["webPageHwnd"].isString()) ? obj["webPageHwnd"].asCString() : _T("");
		if ( !str.IsEmpty() )
			m_hWndWebPage = (HWND)strtol( (char*)(LPCTSTR)str, NULL, 10 );

		m_strEventId = ( obj["eventId"].isString() ) ? obj["eventId"].asCString() : _T("");

		str = ( obj["isAnswer"].isString()) ? obj["isAnswer"].asCString() : _T("");
		m_bAnswer = ( str.CompareNoCase( "true" ) == 0 );

		str = ( obj["isSync"].isString()) ? obj["isSync"].asCString() : _T("");
		m_bSync = ( str.CompareNoCase( "true" ) == 0 );

		if ( obj["data"].isObject() )
		{
			Json::Value * pData = new Json::Value( Json::objectValue );
			*pData = obj["data"];
			m_lpJsonObj = reinterpret_cast< LPVOID >( pData );

			if ( m_pData != NULL )
			{
				if ( !m_pData->OnParseJsonObj( m_lpJsonObj ) )
				{
					return FALSE;
				}
			}
		}
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CCxWebPageEvent::OnGetJsonStr( LPVOID lpJsonObj )
{
	if ( !CanParseJsonObjPtr( lpJsonObj ) ) return FALSE;
	Json::Value& obj = *reinterpret_cast< Json::Value * > ( lpJsonObj );
	CString str;
	
	str.Format( "%d", m_nModuleFlag );
	obj["moduleFlag"] = Json::Value( (LPCTSTR)str );

	str.Format( "%d", m_nEventFlag );
	obj["eventFlag"] = Json::Value( (LPCTSTR)str );

	str.Format( "%d", m_dwMainProcessId );
	obj["mainProcessId"] = Json::Value( (LPCTSTR)str );

	str.Format( "%d", m_hWndMain );
	obj["mainHwnd"] = Json::Value( (LPCTSTR)str );

	str.Format( "%d", m_dwWebPageProcessId );
	obj["webPageProcessId"] = Json::Value( (LPCTSTR)str );
	
	str.Format( "%d", m_hWndWebPage );
	obj["webPageHwnd"] = Json::Value( (LPCTSTR)str );

	obj["eventId"] = Json::Value( (LPCTSTR)m_strEventId );
	obj["isAnswer"] = m_bAnswer ? Json::Value( (LPCTSTR)"true" ) :  Json::Value( (LPCTSTR)"false" );
	obj["isSync"] = m_bSync ? Json::Value( (LPCTSTR)"true" ) :  Json::Value( (LPCTSTR)"false" );

	if ( m_pData != NULL )
	{
		if ( !m_pData->OnGetJsonStr( lpJsonObj ) )
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CCxWebPageEvent::CreateSyncEvent( void )
{
	if ( m_strEventId.IsEmpty() ) return FALSE;

	HANDLE hEvent = GetEventWin7( m_strEventId );
	if ( hEvent == NULL ) return FALSE;

	if ( !WaitEvent( hEvent, 60 * 30 * 1000, FALSE ) )
	{
		::CloseHandle( hEvent ); hEvent = NULL; return FALSE;
	}
	::CloseHandle( hEvent ); hEvent = NULL;
	return TRUE;
}

BOOL CCxWebPageEvent::AnswerSyncEvent( void )
{
	if ( m_strEventId.IsEmpty() ) return FALSE;

	HANDLE hEvent = GetEventWin7( m_strEventId );
	if ( hEvent == NULL ) return FALSE;
	
	if ( !::SetEvent( hEvent ) )
	{
		::CloseHandle( hEvent ); hEvent = NULL; return FALSE;
	}
	::CloseHandle( hEvent ); hEvent = NULL;
	return TRUE;
}

//WEBPAGE_EVENT_MAP::iterator CCxWebPageEvent::_Find( LPCTSTR lpszId )
//{
//	CString strId = lpszId == NULL ? "" : lpszId;
//	if ( strId.IsEmpty() ) return m_mapEvent.end();
//	return m_mapEvent.find( strId );
//}

//WEBPAGE_EVENT_MAP::iterator CCxWebPageEvent::_GetIter( void )
//{
//	for ( WEBPAGE_EVENT_MAP::iterator itr = m_mapEvent.begin();
//		itr != m_mapEvent.end(); ++itr )
//	{
//		if ( itr->second == this ) return itr;
//	}
//	return m_mapEvent.end();
//}

//CCxWebPageEvent* CCxWebPageEvent::Find( LPCTSTR lpszId )
//{
//	WEBPAGE_EVENT_MAP::iterator itr = _Find( lpszId );
//	if ( itr == m_mapEvent.end() ) return NULL;
//	else return itr->second;
//}

BOOL CCxWebPageEvent::SendCopyData( HWND hwndRecv )
{
	if ( !::IsWindow( hwndRecv ) ) return FALSE;
	CString strData = GetJsonStr();
	if ( strData.IsEmpty() ) return FALSE;

	DWORD dwLen = strData.GetLength() + 2;
	TCHAR * pBuf = new TCHAR [ dwLen ];
	memset( pBuf, NULL, sizeof(TCHAR) * dwLen );
	memcpy( pBuf, (LPVOID)(LPCTSTR)strData, strData.GetLength() );
	
	ZTools::WriteZToolsFormatLog( "CCxWebPageEvent::SendCopyData begin>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n %s", pBuf );
	::SendCopyData( hwndRecv, CPMSG_WEBPAGE, (LPVOID)pBuf, dwLen );
	ZTools::WriteZToolsFormatLog( "CCxWebPageEvent::SendCopyData end<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\r\n %s\r\n\r\n", pBuf );

	delete[] pBuf; pBuf = NULL;

	return TRUE;
}
