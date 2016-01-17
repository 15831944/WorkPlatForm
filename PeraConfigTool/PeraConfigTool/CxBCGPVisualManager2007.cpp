// CxBCGPVisualManager2007.cpp : 实现文件
//

#include "stdafx.h"
#include "CxBCGPVisualManager2007.h"
#include "UserDefine.h"
#include <afxext.h>
#include <BCGPTagManager.h>
#include "BCGPDockBar.h"
#include "BCGPVisualManager.h"

#define MENUBAR_HEIGHT          24
#define MENUBAR_HEIGHT_ZOOM_IN          30

// CCxBCGPVisualManager2007

IMPLEMENT_DYNCREATE(CCxBCGPVisualManager2007, CBCGPVisualManager2007)

CCxBCGPVisualManager2007::CCxBCGPVisualManager2007()
{
}

CCxBCGPVisualManager2007::~CCxBCGPVisualManager2007()
{
}


BOOL CCxBCGPVisualManager2007::OnNcPaint (CWnd* pWnd, const CObList& lstSysButtons, CRect rectRedraw)
{
	if (!CanDrawImage ())
	{
		return CBCGPVisualManager2003::OnNcPaint (pWnd, lstSysButtons, rectRedraw);
	}

	ASSERT_VALID (pWnd);

	if (pWnd->GetSafeHwnd () == NULL)
	{
		return FALSE;
	}

	CWindowDC dc (pWnd);

	if (dc.GetSafeHdc () != NULL)
	{
		CRgn rgn;
		if (!rectRedraw.IsRectEmpty ())
		{
			rgn.CreateRectRgnIndirect (rectRedraw);
			dc.SelectClipRgn (&rgn);
		}

#ifndef BCGP_EXCLUDE_RIBBON
		CBCGPRibbonBar* pBar = GetRibbonBar (pWnd);
		BOOL bRibbonCaption  = pBar != NULL && 
			pBar->IsWindowVisible () &&
			pBar->IsReplaceFrameCaption ();
#else
		BOOL bRibbonCaption = FALSE;
#endif

		CRect rtWindow;
		pWnd->GetWindowRect (rtWindow);
		pWnd->ScreenToClient (rtWindow);

		CRect rtClient;
		pWnd->GetClientRect (rtClient);

		rtClient.OffsetRect (-rtWindow.TopLeft ());
		dc.ExcludeClipRect (rtClient);

		rtWindow.OffsetRect (-rtWindow.TopLeft ());

		BOOL bActive = IsWindowActive (pWnd);

		CRect rectCaption (rtWindow);
		CSize szSysBorder (GetSystemBorders (bRibbonCaption));

		BOOL bDialog = pWnd->IsKindOf (RUNTIME_CLASS (CBCGPDialog)) || pWnd->IsKindOf (RUNTIME_CLASS (CBCGPPropertySheet));
		if (bDialog && (pWnd->GetStyle () & WS_THICKFRAME) == 0 && (pWnd->GetStyle () & WS_MAXIMIZE) != WS_MAXIMIZE)
		{
			// 对话框无WS_THICKFRAME属性，且不是最大化情况下显示纠正
			szSysBorder.cy = ::GetSystemMetrics (SM_CYFIXEDFRAME) - 1;
		}

		if (!pWnd->IsIconic ())
		{
			rectCaption.bottom = rectCaption.top + szSysBorder.cy;
		}

		const DWORD dwStyle = pWnd->GetStyle ();
		BOOL bMaximized = (dwStyle & WS_MAXIMIZE) == WS_MAXIMIZE;

		if (!bRibbonCaption)
		{
			const DWORD dwStyleEx = pWnd->GetExStyle ();
			const BOOL bIsSmallCaption = (dwStyleEx & WS_EX_TOOLWINDOW) != 0;

			if (!pWnd->IsIconic ())
			{
				rectCaption.bottom += bIsSmallCaption ? ::GetSystemMetrics (SM_CYSMCAPTION) : ::GetSystemMetrics (SM_CYCAPTION);
			}
			else
			{
				rectCaption.bottom -= 1;
			}

			BOOL bDestroyIcon = FALSE;
			HICON hIcon = globalUtils.GetWndIcon (pWnd, &bDestroyIcon);

			CString strText;
			pWnd->GetWindowText (strText);

			CString strTitle (strText);
			CString strDocument;

			BOOL bPrefix = FALSE;
			if ((dwStyle & FWS_ADDTOTITLE) == FWS_ADDTOTITLE)
			{
				bPrefix = (dwStyle & FWS_PREFIXTITLE) == FWS_PREFIXTITLE;
				CFrameWnd* pFrameWnd = DYNAMIC_DOWNCAST(CFrameWnd, pWnd);

				if (pFrameWnd != NULL)
				{
					strTitle = pFrameWnd->GetTitle();

					if (!strTitle.IsEmpty ())
					{
						if (strText.GetLength () >= strTitle.GetLength ())
						{
							if (bPrefix)
							{
								int pos = strText.Find (strTitle, strText.GetLength () - strTitle.GetLength ());
								if (pos != -1)
								{
									strTitle = strText.Right (strTitle.GetLength () + 3);
									strDocument = strText.Left (strText.GetLength () - strTitle.GetLength ());
								}
							}
							else
							{
								int pos = strText.Find (strTitle);
								if (pos != -1)
								{
									strTitle = strText.Left (strTitle.GetLength () + 3);
									strDocument = strText.Right (strText.GetLength () - strTitle.GetLength ());
								}	
							}
						}
					}
					else
					{
						strDocument = strText;
					}
				}
			}
			// 窗口最大化且有图标的显示纠正
			if (bMaximized && hIcon!=NULL)
			{
				rectCaption.InflateRect (szSysBorder.cx, szSysBorder.cy, szSysBorder.cx, 0);
			}
			
			DrawNcCaption (&dc, rectCaption, dwStyle, dwStyleEx, 
					strTitle, strDocument, hIcon, bPrefix, bActive, m_bNcTextCenter,
					lstSysButtons);

			if (bDestroyIcon)
			{
				::DestroyIcon (hIcon);
			}

			if (bMaximized)
			{
				return TRUE;
			}
		}
#ifndef BCGP_EXCLUDE_RIBBON
		else
		{
			if (bMaximized)
			{
				return TRUE;
			}

			rectCaption.bottom += pBar->GetCaptionHeight ();

			if (IsBeta ())
			{
				CRect rectBorder (m_ctrlMainBorderCaption.GetParams ().m_rectSides);

				COLORREF clr1  = bActive 
					? m_clrAppCaptionActiveStart 
					: m_clrAppCaptionInactiveStart;
				COLORREF clr2  = bActive 
					? m_clrAppCaptionActiveFinish 
					: m_clrAppCaptionInactiveFinish;

				CRect rectCaption2 (rectCaption);
				rectCaption2.DeflateRect (rectBorder.left, rectBorder.top, 
					rectBorder.right, rectBorder.bottom);

				{
					CBCGPDrawManager dm (dc);
					dm.Fill4ColorsGradient (rectCaption2, clr1, clr2, clr2, clr1, FALSE);
				}

				m_ctrlMainBorderCaption.DrawFrame (&dc, rectCaption, bActive ? 0 : 1);
			}
			else
			{
				m_ctrlMainBorderCaption.Draw (&dc, rectCaption, bActive ? 0 : 1);
			}
		}
#endif // BCGP_EXCLUDE_RIBBON

		rtWindow.top = rectCaption.bottom;

		dc.ExcludeClipRect (rectCaption);

		if (pWnd->IsKindOf (RUNTIME_CLASS (CMDIChildWnd)) ||
			(bDialog && !m_ctrlDialogBorder.IsValid ()))
		{
			if (bDialog)
			{
				CRect rtDialog (rtWindow);
				rtDialog.DeflateRect (1, 0, 1, 1);
				dc.FillRect (rtDialog, &GetDlgBackBrush (pWnd));

				dc.ExcludeClipRect (rtDialog);
			}

			m_ctrlMDIChildBorder.DrawFrame (&dc, rtWindow, bActive ? 0 : 1);
		}
		else if (bDialog)
		{
			m_ctrlDialogBorder.DrawFrame (&dc, rtWindow, bActive ? 0 : 1);
		}
		else
		{
			m_ctrlMainBorder.DrawFrame (&dc, rtWindow, bActive ? 0 : 1);
		}

		if (bDialog)
		{
			dc.SelectClipRgn (NULL);
			return TRUE;
		}

		//-------------------------------
		// Find status bar extended area:
		//-------------------------------
		CRect rectExt (0, 0, 0, 0);
		BOOL bExtended    = FALSE;
		BOOL bBottomFrame = FALSE;
		BOOL bIsStatusBar = FALSE;

		CWnd* pStatusBar = pWnd->GetDescendantWindow (AFX_IDW_STATUS_BAR, TRUE);

		if (pStatusBar->GetSafeHwnd () != NULL && pStatusBar->IsWindowVisible ())
		{
			CBCGPStatusBar* pClassicStatusBar = DYNAMIC_DOWNCAST (
				CBCGPStatusBar, pStatusBar);
			if (pClassicStatusBar != NULL)
			{
				bExtended = pClassicStatusBar->GetExtendedArea (rectExt);
				bIsStatusBar = TRUE;
			}
#ifndef BCGP_EXCLUDE_RIBBON
			else
			{
				CBCGPRibbonStatusBar* pRibbonStatusBar = DYNAMIC_DOWNCAST (
					CBCGPRibbonStatusBar, pStatusBar);
				if (pRibbonStatusBar != NULL)
				{
					bExtended    = pRibbonStatusBar->GetExtendedArea (rectExt);
					bBottomFrame = pRibbonStatusBar->IsBottomFrame ();
					bIsStatusBar = TRUE;
				}
			}
#endif // BCGP_EXCLUDE_RIBBON
		}

		if (bIsStatusBar)
		{
			CRect rectStatus;
			pStatusBar->GetClientRect (rectStatus);

			int nHeight = rectStatus.Height ();
			rectStatus.bottom = rtWindow.bottom;
			rectStatus.top    = rectStatus.bottom - nHeight - (bBottomFrame ? -1 : szSysBorder.cy);
			rectStatus.left   = rtWindow.left;
			rectStatus.right  = rtWindow.right;

			if (bExtended)
			{
				rectExt.left   = rectStatus.right - rectExt.Width () - szSysBorder.cx;
				rectExt.top    = rectStatus.top;
				rectExt.bottom = rectStatus.bottom;
				rectExt.right  = rtWindow.right;
			}

			m_ctrlStatusBarBack.Draw (&dc, rectStatus, bActive ? 0 : 1);

			if (bExtended)
			{
				rectExt.left -= m_ctrlStatusBarBack_Ext.GetParams ().m_rectCorners.left;

				m_ctrlStatusBarBack_Ext.Draw (&dc, rectExt, bActive ? 0 : 1);
			}
		}

		dc.SelectClipRgn (NULL);

		return TRUE;
	}

	return CBCGPVisualManager2003::OnNcPaint (pWnd, lstSysButtons, rectRedraw);
}

