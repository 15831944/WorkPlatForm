#include "StdAfx.h"
#include "IceService.h"
#include "ntserv_msg.h"
#include "Tools.h"
#include "PeraNetDiskIceI.h"


CIceService::CIceService(void) 
	: CNTService(_T("PeraDCNetDiskDaemon"), _T("PeraDCNetDiskDaemon"), SERVICE_ACCEPT_STOP, _T("个人工作平台文件操作守护进程"))
{
	//m_bEventLogSource = FALSE;
}

CIceService::~CIceService(void)
{
}

static Ice::CommunicatorPtr InitCommunicator()
{
	Ice::InitializationData initData;
	initData.properties = Ice::createProperties();
	initData.properties->setProperty("Ice.MessageSizeMax", "1024");//默认是1024，单位KB
	return Ice::initialize(initData);
}

static std::string GetEndPoints()
{
	std::string strEndPoints;

	std::string strPort;
	strPort = "20137";
	
	strEndPoints = ZTools::FormatString("tcp -h 0.0.0.0 -p %s", strPort.c_str());

	return strEndPoints;
}

void WINAPI CIceService::ServiceMain( DWORD dwArgc, LPTSTR* lpszArgv )
{
	//throw std::exception("The method or operation is not implemented.");
	ZTools::WriteZToolsLog("Function CIceService::ServiceMain");

	ReportStatus(SERVICE_START_PENDING, 1, 1100);
	Sleep(1000);
	ReportStatus(SERVICE_RUNNING);
	
	try
	{
		communicator = InitCommunicator();
		Ice::ObjectAdapterPtr adapter = communicator->createObjectAdapterWithEndpoints("PeraNetDiskDaemonService", GetEndPoints());
		Ice::ObjectPtr servant = new PeraNetDiskIceI();
		adapter->add(servant, communicator->stringToIdentity("PeraNetDiskDaemonService"));
		adapter->activate();
		communicator->waitForShutdown();
		communicator->destroy();
	}
	catch(const Ice::Exception& ex)
	{
		//cerr << ex << endl;
		ZTools::WriteZToolsLog(
			ZTools::FormatString("ice_name:%s\nice_stack:%s", 
			ex.ice_name().c_str(), 
			ex.ice_stackTrace().c_str())
			);

		if(communicator)
		{
			try
			{
				communicator->destroy();
			}
			catch(const Ice::Exception& ex)
			{
				//cerr << ex << endl;
				ZTools::WriteZToolsLog(
					ZTools::FormatString("ice_name:%s\nice_stack:%s", 
					ex.ice_name().c_str(), 
					ex.ice_stackTrace().c_str())
					);
			}
		}
	}

	//Report to the event log that the service has started successfully
	//m_EventLogSource.Report(EVENTLOG_INFORMATION_TYPE, CNTS_MSG_SERVICE_STARTED, m_sDisplayName);

	//The tight loop which constitutes the service
// 	long lOldPause = m_lPaused;
// 	while (!m_lWantStop)
// 	{
// 		//As a demo, we just do a message beep
// 		if (!m_lPaused)
// 			MessageBeep(MB_OK);
// 
// 		//Wait for the specified time
// 		Sleep(m_dwBeepInternal);
// 
// 		//SCM has requested a Pause / Continue
// 		if (m_lPaused != lOldPause)
// 		{
// 			if (m_lPaused)
// 			{
// 				ReportStatus(SERVICE_PAUSED);
// 
// 				//Report to the event log that the service has paused successfully
// 				m_EventLogSource.Report(EVENTLOG_INFORMATION_TYPE, CNTS_MSG_SERVICE_PAUSED, m_sDisplayName);
// 			}
// 			else
// 			{
// 				ReportStatus(SERVICE_RUNNING);
// 
// 				//Report to the event log that the service has stopped continued
// 				m_EventLogSource.Report(EVENTLOG_INFORMATION_TYPE, CNTS_MSG_SERVICE_CONTINUED, m_sDisplayName);
// 			}
// 		}
// 
// 		lOldPause = m_lPaused;
// 	}

	//Pretend that closing down takes some time
	ReportStatus(SERVICE_STOP_PENDING, 1, 1100);
	Sleep(1000);

	//Report to the event log that the service has stopped successfully
	//m_EventLogSource.Report(EVENTLOG_INFORMATION_TYPE, CNTS_MSG_SERVICE_STOPPED, m_sDisplayName);
}

void CIceService::OnStop()
{
	ZTools::WriteZToolsLog("Function CIceService::OnStop");

	if(communicator)
	{
		try
		{
			communicator->shutdown();
		}
		catch(const Ice::Exception& ex)
		{
			//cerr << ex << endl;
			ZTools::WriteZToolsLog(
				ZTools::FormatString("ice_name:%s\nice_stack:%s", 
				ex.ice_name().c_str(), 
				ex.ice_stackTrace().c_str())
				);
		}
	}
}