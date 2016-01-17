// dllmain.cpp : DllMain ��ʵ�֡�

#include "stdafx.h"
#include "resource.h"
#include "PeraStartToolCom_i.h"
#include "dllmain.h"
#include "ToolsProUnity.h"
#include "Tools.h"
#include "CxBCGPVisualManager2007.h"
CStartToolComModule _AtlModule;

class CStartToolComApp : public CWinApp
{
public:

// ��д
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CStartToolComApp, CWinApp)
END_MESSAGE_MAP()

CStartToolComApp theApp;

BOOL CStartToolComApp::InitInstance()
{
	ZTools::InitZToolsLog("PeraStartToolCom", "PeraStartToolCom.dll");

	CString str = ZTools::GetCurrentModulePath("PeraStartToolCom.dll").c_str();
	CToolsProUnity::g_strConfigXml = str + "\\LocalConfig.xml";
	
	char szBuf[512] = {0};
	GetModuleFileName(NULL,szBuf,512);
	CString strApp = szBuf;

	ZTools::WriteZToolsFormatLog("PeraStartToolCom.dll����exeΪ%s",strApp);
	strApp = strApp.Mid(strApp.ReverseFind('\\')+1);
	strApp.MakeLower();
	if (strApp == "peradeskclient.exe" || strApp == "peradcwebcontainer.exe" ||strApp == "360se.exe")
	{
		ZTools::WriteZToolsFormatLog("�ͻ��������У�����CBCGPVisualManager2007");
		CCxBCGPVisualManager2007::SetStyle (CCxBCGPVisualManager2007::VS2007_Silver);
		CCxBCGPVisualManager2007::SetDefaultManager (RUNTIME_CLASS (CCxBCGPVisualManager2007));
	}
	else if (strApp=="iexplore.exe")
	{
		ZTools::WriteZToolsFormatLog("��IE����������У�������CBCGPVisualManager2007");
	}
		

	return CWinApp::InitInstance();
}

int CStartToolComApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
