// RpcCallHelper.h : CRpcCallHelper ������

#pragma once
#include "resource.h"       // ������

#include "ProcessClientServices_i.h"
#include "_IRpcCallHelperEvents_CP.h"

#include "PeraIceLocalWs.h"
#include "PeraIceWs.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE ƽ̨(�粻�ṩ��ȫ DCOM ֧�ֵ� Windows Mobile ƽ̨)���޷���ȷ֧�ֵ��߳� COM ���󡣶��� _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA ��ǿ�� ATL ֧�ִ������߳� COM ����ʵ�ֲ�����ʹ���䵥�߳� COM ����ʵ�֡�rgs �ļ��е��߳�ģ���ѱ�����Ϊ��Free����ԭ���Ǹ�ģ���Ƿ� DCOM Windows CE ƽ̨֧�ֵ�Ψһ�߳�ģ�͡�"
#endif

using namespace ATL;


// CRpcCallHelper

class ATL_NO_VTABLE CRpcCallHelper :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CRpcCallHelper, &CLSID_RpcCallHelper>,
	public IConnectionPointContainerImpl<CRpcCallHelper>,
	public CProxy_IRpcCallHelperEvents<CRpcCallHelper>,
	public IDispatchImpl<IRpcCallHelper, &IID_IRpcCallHelper, &LIBID_ProcessClientServicesLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IObjectSafetyImpl<CRpcCallHelper, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA>
{
public:
	CRpcCallHelper();

DECLARE_REGISTRY_RESOURCEID(IDR_RPCCALLHELPER)


BEGIN_COM_MAP(CRpcCallHelper)
	COM_INTERFACE_ENTRY(IRpcCallHelper)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CRpcCallHelper)
	CONNECTION_POINT_ENTRY(__uuidof(_IRpcCallHelperEvents))
END_CONNECTION_POINT_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease();

private:
	CPeraIceLocalWs m_peraIceLocalWs;
	CPeraIceWs m_peraIceWs;
	Ice::CommunicatorPtr m_pCommunicator;

public:

	STDMETHOD(InvokeWsMethod)(BSTR bstrMethod, BSTR bstrJsonParams, VARIANT* pvRet);
	STDMETHOD(InvokeWsMethodOrignal)(BSTR bstrMethod, BSTR bstrJsonParams, VARIANT* pvRet);
	STDMETHOD(InvokeLocalWsMethod)(BSTR bstrMethod, BSTR bstrJsonParams, VARIANT* pvRet);
	STDMETHOD(InvokeLocalWsMethodOrignal)(BSTR bstrMethod, BSTR bstrJsonParams, VARIANT* pvRet);
	STDMETHOD(ERMDoLogin)(BSTR bstrUser, BSTR bstrPassword, BSTR bstrIp, BSTR bstrForce, VARIANT* pvRet);
	STDMETHOD(ERMLogout)(BSTR bstrTicket, VARIANT* pvRet);
	STDMETHOD(ClientLogin)(BSTR bstrUser, BSTR bstrPassword, VARIANT* pvRet);
	STDMETHOD(ERMKeepTicket)(BSTR bstrJsonIn, VARIANT* pvRet);
	CString GetWebServiceUrl(CString sServiceName);
	BOOL Logout(CString& sResult, CString sTicket);
	BOOL Login(CString& sResult, CString sUser, CString sPassword, CString sIp, CString sForce);
	CString GetLocalIP();
	BOOL GetUserByUserName(CString& sResult, CString sUser);
	BOOL KeepTicket(CString& sResult, CString sJsonIn);


};

OBJECT_ENTRY_AUTO(__uuidof(RpcCallHelper), CRpcCallHelper)
