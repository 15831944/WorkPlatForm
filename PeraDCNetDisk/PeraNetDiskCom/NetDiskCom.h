// NetDiskCom.h : CNetDiskCom ������

#pragma once
#include "resource.h"       // ������
#include <string>
using namespace std;

#include "PeraNetDiskCom_i.h"
#include "_INetDiskComEvents_CP.h"

#include "PeraIceNetDisk.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE ƽ̨(�粻�ṩ��ȫ DCOM ֧�ֵ� Windows Mobile ƽ̨)���޷���ȷ֧�ֵ��߳� COM ���󡣶��� _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA ��ǿ�� ATL ֧�ִ������߳� COM ����ʵ�ֲ�����ʹ���䵥�߳� COM ����ʵ�֡�rgs �ļ��е��߳�ģ���ѱ�����Ϊ��Free����ԭ���Ǹ�ģ���Ƿ� DCOM Windows CE ƽ̨֧�ֵ�Ψһ�߳�ģ�͡�"
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
