// ToolCore.h : CToolCore ������

#pragma once
#include "resource.h"       // ������



#include "PeraStartToolCom_i.h"
#include "_IToolCoreEvents_CP.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE ƽ̨(�粻�ṩ��ȫ DCOM ֧�ֵ� Windows Mobile ƽ̨)���޷���ȷ֧�ֵ��߳� COM ���󡣶��� _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA ��ǿ�� ATL ֧�ִ������߳� COM ����ʵ�ֲ�����ʹ���䵥�߳� COM ����ʵ�֡�rgs �ļ��е��߳�ģ���ѱ�����Ϊ��Free����ԭ���Ǹ�ģ���Ƿ� DCOM Windows CE ƽ̨֧�ֵ�Ψһ�߳�ģ�͡�"
#endif

using namespace ATL;


// CToolCore

class ATL_NO_VTABLE CToolCore :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CToolCore, &CLSID_ToolCore>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CToolCore>,
	public CProxy_IToolCoreEvents<CToolCore>,
	public IObjectWithSiteImpl<CToolCore>,
	public IDispatchImpl<IToolCore, &IID_IToolCore, &LIBID_PeraStartToolComLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IObjectSafetyImpl<CToolCore, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA>
{
public:
	CToolCore()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_TOOLCORE)


BEGIN_COM_MAP(CToolCore)
	COM_INTERFACE_ENTRY(IToolCore)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CToolCore)
	CONNECTION_POINT_ENTRY(__uuidof(_IToolCoreEvents))
END_CONNECTION_POINT_MAP()
// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:



	STDMETHOD(InvokeApp)(BSTR strInJson, VARIANT* pVOut);
	STDMETHOD(ManageApp)(BSTR strInJson, VARIANT* pVOut);
	STDMETHOD(ConfigApp)(BSTR strInJson, VARIANT* pVOut);
	STDMETHOD(InvokeAppById)(BSTR strInParam, VARIANT* pVOut);
	STDMETHOD(OpenUrl)(BSTR strUrl, VARIANT* pVout);
};

OBJECT_ENTRY_AUTO(__uuidof(ToolCore), CToolCore)
