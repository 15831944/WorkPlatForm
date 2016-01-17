#include "StdAfx.h"
#include "PeraIceNetDisk.h"
#include "Tools.h"

CPeraIceNetDisk::CPeraIceNetDisk(void)
{
	ClearProxy();
}

CPeraIceNetDisk::~CPeraIceNetDisk(void)
{
	ClearProxy();
}

BOOL CPeraIceNetDisk::InitProxy()
{
	try
	{
		if (m_pPrx == NULL)
		{
			m_critical_section_Prx.Lock();
			if (m_pPrx == NULL)
			{
				InitCommunicator();
				m_pPrx = PeraNetDiskDaemonServicePrx::checkedCast(m_pCommunicator->stringToProxy(GetIceEndPoints()));
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

void CPeraIceNetDisk::ClearProxy()
{
	if (m_pPrx != NULL)
	{
		m_pPrx = NULL;
	}
}

std::string CPeraIceNetDisk::GetIceEndPoints()
{
	std::string strEndPoints;

	std::string strIp;
	strIp = "127.0.0.1";
	std::string strPort;
	strPort = " 20137";

	strEndPoints = ZTools::FormatString("PeraNetDiskDaemonService:tcp -h %s -p %s", strIp.c_str(), strPort.c_str());

	return strEndPoints;
}

std::string CPeraIceNetDisk::InvokeMethod(std::string strMethod, std::string strJson)
{
	std::string strResult;

	if (!InitProxy())
	{
		return strResult;
	}

	try
	{
		strResult = m_pPrx->InvokeMethod(strMethod, strJson);	 	
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog("ICE“Ï≥££∫%s", ex.ice_name().c_str());
	}

	return strResult;
}
