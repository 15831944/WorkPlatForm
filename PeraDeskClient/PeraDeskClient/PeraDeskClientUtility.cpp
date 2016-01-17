#include "StdAfx.h"
#include "PeraDeskClientUtility.h"
#include "CxViewMgr.h"
#include "PeraDeskClient.h"
//#include "DlgDistribution.h"
#include "CStringChecker.h"
#include "ConvertData.h"
#include <winuser.h>
#include "Tools.h"
#include "AppFunc.h"
#include "PtrVectorHeader.h"

//CDlgHistoryMonitor * GetHistoryMonitorDilogPtr (void)
//{
//	VIEW_DATA * pView = theViewMgr.Find( g_lpszViewTitleHistoryMonitor );
//	if ( pView == NULL ) return NULL;
//	CDlgHistoryMonitor * pDlg = reinterpret_cast< CDlgHistoryMonitor * >( pView->m_dwExData );
//	if ( pDlg == NULL ) return NULL;
//	if ( !::IsWindow( pDlg->GetSafeHwnd() ) ) return NULL;
//
//	return pDlg;
//}


/*
模拟鼠标点击事件。
参数：
	ptDest：目标窗口上，鼠标点击的坐标（0,0点从非客户区左上角计算）
	hwndDest：目标窗口
	ptSrc：鼠标所在的原始窗口上，鼠标位置（0,0点从非客户区左上角计算）
	hwndSrc：鼠标所在的原始窗口
*/
void SetMouseClick( CPoint ptDest, HWND hwndDest, CPoint ptSrc, HWND hwndSrc )
{
	if ( hwndDest != NULL && !IsWindow(hwndDest) ) return;
	if ( hwndSrc != NULL && !IsWindow(hwndSrc) ) return;

	if ( hwndSrc == NULL && ptSrc == CPoint(-1, -1) )
	{
		::GetCursorPos( &ptSrc );
	}

	CPoint ptSrcScreen = ptSrc;
	CPoint ptDestScreen = ptDest;

	WINDOWINFO wiSrc, wiDest;
	memset(&wiSrc, NULL, sizeof(WINDOWINFO));
	wiSrc.cbSize = sizeof(WINDOWINFO);
	memset(&wiDest, NULL, sizeof(WINDOWINFO));
	wiDest.cbSize = sizeof(WINDOWINFO);

	BOOL bSrcClient, bSrcWnd, bDestClient, bDestWnd;
	bSrcClient = bSrcWnd = bDestClient = bDestWnd = FALSE;

	if ( hwndSrc != NULL )
	{
		CWnd::FromHandle( hwndSrc )->GetWindowInfo( &wiSrc );
		::ClientToScreen( hwndSrc, &ptSrcScreen );

		if ( ptSrcScreen.x > wiSrc.rcWindow.left && ptSrcScreen.x < wiSrc.rcWindow.right 
			&& ptSrcScreen.y > wiSrc.rcWindow.top && ptSrcScreen.y < wiSrc.rcWindow.bottom )
		{
			bSrcWnd = TRUE;
		}

		if ( ptSrcScreen.x > wiSrc.rcClient.left && ptSrcScreen.x < wiSrc.rcClient.right 
			&& ptSrcScreen.y > wiSrc.rcClient.top && ptSrcScreen.y < wiSrc.rcClient.bottom )
		{
			bSrcClient = TRUE;
		}
	}
		
	if ( hwndDest != NULL )
	{
		CWnd::FromHandle( hwndDest )->GetWindowInfo( &wiDest );
		::ClientToScreen( hwndDest, &ptDestScreen );

		if ( ptDestScreen.x > wiSrc.rcWindow.left && ptDestScreen.x < wiSrc.rcWindow.right 
			&& ptDestScreen.y > wiSrc.rcWindow.top && ptDestScreen.y < wiSrc.rcWindow.bottom )
		{
			bDestWnd = TRUE;
		}

		if ( ptDestScreen.x > wiSrc.rcClient.left && ptDestScreen.x < wiSrc.rcClient.right 
			&& ptDestScreen.y > wiSrc.rcClient.top && ptDestScreen.y < wiSrc.rcClient.bottom )
		{
			bDestClient = TRUE;
		}
	}

	::SetCursorPos( ptDestScreen.x, ptDestScreen.y );
	Sleep(0);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	Sleep(0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	Sleep(0);
	::SetCursorPos( ptSrcScreen.x, ptSrcScreen.y );
	Sleep(0);
}


HWND FindPeraDeskClientMainWindow( void )
{
	CString strProcessName = "PeraDeskClient.exe";
	
	vector<CString> vCaption;
	vCaption.push_back( g_lpszAppTitle );

	DWORD dwCurProcessId = GetCurrentProcessId();
	DWORD dwProcessIdOther = 0;

	vector<PROCESSENTRY32 *> vProcess;
	int nCnt = GetProcessEntry32( vProcess, (LPCTSTR)strProcessName );
	if ( nCnt > 1 )
	{
		for (int i=0; i<nCnt; i++)
		{
			if ( vProcess[i]->th32ProcessID == dwCurProcessId )
			{
				continue;
			}
			dwProcessIdOther = vProcess[i]->th32ProcessID;
			break;
		}
		PtrVectorClear( vProcess );
	}
	else
	{
		PtrVectorClear( vProcess );
		return NULL;
	}

	if ( dwProcessIdOther == 0 ) return NULL;


	HWND hwndOther = GetMainWindow( dwProcessIdOther, vCaption );
	return hwndOther;
}
HWND FindPeraDeskClientLonginWindow( void )
{
	CWnd * pDeskTopWnd = CWnd::GetDesktopWindow();
	CWnd *pFind = pDeskTopWnd->GetWindow(GW_CHILD);
	while (pFind && pFind->m_hWnd)
	{
		if (::GetProp(pFind->m_hWnd,"PeraDeskClient_LoginWnd"))
			return pFind->m_hWnd;

		pFind = pFind->GetWindow(GW_HWNDNEXT);
	}
	return NULL;
}