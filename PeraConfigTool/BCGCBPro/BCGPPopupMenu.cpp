//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a part of the BCGControlBar Library
// Copyright (C) 1998-2011 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************

// BCGPPopupMenu.cpp : implementation file
//

#include "stdafx.h"

#pragma warning (disable : 4706)

#ifdef _AFXDLL
#define COMPILE_MULTIMON_STUBS
#endif // _AFXDLL

#include "multimon.h"

#pragma warning (default : 4706)

#include "BCGPPopupMenu.h"
#include "BCGPMenuBar.h"
#include "BCGGlobals.h"
#include "BCGPToolbarMenuButton.h"
#include "BCGPMDIFrameWnd.h"
#include "BCGPFrameWnd.h"
#include "BCGPOleIPFrameWnd.h"
#include "BCGPOleDocIPFrameWnd.h"
#include "BCGPMenuBar.h"
#include "BCGPMenuPage.h"
#include "MenuHash.h"
#include "MenuImages.h"
#include "BCGPShowAllButton.h"
#include "BCGPUserToolsManager.h"
#include "BCGPTearOffManager.h"
#include "BCGPUserTool.h"
#include "BCGPSound.h"
#include "bcgprores.h"
#include "BCGPLocalResource.h"
#include "BCGPDialog.h"
#include "BCGPPropertyPage.h"
#include "BCGPVisualManager.h"
#include "BCGPDrawManager.h"
#include "BCGPCommandManager.h"
#include "BCGPControlBar.h"
#include "BCGPMiniFrameWnd.h"
#include "BCGPCustomizeMenuButton.h"
#include "BCGPCaptionMenuButton.h"
#include "BCGPAccessibility.h"
#include "BCGPBaseRibbonElement.h"
#include "BCGPRibbonFloaty.h"
#include "BCGPControlRenderer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBCGPShadowWnd

class CBCGPShadowWnd : public CMiniFrameWnd
{
	friend class CBCGPPopupMenu;

	CBCGPShadowWnd(CBCGPPopupMenu* pOwner, int nOffset)
	{
		m_pOwner = pOwner;
		m_nOffset = nOffset;
		m_bIsRTL = FALSE;
	}

	~CBCGPShadowWnd()
	{
	}

	void Repos();
	void UpdateTransparency (BYTE nTransparency);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBCGPShadowWnd)
	public:
	virtual BOOL Create();
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CBCGPShadowWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void DrawShadow ();	

	CBCGPPopupMenu*		m_pOwner;
	int					m_nOffset;
	CBCGPShadowRenderer m_Shadow;
	BOOL				m_bIsRTL;
	BYTE				m_nTransparency;
};

BEGIN_MESSAGE_MAP(CBCGPShadowWnd, CMiniFrameWnd)
	//{{AFX_MSG_MAP(CBCGPShadowWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CBCGPShadowWnd::Create ()
{
	ASSERT_VALID (m_pOwner);

	if (!globalData.IsWindowsLayerSupportAvailable () || globalData.m_nBitsPerPixel <= 8)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	CString strClassName = ::AfxRegisterWndClass (
			CS_SAVEBITS,
			::LoadCursor(NULL, IDC_ARROW),
			(HBRUSH)(COLOR_BTNFACE + 1), NULL);

	CRect rectDummy (0, 0, 0, 0);

	if (m_pOwner->GetExStyle() & WS_EX_LAYOUTRTL)
	{
		m_bIsRTL = TRUE;
	}

	if (!CMiniFrameWnd::CreateEx (WS_EX_TOOLWINDOW | WS_EX_LAYERED, strClassName, _T(""), WS_POPUP, rectDummy, m_pOwner->GetParent ()))
	{
		return FALSE;
	}

	m_nTransparency = 255;

	m_Shadow.Create (m_nOffset, 
		CBCGPVisualManager::GetInstance ()->GetMenuShadowBaseColor (),
		0, 35, FALSE);

	return TRUE;
}

BOOL CBCGPShadowWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CBCGPShadowWnd::DrawShadow ()
{
	CRect rect;
	GetWindowRect (rect);

	if (rect.Width () == 0 || rect.Height () == 0)
	{
		return;
	}

	CPoint point (0, 0);
	CSize size (rect.Size ());

	LPBYTE pBits = NULL;
	HBITMAP hBitmap = CBCGPDrawManager::CreateBitmap_32 (size, (void**)&pBits);
	if (hBitmap == NULL)
	{
		return;
	}

	CBitmap bitmap;
	bitmap.Attach (hBitmap);

	CClientDC clientDC(this);
	CDC dc;
	dc.CreateCompatibleDC (&clientDC);

	CBitmap* pBitmapOld = (CBitmap*)dc.SelectObject (&bitmap);

	m_Shadow.Draw (&dc, CRect (point, size), 0, m_nTransparency);

	BLENDFUNCTION bf;
	bf.BlendOp             = AC_SRC_OVER;
	bf.BlendFlags          = 0;
	bf.SourceConstantAlpha = 255;
	bf.AlphaFormat         = LWA_COLORKEY;

	globalData.UpdateLayeredWindow (GetSafeHwnd (), NULL, 0, &size, dc.GetSafeHdc (), 
		&point, 0, &bf, 0x02);

	dc.SelectObject (pBitmapOld);
}

void CBCGPShadowWnd::OnSize(UINT nType, int cx, int cy)
{
	CMiniFrameWnd::OnSize (nType, cx, cy);

	if (cx == 0 || cy == 0)
	{
		return;
	}

	DrawShadow ();
}

void CBCGPShadowWnd::UpdateTransparency (BYTE nTransparency)
{
	if (m_nTransparency != nTransparency)
	{
		m_nTransparency = nTransparency;

		DrawShadow ();
	}
}

