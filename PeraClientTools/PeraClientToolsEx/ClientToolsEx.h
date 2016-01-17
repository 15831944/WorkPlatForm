// ClientToolsEx.h : CClientToolsEx ������

#pragma once
#include "resource.h"       // ������



#include "PeraClientToolsEx_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE ƽ̨(�粻�ṩ��ȫ DCOM ֧�ֵ� Windows Mobile ƽ̨)���޷���ȷ֧�ֵ��߳� COM ���󡣶��� _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA ��ǿ�� ATL ֧�ִ������߳� COM ����ʵ�ֲ�����ʹ���䵥�߳� COM ����ʵ�֡�rgs �ļ��е��߳�ģ���ѱ�����Ϊ��Free����ԭ���Ǹ�ģ���Ƿ� DCOM Windows CE ƽ̨֧�ֵ�Ψһ�߳�ģ�͡�"
#endif

using namespace ATL;


// CClientToolsEx

class ATL_NO_VTABLE CClientToolsEx :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CClientToolsEx, &CLSID_ClientToolsEx>,
	public IDispatchImpl<IClientToolsEx, &IID_IClientToolsEx, &LIBID_PeraClientToolsExLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CClientToolsEx()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_CLIENTTOOLSEX)


BEGIN_COM_MAP(CClientToolsEx)
	COM_INTERFACE_ENTRY(IClientToolsEx)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:



	STDMETHOD(CheckCustomProtocol)(BSTR bstrProtocolName, VARIANT* pvRet);
	STDMETHOD(InvokeWorkspace)(void);
	STDMETHOD(InvokePeraComponent)(void);
};

OBJECT_ENTRY_AUTO(__uuidof(ClientToolsEx), CClientToolsEx)
