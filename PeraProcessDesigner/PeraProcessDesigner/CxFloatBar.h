#pragma once
#include "BCGPDockingControlBar.h"

//typedef struct _FloatBarData
//{
//	static const DWORD DEFAULT_STYLE = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS 
//		| WS_CLIPCHILDREN | CBRS_FLOAT_MULTI |  CBRS_SIZE_FIXED;
//
//	void Clear(void);
//	UINT m_nId; //窗口ID
//	CString m_strTitle; //标题
//	HWND m_hwndInner;
//	CRect m_rcBar; //窗口区域
//	DWORD m_dwStyle;
//	DWORD m_dwTabbedStyle;
//	DWORD m_dwBCGStyle;
//	DWORD m_dwAlignment;
//	DWORD m_dwDockStyle;
//	
//} FLOAT_BAR_DATA;
 
class CCxFloatBar : public CBCGPDockingControlBar
{
	DECLARE_DYNCREATE(CCxFloatBar)
public:
	CCxFloatBar(void);
	virtual ~CCxFloatBar(void);
	virtual void SetCaptionButtons ();
	//排除 HTCLOSE_BCG、HTMAXBUTTON、HTMINBUTTON 以外的 标题栏按钮按下。
	virtual void OnPressButtons (UINT nHit);
	//关闭按钮按下
	virtual void OnPressCloseButton ();
	//virtual BOOL Create(UINT Id, LPCTSTR lpszTitle, CWnd * pParentWnd
	//	, DWORD dwAlignment = CBRS_ALIGN_LEFT
	//	, DWORD dwDockStyle = CBRS_ALIGN_LEFT
	//	, DWORD dwLocation = CBRS_ALIGN_LEFT
	//	, int nLeft = 0, int nTop = 0, int nWidth = 200, int nHeight = 768
	//	, HWND hwndInner = NULL
	//	, DWORD dwTabbedStyle = CBRS_BCGP_REGULAR_TABS
	//	, DWORD dwStyle = DEFAULT_STYLE
	//	, DWORD dwBCGStyle = dwDefaultBCGDockingBarStyle);
	//virtual BOOL OnCreateInnerWindow(void);

	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

public:
	//virtual CBCGPAutoHideToolBar* SetAutoHideMode (BOOL bMode, DWORD dwAlignment, 
	//	CBCGPAutoHideToolBar* pCurrAutoHideBar = NULL, 
	//	BOOL bUseTimer = TRUE);
	//virtual BOOL OnShowControlBarMenu (CPoint point);
	//virtual void DrawCaption (CDC* pDC, CRect rectCaption);
protected:
	afx_msg void OnNcPaint();


protected:
	virtual void DrawCaption (CDC* pDC, CRect rectCaption);
	//virtual void OnPressCloseButton ();

	//virtual void EnableGripper( BOOL bEnable );

//	virtual void SetCaptionButtons();
//	afx_msg BOOL OnNeedTipText(UINT id, NMHDR* pNMH, LRESULT* pResult);

//	virtual void OnPressButtons( UINT nHit);

public:
	//afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	//afx_msg void OnNcPaint();
};
