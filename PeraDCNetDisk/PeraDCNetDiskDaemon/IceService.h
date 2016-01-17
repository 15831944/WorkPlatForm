#pragma once
#include "ntserv.h"
#include <Ice/Ice.h>
class CIceService :
	public CNTService
{
public:
	CIceService(void);
	~CIceService(void);

	virtual void WINAPI ServiceMain( DWORD dwArgc, LPTSTR* lpszArgv );

	virtual void OnStop();
	Ice::CommunicatorPtr communicator;
};

