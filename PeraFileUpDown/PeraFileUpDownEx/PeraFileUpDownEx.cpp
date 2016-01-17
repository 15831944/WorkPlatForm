// PeraFileUpDownEx.cpp : WinMain 的实现


#include "stdafx.h"
#include "resource.h"
#include "PeraFileUpDownEx_i.h"
#include "xdlldata.h"
#include "tools.h"

using namespace ATL;

#include "DataDefine.h"
#include "peraicebase.h"

std::map<std::string, CPfudeTheadData*> g_mapUploads;

void ClearMap()
{
	for (std::map<std::string, CPfudeTheadData*>::iterator it = g_mapUploads.begin(); it != g_mapUploads.end(); ++it)
	{
		delete it->second;
	}
	g_mapUploads.clear();
}

class CPeraFileUpDownExModule : public CComModule
{

public :
	/*DECLARE_LIBID(LIBID_DDHPlugInLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_DDHPLUGIN, "{5FE23615-B7BA-4ED8-B278-6A820C6822C1}")*/
	LONG Unlock();
	DWORD dwThreadID;
	HANDLE hEventShutdown;
	void MonitorShutdown();
	bool StartMonitor();
	bool bActivity;
};
CPeraFileUpDownExModule _Module;

const DWORD dwTimeOut = 5000; // time for EXE to be idle before shutting down
const DWORD dwPause = 1000; // time to wait for threads to finish up

// Passed to CreateThread to monitor the shutdown event
static DWORD WINAPI MonitorProc(void* pv)
{
	CPeraFileUpDownExModule* p = (CPeraFileUpDownExModule*)pv;
	p->MonitorShutdown();
	return 0;
}

LONG CPeraFileUpDownExModule::Unlock()
{
	LONG l = CComModule::Unlock();
	if (l == 0)
	{
		bActivity = true;
		SetEvent(hEventShutdown); // tell monitor that we transitioned to zero
	}
	return l;
}

//Monitors the shutdown event
void CPeraFileUpDownExModule::MonitorShutdown()
{
	while (1)
	{
		WaitForSingleObject(hEventShutdown, INFINITE);
		DWORD dwWait=0;
		do
		{
			bActivity = false;
			dwWait = WaitForSingleObject(hEventShutdown, dwTimeOut);
		} while (dwWait == WAIT_OBJECT_0);
		// timed out
		if (!bActivity && m_nLockCnt == 0) // if no activity let's really bail
		{
#if _WIN32_WINNT >= 0x0400 & defined(_ATL_FREE_THREADED)
			CoSuspendClassObjects();
			if (!bActivity && m_nLockCnt == 0)
#endif
				break;
		}
	}
	CloseHandle(hEventShutdown);
	PostThreadMessage(dwThreadID, WM_QUIT, 0, 0);
}

bool CPeraFileUpDownExModule::StartMonitor()
{
	hEventShutdown = CreateEvent(NULL, false, false, NULL);
	if (hEventShutdown == NULL)
		return false;
	DWORD dwThreadID;
	HANDLE h = CreateThread(NULL, 0, MonitorProc, this, 0, &dwThreadID);
	return (h != NULL);
}

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

