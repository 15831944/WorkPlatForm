//�������ṩ�Ľӿ�����ice�ļ����Ʒ�װ��WS�ӿ��࣬���ʱ���WS����
#pragma once

#include "PeraIceWsBase.h"

//�ص�����ָ��
typedef int(CALLBACK * cbInvokeLocalWsMethod)(CString);
typedef int(CALLBACK * cbInvokeLocalWsMethodOrignal)(CString);

class AMI_WsICEService_invokeLocalWsMethod_I :
	public AMI_WsICEService_invokeLocalWsMethod
{
public:
	AMI_WsICEService_invokeLocalWsMethod_I(cbInvokeLocalWsMethod cbFunc);
	virtual void ice_response( const ::std::string& );
	virtual void ice_exception( const Ice::Exception& );

private:
	cbInvokeLocalWsMethod m_cbFunc;
};

class AMI_WsICEService_invokeLocalWsMethodOrignal_I :
	public AMI_WsICEService_invokeLocalWsMethod
{
public:
	AMI_WsICEService_invokeLocalWsMethodOrignal_I(cbInvokeLocalWsMethodOrignal cbFunc);
	virtual void ice_response( const ::std::string& );
	virtual void ice_exception( const Ice::Exception& );

private:
	cbInvokeLocalWsMethodOrignal m_cbFunc;
};

class AMI_WsICEService_runLocalRobotFinished_I : public AMI_WsICEService_runLocalRobotFinished
{
	virtual void ice_response( const ::std::string& );
	virtual void ice_exception( const Ice::Exception& ex);
};

class CPeraIceLocalWs :
	public CPeraIceWsBase
{
public:
	CPeraIceLocalWs(void);
	~CPeraIceLocalWs(void);

protected:
	virtual std::string GetIceEndPoints();

public:
	std::string InvokeLocalWsMethodOrignal(std::string strMethod, std::string strJson);

	BOOL InvokeLocalWsMethod_Async(std::string strMethod, std::string strJson, AMI_WsICEService_invokeLocalWsMethod_I* pCbObj);
	BOOL InvokeLocalWsMethodOrignal_Async(std::string strMethod, std::string strJson, AMI_WsICEService_invokeLocalWsMethodOrignal_I* pCbObj);
	
	BOOL runLocalRobotLog(std::string strLog);
	BOOL runLocalRobotRate(std::string strRate);
	std::string runLocalRobotFinished(std::string strTaskId, std::string strOutxml, std::string strRunType);
	BOOL amiRunLocalRobotFinished(std::string strTaskId, std::string strOutxml, std::string strRunType);
};