//*****************************************************************************
void CCxBCGPVisualManager2007::DrawNcText (CDC* pDC, CRect& rect, 
										 const CString& strTitle, 
										 const CString& strDocument, 
										 BOOL bPrefix, BOOL bActive, BOOL bIsRTL, 
										 BOOL bTextCenter,
										 BOOL bGlass/* = FALSE*/, int nGlassGlowSize/* = 0*/, 
										 COLORREF clrGlassText/* = (COLORREF)-1*/)
{
	if ((strTitle.IsEmpty () && strDocument.IsEmpty ()) || 
		rect.right <= rect.left)
	{
		return;
	}

	CString strTitle2 = strTitle;
	bTextCenter = FALSE;

	ASSERT_VALID (pDC);

	int nOldMode = pDC->SetBkMode (TRANSPARENT);
	COLORREF clrOldText = pDC->GetTextColor ();

	DWORD dwTextStyle = DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX |
		(bIsRTL ? DT_RTLREADING : 0);

	if (/*strDocument.IsEmpty ()*/TRUE)
	{
		COLORREF clrText = bActive 
			? m_clrAppCaptionActiveTitleText
			: m_clrAppCaptionInactiveTitleText;

		int widthFull = rect.Width ();
		int width = pDC->GetTextExtent (strTitle2).cx;

		if (bTextCenter && width < widthFull)
		{
			rect.left += (widthFull - width) / 2;
		}

		rect.right = min (rect.left + width, rect.right);

		if (rect.right > rect.left)
		{
			if (bGlass)
			{
				DrawTextOnGlass (pDC, strTitle2, rect, dwTextStyle, nGlassGlowSize, clrGlassText);
			}
			else
			{
				pDC->SetTextColor (clrText);
				pDC->DrawText (strTitle2, rect, dwTextStyle);
			}
		}
	}
	else
	{
		const CString& str1 = bPrefix ? strDocument : strTitle;
		const CString& str2 = bPrefix ? strTitle : strDocument;

		COLORREF clrText1 = bActive 
			? m_clrAppCaptionActiveText
			: m_clrAppCaptionInactiveText;
		COLORREF clrText2 = bActive 
			? m_clrAppCaptionActiveTitleText
			: m_clrAppCaptionInactiveTitleText;

		if (!bPrefix)
		{
			COLORREF clr = clrText1;
			clrText1 = clrText2;
			clrText2 = clr;
		}

		int widthFull = rect.Width ();
		CSize sz1 = pDC->GetTextExtent (str1);
		CSize sz2 = pDC->GetTextExtent (str2);
		int width = sz1.cx + sz2.cx;
		int left = rect.left;

		if (bTextCenter && width < widthFull)
		{
			rect.left += (widthFull - width) / 2;
		}

		rect.right = min (rect.left + width, rect.right);

		if (bIsRTL)
		{
			if (width <= rect.Width ())
			{
				rect.left += sz2.cx;
			}
			else
			{
				if (sz1.cx < rect.Width ())
				{
					rect.left += max (0, sz2.cx + (rect.Width () - width));
				}
			}
		}

		if (bGlass)
		{
			DrawTextOnGlass (pDC, str1, rect, dwTextStyle, nGlassGlowSize, clrGlassText);
		}
		else
		{
			pDC->SetTextColor (clrText1);
			pDC->DrawText (str1, rect, dwTextStyle);
		}

		if (bIsRTL)
		{
			if (width <= (rect.right - left))
			{
				rect.right = rect.left;
				rect.left  = rect.right - sz2.cx;
			}
			else
			{
				rect.left = left;
				rect.right -= sz1.cx;
			}
		}
		else
		{
			rect.left += sz1.cx;
		}

		if (rect.right > rect.left)
		{
			if (bGlass)
			{
				DrawTextOnGlass (pDC, str2, rect, dwTextStyle, nGlassGlowSize, clrGlassText);
			}
			else
			{
				pDC->SetTextColor (clrText2);
				pDC->DrawText (str2, rect, dwTextStyle);
			}
		}
	}

	pDC->SetBkMode    (nOldMode);
	pDC->SetTextColor (clrOldText);
}

