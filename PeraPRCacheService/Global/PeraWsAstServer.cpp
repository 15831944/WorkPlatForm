#include "StdAfx.h"
#include "PeraWsAstServer.h"
#include "InterfaceData.h"
#include <Strsafe.h>
#include "ZipTool.h"
#include "FileFunc.h"
#include "GetGlobalDataFromReg.h"
#include "Xpdl.h"
#include "FileOperate.h"



static LPCTSTR lpIceMethodName[6] = { "CreateCacheByWS", "CreateWSByCache", "CopyCache", "DeleteCache", "CreateCacheByXpdlStream", "CreateWSByXpdlStream" };

static BOOL DeleteDir(char * path);


CPeraWsAstServer::CPeraWsAstServer(void)
{
}


CPeraWsAstServer::~CPeraWsAstServer(void)
{
}

/*
缓存目录
*/
static CString GetCacheDirectory()
{
#if 0
	char szModule[MAX_PATH*2] = {0};
	GetModuleFileName( NULL, szModule, MAX_PATH*2 );
	if ( !PathFileExists( szModule ) )
	{
		return "";
	}
	char* pTemp = strrchr( szModule, '\\' );
	if ( pTemp == NULL )
	{
		return "";
	}
	szModule[ pTemp-szModule ] = '\0';
	StringCbCat( szModule, MAX_PATH*2, "\\RunCache" );
	if ( !PathFileExists( szModule ) )
	{
		if ( !MakeSureDirectoryPathExists( szModule ) )
		{
			return "";
		}
	}
	return szModule;
#else
	CString sCacheDirectory = GetPeraGlobalDataFromReg( "LM", "PeraPRCacheDir" );
	if ( MakeSureDirectoryPathExists( sCacheDirectory ) )
	{
	}
	return sCacheDirectory;
#endif
}


static BOOL CopyCacheFiles( LPCTSTR lpszForm, LPCTSTR lpszTo )
{
	if ( lpszForm == NULL || lpszTo == NULL )
	{
		return FALSE;
	}
	SHFILEOPSTRUCT ShFileOp = {0};
	ShFileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_SILENT;
	ShFileOp.wFunc = FO_COPY;
	char* pszFrom = new char[ MAX_PATH*2 ];
	char* pszTo = new char[ MAX_PATH*2 ];
	if ( pszTo == NULL || pszFrom == NULL )
	{
		return FALSE;
	}
	ZeroMemory( pszFrom, MAX_PATH*2 );//确保第二个是0
	ZeroMemory( pszTo, MAX_PATH*2 );
	StringCbCopy( pszFrom, MAX_PATH*2, lpszForm );
	StringCbCopy( pszTo, MAX_PATH*2, lpszTo );
	if ( !PathFileExists( pszFrom ) )
	{
		delete [] pszFrom; pszFrom = NULL;
		delete [] pszTo; pszTo = NULL;
		return FALSE;
	}
	ShFileOp.pFrom = pszFrom;
	ShFileOp.pTo = pszTo;
	int nRet = SHFileOperation( &ShFileOp );

	delete [] pszFrom; pszFrom = NULL;
	delete [] pszTo; pszTo = NULL;
	if ( nRet == 0 )
	{
		return TRUE;
	}
	ZTools::WriteZToolsFormatLog( "\n SHFileOperation [%d][%d] \n", nRet, GetLastError() );
	return FALSE;
}


/*
生成GUID
*/
::std::string CPeraWsAstServer::GetRunId( const ::Ice::Current& /*= ::Ice::Current()*/ )
{
	ZTools::WriteZToolsLog( "------------GetRunId 进入-------------------\n" );

	char buf[64] = {0};
	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		_snprintf_s(
			buf, sizeof(buf)
			, "{%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}"
			, guid.Data1
			, guid.Data2
			, guid.Data3
			, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3]
		, guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]
		);
	}
	CInterfaceData_CacheServer clsInter;
	clsInter.m_sRunId = buf;
	ZTools::WriteZToolsLog( "------------GetRunId 返回-------------------\n" );
	return (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_GETRUNID, TRUE );
}


