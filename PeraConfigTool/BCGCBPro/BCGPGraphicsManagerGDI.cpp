//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a part of BCGControlBar Library Professional Edition
// Copyright (C) 1998-2011 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************
//
// BCGPGraphicsManagerGDI.cpp: implementation of the CBCGPGraphicsManagerGDI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BCGPGraphicsManagerGDI.h"
#include "BCGPDrawManager.h"
#ifndef _BCGSUITE_
#include "BCGPToolBarImages.h"
#endif
#include "BCGPMath.h"
#include "BCGPImageProcessing.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static inline void SwapColors(COLORREF& color1, COLORREF& color2)
{
	COLORREF colorTemp = color1;
	color1 = color2;
	color2 = colorTemp;
}

static inline BOOL UseGDIPen(const CBCGPStrokeStyle* pStrokeStyle, double lineWidth)
{
	return (pStrokeStyle != NULL && pStrokeStyle->GetDashStyle() != CBCGPStrokeStyle::BCGP_DASH_STYLE_SOLID) ||
		lineWidth > 1.5;
}

static void CreateBezierPoints(const CBCGPPoint& ptFrom, const CBCGPBezierSegment& segment, CArray<POINT, POINT>& arPoints)
{
	arPoints.RemoveAll();

	const CBCGPPoint A((segment.m_Point1 - segment.m_Point2) * 3.0 - ptFrom + segment.m_Point3);
	const CBCGPPoint B((ptFrom - segment.m_Point1 * 2.0 + segment.m_Point2) * 3.0);
	const CBCGPPoint C((segment.m_Point1 - ptFrom) * 3.0);
	const CBCGPPoint D(ptFrom);

	const double dist = bcg_distance (ptFrom, segment.m_Point1) + 
		bcg_distance (segment.m_Point1, segment.m_Point2) + 
		bcg_distance (segment.m_Point2, segment.m_Point3);

	CPoint pt(bcg_round(ptFrom.x), bcg_round(ptFrom.y));
	arPoints.Add(pt);

	if (dist > 1.0)
	{
		double dt = 1.0 / dist;
		double t = dt;

		for (int i = 1; i < (int)dist; i++)
		{
			double t2 = t * t;
			double t3 = t2 * t;

			pt.x = bcg_round(((A.x * t3) + (B.x * t2) + (C.x * t) + D.x));
			pt.y = bcg_round(((A.y * t3) + (B.y * t2) + (C.y * t) + D.y));
			if (pt != arPoints[arPoints.GetSize () - 1])
			{
				arPoints.Add(pt);
			}

			t += dt;
		}
	}

	pt.x = bcg_round(segment.m_Point3.x);
	pt.y = bcg_round(segment.m_Point3.y);
	if (pt != arPoints[arPoints.GetSize () - 1])
	{
		arPoints.Add(pt);
	}
}

static BYTE LocaleNameToCharset(LPCTSTR szLn)
{
	BYTE charset = DEFAULT_CHARSET;

	CString ln(szLn);
	ln.TrimLeft (TCHAR(' '));
	ln.TrimRight (TCHAR(' '));

	int length = ln.GetLength();
	if (length < 2)
	{
		return charset;
	}

	ln.MakeLower ();

	CString ln2(ln.Left(2));

	if (length >= 5)
	{
		if (length >= 7)
		{
			if (ln2.Compare (_T("az")) == 0 || ln2.Compare (_T("uz")) == 0)
			{
				if (ln.Find (_T("cyrl")) != -1)
				{
					return RUSSIAN_CHARSET;
				}
				else if (ln.Find (_T("latn")) != -1)
				{
					return TURKISH_CHARSET;
				}
			}
			else if (ln2.Compare (_T("bs")) == 0 || ln2.Compare (_T("sr")) == 0)
			{
				if (ln.Find (_T("cyrl")) != -1)
				{
					return RUSSIAN_CHARSET;
				}
				else if (ln.Find (_T("latn")) != -1)
				{
					return 0xEE; // EE_CHARSET
				}
			}
			else if ((ln2.Compare (_T("mn")) == 0 || ln2.Compare (_T("tg")) == 0) &&
					ln.Find (_T("cyrl")) != -1)
			{
				return RUSSIAN_CHARSET;
			}
			else if (ln.Find (_T("iu-latn")) != -1 || ln.Find (_T("tmz-latn")) != -1)
			{
				return ANSI_CHARSET;
			}
		}
		else if (ln2.Compare (_T("zh")) == 0)
		{
			if (ln.Find (_T("cn")) != -1 || ln.Find (_T("sg")) != -1)
			{
				return GB2312_CHARSET;
			}
			else if (ln.Find (_T("hk")) != -1 || ln.Find (_T("mo")) != -1 ||
					ln.Find (_T("tw")) != -1)
			{
				return CHINESEBIG5_CHARSET;
			}
		}
	}

	int i = 0;

	if (length >= 3)
	{
		CString ln3(ln.Left(3));

		if (ln3.Compare (_T("sah")) == 0)
		{
			return RUSSIAN_CHARSET;
		}
		else if (ln3.Compare (_T("gbz")) == 0)
		{
			return ARABIC_CHARSET;
		}
		else
		{
			LPCTSTR szANSI[] = {_T("arn"), _T("moh"), _T("wen"), _T("smn"), 
								_T("smj"), _T("quz"), _T("sms"), _T("dsb"), 
								_T("sma"), _T("fil"), _T("qut")};

			for (i = 0; i < _countof(szANSI); i++)
			{
				if (ln3.Compare (szANSI[i]) == 0)
				{
					return ANSI_CHARSET;
				}
			}
		}
	}

	if (ln2.Compare (_T("el")) == 0)
	{
		return GREEK_CHARSET;
	}
	else if (ln2.Compare (_T("he")) == 0)
	{
		return HEBREW_CHARSET;
	}
	else if (ln2.Compare (_T("ja")) == 0)
	{
		return SHIFTJIS_CHARSET;
	}
	else if (ln2.Compare (_T("th")) == 0)
	{
		return THAI_CHARSET;
	}
	else if (ln2.Compare (_T("tr")) == 0)
	{
		return TURKISH_CHARSET;
	}
	else if (ln2.Compare (_T("ko")) == 0)
	{
		return HANGUL_CHARSET;
	}


	// ANSI_CHARSET
	LPCTSTR sz00[] = {_T("af"), _T("sq"), _T("eu"), _T("br"), _T("ca"), _T("da"),
					_T("nl"), _T("en"), _T("fo"), _T("fi"), _T("fr"), _T("fy"),
					_T("gl"), _T("de"), _T("kl"), _T("ha"), _T("is"), _T("id"),
					_T("ga"), _T("it"), _T("rw"), _T("lb"), _T("ms"), _T("mt"),
					_T("mi"), _T("nb"), _T("nn"), _T("oc"), _T("pt"), _T("rm"),
					_T("se"), _T("ns"), _T("tn"), _T("es"), _T("cy"), _T("wo"),
					_T("xh"), _T("zu")};

	// ARABIC_CHARSET
	LPCTSTR szB2[] = {_T("ar"), _T("fa"), _T("ug"), _T("ur")};

	// RUSSIAN_CHARSET
	LPCTSTR szCC[] = {_T("be"), _T("bg"), _T("kk"), _T("ky"), _T("mk"), _T("ru"),
					_T("tt"), _T("tk"), _T("uk")};

	// BALTIC_CHARSET
	LPCTSTR szBA[] = {_T("et"), _T("lv"), _T("lt")};

	// EE_CHARSET - 0xEE (238)	
	LPCTSTR szEE[] = {_T("hr"), _T("cs"), _T("hu"), _T("pl"), _T("ro"), _T("sk"),
					_T("sl")};

	for (i = 0; i < _countof(sz00); i++)
	{
		if (ln2.Compare (sz00[i]) == 0)
		{
			return ANSI_CHARSET;
		}
	}

	for (i = 0; i < _countof(szB2); i++)
	{
		if (ln2.Compare (szB2[i]) == 0)
		{
			return ARABIC_CHARSET;
		}
	}

	for (i = 0; i < _countof(szBA); i++)
	{
		if (ln2.Compare (szBA[i]) == 0)
		{
			return BALTIC_CHARSET;
		}
	}

	for (i = 0; i < _countof(szCC); i++)
	{
		if (ln2.Compare (szCC[i]) == 0)
		{
			return RUSSIAN_CHARSET;
		}
	}

	for (i = 0; i < _countof(szEE); i++)
	{
		if (ln2.Compare (szEE[i]) == 0)
		{
			return 0xEE;
		}
	}

	return charset;
}


IMPLEMENT_DYNAMIC(CBCGPGraphicsManagerGDI, CBCGPGraphicsManager)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CBCGPGraphicsManagerGDI::m_bTransparency = FALSE;

CBCGPGraphicsManagerGDI::CBCGPGraphicsManagerGDI(CDC* pDC, BOOL bDoubleBuffering, CBCGPGraphicsManagerParams*)
{
	m_Type = BCGP_GRAPHICS_MANAGER_GDI;
	m_pDM = NULL;
	m_pDCPaint = NULL;
	m_pMemDC = NULL;
	m_pBmpScreenOld = NULL;
	m_bIsAntialiasing = TRUE;
	m_bIsTransparentGradient = FALSE;

	BindDC(pDC, bDoubleBuffering);

	if (m_pDC != NULL)
	{
		BeginDraw();
	}
}

CBCGPGraphicsManagerGDI::CBCGPGraphicsManagerGDI(const CBCGPRect& rectDest, CBCGPImage* pImageDest) :
	CBCGPGraphicsManager(rectDest, pImageDest)
{
	m_Type = BCGP_GRAPHICS_MANAGER_GDI;
	m_pDM = NULL;
	m_pDCPaint = NULL;
	m_pMemDC = NULL;
	m_pBmpScreenOld = NULL;
	m_bIsAntialiasing = TRUE;
	m_bIsTransparentGradient = FALSE;
	m_pDC = NULL;
}

CBCGPGraphicsManagerGDI::~CBCGPGraphicsManagerGDI()
{
	EndDraw();

	CleanResources();
}

CBCGPGraphicsManager* CBCGPGraphicsManagerGDI::CreateOffScreenManager(const CBCGPRect& rect, CBCGPImage* pImageDest)
{
	if (m_pDC == NULL)
	{
		return NULL;
	}

	if (rect.IsRectEmpty())
	{
		return NULL;
	}

	CBCGPGraphicsManagerGDI* pGM = new CBCGPGraphicsManagerGDI(rect, pImageDest);

	pGM->m_nSupportedFeatures = m_nSupportedFeatures;
	pGM->m_pOriginal = this;

	pGM->BeginDraw();

	return pGM;
}