//*******************************************************************************
void CCxBCGPVisualManager2007::OnFillBarBackground (CDC* pDC, CBCGPBaseControlBar* pBar,
						CRect rectClient, CRect rectClip,
						BOOL bNCArea/* = FALSE*/)
{
	ASSERT_VALID (pBar);

	if (pBar->IsOnGlass ())
	{
		pDC->FillSolidRect (rectClient, RGB (0, 0, 0));
		return;
	}

	if (DYNAMIC_DOWNCAST (CBCGPReBar, pBar) != NULL ||
		DYNAMIC_DOWNCAST (CBCGPReBar, pBar->GetParent ()))
	{
		FillRebarPane (pDC, pBar, rectClient);
		return;
	}

    CRuntimeClass* pBarClass = pBar->GetRuntimeClass ();

	if (!CanDrawImage () ||
		pBar->IsDialogControl () ||
		pBarClass->IsDerivedFrom (RUNTIME_CLASS (CBCGPColorBar)) ||
		pBarClass->IsDerivedFrom (RUNTIME_CLASS (CBCGPCalendarBar)))
	{
		CBCGPVisualManager2003::OnFillBarBackground (pDC, pBar, rectClient, rectClip, bNCArea);
		return;
	}

	if (pBarClass->IsDerivedFrom (RUNTIME_CLASS (CBCGPCalculator)))
	{
		if (m_brDlgBackground.GetSafeHandle() != NULL)
		{
			pDC->FillRect(rectClient, &m_brDlgBackground);
		}
		else
		{
			CBCGPVisualManager2003::OnFillBarBackground (pDC, pBar, rectClient, rectClip, bNCArea);
		}
		return;
	}

    if (pBar->IsKindOf (RUNTIME_CLASS (CBCGPMenuBar)))
    {
	    BOOL bIsHorz = (pBar->GetBarStyle () & CBRS_ORIENT_HORZ);
	    COLORREF clr1 = bIsHorz ? m_clrMenuBarGradientDark : m_clrMenuBarGradientVertLight;
	    COLORREF clr2 = bIsHorz ? m_clrMenuBarGradientLight : m_clrMenuBarGradientVertDark;

        CBCGPDrawManager dm (*pDC);
        //dm.Fill4ColorsGradient (rectClient, clr1, clr2, clr2, clr1, !bIsHorz);
        //dm.DrawRect( rectClient, g_crToolbarBg, g_crToolbarBg );
		dm.DrawRect( rectClient, g_crToolbarBg, g_crToolbarBg );
		if ((rectClient.Height() == MENUBAR_HEIGHT || rectClient.Height() == MENUBAR_HEIGHT_ZOOM_IN) && rectClient.right != 0x7fff)
		{
			//区域顶部内边线
			dm.DrawLine( rectClient.left, rectClient.top 
				, rectClient.right, rectClient.top 
				, g_crToolbarTop2 );
			//区域底部外边线
			dm.DrawLine( rectClient.left, rectClient.bottom - 2
				, rectClient.right, rectClient.bottom - 2
				, g_crToolbarTop );
			//区域底部内边线
			dm.DrawLine( rectClient.left, rectClient.bottom - 1
				, rectClient.right, rectClient.bottom - 1
				, g_crToolbarTop2 );
		}

		return;
    }
    else if (pBar->IsKindOf (RUNTIME_CLASS (CBCGPPopupMenuBar)))
    {
		pDC->FillRect (rectClip, &m_brMenuLight);

		CBCGPPopupMenuBar* pMenuBar = DYNAMIC_DOWNCAST (CBCGPPopupMenuBar, pBar);
		if (!pMenuBar->m_bDisableSideBarInXPMode)
		{
			CRect rectGutter = rectClient;

			rectGutter.right = rectGutter.left + pMenuBar->GetGutterWidth ();
			rectGutter.DeflateRect (0, 1);

            pDC->FillRect (rectGutter, &m_brBarBkgnd);

            rectGutter.left = rectGutter.right;
            rectGutter.right += 2;
            DrawSeparator (pDC, rectGutter, FALSE);
		}

        return;
    }
    else if (pBar->IsKindOf (RUNTIME_CLASS (CBCGPStatusBar)))
    {
		CSize szSysBorder (GetSystemBorders (TRUE));

		CRect rect (rectClient);
		CRect rectExt (0, 0, 0, 0);
		BOOL bExtended = ((CBCGPStatusBar*)pBar)->GetExtendedArea (rectExt);

		//CRect rectBorders = m_ctrlStatusBarBack.GetParams ().m_rectSides;

		if (bExtended)
		{
			rect.right = rectExt.left;
		}

		CWnd* pWnd = ((CBCGPStatusBar*)pBar)->GetParent ();
		ASSERT_VALID (pWnd);

		BOOL bActive = IsWindowActive (pWnd);

		rect.InflateRect (szSysBorder.cx, 0, szSysBorder.cx, szSysBorder.cy);
		m_ctrlStatusBarBack.Draw (pDC, rect, bActive ? 0 : 1);

		if (bExtended)
		{
			rectExt.InflateRect (0, 0, szSysBorder.cx, szSysBorder.cy);
			rectExt.left -= m_ctrlStatusBarBack_Ext.GetParams ().m_rectCorners.left;
			m_ctrlStatusBarBack_Ext.Draw (pDC, rectExt, bActive ? 0 : 1);
		}

        return;
    }
#ifndef BCGP_EXCLUDE_RIBBON
    else if (pBar->IsKindOf (RUNTIME_CLASS (CBCGPRibbonStatusBar)))
    {
		CBCGPRibbonStatusBar* pRibbonStatusBar = DYNAMIC_DOWNCAST (
			CBCGPRibbonStatusBar, pBar);

		CSize szSysBorder (GetSystemBorders (TRUE));

		CRect rect (rectClient);
		CRect rectExt (0, 0, 0, 0);

		BOOL bExtended    = pRibbonStatusBar->GetExtendedArea (rectExt);
		BOOL bBottomFrame = pRibbonStatusBar->IsBottomFrame ();

		if (bExtended)
		{
			rect.right = rectExt.left;
		}

		CWnd* pWnd = pBar->GetParent ();
		ASSERT_VALID (pWnd);

		BOOL bActive = IsWindowActive (pWnd);

		rect.InflateRect (szSysBorder.cx, 0, szSysBorder.cx, bBottomFrame ? -1 : szSysBorder.cy);
		m_ctrlStatusBarBack.Draw (pDC, rect, bActive ? 0 : 1);

		if (bExtended)
		{
			rectExt.InflateRect (0, 0, szSysBorder.cx, bBottomFrame ? -1 : szSysBorder.cy);
			rectExt.left -= m_ctrlStatusBarBack_Ext.GetParams ().m_rectCorners.left;
			m_ctrlStatusBarBack_Ext.Draw (pDC, rectExt, bActive ? 0 : 1);
		}

        return;
    }
#endif
	else if (pBar->IsKindOf (RUNTIME_CLASS (CBCGPOutlookBarToolBar)))
	{
		if (m_ctrlOutlookWndBar.IsValid ())
		{
			m_ctrlOutlookWndBar.Draw (pDC, rectClient);
		}
		else
		{
			CBCGPDrawManager dm (*pDC);
			dm.FillGradient (rectClient,	m_clrToolBarGradientDark,
											m_clrToolBarGradientLight,
											TRUE);
		}

		return;
	}

#if !defined(BCGP_EXCLUDE_TOOLBOX) && !defined(BCGP_EXCLUDE_TASK_PANE)
	BOOL bIsToolBox =	pBar->IsKindOf (RUNTIME_CLASS (CBCGPToolBoxPage)) ||
						pBar->IsKindOf (RUNTIME_CLASS (CBCGPToolBox)) ||
						pBar->IsKindOf (RUNTIME_CLASS (CBCGPToolBoxEx));

	if (bIsToolBox)
	{
			CBCGPDrawManager dm (*pDC);
			dm.FillGradient (rectClient,	m_clrToolBarGradientLight,
											m_clrToolBarGradientDark,
											FALSE);
		return;
	}

#endif
	//////////////////////////////////////////////////////////////////////////
	if (
		pBar->IsKindOf(RUNTIME_CLASS(CBCGPSlider)) 
		)
	{
		CBCGPDrawManager dm (*pDC);
		dm.DrawRect( rectClient, g_crSplitterBg, g_crSplitterBg );
		return;
	}

	if (
		pBar->IsKindOf (RUNTIME_CLASS (CBCGPDockBar))
		)
	{
		CBCGPDrawManager dm (*pDC);
		dm.DrawRect( rectClient, g_crToolbarBg, g_crToolbarBg );
		return;
	}

	CBCGPVisualManager2003::OnFillBarBackground (pDC, pBar, rectClient, rectClip, bNCArea);
}

//****************************************************************************************
void CCxBCGPVisualManager2007::OnDrawBarBorder (CDC* pDC, CBCGPBaseControlBar* pBar, CRect& rect)
{
	//return;
	ASSERT_VALID (pBar);

	if (pBar->IsDialogControl () || globalData.m_nBitsPerPixel <= 8 || 
		globalData.IsHighContastMode ())
	{
		CBCGPVisualManagerXP::OnDrawBarBorder (pDC, pBar, rect);
	}
}

//*****************************************************************************
void CCxBCGPVisualManager2007::OnDrawBarGripper (CDC* pDC, CRect rectGripper, BOOL bHorz, CBCGPBaseControlBar* pBar)
{
	if (!CanDrawImage () ||
		(pBar != NULL && pBar->IsDialogControl ()) ||
        m_ToolBarGripper.GetCount () == 0)
	{
		CBCGPVisualManager2003::OnDrawBarGripper (pDC, rectGripper, bHorz, pBar);
		return;
	}

    CSize szBox (m_ToolBarGripper.GetImageSize ());

    if (szBox != CSize (0, 0))
    {
		if (bHorz)
		{
			rectGripper.left = rectGripper.right - szBox.cx;
		}
		else
		{
			rectGripper.top = rectGripper.bottom - szBox.cy;
		}

		CBCGPToolBar* pToolBar = DYNAMIC_DOWNCAST (CBCGPToolBar, pBar);
		if (pToolBar != NULL)
		{
			if (bHorz)
			{
				const int nHeight = CBCGPToolBar::IsLargeIcons () ? 
					pToolBar->GetRowHeight () : pToolBar->GetButtonSize ().cy;

				const int nDelta = max (0, (nHeight - pToolBar->GetImageSize ().cy) / 2);
				rectGripper.DeflateRect (0, nDelta);
			}
			else
			{
				const int nWidth = CBCGPToolBar::IsLargeIcons () ? 
					pToolBar->GetColumnWidth () : pToolBar->GetButtonSize ().cx;

				const int nDelta = max (0, (nWidth - pToolBar->GetImageSize ().cx) / 2);
				rectGripper.DeflateRect (nDelta, 0);
			}
		}

		int nBoxesNumber = bHorz ?
			(rectGripper.Height () - szBox.cy) / szBox.cy : 
			(rectGripper.Width () - szBox.cx) / szBox.cx;

		int nOffset = bHorz ? 
			(rectGripper.Height () - nBoxesNumber * szBox.cy) / 2 :
			(rectGripper.Width () - nBoxesNumber * szBox.cx) / 2;

		for (int nBox = 0; nBox < nBoxesNumber; nBox++)
		{
			int x = bHorz ? 
				rectGripper.left : 
				rectGripper.left + nOffset;

			int y = bHorz ? 
				rectGripper.top + nOffset - 1: 
				rectGripper.top;

            m_ToolBarGripper.DrawEx (pDC, CRect (CPoint (x, y), szBox), 0);

			nOffset += bHorz ? szBox.cy : szBox.cx;
		}
	}
}

