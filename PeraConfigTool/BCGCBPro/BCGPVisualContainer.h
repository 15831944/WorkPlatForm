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
// BCGPVisualContainer.h: interface for the CBCGPVisualContainer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BCGPVISUALCONTAINER_H__F3037CD5_27F5_47A1_9D7E_189F956D2CB9__INCLUDED_)
#define AFX_BCGPVISUALCONTAINER_H__F3037CD5_27F5_47A1_9D7E_189F956D2CB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXMT_H__
	#include <afxmt.h>
#endif

#include "bcgcbpro.h"
#include "BCGPGraphicsManager.h"

#ifndef _BCGPCHART_STANDALONE
#include "BCGPLayout.h"
#endif

// Object Drawing flags
#define BCGP_DRAW_STATIC	0x0001
#define BCGP_DRAW_DYNAMIC	0x0002

// Object Editing flags
#define BCGP_EDIT_NOMOVE					0x0001
#define BCGP_EDIT_NOSIZE					0x0002
#define BCGP_EDIT_SIZE_LOCK_ASPECT_RATIO	0x0004
#define BCGP_EDIT_NO_NORMALIZE_RECT			0x0008

// Container Editing flags
#define BCGP_CONTAINER_SINGLE_SEL			0x0001
#define	BCGP_CONTAINER_GRID					0x0002

////////////////////////////////////////////////////////////////
// CBCGPVisualDataObject

class BCGCBPRODLLEXPORT CBCGPVisualDataObject : public CObject
{
	DECLARE_DYNAMIC(CBCGPVisualDataObject)

// Construction
public:
	CBCGPVisualDataObject()
	{
		m_dblValue = 0.;
		m_dblAnimatedValue = 0.;
		m_dblAnimationStep = 0.;
		m_nAnimTimerID = 0;
	}

	CBCGPVisualDataObject(const CBCGPVisualDataObject & src)
	{
		CopyFrom(src);
	}

	CBCGPVisualDataObject & operator = (const CBCGPVisualDataObject & src)
	{
		CopyFrom(src);
		return *this;
	}

	virtual void CopyFrom(const CBCGPVisualDataObject & src)
	{
		m_dblValue = src.m_dblValue;
		m_dblAnimatedValue = src.m_dblAnimatedValue;
		m_dblAnimationStep = src.m_dblAnimationStep;
		m_nAnimTimerID = src.m_nAnimTimerID;
	}

	virtual ~CBCGPVisualDataObject()
	{
	}

// Attributes
public:
	double GetValue() const
	{
		return m_dblValue;
	}

	void SetValue(double dblValue)
	{
		m_dblValue = dblValue;
	}

	BOOL IsAnimated() const
	{
		return m_nAnimTimerID != 0;
	}

	UINT GetAnimationID() const
	{
		return m_nAnimTimerID;
	}

	void SetAnimationID(UINT id)
	{
		m_nAnimTimerID = id;
	}

	double GetAnimatedValue() const
	{
		return m_dblAnimatedValue;
	}

	void SetAnimatedValue(double dblValue)
	{
		m_dblAnimatedValue = dblValue;
	}

	double GetAnimationStep() const
	{
		return m_dblAnimationStep;
	}

	void SetAnimationStep(double dblStep)
	{
		m_dblAnimationStep = dblStep;
	}

protected:
	double		m_dblValue;
	double		m_dblAnimatedValue;
	UINT		m_nAnimTimerID;
	double		m_dblAnimationStep;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBCGPBaseVisualObject

class BCGCBPRODLLEXPORT CBCGPBaseVisualObject : public CObject  
{
	DECLARE_DYNAMIC(CBCGPBaseVisualObject)

	friend class CBCGPVisualContainer;

public:
	CBCGPBaseVisualObject(CBCGPVisualContainer* pContainer = NULL);
	virtual ~CBCGPBaseVisualObject();

// Attributes:
public:
	// General attributes:
	UINT GetID() const
	{
		return m_nID;
	}

	void SetID(UINT nID)
	{
		m_nID = nID;
	}

	CString	GetName() const
	{
		return m_strName;
	}