/*
终止提交的任务
参数：由RunId组成的json对象
*/
::std::string CPeraWsAstServer::Stop( const ::std::string& sJsonStr, const ::Ice::Current& /*= ::Ice::Current()*/ )
{
	ZTools::WriteZToolsFormatLog( " --------------------Stop进入:[%s]--------------------- \n", sJsonStr.c_str() );
	CInterfaceData_CacheServer clsInter;
	if ( !clsInter.Parse( sJsonStr.c_str() ) )
	{
		clsInter.m_sErrMsg = "解析参数失败";
	}
	else
	{
		if ( !AddToTQueue( clsInter.m_sRunId, NULL ) )
		{
			clsInter.m_sErrMsg = "添加终止任务队列失败";
		}
	}
	ZTools::WriteZToolsFormatLog( "stop返回[%s]\n", clsInter.m_sRunId );
	return (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_STOP, TRUE );
}


/*
解压ws文件生成缓存目录
参数：
	1、RunId
	2、wsPath
	组成的json对象
返回：RunId、wsPath、ErrMsg组成的json对象
*/
void CPeraWsAstServer::CreateCacheByWS_async( const AMD_PeraWsAstICE_CreateCacheByWSPtr& pObj, const ::std::string& sJsonStr, const ::Ice::Current& /*= ::Ice::Current()*/ )
{
	ZTools::WriteZToolsFormatLog( " --------------------CreateCacheByWS进入:[%s]--------------------- \n", sJsonStr.c_str() );
	char szDirectory[ MAX_PATH*2 ] = {0};
	CInterfaceData_CacheServer clsInter;
	if ( !clsInter.Parse( sJsonStr.c_str() ) )
	{
		clsInter.m_sErrMsg = "解析参数失败";
	}
	else
	{
		if ( !clsInter.m_sErrMsg.IsEmpty() || clsInter.m_sRunId.IsEmpty() || clsInter.m_sWsPath.IsEmpty() )
		{
			clsInter.m_sErrMsg = "参数传值不正确";
		}
		else
		{
			if ( !PathFileExists( clsInter.m_sWsPath ) )
			{
				clsInter.m_sErrMsg = "指定路径ws文件不存在";
			}
			else
			{
				if ( FindFromTQueue( clsInter.m_sRunId, lpIceMethodName[0] ) )
				{
					if ( DelFromTQueue( clsInter.m_sRunId, lpIceMethodName[0] ) )
					{
						ZTools::WriteZToolsLog( " CreateCacheByWS_async DelFromTQueue Failed \n" );
					}
					clsInter.m_sErrMsg = "指定任务被终止运行";
				}
				else
				{
					if ( clsInter.m_sUnzipDir.IsEmpty() )
					{
						StringCbCopy( szDirectory, MAX_PATH*2, GetCacheDirectory() );
						StringCbCat( szDirectory, MAX_PATH*2, "\\");
						StringCbCat( szDirectory, MAX_PATH*2, clsInter.m_sRunId );
					}
					else
					{
						StringCbCopy( szDirectory, MAX_PATH*2, clsInter.m_sUnzipDir );
					}
					if ( PathFileExists( szDirectory ) )
					{
						clsInter.m_sErrMsg = "指定缓存目录已经存在";
					}
					else
					{
						if ( !AddToRQueue( clsInter.m_sRunId, lpIceMethodName[0] ) )
						{
							if ( FindFromDQueue( clsInter.m_sRunId ) )
							{
								clsInter.m_sErrMsg = "添加任务的文件正在被删除";
							}
							else
							{
								clsInter.m_sErrMsg = "添加运行队列失败";
							}
						}
						else
						{
							if ( !ZipTool::Unzip( (LPCTSTR)clsInter.m_sWsPath, (LPCTSTR)szDirectory ) ) 
							{
								clsInter.m_sErrMsg = "解压ws文件失败";
							}
							if ( !DelFromRQueue( clsInter.m_sRunId, lpIceMethodName[0] ) )
							{
								clsInter.m_sErrMsg += " 删除运行队列失败";
							}
						}
					}
				}
			}
		}
	}
	try
	{
		pObj->ice_response( (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_CREATECACHEBYWS, TRUE ) );
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog("CreateCacheByWS : Ice Exception [%s]\n", ex.ice_name().c_str() );
		if ( PathFileExists( szDirectory ) )
		{
			DeleteFiles( szDirectory );
		}
	}
	ZTools::WriteZToolsFormatLog("CreateCacheByWS执行返回 [%s]\n", clsInter.m_sRunId );
}


