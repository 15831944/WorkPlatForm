
// PeraPRCacheService.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "PeraPRCacheService.h"
#include "PeraPRCacheServiceDlg.h"
#include "IceService.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPeraPRCacheServiceApp

BEGIN_MESSAGE_MAP(CPeraPRCacheServiceApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPeraPRCacheServiceApp ����

CPeraPRCacheServiceApp::CPeraPRCacheServiceApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CPeraPRCacheServiceApp ����

CPeraPRCacheServiceApp theApp;


// CPeraPRCacheServiceApp ��ʼ��

BOOL CPeraPRCacheServiceApp::InitInstance()
{
	HANDLE hMutex = CreateMutex( NULL, TRUE, "{F24F8ABA-50D6-4063-997B-25A2ECFA12C3}" );
	if ( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		ZTools::WriteZToolsLog( "�Ѿ�����һ��Ӧ�ó��򣬱�������ʧ��" );
		CloseHandle( hMutex );
		hMutex = NULL;
		return FALSE;
	}
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	CIceService Service;
	Service.ServiceMain(0, NULL);

	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CPeraPRCacheServiceDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