	void SetName(const CString& strName)
	{
		m_strName = strName;
	}

	CWnd* GetOwner() const
	{
		return m_pWndOwner;
	}

	virtual CWnd* SetOwner(CWnd* pWndOwner, BOOL bRedraw = FALSE);

	const CBCGPVisualContainer* GetParentContainer () const
	{
		return m_pParentContainer;
	}

	virtual void SetRect(const CBCGPRect& rect, BOOL bRedraw = FALSE);
	const CBCGPRect& GetRect() const
	{
		return m_rect;
	}

	BOOL IsEditMode() const;

	BOOL IsVisible() const
	{
		return m_bIsVisible;
	}

	void SetVisible(BOOL bIsVisible = TRUE, BOOL bRedraw = FALSE)
	{
		m_bIsVisible = bIsVisible;

		if (bRedraw)
		{
			Redraw();
		}
	}

	// Data attributes:
	int GetDataCount() const
	{
		return (int)m_arData.GetSize();
	}

	const CBCGPVisualDataObject* GetData(int nIndex) const
	{
		if (0 <= nIndex && nIndex < GetDataCount ())
		{
			return m_arData[nIndex];
		}

		return NULL;
	}

	virtual BOOL SetValue(double dblVal, int nIndex = 0, UINT uiAnimationTime = 100 /* Ms, 0 - no animation */, BOOL bRedraw = FALSE);
	double GetValue(int nIndex = 0) const;

	virtual void SetDirty(BOOL bSet = TRUE, BOOL bRedraw = FALSE)
	{
		m_bIsDirty = bSet;

		if (m_bIsDirty && bRedraw)
		{
			Redraw();
		}
	}

	BOOL IsDirty() const
	{
		return m_bIsDirty;
	}

	void EnableImageCache(BOOL bEnable = TRUE);
	BOOL IsImageCache() const
	{
		return m_bCacheImage;
	}

	void SetAutoDestroy(BOOL bAutoDestroy = TRUE)
	{
		m_bIsAutoDestroy = bAutoDestroy;
	}

	BOOL IsAutoDestroy() const
	{
		return m_bIsAutoDestroy;
	}

	// Selection attributes:
	BOOL IsSelected() const
	{
		return m_bIsSelected;
	}

	virtual void SetSelected(BOOL bSet = TRUE);

	UINT GetEditFlags() const
	{
		return m_uiEditFlags;
	}

	void SetEditFlags(UINT nFlags)
	{
		m_uiEditFlags = nFlags;
	}

	const CBCGPSize& GetScaleRatio() const
	{
		return m_sizeScaleRatio;
	}

	void SetScaleRatio(const CBCGPSize& sizeScaleRatio)
	{
		if (m_sizeScaleRatio == sizeScaleRatio)
		{
			return;
		}

		CBCGPSize sizeScaleRatioOld = m_sizeScaleRatio;
		m_sizeScaleRatio = sizeScaleRatio;
		
		OnScaleRatioChanged(sizeScaleRatioOld);
	}

// Operations:
public:
	virtual void OnDraw(CBCGPGraphicsManager* pGM, const CBCGPRect& rectClip, DWORD dwFlags = BCGP_DRAW_STATIC | BCGP_DRAW_DYNAMIC) = 0;
	virtual void Redraw();
	virtual void PostRedraw(BOOL bSetDirty = FALSE);

	void StopAllAnimations(BOOL bRedraw = FALSE);

	BOOL ExportToImage(CBCGPGraphicsManager* pGM, CBCGPImage& image);
	HBITMAP ExportToBitmap(CBCGPGraphicsManager* pGM);

	BOOL CopyToClipboard(CBCGPGraphicsManager* pGM);
	BOOL ExportToFile(const CString& strFilePath, CBCGPGraphicsManager* pGM);

