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
// BCGPVisualCtrl.h : header file
//

#if !defined(AFX_BCGPVISUALCTRL_H__E8F1CF04_6B23_493B_97EE_53AD84583B04__INCLUDED_)
#define AFX_BCGPVISUALCTRL_H__E8F1CF04_6B23_493B_97EE_53AD84583B04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bcgcbpro.h"
#include "BCGPGraphicsManager.h"
#include "BCGPVisualContainer.h"

/////////////////////////////////////////////////////////////////////////////
// CBCGPBaseVisualCtrl window

class BCGCBPRODLLEXPORT CBCGPBaseVisualCtrl : public CStatic
{
	DECLARE_DYNAMIC(CBCGPBaseVisualCtrl)

// Construction
public:
	CBCGPBaseVisualCtrl();

// Operations
public:
	void SetGraphicsManager(CBCGPGraphicsManager::BCGP_GRAPHICS_MANAGER manager);
	void SetGraphicsManager(CRuntimeClass* pRTI);

	void EnableTooltip(BOOL bEnable = TRUE);

	BOOL CreatePopup(const CRect& rect, BYTE nTransparency = 255 /* from 0 to 255 */, CWnd* pWndOwner = NULL);
	static BOOL HasActivePopup()
	{
		return m_hwndHookedPopup != NULL;
	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBCGPBaseVisualCtrl)
	public:
	virtual BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwStyle = WS_CHILD | WS_VISIBLE, CCreateContext* pContext = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

public:
	virtual void InitCtrl() = 0;

	virtual CBCGPRect GetRect() = 0;
	virtual void SetRect(const CBCGPRect& rect, BOOL bRedraw = FALSE) = 0;

	virtual BOOL IsDirty() = 0;
	virtual void SetDirty(BOOL bSet) = 0;

	virtual void OnDraw(CBCGPGraphicsManager* pGM, const CBCGPRect& rectClip) = 0;

	virtual BOOL CopyToClipboard() = 0;
	virtual BOOL ExportToFile(const CString& strFilePath) = 0;

	// Mose events:
	virtual BOOL OnMouseDown(int nButton, const CBCGPPoint& pt) = 0;
	virtual void OnMouseUp(int nButton, const CBCGPPoint& pt) = 0;
	virtual void OnMouseMove(const CBCGPPoint& pt) = 0;
	virtual void OnMouseLeave() = 0;
	virtual BOOL OnMouseWheel(const CBCGPPoint& pt, short zDelta) = 0;
	virtual void OnCancelMode() = 0;

	virtual BOOL OnSetMouseCursor(const CBCGPPoint& pt) = 0;

	virtual BOOL IsCloseOnClick(CPoint ptScreen);

	// Keyboard events:
	virtual BOOL OnKeyboardDown(UINT nChar, UINT nRepCnt, UINT nFlags) = 0;
	virtual BOOL OnKeyboardUp(UINT nChar, UINT nRepCnt, UINT nFlags) = 0;

	// Tooltip:
	virtual BOOL OnGetToolTip(const CBCGPPoint& pt, CString& strToolTip, CString& strDescr) = 0;

	BOOL IsFocused() const
	{
		return m_bIsFocused;
	}

	BOOL IsPopup() const
	{
		return m_bIsPopup;
	}

	virtual void OnDrawLayeredPopup();

// Implementation
public:
	virtual ~CBCGPBaseVisualCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBCGPBaseVisualCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnDestroy();
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnNcDestroy();
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	afx_msg LRESULT OnBCGUpdateToolTips(WPARAM, LPARAM);
	afx_msg BOOL OnTTNeedTipText(UINT id, NMHDR* pNMH, LRESULT* pResult);
	afx_msg LRESULT OnFloatStatus(WPARAM, LPARAM);
	afx_msg LRESULT OnBCGPostRedraw(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()

	static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);

	void InitTooltip();

	CBCGPGraphicsManager*	m_pGM;
	BOOL					m_bIsCaptured;
	BOOL					m_bIsTracked;
	BOOL					m_bIsTooltip;
	CToolTipCtrl*			m_pToolTip;
	BOOL					m_bToolTipCleared;
	CString					m_strLastDisplayedToolTip;
	BOOL					m_bIsFocused;
	UINT					m_nDlgCode;
	BOOL					m_bIsPopup;
	BYTE					m_nPopupAlpha;
	static HHOOK			m_hookMouse;
	static HWND				m_hwndHookedPopup;
};

/////////////////////////////////////////////////////////////////////////////
// CBCGPVisualCtrl window

