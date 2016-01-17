// FileManage.h : CFileManage ������

#pragma once
#include "resource.h"       // ������



#include "PeraFileManage_i.h"
#include "_IFileManageEvents_CP.h"

#include <string>



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE ƽ̨(�粻�ṩ��ȫ DCOM ֧�ֵ� Windows Mobile ƽ̨)���޷���ȷ֧�ֵ��߳� COM ���󡣶��� _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA ��ǿ�� ATL ֧�ִ������߳� COM ����ʵ�ֲ�����ʹ���䵥�߳� COM ����ʵ�֡�rgs �ļ��е��߳�ģ���ѱ�����Ϊ��Free����ԭ���Ǹ�ģ���Ƿ� DCOM Windows CE ƽ̨֧�ֵ�Ψһ�߳�ģ�͡�"
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


	//����������Ĭ�϶�дʱ����utf8��ת�������ڶ�дxpdl
	STDMETHOD(SaveFile)(BSTR bstrFile, BSTR bstrContent, VARIANT* pvRet);
	STDMETHOD(OpenFile)(BSTR bstrFile, VARIANT* pvRet);
	//����������Ĭ�϶�дʱ�����б���ת��
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
	//ȡ�ļ���
	std::string GetFileName(std::string strPath);
	//ȡ�ļ�����Ŀ¼
	std::string GetFileDir(std::string strPath);
	//�Ƚ��ļ�·��
	BOOL IsEqualNoCase(std::string strSrc, std::string strDst);
	//��lasterrorֵ���л�Ϊ�ַ���
	std::string GetLastErrorMsg();
	//�����ļ�·���Ƿ�Ϸ�
	BOOL CheckFilePath(std::string strPath);
	//����Ŀ¼·���Ƿ�Ϸ�
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