void CBCGPGraphicsManagerGDI::EnableTransparency(BOOL bEnable/* = TRUE*/)
{
#if _MSC_VER < 1600
	if (!globalData.bIsOSAlphaBlendingSupport)
	{
		bEnable = FALSE;
	}
#endif

	m_bTransparency = bEnable;
}

void CBCGPGraphicsManagerGDI::BindDC(CDC* pDC, BOOL bDoubleBuffering)
{
	if (m_pImageDest != NULL)
	{
		ASSERT(FALSE);
		return;
	}

	m_pDC = pDC;
	m_bDoubleBuffering = m_pDC != NULL && bDoubleBuffering && m_pDC->GetWindow()->GetSafeHwnd() != NULL;
}

BOOL CBCGPGraphicsManagerGDI::BeginDraw()
{
	if (m_pDM != NULL)
	{
		return FALSE;
	}

	if (m_pImageDest != NULL)
	{
		CSize sizeDest = m_rectDest.Size();
		if (sizeDest.cx <= 0 || sizeDest.cy <= 0)
		{
			return FALSE;
		}

		m_dcScreen.CreateCompatibleDC(NULL);

		HBITMAP hBitmap = CBCGPDrawManager::CreateBitmap_32(sizeDest, NULL);
		if (hBitmap == NULL)
		{
			return FALSE;
		}

		m_bmpScreen.Attach(hBitmap);
		m_pBmpScreenOld = m_dcScreen.SelectObject(&m_bmpScreen);

		m_dcScreen.FillSolidRect(CRect(0, 0, sizeDest.cx, sizeDest.cy), RGB(255, 255, 255));

		CBCGPGraphicsManagerGDI* pGMOrig = (CBCGPGraphicsManagerGDI*)m_pOriginal;
		if (pGMOrig != NULL && pGMOrig->m_pDCPaint != NULL)
		{
			m_dcScreen.BitBlt(0, 0, sizeDest.cx, sizeDest.cy, pGMOrig->m_pDCPaint, 
				(int)m_rectDest.left, (int)m_rectDest.top, SRCCOPY);
		}

		m_pDCPaint = &m_dcScreen;
	}
	else if (m_bDoubleBuffering)
	{
		ASSERT(m_pMemDC == NULL);

		m_pMemDC = new CBCGPMemDC(*m_pDC, m_pDC->GetWindow());
		m_pDCPaint = &m_pMemDC->GetDC ();
	}
	else
	{
		m_pDCPaint = m_pDC;
	}

	m_pDM = new CBCGPDrawManager(*m_pDCPaint);
	return TRUE;
}

void CBCGPGraphicsManagerGDI::EndDraw()
{
	ReleaseClipArea();

	if (m_pDM != NULL)
	{
		delete m_pDM;
		m_pDM = NULL;
	}

	if (m_pMemDC != NULL)
	{
		delete m_pMemDC;
		m_pMemDC = NULL;
	}

	if (m_pBmpScreenOld != NULL && m_dcScreen.GetSafeHdc() != NULL)
	{
		m_dcScreen.SelectObject(m_pBmpScreenOld);
		m_pBmpScreenOld = NULL;
	}

	if (m_bmpScreen.GetSafeHandle() != NULL && m_pImageDest != NULL)
	{
		m_dcScreen.DeleteDC();

		ASSERT_VALID(m_pImageDest);

		CBCGPToolBarImages* pImage = (CBCGPToolBarImages*)CreateImage(*m_pImageDest);

		pImage->Clear();

		pImage->AddImage((HBITMAP)m_bmpScreen, FALSE);

#ifndef _BCGSUITE_
		pImage->SetSingleImage(FALSE);
#else
		pImage->SetSingleImage();
#endif
		m_bmpScreen.DeleteObject();
	}

	m_pDCPaint = NULL;
}

void CBCGPGraphicsManagerGDI::Clear(const CBCGPColor& color)
{
	if (m_pDM == NULL)
	{
		return;
	}

	CWnd* pWnd = m_pDC->GetWindow();
	if (pWnd->GetSafeHwnd() != NULL)
	{
		CRect rectWindow;
		pWnd->GetClientRect(rectWindow);
		rectWindow.OffsetRect (pWnd->GetScrollPos (SB_HORZ), pWnd->GetScrollPos (SB_VERT));

		m_pDM->DrawRect(rectWindow, 
			color.IsNull() ? globalData.clrWindow : color, (COLORREF)-1);
	}
	else if (!m_rectDest.IsRectEmpty())
	{
		m_pDM->DrawRect(m_rectDest, 
			color.IsNull() ? globalData.clrWindow : color, (COLORREF)-1);
	}
}

void CBCGPGraphicsManagerGDI::DrawLine(	
	const CBCGPPoint& ptFrom, const CBCGPPoint& ptTo, const CBCGPBrush& brush, 
	double lineWidth, const CBCGPStrokeStyle* pStrokeStyle)
{
	if (m_pDM == NULL || m_pDCPaint == NULL || brush.IsEmpty())
	{
		return;
	}

	if (fabs(ptFrom.x - ptTo.x) < .1 || fabs(ptFrom.y - ptTo.y) < .1 ||
		UseGDIPen(pStrokeStyle, lineWidth) || !m_bIsAntialiasing)
	{
		HPEN hPen = CreateGDIPen(brush, pStrokeStyle, lineWidth);
		HPEN hOldPen = (HPEN)::SelectObject(m_pDCPaint->GetSafeHdc(), hPen);
		
		m_pDCPaint->MoveTo(ptFrom);
		m_pDCPaint->LineTo(ptTo);

		::SelectObject(m_pDCPaint->GetSafeHdc(), hOldPen);
		return;
	}
	
	m_pDM->DrawLineA(ptFrom.x, ptFrom.y, ptTo.x, ptTo.y, brush.GetColor());
}

void CBCGPGraphicsManagerGDI::DrawLines(	
		const CBCGPPointsArray& arPoints, const CBCGPBrush& brush, 
		double lineWidth, const CBCGPStrokeStyle* pStrokeStyle)
{
	const int nSize = (int)arPoints.GetSize();
	if (nSize < 2)
	{
		return;
	}

	if (m_pDM == NULL || m_pDCPaint == NULL || brush.IsEmpty())
	{
		return;
	}

	HPEN hPen = CreateGDIPen(brush, pStrokeStyle, lineWidth);
	HPEN hOldPen = (HPEN)::SelectObject(m_pDCPaint->GetSafeHdc(), hPen);

	m_pDCPaint->MoveTo(arPoints[0]);

	for (int i = 1; i < nSize; i++)
	{
		m_pDCPaint->LineTo(arPoints[i]);
	}

	::SelectObject(m_pDCPaint->GetSafeHdc(), hOldPen);
}

void CBCGPGraphicsManagerGDI::DrawRectangle(
	const CBCGPRect& rect, const CBCGPBrush& brush, 
	double lineWidth, const CBCGPStrokeStyle* pStrokeStyle)
{
	if (m_pDCPaint == NULL || brush.IsEmpty())
	{
		return;
	}

	CBrush* pOldBrush = (CBrush*)m_pDCPaint->SelectStockObject(NULL_BRUSH);

	HPEN hPen = CreateGDIPen(brush, pStrokeStyle, lineWidth);
	HPEN hOldPen = (HPEN)::SelectObject(m_pDCPaint->GetSafeHdc(), hPen);
		
	m_pDCPaint->Rectangle((CRect)rect);

	SelectObject(m_pDCPaint->GetSafeHdc(), hOldPen);
	m_pDCPaint->SelectObject(pOldBrush);
}

void CBCGPGraphicsManagerGDI::FillRectangle(const CBCGPRect& rect, const CBCGPBrush& brush)
{
	if (brush.GetGradientType() == CBCGPBrush::BCGP_GRADIENT_BEVEL)
	{
		DrawBeveledRectangle(rect, brush);
	}
	else
	{
		FillGradientByType(rect, brush);
	}
}

void CBCGPGraphicsManagerGDI::DrawRoundedRectangle(
	const CBCGPRoundedRect& rect, const CBCGPBrush& brush, 
	double lineWidth, const CBCGPStrokeStyle* pStrokeStyle)
{
	if (m_pDCPaint == NULL || brush.IsEmpty())
	{
		return;
	}

	HPEN hPen = CreateGDIPen(brush, pStrokeStyle, lineWidth);
	HPEN hOldPen = (HPEN)::SelectObject(m_pDCPaint->GetSafeHdc(), hPen);

	CBrush* pOldBrush = (CBrush*)m_pDCPaint->SelectStockObject(NULL_BRUSH);

	m_pDCPaint->RoundRect((const CRect&)rect.rect, CBCGPPoint(2. * rect.radiusX, 2. * rect.radiusY));

	SelectObject(m_pDCPaint->GetSafeHdc(), hOldPen);
	m_pDCPaint->SelectObject(pOldBrush);
}

void CBCGPGraphicsManagerGDI::FillRoundedRectangle(
	const CBCGPRoundedRect& rect, const CBCGPBrush& brush)
{
	if (m_pDM == NULL || brush.IsEmpty())
	{
		return;
	}

	if (brush.GetGradientType() == CBCGPBrush::BCGP_GRADIENT_BEVEL)
	{
		CBCGPColor colorLight, colorDark;
		double nDepth = PrepareBevelColors(brush.GetColor(), colorLight, colorDark);

		FillRoundedRectangle(rect, 
			CBCGPBrush(colorDark, colorLight, CBCGPBrush::BCGP_GRADIENT_DIAGONAL_LEFT, brush.GetOpacity()));

		CBCGPRoundedRect rectInternal = rect;
		rectInternal.rect.DeflateRect(nDepth, nDepth);
		rectInternal.radiusX -= nDepth / 2.;
		rectInternal.radiusY -= nDepth / 2.;

		FillRoundedRectangle(rectInternal, 
			CBCGPBrush(brush.GetColor(), colorLight, CBCGPBrush::BCGP_GRADIENT_HORIZONTAL, brush.GetOpacity()));
	}
	else
	{
		int nSaved = m_pDCPaint->SaveDC();

		SetClipArea(CBCGPRoundedRectangleGeometry(rect), RGN_AND);

		FillGradientByType(rect.rect, brush);

		ReleaseClipArea();
		
		m_pDCPaint->RestoreDC(nSaved);
	}
}

