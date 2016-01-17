#include "StdAfx.h"
#include "PeraWsAstIceClient_I.h"
#include "InterfaceData.h"
#include <Strsafe.h>


CPeraWsAstIceClient_I::CPeraWsAstIceClient_I(void)
{
}


CPeraWsAstIceClient_I::~CPeraWsAstIceClient_I(void)
{
	Destroy();
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
	if (!InitIceProxy())
	{
		return "";
	}
	try
	{
		std::string sRet =  m_pPrx->GetRunId();	 	
		CInterfaceData_CacheServer clsInter;
		if ( !clsInter.Parse( sRet.c_str() ) )
		{
			return "";
		}
		if ( !clsInter.m_sErrMsg.IsEmpty() )
		{
			return "";
		}
		return (char*)(LPCTSTR)clsInter.m_sRunId;
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::GetRunId [%s][%s] \n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
	}
	return "";
}

std::string CPeraWsAstIceClient_I::Stop( LPCTSTR lpRunId )
{
	if ( lpRunId == NULL )
	{
		return "lpRunId == NULL";
	}
	if (!InitIceProxy())
	{
		return "Init Failed";
	}
	CInterfaceData_CacheServer clsInter;
	clsInter.m_sRunId = lpRunId;
	try
	{
		return	m_pPrx->Stop( (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_STOP, FALSE ) );
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::Stop [%s][%s] \n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
	}
	return "Failed";
}

BOOL CPeraWsAstIceClient_I::CreateCacheByWS( LPCTSTR lpRunId, LPCTSTR lpWsPath )
{
	if ( lpRunId == NULL || lpWsPath == NULL )
	{
		return FALSE;
	}
	if (!InitIceProxy())
	{
		return FALSE;
	}
	AMI_PeraWsAstICE_CreateCacheByWS_I* pIce = new AMI_PeraWsAstICE_CreateCacheByWS_I;
	if ( pIce == NULL )
	{
		return FALSE;
	}
	CInterfaceData_CacheServer clsInter;
	clsInter.m_sRunId = lpRunId;
	clsInter.m_sWsPath = lpWsPath;
	try
	{
		m_pPrx->CreateCacheByWS_async( pIce, (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_CREATECACHEBYWS, FALSE ) );	 
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::CreateCacheByWS [%s][%s] \n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
		return FALSE;
	}
	return TRUE;
}

BOOL CPeraWsAstIceClient_I::CreateWSByCache( LPCTSTR lpRunId, LPCTSTR lpWsPath )
{
	if ( lpRunId == NULL || lpWsPath == NULL )
	{
		return FALSE;
	}
	if (!InitIceProxy())
	{
		return FALSE;
	}
	AMI_PeraWsAstICE_CreateWSByCache_I* pIce = new AMI_PeraWsAstICE_CreateWSByCache_I;
	if ( pIce == NULL )
	{
		return FALSE;
	}
	CInterfaceData_CacheServer clsInter;
	clsInter.m_sRunId = lpRunId;
	clsInter.m_sWsPath = lpWsPath;
	try
	{
		m_pPrx->CreateWSByCache_async( pIce, (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_CREATEWSBYCACHE, FALSE ) );	
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::CreateWSByCache [%s][%s] \n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
		return FALSE;
	}
	return TRUE;
}

BOOL CPeraWsAstIceClient_I::CopyCache( LPCTSTR lpCacheId, LPCTSTR lpRunId )
{
	if ( lpCacheId == NULL || lpRunId == NULL )
	{
		return FALSE;
	}
	if (!InitIceProxy())
	{
		return FALSE;
	}
	AMI_PeraWsAstICE_CopyCache_I* pIce = new AMI_PeraWsAstICE_CopyCache_I;
	if ( pIce == NULL )
	{
		return FALSE;
	}
	CInterfaceData_CacheServer clsInter;
	clsInter.m_sRunId = lpRunId;
	clsInter.m_sCacheDirId = lpCacheId;
	try
	{
		m_pPrx->CopyCache_async( pIce, (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_COPYCACHE, FALSE ) );
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::CopyCache [%s][%s] \n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
		return FALSE;
	}
	return TRUE;
}

BOOL CPeraWsAstIceClient_I::DeleteCache( LPCTSTR lpRunId )
{
	if ( lpRunId == NULL )
	{
		return FALSE;
	}
	if (!InitIceProxy())
	{
		return FALSE;
	}
	AMI_PeraWsAstICE_DeleteCache_I* pIce = new AMI_PeraWsAstICE_DeleteCache_I;
	if ( pIce == NULL )
	{
		return FALSE;
	}
	CInterfaceData_CacheServer clsInter;
	clsInter.m_sRunId = lpRunId;
	try
	{
		m_pPrx->DeleteCache_async( pIce, (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_DELETECACHE, FALSE ) );	
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::DeleteCache [%s][%s] \n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
		return FALSE;
	}
	return TRUE;
}

