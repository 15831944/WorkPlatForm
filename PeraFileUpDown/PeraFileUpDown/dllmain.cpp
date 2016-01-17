// dllmain.cpp : DllMain 的实现。

#include "stdafx.h"
#include "resource.h"
#include "PeraFileUpDown_i.h"
#include "dllmain.h"
#include "xdlldata.h"
#include "Tools.h"
#include "CrashHandler.h"

CPeraFileUpDownModule _AtlModule;

class CPeraFileUpDownApp : public CWinApp
{
public:

// 重写
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CPeraFileUpDownApp, CWinApp)
END_MESSAGE_MAP()

CPeraFileUpDownApp theApp;

BOOL CPeraFileUpDownApp::InitInstance()
{
	//CCrashHandler ch;
	//ch.SetProcessExceptionHandlers();
	//ch.SetThreadExceptionHandlers();
#ifdef _MERGE_PROXYSTUB
	if (!PrxDllMain(m_hInstance, DLL_PROCESS_ATTACH, NULL))
		return FALSE;
#endif
	return CWinApp::InitInstance();
}

int CPeraFileUpDownApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