void CBCGPShadowWnd::Repos() 
{
	ASSERT_VALID (m_pOwner);

	CRect rectWindow;
	m_pOwner->GetWindowRect (rectWindow);

	rectWindow.OffsetRect (m_bIsRTL ? -m_nOffset : m_nOffset, m_nOffset);

	SetRedraw (FALSE);

	if (!IsWindowVisible ())
	{
		ShowWindow (SW_SHOWNOACTIVATE);

		SetWindowPos (&CWnd::wndTop, rectWindow.left, rectWindow.top, rectWindow.Width (), rectWindow.Height (),
			SWP_NOACTIVATE);

		m_pOwner->SetWindowPos (&CWnd::wndTop, -1, -1, -1, -1,
			SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	}
	else
	{
		SetWindowPos (NULL, rectWindow.left, rectWindow.top, rectWindow.Width (), rectWindow.Height (),
			SWP_NOACTIVATE | SWP_NOZORDER);
	}

	SetRedraw ();
	RedrawWindow ();
}


/////////////////////////////////////////////////////////////////////////////
// CBCGPPopupMenu

static const int iFadeStep = 10;
static const int iAnimTimerId = 1;
static const int iScrollTimerId = 2;
static const int iScrollTimerDuration = 80;
static const int iMenuBarId = 1;
static const int iTearOffBarHeight = 10;
static const int iResizeBarBarHeightRight = 12;
static const int iResizeBarBarHeight = 9;
static const int nScrollBarID = 1;

CBCGPPopupMenu::ANIMATION_TYPE CBCGPPopupMenu::m_AnimationType = NO_ANIMATION;
UINT CBCGPPopupMenu::m_AnimationSpeed = 15;
CBCGPPopupMenu* CBCGPPopupMenu::m_pActivePopupMenu = NULL;
BOOL CBCGPPopupMenu::m_bForceShadow = TRUE;
BOOL CBCGPPopupMenu::m_bForceMenuFocus = TRUE;
BOOL CBCGPPopupMenu::m_bMenuSound = TRUE;
BOOL CBCGPPopupMenu::m_bAlwaysShowEmptyToolsEntry = FALSE;
BOOL CBCGPPopupMenu::m_bSendMenuSelectMsg = FALSE;
int	CBCGPPopupMenu::m_nMinWidth = 0;
BOOL CBCGPPopupMenu::m_bForceRTL = FALSE;

static clock_t nLastAnimTime = 0;

IMPLEMENT_SERIAL(CBCGPPopupMenu, CMiniFrameWnd, VERSIONABLE_SCHEMA | 1)

CBCGPPopupMenu::CBCGPPopupMenu() :
	m_pMenuCustomizationPage (NULL)
{
	Initialize ();
}
//****************************************************************************************
CBCGPPopupMenu::CBCGPPopupMenu(CBCGPMenuPage* pCustPage, LPCTSTR lpszTitle) :
	m_pMenuCustomizationPage (pCustPage),
	m_strCaption (lpszTitle)
{
	Initialize ();
}
//****************************************************************************************
void CBCGPPopupMenu::Initialize ()
{
	if (globalData.bIsRemoteSession)
	{
		m_AnimationType = NO_ANIMATION;
	}

	if (globalData.bIsWindows9x || globalData.bIsWindowsNT4)
	{
		CBCGPMenuBar::EnableMenuShadows (FALSE);
	}

	m_hMenu = NULL;
	m_ptLocation = CPoint (0, 0);
	m_ptLocationInitial = CPoint (0, 0);
	m_pParentBtn = NULL;
	m_bAutoDestroyParent = TRUE;
	m_bAutoDestroy = TRUE;
	m_FinalSize = CSize (0, 0);
	m_AnimSize = CSize (0, 0);
	m_nMenuBarHeight = 0;
	m_bAnimationIsDone = (GetAnimationType () == NO_ANIMATION);
	m_bDisableAnimation = FALSE;
	m_bScrollable = FALSE;
	m_bShowScrollBar = FALSE;
	m_nMaxHeight = -1;
	m_bTobeDstroyed = FALSE;
	m_bShown = FALSE;

	m_iMaxWidth = -1;
	m_iLogoWidth = 0;
	m_nLogoLocation = MENU_LOGO_LEFT;

	m_rectScrollUp.SetRectEmpty ();
	m_rectScrollDn.SetRectEmpty ();

	m_iScrollBtnHeight = CBCGPMenuImages::Size ().cy + 2 * 
		CBCGPVisualManager::GetInstance ()->GetPopupMenuBorderSize ();
	m_iScrollMode = 0;

	m_bIsAnimRight = TRUE;
	m_bIsAnimDown = TRUE;

	m_iShadowSize = CBCGPMenuBar::IsMenuShadows () &&
					!CBCGPToolBar::IsCustomizeMode () &&
					globalData.m_nBitsPerPixel > 8 ? // Don't draw shadows in 256 colors or less
						CBCGPVisualManager::GetInstance ()->GetMenuShadowDepth () : 0;

	m_iFadePercent = 0;
	if (GetAnimationType () == FADE && globalData.m_nBitsPerPixel <= 8)
	{
		m_AnimationType = NO_ANIMATION;
		m_bAnimationIsDone = TRUE;
	}

	m_bTearOffTracking = FALSE;
	m_bIsTearOffCaptionActive = FALSE;
	m_rectTearOffCaption.SetRectEmpty ();

	m_DropDirection = DROP_DIRECTION_NONE;

	m_pMessageWnd = NULL;
	m_bTrackMode = FALSE;
	m_bRightAlign = FALSE;
	m_bQuickCusomize = FALSE;
	m_QuickType = QUICK_CUSTOMIZE_NONE;
	m_bEscClose   = FALSE;

	m_pParentRibbonElement = NULL;
	m_hwndConnectedFloaty = NULL;
	m_nLastCommandID = 0;

	m_bIsResizable = FALSE;
	m_sizeMinResize = CSize (0, 0);
	m_rectResize.SetRectEmpty ();
	m_bResizeTracking = FALSE;
	m_bWasResized = FALSE;
	m_bIsResizeBarOnTop = FALSE;
	m_sizeCurrent = CSize (0, 0);
	m_bHasBeenResized = FALSE;
	m_pWndShadow = NULL;
}
//****************************************************************************************
CBCGPPopupMenu::~CBCGPPopupMenu()
{
#ifndef BCGP_EXCLUDE_RIBBON
	if (m_pParentRibbonElement != NULL)
	{
		ASSERT_VALID (m_pParentRibbonElement);
		m_pParentRibbonElement->SetDroppedDown (NULL);
	}
#endif

	if (m_bAutoDestroy && m_hMenu != NULL)
	{
		::DestroyMenu (m_hMenu);
	}

	if (m_hwndConnectedFloaty != NULL && ::IsWindow (m_hwndConnectedFloaty))
	{
		::SendMessage (m_hwndConnectedFloaty, WM_CLOSE, 0, 0);
	}
}

BEGIN_MESSAGE_MAP(CBCGPPopupMenu, CMiniFrameWnd)
	//{{AFX_MSG_MAP(CBCGPPopupMenu)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_MOUSEACTIVATE()
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SHOWWINDOW()
	ON_WM_SETCURSOR()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_NCHITTEST()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATEAPP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCGPPopupMenu message handlers

BOOL CBCGPPopupMenu::Create (CWnd* pWndParent, int x, int y, HMENU hMenu, BOOL bLocked, BOOL bOwnMessage)
{
	BCGPlaySystemSound (BCGSOUND_MENU_POPUP);

	ASSERT (pWndParent != NULL);

	UINT nClassStyle = CS_SAVEBITS;

	CString strClassName = ::AfxRegisterWndClass (
			nClassStyle,
			::LoadCursor(NULL, IDC_ARROW),
			(HBRUSH)(COLOR_BTNFACE + 1), NULL);

	m_hMenu = hMenu;

	if (x == -1 && y == -1)	// Undefined position
	{
		if (pWndParent != NULL)
		{
			CRect rectParent;
			pWndParent->GetClientRect (&rectParent);
			pWndParent->ClientToScreen (&rectParent);

			m_ptLocation = CPoint (rectParent.left + 5, rectParent.top + 5);
		}
		else
		{
			m_ptLocation = CPoint (0, 0);
		}
	}
	else
	{
		m_ptLocation = CPoint (x, y);
	}

	m_ptLocationInitial = m_ptLocation;

	DWORD dwStyle = WS_POPUP;
	if (m_pMenuCustomizationPage != NULL)
	{
		dwStyle |= (WS_CAPTION | WS_SYSMENU);
	}

	if (pWndParent->GetSafeHwnd () != NULL &&
		(pWndParent->GetExStyle() & WS_EX_LAYOUTRTL))
	{
		m_bDisableAnimation = TRUE;
	}

	if (m_bDisableAnimation)
	{
		m_bAnimationIsDone = TRUE;
	}

	BOOL bIsAnimate = (GetAnimationType () != NO_ANIMATION) && 
					!CBCGPToolBar::IsCustomizeMode () &&
					!m_bDisableAnimation;

	CBCGPPopupMenu* pParentMenu = GetParentPopupMenu ();
	if (pParentMenu != NULL)
	{
		m_bTrackMode = pParentMenu->m_bTrackMode;
	}

	if (bOwnMessage)
	{
		m_pMessageWnd = pWndParent;
	}
	else if (pParentMenu != NULL)
	{
		m_pMessageWnd = pParentMenu->GetMessageWnd ();
	}


	CRect rect (x, y, x, y);
	
	DWORD dwExStyle = 0;

	if (m_bForceRTL || (pWndParent->GetExStyle() & WS_EX_LAYOUTRTL))
	{
		dwExStyle |= WS_EX_LAYOUTRTL;
	}

	BOOL bCreated = CMiniFrameWnd::CreateEx (
				dwExStyle,
				strClassName, m_strCaption,
				dwStyle, rect,
				pWndParent->GetOwner () == NULL ? 
					pWndParent : pWndParent->GetOwner ());
	if (!bCreated)
	{
		return FALSE;
	}

	if (m_bRightAlign)
	{
		m_ptLocation.x -= m_FinalSize.cx - 1;
		m_ptLocationInitial = m_ptLocation;

		RecalcLayout ();
	}

	CBCGPPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	pMenuBar->m_bLocked = bLocked;
	pMenuBar->m_bDropDownListMode = m_bShowScrollBar;

	if (bIsAnimate)
	{
		//--------------------------
		// Adjust initial menu size:
		//--------------------------
		m_AnimSize = m_FinalSize + CSize (m_iShadowSize, m_iShadowSize);

		switch (GetAnimationType ())
		{
		case UNFOLD:
			m_AnimSize.cx = pMenuBar->GetColumnWidth ();

		case SLIDE:
			m_AnimSize.cy = pMenuBar->GetRowHeight ();
			break;
		}

		if (pMenuBar->IsWindowVisible ())
		{
			pMenuBar->ShowWindow (SW_HIDE);
		}

		SetTimer (iAnimTimerId, m_AnimationSpeed, NULL);
		nLastAnimTime = clock ();
	}

	//--------------------------------
	// Update windows covered by menu:
	//--------------------------------
	UpdateBottomWindows ();
	if (m_iShadowSize == 0 && GetAnimationType () == FADE && bIsAnimate)
	{
		m_AnimSize = m_FinalSize;
	}

	SetWindowPos (&wndTop, -1, -1, -1, -1,
		SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

	if (CBCGPToolBar::IsCustomizeMode ())
	{
		pMenuBar->Invalidate ();
		pMenuBar->UpdateWindow ();
	}

	return TRUE;
}
//****************************************************************************************

//-----------------------------------------------------
// My "classic " trick - how I can access to protected
// member m_pRecentFileList?
//-----------------------------------------------------
class CBCGPApp : public CWinApp
{
	friend class CBCGPPopupMenu;
};

int CBCGPPopupMenu::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMiniFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	DWORD toolbarStyle = dwDefaultToolbarStyle;
	if (GetAnimationType () != NO_ANIMATION && !CBCGPToolBar::IsCustomizeMode () &&
		!m_bDisableAnimation)
	{
		toolbarStyle &= ~WS_VISIBLE;
	}

	CBCGPPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	pMenuBar->m_bTrackMode = m_bTrackMode;

	if (m_pParentBtn != NULL && m_pParentBtn->IsMenuPaletteMode ())
	{
		pMenuBar->m_bPaletteMode = TRUE;
		pMenuBar->m_bDisableSideBarInXPMode = TRUE;
		pMenuBar->m_bPaletteRows = m_pParentBtn->GetPaletteRows ();
	}

	if (!pMenuBar->Create (this, toolbarStyle | CBRS_TOOLTIPS | CBRS_FLYBY, iMenuBarId))
	{
		TRACE(_T("Can't create popup menu bar\n"));
		return FALSE;
	}

	CBCGPPopupMenu* pParentPopupMenu = GetParentPopupMenu ();
	if (pParentPopupMenu != NULL)
	{
		m_iMaxWidth = pParentPopupMenu->m_iMaxWidth;
	}

	pMenuBar->m_iMaxWidth = m_iMaxWidth;
	pMenuBar->m_iMinWidth = m_nMinWidth;
	pMenuBar->SetOwner (GetParent ());

	if (m_iShadowSize > 0 && globalData.IsWindowsLayerSupportAvailable () &&
		(m_pParentRibbonElement != NULL || IsFloaty ()))
	{
		m_pWndShadow = new CBCGPShadowWnd (this, m_iShadowSize);
		m_iShadowSize = 0;

		m_pWndShadow->Create ();
	}

    return InitMenuBar()? 0 : -1;
}
//****************************************************************************************
void CBCGPPopupMenu::OnSize(UINT nType, int cx, int cy) 
{
	CMiniFrameWnd::OnSize(nType, cx, cy);

	CBCGPPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	if (pMenuBar->m_bInUpdateShadow)
	{
		return;
	}
	
	if (pMenuBar->GetSafeHwnd () != NULL)
	{
		AdjustScroll (TRUE /*bForceMenuBarResize*/);
		SetScrollBar ();
	}

	const int nBorderSize = GetBorderSize ();

	if (m_iLogoWidth > 0)
	{
		CRect rectLogo;
		GetClientRect (rectLogo);

		switch (m_nLogoLocation)
		{
		case MENU_LOGO_LEFT:
			rectLogo.right = rectLogo.left + nBorderSize + m_iLogoWidth;
			break;

		case MENU_LOGO_RIGHT:
			rectLogo.left = rectLogo.right - nBorderSize - m_iLogoWidth;
			break;

		case MENU_LOGO_TOP:
			rectLogo.bottom = rectLogo.top + nBorderSize + m_iLogoWidth;
			break;

		case MENU_LOGO_BOTTOM:
			rectLogo.top = rectLogo.bottom - nBorderSize - m_iLogoWidth;
			break;
		}

		InvalidateRect (rectLogo);
		UpdateWindow ();
	}
}
//****************************************************************************************
void CBCGPPopupMenu::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if (!m_bAnimationIsDone && !CBCGPToolBar::IsCustomizeMode ())
	{
		DrawFade (&dc);
	}
	else
	{
		DoPaint (&dc);
    }
}
//****************************************************************************************
int CBCGPPopupMenu::OnMouseActivate(CWnd* /*pDesktopWnd*/, UINT /*nHitTest*/, UINT /*message*/) 
{
	return MA_NOACTIVATE;
}
//****************************************************************************************
void CBCGPPopupMenu::RecalcLayout (BOOL /*bNotify*/) 
{
#ifdef _DEBUG
	if (m_pParentBtn != NULL)
	{
		ASSERT_VALID (m_pParentBtn);
		ASSERT (m_pParentBtn->m_pPopupMenu == this);
	}

#ifndef BCGP_EXCLUDE_RIBBON
	if (m_pParentRibbonElement != NULL)
	{
		ASSERT_VALID (m_pParentRibbonElement);
		ASSERT (m_pParentRibbonElement->m_pPopupMenu == this);
	}
#endif
#endif // _DEBUG

	CBCGPPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	if (!::IsWindow (m_hWnd) ||
		pMenuBar == NULL ||
		!::IsWindow (pMenuBar->m_hWnd))
	{
		return;
	}

	if (pMenuBar->m_bInUpdateShadow)
	{
		return;
	}

	//-------------------------
	// Set tear-off attributes:
	//-------------------------
	BOOL bIsTearOff = (m_pParentBtn != NULL && m_pParentBtn->IsTearOffMenu () &&
						!CBCGPToolBar::IsCustomizeMode ());

	CRect rectScreen;

	MONITORINFO mi;
	mi.cbSize = sizeof (MONITORINFO);
	if (GetMonitorInfo (MonitorFromPoint (m_ptLocation, MONITOR_DEFAULTTONEAREST),
		&mi))
	{
		rectScreen = mi.rcWork;
	}
	else
	{
		::SystemParametersInfo (SPI_GETWORKAREA, 0, &rectScreen, 0);
	}

	const int nBorderSize = GetBorderSize ();
	const BOOL bRTL = GetExStyle() & WS_EX_LAYOUTRTL;

	if (m_bResizeTracking)
	{
		if (bRTL)
		{
			m_sizeCurrent.cx = min (
				m_sizeCurrent.cx, 
				m_ptLocation.x - rectScreen.left - 2 * nBorderSize);
		}
		else
		{
			m_sizeCurrent.cx = min (
				m_sizeCurrent.cx, 
				rectScreen.right - m_ptLocation.x - 2 * nBorderSize);
		}

		m_sizeCurrent.cy = min (
			m_sizeCurrent.cy, 
			rectScreen.bottom - m_ptLocation.y - 2 * nBorderSize - m_rectResize.Height ());
	}

	//---------------------------------------
	// Normalize location in the screen area:
	//---------------------------------------
	m_ptLocation.x = max (rectScreen.left, min (m_ptLocation.x, rectScreen.right));

	if (m_pParentBtn == NULL)
	{
		m_ptLocation.y = max (rectScreen.top, min (m_ptLocation.y, rectScreen.bottom));
	}

	CSize size = m_sizeCurrent;
	
	if (!m_bResizeTracking && !m_bWasResized)
	{
		size = pMenuBar->CalcSize (TRUE);
	};

	if (!m_bResizeTracking && !m_bWasResized)
	{
		size.cx += nBorderSize * 2;
		size.cy += nBorderSize * 2;

		switch (m_nLogoLocation)
		{
		case MENU_LOGO_LEFT:
		case MENU_LOGO_RIGHT:
			size.cx += m_iLogoWidth;
			break;

		case MENU_LOGO_TOP:
		case MENU_LOGO_BOTTOM:
			size.cy += m_iLogoWidth;
			break;
		}
	}

	if (m_pMenuCustomizationPage != NULL)
	{
		size.cy += ::GetSystemMetrics (SM_CYSMCAPTION);
		size.cy += 2 * ::GetSystemMetrics (SM_CYBORDER) + 5;
	}
	else if (bIsTearOff)
	{
		m_rectTearOffCaption = CRect (CPoint (nBorderSize, nBorderSize), 
			CSize (size.cx - 2 * nBorderSize, iTearOffBarHeight));
		size.cy += iTearOffBarHeight;

		if (!CBCGPToolBar::IsCustomizeMode () && m_wndToolTip.GetSafeHwnd () == NULL)
		{
			CBCGPLocalResource locaRes;

			m_wndToolTip.Create (this);
			m_wndToolTip.Activate (TRUE);
			if (globalData.m_nMaxToolTipWidth != -1)
			{
				m_wndToolTip.SetMaxTipWidth(globalData.m_nMaxToolTipWidth);
			}

			m_wndToolTip.AddTool (this, IDS_TEAR_OFF, m_rectTearOffCaption, 1);
		}
	}

	if (m_nMaxHeight != -1 && size.cy > m_nMaxHeight)
	{
		if (!m_bResizeTracking && !m_bWasResized)
		{
			int nMaxHeight = m_nMaxHeight - nBorderSize * 2;

			size.cy = nMaxHeight - (nMaxHeight % pMenuBar->GetRowHeight ()) + nBorderSize * 2 + 2;
			m_bHasBeenResized = TRUE;
		}

		m_bScrollable = TRUE;
	}

	if (m_bIsResizable)
	{
		const int nResizeBarHeight = 
			m_sizeMinResize.cx > 0 ? iResizeBarBarHeightRight : iResizeBarBarHeight;

		if (m_bIsResizeBarOnTop)
		{
			m_rectResize = CRect (CPoint (nBorderSize, nBorderSize),
				CSize (size.cx - 2 * nBorderSize, nResizeBarHeight));
		}
		else
		{
			m_rectResize = CRect (CPoint (nBorderSize, size.cy - nBorderSize),
				CSize (size.cx - 2 * nBorderSize, nResizeBarHeight));
		}

		size.cy += nResizeBarHeight;
	}

	BOOL bIsRightAlign = bRTL || m_bRightAlign;

	CBCGPMenuBar* pParentMenuBar = m_pParentBtn == NULL ? NULL :
		DYNAMIC_DOWNCAST (CBCGPMenuBar, m_pParentBtn->m_pWndParent);

	if (pParentMenuBar != NULL && pParentMenuBar->IsFloating ())
	{
		// When the popup menu is dropped-down from the floating menu bar,
		// it should not cover other menu bar items. Ask parent menu bar about
		// right popup menu location:
		DROP_DIRECTION direction = (DROP_DIRECTION) 
			pParentMenuBar->GetFloatPopupDirection (m_pParentBtn);

		switch (direction)
		{
		case DROP_DIRECTION_TOP:
			m_ptLocation.y = m_ptLocationInitial.y - size.cy - m_pParentBtn->Rect ().Height () + 1;
			m_DropDirection = direction;
			break;

		case DROP_DIRECTION_LEFT:
			m_ptLocation.y = m_ptLocationInitial.y - m_pParentBtn->Rect ().Height () + 1;
			m_DropDirection = direction;

			if (bRTL)
			{
				m_ptLocation.x = m_ptLocationInitial.x + size.cx;

				if (m_ptLocation.x > rectScreen.right)
				{
					m_ptLocation.x = rectScreen.right;
					m_DropDirection = DROP_DIRECTION_NONE;
				}
			}
			else
			{
				m_ptLocation.x = m_ptLocationInitial.x - size.cx;

				if (m_ptLocation.x < rectScreen.left)
				{
					m_ptLocation.x = rectScreen.left;
					m_DropDirection = DROP_DIRECTION_NONE;
				}
			}
			break;

		case DROP_DIRECTION_RIGHT:
			if (bRTL)
			{
				m_ptLocation.x = m_ptLocationInitial.x - m_pParentBtn->Rect ().Width ();
			}
			else
			{
				m_ptLocation.x = m_ptLocationInitial.x + m_pParentBtn->Rect ().Width ();
			}

			m_ptLocation.y = m_ptLocationInitial.y - m_pParentBtn->Rect ().Height () + 1;
			m_DropDirection = direction;
			break;
		}
	}

	//---------------------------------
	// Prepare Quick Customize Drawing 
	//----------------------------------
	CRect rectQCParent;
	rectQCParent.SetRectEmpty ();

	BOOL bConnectQCToParent = FALSE;

	if (CBCGPVisualManager::GetInstance ()->IsOfficeXPStyleMenus ())
	{
		CBCGPPopupMenu* pParentPopup = GetParentPopupMenu();
		
		if (pParentPopup != NULL)
		{
			CBCGPToolbarMenuButton* pParentBtn = GetParentButton ();
			if ((pParentBtn != NULL) && (pParentBtn->IsQuickMode()))
			{
				if (!bRTL)
				{
					pParentPopup->RedrawWindow();
				}
				
				if (pParentPopup->IsQuickCustomize ())
				{
					if (!m_bQuickCusomize)
					{
						rectQCParent = pParentBtn->Rect ();
						CWnd* pParentWnd = pParentBtn->GetParentWnd ();

						if (pParentWnd->GetSafeHwnd () != NULL)
						{
							pParentWnd->ClientToScreen (&rectQCParent);

							m_ptLocation.y = rectQCParent.top;
							bConnectQCToParent = TRUE;

							if (m_DropDirection == DROP_DIRECTION_LEFT)
							{
								m_ptLocation.x = 
									bRTL ?
										rectQCParent.left :
										rectQCParent.left - size.cx;
							}
							else
							{
								m_ptLocation.x = 
									bRTL ?
										rectQCParent.right + size.cx :
										rectQCParent.right;
							}
						}
					}
				}
			}
		}
	}

	//---------------------------------------------
	// Adjust the menu position by the screen size:
	//---------------------------------------------
	if ((bRTL && (m_ptLocation.x - size.cx < rectScreen.left)) || 
		(!bIsRightAlign && (m_ptLocation.x + size.cx > rectScreen.right)))
	{
		//-----------------------------------------------------
		// Menu can't be overlapped with the parent popup menu!
		//-----------------------------------------------------
		CBCGPPopupMenu* pParentMenu = GetParentPopupMenu ();

		if (pParentMenu != NULL)
		{
			CRect rectParent;
			pParentMenu->GetWindowRect (rectParent);

			m_ptLocation.x = bRTL ? rectParent.right + size.cx : rectParent.left - size.cx;

#ifndef BCGP_EXCLUDE_RIBBON
			if (m_pParentRibbonElement != NULL)
			{
				ASSERT_VALID (m_pParentRibbonElement);

				if (!m_pParentRibbonElement->IsMenuMode ())
				{
					rectParent = m_pParentRibbonElement->GetRect ();
					pParentMenu->ClientToScreen (&rectParent);

					m_ptLocation.x = bRTL ? rectParent.left + size.cx : rectParent.right - size.cx;
				}
			}
			
#endif // BCGP_EXCLUDE_RIBBON

			m_DropDirection = bRTL ? DROP_DIRECTION_RIGHT : DROP_DIRECTION_LEFT;
		}
		else if (pParentMenuBar != NULL && 
			(pParentMenuBar->IsHorizontal ()) == 0)
		{
			//------------------------------------------------
			// Parent menu bar is docked vertical, place menu 
			// in the left or right side of the parent frame:
			//------------------------------------------------
			CRect rectParentBtn = m_pParentBtn->Rect();
			pParentMenuBar->ClientToScreen(&rectParentBtn);

			m_ptLocation.x = bRTL ? rectParentBtn.right + size.cx : rectParentBtn.left - size.cx;

			if (m_ptLocation.x + size.cx >= rectScreen.right)
			{
				m_ptLocation.x = rectScreen.right - size.cx - 1;
			}

			m_DropDirection = DROP_DIRECTION_LEFT;
		}
		else
		{
			m_ptLocation.x = bRTL ? rectScreen.left + size.cx + 1 : m_bRightAlign ? rectScreen.left + 1 : rectScreen.right - size.cx - 1;
			m_DropDirection = DROP_DIRECTION_NONE;
		}

		if (!bRTL && (m_ptLocation.x < rectScreen.left))
		{
			m_ptLocation.x = rectScreen.left;
			m_DropDirection = DROP_DIRECTION_NONE;
		}

		if (bRTL && (m_ptLocation.x > rectScreen.right))
		{
			m_ptLocation.x = rectScreen.right;
			m_DropDirection = DROP_DIRECTION_NONE;
		}

		if (!m_bDisableAnimation)
		{
			if (GetAnimationType () == UNFOLD)
			{
				m_bIsAnimRight = FALSE;
			}
			else if (GetAnimationType () == FADE)
			{
				m_bIsAnimRight = FALSE;
				m_bIsAnimDown = FALSE;
			}
		}
	}

	if (m_ptLocation.y + size.cy > rectScreen.bottom)
	{
		const int nResizeBarHeight = m_rectResize.Height ();

		if (m_bIsResizable)
		{
			m_rectResize = CRect (CPoint (nBorderSize, nBorderSize),
				CSize (size.cx - 2 * nBorderSize, nResizeBarHeight));

			m_bIsResizeBarOnTop = TRUE;
		}

		m_bIsAnimDown = FALSE;

		CRect rectParentBtn;
		CWnd* pWndParent = GetParentArea (rectParentBtn);

		if (pWndParent != NULL && 
			m_DropDirection != DROP_DIRECTION_LEFT && m_DropDirection != DROP_DIRECTION_RIGHT)
		{
			CPoint ptRight (rectParentBtn.right, 0);
            pWndParent->ClientToScreen (&ptRight);

            CPoint ptTop (0, rectParentBtn.top - size.cy);
			pWndParent->ClientToScreen (&ptTop);

            if (ptTop.y < 0)
            {
				int yParentButtonTop = ptTop.y + size.cy;

				//-----------------------------------------------------
				// Where more space: on top or on bottom of the button?
				//-----------------------------------------------------
				if (rectScreen.bottom - yParentButtonTop < 
					yParentButtonTop - rectScreen.top)
				{
					m_ptLocation.y = rectScreen.top;
					m_DropDirection = DROP_DIRECTION_NONE;
					size.cy += ptTop.y;
				}
				else
				{
					size.cy = rectScreen.bottom - m_ptLocation.y;
					m_bIsAnimDown = TRUE;

					if (m_bIsResizable)
					{
						//-----------------------------------
						// Restore resize box back to bottom:
						//-----------------------------------
						m_bIsResizeBarOnTop = FALSE;
						m_rectResize = CRect (CPoint (nBorderSize, size.cy - nBorderSize - nResizeBarHeight),
							CSize (size.cx - 2 * nBorderSize, nResizeBarHeight));
					}
				}

				m_bHasBeenResized = TRUE;
				m_bScrollable = TRUE;
            }
            else
            {
                m_ptLocation.y = ptTop.y;

				m_DropDirection = 
					(pParentMenuBar != NULL && pParentMenuBar->IsHorizontal ()) ?
									DROP_DIRECTION_TOP : DROP_DIRECTION_NONE;
            }
		}
		else
		{
			if (bConnectQCToParent)
			{
				m_ptLocation.y = rectQCParent.bottom - size.cy - 1;
			}
			else
			{
				m_ptLocation.y -= size.cy;

				if (GetParentPopupMenu () != NULL)
				{
					m_ptLocation.y += pMenuBar->GetRowHeight () + nBorderSize * 2;
				}
			}
		}

		if (m_ptLocation.y < rectScreen.top)
		{
			m_ptLocation.y = rectScreen.top;
			m_DropDirection = DROP_DIRECTION_NONE;
		}

		if (m_ptLocation.y + size.cy > rectScreen.bottom)
		{
			size.cy = rectScreen.bottom - m_ptLocation.y;
			m_bHasBeenResized = TRUE;
			m_bScrollable = TRUE;
		}
	}

	if (m_ptLocation.y < rectScreen.top)
	{
		if (m_pParentBtn != NULL && m_pParentBtn->GetParentWnd () != NULL &&
			GetParentPopupMenu () == NULL)
		{
			CPoint ptRight (m_pParentBtn->Rect ().right, 0);
            m_pParentBtn->GetParentWnd ()->ClientToScreen (&ptRight);

            CPoint ptBottom (0, m_pParentBtn->Rect ().bottom);
			m_pParentBtn->GetParentWnd ()->ClientToScreen (&ptBottom);

			m_ptLocation.y = ptBottom.y;

			m_DropDirection = 
				(pParentMenuBar != NULL && pParentMenuBar->IsHorizontal ()) ?
								DROP_DIRECTION_BOTTOM : DROP_DIRECTION_NONE;
		}
		else
		{
			m_ptLocation.y = rectScreen.top;
		}

		if (m_ptLocation.y + size.cy > rectScreen.bottom)
		{
			m_ptLocation.y = rectScreen.top;

			if (size.cy > rectScreen.Height ())
			{
				size.cy = rectScreen.Height ();
				m_bHasBeenResized = TRUE;
				m_bScrollable = TRUE;
			}

			m_DropDirection = DROP_DIRECTION_NONE;
		}
	}

	if (m_bScrollable && m_bShowScrollBar && !m_bResizeTracking && !m_bWasResized)
	{
		size.cx += ::GetSystemMetrics (SM_CXVSCROLL);

		if (!m_rectResize.IsRectEmpty ())
		{
			m_rectResize.right += ::GetSystemMetrics (SM_CXVSCROLL);
		}
	}

	m_FinalSize = size;

	if (GetAnimationType () != NO_ANIMATION || m_bAnimationIsDone || 
		CBCGPToolBar::IsCustomizeMode ())
	{
		if (!CBCGPToolBar::IsCustomizeMode ())
		{
			size.cx += m_iShadowSize;
			size.cy += m_iShadowSize;
		}

		if (m_pMenuCustomizationPage != NULL)
		{
			SetWindowPos (NULL, -1, -1, size.cx, size.cy,
						SWP_NOMOVE | SWP_NOZORDER |
						SWP_NOACTIVATE);
		}
		else
		{
			SetWindowPos (NULL, m_ptLocation.x - (bRTL ? size.cx : 0), m_ptLocation.y, size.cx, size.cy,
						SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	if (CBCGPToolBar::IsCustomizeMode ())
	{
		pMenuBar->AdjustLocations ();
	}

	//---------------------------------------------------
	// Parent button maybe covered by shadow, repaint it:
	//---------------------------------------------------
	if (m_iShadowSize != 0 &&
		!CBCGPToolBar::IsCustomizeMode () &&
		m_pParentBtn != NULL &&
		m_pParentBtn->GetParentWnd () != NULL)
	{
		BOOL bOldShown = m_bShown;
		m_bShown = TRUE;

		CWnd* pWndParent = m_pParentBtn->GetParentWnd ();

		CRect rectInter;

		CRect rectMenu;
		GetWindowRect (rectMenu);

		CRect rectShadowRight (CPoint (bRTL ? rectMenu.left - 1 - m_iShadowSize : rectMenu.right + 1, rectMenu.top), 
					CSize (m_iShadowSize, rectMenu.Height () + m_iShadowSize));
		pWndParent->ScreenToClient (&rectShadowRight);

		if (rectInter.IntersectRect (rectShadowRight, m_pParentBtn->m_rect))
		{
			pWndParent->InvalidateRect (m_pParentBtn->m_rect);
			pWndParent->UpdateWindow ();
		}

		CRect rectShadowBottom (CPoint (rectMenu.left, rectMenu.bottom + 1),
					CSize (rectMenu.Width () + m_iShadowSize, m_iShadowSize));
		pWndParent->ScreenToClient (&rectShadowBottom);

		if (rectInter.IntersectRect (rectShadowBottom, m_pParentBtn->m_rect))
		{
			pWndParent->InvalidateRect (m_pParentBtn->m_rect);
			pWndParent->UpdateWindow ();
		}

		m_bShown = bOldShown;
	}

	if (m_bScrollable && m_bShowScrollBar && !m_bResizeTracking && !m_bWasResized)
	{
		RedrawWindow ();
	}
}
//****************************************************************************************
void CBCGPPopupMenu::OnDestroy() 
{
	if (m_pWndShadow->GetSafeHwnd () != NULL)
	{
		m_pWndShadow->ShowWindow (SW_HIDE);
		m_pWndShadow->DestroyWindow ();
	}

	if (m_bQuickCusomize)
	{
		//------------------------------
		// Restore recently used state
		//------------------------------
		CBCGPMenuBar::SetRecentlyUsedMenus(CBCGPCustomizeMenuButton::m_bRecentlyUsedOld);	

		//--------------------------------
		// Made caption button non-selected
		//--------------------------------
		CWnd* pWnd = GetOwner();
		if (pWnd != NULL)
		{
			if (pWnd->IsKindOf(RUNTIME_CLASS(CBCGPMiniFrameWnd)))
			{
				CBCGPMiniFrameWnd* pMiniFrm  = DYNAMIC_DOWNCAST (CBCGPMiniFrameWnd, pWnd);
				CBCGPCaptionMenuButton* pBtn = (CBCGPCaptionMenuButton *)pMiniFrm->FindButton(HTMENU_BCG);
				if (pBtn != NULL)
				{
					pBtn->m_bPushed = FALSE;
					pMiniFrm->OnNcPaint();
					
				}
			}
		}
	}

	CBCGPPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	//---------------------------------------------------------
	// First, maybe we have a dragged menu item. Remove it now:
	//---------------------------------------------------------
	if (pMenuBar->m_pDragButton != NULL &&
		!pMenuBar->m_bIsDragCopy)
	{
		pMenuBar->RemoveButton (
			pMenuBar->ButtonToIndex (pMenuBar->m_pDragButton));
		pMenuBar->m_pDragButton = NULL;
	}

#ifndef BCGP_EXCLUDE_RIBBON
	if (m_pParentRibbonElement != NULL)
	{
		m_pParentRibbonElement->m_pPopupMenu = NULL;

		CBCGPPopupMenu* pParentMenu = GetParentPopupMenu ();
		if (pParentMenu != NULL)
		{
			ASSERT_VALID (pParentMenu);

			if (pParentMenu->IsFloaty ())
			{
				CBCGPRibbonFloaty* pFloaty = DYNAMIC_DOWNCAST (
					CBCGPRibbonFloaty, pParentMenu);

				if (pFloaty != NULL && !pFloaty->IsContextMenuMode ())
				{
					m_bAutoDestroyParent = FALSE;
				}
			}

			if (m_bAutoDestroyParent && !CBCGPToolBar::IsCustomizeMode ())
			{
				CPoint ptCursor;
				::GetCursorPos (&ptCursor);

				CRect rectParent;
				pParentMenu->GetWindowRect (rectParent);

				//-------------------------------------------
				// Automatically close the parent popup menu:
				//-------------------------------------------
				if (pParentMenu->IsAlwaysClose () || !rectParent.PtInRect (ptCursor))
				{
					pParentMenu->SendMessage (WM_CLOSE);
					m_pParentRibbonElement = NULL;
				}
			}
		}
	}
#endif

	if (m_pParentBtn != NULL)
	{
		ASSERT (m_pParentBtn->m_pPopupMenu == this);

		SaveState ();

		m_pParentBtn->m_pPopupMenu = NULL;
		m_pParentBtn->m_bClickedOnMenu = FALSE;

		CBCGPPopupMenu* pParentMenu = GetParentPopupMenu ();
		if (pParentMenu != NULL)
		{
			if (m_bAutoDestroyParent && !CBCGPToolBar::IsCustomizeMode ())
			{
				//-------------------------------------------
				// Automatically close the parent popup menu:
				//-------------------------------------------
				pParentMenu->SendMessage (WM_CLOSE);
				m_pParentBtn = NULL;
			}
		}
	}
	else
	{
		CBCGPMenuBar::SetShowAllCommands (FALSE);
	}

	if (m_pMenuCustomizationPage != NULL)
	{
		m_pMenuCustomizationPage->CloseContextMenu (this);
	}
				
	NotifyParentDlg (FALSE);

	//------------------------------------------------
	// Inform the main frame about the menu detsroyng:
	//------------------------------------------------
	CFrameWnd* pWndMain = BCGCBProGetTopLevelFrame (this);

	CBCGPMDIFrameWnd* pMainFrame = DYNAMIC_DOWNCAST (CBCGPMDIFrameWnd, pWndMain);
	if (pMainFrame != NULL)
	{
		pMainFrame->OnClosePopupMenu (this);
	}
	else	// Maybe, SDI frame...
	{
		CBCGPFrameWnd* pFrame = DYNAMIC_DOWNCAST (CBCGPFrameWnd, pWndMain);
		if (pFrame != NULL)
		{
			pFrame->OnClosePopupMenu (this);
		}
		else	// Maybe, OLE frame...
		{
			CBCGPOleIPFrameWnd* pOleFrame = 
				DYNAMIC_DOWNCAST (CBCGPOleIPFrameWnd, pWndMain);
			if (pOleFrame != NULL)
			{
				pOleFrame->OnClosePopupMenu (this);
			}
			else
			{
				CBCGPOleDocIPFrameWnd* pOleDocFrame = 
					DYNAMIC_DOWNCAST (CBCGPOleDocIPFrameWnd, pWndMain);
				if (pOleDocFrame != NULL)
				{
					pOleDocFrame->OnClosePopupMenu (this);
				}
			}
		}
	}

	if (m_bTrackMode && CBCGPPopupMenu::m_pActivePopupMenu == this)
	{
		CBCGPPopupMenu::m_pActivePopupMenu = NULL;
	}

	if (!CBCGPToolBar::IsCustomizeMode () &&
		pWndMain != NULL && m_pActivePopupMenu == NULL &&
		GetParentToolBar () != NULL &&
		GetParentToolBar () != GetFocus ())
	{
		GetParentToolBar ()->Deactivate ();
	}

	CMiniFrameWnd::OnDestroy();
}
//****************************************************************************************
void CBCGPPopupMenu::PostNcDestroy() 
{
	if (m_pParentBtn != NULL)
	{
		ASSERT_VALID (m_pParentBtn);
		m_pParentBtn->OnCancelMode ();
	}
		
#ifndef BCGP_EXCLUDE_RIBBON
	if (m_pParentRibbonElement != NULL)
	{
		ASSERT_VALID (m_pParentRibbonElement);
		m_pParentRibbonElement->ClosePopupMenu ();
	}
#endif

	CMiniFrameWnd::PostNcDestroy();
}
//****************************************************************************************
void CBCGPPopupMenu::SaveState ()
{
	if (!CBCGPToolBar::IsCustomizeMode ())
	{
		return;
	}

	if (m_pParentBtn == NULL || m_pParentBtn->IsMenuPaletteMode ())
	{
		return;
	}

	ASSERT_VALID (m_pParentBtn);

	CBCGPPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	HMENU hmenu = pMenuBar->ExportToMenu ();
	ASSERT (hmenu != NULL);

	m_pParentBtn->CreateFromMenu (hmenu);
	::DestroyMenu (hmenu);

	CBCGPPopupMenu* pParentMenu = GetParentPopupMenu ();
	if (pParentMenu != NULL)
	{
		pParentMenu->SaveState ();
	}
}
//****************************************************************************************
void CBCGPPopupMenu::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	ASSERT_VALID (this);

	CBCGPPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

#ifdef _DEBUG
	if (m_pParentBtn != NULL)
	{
		ASSERT_VALID (m_pParentBtn);
		ASSERT (m_pParentBtn->m_pPopupMenu == this);
	}
#endif // _DEBUG

	BOOL bHightlightWasChanged = FALSE;
	BOOL bIsRTL = GetExStyle() & WS_EX_LAYOUTRTL;

	if (bIsRTL)
	{
		if (nChar == VK_LEFT)
		{
			nChar = VK_RIGHT;
		}
		else if (nChar == VK_RIGHT)
		{
			nChar = VK_LEFT;
		}
	}

	switch (nChar)
	{
	case VK_RIGHT:
		{
			//-------------------------------------------
			// Save animation type and disable animation:
			//-------------------------------------------
			ANIMATION_TYPE animType = GetAnimationType ();
			m_AnimationType = NO_ANIMATION;

			//-------------------------------
			// Try to open next cascade menu:
			//-------------------------------
			CBCGPToolbarMenuButton* pSelItem = GetSelItem ();
			if (pSelItem != NULL && 
				(pSelItem->m_nID == (UINT) -1 || pSelItem->m_nID == 0 ||
				pSelItem->IsEmptyMenuAllowed ()) &&
				pSelItem->OpenPopupMenu ())
			{
				if (pSelItem->m_pPopupMenu != NULL)
				{
					//--------------------------
					// Select a first menu item:
					//--------------------------
					if (GetSelItem () == pSelItem)
					{
						pSelItem->m_pPopupMenu->OnKeyDown (VK_HOME, 0, 0);
					}
				}
			}
			else
			{
				//------------------------------------------------------
				// No next menu, first try to go to the parent menu bar:
				//------------------------------------------------------
				CBCGPToolBar* pToolBar = GetParentToolBar ();
				if (pToolBar != NULL && 
					!pToolBar->IsKindOf (RUNTIME_CLASS (CBCGPPopupMenuBar)))
				{
					pToolBar->NextMenu ();
				}
				else
				{
					//------------------------------------------------------
					// Close the current menu and move control to the parent
					// popup menu:
					//------------------------------------------------------
					CBCGPPopupMenu* pParenMenu = GetParentPopupMenu ();
					if (pParenMenu != NULL)
					{
						pParenMenu->SendMessage (WM_KEYDOWN, 
							bIsRTL ? VK_LEFT : VK_RIGHT);
					}
				}
			}

			//------------------------
			// Restore animation type:
			//------------------------
			m_AnimationType = animType;
		}
		return;
				
	case VK_LEFT:
		{
			CBCGPToolBar* pToolBar = GetParentToolBar ();
			if (pToolBar != NULL)
			{
				pToolBar->PrevMenu ();
			}
			else if (m_pParentBtn != NULL && m_pParentBtn->IsDroppedDown ())		
			{
				CloseMenu ();
			}
		}
		return;

	case VK_DOWN:
		if ((::GetAsyncKeyState (VK_CONTROL) & 0x8000) &&	// Ctrl+Down
			!pMenuBar->m_bAreAllCommandsShown)
		{
			ShowAllCommands ();
			break;
		}

	case VK_UP:
	case VK_HOME:
	case VK_END:
		bHightlightWasChanged = TRUE;

	case VK_RETURN:
		if (!CBCGPToolBar::IsCustomizeMode ())
		{
			pMenuBar->OnKey (nChar);
		}
		break;

	case VK_ESCAPE:
		{
			m_bEscClose = TRUE;
			CloseMenu (TRUE);
		}

		return;

	case VK_PRIOR:
	case VK_NEXT:
		if (m_bShowScrollBar)
		{
			bHightlightWasChanged = TRUE;
			pMenuBar->OnKey (nChar);
			break;
		}

	default:
		if (pMenuBar->OnKey (nChar))
		{
			return;
		}
		else
		{
			CMiniFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
		}
	}

	if (bHightlightWasChanged &&
		m_bScrollable && pMenuBar->m_iHighlighted >= 0)
	{
		//---------------------------------------
		// Maybe, selected item is invisible now?
		//---------------------------------------
		CBCGPToolbarButton* pItem = pMenuBar->GetButton (pMenuBar->m_iHighlighted);
		if (pItem == NULL && pMenuBar->GetRowHeight () == 0)
		{
			ASSERT (FALSE);
		}
		else
		{
			CRect rectBar;
			pMenuBar->GetClientRect (rectBar);

			int iOffset = pMenuBar->GetOffset ();
			int iOffsetDelta = 0;

			if (pItem->Rect ().top < rectBar.top)
			{
				//---------------------
				// Scroll up is needed!
				//---------------------
				iOffsetDelta = (pItem->Rect ().top - rectBar.top) / 
					pMenuBar->GetRowHeight () - 1;
			}
			else if (pItem->Rect ().bottom > rectBar.bottom)
			{
				//-----------------------
				// Scroll down is needed!
				//-----------------------
				iOffsetDelta = (pItem->Rect ().bottom - rectBar.bottom) / 
					pMenuBar->GetRowHeight () + 1;
			}

			if (iOffsetDelta != 0)
			{
				int iTotalRows = m_FinalSize.cy / pMenuBar->GetRowHeight () - 2;

				iOffset += iOffsetDelta;
				iOffset = min (max (0, iOffset), 
					(int) pMenuBar->m_Buttons.GetCount () - iTotalRows - 1);

				pMenuBar->SetOffset (iOffset);

				const BOOL bScrollButtonsChanged = AdjustScroll ();

				if (m_bShowScrollBar && m_wndScrollBarVert.GetSafeHwnd () != NULL)
				{
					m_wndScrollBarVert.SetScrollPos (iOffset);
				}
				else if (bScrollButtonsChanged)
				{
					//------------------------------------------
					// Scroll buttons were changed, adjust again
					//------------------------------------------
					AdjustScroll ();
				}
			}
		}
	}

	if (bHightlightWasChanged && pMenuBar->m_bDropDownListMode)
	{
		OnChangeHot (pMenuBar->m_iHighlighted);
	}
}
//****************************************************************************************
CBCGPPopupMenu* CBCGPPopupMenu::GetParentPopupMenu () const
{
	ASSERT_VALID (this);

	CBCGPPopupMenuBar* pParentBar = NULL;

	if (m_pParentBtn != NULL)
	{
		ASSERT_VALID (m_pParentBtn);

		pParentBar = DYNAMIC_DOWNCAST (CBCGPPopupMenuBar, 
			m_pParentBtn->m_pWndParent);
	}
#ifndef BCGP_EXCLUDE_RIBBON
	else if (m_pParentRibbonElement != NULL)
	{
		ASSERT_VALID (m_pParentRibbonElement);

		pParentBar = DYNAMIC_DOWNCAST (CBCGPPopupMenuBar, 
			m_pParentRibbonElement->GetParentWnd ());
	}
#endif

	if (pParentBar != NULL)
	{
		CBCGPPopupMenu* pParentMenu =
			DYNAMIC_DOWNCAST (CBCGPPopupMenu, BCGPGetParentFrame (pParentBar));

		return pParentMenu;
	}
	else
	{
		return NULL;
	}
}
//****************************************************************************************
CBCGPToolBar* CBCGPPopupMenu::GetParentToolBar () const
{
	if (m_pParentBtn == NULL)
	{
		return NULL;
	}

	CBCGPToolBar* pParentBar = 
		DYNAMIC_DOWNCAST (CBCGPToolBar, m_pParentBtn->m_pWndParent);
	return pParentBar;
}
//****************************************************************************************
CBCGPToolbarMenuButton* CBCGPPopupMenu::GetSelItem ()
{
	CBCGPPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	return DYNAMIC_DOWNCAST (CBCGPToolbarMenuButton,
							pMenuBar->GetHighlightedButton ());
}
//****************************************************************************************
void CBCGPPopupMenu::CloseMenu (BOOL bSetFocusToBar)
{
	if (GetSafeHwnd () == NULL)
	{
		return;
	}

	m_bTobeDstroyed = TRUE;

	SaveState ();

	CBCGPPopupMenu* pParentMenu = GetParentPopupMenu ();
	CBCGPToolBar* pParentToolBar = GetParentToolBar ();

	CFrameWnd* pWndMain = BCGCBProGetTopLevelFrame (this);
	if (pParentMenu != NULL)
	{
		m_bAutoDestroyParent = FALSE;
		ActivatePopupMenu (pWndMain, pParentMenu);
	}
	else if (pParentToolBar != NULL)
	{
		ActivatePopupMenu (pWndMain, NULL);
		NotifyParentDlg (FALSE);

		if (bSetFocusToBar)
		{
			pParentToolBar->SetFocus ();
		}
	}
	else
	{
		ActivatePopupMenu (pWndMain, NULL);
		NotifyParentDlg (FALSE);
	}

	SendMessage (WM_CLOSE);
}
//****************************************************************************************
int CBCGPPopupMenu::InsertItem (const CBCGPToolbarMenuButton& button, int iInsertAt)
{
	CBCGPPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	return pMenuBar->InsertButton (button, iInsertAt);
}
//****************************************************************************************
int CBCGPPopupMenu::InsertSeparator (int iInsertAt)
{
	CBCGPPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	return pMenuBar->InsertSeparator (iInsertAt);
}
//****************************************************************************************
int CBCGPPopupMenu::GetMenuItemCount () const
{
	CBCGPPopupMenuBar* pMenuBar = ((CBCGPPopupMenu*) this)->GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	return (int) pMenuBar->m_Buttons.GetCount ();
}
//****************************************************************************************
CBCGPToolbarMenuButton* CBCGPPopupMenu::GetMenuItem (int iIndex) const
{
	CBCGPPopupMenuBar* pMenuBar = ((CBCGPPopupMenu*) this)->GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	return (CBCGPToolbarMenuButton*) pMenuBar->GetButton (iIndex);
}
//****************************************************************************************
CBCGPToolbarMenuButton* CBCGPPopupMenu::FindSubItemByCommand (UINT uiCmd) const
{
	CBCGPPopupMenuBar* pMenuBar = ((CBCGPPopupMenu*) this)->GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	for (POSITION pos = pMenuBar->m_Buttons.GetHeadPosition (); pos != NULL;)
	{
		CBCGPToolbarMenuButton* pItem = 
			DYNAMIC_DOWNCAST (CBCGPToolbarMenuButton, pMenuBar->m_Buttons.GetNext (pos));

		if (pItem != NULL)
		{
			ASSERT_VALID (pItem);

			const CObList& listCommands = pItem->GetCommands ();

			for (POSITION posList = listCommands.GetHeadPosition (); posList != NULL;)
			{
				CBCGPToolbarMenuButton* pSubItem = (CBCGPToolbarMenuButton*) listCommands.GetNext (posList);
				ASSERT_VALID (pSubItem);

				if (pSubItem->m_nID == uiCmd)
				{
					return pItem;
				}
			}
		}
	}

	return NULL;
}
//****************************************************************************************
BOOL CBCGPPopupMenu::RemoveItem (int iIndex)
{
	CBCGPPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	return pMenuBar->RemoveButton (iIndex);
}
//****************************************************************************************
void CBCGPPopupMenu::RemoveAllItems ()
{
	CBCGPPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	pMenuBar->RemoveAllButtons ();
}
//****************************************************************************************
BOOL CBCGPPopupMenu::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return TRUE;
}
//****************************************************************************************
BOOL CBCGPPopupMenu::ActivatePopupMenu (CFrameWnd* pTopFrame, CBCGPPopupMenu* pPopupMenu)
{
	if (pPopupMenu != NULL)
	{
		pPopupMenu->NotifyParentDlg (TRUE);
	}

	if (pTopFrame != NULL)
	{
		ASSERT_VALID (pTopFrame);

		BOOL bRes = TRUE;

		CBCGPMDIFrameWnd* pMainFrame = DYNAMIC_DOWNCAST (CBCGPMDIFrameWnd, pTopFrame);
		if (pMainFrame != NULL)
		{
			bRes = pMainFrame->ShowPopupMenu (pPopupMenu);
		}
		else	// Maybe, SDI frame...
		{
			CBCGPFrameWnd* pFrame = DYNAMIC_DOWNCAST (CBCGPFrameWnd, pTopFrame);
			if (pFrame != NULL)
			{
				bRes = pFrame->ShowPopupMenu (pPopupMenu);
			}
			else	// Maybe, OLE frame
			{
				CBCGPOleIPFrameWnd* pOleFrame = 
					DYNAMIC_DOWNCAST (CBCGPOleIPFrameWnd, pTopFrame);
				if (pOleFrame != NULL)
				{
					bRes = pOleFrame->ShowPopupMenu (pPopupMenu);
				}
				else
				{
					CBCGPOleDocIPFrameWnd* pOleDocFrame = 
						DYNAMIC_DOWNCAST (CBCGPOleDocIPFrameWnd, pTopFrame);
					if (pOleDocFrame != NULL)
					{
						bRes = pOleDocFrame->ShowPopupMenu (pPopupMenu);
					}
				}
			}
		}

		if (!bRes)
		{
			if (pPopupMenu != NULL && !pPopupMenu->m_bTobeDstroyed)
			{
				pPopupMenu->CloseMenu ();
			}

			return FALSE;
		}
	}

	if (pPopupMenu != NULL)
	{
		CBCGPPopupMenuBar* pMenuBar = pPopupMenu->GetMenuBar ();
		ASSERT_VALID (pMenuBar);

		CBCGPPopupMenu* pParentMenu = DYNAMIC_DOWNCAST (CBCGPPopupMenu, pMenuBar->GetParent ());
		if (pParentMenu != NULL && pParentMenu->GetParentButton () != NULL &&
			!pMenuBar->m_bAreAllCommandsShown)
		{
			// Check if "Show all" button is not exist yet:
			if (pMenuBar->m_Buttons.IsEmpty () ||
				DYNAMIC_DOWNCAST (CBCGPShowAllButton, pMenuBar->m_Buttons.GetTail ()) == NULL)
			{
				pMenuBar->InsertButton (CBCGPShowAllButton ());
			}
		}

		if (pPopupMenu->m_bTrackMode)
		{
			CBCGPPopupMenu::m_pActivePopupMenu = pPopupMenu;
		}
	}

	return TRUE;
}
//************************************************************************************
#if _MSC_VER >= 1300
void CBCGPPopupMenu::OnActivateApp(BOOL bActive, DWORD /*dwThreadID*/)
#else
void CBCGPPopupMenu::OnActivateApp(BOOL bActive, HTASK /*hTask*/) 
#endif
{
	if (!bActive && !CBCGPToolBar::IsCustomizeMode () &&
		!InCommand ())
	{
		if (m_bTrackMode)
		{
			m_bTobeDstroyed = TRUE;
		}

		PostMessage (WM_CLOSE);
	}
}
//*************************************************************************************
void CBCGPPopupMenu::OnTimer(UINT_PTR nIDEvent) 
{
	CBCGPPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	switch (nIDEvent)
	{
	case iAnimTimerId:
		if (!m_bAnimationIsDone)
		{
			clock_t nCurrAnimTime = clock ();

			int nDuration = nCurrAnimTime - nLastAnimTime;
			int nSteps = (int) (.5 + (float) nDuration / m_AnimationSpeed);

			switch (GetAnimationType ())
			{
			case UNFOLD:
				m_AnimSize.cx += nSteps * pMenuBar->GetColumnWidth ();
				// no break intentionally

			case SLIDE:
				m_AnimSize.cy += nSteps * pMenuBar->GetRowHeight ();
				break;

			case FADE:
				m_iFadePercent += iFadeStep;
				if (m_iFadePercent > 100 + nSteps * iFadeStep)
				{
					m_iFadePercent = 101;
				}
				break;
			}

			if ((GetAnimationType () != FADE && m_AnimSize.cy - m_iShadowSize >= m_FinalSize.cy) ||
				(GetAnimationType () == UNFOLD && m_AnimSize.cx - m_iShadowSize >= m_FinalSize.cx) ||
				(GetAnimationType () == FADE && m_iFadePercent > 100))
			{

				m_AnimSize.cx = m_FinalSize.cx + m_iShadowSize;
				m_AnimSize.cy = m_FinalSize.cy + m_iShadowSize;

				KillTimer (iAnimTimerId);

				pMenuBar->SetWindowPos (NULL,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOREDRAW|SWP_NOZORDER|SWP_SHOWWINDOW | SWP_NOACTIVATE);
				pMenuBar->ValidateRect (NULL);
    
				m_bAnimationIsDone = TRUE;

				if (m_iShadowSize != 0 &&
					GetAnimationType () != FADE &&
					m_DropDirection == DROP_DIRECTION_TOP)
				{
					UpdateShadow ();
				}
			}

			RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN); 

			nLastAnimTime = nCurrAnimTime;
		}
		break;

	case iScrollTimerId:
		{
			CPoint point;
			::GetCursorPos (&point);
			ScreenToClient (&point);

			CBCGPToolbarMenuButton* pSelItem = GetSelItem ();
			if (pSelItem != NULL)
			{
				pSelItem->OnCancelMode ();
			}

			int iOffset = pMenuBar->GetOffset ();

			if (m_rectScrollUp.PtInRect (point) && m_iScrollMode < 0)	// Scroll Up
			{
				pMenuBar->SetOffset (iOffset - 1);
				AdjustScroll ();
			}
			else if (m_rectScrollDn.PtInRect (point) && m_iScrollMode > 0)	// Scroll Down
			{
				pMenuBar->SetOffset (iOffset + 1);
				AdjustScroll ();
			}
			else
			{
				KillTimer (iScrollTimerId);
				m_iScrollMode = 0;
				InvalidateRect (m_rectScrollDn);
				InvalidateRect (m_rectScrollUp);
			}
		}
		break;
	}

	CMiniFrameWnd::OnTimer(nIDEvent);
}
//****************************************************************************************
void CBCGPPopupMenu::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bTearOffTracking)
	{
		if (!m_rectTearOffCaption.PtInRect (point))
		{
			ReleaseCapture ();
			m_bTearOffTracking = FALSE;

			TearOff (point);
		}

		return;
	}

	CMiniFrameWnd::OnMouseMove(nFlags, point);

	if (!m_bScrollable || m_iScrollMode != 0)
	{
		return;
	}
	
	if (m_rectScrollUp.PtInRect (point) && IsScrollUpAvailable ())
	{
		m_iScrollMode = -1;
		InvalidateRect (m_rectScrollUp);
	}
	else if (m_rectScrollDn.PtInRect (point) && IsScrollDnAvailable ())
	{
		m_iScrollMode = 1;
		InvalidateRect (m_rectScrollDn);
	}
	else
	{
		m_iScrollMode = 0;
	}

	if (m_iScrollMode != 0)
	{
		SetTimer (iScrollTimerId, iScrollTimerDuration, NULL);
	}
}
//****************************************************************************************
BOOL CBCGPPopupMenu::IsScrollUpAvailable ()
{
	CBCGPPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	return pMenuBar->GetOffset () > 0;
}
//****************************************************************************************
BOOL CBCGPPopupMenu::IsScrollDnAvailable ()
{
	CBCGPPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	if (pMenuBar->GetCount () == 0)
	{
		return FALSE;
	}

	CRect rectLastItem;
	pMenuBar->GetItemRect (pMenuBar->GetCount () - 1, rectLastItem);

	return rectLastItem.bottom > m_nMenuBarHeight + pMenuBar->GetRowHeight ();
}
//****************************************************************************************
void CBCGPPopupMenu::CollapseSubmenus ()
{
	CBCGPPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	for (POSITION pos = pMenuBar->m_Buttons.GetHeadPosition (); pos != NULL;)
	{
		CBCGPToolbarButton* pButton = 
			(CBCGPToolbarButton*) pMenuBar->m_Buttons.GetNext (pos);
		ASSERT (pButton != NULL);

		pButton->OnCancelMode ();
	}
}
//****************************************************************************************
void CBCGPPopupMenu::ShowAllCommands ()
{
	CBCGPToolbarMenuButton* pParentMenuButton =
		DYNAMIC_DOWNCAST (CBCGPToolbarMenuButton, m_pParentBtn);
	if (pParentMenuButton != NULL)
	{
		CBCGPPopupMenuBar* pMenuBar = GetMenuBar ();
		ASSERT_VALID (pMenuBar);

		pMenuBar->SetHot (NULL);

		CBCGPMenuBar::SetShowAllCommands ();

		// Play standard menu popup sound!
		BCGPlaySystemSound (BCGSOUND_MENU_POPUP);

		ShowWindow (SW_HIDE);
		m_bShown = FALSE;

		if (m_bmpShadowRight.GetSafeHandle () != NULL)
		{
			m_bmpShadowRight.DeleteObject ();
		}
		
		if (m_bmpShadowBottom.GetSafeHandle () != NULL)
		{
			m_bmpShadowBottom.DeleteObject ();
		}

		m_ptLocation = m_ptLocationInitial;

		InitMenuBar ();

		if (m_bScrollable)
		{
			AdjustScroll ();
			SetScrollBar ();
		}

		UpdateBottomWindows ();

		ShowWindow (SW_SHOWNOACTIVATE);

		if (pParentMenuButton->m_pWndParent != NULL 
			&& ::IsWindow (pParentMenuButton->m_pWndParent->m_hWnd))
		{
			pParentMenuButton->m_pWndParent->InvalidateRect (
				pParentMenuButton->Rect ());
			pParentMenuButton->m_pWndParent->UpdateWindow ();
		}
	}
}
//**************************************************************************************
void CBCGPPopupMenu::SetMaxWidth (int iMaxWidth)
{
	if (iMaxWidth == m_iMaxWidth)
	{
		return;
	}

	m_iMaxWidth = iMaxWidth;
	if (GetSafeHwnd () == NULL)
	{
		return;
	}

	CBCGPPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	if (!::IsWindow (m_hWnd) ||
		!::IsWindow (pMenuBar->m_hWnd))
	{
		return;
	}

	pMenuBar->m_iMaxWidth = m_iMaxWidth;
	RecalcLayout ();
}
//*************************************************************************************
BOOL CBCGPPopupMenu::InitMenuBar ()
{
	CBCGPPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	if (m_hMenu != NULL)
	{
		ASSERT (::IsMenu (m_hMenu));

		if (m_pParentBtn != NULL || 
			!g_menuHash.LoadMenuBar (m_hMenu, pMenuBar))
		{
			//-------------------------------------------
			// Failed to restore, load the default state:
			//-------------------------------------------
			if (CBCGPMenuBar::IsShowAllCommands())
			{
				if (!pMenuBar->ImportFromMenu (m_hMenu, TRUE))
				{
					TRACE(_T("Can't import menu\n"));
					return FALSE;
				}


			}else
			{

				if (!pMenuBar->ImportFromMenu (m_hMenu, !HideRarelyUsedCommands ()))
				{
					TRACE(_T("Can't import menu\n"));
					return FALSE;
				}
			}
		}
	}

	POSITION pos;

	//----------------------------------------
	// Maybe, we should process the MRU files:
	//----------------------------------------
	CRecentFileList* pMRUFiles = 
		((CBCGPApp*) AfxGetApp ())->m_pRecentFileList;

	if (pMRUFiles != NULL && !CBCGPToolBar::IsCustomizeMode ())
	{
		int iMRUItemIndex = 0;
		BOOL bIsPrevSeparator = FALSE;

		for (pos = pMenuBar->m_Buttons.GetHeadPosition (); 
			pos != NULL; iMRUItemIndex ++)
		{
			POSITION posSave = pos;

			CBCGPToolbarButton* pButton = 
				(CBCGPToolbarButton*) pMenuBar->m_Buttons.GetNext (pos);
			ASSERT (pButton != NULL);

			if (pButton->m_nID == ID_FILE_MRU_FILE1 &&
				pButton->m_strText == _T("Recent File"))
			{
				//------------------------------
				// Remove dummy item ("Recent"):
				//------------------------------
				pMenuBar->m_Buttons.RemoveAt (posSave);
				delete pButton;

				TCHAR szCurDir [_MAX_PATH];
				::GetCurrentDirectory (_MAX_PATH, szCurDir);

				int nCurDir = lstrlen (szCurDir);
				ASSERT (nCurDir >= 0);

				szCurDir [nCurDir] = _T('\\');
				szCurDir [++ nCurDir] = _T('\0');

				//---------------
				// Add MRU files:
				//---------------
				int iNumOfFiles = 0;	// Actual added to menu
				for (int i = 0; i < pMRUFiles->GetSize (); i ++)
				{
					CString strName;

					if (pMRUFiles->GetDisplayName (strName, i, 
						szCurDir, nCurDir))
					{
						//---------------------
						// Add shortcut number:
						//---------------------
						CString strItem;
						strItem.Format (_T("&%d %s"), ++iNumOfFiles, strName);

						pMenuBar->InsertButton (
							CBCGPToolbarMenuButton (
								ID_FILE_MRU_FILE1 + i, NULL,
								-1, strItem),
							iMRUItemIndex ++);
					}
				}

				//------------------------------------------------------
				// Usualy, the MRU group is "covered" by two seperators.
				// If MRU list is empty, remove redandant separator:
				//------------------------------------------------------
				if (iNumOfFiles == 0 &&	// No files were added
					bIsPrevSeparator &&	// Prev. button was separator
					pos != NULL)		// Not a last button
				{
					posSave = pos;

					pButton = (CBCGPToolbarButton*) 
						pMenuBar->m_Buttons.GetNext (pos);
					ASSERT (pButton != NULL);

					if (pButton->m_nStyle & TBBS_SEPARATOR)
					{
						//---------------------------------------
						// Next button also separator, remove it:
						//---------------------------------------
						pMenuBar->m_Buttons.RemoveAt (posSave);
						delete pButton;
					}
				}

				break;
			}

			bIsPrevSeparator = (pButton->m_nStyle & TBBS_SEPARATOR);
		}
	}

	//--------------------------
	// Setup user-defined tools:
	//--------------------------
	if (g_pUserToolsManager != NULL && !CBCGPToolBar::IsCustomizeMode ())
	{
		BOOL bToolsAreReady = FALSE;
		int iToolItemIndex = 0;

		BOOL bIsPrevSeparator = FALSE;

		for (pos = pMenuBar->m_Buttons.GetHeadPosition (); pos != NULL; iToolItemIndex ++)
		{
			POSITION posSave = pos;

			CBCGPToolbarButton* pButton = 
				(CBCGPToolbarButton*) pMenuBar->m_Buttons.GetNext (pos);
			ASSERT (pButton != NULL);

			if (g_pUserToolsManager->GetToolsEntryCmd () == pButton->m_nID)
			{
				const CObList& lstTools = g_pUserToolsManager->GetUserTools ();

				//----------------------------------------------------
				// Replace dummy tools command by the user tools list:
				//----------------------------------------------------
				if (!m_bAlwaysShowEmptyToolsEntry || !lstTools.IsEmpty ())
				{
					pMenuBar->m_Buttons.RemoveAt (posSave);
					delete pButton;
				}

				if (!bToolsAreReady)
				{
					if (!bIsPrevSeparator && !lstTools.IsEmpty () &&
						!pMenuBar->m_Buttons.IsEmpty ())
					{
						//-------------------------------------
						// Add separator before the first tool:
						//-------------------------------------
						pMenuBar->InsertSeparator (iToolItemIndex++);
					}

					for (POSITION posTool = lstTools.GetHeadPosition (); posTool != NULL;)
					{
						CBCGPUserTool* pTool = (CBCGPUserTool*) lstTools.GetNext (posTool);
						ASSERT_VALID (pTool);

						//----------------------------------------------
						// Is user tool associated with the user image?
						//----------------------------------------------
						int iUserImage = BCGPCMD_MGR.GetCmdImage (pTool->GetCommandId (), TRUE);

						pMenuBar->InsertButton (
							CBCGPToolbarMenuButton (
								pTool->GetCommandId (), NULL,
								iUserImage == -1 ? 0 : iUserImage, pTool->m_strLabel,
								iUserImage != -1),
							iToolItemIndex ++);
					}

					if (pos != NULL)
					{
						//-------------------------------------
						// Add separator after the last tool:
						//-------------------------------------
						bIsPrevSeparator = pMenuBar->InsertSeparator (iToolItemIndex++) >= 0;
					}

					bToolsAreReady = TRUE;
				}
			}
			else if (pButton->m_nStyle & TBBS_SEPARATOR)
			{
				if (bIsPrevSeparator)
				{
					pMenuBar->m_Buttons.RemoveAt (posSave);
					delete pButton;
				}

				bIsPrevSeparator = TRUE;
			}
			else	
			{
				bIsPrevSeparator = FALSE;
			}
		}
	}

    CFrameWnd* pTarget = (CFrameWnd*) pMenuBar->GetCommandTarget ();
	if (pTarget == NULL || !pTarget->IsFrameWnd())
	{
		pTarget = BCGPGetParentFrame(this);
	}
	if (pTarget != NULL)
	{
		pMenuBar->OnUpdateCmdUI(pTarget, FALSE);
	}

	//-----------------------------------------------------------------------------
	// Maybe, main application frame should update the popup menu context before it
	// displayed (example - windows list):
	//-----------------------------------------------------------------------------
	if (!ActivatePopupMenu (BCGCBProGetTopLevelFrame (this), this))
	{
		return FALSE;
	}

	RecalcLayout ();
	return TRUE;
}
//************************************************************************************
BOOL CBCGPPopupMenu::HideRarelyUsedCommands () const
{
	return (m_pParentBtn != NULL);
}
//************************************************************************************
void CBCGPPopupMenu::UpdateBottomWindows (BOOL bCheckOnly)
{
	CWnd* pWndMain = GetTopLevelParent ();
	if (pWndMain == NULL)
	{
		return;
	}

	if (m_iShadowSize == 0 || GetForceShadow ())
	{
		pWndMain->UpdateWindow ();
		return;
	}

	//---------------------------------------------------------
	// If menu will be shown outside of the application window,
	// don't show shadows!
	//---------------------------------------------------------
	CRect rectMain;
	pWndMain->GetWindowRect (rectMain);

	CRect rectMenu (m_ptLocation, 
		CSize (m_FinalSize.cx + m_iShadowSize, m_FinalSize.cy + m_iShadowSize));

	if (GetExStyle() & WS_EX_LAYOUTRTL)
	{
		rectMenu.OffsetRect (-(m_FinalSize.cx + m_iShadowSize), 0);
	}

	CRect rectInter;
	rectInter.UnionRect (&rectMenu, &rectMain);

	if (rectInter != rectMain)
	{
		m_iShadowSize = 0;

		if (!bCheckOnly)
		{
			SetWindowPos (NULL, -1, -1, m_FinalSize.cx, m_FinalSize.cy,
						SWP_NOMOVE | SWP_NOZORDER |
						SWP_NOACTIVATE);
		}
	}
	else
	{
		pWndMain->UpdateWindow ();
	}
}
//*************************************************************************************
void CBCGPPopupMenu::DoPaint (CDC* pPaintDC)
{
	CRect rectClient;	// Client area rectangle
	GetClientRect (&rectClient);

	BOOL bDrawShadows = m_iShadowSize != 0 &&
						!CBCGPToolBar::IsCustomizeMode ();

	if (bDrawShadows)
	{
		BOOL bRTL = GetExStyle() & WS_EX_LAYOUTRTL;

		if (bRTL)
		{
			rectClient.left += m_iShadowSize;
		}
		else
		{
			rectClient.right -= m_iShadowSize;
		}

		rectClient.bottom -= m_iShadowSize;

		const int iMinBrightness = 100;
		const int iMaxBrightness = 65;

		//--------------------------------------------
		// Draw the shadow, exclude the parent button:
		//--------------------------------------------
		CRect rectParentBtn;
		rectParentBtn.SetRectEmpty ();

		if (m_pParentBtn != NULL && GetParentPopupMenu () == NULL)
		{
			CWnd* pWnd = m_pParentBtn->GetParentWnd();
			if (pWnd!= NULL && pWnd->GetSafeHwnd() != NULL)
			{
				rectParentBtn = m_pParentBtn->Rect ();
				rectParentBtn.right--;
				rectParentBtn.bottom--;
				pWnd->MapWindowPoints(this, &rectParentBtn);
			}
		}

		//--------------------------------------------------------------
		// Prevent shadow drawing over Quick Customize Add-Remove button
		//--------------------------------------------------------------
		if (CBCGPVisualManager::GetInstance ()->IsOfficeXPStyleMenus ())
		{
			CBCGPPopupMenu* pParentPopup = GetParentPopupMenu ();
			if (pParentPopup != NULL)
			{
				ASSERT_VALID (pParentPopup);

				CBCGPToolbarMenuButton* pParentBtn = GetParentButton ();
				if ((pParentBtn != NULL) && (pParentBtn->IsQuickMode ()))
				{
					if (pParentPopup->IsQuickCustomize ())
					{
						if (!m_bQuickCusomize && (m_DropDirection == DROP_DIRECTION_LEFT))
						{
							CWnd* pWnd = m_pParentBtn->GetParentWnd ();
							if (pWnd != NULL && pWnd->GetSafeHwnd () != NULL)
							{
								rectParentBtn = m_pParentBtn->Rect ();
								rectParentBtn.bottom += 2;
								pWnd->MapWindowPoints (this, &rectParentBtn);
							}
						}
					}
				}
			}
		}

		if (globalData.bIsWindowsVista)
		{
			CClientDC dcDesktop (NULL);

			CRect rectMenu;
			GetWindowRect (rectMenu);

			pPaintDC->BitBlt (0, 0,
					rectMenu.Width(), rectMenu.Height(),
					&dcDesktop, rectMenu.left, rectMenu.top, SRCCOPY);

			if (bRTL)
			{
				CBCGPDrawManager dm (*pPaintDC);
				dm.MirrorRect (CRect (0, 0, 
					rectMenu.Width (), rectMenu.Height()));
			}
		}

		CBCGPVisualManager::GetInstance ()->OnDrawMenuShadow (
			pPaintDC, rectClient, rectParentBtn,
			m_iShadowSize, iMinBrightness, iMaxBrightness,
			&m_bmpShadowBottom, &m_bmpShadowRight, bRTL);

		if (bRTL)
		{
			rectClient.OffsetRect(-m_iShadowSize,0);
		}
	}

	CBCGPVisualManager::GetInstance ()->OnDrawMenuBorder (pPaintDC, this, rectClient);
	const int nBorderSize = GetBorderSize ();

	rectClient.DeflateRect (nBorderSize, nBorderSize);

	//---------------------------
	// Draw menu logo (if exist):
	//---------------------------
	if (m_iLogoWidth > 0)
	{
		CRect rectLogo = rectClient;

		switch (m_nLogoLocation)
		{
		case MENU_LOGO_LEFT:
			rectLogo.right = rectLogo.left + nBorderSize + m_iLogoWidth;
			break;

		case MENU_LOGO_RIGHT:
			rectLogo.left = rectLogo.right - nBorderSize - m_iLogoWidth;
			break;

		case MENU_LOGO_TOP:
			rectLogo.bottom = rectLogo.top + nBorderSize + m_iLogoWidth;
			break;

		case MENU_LOGO_BOTTOM:
			rectLogo.top = rectLogo.bottom - nBorderSize - m_iLogoWidth;
			break;
		}

		CFrameWnd* pWndMain = BCGCBProGetTopLevelFrame (this);

		CBCGPMDIFrameWnd* pMainFrame = DYNAMIC_DOWNCAST (CBCGPMDIFrameWnd, pWndMain);
		if (pMainFrame != NULL)
		{
			pMainFrame->OnDrawMenuLogo (pPaintDC, this, rectLogo);
		}
		else	// Maybe, SDI frame...
		{
			CBCGPFrameWnd* pFrame = DYNAMIC_DOWNCAST (CBCGPFrameWnd, pWndMain);
			if (pFrame != NULL)
			{
				pFrame->OnDrawMenuLogo (pPaintDC, this, rectLogo);
			}
			else	// Maybe, OLE frame...
			{
				CBCGPOleIPFrameWnd* pOleFrame = 
					DYNAMIC_DOWNCAST (CBCGPOleIPFrameWnd, pWndMain);
				if (pOleFrame != NULL)
				{
					pOleFrame->OnDrawMenuLogo (pPaintDC, this, rectLogo);
				}
				else
				{
					CBCGPOleDocIPFrameWnd* pOleDocFrame = 
						DYNAMIC_DOWNCAST (CBCGPOleDocIPFrameWnd, pWndMain);
					if (pOleDocFrame != NULL)
					{
						pOleDocFrame->OnDrawMenuLogo (pPaintDC, this, rectLogo);
					}
				}

			}
		}
	}

	if (!m_rectTearOffCaption.IsRectEmpty ())
	{
		CBCGPVisualManager::GetInstance ()->OnDrawTearOffCaption (
			pPaintDC, m_rectTearOffCaption, m_bIsTearOffCaptionActive);
	}

	if (m_bScrollable)
	{
		if (IsScrollUpAvailable ())
		{
			CBCGPVisualManager::GetInstance ()->OnDrawMenuScrollButton (
				pPaintDC, m_rectScrollUp, FALSE, m_iScrollMode < 0, FALSE, FALSE);
		}

		if (IsScrollDnAvailable ())
		{
			if (GetMenuBar () != NULL)
			{
				CRect rectFill = rectClient;
				rectFill.bottom = m_rectScrollDn.top;
				rectFill.top = rectFill.bottom - nBorderSize - 1;

				CBCGPVisualManager::GetInstance ()->OnFillBarBackground (pPaintDC, GetMenuBar (),
					rectFill, rectFill);
			}

			CBCGPVisualManager::GetInstance ()->OnDrawMenuScrollButton (
				pPaintDC, m_rectScrollDn, TRUE, m_iScrollMode > 0, FALSE, FALSE);

		}
	}

	if (!m_rectResize.IsRectEmpty ())
	{
		MENU_RESIZE_TYPE resizeType;

		if (m_bIsResizeBarOnTop)
		{
			resizeType = m_sizeMinResize.cx > 0 ? MENU_RESIZE_TOP_RIGHT : MENU_RESIZE_TOP;
		}
		else
		{
			resizeType = m_sizeMinResize.cx > 0 ? MENU_RESIZE_BOTTOM_RIGHT : MENU_RESIZE_BOTTOM;
		}

		CBCGPVisualManager::GetInstance ()->OnDrawMenuResizeBar (
			pPaintDC, m_rectResize, resizeType);
	}

	m_bShown = TRUE;
}
//*************************************************************************************
void CBCGPPopupMenu::DrawFade (CDC* pPaintDC)
{
	CRect rectClient;
	GetClientRect (&rectClient);

	int cx = m_FinalSize.cx + m_iShadowSize;
	int cy = m_FinalSize.cy + m_iShadowSize;

	CDC dcMem;
	if (!dcMem.CreateCompatibleDC (pPaintDC))
	{
		return;
	}

	// create the three bitmaps if not done yet
	if (m_bmpScreenDst.GetSafeHandle() == NULL)
	{
		CBitmap* pBmpOld = NULL;

		if (GetAnimationType () == FADE || globalData.m_nBitsPerPixel > 8)
		{
			// Fill in the BITMAPINFOHEADER
			BITMAPINFOHEADER bih;
			bih.biSize = sizeof(BITMAPINFOHEADER);
			bih.biWidth = cx;
			bih.biHeight = cy;
			bih.biPlanes = 1;
			bih.biBitCount = 32;
			bih.biCompression = BI_RGB;
			bih.biSizeImage = cx * cy;
			bih.biXPelsPerMeter = 0;
			bih.biYPelsPerMeter = 0;
			bih.biClrUsed = 0;
			bih.biClrImportant = 0;

			HBITMAP hmbpDib;
			// Create a DIB section and attach it to the source bitmap
			hmbpDib = CreateDIBSection (
				dcMem.m_hDC, (LPBITMAPINFO)&bih, DIB_RGB_COLORS, (void **)&m_cFadeSrcBits,
				NULL, NULL);
			if (hmbpDib == NULL || m_cFadeSrcBits == NULL)
			{
				return;
			}

			m_bmpScreenSrc.Attach( hmbpDib );

			// Create a DIB section and attach it to the destination bitmap
			hmbpDib = CreateDIBSection (
				dcMem.m_hDC, (LPBITMAPINFO)&bih, DIB_RGB_COLORS, (void **)&m_cFadeDstBits,
				NULL, NULL);
			if (hmbpDib == NULL || m_cFadeDstBits == NULL)
			{
				return;
			}
			m_bmpScreenDst.Attach( hmbpDib );

			// Create a DIB section and attach it to the temporary bitmap
			hmbpDib = CreateDIBSection (
				dcMem.m_hDC, (LPBITMAPINFO)&bih, DIB_RGB_COLORS, (void **)&m_cFadeTmpBits,
				NULL, NULL);
			if (hmbpDib == NULL || m_cFadeTmpBits == NULL)
			{
				return;
			}

			m_bmpScreenTmp.Attach( hmbpDib );

			// get source image, representing the window below the popup menu
			pBmpOld = dcMem.SelectObject (&m_bmpScreenSrc);
			dcMem.BitBlt (0, 0, cx, cy, pPaintDC, rectClient.left, rectClient.top, SRCCOPY);

			// copy it to the destination so that shadow will be ok
			memcpy (m_cFadeDstBits, m_cFadeSrcBits, sizeof(COLORREF)* cx*cy);
			dcMem.SelectObject (&m_bmpScreenDst);
		}
		else
		{
			m_bmpScreenDst.CreateCompatibleBitmap (pPaintDC, cx, cy);
			pBmpOld = dcMem.SelectObject (&m_bmpScreenDst);
		}

		// get final image
		CRect rect;

		DoPaint (&dcMem);

		CBCGPPopupMenuBar* pMenuBar = GetMenuBar ();
		ASSERT_VALID (pMenuBar);

		pMenuBar->GetWindowRect (&rect);
		ScreenToClient (&rect);

		dcMem.SetViewportOrg (rect.TopLeft());
		pMenuBar->DoPaint (&dcMem);
		dcMem.SetViewportOrg (CPoint (0,0));

		dcMem.SelectObject (pBmpOld);
	}
	
	COLORREF *src = m_cFadeSrcBits;
	COLORREF *dst = m_cFadeDstBits;
	COLORREF *tmp = m_cFadeTmpBits;

	CBitmap* pBmpOld = NULL;

	switch (GetAnimationType ())
	{
	case UNFOLD:
	case SLIDE:
		pBmpOld = dcMem.SelectObject (&m_bmpScreenDst);

		pPaintDC->BitBlt (
			m_bIsAnimRight ? rectClient.left : rectClient.right - m_AnimSize.cx,
			m_bIsAnimDown ? rectClient.top : rectClient.bottom - m_AnimSize.cy,
			m_AnimSize.cx, m_AnimSize.cy, &dcMem, 0, 0, SRCCOPY);
		break;

	case FADE:
		pBmpOld = dcMem.SelectObject (&m_bmpScreenTmp);
		for (int pixel = 0; pixel < cx * cy; pixel++)
		{
			*tmp++ = CBCGPDrawManager::PixelAlpha (*src++, *dst++, 100 - m_iFadePercent);
		}

		pPaintDC->BitBlt (rectClient.left, rectClient.top, cx, cy, 
						&dcMem, 0, 0, SRCCOPY);
	}

	dcMem.SelectObject (pBmpOld);
}

