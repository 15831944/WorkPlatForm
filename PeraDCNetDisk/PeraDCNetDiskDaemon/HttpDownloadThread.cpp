#include "StdAfx.h"
#include "HttpDownloadThread.h"
#include "FileFunc.h"
#include "HttpDownloadMgr.h"
#include <io.h>
#include "UtilityFunc.h"
#include "Tools.h"
#include "UserDefine.h"


CHttpDownloadThread::CHttpDownloadThread( CHttpDownloadMgr* pMgr )
{
	m_pMgr = pMgr;
}

CHttpDownloadThread::~CHttpDownloadThread(void)
{
}


CString CHttpDownloadThread::GetUrlForDownload( void )
{
	return "";
}


CString CHttpDownloadThread::GetLocalPathForDownload( void )
{
	return "";
}

CString CHttpDownloadThread::GetFileTotalSize(void)
{
	return "";
}

long long CHttpDownloadThread::GetFileTotalSizeByCurl( void )
{
#ifdef _CURL_MODE_MULTY

#else

	long long llFileSize = 0;
	double dFileSize = 0.0f;
	CURL* pCurl = NULL;
	pCurl  = curl_easy_init();
	curl_easy_setopt( pCurl, CURLOPT_URL, (char*)(LPCTSTR)GetUrlForDownload() );
	curl_easy_setopt(pCurl, CURLOPT_HEADER, 1L);
	curl_easy_setopt(pCurl, CURLOPT_NOBODY, 1L);
	if ( curl_easy_perform(pCurl) == CURLE_OK )
	{
		curl_easy_getinfo( pCurl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &dFileSize );
		llFileSize = ConvertDouble2LongLong( dFileSize );
	}
	else
	{
		llFileSize = -1;
	}
	curl_easy_cleanup( pCurl );
	return llFileSize;

#endif

	return -1;
}

long long CHttpDownloadThread::GetLocalFileRealSize( void )
{
	CString sPath = GetLocalPathForDownload();
	long long llFileSize = 0;
	FILE* fp = fopen((char*)(LPCTSTR)sPath, "rb");
	if ( fp != NULL )
	{
		llFileSize = filelength( fileno(fp) );
		fclose( fp );
	}
	
	return llFileSize;
}

BOOL CHttpDownloadThread::CheckReDownload( void )
{
	return (!m_bPause);
}

BOOL CHttpDownloadThread::IsCopy( void )
{
	return FALSE;
}

BOOL CHttpDownloadThread::ReNameExistFile()
{
	return FALSE;
}

CString CHttpDownloadThread::GetCopySourcePath( void )
{
	return "";
}

bool CHttpDownloadThread::CheckFileType( const  curl_slist* m_pHeader )
{
	return true;
}

CString CHttpDownloadThread::GetFileId()
{
	return "";
}

CString CHttpDownloadThread::GetidPath()
{
	return "";
}

void CHttpDownloadThread::OnStatusChanged()
{
	return ;
}

