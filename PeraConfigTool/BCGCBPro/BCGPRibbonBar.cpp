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
//
// BCGPRibbonBar.cpp : implementation file
//

#include "stdafx.h"
#include "BCGPContextMenuManager.h"
#include "BCGPRibbonBar.h"
#include "BCGPRibbonCategory.h"
#include "BCGPRibbonButton.h"
#include "BCGPFrameWnd.h"
#include "BCGPMDIFrameWnd.h"
#include "BCGPVisualManager.h"
#include "BCGPTooltipManager.h"
#include "BCGPToolTipCtrl.h"
#include "trackmouse.h"
#include "BCGPToolbarMenuButton.h"
#include "RegPath.h"
#include "BCGPRegistry.h"
#include "BCGPRibbonPanel.h"
#include "BCGPRibbonPanelMenu.h"
#include "BCGPRibbonMainPanel.h"
#include "BCGPRibbonBackstageViewPanel.h"
#include "BCGPLocalResource.h"
#include "BCGPRibbonCustomizePage.h"
#include "BCGPRibbonEdit.h"
#include "BCGPKeyboardManager.h"
#include "BCGPRibbonKeyTip.h"
#include "BCGPRibbonPaletteButton.h"
#include "BCGPRibbonDialogBar.h"
#include "BCGPRibbonInfoLoader.h"
#include "BCGPRibbonConstructor.h"
#include "BCGPRibbonCollector.h"
#include "BCGPRibbonInfoWriter.h"
#include "BCGPRibbonFloaty.h"
#include "BCGPKeyHelper.h"
#include "BCGPRibbonBackstageView.h"
#include "BCGPRibbonBackstagePagePrint.h"

#ifndef BCGP_EXCLUDE_RIBBON

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT BCGM_POSTRECALCLAYOUT = ::RegisterWindowMessage (_T("BCGM_POSTRECALCLAYOUT"));
UINT BCGM_ON_CHANGE_RIBBON_CATEGORY = ::RegisterWindowMessage (_T("BCGM_ON_CHANGE_RIBBON_CATEGORY"));
UINT BCGM_ON_RIBBON_CUSTOMIZE = ::RegisterWindowMessage (_T("BCGM_ON_RIBBON_CUSTOMIZE"));
UINT BCGM_ON_HIGHLIGHT_RIBBON_LIST_ITEM = ::RegisterWindowMessage (_T("BCGM_ON_HIGHLIGHT_RIBBON_LIST_ITEM"));
UINT BCGM_ON_BEFORE_SHOW_RIBBON_ITEM_MENU = ::RegisterWindowMessage (_T("BCGM_ON_BEFORE_SHOW_RIBBON_ITEM_MENU"));
UINT BCGM_ON_BEFORE_SHOW_PALETTE_CONTEXTMENU = ::RegisterWindowMessage (_T("BCGM_ON_BEFORE_SHOW_PALETTE_CONTEXTMENU"));

static const int nMinRibbonWidth = 300;
static const int nMinRibbonHeight = 250;

static const int nTooltipMinWidthDefault = 210;
static const int nTooltipWithImageMinWidthDefault = 318;
static const int nTooltipMaxWidth = 640;

static const int xTabMargin = 14;
static const int yTabMargin = 5;

static const int xMargin = 2;
static const int yMargin = 2;

static const UINT IdAutoCommand = 1;
static const UINT IdShowKeyTips = 2;

static const int idToolTipClient = 1;
static const int idToolTipCaption = 2;

static const UINT idCut			= (UINT) -10002;
static const UINT idCopy		= (UINT) -10003;
static const UINT idPaste		= (UINT) -10004;
static const UINT idSelectAll	= (UINT) -10005;

static const CString strRibbonProfile	= _T("BCGPRibbons");

#define REG_SECTION_FMT					_T("%sBCGRibbonBar-%d")
#define REG_SECTION_FMT_EX				_T("%sBCGRibbonBar-%d%x")

#define REG_ENTRY_QA_TOOLBAR_LOCATION	_T("QuickAccessToolbarOnTop")
#define REG_ENTRY_QA_TOOLBAR_COMMANDS	_T("QuickAccessToolbarCommands")
#define REG_ENTRY_RIBBON_IS_MINIMIZED	_T("IsMinimized")

#ifdef _UNICODE
	#define _TCF_TEXT	CF_UNICODETEXT
#else
	#define _TCF_TEXT	CF_TEXT
#endif

/////////////////////////////////////////////////////////////////////////////
// CBCGPRibbonBar idle update through CBCGPRibbonCmdUI class

