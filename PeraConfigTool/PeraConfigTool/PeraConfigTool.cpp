// PeraConfigTool.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PeraConfigTool.h"
#include "PeraConfigToolDlg.h"
#include "CxBCGPVisualManager2007.h"
#include "Tools.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPeraConfigToolApp

BEGIN_MESSAGE_MAP(CPeraConfigToolApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPeraConfigToolApp construction

CPeraConfigToolApp::CPeraConfigToolApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CPeraConfigToolApp object

CPeraConfigToolApp theApp;


// CPeraConfigToolApp initialization

BOOL CPeraConfigToolApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();
	ZTools::InitZToolsLog("PeraConfigTool","PeraConfigTool.exe");
	CCxBCGPVisualManager2007::SetStyle (CCxBCGPVisualManager2007::VS2007_Silver);
	CCxBCGPVisualManager2007::SetDefaultManager (RUNTIME_CLASS (CCxBCGPVisualManager2007));

	CWinApp::InitInstance();

	AfxEnableControlContainer();


	globalData.SetDPIAware ();


	CPeraConfigToolDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	BCGCBProCleanUp ();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
