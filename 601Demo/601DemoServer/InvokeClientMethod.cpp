#include "StdAfx.h"
#include "601DemoServer.h"
#include "InvokeClientMethod.h"
#include <ice/ice.h>
#include "JsonData.h"

extern CString GetLocalIP();

InvokeClientMethod::InvokeClientMethod(void)
{
	m_strCLientPort.Empty();
	m_strClientIp.Empty();
}


InvokeClientMethod::~InvokeClientMethod(void)
{
	if (m_communicatorPtr)
	{
		m_communicatorPtr->destroy();
		m_pPrx = NULL;
	}
}

BOOL InvokeClientMethod::InitProxy(std::string & strErrMsg)
{
	try
	{
		m_critical_section_Prx.Lock();
		m_communicatorPtr = InitCommunicator();
		m_pPrx = PeraDemo601ClientServicePrx::checkedCast(m_communicatorPtr->stringToProxy(GetIceEndPoints()));
		m_pPrx = m_pPrx->ice_twoway()->ice_timeout(5000)->ice_secure(false);
		m_critical_section_Prx.Unlock();
	}
	catch(const Ice::Exception& ex)
	{
		m_critical_section_Prx.Unlock();
		CString strMsg;
		strMsg.Format("ICE异常：%s", ex.ice_name().c_str());
		strErrMsg = (LPCTSTR)strMsg;
		theApp.TraceRunLog(strMsg);
		return FALSE;
	}
	return TRUE;
}

void InvokeClientMethod::ClearProxy()
{
	if (m_pPrx != NULL)
	{
		m_pPrx = NULL;
	}
}
Ice::CommunicatorPtr InvokeClientMethod::InitCommunicator()
{
	Ice::InitializationData initData;
	initData.properties = Ice::createProperties();
	initData.properties->setProperty("Ice.MessageSizeMax", "1024");//默认是1024，单位KB
	if (m_communicatorPtr == NULL)
		m_communicatorPtr = Ice::initialize(initData);

	return m_communicatorPtr;
}
std::string InvokeClientMethod::GetIceEndPoints()
{
	CString Str;
	Str.Format("Pera601DemoClientService:tcp -h %s -p %s -t 5000", m_strClientIp, m_strCLientPort);
	std::string strEndPoints = Str;
	return strEndPoints;
}
std::string InvokeClientMethod::InvokeMethod(std::string strMethod, std::string strJson,std::string strClientIp,std::string strClientPort,std::string & strErrMsg)
{
	std::string strResult;
	CString strMsg;
	strMsg.Format("开始远程调用客户端,IP = %s 端口号=%s,方法名称[%s],调用参数[%s]",strClientIp.c_str(),strClientPort.c_str(),strMethod.c_str(),strJson.c_str());
	theApp.TraceRunLog(strMsg);
	m_strClientIp = strClientIp.c_str();
	m_strCLientPort = strClientPort.c_str();

	if (!InitProxy(strErrMsg))
	{
		strErrMsg = "服务器远程调用客户端失败:" + strErrMsg;
		theApp.TraceRunLog("服务器远程调用客户端失败");
		return strResult;
	}

	try
	{
		strResult = m_pPrx->InvokeMethod(strMethod, strJson);	 	
	}
	catch(const Ice::Exception& ex)
	{
		strMsg.Format("远程调用客户端失败,ICE异常：%s", ex.ice_name().c_str());
		strErrMsg = strMsg;
		theApp.TraceRunLog(strMsg);
		return strResult;
	}

	strMsg.Format("远程调用客户端结束,客户端返回%s",strResult.c_str());
	theApp.TraceRunLog(strMsg);
	return strResult;
}
