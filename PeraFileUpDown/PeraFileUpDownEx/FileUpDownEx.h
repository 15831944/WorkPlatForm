// FileUpDownEx.h : CFileUpDownEx 的声明

#pragma once
#include "resource.h"       // 主符号



#include "PeraFileUpDownEx_i.h"
#include "DataDefine.h"
#include <map>



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free”，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
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
