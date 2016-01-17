#include "StdAfx.h"
#include "PeraDownloadThread.h"
#include "PeraNetDiskSetting.h"
#include "DownUploadCommonFunc.h"
#include "FileFunc.h"
#include "UtilityFunc.h"
#include "Tools.h"
#include "UserDefine.h"
#include "CPeraDataPersist.h"


extern CPeraNetDiskSetting theSetting;

CPeraDownloadThread::CPeraDownloadThread( CHttpDownloadMgr* pMgr )
	: CHttpDownloadThread( pMgr )
{
	m_pData = NULL;
}


CPeraDownloadThread::~CPeraDownloadThread(void)
{
	if (m_pData)
	{
		delete m_pData;
		m_pData = NULL;
	}
}

CString CPeraDownloadThread::GetUrlForDownload( void )
{
	return m_pData->GetUrl();
}

CString CPeraDownloadThread::GetLocalPathForDownload( void )
{
	return m_pData->m_sLocalPath;
}

CString CPeraDownloadThread::GetFileTotalSize(void)
{
	return m_pData->m_sFileSize;
}

CString CPeraDownloadThread::GetCopySourcePath(void)
{
	CString strCacheDir = theSetting.GetCacheDir();

	if ( strCacheDir.IsEmpty() )
	{
		GetCurrentDirectory( MAX_PATH*2, strCacheDir.GetBuffer( MAX_PATH*2 ) ); //获取当前目录也许不合时宜

		strCacheDir.ReleaseBuffer( );
	}

	if ( strCacheDir.GetAt( strCacheDir.GetLength() ) == '\\' )
	{
		strCacheDir = strCacheDir.Left( strCacheDir.GetLength()-1 );
	}

	strCacheDir += "\\";

	strCacheDir += m_pData->m_sUserName;

	strCacheDir += "\\";

	strCacheDir += m_pData->m_sFlowId;

	if ( m_pData->m_sNetDiskPath.GetAt( 0 ) != '\\')
	{
		strCacheDir += "\\";
	}

	strCacheDir += m_pData->m_sNetDiskPath;

	return strCacheDir;
}

BOOL CPeraDownloadThread::IsCopy( void )
{
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsFormatLog( "CPeraDownloadThread::IsCopy [%s][%d] [%d]\n", GetLocalPathForDownload(),m_dwStatus , m_dwThreadId );
#endif

	CString strCacheFile = GetCopySourcePath();

	WIN32_FIND_DATA FINDDATA = {0};

	HANDLE hFind = FindFirstFile( strCacheFile, &FINDDATA );

	if( hFind == INVALID_HANDLE_VALUE )
	{
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
		ZTools::WriteZToolsFormatLog( "CPeraDownloadThread::IsCopy Invalid File [%s][%d] [%d]\n", GetLocalPathForDownload(),m_dwStatus , m_dwThreadId );
#endif

		return FALSE;
	}

	FindClose( hFind );

	hFind = NULL;

	__int64 i64Filesize = ((__int64)FINDDATA.nFileSizeHigh * ((__int64)MAXDWORD+1)) + (__int64)FINDDATA.nFileSizeLow;

	__int64 i64SizeFrmSrv;

	sscanf( m_pData->m_sFileSize,"%I64d", &i64SizeFrmSrv );

	if ( i64Filesize != i64SizeFrmSrv )
	{
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
		ZTools::WriteZToolsFormatLog( "CPeraDownloadThread::IsCopy Filesize [%s][%d] [%d]\n", GetLocalPathForDownload(),m_dwStatus , m_dwThreadId );
#endif

		return FALSE;
	}

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsFormatLog( "CPeraDownloadThread::IsCopy GetFileMD5 [%s][%d] [%d]\n", GetLocalPathForDownload(),m_dwStatus , m_dwThreadId );
#endif

	::std::string strFileMD5 = GetFileMd5( (char*)(LPCTSTR)strCacheFile ); 

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsFormatLog( "CPeraDownloadThread::IsCopy GetFileMD5 Finished [%s][%d] [%d]\n", GetLocalPathForDownload(),m_dwStatus , m_dwThreadId );
#endif

	if ( m_pData->m_sFileMd5.CompareNoCase( strFileMD5.c_str() ) != 0)
	{
		return FALSE;
	}

	return TRUE;
}

CString CPeraDownloadThread::GetFileId()
{
	return m_pData->m_sFileId;
}

BOOL CPeraDownloadThread::ReNameExistFile()  //开始下载或拷贝前先判断是否有重名文件，如果有重名文件存在，改变任务中的文件名
{
#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsFormatLog( "CPeraDownloadThread::ReNameExistFile  [%s][%d ][%d]\n", GetLocalPathForDownload(),m_dwStatus , m_dwThreadId );
#endif

	CString sLocalPath =  GetLocalPathForDownload();

	CString strCopy, strFormat, strDirectory, strName, strNumb, strCompare;

	int nFile = 1;

	while ( IsAccessibleFile( sLocalPath ) )
	{

		char szDriver [3] ={0};
		char szFolder [2048] ={0};
		char szName [1024] ={0};
		char szExt [256] ={0};

		strNumb.Empty();
		_splitpath_s(sLocalPath,szDriver,szFolder,szName,szExt);
		strDirectory = CString(szDriver) + szFolder;
		strName   = szName;
		strFormat = szExt;
		if (!strName.IsEmpty() && strName.Right(1)==")")
		{
			//文件名末尾有可能有数字编号
			int iPos = strName.ReverseFind('(');
			if (iPos >=0)
			{
				strNumb = strName.Mid(iPos+1,strName.ReverseFind(')') - iPos -1);
			}

			if (!strNumb.IsEmpty())
			{
				nFile = atoi( strNumb );
				CString strTmp = strName.Left(strName.ReverseFind('('));
				if (!strTmp.IsEmpty()) // 名称不是"(1)"这样的括号开头结尾，中间是数字的形式
				{
					nFile++;
					strName = strTmp;
				}
			}
		}

		strNumb.Format( "%d", nFile );

		sLocalPath = strDirectory + strName + "(" + strNumb + ")" + strFormat;
	}

	m_pData->m_sLocalPath = sLocalPath;

#if DBG_LOG_OUTPUTSTRING_DWONLOAD
	ZTools::WriteZToolsFormatLog( "CPeraDownloadThread::ReNameExistFile Finished [%s][%d] [%d]\n", GetLocalPathForDownload(),m_dwStatus , m_dwThreadId );
#endif

	return TRUE;
}


