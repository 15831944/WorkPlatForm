#include "StdAfx.h"
#include "UtilityFunc.h"
#include "IniClass.h"
#include "FileFunc.h"

BOOL GetChannelUrl(CString &strChannelUrl, const CString &strChannelName)
{
	strChannelUrl = "";

	CString strIniPath = GetExecDir() + _T("\\config.ini");
	if (!IsAccessibleFile(strIniPath)) return FALSE;

	CIniClass IniObj(strIniPath);
	strChannelUrl = IniObj.GetKeyValue(strChannelName, "WebURL");
	if (strChannelUrl.IsEmpty())
	{
		return FALSE;
	}
	return TRUE;
}


BOOL GetWebServiceUrl(CString &strUrl, const CString &strName)
{
	strUrl = "";

	CString strIniPath = GetExecDir() + _T("\\ws-config.ini");
	if (!IsAccessibleFile(strIniPath)) return FALSE;

	CIniClass IniObj(strIniPath);
	strUrl = IniObj.GetKeyValue(strName,"WebService");
	if (strUrl.IsEmpty())
	{
		return FALSE;
	}
	return TRUE;
}


void AttachTicket(CString& strUrl, CString strTicket)
{
	if (strUrl.IsEmpty() || strTicket.IsEmpty()) return;
	if (strUrl.CompareNoCase("about:blank") == 0) return;

	if (strUrl.Find(_T("?")) == -1)
		strUrl = strUrl + _T("?") + strTicket;
	else
		strUrl = strUrl + _T("&") + strTicket;
}


void AttachUrlParam(CString& strUrl, CString strParam)
{
	if (strUrl.IsEmpty()) return;
	if (strUrl.CompareNoCase("about:blank") == 0) return;

	if (strUrl.Find(_T("?")) == -1)
		strUrl = strUrl + _T("?") + strParam;
	else
		strUrl = strUrl + _T("&") + strParam;
}


// 弹出右键菜单 ////////////////////////////////////////////////////////////////////////
void ShowPopMenu( UINT nPopMenuID, CWnd * pParent )
{
	if (pParent == NULL) return;
	if ( !IsWindow( pParent->GetSafeHwnd() ) )return;

	POINT pt;
	GetCursorPos(&pt);
	CMenu menu;
	VERIFY(menu.LoadMenu(nPopMenuID));
	CMenu * pPopup = menu.GetSubMenu(0);
	// 这个就是菜单在菜单资源条中的位置
	ASSERT(pPopup != NULL);
	// 控制菜单状态，单击空白处使菜单项变灰无效，当然也可以不显示菜单，看你的需要了
	//if (pNMItemActivate->iItem != -1)
	//	pPopup->EnableMenuItem(ID_DELETE, MF_ENABLED);
	//else
	//	pPopup->EnableMenuItem(ID_DELETE, MF_GRAYED);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, pParent);
}

void SwitchPopMenuCheck(CMenu& menu, UINT nItemID)
{
	CMenu * pPopup = menu.GetSubMenu(0);
	if ( pPopup == NULL ) return;
	UINT state = pPopup->GetMenuState(nItemID, MF_BYCOMMAND);
	if ( state & MF_CHECKED )
		pPopup->CheckMenuItem(nItemID, MF_UNCHECKED | MF_BYCOMMAND);
	else
		pPopup->CheckMenuItem(nItemID, MF_CHECKED | MF_BYCOMMAND);
}


HBITMAP CombineBitmap(vector<HBITMAP> hbmps)
{
	HDC hDC  = ::GetDC(NULL);
	BITMAP bm;
	HBITMAP hbmpOldSrc = NULL;
	HBITMAP hbmpOldCombined = NULL;

	CDC *pDC = CDC::FromHandle(hDC);
	CDC SrcDC;
	CDC OutDC;
	SrcDC.CreateCompatibleDC(pDC);
	OutDC.CreateCompatibleDC(pDC);

	if (0 == hbmps.size()) return NULL;

	::GetObject(hbmps[0], sizeof(BITMAP), &bm);
	int nCx = bm.bmWidth * (int)hbmps.size();
	int nCy = bm.bmHeight;
	HBITMAP hbmpCombined = ::CreateCompatibleBitmap(pDC->GetSafeHdc(), nCx, nCy);
	hbmpOldCombined = (HBITMAP)::SelectObject(OutDC.GetSafeHdc(), hbmpCombined);
	for (int i=0; i<(int)hbmps.size(); i++)
	{
		hbmpOldSrc = (HBITMAP)::SelectObject(SrcDC.GetSafeHdc(), hbmps[i]);
		::BitBlt(OutDC.GetSafeHdc(), bm.bmWidth * i, 0, bm.bmWidth, nCy
			, SrcDC.GetSafeHdc(), 0, 0, SRCCOPY);
		::SelectObject(SrcDC.GetSafeHdc(), hbmpOldSrc);
	}

	::SelectObject(OutDC.GetSafeHdc(), hbmpOldCombined);
	SrcDC.DeleteDC();
	OutDC.DeleteDC();
	::ReleaseDC(NULL, hDC);  
	return hbmpCombined;
}