COLORREF CCxBCGPVisualManager2007::OnDrawControlBarCaption (CDC* pDC, CBCGPDockingControlBar* pBar, 
	BOOL bActive, CRect rectCaption, CRect rectButtons)
{
	if (!CanDrawImage () || pBar == NULL || pBar->IsDialogControl ())
	{
		return CBCGPVisualManager2003::OnDrawControlBarCaption (pDC, pBar, bActive, rectCaption, rectButtons);
	}

	ASSERT_VALID (pDC);

	CBCGPDrawManager dm (*pDC);
	//上边外线
	dm.DrawLine(rectCaption.left, rectCaption.top, rectCaption.right - 1, rectCaption.top, g_crBarBorderOut );
	//上边内线
	dm.DrawLine(rectCaption.left + 1, rectCaption.top + 1, rectCaption.right - 2, rectCaption.top + 1, g_crBarBorderIn );
	//caption下边内线
	//dm.DrawLine(rectCaption.left + 1, rectCaption.bottom + 0 , rectCaption.right - 2, rectCaption.bottom + 0, RGB(255,0,0) );
	dm.DrawLine(rectCaption.left + 1, rectCaption.bottom + 1 , rectCaption.right - 2, rectCaption.bottom + 1, g_crBarCaptionBottom );
	dm.DrawLine(rectCaption.left + 1, rectCaption.bottom + 2 , rectCaption.right - 2, rectCaption.bottom + 2, g_crBarCaptionBottom );
	CRect rc = rectCaption;
	rc.DeflateRect (1, 2, 1, -1);
	COLORREF crTop = g_crBarCaptionStart;
	COLORREF crBottom = g_crBarCaptionEnd;
	dm.FillGradient (rc, crBottom, crTop, TRUE);

	//rectCaption.bottom += 2;

	//CPen pen (PS_SOLID, 1, globalData.clrBarFace);
	//CPen pen1 (PS_SOLID, 1, g_crBarBorderOut);
	//CPen pen2 (PS_SOLID, 1, g_crBarBorderIn);
	//CPen pen3 (PS_SOLID, 1, g_crBarCaptionBottom);

	//CPen* pOldPen = pDC->SelectObject (&pen1);
	////左
	//pDC->MoveTo (rectCaption.left, rectCaption.bottom);
	//pDC->LineTo (rectCaption.left, rectCaption.top);
	////上-out
	//pDC->MoveTo (rectCaption.left, rectCaption.top);
	//pDC->LineTo (rectCaption.right, rectCaption.top);
	////右
	//pDC->MoveTo (rectCaption.right - 1, rectCaption.top);
	//pDC->LineTo (rectCaption.right - 1, rectCaption.bottom);

	//pDC->SelectObject (&pen2);
	////上-in
	//pDC->MoveTo (rectCaption.left + 1, rectCaption.top + 1);
	//pDC->LineTo (rectCaption.right - 1, rectCaption.top + 1);
	//
	//pDC->SelectObject (&pen3);
	////下
	//pDC->MoveTo (rectCaption.left + 1, rectCaption.bottom );
	//pDC->LineTo (rectCaption.right - 1, rectCaption.bottom );

	//pDC->SelectObject (pOldPen);

	//rectCaption.DeflateRect (1, 2, 1, 1);
	//CBCGPDrawManager dm(*pDC);
	//COLORREF crTop = g_crBarCaptionStart;
	//COLORREF crBottom = g_crBarCaptionEnd;
	//dm.FillGradient (rectCaption, crBottom, crTop, TRUE);

	//pDC->FillRect (rectCaption, bActive ? &globalData.brActiveCaption : &globalData.brInactiveCaption);

	return bActive ? globalData.clrCaptionText : globalData.clrInactiveCaptionText;
}
//
//void CCxBCGPVisualManager2007::OnDrawCaptionBarInfoArea (CDC* pDC, CBCGPCaptionBar* pBar, CRect rect)
//{
//	if (!CanDrawImage ())
//	{
//		CBCGPVisualManager2003::OnDrawCaptionBarInfoArea (pDC, pBar, rect);
//		return;
//	}
//
//	ASSERT_VALID (pDC);
//
//	COLORREF clrFill = pBar->m_clrBarBackground != -1 ? pBar->m_clrBarBackground : globalData.clrBarFace;
//	COLORREF clrGradient = pBar->m_clrBarBackground != -1 ? CBCGPDrawManager::PixelAlpha (pBar->m_clrBarBackground, 150) : RGB (255, 255, 255);
//	COLORREF clrBorder = pBar->m_clrBarBackground != -1 ? CBCGPDrawManager::PixelAlpha (pBar->m_clrBarBackground, 80) : globalData.clrBarDkShadow;
//
//	CBCGPDrawManager dm (*pDC);
//
//	dm.FillGradient (rect, clrFill, clrGradient, TRUE);
//	pDC->Draw3dRect(rect, clrBorder, clrBorder);
//}
//
//void CCxBCGPVisualManager2007::OnDrawMiniFrameBorder (
//	CDC* pDC, CBCGPMiniFrameWnd* pFrameWnd,
//	CRect rectBorder, CRect rectBorderSize)
//{
//	if (!CanDrawImage ())
//	{
//		CBCGPVisualManager2003::OnDrawMiniFrameBorder (
//			pDC, pFrameWnd,
//			rectBorder, rectBorderSize);
//		return;
//	}
//
//	ASSERT_VALID (pDC);
//	ASSERT_VALID (pFrameWnd);
//
//#ifndef BCGP_EXCLUDE_TASK_PANE
//	BOOL bIsTasksPane = pFrameWnd->IsKindOf( RUNTIME_CLASS( CBCGPTaskPaneMiniFrameWnd ) );
//#else
//	BOOL bIsTasksPane = FALSE;
//#endif
//
//	if (bIsTasksPane)
//	{
//		CBrush* pOldBrush = pDC->SelectObject (&m_brFloatToolBarBorder);
//		ASSERT (pOldBrush != NULL);
//
//		pDC->PatBlt (rectBorder.left, rectBorder.top, rectBorderSize.left, rectBorder.Height (), PATCOPY);
//		pDC->PatBlt (rectBorder.left, rectBorder.top, rectBorder.Width (), rectBorderSize.top, PATCOPY);
//		pDC->PatBlt (rectBorder.right - rectBorderSize.right, rectBorder.top, rectBorderSize.right, rectBorder.Height (), PATCOPY);
//		pDC->PatBlt (rectBorder.left, rectBorder.bottom - rectBorderSize.bottom, rectBorder.Width (), rectBorderSize.bottom, PATCOPY);
//
//		rectBorderSize.DeflateRect (2, 2);
//		rectBorder.DeflateRect (2, 2);
//
//		pDC->SelectObject (&globalData.brBarFace);
//
//		pDC->PatBlt (rectBorder.left, rectBorder.top + 1, rectBorderSize.left, rectBorder.Height () - 1, PATCOPY);
//		pDC->PatBlt (rectBorder.left + 1, rectBorder.top, rectBorder.Width () - 2, rectBorderSize.top, PATCOPY);
//		pDC->PatBlt (rectBorder.right - rectBorderSize.right, rectBorder.top + 1, rectBorderSize.right, rectBorder.Height () - 1, PATCOPY);
//		pDC->PatBlt (rectBorder.left + 1, rectBorder.bottom - rectBorderSize.bottom, rectBorder.Width () - 2, rectBorderSize.bottom, PATCOPY);
//
//		pDC->SelectObject (pOldBrush);
//	}
//	else
//	{
//		CBCGPVisualManager2003::OnDrawMiniFrameBorder (pDC, pFrameWnd, rectBorder, rectBorderSize);
//	}
//}
//
//void CCxBCGPVisualManager2007::OnDrawFloatingToolbarBorder (
//	CDC* pDC, CBCGPBaseToolBar* pToolBar, 
//	CRect rectBorder, CRect rectBorderSize)
//{
//	if (!CanDrawImage ())
//	{
//		CBCGPVisualManager2003::OnDrawFloatingToolbarBorder (pDC, pToolBar, 
//			rectBorder, rectBorderSize);
//		return;
//	}
//
//	ASSERT_VALID (pDC);
//
//	CBrush* pOldBrush = pDC->SelectObject (&m_brFloatToolBarBorder);
//	ASSERT (pOldBrush != NULL);
//
//	pDC->PatBlt (rectBorder.left, rectBorder.top, rectBorderSize.left, rectBorder.Height (), PATCOPY);
//	pDC->PatBlt (rectBorder.left, rectBorder.top, rectBorder.Width (), rectBorderSize.top, PATCOPY);
//	pDC->PatBlt (rectBorder.right - rectBorderSize.right, rectBorder.top, rectBorderSize.right, rectBorder.Height (), PATCOPY);
//	pDC->PatBlt (rectBorder.left, rectBorder.bottom - rectBorderSize.bottom, rectBorder.Width (), rectBorderSize.bottom, PATCOPY);
//
//	rectBorderSize.DeflateRect (2, 2);
//	rectBorder.DeflateRect (2, 2);
//
//	pDC->SelectObject (&globalData.brBarFace);
//
//	pDC->PatBlt (rectBorder.left, rectBorder.top + 1, rectBorderSize.left, rectBorder.Height () - 1, PATCOPY);
//	pDC->PatBlt (rectBorder.left + 1, rectBorder.top, rectBorder.Width () - 2, rectBorderSize.top, PATCOPY);
//	pDC->PatBlt (rectBorder.right - rectBorderSize.right, rectBorder.top + 1, rectBorderSize.right, rectBorder.Height () - 1, PATCOPY);
//	pDC->PatBlt (rectBorder.left + 1, rectBorder.bottom - rectBorderSize.bottom, rectBorder.Width () - 2, rectBorderSize.bottom, PATCOPY);
//
//	pDC->SelectObject (pOldBrush);
//}

COLORREF CCxBCGPVisualManager2007::OnFillGridGroupByBoxBackground (CDC* pDC, CRect rect)
{
	if (!CanDrawImage ())
	{
		return CBCGPVisualManager2003::OnFillGridGroupByBoxBackground (pDC, rect);
	}

	ASSERT_VALID (pDC);

	pDC->FillRect (rect, &globalData.brBarFace);

	return globalData.clrBarText;
}
//********************************************************************************
COLORREF CCxBCGPVisualManager2007::OnFillGridGroupByBoxTitleBackground (CDC* pDC, CRect rect)
{
	if (!CanDrawImage ())
	{
		return CBCGPVisualManager2003::OnFillGridGroupByBoxTitleBackground (pDC, rect);
	}

	ASSERT_VALID (pDC);
	return globalData.clrBarText;
}
//********************************************************************************
void CCxBCGPVisualManager2007::OnDrawGridGroupByBoxItemBorder (CBCGPGridCtrl* pCtrl,
														 CDC* pDC, CRect rect)
{
	if (!CanDrawImage ())
	{
		CBCGPVisualManager2003::OnDrawGridGroupByBoxItemBorder (pCtrl,
														 pDC, rect);
		return;
	}

	ASSERT_VALID (pDC);

	COLORREF clrStart  = m_clrGridHeaderNormalStart;
	COLORREF clrFinish = m_clrGridHeaderNormalFinish;
	COLORREF clrBorder = m_clrGridHeaderNormalBorder;

	{
		CBCGPDrawManager dm (*pDC);
		dm.FillGradient (rect, clrFinish, clrStart);
	}

	pDC->Draw3dRect (rect, clrBorder, clrBorder);
}
//*****************************************************************************
COLORREF CCxBCGPVisualManager2007::GetGridLeftOffsetColor (CBCGPGridCtrl* pCtrl)
{
	if (!CanDrawImage ())
	{
		return CBCGPVisualManager2003::GetGridLeftOffsetColor (pCtrl);
	}

	return m_clrGridLeftOffset;
}

