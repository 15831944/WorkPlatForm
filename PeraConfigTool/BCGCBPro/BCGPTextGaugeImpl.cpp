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
// BCGPTextGaugeImpl.cpp: implementation of the CBCGPTextGaugeImpl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "bcgcbpro.h"
#include "BCGPTextGaugeImpl.h"
#include "BCGPDrawManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CBCGPTextGaugeImpl, CBCGPStaticGaugeImpl)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBCGPTextGaugeImpl::CBCGPTextGaugeImpl(LPCTSTR lpszText, const CBCGPColor& color, CBCGPVisualContainer* pContainer) :
	CBCGPStaticGaugeImpl(pContainer)
{
	if (lpszText != NULL)
	{
		SetText(lpszText, color);
	}
}
//*******************************************************************************
CBCGPTextGaugeImpl::~CBCGPTextGaugeImpl()
{
}
//*******************************************************************************
void CBCGPTextGaugeImpl::OnDraw(CBCGPGraphicsManager* pGM, const CBCGPRect& /*rectClip*/, DWORD dwFlags)
{
	ASSERT_VALID(pGM);

	if (m_rect.IsRectEmpty() || !m_bIsVisible || m_strText.IsEmpty())
	{
		return;
	}

	if ((dwFlags & BCGP_DRAW_DYNAMIC) == 0)
	{
		return;
	}

	CBCGPRect rect = m_rect;

	const CBCGPBrush& br = m_bOff ? m_brTextLight : m_brText;

	CreateResources(CBCGPColor());
	pGM->DrawText(m_strText, rect, m_textFormat, br);
	
	SetDirty(FALSE);
}
//*******************************************************************************
void CBCGPTextGaugeImpl::SetText(const CString& str, const CBCGPColor& clrText, BOOL bRedraw)
{
	m_strText = str;
	CreateResources(clrText);

	SetDirty();

	if (bRedraw)
	{
		Redraw();
	}
}
//*******************************************************************************
void CBCGPTextGaugeImpl::CreateResources(const CBCGPColor& clrText)
{
	if (clrText != CBCGPColor())
	{
		m_brTextLight = m_brText = CBCGPBrush(clrText);

		COLORREF clr = m_brTextLight.GetColor();

		double H;
		double S;
		double L;

		CBCGPDrawManager::RGBtoHSL (clr, &H, &S, &L);

		m_brTextLight.MakeLighter(S < .99 ? .3 : .6);
	}

	if (m_textFormat.GetFontFamily().IsEmpty())
	{
		LOGFONT lf;
		globalData.fontBold.GetLogFont(&lf);

		m_textFormat.CreateFromLogFont(lf);

		m_textFormat.SetTextAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
		m_textFormat.SetTextVerticalAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	}
}
//*******************************************************************************
CBCGPSize CBCGPTextGaugeImpl::GetDefaultSize(CBCGPGraphicsManager* pGM, const CBCGPBaseVisualObject* /*pParentGauge*/)
{
	ASSERT_VALID(pGM);

	CreateResources(CBCGPColor());
	return pGM->GetTextSize(m_strText, m_textFormat);
}
//*******************************************************************************
void CBCGPTextGaugeImpl::SetTextFormat(const CBCGPTextFormat& textFormat)
{
	m_textFormat = textFormat;
	SetDirty();
}
//*******************************************************************************
void CBCGPTextGaugeImpl::SetTextColor(const CBCGPColor& color)
{
	CreateResources(color);
	SetDirty();
}