BOOL CPeraWsAstIceClient_I::CreateCacheByXpdlStream( LPCTSTR lpRunId, LPCTSTR lpXpdlStream )
{
	if ( lpRunId == NULL || lpXpdlStream == NULL )
	{
		return FALSE;
	}
	if (!InitIceProxy())
	{
		return FALSE;
	}
	AMI_PeraWsAstICE_CreateCacheByXpdlStream_I* pIce = new AMI_PeraWsAstICE_CreateCacheByXpdlStream_I;
	if ( pIce == NULL )
	{
		return FALSE;
	}
	CInterfaceData_CacheServer clsInter;
	clsInter.m_sRunId = lpRunId;
	clsInter.m_sXpdlStream = lpXpdlStream;
	try
	{
		m_pPrx->CreateCacheByXpdlStream_async( pIce, (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_CREATECACHEBYXPDLSTREAM, FALSE ) );	 	
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::CreateCacheByXpdlStream [%s][%s] \n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
		return FALSE;
	}
	return TRUE;
}

BOOL CPeraWsAstIceClient_I::CreateWSByXpdlStream( LPCTSTR lpRunId, LPCTSTR lpWsPath, LPCTSTR lpXpdlStream )
{
	if ( lpRunId == NULL || lpWsPath == NULL || lpXpdlStream == NULL )
	{
		return FALSE;
	}
	if (!InitIceProxy())
	{
		return FALSE;
	}
	AMI_PeraWsAstICE_CreateWSByXpdlStream_I* pIce = new AMI_PeraWsAstICE_CreateWSByXpdlStream_I;
	if ( pIce == NULL )
	{
		return FALSE;
	}
	CInterfaceData_CacheServer clsInter;
	clsInter.m_sRunId = lpRunId;
	clsInter.m_sWsPath = lpWsPath;
	clsInter.m_sXpdlStream = lpXpdlStream;
	try
	{
		m_pPrx->CreateWSByXpdlStream_async( pIce, (char*)(LPCTSTR)clsInter.GetJsonStr( CInterfaceData_CacheServer::CACHESERVER_INTERFACE_CREATEWSBYXPSLSTREAM, FALSE ) );	 	
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog( "CPeraWsAstIceClient_I::CreateWSByXpdlStream [%s][%s] \n", ex.ice_name().c_str(), ex.ice_stackTrace().c_str() );
		return FALSE;
	}
	return TRUE;
}

static void SendIceMsg( std::string sMsg, CString sFlag )
{
	HWND hWnd = ::FindWindow( NULL, "ICEClientDemo" );
	if ( hWnd != NULL )
	{
		int nWsize = sMsg.size() + 16;
		int nPsize = sFlag.GetLength() + 16;
		char* pWaram = new char[ nWsize ];
		char* pParam = new char[ nPsize ];
		ZeroMemory( pWaram, nWsize );
		ZeroMemory( pParam, nPsize );
		StringCbCopy( pWaram, nWsize, sMsg.c_str() );
		StringCbCopy( pParam, nPsize, sFlag );
		::PostMessage( hWnd, WM_ICE_MESSAGE, (WPARAM)pWaram, (LPARAM)(char*)pParam );
	}
}

void AMI_PeraWsAstICE_CreateCacheByWS_I::ice_response( const ::std::string& sRet )
{
	SendIceMsg( sRet, "open" );
}

void AMI_PeraWsAstICE_CreateCacheByWS_I::ice_exception( const Ice::Exception& ex )
{
	SendIceMsg( ex.ice_name().c_str(), "open" );
}

void AMI_PeraWsAstICE_CreateWSByCache_I::ice_response( const ::std::string& sRet )
{
	SendIceMsg( sRet, "save" );
}

void AMI_PeraWsAstICE_CreateWSByCache_I::ice_exception( const Ice::Exception& ex )
{
	SendIceMsg( ex.ice_name().c_str(), "save" );
}

void AMI_PeraWsAstICE_CopyCache_I::ice_response( const ::std::string& sRet )
{
	SendIceMsg( sRet, "resume" );
}

void AMI_PeraWsAstICE_CopyCache_I::ice_exception( const Ice::Exception& ex )
{
	SendIceMsg( ex.ice_name().c_str(), "resume" );
}

void AMI_PeraWsAstICE_DeleteCache_I::ice_response( const ::std::string& sRet )
{
	SendIceMsg( sRet, "delete" );
}

void AMI_PeraWsAstICE_DeleteCache_I::ice_exception( const Ice::Exception& ex )
{
	SendIceMsg( ex.ice_name().c_str(), "delete" );
}

void AMI_PeraWsAstICE_CreateCacheByXpdlStream_I::ice_response( const ::std::string& sRet )
{
	SendIceMsg( sRet, "createcache" );
}

void AMI_PeraWsAstICE_CreateCacheByXpdlStream_I::ice_exception( const Ice::Exception& ex )
{
	SendIceMsg( ex.ice_name().c_str(), "createcache" );
}

void AMI_PeraWsAstICE_CreateWSByXpdlStream_I::ice_response( const ::std::string& sRet )
{
	SendIceMsg( sRet, "createws" );
}

void AMI_PeraWsAstICE_CreateWSByXpdlStream_I::ice_exception( const Ice::Exception& ex )
{
	SendIceMsg( ex.ice_name().c_str(), "createws" );
}