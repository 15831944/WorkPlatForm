#pragma once
#include "Pera601DemoClientIce.h"
using namespace pera::demo601Client;

class InvokeClientMethod
{
public:
	InvokeClientMethod(void);
	~InvokeClientMethod(void);
protected:
	virtual BOOL InitProxy(std::string & strErrMsg);
	virtual void ClearProxy();
	Ice::CommunicatorPtr InitCommunicator();
	virtual std::string GetIceEndPoints();

	PeraDemo601ClientServicePrx m_pPrx;
	CCriticalSection m_critical_section_Prx;
private:
	Ice::CommunicatorPtr m_communicatorPtr;
	CString m_strCLientPort;
	CString m_strClientIp;
public:
	std::string InvokeMethod(std::string strMethod, std::string strJson,std::string strClientIp,std::string strClientPort,std::string & strErrMsg);
};

