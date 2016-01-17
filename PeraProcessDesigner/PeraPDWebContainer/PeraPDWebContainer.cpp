
// WebPage.cpp : 定义应用程序的类行为。
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


// CWebPageApp 构造

CWebPageApp::CWebPageApp()
{
	ZTools::InitZToolsLog();
	ch.SetProcessExceptionHandlers();
	ch.SetThreadExceptionHandlers();
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CWebPageApp 对象

CWebPageApp theApp;


// CWebPageApp 初始化

BOOL CWebPageApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	//m_pStartEvent = theWebPageMgr.NewEvent();

	//ParseCustomCommandLine();
	//ZTools::WriteZToolsFormatLog("CWebPageApp::ParseCustomCommandLine %s", "end <<<<<<<<<<<<<<<\r\n");
	theCmd.Parse();

	CDlgMain dlg;
	m_pMainWnd = &dlg;

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

//************************************
// PeraWebContainer.exe  -json {"url" : "www.baidu.com", "user" : "fei", "realusername" : "赵?飞え?, "ticket": "1", "password": "999999", "role": "admin"}
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
 //	if ( nExecPos == 1 ) //路径带双引号
 //	{
 //		strExec = "\"" + strExec + "\"";
 //	}
 //	else if ( nExecPos == 0 ) //路径不带双引号
 //	{
 //
 //	}
 //	else if ( nExecPos == -1 ) //不含路径
 //	{
 //		strExec = "";
 //	}
 //	else
 //	{
 //		AfxMessageBox("解析命令行参数失败。"); return FALSE;
 //	}
 //
 //	if ( strExec.GetLength() >= strSrc.GetLength() )
 //	{
 //		AfxMessageBox("解析命令行参数失败。"); return FALSE;
 //	}
 //
 //	CString strParam = strSrc.Mid( strExec.GetLength(), strSrc.GetLength() - strExec.GetLength() );
 //	strParam.Trim();
 //	
 //	int nTotalLen = strParam.GetLength();
 //	if ( nTotalLen < 7 )
 //	{
 //		AfxMessageBox("解析命令行参数失败。"); return FALSE;
 //	}
 //
 //	//找“-”
 //	int nHeaderPos = strParam.Find( "-" );
 //	if ( nHeaderPos != 0 ) return FALSE;
 //	//检测“json”标识
 //	CString strHeader = strParam.Mid(1, 4);
 //	if ( strHeader.CompareNoCase("json") != 0 )
 //	{
 //		AfxMessageBox("解析命令行参数失败。"); return FALSE;
 //	}
 //	//
 //	CString strJsonStr = strParam.Mid( 5, nTotalLen - 5 );
 //	strJsonStr.Trim();
 //
 //	m_pStartEvent = theWebPageMgr.NewEvent( strJsonStr );
 //	if ( m_pStartEvent == NULL )
 //	{
 //		AfxMessageBox("解析命令行参数失败。"); return FALSE;
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
 //	//{//JSON传?递蘗
 //	//	int nBraceLeft = strCommand.find('{');
 //	//	int nBraceRight = strCommand.find('}');
 //
 //	//	if(nBraceLeft == std::string::npos || nBraceRight == std::string::npos || nBraceLeft >= nBraceRight)
 //	//	{//命ü令?行D格?式?错洙?误ó
 //	//		return;
 //	//	}
 //
 //		//int nCount = nBraceRight - nBraceLeft + 1;
 //		//std::string strJson = strCommand.substr(nBraceLeft, nCount);
 //
 //		//Json::Reader jReader;
 //		//Json::Value jV;
 //		//if(jReader.parse(strJson, jV) == false)
 //		//{//解a析?错洙?误ó
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
