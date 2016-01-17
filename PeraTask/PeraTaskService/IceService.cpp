#include "StdAfx.h"
#include "IceService.h"
#include "ntserv_msg.h"
#include "RobotRemoteServiceI.h"
#include "Tools.h"
#include "WsICEService.h"
#include "csharedmemory.h"
using namespace com::pera::base::runtime::remote;
#include "ConfigFunc.h"
#include "peraicebase.h"

static Ice::CommunicatorPtr InitCommunicator()
{
	Ice::InitializationData initData;
	initData.properties = Ice::createProperties();
	initData.properties->setProperty("Ice.MessageSizeMax", "10240");//默认是1024，单位KB
	return Ice::initialize(initData);
}

CIceService::CIceService(void) 
	: CNTService(_T("PeraTaskService"), _T("PeraTaskService"), SERVICE_ACCEPT_STOP, _T("用户机器上的ICE服务，用于接收WS服务器分发的任务"))
{
	//m_bEventLogSource = FALSE;
}


CIceService::~CIceService(void)
{
}

static CString GetLocalIP()
{
	CString sHostName,sIPAddress;
	WORD    wVersionRequested;
	WSADATA wsaData;
	int     err;
	char szHostName[256];   
	int  nRetCode;   

	wVersionRequested = MAKEWORD( 2, 0 );    
	err = WSAStartup( wVersionRequested, &wsaData );   
	if ( err != 0 ) 
	{    
		return "";   
	}    

	if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 0 ) 
	{   
		WSACleanup( );   
		return "";    
	}

	nRetCode = gethostname(szHostName, sizeof(szHostName));   

	if (nRetCode != 0) 
	{   
		sHostName = (""); 
		return "";   
	}   

	sHostName = szHostName;

	struct hostent FAR *lpHostEnt = gethostbyname (sHostName);   

	if (lpHostEnt == NULL) 
	{   
		sIPAddress = _T("");
		return sIPAddress;
	}   

	LPSTR lpAddr = lpHostEnt->h_addr_list[0];   
	if (lpAddr) 
	{   
		struct in_addr  inAddr;   
		memmove (&inAddr, lpAddr, 4);   
		sIPAddress = inet_ntoa (inAddr);   
		if (sIPAddress.IsEmpty())   
			sIPAddress = _T("");   
	}

	nRetCode = WSACleanup();  

	if (nRetCode != 0) 
	{ 
		return "";   
	}   

	return sIPAddress;   
}

static std::vector<CString> GetLocalIPs()
{
	std::vector<CString> vIPs;
	CString sHostName,sIPAddress;
	WORD    wVersionRequested;
	WSADATA wsaData;
	int     err;
	char szHostName[256];   
	int  nRetCode;   

	wVersionRequested = MAKEWORD( 2, 0 );    
	err = WSAStartup( wVersionRequested, &wsaData );   
	if ( err != 0 ) 
	{    
		return vIPs;   
	}    

	if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 0 ) 
	{   
		WSACleanup( );   
		return vIPs;    
	}

	nRetCode = gethostname(szHostName, sizeof(szHostName));   

	if (nRetCode != 0) 
	{   
		sHostName = (""); 
		return vIPs;   
	}   

	sHostName = szHostName;

	struct hostent FAR *lpHostEnt = gethostbyname (sHostName);   

	if (lpHostEnt == NULL) 
	{   
		sIPAddress = _T("");
		return vIPs;
	}   

	int i = 0;
	LPSTR lpAddr = lpHostEnt->h_addr_list[i];
	while (lpAddr)
	{
		struct in_addr  inAddr;   
		memmove (&inAddr, lpAddr, 4);   
		sIPAddress = inet_ntoa (inAddr);   
		if (!sIPAddress.IsEmpty())
		{
			vIPs.push_back(sIPAddress);
		}
		i++;
		lpAddr = lpHostEnt->h_addr_list[i];
	}
	
	nRetCode = WSACleanup();  

	if (nRetCode != 0) 
	{ 
		return vIPs;   
	}   

	return vIPs;   
}

#define SM_LOCAL_IP_LENGTH 256
//保存IP的共享内存
static CSharedMemory* s_pSmIp = NULL;
static void WriteIp2Sm(CString sIp)
{
	if (sIp.GetLength() >= SM_LOCAL_IP_LENGTH)
	{
		sIp.Left(SM_LOCAL_IP_LENGTH - 1);
	}
	if (s_pSmIp)
	{
		s_pSmIp->Lock();
		memset(s_pSmIp->GetData(), 0, SM_LOCAL_IP_LENGTH);
		memcpy(s_pSmIp->GetData(), (LPCTSTR)sIp, sIp.GetLength() + 1);
		s_pSmIp->Unlock();
	}
}

