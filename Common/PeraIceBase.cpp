#include "StdAfx.h"
#include "PeraIceBase.h"
#include "Tools.h"

static Ice::CommunicatorPtr s_pCommunicator;
static int s_nMsgSize = 1024;
static CCriticalSection m_critical_section_Communicator;

Ice::CommunicatorPtr InitPeraIce()
{
	if (s_pCommunicator == NULL)
	{
		Ice::InitializationData initData;
		initData.properties = Ice::createProperties();
		std::string strSize = ZTools::FormatString("%d", s_nMsgSize);
		initData.properties->setProperty("Ice.MessageSizeMax", strSize);//默认是1024，单位KB

		m_critical_section_Communicator.Lock();
		if (s_pCommunicator == NULL)
		{
			s_pCommunicator = Ice::initialize(initData);
		}
		m_critical_section_Communicator.Unlock();
	}

	return s_pCommunicator;
}

void ClearPeraIce()
{
	if (s_pCommunicator != NULL)
	{
		s_pCommunicator->destroy();
		s_pCommunicator = NULL;
	}
}

BOOL SetMessageSizeMax(int nSize /* = 1024 */)
{
	if (nSize <= 0)
	{
		return FALSE;
	}

	s_nMsgSize = nSize;

	return TRUE;
}

CPeraIceBase::CPeraIceBase(void)
{
}

CPeraIceBase::~CPeraIceBase(void)
{
	m_pCommunicator = NULL;
}

void CPeraIceBase::InitCommunicator()
{
	if (m_pCommunicator == NULL)
	{
		SetMessageSizeMax(10240);	
		m_pCommunicator = InitPeraIce();
	}
}

void CPeraIceBase::SetCommunicator(Ice::CommunicatorPtr pCommunicator)
{
	m_pCommunicator = pCommunicator;
}