/*
此函数在不能确定错误的情况下均对未确定的情况返回真
*/
bool CPeraDownloadThread::CheckFileType( const curl_slist* m_pHeader )
{
	char* pTemp = strrchr( (char *)(LPCTSTR)m_pData->m_sNetDiskPath, '.' );

	if ( pTemp == NULL )
	{
		return true;
	}

	char* pFileFormat = NULL;

	if ( strlen( pTemp ) > 1 )
	{
		pFileFormat = pTemp + 1;
	}
	else
	{
		return true;
	}

	if( pFileFormat == NULL ) return true; // exe   jpg   rar

	char szCatchHttp[] = {"HTTP"};

	char szCatchTitle[] = { "filename" };

	char szCatchFileLength[] = {"Content-Length"};  //协议中定义的固定名称

	curl_slist* m_pHeaderCopy = (curl_slist*)m_pHeader;

	curl_slist* m_pHeaderTemp = (curl_slist*)m_pHeader;

	BOOL bHttpOk = TRUE;

	CString strData, strHttpRetCode, strLength;

	if ( m_pHeaderCopy->next !=NULL )
	{
		strData = m_pHeaderCopy->next->data;
		
		AfxExtractSubString( strHttpRetCode, strData, 1, ' ' );

		if ( atoi( strHttpRetCode ) > 300 )
		{
			ZTools::WriteZToolsFormatLog("------CPeraDownloadThread::CheckFileType NetPageErr[%s]------\n", strData );

			AfxMessageBox( "异常，下载页面错误!" );

			return false;
		}
	}

	CString strCatchFileLen = szCatchFileLength;
	strCatchFileLen.MakeUpper();

	CString strCatchTitle = szCatchTitle;
	strCatchTitle.MakeUpper();

	CString strFormat = pFileFormat;
	strFormat.MakeUpper();

	while ( m_pHeaderTemp != NULL )
	{
		strData.Empty();
		strData = m_pHeaderTemp->data;
		strData.MakeUpper();

		if ( strData.Find( strCatchFileLen ) >= 0 )
		{
			AfxExtractSubString( strLength, strData, 1, ':' );

			__int64 i64FileLength = 0, i64FileSize = 0;

			sscanf( strLength, "%I64d", &i64FileLength );

			sscanf( m_pData->m_sFileSize, "%I64d", &i64FileSize );

			if ( i64FileSize != ( i64FileLength + m_llCurSize ) )
			{
				ZTools::WriteZToolsFormatLog("------CPeraDownloadThread::CheckFileType FileLength[%s][%s][%I64d]------\n", strData, m_pData->m_sFileSize, i64FileLength + m_llCurSize  );

				bHttpOk = FALSE;

				AfxMessageBox("下载异常，文件大小不一致");
			}
		}

		if ( strData.Find( strCatchTitle ) >= 0)
		{
			if ( strData.Find( strFormat ) == -1 )
			{
				ZTools::WriteZToolsFormatLog("------CPeraDownloadThread::CheckFileType FileFormat[%s][%s]------\n", strData, pFileFormat );

				bHttpOk = FALSE;

				AfxMessageBox("下载异常，下载文件类型不匹配");
			}
		}

		m_pHeaderTemp = m_pHeaderTemp->next;
	}
	
	if ( !bHttpOk  )
	{
		return false;
	}

	return true;
}

CString CPeraDownloadThread::GetidPath()
{
	return m_pData->m_sIdPath;
}


void CPeraDownloadThread::OnStatusChanged()
{
	CTaskData* pTaskData = new CTaskData;

	pTaskData->From( m_pData );

	pTaskData->SetStatus( m_dwStatus );

	pTaskData->SetDelFlag( FALSE );

	thePersistMrg.DoOperation( pTaskData );

	delete pTaskData; pTaskData = NULL;
}

CPeraDownloadThread* CPeraDownloadThread::GetCopied()
{
	CPeraDownloadThread* pNew = new CPeraDownloadThread( this->m_pMgr );
	pNew->m_pData = m_pData->GetCopied();
	pNew->m_dProgressNow = m_dProgressNow;
	pNew->m_dwStatus = m_dwStatus;
	pNew->m_strErrMsg = m_strErrMsg;
	return pNew;
}

void CPeraDownloadThread::ClearAllData()
{
	Release();
	m_bBreakPointDown = false;
}

void CPeraDownloadThread::SetBreakPointDownload()
{
	if ( WaitForSingleObject( m_hEventRun, 0 ) != WAIT_OBJECT_0 )
	{
		m_bBreakPointDown = false;
		return;
	}
	if ( WaitForSingleObject( m_hEventExit, 0 ) == WAIT_OBJECT_0 )
	{
		m_bBreakPointDown = true;
	}
}