/*
压缩缓存目录成ws文件
参数：
	1、wsPath
	2、RunId
	组成的json对象
返回：RunId、wsPath、ErrMsg组成的json对象
*/
void CPeraWsAstServer::CreateWSByCache_async( const AMD_PeraWsAstICE_CreateWSByCachePtr& pObj, const ::std::string& sJsonStr, const ::Ice::Current& /*= ::Ice::Current()*/ )
{
	ZTools::WriteZToolsFormatLog( " --------------------CreateWSByCache进入:[%s]--------------------- \n", sJsonStr.c_str() );
	CInterfaceData_CacheServer clsInter;
	if ( !clsInter.Parse( sJsonStr.c_str() ) )
	{
		clsInter.m_sErrMsg = "解析参数失败";
	}
	else
	{
		if ( clsInter.m_sWsPath.IsEmpty() || clsInter.m_sRunId.IsEmpty() || !clsInter.m_sErrMsg.IsEmpty() )
		{
			clsInter.m_sErrMsg = "参数传值不正确";
		}
		else
		{
			if ( FindFromTQueue( clsInter.m_sRunId, lpIceMethodName[1] ) )
			{
				if ( DelFromTQueue( clsInter.m_sRunId, lpIceMethodName[1] ) )
				{
					ZTools::WriteZToolsLog( " CreateWSByCache_async DelFromTQueue Failed \n" );
				}
				clsInter.m_sErrMsg = "指定任务被终止运行";
			}
			else
			{
				char szDirectory[ MAX_PATH*2 ] = {0};
				StringCbCopy( szDirectory, MAX_PATH*2, GetCacheDirectory() );
				StringCbCat( szDirectory, MAX_PATH*2, "\\");
				StringCbCat( szDirectory, MAX_PATH*2, clsInter.m_sRunId );
				if ( !PathFileExists( szDirectory ) )
				{
					clsInter.m_sErrMsg = "指定缓存目录不存在";
				}
				else
				{
					if ( !AddToRQueue( clsInter.m_sRunId, lpIceMethodName[1] ) )
					{
						if ( FindFromDQueue( clsInter.m_sRunId ) )
						{
							clsInter.m_sErrMsg = "添加任务的文件正在被删除";
						}
						else
						{
							clsInter.m_sErrMsg = "添加运行队列失败";
						}
					}
					else
					{
						if ( !ZipTool::ZipFromDir((LPCTSTR)clsInter.m_sWsPath,  (LPCTSTR)szDirectory ) )
						{
							clsInter.m_sErrMsg = "压缩文件失败";
						}
						if ( !DelFromRQueue( clsInter.m_sRunId, lpIceMethodName[1] ) )
						{
							clsInter.m_sErrMsg += " 删除运行队列失败";
						}
					}
				}
			}
		}
	}
	try
	{
		pObj->ice_response( (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_CREATEWSBYCACHE, TRUE ) );
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog("CreateWSByCache : Ice Exception [%s]\n", ex.ice_name().c_str() );
	}
	ZTools::WriteZToolsFormatLog("CreateWSByCache执行返回 [%s]\n", clsInter.m_sRunId );
}


