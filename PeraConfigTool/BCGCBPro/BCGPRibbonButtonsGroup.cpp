//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a part of BCGControlBar Library Professional Edition
// Copyright (C) 1998-2011 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************
//
// BCGPRibbonButtonsGroup.cpp: implementation of the CBCGPRibbonButtonsGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "bcgglobals.h"
#include "BCGPRibbonButtonsGroup.h"
#include "BCGPVisualManager.h"
#include "BCGPRibbonBar.h"
#include "BCGPRibbonStatusBar.h"

#ifndef BCGP_EXCLUDE_RIBBON

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CBCGPRibbonButtonsGroup, CBCGPBaseRibbonElement)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBCGPRibbonButtonsGroup::CBCGPRibbonButtonsGroup()
{
	m_bIsRibbonTabElements = FALSE;
}
//********************************************************************************
CBCGPRibbonButtonsGroup::CBCGPRibbonButtonsGroup(CBCGPBaseRibbonElement* pButton)
{
	m_bIsRibbonTabElements = FALSE;
	AddButton (pButton);
}
//********************************************************************************
CBCGPRibbonButtonsGroup::~CBCGPRibbonButtonsGroup()
{
	RemoveAll ();
}
//*******************************************************************************
void CBCGPRibbonButtonsGroup::AddButton (CBCGPBaseRibbonElement* pButton)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pButton);

	pButton->SetParentCategory (m_pParent);
	pButton->m_pParentGroup = this;

	if (IsBackstageViewMode())
	{
		pButton->SetBackstageViewMode();
	}

	m_arButtons.Add (pButton);
}
//********************************************************************************
void CBCGPRibbonButtonsGroup::AddButtons (
		const CList<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*>& lstButtons)
{
	ASSERT_VALID (this);
	
	for (POSITION pos = lstButtons.GetHeadPosition (); pos != NULL;)
	{
		AddButton (lstButtons.GetNext (pos));
	}
}
//********************************************************************************
void CBCGPRibbonButtonsGroup::RemoveAll ()
{
	ASSERT_VALID (this);

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		delete m_arButtons [i];
	}

	m_arButtons.RemoveAll ();
}
//********************************************************************************
void CBCGPRibbonButtonsGroup::OnDraw (CDC* pDC)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);

	if (m_rect.IsRectEmpty ())
	{
		return;
	}

	//-----------------------
	// Fill group background:
	//-----------------------
	COLORREF clrText = 
		CBCGPVisualManager::GetInstance ()->OnDrawRibbonButtonsGroup (
		pDC, this, m_rect);

	COLORREF clrTextOld = (COLORREF)-1;
	if (clrText != (COLORREF)-1)
	{
		clrTextOld = pDC->SetTextColor (clrText);
	}

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		if (pButton->m_rect.IsRectEmpty ())
		{
			continue;
		}

		CString strText = pButton->m_strText;
		BOOL bIsDisabled = pButton->IsDisabled();

		if ((IsQAT() || m_bIsRibbonTabElements) && pButton->m_pRibbonBar != NULL && pButton->m_pRibbonBar->IsBackstageViewActive())
		{
			pButton->m_bIsDisabled = TRUE;
		}

		if (pButton->GetImageSize (CBCGPBaseRibbonElement::RibbonImageSmall) 
			!= CSize (0, 0))
		{
			pButton->m_strText.Empty ();
		}

		pButton->OnDraw (pDC);

		pButton->m_strText = strText;
		pButton->m_bIsDisabled = bIsDisabled;
	}

	if (clrTextOld != (COLORREF)-1)
	{
		pDC->SetTextColor (clrTextOld);
	}
}
//********************************************************************************
CSize CBCGPRibbonButtonsGroup::GetRegularSize (CDC* pDC)
{
	ASSERT_VALID (this);

	const BOOL bIsOnStatusBar = DYNAMIC_DOWNCAST (CBCGPRibbonStatusBar, GetParentRibbonBar ()) != NULL;

	CSize size (0, 0);

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		pButton->SetInitialMode (TRUE);
		pButton->OnCalcTextSize (pDC);

		CSize sizeButton = pButton->GetSize (pDC);
		
		size.cx += sizeButton.cx;
		size.cy = max (size.cy, sizeButton.cy);
	}

	if (bIsOnStatusBar)
	{
		size.cx += 2;
	}

	if (!IsQAT() && !bIsOnStatusBar && m_pParentMenu == NULL)
	{
		size.cx += CBCGPVisualManager::GetInstance ()->GetRibbonButtonsGroupHorzMargin() * 2;
	}

	return size;
}
//********************************************************************************
void CBCGPRibbonButtonsGroup::OnUpdateCmdUI (CBCGPRibbonCmdUI* pCmdUI,
											CFrameWnd* pTarget,
											BOOL bDisableIfNoHndler)
{
	ASSERT_VALID (this);

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		pButton->OnUpdateCmdUI (pCmdUI, pTarget, bDisableIfNoHndler);
	}
}
//********************************************************************************
void CBCGPRibbonButtonsGroup::OnAfterChangeRect (CDC* pDC)
{
	ASSERT_VALID (this);

	BOOL bIsFirst = TRUE;

	const BOOL bIsOnStatusBar = DYNAMIC_DOWNCAST (CBCGPRibbonStatusBar, GetParentRibbonBar ()) != NULL;
	const BOOL bIsQATOnBottom = IsQAT () && !m_pRibbonBar->IsQuickAccessToolbarOnTop ();

	const int nMarginX = IsQAT () ? 2 : 0;
	const int nMarginTop = bIsQATOnBottom ? 2 : bIsOnStatusBar ? 1 : 0;
	const int nMarginBottom = (IsQAT () || bIsOnStatusBar) ? 1 : 0;

	const int nButtonHeight = m_rect.Height () - nMarginTop - nMarginBottom;

	CRect rectGroup = m_rect;

	if (!IsQAT() && !bIsOnStatusBar && m_pParentMenu == NULL)
	{
		rectGroup.DeflateRect(CBCGPVisualManager::GetInstance ()->GetRibbonButtonsGroupHorzMargin(), 0);
	}

	int x = rectGroup.left + nMarginX;

	int nCustomizeButtonIndex = -1;

	if (IsQAT () && m_arButtons.GetSize () > 0)
	{
		//---------------------------------------------
		// Last button is customize - it always visible.
		// Leave space for it:
		//---------------------------------------------
		nCustomizeButtonIndex = (int) m_arButtons.GetSize () - 1;

		CBCGPBaseRibbonElement* pButton = m_arButtons [nCustomizeButtonIndex];
		ASSERT_VALID (pButton);

		CSize sizeButton = pButton->GetSize (pDC);
		rectGroup.right -= sizeButton.cx;
	}

	BOOL bHasHiddenItems = FALSE;

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		pButton->m_bShowGroupBorder = TRUE;

		if (pButton->m_pRibbonBar != NULL && 
			!pButton->m_pRibbonBar->IsShowGroupBorder (this))
		{
			pButton->m_bShowGroupBorder = FALSE;
		}

		if (m_rect.IsRectEmpty ())
		{
			pButton->m_rect = CRect (0, 0, 0, 0);
			pButton->OnAfterChangeRect (pDC);
			continue;
		}

		BOOL bIsLast = i == m_arButtons.GetSize () - 1;

		pButton->SetParentCategory (m_pParent);

		CSize sizeButton = pButton->GetSize (pDC);
		sizeButton.cy = i != nCustomizeButtonIndex ? nButtonHeight : nButtonHeight - 1;

		const int y = i != nCustomizeButtonIndex ? rectGroup.top + nMarginTop : rectGroup.top;

		pButton->m_rect = CRect (CPoint (x, y), sizeButton);

		const BOOL bIsHiddenSeparator = bHasHiddenItems && pButton->IsSeparator ();

		if ((pButton->m_rect.right > rectGroup.right || bIsHiddenSeparator) &&
			i != nCustomizeButtonIndex)
		{
			pButton->m_rect = CRect (0, 0, 0, 0);
			bHasHiddenItems = TRUE;
		}
		else
		{
			x += sizeButton.cx;
		}

		pButton->OnAfterChangeRect (pDC);

		if (bIsFirst && bIsLast)
		{
			pButton->m_Location = RibbonElementSingleInGroup;
		}
		else if (bIsFirst)
		{
			pButton->m_Location = RibbonElementFirstInGroup;
		}
		else if (bIsLast)
		{
			pButton->m_Location = RibbonElementLastInGroup;
		}
		else
		{
			pButton->m_Location = RibbonElementMiddleInGroup;
		}

		bIsFirst = FALSE;
	}
}
//********************************************************************************
void CBCGPRibbonButtonsGroup::OnShow (BOOL bShow)
{
	ASSERT_VALID (this);

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		pButton->OnShow (bShow);
	}
}
//********************************************************************************
CBCGPBaseRibbonElement* CBCGPRibbonButtonsGroup::HitTest (CPoint point)
{
	ASSERT_VALID (this);

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		if (pButton->m_rect.PtInRect (point))
		{
			return pButton;
		}
	}

	return NULL;
}
//******************************************************************************
CBCGPBaseRibbonElement* CBCGPRibbonButtonsGroup::Find (const CBCGPBaseRibbonElement* pElement)
{
	ASSERT_VALID (this);

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		CBCGPBaseRibbonElement* pElem = pButton->Find (pElement);
		if (pElem != NULL)
		{
			ASSERT_VALID (pElem);
			return pElem;
		}
	}
	
	return CBCGPBaseRibbonElement::Find (pElement);
}
//******************************************************************************
CBCGPBaseRibbonElement* CBCGPRibbonButtonsGroup::FindByID (UINT uiCmdID)
{
	ASSERT_VALID (this);

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		CBCGPBaseRibbonElement* pElem = pButton->FindByID (uiCmdID);
		if (pElem != NULL)
		{
			ASSERT_VALID (pElem);
			return pElem;
		}
	}
	
	return NULL;
}
//******************************************************************************
CBCGPBaseRibbonElement* CBCGPRibbonButtonsGroup::FindByData (DWORD_PTR dwData)
{
	ASSERT_VALID (this);

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		CBCGPBaseRibbonElement* pElem = pButton->FindByData (dwData);
		if (pElem != NULL)
		{
			ASSERT_VALID (pElem);
			return pElem;
		}
	}
	
	return NULL;
}
//******************************************************************************
CBCGPBaseRibbonElement* CBCGPRibbonButtonsGroup::FindByOriginal (CBCGPBaseRibbonElement* pOriginal)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pOriginal);

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		CBCGPBaseRibbonElement* pElem = pButton->FindByOriginal (pOriginal);
		if (pElem != NULL)
		{
			ASSERT_VALID (pElem);
			return pElem;
		}
	}
	
	return NULL;
}
//******************************************************************************
CBCGPBaseRibbonElement* CBCGPRibbonButtonsGroup::GetPressed ()
{
	ASSERT_VALID (this);

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		CBCGPBaseRibbonElement* pElem = pButton->GetPressed ();
		if (pElem != NULL)
		{
			ASSERT_VALID (pElem);
			return pElem;
		}
	}
	
	return NULL;
}
//******************************************************************************
CBCGPBaseRibbonElement* CBCGPRibbonButtonsGroup::GetDroppedDown ()
{
	ASSERT_VALID (this);

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		CBCGPBaseRibbonElement* pElem = pButton->GetDroppedDown ();
		if (pElem != NULL)
		{
			ASSERT_VALID (pElem);
			return pElem;
		}
	}
	
	return NULL;
}
//******************************************************************************
CBCGPBaseRibbonElement* CBCGPRibbonButtonsGroup::GetHighlighted ()
{
	ASSERT_VALID (this);

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		CBCGPBaseRibbonElement* pElem = pButton->GetHighlighted ();
		if (pElem != NULL)
		{
			ASSERT_VALID (pElem);
			return pElem;
		}
	}
	
	return NULL;
}
//******************************************************************************
CBCGPBaseRibbonElement* CBCGPRibbonButtonsGroup::GetFocused ()
{
	ASSERT_VALID (this);

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		CBCGPBaseRibbonElement* pElem = pButton->GetFocused ();
		if (pElem != NULL)
		{
			ASSERT_VALID (pElem);
			return pElem;
		}
	}
	
	return NULL;
}
//******************************************************************************
BOOL CBCGPRibbonButtonsGroup::ReplaceByID (UINT uiCmdID, CBCGPBaseRibbonElement* pElem)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pElem);

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		if (pButton->GetID () == uiCmdID)
		{
			pElem->CopyFrom (*pButton);
			m_arButtons [i] = pElem;

			delete pButton;
			return TRUE;
		}

		if (pButton->ReplaceByID (uiCmdID, pElem))
		{
			return TRUE;
		}
	}
	
	return FALSE;
}
//******************************************************************************
void CBCGPRibbonButtonsGroup::SetImages (
		CBCGPToolBarImages* pImages,
		CBCGPToolBarImages* pHotImages,
		CBCGPToolBarImages* pDisabledImages,
		BOOL bDontScaleInHighDPIMode)
{
	ASSERT_VALID (this);

	if (pImages != NULL)
	{
		pImages->CopyTo (m_Images);
	}

	if (pHotImages != NULL)
	{
		pHotImages->CopyTo (m_HotImages);
	}

	if (pDisabledImages != NULL)
	{
		pDisabledImages->CopyTo (m_DisabledImages);
	}

	const CSize sizeImage = m_Images.GetImageSize ();

	const double dblScale = globalData.GetRibbonImageScale ();
	if (dblScale != 1.0 && sizeImage.cx <= 16 && sizeImage.cy <= 16 && !bDontScaleInHighDPIMode)
	{
		m_Images.SetTransparentColor (globalData.clrBtnFace);
		m_Images.SmoothResize (dblScale);

		m_HotImages.SetTransparentColor (globalData.clrBtnFace);
		m_HotImages.SmoothResize (dblScale);

		m_DisabledImages.SetTransparentColor (globalData.clrBtnFace);
		m_DisabledImages.SmoothResize (dblScale);
	}
}
//*******************************************************************************
void CBCGPRibbonButtonsGroup::OnDrawImage (
	CDC* pDC, CRect rectImage,  CBCGPBaseRibbonElement* pButton,
	int nImageIndex)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);
	ASSERT_VALID (pButton);

	CBCGPToolBarImages& image = 
		(pButton->IsDisabled () && m_DisabledImages.GetCount () != 0) ?
			m_DisabledImages :
		(pButton->IsHighlighted () && m_HotImages.GetCount () != 0) ?
			m_HotImages : m_Images;

	if (image.GetCount () <= 0)
	{
		return;
	}

	CBCGPDrawState ds;

	CPoint ptImage = rectImage.TopLeft ();
	ptImage.x++;

	image.SetTransparentColor (globalData.clrBtnFace);
	image.PrepareDrawImage (ds);

	image.SetTransparentColor (globalData.clrBtnFace);
	image.Draw (pDC, ptImage.x, ptImage.y, nImageIndex, FALSE, 
		pButton->IsDisabled () && m_DisabledImages.GetCount () == 0);

	image.EndDrawImage (ds);
}
//*****************************************************************************
void CBCGPRibbonButtonsGroup::CopyFrom (const CBCGPBaseRibbonElement& s)
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement::CopyFrom (s);

	CBCGPRibbonButtonsGroup& src = (CBCGPRibbonButtonsGroup&) s;

	RemoveAll ();

	for (int i = 0; i < src.m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pSrcElem = src.m_arButtons [i];
		ASSERT_VALID (pSrcElem);

		CBCGPBaseRibbonElement* pElem =
			(CBCGPBaseRibbonElement*) pSrcElem->GetRuntimeClass ()->CreateObject ();
		ASSERT_VALID (pElem);

		pElem->CopyFrom (*pSrcElem);

		m_arButtons.Add (pElem);
	}

	src.m_Images.CopyTo (m_Images);
	src.m_HotImages.CopyTo (m_HotImages);
	src.m_DisabledImages.CopyTo (m_DisabledImages);
}
//*****************************************************************************
void CBCGPRibbonButtonsGroup::SetParentMenu (CBCGPRibbonPanelMenuBar* pMenuBar)
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement::SetParentMenu (pMenuBar);

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		pButton->SetParentMenu (pMenuBar);
	}
}
//*****************************************************************************
void CBCGPRibbonButtonsGroup::SetOriginal (CBCGPBaseRibbonElement* pOriginal)
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement::SetOriginal (pOriginal);

	CBCGPRibbonButtonsGroup* pOriginalGroup =
		DYNAMIC_DOWNCAST (CBCGPRibbonButtonsGroup, pOriginal);

	if (pOriginalGroup == NULL)
	{
		return;
	}

	ASSERT_VALID (pOriginalGroup);

	if (pOriginalGroup->m_arButtons.GetSize () != m_arButtons.GetSize ())
	{
		ASSERT (FALSE);
		return;
	}

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		pButton->SetOriginal (pOriginalGroup->m_arButtons [i]);
	}
}
//*************************************************************************************
void CBCGPRibbonButtonsGroup::GetItemIDsList (CList<UINT,UINT>& lstItems) const
{
	ASSERT_VALID (this);

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		pButton->GetItemIDsList (lstItems);
	}
}
//*************************************************************************************
void CBCGPRibbonButtonsGroup::GetElementsByID (UINT uiCmdID, 
	CArray<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*>& arElements)
{
	ASSERT_VALID (this);

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		pButton->GetElementsByID (uiCmdID, arElements);
	}
}
//*************************************************************************************
void CBCGPRibbonButtonsGroup::GetElementsByName (LPCTSTR lpszName, 
		CArray<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*>& arButtons, DWORD dwFlags)
{
	ASSERT_VALID (this);

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		pButton->GetElementsByName (lpszName, arButtons, dwFlags);
	}
}
//*************************************************************************************
void CBCGPRibbonButtonsGroup::GetVisibleElements (
	CArray<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*>& arElements)
{
	ASSERT_VALID (this);

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		pButton->GetVisibleElements (arElements);
	}
}
//*************************************************************************************
int CBCGPRibbonButtonsGroup::AddToListBox (CBCGPRibbonCommandsListBox* pWndListBox, BOOL bDeep)
{
	ASSERT_VALID (this);

	int nIndex = -1;

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		nIndex = pButton->AddToListBox (pWndListBox, bDeep);
	}

	return nIndex;
}
//*************************************************************************************
void CBCGPRibbonButtonsGroup::CleanUpSizes ()
{
	ASSERT_VALID (this);

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		pButton->CleanUpSizes ();
	}
}
//*************************************************************************************
void CBCGPRibbonButtonsGroup::SetParentRibbonBar (CBCGPRibbonBar* pRibbonBar)
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement::SetParentRibbonBar (pRibbonBar);

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		pButton->SetParentRibbonBar (pRibbonBar);
	}
}
//*************************************************************************************
void CBCGPRibbonButtonsGroup::SetBackstageViewMode()
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement::SetBackstageViewMode();

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		pButton->SetBackstageViewMode();
	}
}
//*************************************************************************************
void CBCGPRibbonButtonsGroup::SetParentCategory (CBCGPRibbonCategory* pCategory)
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement::SetParentCategory (pCategory);

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		pButton->SetParentCategory (pCategory);
	}
}
//*************************************************************************************
void CBCGPRibbonButtonsGroup::AddToKeyList (CArray<CBCGPRibbonKeyTip*,CBCGPRibbonKeyTip*>& arElems)
{
	ASSERT_VALID (this);

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		pButton->AddToKeyList (arElems);
	}
}
//*************************************************************************************
void CBCGPRibbonButtonsGroup::OnRTLChanged(BOOL bIsRTL)
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement::OnRTLChanged (bIsRTL);

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		pButton->OnRTLChanged (bIsRTL);
	}
}
//*************************************************************************************
const CSize CBCGPRibbonButtonsGroup::GetImageSize () const
{
	ASSERT_VALID (this);

	if (m_Images.GetCount () <= 0)
	{
		return CSize (0, 0);
	}

	return m_Images.GetImageSize ();
}
//*************************************************************************************
CBCGPBaseRibbonElement* CBCGPRibbonButtonsGroup::GetFirstTabStop ()
{
	ASSERT_VALID (this);

	for (int i = 0; i < (int)m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		CBCGPBaseRibbonElement* pTabStop = pButton->GetFirstTabStop ();
		if (pTabStop != NULL)
		{
			return pTabStop;
		}
	}

	return NULL;
}
//*************************************************************************************
CBCGPBaseRibbonElement* CBCGPRibbonButtonsGroup::GetLastTabStop ()
{
	ASSERT_VALID (this);

	for (int i = (int)m_arButtons.GetSize () - 1; i >= 0; i--)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		CBCGPBaseRibbonElement* pTabStop = pButton->GetLastTabStop ();
		if (pTabStop != NULL)
		{
			return pTabStop;
		}
	}

	return NULL;
}
//*************************************************************************************
BOOL CBCGPRibbonButtonsGroup::CanBeSeparated() const
{
	if (m_arButtons.GetSize () <= 0)
	{
		return FALSE;
	}

	return m_arButtons[m_arButtons.GetSize () - 1]->CanBeSeparated();
}
//*************************************************************************************
void CBCGPRibbonButtonsGroup::OnChangeVisualManager()
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement::OnChangeVisualManager();

	for (int i = 0; i < m_arButtons.GetSize (); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arButtons [i];
		ASSERT_VALID (pButton);

		pButton->OnChangeVisualManager();
	}
}

#endif // BCGP_EXCLUDE_RIBBON
