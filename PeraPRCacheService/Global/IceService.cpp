#include "StdAfx.h"
#include "IceService.h"
#include "PeraWsAstServer.h"


static LPCTSTR g_IceWsRunnerAstPort = "20136";
static LPCTSTR g_IceWsRunnerAstAdapter = "PeraPRCacheService";
static LPCTSTR g_IceWsRunnerAstThreadPoolSize = "1";
static LPCTSTR g_IceWsRunnerAstThreadPoolMaxSize = "10000";
static LPCTSTR g_IceWsRunnerAstThreadPoolWarnSize = "5000";
static LPCTSTR g_IceWsRunnerAstMessageSizeMax = "102400";

CIceService::CIceService(void) 
{
}

CIceService::~CIceService(void)
{
}

static Ice::CommunicatorPtr InitCommunicator()
{
	Ice::InitializationData initData;
	initData.properties = Ice::createProperties();
	initData.properties->setProperty("Ice.MessageSizeMax", g_IceWsRunnerAstMessageSizeMax );//默认是1024，单位KB
	initData.properties->setProperty("Ice.ThreadPool.Server.Size", g_IceWsRunnerAstThreadPoolSize );
	initData.properties->setProperty("Ice.ThreadPool.Server.SizeMax", g_IceWsRunnerAstThreadPoolMaxSize );
	initData.properties->setProperty("Ice.ThreadPool.Server.SizeWarn", g_IceWsRunnerAstThreadPoolWarnSize );
	return Ice::initialize(initData);
}

static std::string GetEndPoints()
{
	std::string strEndPoints;

	std::string strPort;
	strPort = g_IceWsRunnerAstPort;
	
	CString sMsg;
	sMsg.Format( "tcp -h 0.0.0.0 -p %s", strPort.c_str() );
	strEndPoints = sMsg;

	return strEndPoints;
}

void WINAPI CIceService::ServiceMain( DWORD dwArgc, LPTSTR* lpszArgv )
{
	try
	{
		communicator = InitCommunicator();
		Ice::ObjectAdapterPtr adapter = communicator->createObjectAdapterWithEndpoints( g_IceWsRunnerAstAdapter, GetEndPoints() );
		Ice::ObjectPtr servant = new CPeraWsAstServer;
		adapter->add(servant, communicator->stringToIdentity( g_IceWsRunnerAstAdapter ) );
		adapter->activate();
		communicator->waitForShutdown();
		communicator->destroy();
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog("Ice Exception [%s]\n", ex.ice_name().c_str() );

		if(communicator)
		{
			try
			{
				communicator->destroy();
			}
			catch(const Ice::Exception& ex)
			{
				ZTools::WriteZToolsFormatLog("Ice Exception [%s]\n", ex.ice_name().c_str() );
			}
		}
	}
}

void CIceService::OnStop()
{
	if(communicator)
	{
		try
		{
			communicator->shutdown();
		}
		catch(const Ice::Exception& ex)
		{
			ZTools::WriteZToolsFormatLog("Ice Exception [%s]\n", ex.ice_name().c_str() );
		}
	}
}