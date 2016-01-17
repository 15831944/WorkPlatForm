#pragma once
#include <Ice/Application.h>
#include <IceUtil/Monitor.h>
#include "PeraProcessRunnerCacheServerIce.h"
#include <vector>
#include <map>
#include <list>

using namespace std;


using namespace pera::runner::assist;


class CPeraWsAstServer : public PeraWsAstICE
{
public:
	CPeraWsAstServer(void);
	virtual ~CPeraWsAstServer(void);

public:
	virtual ::std::string GetRunId(const ::Ice::Current& = ::Ice::Current());
	virtual ::std::string Stop(const ::std::string&, const ::Ice::Current& = ::Ice::Current());
	virtual void CreateCacheByWS_async(const ::pera::runner::assist::AMD_PeraWsAstICE_CreateCacheByWSPtr&, const ::std::string&, const ::Ice::Current& = ::Ice::Current());
	virtual void CreateWSByCache_async(const ::pera::runner::assist::AMD_PeraWsAstICE_CreateWSByCachePtr&, const ::std::string&, const ::Ice::Current& = ::Ice::Current());
	virtual void CopyCache_async(const ::pera::runner::assist::AMD_PeraWsAstICE_CopyCachePtr&, const ::std::string&, const ::Ice::Current& = ::Ice::Current());
	virtual void DeleteCache_async(const ::pera::runner::assist::AMD_PeraWsAstICE_DeleteCachePtr&, const ::std::string&, const ::Ice::Current& = ::Ice::Current());
	virtual void CreateCacheByXpdlStream_async(const ::pera::runner::assist::AMD_PeraWsAstICE_CreateCacheByXpdlStreamPtr&, const ::std::string&, const ::Ice::Current& = ::Ice::Current());
	virtual void CreateWSByXpdlStream_async(const ::pera::runner::assist::AMD_PeraWsAstICE_CreateWSByXpdlStreamPtr&, const ::std::string&, const ::Ice::Current& = ::Ice::Current());

protected:
	BOOL	AddToRQueue( LPCTSTR lpRunId, LPCTSTR lpFlag = NULL );
	BOOL	AddToTQueue( LPCTSTR lpRunId, LPCTSTR lpFlag = NULL );
	BOOL	AddToDQueue( LPCTSTR lpRunId );
	BOOL	DelFromTQueue( LPCTSTR lpRunId, LPCTSTR lpFlag = NULL );
	BOOL	DelFromRQueue( LPCTSTR lpRunId, LPCTSTR lpFlag = NULL );
	BOOL	DelFromDQueue( LPCTSTR lpRunId );
	BOOL	FindFromRQueue( LPCTSTR lpRunId, LPCTSTR lpFlag = NULL );
	BOOL	FindFromTQueue( LPCTSTR lpRunId, LPCTSTR lpFlag = NULL );
	BOOL	FindFromDQueue( LPCTSTR lpRunId );
	vector< map< CString, CString >	>		m_vecRunningQueue;
	vector< map< CString, CString >	>		m_vecTerminationQueue;
	list< CString >							m_listDelQueue;
	CCriticalSection						m_CriticalExecuteTask;
	CCriticalSection						m_CriticalTermination;
};
