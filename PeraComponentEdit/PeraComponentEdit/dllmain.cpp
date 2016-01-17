// dllmain.cpp : DllMain 的实现。

#include "stdafx.h"
#include "resource.h"
#include "PeraComponentEdit_i.h"
#include "dllmain.h"
#include "xdlldata.h"
#include "Tools.h"

CPeraComponentEditModule _AtlModule;

class CPeraComponentEditApp : public CWinApp
{
public:

// 重写
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CPeraComponentEditApp, CWinApp)
END_MESSAGE_MAP()

CPeraComponentEditApp theApp;

BOOL CPeraComponentEditApp::InitInstance()
{
#ifdef _MERGE_PROXYSTUB
	if (!PrxDllMain(m_hInstance, DLL_PROCESS_ATTACH, NULL))
		return FALSE;
#endif
	return CWinApp::InitInstance();
}

int CPeraComponentEditApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