/*
功能：
	当窗口需要处理 WM_MOUSEHOVER、WM_MOUSELEAVE 消息时，调用本函数。
参数：
	bTracked：窗口的成员变量，表示，当前窗口已经设置过跟踪，不用重复设置。
	hwnd：窗口句柄。
	dwHoverTime：悬停等待时间。
说明：
	在窗口的 ON_WM_MOUSEMOVE()消息 中，调用：
	SetMouseTrack( m_bMouseTracked, GetSafeHwnd() );

	添加以下消息影射：
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeaved)
	
	添加以下消息声明：
	afx_msg LRESULT OnMouseHover(WPARAM wp,LPARAM lp);
	afx_msg LRESULT OnMouseLeaved(WPARAM wp,LPARAM lp);
	
	重写消息实现：
	LRESULT CXXX::OnMouseHover(WPARAM wp,LPARAM lp) { return 0; }
	LRESULT CXXX::OnMouseHover(WPARAM wp,LPARAM lp) { return 0; }
*/
void SetMouseTrack( BOOL& bTracked, HWND hwnd, DWORD dwHoverTime )
{
	if ( !IsWindow(hwnd) ) return;
	if ( bTracked ) return;
	
	bTracked = TRUE;

	TRACKMOUSEEVENT tme;
	memset( &tme, NULL, sizeof( TRACKMOUSEEVENT ) );
	tme.cbSize = sizeof( TRACKMOUSEEVENT );
	tme.dwFlags = TME_LEAVE|TME_HOVER;
	tme.hwndTrack = hwnd;
	tme.dwHoverTime = dwHoverTime;
	_TrackMouseEvent(&tme);	
}

CString GetNewGUID(void)
{
	char buf[64] = {0};
	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		_snprintf(
			buf, sizeof(buf)
			, "{%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}"
			, guid.Data1
			, guid.Data2
			, guid.Data3
			, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3]
		, guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]
		);
		return (char *)buf;
	}
	else
	{
		return "";
	}
}

CString GetNewEventNameWin7(void)
{
	CString strEventName = _T("Global\\");
	strEventName += GetNewGUID();
	return strEventName;
}

HANDLE GetEventWin7( LPCTSTR lpszEventName )
{
	CString strEventName = lpszEventName == NULL ? "" : lpszEventName;
	if ( strEventName.IsEmpty() ) return NULL;
	HANDLE hEvent = NULL;

	SECURITY_ATTRIBUTES sa;
	SECURITY_DESCRIPTOR sd;
	BYTE aclBuffer[1024] = {NULL};
	BYTE sidBuffer[100] = {NULL};
	PACL pacl = (PACL)&aclBuffer;
	PSID psid = (PSID)&sidBuffer;
	SID_NAME_USE snu;
	DWORD sidBufferSize = 100;
	DWORD domainBufferSize = 80;
	TCHAR domainBuffer[80] = {NULL};
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	InitializeAcl(pacl, 1024, ACL_REVISION);
	LookupAccountName(0, _T("everyone"), psid, &sidBufferSize, domainBuffer
		, &domainBufferSize, &snu);
	AddAccessAllowedAce(pacl, ACL_REVISION, GENERIC_ALL, psid);
	SetSecurityDescriptorDacl(&sd, TRUE, pacl, FALSE);
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = &sd;
	sa.bInheritHandle = FALSE;

	hEvent = ::CreateEvent(&sa
		, TRUE //是否为手动复位
		, FALSE //初始时是否为有信号状态
		, (LPCTSTR)strEventName); //事件名称;

	if ( hEvent != NULL )
	{
		return hEvent;
	}
	else
	{
		DWORD dwCode = GetLastError();
		CString str;
		str.Format("创建全局事件失败，错误代码为：%d", dwCode);
		::MessageBox( NULL, str, NULL, MB_OK|MB_ICONERROR );
		return NULL;
	}
}

BOOL CreateEventWin7(HANDLE& hEvent, CString& strEventName)
{
	hEvent = NULL;
	strEventName = GetNewEventNameWin7();
	hEvent = GetEventWin7( strEventName );

	if ( hEvent == NULL || strEventName.IsEmpty() )
	{
		::CloseHandle( hEvent ); hEvent = NULL; strEventName = ""; return FALSE;
	}
	return TRUE;
}


