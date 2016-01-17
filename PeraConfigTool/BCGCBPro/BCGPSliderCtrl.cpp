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
// BCGPSliderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "BCGPVisualManager.h"
#include "bcgglobals.h"
#include "BCGPSliderCtrl.h"
#include "BCGPDlgImpl.h"
#ifndef _BCGSUITE_
#include "BCGPBaseControlBar.h"
#include "BCGPToolBarImages.h"
#endif
#include "trackmouse.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBCGPSliderCtrl

IMPLEMENT_DYNAMIC(CBCGPSliderCtrl, CSliderCtrl)

CBCGPSliderCtrl::CBCGPSliderCtrl()
{
	m_bVisualManagerStyle = FALSE;
	m_bOnGlass = FALSE;
	m_bTracked = FALSE;
	m_bIsThumbHighligted = FALSE;
	m_bIsThumPressed = FALSE;
}

CBCGPSliderCtrl::~CBCGPSliderCtrl()
{
}

BEGIN_MESSAGE_MAP(CBCGPSliderCtrl, CSliderCtrl)
	//{{AFX_MSG_MAP(CBCGPSliderCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_CANCELMODE()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_REGISTERED_MESSAGE(BCGM_ONSETCONTROLVMMODE, OnBCGSetControlVMMode)
	ON_REGISTERED_MESSAGE(BCGM_ONSETCONTROLAERO, OnBCGSetControlAero)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCGPSliderCtrl message handlers

BOOL CBCGPSliderCtrl::OnEraseBkgnd(CDC* /*pDC*/)
{
	if (!m_bVisualManagerStyle)
	{
		return (BOOL) Default ();
	}

	return TRUE;
}
//**************************************************************************
LRESULT CBCGPSliderCtrl::OnBCGSetControlVMMode (WPARAM wp, LPARAM)
{
	m_bVisualManagerStyle = (BOOL) wp;
	RedrawWindow ();
	return 0;
}
//**************************************************************************
LRESULT CBCGPSliderCtrl::OnBCGSetControlAero (WPARAM wp, LPARAM)
{
	m_bOnGlass = (BOOL) wp;
	return 0;
}
//**************************************************************************
void CBCGPSliderCtrl::OnCancelMode() 
{
	CSliderCtrl::OnCancelMode();

	m_bIsThumbHighligted = FALSE;
	m_bIsThumPressed = FALSE;

	RedrawWindow ();
}
//*****************************************************************************************
void CBCGPSliderCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	BOOL bIsThumbHighligted = m_bIsThumbHighligted;

	CRect rectThumb;
	GetThumbRect (rectThumb);

	m_bIsThumbHighligted = rectThumb.PtInRect (point);

	CSliderCtrl::OnMouseMove(nFlags, point);

	if (bIsThumbHighligted != m_bIsThumbHighligted)
	{
		RedrawWindow ();
	}

	if (!m_bTracked)
	{
		m_bTracked = TRUE;
		
		TRACKMOUSEEVENT trackmouseevent;
		trackmouseevent.cbSize = sizeof(trackmouseevent);
		trackmouseevent.dwFlags = TME_LEAVE;
		trackmouseevent.hwndTrack = GetSafeHwnd();
		trackmouseevent.dwHoverTime = HOVER_DEFAULT;
		::BCGPTrackMouse (&trackmouseevent);	
	}
}
//*****************************************************************************************
LRESULT CBCGPSliderCtrl::OnMouseLeave(WPARAM,LPARAM)
{
	m_bTracked = FALSE;

	if (m_bIsThumbHighligted)
	{
		m_bIsThumbHighligted = FALSE;
		RedrawWindow ();
	}

	return 0;
}
//*****************************************************************************************
void CBCGPSliderCtrl::OnPaint() 
{
	if (!m_bVisualManagerStyle)
	{
		Default ();
		return;
	}

	CPaintDC dc(this); // device context for painting
	
	CBCGPMemDC memDC (dc, this);
	CDC* pDC = &memDC.GetDC ();

	if (!CBCGPVisualManager::GetInstance ()->OnFillParentBarBackground (this, pDC))
	{
		globalData.DrawParentBackground (this, pDC, NULL);
	}

	DWORD dwStyle = GetStyle ();
	BOOL bVert = (dwStyle & TBS_VERT);
	BOOL bLeftTop = (dwStyle & TBS_BOTH) || (dwStyle & TBS_LEFT);
	BOOL bRightBottom = (dwStyle & TBS_BOTH) || ((dwStyle & TBS_LEFT) == 0);
	BOOL bIsFocused = GetSafeHwnd () == ::GetFocus ();

	CRect rectChannel;
	GetChannelRect (rectChannel);

	if (bVert)
	{
		CRect rect = rectChannel;

		rectChannel.left = rect.top;
		rectChannel.right = rect.bottom;
		rectChannel.top = rect.left;
		rectChannel.bottom = rect.right;
	}

	CBCGPDrawOnGlass dog(m_bOnGlass);

	CBCGPVisualManager::GetInstance ()->OnDrawSliderChannel (pDC, this, bVert, rectChannel, m_bOnGlass);

	CRect rectThumb;
	GetThumbRect (rectThumb);

	CBCGPVisualManager::GetInstance ()->OnDrawSliderThumb (
		pDC, this, rectThumb, m_bIsThumbHighligted || bIsFocused,
		m_bIsThumPressed, !IsWindowEnabled (),
		bVert, bLeftTop, bRightBottom, m_bOnGlass);

	if (bIsFocused && m_bDrawFocus)
	{
		CRect rectFocus;
		GetClientRect (rectFocus);

		pDC->DrawFocusRect (rectFocus);
	}
}
//*****************************************************************************************
void CBCGPSliderCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bIsThumPressed)
	{
		m_bIsThumPressed = FALSE;
		RedrawWindow ();
	}
	
	CSliderCtrl::OnLButtonUp(nFlags, point);
}
//*****************************************************************************************
void CBCGPSliderCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect rectThumb;
	GetThumbRect (rectThumb);

	m_bIsThumPressed = rectThumb.PtInRect (point);
	
	if (m_bIsThumPressed)
	{
		RedrawWindow ();
	}
	
	CSliderCtrl::OnLButtonDown(nFlags, point);
}
