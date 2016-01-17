#pragma once
#include "Pera601DemoServerIce.h"
using namespace pera::Demo601Server;

class CPeraServerIceI :
	public PeraDemo601ServerService
{
public:
	CPeraServerIceI(void);
	~CPeraServerIceI(void);
 virtual ::std::string InvokeMethod(const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current());
};

