#pragma once
#include "RobotRemoteService.h"
#include <set>

class RobotRemoteServiceI : public com::pera::base::runtime::remote::RobotRemoteService
{
public:
	RobotRemoteServiceI();
	~RobotRemoteServiceI();

	virtual ::std::string runRobotStart( const ::std::string&, const ::Ice::Current& = ::Ice::Current( ) );
	virtual void runRobotStop( const ::std::string&, const ::Ice::Current& = ::Ice::Current( ) );
	virtual ::std::string runLocalRobotStart(const ::std::string&, const ::Ice::Current& = ::Ice::Current() );

	static DWORD WINAPI PrepareComponentDataThread(LPVOID lpParameter);
	static BOOL PrepareComponentDataThreadFunc(LPVOID lpParameter);

	static DWORD WINAPI PrepareComponentDataThreadLocal(LPVOID lpParameter);
	static BOOL PrepareComponentDataThreadFuncLocal(LPVOID lpParameter);

	//static BOOL GetSvnConfig(std::string& strRootUrl, std::string& strUser, std::string& strPassword);
	static std::string ProcessInxml( std::string strInxmlContext, std::string strBasePath, std::string strTaskId );
	static std::string DownloadHprose(std::string strFileId, std::string strFileDir, std::string strFileName);
	static std::string ProcessInxml2( std::string strInxmlContext, std::string strBasePath ); //多线程版本
	static std::string DownloadHprose2( std::string strJson );//多线程版本

private:
	std::set<std::string> m_setId;

};

