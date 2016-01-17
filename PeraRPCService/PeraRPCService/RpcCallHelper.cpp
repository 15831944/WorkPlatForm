// RpcCallHelper.cpp : CRpcCallHelper 的实现

#include "stdafx.h"
#include "RpcCallHelper.h"
#include "Tools.h"
#include <comdef.h>

#include "csharedmemory.h"

#include "ERMClientServiceremoting_x002fERMClientServiceSoapBindingProxy.h"
#include "ERMClientService.nsmap"
#include "ConfigFunc.h"

STDMETHODIMP CRpcCallHelper::InvokeWsMethod(BSTR bstrMethod, BSTR bstrJsonParams, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	std::string strMethodName = _bstr_t(bstrMethod);
	std::string strParamJsonArray = _bstr_t(bstrJsonParams);
	ZTools::WriteZToolsFormatLog(">>>>>>>CClientHelper::InvokeWsMethod");
	ZTools::WriteZToolsLog(strMethodName);
	ZTools::WriteZToolsLog(strParamJsonArray);

	ZTools::MBToUTF8(strMethodName);
	ZTools::MBToUTF8(strParamJsonArray);
	std::string strResult = m_peraIceWs.InvokeWsMethodOrignal(strMethodName, strParamJsonArray);
	ZTools::UTF8ToMB(strResult);

	ZTools::WriteZToolsLog(strResult);
	CComVariant v(_bstr_t(strResult.c_str()).GetBSTR());
	v.Detach(pvRet);
	return S_OK;
}

STDMETHODIMP CRpcCallHelper::InvokeWsMethodOrignal(BSTR bstrMethod, BSTR bstrJsonParams, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	std::string strMethodName = _bstr_t(bstrMethod);
	std::string strParamJsonArray = _bstr_t(bstrJsonParams);
	ZTools::WriteZToolsFormatLog(">>>>>>>CClientHelper::InvokeWsMethod");
	ZTools::WriteZToolsLog(strMethodName);
	ZTools::WriteZToolsLog(strParamJsonArray);

	//ZTools::MBToUTF8(strMethodName);
	//ZTools::MBToUTF8(strParamJsonArray);
	std::string strResult = m_peraIceWs.InvokeWsMethodOrignal(strMethodName, strParamJsonArray);
	//ZTools::UTF8ToMB(strResult);

	ZTools::WriteZToolsLog(strResult);
	CComVariant v(_bstr_t(strResult.c_str()).GetBSTR());
	v.Detach(pvRet);
	return S_OK;
}

STDMETHODIMP CRpcCallHelper::InvokeLocalWsMethod(BSTR bstrMethod, BSTR bstrJsonParams, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// TODO: 在此添加实现代码
	//Ice::CommunicatorPtr communicator;

	std::string strMethodName = _bstr_t(bstrMethod);
	std::string strParamJsonArray = _bstr_t(bstrJsonParams);
	ZTools::WriteZToolsFormatLog(">>>>>>>CClientHelper::InvokeLocalWsMethod");
	ZTools::WriteZToolsLog(strMethodName);
	ZTools::WriteZToolsLog(strParamJsonArray);

	ZTools::MBToUTF8(strMethodName);
	ZTools::MBToUTF8(strParamJsonArray);
	std::string strResult = m_peraIceLocalWs.InvokeLocalWsMethodOrignal(strMethodName, strParamJsonArray);
	ZTools::UTF8ToMB(strResult);

	ZTools::WriteZToolsLog(strResult);
	CComVariant v(_bstr_t(strResult.c_str()).GetBSTR());
	v.Detach(pvRet);
	return S_OK;
}

STDMETHODIMP CRpcCallHelper::InvokeLocalWsMethodOrignal(BSTR bstrMethod, BSTR bstrJsonParams, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加实现代码
	Ice::CommunicatorPtr communicator;

	std::string strMethodName = _bstr_t(bstrMethod);
	std::string strParamJsonArray = _bstr_t(bstrJsonParams);
	ZTools::WriteZToolsFormatLog(">>>>>>>CClientHelper::InvokeLocalWsMethodOrignal");
	ZTools::WriteZToolsLog(strMethodName);
	ZTools::WriteZToolsLog(strParamJsonArray);

	//ZTools::MBToUTF8(strMethodName);
	//ZTools::MBToUTF8(strParamJsonArray);
	std::string strResult = m_peraIceLocalWs.InvokeLocalWsMethodOrignal(strMethodName, strParamJsonArray);
	//ZTools::UTF8ToMB(strResult);

	ZTools::WriteZToolsLog(strResult);
	CComVariant v(_bstr_t(strResult.c_str()).GetBSTR());
	v.Detach(pvRet);
	return S_OK;
}