class BCGCBPRODLLEXPORT CBCGPVisualCtrl : public CBCGPBaseVisualCtrl
{
	DECLARE_DYNAMIC(CBCGPVisualCtrl)

public:
	virtual CBCGPBaseVisualObject* GetVisualObject() = 0;

	virtual void InitCtrl()
	{
		CBCGPBaseVisualObject* pVisualObject = GetVisualObject();
		if (pVisualObject != NULL)
		{
			pVisualObject->SetOwner(this, FALSE);
		}
	}

	virtual CBCGPRect GetRect()
	{
		CBCGPBaseVisualObject* pVisualObject = GetVisualObject();
		if (pVisualObject != NULL)
		{
			return pVisualObject->GetRect();
		}

		return CBCGPRect();
	}

	CBCGPGraphicsManager* GetGraphicsManager()
	{
		if (m_pGM == NULL)
		{
			m_pGM = CBCGPGraphicsManager::CreateInstance();
		}

		return m_pGM;
	}

	virtual void SetRect(const CBCGPRect& rect, BOOL bRedraw = FALSE)
	{
		CBCGPBaseVisualObject* pVisualObject = GetVisualObject();
		if (pVisualObject != NULL)
		{
			const CBCGPSize sizeMin = pVisualObject->GetMinSize();
			
			if (sizeMin.IsEmpty())
			{
				pVisualObject->SetRect(rect, bRedraw);
			}
			else
			{
				CBCGPRect rectMin = rect;

				rectMin.SetWidth(max(rect.Width(), sizeMin.cx));
				rectMin.SetHeight(max(rect.Height(), sizeMin.cy));

				pVisualObject->SetRect(rectMin, bRedraw);
			}
		}
	}

	virtual BOOL IsDirty()
	{
		CBCGPBaseVisualObject* pVisualObject = GetVisualObject();
		if (pVisualObject != NULL)
		{
			return pVisualObject->IsDirty();
		}

		return FALSE;
	}

	virtual void SetDirty(BOOL bSet)
	{
		CBCGPBaseVisualObject* pVisualObject = GetVisualObject();
		if (pVisualObject != NULL)
		{
			pVisualObject->SetDirty(bSet);
		}
	}

	virtual void OnDraw(CBCGPGraphicsManager* pGM, const CBCGPRect& rectClip)
	{
		CBCGPBaseVisualObject* pVisualObject = GetVisualObject();
		if (pVisualObject != NULL)
		{
			pVisualObject->OnDraw(pGM, rectClip, BCGP_DRAW_STATIC);
			pVisualObject->OnDraw(pGM, rectClip, BCGP_DRAW_DYNAMIC);
		}
	}

	virtual BOOL CopyToClipboard()
	{
		CBCGPBaseVisualObject* pVisualObject = GetVisualObject();
		if (pVisualObject == NULL)
		{
			return FALSE;
		}

		if (m_pGM == NULL)
		{
			m_pGM = CBCGPGraphicsManager::CreateInstance();
		}

		return pVisualObject->CopyToClipboard(m_pGM);
	}

	virtual BOOL ExportToFile(const CString& strFilePath)
	{
		CBCGPBaseVisualObject* pVisualObject = GetVisualObject();
		if (pVisualObject == NULL)
		{
			return FALSE;
		}

		if (m_pGM == NULL)
		{
			m_pGM = CBCGPGraphicsManager::CreateInstance();
		}

		return pVisualObject->ExportToFile(strFilePath, m_pGM);
	}

	virtual BOOL OnMouseDown(int nButton, const CBCGPPoint& pt)
	{
		CBCGPBaseVisualObject* pVisualObject = GetVisualObject();
		if (pVisualObject != NULL && pVisualObject->GetRect().PtInRect(pt))
		{
			return pVisualObject->OnMouseDown(nButton, pt);
		}

		return FALSE;
	}

	virtual void OnMouseUp(int nButton, const CBCGPPoint& pt)
	{
		CBCGPBaseVisualObject* pVisualObject = GetVisualObject();
		if (pVisualObject != NULL && (m_bIsCaptured || pVisualObject->GetRect().PtInRect(pt)))
		{
			pVisualObject->OnMouseUp(nButton, pt);
		}
	}

	virtual void OnMouseMove(const CBCGPPoint& pt)
	{
		CBCGPBaseVisualObject* pVisualObject = GetVisualObject();
		if (pVisualObject != NULL && (m_bIsCaptured || pVisualObject->GetRect().PtInRect(pt)))
		{
			pVisualObject->OnMouseMove(pt);
		}
	}

