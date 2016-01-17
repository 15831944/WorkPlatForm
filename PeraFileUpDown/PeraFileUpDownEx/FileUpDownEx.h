// FileUpDownEx.h : CFileUpDownEx ������

#pragma once
#include "resource.h"       // ������



#include "PeraFileUpDownEx_i.h"
#include "DataDefine.h"
#include <map>



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE ƽ̨(�粻�ṩ��ȫ DCOM ֧�ֵ� Windows Mobile ƽ̨)���޷���ȷ֧�ֵ��߳� COM ���󡣶��� _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA ��ǿ�� ATL ֧�ִ������߳� COM ����ʵ�ֲ�����ʹ���䵥�߳� COM ����ʵ�֡�rgs �ļ��е��߳�ģ���ѱ�����Ϊ��Free����ԭ���Ǹ�ģ���Ƿ� DCOM Windows CE ƽ̨֧�ֵ�Ψһ�߳�ģ�͡�"
#endif

using namespace ATL;

extern std::map<std::string, CPfudeTheadData*> g_mapUploads;
// CFileUpDownEx

class ATL_NO_VTABLE CFileUpDownEx :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CFileUpDownEx, &CLSID_FileUpDownEx>,
	public IDispatchImpl<IFileUpDownEx, &IID_IFileUpDownEx, &LIBID_PeraFileUpDownExLib, /*wMajor =*/ 1, /*wMinor =*/ 0>//,
	//public IObjectSafetyImpl<CFileUpDownEx, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA>
{
public:
	CFileUpDownEx();

DECLARE_REGISTRY_RESOURCEID(IDR_FILEUPDOWNEX)


BEGIN_COM_MAP(CFileUpDownEx)
	COM_INTERFACE_ENTRY(IFileUpDownEx)
	COM_INTERFACE_ENTRY(IDispatch)
	//COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();

	void FinalRelease();

public:

	static DWORD WINAPI StartUploadThread(LPVOID lpParameter);
	static bool PrepareUplaodInfo(LPVOID lpParameter);
	static bool UploadAllFiles(LPVOID lpParameter);
	static bool ClearTempDir(LPVOID lpParameter);

	//void ClearMap();
	

	STDMETHOD(StartUpload)(BSTR bstrJson, VARIANT* pvRet);
	STDMETHOD(QueryUpload)(BSTR bstrJson, VARIANT* pvRet);
	STDMETHOD(CancelUpload)(BSTR bstrJson, VARIANT* pvRet);
	STDMETHOD(parseXpdl)(BSTR bstrJsonInfo, BSTR bstrJsonResult, VARIANT* pvRet);
};

OBJECT_ENTRY_AUTO(__uuidof(FileUpDownEx), CFileUpDownEx)