/*
初始化线程数据。
在启动下载线程之前调用。
*/
BOOL CHttpDownloadThread::Init( void )
{
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::Init Begin [%s][%d]\n ", GetLocalPathForDownload(), m_dwStatus ) ;
#endif

	m_bHeader = TRUE;
	m_bProgress = TRUE;

	CString sLocal = GetLocalPathForDownload();
	//是否需要重新下载

	//curl 错误缓冲区
	if ( m_pCurlErrBuf != NULL )
	{
		delete m_pCurlErrBuf; m_pCurlErrBuf = NULL;
	}
	if ( m_bErrBuf )
	{
		m_pCurlErrBuf = new char[ CURL_ERROR_SIZE + 1 ];
		memset( (void*)m_pCurlErrBuf, NULL, CURL_ERROR_SIZE + 1 );
	}

	//Header
	curl_slist_free_all( m_pHeader );
	m_pHeader = NULL;
	if ( m_bHeader )
	{
		m_pHeader = curl_slist_append(m_pHeader, "");
	}

#ifdef _CURL_MODE_MULTY

	m_pEasyCurl = curl_easy_init();
	curl_easy_setopt(m_pEasyCurl, CURLOPT_WRITEFUNCTION, CHttpDownloadThread::OnCurlWriteFile);

	//
	curl_easy_setopt( m_pEasyCurl, CURLOPT_URL, GetUrlForDownload() );

	m_pMultiCurl = curl_multi_init();
	//
	curl_multi_add_handle(m_pMultiCurl, m_pEasyCurl);

#else

	m_pEasyCurl = curl_easy_init();

	if ( m_bErrBuf )
	{
		curl_easy_setopt( m_pEasyCurl, CURLOPT_VERBOSE, 1 );
	}

	//设置错误缓冲区
	if ( m_bErrBuf )
	{
		curl_easy_setopt( m_pEasyCurl, CURLOPT_ERRORBUFFER, m_pCurlErrBuf );
	}

	curl_easy_setopt( m_pEasyCurl, CURLOPT_LOW_SPEED_LIMIT, 1L);
	curl_easy_setopt( m_pEasyCurl, CURLOPT_LOW_SPEED_TIME, 60L);

	//设置Header
	if ( m_bHeader )
	{
		//curl_easy_setopt( m_pEasyCurl, CURLOPT_HEADER, 1 );
		curl_easy_setopt( m_pEasyCurl, CURLOPT_HEADERDATA, (void *)this );
		curl_easy_setopt( m_pEasyCurl, CURLOPT_HEADERFUNCTION, CHttpDownloadThread::OnCurlWriteHeader );
	}

	if ( m_pMgr->m_dwConnectTimeout > 0 ) //设置连接超时
	{
		curl_easy_setopt( m_pEasyCurl, CURLOPT_CONNECTTIMEOUT, m_pMgr->m_dwConnectTimeout );
	}

	if ( m_pMgr->m_dwMaxSpeed > 0) //设置最大下载速度
	{
		curl_easy_setopt(m_pEasyCurl, CURLOPT_MAX_RECV_SPEED_LARGE, (curl_off_t) m_pMgr->m_dwMaxSpeed * 1024); 
	}

	//设置由服务器返回进度信息
	if ( m_bProgress )
	{
		curl_easy_setopt( m_pEasyCurl, CURLOPT_POST, false ); 
		curl_easy_setopt( m_pEasyCurl, CURLOPT_NOPROGRESS, false );
		curl_easy_setopt( m_pEasyCurl, CURLOPT_PROGRESSDATA, (void *)this );
		curl_easy_setopt( m_pEasyCurl, CURLOPT_PROGRESSFUNCTION, CHttpDownloadThread::OnCurlProgress );
	}

	//设置下载缓冲区尺寸
	if ( m_pMgr->m_llMaxBufferSizeForDownload > 0 )
	{
		curl_easy_setopt( m_pEasyCurl, CURLOPT_BUFFERSIZE
			, m_pMgr->m_llMaxBufferSizeForDownload );
	}

	curl_easy_setopt( m_pEasyCurl, CURLOPT_FOLLOWLOCATION
		, 1 );

	//////////////////////////////////////////////////////////////////////////

	//设置下载的原始地址（网络Url）
	curl_easy_setopt( m_pEasyCurl, CURLOPT_URL, GetUrlForDownload() );

	//取得文件总尺寸
	CString sTotalSize = GetFileTotalSize();
	if ( sTotalSize.IsEmpty() )
	{
		m_llTotalSize = GetFileTotalSizeByCurl();
	}
	else
	{
		m_llTotalSize = ConvertStr2LongLong( sTotalSize );
	}

	//设置写数据函数和文件句柄
	m_pLocalFile = CreateFile( sLocal,
								GENERIC_WRITE,
								FILE_SHARE_WRITE,
								NULL,
								OPEN_ALWAYS,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
	if ( m_pLocalFile == INVALID_HANDLE_VALUE )
	{
		ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::Init CreateFile Error[%d] \n", GetLastError() );
		curl_easy_cleanup( m_pEasyCurl );
		m_pEasyCurl = NULL;
		m_strErrMsg = "下载出错,创建文件失败";
		return FALSE;
	}

	SetFilePointer( m_pLocalFile, 0, NULL, FILE_END );

	curl_easy_setopt( m_pEasyCurl, CURLOPT_WRITEDATA, (void *)this );
	curl_easy_setopt( m_pEasyCurl, CURLOPT_WRITEFUNCTION, CHttpDownloadThread::OnCurlWriteFile );

	//断点续传
	curl_easy_setopt( m_pEasyCurl, CURLOPT_RESUME_FROM_LARGE
		, m_llCurSize );
	//curl_easy_setopt(handler, CURLOPT_RESUME_FROM, “500-999”)

	if ( m_llCurSize > m_llTotalSize ) //
	{
		ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::Init Error CurSize[%d] TotalSize[%d] \n ",m_llCurSize, m_llTotalSize ) ;

		return FALSE; //暂时还没有md5校验功能，断点续传存在问题
	}
	if ( m_llCurSize == m_llTotalSize ) //断点续传的逻辑
	{
	}

#endif

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::Init Finished [%s][%d]\n ", GetLocalPathForDownload(), m_dwStatus ) ;
#endif

	return TRUE;
}


/*
释放线程数据。
在线程函数 CHttpDownloadThread::_DownloadThread 返回后调用。
*/
BOOL CHttpDownloadThread::Release( void )
{
#ifdef _CURL_MODE_MULTY

#else

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::Release [%s][%d][%d]\n ", GetLocalPathForDownload(), m_dwStatus, m_dwThreadId );
#endif

	if ( m_pEasyCurl )
	{
		curl_easy_cleanup( m_pEasyCurl );

		m_pEasyCurl = NULL;
	}

	if ( m_pLocalFile != NULL )
	{
		CloseHandle(m_pLocalFile);

		m_pLocalFile = NULL;
	}

#endif

	if ( m_pCurlErrBuf != NULL )
	{
		delete[] m_pCurlErrBuf; m_pCurlErrBuf = NULL;
	}

	if ( m_pHeader )
	{
		curl_slist_free_all( m_pHeader );

		m_pHeader = NULL;
	}

	ClearThreadData();

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::Release Finished [%s][%d][%d]\n ", GetLocalPathForDownload(), m_dwStatus, m_dwThreadId );
#endif

	return TRUE;
}

/*
下载线程。
*/
DWORD WINAPI CHttpDownloadThread::_DownloadThread(LPVOID lpParameter)
{
	CHttpDownloadThread* pThis = (CHttpDownloadThread*)lpParameter;

	if ( pThis == NULL ) return 0;

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsFormatLog("------CHttpDownloadThread::_DownloadThread go in Path[%s] TheadId[%d] Status[%d]------\n",pThis->GetLocalPathForDownload(), pThis->m_dwThreadId, pThis->m_dwStatus );
#endif

	if ( !pThis->SetStatusRun() )
	{
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
		ZTools::WriteZToolsFormatLog("------CHttpDownloadThread::_DownloadThread SetStatusRun  Error m_dwStatus[%d] Path[%s] Id[%d]------\n",pThis->m_dwStatus, pThis->GetLocalPathForDownload(), pThis->m_dwThreadId);
#endif

		goto CHttpDownloadThread_GlobalModule_ClearWork;
	}

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsFormatLog("------CHttpDownloadThread::_DownloadThread SetStatusRun m_dwStatus[%d] Path[%s] Id[%d]------\n",pThis->m_dwStatus, pThis->GetLocalPathForDownload(), pThis->m_dwThreadId );
#endif

	SetEvent( pThis->m_hEventRun );

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsFormatLog("------CHttpDownloadThread::_DownloadThread go in Success[%s][%d][%d]------\n", pThis->GetLocalPathForDownload(), pThis->m_dwThreadId, pThis->m_dwStatus );
#endif

#if 0

	if ( pThis->IsCopy() )							//拷贝模块分支
	{
		DWORD dwReadByte = 0;						//每次读取字节数

		const DWORD dwMaxReadByte = 16 * 1024;		//每次读取最大字节数

		__int64 i64Filesize;						//未读取文件大小

		HANDLE hSource = NULL, hAim = NULL;			//拷贝源文件 和 目标文件

		char* pBuffer = NULL;						//读取文件缓存

		LARGE_INTEGER FileSize = {0}, RetFileSize = {0};

		sscanf( (char*)(LPCTSTR)pThis->GetFileTotalSize(),"%I64d",&i64Filesize );

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
		ZTools::WriteZToolsFormatLog("------CHttpDownloadThread::_DownloadThread Copy IsCopy [%s][%d][%d]------\n", pThis->GetLocalPathForDownload(),  pThis->m_dwThreadId, pThis->m_dwStatus );
#endif

		hSource = CreateFile( pThis->GetCopySourcePath(),
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL| FILE_FLAG_SEQUENTIAL_SCAN ,
			NULL );

		hAim = CreateFile( pThis->GetLocalPathForDownload(),
			GENERIC_WRITE,
			0,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL );

		if ( hSource == INVALID_HANDLE_VALUE || hAim == INVALID_HANDLE_VALUE )										
		{
			pThis->SetStatusError();

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
			ZTools::WriteZToolsFormatLog("------CHttpDownloadThread::_DownloadThread Copy Error[%d] [%d] [%d]------\n", GetLastError(), pThis->m_dwThreadId,pThis->m_dwStatus );
#endif

			goto CHttpDownloadThread_CopyModule_ClearWork;
		}

		GetFileSizeEx( hAim, &FileSize ); 

		SetFilePointerEx( hSource, FileSize, &RetFileSize, FILE_BEGIN );

		SetFilePointerEx( hAim, FileSize, &RetFileSize, FILE_BEGIN );

		pThis->m_dProgressTotal = (double)i64Filesize;

		pThis->m_dProgressNow = (double)FileSize.QuadPart;

		pBuffer = new char[dwMaxReadByte+64];

		if ( !pBuffer )
		{
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
			ZTools::WriteZToolsFormatLog("------CHttpDownloadThread::_DownloadThread Copy Err[%d] id[%d]------\n", GetLastError(), pThis->m_dwThreadId );
#endif

			pThis->SetStatusError();

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
			ZTools::WriteZToolsFormatLog("------CHttpDownloadThread::_DownloadThread Copy 1 Err[%d] id[%d]------\n", GetLastError(), pThis->m_dwThreadId );
#endif

			goto CHttpDownloadThread_CopyModule_ClearWork;
		}

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
		ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::_DownloadThread copy Write [%s][%d][%d]\n",pThis->GetLocalPathForDownload(), m_dwStatus, pThis->m_dwThreadId );
#endif

		while( !pThis->m_bStop && !pThis->m_bPause )
		{
			if ( i64Filesize <= 0)
			{
				goto CHttpDownloadThread_CopyModule_ClearWork;
			}

			if ( i64Filesize > dwMaxReadByte )
			{
				dwReadByte = dwMaxReadByte;
			}
			else
			{
				dwReadByte = (DWORD)i64Filesize;
			}

			ZeroMemory( pBuffer, dwMaxReadByte+64 );

			DWORD dwRealRByte = 0, dwRealWByte = 0;

			if ( !ReadFile( hSource, pBuffer, dwReadByte, &dwRealRByte, NULL ) )
			{
				goto CHttpDownloadThread_CopyModule_ClearWork;
			}

			if( dwRealRByte != dwReadByte )
			{
				goto CHttpDownloadThread_CopyModule_ClearWork;
			}

			if ( !WriteFile( hAim, pBuffer, dwRealRByte, &dwRealWByte, NULL ) )
			{
				goto CHttpDownloadThread_CopyModule_ClearWork;
			}
			if( dwRealWByte != dwRealRByte )
			{
				goto CHttpDownloadThread_CopyModule_ClearWork;
			}

			i64Filesize -= dwRealWByte;

			pThis->m_dProgressNow += dwRealWByte;
		}

CHttpDownloadThread_CopyModule_ClearWork:

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
		ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::_DownloadThread copy ClearWork [%s][%d][%d] \n",pThis->GetLocalPathForDownload(), pThis->m_dwStatus,pThis->m_dwThreadId );
#endif

		if ( hSource )
		{
			CloseHandle( hSource );

			hSource = NULL;
		}

		if ( hAim )
		{
			CloseHandle( hAim );

			hAim = NULL;
		}

		if ( pBuffer )
		{
			delete [] pBuffer;

			pBuffer = NULL;
		}

		if ( pThis->m_bStop )
		{
			pThis->SetStatusStop();

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
			ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::_DownloadThread Copy Stop [%s][%d][%d]\n", pThis->GetLocalPathForDownload(), m_dwStatus, pThis->m_dwThreadId );
#endif
		}

		if ( pThis->m_bPause )
		{
			pThis->SetStatusPause();

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
			ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::_DownloadThread Copy Pause [%s][%d][%d]\n",pThis->GetLocalPathForDownload(), m_dwStatus, pThis->m_dwThreadId );
#endif
		}

		if (!pThis->m_bPause && !pThis->m_bStop )
		{
			if ( i64Filesize != 0 )
			{
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
				ZTools::WriteZToolsFormatLog("------CHttpDownloadThread::_DownloadThread Copy  Failed[%d] Path[%s][%d][%d]------\n", GetLastError(), pThis->GetLocalPathForDownload(), m_dwStatus, pThis->m_dwThreadId );
#endif

				pThis->SetStatusError();

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
				ZTools::WriteZToolsFormatLog("------CHttpDownloadThread::_DownloadThread Copy 1 Failed[%d] Path[%s][%d][%d]------\n", GetLastError(), pThis->GetLocalPathForDownload(), m_dwStatus, pThis->m_dwThreadId );
#endif

			}
			else
			{
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
				ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::_DownloadThread Copy Finished [%s][%d][%d]\n",pThis->GetLocalPathForDownload(), m_dwStatus,pThis->m_dwThreadId );
#endif

				pThis->SetStatusFinished();

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
				ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::_DownloadThread Copy Finished  1 [%s][%d][%d]\n", pThis->GetLocalPathForDownload(), m_dwStatus, pThis->m_dwThreadId );
#endif

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
				ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::_DownloadThread Copy Finished 2 [%s][%d][%d]\n", pThis->GetLocalPathForDownload(), m_dwStatus, pThis->m_dwThreadId );
#endif
			}
		}
	}
	else   //下载模块分支
#endif
	{
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
		ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::_DownloadThread Download Begin [%s][%d][%d]\n", pThis->GetLocalPathForDownload(), pThis->m_dwStatus, pThis->m_dwThreadId );
#endif

		CURLcode nRet = CURLE_OK;

#if 0

		if ( !pThis->Init() )
		{
			nRet = CURLE_FAILED_INIT;

			goto CHttpDownloadThread_DownLoadModule_ClearWork;
		}

#endif

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
		ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::_DownloadThread Download Init Finished [%s][%d][%d]\n ",  pThis->GetLocalPathForDownload(), pThis->m_dwStatus, pThis->m_dwThreadId );
#endif

#ifdef _CURL_MODE_MULTY

#else

#if 0 //测试用区块方式去避开curl_easy_perform阻塞

		//nRet = curl_easy_perform(pThis->m_pEasyCurl);

		while ( pThis->m_llCurSize < pThis->m_llTotalSize )
		{
			CString sStart = pThis->ConvertLongLong2Str( pThis->m_llCurSize );
			CString sEnd = pThis->ConvertLongLong2Str( pThis->m_llCurSize 
				+ pThis->m_pMgr->m_llMaxBufferSizeForDownload );

			CString sRange;
			sRange.Format( "%s-%s",sStart, sEnd );

			//nRet = curl_easy_setopt( pThis->m_pEasyCurl, CURLOPT_RANGE, (char*)(LPCTSTR)sRange );
			nRet = curl_easy_setopt( pThis->m_pEasyCurl, CURLOPT_RESUME_FROM, (char*)(LPCTSTR)sRange );
			nRet = curl_easy_perform( pThis->m_pEasyCurl );

			//if ( nRet != CURLE_OK)
			//{
			//	break; //出错、暂停、停止等操作。
			//}
			//else
			{
				pThis->m_llCurSize += pThis->m_llWrittenSize;
			}
		}

#endif
		nRet = curl_easy_perform( pThis->m_pEasyCurl );
#endif

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
		ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::_DownloadThread Download ClearWork [%s][%d][%d]\n", pThis->GetLocalPathForDownload(), pThis->m_dwStatus,pThis->m_dwThreadId ) ;
#endif

		if ( pThis->m_pLocalFile != NULL )
		{
			CloseHandle( pThis->m_pLocalFile );

			pThis->m_pLocalFile = NULL;
		}

		if ( nRet == CURLE_OK )
		{
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
			ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::_DownloadThread Download Finished [%s][%d][%d]\n", pThis->GetLocalPathForDownload(), pThis->m_dwStatus, pThis->m_dwThreadId ) ;
#endif

			pThis->SetStatusFinished();

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
			ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::_DownloadThread Download Finished 1 [%s][%d][%d]\n", pThis->GetLocalPathForDownload(), pThis->m_dwStatus, pThis->m_dwThreadId) ;
#endif

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
			ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::_DownloadThread Download Finished 2 [%s][%d][%d]\n", pThis->GetLocalPathForDownload(), pThis->m_dwStatus, pThis->m_dwThreadId ) ;
#endif
		}
		else if (nRet == CURLE_COULDNT_CONNECT
			|| nRet==CURLE_RECV_ERROR || nRet==CURLE_OPERATION_TIMEDOUT )
		{
			ZTools::WriteZToolsFormatLog("网络异常:curl_easy_performf返回:%d,状态设置为stop，以便重试\n",nRet);
			pThis->SetStatusStop();
		}
		else
		{
			if ( pThis->m_bPause )
			{
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
				ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::_DownloadThread Download Pause [%s][%d][%d]\n", pThis->GetLocalPathForDownload(), pThis->m_dwStatus, pThis->m_dwThreadId ) ;
#endif

				pThis->SetStatusPause();
				
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
				ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::_DownloadThread Download Pause 1 [%s][%d][%d]\n", pThis->GetLocalPathForDownload(), pThis->m_dwStatus, pThis->m_dwThreadId ) ;
#endif

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
				ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::_DownloadThread Download Pause 2 [%s][%d][%d]\n", pThis->GetLocalPathForDownload(), pThis->m_dwStatus, pThis->m_dwThreadId ) ;
#endif
			}

			if ( pThis->m_bStop )
			{
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
				ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::_DownloadThread Download Stop [%s][%d][%d]\n", pThis->GetLocalPathForDownload(), pThis->m_dwStatus,pThis->m_dwThreadId ) ;
#endif

				pThis->SetStatusStop();

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
				ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::_DownloadThread Download Stop 1 [%s][%d][%d]\n", pThis->GetLocalPathForDownload(), pThis->m_dwStatus, pThis->m_dwThreadId ) ;
#endif

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
				ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::_DownloadThread Download Stop 2 [%s][%d][%d]\n", pThis->GetLocalPathForDownload(), pThis->m_dwStatus,pThis->m_dwThreadId ) ;
#endif
			}

			if( !pThis->m_bPause &&  !pThis->m_bStop )
			{
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
				ZTools::WriteZToolsFormatLog("------CHttpDownloadThread::_DownloadThread Finished with Error [%d] [%s][%d][%d]------\n", nRet, pThis->GetLocalPathForDownload(), pThis->m_dwStatus, pThis->m_dwThreadId );
#endif

				pThis->SetStatusError();

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
				ZTools::WriteZToolsFormatLog("------CHttpDownloadThread::_DownloadThread Finished 1 with Error [%d] [%s][%d][%d]------\n", nRet, pThis->GetLocalPathForDownload(), pThis->m_dwStatus, pThis->m_dwThreadId );
#endif

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
				ZTools::WriteZToolsFormatLog("------CHttpDownloadThread::_DownloadThread Finished 2 with Error [%d] [%s][%d][%d]------\n", nRet, pThis->GetLocalPathForDownload(), pThis->m_dwStatus, pThis->m_dwThreadId );
#endif
			}
		}
	}

CHttpDownloadThread_GlobalModule_ClearWork:

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::_DownloadThread SetEvent [%s][%d][%d]\n", pThis->GetLocalPathForDownload(), pThis->m_dwStatus, pThis->m_dwThreadId ) ;
#endif

	SetEvent( pThis->m_hEventExit );

	return 0;
}


/*
如果需要自己控制写文件，在初始化中设置此回调函数。
*/
size_t CHttpDownloadThread::OnCurlWriteFile( char *ptr, size_t size, size_t nmemb, void *userdata )
{
	CHttpDownloadThread* pThis = (CHttpDownloadThread*)userdata;
	if ( pThis == NULL ) return CURL_WRITEFUNC_PAUSE;
	if ( pThis->m_bPause || pThis->m_bStop )
	{
		return CURLPAUSE_RECV;
	}

#ifdef _CURL_MODE_MULTY


#else

	BOOL bWriteErr = FALSE;
	DWORD dwWriteByte = 0;

	__try
	{
		WriteFile( pThis->m_pLocalFile, ptr, size*nmemb, &dwWriteByte, NULL );

		pThis->m_llWrittenSize = (long long)dwWriteByte;
	}
	__except(1)
	{
		pThis->SetStatusError();

		ZTools::WriteZToolsFormatLog("------CHttpDownloadThread::OnCurlWriteFile Error [%d]------\n", GetLastError() );

		bWriteErr = TRUE;
	}

	if ( bWriteErr )
	{
		return CURLPAUSE_RECV ;
	}

	if ( pThis->m_llWrittenSize != (long long)(size * nmemb) )
	{
		AfxMessageBox("异常，下载过程中，发生写数据错误，CHttpDownloadThread::CurlWriteFile");
	}

	return ( size_t )pThis->m_llWrittenSize;

#endif		
	return 0;
}


/*
如果需要解析http-header，在初始化中设置此回调函数。
*/
size_t CHttpDownloadThread::OnCurlWriteHeader( void *ptr, size_t size, size_t nmemb, void *userdata )
{
	CHttpDownloadThread* pThis = (CHttpDownloadThread*)userdata;
	if ( pThis == NULL ) return 0;

	if ( pThis->m_bPause || pThis->m_bStop )
	{
		return CURLPAUSE_RECV;
	}
	if ( size * nmemb >= CURL_MAX_HTTP_HEADER )
	{
		AfxMessageBox("异常，Header数据长度越界");
		return 0;
	}

	curl_slist_append( pThis->m_pHeader, (char*)ptr );

	if ( !pThis->CheckFileType( pThis->m_pHeader ) )
	{
		return CURLPAUSE_RECV;
	}

	return ( size * nmemb ); //成功
}

int CHttpDownloadThread::OnCurlProgress( void *clientp, double dltotal, double dlnow, double ultotal, double ulnow )
{
	CHttpDownloadThread* pThis = (CHttpDownloadThread*)clientp;

	if ( pThis == NULL ) return 0;

	if ( pThis->m_bPause || pThis->m_bStop )
	{
		return CURLPAUSE_RECV;
	}

	if ((__int64)dlnow == 0)
	{
		pThis->m_dProgressNow = (double)pThis->m_llCurSize; // 断点续传
	}
	else
		pThis->m_dProgressNow  = dlnow + (double)pThis->m_llCurSize;

	if ( (__int64)dltotal == 0 )
	{
		pThis->m_dProgressTotal = (double)pThis->m_llTotalSize;
	}
	else
	{
		pThis->m_dProgressTotal = dltotal; 
	}
	return 0;
}


/*
启动（创建）一个下载线程。
非线程安全。仅由 CHttpDownloadMgr 类及其派生类调用。
*/
BOOL CHttpDownloadThread::Run( void )
{
	if ( IsError() ) return FALSE;
	if ( !IsWait() ) return FALSE;

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::Run Begin [%s][%d][%d]\n ", GetLocalPathForDownload(), m_dwStatus, m_dwThreadId );
#endif

	if ( GetFileId().IsEmpty() )
	{
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
		ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::Run CreateDirectory [%s][%d][%d]\n", GetLocalPathForDownload(), m_dwStatus, m_dwThreadId );
#endif

		CString strDir =  GetLocalPathForDownload();
		if ( !CreateMultiDirectoryEx(strDir, false ) )
		{
			CString strErrMsg;
			strErrMsg.Format("创建文件夹[%s]失败！",strDir);
			MessageBox(NULL,strErrMsg,"错误",MB_OK|MB_ICONERROR|MB_TOPMOST);
			return FALSE;
		}

		m_hEventExit = CreateEvent( NULL, TRUE, FALSE, GetNewGUID() );

		if ( !m_hEventExit )
		{
			AfxMessageBox("创建退出事件失败，CHttpDownloadThread::Run");
			return FALSE;
		}

		if ( !SetStatusRun() ) return FALSE;

		if ( !SetStatusFinished() ) return FALSE;

		SetEvent( m_hEventExit );

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
		ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::Run CreateDirectory Finished [%s][%d][%d]\n", GetLocalPathForDownload(), m_dwStatus, m_dwThreadId  );
#endif

		return TRUE;
	}
	if ( !m_bBreakPointDown )
	{
		ReNameExistFile(); 
	}

	if ( !CreateMultiDirectoryEx( GetLocalPathForDownload(), true ) )
	{
		return FALSE;
	}

	if ( !GetLocalFileSize() )
	{
		CString sFile = GetLocalPathForDownload();
		if ( PathFileExists( sFile ) )
		{
			DeleteFile( sFile );
		}
		return FALSE;
	}

	if ( !Init() )
	{
		//AfxMessageBox( "错误  initthread");
		ZTools::WriteZToolsFormatLog( "错误  initthread");
		return FALSE;
	}

	m_hEventRun = CreateEvent( NULL, TRUE, FALSE, GetNewGUID() );

	if ( !m_hEventRun )
	{
		AfxMessageBox("创建启动事件失败，CHttpDownloadThread::Run");
		return FALSE;
	}

	m_hEventExit = CreateEvent( NULL, TRUE, FALSE, GetNewGUID() );

	if ( !m_hEventExit )
	{
		AfxMessageBox("创建退出事件失败，CHttpDownloadThread::Run");
		return FALSE;
	}

	//创建控制线程
	m_hThread = ::CreateThread(
		NULL
		, 0
		, CHttpDownloadThread::_DownloadThread
		, (LPVOID)this
		, CREATE_SUSPENDED
		, &m_dwThreadId);

	if (m_hThread == NULL || m_dwThreadId == 0)
	{
		AfxMessageBox("创建线程失败，CHttpDownloadThread::_DownloadThread");
		return FALSE;
	}

	ResumeThread( m_hThread );

	if ( WaitForSingleObject( m_hEventRun, TIMEOUT_PERSIST_DOWNLOAD_THREAD_INIT_TIME ) != WAIT_OBJECT_0 )
	{
		AfxMessageBox("异常，等待启动事件超时");
		return FALSE;
	}

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsFormatLog( "CHttpDownloadThread::Run Finished [%s][%d] [%d]\n", GetLocalPathForDownload(), m_dwStatus , m_dwThreadId  );
#endif

	return TRUE;
}

/*
停止下载，主线程调用。
*/
BOOL CHttpDownloadThread::Stop( void )
{
	m_bStop = TRUE;

	if ( WaitForSingleObject( m_hEventRun, 0 ) != WAIT_OBJECT_0 )
	{
		return TRUE;
	}

#ifdef _CURL_MODE_MULTY

#else

	if ( WaitForSingleObject( m_hEventExit, TIMEOUT_PERSIST_DOWNLOAD_THREAD_INIT_TIME ) == WAIT_OBJECT_0 )
	{
		if ( IsStop() )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

#endif
	
	return TRUE;
}


/*
恢复下载，主线程调用。
*/
BOOL CHttpDownloadThread::Resume( void )
{
	if ( !IsPause() && !IsStop() )
	{
		return FALSE;
	}

	Release();

	m_bStop = FALSE;

	m_bPause = FALSE;

	m_dProgressNow = 0;

	m_dProgressTotal = 0;
	m_bBreakPointDown = true;
	SetStatusWait();

	return TRUE;
}

/*
暂停下载，主线程调用。
*/
BOOL CHttpDownloadThread::Pause()
{
	m_bPause = TRUE;

	if ( WaitForSingleObject( m_hEventRun, 0 ) != WAIT_OBJECT_0 )
	{
		return TRUE;
	}

#ifdef _CURL_MODE_MULTY

#else

	if ( WaitForSingleObject( m_hEventExit, TIMEOUT_PERSIST_DOWNLOAD_THREAD_INIT_TIME ) == WAIT_OBJECT_0 )
	{
		if ( IsPause() )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

#endif
	
	return TRUE;
}

BOOL CHttpDownloadThread::IsThreadRun()
{
	if ( WaitForSingleObject( m_hEventRun, 0 ) != WAIT_OBJECT_0 )
	{
		return FALSE;
	}
	if ( WaitForSingleObject( m_hEventExit, 0 ) != WAIT_OBJECT_0 )
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CHttpDownloadThread::GetLocalFileSize()
{
	CString sLocal = GetLocalPathForDownload();

	if ( !m_bBreakPointDown )
	{
// 		if ( IsAccessibleFile( sLocal ) )
// 		{
// 			DeleteFiles( sLocal );
// 		}
// 		HANDLE hFile = ::CreateFile( sLocal
// 			, GENERIC_WRITE
// 			, FILE_SHARE_WRITE
// 			, NULL
// 			, CREATE_ALWAYS
// 			, NULL
// 			, NULL );
// 
// 		if ( hFile == INVALID_HANDLE_VALUE )
// 		{
// 			ZTools::WriteZToolsFormatLog("------CHttpDownloadThread::Init 创建本地文件失败 Error[%d]------\n",GetLastError() );
// 			AfxMessageBox("创建本地文件失败");
// 			return FALSE;
// 		}
// 		CloseHandle( hFile );

		m_llCurSize = 0;
	}
	//不需要重新下载，断点续传
	else
	{
		m_llCurSize = GetLocalFileRealSize();
		m_bBreakPointDown = false; //判断使用完成之后，把变量恢复默认
	}

	return TRUE;
}

/*
创建多级目录。
参数1：目录绝对路径
参数2：文件为true，目录为false
*/
BOOL CHttpDownloadThread::CreateMultiDirectoryEx( const char* pszDir, bool bIsFile )
{
	CString strPath = pszDir;
	strPath.Replace("/","\\");
	if (strPath.IsEmpty()) return FALSE;
	if (!bIsFile)
	{
		if (strPath.Right(1)!="\\")
		{
			strPath.AppendChar('\\');
		}
	}
	return MakeSureDirectoryPathExists(strPath);

	//DWORD dwRet = 0;
	//char* pszDirectory = NULL;
	//char* pszName = NULL;
	//char* pszPath = NULL;
	//__try
	//{
	//	if ( PathIsRelative( pszDir ) || pszDir[0] == '\\' || pszDir[0] == '.' ) //判断相对路径判断不全
	//	{
	//		dwRet = 33;
	//		__leave;
	//	}
	//	if ( !pszDir || stricmp( pszDir, "" ) == 0 )
	//	{
	//		dwRet = 1;
	//		__leave;
	//	}
	//	pszDirectory = new char[MAX_PATH*2];
	//	pszName = new char[MAX_PATH*2];
	//	pszPath = new char[MAX_PATH*2];
	//	if ( !pszPath || !pszName || !pszDirectory )
	//	{
	//		dwRet = 2;
	//		__leave;
	//	}

	//	ZeroMemory( pszPath, MAX_PATH*2 );
	//	ZeroMemory( pszName, MAX_PATH*2 );
	//	ZeroMemory( pszDirectory, MAX_PATH*2 );

	//	strcpy( pszDirectory, pszDir );

	//	if ( pszDirectory[ strlen(pszDirectory)-1 ] == '\\' )
	//	{
	//		pszDirectory[ strlen(pszDirectory)-1 ] = '\0'; //去除尾部的反斜线
	//	}

	//	int iCount = 32; //多级目录不能超过32级，避免死循环

	//	while( iCount-- > 0 )
	//	{
	//		char* pTemp = strchr( pszDirectory , '\\' );
	//		if ( !pTemp && stricmp( pszDirectory, "" ) != 0 )
	//		{
	//			if ( !bIsFile )
	//			{
	//				if ( stricmp( pszName, "" ) != 0)
	//				{
	//					strcat( pszName, "\\");
	//				}
	//				strcat( pszName, pszDirectory );
	//				dwRet = (DWORD)!CreateDirectory( pszName, NULL );
	//			}
	//			__leave;
	//		}
	//		else if ( !pTemp )
	//		{
	//			__leave;
	//		}
	//		char szDirName[MAX_PATH*2] = {0};
	//		memcpy( szDirName, pszDirectory, strlen(pszDirectory)-strlen(pTemp) );
	//		if ( stricmp( pszName, "") != 0)
	//		{
	//			strcat( pszName, "\\" );
	//		}
	//		strcat( pszName, szDirName );
	//		ZeroMemory( pszPath, MAX_PATH*2 );
	//		memcpy( pszPath, &pszDirectory[ strlen( szDirName )+1 ], strlen( pszDirectory )-strlen( szDirName ) );
	//		ZeroMemory( pszDirectory, MAX_PATH*2 );
	//		strcpy( pszDirectory, pszPath );
	//		if ( !PathFileExists( pszName ) && !CreateDirectory( pszName, NULL ) )
	//		{
	//			dwRet = 3;
	//			__leave;
	//		}
	//	}
	//}
	//__except(1)
	//{
	//	dwRet = 100;
	//	ZTools::WriteZToolsFormatLog("------CHttpDownloadThread::CreateMultiDirectoryEx Error[%d][%d]------\n",GetLastError(), dwRet );
	//}
	//if ( pszName )
	//{
	//	ZTools::WriteZToolsFormatLog("------CHttpDownloadThread::CreateMultiDirectoryEx Path[%s][%s][%d]------\n", pszDir, pszName, dwRet );
	//}

	//if ( pszDirectory )
	//{
	//	delete [] pszDirectory;
	//	pszDirectory = NULL;
	//}
	//if ( pszName )
	//{
	//	delete [] pszName;
	//	pszName = NULL;
	//}
	//if ( pszPath )
	//{
	//	delete [] pszPath;
	//	pszPath = NULL;
	//}
	//return !dwRet;
}