	virtual CBCGPSize GetMinSize() const
	{
		return CBCGPSize(0, 0);
	}

// Overrides:
	virtual BOOL OnMouseDown(int /*nButton*/, const CBCGPPoint& /*pt*/)	{	return FALSE;	}
	virtual void OnMouseUp(int /*nButton*/, const CBCGPPoint& /*pt*/)	{}
	virtual void OnMouseMove(const CBCGPPoint& /*pt*/)					{}
	virtual void OnMouseLeave()											{}
	virtual BOOL OnMouseWheel(const CBCGPPoint& /*pt*/, short /*zDelta*/){	return FALSE;	}
	virtual BOOL OnSetMouseCursor(const CBCGPPoint& pt);
	virtual void OnCancelMode()											{}
	virtual BOOL OnGetToolTip(const CBCGPPoint& /*pt*/, CString& /*strToolTip*/, CString& /*strDescr*/)	{	return FALSE;	}

	virtual int HitTest(const CBCGPPoint& pt) const;

	virtual BOOL OnKeyboardDown(UINT /*nChar*/, UINT /*nRepCnt*/, UINT /*nFlags*/)	{	return FALSE;	}
	virtual BOOL OnKeyboardUp(UINT /*nChar*/, UINT /*nRepCnt*/, UINT /*nFlags*/)	{	return FALSE;	}

	virtual void DrawTrackingRect(CBCGPGraphicsManager* pGM,
		const CBCGPBrush& brOutline, const CBCGPBrush& brFill);

	virtual void OnScaleRatioChanged(const CBCGPSize& /*sizeScaleRatioOld*/)	{}

	virtual BOOL IsCaptured() const	{	return FALSE;	}

protected:
	static VOID CALLBACK AnimTimerProc (HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

	void StopAnimation(UINT idEvent, BOOL bRedraw);
	virtual BOOL OnAnimation(UINT idEvent);

	CBCGPVisualDataObject* GetAnimated(UINT uiID) const;

	virtual double GetAnimationRange(int /*nIndex*/)
	{
		return 0.;
	}

	virtual void SetTrackingRect(const CBCGPRect& rect);

	CBCGPRect MakeTrackMarker(double x, double y);
	CBCGPRect MakeTrackMarker(const CBCGPPoint& pt);

protected:
	UINT					m_nID;
	CString					m_strName;
	BOOL					m_bIsVisible;

	CBCGPRect				m_rect;
	CBCGPRect				m_rectTrack;
	CWnd*					m_pWndOwner;

	CBCGPVisualContainer*	m_pParentContainer;
	BOOL					m_bIsAutoDestroy;

	BOOL					m_bIsDirty;
	BOOL					m_bCacheImage;
	BOOL					m_bIsSelected;
	UINT					m_uiEditFlags;

	BOOL					m_bIsTracked;

	CBCGPSize				m_sizeScaleRatio;

	CArray<CBCGPVisualDataObject*, CBCGPVisualDataObject*>	m_arData;
	CMap<UINT, UINT, CBCGPRect, const CBCGPRect&>			m_mapTrackRects;

	static CMap<UINT,UINT,CBCGPBaseVisualObject*,CBCGPBaseVisualObject*> m_mapAnimations;
	static CCriticalSection g_cs;			// For multi-theard applications
};

#ifndef _BCGPCHART_STANDALONE

////////////////////////////////////////////////////////////////////////////////////////////////////
// CBCGPVisualLayout

class BCGCBPRODLLEXPORT CBCGPVisualLayout : public CBCGPStaticLayout
{
	friend class CBCGPVisualContainer;

public:
	CBCGPVisualLayout(CBCGPVisualContainer& container) :
		m_container(container)
	{
	}

	BOOL AddAnchor (CBCGPBaseVisualObject* pObject, XMoveType typeMove, XSizeType typeSize, 
					const CPoint& percMove = CPoint (100, 100), const CPoint& percSize = CPoint (100, 100));

protected:
	virtual void AdjustLayout ();
	virtual CRect GetItemRect(XWndItem& item) const;
	virtual void GetHostWndRect (CRect& rect) const;

