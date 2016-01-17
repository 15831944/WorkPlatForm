#include "StdAfx.h"
#include "PeraIceRunRobot.h"
#include "Tools.h"

CPeraIceRunRobot::CPeraIceRunRobot(void)
{
	ClearProxy();
}

CPeraIceRunRobot::~CPeraIceRunRobot(void)
{
	ClearProxy();
}

BOOL CPeraIceRunRobot::InitProxy()
{
	try
	{
		if (m_pPrx == NULL)
		{
			m_critical_section_Prx.Lock();
			if (m_pPrx == NULL)
			{
				InitCommunicator();
				m_pPrx = RunTimeICEServicePrx::checkedCast(m_pCommunicator->stringToProxy(GetIceEndPoints()));
				m_pPrx = m_pPrx->ice_twoway()->ice_timeout(20000)->ice_secure(false);
			}
			m_critical_section_Prx.Unlock();
		} 	
	}
	catch(const Ice::Exception& ex)
	{
		m_critical_section_Prx.Unlock();
		ZTools::WriteZToolsFormatLog("ICE“Ï≥££∫%s", ex.ice_name().c_str());
		return FALSE;
	}
	return TRUE;
}

void CPeraIceRunRobot::ClearProxy()
{
	if (m_pPrx != NULL)
	{
		m_pPrx = NULL;
	}
}

std::string CPeraIceRunRobot::GetIceEndPoints()
{
	std::string strEndPoints;

	std::string strIp;
	strIp = "localhost";
	std::string strPort;
	strPort = "20139";

	strEndPoints = ZTools::FormatString("FlexwareRuntimeICEService:tcp -h %s -p %s", strIp.c_str(), strPort.c_str());
	
	return strEndPoints;
}

BOOL CPeraIceRunRobot::RunRobot(std::string strArgs, int nProcessId)
{
	if (!InitProxy())
	{
		return FALSE;
	}

	try
	{
		m_pPrx->RunRobot(strArgs, nProcessId); 	
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog("ICE“Ï≥££∫%s", ex.ice_name().c_str());
		return FALSE;
	}

	return TRUE;
}

