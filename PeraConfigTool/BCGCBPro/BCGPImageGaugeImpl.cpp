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
// BCGPImageGaugeImpl.cpp: implementation of the CBCGPImageGaugeImpl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "bcgcbpro.h"
#include "BCGPImageGaugeImpl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CBCGPImageGaugeImpl, CBCGPStaticGaugeImpl)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBCGPImageGaugeImpl::CBCGPImageGaugeImpl(UINT uiImageResID, CBCGPVisualContainer* pContainer) :
	CBCGPStaticGaugeImpl(pContainer)
{
	SetImage(uiImageResID);
}
//*******************************************************************************
CBCGPImageGaugeImpl::~CBCGPImageGaugeImpl()
{
}
//*******************************************************************************
void CBCGPImageGaugeImpl::OnDraw(CBCGPGraphicsManager* pGM, const CBCGPRect& /*rectClip*/, DWORD dwFlags)
{
	ASSERT_VALID(pGM);

	if (m_rect.IsRectEmpty() || !m_bIsVisible)
	{
		return;
	}

	if ((dwFlags & BCGP_DRAW_DYNAMIC) == 0)
	{
		return;
	}

	if (!m_bOff)
	{
		pGM->DrawImage(m_Image, m_rect.TopLeft());
	}
	
	SetDirty(FALSE);
}
//*******************************************************************************
void CBCGPImageGaugeImpl::SetImage(UINT uiImageResID, BOOL bRedraw)
{
	m_Image.Destroy();
	m_Image = CBCGPImage(uiImageResID);

	SetDirty();

	if (bRedraw)
	{
		Redraw();
	}
}
//*******************************************************************************
CBCGPSize CBCGPImageGaugeImpl::GetDefaultSize(CBCGPGraphicsManager* pGM, const CBCGPBaseVisualObject* /*pParentGauge*/)
{
	ASSERT_VALID(pGM);
	return  m_Image.GetSize(pGM);
}
