#pragma once
#include "StlHeader.h"

extern BOOL GetChannelUrl(CString &strChannelUrl, const CString &strChannelName);
extern BOOL GetWebServiceUrl(CString &strUrl, const CString &strName);
extern void AttachTicket(CString& strUrl, CString strTicket);
extern void AttachUrlParam(CString& strUrl, CString strParam);
extern void ShowPopMenu( UINT nPopMenuID, CWnd * pParent );
extern HBITMAP CombineBitmap(vector<HBITMAP> hbmps);
extern void SwitchPopMenuCheck(CMenu& menu, UINT nItemID);

extern void SetMouseTrack( BOOL& bTracked, HWND hwnd, DWORD dwHoverTime = 500 );
extern CString GetNewGUID(void);
extern CString GetNewEventNameWin7(void);
extern BOOL CreateEventWin7(HANDLE& hEvent, CString& strEventName);
extern HANDLE GetEventWin7( LPCTSTR lpszEventName );
extern BOOL WaitEvent(HANDLE& hEvent, DWORD dwTimeout, BOOL bMsgBox);
extern int SendCopyData(HWND hWnd, DWORD dwMessageId, LPVOID lpData, DWORD dwDataSize);
extern BOOL AttachWindow( HWND hwndParent, HWND hwndChild, BOOL bResize = TRUE );
extern BOOL AttachWindowTestWebContainer( HWND hwndParent, HWND hwndChild, BOOL bResize = TRUE );
extern BOOL AddDropFileAuthorization();