
// PeraDCNetDiskDaemon.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "PeraDCNetDiskDaemon.h"
#include "PeraDCNetDiskDaemonDlg.h"
#include "IceService.h"
#include "tools.h"
#include "CrashHandler.h"
#include "PeraHttpDownloadMgr.h"
#include "PeraUploadManager.h"
#include "PeraCopyManager.h"
#include "PeraNetDiskSetting.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPeraDCNetDiskDaemonApp

BEGIN_MESSAGE_MAP(CPeraDCNetDiskDaemonApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPeraDCNetDiskDaemonApp ����

CPeraDCNetDiskDaemonApp::CPeraDCNetDiskDaemonApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CPeraDCNetDiskDaemonApp ����

CPeraDCNetDiskDaemonApp theApp;

// CPeraDCNetDiskDaemonApp ��ʼ��

BOOL CPeraDCNetDiskDaemonApp::InitInstance()
{
	HANDLE hMutex=CreateMutex(NULL,TRUE,"PeraDCNetDiskDaemon");
	if(hMutex)
	{
		if(ERROR_ALREADY_EXISTS==GetLastError())
		{
			//AfxMessageBox("�������������У�");
			CloseHandle(hMutex);
			hMutex = NULL;
			return FALSE;
		}
	}

	CWinApp::InitInstance();

	ZTools::InitZToolsLog();
	CCrashHandler ch;
	ch.SetProcessExceptionHandlers();
	ch.SetThreadExceptionHandlers();

	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	thePersistMrg.Init();

	theDownloadMgr.Init();
	theSetting.GetSettingFromConfig();
	theDownloadMgr.GetSysConfig();

	if ( !theDownloadMgr.RecoverDownTask() )
	{
		AfxMessageBox( "����ģ��ָ�����ʧ��" );
	}



	// ����ʱ��������ʷ����
	CPeraUploadManager::Instance()->LoadHistoryUploadTask(); 
	CPeraCopyManager::Instance()->LoadHistoryCopyTask();

	CNTServiceCommandLineInfo cmdInfo;
	CIceService Service;
// 	Service.SetAllowCommandLine(TRUE);
// 	Service.SetAllowNameChange(TRUE);       
// 	Service.SetAllowDescriptionChange(TRUE);
// 	Service.SetAllowDisplayNameChange(TRUE);
	//Service.ParseCommandLine(cmdInfo);
	//Service.ProcessShellCommand(cmdInfo);

	Service.ServiceMain(0, NULL);
	CloseHandle(hMutex);



	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	//CShellManager *pShellManager = new CShellManager;

	//// ��׼��ʼ��
	//// ���δʹ����Щ���ܲ�ϣ����С
	//// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	//// ����Ҫ���ض���ʼ������
	//// �������ڴ洢���õ�ע�����
	//// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	//// �����޸�Ϊ��˾����֯��
	//SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

 //	CPeraDCNetDiskDaemonDlg dlg;
 //	m_pMainWnd = &dlg;
 //	INT_PTR nResponse = dlg.DoModal();
 //	if (nResponse == IDOK)
 //	{
 //		// TODO: �ڴ˷��ô����ʱ��
 //		//  ��ȷ�������رնԻ���Ĵ���
 //	}
 //	else if (nResponse == IDCANCEL)
 //	{
 //		// TODO: �ڴ˷��ô����ʱ��
 //		//  ��ȡ�������رնԻ���Ĵ���
 //	}

	//// ɾ�����洴���� shell ��������
 //	if (pShellManager != NULL)
 //	{
 //		delete pShellManager;
 //	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}
int CPeraDCNetDiskDaemonApp::ExitInstance()
{
	GdiplusShutdown(gdiplusToken);
	theDownloadMgr.Release();
	thePersistMrg.Release();
	return TRUE;
}