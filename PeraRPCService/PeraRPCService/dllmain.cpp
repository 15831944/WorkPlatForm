// dllmain.cpp : DllMain ��ʵ�֡�

#include "stdafx.h"
#include "resource.h"
#include "ProcessClientServices_i.h"
#include "dllmain.h"
#include "xdlldata.h"
#include "tools.h"

CProcessClientServicesModule _AtlModule;

class CProcessClientServicesApp : public CWinApp
{
public:

// ��д
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CProcessClientServicesApp, CWinApp)
END_MESSAGE_MAP()

CProcessClientServicesApp theApp;

BOOL CProcessClientServicesApp::InitInstance()
{
#ifdef _MERGE_PROXYSTUB
	if (!PrxDllMain(m_hInstance, DLL_PROCESS_ATTACH, NULL))
		return FALSE;
#endif
	return CWinApp::InitInstance();
}

int CProcessClientServicesApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
