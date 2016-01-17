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
// BCGPListBox.cpp : implementation file
//

#include "stdafx.h"
#include "BCGPListBox.h"
#include "BCGPDlgImpl.h"
#include "BCGPVisualManager.h"
#include "BCGPPropertySheet.h"
#include "BCGPStatic.h"
#include "TrackMouse.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CBCGPListBox, CListBox)

/////////////////////////////////////////////////////////////////////////////
// CBCGPListBox

CBCGPListBox::CBCGPListBox()
{
	m_bVisualManagerStyle = FALSE;
	m_bOnGlass = FALSE;
	m_nHighlightedItem = -1;
	m_bTracked = FALSE;
	m_hImageList = NULL;
	m_sizeImage = CSize(0, 0);
	m_bBackstageMode = FALSE;
	m_bPropertySheetNavigator = FALSE;
}

CBCGPListBox::~CBCGPListBox()
{
}


BEGIN_MESSAGE_MAP(CBCGPListBox, CListBox)
	//{{AFX_MSG_MAP(CBCGPListBox)
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(BCGM_ONSETCONTROLVMMODE, OnBCGSetControlVMMode)
	ON_REGISTERED_MESSAGE(BCGM_ONSETCONTROLAERO, OnBCGSetControlAero)
	ON_REGISTERED_MESSAGE(BCGM_ONSETCONTROLBACKSTAGEMODE, OnBCGSetControlBackStageMode)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_CONTROL_REFLECT_EX(LBN_SELCHANGE, OnSelchange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCGPListBox message handlers

LRESULT CBCGPListBox::OnBCGSetControlVMMode (WPARAM wp, LPARAM)
{
	m_bVisualManagerStyle = (BOOL) wp;
	return 0;
}
//**************************************************************************
LRESULT CBCGPListBox::OnBCGSetControlAero (WPARAM wp, LPARAM)
{
	m_bOnGlass = (BOOL) wp;
	return 0;
}
//**************************************************************************
int CBCGPListBox::HitTest(CPoint pt)
{
	if ((GetStyle() & LBS_NOSEL) == LBS_NOSEL)
	{
		return -1;
	}

	for (int i = 0; i < GetCount (); i++)
	{
		CRect rectItem;
		GetItemRect (i, rectItem);

		if (rectItem.PtInRect (pt))
		{
			return IsCaptionItem(i) ? -1 : i;
		}
	}

	return -1;
}
//**************************************************************************
void CBCGPListBox::OnMouseMove(UINT nFlags, CPoint point)
{
	CListBox::OnMouseMove (nFlags, point);

	ASSERT (IsWindowEnabled ());

	int nHighlightedItem = HitTest(point);

	if (!m_bTracked)
	{
		m_bTracked = TRUE;
		
		TRACKMOUSEEVENT trackmouseevent;
		trackmouseevent.cbSize = sizeof(trackmouseevent);
		trackmouseevent.dwFlags = TME_LEAVE;
		trackmouseevent.hwndTrack = GetSafeHwnd();
		trackmouseevent.dwHoverTime = HOVER_DEFAULT;
		BCGPTrackMouse (&trackmouseevent);
	}

	if (nHighlightedItem != m_nHighlightedItem)
	{
		CRect rectItem;

		if (m_nHighlightedItem >= 0)
		{
			GetItemRect (m_nHighlightedItem, rectItem);
			InvalidateRect (rectItem);
		}

		m_nHighlightedItem = nHighlightedItem;

		if (m_nHighlightedItem >= 0)
		{
			GetItemRect (m_nHighlightedItem, rectItem);
			InvalidateRect (rectItem);
		}

		UpdateWindow ();
	}
}
//***********************************************************************************************	
LRESULT CBCGPListBox::OnMouseLeave(WPARAM,LPARAM)
{
	m_bTracked = FALSE;

	if (m_nHighlightedItem >= 0)
	{
		CRect rectItem;
		GetItemRect (m_nHighlightedItem, rectItem);

		m_nHighlightedItem = -1;
		RedrawWindow (rectItem);
	}

	return 0;
}
//***********************************************************************************************	
void CBCGPListBox::DrawItem(LPDRAWITEMSTRUCT /*lpDIS*/) 
{
}
//***********************************************************************************************	
void CBCGPListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	lpMeasureItemStruct->itemHeight = max(m_sizeImage.cy + 4, globalData.GetTextHeight () * 9 / 5);
}
//***********************************************************************************************	
void CBCGPListBox::OnDrawItemContent(CDC* pDC, CRect rect, int nIndex)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

	const BOOL bIsCaption = IsCaptionItem(nIndex);

	CFont* pOldFont = NULL;

	if (!bIsCaption)
	{
		int nIcon = -1;
		if (m_hImageList != NULL && m_ItemIcons.Lookup(nIndex, nIcon))
		{
			CRect rectImage = rect;

			rectImage.top += (rect.Height () - m_sizeImage.cy) / 2;
			rectImage.bottom = rectImage.top + m_sizeImage.cy;

			rectImage.left += 4;
			rectImage.right = rectImage.left + m_sizeImage.cx;

			CImageList* pImageList = CImageList::FromHandle(m_hImageList);
			pImageList->Draw (pDC, nIcon, rectImage.TopLeft (), ILD_TRANSPARENT);

			rect.left += m_sizeImage.cx;
		}
		
		rect.DeflateRect (10, 0);
	}
