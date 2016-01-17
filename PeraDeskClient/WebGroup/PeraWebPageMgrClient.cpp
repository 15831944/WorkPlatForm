#include "StdAfx.h"
#include "PeraWebPageMgrClient.h"
#include "PeraDCWebContainer.h"

CPeraWebPageMgrClient theWebPageMgr;

CPeraWebPageMgrClient::CPeraWebPageMgrClient(void)
{
}


CPeraWebPageMgrClient::~CPeraWebPageMgrClient(void)
{
}

BOOL CPeraWebPageMgrClient::_OnNewEvent( CCxWebPageEvent * pEvent )
{
	if ( pEvent == NULL ) return FALSE;

	if ( pEvent->m_dwWebPageProcessId == 0 )
		pEvent->m_dwWebPageProcessId = GetCurrentProcessId();
	if ( pEvent->m_hWndWebPage == NULL ) 
		pEvent->m_hWndWebPage = theApp.m_pMainWnd->GetSafeHwnd();
	return TRUE;
}