//写安全站点信息
static void WriteSafeDomains()
{
	CString sIP = GetConfig("PeraProcessDesigner", "PeraRemoteEngineIp");
	if (sIP.IsEmpty())
	{
		return;
	}

	CString sDomainsDir;
	sDomainsDir.Format("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\ZoneMap\\Domains\\%s", sIP);
	CRegKey keyDomains;
	keyDomains.Create(HKEY_CURRENT_USER, sDomainsDir);
	keyDomains.SetDWORDValue("http", 2);

	CString sRangesDir;
	sRangesDir.Format("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\ZoneMap\\Ranges\\%s", sIP);
	CRegKey keyRanges;
	keyRanges.Create(HKEY_CURRENT_USER, sRangesDir);
	keyRanges.SetDWORDValue("http", 2);
	keyRanges.SetStringValue(":Range", sIP);
}

static DWORD WINAPI InitIpThread(LPVOID lpParameter)
{
	CString sIp("");
	sIp = GetLocalIP();
	ZTools::WriteZToolsFormatLog("默认取得本机IP为%s", sIp);
	WriteIp2Sm(sIp);

	CString sOldIP = GetConfig("PeraProcessDesigner", "PeraRemoteEngineIp", TRUE);
	WriteSafeDomains();

	BOOL bIceSuccsed = FALSE;
	while (TRUE/*bIceSuccsed == FALSE*/)
	{
		CString sIP = GetConfig("PeraProcessDesigner", "PeraRemoteEngineIp", TRUE);
		CString sPort = GetConfig("PeraProcessDesigner", "PeraRemoteEnginePort", TRUE);
		if (sIP.IsEmpty() || sPort.IsEmpty())
		{
			Sleep(5000);
			continue;
		}

		if (sOldIP.CompareNoCase(sIP) != 0)
		{
			sOldIP = sIP;
			WriteSafeDomains();
		}

		CString sEndPoints;
		sEndPoints.Format("WsICEService:tcp -h %s -p %s -t 3000", sIP, sPort);

		ZTools::WriteZToolsFormatLog("通过建立ICE连接确定本机IP-->%s", sEndPoints);

		Ice::CommunicatorPtr communicator;
		try
		{
			communicator = InitCommunicator();
			WsICEServicePrx wsICEServicePrx = WsICEServicePrx::checkedCast(communicator->stringToProxy((LPCTSTR)sEndPoints));
			Ice::TCPConnectionInfoPtr p = Ice::TCPConnectionInfoPtr::dynamicCast(wsICEServicePrx->ice_getConnection()->getInfo());
			sIp = p->localAddress.c_str();
			bIceSuccsed = TRUE;
			WriteIp2Sm(sIp);
			ZTools::WriteZToolsFormatLog("通过建立ICE连接确定的本机IP为%s", sIp);
			communicator->destroy();
		}
		catch(const Ice::Exception& ex)
		{
			//cerr << ex << endl;
			ZTools::WriteZToolsLog(
				ZTools::FormatString("ice_name:%s", 
				ex.ice_name().c_str())
				);
			try
			{
				if(communicator)
				{
					communicator->destroy();
				}
			}
			catch(const Ice::Exception& ex)
			{
				//cerr << ex << endl;
				ZTools::WriteZToolsLog(
					ZTools::FormatString("ice_name:%s", 
					ex.ice_name().c_str())
					);
			}
		}

		//ICE连接取得ip失败后，不断重试
		//20140521,lc,应演示人员要求，经常有线无线切换，修改为定时取本机IP
		Sleep(5000);
	}

	return 0;
}
static void InitIp()
{	
	if (s_pSmIp == NULL)
	{
		s_pSmIp = new CSharedMemory("SM_LOCAL_IP", SM_LOCAL_IP_LENGTH);
		s_pSmIp->Lock();
		memset(s_pSmIp->GetData(), 0, SM_LOCAL_IP_LENGTH);
		s_pSmIp->Unlock();
	}

	DWORD dwThreadId = 0;
	HANDLE hThread = ::CreateThread(
		NULL
		, 0
		, InitIpThread
		, NULL
		, CREATE_SUSPENDED
		, &dwThreadId);

	if (hThread == NULL || hThread == 0)
	{
		ZTools::WriteZToolsFormatLog("创建取本机IP线程失败");
		return;
	}
	ResumeThread(hThread);
	::CloseHandle(hThread);
}