	virtual void OnMouseLeave()
	{
		CBCGPBaseVisualObject* pVisualObject = GetVisualObject();
		if (pVisualObject != NULL)
		{
			pVisualObject->OnMouseLeave();
		}
	}

	virtual BOOL OnMouseWheel(const CBCGPPoint& pt, short zDelta)
	{
		CBCGPBaseVisualObject* pVisualObject = GetVisualObject();
		if (pVisualObject != NULL && pVisualObject->GetRect().PtInRect(pt))
		{
			return pVisualObject->OnMouseWheel(pt, zDelta);
		}

		return FALSE;
	}

	virtual BOOL OnSetMouseCursor(const CBCGPPoint& pt)
	{
		CBCGPBaseVisualObject* pVisualObject = GetVisualObject();
		if (pVisualObject != NULL && pVisualObject->GetRect().PtInRect(pt))
		{
			return pVisualObject->OnSetMouseCursor(pt);
		}

		return FALSE;
	}

	virtual void OnCancelMode()
	{
		CBCGPBaseVisualObject* pVisualObject = GetVisualObject();
		if (pVisualObject != NULL)
		{
			pVisualObject->OnCancelMode();
		}
	}

	virtual BOOL OnKeyboardDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		CBCGPBaseVisualObject* pVisualObject = GetVisualObject();
		if (pVisualObject != NULL)
		{
			return pVisualObject->OnKeyboardDown(nChar, nRepCnt, nFlags);
		}

		return FALSE;
	}

	virtual BOOL OnKeyboardUp(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		CBCGPBaseVisualObject* pVisualObject = GetVisualObject();
		if (pVisualObject != NULL)
		{
			return pVisualObject->OnKeyboardUp(nChar, nRepCnt, nFlags);
		}

		return FALSE;
	}

	virtual BOOL OnGetToolTip(const CBCGPPoint& pt, CString& strToolTip, CString& strDescr)
	{
		CBCGPBaseVisualObject* pVisualObject = GetVisualObject();
		if (pVisualObject != NULL && pVisualObject->GetRect().PtInRect(pt))
		{
			return pVisualObject->OnGetToolTip(pt, strToolTip, strDescr);
		}

		return FALSE;
	}
};

/////////////////////////////////////////////////////////////////////////////
// CBCGPVisualContainerCtrl window

class BCGCBPRODLLEXPORT CBCGPVisualContainerCtrl : public CBCGPBaseVisualCtrl
{
	DECLARE_DYNAMIC(CBCGPVisualContainerCtrl)

public:
	CBCGPVisualContainerCtrl()
	{
		m_pContainer = NULL;
	}

	virtual ~CBCGPVisualContainerCtrl()
	{
		if (m_pContainer != NULL)
		{
			delete m_pContainer;
		}
	}

	virtual CBCGPVisualContainer* GetVisualContainer()
	{
		if (m_pContainer == NULL)
		{
			m_pContainer = new CBCGPVisualContainer;
		}

		return m_pContainer;
	}

protected:
	virtual void InitCtrl()
	{
		CBCGPVisualContainer* pVisualContainer = GetVisualContainer();
		if (pVisualContainer != NULL)
		{
			pVisualContainer->SetOwner(this, FALSE);
		}
	}

	virtual CBCGPRect GetRect()
	{
		CBCGPVisualContainer* pVisualContainer = GetVisualContainer();
		if (pVisualContainer != NULL)
		{
			return pVisualContainer->GetRect();
		}

		return CBCGPRect();
	}

	virtual void SetRect(const CBCGPRect& rect, BOOL bRedraw = FALSE)
	{
		CBCGPVisualContainer* pVisualContainer = GetVisualContainer();
		if (pVisualContainer != NULL)
		{
			pVisualContainer->SetRect(rect, bRedraw);
		}
	}

	virtual BOOL IsDirty()
	{
		CBCGPVisualContainer* pVisualContainer = GetVisualContainer();
		if (pVisualContainer != NULL)
		{
			return pVisualContainer->IsDirty();
		}

		return FALSE;
	}

	virtual void SetDirty(BOOL bSet)
	{
		CBCGPVisualContainer* pVisualContainer = GetVisualContainer();
		if (pVisualContainer != NULL)
		{
			pVisualContainer->SetDirty(bSet);
		}
	}

