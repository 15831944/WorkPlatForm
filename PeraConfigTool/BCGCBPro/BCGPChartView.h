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
// BCGPChartView.h : header file
//

#if !defined(AFX_BCGPCHARTVIEW_H__1951C624_2500_4213_814A_654981E6632B__INCLUDED_)
#define AFX_BCGPCHARTVIEW_H__1951C624_2500_4213_814A_654981E6632B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BCGCBPro.h"

class CBCGPChartCtrl;

/////////////////////////////////////////////////////////////////////////////
// CBCGPChartView view

class BCGCBPRODLLEXPORT CBCGPChartView : public CScrollView
{
protected:
	CBCGPChartView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CBCGPChartView)

// Attributes
public:
	CBCGPChartCtrl* GetChartCtrl () const
	{
		return m_pWndChartCtrl;
	}

protected:
	CBCGPChartCtrl*	m_pWndChartCtrl;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBCGPChartView)
	protected:
	virtual void OnInitialUpdate();     // first time after construct
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

	virtual CBCGPChartCtrl* CreateChart ();

// Implementation
protected:
	virtual ~CBCGPChartView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CBCGPChartView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg void OnFilePrintPreview();
	DECLARE_MESSAGE_MAP()

	CDC			m_printDC;
	CBitmap		m_printBMP;
	CBitmap*	m_printOldBMP;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCGPCHARTVIEW_H__1951C624_2500_4213_814A_654981E6632B__INCLUDED_)
