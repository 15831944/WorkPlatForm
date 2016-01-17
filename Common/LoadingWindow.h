#pragma once
#include <AfxDialogEx.h>
#include "GDIPlusInit.h"

/*
使用说明：

使用前，请先确定当前实例进程成功初始化 Gdiplus。
可以给 CXXXApp 添加 CGDIPlusInit 基类，然后在 CXXXApp::InitInstance 中，添加以下调用初始化：
InitGDIPlus();

注意事项：
请尽量确保主线程不被阻塞，当主线程被阻塞时，无法正常显示。

*/

class CLoadingWindow : public CDialogEx
{
	DECLARE_DYNAMIC(CLoadingWindow)

public:
	CLoadingWindow( UINT nTimerId, UINT nWindowId, CWnd* pParent );
	virtual ~CLoadingWindow();

	/*
	初始化：
	lpszFrontImagePath：	图片路径，含一组Loading状态。
	dwFrameCount：			Loading状态个数。
	byteBkGray：			背景色灰度值（0 - 255，0：黑色，255：白色）。
	byteTransparent：		透明值（0 - 255）
	dwDelay：				切换Loading状态的间隔。
	dwZoom：				缩放比例（0 - 100）
	*/
	BOOL Init( LPCTSTR lpszFrontImagePath, DWORD dwFrameCount = 12
		, BYTE byteBkGray = 250, BYTE byteTransparent = 128, DWORD dwDelay = 90, DWORD dwZoom = 100 );
	BOOL Release();
	BOOL Show();
	BOOL Hide();

protected:
	BOOL DrawFrontImage( void );
	void NextFrame();
	BOOL ResetWindowPos( CRect& rcWnd );
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
	
public:
	BOOL m_bCanceled;
protected:
	UINT m_nTimerId;
	Gdiplus::Image* m_pFrontImage;
	DWORD m_dwFrontImageFrameCount;
	DWORD m_dwCurFrame;
	DWORD m_dwDelay;
	DWORD m_dwZoom;
	BLENDFUNCTION m_Blend;
	Gdiplus::Brush * m_pBkBr;
	CRect m_rcWnd;
	BOOL  m_bVisable;
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

extern CLoadingWindow* g_pLoadingWindow;