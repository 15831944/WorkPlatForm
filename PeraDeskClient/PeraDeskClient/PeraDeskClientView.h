// PeraDeskClientView.h : interface of the CPeraDeskClientView class
//


#pragma once
#include "CxViewMgr.h"


class CPeraDeskClientView : public CxViewExt
{
protected: // create from serialization only
	CPeraDeskClientView();
	DECLARE_DYNCREATE(CPeraDeskClientView)

// Attributes
public:
	CPeraDeskClientDoc* GetDocument() const;

// Operations
public:

// Overrides
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CPeraDeskClientView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView,
		CView* pDeactiveView);
};

#ifndef _DEBUG  // debug version in PeraDeskClientView.cpp
inline CPeraDeskClientDoc* CPeraDeskClientView::GetDocument() const
   { return reinterpret_cast<CPeraDeskClientDoc*>(m_pDocument); }
#endif

