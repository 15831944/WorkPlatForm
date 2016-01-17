#include "StdAfx.h"
#include "PeraWebPageMgrHost.h"
#include <Tools.h>
#include "PeraDeskClient.h"
#include "PeraWebPageEventExData.h"
#include <ProcessFunc.h>
//#include <DlgRunFlow.h>
#include "UtilityFunc.h"

CPeraWebPageMgrHost theWebPageMgr;

static CString GetWebPagePath( void )
{
	return GetExecDir() + "\\PeraDCWebContainer.exe";
}

CPeraWebPageMgrHost::CPeraWebPageMgrHost(void)
{
}


CPeraWebPageMgrHost::~CPeraWebPageMgrHost(void)
{
}

BOOL CPeraWebPageMgrHost::_OnNewEvent( CCxWebPageEvent * pEvent )
{
	if ( pEvent == NULL ) return FALSE;

	if ( pEvent->m_dwMainProcessId == 0 )
		pEvent->m_dwMainProcessId = GetCurrentProcessId();
	if ( pEvent->m_hWndMain == NULL ) 
		pEvent->m_hWndMain = theApp.m_pMainWnd->GetSafeHwnd();
	return TRUE;
}

BOOL CPeraWebPageMgrHost::OnWebPageEvent( LPVOID lpData )
{
	if ( lpData == NULL ) return FALSE;
	LPCTSTR lpszData = (LPCTSTR)lpData;

	ZTools::WriteZToolsFormatLog( "CPeraWebPageMgrHost::OnWebPageEvent begin >>>>>>>>>>>>>>>>>>>>>>>\r\n %s", (LPCTSTR)lpData );

	CCxWebPageEvent* pIncomingEvent = theWebPageMgr.NewEvent( lpszData );
	if ( pIncomingEvent == NULL )
	{
		//ZTools::WriteZToolsFormatLog( "CPeraWebPageMgrHost::OnWebPageEvent begin >>>>>>>>>>>>>>>>>>>>>>>\r\n %s", (LPCTSTR)lpData );
		return FALSE;
	}
	CCxWebPageEvent *pSpecEvent = NULL; // Host端发起的事件，存在于Mgr中

	//应答事件////////////////////////////////////////////////////////////////////////
	if ( pIncomingEvent->m_bAnswer )
	{
		pSpecEvent = theWebPageMgr.Find( pIncomingEvent->m_strEventId );
		if ( pSpecEvent == NULL )
		{
			theWebPageMgr.DeleteEvent( pIncomingEvent ); return FALSE;
		}

		switch ( pIncomingEvent->GetModuleFlag() )
		{
		case FW_TABVIEW_TASK_MODEL:
			switch ( pIncomingEvent->GetEventFlag() )
			{
			case FE_START:
				pSpecEvent->m_hWndWebPage = pIncomingEvent->m_hWndWebPage;
				pSpecEvent->m_dwWebPageProcessId = pIncomingEvent->m_dwWebPageProcessId;
				ZTools::WriteZToolsFormatLog( "CPeraWebPageMgrHost::OnWebPageEvent end <<<<<<<<<<<<<<<<<<<<<<<<<<\r\n %s\r\n", GetEventDisplay( pSpecEvent ) );
				break;

			default:
				break;
			}
			break;

		default:
			break;
		}
	} // end of if Anwser

	//请求事件////////////////////////////////////////////////////////////////////////
	else
	{
		switch ( pIncomingEvent->GetEventFlag() )
		{
		case FE_ATTACH:
			pSpecEvent = theWebPageMgr.Find( pIncomingEvent->m_strEventId );
			if ( pSpecEvent == NULL )
			{
				theWebPageMgr.DeleteEvent( pIncomingEvent ); return FALSE;
			}

			switch ( pIncomingEvent->GetModuleFlag() )
			{
			case FW_TABVIEW_TASK_MODEL:
				::PostMessage( theApp.m_pMainWnd->GetSafeHwnd(), WM_ATTACH_WEBPAGE, (WPARAM)pSpecEvent, 0 );
				ZTools::WriteZToolsFormatLog( "CMainFrame::OnWebPageEvent end <<<<<<<<<<<<<<<<<<<<<<< \r\n %s\r\n", GetEventDisplay( pSpecEvent ) );
				break;

			default:
				break;
			}
			break;

		default:
			break;
		}
	}

	theWebPageMgr.DeleteEvent( pIncomingEvent );
	return TRUE;
}