//************************************************************************************
BOOL CBCGPPopupMenu::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (!CMiniFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
	{
		return m_pMessageWnd != NULL ? 
			m_pMessageWnd->OnCmdMsg (nID, nCode, pExtra, pHandlerInfo) : FALSE;
	}

	return TRUE;
}
//************************************************************************************
BOOL CBCGPPopupMenu::PostCommand (UINT commandID)
{
	if (m_pMessageWnd != NULL)
	{
		BOOL bIsSysCommand = (commandID >= 0xF000 && commandID < 0xF1F0);
		return m_pMessageWnd->PostMessage (bIsSysCommand ? WM_SYSCOMMAND : WM_COMMAND, commandID);
	}

	return FALSE;
}
//************************************************************************************
void CBCGPPopupMenu::EnableMenuLogo (int iLogoSize, LOGO_LOCATION nLogoLocation/* = MENU_LOGO_LEFT*/)
{
	m_iLogoWidth = iLogoSize;
	m_nLogoLocation = nLogoLocation;

	RecalcLayout ();
}
//************************************************************************************
BOOL CBCGPPopupMenu::AdjustScroll (BOOL bForceMenuBarResize)
{
	CBCGPPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	CRect rectClient;
	GetClientRect (rectClient);

	if (!CBCGPToolBar::IsCustomizeMode ())
	{
		rectClient.right -= m_iShadowSize;
		rectClient.bottom -= m_iShadowSize;
	}

	const int nBorderSize = GetBorderSize ();
	rectClient.DeflateRect (nBorderSize, nBorderSize);

	switch (m_nLogoLocation)
	{
	case MENU_LOGO_LEFT:
		rectClient.left += m_iLogoWidth;
		break;

	case MENU_LOGO_RIGHT:
		rectClient.right -= m_iLogoWidth;
		break;

	case MENU_LOGO_TOP:
		rectClient.top += m_iLogoWidth;
		break;

	case MENU_LOGO_BOTTOM:
		rectClient.bottom -= m_iLogoWidth;
		break;
	}

	rectClient.top += m_rectTearOffCaption.Height ();

	if (m_bIsResizeBarOnTop)
	{
		rectClient.top += m_rectResize.Height ();
	}
	else
	{
		rectClient.bottom -= m_rectResize.Height ();
	}

	CRect rectScrollUpOld = m_rectScrollUp;
	CRect rectScrollDnOld = m_rectScrollDn;

	m_rectScrollUp.SetRectEmpty ();
	m_rectScrollDn.SetRectEmpty ();

	UINT uiSWPFlags = SWP_NOZORDER | SWP_NOACTIVATE;

	if (m_bScrollable)
	{
		if (m_bShowScrollBar)
		{
			rectClient.right -= ::GetSystemMetrics (SM_CXVSCROLL);
		}
		else
		{
			if (IsScrollUpAvailable ())
			{
				m_rectScrollUp = rectClient;
				m_rectScrollUp.top += nBorderSize;
				m_rectScrollUp.bottom = m_rectScrollUp.top + m_iScrollBtnHeight;

				rectClient.top += m_iScrollBtnHeight + nBorderSize;
			}

			if (IsScrollDnAvailable ())
			{
				m_rectScrollDn = rectClient;
				m_rectScrollDn.top = m_rectScrollDn.bottom - m_iScrollBtnHeight;

				rectClient.bottom -= m_iScrollBtnHeight + nBorderSize;
			}
		}
	}
	else if (!m_bAnimationIsDone)
	{
		uiSWPFlags |= SWP_NOREDRAW;

		KillTimer (iScrollTimerId);
		m_iScrollMode = 0;
	}

	if (bForceMenuBarResize ||
		rectScrollUpOld != m_rectScrollUp ||
		rectScrollDnOld != m_rectScrollDn)
	{
		pMenuBar->SetWindowPos (NULL, rectClient.left, rectClient.top,
					rectClient.Width (), 
					rectClient.Height (),
					uiSWPFlags);
		m_nMenuBarHeight = rectClient.Height ();
	}
	else
	{
		pMenuBar->AdjustLayout ();
	}

	BOOL bScrollButtonsChanged = FALSE;

	if (rectScrollUpOld != m_rectScrollUp)
	{
		InvalidateRect (rectScrollUpOld);
		InvalidateRect (m_rectScrollUp);

		bScrollButtonsChanged = TRUE;
	}

	if (rectScrollDnOld != m_rectScrollDn)
	{
		InvalidateRect (rectScrollDnOld);
		InvalidateRect (m_rectScrollDn);

		bScrollButtonsChanged = TRUE;
	}

	if (bScrollButtonsChanged)
	{
		UpdateWindow ();
	}

	return bScrollButtonsChanged;
}
//************************************************************************************
CBCGPPopupMenu::MENUAREA_TYPE CBCGPPopupMenu::CheckArea (const CPoint& ptScreen) const
{
	ASSERT_VALID (this);

	CRect rectWindow;
	GetClientRect (rectWindow);
	ClientToScreen (rectWindow);

	if (!rectWindow.PtInRect (ptScreen))
	{
		return OUTSIDE;
	}

	CRect rectLogo = rectWindow;

	switch (m_nLogoLocation)
	{
	case MENU_LOGO_LEFT:
		rectLogo.right = rectLogo.left + m_iLogoWidth;
		break;

	case MENU_LOGO_RIGHT:
		rectLogo.left = rectLogo.right - m_iLogoWidth;
		break;

	case MENU_LOGO_TOP:
		rectLogo.bottom = rectLogo.top + m_iLogoWidth;
		break;

	case MENU_LOGO_BOTTOM:
		rectLogo.top = rectLogo.bottom - m_iLogoWidth;
		break;
	}

	if (rectLogo.PtInRect (ptScreen))
	{
		return LOGO;
	}

	if (ptScreen.x > rectWindow.right - m_iShadowSize)
	{
		return SHADOW_RIGHT;
	}

	if (ptScreen.y > rectWindow.bottom - m_iShadowSize)
	{
		return SHADOW_BOTTOM;
	}

	if (!m_rectTearOffCaption.IsRectEmpty ())
	{
		CRect rectTearOffCaption = m_rectTearOffCaption;
		ClientToScreen (&rectTearOffCaption);

		if (rectTearOffCaption.PtInRect (ptScreen))
		{
			return TEAROFF_CAPTION;
		}
	}

	return MENU;
}
//************************************************************************************
void CBCGPPopupMenu::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (!CBCGPToolBar::IsCustomizeMode () &&
		m_rectTearOffCaption.PtInRect (point))
	{
		m_bIsTearOffCaptionActive = TRUE;
		InvalidateRect (m_rectTearOffCaption);

		m_bTearOffTracking = TRUE;
		SetCapture ();
		return;
	}

	CMiniFrameWnd::OnLButtonDown(nFlags, point);
}
//*************************************************************************************
void CBCGPPopupMenu::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bTearOffTracking)
	{
		m_bTearOffTracking = FALSE;
		ReleaseCapture ();

		m_bIsTearOffCaptionActive = FALSE;
		InvalidateRect (m_rectTearOffCaption);
	}

	CMiniFrameWnd::OnLButtonUp(nFlags, point);
}
//*************************************************************************************
BOOL CBCGPPopupMenu::TearOff (CPoint point)
{
	if (m_pParentBtn == NULL)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	UINT uiID = m_pParentBtn->m_uiTearOffBarID;
	if (uiID == 0)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	CPoint ptScreen = point;
	ClientToScreen (&ptScreen);

	CFrameWnd* pWndMain = BCGCBProGetTopLevelFrame (this);
	if (pWndMain == NULL)
	{
		return FALSE;
	}
	CBCGPControlBar* pBar = NULL;
	CBCGPFrameImpl* pFrameImpl = NULL;

	CBCGPMDIFrameWnd* pMainFrame = DYNAMIC_DOWNCAST (CBCGPMDIFrameWnd, pWndMain);
	if (pMainFrame != NULL)
	{
		pBar = (CBCGPControlBar*) pMainFrame->GetControlBar (uiID);
		pFrameImpl = &pMainFrame->m_Impl;
	}
	else	// Maybe, SDI frame...
	{
		CBCGPFrameWnd* pFrame = DYNAMIC_DOWNCAST (CBCGPFrameWnd, pWndMain);
		if (pFrame != NULL)
		{
			pBar = (CBCGPControlBar*) pFrame->GetControlBar (uiID);
			pFrameImpl = &pFrame->m_Impl;
		}
		else	// Maybe, OLE frame...
		{
			CBCGPOleIPFrameWnd* pOleFrame = 
				DYNAMIC_DOWNCAST (CBCGPOleIPFrameWnd, pWndMain);
			if (pOleFrame != NULL)
			{
				pBar = (CBCGPControlBar*) pOleFrame->GetControlBar (uiID);
				pFrameImpl = &pOleFrame->m_Impl;
			}
			else
			{
				CBCGPOleDocIPFrameWnd* pOleDocFrame = 
					DYNAMIC_DOWNCAST (CBCGPOleDocIPFrameWnd, pWndMain);
				if (pOleDocFrame != NULL)
				{
					pBar = (CBCGPControlBar*) pOleDocFrame->GetControlBar (uiID);
					pFrameImpl = &pOleDocFrame->m_Impl;
				}
			}
		}
	}
	
	if (pFrameImpl == NULL)
	{
		return FALSE;
	}

	if (pBar != NULL)	// Already exist, just show it
	{
		pBar->ShowControlBar (TRUE, FALSE, TRUE);

		CRect rectBar;
		pBar->GetWindowRect (rectBar);

		int cx = rectBar.Width ();
		rectBar.left = ptScreen.x;
		rectBar.right = rectBar.left + cx;

		int cy = rectBar.Height ();
		rectBar.top = ptScreen.y;
		rectBar.bottom = rectBar.top + cy;

		if (pBar->IsDocked ())
		{
			pBar->FloatControlBar (rectBar, BCGP_DM_SHOW);
		}
		else
		{
			pBar->MoveWindow (rectBar);
		}
	}
	else
	{
		CString strCaption = m_pParentBtn->m_strText;
		strCaption.Remove (_T('&'));

		if ((pBar = CreateTearOffBar (pWndMain, uiID, strCaption)) == NULL)
		{
			return FALSE;
		}

		pFrameImpl->AddTearOffToolbar (pBar);
	}

	ASSERT_VALID (pBar);

	//--------------------------------
	// Send trigger to the main frame:
	//--------------------------------
	BOOL bTearOff = TRUE;

	if (pMainFrame != NULL)
	{
		bTearOff = pMainFrame->OnTearOffMenu (this, pBar);
	}
	else	// Maybe, SDI frame...
	{
		CBCGPFrameWnd* pFrame = DYNAMIC_DOWNCAST (CBCGPFrameWnd, pWndMain);
		if (pFrame != NULL)
		{
			bTearOff = pFrame->OnTearOffMenu (this, pBar);
		}
		else	// Maybe, OLE frame...
		{
			CBCGPOleIPFrameWnd* pOleFrame = 
				DYNAMIC_DOWNCAST (CBCGPOleIPFrameWnd, pWndMain);
			if (pOleFrame != NULL)
			{
				bTearOff = pOleFrame->OnTearOffMenu (this, pBar);
			}
			else
			{
				CBCGPOleDocIPFrameWnd* pOleDocFrame = 
					DYNAMIC_DOWNCAST (CBCGPOleDocIPFrameWnd, pWndMain);
				if (pOleDocFrame != NULL)
				{
					bTearOff = pOleDocFrame->OnTearOffMenu (this, pBar);
				}
			}
		}
	}

	if (!bTearOff)
	{
		pBar->DestroyWindow ();
		delete pBar;

		return FALSE;
	}

	pBar->OnUpdateCmdUI (pWndMain, TRUE);

	CRect rectBar (ptScreen, pBar->CalcSize (FALSE));
	pBar->FloatControlBar (rectBar, BCGP_DM_SHOW);
	pBar->RecalcLayout ();
	pWndMain->RecalcLayout ();

	CBCGPMiniFrameWnd* pFloatFrame = pBar->GetParentMiniFrame (TRUE);
	if (pFloatFrame != NULL)
	{
		return pFloatFrame->StartTearOff (this);
	}

	return FALSE;
}
//*************************************************************************************
CBCGPControlBar* CBCGPPopupMenu::CreateTearOffBar (CFrameWnd* pWndMain, UINT uiID,
											  LPCTSTR lpszName)
{
	ASSERT_VALID (pWndMain);
	ASSERT (lpszName != NULL);
	ASSERT (uiID != 0);

	if (m_hMenu == NULL)
	{
		return NULL;
	}

	CMenu* pMenu = CMenu::FromHandle (m_hMenu);
	if (pMenu == NULL)
	{
		return NULL;
	}

	CBCGPToolBar* pNewToolbar = new CBCGPToolBar;
	if (!pNewToolbar->Create (pWndMain,
		dwDefaultToolbarStyle,
		uiID))
	{
		TRACE0 ("Failed to create a new toolbar!\n");
		delete pNewToolbar;
		return NULL;
	}

	pNewToolbar->SetWindowText (lpszName);

	int iCount = (int) pMenu->GetMenuItemCount ();
	for (int i = 0; i < iCount; i ++)
	{
		UINT uiCmd = pMenu->GetMenuItemID (i);

		CString strText;
		pMenu->GetMenuString (i, strText, MF_BYPOSITION);

		switch (uiCmd)
		{
		case 0:
			if (i != iCount - 1)
			{
				pNewToolbar->InsertSeparator ();
			}
			break;

		case -1:
			{
				UINT uiTearOffId = 0;
				if (g_pBCGPTearOffMenuManager != NULL)
				{
					uiTearOffId = g_pBCGPTearOffMenuManager->Parse (strText);
				}

				//---------------
				// Remove hotkey:
				//---------------
				int iTabOffset = strText.Find (_T('\t'));
				if (iTabOffset >= 0)
				{
					strText = strText.Left (iTabOffset);
				}

				CBCGPToolbarMenuButton menuButton ((UINT) -1, 
					pMenu->GetSubMenu (i)->GetSafeHmenu (),
					-1, strText);
				if (menuButton.GetImage () == -1)
				{
					menuButton.m_bImage = FALSE;
					menuButton.m_bText = TRUE;
				}

				menuButton.SetTearOff (uiTearOffId);
				pNewToolbar->InsertButton (menuButton);
			}
			break;

		default:
			if (!IsBCGPStandardCommand (uiCmd))
			{
				CBCGPToolbarButton button (uiCmd, -1, strText);
				if (button.GetImage () == -1)
				{
					button.m_bImage = FALSE;
					button.m_bText = TRUE;
				}

				pNewToolbar->InsertButton (button);
			}
		}
	}

	pNewToolbar->SetBarStyle (pNewToolbar->GetBarStyle () |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	pNewToolbar->EnableDocking (CBRS_ALIGN_ANY);

	if (m_pParentBtn != NULL && m_pParentBtn->IsMenuPaletteMode ())
	{
		int nColumns = iCount / m_pParentBtn->GetPaletteRows () + 1;
		int cx = pNewToolbar->GetColumnWidth () * nColumns;

		pNewToolbar->StretchControlBar (cx, FALSE);
	}

	return pNewToolbar;
}
//*************************************************************************************
BOOL CBCGPPopupMenu::PreTranslateMessage(MSG* pMsg) 
{
	if (m_wndToolTip.GetSafeHwnd () != NULL)
	{
		m_wndToolTip.RelayEvent (pMsg);
	}

	if (pMsg->message == WM_MOUSEMOVE &&
		(!m_rectScrollUp.IsRectEmpty () || !m_rectScrollDn.IsRectEmpty ()))
	{
		CPoint ptCursor;
		::GetCursorPos (&ptCursor);
		ScreenToClient (&ptCursor);

		if (m_rectScrollUp.PtInRect (ptCursor) || m_rectScrollDn.PtInRect (ptCursor))
		{
			OnMouseMove ((UINT)pMsg->wParam, ptCursor);
			return TRUE;
		}
	}

	return CMiniFrameWnd::PreTranslateMessage(pMsg);
}
//*************************************************************************************
BOOL CBCGPPopupMenu::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR* pNMHDR = (NMHDR*) lParam;
	ASSERT (pNMHDR != NULL);

	if (pNMHDR->code == TTN_SHOW)
	{
		m_bIsTearOffCaptionActive = TRUE;
		InvalidateRect (m_rectTearOffCaption);
		UpdateWindow ();
	}
	else if (pNMHDR->code == TTN_POP)
	{
		m_bIsTearOffCaptionActive = FALSE;
		InvalidateRect (m_rectTearOffCaption);
		UpdateWindow ();
	}
	
	return CMiniFrameWnd::OnNotify(wParam, lParam, pResult);
}
//**************************************************************************************
void CBCGPPopupMenu::SetAnimationSpeed (UINT nElapse)
{
	if (nElapse == 0 || nElapse > 200)
	{
		ASSERT (FALSE);
		return;
	}

	m_AnimationSpeed = nElapse;
}
//**************************************************************************************
BOOL CBCGPPopupMenu::NotifyParentDlg (BOOL bActivate)
{
	CBCGPDialog* pDlg = DYNAMIC_DOWNCAST (CBCGPDialog, m_pMessageWnd);
	CBCGPPropertyPage* pPropPage = DYNAMIC_DOWNCAST (CBCGPPropertyPage, m_pMessageWnd);

	if (pDlg == NULL && pPropPage == NULL)
	{
		return FALSE;
	}

	if (!bActivate && m_pActivePopupMenu != this)
	{
		return FALSE;
	}

	if (pDlg != NULL)
	{
		pDlg->SetActiveMenu (bActivate ? this : NULL);
	}

	if (pPropPage != NULL)
	{
		pPropPage->SetActiveMenu (bActivate ? this : NULL);
	}

	return TRUE;
}
//*******************************************************************************
void CBCGPPopupMenu::UpdateShadow (LPRECT lprectScreen)
{
	ASSERT_VALID (this);

	if (m_iShadowSize <= 0)
	{
		// No menu shadow, nothing to do.
		return;
	}

	CWnd* pWndMain = GetTopLevelParent ();
	if (pWndMain->GetSafeHwnd () == NULL)
	{
		return;
	}

	const BOOL bIsRTL = GetExStyle() & WS_EX_LAYOUTRTL;

	ASSERT_VALID (pWndMain);

	CRect rectClient;
	GetClientRect (rectClient);

	CRect rectUpdate1 = rectClient;
	CRect rectUpdate2 = rectClient;

	if (lprectScreen != NULL)
	{
		CRect rectRedraw = lprectScreen;
		ScreenToClient (&rectRedraw);

		CRect rectShadowRight = rectClient;

		if (bIsRTL)
		{
			rectShadowRight.right = rectShadowRight.left + m_iShadowSize + 1;
		}
		else
		{
			rectShadowRight.left = rectShadowRight.right - m_iShadowSize - 1;
		}

		if (!rectUpdate1.IntersectRect (rectRedraw, rectShadowRight))
		{
			rectUpdate1.SetRectEmpty ();
		}

		CRect rectShadowBottom = rectClient;
		rectShadowBottom.top = rectShadowBottom.bottom - m_iShadowSize - 1;

		if (!rectUpdate2.IntersectRect (rectRedraw, rectShadowBottom))
		{
			rectUpdate2.SetRectEmpty ();
		}

		if (rectUpdate1.IsRectEmpty () && rectUpdate2.IsRectEmpty ())
		{
			return;
		}
	}

	CRect rectMenu;
	GetWindowRect (rectMenu);

	if (!m_bForceShadow)
	{
		CRect rectMain;
		pWndMain->GetWindowRect (rectMain);

		CRect rectInter;
		rectInter.UnionRect (&rectMenu, &rectMain);

		if (rectInter != rectMain)
		{
			return;
		}
	}

	int iShadowSize = m_iShadowSize;
	m_iShadowSize = 0;

	//------------------------------------------------------------
	// m_bmpShadowRight and m_bmpShadowBottom contain the previous
	// screen shots. Delete them now:
	//------------------------------------------------------------
	if (m_bmpShadowRight.GetSafeHandle () != NULL)
	{
		m_bmpShadowRight.DeleteObject ();
	}
	
	if (m_bmpShadowBottom.GetSafeHandle () != NULL)
	{
		m_bmpShadowBottom.DeleteObject ();
	}

	UINT uiSWPFlags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOSENDCHANGING;
	
	CRect rectWindow (-1, -1, -1, -1);

	CBCGPPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	pMenuBar->m_bInUpdateShadow = TRUE;

	if (bIsRTL)
	{
		GetWindowRect (rectWindow);

		SetWindowPos (NULL, rectWindow.left + iShadowSize, rectWindow.top,
			rectClient.Width () - iShadowSize, rectClient.Height () - iShadowSize,
			uiSWPFlags);
	}
	else
	{
		uiSWPFlags |= SWP_NOMOVE;

		//--------------------------------------------------
		// Reduce menu size ("cut" left and bottom shadows):
		//--------------------------------------------------
		SetWindowPos (NULL, -1, -1,
			rectClient.Width () - iShadowSize, rectClient.Height () - iShadowSize,
			uiSWPFlags);
	}


	UINT uiRDWFlags = RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN;

	CRect rectParent = rectUpdate1;

	if (!rectUpdate1.IsRectEmpty ())
	{
		MapWindowPoints (pWndMain, &rectParent);
		pWndMain->RedrawWindow (rectParent, NULL, uiRDWFlags);
	}

	rectParent = rectUpdate2;

	if (!rectUpdate2.IsRectEmpty () && rectUpdate1 != rectUpdate2)
	{
		MapWindowPoints (pWndMain, &rectParent);
		pWndMain->RedrawWindow (rectParent, NULL, uiRDWFlags);
	}

	pWndMain->UpdateWindow ();

	//-----------------------------------------------------------------
	// Restore original size and update windows under the menu shadows:
	//-----------------------------------------------------------------
	m_iShadowSize = iShadowSize;

	if (bIsRTL)
	{
		SetWindowPos (NULL, rectWindow.left, rectWindow.top,
			rectClient.Width (), rectClient.Height (),
			uiSWPFlags);
	}
	else
	{
		SetWindowPos (NULL, -1, -1,
			rectClient.Width (), rectClient.Height (),
			uiSWPFlags);
	}

	if (!rectUpdate1.IsRectEmpty ())
	{
		InvalidateRect (rectUpdate1);
	}

	if (!rectUpdate2.IsRectEmpty () && rectUpdate1 != rectUpdate2)
	{
		InvalidateRect (rectUpdate2);
	}

	UpdateWindow ();

	pMenuBar->m_bInUpdateShadow = FALSE;
}
//*******************************************************************************
void CBCGPPopupMenu::UpdateShadowTransparency (BYTE nTransparency)
{
	if (m_pWndShadow->GetSafeHwnd () != NULL)
	{
		m_pWndShadow->UpdateTransparency (nTransparency);
	}
}
//*******************************************************************************
void CBCGPPopupMenu::UpdateAllShadows (LPRECT lprectScreen)
{
	for (CBCGPPopupMenu* pMenu = m_pActivePopupMenu; pMenu != NULL;
		pMenu = pMenu->GetParentPopupMenu ())
	{
		if (CWnd::FromHandlePermanent (pMenu->GetSafeHwnd ()) != NULL)
		{
			ASSERT_VALID (pMenu);
			pMenu->UpdateShadow (lprectScreen);
		}
	}
}
//*******************************************************************************
void CBCGPPopupMenu::SetQuickMode()
{
	//-------------------------------
	// Store recently used mode state
	//------------------------------
	CBCGPCustomizeMenuButton::m_bRecentlyUsedOld = CBCGPMenuBar::IsRecentlyUsedMenus();
	CBCGPMenuBar::SetRecentlyUsedMenus(FALSE);

	m_bQuickCusomize = TRUE;	
}
//*******************************************************************************
void CBCGPPopupMenu::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CMiniFrameWnd::OnShowWindow(bShow, nStatus);
	
	if (!bShow)
	{
		m_bShown = FALSE;
	}
}
//********************************************************************************
BOOL CBCGPPopupMenu::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CPoint ptCursor;
	::GetCursorPos (&ptCursor);
	ScreenToClient (&ptCursor);

	if (m_rectTearOffCaption.PtInRect (ptCursor))
	{
		if (globalData.m_hcurSizeAll == NULL)
		{
			globalData.m_hcurSizeAll = AfxGetApp ()->LoadStandardCursor (IDC_SIZEALL);
		}

		SetCursor (globalData.m_hcurSizeAll);
		return TRUE;
	}
	
	return CMiniFrameWnd::OnSetCursor(pWnd, nHitTest, message);
}
//**************************************************************************
void CBCGPPopupMenu::SetParentRibbonElement (CBCGPBaseRibbonElement* pElem)
{
	ASSERT_VALID (this);
	
#ifndef BCGP_EXCLUDE_RIBBON
	m_pParentRibbonElement = pElem;
	pElem->m_pPopupMenu = this;
#endif
}
//**************************************************************************
void CBCGPPopupMenu::SetScrollBar ()
{
	if (!m_bShowScrollBar || !m_bScrollable)
	{
		return;
	}

	CBCGPPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	CRect rectClient;
	GetClientRect (rectClient);

	if (!CBCGPToolBar::IsCustomizeMode ())
	{
		rectClient.right -= m_iShadowSize;
		rectClient.bottom -= m_iShadowSize;
	}

	const int nBorderSize = GetBorderSize ();
	rectClient.DeflateRect (nBorderSize, nBorderSize);

	switch (m_nLogoLocation)
	{
	case MENU_LOGO_LEFT:
		rectClient.left += m_iLogoWidth;
		break;

	case MENU_LOGO_RIGHT:
		rectClient.right -= m_iLogoWidth;
		break;

	case MENU_LOGO_TOP:
		rectClient.top += m_iLogoWidth;
		break;

	case MENU_LOGO_BOTTOM:
		rectClient.bottom -= m_iLogoWidth;
		break;
	}

	rectClient.top += m_rectTearOffCaption.Height ();
	
	if (m_bIsResizeBarOnTop)
	{
		rectClient.top += m_rectResize.Height ();
	}
	else
	{
		rectClient.bottom -= m_rectResize.Height ();
	}

	const int cxScroll = ::GetSystemMetrics (SM_CXVSCROLL);

	CRect rectScrollBar = rectClient;
	rectScrollBar.left = rectScrollBar.right - cxScroll;
	rectClient.right -= cxScroll;

	SCROLLINFO si;

	ZeroMemory (&si, sizeof (SCROLLINFO));
	si.cbSize = sizeof (SCROLLINFO);
	si.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;

	if (m_wndScrollBarVert.GetSafeHwnd () == NULL)
	{
		m_wndScrollBarVert.Create (WS_CHILD | WS_VISIBLE | SBS_VERT,
			rectScrollBar, this, nScrollBarID);
	}
	else
	{
		m_wndScrollBarVert.SetWindowPos (NULL, 
			rectScrollBar.left, rectScrollBar.top,
			rectScrollBar.Width (), rectScrollBar.Height (),
			SWP_NOZORDER | SWP_NOACTIVATE);

		m_wndScrollBarVert.GetScrollInfo (&si);
	}

	si.nMin = 0;
	si.nMax = 0;
	si.nPage = 0;

	const int nMenuRowHeight = pMenuBar->GetRowHeight ();
	const int nMenuTotalItems = pMenuBar->GetCount ();

	if (nMenuTotalItems > 0 && nMenuRowHeight > 0)
	{
		si.nMax = (nMenuTotalItems * nMenuRowHeight - m_nMenuBarHeight) / nMenuRowHeight + 1;
		si.nPage = /*m_nMenuBarHeight / nMenuRowHeight*/1;
		
		pMenuBar->m_nDropDownPageSize = m_nMenuBarHeight / nMenuRowHeight;
	}
	else
	{
		pMenuBar->m_nDropDownPageSize = 0;
	}

	m_wndScrollBarVert.SetScrollInfo (&si, TRUE);
	m_wndScrollBarVert.EnableScrollBar (si.nMax > 0 ? ESB_ENABLE_BOTH : ESB_DISABLE_BOTH);
	m_wndScrollBarVert.EnableWindow ();
}
//**************************************************************************
void CBCGPPopupMenu::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (pScrollBar->GetSafeHwnd () != m_wndScrollBarVert.GetSafeHwnd ())
	{
		CMiniFrameWnd::OnVScroll(nSBCode, nPos, pScrollBar);
		return;
	}

	CBCGPPopupMenuBar* pMenuBar = GetMenuBar ();
	ASSERT_VALID (pMenuBar);

	SCROLLINFO scrollInfo;
	ZeroMemory(&scrollInfo, sizeof(SCROLLINFO));

    scrollInfo.cbSize = sizeof(SCROLLINFO);
	scrollInfo.fMask = SIF_ALL;

	m_wndScrollBarVert.GetScrollInfo (&scrollInfo);

	int iOffset = pMenuBar->GetOffset ();
	int nMaxOffset = scrollInfo.nMax;

	if (nMaxOffset == 0)
	{
		const int nMenuRowHeight = pMenuBar->GetRowHeight ();
		const int nMenuTotalItems = pMenuBar->GetCount ();

		if (nMenuTotalItems > 0 && nMenuRowHeight > 0 && nMenuTotalItems)
		{
			nMaxOffset = max(0, (nMenuTotalItems * nMenuRowHeight - m_nMenuBarHeight) / nMenuRowHeight + 1);
		}
	}

	int nPage = scrollInfo.nPage;

	switch (nSBCode)
	{
	case SB_LINEUP:
		iOffset--;
		break;

	case SB_LINEDOWN:
		iOffset++;
		break;

	case SB_TOP:
		iOffset = 0;
		break;

	case SB_BOTTOM:
		iOffset = nMaxOffset;
		break;

	case SB_PAGEUP:
		iOffset -= nPage;
		break;

	case SB_PAGEDOWN:
		iOffset += nPage;
		break;

	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		iOffset = nPos;
		break;

	default:
		return;
	}

	iOffset = min (max (0, iOffset), nMaxOffset);

	if (iOffset == pMenuBar->GetOffset ())
	{
		return;
	}

	pMenuBar->SetOffset (iOffset);

	if (m_wndScrollBarVert.GetSafeHwnd () != NULL)
	{
		m_wndScrollBarVert.SetScrollPos (iOffset);
	}

	AdjustScroll ();
}
//**************************************************************************
BOOL CBCGPPopupMenu::OnMouseWheel(UINT /*nFlags*/, short zDelta, CPoint /*pt*/) 
{
	ASSERT_VALID (this);

	if (GetActiveMenu () != this || !m_bScrollable)
	{
		return TRUE;
	}

	const int nSteps = abs(zDelta) / WHEEL_DELTA;

	for (int i = 0; i < nSteps; i++)
	{
		OnVScroll (zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0, &m_wndScrollBarVert);
	}

	return TRUE;
}
//**************************************************************************
CWnd* CBCGPPopupMenu::GetParentArea (CRect& rectParentBtn)
{
	ASSERT_VALID (this);

	if (m_pParentBtn != NULL)
	{
		ASSERT_VALID (m_pParentBtn);

		CWnd* pWndParent = m_pParentBtn->GetParentWnd ();

		if (pWndParent != NULL)
		{
			rectParentBtn = m_pParentBtn->Rect ();
			return pWndParent;
		}
	}
#ifndef BCGP_EXCLUDE_RIBBON
	else if (m_pParentRibbonElement != NULL)
	{
		ASSERT_VALID (m_pParentRibbonElement);

		CWnd* pWndParent = m_pParentRibbonElement->GetParentWnd ();

		if (pWndParent != NULL)
		{
			rectParentBtn = m_pParentRibbonElement->GetRect ();
			return pWndParent;
		}
	}
#endif

	return NULL;
}
//**************************************************************************
int CBCGPPopupMenu::GetBorderSize () const
{
	return CBCGPVisualManager::GetInstance ()->GetPopupMenuBorderSize ();
}
//**************************************************************************
BCGNcHitTestType CBCGPPopupMenu::OnNcHitTest(CPoint point) 
{
	BOOL bRTL = GetExStyle() & WS_EX_LAYOUTRTL;

	// hit test the size box - convert to HTCAPTION if so
	if (!m_rectResize.IsRectEmpty ())
	{
		CRect rect = m_rectResize;

		if (m_sizeMinResize.cx <= 0)
		{
			// Vertical resizing:
			ClientToScreen(&rect);

			if (rect.PtInRect (point))
			{
				return m_bIsResizeBarOnTop ? HTTOP : HTBOTTOM;
			}
		}
		else
		{
			rect.left = rect.right - rect.Height ();

			ClientToScreen(&rect);

			if (rect.PtInRect (point))
			{
				if (m_bIsResizeBarOnTop)
				{
					return bRTL ? HTTOPLEFT : HTTOPRIGHT;
				}
				else
				{
					return bRTL ? HTBOTTOMLEFT : HTBOTTOMRIGHT;
				}
			}
		}
	}
	
	return CMiniFrameWnd::OnNcHitTest(point);
}
//**************************************************************************
void CBCGPPopupMenu::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	if (m_bIsResizeBarOnTop)
	{
		if (nHitTest == HTTOPLEFT || nHitTest == HTTOPRIGHT || nHitTest == HTTOP)
		{
			StartResize ();
			return;
		}
	}
	else
	{
		if (nHitTest == HTBOTTOMLEFT || nHitTest == HTBOTTOMRIGHT || nHitTest == HTBOTTOM)
		{
			StartResize ();
			return;
		}
	}
	
	CMiniFrameWnd::OnNcLButtonDown(nHitTest, point);
}
//*********************************************************************************
void CBCGPPopupMenu::EnableResize (CSize sizeMinResize)
{
	m_bIsResizable = (sizeMinResize != CSize (0, 0));

	if (m_bIsResizable)
	{
		sizeMinResize.cy += 
			sizeMinResize.cx > 0 ? iResizeBarBarHeightRight : iResizeBarBarHeight;
	}

	m_sizeMinResize = sizeMinResize;
}
//*********************************************************************************
void CBCGPPopupMenu::EnableVertResize (int nMinResize)
{
	m_bIsResizable = nMinResize > 0;
	m_sizeMinResize = CSize (0, nMinResize);
}
//*********************************************************************************
BOOL CBCGPPopupMenu::StartResize ()
{
	ASSERT_VALID (this);

	HWND hwndMenu = GetSafeHwnd ();

	// handle pending WM_PAINT messages
	MSG msg;
	while (::PeekMessage(&msg, NULL, WM_PAINT, WM_PAINT, PM_NOREMOVE))
	{
		if (!GetMessage(&msg, NULL, WM_PAINT, WM_PAINT))
			return FALSE;
		DispatchMessage(&msg);
	}

	// don't handle if capture already set
	if (::GetCapture() != NULL)
		return FALSE;

	// set capture to the window which received this message
	SetCapture();
	ASSERT(this == CWnd::GetCapture());

	BOOL bSuccess = FALSE;
	BOOL bStop = FALSE;

	CRect rectWindow;
	GetWindowRect (rectWindow);

	m_bResizeTracking = TRUE;
	GetMenuBar ()->m_bResizeTracking = TRUE;

	int iShadowSize = m_iShadowSize;
	m_iShadowSize = 0;

	if (m_pWndShadow->GetSafeHwnd () != NULL)
	{
		m_pWndShadow->ShowWindow (SW_HIDE);
	}

	BOOL bIsMoseMove = FALSE;

	const int nBorderSize = GetBorderSize ();

	// get messages until capture lost or cancelled/accepted
	while (!bStop && CWnd::GetCapture() == this)
	{
		MSG msg;
		if (!::GetMessage(&msg, NULL, 0, 0))
		{
			AfxPostQuitMessage((int) msg.wParam);
			break;
		}

		switch (msg.message)
		{
		case WM_LBUTTONUP:
			bStop = TRUE;
			bSuccess = TRUE;

			m_iShadowSize = iShadowSize;

			if (bIsMoseMove)
			{
				if (m_bmpShadowRight.GetSafeHandle () != NULL)
				{
					m_bmpShadowRight.DeleteObject ();
				}
				
				if (m_bmpShadowBottom.GetSafeHandle () != NULL)
				{
					m_bmpShadowBottom.DeleteObject ();
				}

				RecalcLayout ();
				UpdateBottomWindows ();

				if (m_pWndShadow->GetSafeHwnd () != NULL)
				{
					m_pWndShadow->Repos ();
				}

				ShowWindow (SW_SHOWNOACTIVATE);
			}

			break;

		case WM_MOUSEMOVE:
			{
				bIsMoseMove = TRUE;

				const BOOL bRTL = GetExStyle() & WS_EX_LAYOUTRTL;

				const int dx = (bRTL ? rectWindow.right - msg.pt.x : msg.pt.x - rectWindow.left) + iShadowSize + 2 * nBorderSize;

				CSize sizeNew = CSize (
					m_sizeMinResize.cx > 0 ?
						max (m_sizeMinResize.cx, dx) :
						rectWindow.Width () - iShadowSize,
					max (m_sizeMinResize.cy, 
						m_bIsResizeBarOnTop ? rectWindow.bottom - msg.pt.y: msg.pt.y - rectWindow.top));

				if (sizeNew != m_sizeCurrent)
				{
					m_sizeCurrent = sizeNew;

					if (m_bIsResizeBarOnTop && sizeNew.cy > m_sizeMinResize.cy)
					{
						m_ptLocation.y = msg.pt.y - m_rectResize.Height () - GetBorderSize () - 1;
					}

					RecalcLayout ();
					GetMenuBar ()->AdjustLocations ();
					
					if (m_wndScrollBarVert.GetSafeHwnd () != NULL)
					{
						OnVScroll (SB_THUMBTRACK, m_wndScrollBarVert.GetScrollPos (), &m_wndScrollBarVert);
					}

					RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW | RDW_ALLCHILDREN);
					UpdateBottomWindows ();

					m_bWasResized = TRUE;
				}
			}
			break;

		// just dispatch rest of the messages
		default:
			DispatchMessage(&msg);
			break;
		}
	}

	ReleaseCapture();

	if (::IsWindow (hwndMenu))
	{
		m_bResizeTracking = FALSE;
		GetMenuBar ()->m_bResizeTracking = FALSE;
	}

	return bSuccess;
}
//*********************************************************************************
CBCGPPopupMenu* CBCGPPopupMenu::FindMenuWithConnectedFloaty ()
{
#ifndef BCGP_EXCLUDE_RIBBON
	if (CBCGPRibbonFloaty::m_pCurrent == NULL)
	{
		return NULL;
	}

	for (CBCGPPopupMenu* pMenu = m_pActivePopupMenu; pMenu != NULL;
		pMenu = pMenu->GetParentPopupMenu ())
	{
		if (CWnd::FromHandlePermanent (pMenu->GetSafeHwnd ()) != NULL)
		{
			ASSERT_VALID (pMenu);

			if (pMenu->m_hwndConnectedFloaty != NULL)
			{
				return pMenu;
			}
		}
	}
#endif
	return NULL;
}
//*********************************************************************************
void CBCGPPopupMenu::TriggerResize ()
{
	ASSERT_VALID (this);

	m_bResizeTracking = TRUE;
	GetMenuBar ()->m_bResizeTracking = TRUE;

	GetMenuBar ()->AdjustLocations ();
	
	RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW | RDW_ALLCHILDREN);
	UpdateBottomWindows ();

	m_bResizeTracking = FALSE;
	GetMenuBar ()->m_bResizeTracking = FALSE;
}
//*********************************************************************************
void CBCGPPopupMenu::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CMiniFrameWnd::OnWindowPosChanged(lpwndpos);

	if (m_pWndShadow->GetSafeHwnd () != NULL && !m_bResizeTracking)
	{
		if (lpwndpos->flags & SWP_HIDEWINDOW)
		{
			m_pWndShadow->ShowWindow (SW_HIDE);
		}
		else if ((lpwndpos->flags & (SWP_NOSIZE | SWP_NOMOVE)) == 0 ||
			(lpwndpos->flags & SWP_SHOWWINDOW))
		{
			m_pWndShadow->Repos ();
		}
	}
}
//************************************************************************************
CBCGPPopupMenu* CBCGPPopupMenu::GetSafeActivePopupMenu()
{
	if (m_pActivePopupMenu != NULL &&
		::IsWindow (m_pActivePopupMenu->m_hWnd) &&
		CWnd::FromHandlePermanent(m_pActivePopupMenu->m_hWnd) != NULL)
	{
		return m_pActivePopupMenu;
	}

	return NULL;
}
