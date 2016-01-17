#pragma once
#include "pera601democlientice.h"

using namespace pera::demo601Client;

class CClientIceServiceI :
	public PeraDemo601ClientService
{
public:
	CClientIceServiceI(void);
	~CClientIceServiceI(void);
	std::string InvokeMethod(const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current());
};