STDMETHODIMP CRpcCallHelper::ERMDoLogin(BSTR bstrUser, BSTR bstrPassword, BSTR bstrIp, BSTR bstrForce, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加实现代码
	CString sUser = _bstr_t(bstrUser);
	CString sPassword = _bstr_t(bstrPassword);
	CString sIp = _bstr_t(bstrIp);
	CString sForce = _bstr_t(bstrForce);
	CString sResult;
	Login(sResult, sUser, sPassword, sIp, sForce);

	CComVariant v(_bstr_t(sResult).GetBSTR());
	v.Detach(pvRet);
	return S_OK;
}


STDMETHODIMP CRpcCallHelper::ERMLogout(BSTR bstrTicket, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加实现代码
	CString sTicket = _bstr_t(bstrTicket);
	CString sResult;
	Logout(sResult, sTicket);

	CComVariant v(_bstr_t(sResult).GetBSTR());
	v.Detach(pvRet);
	return S_OK;
}

//从共享内存中读
#define SM_LOCAL_IP_LENGTH 256
CString GetIpFromShareMemory()
{
	CString sIp("");
	CSharedMemory* pSmIp = NULL;
	pSmIp = new CSharedMemory("SM_LOCAL_IP", SM_LOCAL_IP_LENGTH);
	pSmIp->Lock();
	sIp = (char*)pSmIp->GetData();
	pSmIp->Unlock();
	delete pSmIp;
	pSmIp = NULL;
	return sIp;
}

// 返回json串，有如下属性,error为0时登陆成功，error为1时，失败
// "error"
// "userName"
// "password"
// "ticket"
// "realName"
// "roles"
STDMETHODIMP CRpcCallHelper::ClientLogin(BSTR bstrUser, BSTR bstrPassword, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加实现代码
	CComVariant v(_bstr_t("").GetBSTR());
	CString sUser = _bstr_t(bstrUser);
	CString sPassword = _bstr_t(bstrPassword);
	CString sIp;
	sIp = GetIpFromShareMemory();
	if (sIp.IsEmpty())
	{
		sIp = GetLocalIP();
	}
	CString sForce = "false";
	CString sResult;
	Login(sResult, sUser, sPassword, sIp, sForce);
	//////////////////////////////////////////////////////////////////////////
	//GetUserByUserName(sResult, "admin");	
	
	if (sResult.CompareNoCase("-11") != 0 && sResult.Find("ticket-proxy=") == -1)
	{
		CString sErrInfo;
		if (sResult.Compare("-2") == 0)
		{
			sErrInfo = "系统错误，请确认服务器工作正常";
		}
		else if (sResult.Compare("-6") == 0)
		{
			sErrInfo = "用户不存在";
		}
		else if (sResult.Compare("-7") == 0)
		{
			sErrInfo = "密码错误";
		}
		else if (sResult.Compare("-8") == 0)
		{
			sErrInfo = "用户被锁定";
		}
		else if (sResult.Compare("-9") == 0)
		{
			sErrInfo = "用户被删除";
		}
		else if (sResult.Compare("-10") == 0)
		{
			sErrInfo = "用户被禁用";
		}
		else
		{
			sErrInfo = "登录失败，请确认服务器工作正常";
		}
		CString sJson;
		sJson.Format("{\"error\":\"1\",\"errInfo\":\"%s\"}", sErrInfo);
		v = _bstr_t(sJson).GetBSTR();
		v.Detach(pvRet);
		return S_OK;
	}
	else if (sResult.CompareNoCase("-11") == 0)
	{
		int nCode = MessageBox(AfxGetMainWnd()->GetSafeHwnd(), "该用户已在别处登录，是否继续登录并强制对方下线？", "登录", MB_YESNO|MB_ICONINFORMATION);
		if (nCode == IDYES)
		{
			sForce = "true";
			Login(sResult, sUser, sPassword, sIp, sForce);
		}
	}

	CString sTicket = sResult;
	if (sTicket.Find("ticket-proxy=") == -1)
	{

		CString sErrInfo;
		if (sResult.Compare("-2") == 0)
		{
			sErrInfo = "系统错误，请确认服务器工作正常";
		}
		else if (sResult.Compare("-6") == 0)
		{
			sErrInfo = "用户不存在";
		}
		else if (sResult.Compare("-7") == 0)
		{
			sErrInfo = "密码错误";
		}
		else if (sResult.Compare("-8") == 0)
		{
			sErrInfo = "用户被锁定";
		}
		else if (sResult.Compare("-9") == 0)
		{
			sErrInfo = "用户被删除";
		}
		else if (sResult.Compare("-10") == 0)
		{
			sErrInfo = "用户被禁用";
		}
		else
		{
			sErrInfo = "登录失败，请确认服务器工作正常";
		}
		CString sJson;
		sJson.Format("{\"error\":\"1\",\"errInfo\":\"%s\"}", sErrInfo);
		v = _bstr_t(sJson).GetBSTR();
		v.Detach(pvRet);
		return S_OK;
	}

	CString sRealName(sUser);
	CString sRoles;

	GetUserByUserName(sResult, sUser);	

	Json::Reader reader;
	Json::Value root;
	if (reader.parse((LPCTSTR)sResult, root))
	{
		sRealName = root["realName"].asString().c_str();
		sRoles = root["roles"].asString().c_str();
	}

	Json::Value jsonReturn;
	jsonReturn["error"] = Json::Value("0");
	jsonReturn["userName"] = Json::Value(sUser);
	jsonReturn["password"] = Json::Value(sPassword);
	jsonReturn["ticket"] = Json::Value(sTicket);
	jsonReturn["realName"] = Json::Value(sRealName);
	jsonReturn["roles"] = Json::Value(sRoles);
	
	Json::FastWriter fast_writer;
	CString sJson = fast_writer.write(jsonReturn).c_str();

	v = _bstr_t(sJson).GetBSTR();
	v.Detach(pvRet);
	return S_OK;
}

