// dllmain.cpp : DllMain 的实现。

#include "stdafx.h"
#include "resource.h"
#include "PeraNetDiskCom_i.h"
#include "dllmain.h"
#include "xdlldata.h"

CPeraNetDiskComModule _AtlModule;

class CPeraNetDiskComApp : public CWinApp
{
public:

// 重写
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CPeraNetDiskComApp, CWinApp)
END_MESSAGE_MAP()

CPeraNetDiskComApp theApp;

BOOL CPeraNetDiskComApp::InitInstance()
{
#ifdef _MERGE_PROXYSTUB
	if (!PrxDllMain(m_hInstance, DLL_PROCESS_ATTACH, NULL))
		return FALSE;
#endif
	return CWinApp::InitInstance();
}

int CPeraNetDiskComApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
