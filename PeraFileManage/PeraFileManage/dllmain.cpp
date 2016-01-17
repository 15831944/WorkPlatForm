// dllmain.cpp : DllMain 的实现。

#include "stdafx.h"
#include "resource.h"
#include "PeraFileManage_i.h"
#include "dllmain.h"
#include "xdlldata.h"
#include "Tools.h"

CPeraFileManageModule _AtlModule;

class CPeraFileManageApp : public CWinApp
{
public:

// 重写
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CPeraFileManageApp, CWinApp)
END_MESSAGE_MAP()

CPeraFileManageApp theApp;

BOOL CPeraFileManageApp::InitInstance()
{
#ifdef _MERGE_PROXYSTUB
	if (!PrxDllMain(m_hInstance, DLL_PROCESS_ATTACH, NULL))
		return FALSE;
#endif
	return CWinApp::InitInstance();
}

int CPeraFileManageApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
