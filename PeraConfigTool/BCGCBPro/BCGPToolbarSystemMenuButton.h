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

// BCGPToolbarSystemMenuButton.h: interface for the CBCGPToolbarSystemMenuButton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BCGPTOOLBARSYSTEMMENUBUTTON_H__9BE97874_D37C_11D1_A649_00A0C93A70EC__INCLUDED_)
#define AFX_BCGPTOOLBARSYSTEMMENUBUTTON_H__9BE97874_D37C_11D1_A649_00A0C93A70EC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "BCGPToolbarMenuButton.h"

class BCGCBPRODLLEXPORT CBCGPToolbarSystemMenuButton : public CBCGPToolbarMenuButton  
{
	DECLARE_SERIAL(CBCGPToolbarSystemMenuButton)

public:
	CBCGPToolbarSystemMenuButton();
	CBCGPToolbarSystemMenuButton (HMENU hSystemMenu, HICON hSystemIcon);
	virtual ~CBCGPToolbarSystemMenuButton();

// Overrides:
	virtual void CopyFrom (const CBCGPToolbarButton& src);
	virtual SIZE OnCalculateSize (CDC* pDC, const CSize& sizeDefault, BOOL bHorz);
	virtual void OnDraw (CDC* pDC, const CRect& rect, CBCGPToolBarImages* pImages,
						BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,
						BOOL bHighlight = FALSE,
						BOOL bDrawBorder = TRUE,
						BOOL bGrayDisabledButtons = TRUE);
	virtual void OnDblClick (CWnd* pWnd);
	virtual BOOL CanBeStored () const			{	return FALSE;	}
	virtual BOOL HaveHotBorder () const			{	return FALSE;	}
	virtual void Serialize (CArchive& /*ar*/)	{	ASSERT (FALSE);	}
	virtual void OnCancelMode ();

	virtual void CreateFromMenu (HMENU hMenu);
	virtual HMENU CreateMenu () const;

	virtual BOOL IsBorder () const
	{
		return FALSE;
	}

	virtual void OnAfterCreatePopupMenu ();

/// Attributes:
public:
	HICON GetSysMenuIcon () const
	{
		return m_hSysMenuIcon;
	}

	HMENU GetSysMenu () const
	{
		return m_hSystemMenu;
	}

protected:
	HICON	m_hSysMenuIcon;
	HMENU	m_hSystemMenu;
};

#endif // !defined(AFX_BCGPTOOLBARSYSTEMMENUBUTTON_H__9BE97874_D37C_11D1_A649_00A0C93A70EC__INCLUDED_)
