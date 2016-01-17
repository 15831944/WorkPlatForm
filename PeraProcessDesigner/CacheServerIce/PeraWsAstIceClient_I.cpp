#include "StdAfx.h"
#include "PeraWsAstIceClient_I.h"
#include <Strsafe.h>
#include "PeraCacheServerIceData.h"
#include "Tools.h"


const std::string g_sAstIceInterfaceName[6] = { "CreateCacheByWS", "CreateWSByCache", "CopyCache", "DeleteCache", "CreateCacheByXpdlStream", "CreateWSByXpdlStream" };

CPeraWsAstIceClient_I::CPeraWsAstIceClient_I(void)
{
}


CPeraWsAstIceClient_I::~CPeraWsAstIceClient_I(void)
{
//	Destroy();
}

BOOL CPeraWsAstIceClient_I::InitIceProxy()
{
	if ( m_pPrx == NULL )
	{
		try
		{
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock_Prx(m_monitor_Prx);
			Ice::InitializationData initData;
			initData.properties = Ice::createProperties();
			initData.properties->setProperty("Ice.MessageSizeMax", "102400" );//默认是1024，单位KB
			initData.properties->setProperty("Ice.ThreadPool.Client.Size", "1");
			initData.properties->setProperty("Ice.ThreadPool.Client.SizeMax", "50");
			initData.properties->setProperty("Ice.ThreadPool.Client.SizeWarn", "50");
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock_Communicator(m_monitor_Communicator);
			if ( m_pCommunicator == NULL )
			{
				m_pCommunicator = Ice::initialize(initData);
			}
			m_pPrx = PeraWsAstICEPrx::checkedCast(m_pCommunicator->stringToProxy( "PeraPRCacheService:tcp -h 127.0.0.1 -p 20136" ) );
			m_pPrx = m_pPrx->ice_twoway()->ice_timeout(3600000)->ice_secure(false);
			return TRUE;
		}
		catch(const Ice::Exception& ex)
		{
			if ( m_pCommunicator != NULL )
			{
				try
				{
					m_pCommunicator->destroy();
					m_pCommunicator = NULL;
				}
				catch(const IceUtil::Exception&)
				{
					ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::InitIceProxy [%s][%s] \n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
				}
			}
			ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::InitIceProxy [%s][%s] \n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CPeraWsAstIceClient_I::Destroy()
{
	if ( m_pCommunicator != NULL )
	{
		try
		{
			m_pCommunicator->destroy();
			m_pCommunicator = NULL;
		}
		catch( const IceUtil::Exception& ex )
		{
			ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::Destroy [%s][%s] \n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
		}
	}
	if ( m_pPrx != NULL )
	{
		m_pPrx = NULL;
	}
	return TRUE;
}

std::string CPeraWsAstIceClient_I::GetRunId()
{
	ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::GetRunId() 进入\n" );
	if (!InitIceProxy())
	{
		ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::GetRunId() 初始化失败\n" );
		return "";
	}
	try
	{
		std::string sRet =  m_pPrx->GetRunId();	 	
		CInterfaceData_CacheServer clsInter;
		if ( !clsInter.Parse( sRet.c_str() ) )
		{
			ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::GetRunId() 解析失败\n" );
			return "";
		}
		if ( !clsInter.m_sErrMsg.IsEmpty() )
		{
			ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::GetRunId() 错误不为空[%s]\n", clsInter.m_sErrMsg );
			return "";
		}
		ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::GetRunId() 成功返回 [%s]\n", clsInter.m_sRunId );
		return (char*)(LPCTSTR)clsInter.m_sRunId;
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::GetRunId [%s][%s] \n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
	}
	ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::GetRunId() 返回空\n" );
	return "";
}

std::string CPeraWsAstIceClient_I::Stop( LPCTSTR lpRunId )
{
	ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::Stop 进入[%s]\n", lpRunId );
	if ( lpRunId == NULL )
	{
		ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::Stop lpRunId为空\n" );
		return "lpRunId == NULL";
	}
	if (!InitIceProxy())
	{
		ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::Stop 初始化失败\n" );
		return "Init Failed";
	}
	CInterfaceData_CacheServer clsInter;
	clsInter.m_sRunId = lpRunId;
	try
	{
		ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::Stop 返回\n" );
		return	m_pPrx->Stop( (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_STOP, FALSE ) );
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::Stop [%s][%s] \n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
	}
	ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::Stop 返回失败\n" );
	return "Failed";
}

BOOL CPeraWsAstIceClient_I::CreateCacheByWS( LPCTSTR lpRunId, LPCTSTR lpWsPath, BOOL bAsync )
{
	ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::CreateCacheByWS 进入[%s][%s]\n", lpRunId, lpWsPath );
	if ( lpRunId == NULL || lpWsPath == NULL )
	{
		ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::CreateCacheByWS 参数错误\n" );
		return FALSE;
	}
	if (!InitIceProxy())
	{
		ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::CreateCacheByWS 初始化失败\n" );
		return FALSE;
	}
	CInterfaceData_CacheServer clsInter;
	clsInter.m_sRunId = lpRunId;
	clsInter.m_sWsPath = lpWsPath;
	if ( bAsync )
	{
		AMI_PeraWsAstICE_CreateCacheByWS_I* pIce = new AMI_PeraWsAstICE_CreateCacheByWS_I;
		if ( pIce == NULL )
		{
			ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::CreateCacheByWS 创建AMI_PeraWsAstICE_CreateCacheByWS_I错误\n" );
			return FALSE;
		}
		try
		{
			m_pPrx->CreateCacheByWS_async( pIce, (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_CREATECACHEBYWS, FALSE ) );	 
		}
		catch(const Ice::Exception& ex)
		{
			ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::CreateCacheByWS [%s][%s] \n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
			return FALSE;
		}
	}
	else
	{
		try
		{
			string sRet = m_pPrx->CreateCacheByWS( (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_CREATECACHEBYWS, FALSE ) );	 
			if ( !clsInter.Parse( sRet.c_str() ) )
			{
				return FALSE;
			}
			if ( !clsInter.m_sErrMsg.IsEmpty() )
			{
				ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::CreateCacheByWS Sync error[%s]\n", clsInter.m_sErrMsg );
				return FALSE;
			}
		}
		catch(const Ice::Exception& ex)
		{
			ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::CreateCacheByWS Sync [%s][%s] \n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
			return FALSE;
		}
	}
	ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::CreateCacheByWS 执行返回\n" );
	return TRUE;
}

BOOL CPeraWsAstIceClient_I::CreateWSByCache( LPCTSTR lpRunId, LPCTSTR lpWsPath, BOOL bAsync )
{
	ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::CreateWSByCache 进入[%s][%s]\n", lpRunId, lpWsPath );
	if ( lpRunId == NULL || lpWsPath == NULL )
	{
		ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::CreateWSByCache 参数错误\n " );
		return FALSE;
	}
	if (!InitIceProxy())
	{
		ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::CreateWSByCache 初始化错误\n " );
		return FALSE;
	}
	CInterfaceData_CacheServer clsInter;
	clsInter.m_sRunId = lpRunId;
	clsInter.m_sWsPath = lpWsPath;
	if ( bAsync )
	{
		AMI_PeraWsAstICE_CreateWSByCache_I* pIce = new AMI_PeraWsAstICE_CreateWSByCache_I;
		if ( pIce == NULL )
		{
			ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::CreateWSByCache 创建AMI_PeraWsAstICE_CreateWSByCache_I错误\n " );
			return FALSE;
		}
		try
		{
			m_pPrx->CreateWSByCache_async( pIce, (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_CREATEWSBYCACHE, FALSE ) );	
		}
		catch(const Ice::Exception& ex)
		{
			ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::CreateWSByCache [%s][%s] \n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
			return FALSE;
		}
	}
	else
	{
		try
		{
			::string sRet = m_pPrx->CreateWSByCache( (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_CREATEWSBYCACHE, FALSE ) );	
			if ( !clsInter.Parse( sRet.c_str() ) )
			{
				ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::CreateWSByCache Sync Parse faile\n" );
				return FALSE;
			}
			if ( !clsInter.m_sErrMsg.IsEmpty() )
			{
				ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::CreateWSByCache Sync error[%s]\n", clsInter.m_sErrMsg );
				return FALSE;
			}
		}
		catch(const Ice::Exception& ex)
		{
			ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::CreateWSByCache Sync [%s][%s] \n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
			return FALSE;
		}
	}
	ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::CreateWSByCache 执行返回\n " );
	return TRUE;
}

BOOL CPeraWsAstIceClient_I::CopyCache( LPCTSTR lpCachedirId, LPCTSTR lpRunId, BOOL bAsync )
{
	ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::CopyCache 进入[%s][%s]\n", lpCachedirId, lpRunId );
	if ( lpCachedirId == NULL || lpRunId == NULL )
	{
		ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::CopyCache 参数错误\n" );
		return FALSE;
	}
	if (!InitIceProxy())
	{
		ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::CopyCache 初始化失败\n" );
		return FALSE;
	}
	CInterfaceData_CacheServer clsInter;
	clsInter.m_sRunId = lpRunId;
	clsInter.m_sCacheDirId = lpCachedirId;
	if ( bAsync )
	{
		AMI_PeraWsAstICE_CopyCache_I* pIce = new AMI_PeraWsAstICE_CopyCache_I;
		if ( pIce == NULL )
		{
			ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::CopyCache 创建AMI_PeraWsAstICE_CopyCache_I失败\n" );
			return FALSE;
		}
		try
		{
			m_pPrx->CopyCache_async( pIce, (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_COPYCACHE, FALSE ) );
		}
		catch(const Ice::Exception& ex)
		{
			ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::CopyCache [%s][%s] \n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
			return FALSE;
		}
	}
	else
	{
		try
		{
			::string sRet = m_pPrx->CopyCache( (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_COPYCACHE, FALSE ) );	
			if ( !clsInter.Parse( sRet.c_str() ) )
			{
				ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::CopyCache Sync Parse faile\n" );
				return FALSE;
			}
			if ( !clsInter.m_sErrMsg.IsEmpty() )
			{
				ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::CopyCache Sync error[%s]\n", clsInter.m_sErrMsg );
				return FALSE;
			}
		}
		catch(const Ice::Exception& ex)
		{
			ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::CopyCache Sync [%s][%s] \n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
			return FALSE;
		}
	}
	ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::CopyCache 执行返回\n" );
	return TRUE;
}

BOOL CPeraWsAstIceClient_I::DeleteCache( LPCTSTR lpRunId, BOOL bAsync )
{
	ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::DeleteCache 进入[%s]\n", lpRunId );
	if ( lpRunId == NULL )
	{
		ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::DeleteCache 参数错误\n" );
		return FALSE;
	}
	if (!InitIceProxy())
	{
		ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::DeleteCache 初始化错误\n" );
		return FALSE;
	}
	CInterfaceData_CacheServer clsInter;
	clsInter.m_sRunId = lpRunId;
	if ( bAsync )
	{
		AMI_PeraWsAstICE_DeleteCache_I* pIce = new AMI_PeraWsAstICE_DeleteCache_I;
		if ( pIce == NULL )
		{
			ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::DeleteCache 创建AMI_PeraWsAstICE_DeleteCache_I错误\n" );
			return FALSE;
		}
		try
		{
			m_pPrx->DeleteCache_async( pIce, (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_DELETECACHE, FALSE ) );	
		}
		catch(const Ice::Exception& ex)
		{
			ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::DeleteCache [%s][%s] \n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
			return FALSE;
		}
	}
	else
	{
		try
		{
			::string sRet = m_pPrx->DeleteCache( (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_DELETECACHE, FALSE ) );		
			if ( !clsInter.Parse( sRet.c_str() ) )
			{
				ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::DeleteCache Sync Parse faile\n" );
				return FALSE;
			}
			if ( !clsInter.m_sErrMsg.IsEmpty() )
			{
				ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::DeleteCache Sync error[%s]\n", clsInter.m_sErrMsg );
				return FALSE;
			}
		}
		catch(const Ice::Exception& ex)
		{
			ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::DeleteCache Sync [%s][%s] \n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
			return FALSE;
		}
	}
	ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::DeleteCache 执行返回\n" );
	return TRUE;
}

BOOL CPeraWsAstIceClient_I::CreateCacheByXpdlStream( LPCTSTR lpRunId, LPCTSTR lpXpdlStream, BOOL bAsync )
{
	ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::CreateCacheByXpdlStream 进入[%s][%s]\n", lpRunId, lpXpdlStream );
	if ( lpRunId == NULL || lpXpdlStream == NULL )
	{
		ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::CreateCacheByXpdlStream 参数错误\n" );
		return FALSE;
	}
	if (!InitIceProxy())
	{
		ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::CreateCacheByXpdlStream 初始化错误\n" );
		return FALSE;
	}
	CInterfaceData_CacheServer clsInter;
	clsInter.m_sRunId = lpRunId;
	clsInter.m_sXpdlStream = lpXpdlStream;
	clsInter.m_sErrMsg = "";
	if ( bAsync )
	{
		AMI_PeraWsAstICE_CreateCacheByXpdlStream_I* pIce = new AMI_PeraWsAstICE_CreateCacheByXpdlStream_I;
		if ( pIce == NULL )
		{
			ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::CreateCacheByXpdlStream 创建AMI_PeraWsAstICE_CreateCacheByXpdlStream_I错误\n" );
			return FALSE;
		}
		try
		{
			m_pPrx->CreateCacheByXpdlStream_async( pIce, (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_CREATECACHEBYXPDLSTREAM, FALSE ) );	 	
		}
		catch(const Ice::Exception& ex)
		{
			ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::CreateCacheByXpdlStream [%s][%s] \n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
			return FALSE;
		}
	}
	else
	{
		try
		{
			::string sRet = m_pPrx->CreateCacheByXpdlStream( (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_CREATECACHEBYXPDLSTREAM, FALSE ) );
			if ( !clsInter.Parse( sRet.c_str() ) )
			{
				ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::CreateCacheByXpdlStream Sync Parse faile\n" );
				return FALSE;
			}
			if ( !clsInter.m_sErrMsg.IsEmpty() )
			{
				ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::CreateCacheByXpdlStream Sync error[%s]\n", clsInter.m_sErrMsg );
				return FALSE;
			}
		}
		catch(const Ice::Exception& ex)
		{
			ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::CreateCacheByXpdlStream [%s][%s] \n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
			return FALSE;
		}
	}
	ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::CreateCacheByXpdlStream 执行返回\n" );
	return TRUE;
}

BOOL CPeraWsAstIceClient_I::CreateWSByXpdlStream( LPCTSTR lpRunId, LPCTSTR lpWsPath, LPCTSTR lpXpdlStream, BOOL bAsync )
{
	ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::CreateWSByXpdlStream 进入[%s][%s][%s]", lpRunId, lpWsPath, lpXpdlStream );
	if ( lpRunId == NULL || lpWsPath == NULL || lpXpdlStream == NULL )
	{
		ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::CreateWSByXpdlStream 参数错误\n" );
		return FALSE;
	}
	if (!InitIceProxy())
	{
		ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::CreateWSByXpdlStream 初始化错误\n" );
		return FALSE;
	}
	CInterfaceData_CacheServer clsInter;
	clsInter.m_sRunId = lpRunId;
	clsInter.m_sWsPath = lpWsPath;
	clsInter.m_sXpdlStream = lpXpdlStream;
	if ( bAsync )
	{
		AMI_PeraWsAstICE_CreateWSByXpdlStream_I* pIce = new AMI_PeraWsAstICE_CreateWSByXpdlStream_I;
		if ( pIce == NULL )
		{
			ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::CreateWSByXpdlStream 创建AMI_PeraWsAstICE_CreateWSByXpdlStream_I错误\n" );
			return FALSE;
		}
		try
		{
			m_pPrx->CreateWSByXpdlStream_async( pIce, (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_CREATEWSBYXPSLSTREAM, FALSE ) );	 	
		}
		catch(const Ice::Exception& ex)
		{
			ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::CreateWSByXpdlStream [%s][%s] \n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
			return FALSE;
		}
	}
	else
	{
		try
		{
			::string sRet = m_pPrx->CreateWSByXpdlStream( (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_CREATEWSBYXPSLSTREAM, FALSE ) );	 		
			if ( !clsInter.Parse( sRet.c_str() ) )
			{
				ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::CreateWSByXpdlStream Sync Parse faile\n" );
				return FALSE;
			}
			if ( !clsInter.m_sErrMsg.IsEmpty() )
			{
				ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::CreateWSByXpdlStream Sync error[%s]\n", clsInter.m_sErrMsg );
				return FALSE;
			}
		}
		catch(const Ice::Exception& ex)
		{
			ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::CreateWSByXpdlStream Sync [%s][%s] \n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
			return FALSE;
		}
	}
	ZTools::WriteZToolsLog( "CPeraWsAstIceClient_I::CreateWSByXpdlStream 执行返回\n" );
	return TRUE;
}

static void SendIceMsg( std::string sMsg, std::string sFlag )
{
	ZTools::WriteZToolsFormatLog( "ICE Server返回 [%s][%s]\n", sMsg.c_str(), sFlag.c_str() );
	CWinApp* app = AfxGetApp();
	if ( app == NULL )
	{
		return;
	}
	CFrameWnd* pFrame = (CFrameWnd*)app->m_pMainWnd;
	if ( pFrame == NULL && pFrame->GetSafeHwnd() == NULL )
	{
		return;
	}
	int nWsize = sMsg.size() + 16;
	int nPsize = sFlag.size() + 16;
	char* pWaram = new char[ nWsize ];
	char* pParam = new char[ nPsize ];
	ZeroMemory( pWaram, nWsize );
	ZeroMemory( pParam, nPsize );
	StringCbCopy( pWaram, nWsize, sMsg.c_str() );
	StringCbCopy( pParam, nPsize, sFlag.c_str() );
//	pFrame->PostMessage( WM_ASTICE_RESULT, (WPARAM)pWaram, (LPARAM)(char*)pParam );
}

void AMI_PeraWsAstICE_CreateCacheByWS_I::ice_response( const ::std::string& sRet )
{
	SendIceMsg( sRet, g_sAstIceInterfaceName[0] );
}

void AMI_PeraWsAstICE_CreateCacheByWS_I::ice_exception( const Ice::Exception& ex )
{
	CInterfaceData_CacheServer clsInter;
	clsInter.m_sErrMsg = ex.ice_name().c_str();
	SendIceMsg( (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_CREATECACHEBYWS, TRUE ), g_sAstIceInterfaceName[0] );
	ZTools::WriteZToolsFormatLog( "AMI_PeraWsAstICE_CreateCacheByWS_I::ice_exception [%s][%s]\n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
}

void AMI_PeraWsAstICE_CreateWSByCache_I::ice_response( const ::std::string& sRet )
{
	SendIceMsg( sRet, g_sAstIceInterfaceName[1] );
}

void AMI_PeraWsAstICE_CreateWSByCache_I::ice_exception( const Ice::Exception& ex )
{
	CInterfaceData_CacheServer clsInter;
	clsInter.m_sErrMsg = ex.ice_name().c_str();
	SendIceMsg( (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_CREATEWSBYCACHE, TRUE ), g_sAstIceInterfaceName[1] );
	ZTools::WriteZToolsFormatLog( "AMI_PeraWsAstICE_CreateWSByCache_I::ice_exception [%s][%s]\n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
}

void AMI_PeraWsAstICE_CopyCache_I::ice_response( const ::std::string& sRet )
{
	SendIceMsg( sRet, g_sAstIceInterfaceName[2] );
}

void AMI_PeraWsAstICE_CopyCache_I::ice_exception( const Ice::Exception& ex )
{
	CInterfaceData_CacheServer clsInter;
	clsInter.m_sErrMsg = ex.ice_name().c_str();
	SendIceMsg( (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_COPYCACHE, TRUE ), g_sAstIceInterfaceName[2] );
	ZTools::WriteZToolsFormatLog( "AMI_PeraWsAstICE_CopyCache_I::ice_exception [%s][%s]\n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
}

void AMI_PeraWsAstICE_DeleteCache_I::ice_response( const ::std::string& sRet )
{
	SendIceMsg( sRet, g_sAstIceInterfaceName[3] );
}

void AMI_PeraWsAstICE_DeleteCache_I::ice_exception( const Ice::Exception& ex )
{
	CInterfaceData_CacheServer clsInter;
	clsInter.m_sErrMsg = ex.ice_name().c_str();
	//SendIceMsg( (char*)(LPCTSTR)clsInter.GetJsonStr(), g_sAstIceInterfaceName[3] );
	ZTools::WriteZToolsFormatLog( "CICEInterfaceData_DeleteCache::ice_exception [%s][%s]\n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
}

void AMI_PeraWsAstICE_CreateCacheByXpdlStream_I::ice_response( const ::std::string& sRet )
{
	SendIceMsg( sRet, g_sAstIceInterfaceName[4] );
}

void AMI_PeraWsAstICE_CreateCacheByXpdlStream_I::ice_exception( const Ice::Exception& ex )
{
	CInterfaceData_CacheServer clsInter;
	clsInter.m_sErrMsg = ex.ice_name().c_str();
	SendIceMsg( (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_CREATECACHEBYXPDLSTREAM, TRUE ), g_sAstIceInterfaceName[4] );
	ZTools::WriteZToolsFormatLog( "AMI_PeraWsAstICE_CreateCacheByXpdlStream_I::ice_exception [%s][%s]\n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
}

void AMI_PeraWsAstICE_CreateWSByXpdlStream_I::ice_response( const ::std::string& sRet )
{
	SendIceMsg( sRet, g_sAstIceInterfaceName[5] );
}

void AMI_PeraWsAstICE_CreateWSByXpdlStream_I::ice_exception( const Ice::Exception& ex )
{
	CInterfaceData_CacheServer clsInter;
	clsInter.m_sErrMsg = ex.ice_name().c_str();
	SendIceMsg( (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_CREATEWSBYXPSLSTREAM, TRUE ), g_sAstIceInterfaceName[5] );
	ZTools::WriteZToolsFormatLog( "AMI_PeraWsAstICE_CreateWSByXpdlStream_I::ice_exception [%s][%s]\n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
}



CPeraWsAstIceClient_I	g_IceWsAstClient;