LPCTSTR FindOneOf(LPCTSTR p1, LPCTSTR p2)
{
	while (p1 != NULL && *p1 != NULL)
	{
		LPCTSTR p = p2;
		while (p != NULL && *p != NULL)
		{
			if (*p1 == *p)
				return CharNext(p1);
			p = CharNext(p);
		}
		p1 = CharNext(p1);
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////注册控件安全与卸载
char szFileNamePath [MAX_PATH];
const GUID CDECL CLSID_SafeItem = 
{0xB6067CCC,0x7BAB,0x496C,{0xA9,0xA8,0x3A,0x37,0xAF,0xF0,0xCA,0xCC}};
// 注册组件种类为安全
HRESULT AddCategorySafty(CATID catid, WCHAR* catDescription)
{
	ICatRegister* pcr = NULL ;
	HRESULT hr = S_OK ;

	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, 
		NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
	if (FAILED(hr))
		return hr;

	CATEGORYINFO catinfo;
	catinfo.catid = catid;
	catinfo.lcid = 0x0409 ; // 英语语言

	// 最长只拷贝127个字符。
	size_t len = wcslen(catDescription);
	if (len > 127)
	{
		len = 127;
	}
	wcsncpy_s(catinfo.szDescription, catDescription, len); 
	// Make sure the description is null terminated. 
	catinfo.szDescription[len] = '\0'; 

	//lstrcpyn((TCHAR*)(catinfo.szDescription), catDescription, len+1);

	hr = pcr->RegisterCategories(1, &catinfo);
	pcr->Release();

	return hr;
}

//移除已经注册为安全的组件种类
HRESULT RemoveCategorySafty(CATID catid)
{
	ICatRegister* pcr = NULL ;
	HRESULT hr = S_OK ;

	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, 
		NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
	if (FAILED(hr))
		return hr;

	hr = pcr->UnRegisterCategories(1, &catid);
	pcr->Release();

	return hr;
}

// 把你的控件注册到已经注册为安全的组件种类
HRESULT RegisterCLSIDInCategory(REFCLSID clsid, CATID catid)
{
	ICatRegister* pcr = NULL ;
	HRESULT hr = S_OK ;
	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, 
		NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
	if (SUCCEEDED(hr))
	{
		CATID rgcatid[1] ;
		rgcatid[0] = catid;
		hr = pcr->RegisterClassImplCategories(CLSID_SafeItem, 1, rgcatid);
	}
	if (pcr != NULL)
		pcr->Release();
	return hr;
}
// 把你的控件从安全组件种类移除
HRESULT UnRegisterCLSIDInCategory(REFCLSID clsid, CATID catid)
{
	ICatRegister* pcr = NULL ;
	HRESULT hr = S_OK ;

	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, 
		NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
	if (SUCCEEDED(hr))
	{
		// Unregister this category as being "implemented" by the class.
		CATID rgcatid[1] ;
		rgcatid[0] = catid;
		hr = pcr->UnRegisterClassImplCategories(CLSID_SafeItem, 1, rgcatid);
	}

	if (pcr != NULL)
		pcr->Release();

	return hr;
}

//使你的控件不弹出警告地执行
HRESULT MakeActiveXSafty(REFCLSID clsid)
{
	HRESULT hr;

	hr = AddCategorySafty(CATID_SafeForInitializing, 
		L"PeraFileUpDownEx.FileUpDownEx Controls safely initializable!");
	if (FAILED(hr))
		return hr;
	hr = RegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForInitializing);
	if (FAILED(hr))
		return hr;

	hr = AddCategorySafty(CATID_SafeForScripting, L"PeraFileUpDownEx.FileUpDownEx Controls safely scriptable!");
	if (FAILED(hr))
		return hr;
	hr = RegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForScripting);

	return hr;
}

//去除控件的安全执行性
HRESULT UnMakeActiveXSafty(REFCLSID clsid)
{
	HRESULT hr;
	hr = UnRegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForInitializing);
	if (FAILED(hr))
		return hr;
	hr = UnRegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForScripting);
	if (FAILED(hr))
		return hr;

	//下面的代码是把安全组件种类去掉。去掉的话，如果有其他的控件注册为这两个种类
	//那么其他的控件执行时就会弹出警告。需不需要下面的代码就见仁见智，看实际情况了
	//hr = RemoveCategorySafty(CATID_SafeForInitializing);
	//if (FAILED(hr))
	//	return hr;
	//hr = RemoveCategorySafty(CATID_SafeForScripting);

	return hr;
}
//////////////////////////////////////////////////////////////////////////////
//CPeraFileUpDownExModule _AtlModule;

class CPeraFileUpDownExApp : public CWinApp
{
public:

	// Overrides
	virtual BOOL InitInstance();
	virtual int ExitInstance();
protected:   
	BOOL   m_bRun ; 
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CPeraFileUpDownExApp, CWinApp)
END_MESSAGE_MAP()

CPeraFileUpDownExApp theApp;

