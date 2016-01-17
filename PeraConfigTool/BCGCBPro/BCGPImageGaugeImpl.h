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
// BCGPImageGaugeImpl.h: interface for the CBCGPImageGaugeImpl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BCGPIMAGEGAUGEIMPL_H__9F10A362_8B4D_499E_BC20_6FDECBC95676__INCLUDED_)
#define AFX_BCGPIMAGEGAUGEIMPL_H__9F10A362_8B4D_499E_BC20_6FDECBC95676__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BCGPStaticGaugeImpl.h"
#include "BCGPGraphicsManager.h"

class BCGCBPRODLLEXPORT CBCGPImageGaugeImpl : public CBCGPStaticGaugeImpl  
{
	DECLARE_DYNCREATE(CBCGPImageGaugeImpl)

public:
	CBCGPImageGaugeImpl(UINT uiImageResID = 0, CBCGPVisualContainer* pContainer = NULL);
	virtual ~CBCGPImageGaugeImpl();

// Overrides:
public:
	virtual void OnDraw(CBCGPGraphicsManager* pGM, const CBCGPRect& rectClip, DWORD dwFlags = BCGP_DRAW_STATIC | BCGP_DRAW_DYNAMIC);
	virtual CBCGPSize GetDefaultSize(CBCGPGraphicsManager* pGM, const CBCGPBaseVisualObject* pParentGauge);

// Attributes:
public:
	void SetImage(UINT uiImageResID, BOOL bRedraw = FALSE);

protected:
	CBCGPImage	m_Image;
};

#endif // !defined(AFX_BCGPIMAGEGAUGEIMPL_H__9F10A362_8B4D_499E_BC20_6FDECBC95676__INCLUDED_)
