// PropertySheetCtrl.cpp : implementation file
//
// This is a part of the BCGControlBar Library
// Copyright (C) 1998-2011 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.

#include "stdafx.h"
#include "BCGCBPro.h"
#include "PropertySheetCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBCGPPropertySheetCtrl

IMPLEMENT_DYNAMIC(CBCGPPropertySheetCtrl, CBCGPPropertySheet)

CBCGPPropertySheetCtrl::CBCGPPropertySheetCtrl(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
:CBCGPPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_hAccel = NULL;
	m_bIsAutoDestroy = TRUE;
}

CBCGPPropertySheetCtrl::CBCGPPropertySheetCtrl(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
:CBCGPPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_hAccel = NULL;
	m_bIsAutoDestroy = TRUE;
}

CBCGPPropertySheetCtrl::CBCGPPropertySheetCtrl()
:CBCGPPropertySheet(_T(""), NULL, 0)
{
	m_hAccel = NULL;
	m_bIsAutoDestroy = TRUE;
}

CBCGPPropertySheetCtrl::~CBCGPPropertySheetCtrl()
{
}


BEGIN_MESSAGE_MAP(CBCGPPropertySheetCtrl, CBCGPPropertySheet)
	//{{AFX_MSG_MAP(CBCGPPropertySheetCtrl)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCGPPropertySheetCtrl message handlers

void CBCGPPropertySheetCtrl::PostNcDestroy()
{
	// Call the base class routine first
	CBCGPPropertySheet::PostNcDestroy();
	
	if (m_bModeless && m_bIsAutoDestroy)
	{
		delete this;
	}
}

BOOL CBCGPPropertySheetCtrl::OnInitDialog()
{
	ASSERT_VALID(this);
	
	// Call the base class routine
	BOOL bRtnValue = CBCGPPropertySheet::OnInitDialog();
	
	ModifyStyleEx(0, WS_EX_CONTROLPARENT);

	if (IsVisualManagerStyle ())
	{
		ModifyStyle(WS_BORDER, 0);
	}

	ResizeControl();
	return bRtnValue;
}

BOOL CBCGPPropertySheetCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR* pNMHDR = (NMHDR*) lParam;
	ASSERT (pNMHDR != NULL);

	if (pNMHDR->code == TCN_SELCHANGE)
	{
		ResizeControl ();
	}
	
	return CBCGPPropertySheet::OnNotify(wParam, lParam, pResult);
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
void CBCGPPropertySheetCtrl::LoadAcceleratorTable(UINT nAccelTableID /*=0*/)
{
	if (nAccelTableID)
	{
		m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(nAccelTableID));
		ASSERT(m_hAccel);
	}
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
BOOL CBCGPPropertySheetCtrl::PreTranslateMessage(MSG* pMsg)
{
	//TRACE("[%s - %d] - CBCGPPropertySheetCtrl::PreTranslateMessage().....\n", __FILE__,__LINE__);
	////////
	// Check to see if the property sheet has an accelerator table
	// attached to it. If there is one call it. Return TRUE if it has
	// been processed. Otherwise, past it to the base class function.
	////////
	if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
	{
		return TRUE;
	}

	return CBCGPPropertySheet::PreTranslateMessage(pMsg);
}

void CBCGPPropertySheetCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPPropertySheet::OnSize(nType, cx, cy);
	ResizeControl();
}

void CBCGPPropertySheetCtrl::ResizeControl()
{
	CWnd* pTabCtrl = GetTabControl();
	if (m_wndTab.GetSafeHwnd () != NULL)
	{
		pTabCtrl = &m_wndTab;
		m_wndTab.SetButtonsVisible (FALSE);
	}

	if (pTabCtrl->GetSafeHwnd () == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect (rectClient);

	pTabCtrl->SetWindowPos (NULL, 
		0, 0, rectClient.Width (), rectClient.Height (), 
		SWP_NOZORDER | SWP_NOACTIVATE);

	int nPageCount = CBCGPPropertySheet::GetPageCount();

	int nXBorder = ::GetSystemMetrics(SM_CXEDGE);
	int nYBorder = ::GetSystemMetrics(SM_CYEDGE);
	
	for (int nPage = 0; nPage <= nPageCount - 1; nPage++)
	{
		CPropertyPage* pPage = GetPage(nPage);
		
		if ((pPage != NULL) && (pPage->m_hWnd != NULL))
		{
			CRect rcTabCtrl;
			pPage->GetWindowRect(&rcTabCtrl);
			pTabCtrl->ScreenToClient(rcTabCtrl);
			
			pPage->SetWindowPos (NULL, 
				rcTabCtrl.left, rcTabCtrl.top, 
				rectClient.Width () - (nXBorder * 3), 
				rectClient.Height () - (rcTabCtrl.top + nYBorder), 
				SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}
}
