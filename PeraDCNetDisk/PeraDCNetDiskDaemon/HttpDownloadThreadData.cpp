#include "StdAfx.h"
#include "HttpDownloadThreadData.h"
#include "UtilityFunc.h"
#include "PeraNetDiskSetting.h"
#include "Tools.h"
#include "UserDefine.h"

CHttpDownloadThreadData::CHttpDownloadThreadData()
{
	m_bPause = FALSE;
	m_bStop = FALSE;

	m_dwThreadId = 0;
	m_hThread = NULL;

	m_sId = GetNewGUID();
	m_pMgr = NULL;
//	m_lpExData = NULL;
	m_dwStatus = S_NULL;
	m_pMultiCurl = NULL;
	m_pEasyCurl = NULL;
	m_pLocalFile = NULL;
	m_llWrittenSize = 0;
	m_pCurlErrBuf = NULL;
	m_bErrBuf = FALSE;
	m_pHeader = NULL;
	m_bHeader = FALSE;
	m_bProgress = FALSE;

	m_dProgressTotal = 0.0f;
	m_dProgressNow = 0.0f;

	m_llTotalSize = 0;
	m_llCurSize = 0;

	m_hEventRun = NULL;
	m_hEventExit = NULL;

	m_bBreakPointDown = false;
	m_strErrMsg.Empty();
}

CHttpDownloadThreadData::~CHttpDownloadThreadData()
{
	Clear();
}

void CHttpDownloadThreadData::ClearThreadData( void )
{
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsFormatLog( "CHttpDownloadThreadData::ClearThreadData [%d] [%d]\n " , m_dwStatus , m_dwThreadId );
#endif

	if ( m_hThread != NULL )
	{
		CloseHandle( m_hThread ); 
		
		m_hThread = NULL;
	}

	m_dwThreadId = 0;
	
	m_bPause = FALSE;

	m_bStop = FALSE;

	if ( m_hEventRun )
	{
		CloseHandle( m_hEventRun );

		m_hEventRun = NULL;
	}
	if ( m_hEventExit )
	{
		CloseHandle( m_hEventExit );

		m_hEventExit = NULL;
	}

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsFormatLog( "CHttpDownloadThreadData::ClearThreadData Finished [%d] [%d]\n " , m_dwStatus , m_dwThreadId );
#endif
}

void CHttpDownloadThreadData::Clear( void )
{
	if ( m_hEventRun )
	{
		CloseHandle( m_hEventRun );

		m_hEventRun = NULL;
	}
	if ( m_hEventExit )
	{
		CloseHandle( m_hEventExit );

		m_hEventExit = NULL;
	}
}

long long CHttpDownloadThreadData::ConvertDouble2LongLong( double dValue )
{
	CString str = ConvertDouble2Str( dValue );
	if ( str.IsEmpty() ) return 0;
	long long llValue = _atoi64( (char*)(LPCTSTR)str );
	return llValue;
}

CString CHttpDownloadThreadData::ConvertLongLong2Str( long long llValue )
{
	CString sResult;

	static const int BufSize = 128;
	char pBuf[ BufSize ] = {NULL};
	errno_t nErr = _i64toa_s( llValue, pBuf, BufSize, 10 );
	if ( nErr != 0 ) return "";
	if ( pBuf == NULL ) return 0;
	sResult = pBuf;
	return sResult;
}

CString CHttpDownloadThreadData::ConvertDouble2Str( double dValue )
{
	CString str;
	str.Format( "%.0lf", dValue );
	return str;
}

long long CHttpDownloadThreadData::ConvertStr2LongLong( LPCTSTR lpszValue )
{
	CString sSrc = lpszValue == NULL ? "" : lpszValue;
	if ( sSrc.IsEmpty() ) return -1;
	return _strtoi64( (char*)(LPCTSTR)sSrc, NULL, 10 );
}

CHttpDownloadMgr* CHttpDownloadThreadData::GetMgr( void ) const
{
	return m_pMgr;
}

BOOL CHttpDownloadThreadData::SetStatusNull()
{
	m_dwStatus = S_NULL;
	return TRUE;
}

BOOL CHttpDownloadThreadData::SetStatusWait( void )
{
	if ( IsError() ) return FALSE;

	if ( IsRunning() ) return FALSE;

	DWORD dwStatus = m_dwStatus;

	dwStatus &= ( ~S_PAUSE & ~S_STOP & ~S_FINISHED );

	dwStatus |= S_WAIT;

	m_dwStatus = dwStatus;

	OnStatusChanged();

	return TRUE;
}

BOOL CHttpDownloadThreadData::SetStatusRun( void )
{
	if ( IsError() ) return FALSE;

	if( IsStop() ) return FALSE;

	if ( IsPause() ) return FALSE;

	if ( IsFinished() ) return FALSE;

	DWORD dwStatus = m_dwStatus;

	dwStatus &= ~S_WAIT;

	dwStatus |= S_RUN;

	m_dwStatus = dwStatus;

	OnStatusChanged();

	return TRUE;
}

BOOL CHttpDownloadThreadData::SetStatusFinished( void )
{
	if ( IsError() ) return FALSE;

	if( IsWait() ) return FALSE;

	DWORD dwStatus = m_dwStatus;

	dwStatus &= ~S_RUN;

	dwStatus |= S_FINISHED;

	m_dwStatus = dwStatus;

	OnStatusChanged();

	return TRUE;
}

BOOL CHttpDownloadThreadData::SetStatusError( void )
{
	m_dwStatus = S_ERROR;

	OnStatusChanged();

	return TRUE;
}

BOOL CHttpDownloadThreadData::SetStatusPause( void )
{
	if ( IsError() ) return FALSE;

	if ( IsWait() ) return FALSE;

	if ( IsStop() ) return FALSE;

	DWORD dwStatus = m_dwStatus;

	dwStatus &= ~S_RUN;

	dwStatus |= S_PAUSE;

	m_dwStatus = dwStatus;

	OnStatusChanged();

	return TRUE;
}

BOOL CHttpDownloadThreadData::SetStatusStop( void )
{
	if ( IsError() ) return FALSE;

	if ( IsWait() ) return FALSE;

	DWORD dwStatus = m_dwStatus;

	dwStatus &= ( ~S_RUN & ~S_PAUSE );

	dwStatus |= S_STOP;

	m_dwStatus = dwStatus;

	OnStatusChanged();

	return TRUE;
}

void CHttpDownloadThreadData::OnStatusChanged()
{
	return ;
}
