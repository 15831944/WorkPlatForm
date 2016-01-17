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
// BCGPColorIndicatorImpl.cpp: implementation of the CBCGPColorIndicatorImpl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BCGPColorIndicatorImpl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CBCGPColorIndicatorColors::CBCGPColorIndicatorColors()
{
	m_brFill = CBCGPBrush(CBCGPColor::LimeGreen, CBCGPColor::White, CBCGPBrush::BCGP_GRADIENT_RADIAL_TOP_LEFT);
	m_brOutline.SetColor(m_brFill.GetColor());
	m_brOutline.MakeDarker(.1);
}

IMPLEMENT_DYNCREATE(CBCGPColorIndicatorImpl, CBCGPStaticGaugeImpl)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBCGPColorIndicatorImpl::CBCGPColorIndicatorImpl(CBCGPVisualContainer* pContainer) :
	CBCGPStaticGaugeImpl(pContainer)
{
	m_Style = BCGP_COLOR_INDICATOR_ELLIPSE;
	m_bStretched = FALSE;

	CreateResources();
}
//*******************************************************************************
CBCGPColorIndicatorImpl::CBCGPColorIndicatorImpl(const CBCGPColor& color,
							const CBCGPColor& clrGradient, 
							const CBCGPColor& clrBorder,
							CBCGPVisualContainer* pContainer) :
	CBCGPStaticGaugeImpl(pContainer)
{
	m_Style = BCGP_COLOR_INDICATOR_ELLIPSE;
	m_bStretched = FALSE;

	SetColor(color, clrGradient, clrBorder);
}
//*******************************************************************************
CBCGPColorIndicatorImpl::~CBCGPColorIndicatorImpl()
{
}
//*******************************************************************************
void CBCGPColorIndicatorImpl::OnDraw(CBCGPGraphicsManager* pGM, const CBCGPRect& /*rectClip*/, DWORD dwFlags)
{
	ASSERT_VALID(pGM);

	if (m_rect.IsRectEmpty() || !m_bIsVisible)
	{
		return;
	}

	if ((dwFlags & BCGP_DRAW_STATIC) == 0)
	{
		return;
	}

	CBCGPRect rect = m_rect;
	
	if (!m_bStretched)
	{
		if (rect.Width() < rect.Height())
		{
			rect.top += (rect.Height() - rect.Width()) / 2;
			rect.bottom = rect.top + rect.Width();
		}
		else if (rect.Height() < rect.Width())
		{
			rect.left += (rect.Width() - rect.Height()) / 2;
			rect.right = rect.left + rect.Height();
		}
	}

	const CBCGPColorIndicatorColors& colors = m_bOff ? m_LightColors : m_Colors;

	if (m_Style == BCGP_COLOR_INDICATOR_ELLIPSE)
	{
		CBCGPEllipse ellipse(rect);

		pGM->SetClipEllipse(ellipse);

		pGM->FillRectangle(rect, colors.m_brFill);

		pGM->ReleaseClipArea();

		pGM->DrawEllipse(ellipse, colors.m_brOutline);

		if (pGM->GetType() == CBCGPGraphicsManager::BCGP_GRAPHICS_MANAGER_GDI)
		{
			ellipse.radiusX = ellipse.radiusX - 1.;
			ellipse.radiusY = ellipse.radiusY - 1.;

			pGM->DrawEllipse(ellipse, colors.m_brOutline);
		}
	}
	else
	{
		pGM->FillRectangle(rect, colors.m_brFill);

		pGM->DrawRectangle(rect, colors.m_brOutline);

		if (pGM->GetType() == CBCGPGraphicsManager::BCGP_GRAPHICS_MANAGER_GDI)
		{
			rect.DeflateRect(1, 1);
			pGM->DrawRectangle(rect, colors.m_brOutline);
		}
	}
}
//*******************************************************************************
void CBCGPColorIndicatorImpl::SetColor(const CBCGPColor& color, const CBCGPColor& clrGradient, const CBCGPColor& clrBorder, BOOL bRedraw)
{
	if (m_Colors.m_brFill.GetColor() == color &&
		m_Colors.m_brFill.GetGradientColor() == clrGradient &&
		m_Colors.m_brOutline.GetColor() == clrBorder)
	{
		return;
	}

	m_Colors.m_brFill = CBCGPBrush(color, 
		clrGradient.IsNull() ? m_Colors.m_brFill.GetGradientColor() : clrGradient, 
		CBCGPBrush::BCGP_GRADIENT_RADIAL_TOP_LEFT);

	if (!clrBorder.IsNull())
	{
		m_Colors.m_brOutline.SetColor(clrBorder);
	}
	else
	{
		m_Colors.m_brOutline.SetColor(color);
		m_Colors.m_brOutline.MakeDarker(.1);
	}

	CreateResources();
	SetDirty();

	if (bRedraw)
	{
		Redraw();
	}
}
//*******************************************************************************
void CBCGPColorIndicatorImpl::CreateResources()
{
	m_LightColors = m_Colors;

	m_LightColors.m_brFill.MakeLighter(.3);
	m_LightColors.m_brOutline.MakeLighter(.3);
}
//*******************************************************************************
CBCGPSize CBCGPColorIndicatorImpl::GetDefaultSize(CBCGPGraphicsManager* /*pGM*/, const CBCGPBaseVisualObject* /*pParentGauge*/)
{
	return CBCGPSize(15., 15.);
}