#ifndef _BCGSUITE_
	else
	{
		pOldFont = pDC->SelectObject(&globalData.fontCaption);
	}
#endif

	CString strText;
	GetText (nIndex, strText);

	int nTextHeight = pDC->DrawText (strText, rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);

	if (bIsCaption)
	{
		CBCGPStatic ctrl;
		ctrl.m_bBackstageMode = m_bBackstageMode;

		CRect rectSeparator = rect;
		rectSeparator.top = rect.CenterPoint().y + nTextHeight / 2;
		rectSeparator.bottom = rectSeparator.top + 1;
		rectSeparator.right -= 5;

#ifndef _BCGSUITE_
		CBCGPVisualManager::GetInstance ()->OnDrawDlgSeparator(pDC, &ctrl, rectSeparator, TRUE);
#endif
		pDC->SelectObject(pOldFont);
	}
}
//***************************************************************************************
BOOL CBCGPListBox::SetImageList (HIMAGELIST hImageList)
{
	ASSERT (hImageList != NULL);

	CImageList* pImageList = CImageList::FromHandle (hImageList);
	if (pImageList == NULL)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	IMAGEINFO info;
	pImageList->GetImageInfo (0, &info);

	CRect rectImage = info.rcImage;
	m_sizeImage = rectImage.Size ();

	m_hImageList = hImageList;

	SetItemHeight(-1, max(GetItemHeight(-1), m_sizeImage.cy + 10));
	return TRUE;
}
//***********************************************************************************************************
BOOL CBCGPListBox::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}
//***********************************************************************************************************
void CBCGPListBox::OnPaint() 
{
	CPaintDC dcPaint(this); // device context for painting

	CRect rectClient;
	GetClientRect(rectClient);

	CRgn rgn;
	rgn.CreateRectRgnIndirect (rectClient);
	dcPaint.SelectClipRgn (&rgn);

	{
		CBCGPMemDC memDC(dcPaint, this);
		CDC* pDC = &memDC.GetDC();

		pDC->FillRect(rectClient, &globalData.brWindow);

#ifndef _BCGSUITE_
		if (m_bBackstageMode)
		{
			if (m_bPropertySheetNavigator)
			{
				CBCGPPropertySheet* pPropSheet = DYNAMIC_DOWNCAST(CBCGPPropertySheet, GetParent());
				if (pPropSheet != NULL)
				{
					ASSERT_VALID(pPropSheet);
					pPropSheet->OnDrawListBoxBackground(pDC, this);
				}

				CRect rectSeparator = rectClient;
				rectSeparator.left = rectSeparator.right - 3;
				rectSeparator.right -= 2;

				CBCGPStatic ctrl;
				ctrl.m_bBackstageMode = TRUE;

				CBCGPVisualManager::GetInstance ()->OnDrawDlgSeparator(pDC, &ctrl, rectSeparator, FALSE);
			}
		}
#endif
		int nStart = GetTopIndex ();
		int nCount = GetCount ();
		if (nStart != LB_ERR && nCount > 0)
		{
			pDC->SetBkMode (TRANSPARENT);

			CFont* pOldFont = pDC->SelectObject (&globalData.fontRegular);
			ASSERT_VALID (pOldFont);

			CArray<int,int> arSelection;
			int nSelCount = GetSelCount();
			if (nSelCount != LB_ERR)
			{
				if (nSelCount > 0)
				{
					arSelection.SetSize (nSelCount);
					GetSelItems (nSelCount, arSelection.GetData());	
				}
			}
			else
			{
				int nSel = GetCurSel();
				if (nSel != LB_ERR)
				{
					nSelCount = 1;
					arSelection.Add (nSel);
				}
			}
			nSelCount = (int)arSelection.GetSize ();

			for (int nIndex = nStart; nIndex < nCount; nIndex++)
			{
				CRect rect;
				GetItemRect(nIndex, rect);

				if (rect.bottom < rectClient.top || rectClient.bottom < rect.top)
				{
					break;
				}

				BOOL bIsSelected = FALSE;
				for (int nSelIndex = 0; nSelIndex < nSelCount; nSelIndex++)
				{
					if (nIndex == arSelection[nSelIndex])
					{
						bIsSelected = TRUE;
						break;
					}
				}
				const BOOL bIsHighlihted = (nIndex == m_nHighlightedItem) || (bIsSelected && CWnd::GetFocus() == this);

				COLORREF clrText = (COLORREF)-1;

				BOOL bIsCaptionItem = IsCaptionItem(nIndex);

				if ((bIsHighlihted || bIsSelected) && !bIsCaptionItem)
				{
					if (m_bVisualManagerStyle)
					{
						clrText = CBCGPVisualManager::GetInstance ()->OnFillListBoxItem (
							pDC, this, nIndex, rect, bIsHighlihted, bIsSelected);
					}
					else
					{
						pDC->FillRect (rect, &globalData.brHilite);
						
						if (bIsHighlihted)
						{
							pDC->DrawFocusRect (rect);
						}

						clrText = globalData.clrTextHilite;
					}
				}

				if (clrText == (COLORREF)-1)
				{
					pDC->SetTextColor (globalData.clrWindowText);
				}
				else
				{
					pDC->SetTextColor (clrText);
				}

				OnDrawItemContent(pDC, rect, nIndex);
			}

			pDC->SelectObject (pOldFont);
		}
	}

	dcPaint.SelectClipRgn (NULL);
}
//************************************************************************************************************
BOOL CBCGPListBox::OnSelchange() 
{
	RedrawWindow();
	return FALSE;
}
//************************************************************************************************************
void CBCGPListBox::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CListBox::OnVScroll(nSBCode, nPos, pScrollBar);
	RedrawWindow();
}
//**************************************************************************
LRESULT CBCGPListBox::OnBCGSetControlBackStageMode (WPARAM, LPARAM)
{
	m_bBackstageMode = TRUE;
	return 0;
}
//**************************************************************************
void CBCGPListBox::AddCaption(LPCTSTR lpszCaption)
{
	int nIndex = AddString(lpszCaption == NULL ? _T("") : lpszCaption);

	m_lstCaptionIndexes.AddTail(nIndex);
}
//**************************************************************************
BOOL CBCGPListBox::IsCaptionItem(int nIndex) const
{
	return m_lstCaptionIndexes.Find(nIndex) != 0;
}
//**************************************************************************
void CBCGPListBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (!m_lstCaptionIndexes.IsEmpty() && HitTest(point) == -1)
	{
		return;
	}
	
	CListBox::OnLButtonDown(nFlags, point);
}
//**************************************************************************
void CBCGPListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	const int nPrevSel = GetCurSel();

	CListBox::OnKeyDown(nChar, nRepCnt, nFlags);

	if ((GetStyle() & LBS_NOSEL) == LBS_NOSEL)
	{
		return;
	}

	if (m_lstCaptionIndexes.IsEmpty())
	{
		return;
	}

	const int nCurSel = GetCurSel();
	if (!IsCaptionItem(nCurSel) || nCurSel == nPrevSel)
	{
		return;
	}

	BOOL bFindNext = FALSE;

	switch (nChar)
	{
	case VK_HOME:
	case VK_DOWN:
	case VK_NEXT:
		bFindNext = TRUE;
		break;
	}

	int nNewSel = -1;

	if (bFindNext)
	{
		for(int i = nCurSel + 1; i < GetCount(); i++)
		{
			if (!IsCaptionItem(i))
			{
				nNewSel = i;
				break;
			}
		}
	}
	else
	{
		for(int i = nCurSel - 1; i >= 0; i--)
		{
			if (!IsCaptionItem(i))
			{
				nNewSel = i;
				break;
			}
		}
	}

	SetCurSel(nNewSel != -1 ? nNewSel : nPrevSel);
	RedrawWindow();
}
//**************************************************************************
BOOL CBCGPListBox::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if (CListBox::OnMouseWheel(nFlags, zDelta, pt))
	{
		RedrawWindow();
		return TRUE;
	}

	return FALSE;
}