/*
拷贝缓存
参数：
	1、RunId_Org
	2、RunId_New
	组成的json对象
返回：RunId_New ErrMsg组成的json对象
*/
void CPeraWsAstServer::CopyCache_async( const AMD_PeraWsAstICE_CopyCachePtr& pObj, const ::std::string& sJsonStr, const ::Ice::Current& /*= ::Ice::Current()*/ )
{
	ZTools::WriteZToolsFormatLog( " --------------------CopyCache进入:[%s]--------------------- \n", sJsonStr.c_str() );
	CInterfaceData_CacheServer clsInter;
	if ( !clsInter.Parse( sJsonStr.c_str() ) )
	{
		clsInter.m_sErrMsg = "解析参数失败";
	}
	else
	{
		if ( clsInter.m_sCacheDirId.IsEmpty() || clsInter.m_sRunId.IsEmpty() || !clsInter.m_sErrMsg.IsEmpty() )
		{
			clsInter.m_sErrMsg = "参数传值不正确";
		}
		else
		{
			if ( FindFromTQueue( clsInter.m_sRunId, lpIceMethodName[2] ) )
			{
				if ( DelFromTQueue( clsInter.m_sRunId, lpIceMethodName[2] ) )
				{
					ZTools::WriteZToolsLog( " CopyCache_async DelFromTQueue Failed \n" );
				}
				clsInter.m_sErrMsg = "指定任务被终止运行";
			}
			else
			{
				char szOrgDirectory[ MAX_PATH*2 ] = {0}, szNewDirectory[ MAX_PATH*2 ] = {0};

				StringCbCopy( szOrgDirectory, MAX_PATH*2, GetCacheDirectory() );
				StringCbCat( szOrgDirectory, MAX_PATH*2, "\\");
				StringCbCat( szOrgDirectory, MAX_PATH*2, clsInter.m_sCacheDirId );

				StringCbCopy( szNewDirectory, MAX_PATH*2, GetCacheDirectory() );
				StringCbCat( szNewDirectory, MAX_PATH*2, "\\" );
				StringCbCat( szNewDirectory, MAX_PATH*2, clsInter.m_sRunId );

				if ( !PathFileExists( szOrgDirectory ) )
				{
					clsInter.m_sErrMsg = "指定源缓存目录不存在";
				}
				else
				{
					if ( !AddToRQueue( clsInter.m_sRunId, lpIceMethodName[2] ) )
					{
						if ( FindFromDQueue( clsInter.m_sRunId ) )
						{
							clsInter.m_sErrMsg = "添加任务的文件正在被删除";
						}
						else
						{
							clsInter.m_sErrMsg = "添加运行队列失败";
						}
					}
					else
					{
						if ( !CopyCacheFiles( szOrgDirectory, szNewDirectory ) )
						{
							clsInter.m_sErrMsg = "拷贝缓存目录失败";
						}
						if ( !DelFromRQueue( clsInter.m_sRunId, lpIceMethodName[2] ) )
						{
							clsInter.m_sErrMsg += " 删除运行队列失败";
						}
					}
				}
			}
		}
	}
	try
	{
		pObj->ice_response( (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_COPYCACHE, TRUE ) );
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog("CopyCache : Ice Exception [%s]\n", ex.ice_name().c_str() );
	}
	ZTools::WriteZToolsFormatLog("CopyCache执行返回 [%s]\n", clsInter.m_sRunId );
}


/*
删除缓存
参数：
	RunId json对象
返回：RunId、ErrMsg组成的json对象
*/
void CPeraWsAstServer::DeleteCache_async( const AMD_PeraWsAstICE_DeleteCachePtr& pObj, const ::std::string& sJsonStr, const ::Ice::Current& /*= ::Ice::Current()*/ )
{
	ZTools::WriteZToolsFormatLog( " --------------------DeleteCache进入:[%s]--------------------- \n", sJsonStr.c_str() );
	CInterfaceData_CacheServer clsInter;
	if ( !clsInter.Parse( sJsonStr.c_str() ) )
	{
		clsInter.m_sErrMsg = "解析参数失败";
	}
	else
	{
		if ( clsInter.m_sRunId.IsEmpty()|| !clsInter.m_sErrMsg.IsEmpty() )
		{
			clsInter.m_sErrMsg = "参数传值不正确";
		}
		else
		{
			char szDirectory[ MAX_PATH*2 ] = {0};

			StringCbCopy( szDirectory, MAX_PATH*2, GetCacheDirectory() );
			StringCbCat( szDirectory, MAX_PATH*2, "\\");
			StringCbCat( szDirectory, MAX_PATH*2, clsInter.m_sRunId );

			if ( !PathFileExists( szDirectory ) )
			{
				clsInter.m_sErrMsg = "指定缓存目录不存在";
			}
			else
			{
				while( 1 )
				{
					if (  !AddToDQueue( clsInter.m_sRunId ) )
					{
						if ( FindFromRQueue( clsInter.m_sRunId, NULL ) )
						{
							Sleep( 100 );
							continue;
						}
						else
						{
							clsInter.m_sErrMsg = "添加清除任务队列失败";
							break;
						}
					}
					//DeleteFiles( szDirectory );
					DeleteDir( szDirectory );
					if ( !DelFromDQueue( clsInter.m_sRunId ) )
					{
						clsInter.m_sErrMsg += " 删除清除任务队列失败";
					}
					break;
				}
			}
		}
	}
	try
	{
		pObj->ice_response( (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_DELETECACHE, TRUE ) );
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog("DeleteCache : Ice Exception [%s]\n", ex.ice_name().c_str() );
	}
	ZTools::WriteZToolsFormatLog("DeleteCache执行返回 [%s]\n", clsInter.m_sRunId );
}


