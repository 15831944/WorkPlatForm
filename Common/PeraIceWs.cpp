#include "StdAfx.h"
#include "PeraIceWs.h"
#include "Tools.h"
#include "ConfigFunc.h"

//线程，检测如果IP发生变化，重新连接新的服务器
static DWORD WINAPI CheckIpThread(LPVOID lpParameter)
{
	CPeraIceWs* pPeraIceWs = (CPeraIceWs*)lpParameter;

	CString sIP = GetConfig("PeraProcessDesigner", "PeraRemoteEngineIp", TRUE);
	CString sPort = GetConfig("PeraProcessDesigner", "PeraRemoteEnginePort", TRUE);

	while (TRUE)
	{
		Sleep(5000);

		CString sNewIP = GetConfig("PeraProcessDesigner", "PeraRemoteEngineIp", TRUE);
		CString sNewPort = GetConfig("PeraProcessDesigner", "PeraRemoteEnginePort", TRUE);

		if (sNewIP.CompareNoCase(sIP) !=0 || sNewPort.Compare(sPort) != 0)
		{
			//检测到配置文件中IP发生变化，进行变更
			ZTools::WriteZToolsFormatLog("检测到配置文件中服务器IP发生变化");
			if (pPeraIceWs->ChangeProxy())
			{
				ZTools::WriteZToolsFormatLog("成功连接变更后的服务器IP");
				sIP = sNewIP;
				sPort = sNewPort;
			}
		}
	}

	return 0;
}

CPeraIceWs::CPeraIceWs(void)
{
	m_bCheckIpThreadStarted = FALSE;
}

CPeraIceWs::~CPeraIceWs(void)
{
}

BOOL CPeraIceWs::StartThreadCheckIp()
{
	if (m_bCheckIpThreadStarted == FALSE)
	{
		m_bCheckIpThreadStarted = TRUE;
		DWORD dwThreadId = 0;
		HANDLE hThread = ::CreateThread(
			NULL
			, 0
			, CheckIpThread
			, this
			, CREATE_SUSPENDED
			, &dwThreadId);

		if (hThread == NULL || hThread == 0)
		{
			ZTools::WriteZToolsFormatLog("创建CheckIp线程失败");
			return FALSE;
		}
		ResumeThread(hThread);
		::CloseHandle(hThread);
	}

	return TRUE;
}

BOOL CPeraIceWs::ChangeProxy()
{
	try
	{
		if (m_pPrx == NULL)
		{
			return TRUE;
		}
		else
		{
			m_critical_section_Prx.Lock();
			WsICEServicePrx pPrx;
			InitCommunicator();
			pPrx = WsICEServicePrx::checkedCast(m_pCommunicator->stringToProxy(GetIceEndPoints()));
			pPrx = pPrx->ice_twoway()->ice_timeout(20000)->ice_secure(false);
			m_pPrx = pPrx;
			m_critical_section_Prx.Unlock();	
			return TRUE;
		}
	}
	catch(const Ice::Exception& ex)
	{
		m_critical_section_Prx.Unlock();
		ZTools::WriteZToolsFormatLog("ICE异常：%s", ex.ice_name().c_str());
		return FALSE;
	}
	return FALSE;
}

std::string CPeraIceWs::GetIceEndPoints()
{
	CString sIP = GetConfig("PeraProcessDesigner", "PeraRemoteEngineIp", TRUE);
	CString sPort = GetConfig("PeraProcessDesigner", "PeraRemoteEnginePort", TRUE);

	if (sIP.IsEmpty() || sPort.IsEmpty())
	{
		return "";
	}

	std::string strEndPoints = ZTools::FormatString("WsICEService:tcp -h %s -p %s", sIP, sPort);
	return strEndPoints;
}

std::string CPeraIceWs::InvokeWsMethodOrignal( std::string strMethod, std::string strJson )
{
	std::string strResult;

	if (!InitProxy())
	{
		return strResult;
	}

	try
	{
		strResult = m_pPrx->invokeWsMethod(strMethod, strJson);	 	
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog("ICE异常：%s", ex.ice_name().c_str());
	}

	return strResult;
}

BOOL CPeraIceWs::runRobotLog(std::string strLog)
{
	BOOL bResult = FALSE;

	if (!InitProxy())
	{
		return bResult;
	}

	try
	{
		m_pPrx->runRobotLog(strLog);	 
		bResult = TRUE;
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog("ICE异常：%s", ex.ice_name().c_str());
		bResult = FALSE;
	}

	return bResult;
}

BOOL CPeraIceWs::runRobotRate(std::string strRate)
{
	BOOL bResult = FALSE;

	if (!InitProxy())
	{
		return bResult;
	}

	try
	{
		m_pPrx->runRobotRate(strRate);	 
		bResult = TRUE;
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog("ICE异常：%s", ex.ice_name().c_str());
		bResult = FALSE;
	}

	return bResult;
}

std::string CPeraIceWs::runRobotFinished(std::string strTaskId, std::string strOutxml, std::string strRunType)
{
	std::string strResult;

	if (!InitProxy())
	{
		return strResult;
	}

	try
	{
		strResult = m_pPrx->runRobotFinished(strTaskId, strOutxml, strRunType);	 	
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog("ICE异常：%s", ex.ice_name().c_str());
	}

	return strResult;
	return "";
}

BOOL CPeraIceWs::amiRunRobotFinished(std::string strTaskId, std::string strOutxml, std::string strRunType)
{
	BOOL bResult = FALSE;

	if (!InitProxy())
	{
		return bResult;
	}

	try
	{
		bResult = m_pPrx->runRobotFinished_async(new AMI_WsICEService_runRobotFinished_I, strTaskId, strOutxml, strRunType);
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog("ICE异常：%s", ex.ice_name().c_str());
		bResult = FALSE;
	}

	return bResult;
}

std::string CPeraIceWs::savePlanByByteXpdl(std::string strJsonInfo, ByteSeq byteSeq)
{
	std::string strResult;

	if (!InitProxy())
	{
		return strResult;
	}

	try
	{
		strResult = m_pPrx->savePlanByByteXpdl(strJsonInfo, byteSeq);	 	
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog("ICE异常：%s", ex.ice_name().c_str());
	}

	return strResult;
}

void AMI_WsICEService_runRobotFinished_I::ice_response( const ::std::string& str)
{
	std::string strResponse = str;
	ZTools::UTF8ToMB(strResponse);
	ZTools::WriteZToolsFormatLog("调用运行组件完成return:%s", strResponse.c_str());
}

void AMI_WsICEService_runRobotFinished_I::ice_exception( const Ice::Exception& ex)
{
	ZTools::WriteZToolsLog(
		ZTools::FormatString("调用finish接口出现异常\nice_name:%s", 
		ex.ice_name().c_str())
		);
}