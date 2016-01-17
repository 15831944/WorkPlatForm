// NetDiskCom.h : CNetDiskCom 的声明

#pragma once
#include "resource.h"       // 主符号
#include <string>
using namespace std;

#include "PeraNetDiskCom_i.h"
#include "_INetDiskComEvents_CP.h"

#include "PeraIceNetDisk.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free”，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
#endif

using namespace ATL;


// CNetDiskCom

class ATL_NO_VTABLE CNetDiskCom :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CNetDiskCom, &CLSID_NetDiskCom>,
	public IConnectionPointContainerImpl<CNetDiskCom>,
	public CProxy_INetDiskComEvents<CNetDiskCom>,
	public IObjectWithSiteImpl<CNetDiskCom>,
	public IDispatchImpl<INetDiskCom, &IID_INetDiskCom, &LIBID_PeraNetDiskComLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IObjectSafetyImpl<CNetDiskCom, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA>
{
public:
	CNetDiskCom();

DECLARE_REGISTRY_RESOURCEID(IDR_NETDISKCOM)


BEGIN_COM_MAP(CNetDiskCom)
	COM_INTERFACE_ENTRY(INetDiskCom)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CNetDiskCom)
	CONNECTION_POINT_ENTRY(__uuidof(_INetDiskComEvents))
END_CONNECTION_POINT_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease();
private:
	HRESULT SelectFile(string strInJson, string & strOutJson);
	HRESULT SelectDir (string strJsonIn,string & strJsonOut);
	HRESULT SelectItem(string strInJson, string & strOutJson);

	
	CPeraIceNetDisk m_peraIceNetDisk;
	Ice::CommunicatorPtr m_pCommunicator;
public:



	STDMETHOD(InvokeMethod)(BSTR bstrMethodName, BSTR bstrJsonIn, VARIANT* pvRet);
};

OBJECT_ENTRY_AUTO(__uuidof(NetDiskCom), CNetDiskCom)
