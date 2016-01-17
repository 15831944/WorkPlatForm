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
// BCGPStaticGaugeImpl.cpp: implementation of the CBCGPStaticGaugeImpl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "bcgcbpro.h"
#include "BCGPStaticGaugeImpl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

UINT BCGM_ON_GAUGE_CLICK = ::RegisterWindowMessage (_T("BCGM_ON_GAUGE_CLICK"));

IMPLEMENT_DYNAMIC(CBCGPStaticGaugeImpl, CBCGPGaugeImpl)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBCGPStaticGaugeImpl::CBCGPStaticGaugeImpl(CBCGPVisualContainer* pContainer) :
	CBCGPGaugeImpl(pContainer)
{
	m_arData.Add(new CBCGPGaugeDataObject);
	m_nFlashTime = 0;
	m_bOff = FALSE;
	m_bIsPressed = FALSE;
}
//*******************************************************************************
CBCGPStaticGaugeImpl::~CBCGPStaticGaugeImpl()
{
}
//*******************************************************************************
void CBCGPStaticGaugeImpl::StartFlashing(UINT nTime)
{
	if (IsFlashing() || nTime == 0)
	{
		return;
	}

	m_arData[0]->SetAnimationID((UINT) ::SetTimer (NULL, 0, nTime, AnimTimerProc));

	g_cs.Lock ();
	m_mapAnimations.SetAt (m_arData[0]->GetAnimationID(), this);
	g_cs.Unlock ();

	m_nFlashTime = nTime;
}
//*******************************************************************************
void CBCGPStaticGaugeImpl::StopFlashing()
{
	if (m_arData[0]->GetAnimationID() > 0)
	{
		::KillTimer(NULL, m_arData[0]->GetAnimationID());
		m_arData[0]->SetAnimationID(0);

		m_bOff = FALSE;
		Redraw();
	}
}
//*******************************************************************************
CWnd* CBCGPStaticGaugeImpl::SetOwner(CWnd* pWndOwner, BOOL bRedraw)
{
	BOOL bIsFlashing = IsFlashing();

	CWnd* pWndRes = CBCGPGaugeImpl::SetOwner(pWndOwner, bRedraw);

	if (bIsFlashing)
	{
		StartFlashing(m_nFlashTime);
	}

	return pWndRes;
}
//*******************************************************************************
BOOL CBCGPStaticGaugeImpl::OnMouseDown(int nButton, const CBCGPPoint& pt)
{
	if (!m_bIsInteractiveMode || nButton != 0)
	{
		return CBCGPGaugeImpl::OnMouseDown(nButton, pt);
	}

	m_bIsPressed = TRUE;
	return TRUE;
}
//*******************************************************************************
void CBCGPStaticGaugeImpl::OnMouseUp(int nButton, const CBCGPPoint& pt)
{
	if (!m_bIsInteractiveMode || nButton != 0)
	{
		CBCGPGaugeImpl::OnMouseUp(nButton, pt);
		return;
	}

	if (!m_bIsPressed)
	{
		return;
	}

	m_bIsPressed = FALSE;

	if (m_rect.PtInRect(pt))
	{
		FireClickEvent(pt);
	}
}
//*******************************************************************************
void CBCGPStaticGaugeImpl::OnMouseMove(const CBCGPPoint& pt)
{
	if (!m_bIsPressed)
	{
		CBCGPGaugeImpl::OnMouseMove(pt);
	}
}
//*******************************************************************************
void CBCGPStaticGaugeImpl::OnCancelMode()
{
	if (!m_bIsPressed)
	{
		CBCGPGaugeImpl::OnCancelMode();
		return;
	}

	m_bIsPressed = FALSE;
}
//*******************************************************************************
void CBCGPStaticGaugeImpl::FireClickEvent(const CBCGPPoint& pt)
{
	if (m_pWndOwner->GetSafeHwnd() == NULL)
	{
		return;
	}

	CWnd* pOwner = m_pWndOwner->GetOwner();
	if (pOwner->GetSafeHwnd() == NULL)
	{
		return;
	}

	m_pWndOwner->GetOwner()->SendMessage(BCGM_ON_GAUGE_CLICK, (WPARAM)GetID(), MAKELPARAM(pt.x, pt.y));
}
//*******************************************************************************
BOOL CBCGPStaticGaugeImpl::OnSetMouseCursor(const CBCGPPoint& pt)
{
	if (m_bIsInteractiveMode)
	{
		::SetCursor (globalData.GetHandCursor());
		return TRUE;
	}

	return CBCGPGaugeImpl::OnSetMouseCursor(pt);
}