CBCGPRibbonCmdUI::CBCGPRibbonCmdUI ()
{
	m_pUpdated = NULL;
}
//*************************************************************************************
void CBCGPRibbonCmdUI::Enable (BOOL bOn)
{
	m_bEnableChanged = TRUE;

	ASSERT_VALID (m_pOther);
	ASSERT_VALID (m_pUpdated);

	const BOOL bIsDisabled = !bOn;

	if (m_pUpdated->IsDisabled () != bIsDisabled)
	{
		m_pUpdated->m_bIsDisabled = bIsDisabled;
		m_pUpdated->OnEnable (!bIsDisabled);
		m_pOther->RedrawWindow (m_pUpdated->GetRect ());
	}
}
//*************************************************************************************
void CBCGPRibbonCmdUI::SetCheck (int nCheck)
{
	ASSERT_VALID (m_pOther);
	ASSERT_VALID (m_pUpdated);

	if (m_pUpdated->IsChecked () != (BOOL)nCheck)
	{
		m_pUpdated->m_bIsChecked = (BOOL)nCheck;
		m_pUpdated->OnCheck ((BOOL)nCheck);
		m_pOther->RedrawWindow (m_pUpdated->GetRect ());
	}
}
//*************************************************************************************
void CBCGPRibbonCmdUI::SetRadio (BOOL bOn)
{
	ASSERT_VALID (m_pUpdated);

	m_pUpdated->m_bIsRadio = bOn;
	SetCheck (bOn ? 1 : 0);
}
//*************************************************************************************
void CBCGPRibbonCmdUI::SetText (LPCTSTR lpszText)
{
	ASSERT (lpszText != NULL);

	ASSERT_VALID (m_pOther);
	ASSERT_VALID (m_pUpdated);

	if (lstrcmp (m_pUpdated->GetText (), lpszText) != 0)
	{
		m_pUpdated->SetText (lpszText);
		m_pOther->RedrawWindow (m_pUpdated->GetRect ());
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBCGPRibbonCaptionButton

IMPLEMENT_DYNCREATE (CBCGPRibbonCaptionButton, CBCGPRibbonButton)

CBCGPRibbonCaptionButton::CBCGPRibbonCaptionButton (UINT uiCmd,
	HWND hwndMDIChild)
{
	m_nID = uiCmd;
	m_hwndMDIChild = hwndMDIChild;
}
//*******************************************************************************
void CBCGPRibbonCaptionButton::OnDraw (CDC* pDC)
{
	ASSERT_VALID (pDC);

	CBCGPVisualManager::GetInstance ()->OnDrawRibbonCaptionButton
		(pDC, this);
}
//*******************************************************************************
void CBCGPRibbonCaptionButton::OnLButtonUp (CPoint /*point*/)
{
	ASSERT_VALID (this);
	ASSERT (m_nID != 0);

	if (IsPressed () && IsHighlighted ())
	{
		if (m_hwndMDIChild != NULL)
		{
			::PostMessage (m_hwndMDIChild, WM_SYSCOMMAND, m_nID, 0);
		}
		else
		{
			ASSERT_VALID (m_pRibbonBar);
			m_pRibbonBar->GetParent ()->PostMessage (WM_SYSCOMMAND, m_nID);
		}

		m_bIsHighlighted = FALSE;
	}
}
//*******************************************************************************
CSize CBCGPRibbonCaptionButton::GetRegularSize (CDC* /*pDC*/)
{
	ASSERT_VALID (this);
	return CSize (::GetSystemMetrics (SM_CXMENUSIZE), ::GetSystemMetrics (SM_CYMENUSIZE));
}

/////////////////////////////////////////////////////////////////////////////
// CBCGPRibbonContextCaption

IMPLEMENT_DYNCREATE (CBCGPRibbonContextCaption, CBCGPRibbonButton)

CBCGPRibbonContextCaption::CBCGPRibbonContextCaption (
	LPCTSTR lpszName, UINT uiID, BCGPRibbonCategoryColor clrContext)
{
	m_strText = lpszName;
	m_uiID = uiID;
	m_Color = clrContext;
	m_nRightTabX = -1;
}
//*******************************************************************************
CBCGPRibbonContextCaption::CBCGPRibbonContextCaption ()
{
	m_uiID = 0;
	m_Color = BCGPCategoryColor_None;
	m_nRightTabX = -1;
}
//*******************************************************************************
void CBCGPRibbonContextCaption::OnDraw (CDC* pDC)
{
	ASSERT_VALID (pDC);

	if (m_rect.IsRectEmpty ())
	{
		return;
	}

	COLORREF clrText = CBCGPVisualManager::GetInstance()->OnDrawRibbonCategoryCaption(pDC, this);
	COLORREF clrTextOld = pDC->SetTextColor (clrText);

	CRect rectText = m_rect;
	rectText.left += xTabMargin / 2;

	CBCGPVisualManager::GetInstance ()->OnDrawRibbonCategoryCaptionText(pDC, this, m_strText, rectText,
		GetParentRibbonBar ()->IsTransparentCaption (),
		GetParentRibbonBar ()->GetParent ()->IsZoomed ());

	pDC->SetTextColor (clrTextOld);
}
//*******************************************************************************
void CBCGPRibbonContextCaption::OnLButtonUp (CPoint /*point*/)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pRibbonBar);

	if (m_pRibbonBar->GetActiveCategory () != NULL && 
		m_pRibbonBar->GetActiveCategory ()->GetContextID () == m_uiID &&
		(m_pRibbonBar->GetHideFlags () & BCGPRIBBONBAR_HIDE_ELEMENTS) == 0)
	{
		return;
	}

	for (int i = 0; i < m_pRibbonBar->GetCategoryCount (); i++)
	{
		CBCGPRibbonCategory* pCategory = m_pRibbonBar->GetCategory (i);
		ASSERT_VALID (pCategory);

		if (pCategory->GetContextID () == m_uiID)
		{
			m_pRibbonBar->SetActiveCategory (pCategory,
				m_pRibbonBar->GetHideFlags () & BCGPRIBBONBAR_HIDE_ELEMENTS);
			return;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CBCGPRibbonMinimizeButton

IMPLEMENT_DYNAMIC(CBCGPRibbonMinimizeButton, CBCGPRibbonButton);

CBCGPRibbonMinimizeButton::CBCGPRibbonMinimizeButton (CBCGPRibbonBar& parentRibbon) :
	 m_parentRibbon(parentRibbon)
{
	SetParentRibbonBar(&parentRibbon);

	m_bIsTabElement = TRUE;
	m_nID = (UINT)-1;

	CString strTooltip;

	{
		CBCGPLocalResource locaRes;
		strTooltip.LoadString (IDS_BCGBARRES_MINIMIZE_RIBBON);
	}

	strTooltip += _T(" (");

	ACCEL accell;
	accell.fVirt = FCONTROL | FVIRTKEY;
	accell.key = VK_F1;

	CBCGPKeyHelper key(&accell);

	CString strKey;
	key.Format(strKey);

	strTooltip += strKey;

	strTooltip += _T(')');

	SetToolTipText(strTooltip);
}

void CBCGPRibbonMinimizeButton::DrawImage (CDC* pDC, RibbonImageType /*type*/, CRect /*rectImage*/)
{
	ASSERT_VALID(pDC);

	CBCGPVisualManager::GetInstance()->OnDrawRibbonMinimizeButtonImage(pDC, this,
		(m_parentRibbon.GetHideFlags() & BCGPRIBBONBAR_HIDE_ELEMENTS));
}

CSize CBCGPRibbonMinimizeButton::GetImageSize (RibbonImageType /*type*/) const
{
	return CBCGPVisualManager::GetInstance()->GetRibbonMinimizeButtonImageSize();
}

//////////////////////////////////////////////////////////////////////
// CBCGPRibbonMainButton

IMPLEMENT_DYNCREATE(CBCGPRibbonMainButton, CBCGPRibbonButton);

void CBCGPRibbonMainButton::SetImage (UINT uiBmpResID)
{
	ASSERT_VALID (this);

	m_Image.Load (uiBmpResID);
	m_Image.SetSingleImage ();

	if (m_Image.IsValid () && m_Image.GetBitsPerPixel () < 32 && globalData.bIsWindowsVista)
	{
		m_Image.ConvertTo32Bits (globalData.clrBtnFace);
	}
}
//*******************************************************************************
void CBCGPRibbonMainButton::SetImage (HBITMAP hBmp)
{
	ASSERT_VALID (this);

	if (m_Image.IsValid ())
	{
		m_Image.Clear ();
	}

	if (hBmp == NULL)
	{
		return;
	}

	m_Image.AddImage (hBmp, TRUE);
	m_Image.SetSingleImage ();

	if (m_Image.IsValid () && m_Image.GetBitsPerPixel () < 32 && globalData.bIsWindowsVista)
	{
		m_Image.ConvertTo32Bits (globalData.clrBtnFace);
	}
}
//********************************************************************************
void CBCGPRibbonMainButton::SetScenicImage (UINT uiBmpResID)
{
	ASSERT_VALID (this);

	m_ImageScenic.Load (uiBmpResID);
	m_ImageScenic.SetSingleImage ();

	if (m_ImageScenic.IsValid () && m_ImageScenic.GetBitsPerPixel () < 32 && globalData.bIsWindowsVista)
	{
		m_ImageScenic.ConvertTo32Bits (globalData.clrBtnFace);
	}
}
//*******************************************************************************
void CBCGPRibbonMainButton::SetScenicImage (HBITMAP hBmp)
{
	ASSERT_VALID (this);

	if (m_ImageScenic.IsValid ())
	{
		m_ImageScenic.Clear ();
	}

	if (hBmp == NULL)
	{
		return;
	}

	m_ImageScenic.AddImage (hBmp, TRUE);
	m_ImageScenic.SetSingleImage ();

	if (m_ImageScenic.IsValid () && m_ImageScenic.GetBitsPerPixel () < 32 && globalData.bIsWindowsVista)
	{
		m_ImageScenic.ConvertTo32Bits (globalData.clrBtnFace);
	}
}
//********************************************************************************
void CBCGPRibbonMainButton::SetScenicText (LPCTSTR lpszText)
{
	ASSERT_VALID (this);

	m_strTextScenic = lpszText == NULL ? _T("") : lpszText;
}
//********************************************************************************
void CBCGPRibbonMainButton::OnLButtonDblClk (CPoint /*point*/)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pRibbonBar);

	if (!m_pRibbonBar->IsScenicLook ())
	{
		m_pRibbonBar->GetParent ()->PostMessage (WM_SYSCOMMAND, SC_CLOSE);
	}
}
//********************************************************************************
void CBCGPRibbonMainButton::OnLButtonDown (CPoint point)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pRibbonBar);

	if (m_pRibbonBar->GetMainCategory () == NULL && m_pRibbonBar->GetBackstageCategory () == NULL)
	{
		CBCGPRibbonButton::OnLButtonDown (point);
		return;
	}

	CBCGPBaseRibbonElement::OnLButtonDown (point);

	if (!ShowMainMenu ())
	{
		CBCGPRibbonButton::OnLButtonDown (point);
	}
}
//********************************************************************************
void CBCGPRibbonMainButton::OnCalcTextSize (CDC* pDC)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);

	if (m_pRibbonBar->IsScenicLook () && !m_strTextScenic.IsEmpty())
	{
		m_sizeTextRight = pDC->GetTextExtent (m_strTextScenic);
	}
	else
	{
		m_sizeTextRight = CSize(0, 0);
	}
}
//********************************************************************************
void CBCGPRibbonMainButton::DrawImage (CDC* pDC, RibbonImageType /*type*/, CRect rectImage)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);

	if (m_pRibbonBar->IsScenicLook () && !m_strTextScenic.IsEmpty())
	{
		UINT nDTFlags = DT_SINGLELINE | DT_CENTER | DT_VCENTER;

		CRect rectText = rectImage;
		rectText.OffsetRect(0, 1);

		CRect rectShadow = rectText;
		rectShadow.OffsetRect(1, 1);

		m_nTextGlassGlowSize = 0;

		BOOL bIsDrawOnGlass = CBCGPToolBarImages::m_bIsDrawOnGlass;
		CBCGPToolBarImages::m_bIsDrawOnGlass = bIsDrawOnGlass && m_pRibbonBar->AreTransparentTabs ();

		int nBkMode = pDC->SetBkMode (TRANSPARENT);

		if (globalData.IsHighContastMode())
		{
			DoDrawText(pDC, m_strTextScenic, rectText, nDTFlags, globalData.clrWindowText);
		}
		else
		{
			DoDrawText(pDC, m_strTextScenic, rectShadow, nDTFlags, RGB(100, 100, 100));
			DoDrawText(pDC, m_strTextScenic, rectText, nDTFlags, RGB(255, 255, 255));
		}

		pDC->SetBkMode (nBkMode);

		CBCGPToolBarImages::m_bIsDrawOnGlass = bIsDrawOnGlass;

		return;
	}

	CBCGPToolBarImages* pImage = &m_Image;
	CBCGPToolBarImages::ImageAlignHorz horz = CBCGPToolBarImages::ImageAlignHorzLeft;
	CBCGPToolBarImages::ImageAlignVert vert = CBCGPToolBarImages::ImageAlignVertTop;

	const double dScale = globalData.GetRibbonImageScale ();

	if (m_pRibbonBar->IsScenicLook ())
	{
		if (m_ImageScenic.IsValid ())
		{
			pImage = &m_ImageScenic;
		}

		horz = CBCGPToolBarImages::ImageAlignHorzCenter;
		vert = CBCGPToolBarImages::ImageAlignVertCenter;

		CSize sizeImage (pImage->GetImageSize ());
		CSize sizeIdeal ((int)(16 * dScale), (int)(16 * dScale));
		if (sizeImage.cx != sizeIdeal.cx)
		{
			sizeImage.cx = sizeIdeal.cx;
			horz = CBCGPToolBarImages::ImageAlignHorzStretch;
		}
		if (sizeImage.cy != sizeIdeal.cy)
		{
			sizeImage.cy = sizeIdeal.cy;
			vert = CBCGPToolBarImages::ImageAlignVertStretch;
		}

		rectImage.left += (rectImage.Width () - sizeImage.cx) / 2;
		rectImage.right = rectImage.left + sizeImage.cx;
		rectImage.top += (rectImage.Height () - sizeImage.cy) / 2;
		rectImage.bottom = rectImage.top + sizeImage.cy;

		CSize sizeArrow (CBCGPMenuImages::Size ());

		CRect rectArrow(CPoint (rectImage.right - sizeArrow.cx / 2 + xMargin,
			rectImage.top + (sizeImage.cy - sizeArrow.cy) / 2), 
			sizeArrow);

		CRect rectShadow = rectArrow;
		rectShadow.OffsetRect (0, 1);

		CBCGPMenuImages::IMAGES_IDS id = CBCGPMenuImages::IdArowDown;

		CBCGPMenuImages::Draw (pDC, id, rectShadow, CBCGPMenuImages::ImageBlack);
		CBCGPMenuImages::Draw (pDC, id, rectArrow, 
			m_bIsDisabled ? CBCGPMenuImages::ImageGray : CBCGPMenuImages::ImageWhite);

		rectImage.OffsetRect (-sizeArrow.cx / 2, 0);
	}
	else if (dScale != 1.)
	{
		const CSize sizeImage = m_Image.GetImageSize ();

		if (sizeImage.cx >= 32 && sizeImage.cy >= 32)
		{
			// The image is already scaled
			horz = CBCGPToolBarImages::ImageAlignHorzCenter;
			vert = CBCGPToolBarImages::ImageAlignVertCenter;
		}
		else
		{
			horz = CBCGPToolBarImages::ImageAlignHorzStretch;
			vert = CBCGPToolBarImages::ImageAlignVertStretch;
		}
	}

	pImage->SetTransparentColor (globalData.clrBtnFace);
	pImage->DrawEx (pDC, rectImage, 0, horz, vert);
}
//********************************************************************************
BOOL CBCGPRibbonMainButton::ShowMainMenu ()
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pRibbonBar);

	if (m_pRibbonBar->IsBackstageViewActive())
	{
		return FALSE;
	}

	CBCGPRibbonCategory* pCategory = NULL;

	if (m_pRibbonBar->GetBackstageCategory () != NULL && m_pRibbonBar->GetMainCategory () != NULL)
	{
		pCategory = m_pRibbonBar->m_bBackstageMode ? m_pRibbonBar->GetBackstageCategory () : m_pRibbonBar->GetMainCategory ();
	}
	else
	{
		pCategory = m_pRibbonBar->GetBackstageCategory () != NULL ? m_pRibbonBar->GetBackstageCategory () : m_pRibbonBar->GetMainCategory ();
	}

	if (pCategory == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (pCategory->GetPanelCount () == 0)
	{
		return FALSE;
	}

	if (pCategory == m_pRibbonBar->GetBackstageCategory () && m_pRibbonBar->m_bPrintPreviewMode)
	{
		return FALSE;
	}

	CBCGPBaseRibbonElement::OnShowPopupMenu ();

	const BOOL bIsRTL = (m_pRibbonBar->GetExStyle () & WS_EX_LAYOUTRTL);

	CRect rectBtn = m_rect;
	m_pRibbonBar->ClientToScreen (&rectBtn);

	CBCGPRibbonMainPanel* pPanel = DYNAMIC_DOWNCAST (CBCGPRibbonMainPanel, pCategory->GetPanel (0));
	ASSERT_VALID (pPanel);

	if (pPanel->IsBackstageView())
	{
		pPanel->m_nTopMargin = 6;
	}
	else if (!m_pRibbonBar->IsScenicLook ())
	{
		pPanel->m_nTopMargin = rectBtn.Height () / 2 - 2;
	}
	else
	{
		pPanel->m_nTopMargin = m_pRibbonBar->m_nTabsHeight - 1;
	}

	pPanel->m_nXMargin = CBCGPVisualManager::GetInstance ()->GetRibbonPanelMargin (pCategory);

	pPanel->m_pMainButton = this;

	CClientDC dc (m_pRibbonBar);

	CFont* pOldFont = dc.SelectObject (m_pRibbonBar->GetFont ());
	ASSERT (pOldFont != NULL);

	pPanel->RecalcWidths (&dc, 32767);

	dc.SelectObject (pOldFont);

	if (pPanel->IsKindOf(RUNTIME_CLASS(CBCGPRibbonBackstageViewPanel)))
	{
		CBCGPRibbonBackstageView* pView = new CBCGPRibbonBackstageView(m_pRibbonBar, pPanel);
		pView->Create();

		m_bIsDroppedDown = TRUE;
	}
	else
	{
		CBCGPRibbonPanelMenu* pMenu = new CBCGPRibbonPanelMenu (pPanel);
		pMenu->SetParentRibbonElement (this);

		const int y = m_pRibbonBar->IsScenicLook () ? rectBtn.bottom - m_pRibbonBar->m_nTabsHeight - 1 : rectBtn.CenterPoint ().y;

		pMenu->Create (m_pRibbonBar, bIsRTL ? rectBtn.right : rectBtn.left, y, (HMENU) NULL);

		SetDroppedDown (pMenu);
	}

	return TRUE;
}
//********************************************************************************
BOOL CBCGPRibbonMainButton::OnKey (BOOL bIsMenuKey)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pRibbonBar);

	if (m_pRibbonBar->m_nKeyboardNavLevel == 0)
	{
		m_pRibbonBar->RemoveAllKeys ();
		m_pRibbonBar->m_nCurrKeyChar = 0;
	}

	if (m_pRibbonBar->GetMainCategory () == NULL && m_pRibbonBar->GetBackstageCategory () == NULL)
	{
		return CBCGPRibbonButton::OnKey (bIsMenuKey);
	}

	ShowMainMenu ();

	if (m_pPopupMenu != NULL)
	{
		ASSERT_VALID (m_pPopupMenu);
		m_pPopupMenu->SendMessage (WM_KEYDOWN, VK_HOME);
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CBCGPRibbonBar

BOOL CBCGPRibbonBar::SaveRibbon (const CBCGPRibbonBar* rb, const CBCGPRibbonStatusBar* sb,
		const CString& strFolder, 
		const CString& strResourceName,
		DWORD dwFlags/* = 0x0F*/,
		const CString& strResourceFolder/* = _T("res\\BCGSoft_ribbon")*/, 
		const CString& strResourcePrefix/* = _T("IDR_BCGP")*/)
{
	if (rb == NULL && sb == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CBCGPRibbonInfo info;

	CBCGPRibbonCollector collector (info, dwFlags);
	if (rb != NULL)
	{
		collector.CollectRibbonBar (*rb);
	}
	if (sb != NULL)
	{
		collector.CollectStatusBar (*sb);
	}

	CBCGPRibbonInfoWriter writer (info);
	return writer.Save (strFolder, strResourceName, strResourceFolder, strResourcePrefix);
}

IMPLEMENT_DYNAMIC(CBCGPRibbonBar, CBCGPControlBar)

CBCGPRibbonBar::CBCGPRibbonBar(BOOL bReplaceFrameCaption) :
	m_bReplaceFrameCaption (bReplaceFrameCaption)
{
	m_bDlgBarMode = FALSE;
	m_dwHideFlags = 0;
	m_nCategoryHeight = 0;
	m_nCategoryMinWidth = 0;
	m_bRecalcCategoryHeight = TRUE;
	m_bRecalcCategoryWidth = TRUE;
	m_nTabsHeight = 0;
	m_hFont = NULL;
	m_pActiveCategory = NULL;
	m_pActiveCategorySaved = NULL;
	m_nHighlightedTab = -1;
	m_pMainButton = NULL;
	m_nInitialBackstagePage = -1;
	m_bAutoDestroyMainButton = FALSE;
	m_pMainCategory = NULL;
	m_pBackstageCategory = NULL;
	m_pPrintPreviewCategory = NULL;
	m_bIsPrintPreview = TRUE;
	m_sizeMainButton = CSize (0, 0);
	m_pHighlighted = NULL;
	m_pPressed = NULL;
	m_bTracked = FALSE;
	m_nTabTrancateRatio = 0;
	m_nBackstageViewTop = 0;
	m_pToolTip = NULL;
	m_bForceRedraw = FALSE;
	m_nSystemButtonsNum = 0;
	m_bMaximizeMode = FALSE;
	m_bAutoCommandTimer = FALSE;
	m_bPrintPreviewMode = FALSE;
	m_bBackstageViewActive = FALSE;
	m_bIsTransparentCaption = FALSE;
	m_bTransparentTabs = FALSE;
	m_bIsMaximized = FALSE;
	m_bToolTip = TRUE;
	m_bToolTipDescr = TRUE;
	m_nTooltipWidthRegular = nTooltipMinWidthDefault;
	m_nTooltipWidthLargeImage = nTooltipWithImageMinWidthDefault;
	m_bKeyTips = TRUE;
	m_bIsCustomizeMenu = FALSE;
	m_nKeyboardNavLevel = -1;
	m_pKeyboardNavLevelParent = NULL;
	m_pKeyboardNavLevelCurrent = NULL;
	m_nCurrKeyChar = 0;
	m_bDontSetKeyTips = FALSE;
	m_bHasMinimizeButton = FALSE;

	m_rectCaption.SetRectEmpty ();
	m_rectCaptionText.SetRectEmpty ();
	m_rectSysButtons.SetRectEmpty ();

	m_nCaptionHeight = 0;

	m_bQuickAccessToolbarOnTop = TRUE;

#if _MSC_VER >= 1300
		EnableActiveAccessibility ();
#endif

	m_bScenicLook = FALSE;
	m_bIsScenicSet = FALSE;
	m_bBackstageMode = FALSE;

	m_TabElements.m_bIsRibbonTabElements = TRUE;
}
//******************************************************************************************
CBCGPRibbonBar::~CBCGPRibbonBar()
{
	int i = 0;

	for (i = 0; i < m_arCategories.GetSize (); i++)
	{
		ASSERT_VALID (m_arCategories [i]);
		delete m_arCategories [i];
	}

	for (i = 0; i < m_arContextCaptions.GetSize (); i++)
	{
		ASSERT_VALID (m_arContextCaptions [i]);
		delete m_arContextCaptions [i];
	}

	if (m_pMainCategory != NULL)
	{
		ASSERT_VALID (m_pMainCategory);
		delete m_pMainCategory;
	}

	if (m_pBackstageCategory != NULL)
	{
		ASSERT_VALID (m_pBackstageCategory);
		delete m_pBackstageCategory;
	}

	if (m_bAutoDestroyMainButton && m_pMainButton != NULL)
	{
		ASSERT_VALID (m_pMainButton);
		delete m_pMainButton;
	}
}
//*****************************************************************************
void CBCGPRibbonBar::SetScenicLook (BOOL bScenicLook, BOOL bRecalc/* = TRUE*/)
{
	m_bIsScenicSet = TRUE;

	if (bScenicLook != m_bScenicLook)
	{
		m_bScenicLook = bScenicLook;

		if (GetSafeHwnd () != NULL && bRecalc)
		{
			ForceRecalcLayout ();
		}
	}
}
//******************************************************************************
BOOL CBCGPRibbonBar::IsScenicLook () const
{
	if (m_bIsScenicSet)
	{
		return m_bScenicLook;
	}

	return CBCGPVisualManager::GetInstance ()->IsRibbonScenicLook();
}

BEGIN_MESSAGE_MAP(CBCGPRibbonBar, CBCGPControlBar)
	//{{AFX_MSG_MAP(CBCGPRibbonBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CANCELMODE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DESTROY()
	ON_WM_SIZING()
	ON_WM_MOUSEWHEEL()
	ON_WM_SETTINGCHANGE()
	ON_WM_TIMER()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_ERASEBKGND()
	ON_WM_SYSCOMMAND()
	ON_WM_SETCURSOR()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_MESSAGE(WM_GETFONT, OnGetFont)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_REGISTERED_MESSAGE(BCGM_UPDATETOOLTIPS, OnBCGUpdateToolTips)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnNeedTipText)
	ON_REGISTERED_MESSAGE(BCGM_POSTRECALCLAYOUT, OnPostRecalcLayout)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCGPRibbonBar message handlers

//********************************************************************************
BOOL CBCGPRibbonBar::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
	return CreateEx(pParentWnd, 0, dwStyle, nID);
}
//********************************************************************************
BOOL CBCGPRibbonBar::CreateEx (CWnd* pParentWnd, DWORD /*dwCtrlStyle*/, DWORD dwStyle, 
							 UINT nID)
{
	ASSERT_VALID(pParentWnd);   // must have a parent

	m_dwStyle |= CBRS_HIDE_INPLACE;

	// save the style
	SetBarAlignment (dwStyle & CBRS_ALL);

	// create the HWND
	CRect rect;
	rect.SetRectEmpty();

	m_dwBCGStyle = 0; // can't float, resize, close, slide

	if (m_bReplaceFrameCaption && globalData.DwmIsCompositionEnabled ())
	{
		dwStyle |= WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZE;
	}

	if (!CWnd::Create (globalData.RegisterWindowClass (_T("BCGPRibbonBar")),
		NULL, dwStyle | WS_CLIPSIBLINGS, rect, pParentWnd, nID))
	{
		return FALSE;
	}

	if (pParentWnd->IsKindOf (RUNTIME_CLASS (CBCGPFrameWnd)))
	{
		((CBCGPFrameWnd*) pParentWnd)->AddControlBar (this);
	}
	else if (pParentWnd->IsKindOf (RUNTIME_CLASS (CBCGPMDIFrameWnd)))
	{
		((CBCGPMDIFrameWnd*) pParentWnd)->AddControlBar (this);
	}
	else if (pParentWnd->IsKindOf (RUNTIME_CLASS (CBCGPRibbonDialogBar)))
	{
		m_bDlgBarMode = TRUE;
	}
	else
	{
		ASSERT (FALSE);
		return FALSE;
	}

	if (!m_bDlgBarMode)
	{
		pParentWnd->SetMenu (NULL);
		
		if (m_bReplaceFrameCaption)
		{
			if (globalData.DwmIsCompositionEnabled ())
			{
				pParentWnd->SetWindowPos (NULL, -1, -1, -1, -1, 
						SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED);
			}
			else
			{
				pParentWnd->ModifyStyle (WS_CAPTION, 0);
			}
		}
	}

	return TRUE;
}
//******************************************************************************************
BOOL CBCGPRibbonBar::LoadFromXML (UINT uiXMLResID)
{
	ASSERT_VALID (this);
	return LoadFromXML (MAKEINTRESOURCE (uiXMLResID));
}
//******************************************************************************************
BOOL CBCGPRibbonBar::LoadFromXML (LPCTSTR lpszXMLResID)
{
	ASSERT_VALID (this);

	CBCGPRibbonInfo info;
	CBCGPRibbonInfoLoader loader (info, CBCGPRibbonInfo::e_UseRibbon);

	if (!loader.Load (lpszXMLResID))
	{
		TRACE0("Cannot load ribbon from buffer\n");
		return FALSE;
	}

	CBCGPRibbonConstructor constr (info);
	constr.ConstructRibbonBar (*this);

	return TRUE;
}
//******************************************************************************************
BOOL CBCGPRibbonBar::LoadFromVSRibbon(UINT uiXMLResID)
{
	ASSERT_VALID (this);
	return LoadFromVSRibbon(MAKEINTRESOURCE (uiXMLResID));
}
//******************************************************************************************
BOOL CBCGPRibbonBar::LoadFromVSRibbon(LPCTSTR lpszXMLResID)
{
	ASSERT_VALID (this);

	CBCGPRibbonInfo info;
	CBCGPRibbonInfoLoader loader (info, CBCGPRibbonInfo::e_UseRibbon);

#if _MSC_VER >= 1600
	if (!loader.Load (lpszXMLResID, MAKEINTRESOURCE(RT_RIBBON_XML)))
#else
	if (!loader.Load (lpszXMLResID, _T("RT_RIBBON_XML")))
#endif
	{
		TRACE0("Cannot load ribbon from buffer\n");
		return FALSE;
	}

	CBCGPRibbonConstructor constr (info);
	constr.ConstructRibbonBar (*this);

	return TRUE;
}
//******************************************************************************************
BOOL CBCGPRibbonBar::LoadFromBuffer (LPCTSTR lpszXMLBuffer)
{
	ASSERT_VALID (this);
	ASSERT (lpszXMLBuffer != NULL);

	CBCGPRibbonInfo info;
	CBCGPRibbonInfoLoader loader (info, CBCGPRibbonInfo::e_UseRibbon);

	if (!loader.LoadFromBuffer (lpszXMLBuffer))
	{
		TRACE0("Cannot load ribbon from buffer\n");
		return FALSE;
	}

	CBCGPRibbonConstructor constr (info);
	constr.ConstructRibbonBar (*this);

	return TRUE;
}
//******************************************************************************************
CSize CBCGPRibbonBar::CalcFixedLayout(BOOL, BOOL /*bHorz*/)
{
	ASSERT_VALID (this);

	CClientDC dc (this);

	CFont* pOldFont = dc.SelectObject (GetFont ());
	ASSERT (pOldFont != NULL);

	TEXTMETRIC tm;
	dc.GetTextMetrics (&tm);

	m_nCaptionHeight = 0;

	if (m_bReplaceFrameCaption)
	{ 
		m_nCaptionHeight = GetSystemMetrics (SM_CYCAPTION) + 1;

		if (globalData.DwmIsCompositionEnabled ())
		{
			m_nCaptionHeight += GetSystemMetrics (SM_CYSIZEFRAME);
		}
	}

	int cy = 0;

	CSize sizeMainButton(0, 0);
	if (!IsScenicLook ())
	{
		sizeMainButton = m_sizeMainButton;
	}

	double scale = globalData.GetRibbonImageScale ();
	if (scale > 1.)
	{
		sizeMainButton.cx = (int)(.5 + scale * sizeMainButton.cx);
		sizeMainButton.cy = (int)(.5 + scale * sizeMainButton.cy);
	}

	if (m_dwHideFlags & BCGPRIBBONBAR_HIDE_ALL)
	{
		cy = m_nCaptionHeight;
	}
	else
	{
		if (m_bRecalcCategoryHeight)
		{
			m_nCategoryHeight = 0;
		}

		m_nTabsHeight = tm.tmHeight + 2 * yTabMargin;

		if (m_bRecalcCategoryHeight)
		{
			for (int i = 0; i < m_arCategories.GetSize (); i++)
			{
				CBCGPRibbonCategory* pCategory = m_arCategories [i];
				ASSERT_VALID (pCategory);

				m_nCategoryHeight = max (m_nCategoryHeight, pCategory->GetMaxHeight (&dc));
			}

			m_bRecalcCategoryHeight = FALSE;
		}

		const CSize sizeAQToolbar = m_QAToolbar.GetRegularSize (&dc);

		if (IsQuickAccessToolbarOnTop ())
		{
			m_nCaptionHeight = max (m_nCaptionHeight, sizeAQToolbar.cy + (IsScenicLook() ? 0 : (2 * yMargin)));
		}

		const int nQuickAceesToolbarHeight = (IsQuickAccessToolbarOnTop () || m_bBackstageViewActive) ? 0 : sizeAQToolbar.cy;
		int nCategoryHeight = (m_dwHideFlags & BCGPRIBBONBAR_HIDE_ELEMENTS) ? 0 : m_nCategoryHeight;

		if (m_bBackstageViewActive)
		{
			nCategoryHeight = CBCGPVisualManager::GetInstance ()->GetRibbonBackstageTopLineHeight();
		}

		cy = nQuickAceesToolbarHeight + nCategoryHeight + 
			max (	m_nCaptionHeight + m_nTabsHeight, 
					sizeMainButton.cy + yMargin);
	}

	if (globalData.DwmIsCompositionEnabled ())
	{
		if (GetParent ()->IsZoomed () && m_bReplaceFrameCaption)
		{
			cy += ::GetSystemMetrics (SM_CYSIZEFRAME) - 2;
		}
	}

	dc.SelectObject (pOldFont);
	return CSize (32767, cy);
}
//******************************************************************************************
BOOL CBCGPRibbonBar::PreCreateWindow(CREATESTRUCT& cs) 
{
	m_dwStyle &= ~(CBRS_BORDER_ANY|CBRS_BORDER_3D);
	return CBCGPControlBar::PreCreateWindow(cs);
}
//******************************************************************************************
int CBCGPRibbonBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_CaptionButtons [0].SetID (SC_MINIMIZE);
	m_CaptionButtons [1].SetID (SC_MAXIMIZE);
	m_CaptionButtons [2].SetID (SC_CLOSE);

	for (int i = 0; i < RIBBON_CAPTION_BUTTONS; i++)
	{
		m_CaptionButtons [i].m_pRibbonBar = this;
	}

	CString strCaption;
	GetParent ()->GetWindowText (strCaption);

	SetWindowText (strCaption);

	CBCGPTooltipManager::CreateToolTip (m_pToolTip, this,
		BCGP_TOOLTIP_TYPE_RIBBON);

	if (m_pToolTip->GetSafeHwnd () != NULL)
	{
		CRect rectDummy (0, 0, 0, 0);

		m_pToolTip->SetMaxTipWidth (nTooltipMaxWidth);

		m_pToolTip->AddTool (this, LPSTR_TEXTCALLBACK, &rectDummy, idToolTipClient);
		m_pToolTip->AddTool (this, LPSTR_TEXTCALLBACK, &rectDummy, idToolTipCaption);
	}

	m_QAToolbar.m_pRibbonBar = this;

	return 0;
}
//******************************************************************************************
void CBCGPRibbonBar::OnDestroy() 
{
	CBCGPTooltipManager::DeleteToolTip (m_pToolTip);
	RemoveAllKeys ();

	CBCGPControlBar::OnDestroy();
}
//******************************************************************************************
void CBCGPRibbonBar::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPControlBar::OnSize(nType, cx, cy);

	BOOL bWasMaximized = m_bIsMaximized;
	m_bIsMaximized = GetParent ()->IsZoomed ();

	if (bWasMaximized != m_bIsMaximized)
	{
		m_bForceRedraw = TRUE;
		RecalcLayout ();
	}
	else
	{
		RecalcLayout ();
	}

	UpdateToolTipsRect ();
}
//******************************************************************************************
BOOL CBCGPRibbonBar::ShowBackstageView(int nPage)
{
	if (m_pMainButton == NULL || m_pBackstageCategory == NULL)
	{
		return FALSE;
	}

	ASSERT_VALID(m_pMainButton);

	m_nInitialBackstagePage = nPage;
	m_pMainButton->ShowMainMenu();

	m_nInitialBackstagePage = -1;
	return TRUE;
}
//******************************************************************************************
BOOL CBCGPRibbonBar::ShowBackstagePrintView()
{
	if (m_pMainButton == NULL || m_pBackstageCategory == NULL || !IsBackstageMode ())
	{
		return FALSE;
	}

	CBCGPRibbonCategory* pCategory = GetBackstageCategory ();
	if (pCategory == NULL || pCategory->GetPanelCount () == 0)
	{
		return FALSE;
	}

	CBCGPRibbonBackstageViewPanel* pPanel = DYNAMIC_DOWNCAST(CBCGPRibbonBackstageViewPanel, pCategory->GetPanel (0));
	if (pPanel == NULL)
	{
		return FALSE;
	}

	int nPage = pPanel->FindFormPageIndex(RUNTIME_CLASS(CBCGPRibbonBackstagePagePrint));
	if (nPage < 0)
	{
		return FALSE;
	}

	ASSERT_VALID(m_pMainButton);

	m_nInitialBackstagePage = nPage;
	m_pMainButton->ShowMainMenu();

	m_nInitialBackstagePage = -1;
	return TRUE;
}
//******************************************************************************************
void CBCGPRibbonBar::SetVisibilityInFrame (int cxFrame, int cyFrame)
{
	ASSERT_VALID (this);

	const BOOL bHideAll = cyFrame < nMinRibbonHeight || cxFrame < nMinRibbonWidth;
	const BOOL bIsHidden = (m_dwHideFlags & BCGPRIBBONBAR_HIDE_ALL) != 0;

	if (bHideAll == bIsHidden)
	{
		return;
	}

	if (bHideAll)
	{
		m_dwHideFlags |= BCGPRIBBONBAR_HIDE_ALL;
	}
	else
	{
		m_dwHideFlags &= ~BCGPRIBBONBAR_HIDE_ALL;
	}

	if (m_pMainButton != NULL && bHideAll)
	{
		ASSERT_VALID (m_pMainButton);
		m_pMainButton->SetRect (CRect (0, 0, 0, 0));
	}

	PostMessage (BCGM_POSTRECALCLAYOUT);
}
//******************************************************************************************
LRESULT CBCGPRibbonBar::OnPostRecalcLayout(WPARAM,LPARAM)
{
	GetParentFrame ()->RecalcLayout ();
	return 0;
}
//******************************************************************************************
void CBCGPRibbonBar::SetMainButton (CBCGPRibbonMainButton* pButton, CSize sizeButton)
{
	ASSERT_VALID (this);

	m_pMainButton = pButton;

	if (m_pMainButton != NULL)
	{
		ASSERT_VALID (m_pMainButton);

		m_pMainButton->m_pRibbonBar = this;
		m_sizeMainButton = sizeButton;
	}
	else
	{
		m_sizeMainButton = CSize (0, 0);
	}
}
//******************************************************************************************
CBCGPRibbonMainPanel* CBCGPRibbonBar::AddMainCategory (
		LPCTSTR		lpszName,
		UINT		uiSmallImagesResID,
		UINT		uiLargeImagesResID,
		CSize		sizeSmallImage,
		CSize		sizeLargeImage,
		CRuntimeClass*	pRTI)
{
	ASSERT_VALID (this);
	ASSERT (lpszName != NULL);

	if (m_pMainCategory != NULL)
	{
		ASSERT_VALID (m_pMainCategory);
		delete m_pMainCategory;
	}

	if (pRTI != NULL)
	{
		m_pMainCategory = DYNAMIC_DOWNCAST (CBCGPRibbonCategory, pRTI->CreateObject ());

		if (m_pMainCategory == NULL)
		{
			ASSERT (FALSE);
			return NULL;
		}

		m_pMainCategory->CommonInit (this, lpszName, uiSmallImagesResID, uiLargeImagesResID,
			sizeSmallImage, sizeLargeImage);
	}
	else
	{
		m_pMainCategory = new CBCGPRibbonCategory (
			this, lpszName, uiSmallImagesResID, uiLargeImagesResID,
			sizeSmallImage, sizeLargeImage);
	}

	return (CBCGPRibbonMainPanel*) m_pMainCategory->AddPanel (
		lpszName, 0, RUNTIME_CLASS (CBCGPRibbonMainPanel));
}
//******************************************************************************************
CBCGPRibbonBackstageViewPanel* CBCGPRibbonBar::AddBackstageCategory (
		LPCTSTR		lpszName,
		UINT		uiSmallImagesResID,
		CSize		sizeSmallImage,
		CRuntimeClass*	pRTI)
{
	ASSERT_VALID (this);
	ASSERT (lpszName != NULL);

	if (m_pBackstageCategory != NULL)
	{
		ASSERT_VALID (m_pBackstageCategory);
		delete m_pBackstageCategory;
	}

	if (pRTI != NULL)
	{
		m_pBackstageCategory = DYNAMIC_DOWNCAST (CBCGPRibbonCategory, pRTI->CreateObject ());

		if (m_pBackstageCategory == NULL)
		{
			ASSERT (FALSE);
			return NULL;
		}

		m_pBackstageCategory->CommonInit (this, lpszName, uiSmallImagesResID, 0,
			sizeSmallImage, CSize(0, 0));
	}
	else
	{
		m_pBackstageCategory = new CBCGPRibbonCategory (
			this, lpszName, uiSmallImagesResID, 0,
			sizeSmallImage, CSize(0, 0));
	}

	return (CBCGPRibbonBackstageViewPanel*) m_pBackstageCategory->AddPanel (
		lpszName, 0, RUNTIME_CLASS (CBCGPRibbonBackstageViewPanel));
}
//******************************************************************************************
CBCGPRibbonCategory* CBCGPRibbonBar::AddCategory (
		LPCTSTR			lpszName,
		UINT			uiSmallImagesResID,
		UINT			uiLargeImagesResID,
		CSize			sizeSmallImage,
		CSize			sizeLargeImage,
		int				nInsertAt,
		CRuntimeClass*	pRTI)
{
	ASSERT_VALID (this);
	ASSERT (lpszName != NULL);

	CBCGPRibbonCategory* pCategory = NULL;

	if (pRTI != NULL)
	{
		pCategory = DYNAMIC_DOWNCAST (CBCGPRibbonCategory, pRTI->CreateObject ());

		if (pCategory == NULL)
		{
			ASSERT (FALSE);
			return NULL;
		}

		pCategory->CommonInit (this, lpszName, uiSmallImagesResID, uiLargeImagesResID,
			sizeSmallImage, sizeLargeImage);
	}
	else
	{
		pCategory = new CBCGPRibbonCategory (
			this, lpszName, uiSmallImagesResID, uiLargeImagesResID,
			sizeSmallImage, sizeLargeImage);
	}

	if (nInsertAt < 0)
	{
		m_arCategories.Add (pCategory);
	}
	else
	{
		m_arCategories.InsertAt (nInsertAt, pCategory);
	}

	if (m_pActiveCategory == NULL)
	{
		m_pActiveCategory = pCategory;
		m_pActiveCategory->m_bIsActive = TRUE;
	}

	m_bRecalcCategoryHeight = TRUE;
	m_bRecalcCategoryWidth = TRUE;

	return pCategory;
}
//******************************************************************************************
CBCGPRibbonCategory* CBCGPRibbonBar::AddContextCategory (
		LPCTSTR					lpszName,
		LPCTSTR					lpszContextName,
		UINT					uiContextID,
		BCGPRibbonCategoryColor	clrContext,
		UINT					uiSmallImagesResID,
		UINT					uiLargeImagesResID,
		CSize					sizeSmallImage,
		CSize					sizeLargeImage,
		CRuntimeClass*			pRTI)
{
	ASSERT_VALID (this);
	ASSERT (lpszContextName != NULL);
	ASSERT (uiContextID != 0);

	CBCGPRibbonCategory* pCategory = AddCategory (
		lpszName, uiSmallImagesResID, uiLargeImagesResID, 
		sizeSmallImage, sizeLargeImage, -1, pRTI);

	if (pCategory == NULL)
	{
		return NULL;
	}

	ASSERT_VALID (pCategory);

	pCategory->m_bIsVisible = FALSE;

	CBCGPRibbonContextCaption* pCaption = NULL;

	for (int i = 0; i < m_arContextCaptions.GetSize (); i++)
	{
		ASSERT_VALID (m_arContextCaptions [i]);

		if (m_arContextCaptions [i]->m_uiID == uiContextID)
		{
			pCaption = m_arContextCaptions [i];
			pCaption->m_strText = lpszContextName;
			pCaption->m_Color = clrContext;
			break;
		}
	}

	if (pCaption == NULL)
	{
		pCaption = new CBCGPRibbonContextCaption (lpszContextName, uiContextID, clrContext);
		pCaption->m_pRibbonBar = this;

		m_arContextCaptions.Add (pCaption);
	}

	pCategory->SetTabColor (clrContext);
	pCategory->m_uiContextID = uiContextID;

	return pCategory;
}
//******************************************************************************************
CBCGPRibbonCategory* CBCGPRibbonBar::AddQATOnlyCategory (
		LPCTSTR					lpszName,
		UINT					uiSmallImagesResID,
		CSize					sizeSmallImage)
{
	ASSERT_VALID (this);

	CBCGPRibbonCategory* pCategory = AddCategory (
		lpszName, uiSmallImagesResID, 0, sizeSmallImage);

	if (pCategory == NULL)
	{
		return NULL;
	}

	ASSERT_VALID (pCategory);

	pCategory->m_bIsVisible = FALSE;

	return pCategory;
}
//******************************************************************************************
BOOL CBCGPRibbonBar::SetActiveCategory (CBCGPRibbonCategory* pActiveCategory, BOOL bForceRestore)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pActiveCategory);

	if (!pActiveCategory->IsVisible ())
	{
		ASSERT (FALSE);
		return FALSE;
	}

	if (pActiveCategory == m_pMainCategory || pActiveCategory == m_pBackstageCategory)
	{
		//-------------------------------
		// Main category cannot be active
		//-------------------------------
		ASSERT (FALSE);
		return FALSE;
	}

	if (m_pActiveCategory != NULL && pActiveCategory != m_pActiveCategory)
	{
		ASSERT_VALID (m_pActiveCategory);
		m_pActiveCategory->SetActive (FALSE);
	}

	for (int i = 0; i < m_arCategories.GetSize (); i++)
	{
		CBCGPRibbonCategory* pCategory = m_arCategories [i];
		ASSERT_VALID (pCategory);

		if (pCategory == pActiveCategory)
		{
			if (m_dwHideFlags == BCGPRIBBONBAR_HIDE_ELEMENTS && !bForceRestore)
			{
				m_pActiveCategory = pCategory;
				m_pActiveCategory->m_bIsActive = TRUE;

				CRect rectCategory;
				GetClientRect (rectCategory);

				rectCategory.top = pCategory->GetTabRect ().bottom;
				rectCategory.bottom = rectCategory.top + m_nCategoryHeight;

				ClientToScreen (&rectCategory);

				if (m_nKeyboardNavLevel < 0)
				{
					pCategory->NormalizeFloatingRect (this, rectCategory);
				}

				pCategory->ShowFloating (rectCategory);

				RedrawWindow (pCategory->GetTabRect ());

				//---------------------
				// Notify parent frame:
				//---------------------
				CFrameWnd* pParentFrame = GetParentFrame ();
				ASSERT_VALID (pParentFrame);
				
				pParentFrame->SendMessage (BCGM_ON_CHANGE_RIBBON_CATEGORY, 0, (LPARAM)this);
				return TRUE;
			}

			if (pCategory->IsActive ())
			{
				if (m_dwHideFlags == BCGPRIBBONBAR_HIDE_ELEMENTS && bForceRestore)
				{
					pCategory->ShowElements ();
					RedrawWindow ();
				}

				return TRUE;
			}

			m_pActiveCategory = pCategory;
			m_pActiveCategory->SetActive ();

			if (GetSafeHwnd () != NULL)
			{
				if (m_dwHideFlags == BCGPRIBBONBAR_HIDE_ELEMENTS && bForceRestore)
				{
					pCategory->ShowElements ();
				}

				m_bForceRedraw = TRUE;
				RecalcLayout ();
			}

			//---------------------
			// Notify parent frame:
			//---------------------
			CFrameWnd* pParentFrame = GetParentFrame ();
			ASSERT_VALID (pParentFrame);
			
			pParentFrame->SendMessage (BCGM_ON_CHANGE_RIBBON_CATEGORY, 0, (LPARAM)this);
			return TRUE;
		}
	}

	ASSERT (FALSE);
	return FALSE;
}
//******************************************************************************************
int CBCGPRibbonBar::FindCategoryIndexByData (DWORD dwData) const
{
	ASSERT_VALID (this);

	for (int i = 0; i < m_arCategories.GetSize (); i++)
	{
		CBCGPRibbonCategory* pCategory = m_arCategories [i];
		ASSERT_VALID (pCategory);

		if (pCategory->GetData () == dwData)
		{
			return i;
		}
	}

	return -1;
}
//******************************************************************************************
int CBCGPRibbonBar::GetCategoryCount () const
{
	ASSERT_VALID (this);
	return (int) m_arCategories.GetSize ();
}
//******************************************************************************************
int CBCGPRibbonBar::GetVisibleCategoryCount () const
{
	ASSERT_VALID (this);

	int nCount = 0;

	for (int i = 0; i < m_arCategories.GetSize (); i++)
	{
		CBCGPRibbonCategory* pCategory = m_arCategories [i];
		ASSERT_VALID (pCategory);

		if (pCategory->IsVisible ())
		{
			nCount++;
		}
	}

	return nCount;
}
//******************************************************************************************
CBCGPRibbonCategory* CBCGPRibbonBar::GetCategory (int nIndex) const
{
	ASSERT_VALID (this);

	if (nIndex < 0 || nIndex >= m_arCategories.GetSize ())
	{
		ASSERT (FALSE);
		return NULL;
	}

	return m_arCategories [nIndex];
}
//******************************************************************************************
int CBCGPRibbonBar::GetCategoryIndex (CBCGPRibbonCategory* pCategory) const
{
	ASSERT_VALID (this);
	ASSERT_VALID (pCategory);

	for (int i = 0; i < m_arCategories.GetSize (); i++)
	{
		if (m_arCategories [i] == pCategory)
		{
			return i;
		}
	}

	return -1;
}
//******************************************************************************************
void CBCGPRibbonBar::ShowCategory (int nIndex, BOOL bShow/* = TRUE*/)
{
	ASSERT_VALID (this);

	if (nIndex < 0 || nIndex >= m_arCategories.GetSize ())
	{
		ASSERT (FALSE);
		return;
	}

	CBCGPRibbonCategory* pCategory = m_arCategories [nIndex];
	ASSERT_VALID (pCategory);

	pCategory->m_bIsVisible = bShow;
}
//******************************************************************************************
void CBCGPRibbonBar::ShowContextCategories (UINT uiContextID, BOOL bShow/* = TRUE*/)
{
	ASSERT_VALID (this);

	if (uiContextID == 0)
	{
		ASSERT (FALSE);
		return;
	}

	BOOL bChangeActiveCategory = FALSE;
	int i = 0;

	for (i = 0; i < m_arCategories.GetSize (); i++)
	{
		CBCGPRibbonCategory* pCategory = m_arCategories [i];
		ASSERT_VALID (pCategory);

		if (pCategory->GetContextID () == uiContextID)
		{
			pCategory->m_bIsVisible = bShow;

			if (!bShow && pCategory == m_pActiveCategory)
			{
				bChangeActiveCategory = TRUE;
			}
		}
	}

	if (bChangeActiveCategory)
	{
		for (i = 0; i < m_arCategories.GetSize (); i++)
		{
			CBCGPRibbonCategory* pCategory = m_arCategories [i];
			ASSERT_VALID (pCategory);

			if (pCategory->IsVisible ())
			{
				SetActiveCategory (pCategory);
				return;
			}
		}

		m_pActiveCategory = NULL;
	}
}
//******************************************************************************************
BOOL CBCGPRibbonBar::ActivateContextCategory (UINT uiContextID)
{
	ASSERT_VALID (this);

	if (uiContextID == 0)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	for (int i = 0; i < m_arCategories.GetSize (); i++)
	{
		CBCGPRibbonCategory* pCategory = m_arCategories [i];
		ASSERT_VALID (pCategory);

		if (pCategory->GetContextID () == uiContextID &&
			pCategory->m_bIsVisible)
		{
			SetActiveCategory (pCategory);
			return TRUE;
		}
	}

	return FALSE;
}
//******************************************************************************************
BOOL CBCGPRibbonBar::HideAllContextCategories ()
{
	ASSERT_VALID (this);
	BOOL bRes = FALSE;

	BOOL bChangeActiveCategory = FALSE;
	int i = 0;

	for (i = 0; i < m_arCategories.GetSize (); i++)
	{
		CBCGPRibbonCategory* pCategory = m_arCategories [i];
		ASSERT_VALID (pCategory);

		if (pCategory->GetContextID () != 0)
		{
			if (pCategory->m_bIsVisible)
			{
				bRes = TRUE;
			}

			pCategory->m_bIsVisible = FALSE;
			pCategory->m_bIsActive = FALSE;

			if (pCategory == m_pActiveCategory)
			{
				bChangeActiveCategory = TRUE;

				for (int j = 0; j < pCategory->GetPanelCount (); j++)
				{
					CBCGPRibbonPanel* pPanel = pCategory->GetPanel (j);
					ASSERT_VALID (pPanel);

					pPanel->OnShow (FALSE);
				}
			}
		}
	}

	if (bChangeActiveCategory)
	{
		for (i = 0; i < m_arCategories.GetSize (); i++)
		{
			CBCGPRibbonCategory* pCategory = m_arCategories [i];
			ASSERT_VALID (pCategory);

			if (pCategory->IsVisible ())
			{
				if (m_dwHideFlags & BCGPRIBBONBAR_HIDE_ELEMENTS)
				{
					m_pActiveCategory = pCategory;
					m_pActiveCategory->m_bIsActive = TRUE;
				}
				else
				{
					SetActiveCategory (pCategory);
				}

				return bRes;
			}
		}

		m_pActiveCategory = NULL;
	}

	return bRes;
}
//******************************************************************************************
BOOL CBCGPRibbonBar::GetContextName (UINT uiContextID, CString& strName) const
{
	ASSERT_VALID (this);

	if (uiContextID == 0)
	{
		return FALSE;
	}

	CBCGPRibbonContextCaption* pCaption = FindContextCaption (uiContextID);
	if (pCaption == NULL)
	{
		return FALSE;
	}

	ASSERT_VALID (pCaption);
	strName = pCaption->m_strText;

	return TRUE;
}
//******************************************************************************************
BOOL CBCGPRibbonBar::RemoveCategory (int nIndex)
{
	ASSERT_VALID (this);

	if (nIndex < 0 || nIndex >= m_arCategories.GetSize ())
	{
		return FALSE;
	}

	OnCancelMode ();

	CBCGPRibbonCategory* pCategory = m_arCategories [nIndex];
	ASSERT_VALID (pCategory);

	const BOOL bChangeActiveCategory = (pCategory == m_pActiveCategory);

	delete pCategory;

	m_arCategories.RemoveAt (nIndex);

	if (bChangeActiveCategory)
	{
		if (m_arCategories.GetSize () == 0)
		{
			m_pActiveCategory = NULL;
		}
		else
		{
			nIndex = min (nIndex, (int) m_arCategories.GetSize () - 1);

			m_pActiveCategory = m_arCategories [nIndex];
			ASSERT_VALID (m_pActiveCategory);

			if (!m_pActiveCategory->IsVisible ())
			{
				m_pActiveCategory = NULL;

				for (int i = 0; i < m_arCategories.GetSize (); i++)
				{
					CBCGPRibbonCategory* pCategory = m_arCategories [i];
					ASSERT_VALID (pCategory);

					if (pCategory->IsVisible ())
					{
						m_pActiveCategory = pCategory;
						m_pActiveCategory->m_bIsActive = TRUE;
						break;
					}
				}
			}
			else
			{
				m_pActiveCategory->m_bIsActive = TRUE;
			}
		}
	}

	return TRUE;
}
//******************************************************************************************
void CBCGPRibbonBar::RemoveAllCategories ()
{
	OnCancelMode ();

	int i = 0;

	for (i = 0; i < m_arCategories.GetSize (); i++)
	{
		ASSERT_VALID (m_arCategories [i]);
		delete m_arCategories [i];
	}

	for (i = 0; i < m_arContextCaptions.GetSize (); i++)
	{
		ASSERT_VALID (m_arContextCaptions [i]);
		delete m_arContextCaptions [i];
	}

	m_arCategories.RemoveAll ();
	m_arContextCaptions.RemoveAll ();

	m_pActiveCategory = NULL;
}
//******************************************************************************************
LRESULT CBCGPRibbonBar::OnSetFont (WPARAM wParam, LPARAM /*lParam*/)
{
	m_hFont = (HFONT) wParam;
	ForceRecalcLayout ();
	return 0;
}
//******************************************************************************************
LRESULT CBCGPRibbonBar::OnGetFont (WPARAM, LPARAM)
{
	return (LRESULT) (m_hFont != NULL ? m_hFont : (HFONT) globalData.fontRegular);
}
//******************************************************************************************
void CBCGPRibbonBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	int i = 0;

	CBCGPMemDC memDC (dc, this);
	CDC* pDC = &memDC.GetDC ();

	CRect rectClip;
	dc.GetClipBox (rectClip);

	CRgn rgnClip;

	if (!rectClip.IsRectEmpty ())
	{
		rgnClip.CreateRectRgnIndirect (rectClip);
		pDC->SelectClipRgn (&rgnClip);
	}
	
	pDC->SetBkMode (TRANSPARENT);
	
	CRect rectClient;
	GetClientRect (rectClient);

	OnFillBackground (pDC, rectClient);

	CFont* pOldFont = pDC->SelectObject (GetFont ());
	ASSERT (pOldFont != NULL);

	CRect rectTabs = rectClient;
	rectTabs.top = m_rectCaption.IsRectEmpty () ? rectClient.top : m_rectCaption.bottom;
	rectTabs.bottom = rectTabs.top + m_nTabsHeight;

	//------------------
	// Draw caption bar:
	//------------------
	if (!m_rectCaption.IsRectEmpty ())
	{
		if (m_bIsTransparentCaption)
		{
			CRect rectFill = m_rectCaption;

			if (m_bTransparentTabs)
			{
				rectFill.bottom += m_nTabsHeight + 1;
			}

			rectFill.top = 0;

			pDC->FillSolidRect (rectFill, RGB (0, 0, 0));

			CBCGPToolBarImages::m_bIsDrawOnGlass = TRUE;
		}

		CBCGPVisualManager::GetInstance ()->OnDrawRibbonCaption
			(pDC, this, m_rectCaption, m_rectCaptionText);

		CBCGPVisualManager::GetInstance ()->OnPreDrawRibbon (pDC, this, rectTabs);

		for (i = 0; i < RIBBON_CAPTION_BUTTONS; i++)
		{
			if (!m_CaptionButtons [i].GetRect ().IsRectEmpty ())
			{
				m_CaptionButtons [i].OnDraw (pDC);
			}
		}

		for (i = 0; i < m_arContextCaptions.GetSize (); i++)
		{
			ASSERT_VALID (m_arContextCaptions [i]);
			m_arContextCaptions [i]->OnDraw (pDC);
		}

		CBCGPToolBarImages::m_bIsDrawOnGlass = FALSE;
	}

	if (m_bIsTransparentCaption && m_bQuickAccessToolbarOnTop)
	{
		CBCGPToolBarImages::m_bIsDrawOnGlass = TRUE;
	}

	//---------------------------
	// Draw quick access toolbar:
	//---------------------------
	COLORREF cltTextOld = (COLORREF)-1;
	COLORREF cltQATText = CBCGPVisualManager::GetInstance ()->GetRibbonQATTextColor ();
	
	if (cltQATText != (COLORREF)-1)
	{
		cltTextOld = pDC->SetTextColor (cltQATText);
	}

	m_QAToolbar.OnDraw (pDC);

	if (cltTextOld != (COLORREF)-1)
	{
		pDC->SetTextColor (cltTextOld);
	}

	CBCGPToolBarImages::m_bIsDrawOnGlass = FALSE;

	//---------------------
	// Draw active category:
	//---------------------
	if (m_pActiveCategory != NULL && m_dwHideFlags == 0)
	{
		ASSERT_VALID (m_pActiveCategory);
		m_pActiveCategory->OnDraw (pDC);
	}

	//-----------
	// Draw tabs:
	//-----------
	if (AreTransparentTabs ())
	{
		CBCGPToolBarImages::m_bIsDrawOnGlass = TRUE;
	}

	COLORREF clrTextTabs = CBCGPVisualManager::GetInstance ()->OnDrawRibbonTabsFrame
		(pDC, this, rectTabs);

	for (i = 0; i < m_arCategories.GetSize (); i++)
	{
		CBCGPRibbonCategory* pCategory = m_arCategories [i];
		ASSERT_VALID (pCategory);

		if (pCategory->IsVisible ())
		{
			pCategory->m_Tab.OnDraw (pDC);
		}
	}

	if (m_bBackstageViewActive && IsScenicLook())
	{
		CRect rectBackstageLine = rectTabs;
		rectBackstageLine.top = rectTabs.bottom;
		rectBackstageLine.bottom += CBCGPVisualManager::GetInstance ()->GetRibbonBackstageTopLineHeight();

		CBCGPVisualManager::GetInstance ()->OnDrawRibbonBackstageTopLine(pDC, rectBackstageLine);
	}

	//--------------------------------
	// Draw elements on right of tabs:
	//--------------------------------
	COLORREF clrTextOld = (COLORREF)-1;
	if (clrTextTabs != (COLORREF)-1)
	{
		clrTextOld = pDC->SetTextColor (clrTextTabs);
	}

	m_TabElements.OnDraw (pDC);

	if (clrTextOld != (COLORREF)-1)
	{
		pDC->SetTextColor (clrTextOld);
	}

	//------------------
	// Draw main button:
	//------------------
	if (m_pMainButton != NULL)
	{
		ASSERT_VALID (m_pMainButton);

		if (!m_pMainButton->GetRect ().IsRectEmpty ())
		{
			CBCGPVisualManager::GetInstance ()->OnDrawRibbonMainButton
				(pDC, m_pMainButton);

			if (m_bIsTransparentCaption)
			{
				CBCGPToolBarImages::m_bIsDrawOnGlass = TRUE;
			}

			m_pMainButton->OnDraw (pDC);
		}
	}

	CBCGPToolBarImages::m_bIsDrawOnGlass = FALSE;

	pDC->SelectObject (pOldFont);
	pDC->SelectClipRgn (NULL);
}
//******************************************************************************************
void CBCGPRibbonBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CBCGPControlBar::OnLButtonDown(nFlags, point);

	DeactivateKeyboardFocus ();

	CBCGPBaseRibbonElement* pDroppedDown = GetDroppedDown ();
	if (pDroppedDown != NULL)
	{
		ASSERT_VALID (pDroppedDown);
		pDroppedDown->ClosePopupMenu ();
	}

	if ((m_dwHideFlags & BCGPRIBBONBAR_HIDE_ALL) == BCGPRIBBONBAR_HIDE_ALL ||
		IsScenicLook ())
	{
		CRect rectIcon = m_rectCaption;
		rectIcon.right = rectIcon.left + rectIcon.Height ();

		if (rectIcon.PtInRect (point))
		{
			CPoint ptMenu (m_rectCaption.left, m_rectCaption.bottom);
			ClientToScreen (&ptMenu);

			ShowSysMenu (ptMenu);
			return;
		}
	}

	OnMouseMove (nFlags, point);

	CBCGPBaseRibbonElement* pHit = HitTest (point);

	if (pHit != NULL)
	{
		ASSERT_VALID (pHit);

		pHit->OnLButtonDown (point);
		pHit->m_bIsPressed = TRUE;

		CRect rectHit = pHit->GetRect ();
		rectHit.InflateRect (1, 1);

		RedrawWindow (rectHit);

		m_pPressed = pHit;
	}
	else
	{
		CRect rectCaption = m_rectCaption;

		if (CBCGPVisualManager::GetInstance()->DoesRibbonExtendCaptionToTabsArea())
		{
			rectCaption.bottom += m_nTabsHeight;
		}

		if (rectCaption.PtInRect (point))
		{
			if (CBCGPPopupMenu::GetActiveMenu () != NULL)
			{
				CBCGPPopupMenu::GetActiveMenu ()->SendMessage (WM_CLOSE);
			}

			if (!m_rectSysButtons.PtInRect (point))
			{
				CPoint ptScreen = point;
				ClientToScreen(&ptScreen);

				GetParent ()->SendMessage (WM_NCLBUTTONDOWN, 
					(WPARAM) HTCAPTION, MAKELPARAM (ptScreen.x, ptScreen.y));
			}
			return;
		}

		if (m_pActiveCategory != NULL && (m_dwHideFlags & BCGPRIBBONBAR_HIDE_ELEMENTS) == 0)
		{
			ASSERT_VALID (m_pActiveCategory);
			m_pPressed = m_pActiveCategory->OnLButtonDown (point);
		}
	}

	if (m_pPressed != NULL)
	{
		ASSERT_VALID (m_pPressed);

		int nDelay = 100;

		if (m_pPressed->IsAutoRepeatMode (nDelay))
		{
			SetTimer (IdAutoCommand, nDelay, NULL);
			m_bAutoCommandTimer = TRUE;
		}
	}
}
//******************************************************************************************
void CBCGPRibbonBar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CBCGPControlBar::OnLButtonUp(nFlags, point);

	if (m_bAutoCommandTimer)
	{
		KillTimer (IdAutoCommand);
		m_bAutoCommandTimer = FALSE;
	}

	CBCGPBaseRibbonElement* pHit = HitTest (point);

	HWND hwndThis = GetSafeHwnd ();

	if (pHit != NULL)
	{
		ASSERT_VALID (pHit);

		pHit->OnLButtonUp (point);

		if (!::IsWindow (hwndThis))
		{
			return;
		}

		pHit->m_bIsPressed = FALSE;

		RedrawWindow (pHit->GetRect ());
	}

	if (m_pActiveCategory != NULL)
	{
		ASSERT_VALID (m_pActiveCategory);
		m_pActiveCategory->OnLButtonUp (point);

		if (!::IsWindow (hwndThis))
		{
			return;
		}
	}

	const BOOL bIsPressedButon = m_pPressed != NULL;

	if (m_pPressed != NULL)
	{
		ASSERT_VALID (m_pPressed);
		
		CRect rect = m_pPressed->GetRect ();

		m_pPressed->m_bIsPressed = FALSE;
		m_pPressed = NULL;

		RedrawWindow (rect);
	}

	if (bIsPressedButon)
	{
		::GetCursorPos (&point);
		ScreenToClient (&point);

		OnMouseMove (nFlags, point);
	}
}
//******************************************************************************
void CBCGPRibbonBar::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CBCGPControlBar::OnLButtonDblClk(nFlags, point);

	CBCGPBaseRibbonElement* pHit = HitTest (point);
	if (pHit != NULL)
	{
		ASSERT_VALID (pHit);

		if (!pHit->IsKindOf (RUNTIME_CLASS (CBCGPRibbonContextCaption)))
		{
			pHit->OnLButtonDblClk (point);
			return;
		}
	}

	CRect rectCaption = m_rectCaption;

	if (CBCGPVisualManager::GetInstance()->DoesRibbonExtendCaptionToTabsArea())
	{
		rectCaption.bottom += m_nTabsHeight;
	}

	if (rectCaption.PtInRect (point) && !m_rectSysButtons.PtInRect (point))
	{
		BOOL bSysMenu = FALSE;

		if ((m_dwHideFlags & BCGPRIBBONBAR_HIDE_ALL) == BCGPRIBBONBAR_HIDE_ALL ||
			IsScenicLook ())
		{
			CRect rectIcon = m_rectCaption;
			rectIcon.right = rectIcon.left + rectIcon.Height ();

			bSysMenu = rectIcon.PtInRect (point);
		}

		GetParent ()->SendMessage (WM_NCLBUTTONDBLCLK,
			(WPARAM) (bSysMenu ? HTSYSMENU : HTCAPTION), MAKELPARAM (point.x, point.y));

		return;
	}
}
//******************************************************************************************
void CBCGPRibbonBar::OnMouseMove(UINT nFlags, CPoint point) 
{
	CBCGPControlBar::OnMouseMove(nFlags, point);

	CBCGPBaseRibbonElement* pHit = HitTest (point);

	if (point == CPoint (-1, -1))
	{
		m_bTracked = FALSE;
	}
	else if (!m_bTracked)
	{
		m_bTracked = TRUE;
		
		TRACKMOUSEEVENT trackmouseevent;
		trackmouseevent.cbSize = sizeof(trackmouseevent);
		trackmouseevent.dwFlags = TME_LEAVE;
		trackmouseevent.hwndTrack = GetSafeHwnd();
		trackmouseevent.dwHoverTime = HOVER_DEFAULT;
		::BCGPTrackMouse (&trackmouseevent);

		if (m_pPressed != NULL && ((nFlags & MK_LBUTTON) == 0))
		{
			ASSERT_VALID (m_pPressed);
			m_pPressed->m_bIsPressed = FALSE;
		}
	}
		
	if (pHit != m_pHighlighted)
	{
		PopTooltip ();

		if (m_pHighlighted != NULL)
		{
			ASSERT_VALID (m_pHighlighted);
			m_pHighlighted->m_bIsHighlighted = FALSE;
			m_pHighlighted->OnHighlight (FALSE);

			InvalidateRect (m_pHighlighted->GetRect ());

			m_pHighlighted = NULL;
		}

		if (pHit != NULL)
		{
			ASSERT_VALID (pHit);

			if ((nFlags & MK_LBUTTON) == 0 || pHit->IsPressed ())
			{
				m_pHighlighted = pHit;
				m_pHighlighted->OnHighlight (TRUE);
				m_pHighlighted->m_bIsHighlighted = TRUE;
				InvalidateRect (m_pHighlighted->GetRect ());
				m_pHighlighted->OnMouseMove (point);
			}
		}

		UpdateWindow ();
	}
	else if (m_pHighlighted != NULL)
	{
		ASSERT_VALID (m_pHighlighted);

		if (!m_pHighlighted->m_bIsHighlighted)
		{
			m_pHighlighted->m_bIsHighlighted = TRUE;
			RedrawWindow (m_pHighlighted->GetRect ());
		}

		m_pHighlighted->OnMouseMove (point);
	}

	if (m_pActiveCategory != NULL)
	{
		ASSERT_VALID (m_pActiveCategory);
		m_pActiveCategory->OnMouseMove (point);
	}
}
//*****************************************************************************************
BOOL CBCGPRibbonBar::OnMouseWheel(UINT /*nFlags*/, short zDelta, CPoint /*pt*/)
{
	if (CBCGPPopupMenu::GetActiveMenu () != NULL ||
		m_pActiveCategory == NULL ||
		(m_dwHideFlags & BCGPRIBBONBAR_HIDE_ELEMENTS) || m_bBackstageViewActive)
	{
		return FALSE;
	}

	if (m_nKeyboardNavLevel >= 0)
	{
		return FALSE;
	}

	if (GetFocus ()->GetSafeHwnd () != NULL && IsChild (GetFocus ()))
	{
		return FALSE;
	}

	CPoint point;
	::GetCursorPos (&point);

	ScreenToClient (&point);

	CRect rectClient;
	GetClientRect (rectClient);

	if (!rectClient.PtInRect (point))
	{
		return FALSE;
	}

	ASSERT_VALID (m_pActiveCategory);

	int nActiveCategoryIndex = -1;

	for (int i = 0; i < m_arCategories.GetSize (); i++)
	{
		CBCGPRibbonCategory* pCategory = m_arCategories [i];
		ASSERT_VALID (pCategory);

		if (pCategory == m_pActiveCategory)
		{
			nActiveCategoryIndex = i;
			break;
		}
	}

	if (nActiveCategoryIndex == -1)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	const int nSteps = -zDelta / WHEEL_DELTA;
	
	nActiveCategoryIndex = nActiveCategoryIndex + nSteps;
	
	if (nActiveCategoryIndex < 0)
	{
		nActiveCategoryIndex = 0;
	}

	if (nActiveCategoryIndex >= m_arCategories.GetSize ())
	{
		nActiveCategoryIndex = (int) m_arCategories.GetSize () - 1;
	}

	CBCGPRibbonCategory* pNewActiveCategory = m_arCategories [nActiveCategoryIndex];
	ASSERT_VALID (pNewActiveCategory);

	if (!pNewActiveCategory->IsVisible ())
	{
		if (nSteps < 0)
		{
			nActiveCategoryIndex--;

			while (nActiveCategoryIndex >= 0)
			{
				pNewActiveCategory = m_arCategories [nActiveCategoryIndex];
				ASSERT_VALID (pNewActiveCategory);

				if (pNewActiveCategory->IsVisible ())
				{
					return SetActiveCategory (pNewActiveCategory);
				}
			
				nActiveCategoryIndex--;
			}
		}
		else
		{
			nActiveCategoryIndex++;

			while (nActiveCategoryIndex < (int) m_arCategories.GetSize ())
			{
				pNewActiveCategory = m_arCategories [nActiveCategoryIndex];
				ASSERT_VALID (pNewActiveCategory);

				if (pNewActiveCategory->IsVisible ())
				{
					return SetActiveCategory (pNewActiveCategory);
				}

				nActiveCategoryIndex++;
			}
		}

		return TRUE;
	}

	PopTooltip ();
	return SetActiveCategory (pNewActiveCategory);
}
//*****************************************************************************************
LRESULT CBCGPRibbonBar::OnMouseLeave(WPARAM,LPARAM)
{
	CPoint point;
	
	::GetCursorPos (&point);
	ScreenToClient (&point);

	CRect rectClient;
	GetClientRect (rectClient);

	if (!rectClient.PtInRect (point))
	{
		OnMouseMove (0, CPoint (-1, -1));
	}

	m_bTracked = FALSE;
	return 0;
}
//******************************************************************************************
void CBCGPRibbonBar::OnCancelMode() 
{
	CBCGPControlBar::OnCancelMode();

	DeactivateKeyboardFocus (FALSE);

	if (m_bAutoCommandTimer)
	{
		KillTimer (IdAutoCommand);
		m_bAutoCommandTimer = FALSE;
	}

	m_bTracked = FALSE;

	PopTooltip ();

	if (m_pActiveCategory != NULL)
	{
		ASSERT_VALID (m_pActiveCategory);
		m_pActiveCategory->OnCancelMode ();
	}

	if (m_pHighlighted != NULL)
	{
		ASSERT_VALID (m_pHighlighted);
		
		CRect rect = m_pHighlighted->GetRect ();

		m_pHighlighted->m_bIsHighlighted = FALSE;
		m_pHighlighted->OnHighlight (FALSE);
		m_pHighlighted->m_bIsPressed = FALSE;

		if (m_pPressed == m_pHighlighted)
		{
			m_pPressed = NULL;
		}

		m_pHighlighted = NULL;
		RedrawWindow (rect);
	}

	if (m_pPressed != NULL)
	{
		ASSERT_VALID (m_pPressed);
		
		CRect rect = m_pPressed->GetRect ();

		m_pPressed->m_bIsHighlighted = FALSE;
		m_pPressed->m_bIsPressed = FALSE;

		m_pPressed = NULL;
		RedrawWindow (rect);
	}
}
//******************************************************************************************
void CBCGPRibbonBar::RecalcLayout ()
{
	if (GetSafeHwnd () == NULL)
	{
		return;
	}

	CRect rect;
	GetClientRect (rect);

	if (m_bDlgBarMode)
	{
		CClientDC dc (this);

		CFont* pOldFont = dc.SelectObject (GetFont ());
		ASSERT (pOldFont != NULL);

		if (m_pActiveCategory != NULL)
		{
			ASSERT_VALID (m_pActiveCategory);

			m_pActiveCategory->m_rect = rect;
			m_pActiveCategory->RecalcLayout (&dc);
		}

		dc.SelectObject (pOldFont);
		return;
	}

	DeactivateKeyboardFocus ();

	m_bIsTransparentCaption = FALSE;
	
	if (m_pPrintPreviewCategory == NULL && m_bIsPrintPreview)
	{
		AddPrintPreviewCategory ();
		ASSERT_VALID (m_pPrintPreviewCategory);
	}

	m_nTabTrancateRatio = 0;

	CBCGPControlBar::RecalcLayout ();

	const BOOL bHideAll = m_dwHideFlags & BCGPRIBBONBAR_HIDE_ALL;
	int nCategoryHeight = (m_dwHideFlags & BCGPRIBBONBAR_HIDE_ELEMENTS) ? 0 : m_nCategoryHeight;
	if (m_bBackstageViewActive)
	{
		nCategoryHeight = CBCGPVisualManager::GetInstance ()->GetRibbonBackstageTopLineHeight();
	}

	const int cyFrameBorder = GetSystemMetrics (SM_CYSIZEFRAME);

	m_bTransparentTabs = CBCGPVisualManager::GetInstance()->DoesRibbonExtendCaptionToTabsArea() && !bHideAll &&
		(!GetParent ()->IsZoomed () || globalData.bIsWindows7);

	int i = 0;

	CClientDC dc (this);

	CFont* pOldFont = dc.SelectObject (GetFont ());
	ASSERT (pOldFont != NULL);

	CString strCaption;
	GetParent ()->GetWindowText (strCaption);

	const int nCaptionTextWidth = dc.GetTextExtent (strCaption).cx;

	for (i = 0; i < m_arContextCaptions.GetSize (); i++)
	{
		ASSERT_VALID (m_arContextCaptions [i]);
		m_arContextCaptions [i]->SetRect (CRect (0, 0, 0, 0));
	}

	//-----------------------------------
	// Repos caption and caption buttons:
	//-----------------------------------
	int xSysButtonsLeft = 0;
	m_rectSysButtons.SetRectEmpty ();

	if (!m_bReplaceFrameCaption)
	{
		m_rectCaption.SetRectEmpty ();
		m_rectCaptionText.SetRectEmpty ();

		for (i = 0; i < RIBBON_CAPTION_BUTTONS; i++)
		{
			m_CaptionButtons [i].SetRect (CRect (0, 0, 0, 0));
		}
	}
	else
	{
		m_rectCaption = rect;
		m_rectCaption.bottom = m_rectCaption.top + m_nCaptionHeight;

		int x = m_rectCaption.right;
		int nCaptionOffsetY = 0;
	
		if (globalData.DwmIsCompositionEnabled ())
		{
			if (GetParent ()->IsZoomed ())
			{
				rect.top += cyFrameBorder / 2 + yMargin;
				m_rectCaption.OffsetRect (0, cyFrameBorder / 2 + yMargin);
			}

			//------------------
			// Hide our buttons:
			//------------------
			for (i = 0; i < RIBBON_CAPTION_BUTTONS; i++)
			{
				m_CaptionButtons [i].SetRect (CRect (0, 0, 0, 0));
			}

			//-------------------------
			// Get system buttons size:
			//-------------------------
			NONCLIENTMETRICS ncm;
			globalData.GetNonClientMetrics  (ncm);

			int nSysButtonsWidth = ncm.iCaptionWidth;

			if (GetParent ()->GetStyle () & WS_MINIMIZEBOX)
			{
				nSysButtonsWidth += ncm.iCaptionWidth;
			}

			if (GetParent ()->GetStyle () & WS_MAXIMIZEBOX)
			{
				nSysButtonsWidth += ncm.iCaptionWidth;
			}

			x -= nSysButtonsWidth;
			
			m_rectSysButtons = m_rectCaption;
			m_rectSysButtons.left = x;
			xSysButtonsLeft = x;
		}
		else
		{
			NONCLIENTMETRICS ncm;
			globalData.GetNonClientMetrics  (ncm);

			int nSysBtnEdge = min(ncm.iCaptionHeight, m_rectCaption.Height () - yMargin);

			const CSize sizeCaptionButton (ncm.iCaptionWidth, nSysBtnEdge);
			const int yOffsetCaptionButton = max (0, 
				(m_rectCaption.Height () - sizeCaptionButton.cy) / 2);

			for (i = RIBBON_CAPTION_BUTTONS - 1; i >= 0; i--)
			{
				if (m_CaptionButtons [i].GetID () == SC_CLOSE)
				{
					m_CaptionButtons [i].m_bIsDisabled = FALSE;

					CMenu* pSysMenu = GetParent ()->GetSystemMenu (FALSE);
					if (pSysMenu->GetSafeHmenu () != NULL)
					{
						MENUITEMINFO menuInfo;
						ZeroMemory(&menuInfo,sizeof(MENUITEMINFO));
						menuInfo.cbSize = sizeof(MENUITEMINFO);
						menuInfo.fMask = MIIM_STATE;

						if (!pSysMenu->GetMenuItemInfo (SC_CLOSE, &menuInfo) ||
							(menuInfo.fState & MFS_GRAYED) || 
							(menuInfo.fState & MFS_DISABLED))
						{
							m_CaptionButtons [i].m_bIsDisabled = TRUE;
						}
					}
				}

				if ((m_CaptionButtons [i].GetID () == SC_RESTORE ||
					m_CaptionButtons [i].GetID () == SC_MAXIMIZE) &&
					(GetParent ()->GetStyle () & WS_MAXIMIZEBOX) == 0)
				{
					m_CaptionButtons [i].SetRect (CRect (0, 0, 0, 0));
					continue;
				}

				if (m_CaptionButtons [i].GetID () == SC_MINIMIZE &&
					(GetParent ()->GetStyle () & WS_MINIMIZEBOX) == 0)
				{
					m_CaptionButtons [i].SetRect (CRect (0, 0, 0, 0));
					continue;
				}

				CRect rectCaptionButton (
					CPoint (x - sizeCaptionButton.cx, m_rectCaption.top + yOffsetCaptionButton),
					sizeCaptionButton);

				m_CaptionButtons [i].SetRect (rectCaptionButton);

				x -= sizeCaptionButton.cx;

				if (m_CaptionButtons [i].GetID () == SC_RESTORE ||
					m_CaptionButtons [i].GetID () == SC_MAXIMIZE)
				{
					m_CaptionButtons [i].SetID (GetParent ()->IsZoomed () ? SC_RESTORE : SC_MAXIMIZE);
				}
			}
		}

		m_rectCaptionText = m_rectCaption;

		if (globalData.DwmIsCompositionEnabled ())
		{
			m_rectCaptionText.top += GetSystemMetrics (SM_CYSIZEFRAME) / 2;
		}

		m_rectCaptionText.right = x - xMargin;
		m_rectCaptionText.OffsetRect (0, nCaptionOffsetY);

		xSysButtonsLeft = m_rectCaptionText.right;
	}

	//-------------------
	// Repos main button:
	//-------------------
	CSize sizeMainButton = m_sizeMainButton;
	if (IsScenicLook ())
	{
		sizeMainButton.cx += 2 * yTabMargin;
	}

	double scale = globalData.GetRibbonImageScale ();
	if (scale > 1.)
	{
		sizeMainButton.cx = (int)(.5 + scale * sizeMainButton.cx);
		sizeMainButton.cy = (int)(.5 + scale * sizeMainButton.cy);
	}

	if (m_pMainButton != NULL)
	{
		ASSERT_VALID (m_pMainButton);

		m_pMainButton->OnCalcTextSize(&dc);

		sizeMainButton.cx = max(sizeMainButton.cx, m_pMainButton->m_sizeTextRight.cx + 2 * xTabMargin);
		sizeMainButton.cy = max(sizeMainButton.cy, m_pMainButton->m_sizeTextRight.cy);

		if (bHideAll)
		{
			m_pMainButton->SetRect (CRect (0, 0, 0, 0));
		}
		else
		{
			int yOffset = yMargin;

			if (globalData.DwmIsCompositionEnabled ())
			{
				yOffset += GetSystemMetrics (SM_CYSIZEFRAME) / 2;
			}

			m_pMainButton->SetRect (CRect (
				CPoint (rect.left, rect.top + yOffset), sizeMainButton));

			if (!IsScenicLook ())
			{
				m_rectCaptionText.left = m_pMainButton->GetRect ().right + xMargin;
			}
			else
			{
				CRect rectMainBtn = rect;
				rectMainBtn.top = m_rectCaption.IsRectEmpty () ? rect.top : m_rectCaption.bottom;
				rectMainBtn.bottom = rectMainBtn.top + m_nTabsHeight;
				rectMainBtn.right = rectMainBtn.left + sizeMainButton.cx;

				m_pMainButton->SetRect (rectMainBtn);

				if (IsQuickAccessToolbarOnTop ())
				{
					m_rectCaptionText.left = m_rectCaption.left + 
						::GetSystemMetrics (SM_CXSMICON) + 
						4 * xMargin;
				}
			}
		}
	}

	CRect rectCategory = rect;
	m_nBackstageViewTop = m_rectCaption.bottom;

	//----------------------------
	// Repos quick access toolbar:
	//----------------------------
	int nQAToolbarHeight = 0;

	if (bHideAll)
	{
		m_QAToolbar.m_rect.SetRectEmpty ();
		m_TabElements.m_rect.SetRectEmpty ();
	}
	else
	{
		CSize sizeAQToolbar = m_QAToolbar.GetRegularSize (&dc);

		if (IsQuickAccessToolbarOnTop ())
		{
			m_QAToolbar.m_rect = m_rectCaptionText;

			const int yOffset = max (0, 
				(m_rectCaptionText.Height () - sizeAQToolbar.cy) / 2);

			m_QAToolbar.m_rect.top += yOffset;
			m_QAToolbar.m_rect.bottom = m_QAToolbar.m_rect.top + sizeAQToolbar.cy;

			if (globalData.DwmIsCompositionEnabled ())
			{
				m_QAToolbar.m_rect.top += yMargin;
			}

			m_QAToolbar.m_rect.right = 
				min (m_QAToolbar.m_rect.left + sizeAQToolbar.cx, 
					m_rectCaptionText.right - 50);

			m_QAToolbar.OnAfterChangeRect (&dc);
			
			int nQAActualWidth = m_QAToolbar.GetActualWidth ();
			int nQARight = m_QAToolbar.m_rect.left + nQAActualWidth + xMargin;

			if (nQARight < m_QAToolbar.m_rect.right)
			{
				m_QAToolbar.m_rect.right = nQARight;
			}

			m_rectCaptionText.left = m_QAToolbar.m_rect.right;
			if (!IsScenicLook ())
			{
				m_rectCaptionText.left += CBCGPVisualManager::GetInstance ()->GetRibbonQATRightMargin ();
			}
			else
			{
				m_rectCaptionText.left += 3 * xMargin;
			}
		}
		else if (m_bBackstageViewActive)
		{
			m_QAToolbar.m_rect.SetRectEmpty();
		}
		else
		{
			m_QAToolbar.m_rect = rect;
			m_QAToolbar.m_rect.top = m_QAToolbar.m_rect.bottom - sizeAQToolbar.cy;
			nQAToolbarHeight = sizeAQToolbar.cy;

			rectCategory.bottom = m_QAToolbar.m_rect.top;
		}
	}

	m_QAToolbar.OnAfterChangeRect (&dc);

	if (!bHideAll)
	{
		const int yTabTop = m_rectCaption.IsRectEmpty () ? rect.top : m_rectCaption.bottom;
		const int yTabBottom = rect.bottom - nCategoryHeight - nQAToolbarHeight;

		m_nBackstageViewTop = yTabBottom + CBCGPVisualManager::GetInstance ()->GetRibbonBackstageTopLineHeight();

		//--------------------
		// Repos tab elements:
		//--------------------
		CSize sizeTabElemens = m_TabElements.GetCompactSize (&dc);

		const int yOffset = max (0, (yTabBottom - yTabTop - sizeTabElemens.cy) / 2);
		const int nTabElementsHeight = min (m_nTabsHeight, sizeTabElemens.cy);

		m_TabElements.m_rect = CRect (
			CPoint (rect.right - sizeTabElemens.cx, yTabTop + yOffset), 
			CSize (sizeTabElemens.cx, nTabElementsHeight));

		//------------
		// Repos tabs:
		//------------

		const int nTabs = GetVisibleCategoryCount ();

		if (nTabs > 0)
		{
			const int nTabLeftOffset = sizeMainButton.cx + 1;
			const int cxTabsArea = rect.Width () - nTabLeftOffset - sizeTabElemens.cx - xMargin;
			const int nMaxTabWidth = cxTabsArea / nTabs;

			int x = rect.left + nTabLeftOffset;
			BOOL bIsFirstContextTab = TRUE;
			BOOL bCaptionOnRight = FALSE;

			int cxTabs = 0;

			for (i = 0; i < m_arCategories.GetSize (); i++)
			{
				CBCGPRibbonCategory* pCategory = m_arCategories [i];
				ASSERT_VALID (pCategory);

				if (pCategory->IsVisible ())
				{
					CRect rectTabText (0, 0, nMaxTabWidth, m_nTabsHeight);

					dc.DrawText (pCategory->m_strName, rectTabText, 
						DT_CALCRECT | DT_SINGLELINE | DT_VCENTER);

					int nTextWidth = rectTabText.Width ();
					int nCurrTabMargin = xTabMargin + nTextWidth / 40;

					pCategory->m_Tab.m_nFullWidth = nTextWidth + 2 * nCurrTabMargin;

					const UINT uiContextID = pCategory->GetContextID ();

					if (uiContextID != 0 && m_bReplaceFrameCaption)
					{
						// If the current tab is last in current context, and there is no space
						// for category caption width, add extra space:
						BOOL bIsSingle = TRUE;

						for (int j = 0; j < m_arCategories.GetSize (); j++)
						{
							CBCGPRibbonCategory* pCategoryNext = m_arCategories [j];
							ASSERT_VALID (pCategoryNext);

							if (i != j && pCategoryNext->GetContextID () == uiContextID)
							{
								bIsSingle = FALSE;
								break;
							}
						}

						if (bIsSingle)
						{
							CBCGPRibbonContextCaption* pCaption = FindContextCaption (uiContextID);
							if (pCaption != NULL)
							{
								ASSERT_VALID (pCaption);

								const int nMinCaptionWidth = dc.GetTextExtent (pCaption->GetText ()).cx + 2 * xTabMargin;

								if (nMinCaptionWidth > pCategory->m_Tab.m_nFullWidth)
								{
									pCategory->m_Tab.m_nFullWidth = nMinCaptionWidth;
								}
							}
						}
					}

					
					cxTabs += pCategory->m_Tab.m_nFullWidth;
				}
				else
				{
					pCategory->m_Tab.m_nFullWidth = 0;
				}
			}

			BOOL bNoSpace = cxTabs > cxTabsArea;

			for (i = 0; i < m_arCategories.GetSize (); i++)
			{
				CBCGPRibbonCategory* pCategory = m_arCategories [i];
				ASSERT_VALID (pCategory);

				if (!pCategory->IsVisible ())
				{
					pCategory->m_Tab.SetRect (CRect (0, 0, 0, 0));
					continue;
				}

				int nTabWidth = pCategory->m_Tab.m_nFullWidth;

				if (nTabWidth > nMaxTabWidth && bNoSpace)
				{
					pCategory->m_Tab.m_bIsTrancated = TRUE;

					if (nTabWidth > 0)
					{
						m_nTabTrancateRatio = max (m_nTabTrancateRatio,
							(int) (100 - 100. * nMaxTabWidth / nTabWidth));
					}

					nTabWidth = nMaxTabWidth;
				}
				else
				{
					pCategory->m_Tab.m_bIsTrancated = FALSE;
				}

				pCategory->m_Tab.SetRect ( 
					CRect (x, yTabTop, x + nTabWidth, yTabBottom));

				const UINT uiContextID = pCategory->GetContextID ();

				if (uiContextID != 0 && m_bReplaceFrameCaption)
				{
					CBCGPRibbonContextCaption* pCaption = FindContextCaption (uiContextID);
					if (pCaption != NULL)
					{
						ASSERT_VALID (pCaption);

						int nCaptionWidth = nTabWidth;

						CRect rectOld = pCaption->GetRect ();
						CRect rectCaption = m_rectCaption;

						rectCaption.left = rectOld.left == 0 ? x : rectOld.left;
						rectCaption.right = min (xSysButtonsLeft, x + nCaptionWidth);

						if (bIsFirstContextTab)
						{
							if (IsQuickAccessToolbarOnTop () &&
								rectCaption.left - xTabMargin < m_QAToolbar.m_rect.right)
							{
								m_QAToolbar.m_rect.right = rectCaption.left - xTabMargin;

								if (m_QAToolbar.m_rect.right <= m_QAToolbar.m_rect.left)
								{
									m_QAToolbar.m_rect.SetRectEmpty ();
								}

								m_QAToolbar.OnAfterChangeRect (&dc);

								m_rectCaptionText.left = rectCaption.right + xTabMargin;
								bCaptionOnRight = TRUE;
							}
							else
							{
								const int yCaptionRight = min (m_rectCaptionText.right, x);
								const int nCaptionWidthLeft = yCaptionRight - m_rectCaptionText.left;
								const int nCaptionWidthRight = m_rectCaption.right - rectCaption.right - xTabMargin;

								if (nCaptionTextWidth > nCaptionWidthLeft &&
									nCaptionWidthLeft < nCaptionWidthRight)
								{
									m_rectCaptionText.left = rectCaption.right + xTabMargin;
									bCaptionOnRight = TRUE;
								}
								else
								{
									m_rectCaptionText.right = yCaptionRight;
								}
							}

							bIsFirstContextTab = FALSE;
						}
						else if (bCaptionOnRight)
						{
							m_rectCaptionText.left = rectCaption.right + xTabMargin;
						}

						pCaption->SetRect (rectCaption);
						
						pCaption->m_nRightTabX = pCategory->m_Tab.m_bIsTrancated ? -1 : 
							pCategory->m_Tab.GetRect ().right;
					}
				}

				x += nTabWidth;
			}
		}

		rectCategory.top = yTabBottom;
	}

	m_TabElements.OnAfterChangeRect (&dc);

	CRect rectCategoryRedraw = rectCategory;

	if (m_pActiveCategory != NULL)
	{
		ASSERT_VALID (m_pActiveCategory);

		m_pActiveCategory->m_rect = bHideAll ? CRect (0, 0, 0, 0) : rectCategory;

		if (nCategoryHeight > 0 && !m_bBackstageViewActive)
		{
			int nLastPanelIndex = m_pActiveCategory->GetPanelCount () - 1;
			
			CRect rectLastPaneOld;
			rectLastPaneOld.SetRectEmpty ();

			if (nLastPanelIndex >= 0)
			{
				rectLastPaneOld = m_pActiveCategory->GetPanel (nLastPanelIndex)->GetRect ();
			}

			m_pActiveCategory->RecalcLayout (&dc);
			
			if (nLastPanelIndex >= 0 &&
				m_pActiveCategory->GetPanel (nLastPanelIndex)->GetRect () == rectLastPaneOld)
			{
				rectCategoryRedraw.left = rectLastPaneOld.right;
			}
		}
	}

	dc.SelectObject (pOldFont);

	if (globalData.DwmIsCompositionEnabled () && 
		m_bReplaceFrameCaption && IsVisible())
	{
		GetParent ()->ModifyStyleEx (0, WS_EX_WINDOWEDGE);

		BCGPMARGINS margins;
		margins.cxLeftWidth = 0;
		margins.cxRightWidth = 0;
		margins.cyTopHeight = m_rectCaption.bottom;
		margins.cyBottomHeight = 0;

		if (m_bTransparentTabs)
		{
			margins.cyTopHeight += m_nTabsHeight + 1;
		}

		if (globalData.DwmExtendFrameIntoClientArea (
			GetParent ()->GetSafeHwnd (), &margins))
		{
			m_bIsTransparentCaption = TRUE;
		}
		else
		{
			m_bTransparentTabs = FALSE;
		}
	}

	if (m_bForceRedraw)
	{
		RedrawWindow ();
		m_bForceRedraw = FALSE;
	}
	else
	{
		InvalidateRect (m_rectCaption);

		CRect rectTabs = rect;
		rectTabs.top = m_rectCaption.IsRectEmpty () ? rect.top : m_rectCaption.bottom;
		rectTabs.bottom = rectTabs.top + m_nTabsHeight + 2 * yTabMargin;

		InvalidateRect (rectTabs);
		InvalidateRect (m_QAToolbar.m_rect);
		InvalidateRect (rectCategoryRedraw);

		UpdateWindow ();
	}

	CMenu* pSysMenu = GetParent ()->GetSystemMenu (FALSE);

	if (m_bReplaceFrameCaption &&
		pSysMenu->GetSafeHmenu () != NULL && 
		!m_bIsTransparentCaption)
	{
		for (i = 0; i < RIBBON_CAPTION_BUTTONS; i++)
		{
			CString strName;
			pSysMenu->GetMenuString (m_CaptionButtons [i].GetID (), strName, MF_BYCOMMAND);

			strName = strName.SpanExcluding (_T("\t"));
			strName.Remove (_T('&'));

			m_CaptionButtons [i].SetToolTipText (strName);
		}
	}

	UpdateToolTipsRect ();
}
//******************************************************************************
void CBCGPRibbonBar::OnFillBackground (CDC* pDC, CRect rectClient)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);

	if (m_bIsTransparentCaption)
	{
		rectClient.top = m_rectCaption.bottom;
	}

	CBCGPVisualManager::GetInstance ()->OnFillBarBackground (pDC, this, rectClient, rectClient);
}
//******************************************************************************
CBCGPBaseRibbonElement* CBCGPRibbonBar::HitTest (CPoint point, 
												 BOOL bCheckActiveCategory,
												 BOOL bCheckPanelCaption)
{
	ASSERT_VALID (this);

	int i = 0;

	//---------------------------
	// Check for the main button:
	//---------------------------
	if (m_pMainButton != NULL)
	{
		ASSERT_VALID (m_pMainButton);

		CRect rectMainButton = m_pMainButton->GetRect ();

		if (!IsScenicLook ())
		{
			rectMainButton.left = rectMainButton.top = 0;
		}

		if (rectMainButton.PtInRect (point))
		{
			return m_pMainButton;
		}
	}

	if (!m_bBackstageViewActive)
	{
		//--------------------------------
		// Check for quick access toolbar:
		//--------------------------------
		CBCGPBaseRibbonElement* pQAElem = m_QAToolbar.HitTest (point);
		if (pQAElem != NULL)
		{
			ASSERT_VALID (pQAElem);
			return pQAElem;
		}

		//------------------------
		// Check for tab elements:
		//------------------------
		CBCGPBaseRibbonElement* pTabElem = m_TabElements.HitTest (point);
		if (pTabElem != NULL)
		{
			ASSERT_VALID (pTabElem);
			return pTabElem->HitTest (point);
		}
	}

	//---------------------------
	// Check for caption buttons:
	//---------------------------
	for (i = 0; i < RIBBON_CAPTION_BUTTONS; i++)
	{
		if (m_CaptionButtons [i].GetRect ().PtInRect (point))
		{
			return &m_CaptionButtons [i];
		}
	}

	//----------------------------
	// Check for context captions:
	//----------------------------
	for (i = 0; i < m_arContextCaptions.GetSize (); i++)
	{
		ASSERT_VALID (m_arContextCaptions [i]);

		if (m_arContextCaptions [i]->GetRect ().PtInRect (point))
		{
			return m_arContextCaptions [i];
		}
	}

	//----------------
	// Check for tabs:
	//----------------
	for (i = 0; i < m_arCategories.GetSize (); i++)
	{
		CBCGPRibbonCategory* pCategory = m_arCategories [i];
		ASSERT_VALID (pCategory);

		if (pCategory->m_Tab.GetRect ().PtInRect (point))
		{
			return &pCategory->m_Tab;
		}
	}

	if (m_pActiveCategory != NULL &&
		(m_dwHideFlags & BCGPRIBBONBAR_HIDE_ELEMENTS) == 0 &&
		bCheckActiveCategory && !m_bBackstageViewActive)
	{
		ASSERT_VALID (m_pActiveCategory);
		return m_pActiveCategory->HitTest (point, bCheckPanelCaption);
	}

	return NULL;
}
//*******************************************************************************
void CBCGPRibbonBar::SetQuickAccessToolbarOnTop (BOOL bOnTop)
{
	ASSERT_VALID (this);

	m_bQuickAccessToolbarOnTop = bOnTop;
}
//*******************************************************************************
void CBCGPRibbonBar::SetQuickAccessDefaultState (const CBCGPRibbonQATDefaultState& state)
{
	ASSERT_VALID (this);

	m_QAToolbar.m_DefaultState.CopyFrom (state);

	CList<UINT,UINT> lstDefCommands;
	m_QAToolbar.GetDefaultCommands (lstDefCommands);

	SetQuickAccessCommands (lstDefCommands, FALSE);
}
//*******************************************************************************
void CBCGPRibbonBar::SetQuickAccessCommands (const CList<UINT,UINT>& lstCommands, BOOL bRecalcLayout/* = TRUE*/)
{
	ASSERT_VALID (this);

	OnCancelMode ();

	CString strTooltip;

	{
		CBCGPLocalResource locaRes;
		strTooltip.LoadString (IDS_BCGBARRES_CUSTOMIZE_QAT_TOOLTIP);
	}

	m_QAToolbar.SetCommands (this, lstCommands, strTooltip);

	if (bRecalcLayout)
	{
		m_bForceRedraw = TRUE;
		RecalcLayout ();
	}
}
//*******************************************************************************
void CBCGPRibbonBar::GetQuickAccessCommands (CList<UINT,UINT>& lstCommands)
{
	ASSERT_VALID (this);
	m_QAToolbar.GetCommands (lstCommands);
}
//*******************************************************************************
void CBCGPRibbonBar::OnClickButton (CBCGPRibbonButton* pButton, CPoint /*point*/)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pButton);

	const UINT nID = pButton->GetID ();

	pButton->m_bIsHighlighted = pButton->m_bIsPressed = FALSE;
	RedrawWindow (pButton->GetRect ());

	if (nID != 0 && nID != (UINT)-1)
	{
		GetOwner()->SendMessage (WM_COMMAND, nID);
	}
}
//*******************************************************************************
void CBCGPRibbonBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	ASSERT_VALID (this);

	CBCGPRibbonCmdUI state;
	state.m_pOther = this;

	if (m_pActiveCategory != NULL)
	{
		ASSERT_VALID (m_pActiveCategory);
		m_pActiveCategory->OnUpdateCmdUI (&state, pTarget, bDisableIfNoHndler);
	}

	if (!m_bBackstageViewActive)
	{
		m_QAToolbar.OnUpdateCmdUI (&state, pTarget, bDisableIfNoHndler);
		m_TabElements.OnUpdateCmdUI (&state, pTarget, bDisableIfNoHndler);
	}

	// update the dialog controls added to the ribbon
	UpdateDialogControls(pTarget, bDisableIfNoHndler);
}
//*************************************************************************************
BOOL CBCGPRibbonBar::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	BOOL bAccelerator = FALSE;
	int nNotifyCode = HIWORD (wParam);

	// Find the control send the message:
	HWND hWndCtrl = (HWND)lParam;
	if (hWndCtrl == NULL)
	{
		if (wParam == IDCANCEL)	// ESC was pressed
		{
			return TRUE;
		}

		if (wParam != IDOK ||
			(hWndCtrl = ::GetFocus ()) == NULL)
		{
			return FALSE;
		}

		bAccelerator = TRUE;
		nNotifyCode = 0;
	}

	if (m_pActiveCategory != NULL)
	{
		ASSERT_VALID (m_pActiveCategory);

		return m_pActiveCategory->NotifyControlCommand (
			bAccelerator, nNotifyCode, wParam, lParam);
	}

	return FALSE;
}
//*************************************************************************************
CBCGPBaseRibbonElement* CBCGPRibbonBar::FindByID (UINT uiCmdID,
												  BOOL bVisibleOnly,
												  BOOL bExcludeQAT) const
{
	ASSERT_VALID (this);

	if (!bExcludeQAT)
	{
		CBCGPBaseRibbonElement* pQATElem = ((CBCGPRibbonBar*) this)->m_QAToolbar.FindByID (uiCmdID);
		if (pQATElem != NULL)
		{
			ASSERT_VALID (pQATElem);
			return pQATElem;
		}
	}

	if (m_pMainCategory != NULL)
	{
		ASSERT_VALID (m_pMainCategory);

		CBCGPBaseRibbonElement* pElem = m_pMainCategory->FindByID (uiCmdID, bVisibleOnly);
		if (pElem != NULL)
		{
			ASSERT_VALID (pElem);
			return pElem;
		}
	}

	if (m_pBackstageCategory != NULL)
	{
		ASSERT_VALID (m_pBackstageCategory);

		CBCGPBaseRibbonElement* pElem = m_pBackstageCategory->FindByID (uiCmdID, bVisibleOnly);
		if (pElem != NULL)
		{
			ASSERT_VALID (pElem);
			return pElem;
		}
	}

	for (int i = 0; i < m_arCategories.GetSize (); i++)
	{
		CBCGPRibbonCategory* pCategory = m_arCategories [i];
		ASSERT_VALID (pCategory);

		if (bVisibleOnly && !pCategory->IsVisible ())
		{
			continue;
		}

		CBCGPBaseRibbonElement* pElem = pCategory->FindByID (uiCmdID, bVisibleOnly);
		if (pElem != NULL)
		{
			ASSERT_VALID (pElem);
			return pElem;
		}
	}

	return ((CBCGPRibbonBar*) this)->m_TabElements.FindByID (uiCmdID);
}
//*************************************************************************************
CBCGPBaseRibbonElement* CBCGPRibbonBar::FindByData (DWORD_PTR dwData,
												   BOOL bVisibleOnly) const
{
	ASSERT_VALID (this);

	if (m_pMainCategory != NULL)
	{
		ASSERT_VALID (m_pMainCategory);

		CBCGPBaseRibbonElement* pElem = m_pMainCategory->FindByData (dwData, bVisibleOnly);
		if (pElem != NULL)
		{
			ASSERT_VALID (pElem);
			return pElem;
		}
	}

	if (m_pBackstageCategory != NULL)
	{
		ASSERT_VALID (m_pBackstageCategory);

		CBCGPBaseRibbonElement* pElem = m_pBackstageCategory->FindByData (dwData, bVisibleOnly);
		if (pElem != NULL)
		{
			ASSERT_VALID (pElem);
			return pElem;
		}
	}

	for (int i = 0; i < m_arCategories.GetSize (); i++)
	{
		CBCGPRibbonCategory* pCategory = m_arCategories [i];
		ASSERT_VALID (pCategory);

		if (bVisibleOnly && !pCategory->IsVisible ())
		{
			continue;
		}

		CBCGPBaseRibbonElement* pElem = pCategory->FindByData (dwData, bVisibleOnly);
		if (pElem != NULL)
		{
			ASSERT_VALID (pElem);
			return pElem;
		}
	}

	return ((CBCGPRibbonBar*) this)->m_TabElements.FindByData (dwData);
}
//*************************************************************************************
void CBCGPRibbonBar::GetElementsByID (UINT uiCmdID, 
	CArray<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*>& arButtons, BOOL bIncludeFloaty/* = FALSE*/)
{
	ASSERT_VALID (this);

	arButtons.RemoveAll ();

	if (m_pMainCategory != NULL)
	{
		ASSERT_VALID (m_pMainCategory);
		m_pMainCategory->GetElementsByID (uiCmdID, arButtons);
	}

	if (m_pBackstageCategory != NULL)
	{
		ASSERT_VALID (m_pBackstageCategory);
		m_pBackstageCategory->GetElementsByID (uiCmdID, arButtons);
	}

	for (int i = 0; i < m_arCategories.GetSize (); i++)
	{
		CBCGPRibbonCategory* pCategory = m_arCategories [i];
		ASSERT_VALID (pCategory);

		pCategory->GetElementsByID (uiCmdID, arButtons);
	}

	m_QAToolbar.GetElementsByID (uiCmdID, arButtons);
	m_TabElements.GetElementsByID (uiCmdID, arButtons);

	if (bIncludeFloaty && CBCGPRibbonFloaty::m_pCurrent != NULL &&
		::IsWindow (CBCGPRibbonFloaty::m_pCurrent->GetSafeHwnd ()))
	{
		CBCGPRibbonPanel* pFloatyPanel = CBCGPRibbonFloaty::m_pCurrent->GetPanel ();
		if (pFloatyPanel != NULL)
		{
			ASSERT_VALID (pFloatyPanel);
			pFloatyPanel->GetElementsByID (uiCmdID, arButtons);
		}
	}
}
//*************************************************************************************
void CBCGPRibbonBar::GetVisibleElements (
	CArray<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*>& arButtons)
{
	ASSERT_VALID (this);

	arButtons.RemoveAll ();

	if (m_pMainButton != NULL)
	{
		ASSERT_VALID (m_pMainButton);
		m_pMainButton->GetVisibleElements (arButtons);
	}

	m_QAToolbar.GetVisibleElements (arButtons);

	for (int i = 0; i < m_arCategories.GetSize (); i++)
	{
		CBCGPRibbonCategory* pCategory = m_arCategories [i];
		ASSERT_VALID (pCategory);

		if (!pCategory->m_Tab.m_rect.IsRectEmpty ())
		{
			pCategory->m_Tab.GetVisibleElements (arButtons);
		}
	}

	m_TabElements.GetVisibleElements (arButtons);

	if (m_pActiveCategory != NULL && (m_dwHideFlags & BCGPRIBBONBAR_HIDE_ELEMENTS) == 0)
	{
		ASSERT_VALID (m_pActiveCategory);
		m_pActiveCategory->GetVisibleElements (arButtons);
	}
}
//*************************************************************************************
void CBCGPRibbonBar::GetElementsByName (LPCTSTR lpszName, 
	CArray<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*>& arButtons, DWORD dwFlags)
{
	ASSERT_VALID (this);

	arButtons.RemoveAll ();

	if (m_pMainCategory != NULL)
	{
		ASSERT_VALID (m_pMainCategory);
		m_pMainCategory->GetElementsByName (lpszName, arButtons, dwFlags);
	}

	if (m_pBackstageCategory != NULL)
	{
		ASSERT_VALID (m_pBackstageCategory);
		m_pBackstageCategory->GetElementsByName (lpszName, arButtons, dwFlags);
	}

	for (int i = 0; i < m_arCategories.GetSize (); i++)
	{
		CBCGPRibbonCategory* pCategory = m_arCategories [i];
		ASSERT_VALID (pCategory);

		pCategory->GetElementsByName (lpszName, arButtons, dwFlags);
	}

	m_TabElements.GetElementsByName (lpszName, arButtons, dwFlags);
}
//*************************************************************************************
BOOL CBCGPRibbonBar::SetElementKeys (UINT uiCmdID, LPCTSTR lpszKeys, LPCTSTR lpszMenuKeys)
{
	ASSERT_VALID (this);

	int i = 0;

	BOOL bFound = FALSE;

	if (m_pMainCategory != NULL)
	{
		ASSERT_VALID (m_pMainCategory);

		CArray<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*> arButtons;
		m_pMainCategory->GetElementsByID (uiCmdID, arButtons);

		for (int j = 0; j < arButtons.GetSize (); j++)
		{
			ASSERT_VALID (arButtons [j]);
			arButtons [j]->SetKeys (lpszKeys, lpszMenuKeys);
			
			bFound = TRUE;
		}
	}

	if (m_pBackstageCategory != NULL)
	{
		ASSERT_VALID (m_pBackstageCategory);

		CArray<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*> arButtons;
		m_pBackstageCategory->GetElementsByID (uiCmdID, arButtons);

		for (int j = 0; j < arButtons.GetSize (); j++)
		{
			ASSERT_VALID (arButtons [j]);
			arButtons [j]->SetKeys (lpszKeys, lpszMenuKeys);
			
			bFound = TRUE;
		}
	}

	for (i = 0; i < m_arCategories.GetSize (); i++)
	{
		CBCGPRibbonCategory* pCategory = m_arCategories [i];
		ASSERT_VALID (pCategory);

		CArray<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*> arButtons;
		pCategory->GetElementsByID (uiCmdID, arButtons);

		for (int j = 0; j < arButtons.GetSize (); j++)
		{
			ASSERT_VALID (arButtons [j]);
			arButtons [j]->SetKeys (lpszKeys, lpszMenuKeys);
			
			bFound = TRUE;
		}
	}

	CArray<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*> arButtons;
	m_TabElements.GetElementsByID (uiCmdID, arButtons);

	for (i = 0; i < arButtons.GetSize (); i++)
	{
		ASSERT_VALID (arButtons [i]);
		arButtons [i]->SetKeys (lpszKeys, lpszMenuKeys);
		
		bFound = TRUE;
	}

	return bFound;
}
//*************************************************************************************
void CBCGPRibbonBar::AddToTabs (CBCGPBaseRibbonElement* pElement)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pElement);

	pElement->SetParentRibbonBar (this);
	pElement->m_bIsTabElement = TRUE;
	m_TabElements.AddButton (pElement);

	if (m_nSystemButtonsNum > 0)
	{
		// Move the new element prior to system buttons:
		int nSize = (int) m_TabElements.m_arButtons.GetSize () - 1;

		m_TabElements.m_arButtons.RemoveAt (nSize - 1);
		m_TabElements.m_arButtons.InsertAt (nSize - m_nSystemButtonsNum,
			pElement);
	}
}
//*************************************************************************************
void CBCGPRibbonBar::RemoveAllFromTabs ()
{
	ASSERT_VALID (this);

	if (m_nSystemButtonsNum > 0)
	{
		while (m_TabElements.m_arButtons.GetSize () > m_nSystemButtonsNum)
		{
			delete m_TabElements.m_arButtons [0];
			m_TabElements.m_arButtons.RemoveAt (0);
		}
	}
	else
	{
		m_TabElements.RemoveAll ();
	}
}
//**********************************************************************************
BOOL CBCGPRibbonBar::OnNeedTipText(UINT /*id*/, NMHDR* pNMH, LRESULT* /*pResult*/)
{
	static CString strTipText;

	if (!m_bToolTip)
	{
		return TRUE;
	}

	if (m_pToolTip->GetSafeHwnd () == NULL || 
		pNMH->hwndFrom != m_pToolTip->GetSafeHwnd ())
	{
		return FALSE;
	}

	if (CBCGPPopupMenu::GetActiveMenu () != NULL)
	{
		return FALSE;
	}

	LPNMTTDISPINFO	pTTDispInfo	= (LPNMTTDISPINFO) pNMH;
	ASSERT((pTTDispInfo->uFlags & TTF_IDISHWND) == 0);

	CPoint point;
	
	::GetCursorPos (&point);
	ScreenToClient (&point);

	CBCGPBaseRibbonElement* pHit = HitTest (point, TRUE);
	if (pHit == NULL)
	{
		return TRUE;
	}

	ASSERT_VALID (pHit);

	strTipText = pHit->GetToolTipText ();
	if (strTipText.IsEmpty ())
	{
		return TRUE;
	}

	CBCGPToolTipCtrl* pToolTip = DYNAMIC_DOWNCAST (
		CBCGPToolTipCtrl, m_pToolTip);

	if (pToolTip != NULL)
	{
		ASSERT_VALID (pToolTip);

		if (m_bToolTipDescr)
		{
			pToolTip->SetDescription (pHit->GetDescription ());
		}

		pToolTip->SetHotRibbonButton (DYNAMIC_DOWNCAST (CBCGPRibbonButton, pHit));

		if (!m_bDlgBarMode && pHit->IsShowTooltipOnBottom ())
		{
			CRect rectWindow;
			GetWindowRect (rectWindow);

			CRect rectElem = pHit->GetRect ();
			ClientToScreen (&rectElem);

			pToolTip->SetLocation (CPoint (rectElem.left, rectWindow.bottom));
		}

		pToolTip->SetFixedWidth (m_nTooltipWidthRegular, m_nTooltipWidthLargeImage);
	}

	if (m_nKeyboardNavLevel >= 0)
	{
		m_pToolTip->SetWindowPos (&wndTopMost, -1, -1, -1, -1,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	}

	pTTDispInfo->lpszText = const_cast<LPTSTR> ((LPCTSTR) strTipText);
	return TRUE;
}
//*****************************************************************************
BOOL CBCGPRibbonBar::PreTranslateMessage(MSG* pMsg) 
{
   	switch (pMsg->message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_MOUSEMOVE:
		if (m_pToolTip->GetSafeHwnd () != NULL)
		{
			m_pToolTip->RelayEvent(pMsg);
		}

		break;
	}

	if (pMsg->message == WM_LBUTTONDOWN)
	{
		CBCGPRibbonEditCtrl* pEdit = DYNAMIC_DOWNCAST (CBCGPRibbonEditCtrl, GetFocus ());
		if (pEdit != NULL)
		{
			ASSERT_VALID (pEdit);

			CPoint point;
			
			::GetCursorPos (&point);
			ScreenToClient (&point);

			pEdit->GetOwnerRibbonEdit ().PreLMouseDown (point);
		}
	}

	if (pMsg->message == WM_KEYDOWN)
	{
		int nChar = (int) pMsg->wParam;

		if (::GetFocus () == GetSafeHwnd ())
		{
			CBCGPBaseRibbonElement* pFocused = GetFocused ();
			if (pFocused != NULL)
			{
				ASSERT_VALID (pFocused);
				if (pFocused->OnProcessKey (nChar))
				{
					return TRUE;
				}
			}
		}

		switch (nChar)
		{
		case VK_ESCAPE:
			if (m_nKeyboardNavLevel > 0)
			{
				SetKeyboardNavigationLevel (m_pKeyboardNavLevelParent);
			}
			else if (CBCGPPopupMenu::GetActiveMenu () == NULL)
			{
				DeactivateKeyboardFocus ();

				CFrameWnd* pParentFrame = GetParentFrame ();
				if (pParentFrame != NULL)
				{
					ASSERT_VALID (pParentFrame);
					pParentFrame->SetFocus ();
				}
			}

			break;

		case VK_SPACE:
			if (m_nKeyboardNavLevel >= 0 && CBCGPPopupMenu::GetActiveMenu () == NULL &&
				::GetFocus () == GetSafeHwnd ())
			{
				DeactivateKeyboardFocus ();

				CFrameWnd* pParentFrame = GetParentFrame ();
				if (pParentFrame != NULL)
				{
					ASSERT_VALID (pParentFrame);
					pParentFrame->SetFocus ();
				}
			}

		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
		case VK_RETURN:
		case VK_TAB:
			if (::GetFocus () != GetSafeHwnd ())
			{
				if (nChar != VK_TAB)
				{
					break;
				}
				else
				{
					if (!IsChild (GetFocus ()))
					{
						break;
					}
				}
			}

			if (NavigateRibbon (nChar))
			{
				return TRUE;
			}

		default:
			if (ProcessKey (nChar))
			{
				return TRUE;
			}
		}
	}

	return CBCGPControlBar::PreTranslateMessage(pMsg);
}
//**************************************************************************
LRESULT CBCGPRibbonBar::OnBCGUpdateToolTips (WPARAM wp, LPARAM)
{
	UINT nTypes = (UINT) wp;

	if (nTypes & BCGP_TOOLTIP_TYPE_RIBBON)
	{
		CBCGPTooltipManager::CreateToolTip (m_pToolTip, this,
			BCGP_TOOLTIP_TYPE_RIBBON);

		CRect rectDummy (0, 0, 0, 0);

		m_pToolTip->SetMaxTipWidth (nTooltipMaxWidth);
		
		m_pToolTip->AddTool (this, LPSTR_TEXTCALLBACK, &rectDummy, idToolTipClient);
		m_pToolTip->AddTool (this, LPSTR_TEXTCALLBACK, &rectDummy, idToolTipCaption);

		UpdateToolTipsRect ();
	}

	return 0;
}
//**************************************************************************
void CBCGPRibbonBar::PopTooltip ()
{
	ASSERT_VALID (this);

	if (m_pToolTip->GetSafeHwnd () != NULL)
	{
		m_pToolTip->Pop ();
	}
}
//**************************************************************************
BOOL CBCGPRibbonBar::DrawMenuImage (CDC* pDC, const CBCGPToolbarMenuButton* pMenuItem, 
									const CRect& rectImage)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);
	ASSERT_VALID (pMenuItem);

	UINT uiCmdID = pMenuItem->m_nID;
	if (uiCmdID == 0)
	{
		return FALSE;
	}

	if (uiCmdID == idCut)
	{
		uiCmdID = ID_EDIT_CUT;
	}

	if (uiCmdID == idCopy)
	{
		uiCmdID = ID_EDIT_COPY;
	}

	if (uiCmdID == idPaste)
	{
		uiCmdID = ID_EDIT_PASTE;
	}

	if (uiCmdID == idSelectAll)
	{
		uiCmdID = ID_EDIT_SELECT_ALL;
	}

	CBCGPBaseRibbonElement* pElem = FindByID (uiCmdID, FALSE, TRUE);
	if (pElem == NULL)
	{
		return FALSE;
	}

	ASSERT_VALID (pElem);

	BOOL bIsRibbonImageScale = globalData.IsRibbonImageScaleEnabled ();
	globalData.EnableRibbonImageScale (FALSE);

	const CSize sizeElemImage = pElem->GetImageSize (CBCGPRibbonButton::RibbonImageSmall);
	
	if (sizeElemImage == CSize (0, 0) ||
		sizeElemImage.cx > rectImage.Width () ||
		sizeElemImage.cy > rectImage.Height ())
	{
		globalData.EnableRibbonImageScale (bIsRibbonImageScale);
		return FALSE;
	}

	int dx = (rectImage.Width () - sizeElemImage.cx) / 2;
	int dy = (rectImage.Height () - sizeElemImage.cy) / 2;

	CRect rectDraw = rectImage;
	rectDraw.DeflateRect (dx, dy);

	BOOL bWasDisabled = pElem->IsDisabled ();
	BOOL bWasChecked = pElem->IsChecked ();

	pElem->m_bIsDisabled = pMenuItem->m_nStyle & TBBS_DISABLED;
	pElem->m_bIsChecked = pMenuItem->m_nStyle & TBBS_CHECKED;

	BOOL bRes = pElem->OnDrawMenuImage (pDC, rectDraw);

	pElem->m_bIsDisabled = bWasDisabled;
	pElem->m_bIsChecked = bWasChecked;

	globalData.EnableRibbonImageScale (bIsRibbonImageScale);
	return bRes;
}
//***********************************************************************************
HICON CBCGPRibbonBar::ExportImageToIcon(UINT uiCmdID, BOOL bIsLargeIcon)
{
	CBCGPBaseRibbonElement* pElem = FindByID (uiCmdID, FALSE, TRUE);
	if (pElem == NULL)
	{
		return NULL;
	}

	ASSERT_VALID(pElem);
	return pElem->ExportImageToIcon(bIsLargeIcon);
}
//***********************************************************************************
void CBCGPRibbonBar::ShowSysMenu (const CPoint& point)
{
	ASSERT_VALID (this);

	CWnd* pParentWnd = GetParent ();

	if (pParentWnd->GetSafeHwnd () != NULL)
	{
		CMenu* pMenu = pParentWnd->GetSystemMenu (FALSE);
		if (pMenu->GetSafeHmenu () == NULL)
		{
			return;
		}

		pMenu->SetDefaultItem (SC_CLOSE);

		if (GetParent ()->IsZoomed ())
		{
			pMenu->EnableMenuItem (SC_SIZE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			pMenu->EnableMenuItem (SC_MOVE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			pMenu->EnableMenuItem (SC_MAXIMIZE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

			pMenu->EnableMenuItem (SC_RESTORE, MF_BYCOMMAND | MF_ENABLED);
		}
		else
		{
			pMenu->EnableMenuItem (SC_RESTORE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

			pMenu->EnableMenuItem (SC_SIZE, MF_BYCOMMAND | MF_ENABLED);
			pMenu->EnableMenuItem (SC_MOVE, MF_BYCOMMAND | MF_ENABLED);
			pMenu->EnableMenuItem (SC_MAXIMIZE, MF_BYCOMMAND | MF_ENABLED);
		}

		if ((GetParent ()->GetStyle () & WS_MAXIMIZEBOX) == 0)
		{
			pMenu->DeleteMenu (SC_RESTORE, MF_BYCOMMAND);
			pMenu->DeleteMenu (SC_MAXIMIZE, MF_BYCOMMAND);
		}

		if ((GetParent ()->GetStyle () & WS_MINIMIZEBOX) == 0)
		{
			pMenu->DeleteMenu (SC_MINIMIZE, MF_BYCOMMAND);
		}

		if (g_pContextMenuManager != NULL)
		{
			g_pContextMenuManager->ShowPopupMenu
				(pMenu->GetSafeHmenu(), point.x, point.y, GetParent (), TRUE, TRUE, FALSE);
		}
		else
		{
			::TrackPopupMenu 
				(pMenu->GetSafeHmenu(), TPM_CENTERALIGN | TPM_LEFTBUTTON, 
				point.x, point.y, 0, GetOwner ()->GetSafeHwnd (), NULL);
		}
	}
}
//*****************************************************************************
void CBCGPRibbonBar::OnControlBarContextMenu (CWnd* pParentFrame, CPoint point)
{
	if (m_bDlgBarMode)
	{
		CBCGPControlBar::OnControlBarContextMenu (pParentFrame, point);
		return;
	}

	if (point == CPoint (-1, -1))
	{
		CBCGPBaseRibbonElement* pFocused = GetFocused ();
		if (pFocused != NULL)
		{
			ASSERT_VALID (pFocused);

			CRect rectFocus = pFocused->GetRect ();
			ClientToScreen (&rectFocus);

			OnShowRibbonContextMenu (this, rectFocus.left, rectFocus.top, pFocused);

			CFrameWnd* pParentFrame = GetParentFrame ();
			ASSERT_VALID (pParentFrame);

			pParentFrame->SetFocus ();
			return;
		}
	}

	DeactivateKeyboardFocus ();

	CPoint ptClient = point;
	ScreenToClient (&ptClient);

	CBCGPBaseRibbonElement* pHit = HitTest (ptClient, TRUE, TRUE);

	if (pHit != NULL && pHit->IsKindOf (RUNTIME_CLASS (CBCGPRibbonContextCaption)))
	{
		pHit->OnLButtonUp (point);
		return;
	}

	if (m_rectCaption.PtInRect (ptClient) && pHit == NULL)
	{
		ShowSysMenu (point);
		return;
	}

	OnShowRibbonContextMenu (this, point.x, point.y, pHit);
}
//*****************************************************************************
BOOL CBCGPRibbonBar::OnShowRibbonContextMenu (CWnd* pWnd, 
	int x, int y, CBCGPBaseRibbonElement* pHit)
{
	DeactivateKeyboardFocus ();

	ASSERT_VALID (this);
	ASSERT_VALID (pWnd);

	if (m_bAutoCommandTimer)
	{
		KillTimer (IdAutoCommand);
		m_bAutoCommandTimer = FALSE;
	}

	if ((GetAsyncKeyState (VK_LBUTTON) & 0x8000) != 0 &&
		(GetAsyncKeyState (VK_RBUTTON) & 0x8000) != 0)	// Both left and right mouse buttons are pressed
	{
		return FALSE;
	}

	if (g_pContextMenuManager == NULL)
	{
		return FALSE;
	}

	if (m_bDlgBarMode)
	{
		return FALSE;
	}

	if (pHit != NULL)
	{
		ASSERT_VALID (pHit);

		if (!pHit->IsHighlighted ())
		{
			pHit->m_bIsHighlighted = TRUE;
			pHit->Redraw ();
		}
	}

	CBCGPPopupMenu* pPopupMenu = DYNAMIC_DOWNCAST (CBCGPPopupMenu, pWnd->GetParent ());

	CFrameWnd* pParentFrame = GetParentFrame ();
	ASSERT_VALID (pParentFrame);

	const UINT idCustomize		= (UINT) -102;
	const UINT idQATOnBottom	= (UINT) -103;
	const UINT idQATOnTop		= (UINT) -104;
	const UINT idAddToQAT		= (UINT) -105;
	const UINT idRemoveFromQAT	= (UINT) -106;
	const UINT idMinimize		= (UINT) -107;
	const UINT idRestore		= (UINT) -108;

	CMenu menu;
	menu.CreatePopupMenu ();

	BOOL bIsGalleryMenu = FALSE;

	{
		CBCGPLocalResource locaRes;

		CString strItem;

		if (m_bIsCustomizeMenu)
		{
			strItem.LoadString (IDS_BCGBARRES_CUSTOMIZE_QAT_TOOLTIP);
			menu.AppendMenu (MF_STRING, (UINT) nBCGPMenuGroupID, strItem);
			
			for (int i = 0; i < m_QAToolbar.m_DefaultState.m_arCommands.GetSize (); i++)
			{
				const UINT uiCmd = m_QAToolbar.m_DefaultState.m_arCommands [i];

				CBCGPBaseRibbonElement* pElement = FindByID (uiCmd, FALSE);
				if (uiCmd != 0 && pElement != NULL)
				{
					ASSERT_VALID (pElement);

					strItem = pElement->GetText ();

					if (strItem.IsEmpty ())
					{
						pElement->UpdateTooltipInfo ();
						strItem = pElement->GetToolTipText ();
					}

					int uiMenuCmd = - ((int) uiCmd);

					menu.AppendMenu (MF_STRING, uiMenuCmd, strItem);

					if (m_QAToolbar.FindByID (uiCmd) != NULL)
					{
						menu.CheckMenuItem (uiMenuCmd, MF_CHECKED);
					}
				}
			}
		}
		else if (pHit != NULL && m_QAToolbar.GetCount () > 0)
		{
			ASSERT_VALID (pHit);

			UINT nID = pHit->GetQATID ();

			if (pHit->m_bQuickAccessMode)
			{
				strItem.LoadString (IDS_BCGBARRES_REMOVE_FROM_QAT);
				menu.AppendMenu (MF_STRING, idRemoveFromQAT, strItem);
			}
			else if (pHit->CanBeAddedToQAT ())
			{
				CBCGPRibbonPaletteIcon* pGalleryIcon = DYNAMIC_DOWNCAST (CBCGPRibbonPaletteIcon, pHit);
				if (pGalleryIcon != NULL)
				{
					bIsGalleryMenu = TRUE;

					CBCGPRibbonPaletteButton* pGallery = pGalleryIcon->GetOwner();
					if (pGallery != NULL && pGallery->IsSearchResultMode())
					{
						return FALSE;
					}

					if (!OnBeforeShowPaletteContextMenu (pHit, menu))
					{
						return FALSE;
					}

					if (menu.GetMenuItemCount () > 0)
					{
						menu.AppendMenu (MF_SEPARATOR);
					}
				}

				strItem.LoadString (bIsGalleryMenu ? IDS_BCGBARRES_ADD_GALLERY_TO_QAT : IDS_BCGBARRES_ADD_TO_QAT);

				menu.AppendMenu (MF_STRING, idAddToQAT, strItem);

				if (m_QAToolbar.FindByID (nID) != NULL)
				{
					// Already on QAT, disable this item
					menu.EnableMenuItem (idAddToQAT, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
				}
			}
		}

		if (!bIsGalleryMenu)
		{
			if (menu.GetMenuItemCount () > 0)
			{
				menu.AppendMenu (MF_SEPARATOR);
			}

			if (m_QAToolbar.GetCount () > 0)
			{
				strItem.LoadString (m_bIsCustomizeMenu ?
					IDS_BCGBARRES_MORE_COMMANDS : IDS_BCGBARRES_CUSTOMIZE_QAT);

				menu.AppendMenu (MF_STRING, idCustomize, strItem);

				if (IsQuickAccessToolbarOnTop ())
				{
					strItem.LoadString (m_bIsCustomizeMenu ?
						IDS_BCGBARRES_PLACE_BELOW_RIBBON : IDS_BCGBARRES_PLACE_QAT_BELOW_RIBBON);
					menu.AppendMenu (MF_STRING, idQATOnBottom,	strItem);
				}
				else
				{
					strItem.LoadString (m_bIsCustomizeMenu ?
						IDS_BCGBARRES_PLACE_ABOVE_RIBBON : IDS_BCGBARRES_PLACE_QAT_ABOVE_RIBBON);
					menu.AppendMenu (MF_STRING, idQATOnTop,	strItem);
				}

				menu.AppendMenu (MF_SEPARATOR);
			}

			if (m_dwHideFlags == BCGPRIBBONBAR_HIDE_ELEMENTS)
			{
				strItem.LoadString (IDS_BCGBARRES_MINIMIZE_RIBBON);
				menu.AppendMenu (MF_STRING, idRestore,	strItem);
				menu.CheckMenuItem (idRestore, MF_CHECKED);
			}
			else
			{
				strItem.LoadString (IDS_BCGBARRES_MINIMIZE_RIBBON);
				menu.AppendMenu (MF_STRING, idMinimize,	strItem);
			}
		}
	}

	HWND hwndThis = pWnd->GetSafeHwnd ();

	if (pPopupMenu != NULL)
	{
		g_pContextMenuManager->SetDontCloseActiveMenu ();
	}

	OnBeforeShowContextMenu(menu);

	int nMenuResult = g_pContextMenuManager->TrackPopupMenu (
			menu, x, y, pWnd);

	if (pPopupMenu != NULL)
	{
		g_pContextMenuManager->SetDontCloseActiveMenu (FALSE);
	}

	if (!::IsWindow (hwndThis))
	{
		return FALSE;
	}

	if (pHit != NULL)
	{
		ASSERT_VALID (pHit);
		
		pHit->m_bIsHighlighted = FALSE;

		CBCGPBaseRibbonElement* pDroppedDown = pHit->GetDroppedDown ();

		if (pDroppedDown != NULL)
		{
			ASSERT_VALID (pDroppedDown);

			pDroppedDown->ClosePopupMenu ();
			pHit->m_bIsDroppedDown = FALSE;
		}

		pHit->Redraw ();
	}

	BOOL bRecalcLayout = FALSE;

	switch (nMenuResult)
	{
	case idCustomize:
		{
			if (pHit == m_pHighlighted)
			{
				m_pHighlighted = NULL;
			}

			if (pHit == m_pPressed)
			{
				m_pPressed = NULL;
			}

			if (pPopupMenu != NULL)
			{
				pPopupMenu->SendMessage (WM_CLOSE);
			}

			if (pParentFrame->SendMessage (BCGM_ON_RIBBON_CUSTOMIZE, 0, (LPARAM)this) == 0)
			{
				CBCGPRibbonCustomize* pDlg = new CBCGPRibbonCustomize (pParentFrame, this);
				pDlg->DoModal ();
				delete pDlg;
			}

			return TRUE;
		}
		break;

	case idAddToQAT:
		{
			ASSERT_VALID (pHit);

			if (pHit->m_bIsDefaultMenuLook)
			{
				CBCGPBaseRibbonElement* pElem = FindByID (pHit->GetID (), FALSE);
				if (pElem != NULL)
				{
					ASSERT_VALID (pElem);
					pHit = pElem;
				}
			}

			bRecalcLayout = pHit->OnAddToQAToolbar (m_QAToolbar);

		}
		break;

	case idRemoveFromQAT:
		ASSERT_VALID (pHit);

		if (pHit == m_pHighlighted)
		{
			m_pHighlighted = NULL;
		}

		if (pHit == m_pPressed)
		{
			m_pPressed = NULL;
		}

		m_QAToolbar.Remove (pHit);
		bRecalcLayout = TRUE;
		break;

	case idQATOnBottom:
		SetQuickAccessToolbarOnTop (FALSE);
		bRecalcLayout = TRUE;
		break;

	case idQATOnTop:
		SetQuickAccessToolbarOnTop (TRUE);
		bRecalcLayout = TRUE;
		break;

	case idMinimize:
		if (m_pActiveCategory != NULL)
		{
			ASSERT_VALID (m_pActiveCategory);
			m_pActiveCategory->ShowElements (FALSE);
			RedrawWindow ();
		}
		break;

	case idRestore:
		if (m_pActiveCategory != NULL)
		{
			ASSERT_VALID (m_pActiveCategory);
			m_pActiveCategory->ShowElements ();
			RedrawWindow ();
		}
		break;

	default:
		if (m_bIsCustomizeMenu)
		{
			UINT uiCmd = -nMenuResult;
			
			if (uiCmd != 0)
			{
				CBCGPBaseRibbonElement* pElement = FindByID (uiCmd, FALSE);
				if (pElement != NULL)
				{
					ASSERT_VALID (pElement);

					if (m_QAToolbar.FindByID (uiCmd) != NULL)
					{
						m_QAToolbar.Remove (pElement);
					}
					else
					{
						m_QAToolbar.Add (pElement);
					}

					bRecalcLayout = TRUE;
					break;
				}
			}
		}
		else if (bIsGalleryMenu && nMenuResult != 0)
		{
			if (pParentFrame->GetSafeHwnd () != NULL)
			{
				if (pPopupMenu != NULL)
				{
					pPopupMenu->SendMessage (WM_CLOSE);
					pPopupMenu = NULL;
				}

				pParentFrame->PostMessage (WM_COMMAND, nMenuResult);
			}
		}
		
		if (pPopupMenu != NULL)
		{
			CBCGPPopupMenu::m_pActivePopupMenu = pPopupMenu;
		}

		return FALSE;
	}

	if (pPopupMenu != NULL)
	{
		pPopupMenu->SendMessage (WM_CLOSE);
	}

	if (bRecalcLayout)
	{
		m_bForceRedraw = TRUE;
		RecalcLayout ();

		if (pParentFrame->GetSafeHwnd () != NULL)
		{
			pParentFrame->RecalcLayout ();
			pParentFrame->RedrawWindow (NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
		}
	}

	return TRUE;
}
//***********************************************************************************
BOOL CBCGPRibbonBar::OnBeforeShowPaletteContextMenu (const CBCGPBaseRibbonElement* pHit, CMenu& menu)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pHit);

	int nIconNumber = 0;

	CBCGPRibbonPaletteIcon* pIcon = DYNAMIC_DOWNCAST (CBCGPRibbonPaletteIcon, pHit);
	if (pIcon != NULL)
	{
		ASSERT_VALID (pIcon);
		nIconNumber = pIcon->GetIndex ();
	}

	CFrameWnd* pParentFrame = GetParentFrame ();
	if (pParentFrame == NULL)
	{
		return TRUE;
	}

	ASSERT_VALID (pParentFrame);
				
	return pParentFrame->SendMessage (BCGM_ON_BEFORE_SHOW_PALETTE_CONTEXTMENU, 
		(WPARAM)menu.GetSafeHmenu (), (LPARAM)pHit) == 0;
}
//***********************************************************************************
BOOL CBCGPRibbonBar::OnShowRibbonQATMenu (CWnd* pWnd, int x, int y, CBCGPBaseRibbonElement* pHit)
{
	ASSERT_VALID (this);

	BOOL bIsCustomizeMenu = m_bIsCustomizeMenu;
	m_bIsCustomizeMenu = TRUE;

	BOOL bRes = OnShowRibbonContextMenu (pWnd, x, y, pHit);
	
	m_bIsCustomizeMenu = bIsCustomizeMenu;

	return bRes;
}
//***********************************************************************************
void CBCGPRibbonBar::OnSizing(UINT fwSide, LPRECT pRect) 
{
	if (CBCGPPopupMenu::GetActiveMenu () != NULL)
	{
		CBCGPPopupMenu::GetActiveMenu ()->SendMessage (WM_CLOSE);
	}

	CBCGPControlBar::OnSizing(fwSide, pRect);
}
//*******************************************************************************************
BOOL CBCGPRibbonBar::SaveState (LPCTSTR lpszProfileName, int nIndex, UINT uiID)
{
	CString strProfileName = ::BCGPGetRegPath (strRibbonProfile, lpszProfileName);

	BOOL bResult = FALSE;

	if (nIndex == -1)
	{
		nIndex = GetDlgCtrlID ();
	}

	CString strSection;
	if (uiID == (UINT) -1)
	{
		strSection.Format (REG_SECTION_FMT, strProfileName, nIndex);
	}
	else
	{
		strSection.Format (REG_SECTION_FMT_EX, strProfileName, nIndex, uiID);
	}

	CBCGPRegistrySP regSP;
	CBCGPRegistry& reg = regSP.Create (FALSE, FALSE);

	if (reg.CreateKey (strSection))
	{
		reg.Write (REG_ENTRY_QA_TOOLBAR_LOCATION, m_bQuickAccessToolbarOnTop);

		CList<UINT,UINT> lstCommands;
		GetQuickAccessCommands (lstCommands);

		reg.Write (REG_ENTRY_QA_TOOLBAR_COMMANDS, lstCommands);

		reg.Write (REG_ENTRY_RIBBON_IS_MINIMIZED, (m_dwHideFlags & BCGPRIBBONBAR_HIDE_ELEMENTS) != 0);
	}

	bResult = CBCGPControlBar::SaveState (lpszProfileName, nIndex, uiID);

	return bResult;
}
//*********************************************************************
BOOL CBCGPRibbonBar::LoadState (LPCTSTR lpszProfileName, int nIndex, UINT uiID)
{
	CString strProfileName = ::BCGPGetRegPath (strRibbonProfile, lpszProfileName);

	if (nIndex == -1)
	{
		nIndex = GetDlgCtrlID ();
	}

	CString strSection;
	if (uiID == (UINT) -1)
	{
		strSection.Format (REG_SECTION_FMT, strProfileName, nIndex);
	}
	else
	{
		strSection.Format (REG_SECTION_FMT_EX, strProfileName, nIndex, uiID);
	}

	CBCGPRegistrySP regSP;
	CBCGPRegistry& reg = regSP.Create (FALSE, TRUE);

	if (!reg.Open (strSection))
	{
		return FALSE;
	}

	reg.Read (REG_ENTRY_QA_TOOLBAR_LOCATION, m_bQuickAccessToolbarOnTop);
	if (!m_bQuickAccessToolbarOnTop && m_bReplaceFrameCaption)
	{
		m_nCaptionHeight = GetSystemMetrics (SM_CYCAPTION) + 1;

		if (globalData.DwmIsCompositionEnabled ())
		{
			m_nCaptionHeight += GetSystemMetrics (SM_CYSIZEFRAME);
		}
	}

	CList<UINT,UINT> lstCommands;
	reg.Read (REG_ENTRY_QA_TOOLBAR_COMMANDS, lstCommands);

	m_QAToolbar.SetCommands (this, lstCommands, (CBCGPRibbonQuickAccessCustomizeButton*) NULL);

	BOOL bIsMinimized = FALSE;
	reg.Read (REG_ENTRY_RIBBON_IS_MINIMIZED, bIsMinimized);

	if (bIsMinimized)
	{
		m_dwHideFlags |= BCGPRIBBONBAR_HIDE_ELEMENTS;

		if (m_pActiveCategory != NULL)
		{
			ASSERT_VALID (m_pActiveCategory);
			m_pActiveCategory->ShowElements (FALSE);
		}
	}

	RecalcLayout ();

	return CBCGPControlBar::LoadState (lpszProfileName, nIndex, uiID);
}
//****************************************************************************
void CBCGPRibbonBar::OnSettingChange(UINT uFlags, LPCTSTR lpszSection) 
{
	CBCGPControlBar::OnSettingChange(uFlags, lpszSection);

	if (uFlags == SPI_SETNONCLIENTMETRICS ||
		uFlags == SPI_SETWORKAREA ||
		uFlags == SPI_SETICONTITLELOGFONT)
	{
		ForceRecalcLayout ();

		CFrameWnd* pParentFrame = GetParentFrame ();

		if (pParentFrame->GetSafeHwnd () != NULL && pParentFrame->IsZoomed() &&
			globalData.GetShellAutohideBars () != 0)
		{
			pParentFrame->ShowWindow(SW_RESTORE);
			pParentFrame->ShowWindow(SW_MAXIMIZE);
		}
	}
}
//**************************************************************************
void CBCGPRibbonBar::ForceRecalcLayout ()
{
	m_bRecalcCategoryHeight = TRUE;
	m_bRecalcCategoryWidth = TRUE;

	if (m_pMainCategory != NULL)
	{
		ASSERT_VALID (m_pMainCategory);
		m_pMainCategory->CleanUpSizes ();
	}

	if (m_pBackstageCategory != NULL)
	{
		ASSERT_VALID (m_pBackstageCategory);
		m_pBackstageCategory->CleanUpSizes ();
	}

	for (int i = 0; i < m_arCategories.GetSize (); i++)
	{
		CBCGPRibbonCategory* pCategory = m_arCategories [i];
		ASSERT_VALID (pCategory);

		pCategory->CleanUpSizes ();
	}

	globalData.UpdateFonts ();

	m_bForceRedraw = TRUE;
	RecalcLayout ();

	CFrameWnd* pParentFrame = GetParentFrame ();
	if (pParentFrame->GetSafeHwnd () != NULL)
	{
		pParentFrame->RecalcLayout ();
		pParentFrame->RedrawWindow (NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
	}
}
//**************************************************************************
void CBCGPRibbonBar::SetMaximizeMode (BOOL bMax, CWnd* pWnd)
{
	ASSERT_VALID (this);

	if (m_bMaximizeMode == bMax)
	{
		return;
	}

	for (int i = 0; i < m_nSystemButtonsNum; i++)
	{
		int nSize = (int) m_TabElements.m_arButtons.GetSize ();

		delete m_TabElements.m_arButtons [nSize - 1];
		m_TabElements.m_arButtons.SetSize (nSize - 1);
	}

	m_nSystemButtonsNum = 0;

	if (bMax)
	{
		ASSERT_VALID (pWnd);

		CFrameWnd* pFrameWnd = DYNAMIC_DOWNCAST (CFrameWnd, pWnd);
		BOOL bIsOleContainer = pFrameWnd != NULL && pFrameWnd->m_pNotifyHook != NULL;

		HMENU hSysMenu = NULL;

		CMenu* pMenu = pWnd->GetSystemMenu (FALSE);
		if (pMenu != NULL && ::IsMenu (pMenu->m_hMenu))
		{
			hSysMenu = pMenu->GetSafeHmenu ();
			if (!::IsMenu (hSysMenu) || 
				(pWnd->GetStyle () & WS_SYSMENU) == 0 && !bIsOleContainer)
			{
				hSysMenu = NULL;
			}
		}

        LONG style = ::GetWindowLong(*pWnd, GWL_STYLE);

		if (hSysMenu != NULL)
		{
			// Add a minimize box if required.
			if (style & WS_MINIMIZEBOX)
			{
				m_TabElements.AddButton (new CBCGPRibbonCaptionButton (SC_MINIMIZE, pWnd->GetSafeHwnd ()));
				m_nSystemButtonsNum++;
			}

			// Add a restore box if required.
			if (style & WS_MAXIMIZEBOX)
			{
				m_TabElements.AddButton (new CBCGPRibbonCaptionButton (SC_RESTORE, pWnd->GetSafeHwnd ()));
				m_nSystemButtonsNum++;
			}

			// Add a close box:
			CBCGPRibbonCaptionButton* pBtnClose = 
				new CBCGPRibbonCaptionButton (SC_CLOSE, pWnd->GetSafeHwnd ());

			if (hSysMenu != NULL)
			{
				MENUITEMINFO menuInfo;
				ZeroMemory(&menuInfo,sizeof(MENUITEMINFO));
				menuInfo.cbSize = sizeof(MENUITEMINFO);
				menuInfo.fMask = MIIM_STATE;

				if (!::GetMenuItemInfo(hSysMenu, SC_CLOSE, FALSE, &menuInfo) ||
					(menuInfo.fState & MFS_GRAYED) || 
					(menuInfo.fState & MFS_DISABLED))
				{
					pBtnClose->m_bIsDisabled = TRUE;
				}
			}

			m_TabElements.AddButton (pBtnClose);
			m_nSystemButtonsNum++;
		}
	}

	m_bMaximizeMode = bMax;
	m_pHighlighted = NULL;
	m_pPressed = NULL;

	RecalcLayout ();
	RedrawWindow ();
}
//*************************************************************************
void CBCGPRibbonBar::SetActiveMDIChild (CWnd* pWnd)
{
	ASSERT_VALID (this);

	for (int i = 0; i < m_TabElements.m_arButtons.GetSize (); i++)
	{
		CBCGPRibbonCaptionButton* pCaptionButton = DYNAMIC_DOWNCAST (
			CBCGPRibbonCaptionButton,
			m_TabElements.m_arButtons [i]);

		if (pCaptionButton != NULL)
		{
			ASSERT_VALID (pCaptionButton);
			pCaptionButton->m_hwndMDIChild = pWnd->GetSafeHwnd ();
		}
	}
}
//***************************************************************************
void CBCGPRibbonBar::OnTimer(UINT_PTR nIDEvent) 
{
	if (nIDEvent == IdAutoCommand)
	{
		if (m_pPressed != NULL)
		{
			ASSERT_VALID (m_pPressed);

			CPoint point;
			
			::GetCursorPos (&point);
			ScreenToClient (&point);

			if (m_pPressed->GetRect ().PtInRect (point))
			{
				if (!m_pPressed->OnAutoRepeat ())
				{
					KillTimer (IdAutoCommand);
				}
			}
		}
	}

	if (nIDEvent == IdShowKeyTips)
	{
		SetKeyboardNavigationLevel (NULL, FALSE);
		KillTimer (IdShowKeyTips);
	}
	
	CBCGPControlBar::OnTimer(nIDEvent);
}
//***************************************************************************
void CBCGPRibbonBar::SetPrintPreviewMode (BOOL bSet)
{
	ASSERT_VALID (this);

	if (!m_bIsPrintPreview)
	{
		return;
	}

	m_bPrintPreviewMode = bSet;

	if (bSet)
	{
		ASSERT_VALID (m_pPrintPreviewCategory);

		OnSetPrintPreviewKeys (
			m_pPrintPreviewCategory->GetPanel (0),
			m_pPrintPreviewCategory->GetPanel (1),
			m_pPrintPreviewCategory->GetPanel (2));

		m_arVisibleCategoriesSaved.RemoveAll ();

		for (int i = 0; i < m_arCategories.GetSize (); i++)
		{
			CBCGPRibbonCategory* pCategory = m_arCategories [i];
			ASSERT_VALID (pCategory);

			if (pCategory->IsVisible ())
			{
				m_arVisibleCategoriesSaved.Add (i);
				pCategory->m_bIsVisible = FALSE;
			}
		}

		m_pPrintPreviewCategory->m_bIsVisible = TRUE;

		if (m_pActiveCategory != NULL)
		{
			m_pActiveCategory->SetActive (FALSE);
		}

		m_pActiveCategorySaved = m_pActiveCategory;
		m_pActiveCategory = m_pPrintPreviewCategory;

		m_pActiveCategory->SetActive ();
	}
	else
	{
		if (m_pActiveCategory != NULL)
		{
			m_pActiveCategory->SetActive (FALSE);
		}

		for (int i = 0; i < m_arVisibleCategoriesSaved.GetSize (); i++)
		{
			ShowCategory (m_arVisibleCategoriesSaved [i]);
		}

		m_arVisibleCategoriesSaved.RemoveAll ();

		m_pPrintPreviewCategory->m_bIsVisible = FALSE;

		m_pActiveCategory = m_pActiveCategorySaved;

		if (m_pActiveCategory != NULL)
		{
			m_pActiveCategory->SetActive ();
		}
	}

	RecalcLayout ();
	RedrawWindow ();
}
//***************************************************************************
void CBCGPRibbonBar::EnablePrintPreview (BOOL bEnable)
{
	ASSERT_VALID (this);
	m_bIsPrintPreview = bEnable;

	if (!bEnable && m_pPrintPreviewCategory != NULL)
	{
		ASSERT_VALID (m_pPrintPreviewCategory);
		
		RemoveCategory (GetCategoryIndex (m_pPrintPreviewCategory));
		m_pPrintPreviewCategory = NULL;
	}
}
//***************************************************************************
void CBCGPRibbonBar::EnableMinimizeButton (BOOL bEnable)
{
	ASSERT_VALID (this);

	if (m_bHasMinimizeButton == bEnable)
	{
		return;
	}

	m_bHasMinimizeButton = bEnable;

	if (m_bHasMinimizeButton)
	{
		m_TabElements.m_arButtons.InsertAt (0, new CBCGPRibbonMinimizeButton(*this));
	}
	else
	{
		ASSERT(m_TabElements.m_arButtons[0]->IsKindOf(RUNTIME_CLASS(CBCGPRibbonMinimizeButton)));
		delete m_TabElements.m_arButtons[0];
		m_TabElements.m_arButtons.RemoveAt (0);
	}

	RecalcLayout ();
}
//***************************************************************************
static CString LoadCommandLabel (UINT uiCommdnID)
{
	TCHAR szFullText [256];
	CString strText;

	if (AfxLoadString (uiCommdnID, szFullText))
	{
		AfxExtractSubString (strText, szFullText, 1, '\n');
	}

	strText.Remove (_T('&'));
	return strText;
}
//***************************************************************************
CBCGPRibbonCategory* CBCGPRibbonBar::AddPrintPreviewCategory ()
{
	ASSERT_VALID (this);

	if (!m_bIsPrintPreview)
	{
		return NULL;
	}

	ASSERT (m_pPrintPreviewCategory == NULL);

	CBCGPLocalResource locaRes;

	const int nTwoPagesImageSmall	= 1;
	const int nNextPageImageSmall	= 2;
	const int nPrevPageImageSmall	= 3;
	const int nPrintImageSmall		= 4;
	const int nCloseImageSmall		= 5;
	const int nZoomInImageSmall		= 6;
	const int nZoomOutImageSmall	= 7;

	const int nPrintImageLarge		= 0;
	const int nZoomInImageLarge		= 1;
	const int nZoomOutImageLarge	= 2;
	const int nCloseImagLarge		= 3;
	const int nOnePageImageLarge	= 4;

	CString strLabel;
	strLabel.LoadString (IDS_BCGBARRES_PRINT_PREVIEW);
	
	m_pPrintPreviewCategory = new CBCGPRibbonCategory (
		this, strLabel, IDB_BCGBARRES_RIBBON_PRINT_SMALL, IDB_BCGBARRES_RIBBON_PRINT_LARGE);

	m_pPrintPreviewCategory->m_bIsVisible = FALSE;

	strLabel.LoadString (IDS_BCGBARRES_PRINT);

	CBCGPRibbonPanel* pPrintPanel = m_pPrintPreviewCategory->AddPanel (strLabel,
		m_pPrintPreviewCategory->GetSmallImages ().ExtractIcon (nPrintImageSmall));
	ASSERT_VALID (pPrintPanel);

	pPrintPanel->Add (new CBCGPRibbonButton (AFX_ID_PREVIEW_PRINT, 
		LoadCommandLabel (AFX_ID_PREVIEW_PRINT), nPrintImageSmall, nPrintImageLarge));

	strLabel.LoadString (IDS_BCGBARRES_ZOOM);

	CBCGPRibbonPanel* pZoomPanel = m_pPrintPreviewCategory->AddPanel (strLabel,
		m_pPrintPreviewCategory->GetSmallImages ().ExtractIcon (nZoomInImageSmall));
	ASSERT_VALID (pZoomPanel);

	pZoomPanel->Add (new CBCGPRibbonButton (AFX_ID_PREVIEW_ZOOMIN, 
		LoadCommandLabel (AFX_ID_PREVIEW_ZOOMIN), nZoomInImageSmall, nZoomInImageLarge));
	pZoomPanel->Add (new CBCGPRibbonButton (AFX_ID_PREVIEW_ZOOMOUT, 
		LoadCommandLabel (AFX_ID_PREVIEW_ZOOMOUT), nZoomOutImageSmall, nZoomOutImageLarge));

	CString str1;
	str1.LoadString (AFX_IDS_ONEPAGE);

	CString str2;
	str2.LoadString (AFX_IDS_TWOPAGE);

	CString strPages = str1.GetLength () > str2.GetLength () ? str1 : str2;

	pZoomPanel->Add (new CBCGPRibbonButton (AFX_ID_PREVIEW_NUMPAGE, 
		strPages, nTwoPagesImageSmall, nOnePageImageLarge));

	strLabel.LoadString (IDS_BCGBARRES_PREVIEW);

	CBCGPRibbonPanel* pPreviewPanel = m_pPrintPreviewCategory->AddPanel (strLabel,
		m_pPrintPreviewCategory->GetSmallImages ().ExtractIcon (nCloseImageSmall));
	ASSERT_VALID (pPreviewPanel);

	pPreviewPanel->Add (new CBCGPRibbonButton (AFX_ID_PREVIEW_NEXT, 
		LoadCommandLabel (AFX_ID_PREVIEW_NEXT), nNextPageImageSmall, -1));
	pPreviewPanel->Add (new CBCGPRibbonButton (AFX_ID_PREVIEW_PREV, 
		LoadCommandLabel (AFX_ID_PREVIEW_PREV), nPrevPageImageSmall, -1));
	pPreviewPanel->Add (new CBCGPRibbonSeparator);
	pPreviewPanel->Add (new CBCGPRibbonButton (AFX_ID_PREVIEW_CLOSE, 
		LoadCommandLabel (AFX_ID_PREVIEW_CLOSE), nCloseImageSmall, nCloseImagLarge));

	m_arCategories.Add (m_pPrintPreviewCategory);
	return m_pPrintPreviewCategory;
}
//***************************************************************************
void CBCGPRibbonBar::OnSetPrintPreviewKeys (
		CBCGPRibbonPanel* pPrintPanel,
		CBCGPRibbonPanel* pZoomPanel,
		CBCGPRibbonPanel* pPreviewPanel)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pPrintPanel);
	ASSERT_VALID (pZoomPanel);
	ASSERT_VALID (pPreviewPanel);

	pPrintPanel->SetKeys (_T("zp"));
	pZoomPanel->SetKeys (_T("zz"));
	pPreviewPanel->SetKeys (_T("zv"));

	SetElementKeys (AFX_ID_PREVIEW_NEXT,	_T("x"));
	SetElementKeys (AFX_ID_PREVIEW_PREV,	_T("v"));
	SetElementKeys (AFX_ID_PREVIEW_CLOSE,	_T("c"));
	SetElementKeys (AFX_ID_PREVIEW_ZOOMIN,	_T("qi"));
	SetElementKeys (AFX_ID_PREVIEW_ZOOMOUT,	_T("qo"));
	SetElementKeys (AFX_ID_PREVIEW_PRINT,	_T("p"));
	SetElementKeys (AFX_ID_PREVIEW_NUMPAGE,	_T("1"));
}
//***************************************************************************
CBCGPRibbonContextCaption* CBCGPRibbonBar::FindContextCaption (UINT uiID) const
{
	ASSERT_VALID (this);
	ASSERT (uiID != 0);

	for (int i = 0; i < m_arContextCaptions.GetSize (); i++)
	{
		ASSERT_VALID (m_arContextCaptions [i]);

		if (m_arContextCaptions [i]->m_uiID == uiID)
		{
			return m_arContextCaptions [i];
		}
	}

	return NULL;
}
//*******************************************************************************
CBCGPBaseRibbonElement* CBCGPRibbonBar::GetDroppedDown ()
{
	ASSERT_VALID (this);

	//---------------------------
	// Check for the main button:
	//---------------------------
	if (m_pMainButton != NULL)
	{
		ASSERT_VALID (m_pMainButton);

		if (m_pMainButton->GetDroppedDown () != NULL)
		{
			return m_pMainButton;
		}
	}

	//--------------------------------
	// Check for quick access toolbar:
	//--------------------------------
	CBCGPBaseRibbonElement* pQAElem = m_QAToolbar.GetDroppedDown ();
	if (pQAElem != NULL)
	{
		ASSERT_VALID (pQAElem);
		return pQAElem;
	}

	//------------------------
	// Check for tab elements:
	//------------------------
	CBCGPBaseRibbonElement* pTabElem = m_TabElements.GetDroppedDown ();
	if (pTabElem != NULL)
	{
		ASSERT_VALID (pTabElem);
		return pTabElem;
	}

	if (m_pActiveCategory != NULL)
	{
		ASSERT_VALID (m_pActiveCategory);

		if (m_dwHideFlags & BCGPRIBBONBAR_HIDE_ELEMENTS)
		{
			if (m_pActiveCategory->m_Tab.GetDroppedDown () != NULL)
			{
				ASSERT_VALID (m_pActiveCategory->m_Tab.GetDroppedDown ());
				return m_pActiveCategory->m_Tab.GetDroppedDown ();
			}
		}

		return m_pActiveCategory->GetDroppedDown ();
	}

	return NULL;
}
//***********************************************************************************************************
void CBCGPRibbonBar::OnSysColorChange() 
{
	CBCGPControlBar::OnSysColorChange();

	globalData.UpdateSysColors ();

	CBCGPVisualManager::GetInstance ()->OnUpdateSystemColors ();
	RedrawWindow (NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
}
//***********************************************************************************************************
BOOL CBCGPRibbonBar::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return TRUE;
}
//***********************************************************************************************************
LRESULT CBCGPRibbonBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (!m_bIsTransparentCaption)
	{
		return CBCGPControlBar::WindowProc(message, wParam, lParam);
	}

	if (message == WM_NCHITTEST)
	{
		LRESULT lResult = globalData.DwmDefWindowProc (
			GetParent ()->GetSafeHwnd (), message, wParam, lParam);

		if (lResult == HTCLOSE || lResult == HTMINBUTTON || lResult == HTMAXBUTTON)
		{
			return HTTRANSPARENT;
		}

		if (!GetParent ()->IsZoomed ())
		{
			CPoint point(BCG_GET_X_LPARAM(lParam), BCG_GET_Y_LPARAM(lParam));

			CRect rectResizeTop = m_rectCaption;
			rectResizeTop.right = m_rectSysButtons.left - 1;
			rectResizeTop.bottom = rectResizeTop.top + GetSystemMetrics (SM_CYSIZEFRAME) / 2;

			ClientToScreen (&rectResizeTop);

			if (rectResizeTop.PtInRect (point))
			{
				return HTTOP;
			}
		}
	}
	
	return CBCGPControlBar::WindowProc(message, wParam, lParam);
}
//***************************************************************************************************************
void CBCGPRibbonBar::OnSysCommand(UINT nID, LPARAM lParam) 
{
	if (m_bIsTransparentCaption)
	{
		if (nID == SC_MAXIMIZE && GetParent ()->IsZoomed ())
		{
			nID = SC_RESTORE;
		}

		GetParent ()->SendMessage (WM_SYSCOMMAND, (WPARAM) nID, lParam);
	}
	else
	{
		CBCGPControlBar::OnSysCommand (nID, lParam);
	}
}
//***************************************************************************************************************
BOOL CBCGPRibbonBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_bIsTransparentCaption && !GetParent ()->IsZoomed ())
	{
		CRect rectResizeTop = m_rectCaption;
		rectResizeTop.right = m_rectSysButtons.left - 1;
		rectResizeTop.bottom = rectResizeTop.top + GetSystemMetrics (SM_CYSIZEFRAME) / 2;

		ClientToScreen (&rectResizeTop);

		CPoint point;
		GetCursorPos (&point);

		if (rectResizeTop.PtInRect (point))
		{
			SetCursor (AfxGetApp ()->LoadStandardCursor (IDC_SIZENS));
			return TRUE;
		}
	}
	
	return CBCGPControlBar::OnSetCursor(pWnd, nHitTest, message);
}
//***************************************************************************************************************
void CBCGPRibbonBar::DWMCompositionChanged ()
{
	if (!m_bReplaceFrameCaption)
	{
		return;
	}

	DWORD dwStyle = WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZE;

	if (globalData.DwmIsCompositionEnabled ())
	{
		ModifyStyle (0, dwStyle, SWP_FRAMECHANGED);
		GetParent ()->ModifyStyle (0, WS_CAPTION);
		GetParent ()->SetWindowRgn (NULL, TRUE);
	}
	else
	{
		ModifyStyle (dwStyle, 0, SWP_FRAMECHANGED);
		GetParent ()->ModifyStyle (WS_CAPTION, 0);
	}

	GetParent ()->SetWindowPos (NULL, -1, -1, -1, -1, 
			SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	m_bForceRedraw = TRUE;
	RecalcLayout ();
}
//***************************************************************************************************************
void CBCGPRibbonBar::UpdateToolTipsRect ()
{
	if (m_pToolTip->GetSafeHwnd () != NULL)
	{
		CRect rectToolTipClient;
		GetClientRect (rectToolTipClient);

		CRect rectToolTipCaption (0, 0, 0, 0);

		if (m_bIsTransparentCaption)
		{
			rectToolTipClient.top = m_rectCaption.bottom + 1;
			rectToolTipCaption = m_rectCaption;
			rectToolTipCaption.right = m_rectSysButtons.left - 1;
		}

		m_pToolTip->SetToolRect (this, idToolTipClient, rectToolTipClient);
		m_pToolTip->SetToolRect (this, idToolTipCaption, rectToolTipCaption);
	}
}
//***************************************************************************************************************
void CBCGPRibbonBar::OnEditContextMenu (CBCGPRibbonEditCtrl* pEdit, CPoint point)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pEdit);

	if (g_pContextMenuManager == NULL)
	{
		return;
	}

	CString strItem;
	TCHAR szFullText [256];

	CMenu menu;
	menu.CreatePopupMenu ();

	AfxLoadString (ID_EDIT_CUT, szFullText);
	AfxExtractSubString (strItem, szFullText, 1, '\n');
	menu.AppendMenu (MF_STRING, idCut, strItem);

	AfxLoadString (ID_EDIT_COPY, szFullText);
	AfxExtractSubString (strItem, szFullText, 1, '\n');
	menu.AppendMenu (MF_STRING, idCopy, strItem);

	AfxLoadString (ID_EDIT_PASTE, szFullText);
	AfxExtractSubString (strItem, szFullText, 1, '\n');
	menu.AppendMenu (MF_STRING, idPaste, strItem);

	menu.AppendMenu (MF_SEPARATOR);

	AfxLoadString (ID_EDIT_SELECT_ALL, szFullText);
	AfxExtractSubString (strItem, szFullText, 1, '\n');
	menu.AppendMenu (MF_STRING, idSelectAll, strItem);

	if (!::IsClipboardFormatAvailable (_TCF_TEXT))
	{
		menu.EnableMenuItem (idPaste, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}

	long nStart, nEnd;
	pEdit->GetSel (nStart, nEnd);

	if (nEnd <= nStart)
	{
		menu.EnableMenuItem (idCopy, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		menu.EnableMenuItem (idCut, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}

	if (pEdit->GetWindowTextLength () == 0)
	{
		menu.EnableMenuItem (idSelectAll, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}

	BOOL bForceRTL = CBCGPPopupMenu::m_bForceRTL;
	if (GetExStyle() & WS_EX_LAYOUTRTL)
	{
		CBCGPPopupMenu::m_bForceRTL = TRUE;
	}

	int nMenuResult = g_pContextMenuManager->TrackPopupMenu (
			menu, point.x, point.y, pEdit);

	CBCGPPopupMenu::m_bForceRTL = bForceRTL;

	switch (nMenuResult)
	{
	case idCut:
		pEdit->Cut ();
		break;

	case idCopy:
		pEdit->Copy ();
		break;

	case idPaste:
		pEdit->Paste ();
		break;

	case idSelectAll:
		pEdit->SetSel (0, -1);
		break;
	}
}
//***************************************************************************************************************
void CBCGPRibbonBar::EnableToolTips (BOOL bEnable, BOOL bEnableDescr)
{
	ASSERT_VALID (this);

	m_bToolTip = bEnable;
	m_bToolTipDescr = bEnableDescr;
}
//***************************************************************************************************************
void CBCGPRibbonBar::SetTooltipFixedWidth (int nWidthRegular, int nWidthLargeImage)	// 0 - set variable size
{
	ASSERT_VALID (this);

	m_nTooltipWidthRegular = nWidthRegular;
	m_nTooltipWidthLargeImage = nWidthLargeImage;
}
//***************************************************************************************************************
void CBCGPRibbonBar::EnableKeyTips (BOOL bEnable)
{
	ASSERT_VALID (this);

	m_bKeyTips = bEnable;
}
//***************************************************************************************************************
void CBCGPRibbonBar::GetItemIDsList (CList<UINT,UINT>& lstItems, BOOL bHiddenOnly/* = FALSE*/) const
{
	ASSERT_VALID (this);

	lstItems.RemoveAll ();

	if (m_pMainCategory != NULL)
	{
		ASSERT_VALID (m_pMainCategory);

		m_pMainCategory->GetItemIDsList (lstItems, FALSE);
	}

	if (m_pBackstageCategory != NULL)
	{
		ASSERT_VALID (m_pBackstageCategory);

		m_pBackstageCategory->GetItemIDsList (lstItems, FALSE);
	}

	for (int i = 0; i < m_arCategories.GetSize (); i++)
	{
		CBCGPRibbonCategory* pCategory = m_arCategories [i];
		ASSERT_VALID (pCategory);

		pCategory->GetItemIDsList (lstItems, bHiddenOnly);
	}

	if (!bHiddenOnly)
	{
		m_QAToolbar.GetItemIDsList (lstItems);
		m_TabElements.GetItemIDsList (lstItems);
	}
}
//***************************************************************************************************************
void CBCGPRibbonBar::ToggleMimimizeState ()
{
	ASSERT_VALID (this);

	if ((m_dwHideFlags & BCGPRIBBONBAR_HIDE_ALL) != 0 || m_bBackstageViewActive)
	{
		return;
	}

	if (m_pActiveCategory != NULL)
	{
		ASSERT_VALID (m_pActiveCategory);

		const BOOL bIsHidden = m_dwHideFlags == BCGPRIBBONBAR_HIDE_ELEMENTS;
		
		m_pActiveCategory->ShowElements (bIsHidden);
		RedrawWindow ();
	}
}
//***************************************************************************************************************
void CBCGPRibbonBar::OnSetFocus(CWnd* pOldWnd) 
{
	CBCGPControlBar::OnSetFocus(pOldWnd);

	if (m_nKeyboardNavLevel < 0 && !m_bDontSetKeyTips)
	{
		SetKeyboardNavigationLevel (NULL, FALSE);
	}

	m_bDontSetKeyTips = FALSE;
}
//***************************************************************************************************************
void CBCGPRibbonBar::OnKillFocus(CWnd* pNewWnd) 
{
	CBCGPControlBar::OnKillFocus(pNewWnd);

	if (m_nKeyboardNavLevel >= 0)
	{
		m_nKeyboardNavLevel = -1;
		m_pKeyboardNavLevelParent = NULL;
		m_pKeyboardNavLevelCurrent = NULL;
		m_nCurrKeyChar = 0;

		RemoveAllKeys ();
		RedrawWindow ();
	}

	if (!IsChild (pNewWnd))
	{
		CBCGPBaseRibbonElement* pFocused = GetFocused ();
		if (pFocused != NULL && !pFocused->IsDroppedDown ())
		{
			pFocused->m_bIsFocused = FALSE;
			pFocused->OnSetFocus (FALSE);
			pFocused->Redraw ();
		}
	}
}
//****************************************************************************************************************
BOOL CBCGPRibbonBar::TranslateChar (UINT nChar)
{
	if (m_dwHideFlags & BCGPRIBBONBAR_HIDE_ALL)
	{
		return FALSE;
	}

	if (!CBCGPKeyboardManager::IsKeyPrintable (nChar))
	{
		return FALSE;
	}

	if (m_nKeyboardNavLevel < 0)
	{
		SetKeyboardNavigationLevel (NULL, FALSE);
	}

	if (!ProcessKey (nChar))
	{
		DeactivateKeyboardFocus (FALSE);
		return FALSE;
	}

	return TRUE;
}
//****************************************************************************************************************
void CBCGPRibbonBar::DeactivateKeyboardFocus (BOOL bSetFocus)
{
	RemoveAllKeys ();
	m_nCurrKeyChar = 0;

	CBCGPBaseRibbonElement* pFocused = GetFocused ();
	if (pFocused != NULL)
	{
		pFocused->m_bIsFocused = FALSE;
		pFocused->OnSetFocus (FALSE);
		pFocused->Redraw ();
	}

	if (m_nKeyboardNavLevel < 0)
	{
		return;
	}

	m_nKeyboardNavLevel = -1;
	m_pKeyboardNavLevelParent = NULL;
	m_pKeyboardNavLevelCurrent = NULL;

	CFrameWnd* pParentFrame = GetParentFrame ();
	ASSERT_VALID (pParentFrame);

	if (bSetFocus)
	{
		pParentFrame->SetFocus ();
	}

	RedrawWindow ();
}
//****************************************************************************************************************
void CBCGPRibbonBar::SetKeyboardNavigationLevel (CObject* pLevel, BOOL bSetFocus)
{
	if (!m_bKeyTips)
	{
		return;
	}

	if (m_dwHideFlags & BCGPRIBBONBAR_HIDE_ALL)
	{
		return;
	}

	if (bSetFocus)
	{
		SetFocus ();
	}

	int i = 0;

	RemoveAllKeys ();
	m_nCurrKeyChar = 0;
	
	m_pKeyboardNavLevelParent = NULL;
	m_pKeyboardNavLevelCurrent = pLevel;

	CFrameWnd* pParentFrame = GetParentFrame ();
	ASSERT_VALID (pParentFrame);

	CWnd* pFocus = GetFocus();

	BOOL bActive = (pFocus->GetSafeHwnd () != NULL && 
		(pParentFrame->IsChild (pFocus) || pFocus->GetSafeHwnd () == pParentFrame->GetSafeHwnd ()));

	if (!bActive)
	{
		return;
	}

	if (pLevel == NULL)
	{
		if (m_pMainButton != NULL)
		{
			if (m_pMainButton->IsDroppedDown())
			{
				return;
			}

			m_arKeyElements.Add (new CBCGPRibbonKeyTip (m_pMainButton));
		}

		m_nKeyboardNavLevel = 0;

		for (i = 0; i < m_arCategories.GetSize (); i++)
		{
			CBCGPRibbonCategory* pCategory = m_arCategories [i];
			ASSERT_VALID (pCategory);

			if (pCategory->IsVisible ())
			{
				m_arKeyElements.Add (new CBCGPRibbonKeyTip (&pCategory->m_Tab));
			}
		}

		m_QAToolbar.AddToKeyList (m_arKeyElements);
		m_TabElements.AddToKeyList (m_arKeyElements);

		if (m_pActiveCategory != NULL && (m_dwHideFlags & BCGPRIBBONBAR_HIDE_ELEMENTS) == 0)
		{
			m_pActiveCategory->m_Tab.m_bIsFocused = TRUE;
		}
		else if (m_pMainButton != NULL)
		{
			m_pMainButton->m_bIsFocused = TRUE;
		}
	}
	else
	{
		CArray<CBCGPBaseRibbonElement*,CBCGPBaseRibbonElement*> arElems;

		CBCGPRibbonCategory* pCategory = DYNAMIC_DOWNCAST (CBCGPRibbonCategory, pLevel);
		CBCGPRibbonPanel* pPanel = DYNAMIC_DOWNCAST (CBCGPRibbonPanel, pLevel);

		if (pCategory != NULL)
		{
			ASSERT_VALID (pCategory);

			if (m_dwHideFlags == 0 || pCategory->GetParentMenuBar () != NULL)
			{
				pCategory->GetElements (arElems);
			}
		}
		else if (pPanel != NULL)
		{
			ASSERT_VALID (pPanel);
			
			pPanel->GetElements (arElems);

			if (!pPanel->IsMainPanel ())
			{
				CBCGPRibbonCategory* pParentCategory = NULL;

				if (pPanel->GetParentButton () == NULL || !pPanel->GetParentButton ()->IsQATMode ())
				{
					pParentCategory = pPanel->GetParentCategory ();
				}

				if (pPanel->GetParentMenuBar () != NULL)
				{
					CBCGPPopupMenu* pPopupMenu = DYNAMIC_DOWNCAST (CBCGPPopupMenu, 
						pPanel->GetParentMenuBar ()->GetParent ());
					ASSERT_VALID (pPopupMenu);

					CBCGPRibbonPanelMenu* pParentMenu = DYNAMIC_DOWNCAST (CBCGPRibbonPanelMenu,
						pPopupMenu->GetParentPopupMenu ());

					if (pParentMenu != NULL)
					{
						m_pKeyboardNavLevelParent = pParentMenu->GetPanel ();

						if (m_pKeyboardNavLevelParent == NULL)
						{
							pParentCategory = pParentMenu->GetCategory ();
						}
					}
					else
					{
						CBCGPBaseRibbonElement* pParentElement = pPopupMenu->GetParentRibbonElement ();
						if (pParentElement != NULL)
						{
							pParentCategory = pParentElement->GetParentCategory ();
						}
					}
				}

				if (pParentCategory != NULL && !pParentCategory->GetRect ().IsRectEmpty ())
				{
					m_pKeyboardNavLevelParent = pParentCategory;
				}
			}
		}

		for (i = 0; i < arElems.GetSize (); i++)
		{
			CBCGPBaseRibbonElement* pElem = arElems [i];
			ASSERT_VALID (pElem);

			pElem->AddToKeyList (m_arKeyElements);
		}

		m_nKeyboardNavLevel = 1;
	}

	ShowKeyTips ();
	RedrawWindow ();
}
//*************************************************************************************************************
void CBCGPRibbonBar::OnBeforeProcessKey (int& nChar)
{
	nChar = CBCGPKeyboardManager::TranslateCharToUpper (nChar);
}
//*************************************************************************************************************
BOOL CBCGPRibbonBar::ProcessKey (int nChar)
{
	OnBeforeProcessKey (nChar);

	CBCGPBaseRibbonElement* pKeyElem = NULL;

	BOOL bIsMenuKey = FALSE;

	for (int i = 0; i < m_arKeyElements.GetSize () && pKeyElem == NULL; i++)
	{
		CBCGPRibbonKeyTip* pKey = m_arKeyElements [i];
		ASSERT_VALID (pKey);

		CBCGPBaseRibbonElement* pElem = pKey->GetElement ();
		ASSERT_VALID (pElem);

		CString strKeys = pKey->IsMenuKey () ? pElem->GetMenuKeys () : pElem->GetKeys ();
		strKeys.MakeUpper ();

		if (strKeys.IsEmpty ())
		{
			continue;
		}

		if (m_nCurrKeyChar != 0)
		{
			if (strKeys.GetLength () > 1 &&
				strKeys [0] == m_nCurrKeyChar &&
				strKeys [1] == nChar)
			{
				pKeyElem = pElem;
				bIsMenuKey = pKey->IsMenuKey ();
				break;
			}
		}
		else if (strKeys [0] == nChar)
		{
			if (strKeys.GetLength () > 1)
			{
				m_nCurrKeyChar = strKeys [0];
				ShowKeyTips ();
				return TRUE;
			}
			else 
			{
				pKeyElem = pElem;
				bIsMenuKey = pKey->IsMenuKey ();
				break;
			}
		}
	}

	if (pKeyElem == NULL)
	{
		return FALSE;
	}

	ASSERT_VALID (pKeyElem);

	if (::GetFocus () != GetSafeHwnd ())
	{
		SetFocus ();
	}

	CBCGPDisableMenuAnimation disableMenuAnimation;

    HWND hwndThis = GetSafeHwnd ();

    if (pKeyElem->OnKey (bIsMenuKey) && ::IsWindow (hwndThis))
    {
		DeactivateKeyboardFocus ();
    }

	return TRUE;
}
//*****************************************************************************
BOOL CBCGPRibbonBar::NavigateRibbon (int nChar)
{
	ASSERT_VALID (this);

	CArray<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*> arElems;

	const BOOL bIsRibbonMinimized = (m_dwHideFlags & BCGPRIBBONBAR_HIDE_ELEMENTS) != 0;

	if (GetDroppedDown () != NULL || nChar == VK_ESCAPE)
	{
		return FALSE;
	}

	// Get focused element
	CBCGPBaseRibbonElement* pFocused = GetFocused ();
	if (pFocused == NULL)
	{
		return FALSE;
	}

	ASSERT_VALID (pFocused);

	HideKeyTips ();

	RemoveAllKeys ();
	m_nCurrKeyChar = 0;

	m_nKeyboardNavLevel = -1;
	m_pKeyboardNavLevelParent = NULL;
	m_pKeyboardNavLevelCurrent = NULL;

	if (pFocused == m_pMainButton)
	{
		switch (nChar)
		{
		case VK_DOWN:
		case VK_RETURN:
		case VK_SPACE:
			pFocused->OnKey (FALSE);
			return TRUE;

		case VK_UP:
			return TRUE;

		case VK_RIGHT:
			if (m_pActiveCategory != NULL)
			{
				ASSERT_VALID (m_pActiveCategory);

				pFocused->m_bIsFocused = pFocused->m_bIsHighlighted = FALSE;
				pFocused->OnSetFocus (FALSE);
				pFocused->Redraw ();

				m_pActiveCategory->m_Tab.m_bIsFocused = TRUE;
				m_pActiveCategory->m_Tab.OnSetFocus (TRUE);
				m_pActiveCategory->m_Tab.Redraw ();
				return TRUE;
			}
		}
	}

	if (nChar == VK_RETURN || nChar == VK_SPACE)
	{
		pFocused->OnKey (FALSE);
		return TRUE;
	}

	if (nChar == VK_DOWN &&
		pFocused->IsKindOf (RUNTIME_CLASS (CBCGPRibbonTab)) &&
		m_pActiveCategory != NULL && !bIsRibbonMinimized)
	{
		ASSERT_VALID (m_pActiveCategory);

		CBCGPBaseRibbonElement* pFocusedNew = m_pActiveCategory->GetFirstVisibleElement ();
		if (pFocusedNew != NULL)
		{
			ASSERT_VALID (pFocusedNew);

			pFocused->m_bIsFocused = pFocused->m_bIsHighlighted = FALSE;
			pFocused->OnSetFocus (FALSE);
			pFocused->Redraw ();

			pFocusedNew->m_bIsFocused = TRUE;
			pFocusedNew->OnSetFocus (TRUE);
			pFocusedNew->Redraw ();
			return TRUE;
		}
	}

	switch (nChar)
	{
	case VK_DOWN:
	case VK_UP:
	case VK_LEFT:
	case VK_RIGHT:
	case VK_TAB:
		{
			GetVisibleElements (arElems);

			CRect rectClient;
			GetClientRect (rectClient);

			int nScroll = 0;
			BOOL bIsScrollLeftAvailable = FALSE;
			BOOL bIsScrollRightAvailable = FALSE;

			if (m_pActiveCategory != NULL)
			{
				ASSERT_VALID (m_pActiveCategory);

				bIsScrollLeftAvailable = !m_pActiveCategory->m_ScrollLeft.GetRect ().IsRectEmpty ();
				bIsScrollRightAvailable = !m_pActiveCategory->m_ScrollRight.GetRect ().IsRectEmpty ();
			}

			CBCGPBaseRibbonElement* pFocusedNew = FindNextFocusedElement (
				nChar, arElems, rectClient, pFocused,  bIsScrollLeftAvailable, bIsScrollRightAvailable, nScroll);

			if (nScroll != 0 && m_pActiveCategory != NULL)
			{
				switch (nScroll)
				{
				case -2:
					pFocusedNew = m_pActiveCategory->GetFirstVisibleElement ();
					break;

				case 2:
					pFocusedNew = m_pActiveCategory->GetLastVisibleElement ();
					break;

				case -1:
				case 1:
					m_pActiveCategory->OnScrollHorz (nScroll < 0);
				}
			}

			if (globalData.IsAccessibilitySupport () && pFocusedNew != NULL)
			{
				CArray<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*> arButtons;
				GetVisibleElements (arButtons);

				for (int i = 0; i < arButtons.GetSize (); i++)
				{
					CBCGPBaseRibbonElement* pElem = arButtons [i];
					ASSERT_VALID (pElem);

					if (pElem == pFocusedNew)
					{
						m_AccData.Clear ();
						pElem->SetACCData(this, m_AccData);
						globalData.NotifyWinEvent (EVENT_OBJECT_FOCUS, GetSafeHwnd (), OBJID_CLIENT , i + 1);
						break;
					}
				}
			}

			if (pFocusedNew == pFocused)
			{
				return TRUE;
			}

			if (pFocusedNew == NULL)
			{
				return TRUE;
			}

			if (nChar == VK_UP)
			{
				//-----------------------------------------------------
				// Up ouside the current panel should activate the tab!
				//-----------------------------------------------------
				if (pFocusedNew->GetParentPanel () != pFocused->GetParentPanel () &&
					!pFocusedNew->IsKindOf (RUNTIME_CLASS (CBCGPRibbonTab)) &&
					m_pActiveCategory != NULL)
				{
					pFocusedNew = &m_pActiveCategory->m_Tab;
				}
			}

			pFocused->m_bIsHighlighted = pFocused->m_bIsFocused = FALSE;
			pFocused->OnSetFocus (FALSE);
			pFocused->Redraw ();

			ASSERT_VALID (pFocusedNew);

			if (pFocusedNew->IsKindOf (RUNTIME_CLASS (CBCGPRibbonTab)) && !bIsRibbonMinimized)
			{
				if (pFocused->IsKindOf (RUNTIME_CLASS (CBCGPRibbonTab)) &&
					(nChar == VK_LEFT || nChar == VK_RIGHT))
				{
					SetActiveCategory (pFocusedNew->GetParentCategory ());
					pFocusedNew->m_bIsFocused = TRUE;
				}
				else if (m_pActiveCategory != NULL)
				{
					ASSERT_VALID (m_pActiveCategory);
					
					pFocusedNew = &m_pActiveCategory->m_Tab;
					pFocusedNew->m_bIsFocused = TRUE;
					pFocusedNew->OnSetFocus (TRUE);
				}
			}
			else
			{
				pFocusedNew->m_bIsFocused = TRUE;
				pFocusedNew->OnSetFocus (TRUE);
			}

			pFocusedNew->Redraw ();
			return TRUE;
		}
	}

	return FALSE;
}
//*****************************************************************************
CBCGPBaseRibbonElement* CBCGPRibbonBar::FindNearest (CPoint pt, 
									 const CArray<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*>& arButtons)
{
	for (int i = 0; i < arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pElem = arButtons [i];
		ASSERT_VALID (pElem);

		if (pElem->m_rect.PtInRect (pt))
		{
			return pElem;
		}
	}

	return NULL;
}
//*****************************************************************************
CBCGPBaseRibbonElement* CBCGPRibbonBar::FindNextFocusedElement (
	int nChar, const CArray<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*>& arElems,
	CRect rectElems, CBCGPBaseRibbonElement* pFocused,
	BOOL bIsScrollLeftAvailable, BOOL bIsScrollRightAvailable, int& nScroll)
{
	ASSERT_VALID (pFocused);

	nScroll = 0;
	int nIndexFocused = -1;

	for (int i = 0; i < arElems.GetSize (); i++)
	{
		if (arElems [i] == pFocused)
		{
			nIndexFocused = i;
			break;
		}
	}

	if (nIndexFocused < 0)
	{
		return FALSE;
	}

	const BOOL bIsTabFocused = pFocused->IsKindOf (RUNTIME_CLASS (CBCGPRibbonTab));

	CBCGPBaseRibbonElement* pFocusedNew = NULL;

	if (nChar == VK_TAB)
	{
		const BOOL bShift = ::GetAsyncKeyState (VK_SHIFT) & 0x8000;

		int nNewIndex = -1;

		if (bShift)
		{
			for (int i = nIndexFocused - 1; nNewIndex < 0; i--)
			{
				if (i < 0)
				{
					if (bIsScrollLeftAvailable)
					{
						nScroll = -1;
						return NULL;
					}
					else if (bIsScrollRightAvailable)
					{
						nScroll = 2;
						return NULL;
					}

					i = (int)arElems.GetSize () - 1;
				}

				if (i == nIndexFocused)
				{
					return FALSE;
				}

				ASSERT_VALID (arElems [i]);
				
				if (bIsTabFocused && arElems [i]->IsKindOf (RUNTIME_CLASS (CBCGPRibbonTab)))
				{
					continue;
				}
				
				if (arElems [i]->IsTabStop () && !arElems [i]->GetRect ().IsRectEmpty ())
				{
					nNewIndex = i;
				}
			}
		}
		else
		{
			for (int i = nIndexFocused + 1; nNewIndex < 0; i++)
			{
				if (i >= arElems.GetSize ())
				{
					if (bIsScrollRightAvailable)
					{
						nScroll = 1;
						return NULL;
					}
					else if (bIsScrollLeftAvailable)
					{
						nScroll = -2;
						return NULL;
					}

					i = 0;
				}

				if (i == nIndexFocused)
				{
					return FALSE;
				}

				ASSERT_VALID (arElems [i]);

				if (bIsTabFocused && arElems [i]->IsKindOf (RUNTIME_CLASS (CBCGPRibbonTab)))
				{
					continue;
				}
				
				if (arElems [i]->IsTabStop () && !arElems [i]->GetRect ().IsRectEmpty ())
				{
					nNewIndex = i;
				}
			}
		}

		pFocusedNew = arElems [nNewIndex];
	}
	else
	{
		if (pFocused->HasFocus ())
		{
			return NULL;
		}

		CRect rectCurr = pFocused->GetRect ();

		const int nStep = 5;

		switch (nChar)
		{
		case VK_LEFT:
		case VK_RIGHT:
			{
				int xStart = nChar == VK_RIGHT ? 
					rectCurr.right + 1 :
					rectCurr.left - nStep - 1;
				int xStep = nChar == VK_RIGHT ? nStep : -nStep;

				for (int x = xStart; pFocusedNew == NULL; x += xStep)
				{
					if (nChar == VK_RIGHT)
					{
						if (x > rectElems.right)
						{
							if (bIsScrollRightAvailable)
							{
								nScroll = 1;
								return NULL;
							}
							else if (bIsScrollLeftAvailable)
							{
								nScroll = -2;
								return NULL;
							}

							x = rectElems.left;
						}
					}
					else
					{
						if (x < rectElems.left)
						{
							if (bIsScrollLeftAvailable)
							{
								nScroll = -1;
								return NULL;
							}
							else if (bIsScrollRightAvailable)
							{
								nScroll = 2;
								return NULL;
							}

							x = rectElems.right;
						}
					}

					if (x >= rectCurr.left && x <= rectCurr.right)
					{
						break;
					}

					CRect rectArea (x, rectCurr.top, x + nStep, rectCurr.bottom);
					if (pFocused->IsLargeMode () ||
						pFocused->IsWholeRowHeight ())
					{
						rectArea.DeflateRect (0, rectArea.Height () / 3);
					}

					CRect rectInter;

					for (int i = 0; i < arElems.GetSize (); i++)
					{
						CBCGPBaseRibbonElement* pElem = arElems [i];
						ASSERT_VALID (pElem);

						if (pElem->IsTabStop () && rectInter.IntersectRect (pElem->m_rect, rectArea))
						{
							pFocusedNew = pElem;
							break;
						}
					}
				}
			}
			break;

		case VK_UP:
		case VK_DOWN:
			{
				int x = rectCurr.CenterPoint ().x;

				int yStart = nChar == VK_DOWN ? 
					rectCurr.bottom + 1 :
					rectCurr.top - 1;

				int yStep = nChar == VK_DOWN ? nStep : -nStep;

				for (int i = 0; pFocusedNew == NULL; i++)
				{
					int y = yStart;

					int x1 = x - i * nStep;
					int x2 = x + i * nStep;

					if (x1 < rectElems.left && x2 > rectElems.right)
					{
						break;
					}

					while (pFocusedNew == NULL)
					{
						if ((pFocusedNew = FindNearest (CPoint (x1, y), arElems)) == NULL)
						{
							pFocusedNew = FindNearest (CPoint (x2, y), arElems);
						}

						if (pFocusedNew != NULL)
						{
							ASSERT_VALID (pFocusedNew);
							
							if (!pFocusedNew->IsTabStop ())
							{
								pFocusedNew = NULL;
							}
						}

						y += yStep;

						if (nChar == VK_DOWN)
						{
							if (y > rectElems.bottom)
							{
								break;
							}
						}
						else
						{
							if (y < rectElems.top)
							{
								break;
							}
						}
					}
				}
			}
		}
	}

	return pFocusedNew;
}
//*****************************************************************************
CBCGPBaseRibbonElement* CBCGPRibbonBar::GetFocused ()
{
	const BOOL bIsRibbonMinimized = (m_dwHideFlags & BCGPRIBBONBAR_HIDE_ELEMENTS) != 0;

	//---------------------------
	// Check for the main button:
	//---------------------------
	if (m_pMainButton != NULL)
	{
		ASSERT_VALID (m_pMainButton);

		if (m_pMainButton->IsFocused ())
		{
			return m_pMainButton;
		}
	}

	//--------------------------------
	// Check for quick access toolbar:
	//--------------------------------
	CBCGPBaseRibbonElement* pQAElem = m_QAToolbar.GetFocused ();
	if (pQAElem != NULL)
	{
		ASSERT_VALID (pQAElem);
		return pQAElem;
	}

	//------------------------
	// Check for tab elements:
	//------------------------
	CBCGPBaseRibbonElement* pTabElem = m_TabElements.GetFocused ();
	if (pTabElem != NULL)
	{
		ASSERT_VALID (pTabElem);
		return pTabElem;
	}

	if (m_pActiveCategory != NULL)
	{
		ASSERT_VALID (m_pActiveCategory);

		if (m_pActiveCategory->m_Tab.IsFocused ())
		{
			return &m_pActiveCategory->m_Tab;
		}

		if (!bIsRibbonMinimized)
		{
			return m_pActiveCategory->GetFocused ();
		}
	}

	for (int i = 0; i < m_arCategories.GetSize (); i++)
	{
		if (m_arCategories [i]->m_Tab.IsFocused ())
		{
			return &m_arCategories [i]->m_Tab;
		}
	}

	return NULL;
}
//*****************************************************************************
void CBCGPRibbonBar::RemoveAllKeys ()
{
	for (int i = 0; i < m_arKeyElements.GetSize (); i++)
	{
		CBCGPRibbonKeyTip* pKeyTip = m_arKeyElements [i];
		ASSERT_VALID (pKeyTip);

		if (pKeyTip->GetSafeHwnd () != NULL)
		{
			pKeyTip->DestroyWindow ();
		}

		delete pKeyTip;
	}

	m_arKeyElements.RemoveAll ();
}
//*****************************************************************************
void CBCGPRibbonBar::ShowKeyTips (BOOL bRepos)
{
	for (int i = 0; i < m_arKeyElements.GetSize (); i++)
	{
		CBCGPRibbonKeyTip* pKeyTip = m_arKeyElements [i];
		ASSERT_VALID (pKeyTip);

		CBCGPBaseRibbonElement* pElem = pKeyTip->GetElement ();
		ASSERT_VALID (pElem);

		if (m_nCurrKeyChar != 0)
		{
			CString strKeys = pKeyTip->IsMenuKey () ? pElem->GetMenuKeys () : pElem->GetKeys ();
			strKeys.MakeUpper ();

			if (strKeys.GetLength () < 2 || strKeys [0] != m_nCurrKeyChar)
			{
				pKeyTip->Hide ();
				continue;
			}
		}

		pKeyTip->Show (bRepos);
	}

	if (m_pToolTip->GetSafeHwnd () != NULL &&
		m_pToolTip->IsWindowVisible ())
	{
		m_pToolTip->SetWindowPos (&wndTopMost, -1, -1, -1, -1,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	}
}
//*****************************************************************************
void CBCGPRibbonBar::HideKeyTips ()
{
	for (int i = 0; i < m_arKeyElements.GetSize (); i++)
	{
		CBCGPRibbonKeyTip* pKeyTip = m_arKeyElements [i];
		ASSERT_VALID (pKeyTip);

		pKeyTip->Hide ();
	}
}
//*****************************************************************************
void CBCGPRibbonBar::OnRTLChanged (BOOL bIsRTL)
{
	CBCGPControlBar::OnRTLChanged (bIsRTL);

	if (m_pMainButton != NULL)
	{
		ASSERT_VALID (m_pMainButton);
		m_pMainButton->OnRTLChanged (bIsRTL);
	}

	m_QAToolbar.OnRTLChanged (bIsRTL);
	m_TabElements.OnRTLChanged (bIsRTL);

	for (int i = 0; i < m_arCategories.GetSize (); i++)
	{
		CBCGPRibbonCategory* pCategory = m_arCategories [i];
		ASSERT_VALID (pCategory);

		pCategory->OnRTLChanged (bIsRTL);
	}

	m_bForceRedraw = TRUE;
	RecalcLayout ();
}
//*****************************************************************************
BOOL CBCGPRibbonBar::OnSysKeyDown (CFrameWnd* pFrameWnd, WPARAM wParam, LPARAM lParam)
{
	if (!IsWindowVisible())
	{
		return FALSE;
	}

	if (m_bBackstageViewActive && (wParam == VK_F10 || wParam == VK_MENU))
	{
		return TRUE;
	}

	if (!m_bKeyTips)
	{
		return wParam == VK_F10 || wParam == VK_MENU;
	}

	BOOL  isCtrlPressed =  (0x8000 & GetKeyState(VK_CONTROL)) != 0;
	BOOL  isShiftPressed = (0x8000 & GetKeyState(VK_SHIFT)) != 0;

	if (wParam != VK_MENU && wParam != VK_F10)
	{
		KillTimer (IdShowKeyTips);
		return FALSE;
	}

	if (CBCGPPopupMenu::m_pActivePopupMenu == NULL &&
		(m_dwHideFlags & BCGPRIBBONBAR_HIDE_ALL) == 0 &&
		(wParam == VK_MENU || (wParam == VK_F10 && !isCtrlPressed && !isShiftPressed)))
	{
		if (GetFocus () == this && (lParam & (1 << 30)) == 0 &&
			wParam == VK_F10)
		{
			pFrameWnd->SetFocus ();
		}
		else
		{
			if (wParam == VK_F10)
			{
				SetFocus ();
			}
			else if (m_nKeyboardNavLevel < 0)
			{
				int nDelay = 200;
				SetTimer (IdShowKeyTips, nDelay, NULL);
			}
		}

		return TRUE;
	}

	return FALSE;
}
//*****************************************************************************
BOOL CBCGPRibbonBar::OnSysKeyUp (CFrameWnd* pFrameWnd, WPARAM wParam, LPARAM /*lParam*/)
{
	if (!IsWindowVisible())
	{
		return FALSE;
	}

	if (m_bBackstageViewActive && (wParam == VK_F10 || wParam == VK_MENU))
	{
		return TRUE;
	}

	if (!m_bKeyTips)
	{
		return wParam == VK_F10 || wParam == VK_MENU;
	}

	KillTimer (IdShowKeyTips);

	if (wParam == VK_MENU)
	{
		if (GetFocus () != this)
		{
			SetFocus ();
		}
		else if (CBCGPPopupMenu::m_pActivePopupMenu == NULL)
		{
			pFrameWnd->SetFocus ();
		}

		RedrawWindow ();
		return TRUE;
	}

	return FALSE;
}
//**********************************************************************************
void CBCGPRibbonBar::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CBCGPControlBar::OnShowWindow(bShow, nStatus);
	
	if (!bShow)
	{
		m_pHighlighted = NULL;
		m_pPressed = NULL;

		if (m_bIsTransparentCaption)
		{
			BCGPMARGINS margins;
			margins.cxLeftWidth = 0;
			margins.cxRightWidth = 0;
			margins.cyTopHeight = 0;
			margins.cyBottomHeight = 0;

			globalData.DwmExtendFrameIntoClientArea (
				GetParent ()->GetSafeHwnd (), &margins);
		}
	}
}
//**********************************************************************************
BOOL CBCGPRibbonBar::OnSetAccData (long lVal)
{
	ASSERT_VALID (this);

	m_AccData.Clear ();

	CArray<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*> arButtons;
	GetVisibleElements (arButtons);

	int nIndex = (int)lVal - 1;

	if (nIndex < 0 || nIndex >= arButtons.GetSize())
	{
		return FALSE;
	}

	ASSERT_VALID(arButtons[nIndex]);
	return arButtons[nIndex]->SetACCData (this, m_AccData);
}
//****************************************************************************
HRESULT CBCGPRibbonBar::accHitTest(long xLeft, long yTop, VARIANT *pvarChild)
{
	if( !pvarChild )
    {
        return E_INVALIDARG;
    }

	pvarChild->vt = VT_I4;
	pvarChild->lVal = CHILDID_SELF;

	CPoint pt (xLeft, yTop);
	ScreenToClient (&pt);

	CArray<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*> arButtons;
	GetVisibleElements (arButtons);

	for (int i = 0; i < arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pElem = arButtons [i];
		ASSERT_VALID (pElem);

		CRect rectElem = pElem->GetRect();
		if (rectElem.PtInRect (pt))
		{
			pvarChild->lVal = i + 1;
			pElem->SetACCData(this, m_AccData);
			break;
		}
	}

	return S_OK;
}
//****************************************************************************
HRESULT CBCGPRibbonBar::get_accChildCount(long *pcountChildren)
{
	if( !pcountChildren )
    {
        return E_INVALIDARG;
    }

	CArray<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*> arButtons;
	GetVisibleElements (arButtons);

	*pcountChildren = (int)arButtons.GetSize(); 
	return S_OK;
}
//****************************************************************************
HRESULT CBCGPRibbonBar::get_accChild(VARIANT /*varChild*/, IDispatch **ppdispChild)
{
	if( !ppdispChild )
    {
        return E_INVALIDARG;
    }

	return S_FALSE;
}
//****************************************************************************
HRESULT CBCGPRibbonBar::accNavigate(long navDir, VARIANT varStart, VARIANT* pvarEndUpAt)
{
    pvarEndUpAt->vt = VT_EMPTY;

    if (varStart.vt != VT_I4)
    {
        return E_INVALIDARG;
    }

	CArray<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*> arButtons;
	GetVisibleElements (arButtons);

	switch (navDir)
    {
	case NAVDIR_FIRSTCHILD:
		if (varStart.lVal == CHILDID_SELF)
		{
			pvarEndUpAt->vt = VT_I4;
			pvarEndUpAt->lVal = 1;
			return S_OK;	
		}
		break;

	case NAVDIR_LASTCHILD:
		if (varStart.lVal == CHILDID_SELF)
		{
			pvarEndUpAt->vt = VT_I4;
			pvarEndUpAt->lVal = (int)arButtons.GetSize();
			return S_OK;
		}
		break;

	case NAVDIR_NEXT:   
	case NAVDIR_RIGHT:
		if (varStart.lVal != CHILDID_SELF)
		{
			pvarEndUpAt->vt = VT_I4;
			pvarEndUpAt->lVal = varStart.lVal + 1;
			if (pvarEndUpAt->lVal > arButtons.GetSize())
			{
				pvarEndUpAt->vt = VT_EMPTY;
				return S_FALSE;
			}
			return S_OK;
		}
		break;

	case NAVDIR_PREVIOUS: 
	case NAVDIR_LEFT:
		if (varStart.lVal != CHILDID_SELF)
		{
			pvarEndUpAt->vt = VT_I4;
			pvarEndUpAt->lVal = varStart.lVal - 1;
			if (pvarEndUpAt->lVal <= 0)
			{
				pvarEndUpAt->vt = VT_EMPTY;
				return S_FALSE;
			}
			return S_OK;
		}
		break;
	}

	return S_FALSE;
}
//****************************************************************************
HRESULT CBCGPRibbonBar::accDoDefaultAction(VARIANT varChild)
{
    if (varChild.vt != VT_I4)
    {
        return E_INVALIDARG;
    }

    if (varChild.lVal != CHILDID_SELF)
    {
		CArray<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*> arButtons;
		GetVisibleElements (arButtons);

		int nIndex = (int)varChild.lVal - 1;
		if (nIndex < 0 || nIndex >= arButtons.GetSize())
		{
			return E_INVALIDARG;
		}

		CBCGPBaseRibbonElement* pElem = arButtons [nIndex];
		ASSERT_VALID (pElem);

		pElem->OnAccDefaultAction();
    }

    return S_OK;
}
//****************************************************************************
void CBCGPRibbonBar::OnChangeVisualManager()
{
	ASSERT_VALID (this);

	if (m_pMainCategory != NULL)
	{
		ASSERT_VALID (m_pMainCategory);
		m_pMainCategory->OnChangeVisualManager();
	}

	if (m_pBackstageCategory != NULL)
	{
		ASSERT_VALID (m_pBackstageCategory);
		m_pBackstageCategory->OnChangeVisualManager();
	}

	for (int i = 0; i < m_arCategories.GetSize (); i++)
	{
		CBCGPRibbonCategory* pCategory = m_arCategories [i];
		ASSERT_VALID (pCategory);

		pCategory->OnChangeVisualManager();
	}

	m_QAToolbar.OnChangeVisualManager();
	m_TabElements.OnChangeVisualManager();

	ForceRecalcLayout();
}
//****************************************************************************
void CBCGPRibbonBar::SetBackstageViewActve(BOOL bOn)
{
	ASSERT_VALID (this);

	if (CBCGPRibbonFloaty::m_pCurrent != NULL && ::IsWindow (CBCGPRibbonFloaty::m_pCurrent->GetSafeHwnd ()))
	{
		CBCGPRibbonFloaty::m_pCurrent->SendMessage(WM_CLOSE);
	}

	m_bBackstageViewActive = bOn;

	if (!bOn)
	{
		m_bDontSetKeyTips = FALSE;
	}

	int i = 0;

	for (i = 0; i < m_QAToolbar.GetCount(); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_QAToolbar.GetButton(i);
		ASSERT_VALID(pButton);

		pButton->OnEnable(!bOn);
	}

	for (i = 0; i < m_TabElements.GetCount(); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_TabElements.GetButton(i);
		ASSERT_VALID(pButton);

		pButton->OnEnable(!bOn);
	}
}
//****************************************************************************
CBCGPRibbonButton* CBCGPRibbonBar::FindBackstageFormParent (CRuntimeClass* pClass, BOOL bDerived)
{
	CBCGPRibbonCategory* pCategory = GetBackstageCategory ();
	if (!IsBackstageMode () || pCategory == NULL || pCategory->GetPanelCount () == 0)
	{
		return NULL;
	}

	CBCGPRibbonBackstageViewPanel* pPanel = DYNAMIC_DOWNCAST(CBCGPRibbonBackstageViewPanel, pCategory->GetPanel (0));
	if (pPanel == NULL)
	{
		return NULL;
	}

	return pPanel->FindFormParent (pClass, bDerived);
}
//****************************************************************************
BOOL CBCGPRibbonBar::ReplaceRibbonElementByID(UINT uiCmdID, CBCGPBaseRibbonElement& newElement, BOOL bCopyContent) const
{
	if (uiCmdID == 0 || uiCmdID == (UINT)-1)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	CBCGPBaseRibbonElement* pNewElement = DYNAMIC_DOWNCAST(CBCGPBaseRibbonElement, newElement.GetRuntimeClass()->CreateObject());
	if (pNewElement == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	ASSERT_VALID(pNewElement);

	pNewElement->CopyFrom(newElement);

	BOOL bFound = FALSE;

	for (int iTab = 0; iTab < GetCategoryCount(); iTab++)
	{
		CBCGPRibbonCategory* pCategory = GetCategory(iTab);
		ASSERT_VALID(pCategory);

		for (int iPanel = 0; iPanel < pCategory->GetPanelCount(); iPanel++)
		{
			CBCGPRibbonPanel* pPanel = pCategory->GetPanel(iPanel);
			ASSERT_VALID(pPanel);

			if (pPanel->ReplaceByID(uiCmdID, pNewElement, bCopyContent) ||
				pPanel->ReplaceSubitemByID(uiCmdID, pNewElement, bCopyContent))
			{
				bFound = TRUE;

				pNewElement = DYNAMIC_DOWNCAST(CBCGPBaseRibbonElement, newElement.GetRuntimeClass()->CreateObject());
				ASSERT_VALID(pNewElement);

				pNewElement->CopyFrom(newElement);
			}
		}
	}

	delete pNewElement;
	return bFound;
}
//****************************************************************************
CBCGPRibbonMainPanel* CBCGPRibbonBar::GetMainPanel() const
{
	if (m_pMainCategory == NULL || m_pMainCategory->GetPanelCount () == 0)
	{
		return NULL;
	}

	return DYNAMIC_DOWNCAST (CBCGPRibbonMainPanel, m_pMainCategory->GetPanel (0));
}
//****************************************************************************
CBCGPRibbonBackstageViewPanel* CBCGPRibbonBar::GetBackstageViewPanel() const
{
	if (m_pBackstageCategory == NULL || m_pBackstageCategory->GetPanelCount () == 0)
	{
		return NULL;
	}

	return DYNAMIC_DOWNCAST (CBCGPRibbonBackstageViewPanel, m_pBackstageCategory->GetPanel (0));
}

#endif // BCGP_EXCLUDE_RIBBON