BOOL WaitEvent(HANDLE& hEvent, DWORD dwTimeout, BOOL bMsgBox)
{
	if (hEvent == NULL) return TRUE;

//#ifdef _DEBUG
//	dwTimeout = INFINITE;
//#endif

	DWORD dwCode = ::WaitForSingleObject(hEvent, dwTimeout);

	switch (dwCode)
	{
	case WAIT_OBJECT_0:
		//	::CloseHandle(hEvent);
		//	hEvent = NULL;
		return TRUE;

	case WAIT_TIMEOUT:
		if (bMsgBox) ::MessageBox( NULL, "WaitEvent WAIT_TIMEOUT", NULL, MB_OK|MB_ICONINFORMATION );
		//	::CloseHandle(hEvent);
		//	hEvent = NULL;
		return FALSE;

	case WAIT_FAILED:
		if (bMsgBox) ::MessageBox(NULL,"WaitEvent WAIT_FAILED", NULL, MB_OK|MB_ICONINFORMATION);
		//	::CloseHandle(hEvent);
		//	hEvent = NULL;
		return FALSE;

	default:
		if (bMsgBox) ::MessageBox(NULL,"WaitEvent default", NULL, MB_OK|MB_ICONINFORMATION);
		//	::CloseHandle(hEvent);
		//	hEvent = NULL;
		return FALSE;
	}
}

int SendCopyData(HWND hWnd, DWORD dwMessageId, LPVOID lpData, DWORD dwDataSize)
{
	if (!IsWindow(hWnd)) return 0;

	COPYDATASTRUCT cds;
	memset(&cds, NULL, sizeof(cds));
	cds.cbData = dwDataSize;
	cds.dwData = dwMessageId;
	cds.lpData = lpData;
	return (int)::SendMessage(hWnd, WM_COPYDATA, NULL, (LPARAM)&cds);
}

BOOL AttachWindow( HWND hwndParent, HWND hwndChild, BOOL bResize )
{
	if ( !IsWindow( hwndParent ) ) return FALSE;
	if ( !IsWindow( hwndChild ) ) return FALSE;

	//取得原始风格
	DWORD dwExStyleParent = ::GetWindowLong(hwndParent, GWL_EXSTYLE);
	DWORD dwStyleParent = ::GetWindowLong(hwndParent, GWL_STYLE);
	DWORD dwExStyleChild = ::GetWindowLong(hwndChild, GWL_EXSTYLE);
	DWORD dwStyleChild = ::GetWindowLong(hwndChild, GWL_STYLE);

	//修改父窗口-------------------------
	
	//添加-----------------
	
	dwExStyleParent;

	dwStyleParent |= WS_VISIBLE;
	dwStyleParent |= WS_CLIPSIBLINGS;
	dwStyleParent |= WS_CLIPCHILDREN;

	//移除----------------------

	dwExStyleParent;

	dwStyleParent;

	//修改子窗口---------------------

	//添加--------------------
	
	//dwExStyleChild |= WS_EX_TOOLWINDOW; //移走程序的任务栏图标

	dwStyleChild |= WS_CHILD; //如果嵌入的是wpf的程序，必须设置此属性，否则，会导致嵌入的wpf程序在切换tab标签页时，不显示，需要手工点击一次页面才显示。
	dwStyleChild |= WS_VISIBLE;
	dwStyleChild |= WS_CLIPSIBLINGS;
	dwStyleChild |= WS_CLIPCHILDREN;

	//移除---------------

	dwExStyleChild;

	dwStyleChild &= ~WS_CAPTION;
	dwStyleChild &= ~WS_POPUP;
	dwStyleChild &= ~WS_OVERLAPPED;
	dwStyleChild &= ~WS_THICKFRAME;
	dwStyleChild &= ~WS_DLGFRAME;
	dwStyleChild &= ~WS_BORDER; //边框
	dwStyleChild &= ~WS_SYSMENU;
	dwStyleChild &= ~WS_MINIMIZE;
	dwStyleChild &= ~WS_MAXIMIZE;
	dwStyleChild &= ~WS_MINIMIZEBOX;
	dwStyleChild &= ~WS_MAXIMIZEBOX;

	//设置父子关系和窗口风格
	::SetWindowLong(hwndParent, GWL_EXSTYLE, dwExStyleParent);
	::SetWindowLong(hwndParent, GWL_STYLE, dwStyleParent);
	::SetWindowLong(hwndChild, GWL_EXSTYLE, dwExStyleChild);
	::SetWindowLong(hwndChild, GWL_STYLE, dwStyleChild);
	::SetParent(hwndChild, hwndParent);

	CRect rcClient;
	if ( bResize )
	{
		::GetClientRect(hwndParent, &rcClient);
	}
	else
	{
		::GetClientRect(hwndChild, &rcClient);
	}

	::SetWindowPos(hwndChild, NULL
		, 0, 0, rcClient.Width(), rcClient.Height()
		, SWP_NOZORDER);

	::MoveWindow(hwndChild, 0, 0
		, rcClient.Width(), rcClient.Height(), TRUE);

	return TRUE;
}

