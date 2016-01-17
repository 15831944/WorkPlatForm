//按引擎提供的接口描述ice文件定制封装的WS接口类，访问远程WS引擎
#pragma once

#include "PeraIceWsBase.h"

class CPeraIceWs :
	public CPeraIceWsBase
{
public:
	CPeraIceWs(void);
	~CPeraIceWs(void);

	BOOL ChangeProxy();
	virtual BOOL StartThreadCheckIp();
	BOOL m_bCheckIpThreadStarted;

protected:
	virtual std::string GetIceEndPoints();

public:
	std::string InvokeWsMethodOrignal(std::string strMethod, std::string strJson);
	BOOL runRobotLog(std::string strLog);
	BOOL runRobotRate(std::string strRate);
	std::string runRobotFinished(std::string strTaskId, std::string strOutxml, std::string strRunType);
	BOOL amiRunRobotFinished(std::string strTaskId, std::string strOutxml, std::string strRunType);
	std::string savePlanByByteXpdl(std::string strJsonInfo, ByteSeq byteSeq);
};

class AMI_WsICEService_runRobotFinished_I : public AMI_WsICEService_runRobotFinished
{
	virtual void ice_response( const ::std::string& );
	virtual void ice_exception( const Ice::Exception& ex);
};