void CBCGPGraphicsManagerGDI::DrawEllipse(
	const CBCGPEllipse& ellipse, const CBCGPBrush& brush, 
	double lineWidth, const CBCGPStrokeStyle* pStrokeStyle)
{
	if (m_pDM == NULL || m_pDCPaint == NULL || brush.IsEmpty())
	{
		return;
	}

	CRect rect = (CRect)(CBCGPRect)ellipse;

	if (UseGDIPen(pStrokeStyle, lineWidth) || !m_bIsAntialiasing)
	{
		HPEN hPen = CreateGDIPen(brush, pStrokeStyle, lineWidth);
		HPEN hOldPen = (HPEN)::SelectObject(m_pDCPaint->GetSafeHdc(), hPen);
		
		CBrush* pOldBrush = (CBrush*)m_pDCPaint->SelectStockObject(NULL_BRUSH);

		m_pDCPaint->Ellipse(rect);

		SelectObject(m_pDCPaint->GetSafeHdc(), hOldPen);
		m_pDCPaint->SelectObject(pOldBrush);
		return;
	}

	m_pDM->DrawEllipse(rect, (COLORREF)-1, brush.GetColor());
}

void CBCGPGraphicsManagerGDI::FillEllipse(
	const CBCGPEllipse& ellipse, const CBCGPBrush& brush)
{
	if (m_pDM == NULL || m_pDCPaint == NULL || brush.IsEmpty())
	{
		return;
	}

	CRect rect = (CRect)(CBCGPRect)ellipse;

	if (brush.GetGradientType() == CBCGPBrush::BCGP_NO_GRADIENT && (brush.GetOpacity() == 1. || !m_bTransparency))
	{
		m_pDM->DrawEllipse(rect, brush.GetColor(), (COLORREF)-1);
		return;
	}

	if (brush.GetGradientType() == CBCGPBrush::BCGP_GRADIENT_BEVEL)
	{
		CBCGPColor colorLight, colorDark;
		double nDepth = PrepareBevelColors(brush.GetColor(), colorLight, colorDark);

		FillEllipse(ellipse, 
			CBCGPBrush(colorDark, colorLight, CBCGPBrush::BCGP_GRADIENT_DIAGONAL_LEFT, brush.GetOpacity()));

		CBCGPEllipse ellipseInternal = ellipse;
		ellipseInternal.radiusX -= nDepth;
		ellipseInternal.radiusY -= nDepth;

		FillEllipse(ellipseInternal, 
			CBCGPBrush(brush.GetColor(), colorLight, CBCGPBrush::BCGP_GRADIENT_HORIZONTAL, brush.GetOpacity()));
		return;
	}

	int nSaved = m_pDCPaint->SaveDC();

	SetClipArea(CBCGPEllipseGeometry(ellipse), RGN_AND);

	FillGradientByType(rect, brush);

	ReleaseClipArea();

	m_pDCPaint->RestoreDC(nSaved);
}

void CBCGPGraphicsManagerGDI::DrawArc(
		const CBCGPEllipse& ellipse, double dblStartAngle, double dblFinishAngle, 
		BOOL bIsClockwise,
		const CBCGPBrush& brush, double lineWidth, const CBCGPStrokeStyle* pStrokeStyle)
{
	if (m_pDM == NULL || m_pDCPaint == NULL || brush.IsEmpty())
	{
		return;
	}

	if (ellipse.IsNull ())
	{
		return;
	}

	CRect rect = (CRect)(CBCGPRect)ellipse;

	if (UseGDIPen(pStrokeStyle, lineWidth) || !m_bIsAntialiasing)
	{
		double dblAngle = bcg_deg2rad(dblStartAngle);
		double cosA = cos(dblAngle);
		double sinA = sin(dblAngle);
		double r = (ellipse.radiusX * ellipse.radiusY) / bcg_distance(ellipse.radiusY * cosA, ellipse.radiusX * sinA);
		double x1 = ellipse.point.x + cosA * r;
		double y1 = ellipse.point.y - sinA * r;

		dblAngle = bcg_deg2rad(dblFinishAngle);
		cosA = cos(dblAngle);
		sinA = sin(dblAngle);
		r = (ellipse.radiusX * ellipse.radiusY) / bcg_distance(ellipse.radiusY * cosA, ellipse.radiusX * sinA);
		double x2 = ellipse.point.x + cosA * r;
		double y2 = ellipse.point.y - sinA * r;

		CPoint pt1(bcg_round(x1), bcg_round(y1));
		CPoint pt2(bcg_round(x2), bcg_round(y2));

		HPEN hPen = CreateGDIPen(brush, pStrokeStyle, lineWidth);
		HPEN hOldPen = (HPEN)::SelectObject(m_pDCPaint->GetSafeHdc(), hPen);
		
		m_pDCPaint->SetArcDirection(bIsClockwise ? AD_CLOCKWISE : AD_COUNTERCLOCKWISE);

		m_pDCPaint->Arc(rect, pt1, pt2);

		SelectObject(m_pDCPaint->GetSafeHdc(), hOldPen);
	}
	else
	{
		m_pDM->DrawArc(rect, dblStartAngle, dblFinishAngle, bIsClockwise, brush.GetColor());
	}
}

void CBCGPGraphicsManagerGDI::DrawArc(
		const CBCGPPoint& ptFrom, const CBCGPPoint& ptTo, const CBCGPSize sizeRadius, 
		BOOL bIsClockwise, BOOL bIsLargeArc,
		const CBCGPBrush& brush, double lineWidth, const CBCGPStrokeStyle* pStrokeStyle)
{
	if (m_pDM == NULL || m_pDCPaint == NULL || brush.IsEmpty())
	{
		return;
	}

	if (ptFrom == ptTo)
	{
		return;
	}

	CBCGPArcSegment arcSegment(ptTo, sizeRadius, bIsClockwise, bIsLargeArc, 0);

	double rX = 0;
	double rY = 0;

	CBCGPPoint ptCenter = arcSegment.GetArcCenter(ptFrom, bIsLargeArc, rX, rY);

	DrawArc(CBCGPEllipse(ptCenter, rX, rY), 
		bcg_rad2deg(bcg_angle((ptFrom.x - ptCenter.x), (ptCenter.y - ptFrom.y))),
		bcg_rad2deg(bcg_angle((ptTo.x - ptCenter.x), (ptCenter.y - ptTo.y))), 
		arcSegment.m_bIsClockwise, brush.GetColor(), lineWidth, pStrokeStyle);
}

