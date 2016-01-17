#include "StdAfx.h"
#include "HookFunc.h"

//////////////////////////////////////////////////////////////////////////
//HOOK���ı���ҳ�Ի������
static HHOOK g_hPrevHookXP = NULL;
static HWND g_hWndNeedChangeTitle = NULL;// ������Ϣ HOOK �ص�����
static LPCTSTR s_pStrTitle = NULL;
static const LPCTSTR arrDialogTitles[] = {
	_T("PeraProcessDesigner"), 
	_T("Microsoft Internet Explorer"), 
	_T("������ҳ����Ϣ"),
	_T("Windows Internet Explorer"), 
	_T("Alert"), 
	_T("Message from webpage")
};

static LRESULT CALLBACK HookProcXP(int iCode, WPARAM wParam, LPARAM lParam)
{
	if ((((CWPSTRUCT *) lParam)->message == WM_WINDOWPOSCHANGED))
	{
		if (g_hWndNeedChangeTitle != NULL)
		{
			if (IsWindow(g_hWndNeedChangeTitle))
			{
				HWND hwnd = g_hWndNeedChangeTitle;
				g_hWndNeedChangeTitle = NULL;

				char szWindowName[MAX_PATH];
				GetWindowText(hwnd, szWindowName,sizeof(szWindowName) / sizeof(szWindowName[0]));
				CString strWindowName =  szWindowName;
				if(strWindowName.Find(" -- ��ҳ�Ի���") != -1) 
				{
					strWindowName.Replace(" -- ��ҳ�Ի���", "\0");
					SetWindowText(hwnd, strWindowName);
				}
			}
		}
	}
	else if ((((CWPSTRUCT *) lParam)->message == WM_SETTEXT))
	{
		HWND hwnd = ((CWPSTRUCT *) lParam)->hwnd;

		char szClassName[MAX_PATH];
		GetClassName(((CWPSTRUCT *) lParam)->hwnd, szClassName,sizeof(szClassName) / sizeof(szClassName[0]));
		CString strClassName =  szClassName;
		if(strClassName.CompareNoCase(_T("Internet Explorer_TridentDlgFrame")) == 0) 
		{
			CString sTitle = (LPCTSTR)((CWPSTRUCT *) lParam)->lParam;
			if (sTitle.Find(" -- ��ҳ�Ի���") != -1)
			{
				g_hWndNeedChangeTitle = hwnd;
			}
		}
	}
	else if ((((CWPSTRUCT *) lParam)->message == WM_SHOWWINDOW))
	{
		HWND hwnd = ((CWPSTRUCT *) lParam)->hwnd;

		char szClassName[MAX_PATH];
		GetClassName(((CWPSTRUCT *) lParam)->hwnd, szClassName,sizeof(szClassName) / sizeof(szClassName[0]));
		CString strClassName =  szClassName;
		if(strClassName.CompareNoCase(_T("#32770")) == 0) 
		{
			char szWindowName[MAX_PATH];
			GetWindowText(hwnd, szWindowName,sizeof(szWindowName) / sizeof(szWindowName[0]));
			CString strWindowName =  szWindowName;

			BOOL bChangeTitle = FALSE;
			int nCount = sizeof( arrDialogTitles ) / sizeof( LPCTSTR );
			for (int i = 0; i < nCount; i++)
			{
				if (strWindowName.CompareNoCase(arrDialogTitles[i]) == 0)
				{
					bChangeTitle = TRUE;
					break;
				}
			}
			if (bChangeTitle == TRUE && s_pStrTitle != NULL)
			{
				::SetWindowText(hwnd, s_pStrTitle);
			}
		}
	}
	return CallNextHookEx(g_hPrevHookXP, iCode, wParam, lParam);
}
//////////////////////////////////////////////////////////////////////////

BOOL HookWebPageTitle(BOOL bHook, LPCTSTR pStrTitle)
{
	if ( bHook )
	{
		s_pStrTitle = pStrTitle;
		g_hPrevHookXP = SetWindowsHookEx(WH_CALLWNDPROC, HookProcXP, 0, GetCurrentThreadId());
		return TRUE;
	}
	else
	{
		s_pStrTitle = NULL;
		return UnhookWindowsHookEx(g_hPrevHookXP);
	}
}