//********************************************************************************
COLORREF CCxBCGPVisualManager2007::OnFillGridItem (CBCGPGridCtrl* pCtrl, 
											CDC* pDC, CRect rectFill,
											BOOL bSelected, BOOL bActiveItem, BOOL bSortedColumn)
{
	ASSERT_VALID (pDC);
	ASSERT_VALID (pCtrl);

	// Fill area:
	if (bSelected && !bActiveItem)
	{
		if (!pCtrl->IsFocused ())
		{
			pDC->FillRect (rectFill,
				pCtrl->IsControlBarColors () ? &globalData.brBarFace : &globalData.brBtnFace);
			return globalData.clrBtnText;
		}
		else
		{
			pDC->FillRect (rectFill, &globalData.brHilite);
			return globalData.clrTextHilite;
		}
	}
	else
	{
		if (bActiveItem)
		{
			pDC->FillRect (rectFill, &globalData.brWindow);
		}
		else if (bSortedColumn)
		{
			CBrush br (pCtrl->GetSortedColor ());
			pDC->FillRect (rectFill, &br);
		}
		else
		{
			// no painting
		}
	}

	return (COLORREF)-1;
}

void CCxBCGPVisualManager2007::OnDrawSplitterBorder (CDC* pDC, CBCGPSplitterWnd* pSplitterWnd, CRect rect)
{
	static BOOL b = TRUE;

	ASSERT_VALID(pDC);
	CBCGPDrawManager dm(*pDC);

	if (b)
	{
		//左
		//rect.DeflateRect(-1, 0, 2, -1);
		rect.DeflateRect(-1, 1, 2, -1); //有上边框
		dm.DrawRect(rect, g_crResLibTreeBg, g_crBarBorderOut);
		b = FALSE;
	}
	else
	{
		//右
		rect.DeflateRect(-1, 1, -1, 0);
		dm.DrawRect(rect, g_crResLibImageBg, g_crBarBorderOut);
		b = TRUE;
	}
	
	//pDC->Draw3dRect (rect, globalData.clrBarShadow, globalData.clrBarShadow);
	//rect.InflateRect(-CX_BORDER, -CY_BORDER);
	//pDC->Draw3dRect (rect, globalData.clrBarFace, globalData.clrBarFace);
}
//********************************************************************************
void CCxBCGPVisualManager2007::OnDrawSplitterBox (CDC* pDC, CBCGPSplitterWnd* /*pSplitterWnd*/, CRect& rect)
{
	ASSERT_VALID(pDC);
	pDC->Draw3dRect(rect, globalData.clrBarFace, globalData.clrBarFace);
}
//********************************************************************************
void CCxBCGPVisualManager2007::OnFillSplitterBackground (CDC* pDC, CBCGPSplitterWnd* pSplitterWnd, CRect rect)
{
	ASSERT_VALID(pDC);
	CBCGPDrawManager dm(*pDC);
	dm.DrawRect(rect, g_crSplitterBg, g_crSplitterBg);
}

BOOL CCxBCGPVisualManager2007::ParseStyleXMLMainWnd(const CString& strItem)
{
	if (FALSE == CBCGPVisualManager2007::ParseStyleXMLMainWnd(strItem))
	{
		return FALSE;
	}

	if (strItem.IsEmpty ())
	{
		return FALSE;
	}

	CBCGPTagManager tmItem (strItem);

	// caption
	CString strCaption;
	if (tmItem.ExcludeTag (_T("CAPTION"), strCaption))
	{
		CBCGPTagManager tmCaption (strCaption);

		// buttons
		CString strButtons;
		if (tmCaption.ExcludeTag (_T("BUTTONS"), strButtons))
		{
			CBCGPTagManager tmButtons (strButtons);

			for (int i = 0; i < 2; i++)
			{
				CString str;
				CString suffix;
				if (i == 1)
				{
					suffix = _T("_S");
				}
				if (tmButtons.ExcludeTag (i == 0 ? _T("NORMAL") : _T("SMALL"), str))
				{
					CBCGPTagManager tmBtn (str);					
					if (!tmBtn.ReadSize (_T("CloseBtnSize"), m_szNcCloseBtnSize))
					{
						m_szNcCloseBtnSize.cx = 0;
						m_szNcCloseBtnSize.cy = 0;
					}
					CSize sizeIcon (0, 0);
					if (tmBtn.ReadSize (_T("CloseIconSize"), sizeIcon))
					{
						m_SysBtnClose[i].Clear ();
						m_SysBtnClose[i].SetPreMultiplyAutoCheck (TRUE);
						m_SysBtnClose[i].SetImageSize (sizeIcon);
						m_SysBtnClose[i].LoadStr (MakeResourceID(_T("SYS_BTN_CLOSE") + suffix));
					}
				}
			}
		}
	}
	
	return TRUE;
}

//*****************************************************************************
//本函数绘制tab页签本身的矩形区域
void CCxBCGPVisualManager2007::OnDrawTab (CDC* pDC, CRect rectTab,
						int iTab, BOOL bIsActive, const CBCGPBaseTabWnd* pTabWnd)
{
	ASSERT_VALID (pTabWnd);
	ASSERT_VALID (pDC);

	if (!CanDrawImage ())
	{
		CBCGPVisualManager2003::OnDrawTab (pDC, rectTab, iTab, bIsActive, pTabWnd);
		return;
	}

	if(pTabWnd->IsOneNoteStyle () ||
	   pTabWnd->IsColored () ||
	   pTabWnd->IsVS2005Style () ||
	   pTabWnd->IsLeftRightRounded ())
	{
		CBCGPVisualManager2003::OnDrawTab (pDC, rectTab, iTab, bIsActive, pTabWnd);
		return;
	}

	const BOOL bBottom = pTabWnd->GetLocation () == CBCGPTabWnd::LOCATION_BOTTOM;
	const BOOL bIsHighlight = iTab == pTabWnd->GetHighlightedTab ();

	COLORREF clrText = pTabWnd->GetTabTextColor (iTab);
	
	if (pTabWnd->IsFlatTab ())
	{
		int nImage = (bIsActive || bIsHighlight) ? 2 : 1;

		CRgn rgn;

		POINT pts[4];

		if (bBottom)
		{
			rectTab.bottom++;

			pts[0].x = rectTab.left;
			pts[0].y = rectTab.bottom + 1;
			pts[1].x = rectTab.left;
			pts[1].y = rectTab.top;
			pts[2].x = rectTab.right + 1;
			pts[2].y = rectTab.top;
			pts[3].x = rectTab.right - rectTab.Height () + 1;
			pts[3].y = rectTab.bottom + 1;

			rectTab.top++;
		}
		else
		{
			pts[0].x = rectTab.left;
			pts[0].y = rectTab.bottom + 1;
			pts[1].x = rectTab.left;
			pts[1].y = rectTab.top;
			pts[2].x = rectTab.right - rectTab.Height () + 1;
			pts[2].y = rectTab.top;
			pts[3].x = rectTab.right + 1;
			pts[3].y = rectTab.bottom + 1;
		}

		rgn.CreatePolygonRgn (pts, 4, WINDING);

		int isave = pDC->SaveDC ();

		pDC->SelectClipRgn (&rgn, RGN_AND);

		m_ctrlTabFlat[bBottom ? 1 : 0].Draw (pDC, rectTab, nImage);

		CPen* pOldPen = pDC->SelectObject (&m_penTabFlatOuter[bIsActive ? 1 : 0]);

		if (bBottom)
		{
			pDC->MoveTo (pts[2].x, pts[2].y);
			pDC->LineTo (pts[3].x, pts[3].y - 1);
		}
		else
		{
			pDC->MoveTo (pts[2].x - 1, pts[2].y);
			pDC->LineTo (pts[3].x - 1, pts[3].y - 1);
		}

		pDC->SelectObject (&m_penTabFlatInner[bIsActive ? 1 : 0]);

		if (bBottom)
		{
			pDC->MoveTo (pts[2].x - 2, pts[2].y + 1);
			pDC->LineTo (pts[3].x, pts[3].y - 2);
		}
		else
		{
			pDC->MoveTo (pts[2].x - 1, pts[2].y + 1);
			pDC->LineTo (pts[3].x - 2, pts[3].y - 1);
		}

		pDC->SelectObject (pOldPen);

		pDC->SelectClipRgn (NULL);

		if (clrText == (COLORREF)-1)
		{
			clrText = bIsActive
						? m_clrTabFlatTextActive
						: bIsHighlight
							? m_clrTabFlatTextHighlighted
							: m_clrTabFlatTextInactive;
		}

		if(clrText == (COLORREF)-1)
		{
			clrText = globalData.clrBarText;
		}

		pDC->RestoreDC (isave);
	}
	else
	{
		if(clrText == (COLORREF)-1)
		{
			clrText = bIsActive
						? m_clrTab3DTextActive
						: bIsHighlight
							? m_clrTab3DTextHighlighted
							: m_clrTab3DTextInactive;
		}

		int nImage = bIsActive ? 3 : 0;
		if(bIsHighlight)
		{
			nImage += 1;
		}

		//m_ctrlTab3D[bBottom ? 1 : 0].Draw (pDC, rectTab, nImage);
		//////////////////////////////////////////////////////////////////////////

		if (pTabWnd->IsDialogControl ())
		{
			clrText = globalData.clrBtnText;
		}
	}

	COLORREF clrTextOld = (COLORREF)-1;
	if (pTabWnd->IsFlatTab ())
	{
		if (clrText != (COLORREF)-1)
		{
			clrTextOld = pDC->SetTextColor (clrText);
		}
	}

	OnDrawTabContent (pDC, rectTab, iTab, bIsActive, pTabWnd, clrText);

	if (pTabWnd->IsFlatTab ())
	{
		if (clrText != (COLORREF)-1)
		{
			pDC->SetTextColor (clrTextOld);
		}
	}

	//CBCGPDrawManager dm(*pDC);
	//dm.DrawRect( rectTab, g_crToolbarBg, g_crBarBorderOut );
}