	CBCGPVisualContainer& m_container;
};

#endif // _BCGPCHART_STANDALONE

////////////////////////////////////////////////////////////////////////////////////////////////////
// CBCGPVisualContainer

class CBCGPVisualDataObject;

class BCGCBPRODLLEXPORT CBCGPVisualContainer : public CObject  
{
	DECLARE_DYNCREATE(CBCGPVisualContainer)

public:
// Construction
	CBCGPVisualContainer(CWnd* pWndOwner = NULL);
	virtual ~CBCGPVisualContainer();

// Attributes:
public:
	CWnd* GetOwner() const
	{
		return m_pWndOwner;
	}

	virtual CWnd* SetOwner(CWnd* pWndOwner, BOOL bRedraw = FALSE);

	const CBCGPRect& GetRect() const
	{
		return m_rect;
	}

	void SetRect(const CBCGPRect& rect, BOOL bReposObjects = TRUE, BOOL bRedraw = FALSE);
	CBCGPRect GetBoundsRect(BOOL bOnlyVisible = FALSE) const;

	void EnableEditMode(BOOL bEnable = TRUE, DWORD dwEditFlags = 0);
	BOOL IsEditMode() const
	{
		return m_bIsEditMode;
	}

	void SetDirty(BOOL bSet = TRUE) const;
	BOOL IsDirty() const;

	void EnableImageCache(BOOL bEnable = TRUE);
	BOOL IsImageCache() const
	{
		return m_bCacheImage;
	}

	void SetOutlineBrush(const CBCGPBrush& br);
	void SetFillBrush(const CBCGPBrush& br);
	void SetGridBrush(const CBCGPBrush& br);

	const CBCGPBrush& GetOutlineBrush() const
	{
		return m_brOutline;
	}

	const CBCGPBrush& GetFillBrush() const
	{
		return m_brFill;
	}

	const CBCGPBrush& GetGridBrush() const
	{
		return m_brGrid;
	}

	void SetDrawDynamicObjectsOnTop(BOOL bSet = TRUE);

	BOOL IsDrawDynamicObjectsOnTop() const
	{
		return m_bDrawDynamicObjectsOnTop;
	}

// Overrides:
public:
	virtual void OnDraw(CBCGPGraphicsManager* pGM, const CBCGPRect& rectClip);
	virtual void Redraw();

	virtual void OnFillBackground(CBCGPGraphicsManager* pGM);
	virtual void OnDrawBorder(CBCGPGraphicsManager* pGM);
	virtual void OnDrawGrid(CBCGPGraphicsManager* pGM);

	virtual BOOL OnMouseDown(int nButton, const CBCGPPoint& pt);
	virtual void OnMouseUp(int nButton, const CBCGPPoint& pt);
	virtual void OnMouseMove(const CBCGPPoint& pt);
	virtual void OnMouseLeave();
	virtual BOOL OnSetMouseCursor(const CBCGPPoint& pt);
	virtual BOOL OnMouseWheel(const CBCGPPoint& pt, short zDelta);
	virtual void OnCancelMode();
	virtual BOOL OnGetToolTip(const CBCGPPoint& pt, CString& strToolTip, CString& strDescr);
	virtual BOOL OnKeyboardDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnKeyboardUp(UINT nChar, UINT nRepCnt, UINT nFlags);

// Operations:
public:
	// Visual objects:
	BOOL Add(CBCGPBaseVisualObject* pObject, BOOL bAutoDestroy = TRUE);
	BOOL InsertAt(CBCGPBaseVisualObject* pObject, int nIndex, BOOL bAutoDestroy = TRUE);

	BOOL Remove(int nIndex);
	BOOL Remove(CBCGPBaseVisualObject* pObject);
	void RemoveAll();

	int GetCount() const;
	CBCGPBaseVisualObject* GetAt(int nIndex);

	int FindIndex(const CBCGPBaseVisualObject* pObject) const;

	const CBCGPBaseVisualObject* operator[](int nIndex) const;
	CBCGPBaseVisualObject* GetFromPoint(const CPoint& point);

	// Selection:
	void Select(CBCGPBaseVisualObject* pObject, BOOL bAddToSel = FALSE);
	void ClearSelection();

	BOOL IsSingleSel() const
	{
		return (m_dwEditFlags & BCGP_CONTAINER_SINGLE_SEL) == BCGP_CONTAINER_SINGLE_SEL;
	}

