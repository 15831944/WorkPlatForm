// FileUpDown.h : CFileUpDown ������

#pragma once
#include "resource.h"       // ������



#include "PeraFileUpDown_i.h"
#include "_IFileUpDownEvents_CP.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE ƽ̨(�粻�ṩ��ȫ DCOM ֧�ֵ� Windows Mobile ƽ̨)���޷���ȷ֧�ֵ��߳� COM ���󡣶��� _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA ��ǿ�� ATL ֧�ִ������߳� COM ����ʵ�ֲ�����ʹ���䵥�߳� COM ����ʵ�֡�rgs �ļ��е��߳�ģ���ѱ�����Ϊ��Free����ԭ���Ǹ�ģ���Ƿ� DCOM Windows CE ƽ̨֧�ֵ�Ψһ�߳�ģ�͡�"
#endif

using namespace ATL;


// CFileUpDown

// MFC�ٽ��������
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

	//ȡ�ϴ���ַ
	std::string GetHproseRpc();
	//ȡ���ص�ַ
	std::string GetHproseDownloadUrl();

	//�ϴ��ļ�
	//bstrHprose,dms�ṩ�ķ���ĵ�ַ
	//bstrFilePath,Ҫ�ϴ����ļ�·��
	//����ֵΪjson����������error,msg,file
	STDMETHOD(Upload)(BSTR bstrHprose, BSTR bstrFilePath, VARIANT* pvRet);
	//�����ļ�
	//bstrHprose,Ҫ���ص��ļ�������URL
	//bstrDir,Ҫ���ص���Ŀ¼
	//bstrFileName,ָ�����ļ�������Ϊ�գ����URL��ͷ��Ϣ��ȡ�ļ������绹û�У�������ʧ��
	//����ֵΪ�ַ������ձ�ʾʧ�ܣ��ɹ��Ļ�������ֵΪ���غ���ļ�����·��
	STDMETHOD(Download)(BSTR bstrHprose, BSTR bstrDir, BSTR bstrFileName, VARIANT* pvRet);
	//�ϴ��ļ��������ݵ������ļ�������
	//bstrHprose,dms�ṩ�ķ���ĵ�ַ
	//bstrFilePath,Ҫ�ϴ����ļ�·��
	//bstrBoolMove,ָ������ʱ�ƶ����Ǹ��ƣ��ƶ��Ļ�Ч�ʻ��һЩ������ֵӦΪtrue��false��Ĭ��Ϊfalse��ִ�и���
	//����ֵΪfileid����Ϊ�գ���ʾʧ��
	STDMETHOD(Upload2)(BSTR bstrHprose, BSTR bstrFilePath, BSTR bstrBoolMove, VARIANT* pvRet);
	//�ϴ��ļ��������ݵ������ļ�������
	//bstrHprose,dms�ṩ�ķ���ĵ�ַ
	//bstrFilePath,Ҫ�ϴ����ļ�·��
	//bstrFileMd5,Ҫ�ϴ����ļ���md5ֵ
	//bstrBoolMove,ָ������ʱ�ƶ����Ǹ��ƣ��ƶ��Ļ�Ч�ʻ��һЩ������ֵӦΪtrue��false��Ĭ��Ϊfalse��ִ�и���
	//����ֵΪfileid����Ϊ�գ���ʾʧ��
	STDMETHOD(Upload3)(BSTR bstrHprose, BSTR bstrFilePath, BSTR bstrFileMd5, BSTR bstrBoolMove, VARIANT* pvRet);
	//�����ļ����ȴӱ����ļ���������
	//bstrHproseBaseUrl,Ҫ���ص��ļ��Ļ���URL������fileid
	//bstrFileId,Ҫ���ص��ļ���fileid
	//bstrFileName,ָ�����ļ���������ļ��������У���ò����������ã�ֻ����������ʱ�õ�����Ϊ�գ����URL��ͷ��Ϣ��ȡ�ļ������绹û�У�������ʧ��
	//����ֵΪ�ַ������ձ�ʾʧ�ܣ��ɹ��Ļ�������ֵΪ���غ���ļ�����·������·�������ļ������е��ļ�·�����������������Ҫ��ȷ���Ƿ���
	STDMETHOD(Download2)(BSTR bstrHproseBaseUrl, BSTR bstrFileId, BSTR bstrFileName, VARIANT* pvRet);

	STDMETHOD(UploadFiles)(BSTR bstrHprose, BSTR bstrJson, BSTR bstrBoolMove, VARIANT* pvRet);

	STDMETHOD(DownloadFiles)(BSTR bstrHproseBaseUrl, BSTR bstrJson, VARIANT* pvRet);

	std::string LoadFileID(std::string strUrl, std::string strFilePath);
	BOOL SaveFileID(std::string strUrl, std::string strFilePath, std::string strFileId);

	Json::Value m_Jv; //���
};

OBJECT_ENTRY_AUTO(__uuidof(FileUpDown), CFileUpDown)