/*
根据xpdl流创建缓存目录
参数：
	1、xpdlstream
	2、RunId
	组成的json对象
返回：runid、errmsg组成的json对象
*/
void CPeraWsAstServer::CreateCacheByXpdlStream_async( const AMD_PeraWsAstICE_CreateCacheByXpdlStreamPtr& pObj, const ::std::string& sJsonStr, const ::Ice::Current& /*= ::Ice::Current()*/ )
{
	ZTools::WriteZToolsFormatLog( " --------------------CreateCacheByXpdlStream进入:[%s]--------------------- \n", sJsonStr.c_str() );
	CInterfaceData_CacheServer clsInter;
	if ( !clsInter.Parse( sJsonStr.c_str() ) )
	{
		clsInter.m_sErrMsg = "解析参数失败";
	}
	else
	{
		if ( clsInter.m_sXpdlStream.IsEmpty() || clsInter.m_sRunId.IsEmpty()|| !clsInter.m_sErrMsg.IsEmpty() )
		{
			clsInter.m_sErrMsg = "参数传值不正确";
		}
		else
		{
			if ( !AddToRQueue( clsInter.m_sRunId, lpIceMethodName[4] ) )
			{
				clsInter.m_sErrMsg = "添加运行队列失败";
			}
			CXpdl Xpdl;

			std::string sStream = (char*)(LPCTSTR)clsInter.m_sXpdlStream;

			sStream = ZTools::MBToUTF8( sStream );

			if( Xpdl.LoadStream( sStream.c_str(), GetCacheDirectory()+ "\\" + clsInter.m_sRunId+ "\\" + "main.xpdl" ) )
			{
				if ( Xpdl.Parse() )
				{
					if ( !Xpdl.CreateCache( GetCacheDirectory()+ "\\" + clsInter.m_sRunId ) )
					{
						clsInter.m_sErrMsg = "生成本地缓存失败";
					}
				}
				else
				{
					clsInter.m_sErrMsg = "解析xpdl失败";
				}
			}
			else
			{
				clsInter.m_sErrMsg = "加载xpdl失败";
			}


			if ( !DelFromRQueue( clsInter.m_sRunId, lpIceMethodName[4] ) )
			{
				clsInter.m_sErrMsg = "删除运行队列失败";
			}
		}
	}
	try
	{
		pObj->ice_response( (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_CREATECACHEBYXPDLSTREAM, TRUE ) );
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog("CreateCacheByXpdlStream : Ice Exception [%s]\n", ex.ice_name().c_str() );
	}
	ZTools::WriteZToolsFormatLog("CreateCacheByXpdlStream 执行返回 [%s]\n", clsInter.m_sRunId );
}


