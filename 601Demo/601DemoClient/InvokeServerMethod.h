#pragma once
#include "Pera601DemoServerIce.h"
using namespace pera::Demo601Server;

class InvokeServerMethod
{
public:
	InvokeServerMethod(void);
	~InvokeServerMethod(void);
	void SetServerIP(CString strServerIp);
	void SetServerPort(CString strServerPort);
	void SetLocalPort(CString strLocalPort);

protected:
	virtual BOOL InitProxy();
	virtual void ClearProxy();
	Ice::CommunicatorPtr InitCommunicator();
	virtual std::string GetIceEndPoints();

	PeraDemo601ServerServicePrx m_pPrx;
	CCriticalSection m_critical_section_Prx;
private:
	Ice::CommunicatorPtr m_communicatorPtr;
	CString m_strServerPort;
	CString m_strServerIp;
	BOOL m_bServerConfigChanged;
	CString m_strLocalPort;
	CString m_strLocalIp;
public:
	std::string InvokeMethod(std::string strMethod, std::string strJson);
};