void CBCGPGraphicsManagerGDI::DrawGeometry(
	const CBCGPGeometry& geometry, const CBCGPBrush& brush, 
	double lineWidth, const CBCGPStrokeStyle* pStrokeStyle)
{
	if (m_pDM == NULL || m_pDCPaint == NULL || brush.IsEmpty())
	{
		return;
	}

	if (geometry.IsKindOf(RUNTIME_CLASS(CBCGPPolygonGeometry)))
	{
		CBCGPPolygonGeometry& polygonGeometry = (CBCGPPolygonGeometry&)geometry;
		const CBCGPPointsArray& arPoints = polygonGeometry.GetPoints();

		int nCount = (int)arPoints.GetSize();
		if (nCount < 2)
		{
			return;
		}

		if (!UseGDIPen(pStrokeStyle, lineWidth) && polygonGeometry.GetCurveType() != CBCGPPolygonGeometry::BCGP_CURVE_TYPE_BEZIER)
		{
			for (int i = 1; i < nCount; i++)
			{
				DrawLine(arPoints[i - 1], arPoints[i], brush, lineWidth, pStrokeStyle);
			}
		
			if (polygonGeometry.IsClosed())
			{
				DrawLine(arPoints[nCount - 1], arPoints[0], brush, lineWidth, pStrokeStyle);
			}
		}
		else
		{
			POINT* arPointsGDI = new POINT[nCount + 1];

			for (int i = 0; i < arPoints.GetSize(); i++)
			{
				arPointsGDI[i].x = bcg_round(arPoints[i].x);
				arPointsGDI[i].y = bcg_round(arPoints[i].y);
			}

			HPEN hPen = CreateGDIPen(brush, pStrokeStyle, lineWidth);
			HPEN hOldPen = (HPEN)::SelectObject(m_pDCPaint->GetSafeHdc(), hPen);

			if (polygonGeometry.GetCurveType() == CBCGPPolygonGeometry::BCGP_CURVE_TYPE_BEZIER)
			{
				::PolyBezier(m_pDCPaint->GetSafeHdc(), (LPPOINT)arPointsGDI, nCount);

				if (polygonGeometry.IsClosed() && arPoints[0] != arPoints[nCount - 1])
				{
					m_pDCPaint->MoveTo(arPointsGDI[nCount - 1]);
					m_pDCPaint->LineTo(arPointsGDI[0]);
				}
			}
			else
			{
				if (polygonGeometry.IsClosed())
				{
					arPointsGDI[nCount].x = bcg_round(arPoints[0].x);
					arPointsGDI[nCount].y = bcg_round(arPoints[0].y);
					nCount++;
				}

				::Polyline(m_pDCPaint->GetSafeHdc(), (LPPOINT)arPointsGDI, nCount);
			}

			delete [] arPointsGDI;

			SelectObject(m_pDCPaint->GetSafeHdc(), hOldPen);
		}

		return;
	}
	else if (geometry.IsKindOf(RUNTIME_CLASS(CBCGPSplineGeometry)))
	{
		CBCGPSplineGeometry& splineGeometry = (CBCGPSplineGeometry&)geometry;
		const CBCGPPointsArray& arPoints = splineGeometry.GetPoints();
		int nCount = (int)arPoints.GetSize();

		if (nCount < 4)
		{
			return;
		}

		POINT* arPointsGDI = new POINT[nCount];

		for (int i = 0; i < nCount; i++)
		{
			arPointsGDI[i].x = bcg_round(arPoints[i].x);
			arPointsGDI[i].y = bcg_round(arPoints[i].y);
		}

		HPEN hPen = CreateGDIPen(brush, pStrokeStyle, lineWidth);
		HPEN hOldPen = (HPEN)::SelectObject(m_pDCPaint->GetSafeHdc(), hPen);

		::PolyBezier(m_pDCPaint->GetSafeHdc(), (LPPOINT)arPointsGDI, nCount);

		if (splineGeometry.IsClosed() && arPoints[0] != arPoints[nCount - 1])
		{
			m_pDCPaint->MoveTo(arPointsGDI[nCount - 1]);
			m_pDCPaint->LineTo(arPointsGDI[0]);
		}

		delete [] arPointsGDI;

		SelectObject(m_pDCPaint->GetSafeHdc(), hOldPen);

		return;
	}
	else if (geometry.IsKindOf(RUNTIME_CLASS(CBCGPComplexGeometry)))
	{
		CBCGPComplexGeometry& complexGeometry = (CBCGPComplexGeometry&)geometry;

		if (!complexGeometry.IsClosed() || !UseGDIPen (pStrokeStyle, lineWidth))
		{
			CBCGPPoint ptCurr = complexGeometry.GetStartPoint();

			const CObArray& arSegments = complexGeometry.GetSegments();

			for (int i = 0; i < arSegments.GetSize(); i++)
			{
				CObject* pSegment = arSegments[i];
				ASSERT_VALID(pSegment);

				if (pSegment->IsKindOf(RUNTIME_CLASS(CBCGPLineSegment)))
				{
					CBCGPLineSegment* pLineSegment = (CBCGPLineSegment*)pSegment;

					DrawLine(ptCurr, pLineSegment->m_Point, brush, lineWidth, pStrokeStyle);

					ptCurr = pLineSegment->m_Point;
				}
				else if (pSegment->IsKindOf(RUNTIME_CLASS(CBCGPBezierSegment)))
				{
					CBCGPBezierSegment* pBezierSegment = (CBCGPBezierSegment*)pSegment;

					const long c_Points = 4;
					POINT arPointsGDI[c_Points];

					arPointsGDI[0] = CPoint(bcg_round(ptCurr.x), bcg_round(ptCurr.y));
					arPointsGDI[1] = CPoint(bcg_round(pBezierSegment->m_Point1.x), bcg_round(pBezierSegment->m_Point1.y));
					arPointsGDI[2] = CPoint(bcg_round(pBezierSegment->m_Point2.x), bcg_round(pBezierSegment->m_Point2.y));
					arPointsGDI[3] = CPoint(bcg_round(pBezierSegment->m_Point3.x), bcg_round(pBezierSegment->m_Point3.y));

					HPEN hPen = CreateGDIPen(brush, pStrokeStyle, lineWidth);
					HPEN hOldPen = (HPEN)::SelectObject(m_pDCPaint->GetSafeHdc(), hPen);

					::PolyBezier(m_pDCPaint->GetSafeHdc(), arPointsGDI, c_Points);

					SelectObject(m_pDCPaint->GetSafeHdc(), hOldPen);

					ptCurr = pBezierSegment->m_Point3;
				}
				else if (pSegment->IsKindOf(RUNTIME_CLASS(CBCGPArcSegment)))
				{
					CBCGPArcSegment* pArcSegment = (CBCGPArcSegment*)pSegment;

					BOOL bIsLargeArc = pArcSegment->m_bIsLargeArc;

					double rX;
					double rY;

					CBCGPPoint ptCenter = pArcSegment->GetArcCenter(ptCurr, bIsLargeArc, rX, rY);
					if (rX == 0.0 || rY == 0.0)
					{
						break;
					}

					DrawArc(CBCGPEllipse(ptCenter, rX, rY), 
						bcg_rad2deg(bcg_angle((ptCurr.x - ptCenter.x), (ptCenter.y - ptCurr.y))),
						bcg_rad2deg(bcg_angle((pArcSegment->m_Point.x - ptCenter.x), (ptCenter.y - pArcSegment->m_Point.y))), 
						pArcSegment->m_bIsClockwise, brush.GetColor(), lineWidth, pStrokeStyle);

					ptCurr = pArcSegment->m_Point;
				}
			}

			if (complexGeometry.IsClosed() && complexGeometry.GetStartPoint() != ptCurr)
			{
				DrawLine(ptCurr, complexGeometry.GetStartPoint(), brush, lineWidth, pStrokeStyle);
			}

			return;
		}
	}
	else if (geometry.IsKindOf(RUNTIME_CLASS(CBCGPEllipseGeometry)) && !UseGDIPen (pStrokeStyle, lineWidth))
	{
		DrawEllipse (((CBCGPEllipseGeometry&)geometry).GetEllipse (), brush, lineWidth, pStrokeStyle);
		return;
	}

	HRGN hrgn = (HRGN)CreateGeometry((CBCGPGeometry&)geometry);
	if (hrgn == NULL)
	{
		return;
	}

	::FrameRgn(m_pDCPaint->GetSafeHdc(), hrgn, (HBRUSH)CreateBrush((CBCGPBrush&)brush), 
		(int)lineWidth, (int)lineWidth);
}

void CBCGPGraphicsManagerGDI::FillGeometry(
	const CBCGPGeometry& geometry, const CBCGPBrush& brush)
{
	if (m_pDM == NULL || brush.IsEmpty() || m_pDCPaint == NULL)
	{
		return;
	}

	if (geometry.IsKindOf(RUNTIME_CLASS(CBCGPPolygonGeometry)))
	{
		CBCGPPolygonGeometry& polygonGeometry = (CBCGPPolygonGeometry&)geometry;
		
		if (!polygonGeometry.IsClosed())
		{
			return;
		}
	}

	HRGN hrgn = (HRGN)CreateGeometry((CBCGPGeometry&)geometry);
	if (hrgn == NULL)
	{
		return;
	}

	CRect rect;
	::GetRgnBox(hrgn, rect);

	int nSaved = m_pDCPaint->SaveDC();
	SetClipArea(geometry, RGN_AND);

	if (brush.GetGradientType() == CBCGPBrush::BCGP_GRADIENT_BEVEL)
	{
		FillGradientByType(rect, brush);
	}
	else
	{
		FillRectangle(rect, brush);
	}

	ReleaseClipArea();

	m_pDCPaint->RestoreDC(nSaved);
}

void CBCGPGraphicsManagerGDI::DrawImage(
	const CBCGPImage& image, const CBCGPPoint& ptDest, const CBCGPSize& sizeDest,
	double opacity, CBCGPImage::BCGP_IMAGE_INTERPOLATION_MODE /*interpolationMode*/, const CBCGPRect& rectSrc)
{
	if (m_pDCPaint == NULL)
	{
		return;
	}

	CBCGPToolBarImages* pImages = (CBCGPToolBarImages*)CreateImage((CBCGPImage&)image);
	if (pImages == NULL)
	{
		ASSERT(FALSE);
		return;
	}

	if (!pImages->IsValid())
	{
		return;
	}

	CBCGPSize size = sizeDest.IsEmpty() ? image.GetSize() : sizeDest;
	CBCGPRect rectImage(ptDest, size);

	CBCGPToolBarImages::ImageAlignHorz alHorz = CBCGPToolBarImages::ImageAlignHorzLeft;
	CBCGPToolBarImages::ImageAlignVert alVert = CBCGPToolBarImages::ImageAlignVertTop;
	if (!rectSrc.IsRectEmpty ())
	{
		alHorz = CBCGPToolBarImages::ImageAlignHorzStretch;
		alVert = CBCGPToolBarImages::ImageAlignVertStretch;
	}

	int mode = m_pDCPaint->SetStretchBltMode (HALFTONE);

#ifndef _BCGSUITE_
	pImages->DrawEx(m_pDCPaint, rectImage, 
		0, alHorz, alVert, rectSrc, (BYTE)(opacity * 255.0), image.IsIgnoreAlphaBitmap ());
#else
	pImages->DrawEx(m_pDCPaint, rectImage, 
		0, alHorz, alVert, rectSrc, (BYTE)(opacity * 255.0));
#endif

	m_pDCPaint->SetStretchBltMode (mode);
}

static UINT GetDrawTextFlags(const CBCGPTextFormat& textFormat, const CString& str)
{
	UINT uiFlags = 0;

	if (textFormat.GetTextAlignment() == CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_TRAILING)
	{
		uiFlags |= DT_RIGHT;
	}
	else if (textFormat.GetTextAlignment() == CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER)
	{
		uiFlags |= DT_CENTER;
	}

	if (textFormat.IsWordWrap() || str.Find(_T('\n')) >= 0)
	{
		uiFlags |= DT_WORDBREAK;
	}
	else if (textFormat.GetTextVerticalAlignment() == CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER)
	{
		uiFlags |= (DT_VCENTER | DT_SINGLELINE);
	}

	if (!textFormat.IsClipText())
	{
		uiFlags |= DT_NOCLIP;
	}

	return uiFlags;
}

void CBCGPGraphicsManagerGDI::DrawText(
	const CString& strText, const CBCGPRect& rectText, const CBCGPTextFormat& textFormat,
	const CBCGPBrush& foregroundBrush)
{
	if (m_pDCPaint == NULL)
	{
		return;
	}

	HFONT hFont = (HFONT)CreateTextFormat((CBCGPTextFormat&)textFormat);
	ASSERT(hFont != NULL);

	HFONT hFontOld = (HFONT)m_pDCPaint->SelectObject(hFont);
	COLORREF clrTextOld = m_pDCPaint->SetTextColor(foregroundBrush.GetColor());

	m_pDCPaint->SetBkMode(TRANSPARENT);

	double dAngle = bcg_normalize_deg (textFormat.GetDrawingAngle());
	if (!textFormat.IsWordWrap() && dAngle != 0. && strText.Find(_T('\n')) < 0)
	{
		CBCGPPoint point(rectText.TopLeft());
		CBCGPSize size(GetTextSize (strText, textFormat, 0.0, TRUE));
        CBCGPRect rect(CBCGPPoint(-size.cx / 2.0, -size.cy / 2.0), size);

		dAngle = bcg_deg2rad (dAngle);
		double dCos = cos(dAngle);
		double dSin = sin(dAngle);

		rect = CBCGPRect
					(
						rect.left * dCos - rect.top * dSin,
						rect.left * dSin + rect.top * dCos,
						rect.right * dCos - rect.bottom * dSin,
						rect.right * dSin + rect.bottom * dCos
					);

		CBCGPTextFormat::BCGP_TEXT_ALIGNMENT horz = textFormat.GetTextAlignment();
		CBCGPTextFormat::BCGP_TEXT_ALIGNMENT vert = textFormat.GetTextVerticalAlignment();

        if(horz == CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_LEADING)
        {
			point.x += fabs(rect.Width()) / 2.0 + rect.left;

			if (vert == CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER)
			{
				point.y += (rectText.Height() + rect.Height()) / 2.0;
			}
			else
			{
				point.y += -rect.top + fabs(rect.Height()) / 2.0;
			}
        }
        else if(horz == CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER)
        {
			point.x += (rectText.Width() - rect.Width()) / 2.0;

			if (vert == CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER)
			{
				point.y += (rectText.Height() + rect.Height()) / 2.0;
			}
			else
			{
				point.y += -rect.top + fabs(rect.Height()) / 2.0;
			}
        }
        else if(horz == CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_TRAILING)
        {
			point.x = rectText.right + (rect.left - fabs(rect.Width()) / 2.0);

			if (vert == CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER)
			{
				point.y += (rectText.Height() + rect.Height()) / 2.0;
			}
			else
			{
				point.y += -rect.top + fabs(rect.Height()) / 2.0;
			}
        }

		UINT nTextAlign = m_pDCPaint->SetTextAlign(TA_LEFT | TA_BOTTOM);
		m_pDCPaint->TextOut(bcg_round(point.x), bcg_round(point.y), strText);
		m_pDCPaint->SetTextAlign(nTextAlign);
	}
	else
	{
		CRect rect = rectText;

		UINT uiFlags = GetDrawTextFlags(textFormat, strText);
		m_pDCPaint->DrawText(strText, rect, uiFlags);
	}

	m_pDCPaint->SelectObject(hFontOld);
	m_pDCPaint->SetTextColor(clrTextOld);
}

