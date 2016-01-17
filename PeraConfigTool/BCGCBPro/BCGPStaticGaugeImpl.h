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
// BCGPStaticGaugeImpl.h: interface for the CBCGPStaticGaugeImpl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BCGPSTATICGAUGEIMPL_H__2D040C84_CD25_430F_8680_C4131A778363__INCLUDED_)
#define AFX_BCGPSTATICGAUGEIMPL_H__2D040C84_CD25_430F_8680_C4131A778363__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BCGPGaugeImpl.h"

extern BCGCBPRODLLEXPORT UINT BCGM_ON_GAUGE_CLICK;

class BCGCBPRODLLEXPORT CBCGPStaticGaugeImpl : public CBCGPGaugeImpl  
{
	DECLARE_DYNAMIC(CBCGPStaticGaugeImpl)

public:
	CBCGPStaticGaugeImpl(CBCGPVisualContainer* pContainer = NULL);
	virtual ~CBCGPStaticGaugeImpl();

// Operations:
public:
	void StartFlashing(UINT nTime = 100 /* ms */);
	void StopFlashing();

	BOOL IsFlashing() const
	{
		return m_arData[0]->IsAnimated();
	}

	virtual CWnd* SetOwner(CWnd* pWndOwner, BOOL bRedraw = FALSE);

	virtual BOOL OnAnimation(UINT /*idEvent*/)
	{
		m_bOff = !m_bOff;
		return FALSE;
	}

	virtual void FireClickEvent(const CBCGPPoint& pt);

protected:
	virtual BOOL OnMouseDown(int nButton, const CBCGPPoint& pt);
	virtual void OnMouseUp(int nButton, const CBCGPPoint& pt);
	virtual void OnMouseMove(const CBCGPPoint& pt);
	virtual void OnCancelMode();

	virtual BOOL OnSetMouseCursor(const CBCGPPoint& pt);

protected:
	UINT	m_nFlashTime;
	BOOL	m_bOff;
	BOOL	m_bIsPressed;
};

#endif // !defined(AFX_BCGPSTATICGAUGEIMPL_H__2D040C84_CD25_430F_8680_C4131A778363__INCLUDED_)
