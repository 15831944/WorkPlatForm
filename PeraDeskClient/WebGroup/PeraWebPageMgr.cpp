#include "StdAfx.h"
#include "PeraWebPageMgr.h"
#include "PeraWebPageEventExData.h"
#include <UtilityFunc.h>
#include "PeraWebPageMgrHost.h"

CPeraWebPageMgr::CPeraWebPageMgr(void)
{
}

CPeraWebPageMgr::~CPeraWebPageMgr(void)
{
}

CCxWebPageEventExData* CPeraWebPageMgr::_OnNewEventExData( int nModuleFlag, int nEventFlag )
{
	CCxWebPageEventExData* pData = CCxWebPageMgr::_OnNewEventExData( nModuleFlag, nEventFlag );
	if ( pData != NULL ) return pData;

	switch ( nEventFlag )
	{
	case FE_START:
		switch ( nModuleFlag )
		{
		case FW_TABVIEW_TASK_MODEL:
			return new CCxWPENavigate;
		default:
			return NULL;
		}
		return NULL;

	default:
		return NULL;
	}
	return NULL;
}

CString CPeraWebPageMgr::GetEventDisplay( CCxWebPageEvent * pEvent )
{
	if ( pEvent == NULL ) return "";

	CString strResult, str;
	switch ( pEvent->m_nModuleFlag )
	{
	case FW_TABVIEW_TASK_MODEL:
		str = "Module: FW_TABVIEW_TASK_MODEL"; break;
	default:
		str = "Module: 0"; break;
	}
	strResult += str;

	switch ( pEvent->m_nEventFlag )
	{
	case FE_START:
		str = pEvent->m_bAnswer ? ", Event: FE_START Ok" : ", Event: FE_START"; break;
	case FE_ATTACH:
		str = pEvent->m_bAnswer ? ", Event: FE_ATTACH Ok" : ", Event: FE_ATTACH"; break;
	default:
		str = ", Event: 0"; break;
	}
	strResult += str;

	return strResult;
}

//
//
//BOOL CPeraWebPageMgr::NavigateInNewTab( LPCTSTR lpszUrl, int nWebPageFlag )
//{
//	CCxWPENavigate * pData = new CCxWPENavigate();
//	pData->m_strUrl = lpszUrl == NULL ? "about:blank" : lpszUrl;
//
//	if ( !Require( nWebPageFlag
//		, CPeraWebPageMgr::FE_START, pData ) )
//	{
//		//theWebPageMgr.DeleteEvent( pEvent ); return FALSE;
//	}
//	delete pData; pData = NULL;
///*
//	DWORD dwPid = CreateProcessInJob(sExePath, sParam, SW_HIDE);
//
//	HWND hWnd = NULL;
//	while (hWnd == NULL)
//	{
//		hWnd = GetMainWindow(dwPid, "", "#32770");
//
//		if (hWnd == NULL)
//		{
//			Sleep(200);
//		}
//	}
//	*/
//	//CString sTabTitle;
//	//sTabTitle.Format("ÔËÐÐÊµÀý:%s", pMgr->m_sFlowId);
//	//theViewMgr.NewView(sTabTitle, hWnd);
//
//	//
//	//CRunFlowExec eExec;
//	//eExec.m_strFlowId = pMgr->m_sFlowId;
//	//CString strResult = InvokeWsMethod( eExec.GetMethodName(), eExec.GetMethodParamStr() );
//	return TRUE;
//}