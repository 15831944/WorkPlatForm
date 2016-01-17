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
// BCGPComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "BCGPComboBox.h"
#include "BCGPDlgImpl.h"
#ifndef _BCGSUITE_
#include "BCGPToolBarImages.h"
#include "BCGPToolbarComboBoxButton.h"
#endif
#include "BCGPVisualManager.h"
#include "BCGPDrawManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBCGPComboBox

IMPLEMENT_DYNAMIC(CBCGPComboBox, CComboBox)

CBCGPComboBox::CBCGPComboBox()
{
	m_bVisualManagerStyle = FALSE;
	m_bOnGlass = FALSE;
	m_bIsDroppedDown = FALSE;
	m_bIsButtonHighlighted = FALSE;
	m_rectBtn.SetRectEmpty ();
	m_bTracked = FALSE;
}

CBCGPComboBox::~CBCGPComboBox()
{
}

BEGIN_MESSAGE_MAP(CBCGPComboBox, CComboBox)
	//{{AFX_MSG_MAP(CBCGPComboBox)
	ON_WM_NCPAINT()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_CANCELMODE()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_CONTROL_REFLECT_EX(CBN_CLOSEUP, OnCloseup)
	ON_CONTROL_REFLECT_EX(CBN_DROPDOWN, OnDropdown)
	ON_REGISTERED_MESSAGE(BCGM_ONSETCONTROLVMMODE, OnBCGSetControlVMMode)
	ON_REGISTERED_MESSAGE(BCGM_ONSETCONTROLAERO, OnBCGSetControlAero)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCGPComboBox message handlers

LRESULT CBCGPComboBox::OnBCGSetControlVMMode (WPARAM wp, LPARAM)
{
	m_bVisualManagerStyle = (BOOL) wp;
	return 0;
}
//**************************************************************************
LRESULT CBCGPComboBox::OnBCGSetControlAero (WPARAM wp, LPARAM)
{
	m_bOnGlass = (BOOL) wp;

	if (m_bOnGlass && m_wndEdit.GetSafeHwnd () == NULL && (GetStyle () & CBS_DROPDOWN))
	{
		CWnd* pWndChild = GetWindow (GW_CHILD);

		while (pWndChild != NULL)
		{
			ASSERT_VALID (pWndChild);

			if (CWnd::FromHandlePermanent (pWndChild->GetSafeHwnd ()) == NULL)
			{
				#define MAX_CLASS_NAME		255
				#define EDIT_CLASS			_T("Edit")

				TCHAR lpszClassName [MAX_CLASS_NAME + 1];

				::GetClassName (pWndChild->GetSafeHwnd (), lpszClassName, MAX_CLASS_NAME);
				CString strClass = lpszClassName;

				if (strClass == EDIT_CLASS)
				{
					m_wndEdit.SubclassWindow (pWndChild->GetSafeHwnd ());
					m_wndEdit.m_bOnGlass = TRUE;
					break;
				}
			}

			pWndChild = pWndChild->GetNextWindow ();
		}
	}

	return 0;
}
//**************************************************************************
void CBCGPComboBox::OnNcPaint() 
{
#ifndef _BCGSUITE_
	if (globalData.bIsWindows9x)
	{
		Default();
	}
#endif
}
//**************************************************************************
void CBCGPComboBox::OnPaint() 
{
#ifndef _BCGSUITE_
	if (globalData.bIsWindows9x)
	{
		Default();
		return;
	}
#endif

	if ((GetStyle () & 0x0003L) == CBS_SIMPLE)
	{
		Default ();
		return;
	}

	if (!m_bVisualManagerStyle && !m_bOnGlass)
	{
		Default ();
		return;
	}

	CPaintDC dc(this); // device context for painting

	BYTE alpha = 0;
	if (m_bOnGlass)
	{
		alpha = 255;
	}

	CBCGPMemDC memDC (dc, this, alpha);
	CDC* pDC = &memDC.GetDC ();
	
	CRect rectClient;
	GetClientRect (rectClient);

	CBCGPDrawManager dm (*pDC);
	dm.DrawRect (rectClient, globalData.clrWindow, (COLORREF)-1);

	SendMessage (WM_PRINTCLIENT, (WPARAM) pDC->GetSafeHdc (), (LPARAM) PRF_CLIENT);

	const int cxDropDown = ::GetSystemMetrics (SM_CXVSCROLL) + 4;

	m_rectBtn = rectClient;
	m_rectBtn.left = m_rectBtn.right - cxDropDown;

	m_rectBtn.DeflateRect (2, 2);

	CBCGPDrawOnGlass dog (m_bOnGlass);

	CBCGPToolbarComboBoxButton buttonDummy;
#ifndef _BCGSUITE_
	buttonDummy.m_bIsCtrl = TRUE;

	CBCGPVisualManager::GetInstance ()->OnDrawComboDropButton (
		pDC, m_rectBtn, !IsWindowEnabled (), m_bIsDroppedDown,
		m_bIsButtonHighlighted,
		&buttonDummy);
#else
	CMFCVisualManager::GetInstance ()->OnDrawComboDropButton (
		pDC, m_rectBtn, !IsWindowEnabled (), m_bIsDroppedDown,
		m_bIsButtonHighlighted,
		&buttonDummy);
#endif

	dm.DrawRect (rectClient, (COLORREF)-1, globalData.clrBarShadow);
	rectClient.DeflateRect (1, 1);
	dm.DrawRect (rectClient, (COLORREF)-1, globalData.clrWindow);
}
//**************************************************************************
BOOL CBCGPComboBox::OnCloseup() 
{
	m_bIsDroppedDown = FALSE;
	m_bIsButtonHighlighted = FALSE;

	RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW | RDW_ALLCHILDREN);

	return FALSE;
}
//**************************************************************************
BOOL CBCGPComboBox::OnDropdown() 
{
	if (m_bTracked)
	{
		ReleaseCapture ();
		m_bTracked = FALSE;
	}

	m_bIsDroppedDown = TRUE;
	m_bIsButtonHighlighted = FALSE;
	RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW | RDW_ALLCHILDREN);

	return FALSE;
}
//**************************************************************************
void CBCGPComboBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ((nFlags & MK_LBUTTON) == 0)
	{
		BOOL bIsButtonHighlighted = m_bIsButtonHighlighted;
		m_bIsButtonHighlighted = m_rectBtn.PtInRect (point);

		if (bIsButtonHighlighted != m_bIsButtonHighlighted)
		{
			if (!m_bTracked)
			{
				if (m_bIsButtonHighlighted)
				{
					SetCapture ();
					m_bTracked = TRUE;
				}
			}
			else
			{
				if (!m_bIsButtonHighlighted)
				{
					ReleaseCapture ();
					m_bTracked = FALSE;
				}
			}
		
			RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW | RDW_ALLCHILDREN);
		}
	}
	
	CComboBox::OnMouseMove(nFlags, point);
}
//*****************************************************************************************
void CBCGPComboBox::OnCancelMode() 
{
	CComboBox::OnCancelMode();
	
	if (m_bTracked)
	{
		ReleaseCapture ();
		m_bIsButtonHighlighted = FALSE;
		m_bTracked = FALSE;

		RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW | RDW_ALLCHILDREN);
	}
}
//**************************************************************************
void CBCGPComboBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_bTracked)
	{
		ReleaseCapture ();
		m_bIsButtonHighlighted = FALSE;
		m_bTracked = FALSE;

		RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW | RDW_ALLCHILDREN);
	}
	
	CComboBox::OnLButtonDown(nFlags, point);
}