CBCGPSize CBCGPGraphicsManagerGDI::GetTextSize(const CString& strText, const CBCGPTextFormat& textFormat, double dblWidth, BOOL bIgnoreTextRotation)
{
	CDC* pDC = (m_pDCPaint != NULL) ? m_pDCPaint : m_pDC;
	if (pDC == NULL)
	{
		return CBCGPSize(0, 0);
	}

	if (textFormat.IsWordWrap() && dblWidth == 0.)
	{
		ASSERT(FALSE);
		return CBCGPSize(0, 0);
	}

	HFONT hFont = (HFONT)CreateTextFormat((CBCGPTextFormat&)textFormat);
	ASSERT(hFont != NULL);

	HFONT hFontOld = (HFONT)pDC->SelectObject(hFont);

	CSize size;
	
	if (textFormat.IsWordWrap())
	{
		UINT uiFlags = GetDrawTextFlags(textFormat, strText) | DT_CALCRECT;

		CRect rect(0, 0, bcg_round(dblWidth), 32767);
		m_pDCPaint->DrawText(strText, rect, uiFlags);
	
		size = rect.Size();
	}
	else
	{
		if (strText.Find(_T('\n')) < 0)
		{
			size = pDC->GetTextExtent(strText);
		}
		else
		{
			int nMaxWidth = 0;

			for (int i = 0;;)
			{
				int nIndex = strText.Find(_T('\n'), i);

				CString str = nIndex < 0 ? strText.Mid(i) : strText.Mid(i, nIndex - i);

				nMaxWidth = max(nMaxWidth, m_pDCPaint->GetTextExtent(str).cx);

				if (nIndex < 0)
				{
					break;
				}

				i = nIndex + 1;
			}

			UINT uiFlags = GetDrawTextFlags(textFormat, strText) | DT_CALCRECT;

			CRect rect(0, 0, nMaxWidth, 32767);
			int nHeight = m_pDCPaint->DrawText(strText, rect, uiFlags);
		
			size.cx = nMaxWidth;
			size.cy = nHeight;
		}
	}

	pDC->SelectObject(hFontOld);

	CBCGPSize sizeRes((double)size.cx, (double)size.cy);

	if (!bIgnoreTextRotation)
	{
		CBCGPTextFormat::AdjustTextSize(textFormat.GetDrawingAngle(), sizeRes);
	}

	return sizeRes;
}

LPVOID CBCGPGraphicsManagerGDI::CreateGeometry(CBCGPGeometry& geometry)
{
	if (geometry.GetHandle() != NULL)
	{
		CBCGPGraphicsManager* pGM = geometry.GetGraphicsManager();

		if (!pGM->IsKindOf(GetRuntimeClass()))
		{
			ASSERT(FALSE);
			return NULL;
		}
	
		return geometry.GetHandle();
	}

	HRGN hrgn = NULL;

	if (geometry.IsKindOf(RUNTIME_CLASS(CBCGPRectangleGeometry)))
	{
		CBCGPRectangleGeometry& rectGeometry = (CBCGPRectangleGeometry&)geometry;

		const CBCGPRect& rect = rectGeometry.GetRectangle();

		hrgn = ::CreateRectRgnIndirect(CRect(bcg_round(rect.left), bcg_round(rect.top), bcg_round(rect.right), bcg_round(rect.bottom)));
	}
	else if (geometry.IsKindOf(RUNTIME_CLASS(CBCGPEllipseGeometry)))
	{
		CBCGPEllipseGeometry& ellipseGeometry = (CBCGPEllipseGeometry&)geometry;
		hrgn = ::CreateEllipticRgnIndirect((const CRect&)(const CBCGPRect&)ellipseGeometry.GetEllipse());
	}
	else if (geometry.IsKindOf(RUNTIME_CLASS(CBCGPRoundedRectangleGeometry)))
	{
		CBCGPRoundedRectangleGeometry& roundedRectGeometry = (CBCGPRoundedRectangleGeometry&)geometry;

		CBCGPRoundedRect rect = roundedRectGeometry.GetRoundedRect();

		hrgn = ::CreateRoundRectRgn(bcg_round(rect.rect.left), bcg_round(rect.rect.top), 
			bcg_round(rect.rect.right), bcg_round(rect.rect.bottom), 
			bcg_round(2. * rect.radiusX), bcg_round(2. * rect.radiusY));
	}
	else if (geometry.IsKindOf(RUNTIME_CLASS(CBCGPPolygonGeometry)))
	{
		CBCGPPolygonGeometry& polygonGeometry = (CBCGPPolygonGeometry&)geometry;
		const CBCGPPointsArray& arPoints = polygonGeometry.GetPoints();
		int nCount = (int)arPoints.GetSize();

		if (polygonGeometry.IsClosed() && nCount > 0)
		{
			LPPOINT arPointsGDI = new POINT[nCount];

			for (int i = 0; i < nCount; i++)
			{
				arPointsGDI[i] = CPoint(bcg_round(arPoints[i].x), bcg_round(arPoints[i].y));
			}

			hrgn = ::CreatePolygonRgn(arPointsGDI, nCount, ALTERNATE);

			delete [] arPointsGDI;
		}
	}
	else if (geometry.IsKindOf(RUNTIME_CLASS(CBCGPSplineGeometry)))
	{
		CBCGPSplineGeometry& splineGeometry = (CBCGPSplineGeometry&)geometry;
		const CBCGPPointsArray& arPoints = splineGeometry.GetPoints();
		int nCount = (int)arPoints.GetSize();
		if (nCount > 1)
		{
			nCount = (nCount - 1) / 3;
		}

		if (splineGeometry.IsClosed() && nCount > 0)
		{
			CArray<POINT, POINT> arPointsGDI;

			int index = 1;
			for (int i = 0; i < nCount; i++)
			{
				CArray<POINT, POINT> bezier;
				CreateBezierPoints (arPoints[index - 1], CBCGPBezierSegment(arPoints[index], arPoints[index + 1], arPoints[index + 2]), bezier);
				arPointsGDI.Append (bezier);

				index += 3;
			}

			hrgn = ::CreatePolygonRgn(arPointsGDI.GetData(), (int)arPointsGDI.GetSize(), ALTERNATE);
		}
	}
	else if(geometry.IsKindOf(RUNTIME_CLASS(CBCGPComplexGeometry)))
	{
		CBCGPComplexGeometry& complexGeometry = (CBCGPComplexGeometry&)geometry;
		const CObArray& arSegments = complexGeometry.GetSegments();
		if (arSegments.GetSize () == 0)
		{
			return NULL;
		}

		CBCGPPoint ptCurr = complexGeometry.GetStartPoint();

		CArray<POINT, POINT> arPoints;
		arPoints.Add(ptCurr);

		for (int i = 0; i < arSegments.GetSize(); i++)
		{
			CObject* pSegment = arSegments[i];
			ASSERT_VALID(pSegment);

			if (pSegment->IsKindOf(RUNTIME_CLASS(CBCGPLineSegment)))
			{
				CBCGPLineSegment* pLineSegment = (CBCGPLineSegment*)pSegment;

				arPoints.Add(pLineSegment->m_Point);

				ptCurr = pLineSegment->m_Point;
			}
			else if (pSegment->IsKindOf(RUNTIME_CLASS(CBCGPBezierSegment)))
			{
				CBCGPBezierSegment* pBezierSegment = (CBCGPBezierSegment*)pSegment;

				CArray<POINT, POINT> bezier;
				CreateBezierPoints(ptCurr, *pBezierSegment, bezier);
				arPoints.Append (bezier);

				ptCurr = pBezierSegment->m_Point3;
			}
			else if (pSegment->IsKindOf(RUNTIME_CLASS(CBCGPArcSegment)))
			{
				CBCGPArcSegment* pArcSegment = (CBCGPArcSegment*)pSegment;

				BOOL bIsLargeArc = pArcSegment->m_bIsLargeArc;

				double rX;
				double rY;

				CBCGPPoint ptCenter = pArcSegment->GetArcCenter(ptCurr, bIsLargeArc, rX, rY);
				if (rX == 0.0 || rY == 0.0)
				{
					return NULL;
				}

				double dblAngle1 = bcg_normalize_rad(bcg_angle((ptCurr.x - ptCenter.x), (ptCenter.y - ptCurr.y)));
				double dblAngle2 = bcg_normalize_rad(bcg_angle((pArcSegment->m_Point.x - ptCenter.x), (ptCenter.y - pArcSegment->m_Point.y)));

// 				if (bIsLargeArc && fabs(dblAngle2 - dblAngle1) == M_PI)
// 				{
// 					bIsLargeArc = FALSE;
// 				}

				if (pArcSegment->m_bIsClockwise)
				{
					if (dblAngle1 < dblAngle2/* || bIsLargeArc*/)
					{
						dblAngle1 += 2.0 * M_PI;
					}
				}
				else
				{
					if (dblAngle2 < dblAngle1/* || bIsLargeArc*/)
					{
						dblAngle2 += 2.0 * M_PI;
					}
				}

				const int D = (int)(M_PI * (rX + rY));
				const double offset = (dblAngle2 - dblAngle1) / D;
				const double r2 = rX * rY;

				CPoint pt;
				for (int a = 1; a < D - 1; a++)
				{
					dblAngle1 += offset;
					double cosA = cos(dblAngle1);
					double sinA = sin(dblAngle1);
					double r = r2 / bcg_distance(rY * cosA, rX * sinA);

					pt.x = bcg_round(ptCenter.x + cosA * r);
					pt.y = bcg_round(ptCenter.y - sinA * r);
					if (pt != arPoints[arPoints.GetSize () - 1])
					{
						arPoints.Add(pt);
					}
				}

				pt.x = bcg_round(pArcSegment->m_Point.x);
				pt.y = bcg_round(pArcSegment->m_Point.y);
				if (pt != arPoints[arPoints.GetSize () - 1])
				{
					arPoints.Add(pArcSegment->m_Point);
				}

				ptCurr = pArcSegment->m_Point;
			}
		}

		hrgn = ::CreatePolygonRgn((LPPOINT)arPoints.GetData(), (int)arPoints.GetSize(), ALTERNATE);
	}

	if (hrgn != NULL)
	{
		geometry.Set(this, (LPVOID)hrgn);
	}

	return hrgn;
}

