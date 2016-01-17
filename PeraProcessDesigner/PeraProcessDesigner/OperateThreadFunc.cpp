#include "stdafx.h"
#include "OperateThreadFunc.h"
#include "ProcessMgr.h"
#include "PeraProcessDesigner.h"
#include "PeraWsAstIceClient_I.h"
#include "GetGlobalDataFromReg.h"
#include "PeraProcessDesignerUtility.h"
#include "Tools.h"
#include <Strsafe.h>

static HANDLE g_hCopyDataToRunnerThread = NULL;

static DWORD WINAPI CopyDataToRunnerThread( LPVOID lpParameter )
{
	char* pBuffer = (char*)lpParameter;
	if ( pBuffer == NULL )
	{
		HideLoading();
		return 0;
	}
	CString sNewXpdl = pBuffer;
	delete pBuffer;
	pBuffer = NULL;
	if (sNewXpdl.IsEmpty())
	{
		CString sMsg;
		sMsg.Format("单机运行发起设计过程时准备XPDL文件返回空");
		MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), sMsg, g_lpszAppTitle, MB_OK|MB_ICONSTOP);
		HideLoading();
		return 0;
	}
	CString sEventName = "{BF6AE184-19B1-4357-B839-FE273EBE5EB3}";
	HANDLE hWaitEvent = CreateEvent( NULL, FALSE, FALSE, sEventName );
	if ( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		//Error
	}

	CPeraWsAstIceClient_I CacheSrvIce;
	CString sRunId = CacheSrvIce.GetRunId().c_str();
	CacheSrvIce.CreateCacheByXpdlStream( sRunId, sNewXpdl, FALSE );

	CString sSend;
	CString sWsPath = theApp.m_processMgr.m_sProcessFilePath;
	sWsPath.Replace( "\\", "\\\\" );
	sSend.Format( "{\"runFlag\":\"modeling\",\"from\":\"建模\",\"runId\":\"%s\",\"eventName\":\"%s\",\"wsPath\":\"%s\"}", sRunId, sEventName, sWsPath );
	COPYDATASTRUCT CopyData = {0};
	CopyData.dwData = 3;
	CopyData.lpData = (LPVOID)(LPCTSTR)sSend; 
	CopyData.cbData = sSend.GetLength()+1;

	HWND hRunner = GetRunnerHandle();
	if ( hRunner != NULL )
	{
		::SendMessage( hRunner, WM_COPYDATA, NULL, (WPARAM)&CopyData );
		::PostMessage( hRunner, WM_SHOWWINDOW, (WPARAM)SW_NORMAL, NULL);
	}
	else
	{
		CString sRunnerStartEventName = "RunnerStart_C76740D4-D3D5-42D5-AFB9-4B8A9FBA20C9_EventName";
		HANDLE hRunnerStart = CreateEvent( NULL, FALSE, FALSE, sRunnerStartEventName );
		CString sRunnerPath = GetPeraGlobalDataFromReg( "LM", "PeraProcessRunner" );
		sRunnerPath += "\\PeraProcessRunner.exe";
		if ( !PathFileExists( sRunnerPath ) )
		{
			CloseHandle( hRunnerStart );
			CloseHandle( hWaitEvent );
			hRunnerStart = NULL;
			hWaitEvent = NULL;
			HideLoading();
			return 0;
		}
		ShellExecute( NULL, "open", sRunnerPath, NULL, NULL, SW_HIDE );
		if ( WaitForSingleObject( hRunnerStart, 1000*60 ) == WAIT_OBJECT_0 )
		{
			Sleep( 1000 );
			CloseHandle( hRunnerStart );
			hRunnerStart = NULL;
			hRunner = GetRunnerHandle();
			if ( hRunner != NULL )
			{
				::SendMessage( hRunner, WM_COPYDATA, NULL, (LPARAM)&CopyData );
				::PostMessage( hRunner, WM_SHOWWINDOW, (WPARAM)SW_SHOW, NULL);
			}
			else
			{
				HideLoading();
				return 0;
			}
		}
		else
		{
			HideLoading();
			CloseHandle( hWaitEvent );
			CloseHandle( hRunnerStart );
			hWaitEvent = NULL;
			hRunnerStart = NULL;
			return 0;
		}
	}

	//::SetWindowPos( hRunner, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );
	SetForegroundWindow(hRunner);

	if ( WaitForSingleObject( hWaitEvent, INFINITE ) == WAIT_OBJECT_0 )
	{
		HideLoading();
	}
	::SetWindowPos( hRunner, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );
	::SetWindowPos( hRunner, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );
	::CloseHandle( hWaitEvent );
	hWaitEvent = NULL;
	return 0;
}

BOOL RunAloneVersion( BOOL bIsModelRun )
{
	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL )
	{
		std::string strError("OnAloneVersionRun 获取Svg失败");
		ZTools::WriteZToolsLog(strError);
		return FALSE;	
	}

	CWebMethodData wmd;
	if ( bIsModelRun )
	{
		wmd.m_sMethodName = "getReplacedAbsolutePathXpdl";
	}
	else
	{
		wmd.m_sMethodName = "getReplacedAbsolutePathSubXpdl";
	}
	pSvg->InvokeJsMethod( &wmd );
	CString sResult = wmd.m_sResult;
	if ( sResult.IsEmpty() )
	{
		std::string strError("OnAloneVersionRun 获取xpdl失败");
		ZTools::WriteZToolsLog(strError);
		return FALSE;
	}
	DWORD dwLength = sResult.GetLength();
	dwLength += 64;
	char* pBuffer = new char[ dwLength ];
	ZeroMemory( pBuffer, dwLength );
	StringCbCopy( pBuffer, dwLength, sResult );

	DWORD dwThreadId = 0;
	g_hCopyDataToRunnerThread = ::CreateThread(
		NULL
		, 0
		, CopyDataToRunnerThread
		, (LPVOID)pBuffer
		, CREATE_SUSPENDED
		, &dwThreadId);

	if (g_hCopyDataToRunnerThread == NULL || dwThreadId == 0)
	{
		std::string strError("创建线程CreateCopyDataToRunnerThread失败");
		ZTools::WriteZToolsLog(strError);
		return FALSE;	
	}
	ResumeThread(g_hCopyDataToRunnerThread);
	::CloseHandle(g_hCopyDataToRunnerThread);
	g_hCopyDataToRunnerThread = NULL;
	return TRUE;
}