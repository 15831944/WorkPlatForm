#include "StdAfx.h"
#include "PeraIceWsBase.h"
#include "Tools.h"

CPeraIceWsBase::CPeraIceWsBase(void)
{
	ClearProxy();
}

CPeraIceWsBase::~CPeraIceWsBase(void)
{
	ClearProxy();
}

BOOL CPeraIceWsBase::InitProxy()
{
	try
	{
		if (m_pPrx == NULL)
		{
			m_critical_section_Prx.Lock();
			if (m_pPrx == NULL)
			{
				InitCommunicator();
				m_pPrx = WsICEServicePrx::checkedCast(m_pCommunicator->stringToProxy(GetIceEndPoints()));
				m_pPrx = m_pPrx->ice_twoway()->ice_timeout(20000)->ice_secure(false);
				StartThreadCheckIp();
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

void CPeraIceWsBase::ClearProxy()
{
	if (m_pPrx != NULL)
	{
		m_pPrx = NULL;
	}
}

BOOL CPeraIceWsBase::StartThreadCheckIp()
{
	return TRUE;
}
