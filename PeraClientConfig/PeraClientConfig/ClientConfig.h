// ClientConfig.h : CClientConfig 的声明

#pragma once
#include "resource.h"       // 主符号



#include "PeraClientConfig_i.h"
#include "_IClientConfigEvents_CP.h"

#include <string>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free”，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
#endif

using namespace ATL;


// CClientConfig

class ATL_NO_VTABLE CClientConfig :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CClientConfig, &CLSID_ClientConfig>,
	public IConnectionPointContainerImpl<CClientConfig>,
	public CProxy_IClientConfigEvents<CClientConfig>,
	public IDispatchImpl<IClientConfig, &IID_IClientConfig, &LIBID_PeraClientConfigLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IObjectSafetyImpl<CClientConfig, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA>
{
public:
	CClientConfig();

DECLARE_REGISTRY_RESOURCEID(IDR_CLIENTCONFIG)


BEGIN_COM_MAP(CClientConfig)
	COM_INTERFACE_ENTRY(IClientConfig)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CClientConfig)
	CONNECTION_POINT_ENTRY(__uuidof(_IClientConfigEvents))
END_CONNECTION_POINT_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:


	
	STDMETHOD(Set)(BSTR bstrSection, BSTR bstrKey, BSTR bstrValue, VARIANT* pvRet);
	STDMETHOD(Get)(BSTR bstrSection, BSTR bstrKey, VARIANT* pvRet);

public:
	std::string m_strFilePath;
};

OBJECT_ENTRY_AUTO(__uuidof(ClientConfig), CClientConfig)