BOOL AttachWindowTestWebContainer( HWND hwndParent, HWND hwndChild, BOOL bResize )
{
	if ( !IsWindow( hwndParent ) ) return FALSE;
	if ( !IsWindow( hwndChild ) ) return FALSE;

	//取得原始风格
	DWORD dwExStyleParent = ::GetWindowLong(hwndParent, GWL_EXSTYLE);
	DWORD dwStyleParent = ::GetWindowLong(hwndParent, GWL_STYLE);
	DWORD dwExStyleChild = ::GetWindowLong(hwndChild, GWL_EXSTYLE);
	DWORD dwStyleChild = ::GetWindowLong(hwndChild, GWL_STYLE);

	//修改父窗口-------------------------

	//添加-----------------

	dwExStyleParent;

	dwStyleParent |= WS_VISIBLE;
	dwStyleParent |= WS_CLIPSIBLINGS;
	dwStyleParent |= WS_CLIPCHILDREN;

	//移除----------------------

	dwExStyleParent;

	dwStyleParent;

	//修改子窗口---------------------

	//添加--------------------

	//dwExStyleChild |= WS_EX_TOOLWINDOW; //移走程序的任务栏图标

	dwStyleChild |= WS_CHILD; //如果嵌入的是wpf的程序，必须设置此属性，否则，会导致嵌入的wpf程序在切换tab标签页时，不显示，需要手工点击一次页面才显示。
	//dwStyleChild |= WS_VISIBLE;
	dwStyleChild |= WS_CLIPSIBLINGS;
	dwStyleChild |= WS_CLIPCHILDREN;

	//移除---------------

	dwExStyleChild;

	dwStyleChild &= ~WS_CAPTION;
	dwStyleChild &= ~WS_POPUP;
	dwStyleChild &= ~WS_OVERLAPPED;
	dwStyleChild &= ~WS_THICKFRAME;
	dwStyleChild &= ~WS_DLGFRAME;
	dwStyleChild &= ~WS_BORDER; //边框
	dwStyleChild &= ~WS_SYSMENU;
	dwStyleChild &= ~WS_MINIMIZE;
	dwStyleChild &= ~WS_MAXIMIZE;
	dwStyleChild &= ~WS_MINIMIZEBOX;
	dwStyleChild &= ~WS_MAXIMIZEBOX;

	//设置父子关系和窗口风格
	::SetWindowLong(hwndParent, GWL_EXSTYLE, dwExStyleParent);
	::SetWindowLong(hwndParent, GWL_STYLE, dwStyleParent);
	::SetWindowLong(hwndChild, GWL_EXSTYLE, dwExStyleChild);
	::SetWindowLong(hwndChild, GWL_STYLE, dwStyleChild);
	::SetParent(hwndChild, hwndParent);

	if ( bResize )
	{
		CRect rcClient;
		::GetClientRect(hwndParent, &rcClient);

		::SetWindowPos(hwndChild, NULL
			, 0, 0, rcClient.Width(), rcClient.Height()
			, SWP_NOZORDER|SWP_FRAMECHANGED|SWP_SHOWWINDOW);

		::MoveWindow(hwndChild, 0, 0
			, rcClient.Width(), rcClient.Height(), TRUE);
	}

	return TRUE;
}
BOOL AddDropFileAuthorization()
{
	OSVERSIONINFO   osversioninfo;
	osversioninfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osversioninfo);

	if(osversioninfo.dwMajorVersion < 6) // 系统版本低于vista,直接返回true
		return TRUE;

	typedef BOOL(WINAPI*pfnChangeWindowMessageFilter)(UINT,DWORD);
	//动态加载User32．dll
	HMODULE hLib = ::LoadLibrary("User32.dll");
	if (hLib == NULL)
		return FALSE;

	pfnChangeWindowMessageFilter pfun = (pfnChangeWindowMessageFilter)::GetProcAddress(
		hLib,"ChangeWindowMessageFilter");

	if (NULL == pfun)
	{
		FreeLibrary(hLib);
		return FALSE;
	}
	if (!pfun(0x0049,1 /*MSGFLT_ADD*/))
	{
		FreeLibrary(hLib);
		return FALSE;
	}

	if (!pfun(WM_DROPFILES,1 /*MSGFLT_ADD*/))
	{
		FreeLibrary(hLib);
		return FALSE;
	}

	return TRUE;
}