BOOL CBCGPGraphicsManagerGDI::DestroyGeometry(CBCGPGeometry& geometry)
{
	if (geometry.GetHandle() != NULL)
	{
		DeleteObject((HANDLE)geometry.GetHandle());
		geometry.Set(NULL, NULL);
	}

	return TRUE;
}

LPVOID CBCGPGraphicsManagerGDI::CreateTextFormat(CBCGPTextFormat& textFormat)
{
	if (textFormat.GetHandle() != NULL)
	{
		CBCGPGraphicsManager* pGM = textFormat.GetGraphicsManager();

		if (!pGM->IsKindOf(GetRuntimeClass()))
		{
			ASSERT(FALSE);
			return NULL;
		}

		if (textFormat.m_bDrawingAngleWasChanged)
		{
			DestroyTextFormat(textFormat);
			textFormat.m_bDrawingAngleWasChanged = FALSE;
		}
		else
		{
			return textFormat.GetHandle();
		}
	}

	LOGFONT lf;
	memset (&lf, 0, sizeof (LOGFONT));

	if (textFormat.GetFontSize() == 0. && textFormat.GetFontFamily().IsEmpty())
	{
		globalData.fontRegular.GetLogFont(&lf);
	}
	else
	{
		lstrcpyn (lf.lfFaceName, textFormat.GetFontFamily(), LF_FACESIZE);
		lf.lfWeight = textFormat.GetFontWeight();
		lf.lfHeight = (long)textFormat.GetFontSize();
		lf.lfEscapement = lf.lfOrientation = (int)(textFormat.GetDrawingAngle() * 10.);
		lf.lfCharSet = LocaleNameToCharset(textFormat.GetFontLocale());

		if (lf.lfHeight > 0)
		{
			lf.lfHeight = -lf.lfHeight;
		}
	}

	HFONT hFont = ::CreateFontIndirect(&lf);
	textFormat.Set(this, (LPVOID)hFont);

	return textFormat.GetHandle();
}

BOOL CBCGPGraphicsManagerGDI::DestroyTextFormat(CBCGPTextFormat& textFormat)
{
	if (textFormat.GetHandle() != NULL)
	{
		DeleteObject((HFONT)textFormat.GetHandle());
		textFormat.Set(NULL, NULL);
	}

	return TRUE;
}

void CBCGPGraphicsManagerGDI::SetClipArea(const CBCGPGeometry& geometry, int nFlags)
{
	if (m_pDCPaint == NULL)
	{
		return;
	}

	HRGN hrgn = (HRGN)CreateGeometry((CBCGPGeometry&)geometry);
	ASSERT(hrgn != NULL);

	::ExtSelectClipRgn(m_pDCPaint->GetSafeHdc(), hrgn, nFlags);
}

void CBCGPGraphicsManagerGDI::ReleaseClipArea()
{
	if (m_pDCPaint != NULL)
	{
		m_pDCPaint->SelectClipRgn(NULL);
		return;
	}
}

void CBCGPGraphicsManagerGDI::CombineGeometry(CBCGPGeometry& geometryDest, const CBCGPGeometry& geometrySrc1, const CBCGPGeometry& geometrySrc2, int nFlags)
{
	HRGN hrgnSrc1 = (HRGN)CreateGeometry((CBCGPGeometry&)geometrySrc1);
	ASSERT(hrgnSrc1 != NULL);

	HRGN hrgnSrc2 = (HRGN)CreateGeometry((CBCGPGeometry&)geometrySrc2);
	ASSERT(hrgnSrc2 != NULL);

	HRGN hrgnDest = ::CreateRectRgn(0, 0, 1, 1);

	int result = ::CombineRgn(hrgnDest, hrgnSrc1, hrgnSrc2, nFlags);
	if (result == NULLREGION || result == ERROR)
	{
		::DeleteObject (hrgnDest);
		hrgnDest = NULL;
	}

	DestroyGeometry(geometryDest);

	geometryDest.Set(this, hrgnDest);
}

void CBCGPGraphicsManagerGDI::GetGeometryBoundingRect(const CBCGPGeometry& geometry, CBCGPRect& rectOut)
{
	rectOut.SetRectEmpty();

	HRGN hrgn = (HRGN)CreateGeometry((CBCGPGeometry&)geometry);
	if (hrgn == NULL)
	{
		return;
	}

	CRect rect;
	::GetRgnBox(hrgn, rect);

	rectOut = rect;
}

LPVOID CBCGPGraphicsManagerGDI::CreateImage(CBCGPImage& image)
{
	if (image.GetHandle() != NULL)
	{
		CBCGPGraphicsManager* pGM = image.GetGraphicsManager();

		if (!pGM->IsKindOf(GetRuntimeClass()))
		{
			ASSERT(FALSE);
			return NULL;
		}
	
		return image.GetHandle();
	}

	CBCGPToolBarImages* pImages = new CBCGPToolBarImages;

	if (image.GetResourceID() != 0)
	{
		pImages->Load(image.GetResourceID());
	}
	else if (image.GetIcon() != NULL)
	{
		pImages->SetImageSize(image.GetIconSize());
		pImages->AddIcon(image.GetIcon(), image.IsAlphaIcon());
	}
	else if (image.GetBitmap() != NULL)
	{
		pImages->SetPreMultiplyAutoCheck(TRUE);
		pImages->SetMapTo3DColors(FALSE);
		pImages->AddImage(image.GetBitmap(), TRUE);
	}
	else
	{
		CString strPath = image.GetPath() == NULL ? _T("") : image.GetPath();
		if (!strPath.IsEmpty())
		{
			pImages->Load(strPath);
		}
	}

#ifndef _BCGSUITE_
	pImages->SetSingleImage(FALSE);
#else
	pImages->SetSingleImage();
#endif

	image.Set(this, (LPVOID)pImages);

	return image.GetHandle();
}

BOOL CBCGPGraphicsManagerGDI::DestroyImage(CBCGPImage& image)
{
	if (image.GetHandle() != NULL)
	{
		CBCGPToolBarImages* pImages = (CBCGPToolBarImages*)image.GetHandle();
		delete pImages;
		image.Set(NULL, NULL);
	}

	return TRUE;
}

CBCGPSize CBCGPGraphicsManagerGDI::GetImageSize(CBCGPImage& image)
{
	CBCGPToolBarImages* pImages = (CBCGPToolBarImages*)CreateImage(image);
	if (pImages == NULL)
	{
		return CBCGPSize();
	}

	return CBCGPSize(pImages->GetImageSize());
}

BOOL CBCGPGraphicsManagerGDI::CopyImage(CBCGPImage& imageSrc, CBCGPImage& imageDest, 
										const CBCGPRect& rectSrc)
{
	CBCGPToolBarImages* pImageSrc = (CBCGPToolBarImages*)CreateImage(imageSrc);
	if (pImageSrc == NULL)
	{
		return FALSE;
	}

	DestroyImage(imageDest);

	CBCGPToolBarImages* pImageDest = new CBCGPToolBarImages;

	if (rectSrc.IsRectEmpty())
	{
		pImageSrc->CopyTo(*pImageDest);
	}
	else
	{
		CRect rectImage(CPoint(0, 0), imageSrc.GetSize());
		if (!rectImage.IntersectRect(rectImage, (const CRect&)rectSrc))
		{
			return FALSE;
		}

		CWindowDC dc (NULL);

		CDC memDCDest;
		memDCDest.CreateCompatibleDC (NULL);

		CSize sizeGDI = rectImage.Size();
		
		CBitmap bitmapCopy;
		if (!bitmapCopy.CreateCompatibleBitmap (&dc, sizeGDI.cx, sizeGDI.cy))
		{
			return FALSE;
		}

		CBitmap* pOldBitmapDest = memDCDest.SelectObject (&bitmapCopy);

		CRect rectDest(0, 0, sizeGDI.cx, sizeGDI.cy);

		memDCDest.FillSolidRect (rectDest, RGB(0, 0, 0));

		pImageSrc->DrawEx(&memDCDest, rectDest, 
			0, CBCGPToolBarImages::ImageAlignHorzLeft, CBCGPToolBarImages::ImageAlignVertTop, 
			rectImage);

		memDCDest.SelectObject (pOldBitmapDest);

		pImageDest->AddImage((HBITMAP)bitmapCopy.GetSafeHandle(), TRUE);
#ifndef _BCGSUITE_
		pImageDest->SetSingleImage(FALSE);
#else
		pImageDest->SetSingleImage();
#endif
	}

	imageDest.Set(this, (LPVOID)pImageDest);
	return TRUE;
}

HBITMAP CBCGPGraphicsManagerGDI::ExportImageToBitmap(CBCGPImage& /*image*/)
{
	return NULL;
}

LPVOID CBCGPGraphicsManagerGDI::CreateBrush(CBCGPBrush& brush)
{
	if (m_pOriginal != NULL)
	{
		return (CBCGPGraphicsManagerGDI*)m_pOriginal->CreateBrush(brush);
	}

	if (brush.GetHandle() != NULL)
	{
		CBCGPGraphicsManager* pGM = brush.GetGraphicsManager();

		if (!pGM->IsKindOf(GetRuntimeClass()))
		{
			ASSERT(FALSE);
			return NULL;
		}
	
		if (brush.GetPenWidth() <= 0)
		{
			return brush.GetHandle();
		}

		DestroyBrush(brush);
	}

	HBRUSH hbr = ::CreateSolidBrush((COLORREF)brush.GetColor());
	brush.Set(this, (LPVOID)hbr);

	return brush.GetHandle();
}

