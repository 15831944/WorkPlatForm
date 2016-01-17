
// WebPage.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "PeraPDWebContainer.h"
#include "DlgMain.h"
#include "Tools.h"
#include "UserDefine.h"
#include "WebContainerCmdLine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWebPageApp

BEGIN_MESSAGE_MAP(CWebPageApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CWebPageApp ����

CWebPageApp::CWebPageApp()
{
	ZTools::InitZToolsLog();
	ch.SetProcessExceptionHandlers();
	ch.SetThreadExceptionHandlers();
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CWebPageApp ����

CWebPageApp theApp;


// CWebPageApp ��ʼ��

BOOL CWebPageApp::InitInstance()
{
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

	//m_pStartEvent = theWebPageMgr.NewEvent();

	//ParseCustomCommandLine();
	//ZTools::WriteZToolsFormatLog("CWebPageApp::ParseCustomCommandLine %s", "end <<<<<<<<<<<<<<<\r\n");
	theCmd.Parse();

	CDlgMain dlg;
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

//************************************
// PeraWebContainer.exe  -json {"url" : "www.baidu.com", "user" : "fei", "realusername" : "��?�ɤ�?, "ticket": "1", "password": "999999", "role": "admin"}
//************************************
 //BOOL CWebPageApp::ParseCustomCommandLine( void )
 //{
 //	CString strSrc = GetCommandLine();
 //	strSrc.Trim();
 ////	m_pStartEvent->Clear();
 //
 //	ZTools::WriteZToolsFormatLog("CWebPageApp::ParseCustomCommandLine Begin>>>>>>>>>>>>>>>>>>\r\n %s", strSrc);
 //	
 //	TCHAR exeFullPath[1024] = {0};
 //	GetModuleFileName(NULL, exeFullPath, 1024);
 //	CString strExec = exeFullPath;
 //	
 //	int nExecPos = strSrc.Find( strExec );
 //	if ( nExecPos == 1 ) //·����˫����
 //	{
 //		strExec = "\"" + strExec + "\"";
 //	}
 //	else if ( nExecPos == 0 ) //·������˫����
 //	{
 //
 //	}
 //	else if ( nExecPos == -1 ) //����·��
 //	{
 //		strExec = "";
 //	}
 //	else
 //	{
 //		AfxMessageBox("���������в���ʧ�ܡ�"); return FALSE;
 //	}
 //
 //	if ( strExec.GetLength() >= strSrc.GetLength() )
 //	{
 //		AfxMessageBox("���������в���ʧ�ܡ�"); return FALSE;
 //	}
 //
 //	CString strParam = strSrc.Mid( strExec.GetLength(), strSrc.GetLength() - strExec.GetLength() );
 //	strParam.Trim();
 //	
 //	int nTotalLen = strParam.GetLength();
 //	if ( nTotalLen < 7 )
 //	{
 //		AfxMessageBox("���������в���ʧ�ܡ�"); return FALSE;
 //	}
 //
 //	//�ҡ�-��
 //	int nHeaderPos = strParam.Find( "-" );
 //	if ( nHeaderPos != 0 ) return FALSE;
 //	//��⡰json����ʶ
 //	CString strHeader = strParam.Mid(1, 4);
 //	if ( strHeader.CompareNoCase("json") != 0 )
 //	{
 //		AfxMessageBox("���������в���ʧ�ܡ�"); return FALSE;
 //	}
 //	//
 //	CString strJsonStr = strParam.Mid( 5, nTotalLen - 5 );
 //	strJsonStr.Trim();
 //
 //	m_pStartEvent = theWebPageMgr.NewEvent( strJsonStr );
 //	if ( m_pStartEvent == NULL )
 //	{
 //		AfxMessageBox("���������в���ʧ�ܡ�"); return FALSE;
 //	}
 //
 //	g_hwndMainWindow = m_pStartEvent->m_hWndMain;
 //
 //	//switch ( m_pStartEvent->GetModuleFlag() )
 //	//{
 //	//case CPeraWebPageMgr::FW_RUNFLOW:
 //	//	m_pStartEvent->m_pData = new CWPERunFlowStart();
 //	//	m_pStartEvent->ParseExData();
 //	//	break;
 //	//case CPeraWebPageMgr::FW_VIEW_RUNFLOW:
 //	//	m_pStartEvent->m_pData = new CCxWPENavigate();
 //	//	m_pStartEvent->ParseExData();
 //	//	break;
 //	//default:
 //	//	break;
 //	//}
 //
 //
 //	//ZTools::WriteZToolsLog(strCommand);
 //	//if(strCommand.find("-json") != std::string::npos || strCommand.find("-JSON") != std::string::npos)
 //	//{//JSON��?���Y
 //	//	int nBraceLeft = strCommand.find('{');
 //	//	int nBraceRight = strCommand.find('}');
 //
 //	//	if(nBraceLeft == std::string::npos || nBraceRight == std::string::npos || nBraceLeft >= nBraceRight)
 //	//	{//������?��D��?ʽ?���?��
 //	//		return;
 //	//	}
 //
 //		//int nCount = nBraceRight - nBraceLeft + 1;
 //		//std::string strJson = strCommand.substr(nBraceLeft, nCount);
 //
 //		//Json::Reader jReader;
 //		//Json::Value jV;
 //		//if(jReader.parse(strJson, jV) == false)
 //		//{//��a��?���?��
 //		//	return;
 //		//}
 //
 //		//m_strUrl = jV["url"].asString().c_str();
 //		//m_strPassword = jV["password"].asString().c_str();
 //		//m_strRealUserName = jV["realusername"].asString().c_str();
 //		//m_strRole = jV["role"].asString().c_str();
 //		//m_strTicket = jV["ticket"].asString().c_str();
 //		//m_strUser = jV["user"].asString().c_str();
 //	//}
 //
 //	return TRUE;
 //}


HWND GetMainWnd( void )
{
	if ( theApp.m_pMainWnd == NULL ) return NULL;
	if ( !::IsWindow( theApp.m_pMainWnd->GetSafeHwnd() ) ) return NULL;
	return theApp.m_pMainWnd->GetSafeHwnd();
}