//*****************************************************************************
//绘制tab页签下面的托盘区域（类似tabbar的区域）。
void CCxBCGPVisualManager2007::OnEraseTabsArea (CDC* pDC, CRect rect, 
										 const CBCGPBaseTabWnd* pTabWnd)
{
	ASSERT_VALID (pDC);
	ASSERT_VALID (pTabWnd);

	if (!CanDrawImage () || pTabWnd->IsDialogControl ())
	{
		CBCGPVisualManager2003::OnEraseTabsArea (pDC, rect, pTabWnd);
		return;
	}

	if(pTabWnd->IsOneNoteStyle () ||
	   pTabWnd->IsColored () ||
	   pTabWnd->IsVS2005Style () ||
	   pTabWnd->IsLeftRightRounded ())
	{
		CBCGPVisualManager2003::OnEraseTabsArea (pDC, rect, pTabWnd);
		return;
	}

	const BOOL bBottom = pTabWnd->GetLocation () == CBCGPTabWnd::LOCATION_BOTTOM;

	if (pTabWnd->IsFlatTab ())
	{
		m_ctrlTabFlat[bBottom ? 1 : 0].Draw (pDC, rect);
	}
	else
	{
		//CBCGPDrawManager dm (*pDC);

		//COLORREF clr1 = m_clrBarGradientDark;
		//COLORREF clr2 = m_clrBarGradientLight;

		//if (bBottom)
		//{
		//	dm.FillGradient (rect, clr1, clr2, TRUE);
		//}
		//else
		//{
		//	dm.FillGradient (rect, clr2, clr1, TRUE);
		//}
	}

	CBCGPDrawManager dm(*pDC);
	dm.FillGradient( rect, g_crTabBarBg2, g_crTabBarBg );
	//dm.DrawLine(rect.left, rect.top + 0, rect.right, rect.top + 0, g_crTestBoder);
	//dm.DrawLine(rect.left, rect.bottom , rect.right, rect.bottom , g_crTestBoder);
}

//*****************************************************************************
BOOL CCxBCGPVisualManager2007::OnEraseTabsFrame (CDC* pDC, CRect rect, const CBCGPBaseTabWnd* pTabWnd)
{
	ASSERT_VALID (pDC);
	ASSERT_VALID (pTabWnd);

	if (!CanDrawImage () || pTabWnd->IsDialogControl ())
	{
		return CBCGPVisualManager2003::OnEraseTabsFrame (pDC, rect, pTabWnd);
	}

	if(pTabWnd->IsOneNoteStyle () ||
		pTabWnd->IsColored () ||
		pTabWnd->IsVS2005Style () ||
		pTabWnd->IsLeftRightRounded ())
	{
		return CBCGPVisualManager2003::OnEraseTabsFrame (pDC, rect, pTabWnd);
	}

	if (pTabWnd->IsFlatTab ())
	{
		pDC->FillRect (rect, &globalData.brWindow);

		if (pTabWnd->GetLocation () != CBCGPTabWnd::LOCATION_BOTTOM)
		{
			CPen pen (PS_SOLID, 1, m_clrTabFlatBlack);
			CPen* pOldPen = pDC->SelectObject (&pen);

			pDC->MoveTo (rect.left, rect.top + pTabWnd->GetTabsHeight () + 1);
			pDC->LineTo (rect.right, rect.top + pTabWnd->GetTabsHeight () + 1);

			pDC->SelectObject (pOldPen);
		}

		return TRUE;
	}

	//////////////////////////////////////////////////////////////////////////

	return FALSE;
}

//*****************************************************************************
void CCxBCGPVisualManager2007::OnFillTab (CDC* pDC, CRect rectFill, CBrush* pbrFill,
									 int iTab, BOOL bIsActive, 
									 const CBCGPBaseTabWnd* pTabWnd)
{
	CBCGPVisualManager2007::OnFillTab (pDC, rectFill, pbrFill,
		iTab, bIsActive, 
		pTabWnd);
}

//*****************************************************************************
//tabbar区域有按钮状态需要更新时，被调用
void CCxBCGPVisualManager2007::OnEraseTabsButton (CDC* pDC, CRect rect,
											  CBCGPButton* pButton,
											  CBCGPBaseTabWnd* pBaseTab)
{
	ASSERT_VALID (pDC);
	ASSERT_VALID (pButton);
	ASSERT_VALID (pBaseTab);

	CBCGPTabWnd* pWndTab = DYNAMIC_DOWNCAST (CBCGPTabWnd, pBaseTab);

	if (!CanDrawImage () || 
		pWndTab == NULL || 
		pBaseTab->IsDialogControl ())
	{
		CBCGPVisualManager2003::OnEraseTabsButton (pDC, rect, pButton, pBaseTab);
		return;
	}

	if(pBaseTab->IsFlatTab () || 
	   pBaseTab->IsOneNoteStyle () ||
	   pBaseTab->IsColored () ||
	   pBaseTab->IsVS2005Style () ||
	   pBaseTab->IsLeftRightRounded () ||
	   (!pButton->IsPressed () && !pButton->IsHighlighted ()))
	{
		CBCGPVisualManager2003::OnEraseTabsButton (pDC, rect, pButton, pBaseTab);
		return;
	}

	CRgn rgn;
	rgn.CreateRectRgnIndirect (rect);

	pDC->SelectClipRgn (&rgn);

	CRect rectTabs;
	pWndTab->GetClientRect (&rectTabs);

	CRect rectTabArea;
	pWndTab->GetTabsRect (rectTabArea);

	if (pWndTab->GetLocation () == CBCGPBaseTabWnd::LOCATION_BOTTOM)
	{
		rectTabs.top = rectTabArea.top;
	}
	else
	{
		rectTabs.bottom = rectTabArea.bottom;
	}

	pWndTab->MapWindowPoints (pButton, rectTabs);
	OnEraseTabsArea (pDC, rectTabs, pWndTab);

	pDC->SelectClipRgn (NULL);

	int index = pButton->IsPressed () ? 2 : 1;
	m_ctrlToolBarBtn.Draw (pDC, rect, index);
}


//*****************************************************************************
//OnEraseTabsButton调用完后，被调用
void CCxBCGPVisualManager2007::OnDrawTabsButtonBorder (CDC* pDC, CRect& rect, 
												 CBCGPButton* pButton, UINT uiState,
												 CBCGPBaseTabWnd* pWndTab)
{
	if (!CanDrawImage ())
	{
		CBCGPVisualManager2003::OnDrawTabsButtonBorder(pDC, rect, 
												 pButton, uiState,
												 pWndTab);
	}

	ASSERT_VALID(pButton);

	//pButton->ShowWindow(SW_HIDE);

	if (pButton->IsPushed () || pButton->IsHighlighted ())
	{
		COLORREF clrDark = globalData.clrBarDkShadow;
		pDC->Draw3dRect (rect, clrDark, clrDark);
	}

	//pDC->Draw3dRect (rect, g_crBarBorderOut, g_crBarBorderOut);
}