BOOL CBCGPGraphicsManagerGDI::DestroyBrush(CBCGPBrush& brush)
{
	if (brush.GetHandle() != NULL)
	{
		::DeleteObject((HGDIOBJ)brush.GetHandle());
	}

	if (brush.GetHandle1() != NULL)
	{
		::DeleteObject((HGDIOBJ)brush.GetHandle1());
	}

	if (brush.GetHandle2() != NULL)
	{
		::DeleteObject((HGDIOBJ)brush.GetHandle2());
	}

	brush.GetImage().Clear();

	brush.SetPenAttributes(0, PS_SOLID);
	brush.Set(NULL, NULL);

	return TRUE;
}

HPEN CBCGPGraphicsManagerGDI::CreateGDIPen(const CBCGPBrush& brushSrc, const CBCGPStrokeStyle* pStrokeStyle, double lineWidth)
{
	int nPenWidth = bcg_round(lineWidth);
	int nPenStyle = PS_SOLID;

	if (pStrokeStyle != NULL)
	{
		switch (pStrokeStyle->GetDashStyle())
		{
		case CBCGPStrokeStyle::BCGP_DASH_STYLE_SOLID:
			nPenStyle = PS_SOLID;
			break;

		case CBCGPStrokeStyle::BCGP_DASH_STYLE_DASH:
			nPenStyle = PS_DASH;
			break;

		case CBCGPStrokeStyle::BCGP_DASH_STYLE_DOT:
			nPenStyle = PS_DOT;
			break;

		case CBCGPStrokeStyle::BCGP_DASH_STYLE_DASH_DOT:
			nPenStyle = PS_DASHDOT;
			break;

		case CBCGPStrokeStyle::BCGP_DASH_STYLE_DASH_DOT_DOT:
			nPenStyle = PS_DASHDOTDOT;
			break;
		}
	}

	CBCGPBrush& brush = (CBCGPBrush&)brushSrc;

	if (brush.GetHandle() != NULL && brush.GetPenWidth() == nPenWidth && brush.GetPenStyle() == nPenStyle)
	{
#ifdef _DEBUG
		DWORD dwType = ::GetObjectType((HGDIOBJ)brush.GetHandle());
		ASSERT(dwType == OBJ_PEN || dwType == OBJ_EXTPEN);
#endif
		return (HPEN)brush.GetHandle();
	}

	DestroyBrush(brush);

	HPEN hPen = NULL;

	if (nPenWidth > 1 && nPenStyle != PS_SOLID)
	{
		LOGBRUSH lb;
		memset(&lb, 0, sizeof(lb));
		
		lb.lbColor = brush.GetColor();
		lb.lbStyle = BS_SOLID;
		lb.lbHatch = 0;

		hPen = ::ExtCreatePen(PS_GEOMETRIC | nPenStyle, nPenWidth, &lb, 0, NULL);
	}
	else
	{
		hPen = ::CreatePen(nPenStyle, nPenWidth, brush.GetColor());
	}

	brush.Set(this, (LPVOID)hPen);

	brush.SetPenAttributes(nPenWidth, nPenStyle);

	return hPen;
}


static HBITMAP CreateBitmap256(const CSize& size, LPBYTE* lpBits)
{
	BITMAPINFO bi = {0};
	bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biBitCount    = 8;
	bi.bmiHeader.biPlanes      = 1;
	bi.bmiHeader.biWidth       = size.cx;
	bi.bmiHeader.biHeight      = size.cy;
	bi.bmiHeader.biCompression = BI_RGB;

	return CreateDIBSection (NULL, &bi, DIB_RGB_COLORS, (LPVOID*)lpBits, NULL, 0);
}

void CBCGPGraphicsManagerGDI::CreateBrushImage(CBCGPBrush& brush)
{
	CBCGPBrush::BCGP_GRADIENT_TYPE type = brush.GetGradientType ();

	CBCGPToolBarImages& image = brush.GetImage();
	image.Clear();

	CSize szSrc(0, 0);
	const int maxSize = 256;
	CBitmap bmpGrd;

 	const CBCGPColor& clr1 = brush.GetColor ();
 	const CBCGPColor& clr2 = brush.GetGradientColor ();

	switch (type)
	{
	case CBCGPBrush::BCGP_NO_GRADIENT:
	case CBCGPBrush::BCGP_GRADIENT_VERTICAL:
	case CBCGPBrush::BCGP_GRADIENT_CENTER_VERTICAL:
	case CBCGPBrush::BCGP_GRADIENT_PIPE_VERTICAL:
		{
			szSrc.cx = maxSize;
			szSrc.cy = 1;

			LPBYTE lpBits = NULL;
			HBITMAP hBitmap = CreateBitmap256 (szSrc, &lpBits);
			double k = 255.0 / double(szSrc.cx - 1);

			for (int x = 0; x < szSrc.cx; x++)
			{
				*lpBits++ = (BYTE)(255 - bcg_clamp_to_byte(x * k));
			}

			bmpGrd.Attach (hBitmap);
		}
		break;

	case CBCGPBrush::BCGP_GRADIENT_HORIZONTAL:
	case CBCGPBrush::BCGP_GRADIENT_CENTER_HORIZONTAL:
	case CBCGPBrush::BCGP_GRADIENT_PIPE_HORIZONTAL:
		{
			szSrc.cx = 1;
			szSrc.cy = maxSize;

			LPBYTE lpBits = NULL;
			HBITMAP hBitmap = CreateBitmap256 (szSrc, &lpBits);
			double k = 255.0 / double(szSrc.cy - 1);

			CBCGPScanlinerBitmap scan;
			scan.Attach (hBitmap);
			for (int y = 0; y < szSrc.cy; y++)
			{
				*(scan.Get()) = (BYTE)bcg_clamp_to_byte(y * k);
				scan++;
			}

			bmpGrd.Attach (hBitmap);
		}
		break;

	case CBCGPBrush::BCGP_GRADIENT_DIAGONAL_LEFT:
	case CBCGPBrush::BCGP_GRADIENT_DIAGONAL_RIGHT:
		{
			szSrc.cx = maxSize - 1;
			szSrc.cy = maxSize - 1;

			const int nDestSize = szSrc.cx;
			const int nSize     = nDestSize * 2;

			LPBYTE lpBits = NULL;
			HBITMAP hBitmap = CreateBitmap256 (szSrc, &lpBits);
			double k = 255.0 / double(nSize - 1);

			CBCGPScanlinerBitmap scan;
			scan.Attach (hBitmap);

			if (type == CBCGPBrush::BCGP_GRADIENT_DIAGONAL_RIGHT)
			{
				LPBYTE lpData = new BYTE[nSize];
				for (int i = 0; i < nSize; i += 2)
				{
					lpData[i] = (BYTE)(255 - bcg_clamp_to_byte(i * k));
					lpData[i + 1] = lpData[i];
				}

				scan.End();
				for (int y = 0; y < szSrc.cy; y++)
				{
					memcpy(scan.Get(), lpData + y, nDestSize);
					scan--;
				}

				delete [] lpData;
			}
			else
			{
				LPBYTE lpData = new BYTE[nSize];
				for (int i = 0; i < nSize; i += 2)
				{
					lpData[i] = (BYTE)bcg_clamp_to_byte(i * k);
					lpData[i + 1] = lpData[i];
				}

				for (int y = 0; y < szSrc.cy; y++)
				{
					memcpy(scan.Get(), lpData + y, nDestSize);
					scan++;
				}

				delete [] lpData;
			}

			bmpGrd.Attach (hBitmap);
		}
		break;

	case CBCGPBrush::BCGP_GRADIENT_RADIAL_TOP:
	case CBCGPBrush::BCGP_GRADIENT_RADIAL_CENTER:
	case CBCGPBrush::BCGP_GRADIENT_RADIAL_BOTTOM:
	case CBCGPBrush::BCGP_GRADIENT_RADIAL_LEFT:
	case CBCGPBrush::BCGP_GRADIENT_RADIAL_RIGHT:
	case CBCGPBrush::BCGP_GRADIENT_RADIAL_TOP_LEFT:
	case CBCGPBrush::BCGP_GRADIENT_RADIAL_TOP_RIGHT:
	case CBCGPBrush::BCGP_GRADIENT_RADIAL_BOTTOM_LEFT:
	case CBCGPBrush::BCGP_GRADIENT_RADIAL_BOTTOM_RIGHT:
		{
			szSrc.cx = maxSize - 1;
			szSrc.cy = maxSize - 1;

			LPBYTE lpBits = NULL;
			HBITMAP hBitmap = CreateBitmap256 (CSize(255, 255), &lpBits);

			const int nDestSize = szSrc.cx;
			const int nSize     = nDestSize / 2;

			double dispersion = 1.0;
			double minValue   = 0.0;
			double maxValue   = 1.0;
			double delta      = maxValue - minValue;
			double size2S     = nDestSize * nDestSize / 4.0;

			CBCGPScanlinerBitmap scan;
			scan.Attach (hBitmap);

			for(long y = -nSize; y <= nSize; y++)
			{
				double y2 = y * y;

				LPBYTE pRow = scan.Get();
				for(long x = -nSize; x <= nSize; x++)
				{
					double d = y2 + x * x;
					double e = bcg_clamp (exp (-(d / size2S) / dispersion * 2.0) * delta + minValue, minValue, maxValue);

					*pRow++ = (BYTE)(255 - bcg_clamp_to_byte(e * 255.0));
				}

				scan++;
			}

			bmpGrd.Attach (hBitmap);
		}
	}

	if (bmpGrd.GetSafeHandle () == NULL)
	{
		return;
	}

	struct XGradientStop
	{
		float      pos;
		CBCGPColor clr;

		XGradientStop()
			: pos(0.0)
		{
		}
		XGradientStop(float p, const CBCGPColor& c)
			: pos(p)
			, clr(c)
		{
		}
	};
	XGradientStop* gradient = NULL;
	int gradient_count = 0;

	if (type == CBCGPBrush::BCGP_NO_GRADIENT)
	{
		gradient_count = 1;
		gradient = new XGradientStop[gradient_count];

		gradient[0] = XGradientStop(0.0f, clr1);
	}
	else if (type == CBCGPBrush::BCGP_GRADIENT_CENTER_HORIZONTAL || type == CBCGPBrush::BCGP_GRADIENT_CENTER_VERTICAL ||
			type == CBCGPBrush::BCGP_GRADIENT_PIPE_HORIZONTAL || type == CBCGPBrush::BCGP_GRADIENT_PIPE_VERTICAL)
	{
		gradient_count = 3;
		gradient = new XGradientStop[gradient_count];

		CBCGPColor clrS(clr2);
		CBCGPColor clrM(clr1);
		CBCGPColor clrF(clr2);
		float pos = 0.5f;

		if (type == CBCGPBrush::BCGP_GRADIENT_PIPE_HORIZONTAL || type == CBCGPBrush::BCGP_GRADIENT_PIPE_VERTICAL)
		{
			pos = type == CBCGPBrush::BCGP_GRADIENT_PIPE_VERTICAL ? 0.7f : 0.3f;

			clrM = clr2;
			if ((COLORREF)clrM == CBCGPColor::White)
			{
				clrM = clr1;
				clrM.MakeLighter(.5);
			}

			clrS = clr1;
			clrF = clr1;
		}

		gradient[0] = XGradientStop(0.0f, clrS);
		gradient[1] = XGradientStop(pos, clrM);
		gradient[2] = XGradientStop(1.0f, clrF);
	}
	else
	{
		gradient_count = 2;
		gradient = new XGradientStop[gradient_count];

		gradient[0] = XGradientStop(0.0f, clr2);
		gradient[1] = XGradientStop(1.0f, clr1);
	}

	if (gradient == NULL)
	{
		return;
	}

	CBCGPTransferFunction otf;
	CBCGPColorTableFunction ctf;

	for (int i = 0; i < gradient_count; i++)
	{
		BYTE nPos = (BYTE)bcg_clamp_to_byte((double)gradient[i].pos * 255.0);
		double a = (int)bcg_clamp_to_byte(gradient[i].clr.a * 255.0);

		ctf.AddRPoint (nPos, (BYTE)bcg_clamp_to_byte(gradient[i].clr.r * a));
		ctf.AddGPoint (nPos, (BYTE)bcg_clamp_to_byte(gradient[i].clr.g * a));
		ctf.AddBPoint (nPos, (BYTE)bcg_clamp_to_byte(gradient[i].clr.b * a));

		otf.AddPoint (nPos, a);
	}

	delete [] gradient;

	ctf.Calculate ();

	CArray<double, double> arOpacity;
	otf.Calculate (arOpacity);

	{
		LPBYTE lpBits = NULL;
		image.SetImageSize(szSrc);
		HBITMAP hBmp = CBCGPDrawManager::CreateBitmap_32 (szSrc, (LPVOID*)&lpBits);

		CDC dcGrd;
		dcGrd.CreateCompatibleDC (m_pDCPaint);
		CBitmap* pOldGrd = dcGrd.SelectObject (&bmpGrd);
		::SetDIBColorTable(dcGrd.GetSafeHdc(), 0, ctf.GetSize(), ctf.GetColors());

 		CDC dcRes;
 		dcRes.CreateCompatibleDC (m_pDCPaint);
 		HBITMAP hBmpOld = (HBITMAP)dcRes.SelectObject (hBmp);

		dcRes.BitBlt (0, 0, szSrc.cx, szSrc.cy, &dcGrd, 0, 0, SRCCOPY);

		dcRes.SelectObject (hBmpOld);
		dcGrd.SelectObject (pOldGrd);

		image.AddImage(hBmp, TRUE);
		::DeleteObject(hBmp);
	}

	CBCGPScanlinerBitmap scanSrc;
	scanSrc.Attach (bmpGrd);

	CBCGPScanlinerBitmap scanDst;
	scanDst.Attach (image.GetImageWell());

	for (int y = 0; y < szSrc.cy; y++)
	{
		LPBYTE pRowSrc = scanSrc.Get();
		LPBYTE pRowDst = scanDst.Get() + 3;

		for (int x = 0; x < szSrc.cx; x++)
		{
			*pRowDst = (BYTE)arOpacity[*pRowSrc++];
			pRowDst += 4;
		}

		scanSrc++;
		scanDst++;
	}

	brush.Set(this, NULL);
}

