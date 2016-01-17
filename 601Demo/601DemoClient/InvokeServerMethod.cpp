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
		strErrMsg.Format("ICE�쳣��%s", ex.ice_name().c_str());
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
	initData.properties->setProperty("Ice.MessageSizeMax", "1024");//Ĭ����1024����λKB
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
	theApp.TraceRunLog("��ʼԶ�̵��÷����");
	if (!InitProxy())
	{
		theApp.TraceRunLog("---------------�ͻ���Զ�̵��÷����ʧ�ܣ�-------------------------");
		return strResult;
	}

	CDataRpcInvokeServer DataInvoke;
	DataInvoke.m_strParam = strJson.c_str();
	DataInvoke.m_strPort = m_strLocalPort;
	if (!m_strLocalIp.IsEmpty())
		DataInvoke.m_strIp = m_strLocalIp; // ����ʹ����������������ӵı���IP��ַ
	else
		DataInvoke.m_strIp    = GetLocalIP();

	CString strJsonStr = DataInvoke.GetJsonStr();

	CString strMsg;
	strMsg.Format("���÷���˷�������[%s],���ò���[%s],��������ַ��%s,�������˿ںţ�%s",strMethod.c_str(),strJsonStr,m_strServerIp,m_strServerPort);
	theApp.TraceRunLog(strMsg);

	try
	{
		strResult = m_pPrx->InvokeMethod(strMethod, (LPCTSTR)strJsonStr);	 	
	}
	catch(const Ice::Exception& ex)
	{
		CString strErrMsg;
		strErrMsg.Format("Զ�̵��÷����ʧ��,ICE�쳣��%s", ex.ice_name().c_str());
		theApp.TraceRunLog(strErrMsg);
		theApp.TraceRunLog("---------------�ͻ���Զ�̵��÷����ʧ�ܣ�-------------------------");
		return strResult;
	}

	if (!strResult.empty())
	{
		theApp.TraceRunLog("---------------�ͻ���Զ�̵��÷���˳ɹ���-------------------------");
		strMsg.Format("�������˷�����Ϣ:%s",strResult.c_str());
		if (strResult == "Success")
			theApp.TraceRunLog("---------------�����Զ�̵��ÿͻ��˳ɹ���-------------------------");
		else
		{
			strResult = UTF8_TO_MB(strResult);
			theApp.TraceRunLog(strResult.c_str());
			theApp.TraceRunLog("---------------�����Զ�̵��ÿͻ���ʧ�ܣ�-------------------------");
		}
	}
	else
		theApp.TraceRunLog("---------------�ͻ���Զ�̵��÷����ʧ�ܣ�-------------------------");

	return strResult;
}