//*********************************************************************************
void CCxBCGPVisualManager2007::OnDrawTabContent (CDC* pDC, CRect rectTab,
						int iTab, BOOL bIsActive, const CBCGPBaseTabWnd* pTabWnd,
						COLORREF clrText)
{
	ASSERT_VALID (pTabWnd);
	ASSERT_VALID (pDC);

	if (pTabWnd->IsActiveTabCloseButton () && bIsActive)
	{
		CRect rectClose = pTabWnd->GetTabCloseButton ();
		rectClose.DeflateRect( (rectClose.Width() - 12)/2, (rectClose.Height() - 12)/2 );
		//rectTab.right = rectClose.left;

		//OnDrawTabCloseButton (pDC, rectClose, pTabWnd, 
		//	pTabWnd->IsTabCloseButtonHighlighted (),
		//	pTabWnd->IsTabCloseButtonPressed (),
		//	FALSE /* Disabled */);

	}

	rectTab.right -= rectTab.Height() - 16;
	rectTab.OffsetRect(0, -1);

	CString strText;
	pTabWnd->GetTabLabel (iTab, strText);

	if (pTabWnd->IsFlatTab ())
	{
		//---------------
		// Draw tab text:
		//---------------
		UINT nFormat = DT_SINGLELINE | DT_CENTER | DT_VCENTER;
		if (pTabWnd->IsDrawNoPrefix ())
		{
			nFormat |= DT_NOPREFIX;
		}

		pDC->DrawText (strText, rectTab, nFormat);
	}
	else
	{
		CSize sizeImage = pTabWnd->GetImageSize ();
		UINT uiIcon = pTabWnd->GetTabIcon (iTab);
		HICON hIcon = pTabWnd->GetTabHicon (iTab);

		if (uiIcon == (UINT)-1 && hIcon == NULL)
		{
			sizeImage.cx = 0;
		}

		if (sizeImage.cx + 2 * CBCGPBaseTabWnd::TAB_IMAGE_MARGIN <= rectTab.Width ())
		{
			if (hIcon != NULL)
			{
				//---------------------
				// Draw the tab's icon:
				//---------------------
				CRect rectImage = rectTab;

				rectImage.top += (rectTab.Height () - sizeImage.cy) / 2;
				rectImage.bottom = rectImage.top + sizeImage.cy;

				rectImage.left += IMAGE_MARGIN;
				rectImage.right = rectImage.left + sizeImage.cx;

				::DrawIconEx (pDC->GetSafeHdc (),
					rectImage.left, rectImage.top, hIcon,
					0, 0, 0, NULL, DI_NORMAL);
			}
			else
			{
				const CImageList* pImageList = pTabWnd->GetImageList ();
				if (pImageList != NULL && uiIcon != (UINT)-1)
				{
					//----------------------
					// Draw the tab's image:
					//----------------------
					CRect rectImage = rectTab;

					rectImage.top += (rectTab.Height () - sizeImage.cy) / 2;
					rectImage.bottom = rectImage.top + sizeImage.cy;

					rectImage.left += IMAGE_MARGIN;
					rectImage.right = rectImage.left + sizeImage.cx;

					ASSERT_VALID (pImageList);
					((CImageList*) pImageList)->Draw (pDC, uiIcon, rectImage.TopLeft (), ILD_TRANSPARENT);
				}
			}

			//------------------------------
			// Finally, draw the tab's text:
			//------------------------------
			CRect rcText = rectTab;
			rcText.left += sizeImage.cx + 2 * TEXT_MARGIN;

			if (rcText.Width () < sizeImage.cx * 2 && 
				!pTabWnd->IsLeftRightRounded ())
			{
				rcText.right -= TEXT_MARGIN;
			}
			
			if (clrText == (COLORREF)-1)
			{
				clrText = GetTabTextColor (pTabWnd, iTab, bIsActive);
			}

			if (clrText != (COLORREF)-1)
			{
				pDC->SetTextColor (clrText);
			}

			CString strText;
			pTabWnd->GetTabLabel (iTab, strText);

			UINT nFormat = DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_CENTER;
			if (pTabWnd->IsDrawNoPrefix ())
			{
				nFormat |= DT_NOPREFIX;
			}

			if (pTabWnd->IsOneNoteStyle () || pTabWnd->IsVS2005Style ())
			{
				nFormat |= DT_CENTER;
			}
			else
			{
				nFormat |= DT_CENTER; //DT_LEFT;
			}

			rcText.OffsetRect(-4, 2);
			pDC->DrawText (strText, rcText, nFormat);
		}
	}
}

//*****************************************************************************
int CCxBCGPVisualManager2007::GetTabHorzMargin (const CBCGPBaseTabWnd* pTabWnd)
{
	if (!CanDrawImage ())
	{
		return CBCGPVisualManager2003::GetTabHorzMargin (pTabWnd);
	}

	CBCGPControlRenderer* pRenderer = pTabWnd->IsFlatTab ()
										? &m_ctrlTabFlat[0]
										: &m_ctrlTab3D[0];

	if(pTabWnd->IsOneNoteStyle () ||
	   pTabWnd->IsColored () ||
	   pTabWnd->IsVS2005Style () ||
	   pTabWnd->IsLeftRightRounded () ||
	   !pRenderer->IsValid ())
	{
		return CBCGPVisualManager2003::GetTabHorzMargin (pTabWnd);
	}

	return pRenderer->GetParams ().m_rectSides.right / 2;
}

//**********************************************************************************
void CCxBCGPVisualManager2007::OnDrawTabCloseButton (CDC* pDC, CRect rect, 
											   const CBCGPBaseTabWnd* /*pTabWnd*/,
											   BOOL bIsHighlighted,
											   BOOL bIsPressed,
											   BOOL /*bIsDisabled*/)
{
	if (bIsHighlighted)
	{
		pDC->FillRect (rect, &globalData.brBarFace);
	}

	CBCGPMenuImages::Draw (pDC, CBCGPMenuImages::IdClose, rect, CBCGPMenuImages::ImageBlack);

	if (bIsHighlighted)
	{
		if (bIsPressed)
		{
			pDC->Draw3dRect (rect, globalData.clrBarDkShadow, globalData.clrBarHilite);
		}
		else
		{
			pDC->Draw3dRect (rect, globalData.clrBarHilite, globalData.clrBarDkShadow);
		}
	}
}


//********************************************************************************
BOOL CCxBCGPVisualManager2007::OnDrawGridHeaderItemBorder (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rect, BOOL bPressed)
{
	if (!CanDrawImage ())
	{
		return CBCGPVisualManager2003::OnDrawGridHeaderItemBorder (pCtrl, pDC, rect, bPressed);
	}

	ASSERT_VALID(pDC);

	COLORREF clrStart  = bPressed ? m_clrGridHeaderPressedStart : m_clrGridHeaderNormalStart;
	COLORREF clrFinish = bPressed ? m_clrGridHeaderPressedFinish : m_clrGridHeaderNormalFinish;
	COLORREF clrBorder = bPressed ? m_clrGridHeaderPressedBorder : m_clrGridHeaderNormalBorder;

	{
		//CBCGPDrawManager dm (*pDC);
		//dm.FillGradient (rect, clrFinish, clrStart);

		CBCGPDrawManager dm (*pDC);
		dm.DrawRect (rect, g_crStructTreeHeaderBg, g_crStructTreeHeaderBg);
	}

	CPen* pOldPen = pDC->SelectObject (&m_penGridSeparator);

	//int nHeight = rect.Height () / 5;
	int nHeight = 2;
	if (pCtrl->GetHeaderFlags () & BCGP_GRID_HEADER_FORCESIMPLEBORDERS)
	{
		nHeight = 0;
	}

	//分割线
	pDC->MoveTo (rect.right - 1, rect.top + nHeight);
	pDC->LineTo (rect.right - 1, rect.bottom - nHeight);

	pDC->SelectObject (pOldPen);

	{
		//上边线
		CPen pen (PS_SOLID, 1, g_crStructTreeHeaderTop);
		pOldPen = pDC->SelectObject (&pen);
		pDC->MoveTo (rect.left, rect.top);
		pDC->LineTo (rect.right, rect.top);
		//下边线
		CPen pen2 (PS_SOLID, 1, g_crStructTreeHeaderBottom);
		pDC->SelectObject (&pen2);
		pDC->MoveTo (rect.left, rect.bottom - 1);
		pDC->LineTo (rect.right, rect.bottom - 1);

		pDC->SelectObject (pOldPen);
	}

	return FALSE;
}
//********************************************************************************
//当表格有左侧、顶侧标题时，左上角区域的边框
BOOL CCxBCGPVisualManager2007::OnDrawGridSelectAllAreaBorder (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rect, BOOL bPressed)
{
	if (!CanDrawImage ())
	{
		return CBCGPVisualManager2003::OnDrawGridSelectAllAreaBorder (pCtrl, pDC, rect, bPressed);
	}

	ASSERT_VALID(pDC);

	COLORREF clrBorder = bPressed ? m_clrGridHeaderPressedBorder : m_clrGridHeaderNormalBorder;
	pDC->Draw3dRect (rect, clrBorder, clrBorder);

	rect.DeflateRect (1, 1);

	pDC->Draw3dRect 
		(
			rect, 
			bPressed ? m_clrGridHeaderAllPressedBorderHighlighted : m_clrGridHeaderAllNormalBorderHighlighted, 
			bPressed ? m_clrGridHeaderAllPressedBorderShadow : m_clrGridHeaderAllNormalBorderShadow
		);

	return FALSE;
}

BOOL CCxBCGPVisualManager2007::OnUpdateNcButtons( CWnd* pWnd, const CObList& lstSysButtons, CRect rectCaption )
{
	CBCGPDialog* pDlg = dynamic_cast<CBCGPDialog*>(pWnd);

	if ((!pDlg || pDlg->IsVisualManagerNCArea() == FALSE))
	{
		return CBCGPVisualManager2007::OnUpdateNcButtons(pWnd, lstSysButtons, rectCaption);
	}

	CSize sizeButton = GetNcBtnSize (FALSE);
	sizeButton.cy = min (sizeButton.cy, rectCaption.Height () - 2);

	CSize sizeCloseButton = m_szNcCloseBtnSize;
	sizeCloseButton.cy = sizeButton.cy;

	int x = rectCaption.right - sizeCloseButton.cx;
	int y = rectCaption.top + max (0, (rectCaption.Height () - sizeButton.cy) / 2);

	int i = 0;
	CRect rcBar;
	pWnd->GetWindowRect(rcBar);
	pWnd->ScreenToClient (rcBar);
	for (POSITION pos = lstSysButtons.GetHeadPosition (); pos != NULL;)
	{
		CBCGPFrameCaptionButton* pButton = (CBCGPFrameCaptionButton*)
			lstSysButtons.GetNext (pos);
		ASSERT_VALID (pButton);

		if (pButton->m_nHit == HTCLOSE_BCG)
		{
			pButton->SetRect (CRect (CPoint (x, y), sizeCloseButton));
		}
		else
		{
			pButton->SetRect (CRect (CPoint (x, y), sizeButton));
		}

		x -= sizeButton.cx;
	}

	return TRUE;
}