STDMETHODIMP CRpcCallHelper::ERMKeepTicket(BSTR bstrJsonIn, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ZTools::WriteZToolsFormatLog(_T(">>>>>> call func ERMKeepTicket"));
	// TODO: 在此添加实现代码
	CComVariant v(_bstr_t("").GetBSTR());
	CString sJsonIn = (bstrJsonIn == NULL) ? "" : _bstr_t(bstrJsonIn);
	CString sJsonOut;

	if (KeepTicket(sJsonOut, sJsonIn) == FALSE)
	{
		return S_OK;
	}	

	v = _bstr_t(sJsonOut).GetBSTR();
	v.Detach(pvRet);
	return S_OK;
}

CString CRpcCallHelper::GetWebServiceUrl(CString sServiceName)
{
	CString sUrl = GetConfig("PeraRPCService", sServiceName);
	return sUrl;
}

BOOL CRpcCallHelper::Logout(CString& sResult, CString sTicket)
{
	CString sUrl = GetWebServiceUrl("ErmService");
	if (sUrl.IsEmpty())
	{
		//读取服务器地址不正确
		return FALSE;
	}

	std::string strUrl = (LPCSTR)sUrl;

	std::string strTicket = (LPCSTR)sTicket;

	ERMClientService::remoting_x002fERMClientServiceSoapBindingProxy ermProxy(strUrl.c_str(), SOAP_C_UTFSTRING);
	ermProxy.soap->send_timeout = 5; 
	ermProxy.soap->recv_timeout = 5;
	ermProxy.soap->connect_timeout = 5;
	ermProxy.soap->accept_timeout = 5;

	std::string strOut;	
	ZTools::MBToUTF8(strTicket);
	if (SOAP_OK != ermProxy.logout(strTicket, strOut))
	{
		ermProxy.destroy();
		return FALSE;
	}
	ZTools::UTF8ToMB(strOut);
	sResult = strOut.c_str();
	ermProxy.destroy();

	return TRUE;
}

BOOL CRpcCallHelper::Login(CString& sResult, CString sUser, CString sPassword, CString sIp, CString sForce)
{
	CString sUrl = GetWebServiceUrl("ErmService");
	if (sUrl.IsEmpty())
	{
		//读取服务器地址不正确
		return FALSE;
	}

	std::string strUrl = (LPCSTR)sUrl;

	std::string strJson;
	Json::Value root;
	Json::FastWriter fast_writer;

	root["userName"] = Json::Value(sUser);
	root["password"] = Json::Value(sPassword);
	root["ip"] = Json::Value(sIp);
	root["force"] = Json::Value(sForce);
	strJson = fast_writer.write(root);

	ERMClientService::remoting_x002fERMClientServiceSoapBindingProxy ermProxy(strUrl.c_str(), SOAP_C_UTFSTRING);
	ermProxy.soap->send_timeout = 5; 
	ermProxy.soap->recv_timeout = 5;
	ermProxy.soap->connect_timeout = 5;
	ermProxy.soap->accept_timeout = 5;

	std::string strOut;	
	ZTools::MBToUTF8(strJson);
	if (SOAP_OK != ermProxy.dologin(strJson, strOut))
	{
		ermProxy.destroy();
		return FALSE;
	}
	ZTools::UTF8ToMB(strOut);
	sResult = strOut.c_str();
	ermProxy.destroy();

	return TRUE;
}

