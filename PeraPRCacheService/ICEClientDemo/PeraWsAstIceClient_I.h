#pragma once
#include <Ice/Ice.h>
#include "PeraProcessRunnerCacheServerIce.h"

using namespace std;


using namespace pera::runner::assist;



class AMI_PeraWsAstICE_CreateCacheByWS_I : public AMI_PeraWsAstICE_CreateCacheByWS
{
public:
	void ice_response( const ::std::string& sRet );
	void ice_exception( const Ice::Exception& ex);
};


class AMI_PeraWsAstICE_CreateWSByCache_I : public AMI_PeraWsAstICE_CreateWSByCache
{
public:
	void ice_response( const ::std::string& sRet );
	void ice_exception( const Ice::Exception& ex);
};


class AMI_PeraWsAstICE_CopyCache_I : public AMI_PeraWsAstICE_CopyCache
{
public:
	void ice_response( const ::std::string& sRet );
	void ice_exception( const Ice::Exception& ex);
};


class AMI_PeraWsAstICE_DeleteCache_I : public AMI_PeraWsAstICE_DeleteCache
{
public:
	void ice_response( const ::std::string& sRet );
	void ice_exception( const Ice::Exception& ex);
};


class AMI_PeraWsAstICE_CreateCacheByXpdlStream_I : public AMI_PeraWsAstICE_CreateCacheByXpdlStream
{
public:
	void ice_response( const ::std::string& sRet );
	void ice_exception( const Ice::Exception& ex);
};


class AMI_PeraWsAstICE_CreateWSByXpdlStream_I : public AMI_PeraWsAstICE_CreateWSByXpdlStream
{
public:
	void ice_response( const ::std::string& sRet );
	void ice_exception( const Ice::Exception& ex);
};



class CPeraWsAstIceClient_I
{
public:
	CPeraWsAstIceClient_I(void);
	virtual ~CPeraWsAstIceClient_I(void);

public:
	BOOL InitIceProxy();
	BOOL Destroy();

	std::string GetRunId();
	std::string Stop( LPCTSTR lpRunId );

	BOOL CreateCacheByWS( LPCTSTR lpRunId, LPCTSTR lpWsPath );
	BOOL CreateWSByCache( LPCTSTR lpRunId, LPCTSTR lpWsPath );
	BOOL CopyCache( LPCTSTR lpCacheId, LPCTSTR lpRunId );
	BOOL DeleteCache( LPCTSTR lpRunId );
	BOOL CreateCacheByXpdlStream( LPCTSTR lpRunId, LPCTSTR lpXpdlStream );
	BOOL CreateWSByXpdlStream( LPCTSTR lpRunId, LPCTSTR lpWsPath, LPCTSTR lpXpdlStream );

protected:
	IceUtil::Monitor<IceUtil::Mutex>		m_monitor_Prx;
	IceUtil::Monitor<IceUtil::Mutex>		m_monitor_Communicator;
	Ice::CommunicatorPtr					m_pCommunicator;
	PeraWsAstICEPrx							m_pPrx;
};