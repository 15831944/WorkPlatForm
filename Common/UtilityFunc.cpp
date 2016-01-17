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


// �����Ҽ��˵� ////////////////////////////////////////////////////////////////////////
void ShowPopMenu( UINT nPopMenuID, CWnd * pParent )
{
	if (pParent == NULL) return;
	if ( !IsWindow( pParent->GetSafeHwnd() ) )return;

	POINT pt;
	GetCursorPos(&pt);
	CMenu menu;
	VERIFY(menu.LoadMenu(nPopMenuID));
	CMenu * pPopup = menu.GetSubMenu(0);
	// ������ǲ˵��ڲ˵���Դ���е�λ��
	ASSERT(pPopup != NULL);
	// ���Ʋ˵�״̬�������հ״�ʹ�˵�������Ч����ȻҲ���Բ���ʾ�˵����������Ҫ��
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
���ܣ�
	��������Ҫ���� WM_MOUSEHOVER��WM_MOUSELEAVE ��Ϣʱ�����ñ�������
������
	bTracked�����ڵĳ�Ա��������ʾ����ǰ�����Ѿ����ù����٣������ظ����á�
	hwnd�����ھ����
	dwHoverTime����ͣ�ȴ�ʱ�䡣
˵����
	�ڴ��ڵ� ON_WM_MOUSEMOVE()��Ϣ �У����ã�
	SetMouseTrack( m_bMouseTracked, GetSafeHwnd() );

	���������ϢӰ�䣺
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeaved)
	
	���������Ϣ������
	afx_msg LRESULT OnMouseHover(WPARAM wp,LPARAM lp);
	afx_msg LRESULT OnMouseLeaved(WPARAM wp,LPARAM lp);
	
	��д��Ϣʵ�֣�
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
		, TRUE //�Ƿ�Ϊ�ֶ���λ
		, FALSE //��ʼʱ�Ƿ�Ϊ���ź�״̬
		, (LPCTSTR)strEventName); //�¼�����;

	if ( hEvent != NULL )
	{
		return hEvent;
	}
	else
	{
		DWORD dwCode = GetLastError();
		CString str;
		str.Format("����ȫ���¼�ʧ�ܣ��������Ϊ��%d", dwCode);
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

	//ȡ��ԭʼ���
	DWORD dwExStyleParent = ::GetWindowLong(hwndParent, GWL_EXSTYLE);
	DWORD dwStyleParent = ::GetWindowLong(hwndParent, GWL_STYLE);
	DWORD dwExStyleChild = ::GetWindowLong(hwndChild, GWL_EXSTYLE);
	DWORD dwStyleChild = ::GetWindowLong(hwndChild, GWL_STYLE);

	//�޸ĸ�����-------------------------
	
	//���-----------------
	
	dwExStyleParent;

	dwStyleParent |= WS_VISIBLE;
	dwStyleParent |= WS_CLIPSIBLINGS;
	dwStyleParent |= WS_CLIPCHILDREN;

	//�Ƴ�----------------------

	dwExStyleParent;

	dwStyleParent;

	//�޸��Ӵ���---------------------

	//���--------------------
	
	//dwExStyleChild |= WS_EX_TOOLWINDOW; //���߳����������ͼ��

	dwStyleChild |= WS_CHILD; //���Ƕ�����wpf�ĳ��򣬱������ô����ԣ����򣬻ᵼ��Ƕ���wpf�������л�tab��ǩҳʱ������ʾ����Ҫ�ֹ����һ��ҳ�����ʾ��
	dwStyleChild |= WS_VISIBLE;
	dwStyleChild |= WS_CLIPSIBLINGS;
	dwStyleChild |= WS_CLIPCHILDREN;

	//�Ƴ�---------------

	dwExStyleChild;

	dwStyleChild &= ~WS_CAPTION;
	dwStyleChild &= ~WS_POPUP;
	dwStyleChild &= ~WS_OVERLAPPED;
	dwStyleChild &= ~WS_THICKFRAME;
	dwStyleChild &= ~WS_DLGFRAME;
	dwStyleChild &= ~WS_BORDER; //�߿�
	dwStyleChild &= ~WS_SYSMENU;
	dwStyleChild &= ~WS_MINIMIZE;
	dwStyleChild &= ~WS_MAXIMIZE;
	dwStyleChild &= ~WS_MINIMIZEBOX;
	dwStyleChild &= ~WS_MAXIMIZEBOX;

	//���ø��ӹ�ϵ�ʹ��ڷ��
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

	//ȡ��ԭʼ���
	DWORD dwExStyleParent = ::GetWindowLong(hwndParent, GWL_EXSTYLE);
	DWORD dwStyleParent = ::GetWindowLong(hwndParent, GWL_STYLE);
	DWORD dwExStyleChild = ::GetWindowLong(hwndChild, GWL_EXSTYLE);
	DWORD dwStyleChild = ::GetWindowLong(hwndChild, GWL_STYLE);

	//�޸ĸ�����-------------------------

	//���-----------------

	dwExStyleParent;

	dwStyleParent |= WS_VISIBLE;
	dwStyleParent |= WS_CLIPSIBLINGS;
	dwStyleParent |= WS_CLIPCHILDREN;

	//�Ƴ�----------------------

	dwExStyleParent;

	dwStyleParent;

	//�޸��Ӵ���---------------------

	//���--------------------

	//dwExStyleChild |= WS_EX_TOOLWINDOW; //���߳����������ͼ��

	dwStyleChild |= WS_CHILD; //���Ƕ�����wpf�ĳ��򣬱������ô����ԣ����򣬻ᵼ��Ƕ���wpf�������л�tab��ǩҳʱ������ʾ����Ҫ�ֹ����һ��ҳ�����ʾ��
	//dwStyleChild |= WS_VISIBLE;
	dwStyleChild |= WS_CLIPSIBLINGS;
	dwStyleChild |= WS_CLIPCHILDREN;

	//�Ƴ�---------------

	dwExStyleChild;

	dwStyleChild &= ~WS_CAPTION;
	dwStyleChild &= ~WS_POPUP;
	dwStyleChild &= ~WS_OVERLAPPED;
	dwStyleChild &= ~WS_THICKFRAME;
	dwStyleChild &= ~WS_DLGFRAME;
	dwStyleChild &= ~WS_BORDER; //�߿�
	dwStyleChild &= ~WS_SYSMENU;
	dwStyleChild &= ~WS_MINIMIZE;
	dwStyleChild &= ~WS_MAXIMIZE;
	dwStyleChild &= ~WS_MINIMIZEBOX;
	dwStyleChild &= ~WS_MAXIMIZEBOX;

	//���ø��ӹ�ϵ�ʹ��ڷ��
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

	if(osversioninfo.dwMajorVersion < 6) // ϵͳ�汾����vista,ֱ�ӷ���true
		return TRUE;

	typedef BOOL(WINAPI*pfnChangeWindowMessageFilter)(UINT,DWORD);
	//��̬����User32��dll
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