BOOL CRpcCallHelper::GetUserByUserName(CString& sResult, CString sUser)
{
	CString sUrl = GetWebServiceUrl("ErmService");
	if (sUrl.IsEmpty())
	{
		//读取服务器地址不正确
		return FALSE;
	}

	std::string strUrl = (LPCSTR)sUrl;

	std::string strUser = (LPCSTR)sUser;

	ERMClientService::remoting_x002fERMClientServiceSoapBindingProxy ermProxy(strUrl.c_str(), SOAP_C_UTFSTRING);
	ermProxy.soap->send_timeout = 5; 
	ermProxy.soap->recv_timeout = 5;
	ermProxy.soap->connect_timeout = 5;
	ermProxy.soap->accept_timeout = 5;

	std::string strOut;
	ZTools::MBToUTF8(strUser);
	if (SOAP_OK != ermProxy.getUserByUserName(strUser, strOut))
	{
		ermProxy.destroy();
		return FALSE;
	}
	ZTools::UTF8ToMB(strOut);
	sResult = strOut.c_str();
	ermProxy.destroy();

	return TRUE;
}

CString CRpcCallHelper::GetLocalIP()
{
	//return "192.168.114.156";
	CString sHostName,sIPAddress;
	WORD    wVersionRequested;
	WSADATA wsaData;
	int     err;
	char szHostName[256];   
	int  nRetCode;   

	wVersionRequested = MAKEWORD( 2, 0 );    
	err = WSAStartup( wVersionRequested, &wsaData );   
	if ( err != 0 ) 
	{    
		return "";   
	}    

	if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 0 ) 
	{   
		WSACleanup( );   
		return "";    
	}

	nRetCode = gethostname(szHostName, sizeof(szHostName));   

	if (nRetCode != 0) 
	{   
		sHostName = (""); 
		return "";   
	}   

	sHostName = szHostName;

	struct hostent FAR *lpHostEnt = gethostbyname (sHostName);   

	if (lpHostEnt == NULL) 
	{   
		sIPAddress = _T("");
		return sIPAddress;
	}   

	LPSTR lpAddr = lpHostEnt->h_addr_list[0];   
	if (lpAddr) 
	{   
		struct in_addr  inAddr;   
		memmove (&inAddr, lpAddr, 4);   
		sIPAddress = inet_ntoa (inAddr);   
		if (sIPAddress.IsEmpty())   
			sIPAddress = _T("");   
	}

	nRetCode = WSACleanup();  

	if (nRetCode != 0) 
	{ 
		return "";   
	}   

	return sIPAddress;   
}

BOOL CRpcCallHelper::KeepTicket( CString& sResult, CString sJsonIn )
{
	CString sUrl = GetWebServiceUrl("ErmService");
	if (sUrl.IsEmpty())
	{
		//读取服务器地址不正确
		return FALSE;
	}

	std::string strUrl = (LPCSTR)sUrl;

	std::string strJsonIn = (LPCSTR)sJsonIn;

	ERMClientService::remoting_x002fERMClientServiceSoapBindingProxy ermProxy(strUrl.c_str(), SOAP_C_UTFSTRING);
	ermProxy.soap->send_timeout = 5; 
	ermProxy.soap->recv_timeout = 5;
	ermProxy.soap->connect_timeout = 5;
	ermProxy.soap->accept_timeout = 5;

	std::string strOut;
	ZTools::MBToUTF8(strJsonIn);
	if (SOAP_OK != ermProxy.updateLastAccessedTime(strJsonIn, strOut))
	{
		ermProxy.destroy();
		return FALSE;
	}
	ZTools::UTF8ToMB(strOut);
	sResult = strOut.c_str();
	ermProxy.destroy();
	
	return TRUE;
}

CRpcCallHelper::CRpcCallHelper()
{
	ZTools::InitZToolsLog("PeraRPCService", "PeraRPCService.dll");
	
	Ice::InitializationData initData;
	initData.properties = Ice::createProperties();
	initData.properties->setProperty("Ice.MessageSizeMax", "10240");//默认是1024，单位KB

	m_pCommunicator = Ice::initialize(initData);

	m_peraIceWs.SetCommunicator(m_pCommunicator);
	m_peraIceLocalWs.SetCommunicator(m_pCommunicator);
}

void CRpcCallHelper::FinalRelease()
{
	if (m_pCommunicator)
	{
		m_pCommunicator->destroy();
		m_pCommunicator = NULL;
	}
}