static void ClearIp()
{
	if (s_pSmIp)
	{
		delete s_pSmIp;
		s_pSmIp = NULL;
	}
}

std::string CIceService::GetEndPoints()
{
	CString sPort = GetConfig("PeraProcessDesigner", "PeraTaskServicePort");
	if (sPort.IsEmpty())
	{
		return "";
	}
	std::string strEndPoints;

	//tcp -h 192.168.114.156 -p 30000:tcp -h 127.0.0.2 -p 30000
	//tcp -h %s -p %s:tcp -h localhost -p %s
	/*
	std::vector<CString> vIPs = GetLocalIPs();
	if (vIPs.size() < 1)
	{
		strEndPoints = ZTools::FormatString("tcp -h 127.0.0.1 -p %s", sPort);
	}
	else
	{
		for (std::vector<CString>::iterator it = vIPs.begin(); it != vIPs.end(); ++ it)
		{
			std::string strEndPoint = ZTools::FormatString("tcp -h %s -p %s", *it, sPort);
			strEndPoints += strEndPoint;
			strEndPoints += ":";
		}
		strEndPoints.resize(strEndPoints.length() - 1);
	}
	*/
	
	strEndPoints = ZTools::FormatString("tcp -h 0.0.0.0 -p %s", sPort);

	ZTools::WriteZToolsLog(strEndPoints);

	return strEndPoints;
}

//读reg目录下的reg文件，导入到注册表
static DWORD WINAPI ImportRegThread(LPVOID lpParameter)
{
	std::string strRegDir = ZTools::GetCurrentModulePath() + "reg\\";

	if (PathFileExists(strRegDir.c_str()) == FALSE)
	{
		return 0;
	}

	std::string strRegFiles = strRegDir + "*.reg";

	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(strRegFiles.c_str(), &fd);
	if ( hFind != INVALID_HANDLE_VALUE )
	{
		do{
			if ( !(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
			{
				std::string strFileName(fd.cFileName);
				ZTools::LowerString(strFileName);
				std::string strExt = strFileName.substr(strFileName.find_last_of('.'));
				if (strExt.compare(".reg") != 0)
				{
					continue;
				}
				std::string strRegFile = strRegDir + fd.cFileName;
				
				std::string strCmd = ZTools::FormatString("/s \"%s\"", strRegFile.c_str());
				::ShellExecute( NULL, "open", "regedit.exe", strCmd.c_str(), NULL, SW_HIDE );
			}
		}while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}

	return 0;
}

static void ImportReg()
{
	DWORD dwThreadId = 0;
	HANDLE hThread = ::CreateThread(
		NULL
		, 0
		, ImportRegThread
		, NULL
		, CREATE_SUSPENDED
		, &dwThreadId);

	if (hThread == NULL || hThread == 0)
	{
		ZTools::WriteZToolsFormatLog("创建导入REG线程失败");
		return;
	}
	ResumeThread(hThread);
	::CloseHandle(hThread);
}

void WINAPI CIceService::ServiceMain( DWORD dwArgc, LPTSTR* lpszArgv )
{
	//throw std::exception("The method or operation is not implemented.");
	ZTools::WriteZToolsLog("Function CIceService::ServiceMain");

	ReportStatus(SERVICE_START_PENDING, 1, 1100);
	Sleep(1000);
	ReportStatus(SERVICE_RUNNING);

	//取本机IP，放入共享内存
	InitIp();

	//读reg目录下的reg文件，导入到注册表
	ImportReg();

	std::string strEndPoints = GetEndPoints();
	if (strEndPoints.empty())
	{
		ZTools::WriteZToolsLog("初始化ICE配置项出错");
		return;
	}
	ZTools::WriteZToolsLog(strEndPoints);
	
	try
	{
		communicator = InitCommunicator();
		Ice::ObjectAdapterPtr adapter = communicator->createObjectAdapterWithEndpoints("RobotRemoteService", strEndPoints);
		Ice::ObjectPtr servant = new RobotRemoteServiceI();
		adapter->add(servant, communicator->stringToIdentity("RobotRemoteService"));
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

	ClearIp();

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

	ClearPeraIce();
}