/*
根据xpdlstream生成ws文件
参数：
	1、xpdlstream
	2、wsPath
	3、runid
	组成的json对象
返回：runid、errmsg组成的json对象
*/
void CPeraWsAstServer::CreateWSByXpdlStream_async( const AMD_PeraWsAstICE_CreateWSByXpdlStreamPtr& pObj, const ::std::string& sJsonStr, const ::Ice::Current& /*= ::Ice::Current()*/ )
{
	ZTools::WriteZToolsFormatLog( " --------------------CreateWSByXpdlStream进入:[%s]--------------------- \n", sJsonStr.c_str() );
	CInterfaceData_CacheServer clsInter;
	if ( !clsInter.Parse( sJsonStr.c_str() ) )
	{
		clsInter.m_sErrMsg = "解析参数失败";
	}
	else
	{
		if ( clsInter.m_sWsPath.IsEmpty() || clsInter.m_sXpdlStream.IsEmpty() || clsInter.m_sRunId.IsEmpty()|| !clsInter.m_sErrMsg.IsEmpty() )
		{
			clsInter.m_sErrMsg = "参数传值不正确";
		}
		else
		{
			if ( !AddToRQueue( clsInter.m_sRunId, lpIceMethodName[5] ) )
			{
				clsInter.m_sErrMsg = "添加运行队列失败";
			}

			CXpdl Xpdl;

			std::string sStream = (char*)(LPCTSTR)clsInter.m_sXpdlStream;

			sStream = ZTools::MBToUTF8( sStream );

			CString sCacheDir = GetCacheDirectory() + "\\" + clsInter.m_sRunId;

			if( Xpdl.LoadStream( sStream.c_str(), sCacheDir + "\\" + "main.xpdl" ) )
			{
				if ( Xpdl.Parse() )
				{
					if ( !Xpdl.CreateCache( sCacheDir ) )
					{
						clsInter.m_sErrMsg = "生成本地缓存失败";
					}
					else
					{
						if ( !ZipTool::ZipFromDir( (LPCTSTR)clsInter.m_sWsPath, (LPCTSTR)sCacheDir ) )
						{
							clsInter.m_sErrMsg = "生成ws失败";
						}
						if ( !FileOperationFun( sCacheDir, NULL, FO_DELETE ) )
						{
							clsInter.m_sErrMsg += "删除缓存文件失败";
						}
					}
				}
				else
				{
					clsInter.m_sErrMsg = "解析xpdl失败";
				}
			}
			else
			{
				clsInter.m_sErrMsg = "加载xpdl失败";
			}
			if ( !DelFromRQueue( clsInter.m_sRunId, lpIceMethodName[5] ) )
			{
				clsInter.m_sErrMsg = "删除运行队列失败";
			}
		}
	}
	try
	{
		pObj->ice_response( (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_CREATEWSBYXPSLSTREAM, TRUE ) );
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog("CreateWSByXpdlStream : Ice Exception [%s]\n", ex.ice_name().c_str() );
	}
	ZTools::WriteZToolsFormatLog("CreateWSByXpdlStream 执行返回 [%s]\n", clsInter.m_sRunId );
}

BOOL CPeraWsAstServer::AddToRQueue( LPCTSTR lpRunId, LPCTSTR lpFlag /*= NULL */ )
{
	if ( lpRunId == NULL )
	{
		return FALSE;
	}
	CString sFlag = lpFlag == NULL ? "" : lpFlag;
	BOOL bExist = FALSE;
	m_CriticalExecuteTask.Lock();
	for ( list< CString >::iterator itDel = m_listDelQueue.begin(); itDel != m_listDelQueue.end(); itDel++ )
	{
		if ( itDel->CompareNoCase( lpRunId ) == 0 )
		{
			ZTools::WriteZToolsFormatLog( "指定的任务[%s]正在被删除，添加运行任务队列失败\n", lpRunId );
			bExist = TRUE;
			break;
		}
	}
	if ( !bExist )
	{
		map< CString, CString > mapRunnerElement;
		mapRunnerElement.insert( make_pair( lpRunId, sFlag ) );
		m_vecRunningQueue.push_back( mapRunnerElement );
	}
	m_CriticalExecuteTask.Unlock();
	return !bExist;
}

BOOL CPeraWsAstServer::AddToTQueue( LPCTSTR lpRunId, LPCTSTR lpFlag /*= NULL */ )
{
	if ( lpRunId == NULL )
	{
		return FALSE;
	}
	CString sFlag = lpFlag == NULL ? "" : lpFlag;
	m_CriticalTermination.Lock();
	map< CString, CString > mapTerminateElement;
	mapTerminateElement.insert( make_pair( lpRunId, sFlag ) );
	m_vecTerminationQueue.push_back( mapTerminateElement );
	m_CriticalTermination.Unlock();
	return TRUE;
}