//*********************************************************************************
//绘制Tab WndArea区域 左、右、下 最外层边线，全屏时，相当于应用程序的 左、右、下边线
void CCxBCGPVisualManager2007::OnDrawTabBorder(CDC* pDC, CBCGPTabWnd* pTabWnd, CRect rectBorder, COLORREF clrBorder,
										 CPen& penLine)
{
	ASSERT_VALID (pDC);
	ASSERT_VALID (pTabWnd);

	pDC->Draw3dRect (&rectBorder, clrBorder, clrBorder);
	//pDC->Draw3dRect (&rectBorder, g_crTestBoder, g_crTestBoder );

	if (!pTabWnd->IsOneNoteStyle ())
	{
		int xRight = rectBorder.right - 1;

		if (!pTabWnd->IsDrawFrame())
		{
			xRight -= pTabWnd->GetTabBorderSize ();
		}

		CPen pen1 (PS_SOLID, 1, g_crTabBarBg2);

		pDC->SelectObject (&pen1);

		int y = pTabWnd->GetLocation () == CBCGPBaseTabWnd::LOCATION_BOTTOM ? rectBorder.bottom - 1 : rectBorder.top;

		pDC->MoveTo (rectBorder.left, y);
		pDC->LineTo (xRight, y);
	}
}
/************************************************************************/
/* 重载基类的该函数,解决无WS_THICKFRAME属性时标题栏显示的问题及有WS_THICKFRAME属性但无图标时最大化后标题栏文字显示不全的问题     */
/************************************************************************/
void CCxBCGPVisualManager2007::DrawNcCaption (CDC* pDC, CRect rectCaption, 
	DWORD dwStyle, DWORD dwStyleEx,
	const CString& strTitle, const CString& strDocument,
	HICON hIcon, BOOL bPrefix, BOOL bActive, 
	BOOL bTextCenter,
	const CObList& lstSysButtons)
{
	const BOOL bIsRTL           = (dwStyleEx & WS_EX_LAYOUTRTL) == WS_EX_LAYOUTRTL;
	const BOOL bIsSmallCaption	= (dwStyleEx & WS_EX_TOOLWINDOW) != 0;
	const int nSysCaptionHeight = bIsSmallCaption ? ::GetSystemMetrics (SM_CYSMCAPTION) : ::GetSystemMetrics (SM_CYCAPTION);
	CSize szSysBorder (GetSystemBorders (FALSE));

	CDC memDC;
	memDC.CreateCompatibleDC (pDC);
	CBitmap memBmp;
	memBmp.CreateCompatibleBitmap (pDC, rectCaption.Width (), rectCaption.Height ());
	CBitmap* pBmpOld = memDC.SelectObject (&memBmp);
	memDC.BitBlt (0, 0, rectCaption.Width (), rectCaption.Height (), pDC, 0, 0, SRCCOPY);

	BOOL bMaximized = (dwStyle & WS_MAXIMIZE) == WS_MAXIMIZE;

	{
		if (IsBeta ())
		{
			COLORREF clr1  = bActive 
				? m_clrAppCaptionActiveStart 
				: m_clrAppCaptionInactiveStart;
			COLORREF clr2  = bActive 
				? m_clrAppCaptionActiveFinish 
				: m_clrAppCaptionInactiveFinish;

			CRect rectCaption1 (rectCaption);
			CRect rectBorder (m_ctrlMainBorderCaption.GetParams ().m_rectSides);

			rectCaption1.DeflateRect (rectBorder.left, rectBorder.top, 
				rectBorder.right, rectBorder.bottom);

			{
				CBCGPDrawManager dm (memDC);
				dm.Fill4ColorsGradient (rectCaption1, clr1, clr2, clr2, clr1, FALSE);
			}

			m_ctrlAppBorderCaption.DrawFrame (&memDC, rectCaption, bActive ? 0 : 1);
		}
		else
		{
			CRect rectBorderCaption (rectCaption);
			if (bMaximized)
			{
				rectBorderCaption.OffsetRect (-rectBorderCaption.TopLeft ());
				rectBorderCaption.bottom -= szSysBorder.cy;
			}

			m_ctrlAppBorderCaption.Draw (&memDC, rectBorderCaption, bActive ? 0 : 1);
		}
	}
	BOOL bThickFrame = dwStyle & WS_THICKFRAME;
	CRect rect (rectCaption);
	rect.DeflateRect (szSysBorder.cx, szSysBorder.cy, szSysBorder.cx, 0);
	rect.top = rect.bottom - nSysCaptionHeight - 1;

	// Draw icon:
	if (hIcon != NULL && !bIsSmallCaption)
	{
		CSize szIcon (::GetSystemMetrics (SM_CXSMICON), ::GetSystemMetrics (SM_CYSMICON));
		long x = rect.left + (bMaximized ? szSysBorder.cx : 0) + 2;
		long y = rect.top + max (0, (nSysCaptionHeight - szIcon.cy) / 2);
		if ((!bMaximized && bThickFrame))
			y = y - 2; // 有WS_THICKFRAME属性且不是最大化时候

		::DrawIconEx (memDC.GetSafeHdc (), x, y, hIcon, szIcon.cx, szIcon.cy,
			0, NULL, DI_NORMAL);

		rect.left = x + szIcon.cx + (bMaximized ? szSysBorder.cx : 4);
	}

	// Draw system buttons:
	int xButtonsRight = rect.right;

	for (POSITION pos = lstSysButtons.GetHeadPosition (); pos != NULL;)
	{
		CBCGPFrameCaptionButton* pButton = (CBCGPFrameCaptionButton*)
			lstSysButtons.GetNext (pos);
		ASSERT_VALID (pButton);

		BCGBUTTON_STATE state = ButtonsIsRegular;

		if (pButton->m_bPushed && pButton->m_bFocused)
		{
			state = ButtonsIsPressed;
		}
		else if (pButton->m_bFocused)
		{
			state = ButtonsIsHighlighted;
		}

		UINT uiHit = pButton->GetHit ();
		UINT nButton = 0;

		switch (uiHit)
		{
		case HTCLOSE_BCG:
			nButton = SC_CLOSE;
			break;

		case HTMAXBUTTON_BCG:
			nButton = 
				(dwStyle & WS_MAXIMIZE) == WS_MAXIMIZE ? SC_RESTORE : SC_MAXIMIZE;
			break;

		case HTMINBUTTON_BCG:
			nButton = 
				(dwStyle & WS_MINIMIZE) == WS_MINIMIZE ? SC_RESTORE : SC_MINIMIZE;
			break;

		case HTHELPBUTTON_BCG:
			nButton = SC_CONTEXTHELP;
			break;
		}

		CRect rectBtn (pButton->GetRect ());
		// 根据不同情况修改按钮显示位置，使得按钮垂直居中
		if (bMaximized )
		{
			if (hIcon!=NULL)
				rectBtn.OffsetRect (szSysBorder.cx, szSysBorder.cy);
		}
		else
		{
			if (bThickFrame)
			{
				int iH = rectBtn.Height();
				rectBtn.top -= szSysBorder.cy/2;
				rectBtn.bottom = rectBtn.top + iH;
			}
			else
			{
				int iH = rectBtn.Height();
				rectBtn.top = rectBtn.top - szSysBorder.cy/2 + 2;
				rectBtn.bottom = rectBtn.top + iH;
			}
			
		}

		DrawNcBtn (&memDC, rectBtn, nButton, state, FALSE, bActive, FALSE);

		xButtonsRight = min (xButtonsRight, pButton->GetRect ().left);
	}

	// Draw text:
	if ((!strTitle.IsEmpty () || !strDocument.IsEmpty ()) && 
		rect.left < rect.right)
	{
		CFont* pOldFont = (CFont*)memDC.SelectObject (&m_AppCaptionFont);

		CRect rectText = rect;
		rectText.right = xButtonsRight - 1;
		// 根据不同情况修改文字显示位置，使得标题文本垂直居中
		if (bMaximized)
		{
			if ( hIcon==NULL)
			{
				rectText.top   = (rectCaption.Height() - nSysCaptionHeight)/2 - 2;
				rectText.bottom = rectText.top + nSysCaptionHeight - 2;
			}
		}
		else
		{
			rectText.top   = (rectCaption.Height() + szSysBorder.cy - nSysCaptionHeight)/2 - 3;
			rectText.bottom = rectText.top + nSysCaptionHeight;
		}
		

		DrawNcText (&memDC, rectText, strTitle, strDocument, bPrefix, bActive, bIsRTL, bTextCenter);

		memDC.SelectObject (pOldFont);
	}

	pDC->BitBlt (rectCaption.left, rectCaption.top, rectCaption.Width (), rectCaption.Height (),
		&memDC, 0, 0, SRCCOPY);

	memDC.SelectObject (pBmpOld);
}