CCxWebPageEvent * CPeraWebPageMgrHost::Require( int nModuleFlag, int nEventFlag
	, CCxWebPageEventExData * pExData )
{
	CCxWebPageEvent * pEvent = CPeraWebPageMgr::Require( nModuleFlag, nEventFlag, pExData );
	if ( pEvent != NULL ) return pEvent;

	switch ( nEventFlag )
	{
	case FE_START:
		switch ( nModuleFlag )
		{
		case FW_TABVIEW_TASK_MODEL:
			//::PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_PROGRESS_SHOW, (WPARAM)TRUE, NULL);
			pEvent = NewEvent( nModuleFlag, nEventFlag, pExData, TRUE );
			break;
		default:
			return NULL;
		}
		if ( pEvent == NULL ) return NULL;
		if ( !StartWebPage( pEvent ) )
		{
			theWebPageMgr.DeleteEvent( pEvent ); return NULL;
		}
		return pEvent;

	case FE_NAVIGATE:
		{
			VIEW_DATA * pViewData = NULL;
			HWND hwndWebPage = NULL;

			switch ( nModuleFlag )
			{
			case FW_TABVIEW_TASK_MODEL:
				pViewData = theViewMgr.Find( g_lpszViewTitleTaskModel );
				if ( pViewData == NULL ) return NULL;
				hwndWebPage = pViewData->m_hwndInner;
				if ( !IsWindow( hwndWebPage ) ) return NULL;
				pEvent = NewEvent( nModuleFlag, nEventFlag, pExData, FALSE );
				break;
			default:
				return NULL;
			}
			if ( pEvent == NULL ) return NULL;

			if ( !IsWindow( hwndWebPage ) ) return pEvent;
			pEvent->SendCopyData( hwndWebPage );
		}

		return pEvent;

	default:
		break;
	}
	return NULL;
}




BOOL CPeraWebPageMgrHost::StartWebPage( CCxWebPageEvent * pEvent )
{
	if ( pEvent == NULL ) return FALSE;

	CString strExecPath = GetWebPagePath();
	if ( !IsAccessibleFile( strExecPath ) ) return FALSE;
	if ( pEvent->GetEventFlag() != FE_START ) return FALSE;
	if ( pEvent->m_dwMainProcessId == 0 || pEvent->m_hWndMain == NULL ) return FALSE;
	if ( pEvent->GetExData() == NULL ) return FALSE;

	CString strStartParam = " -json " + pEvent->GetJsonStr();
	ZTools::WriteZToolsFormatLog("\r\n\r\n\r\n\r\n@@===CPeraWebPageMgrHost::StartWebPage \r\n%s", strStartParam);

	/*
	SHELLEXECUTEINFO si;
	memset(&si, NULL, sizeof(si));
	si.cbSize = sizeof(si);
	si.nShow = SW_HIDE;
	si.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
	si.lpVerb = _T("open");
	//si.lpDirectory = pThis->m_strAppWorkPath;
	si.lpFile = strExecPath;
	si.lpParameters = (LPCTSTR)strStartParam;
	if ( !ShellExecuteEx(&si) )
	{
		delete pEvent; pEvent = NULL; return NULL;
	}
	//if ( !pEvent->CreateSyncEvent() )
	//{
	//	delete pEvent; pEvent = NULL; return FALSE;
	//}
	*/

	if ( CreateProcessInJob(strExecPath, strStartParam, SW_HIDE, g_lpszAppTitle) == 0)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CPeraWebPageMgrHost::Navigate( int nModuleFlag, LPCTSTR lpszUrl )
{
	CCxWPENavigate * pData = new CCxWPENavigate();
	pData->m_strUrl = lpszUrl == NULL ? "about:blank" : lpszUrl;
	
	if ( !Require( nModuleFlag
		, CPeraWebPageMgrHost::FE_NAVIGATE, pData ) )
	{
		//失败
		delete pData; pData = NULL; return FALSE;
	}
	delete pData; pData = NULL; return TRUE;
}
BOOL CPeraWebPageMgrHost::SendCopyDataLoginOut(HWND hWndSend)
{
	VIEW_DATA * pViewData = NULL;
	HWND hwndWebPage = NULL;
	pViewData = theViewMgr.Find( g_lpszViewTitleTaskModel );

	if ( pViewData == NULL ) return FALSE;

	hwndWebPage = pViewData->m_hwndInner;

	if ( !IsWindow( hwndWebPage ) ) return FALSE;

	SendCopyData(hwndWebPage, CPMSG_LOGIN_OUT, NULL,0);
	return TRUE;
}
BOOL CPeraWebPageMgrHost::SendCopyDataRefresh(HWND hWndSend)
{
	VIEW_DATA * pViewData = NULL;
	HWND hwndWebPage = NULL;
	pViewData = theViewMgr.Find( g_lpszViewTitleTaskModel );

	if ( pViewData == NULL ) return FALSE;

	hwndWebPage = pViewData->m_hwndInner;

	if ( !IsWindow( hwndWebPage ) ) return FALSE;

	SendCopyData(hwndWebPage, CPMSG_REFRESH, NULL,0);
	return TRUE;
}