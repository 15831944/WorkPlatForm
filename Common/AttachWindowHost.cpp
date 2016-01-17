#include "StdAfx.h"
#include "AttachWindowHost.h"
#include "FileFunc.h"
#include "UtilityFunc.h"
#include <atlstr.h>


UINT WM_PROCESS_WND_ATTACHED = ::RegisterWindowMessage (_T("WM_PROCESS_WND_ATTACHED"));

vector< CAttachWindow* > theAttachWnds;

//创建
CString CreateProcessForAttach( int nExeFlag, HWND hHostWnd, LPCTSTR lpszClientExecPath )
{
	if ( nExeFlag <= 0 ) return "";
	if ( !IsWindow( hHostWnd ) ) return "";
	if ( !IsAccessibleFile( lpszClientExecPath ) ) return "";

	CString sId = GetNewEventNameWin7();

	CAttachWindow* pAw = new CAttachWindow();
	if ( !pAw->CreateClient( sId, nExeFlag, CPMSG_ATTACH_PROCESS_WND_RESPONSE
		, GetCurrentProcessId(), hHostWnd, lpszClientExecPath ) )
	{
		delete pAw; pAw = NULL;
		return "";
	}
	theAttachWnds.push_back( pAw );
	return sId;
}

//CMainframe::OnCopyData 被吸附进程创建成功的应答分支
//如果处理过，返回 TRUE，否则，返回 FALSE
BOOL ClientProcessCreatedByCopydata( CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct, HWND hHostWnd )
{
	if ( pCopyDataStruct->dwData != CPMSG_ATTACH_PROCESS_WND_RESPONSE ) return FALSE;

	if ( !IsWindow( hHostWnd ) ) return TRUE;

	CAttachWindow aw;
	if ( !aw.Deserialize( (LPCTSTR)pCopyDataStruct->lpData ) ) return TRUE;

	CAttachWindow* pAttaching = NULL;
	for ( vector< CAttachWindow* >::iterator itr = theAttachWnds.begin();
		itr != theAttachWnds.end(); ++itr )
	{
		if ( (*itr)->GetId() != aw.GetId() ) continue;
		(*itr)->CopyFrom( &aw );
		pAttaching = (*itr);
		break;
	}
	if ( pAttaching == NULL ) return TRUE;
	::PostMessage( hHostWnd, WM_ATTACH_PROCESS_WND, (WPARAM)pAttaching, 0 );
	return TRUE;
}

//绑定
BOOL OnAttachProcessWnd( CAttachWindow * pAttachWindow, HWND hHostWnd, BOOL bTheFilled )
{
	if ( pAttachWindow == NULL ) return FALSE;
	if ( !IsWindow( hHostWnd ) ) return FALSE;
	pAttachWindow->Attach( hHostWnd, WM_PROCESS_WND_ATTACHED, bTheFilled );
	return TRUE;
}

