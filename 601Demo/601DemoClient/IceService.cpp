#include "StdAfx.h"
#include "IceService.h"
#include "ClientIceServiceI.h"
#include "601DemoClient.h"

static LPCTSTR g_Ice601DemoClientAstAdapter = "Pera601DemoClientService";
static LPCTSTR g_Ice601DemoClientAstThreadPoolSize = "1";
static LPCTSTR g_Ice601DemoClientAstThreadPoolMaxSize = "10000";
static LPCTSTR g_Ice601DemoClientAstThreadPoolWarnSize = "5000";
static LPCTSTR g_Ice601DemoClientAstMessageSizeMax = "102400";

CIceService::CIceService(void) 
{
	m_strPort = "20142";
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
	initData.properties->setProperty("Ice.MessageSizeMax", g_Ice601DemoClientAstMessageSizeMax );//默认是1024，单位KB
	initData.properties->setProperty("Ice.ThreadPool.Server.Size", g_Ice601DemoClientAstThreadPoolSize );
	initData.properties->setProperty("Ice.ThreadPool.Server.SizeMax", g_Ice601DemoClientAstThreadPoolMaxSize );
	initData.properties->setProperty("Ice.ThreadPool.Server.SizeWarn", g_Ice601DemoClientAstThreadPoolWarnSize );
	return Ice::initialize(initData);
}

std::string CIceService::GetPort()
{
	return m_strPort;
}
std::string CIceService::GetEndPoints()
{
	CString sMsg;
	sMsg.Format( "tcp -h 0.0.0.0 -p %s", m_strPort.c_str() );
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
		strMsg.Format("正在启动本地服务,端口号:%s",m_strPort.c_str());
		theApp.TraceRunLog(strMsg);
		m_communicatorPtr = InitCommunicator();
		Ice::ObjectAdapterPtr adapter = m_communicatorPtr->createObjectAdapterWithEndpoints( g_Ice601DemoClientAstAdapter, GetEndPoints() );
		Ice::ObjectPtr servant = new CClientIceServiceI;
		adapter->add(servant, m_communicatorPtr->stringToIdentity( g_Ice601DemoClientAstAdapter ) );
		adapter->activate();
		theApp.TraceRunLog("本地服务已启动...");
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
		theApp.TraceRunLog("本地服务启动失败！");
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

	m_strPort = strPortNum;
}