BOOL CPeraWsAstServer::DelFromTQueue( LPCTSTR lpRunId, LPCTSTR lpFlag /*= NULL */ )
{
	if ( lpRunId == NULL )
	{
		return FALSE;
	}
	CString sFlag = lpFlag == NULL ? "" : lpFlag;
	m_CriticalTermination.Lock();
	for ( vector< map< CString, CString > >::iterator itVec = m_vecTerminationQueue.begin(); itVec != m_vecTerminationQueue.end();)
	{
		map< CString, CString > mapTermination = *itVec;
		map< CString, CString >::iterator itMap = mapTermination.begin();
		if ( itMap == mapTermination.end() )
		{
			itVec++;
			continue;
		}
		if ( itMap->first.CompareNoCase( lpRunId ) == 0 )
		{
			if ( sFlag.IsEmpty() || itMap->second.IsEmpty() || itMap->second.CompareNoCase( sFlag ) == 0 )
			{
				itVec = m_vecTerminationQueue.erase( itVec );
			}
			else
			{
				itVec++;
			}
		}
		else
		{
			itVec++;
		}
	}
	m_CriticalTermination.Unlock();
	return TRUE;
}

BOOL CPeraWsAstServer::DelFromRQueue( LPCTSTR lpRunId, LPCTSTR lpFlag /*= NULL */ )
{
	if ( lpRunId == NULL )
	{
		return FALSE;
	}
	CString sFlag = lpFlag == NULL ? "" : lpFlag;
	m_CriticalExecuteTask.Lock();
	for ( vector< map< CString, CString > >::iterator itVec = m_vecRunningQueue.begin(); itVec != m_vecRunningQueue.end();)
	{
		map< CString, CString > mapRunner = *itVec;
		map< CString, CString >::iterator itMap = mapRunner.begin();
		if ( itMap == mapRunner.end() )
		{
			itVec++;
			continue;
		}
		if ( itMap->first.CompareNoCase( lpRunId ) == 0 )
		{
			if ( sFlag.IsEmpty() || itMap->second.IsEmpty() || itMap->second.CompareNoCase( sFlag ) == 0 )
			{
				itVec = m_vecRunningQueue.erase( itVec );
			}
			else
			{
				itVec++;
			}
		}
		else
		{
			itVec++;
		}
	}
	m_CriticalExecuteTask.Unlock();
	return TRUE;
}

BOOL CPeraWsAstServer::FindFromRQueue( LPCTSTR lpRunId, LPCTSTR lpFlag /*= NULL */ )
{
	BOOL bResult = FALSE;
	if ( lpRunId == NULL )
	{
		return bResult;
	}
	CString sFlag = lpFlag == NULL ? "" : lpFlag;
	m_CriticalExecuteTask.Lock();
	for ( vector< map< CString, CString > >::iterator itVec = m_vecRunningQueue.begin(); itVec != m_vecRunningQueue.end(); itVec++ )
	{
		map< CString, CString > mapRunner = *itVec;
		map< CString, CString >::iterator itMap = mapRunner.begin();
		if ( itMap == mapRunner.end() )
		{
			continue;
		}
		if ( itMap->first.CompareNoCase( lpRunId ) == 0 )
		{
			if ( sFlag.IsEmpty() || itMap->second.IsEmpty() || itMap->second.CompareNoCase( sFlag ) == 0 )
			{
				bResult = TRUE;
				break;
			}
		}
	}
	m_CriticalExecuteTask.Unlock();
	return bResult;
}

BOOL CPeraWsAstServer::FindFromTQueue( LPCTSTR lpRunId, LPCTSTR lpFlag /*= NULL */ )
{
	BOOL bResult = FALSE;
	if ( lpRunId == NULL )
	{
		return bResult;
	}
	CString sFlag = lpFlag == NULL ? "" : lpFlag;
	m_CriticalTermination.Lock();
	for ( vector< map< CString, CString > >::iterator itVec = m_vecTerminationQueue.begin(); itVec != m_vecTerminationQueue.end(); itVec++ )
	{
		map< CString, CString > mapTermination = *itVec;
		map< CString, CString >::iterator itMap = mapTermination.begin();
		if ( itMap == mapTermination.end() )
		{
			continue;
		}
		if ( itMap->first.CompareNoCase( lpRunId ) == 0 )
		{
			if ( sFlag.IsEmpty() || itMap->second.IsEmpty() || itMap->second.CompareNoCase( sFlag ) == 0 )
			{
				bResult = TRUE;
				break;
			}
		}
	}
	m_CriticalTermination.Unlock();
	return bResult;
}

