// FileManage.h : CFileManage 的声明

#pragma once
#include "resource.h"       // 主符号



#include "PeraFileManage_i.h"
#include "_IFileManageEvents_CP.h"

#include <string>



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free”，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
#endif

using namespace ATL;


// CFileManage

class ATL_NO_VTABLE CFileManage :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CFileManage, &CLSID_FileManage>,
	public IConnectionPointContainerImpl<CFileManage>,
	public CProxy_IFileManageEvents<CFileManage>,
	public IDispatchImpl<IFileManage, &IID_IFileManage, &LIBID_PeraFileManageLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IObjectSafetyImpl<CFileManage, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA>
{
public:
	CFileManage();

DECLARE_REGISTRY_RESOURCEID(IDR_FILEMANAGE)


BEGIN_COM_MAP(CFileManage)
	COM_INTERFACE_ENTRY(IFileManage)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CFileManage)
	CONNECTION_POINT_ENTRY(__uuidof(_IFileManageEvents))
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


	//这两个方法默认读写时进行utf8的转换，用于读写xpdl
	STDMETHOD(SaveFile)(BSTR bstrFile, BSTR bstrContent, VARIANT* pvRet);
	STDMETHOD(OpenFile)(BSTR bstrFile, VARIANT* pvRet);
	//这两个方法默认读写时不进行编码转换
	STDMETHOD(SaveFileDefault)(BSTR bstrFile, BSTR bstrContent, VARIANT* pvRet);
	STDMETHOD(OpenFileDefault)(BSTR bstrFile, VARIANT* pvRet);

	STDMETHOD(CopyFile)(BSTR bstrSrcFile, BSTR bstrDstFile, VARIANT* pvRet);
	STDMETHOD(CopyDir)(BSTR bstrSrcDir, BSTR bstrDstDir, VARIANT* pvRet);
	STDMETHOD(CreateDir)(BSTR bstrDir, VARIANT* pvRet);
	STDMETHOD(DeleteFile)(BSTR bstrFile, VARIANT* pvRet);
	STDMETHOD(DeleteDir)(BSTR bstrDir, VARIANT* pvRet);
	STDMETHOD(SelectFile)(VARIANT* pvRet);
	STDMETHOD(SelectDir)(VARIANT* pvRet);
	STDMETHOD(GetFileList)(BSTR bstrDir, VARIANT* pvRet);
	//取文件名
	std::string GetFileName(std::string strPath);
	//取文件所在目录
	std::string GetFileDir(std::string strPath);
	//比较文件路径
	BOOL IsEqualNoCase(std::string strSrc, std::string strDst);
	//将lasterror值序列化为字符串
	std::string GetLastErrorMsg();
	//较验文件路径是否合法
	BOOL CheckFilePath(std::string strPath);
	//较验目录路径是否合法
	BOOL CheckDirPath(std::string strPath);
	STDMETHOD(ClearDir)(BSTR bstrDir, VARIANT* pvRet);
	STDMETHOD(RenameFile)(BSTR bstrOldName, BSTR bstrNewName, VARIANT* pvRet);
	STDMETHOD(MoveFile)(BSTR srcFilePath, BSTR dstFilePath, VARIANT* pvRet);
	STDMETHOD(MoveDir)(BSTR srcDir, BSTR dstDir, VARIANT* pvRet);
	STDMETHOD(CompareFile)(BSTR srcFilePath, BSTR dstFilePath, VARIANT* pvRet);
	STDMETHOD(GetContentFromZip)(BSTR bstrZip, BSTR bstrFile, VARIANT* pvRet);
	STDMETHOD(UnzipFile)(BSTR bstrZip, BSTR bstrFileSrc, BSTR bstrFileDst, VARIANT* pvRet);
	STDMETHOD(Unzip)(BSTR bstrZip, BSTR bstrDir, VARIANT* pvRet);
	STDMETHOD(UnzipDir)(BSTR bstrZip, BSTR bstrDir, BSTR bstrDirDst, VARIANT* pvRet);
	STDMETHOD(ZipFromDir)(BSTR bstrZip, BSTR bstrDir, VARIANT* pvRet);
	STDMETHOD(ZipFromJsonString)(BSTR bstrZip, BSTR bstrJson, VARIANT* pvRet);
	STDMETHOD(GetUtf8ContentFromZip)(BSTR bstrZip, BSTR bstrFile, VARIANT* pvRet);
	STDMETHOD(ZipFromDirDeflate)(BSTR bstrZip, BSTR bstrDir, VARIANT* pvRet);
	STDMETHOD(ZipFromJsonStringDeflate)(BSTR bstrZip, BSTR bstrJson, VARIANT* pvRet);
};

OBJECT_ENTRY_AUTO(__uuidof(FileManage), CFileManage)
