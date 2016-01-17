// RunRobotClient.h : CRunRobotClient 的声明

#pragma once
#include "resource.h"       // 主符号



#include "PeraPcIceClient_i.h"
#include "_IRunRobotClientEvents_CP.h"
#include "Tools.h"
//#include "ftlBase.h"
//#include "ftlThreadPool.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free”，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
#endif

using namespace ATL;


// CRunRobotClient

class ATL_NO_VTABLE CRunRobotClient :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CRunRobotClient, &CLSID_RunRobotClient>,
	public IConnectionPointContainerImpl<CRunRobotClient>,
	public CProxy_IRunRobotClientEvents<CRunRobotClient>,
	public IDispatchImpl<IRunRobotClient, &IID_IRunRobotClient, &LIBID_PeraPcIceClientLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IObjectSafetyImpl<CRunRobotClient, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA>
{
public:
	CRunRobotClient();

DECLARE_REGISTRY_RESOURCEID(IDR_RUNROBOTCLIENT)


BEGIN_COM_MAP(CRunRobotClient)
	COM_INTERFACE_ENTRY(IRunRobotClient)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CRunRobotClient)
	CONNECTION_POINT_ENTRY(__uuidof(_IRunRobotClientEvents))
END_CONNECTION_POINT_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();

	void FinalRelease();

public:



	STDMETHOD(RunRobotFinished)(BSTR bstrFlag, BSTR bstrTaskId, BSTR bstrOutxmlPath, BSTR bstrRunType, VARIANT* pvReturn);
	STDMETHOD(RunRobotLog)(BSTR bstrFlag, BSTR bstrLogInfo, VARIANT* pvReturn);
	STDMETHOD(RunRobotRate)(BSTR bstrFlag, BSTR bstrRateInfo, VARIANT* pvReturn);
	STDMETHOD(GetIP)(VARIANT* pvReturn);
	
	static DWORD WINAPI PostComponentOutDataThread(LPVOID lpParameter);
	static BOOL PostComponentOutDataThreadFunc(LPVOID lpParameter);
	static std::string ProcessOutxml(std::string strTaskId, std::string strOutxmlContent);
	//static BOOL GetSvnConfig(std::string& strRootUrl, std::string& strUser, std::string& strPassword);
	static std::string UploadHprose( std::string strFilePath, BOOL bMove = TRUE);
	CString GetLocalIP();

	//FTL::CFThreadPool<void*>* m_pThreadPool;
	static std::string ProcessOutxml2( std::string strTaskId, std::string strOutxmlContent ); //多线程版本
	static std::string UploadHprose2( std::string strJson, BOOL bMove = TRUE ); //多线程版本
	CString GetIpFromShareMemory();

	static DWORD WINAPI PostComponentOutDataThreadLocal(LPVOID lpParameter);
	static BOOL PostComponentOutDataThreadFuncLocal(LPVOID lpParameter);
	static BOOL UploadTranslate(std::string strLocalFile,std::string & strFileID);
};

OBJECT_ENTRY_AUTO(__uuidof(RunRobotClient), CRunRobotClient)
