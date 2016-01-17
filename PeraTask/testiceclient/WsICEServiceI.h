#pragma once
#include "WsICEService.h"
class WsICEServiceI : public com::pera::base::runtime::remote::WsICEService
{
public:
	WsICEServiceI(void);
	~WsICEServiceI(void);

	virtual ::std::string runRobotFinished( const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current( ) );

	virtual ::std::string runRobotFinished(const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current() );

	virtual void runRobotLog( const ::std::string&, const ::Ice::Current& = ::Ice::Current( ) );

	virtual void runRobotRate( const ::std::string&, const ::Ice::Current& = ::Ice::Current( ) );

	virtual ::std::string invokeWsMethod( const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current( ) );

	virtual ::std::string invokeLocalWsMethod( const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current( ) );

	virtual ::std::string runLocalRobotFinished(const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current() );

	virtual void runLocalRobotLog(const ::std::string&, const ::Ice::Current& = ::Ice::Current() );

	virtual void runLocalRobotRate(const ::std::string&, const ::Ice::Current& = ::Ice::Current() );

	virtual ::std::string savePlanByByteXpdl(const ::std::string&, const ::com::pera::base::runtime::remote::ByteSeq&, const ::Ice::Current& = ::Ice::Current() );

};


