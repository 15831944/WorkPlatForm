
// PeraTaskService.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "PeraTaskService.h"
#include "PeraTaskServiceDlg.h"
#include "IceService.h"
#include "tools.h"
#include "CrashHandler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPeraTaskServiceApp

BEGIN_MESSAGE_MAP(CPeraTaskServiceApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPeraTaskServiceApp ����

CPeraTaskServiceApp::CPeraTaskServiceApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CPeraTaskServiceApp ����

CPeraTaskServiceApp theApp;


// CPeraTaskServiceApp ��ʼ��

BOOL CPeraTaskServiceApp::InitInstance()
{
	CWinApp::InitInstance();

	ZTools::InitZToolsLog();
	CCrashHandler ch;
	ch.SetProcessExceptionHandlers();
	ch.SetThreadExceptionHandlers();

	CNTServiceCommandLineInfo cmdInfo;
	CIceService Service;
// 	Service.SetAllowCommandLine(TRUE);
// 	Service.SetAllowNameChange(TRUE);       
// 	Service.SetAllowDescriptionChange(TRUE);
// 	Service.SetAllowDisplayNameChange(TRUE);
	//Service.ParseCommandLine(cmdInfo);
	//Service.ProcessShellCommand(cmdInfo);
	Service.ServiceMain(0, NULL);



	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	//CShellManager *pShellManager = new CShellManager;

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	//SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

// 	CPeraTaskServiceDlg dlg;
// 	m_pMainWnd = &dlg;
// 	INT_PTR nResponse = dlg.DoModal();
// 	if (nResponse == IDOK)
// 	{
// 		// TODO: �ڴ˷��ô����ʱ��
// 		//  ��ȷ�������رնԻ���Ĵ���
// 	}
// 	else if (nResponse == IDCANCEL)
// 	{
// 		// TODO: �ڴ˷��ô����ʱ��
// 		//  ��ȡ�������رնԻ���Ĵ���
// 	}

	// ɾ�����洴���� shell ��������
// 	if (pShellManager != NULL)
// 	{
// 		delete pShellManager;
// 	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