BOOL CBCGPGraphicsManagerGDI::FillTransparentGradient(const CRect& rect, CBCGPBrush& brush)
{
	if (!brush.GetImage().IsValid())
	{
		CreateBrushImage ((CBCGPBrush&)brush);

		if (!brush.GetImage().IsValid())
		{
			return FALSE;
		}
	}

	CRect rtDst(rect);
	CRect rtSrc(CPoint(0, 0), brush.GetImage().GetImageSize());

	int nOldMode = m_pDCPaint->SetStretchBltMode (HALFTONE);

	brush.GetImage().DrawEx(m_pDCPaint, rtDst, 0, 
		CBCGPToolBarImages::ImageAlignHorzStretch, CBCGPToolBarImages::ImageAlignVertStretch,
		rtSrc, (BYTE)bcg_clamp_to_byte(brush.GetOpacity() * 255.0));

	m_pDCPaint->SetStretchBltMode (nOldMode);

	return TRUE;
}

void CBCGPGraphicsManagerGDI::FillGradientByType(const CRect& rect, const CBCGPBrush& brush)
{
	if (rect.Width () == 0 || rect.Height () == 0 || m_pDM == NULL || m_pDCPaint == NULL || brush.IsEmpty())
	{
		return;
	}

	if (m_bIsTransparentGradient || (m_bTransparency && brush.IsTransparent()))
	{
		if (FillTransparentGradient(rect, (CBCGPBrush&)brush))
		{
			return;
		}
	}

	CBCGPBrush::BCGP_GRADIENT_TYPE type = brush.GetGradientType();
	COLORREF colorStart = (COLORREF)brush.GetColor();
	COLORREF colorFinish = (COLORREF)brush.GetGradientColor();

	if (colorStart == colorFinish || type == CBCGPBrush::BCGP_NO_GRADIENT || type == CBCGPBrush::BCGP_GRADIENT_BEVEL)
	{
		::FillRect (m_pDCPaint->GetSafeHdc(), rect, (HBRUSH)CreateBrush((CBCGPBrush&)brush));
		return;
	}

	if (type == CBCGPBrush::BCGP_GRADIENT_PIPE_HORIZONTAL || type == CBCGPBrush::BCGP_GRADIENT_PIPE_VERTICAL)
	{
		colorFinish = colorStart;

		CBCGPColor clrLight = brush.GetGradientColor();
		if ((COLORREF)clrLight == CBCGPColor::White)
		{
			clrLight = brush.GetColor();
			clrLight.MakeLighter(.5);
		}

		colorStart = clrLight;
	}

	switch (type)
	{
	case CBCGPBrush::BCGP_GRADIENT_RADIAL_TOP:
	case CBCGPBrush::BCGP_GRADIENT_RADIAL_TOP_LEFT:
		type = CBCGPBrush::BCGP_GRADIENT_DIAGONAL_LEFT;
		break;

	case CBCGPBrush::BCGP_GRADIENT_RADIAL_LEFT:
		type = CBCGPBrush::BCGP_GRADIENT_VERTICAL;
		SwapColors(colorStart, colorFinish);
		break;

	case CBCGPBrush::BCGP_GRADIENT_RADIAL_RIGHT:
		type = CBCGPBrush::BCGP_GRADIENT_VERTICAL;
		break;

	case CBCGPBrush::BCGP_GRADIENT_RADIAL_BOTTOM:
		type = CBCGPBrush::BCGP_GRADIENT_DIAGONAL_LEFT;
		SwapColors(colorStart, colorFinish);
		break;

	case CBCGPBrush::BCGP_GRADIENT_RADIAL_CENTER:
		type = CBCGPBrush::BCGP_GRADIENT_CENTER_HORIZONTAL;
		SwapColors(colorStart, colorFinish);
		break;

	case CBCGPBrush::BCGP_GRADIENT_RADIAL_TOP_RIGHT:
		type = CBCGPBrush::BCGP_GRADIENT_DIAGONAL_RIGHT;
		break;

	case CBCGPBrush::BCGP_GRADIENT_RADIAL_BOTTOM_LEFT:
		type = CBCGPBrush::BCGP_GRADIENT_DIAGONAL_RIGHT;
		SwapColors(colorStart, colorFinish);
		break;

	case CBCGPBrush::BCGP_GRADIENT_RADIAL_BOTTOM_RIGHT:
		type = CBCGPBrush::BCGP_GRADIENT_DIAGONAL_LEFT;
		SwapColors(colorStart, colorFinish);
		break;
	}

	switch (type)
	{
	case CBCGPBrush::BCGP_GRADIENT_HORIZONTAL:
	case CBCGPBrush::BCGP_GRADIENT_VERTICAL:
		m_pDM->FillGradient(rect, colorStart, colorFinish, type == CBCGPBrush::BCGP_GRADIENT_HORIZONTAL);
		break;

	case CBCGPBrush::BCGP_GRADIENT_DIAGONAL_LEFT:
		m_pDM->FillGradient2(rect, colorFinish, colorStart, //45);
			(int)bcg_normalize_deg(bcg_rad2deg(bcg_angle(CBCGPPoint(rect.left, rect.bottom), CBCGPPoint(rect.right, rect.top)))));
		break;

	case CBCGPBrush::BCGP_GRADIENT_DIAGONAL_RIGHT:
		m_pDM->FillGradient2(rect, colorStart, colorFinish, //135);
			(int)bcg_normalize_deg(bcg_rad2deg(bcg_angle(CBCGPPoint(rect.right, rect.top), CBCGPPoint(rect.left, rect.bottom)))));
		break;

	case CBCGPBrush::BCGP_GRADIENT_CENTER_HORIZONTAL:
	case CBCGPBrush::BCGP_GRADIENT_PIPE_HORIZONTAL:
		m_pDM->Fill4ColorsGradient(rect, colorStart, colorFinish, colorFinish, colorStart, TRUE);
		break;

	case CBCGPBrush::BCGP_GRADIENT_CENTER_VERTICAL:
	case CBCGPBrush::BCGP_GRADIENT_PIPE_VERTICAL:
		m_pDM->Fill4ColorsGradient(rect, colorFinish, colorStart, colorStart, colorFinish, FALSE);
		break;
	}
}

void CBCGPGraphicsManagerGDI::EnableAntialiasing(BOOL bEnable)
{
	m_bIsAntialiasing = bEnable;
}

BOOL CBCGPGraphicsManagerGDI::IsAntialiasingEnabled() const
{
	return m_bIsAntialiasing;
}
