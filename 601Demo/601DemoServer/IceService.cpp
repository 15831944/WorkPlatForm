#include "StdAfx.h"
#include "IceService.h"
#include "PeraServerIceI.h"
#include "601DemoServer.h"

static LPCTSTR g_Ice601DemoServerAstAdapter = "Pera601DemoServerService";
static LPCTSTR g_Ice601DemoServerAstThreadPoolSize = "1";
static LPCTSTR g_Ice601DemoServerAstThreadPoolMaxSize = "10000";
static LPCTSTR g_Ice601DemoServerAstThreadPoolWarnSize = "5000";
static LPCTSTR g_Ice601DemoServerAstMessageSizeMax = "102400";

CIceService::CIceService(void) 
{
	m_strServerPort = "20141";
	m_communicatorPtr = NULL;
	m_hThread = NULL;
}

CIceService::~CIceService(void)
{
}

Ice::CommunicatorPtr CIceService::InitCommunicator()
{
	Ice::InitializationData initData;
	initData.properties = Ice::createProperties();
	initData.properties->setProperty("Ice.MessageSizeMax", g_Ice601DemoServerAstMessageSizeMax );//默认是1024，单位KB
	initData.properties->setProperty("Ice.ThreadPool.Server.Size", g_Ice601DemoServerAstThreadPoolSize );
	initData.properties->setProperty("Ice.ThreadPool.Server.SizeMax", g_Ice601DemoServerAstThreadPoolMaxSize );
	initData.properties->setProperty("Ice.ThreadPool.Server.SizeWarn", g_Ice601DemoServerAstThreadPoolWarnSize );
	return Ice::initialize(initData);
}

std::string CIceService::GetPort()
{
	return m_strServerPort;
}
std::string CIceService::GetEndPoints()
{
	CString sMsg;
	sMsg.Format( "tcp -h 0.0.0.0 -p %s", m_strServerPort.c_str() );
	std::string strEndPoints = sMsg;

	return strEndPoints;
}
DWORD WINAPI CIceService::ThreadFunService(LPVOID lp)
{
	CIceService * pThis = (CIceService *)(lp);
	if (pThis == NULL) return 0;

	pThis->StartServiceInthread();

	return 0;
}
void CIceService::StartServiceInthread()
{
	if (m_communicatorPtr)
		return;

	try
	{
		CString strMsg;
		strMsg.Format("正在启动服务,端口号:%s",m_strServerPort.c_str());
		theApp.TraceRunLog(strMsg);
		m_communicatorPtr = InitCommunicator();
		Ice::ObjectAdapterPtr adapter = m_communicatorPtr->createObjectAdapterWithEndpoints( g_Ice601DemoServerAstAdapter, GetEndPoints() );
		Ice::ObjectPtr servant = new CPeraServerIceI;
		adapter->add(servant, m_communicatorPtr->stringToIdentity( g_Ice601DemoServerAstAdapter ) );
		adapter->activate();
		theApp.TraceRunLog("服务已启动...");
		m_communicatorPtr->waitForShutdown();
		m_communicatorPtr->destroy();
	}
	catch(const Ice::Exception& ex)
	{
		CString strMsg;
		strMsg.Format("Ice Exception [%s]\n", ex.ice_name().c_str());
		theApp.TraceRunLog(strMsg);
		//ZTools::WriteZToolsFormatLog("Ice Exception [%s]\n", ex.ice_name().c_str() );

		if(m_communicatorPtr)
		{
			try
			{
				m_communicatorPtr->destroy();
			}
			catch(const Ice::Exception& ex)
			{
				//ZTools::WriteZToolsFormatLog("Ice Exception [%s]\n", ex.ice_name().c_str() );
				strMsg.Format("Ice Exception [%s]\n", ex.ice_name().c_str());
				theApp.TraceRunLog(strMsg);
			}
			m_communicatorPtr = NULL;
		}
		theApp.TraceRunLog("服务启动失败！");
	}
}
void  CIceService::StartService()
{
	m_hThread = CreateThread(NULL,0,CIceService::ThreadFunService,this,0,NULL);
}
void CIceService::OnStop()
{
	if(m_communicatorPtr)
	{
		try
		{
			m_communicatorPtr->shutdown();
			m_communicatorPtr->destroy();
			m_communicatorPtr = NULL;
		}
		catch(const Ice::Exception& ex)
		{
			CString strMsg;
			strMsg.Format("Ice Exception [%s]\n", ex.ice_name().c_str());
			theApp.TraceRunLog(strMsg);

			//ZTools::WriteZToolsFormatLog("Ice Exception [%s]\n", ex.ice_name().c_str() );
		}
	}
}
void CIceService::SetServerPort(std::string strPortNum)
{
	if (strPortNum.empty())
		return;

	m_strServerPort = strPortNum;
}