BOOL CPeraFileUpDownExApp::InitInstance()
{
	ZTools::InitZToolsLog();
	AFX_MANAGE_STATE(AfxGetAppModuleState()); 
	if (!AfxOleInit())
	{
		AfxMessageBox("OLE initialization failed");
		return FALSE;
	}


#if _WIN32_WINNT >= 0x0400 & defined(_ATL_FREE_THREADED)
	HRESULT hRes = CoInitializeEx(NULL, COINIT_MULTITHREADED);
#else
	HRESULT hRes = CoInitialize(NULL);
#endif
	_ASSERTE(SUCCEEDED(hRes));

	_Module.Init(ObjectMap, m_hInstance);
	_Module.dwThreadID = GetCurrentThreadId();

	// Check command line arguments.
	TCHAR szTokens[] = _T("-/");
	m_bRun = TRUE;
	int nRet = 0;
	LPCTSTR lpszToken = FindOneOf(m_lpCmdLine, szTokens);
	while (lpszToken != NULL)
	{
		// Register ATL and MFC class factories.
		if (lstrcmpi(lpszToken, _T("Embedding"))==0 ||
			lstrcmpi(lpszToken, _T("Automation"))==0)
		{
			AfxOleSetUserCtrl(FALSE);
			break;
		}
		// Unregister servers.
		// There is no unregistration code for MFC
		// servers. Refer to <LINK TYPE="ARTICLE" VALUE="Q186212">Q186212</LINK> "HOWTO: Unregister MFC
		// Automation Servers" for adding unregistration
		// code.
		else if (lstrcmpi(lpszToken, _T("UnRegServer"))==0)
		{
			_Module.UpdateRegistryFromResource(IDR_PERAFILEUPDOWNEX, FALSE);
			nRet = _Module.UnregisterServer(TRUE);
			m_bRun = FALSE;
			//AfxMessageBox(_T("组件反注册成功"));
			/////////////////////////////////////////////////
			UnMakeActiveXSafty(CLSID_SafeItem);
			/////////////////////////////////////////////////
			break;
		}
		// Register ATL and MFC objects in the registry.
		else if (lstrcmpi(lpszToken, _T("RegServer"))==0)
		{
			_Module.UpdateRegistryFromResource(IDR_PERAFILEUPDOWNEX, TRUE);
			nRet = _Module.RegisterServer(TRUE);
			COleObjectFactory::UpdateRegistryAll();
			m_bRun = FALSE;
			//AfxMessageBox(_T("组件注册成功"));
			/////////////////////////////////////////////
			MakeActiveXSafty(CLSID_SafeItem);
			////////////////////////////////////////////
			break;
		}
		else
		{
			AfxMessageBox(_T("组件注册命令错误，请使用[/RegServer][/UnRegServer]"));
			break;
		}

		lpszToken = FindOneOf(lpszToken, szTokens);
	}
	if (m_bRun)
	{
		_Module.StartMonitor();
		//VERIFY(SUCCEEDED(_Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE)));
		//VERIFY(COleObjectFactory::RegisterAll());
#if _WIN32_WINNT >= 0x0400 & defined(_ATL_FREE_THREADED)
		hRes = _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER, 
			REGCLS_MULTIPLEUSE | REGCLS_SUSPENDED);
		_ASSERTE(SUCCEEDED(hRes));
		hRes = CoResumeClassObjects();
#else
		hRes = _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER, 
			REGCLS_MULTIPLEUSE);
#endif
		_ASSERTE(SUCCEEDED(hRes));
	}

	ClearMap();

	return TRUE;

}

int CPeraFileUpDownExApp::ExitInstance()
{
	AFX_MANAGE_STATE(AfxGetAppModuleState()); 
	if (m_bRun)
	{
		_Module.RevokeClassObjects();
		Sleep(dwPause); //wait for any threads to finish
	}

	ClearMap();
	ClearPeraIce();

	_Module.Term();
	return 0;
}


// class CPeraFileUpDownExModule : public ATL::CAtlExeModuleT< CPeraFileUpDownExModule >
// 	{
// public :
// 	DECLARE_LIBID(LIBID_PeraFileUpDownExLib)
// 	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_PERAFILEUPDOWNEX, "{8361B8E6-C612-4056-9B56-9BEC002E0D05}")
// 	};
// 
// CPeraFileUpDownExModule _AtlModule;



//
// extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
// 								LPTSTR /*lpCmdLine*/, int nShowCmd)
// {
// 	return _AtlModule.WinMain(nShowCmd);
// }