	int GetSelCount() const
	{
		return (int)m_lstSel.GetCount();
	}

	CBCGPBaseVisualObject* GetSel(int nIndex = 0) const;

	// New objects:
	void SetAddObjectMode(BOOL bSet = TRUE)
	{
		m_bAddNewObjectMode = (m_bIsEditMode && bSet);
	}

	virtual CBCGPBaseVisualObject* OnStartAddNewObject(const CBCGPPoint& /*pt*/)
	{
		return NULL;
	}

	virtual BOOL OnFinishAddNewObject(CBCGPBaseVisualObject* /*pObj*/, const CBCGPRect& /*rect*/)
	{
		return TRUE;
	}

	// Z-order:
	BOOL MoveForward(CBCGPBaseVisualObject* pObject);
	BOOL MoveBackward(CBCGPBaseVisualObject* pObject);
	BOOL MoveToFront(CBCGPBaseVisualObject* pObject);
	BOOL MoveToBack(CBCGPBaseVisualObject* pObject);

#ifndef _BCGPCHART_STANDALONE
	// Layout:
	void EnableLayout(BOOL bEnable = TRUE);
	BOOL IsLayoutEnabled() const
	{
		return m_pLayout != NULL;
	}

	CBCGPVisualLayout* GetLayout()
	{
		return m_pLayout;
	}
#endif

	virtual void DrawObjects(CBCGPGraphicsManager* pGM, CBCGPRect rectClip, DWORD dwFlags, const CBCGPPoint& ptOffset = CBCGPPoint(0, 0));
	virtual void DrawTrackingRects(CBCGPGraphicsManager* pGM);
	virtual void DrawSelectedArea(CBCGPGraphicsManager* pGM, const CBCGPRect& rectSel);

	void AdjustLayout();

	// Serialization:
#if (!defined _BCGSUITE_) && (!defined _BCGPCHART_STANDALONE)
	virtual BOOL LoadFromXML (UINT uiXMLResID);
	virtual BOOL LoadFromXML (LPCTSTR lpszXMLResID);
	virtual BOOL LoadFromBuffer (LPCTSTR lpszXMLBuffer);

	virtual BOOL SaveToXML (const CString& strFileName);
#endif

	// Image export:
	BOOL ExportToImage(CBCGPGraphicsManager* pGM, CBCGPImage& image);
	HBITMAP ExportToBitmap(CBCGPGraphicsManager* pGM);

	BOOL CopyToClipboard(CBCGPGraphicsManager* pGM);
	BOOL ExportToFile(const CString& strFilePath, CBCGPGraphicsManager* pGM);

// Attributes:
protected:
	CWnd*					m_pWndOwner;
	CBCGPRect				m_rect;
	BOOL					m_bIsEditMode;
	BOOL					m_bDrawDynamicObjectsOnTop;
	DWORD					m_dwEditFlags;
	BOOL					m_bCacheImage;
	CBCGPImage				m_ImageCache;

#ifndef _BCGPCHART_STANDALONE
	CBCGPVisualLayout*		m_pLayout;
#endif

	CBCGPBrush				m_brOutline;
	CBCGPBrush				m_brFill;

	CArray<CBCGPBaseVisualObject*, CBCGPBaseVisualObject*>	m_arObjects;
	CList<CBCGPBaseVisualObject*, CBCGPBaseVisualObject*>	m_lstSel;

	CBCGPPoint				m_ptDragStart;
	CBCGPPoint				m_ptDragFinish;

	CBCGPBaseVisualObject*	m_pNewObject;
	BOOL					m_bAddNewObjectMode;

	CBCGPBrush				m_brSelFill;
	CBCGPBrush				m_brSelOutline;
	CBCGPBrush				m_brGrid;

	CBCGPSize				m_sizeGrid;

	UINT					m_nDragMode;

	CBCGPGraphicsManager::BCGP_GRAPHICS_MANAGER	m_GMType;
};

extern UINT BCGM_POSTREDRAW;

#endif // !defined(AFX_BCGPVISUALCONTAINER_H__F3037CD5_27F5_47A1_9D7E_189F956D2CB9__INCLUDED_)
