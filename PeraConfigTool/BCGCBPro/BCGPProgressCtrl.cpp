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
// BCGPProgressCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "BCGPVisualManager.h"
#ifndef _BCGSUITE_
#include "BCGPRibbonProgressBar.h"
#include "BCGPToolBarImages.h"
#endif
#include "bcgglobals.h"
#include "BCGPProgressCtrl.h"
#include "BCGPDlgImpl.h"

#ifndef _BCGSUITE_
	#define visualManagerMFC	CBCGPVisualManager::GetInstance ()
#else
	#define visualManagerMFC	CMFCVisualManager::GetInstance ()
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBCGPProgressCtrl

IMPLEMENT_DYNAMIC(CBCGPProgressCtrl, CProgressCtrl)

CBCGPProgressCtrl::CBCGPProgressCtrl()
{
	m_bVisualManagerStyle = FALSE;
	m_bOnGlass = FALSE;
}

CBCGPProgressCtrl::~CBCGPProgressCtrl()
{
}

BEGIN_MESSAGE_MAP(CBCGPProgressCtrl, CProgressCtrl)
	//{{AFX_MSG_MAP(CBCGPProgressCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(BCGM_ONSETCONTROLVMMODE, OnBCGSetControlVMMode)
	ON_REGISTERED_MESSAGE(BCGM_ONSETCONTROLAERO, OnBCGSetControlAero)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCGPProgressCtrl message handlers

BOOL CBCGPProgressCtrl::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return TRUE;
}
//**************************************************************************
void CBCGPProgressCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CBCGPMemDC memDC (dc, this);
	CDC* pDC = &memDC.GetDC ();

	if (!CBCGPVisualManager::GetInstance ()->OnFillParentBarBackground (this, pDC))
	{
		globalData.DrawParentBackground (this, pDC);
	}

	CRect rect;
	GetClientRect (rect);

	CRect rectProgress = rect;
	rectProgress.DeflateRect (1, 1);

	int nMin = 0;
	int nMax = 0;
	GetRange (nMin, nMax);

	CRect rectChunk (0, 0, 0, 0);
	
	if (nMax > nMin)
	{
		rectChunk = rectProgress;
		rectChunk.right = rectChunk.left + 
			GetPos () * rectChunk.Width () / (nMax - nMin);

		rectChunk.DeflateRect (1, 1);
	}

	CBCGPDrawOnGlass dog (m_bOnGlass);

#ifndef BCGP_EXCLUDE_RIBBON
	CBCGPRibbonProgressBar dummy;
	visualManagerMFC->OnDrawRibbonProgressBar (
		pDC, &dummy, rectProgress, rectChunk, FALSE);
#else
	visualManagerMFC->OnDrawStatusBarProgress (pDC, NULL,
										rectProgress, nMax - nMin, 
										GetPos (),
										globalData.clrHilite, 
										(COLORREF)-1, 
										(COLORREF)-1,
										FALSE);
#endif
}
//**************************************************************************
void CBCGPProgressCtrl::OnNcCalcSize(BOOL /*bCalcValidRects*/, NCCALCSIZE_PARAMS FAR* /*lpncsp*/) 
{
}
//**************************************************************************
void CBCGPProgressCtrl::OnNcPaint() 
{
}
//**************************************************************************
LRESULT CBCGPProgressCtrl::OnBCGSetControlVMMode (WPARAM wp, LPARAM)
{
	m_bVisualManagerStyle = (BOOL) wp;
	return 0;
}
//**************************************************************************
LRESULT CBCGPProgressCtrl::OnBCGSetControlAero (WPARAM wp, LPARAM)
{
	m_bOnGlass = (BOOL) wp;
	return 0;
}
