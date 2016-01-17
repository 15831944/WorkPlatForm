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
// BCGPChartView.cpp : implementation file
//

#include "stdafx.h"
#include "BCGPChartView.h"
#include "BCGPChartCtrl.h"
#include "BCGPDrawManager.h"
#include "bcgpmath.h"

#ifndef _BCGSUITE_
#include "BCGPPrintPreviewView.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBCGPChartView

IMPLEMENT_DYNCREATE(CBCGPChartView, CScrollView)

CBCGPChartView::CBCGPChartView()
{
	m_pWndChartCtrl = NULL;
	m_printOldBMP = NULL;
}

CBCGPChartView::~CBCGPChartView()
{
}


BEGIN_MESSAGE_MAP(CBCGPChartView, CScrollView)
	//{{AFX_MSG_MAP(CBCGPChartView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CBCGPChartView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CBCGPChartView::OnFilePrint)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCGPChartView drawing

void CBCGPChartView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

void CBCGPChartView::OnDraw(CDC* /*pDC*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// CBCGPChartView diagnostics

#ifdef _DEBUG
void CBCGPChartView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CBCGPChartView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBCGPChartView message handlers

#define ID_CHART_CTRL 1

CBCGPChartCtrl* CBCGPChartView::CreateChart ()
{
	return new CBCGPChartCtrl;
}
//*******************************************************************************
int CBCGPChartView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pWndChartCtrl = CreateChart ();
	if (m_pWndChartCtrl == NULL)
	{
		TRACE0("CBCGPChartView::OnCreate: Chart control is not created\n");
		return -1;
	}

	ASSERT_VALID (m_pWndChartCtrl);
	ASSERT (m_pWndChartCtrl->IsKindOf (RUNTIME_CLASS (CBCGPChartCtrl)));
	
	if (!m_pWndChartCtrl->Create (CBCGPRect(), this, ID_CHART_CTRL))
	{
		TRACE0("CBCGPChartView::OnCreate: cannot create Chart control\n");
		return -1;
	}

	return 0;
}
//*******************************************************************************
void CBCGPChartView::OnSize(UINT nType, int cx, int cy) 
{
	CScrollView::OnSize(nType, cx, cy);
	
	if (m_pWndChartCtrl->GetSafeHwnd () != NULL)
	{
		m_pWndChartCtrl->SetWindowPos (NULL, -1, -1, cx, cy,
			SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	}
}
//*******************************************************************************
BOOL CBCGPChartView::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return TRUE;
}
//*******************************************************************************
void CBCGPChartView::OnSetFocus(CWnd* pOldWnd) 
{
	CScrollView::OnSetFocus(pOldWnd);
	
	if (m_pWndChartCtrl != NULL)
	{
		ASSERT_VALID (m_pWndChartCtrl);
		m_pWndChartCtrl->SetFocus();
	}
}
//*******************************************************************************
void CBCGPChartView::OnDestroy() 
{
	if (m_pWndChartCtrl != NULL)
	{
		ASSERT_VALID (m_pWndChartCtrl);

		m_pWndChartCtrl->DestroyWindow ();
		delete m_pWndChartCtrl;
		m_pWndChartCtrl = NULL;
	}

	CScrollView::OnDestroy();
}
//*******************************************************************************
BOOL CBCGPChartView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	return DoPreparePrinting(pInfo);
}
//*******************************************************************************
void CBCGPChartView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) 
{
}
//*******************************************************************************
void CBCGPChartView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) 
{
	if (m_printDC.GetSafeHdc () != NULL)
	{
		m_printDC.SelectObject (m_printOldBMP);

		m_printBMP.DeleteObject ();
		m_printDC.DeleteDC ();
	}
}
//*******************************************************************************
void CBCGPChartView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	if (m_pWndChartCtrl == NULL)
	{
		return;
	}

	ASSERT_VALID (m_pWndChartCtrl);

	if (m_printDC.GetSafeHdc () == NULL)
	{
		CBCGPChartVisualObject* pChart = m_pWndChartCtrl->GetChart();
		ASSERT_VALID(pChart);

		CBCGPGraphicsManager* pGM = m_pWndChartCtrl->GetGraphicsManager();
		if (pGM == NULL)
		{
			return;
		}

		ASSERT_VALID(pGM);

		CSize size(pInfo->m_rectDraw.Size());
		CBCGPRect rect(CBCGPPoint(), size);
		CBCGPRect rectControl(pChart->GetRect());

		CBCGPSize sizeScaleRatio((double)size.cx / rectControl.Width(), (double)size.cy / rectControl.Height());

		double dAspect = min(sizeScaleRatio.cx, sizeScaleRatio.cy);
		sizeScaleRatio.cx = sizeScaleRatio.cy = dAspect;

		size.cx = bcg_clamp((int)(rectControl.Width() * dAspect), 0, size.cx);
		size.cy = bcg_clamp((int)(rectControl.Height() * dAspect), 0, size.cy);
		rect.SetSize(size);

		CDC* pCDC = GetDC ();

		m_printDC.CreateCompatibleDC (pCDC);
		m_printBMP.Attach(CBCGPDrawManager::CreateBitmap_32(size, NULL));
		m_printOldBMP = m_printDC.SelectObject (&m_printBMP);

		ReleaseDC (pCDC);

		pGM->BindDC (&m_printDC, rect);

		if (pGM->IsBindDCFailed())
		{
			m_printDC.FillSolidRect((CRect)rect, RGB(255, 255, 255));
			m_printDC.TextOut(0, 0, _T("Out of memory"));
		}
		else
		{
			pGM->BeginDraw();

			CBCGPSize sizeScaleRatioOld = pChart->GetScaleRatio();
			pChart->SetScaleRatio(sizeScaleRatio);
			pChart->SetRect(rect);

			pChart->OnDraw (pGM, rect);

			pGM->EndDraw();

			pGM->BindDC(NULL);

			pChart->SetScaleRatio(sizeScaleRatioOld);
			
			CBCGPDrawManager::FillAlpha (rect, (HBITMAP)m_printBMP.GetSafeHandle (), 255);
		}

		pChart->SetRect(rectControl);
		pChart->SetDirty ();
	}

	if (m_printDC.GetSafeHdc () != NULL)
	{
		BITMAP bmp = {0};
		m_printBMP.GetBitmap (&bmp);

		CRect rect(pInfo->m_rectDraw.TopLeft (), CSize(bmp.bmWidth, bmp.bmHeight));

		int mode = pDC->SetStretchBltMode(HALFTONE);

		pDC->BitBlt (rect.left, rect.top, rect.Width(), rect.Height(), &m_printDC, 0, 0, SRCCOPY);
		pDC->SetStretchBltMode(mode);
	}
}
//*******************************************************************************
void CBCGPChartView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	CScrollView::OnPrepareDC(pDC, pInfo);
}
//*******************************************************************************
void CBCGPChartView::OnFilePrintPreview()
{
	BCGPPrintPreview(this);
}
