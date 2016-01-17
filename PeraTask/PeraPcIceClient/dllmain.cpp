// dllmain.cpp : DllMain 的实现。

#include "stdafx.h"
#include "resource.h"
#include "PeraPcIceClient_i.h"
#include "dllmain.h"
#include "xdlldata.h"
#include "tools.h"
#include "CrashHandler.h"

CPeraPcIceClientModule _AtlModule;

class CPeraPcIceClientApp : public CWinApp
{
public:

// 重写
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CPeraPcIceClientApp, CWinApp)
END_MESSAGE_MAP()

CPeraPcIceClientApp theApp;

BOOL CPeraPcIceClientApp::InitInstance()
{
	CCrashHandler ch;
	ch.SetProcessExceptionHandlers();
	ch.SetThreadExceptionHandlers();
#ifdef _MERGE_PROXYSTUB
	if (!PrxDllMain(m_hInstance, DLL_PROCESS_ATTACH, NULL))
		return FALSE;
#endif
	return CWinApp::InitInstance();
}

int CPeraPcIceClientApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
