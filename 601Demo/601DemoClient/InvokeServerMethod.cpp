#include "StdAfx.h"
#include "601DemoClient.h"
#include "InvokeServerMethod.h"
#include <ice/ice.h>
#include "JsonData.h"
#include <string>
using namespace std;
extern CString GetLocalIP();
string UTF8_TO_MB(const string & str)
{
	int nLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);  

	WCHAR * lpszW = NULL;  
	try  
	{  
		lpszW = new WCHAR[nLen];  
	}  
	catch(std::bad_alloc &/*memExp*/)  
	{  
		return "";  
	}  

	int nRtn = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, lpszW, nLen);  

	if(nRtn != nLen)  
	{  
		delete[] lpszW;  
		return "";  
	}  

	// convert an widechar string to Multibyte   
	int MBLen = WideCharToMultiByte(CP_ACP, 0, lpszW, nLen, NULL, 0, NULL, NULL);  
	if (MBLen <=0)  
	{  
		return false;  
	}	
	char * pmb = new char[MBLen+1];
	nRtn = WideCharToMultiByte(CP_ACP, 0, lpszW, nLen, pmb, MBLen, NULL, NULL);
	delete[] lpszW;  

	if(nRtn != MBLen)  
	{		
		delete pmb;
		return "";  
	}  

	string strOut = pmb;
	delete pmb;
	return strOut;  
}

InvokeServerMethod::InvokeServerMethod(void)
{
	m_strServerPort = "20131";
	m_strServerIp = "127.0.0.1";
	m_strLocalPort = "20133";
	m_bServerConfigChanged = FALSE;
}


InvokeServerMethod::~InvokeServerMethod(void)
{
	if (m_communicatorPtr)
	{
		m_communicatorPtr->destroy();
		m_pPrx = NULL;
	}
}

BOOL InvokeServerMethod::InitProxy()
{
	try
	{
		if (m_bServerConfigChanged)
		{
			m_pPrx = NULL;
			m_bServerConfigChanged = FALSE;
		}
		if (m_pPrx == NULL)
		{
			m_critical_section_Prx.Lock();
			if (m_pPrx == NULL)
			{
				m_strLocalIp.Empty();
				m_communicatorPtr = InitCommunicator();

				::Ice::ObjectPrx ptr = m_communicatorPtr->stringToProxy(GetIceEndPoints());
				m_pPrx = PeraDemo601ServerServicePrx::checkedCast(ptr);
				m_pPrx = m_pPrx->ice_twoway()->ice_timeout(20000)->ice_secure(false);

				Ice::TCPConnectionInfoPtr p = Ice::TCPConnectionInfoPtr::dynamicCast(m_pPrx->ice_getConnection()->getInfo());
				m_strLocalIp = p->localAddress.c_str();
			}
			m_critical_section_Prx.Unlock();
		} 	
	}
	catch(const Ice::Exception& ex)
	{
		m_critical_section_Prx.Unlock();
		CString strErrMsg;
		strErrMsg.Format("ICE异常：%s", ex.ice_name().c_str());
		theApp.TraceRunLog(strErrMsg);
		return FALSE;
	}
	return TRUE;
}

void InvokeServerMethod::ClearProxy()
{
	if (m_pPrx != NULL)
	{
		m_pPrx = NULL;
	}
}
Ice::CommunicatorPtr InvokeServerMethod::InitCommunicator()
{
	Ice::InitializationData initData;
	initData.properties = Ice::createProperties();
	initData.properties->setProperty("Ice.MessageSizeMax", "1024");//默认是1024，单位KB
	if (m_communicatorPtr == NULL)
		m_communicatorPtr = Ice::initialize(initData);

	return m_communicatorPtr;
}
void InvokeServerMethod::SetServerIP(CString strServerIp)
{
	m_strServerIp = strServerIp;
	m_bServerConfigChanged = TRUE;
}
void InvokeServerMethod::SetLocalPort(CString strLocalPort)
{
	m_strLocalPort = strLocalPort;
}
void InvokeServerMethod::SetServerPort(CString strServerPort)
{
	m_strServerPort = strServerPort;
	m_bServerConfigChanged = TRUE;
}
std::string InvokeServerMethod::GetIceEndPoints()
{
	CString Str;
	Str.Format("Pera601DemoServerService:tcp -h %s -p %s -t 5000", m_strServerIp, m_strServerPort);
	std::string strEndPoints = Str;
	return strEndPoints;
}
std::string InvokeServerMethod::InvokeMethod(std::string strMethod, std::string strJson)
{
	std::string strResult;
	theApp.TraceRunLog("开始远程调用服务端");
	if (!InitProxy())
	{
		theApp.TraceRunLog("---------------客户端远程调用服务端失败！-------------------------");
		return strResult;
	}

	CDataRpcInvokeServer DataInvoke;
	DataInvoke.m_strParam = strJson.c_str();
	DataInvoke.m_strPort = m_strLocalPort;
	if (!m_strLocalIp.IsEmpty())
		DataInvoke.m_strIp = m_strLocalIp; // 优先使用与服务器建立连接的本机IP地址
	else
		DataInvoke.m_strIp    = GetLocalIP();

	CString strJsonStr = DataInvoke.GetJsonStr();

	CString strMsg;
	strMsg.Format("调用服务端方法名称[%s],调用参数[%s],服务器地址：%s,服务器端口号：%s",strMethod.c_str(),strJsonStr,m_strServerIp,m_strServerPort);
	theApp.TraceRunLog(strMsg);

	try
	{
		strResult = m_pPrx->InvokeMethod(strMethod, (LPCTSTR)strJsonStr);	 	
	}
	catch(const Ice::Exception& ex)
	{
		CString strErrMsg;
		strErrMsg.Format("远程调用服务端失败,ICE异常：%s", ex.ice_name().c_str());
		theApp.TraceRunLog(strErrMsg);
		theApp.TraceRunLog("---------------客户端远程调用服务端失败！-------------------------");
		return strResult;
	}

	if (!strResult.empty())
	{
		theApp.TraceRunLog("---------------客户端远程调用服务端成功！-------------------------");
		strMsg.Format("服务器端返回信息:%s",strResult.c_str());
		if (strResult == "Success")
			theApp.TraceRunLog("---------------服务端远程调用客户端成功！-------------------------");
		else
		{
			strResult = UTF8_TO_MB(strResult);
			theApp.TraceRunLog(strResult.c_str());
			theApp.TraceRunLog("---------------服务端远程调用客户端失败！-------------------------");
		}
	}
	else
		theApp.TraceRunLog("---------------客户端远程调用服务端失败！-------------------------");

	return strResult;
}
