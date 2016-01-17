#pragma once
#include <Ice/Ice.h>

class CIceService
{
public:
	CIceService(void);
	~CIceService(void);

	void StartService();
	void StartServiceInthread();
	void SetServerPort(std::string strPortNum);
	virtual void OnStop();
	std::string GetPort();
	static DWORD WINAPI ThreadFunService(LPVOID);
protected:
	virtual std::string GetEndPoints();
	virtual Ice::CommunicatorPtr CIceService::InitCommunicator();
private:
	Ice::CommunicatorPtr m_communicatorPtr;
	std::string  m_strPort;
	HANDLE  m_hThread;
};