	virtual BOOL OnMouseDown(int nButton, const CBCGPPoint& pt)
	{
		CBCGPVisualContainer* pVisualContainer = GetVisualContainer();
		if (pVisualContainer != NULL && pVisualContainer->GetRect().PtInRect(pt))
		{
			return pVisualContainer->OnMouseDown(nButton, pt);
		}

		return FALSE;
	}

	virtual void OnMouseUp(int nButton, const CBCGPPoint& pt)
	{
		CBCGPVisualContainer* pVisualContainer = GetVisualContainer();
		if (pVisualContainer != NULL && (m_bIsCaptured || pVisualContainer->GetRect().PtInRect(pt)))
		{
			pVisualContainer->OnMouseUp(nButton, pt);
		}
	}

	virtual void OnMouseMove(const CBCGPPoint& pt)
	{
		CBCGPVisualContainer* pVisualContainer = GetVisualContainer();
		if (pVisualContainer != NULL && (m_bIsCaptured || pVisualContainer->GetRect().PtInRect(pt)))
		{
			pVisualContainer->OnMouseMove(pt);
		}
	}

	virtual void OnMouseLeave()
	{
		CBCGPVisualContainer* pVisualContainer = GetVisualContainer();
		if (pVisualContainer != NULL)
		{
			pVisualContainer->OnMouseLeave();
		}
	}

	virtual BOOL OnMouseWheel(const CBCGPPoint& pt, short zDelta)
	{
		CBCGPVisualContainer* pVisualContainer = GetVisualContainer();
		if (pVisualContainer != NULL && pVisualContainer->GetRect().PtInRect(pt))
		{
			return pVisualContainer->OnMouseWheel(pt, zDelta);
		}

		return FALSE;
	}

	virtual BOOL OnSetMouseCursor(const CBCGPPoint& pt)
	{
		CBCGPVisualContainer* pVisualContainer = GetVisualContainer();
		if (pVisualContainer != NULL && pVisualContainer->GetRect().PtInRect(pt))
		{
			return pVisualContainer->OnSetMouseCursor(pt);
		}

		return FALSE;
	}
	
	virtual void OnCancelMode()
	{
		CBCGPVisualContainer* pVisualContainer = GetVisualContainer();
		if (pVisualContainer != NULL)
		{
			pVisualContainer->OnCancelMode();
		}
	}

	virtual BOOL OnKeyboardDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		CBCGPVisualContainer* pVisualContainer = GetVisualContainer();
		if (pVisualContainer != NULL)
		{
			return pVisualContainer->OnKeyboardDown(nChar, nRepCnt, nFlags);
		}

		return FALSE;
	}

	virtual BOOL OnKeyboardUp(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		CBCGPVisualContainer* pVisualContainer = GetVisualContainer();
		if (pVisualContainer != NULL)
		{
			return pVisualContainer->OnKeyboardUp(nChar, nRepCnt, nFlags);
		}

		return FALSE;
	}

	virtual BOOL OnGetToolTip(const CBCGPPoint& pt, CString& strToolTip, CString& strDescr)
	{
		CBCGPVisualContainer* pVisualContainer = GetVisualContainer();
		if (pVisualContainer != NULL && pVisualContainer->GetRect().PtInRect(pt))
		{
			return pVisualContainer->OnGetToolTip(pt, strToolTip, strDescr);
		}

		return FALSE;
	}

	virtual void OnDraw(CBCGPGraphicsManager* pGM, const CBCGPRect& rectClip)
	{
		CBCGPVisualContainer* pVisualContainer = GetVisualContainer();
		if (pVisualContainer != NULL)
		{
			pVisualContainer->OnDraw(pGM, rectClip);
		}
	}

	virtual BOOL CopyToClipboard()
	{
		CBCGPVisualContainer* pVisualContainer = GetVisualContainer();
		if (pVisualContainer == NULL)
		{
			return FALSE;
		}

		if (m_pGM == NULL)
		{
			m_pGM = CBCGPGraphicsManager::CreateInstance();
		}

		return pVisualContainer->CopyToClipboard(m_pGM);
	}

	virtual BOOL ExportToFile(const CString& strFilePath)
	{
		CBCGPVisualContainer* pVisualContainer = GetVisualContainer();
		if (pVisualContainer == NULL)
		{
			return FALSE;
		}

		if (m_pGM == NULL)
		{
			m_pGM = CBCGPGraphicsManager::CreateInstance();
		}

		return pVisualContainer->ExportToFile(strFilePath, m_pGM);
	}

	CBCGPVisualContainer*	m_pContainer;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCGPVISUALCTRL_H__E8F1CF04_6B23_493B_97EE_53AD84583B04__INCLUDED_)
