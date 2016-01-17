// FileUpDown.h : CFileUpDown 的声明

#pragma once
#include "resource.h"       // 主符号



#include "PeraFileUpDown_i.h"
#include "_IFileUpDownEvents_CP.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free”，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
#endif

using namespace ATL;


// CFileUpDown

// MFC临界区类对象
CCriticalSection g_cCriticalSection;


class ATL_NO_VTABLE CFileUpDown :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CFileUpDown, &CLSID_FileUpDown>,
	public IConnectionPointContainerImpl<CFileUpDown>,
	public CProxy_IFileUpDownEvents<CFileUpDown>,
	public IDispatchImpl<IFileUpDown, &IID_IFileUpDown, &LIBID_PeraFileUpDown, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IObjectSafetyImpl<CFileUpDown, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA>
{
public:
	CFileUpDown();

DECLARE_REGISTRY_RESOURCEID(IDR_FILEUPDOWN)


BEGIN_COM_MAP(CFileUpDown)
	COM_INTERFACE_ENTRY(IFileUpDown)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CFileUpDown)
	CONNECTION_POINT_ENTRY(__uuidof(_IFileUpDownEvents))
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

	//取上传地址
	std::string GetHproseRpc();
	//取下载地址
	std::string GetHproseDownloadUrl();

	//上传文件
	//bstrHprose,dms提供的服务的地址
	//bstrFilePath,要上传的文件路径
	//返回值为json对象，有属性error,msg,file
	STDMETHOD(Upload)(BSTR bstrHprose, BSTR bstrFilePath, VARIANT* pvRet);
	//下载文件
	//bstrHprose,要下载的文件的完整URL
	//bstrDir,要下载到的目录
	//bstrFileName,指定的文件名，如为空，则从URL的头信息中取文件名，如还没有，则下载失败
	//返回值为字符串，空表示失败，成功的话，返回值为下载后的文件完整路径
	STDMETHOD(Download)(BSTR bstrHprose, BSTR bstrDir, BSTR bstrFileName, VARIANT* pvRet);
	//上传文件，并备份到本地文件缓存中
	//bstrHprose,dms提供的服务的地址
	//bstrFilePath,要上传的文件路径
	//bstrBoolMove,指定备份时移动还是复制，移动的话效率会高一些，参数值应为true或false，默认为false，执行复制
	//返回值为fileid，如为空，表示失败
	STDMETHOD(Upload2)(BSTR bstrHprose, BSTR bstrFilePath, BSTR bstrBoolMove, VARIANT* pvRet);
	//上传文件，并备份到本地文件缓存中
	//bstrHprose,dms提供的服务的地址
	//bstrFilePath,要上传的文件路径
	//bstrFileMd5,要上传的文件的md5值
	//bstrBoolMove,指定备份时移动还是复制，移动的话效率会高一些，参数值应为true或false，默认为false，执行复制
	//返回值为fileid，如为空，表示失败
	STDMETHOD(Upload3)(BSTR bstrHprose, BSTR bstrFilePath, BSTR bstrFileMd5, BSTR bstrBoolMove, VARIANT* pvRet);
	//下载文件，先从本地文件缓存中找
	//bstrHproseBaseUrl,要下载的文件的基础URL，不带fileid
	//bstrFileId,要下载的文件的fileid
	//bstrFileName,指定的文件名，如从文件缓存中有，则该参数不起作用，只有真正下载时用到，如为空，则从URL的头信息中取文件名，如还没有，则下载失败
	//返回值为字符串，空表示失败，成功的话，返回值为下载后的文件完整路径，该路径是在文件缓存中的文件路径，调用者需根据需要来确定是否复制
	STDMETHOD(Download2)(BSTR bstrHproseBaseUrl, BSTR bstrFileId, BSTR bstrFileName, VARIANT* pvRet);

	STDMETHOD(UploadFiles)(BSTR bstrHprose, BSTR bstrJson, BSTR bstrBoolMove, VARIANT* pvRet);

	STDMETHOD(DownloadFiles)(BSTR bstrHproseBaseUrl, BSTR bstrJson, VARIANT* pvRet);

	std::string LoadFileID(std::string strUrl, std::string strFilePath);
	BOOL SaveFileID(std::string strUrl, std::string strFilePath, std::string strFileId);

	Json::Value m_Jv; //输出
};

OBJECT_ENTRY_AUTO(__uuidof(FileUpDown), CFileUpDown)