BOOL CPeraWsAstServer::AddToDQueue( LPCTSTR lpRunId )
{
	if ( lpRunId == NULL )
	{
		return FALSE;
	}
	BOOL bExist = FALSE;
	m_CriticalExecuteTask.Lock();
	for ( vector< map< CString, CString > >::iterator itRun = m_vecRunningQueue.begin(); itRun != m_vecRunningQueue.end(); itRun++ )
	{
		map< CString, CString > mapRunner = *itRun;
		map< CString, CString >::iterator itMap = mapRunner.begin();
		if ( itMap == mapRunner.end() )
		{
			continue;
		}
		if ( itMap->first.CompareNoCase( lpRunId ) == 0 )
		{
			bExist = TRUE;
			break;
		}
	}
	if ( !bExist )
	{
		m_listDelQueue.push_back( lpRunId );
	}
	m_CriticalExecuteTask.Unlock();
	return !bExist;
}

BOOL CPeraWsAstServer::DelFromDQueue( LPCTSTR lpRunId )
{
	if ( lpRunId == NULL )
	{
		return FALSE;
	}
	m_CriticalExecuteTask.Lock();
	for ( list< CString >::iterator itDel = m_listDelQueue.begin(); itDel != m_listDelQueue.end(); )
	{
		if ( itDel->CompareNoCase( lpRunId ) == 0 )
		{
			itDel = m_listDelQueue.erase( itDel );
			break;//Add和Del一一对应，一次只删除一个
		}
		else
		{
			itDel++;
		}
	}
	m_CriticalExecuteTask.Unlock();
	return TRUE;
}

BOOL CPeraWsAstServer::FindFromDQueue( LPCTSTR lpRunId )
{
	if ( lpRunId == NULL )
	{
		return FALSE;
	}
	BOOL bExist = FALSE;
	m_CriticalExecuteTask.Lock();
	for ( list< CString >::iterator itDel = m_listDelQueue.begin(); itDel != m_listDelQueue.end(); itDel++ )
	{
		if ( itDel->CompareNoCase( lpRunId ) == 0 )
		{
			bExist = TRUE;
			break;
		}
	}
	m_CriticalExecuteTask.Unlock();
	return bExist;
}

static BOOL DeleteDir(char * path)
{
    WIN32_FIND_DATA finddata;
    HANDLE hfind = NULL;
    char * pdir;

	int nDirLength = strlen( path ) + 16;
    pdir=new char[nDirLength];
	ZeroMemory( pdir, nDirLength );
	StringCbCopy( pdir, nDirLength, path );
    if(path[strlen(path)-1]!='\\')
		StringCbCat( pdir, nDirLength, "\\*.*" );
	else
		StringCbCat( pdir, nDirLength, "*.*" );

    hfind=FindFirstFile(pdir,&finddata);
    if(hfind==INVALID_HANDLE_VALUE)
        return FALSE;

    delete []pdir;
    do
    {
		int nPathLength = strlen(path)+strlen(finddata.cFileName)+16;
        pdir=new char[nPathLength];
		ZeroMemory( pdir, nPathLength );
		_stprintf_s( pdir, nPathLength, "%s\\%s", path, finddata.cFileName );
        if(strcmp(finddata.cFileName,".")==0
            ||strcmp(finddata.cFileName,"..")==0)
		{
			delete []pdir; pdir = NULL;
            RemoveDirectory(pdir);
            continue;
        }

        if((finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==0)
            DeleteFile(pdir);
        else
            DeleteDir(pdir);
        delete []pdir; pdir = NULL;
    }while(FindNextFile(hfind,&finddata));

	FindClose( hfind );
	hfind = NULL;

    if(RemoveDirectory(path))
        return TRUE;
    else
        return FALSE;
}
